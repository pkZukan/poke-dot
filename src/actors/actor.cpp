#include "actor.h"
#include "fbs/animation/traef.h"
#include "gflib.h"

#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void ActorObj::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("GetAnimationPlayer"), &ActorObj::GetAnimationPlayer);
    ClassDB::bind_method(D_METHOD("GetAnimationList"), &ActorObj::GetAnimationList);
    ClassDB::bind_method(D_METHOD("GetModel"), &ActorObj::GetModel);
    ClassDB::bind_method(D_METHOD("set_voice_bank", "bank"), &ActorObj::set_voice_bank);
    ClassDB::bind_method(D_METHOD("get_voice_bank"), &ActorObj::get_voice_bank);
    ClassDB::bind_method(D_METHOD("_dispatch_animation_event", "animation_name", "event"), &ActorObj::_dispatch_animation_event);
    ClassDB::bind_method(D_METHOD("_dispatch_animation_events", "animation_name", "events"), &ActorObj::_dispatch_animation_events);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "voice_bank", PROPERTY_HINT_RESOURCE_TYPE, "WwiseSoundBank"), "set_voice_bank", "get_voice_bank");
    ADD_SIGNAL(MethodInfo("animation_event", PropertyInfo(Variant::STRING_NAME, "animation_name"), PropertyInfo(Variant::DICTIONARY, "event")));
}

void ActorObj::Initialize()
{
    //
}

void ActorObj::LoadActor(String mdlFile, String animFile, String variation)
{
    _cleanup();
    
    String base_path = mdlFile.get_base_dir();
    String filename = mdlFile.get_file();  

    //Create and add TrinityModel
    _model = memnew(TrinityModel);
    _model->load_model(base_path, filename, variation);
    add_child(_model);

    //Create and add AnimationPlayer/AnimationLibrary
    _setup_animation();
    _load_animations(animFile);

    //Debug skeleton
    if(debug_skel)
    {
        _debug_mesh = memnew(MeshInstance3D);
        _imm_mesh.instantiate();
        _debug_mesh->set_mesh(_imm_mesh);
        add_child(_debug_mesh);

        Ref<StandardMaterial3D> mat;
        mat.instantiate();
        mat->set_flag(StandardMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
        mat->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
        _debug_mesh->set_material_override(mat);
    }
}

TypedArray<StringName> ActorObj::GetAnimationList()
{
    return _anim_lib->get_animation_list();
}

AABB ActorObj::GetBBox()
{
    return _model->BBox;
}

void ActorObj::_cleanup() 
{
    if (_voice_player)
    {
        _voice_player->stop();
        _voice_player->get_parent()->remove_child(_voice_player);
        _voice_player->queue_free();
        _voice_player = nullptr;
    }
    if (_voice_attachment)
    {
        _voice_attachment->get_parent()->remove_child(_voice_attachment);
        _voice_attachment->queue_free();
        _voice_attachment = nullptr;
    }
    if (_anim_player)
    {
        _anim_player->stop();
        remove_child(_anim_player);
        _anim_player->queue_free();
        _anim_player = nullptr;
    }
    _skeleton = nullptr;
    _skl_path = String();
    _anim_lib.unref();
    _imm_mesh.unref();

    if (_model) 
    {
        remove_child(_model);
        _model->queue_free();
        _model = nullptr;
    }
    if (_debug_mesh) 
    {
        remove_child(_debug_mesh);
        _debug_mesh->queue_free();
        _debug_mesh = nullptr;
    }
}

void ActorObj::_setup_animation() {
    //Setup animation player
    _anim_player = memnew(AnimationPlayer);
    add_child(_anim_player);

    _anim_player->set_name("AnimationPlayer");
    _anim_player->set_process_callback(AnimationPlayer::ANIMATION_PROCESS_IDLE);

    //init animation library
    _anim_lib.instantiate();
    _anim_player->add_animation_library("", _anim_lib);

    _skeleton = _find_skeleton(_model);
    if (!_skeleton) {
        UtilityFunctions::push_error("No Skeleton3D found in model");
        return;
    }
    _anim_player->set_root_node(_anim_player->get_path_to(this));

    //Get skeleton path
    _skl_path = String(get_path_to(_skeleton));
}

void ActorObj::_load_animation_parameter(String filepath)
{
    //
}

void ActorObj::_load_animation_resource(String filepath)
{
    String base_path = filepath.get_base_dir();
    Ref<TRAnimationChannelResource> tracr = ResourceLoader::get_singleton()->load(filepath);
    Ref<TRAnimationTrackListTable> table = tracr->get_table();
    auto list = table->get_list();
    for(int i = 0; i < list.size(); i++)
    {
        Ref<TRAnimationTrack> track = list[i];
        String name = track->get_Name();
        
        Ref<TRAnimationTrackResourceTable> res = track->get_Resources();
        Ref<TRAnimationTrackResource> anim = res->get_animation();
        if(anim.is_valid())
        {
            String file = base_path.path_join(anim->get_FileName());
            _add_animation(file, name);
        }
        Ref<TRAnimationTrackResource> mat = res->get_material();
        if(mat.is_valid())
        {
            String file = base_path.path_join(mat->get_FileName());
            _add_mesh_animation(file, name);
        }
        Ref<TRAnimationTrackResource> eff = res->get_effect();
        if(eff.is_valid())
        {
            String file = base_path.path_join(eff->get_FileName());
            _add_effect_animation(file, name);
        }
    }
}

void ActorObj::_load_animation_layer(String filepath)
{
    //
}

void ActorObj::_load_animation_state(String filepath)
{
    //
}

void ActorObj::_load_animation_look_at(String filepath)
{
    //
}

void ActorObj::_load_animation_slope_orientor(String filepath)
{
    //
}

void ActorObj::_load_animation_motion_detector(String filepath)
{
    //
}

void ActorObj::_load_animations(String tracn_file)
{
    if(tracn_file.is_empty()) return;
    
    //Load TRACN
    String pokeBase = tracn_file.get_base_dir();
    Ref<TRAnimationChannelNames> tracn = ResourceLoader::get_singleton()->load(tracn_file);

    //Load channels
    auto chanList = tracn->get_list();
    for(int i = 0; i < chanList.size(); i++)
    {
        Ref<AnimationChannelNameEntry> nameEnt = chanList[i];
        String aname = nameEnt->get_animation_name();
        String filepath = pokeBase.path_join(nameEnt->get_filename());
        if(aname == "parameter")
        {
            _load_animation_parameter(filepath);
        }
        else if(aname == "resource")
        {
            _load_animation_resource(filepath);
        }
        else if(aname == "layer")
        {
            _load_animation_layer(filepath);
        }
        else if(aname == "state")
        {
            _load_animation_state(filepath);
        }
        else if(aname == "look_at")
        {
            _load_animation_look_at(filepath);
        }
        else if(aname == "slope_orientor")
        {
            _load_animation_slope_orientor(filepath);
        }
        else if(aname == "motion_detector")
        {
            _load_animation_motion_detector(filepath);
        }
    }
}

void ActorObj::_add_animation(String anim_file, String name)
{
    Ref<Animation> godot_anim = TrinityAnimationConverter::convert_to_godot_animation(anim_file, _skeleton, _skl_path);
    if (!godot_anim.is_valid()) {
        UtilityFunctions::push_error("Animation conversion failed");
        return;
    }

    _anim_lib->add_animation(name, godot_anim);
}

void ActorObj::_add_effect_animation(String traef_file, String name)
{
    Ref<TRAnimationEffects> traef = ResourceLoader::get_singleton()->load(traef_file);
    if (traef.is_null())
    {
        UtilityFunctions::push_error("Failed to load TRAEF animation ", traef_file);
        return;
    }

    Ref<Animation> godot_anim;
    if (_anim_lib->has_animation(name))
    {
        godot_anim = _anim_lib->get_animation(name);
    }
    else
    {
        godot_anim.instantiate();
        _anim_lib->add_animation(name, godot_anim);
    }
    godot_anim->set_meta("traef", traef);
    double frame_rate = godot_anim->get_meta("frame_rate", 0.0);
    Array events = traef->get_events(frame_rate);
    godot_anim->set_meta("animation_events", events);
    if (frame_rate <= 0.0 || events.is_empty()) return;

    const int track = godot_anim->add_track(Animation::TYPE_METHOD);
    godot_anim->track_set_path(track, NodePath("."));
    Dictionary events_by_frame;
    for (int i = 0; i < events.size(); ++i)
    {
        Dictionary event = events[i];
        const int64_t frame = event["start_frame"];
        Array batch = events_by_frame.get(frame, Array());
        batch.push_back(event);
        events_by_frame[frame] = batch;
    }
    // Godot replaces keys at identical times, so dispatch same-frame events together.
    for (const Variant &frame : events_by_frame.keys())
    {
        Array args;
        args.push_back(StringName(name));
        args.push_back(events_by_frame[frame]);
        Dictionary key;
        key["method"] = "_dispatch_animation_events";
        key["args"] = args;
        godot_anim->track_insert_key(track, int64_t(frame) / frame_rate, key);
    }
}

void ActorObj::_dispatch_animation_events(StringName animation_name, Array events)
{
    for (int i = 0; i < events.size(); ++i)
        _dispatch_animation_event(animation_name, events[i]);
}

void ActorObj::set_voice_bank(const Ref<WwiseSoundBank> &bank)
{
    if (_voice_player) _voice_player->stop();
    _voice_bank = bank;
    _voice_bank_load_attempted = bank.is_valid();
}

void ActorObj::_dispatch_animation_event(StringName animation_name, Dictionary event)
{
    Dictionary parameters = event.get("parameters", Dictionary());
    // AnimationPlayer's current clip is our top-clip approximation during fades.
    if (bool(parameters.get("TriggerOnlyTopWeighted", false)) && _anim_player &&
        !_anim_player->get_current_animation().is_empty() &&
        _anim_player->get_current_animation() != animation_name) return;
    emit_signal("animation_event", animation_name, event);
    if (String(event.get("path", "")) != "Audio/PokeVoiceEvent" || !is_inside_tree()) return;

    String event_name = parameters.get("PreviewEventName", "");
    if (event_name.is_empty()) return;
    const int probability = CLAMP(int(parameters.get("Probability", 100)), 0, 100);
    if (probability == 0 || (probability < 100 && UtilityFunctions::randf() * 100.0 >= probability)) return;

    if (_voice_bank.is_null() && !_voice_bank_load_attempted)
    {
        _voice_bank_load_attempted = true;
        const String path = GFLib::romfs_path("audio/wwise/bin/Switch/PM_VOICE.bnk");
        if (ResourceLoader::get_singleton()->exists(path))
            _voice_bank = ResourceLoader::get_singleton()->load(path);
    }
    if (_voice_bank.is_null()) return;
    Ref<AudioStream> stream = _voice_bank->resolve_event_stream(event_name, parameters.get("VoiceType", "INDEX"));
    if (stream.is_null()) return;

    if (!_voice_player)
    {
        _voice_player = memnew(AudioStreamPlayer3D);
        _voice_player->set_name("AnimationVoice");
        add_child(_voice_player);
    }
    _voice_player->stop();
    const String joint = parameters.get("JointName", "");
    const int bone = _skeleton && !joint.is_empty() ? _skeleton->find_bone(joint) : -1;
    const bool follow = bool(parameters.get("Follow", false));
    Node3D *parent = this;
    if (follow && bone >= 0)
    {
        if (!_voice_attachment)
        {
            _voice_attachment = memnew(BoneAttachment3D);
            _skeleton->add_child(_voice_attachment);
        }
        _voice_attachment->set_bone_idx(bone);
        _voice_attachment->on_skeleton_update();
        parent = _voice_attachment;
    }
    _voice_player->set_as_top_level(false);
    if (_voice_player->get_parent() != parent) _voice_player->reparent(parent, false);
    _voice_player->set_transform(Transform3D());
    if (!follow)
    {
        const Transform3D origin = bone >= 0 ?
            _skeleton->get_global_transform() * _skeleton->get_bone_global_pose(bone) : get_global_transform();
        _voice_player->set_as_top_level(true);
        _voice_player->set_global_transform(origin);
    }
    _voice_player->set_stream(stream);
    _voice_player->play();
}

void ActorObj::_add_mesh_animation(String tracm_file, String name)
{
    ERR_FAIL_NULL(_model);
    Ref<TRAnimationChannelMeshes> tracm = ResourceLoader::get_singleton()->load(tracm_file);
    if (!tracm.is_valid())
    {
        UtilityFunctions::push_error("Failed to load TRACM animation ", tracm_file);
        return;
    }

    Ref<Animation> godot_anim;
    if (_anim_lib->has_animation(name))
    {
        godot_anim = _anim_lib->get_animation(name);
    }
    else
    {
        godot_anim.instantiate();
        _anim_lib->add_animation(name, godot_anim);
    }

    ERR_FAIL_COND(tracm->get_info().is_null());
    float frame_rate = tracm->get_info()->get_animation_rate();
    ERR_FAIL_COND_MSG(frame_rate <= 0, "TRACM frame rate must be positive");
    if (!godot_anim->has_meta("frame_rate"))
        godot_anim->set_meta("frame_rate", frame_rate);
    int key_frames = tracm->get_info()->get_animation_count();

    float length = (float)key_frames / frame_rate;
    if (godot_anim->get_track_count() == 0)
        godot_anim->set_loop_mode(tracm->get_info()->get_does_loop() ? Animation::LOOP_LINEAR : Animation::LOOP_NONE);
    if (godot_anim->get_track_count() == 0 || length > godot_anim->get_length()) {
        godot_anim->set_length(length);
    }

    TypedArray<Node> meshes = _model->find_children("*", "MeshInstance3D", true, false);
    Array tracks = tracm->get_tracks();
    for (int i = 0; i < tracks.size(); i++)
    {
        Ref<TRMeshAnimeTrack> mat_trk = tracks[i];
        String mesh_path = mat_trk->get_path();

        for (int mesh_idx = 0; mesh_idx < meshes.size(); ++mesh_idx) 
        {
            MeshInstance3D* mesh = Object::cast_to<MeshInstance3D>(meshes[mesh_idx]);
            const String prefix = mesh_path.get_file() + "_";
            if (!String(mesh->get_name()).begins_with(prefix)) continue;

            Ref<TRVisibilityShapeTimeline> vis = mat_trk->get_vis_anim();
            if (vis.is_valid() && vis->get_info().is_valid() && vis->get_info()->get_values().is_valid()) 
            {
                int track_idx = godot_anim->add_track(Animation::TYPE_VALUE);
                godot_anim->track_set_path(track_idx, String(get_path_to(mesh)) + ":visible");
                godot_anim->value_track_set_update_mode(track_idx, Animation::UPDATE_DISCRETE);
                TrinityAnimationConverter::sample_bool_track(godot_anim, track_idx, vis->get_info()->get_values(), frame_rate, key_frames);
            }

            Ref<TRTrackMaterialTimeline> timeline = mat_trk->get_mat_anim();
            if (timeline.is_null()) continue;
            float material_rate = frame_rate;
            if (timeline->get_info().is_valid() && timeline->get_info()->get_animation_rate() > 0)
                material_rate = timeline->get_info()->get_animation_rate();
            Array materials = timeline->get_material_tracks();
            for (int j = 0; j < materials.size(); ++j) {
                Ref<TRTrackMaterial> material = materials[j];
                if (String(mesh->get_name()) != prefix + material->get_Name()) continue;
                Ref<ShaderMaterial> shader_material = mesh->get_material_override();
                if (shader_material.is_null() || shader_material->get_shader().is_null()) continue;
                Dictionary types;
                for (const Variant& entry : shader_material->get_shader()->get_shader_uniform_list()) {
                    Dictionary uniform = entry;
                    types[uniform["name"]] = uniform["type"];
                }
                const String path = String(get_path_to(mesh)) + ":material_override:shader_parameter/";
                TrinityAnimationConverter::add_material_tracks(godot_anim, material, path, types, material_rate);

                // RESET is a built-in anim state that godot can use as a fallback 
                // to prevent the weird UV issue in some material anims
                Ref<Animation> reset;
                if (_anim_lib->has_animation("RESET"))
                    reset = _anim_lib->get_animation("RESET");
                else {
                    reset.instantiate();
                    _anim_lib->add_animation("RESET", reset);
                }
                for (int channel = 0; channel < godot_anim->get_track_count(); ++channel) {
                    if (godot_anim->track_get_type(channel) != Animation::TYPE_VALUE) continue;
                    NodePath channel_path = godot_anim->track_get_path(channel);
                    if (!String(channel_path).begins_with(path) ||
                        reset->find_track(channel_path, Animation::TYPE_VALUE) >= 0) continue;
                    Variant initial = mesh->get_indexed(NodePath(channel_path.get_concatenated_subnames()));
                    if (initial.get_type() == Variant::NIL) continue;
                    int reset_channel = reset->add_track(Animation::TYPE_VALUE);
                    reset->track_set_path(reset_channel, channel_path);
                    reset->value_track_set_update_mode(reset_channel, godot_anim->value_track_get_update_mode(channel));
                    reset->track_insert_key(reset_channel, 0.0, initial);
                }
            }
        }

        // Blendshapes
        Ref<TRBlendShapeTimeline> blend_anim = mat_trk->get_blendshape_anim();
        if (blend_anim.is_valid()) {
            Array btracks = blend_anim->get_blendshape_tracks();
            for (int j = 0; j < btracks.size(); j++) {
                Ref<TRTrackBlendShape> btrk = btracks[j];
                String bname = btrk->get_Name();
                int track_idx = godot_anim->add_track(Animation::TYPE_BLEND_SHAPE);
                godot_anim->track_set_path(track_idx, mesh_path + ":" + bname);
                TrinityAnimationConverter::sample_float_track(godot_anim, track_idx, btrk->get_track(), frame_rate, key_frames);
            }
        }
    }
}

Skeleton3D* ActorObj::_find_skeleton(Node* node) {
    if (!node) return nullptr;
    if (Skeleton3D* skel = Object::cast_to<Skeleton3D>(node)) {
        return skel;
    }
    for (int i = 0; i < node->get_child_count(); i++) {
        Skeleton3D* result = _find_skeleton(node->get_child(i));
        if (result) return result;
    }
    return nullptr;
}

/*
*   Debug 
*/

void ActorObj::DebugDrawSkeleton() 
{
    //Debug skeleton
    if(debug_skel)
    {
        if (!_skeleton || !_imm_mesh.is_valid()) return;
        
        _imm_mesh->clear_surfaces();
        _imm_mesh->surface_begin(Mesh::PRIMITIVE_LINES);
        
        for (int i = 0; i < _skeleton->get_bone_count(); i++) {
            int parent = _skeleton->get_bone_parent(i);
            if (parent < 0) continue;
            
            Transform3D bone_global  = _skeleton->get_bone_global_pose(i);
            Transform3D parent_global = _skeleton->get_bone_global_pose(parent);
            
            Vector3 bone_pos   = _skeleton->get_global_transform().xform(bone_global.origin);
            Vector3 parent_pos = _skeleton->get_global_transform().xform(parent_global.origin);
            
            _imm_mesh->surface_set_color(Color(1, 1, 0));
            _imm_mesh->surface_add_vertex(parent_pos);
            _imm_mesh->surface_add_vertex(bone_pos);
        }
        
        _imm_mesh->surface_end();
    }
}
