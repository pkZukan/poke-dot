#pragma once

/*
Emitter layouts adapted from EffectLibrary.

MIT License

Copyright (c) 2024 KillzXGaming

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include "middleware/bntx.h"
#include <array>

namespace godot
{

namespace PtclFormat
{

constexpr uint32_t NO_SECTION = 0xffffffff;
constexpr uint32_t SECTION_HEADER_SIZE = 32;
constexpr uint32_t EMITTER_RECORD_SIZE = 0xac0;

inline String ReadFixedString(const Ref<StreamPeerBuffer>& sp, uint32_t length)
{
    const Array result = sp->get_data(length);
    const PackedByteArray bytes = result[1];
    int size = 0;
    while (size < bytes.size() && bytes[size] != 0)
    {
        ++size;
    }
    return size > 0 ? String::utf8(reinterpret_cast<const char*>(bytes.ptr()), size) : String();
}

struct FileHeader
{
    static constexpr uint32_t SIZE = 64;
    String magic;
    uint32_t magic_padding = 0;
    uint16_t graphics_api_version = 0;
    uint16_t vfx_version = 0;
    uint16_t byte_order = 0;
    uint8_t alignment = 0;
    uint8_t address_size = 0;
    uint32_t name_offset = 0;
    uint16_t flags = 0;
    uint16_t first_section_offset = 0;
    uint32_t relocation_offset = 0;
    uint32_t file_size = 0;
    String name;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        magic = sp->get_string(4);
        magic_padding = sp->get_u32();
        graphics_api_version = sp->get_u16();
        vfx_version = sp->get_u16();
        byte_order = sp->get_u16();
        alignment = sp->get_u8();
        address_size = sp->get_u8();
        name_offset = sp->get_u32();
        flags = sp->get_u16();
        first_section_offset = sp->get_u16();
        relocation_offset = sp->get_u32();
        file_size = sp->get_u32();
        name = ReadFixedString(sp, 32);
    }
};

struct SectionHeader
{
    String magic;
    uint32_t size = 0;
    uint32_t children_offset = NO_SECTION;
    uint32_t next_offset = NO_SECTION;
    uint32_t attributes_offset = NO_SECTION;
    uint32_t data_offset = NO_SECTION;
    uint32_t padding = 0;
    uint16_t child_count = 0;
    uint16_t unknown = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        magic = sp->get_string(4);
        size = sp->get_u32();
        children_offset = sp->get_u32();
        next_offset = sp->get_u32();
        attributes_offset = sp->get_u32();
        data_offset = sp->get_u32();
        padding = sp->get_u32();
        child_count = sp->get_u16();
        unknown = sp->get_u16();
    }
};

struct EmitterSetInfo
{
    static constexpr uint32_t SIZE = 104;
    std::array<uint32_t, 4> padding{};
    String name;
    uint32_t emitter_count = 0;
    std::array<uint32_t, 3> reserved{};
    uint32_t unknown1 = 0;
    uint32_t unknown2 = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        for (auto& value : padding)
        {
            value = sp->get_u32();
        }
        name = ReadFixedString(sp, 64);
        emitter_count = sp->get_u32();
        for (auto& value : reserved)
        {
            value = sp->get_u32();
        }
        unknown1 = sp->get_u32();
        unknown2 = sp->get_u32();
    }
};

struct TextureDescriptor
{
    static constexpr uint32_t HEADER_SIZE = 16;
    uint64_t id = 0;
    uint32_t next_offset = 0;
    uint32_t name_length = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        id = sp->get_u64();
        next_offset = sp->get_u32();
        name_length = sp->get_u32();
    }
};

struct TexturePatternAnimation
{
    float num = 0.0f;
    float frequency = 0.0f;
    float num_random = 0.0f;
    float pad = 0.0f;
    std::array<int32_t, 32> table{};

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        num = sp->get_float();
        frequency = sp->get_float();
        num_random = sp->get_float();
        pad = sp->get_float();
        for (auto& value : table)
        {
            value = sp->get_32();
        }
    }
};

struct TextureScrollAnimation
{
    float scroll_add_x = 0.0f;
    float scroll_add_y = 0.0f;
    float scroll_x = 0.0f;
    float scroll_y = 0.0f;
    float scroll_random_x = 0.0f;
    float scroll_random_y = 0.0f;
    float scale_add_x = 0.0f;
    float scale_add_y = 0.0f;
    float scale_x = 0.0f;
    float scale_y = 0.0f;
    float scale_random_x = 0.0f;
    float scale_random_y = 0.0f;
    float rotation_add = 0.0f;
    float rotation = 0.0f;
    float rotation_random = 0.0f;
    float rotation_type = 0.0f;
    float uv_scale_x = 0.0f;
    float uv_scale_y = 0.0f;
    float uv_div_x = 0.0f;
    float uv_div_y = 0.0f;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        scroll_add_x = sp->get_float();
        scroll_add_y = sp->get_float();
        scroll_x = sp->get_float();
        scroll_y = sp->get_float();
        scroll_random_x = sp->get_float();
        scroll_random_y = sp->get_float();
        scale_add_x = sp->get_float();
        scale_add_y = sp->get_float();
        scale_x = sp->get_float();
        scale_y = sp->get_float();
        scale_random_x = sp->get_float();
        scale_random_y = sp->get_float();
        rotation_add = sp->get_float();
        rotation = sp->get_float();
        rotation_random = sp->get_float();
        rotation_type = sp->get_float();
        uv_scale_x = sp->get_float();
        uv_scale_y = sp->get_float();
        uv_div_x = sp->get_float();
        uv_div_y = sp->get_float();
    }
};

struct AnimationKey
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float time = 0.0f;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        x = sp->get_float();
        y = sp->get_float();
        z = sp->get_float();
        time = sp->get_float();
    }
};

struct AnimationKeyTable
{
    std::array<AnimationKey, 8> keys{};

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        for (auto& value : keys)
        {
            value.Read(sp);
        }
    }
};

struct EmitterStatic
{
    uint32_t flags1 = 0;
    uint32_t flags2 = 0;
    uint32_t flags3 = 0;
    uint32_t flags4 = 0;
    uint32_t num_color0_keys = 0;
    uint32_t num_alpha0_keys = 0;
    uint32_t num_color1_keys = 0;
    uint32_t num_alpha1_keys = 0;
    uint32_t num_scale_keys = 0;
    uint32_t num_param_keys = 0;
    uint32_t unknown1 = 0;
    uint32_t unknown2 = 0;
    float color0_loop_rate = 0.0f;
    float alpha0_loop_rate = 0.0f;
    float color1_loop_rate = 0.0f;
    float alpha1_loop_rate = 0.0f;
    float scale_loop_rate = 0.0f;
    float color0_loop_random = 0.0f;
    float alpha0_loop_random = 0.0f;
    float color1_loop_random = 0.0f;
    float alpha1_loop_random = 0.0f;
    float scale_loop_random = 0.0f;
    float unknown3 = 0.0f;
    float unknown4 = 0.0f;
    float gravity_dir_x = 0.0f;
    float gravity_dir_y = 0.0f;
    float gravity_dir_z = 0.0f;
    float gravity_scale = 0.0f;
    float air_res = 0.0f;
    float val_0x74 = 0.0f;
    float val_0x78 = 0.0f;
    float val_0x82 = 0.0f;
    float center_x = 0.0f;
    float center_y = 0.0f;
    float offset = 0.0f;
    float padding = 0.0f;
    float amplitude_x = 0.0f;
    float amplitude_y = 0.0f;
    float cycle_x = 0.0f;
    float cycle_y = 0.0f;
    float phase_rnd_x = 0.0f;
    float phase_rnd_y = 0.0f;
    float phase_init_x = 0.0f;
    float phase_init_y = 0.0f;
    float coefficient0 = 0.0f;
    float coefficient1 = 0.0f;
    float val_0x_b8 = 0.0f;
    float val_0x_bc = 0.0f;
    TexturePatternAnimation tex_pattern_anim0;
    TexturePatternAnimation tex_pattern_anim1;
    TexturePatternAnimation tex_pattern_anim2;
    TextureScrollAnimation tex_scroll_anim0;
    TextureScrollAnimation tex_scroll_anim1;
    TextureScrollAnimation tex_scroll_anim2;
    float color_scale = 0.0f;
    float val_0x364 = 0.0f;
    float val_0x368 = 0.0f;
    float val_0x36_a = 0.0f;
    AnimationKeyTable color0;
    AnimationKeyTable alpha0;
    AnimationKeyTable color1;
    AnimationKeyTable alpha1;
    float soft_edge_param1 = 0.0f;
    float soft_edge_param2 = 0.0f;
    float fresnel_alpha_param1 = 0.0f;
    float fresnel_alpha_param2 = 0.0f;
    float near_dist_alpha_param1 = 0.0f;
    float near_dist_alpha_param2 = 0.0f;
    float far_dist_alpha_param1 = 0.0f;
    float far_dist_alpha_param2 = 0.0f;
    float decal_param1 = 0.0f;
    float decal_param2 = 0.0f;
    float alpha_threshold = 0.0f;
    float padding2 = 0.0f;
    float add_vel_to_scale = 0.0f;
    float soft_partcile_dist = 0.0f;
    float soft_particle_volume = 0.0f;
    float padding3 = 0.0f;
    AnimationKeyTable scale_anim;
    AnimationKeyTable param_anim;
    float rotate_init_x = 0.0f;
    float rotate_init_y = 0.0f;
    float rotate_init_z = 0.0f;
    float rotate_init_empty = 0.0f;
    float rotate_init_rand_x = 0.0f;
    float rotate_init_rand_y = 0.0f;
    float rotate_init_rand_z = 0.0f;
    float rotate_init_rand_empty = 0.0f;
    float rotate_add_x = 0.0f;
    float rotate_add_y = 0.0f;
    float rotate_add_z = 0.0f;
    float rotate_regist = 0.0f;
    float rotate_add_rand_x = 0.0f;
    float rotate_add_rand_y = 0.0f;
    float rotate_add_rand_z = 0.0f;
    float padding4 = 0.0f;
    float scale_limit_dist_near = 0.0f;
    float scale_limit_dist_far = 0.0f;
    float padding5 = 0.0f;
    float padding6 = 0.0f;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        flags1 = sp->get_u32();
        flags2 = sp->get_u32();
        flags3 = sp->get_u32();
        flags4 = sp->get_u32();
        num_color0_keys = sp->get_u32();
        num_alpha0_keys = sp->get_u32();
        num_color1_keys = sp->get_u32();
        num_alpha1_keys = sp->get_u32();
        num_scale_keys = sp->get_u32();
        num_param_keys = sp->get_u32();
        unknown1 = sp->get_u32();
        unknown2 = sp->get_u32();
        color0_loop_rate = sp->get_float();
        alpha0_loop_rate = sp->get_float();
        color1_loop_rate = sp->get_float();
        alpha1_loop_rate = sp->get_float();
        scale_loop_rate = sp->get_float();
        color0_loop_random = sp->get_float();
        alpha0_loop_random = sp->get_float();
        color1_loop_random = sp->get_float();
        alpha1_loop_random = sp->get_float();
        scale_loop_random = sp->get_float();
        unknown3 = sp->get_float();
        unknown4 = sp->get_float();
        gravity_dir_x = sp->get_float();
        gravity_dir_y = sp->get_float();
        gravity_dir_z = sp->get_float();
        gravity_scale = sp->get_float();
        air_res = sp->get_float();
        val_0x74 = sp->get_float();
        val_0x78 = sp->get_float();
        val_0x82 = sp->get_float();
        center_x = sp->get_float();
        center_y = sp->get_float();
        offset = sp->get_float();
        padding = sp->get_float();
        amplitude_x = sp->get_float();
        amplitude_y = sp->get_float();
        cycle_x = sp->get_float();
        cycle_y = sp->get_float();
        phase_rnd_x = sp->get_float();
        phase_rnd_y = sp->get_float();
        phase_init_x = sp->get_float();
        phase_init_y = sp->get_float();
        coefficient0 = sp->get_float();
        coefficient1 = sp->get_float();
        val_0x_b8 = sp->get_float();
        val_0x_bc = sp->get_float();
        tex_pattern_anim0.Read(sp);
        tex_pattern_anim1.Read(sp);
        tex_pattern_anim2.Read(sp);
        tex_scroll_anim0.Read(sp);
        tex_scroll_anim1.Read(sp);
        tex_scroll_anim2.Read(sp);
        color_scale = sp->get_float();
        val_0x364 = sp->get_float();
        val_0x368 = sp->get_float();
        val_0x36_a = sp->get_float();
        color0.Read(sp);
        alpha0.Read(sp);
        color1.Read(sp);
        alpha1.Read(sp);
        soft_edge_param1 = sp->get_float();
        soft_edge_param2 = sp->get_float();
        fresnel_alpha_param1 = sp->get_float();
        fresnel_alpha_param2 = sp->get_float();
        near_dist_alpha_param1 = sp->get_float();
        near_dist_alpha_param2 = sp->get_float();
        far_dist_alpha_param1 = sp->get_float();
        far_dist_alpha_param2 = sp->get_float();
        decal_param1 = sp->get_float();
        decal_param2 = sp->get_float();
        alpha_threshold = sp->get_float();
        padding2 = sp->get_float();
        add_vel_to_scale = sp->get_float();
        soft_partcile_dist = sp->get_float();
        soft_particle_volume = sp->get_float();
        padding3 = sp->get_float();
        scale_anim.Read(sp);
        param_anim.Read(sp);
        rotate_init_x = sp->get_float();
        rotate_init_y = sp->get_float();
        rotate_init_z = sp->get_float();
        rotate_init_empty = sp->get_float();
        rotate_init_rand_x = sp->get_float();
        rotate_init_rand_y = sp->get_float();
        rotate_init_rand_z = sp->get_float();
        rotate_init_rand_empty = sp->get_float();
        rotate_add_x = sp->get_float();
        rotate_add_y = sp->get_float();
        rotate_add_z = sp->get_float();
        rotate_regist = sp->get_float();
        rotate_add_rand_x = sp->get_float();
        rotate_add_rand_y = sp->get_float();
        rotate_add_rand_z = sp->get_float();
        padding4 = sp->get_float();
        scale_limit_dist_near = sp->get_float();
        scale_limit_dist_far = sp->get_float();
        padding5 = sp->get_float();
        padding6 = sp->get_float();
    }
};

struct EmitterInfo
{
    uint8_t is_particle_draw = 0;
    uint8_t sort_type = 0;
    uint8_t calc_type = 0;
    uint8_t follow_type = 0;
    uint8_t is_fade_emit = 0;
    uint8_t is_fade_alpha_fade = 0;
    uint8_t is_scale_fade = 0;
    uint8_t random_seed_type = 0;
    uint8_t is_update_matrix_by_emit = 0;
    uint8_t test_always = 0;
    uint8_t interpolate_emission_amount = 0;
    uint8_t is_alpha_fade_in = 0;
    uint8_t is_scale_fade_in = 0;
    uint8_t padding1 = 0;
    uint8_t padding2 = 0;
    uint8_t padding3 = 0;
    uint32_t random_seed = 0;
    uint32_t draw_path = 0;
    int32_t alpha_fade_time = 0;
    int32_t fade_in_time = 0;
    float trans_x = 0.0f;
    float trans_y = 0.0f;
    float trans_z = 0.0f;
    float trans_rand_x = 0.0f;
    float trans_rand_y = 0.0f;
    float trans_rand_z = 0.0f;
    float rotate_x = 0.0f;
    float rotate_y = 0.0f;
    float rotate_z = 0.0f;
    float rotate_rand_x = 0.0f;
    float rotate_rand_y = 0.0f;
    float rotate_rand_z = 0.0f;
    float scale_x = 0.0f;
    float scale_y = 0.0f;
    float scale_z = 0.0f;
    float color0_r = 0.0f;
    float color0_g = 0.0f;
    float color0_b = 0.0f;
    float color0_a = 0.0f;
    float color1_r = 0.0f;
    float color1_g = 0.0f;
    float color1_b = 0.0f;
    float color1_a = 0.0f;
    float emission_range_near = 0.0f;
    float emission_range_far = 0.0f;
    float emission_ratio_far = 0.0f;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        is_particle_draw = sp->get_u8();
        sort_type = sp->get_u8();
        calc_type = sp->get_u8();
        follow_type = sp->get_u8();
        is_fade_emit = sp->get_u8();
        is_fade_alpha_fade = sp->get_u8();
        is_scale_fade = sp->get_u8();
        random_seed_type = sp->get_u8();
        is_update_matrix_by_emit = sp->get_u8();
        test_always = sp->get_u8();
        interpolate_emission_amount = sp->get_u8();
        is_alpha_fade_in = sp->get_u8();
        is_scale_fade_in = sp->get_u8();
        padding1 = sp->get_u8();
        padding2 = sp->get_u8();
        padding3 = sp->get_u8();
        random_seed = sp->get_u32();
        draw_path = sp->get_u32();
        alpha_fade_time = sp->get_32();
        fade_in_time = sp->get_32();
        trans_x = sp->get_float();
        trans_y = sp->get_float();
        trans_z = sp->get_float();
        trans_rand_x = sp->get_float();
        trans_rand_y = sp->get_float();
        trans_rand_z = sp->get_float();
        rotate_x = sp->get_float();
        rotate_y = sp->get_float();
        rotate_z = sp->get_float();
        rotate_rand_x = sp->get_float();
        rotate_rand_y = sp->get_float();
        rotate_rand_z = sp->get_float();
        scale_x = sp->get_float();
        scale_y = sp->get_float();
        scale_z = sp->get_float();
        color0_r = sp->get_float();
        color0_g = sp->get_float();
        color0_b = sp->get_float();
        color0_a = sp->get_float();
        color1_r = sp->get_float();
        color1_g = sp->get_float();
        color1_b = sp->get_float();
        color1_a = sp->get_float();
        emission_range_near = sp->get_float();
        emission_range_far = sp->get_float();
        emission_ratio_far = sp->get_float();
    }
};

struct EmitterInheritance
{
    uint8_t velocity = 0;
    uint8_t scale = 0;
    uint8_t rotate = 0;
    uint8_t color_scale = 0;
    uint8_t color0 = 0;
    uint8_t color1 = 0;
    uint8_t alpha0 = 0;
    uint8_t alpha1 = 0;
    uint8_t draw_path = 0;
    uint8_t pre_draw = 0;
    uint8_t alpha0_each_frame = 0;
    uint8_t alpha1_each_frame = 0;
    uint8_t enable_emitter_particle = 0;
    uint8_t padding1 = 0;
    uint8_t padding2 = 0;
    uint8_t padding3 = 0;
    float velocity_rate = 0.0f;
    float scale_rate = 0.0f;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        velocity = sp->get_u8();
        scale = sp->get_u8();
        rotate = sp->get_u8();
        color_scale = sp->get_u8();
        color0 = sp->get_u8();
        color1 = sp->get_u8();
        alpha0 = sp->get_u8();
        alpha1 = sp->get_u8();
        draw_path = sp->get_u8();
        pre_draw = sp->get_u8();
        alpha0_each_frame = sp->get_u8();
        alpha1_each_frame = sp->get_u8();
        enable_emitter_particle = sp->get_u8();
        padding1 = sp->get_u8();
        padding2 = sp->get_u8();
        padding3 = sp->get_u8();
        velocity_rate = sp->get_float();
        scale_rate = sp->get_float();
    }
};

struct Emission
{
    bool is_one_time = false;
    bool is_world_gravity = false;
    bool is_emit_dist_enabled = false;
    bool is_world_oriented_velocity = false;
    uint32_t start = 0;
    uint32_t timing = 0;
    uint32_t duration = 0;
    float rate = 0.0f;
    float rate_random = 0.0f;
    int32_t interval = 0;
    float interval_random = 0.0f;
    float position_random = 0.0f;
    float gravity_scale = 0.0f;
    float gravity_dir_x = 0.0f;
    float gravity_dir_y = 0.0f;
    float gravity_dir_z = 0.0f;
    float emitter_dist_unit = 0.0f;
    float emitter_dist_min = 0.0f;
    float emitter_dist_max = 0.0f;
    float emitter_dist_marg = 0.0f;
    int32_t emitter_dist_particles_max = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        is_one_time = sp->get_u8();
        is_world_gravity = sp->get_u8();
        is_emit_dist_enabled = sp->get_u8();
        is_world_oriented_velocity = sp->get_u8();
        start = sp->get_u32();
        timing = sp->get_u32();
        duration = sp->get_u32();
        rate = sp->get_float();
        rate_random = sp->get_float();
        interval = sp->get_32();
        interval_random = sp->get_float();
        position_random = sp->get_float();
        gravity_scale = sp->get_float();
        gravity_dir_x = sp->get_float();
        gravity_dir_y = sp->get_float();
        gravity_dir_z = sp->get_float();
        emitter_dist_unit = sp->get_float();
        emitter_dist_min = sp->get_float();
        emitter_dist_max = sp->get_float();
        emitter_dist_marg = sp->get_float();
        emitter_dist_particles_max = sp->get_32();
    }
};

struct EmitterShape
{
    uint8_t volume_type = 0;
    uint8_t sweep_start_random = 0;
    uint8_t arc_type = 0;
    uint8_t is_volume_latitude_enabled = 0;
    uint8_t volume_tbl_index = 0;
    uint8_t volume_tbl_index64 = 0;
    uint8_t volume_latitude_dir = 0;
    uint8_t is_gpu_emitter = 0;
    float sweep_longitude = 0.0f;
    float sweep_latitude = 0.0f;
    float sweep_start = 0.0f;
    float volume_surface_pos_rand = 0.0f;
    float caliber_ratio = 0.0f;
    float line_center = 0.0f;
    float line_length = 0.0f;
    float volume_radius_x = 0.0f;
    float volume_radius_y = 0.0f;
    float volume_radius_z = 0.0f;
    float volume_form_scale_x = 0.0f;
    float volume_form_scale_y = 0.0f;
    float volume_form_scale_z = 0.0f;
    int32_t prim_emit_type = 0;
    uint64_t primitive_index = 0;
    int32_t num_divide_circle = 0;
    int32_t num_divide_circle_random = 0;
    int32_t num_divide_line = 0;
    int32_t num_divide_line_random = 0;
    uint8_t is_on_another_binary_volume_primitive = 0;
    uint8_t padding1 = 0;
    uint8_t padding2 = 0;
    uint8_t padding3 = 0;
    uint32_t padding4 = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        volume_type = sp->get_u8();
        sweep_start_random = sp->get_u8();
        arc_type = sp->get_u8();
        is_volume_latitude_enabled = sp->get_u8();
        volume_tbl_index = sp->get_u8();
        volume_tbl_index64 = sp->get_u8();
        volume_latitude_dir = sp->get_u8();
        is_gpu_emitter = sp->get_u8();
        sweep_longitude = sp->get_float();
        sweep_latitude = sp->get_float();
        sweep_start = sp->get_float();
        volume_surface_pos_rand = sp->get_float();
        caliber_ratio = sp->get_float();
        line_center = sp->get_float();
        line_length = sp->get_float();
        volume_radius_x = sp->get_float();
        volume_radius_y = sp->get_float();
        volume_radius_z = sp->get_float();
        volume_form_scale_x = sp->get_float();
        volume_form_scale_y = sp->get_float();
        volume_form_scale_z = sp->get_float();
        prim_emit_type = sp->get_32();
        primitive_index = sp->get_u64();
        num_divide_circle = sp->get_32();
        num_divide_circle_random = sp->get_32();
        num_divide_line = sp->get_32();
        num_divide_line_random = sp->get_32();
        is_on_another_binary_volume_primitive = sp->get_u8();
        padding1 = sp->get_u8();
        padding2 = sp->get_u8();
        padding3 = sp->get_u8();
        padding4 = sp->get_u32();
    }
};

struct RenderState
{
    bool is_blend_enable = false;
    bool is_depth_test = false;
    uint8_t depth_func = 0;
    bool is_depth_mask = false;
    bool is_alpha_test = false;
    uint8_t alpha_func = 0;
    uint8_t blend_type = 0;
    uint8_t display_side = 0;
    float alpha_threshold = 0.0f;
    uint32_t padding = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        is_blend_enable = sp->get_u8();
        is_depth_test = sp->get_u8();
        depth_func = sp->get_u8();
        is_depth_mask = sp->get_u8();
        is_alpha_test = sp->get_u8();
        alpha_func = sp->get_u8();
        blend_type = sp->get_u8();
        display_side = sp->get_u8();
        alpha_threshold = sp->get_float();
        padding = sp->get_u32();
    }
};

struct ParticleData
{
    bool infinite_life = false;
    bool is_triming = false;
    uint8_t billboard_type = 0;
    uint8_t rot_type = 0;
    uint8_t offset_type = 0;
    bool rot_rev_rand_x = false;
    bool rot_rev_rand_y = false;
    bool rot_rev_rand_z = false;
    bool is_rotate_x = false;
    bool is_rotate_y = false;
    uint8_t is_rotate_z = 0;
    uint8_t primitive_scale_type = 0;
    uint8_t is_texture_common_random = 0;
    uint8_t connect_ptcl_scale_and_z_offset = 0;
    uint8_t enable_avoid_z_fighting = 0;
    uint8_t val_0x_f = 0;
    int32_t life = 0;
    int32_t life_random = 0;
    float momentum_random = 0.0f;
    uint32_t primitive_vertex_info_flags = 0;
    uint64_t primitive_id = 0;
    uint64_t primitive_ex_id = 0;
    bool loop_color0 = false;
    bool loop_alpha0 = false;
    bool loop_color1 = false;
    bool loop_alpha1 = false;
    bool scale_loop = false;
    bool loop_random_color0 = false;
    bool loop_random_alpha0 = false;
    bool loop_random_color1 = false;
    bool loop_random_alpha1 = false;
    bool scale_loop_random = false;
    uint8_t prim_flag1 = 0;
    uint8_t prim_flag2 = 0;
    int32_t color0_loop_rate = 0;
    int32_t alpha0_loop_rate = 0;
    int32_t color1_loop_rate = 0;
    int32_t alpha1_loop_rate = 0;
    int32_t scale_loop_rate = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        infinite_life = sp->get_u8();
        is_triming = sp->get_u8();
        billboard_type = sp->get_u8();
        rot_type = sp->get_u8();
        offset_type = sp->get_u8();
        rot_rev_rand_x = sp->get_u8();
        rot_rev_rand_y = sp->get_u8();
        rot_rev_rand_z = sp->get_u8();
        is_rotate_x = sp->get_u8();
        is_rotate_y = sp->get_u8();
        is_rotate_z = sp->get_u8();
        primitive_scale_type = sp->get_u8();
        is_texture_common_random = sp->get_u8();
        connect_ptcl_scale_and_z_offset = sp->get_u8();
        enable_avoid_z_fighting = sp->get_u8();
        val_0x_f = sp->get_u8();
        life = sp->get_32();
        life_random = sp->get_32();
        momentum_random = sp->get_float();
        primitive_vertex_info_flags = sp->get_u32();
        primitive_id = sp->get_u64();
        primitive_ex_id = sp->get_u64();
        loop_color0 = sp->get_u8();
        loop_alpha0 = sp->get_u8();
        loop_color1 = sp->get_u8();
        loop_alpha1 = sp->get_u8();
        scale_loop = sp->get_u8();
        loop_random_color0 = sp->get_u8();
        loop_random_alpha0 = sp->get_u8();
        loop_random_color1 = sp->get_u8();
        loop_random_alpha1 = sp->get_u8();
        scale_loop_random = sp->get_u8();
        prim_flag1 = sp->get_u8();
        prim_flag2 = sp->get_u8();
        color0_loop_rate = sp->get_32();
        alpha0_loop_rate = sp->get_32();
        color1_loop_rate = sp->get_32();
        alpha1_loop_rate = sp->get_32();
        scale_loop_rate = sp->get_32();
    }
};

struct TextureCombiner
{
    uint8_t color_combiner_process = 0;
    uint8_t alpha_combiner_process = 0;
    uint8_t texture1_color_blend = 0;
    uint8_t texture2_color_blend = 0;
    uint8_t primitive_color_blend = 0;
    uint8_t texture1_alpha_blend = 0;
    uint8_t texture2_alpha_blend = 0;
    uint8_t primitive_alpha_blend = 0;
    uint8_t tex_color0_input_type = 0;
    uint8_t tex_color1_input_type = 0;
    uint8_t tex_color2_input_type = 0;
    uint8_t tex_alpha0_input_type = 0;
    uint8_t tex_alpha1_input_type = 0;
    uint8_t tex_alpha2_input_type = 0;
    uint8_t primitive_color_input_type = 0;
    uint8_t primitive_alpha_input_type = 0;
    uint8_t shader_type = 0;
    uint8_t apply_alpha = 0;
    uint8_t is_distortion_by_camera_distance = 0;
    uint8_t padding1 = 0;
    uint32_t padding2 = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        color_combiner_process = sp->get_u8();
        alpha_combiner_process = sp->get_u8();
        texture1_color_blend = sp->get_u8();
        texture2_color_blend = sp->get_u8();
        primitive_color_blend = sp->get_u8();
        texture1_alpha_blend = sp->get_u8();
        texture2_alpha_blend = sp->get_u8();
        primitive_alpha_blend = sp->get_u8();
        tex_color0_input_type = sp->get_u8();
        tex_color1_input_type = sp->get_u8();
        tex_color2_input_type = sp->get_u8();
        tex_alpha0_input_type = sp->get_u8();
        tex_alpha1_input_type = sp->get_u8();
        tex_alpha2_input_type = sp->get_u8();
        primitive_color_input_type = sp->get_u8();
        primitive_alpha_input_type = sp->get_u8();
        shader_type = sp->get_u8();
        apply_alpha = sp->get_u8();
        is_distortion_by_camera_distance = sp->get_u8();
        padding1 = sp->get_u8();
        padding2 = sp->get_u32();
    }
};

struct ShaderReferences
{
    uint8_t type = 0;
    uint8_t val_0x2 = 0;
    uint8_t val_0x3 = 0;
    uint8_t val_0x4 = 0;
    int32_t shader_index = 0;
    int32_t compute_shader_index = 0;
    int32_t user_shader_index1 = 0;
    int32_t user_shader_index2 = 0;
    int32_t custom_shader_index = 0;
    uint64_t custom_shader_flag = 0;
    uint64_t custom_shader_switch = 0;
    int32_t extra_shader_index2 = 0;
    int32_t val_0x34 = 0;
    std::array<uint8_t, 16> user_shader_define1{};
    std::array<uint8_t, 16> user_shader_define2{};

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        type = sp->get_u8();
        val_0x2 = sp->get_u8();
        val_0x3 = sp->get_u8();
        val_0x4 = sp->get_u8();
        shader_index = sp->get_32();
        compute_shader_index = sp->get_32();
        user_shader_index1 = sp->get_32();
        user_shader_index2 = sp->get_32();
        custom_shader_index = sp->get_32();
        custom_shader_flag = sp->get_u64();
        custom_shader_switch = sp->get_u64();
        extra_shader_index2 = sp->get_32();
        val_0x34 = sp->get_32();
        for (auto& value : user_shader_define1)
        {
            value = sp->get_u8();
        }

        for (auto& value : user_shader_define2)
        {
            value = sp->get_u8();
        }
    }
};

struct Action
{
    uint32_t action_index = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        action_index = sp->get_u32();
    }
};

struct ParticleVelocity
{
    float all_direction = 0.0f;
    float designated_dir_scale = 0.0f;
    float designated_dir_x = 0.0f;
    float designated_dir_y = 0.0f;
    float designated_dir_z = 0.0f;
    float diffusion_dir_angle = 0.0f;
    float xz_diffusion = 0.0f;
    float diffusion_x = 0.0f;
    float diffusion_y = 0.0f;
    float diffusion_z = 0.0f;
    float vel_random = 0.0f;
    float em_vel_inherit = 0.0f;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        all_direction = sp->get_float();
        designated_dir_scale = sp->get_float();
        designated_dir_x = sp->get_float();
        designated_dir_y = sp->get_float();
        designated_dir_z = sp->get_float();
        diffusion_dir_angle = sp->get_float();
        xz_diffusion = sp->get_float();
        diffusion_x = sp->get_float();
        diffusion_y = sp->get_float();
        diffusion_z = sp->get_float();
        vel_random = sp->get_float();
        em_vel_inherit = sp->get_float();
    }
};

struct ParticleColor
{
    uint8_t is_soft_particle = 0;
    uint8_t is_fresnel_alpha = 0;
    uint8_t is_near_dist_alpha = 0;
    uint8_t is_far_dist_alpha = 0;
    uint8_t is_decal = 0;
    uint8_t val_0x5 = 0;
    uint8_t val_0x6 = 0;
    uint8_t val_0x7 = 0;
    uint8_t color0_type = 0;
    uint8_t color1_type = 0;
    uint8_t alpha0_type = 0;
    uint8_t alpha1_type = 0;
    float color0_r = 0.0f;
    float color0_g = 0.0f;
    float color0_b = 0.0f;
    float alpha0 = 0.0f;
    float color1_r = 0.0f;
    float color1_g = 0.0f;
    float color1_b = 0.0f;
    float alpha1 = 0.0f;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        is_soft_particle = sp->get_u8();
        is_fresnel_alpha = sp->get_u8();
        is_near_dist_alpha = sp->get_u8();
        is_far_dist_alpha = sp->get_u8();
        is_decal = sp->get_u8();
        val_0x5 = sp->get_u8();
        val_0x6 = sp->get_u8();
        val_0x7 = sp->get_u8();
        color0_type = sp->get_u8();
        color1_type = sp->get_u8();
        alpha0_type = sp->get_u8();
        alpha1_type = sp->get_u8();
        color0_r = sp->get_float();
        color0_g = sp->get_float();
        color0_b = sp->get_float();
        alpha0 = sp->get_float();
        color1_r = sp->get_float();
        color1_g = sp->get_float();
        color1_b = sp->get_float();
        alpha1 = sp->get_float();
    }
};

struct ParticleScale
{
    float scale_x = 0.0f;
    float scale_y = 0.0f;
    float scale_z = 0.0f;
    float scale_random_x = 0.0f;
    float scale_random_y = 0.0f;
    float scale_random_z = 0.0f;
    uint8_t enable_scaling_by_camera_dist_near = 0;
    uint8_t enable_scaling_by_camera_dist_far = 0;
    uint8_t enable_add_scale_y = 0;
    uint8_t enable_link_fovy_to_scale_value = 0;
    float scale_min = 0.0f;
    float scale_max = 0.0f;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        scale_x = sp->get_float();
        scale_y = sp->get_float();
        scale_z = sp->get_float();
        scale_random_x = sp->get_float();
        scale_random_y = sp->get_float();
        scale_random_z = sp->get_float();
        enable_scaling_by_camera_dist_near = sp->get_u8();
        enable_scaling_by_camera_dist_far = sp->get_u8();
        enable_add_scale_y = sp->get_u8();
        enable_link_fovy_to_scale_value = sp->get_u8();
        scale_min = sp->get_float();
        scale_max = sp->get_float();
    }
};

struct ParticleFluctuation
{
    uint8_t is_apply_alpha = 0;
    uint8_t is_applay_scale = 0;
    uint8_t is_applay_scale_y = 0;
    uint8_t is_wave_type = 0;
    uint8_t is_phase_random_x = 0;
    uint8_t is_phase_random_y = 0;
    uint8_t padding1 = 0;
    uint8_t padding2 = 0;
    uint32_t padding3 = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        is_apply_alpha = sp->get_u8();
        is_applay_scale = sp->get_u8();
        is_applay_scale_y = sp->get_u8();
        is_wave_type = sp->get_u8();
        is_phase_random_x = sp->get_u8();
        is_phase_random_y = sp->get_u8();
        padding1 = sp->get_u8();
        padding2 = sp->get_u8();
        padding3 = sp->get_u32();
    }
};

struct TextureSampler
{
    uint64_t texture_id = 0;
    uint8_t wrap_u = 0;
    uint8_t wrap_v = 0;
    uint8_t filter = 0;
    uint8_t is_sphere_map = 0;
    float max_lod = 0.0f;
    float lod_bias = 0.0f;
    uint8_t mip_level_limit = 0;
    uint8_t is_density_fixed_u = 0;
    uint8_t is_density_fixed_v = 0;
    uint8_t is_square_rgb = 0;
    uint8_t is_on_another_binary = 0;
    uint8_t padding1 = 0;
    uint8_t padding2 = 0;
    uint8_t padding3 = 0;
    uint32_t padding4 = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        texture_id = sp->get_u64();
        wrap_u = sp->get_u8();
        wrap_v = sp->get_u8();
        filter = sp->get_u8();
        is_sphere_map = sp->get_u8();
        max_lod = sp->get_float();
        lod_bias = sp->get_float();
        mip_level_limit = sp->get_u8();
        is_density_fixed_u = sp->get_u8();
        is_density_fixed_v = sp->get_u8();
        is_square_rgb = sp->get_u8();
        is_on_another_binary = sp->get_u8();
        padding1 = sp->get_u8();
        padding2 = sp->get_u8();
        padding3 = sp->get_u8();
        padding4 = sp->get_u32();
    }
};

struct TextureAnim
{
    uint8_t pattern_anim_type = 0;
    bool is_scroll = false;
    bool is_rotate = false;
    bool is_scale = false;
    uint8_t repeat = 0;
    uint8_t inv_rand_u = 0;
    uint8_t inv_rand_v = 0;
    uint8_t is_pat_anim_loop_random = 0;
    uint8_t uv_channel = 0;
    uint8_t is_crossfade = 0;
    uint8_t padding1 = 0;
    uint8_t padding2 = 0;
    uint32_t padding3 = 0;

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        pattern_anim_type = sp->get_u8();
        is_scroll = sp->get_u8();
        is_rotate = sp->get_u8();
        is_scale = sp->get_u8();
        repeat = sp->get_u8();
        inv_rand_u = sp->get_u8();
        inv_rand_v = sp->get_u8();
        is_pat_anim_loop_random = sp->get_u8();
        uv_channel = sp->get_u8();
        is_crossfade = sp->get_u8();
        padding1 = sp->get_u8();
        padding2 = sp->get_u8();
        padding3 = sp->get_u32();
    }
};

struct EmitterRecord
{
    uint32_t flag = 0;
    uint32_t random_seed = 0;
    uint32_t padding1 = 0;
    uint32_t padding2 = 0;
    String name;
    EmitterStatic emitter_static;
    EmitterInfo emitter_info;
    EmitterInheritance child_inheritance;
    Emission emission;
    EmitterShape shape_info;
    RenderState render_state;
    ParticleData particle_data;
    TextureCombiner combiner;
    ShaderReferences shader_references;
    Action action;
    ParticleVelocity particle_velocity;
    ParticleColor particle_color;
    ParticleScale particle_scale;
    ParticleFluctuation particle_fluctuation;
    TextureSampler sampler0;
    TextureSampler sampler1;
    TextureSampler sampler2;
    TextureAnim texture_anim0;
    TextureAnim texture_anim1;
    TextureAnim texture_anim2;
    std::array<uint8_t, 64> reserved{};

    void Read(const Ref<StreamPeerBuffer>& sp)
    {
        flag = sp->get_u32();
        random_seed = sp->get_u32();
        padding1 = sp->get_u32();
        padding2 = sp->get_u32();
        name = ReadFixedString(sp, 64);
        emitter_static.Read(sp);
        emitter_info.Read(sp);
        child_inheritance.Read(sp);
        emission.Read(sp);
        shape_info.Read(sp);
        render_state.Read(sp);
        particle_data.Read(sp);
        combiner.Read(sp);
        shader_references.Read(sp);
        action.Read(sp);
        particle_velocity.Read(sp);
        particle_color.Read(sp);
        particle_scale.Read(sp);
        particle_fluctuation.Read(sp);
        sampler0.Read(sp);
        sampler1.Read(sp);
        sampler2.Read(sp);
        texture_anim0.Read(sp);
        texture_anim1.Read(sp);
        texture_anim2.Read(sp);
        for (auto& value : reserved)
        {
            value = sp->get_u8();
        }
    }
};

}

class ParticleArchive : public Resource
{
    GDCLASS(ParticleArchive, Resource)

    Array emitter_sets;
    Dictionary texture_names;
    Ref<BinaryTextureArchive> texture_archive;
    mutable Dictionary texture_cache;
    PackedStringArray warnings;
    int version = 0;

protected:
    static void _bind_methods();

public:
    Error LoadFromFile(const String& path);
    Error LoadFromBuffer(const PackedByteArray& bytes);

    Array get_emitter_sets() const
    {
        return emitter_sets.duplicate(true);
    }

    Dictionary get_texture_names() const
    {
        return texture_names.duplicate();
    }

    Ref<Texture2D> get_texture(const String& id) const;

    PackedStringArray get_warnings() const
    {
        return warnings;
    }

    int get_version() const
    {
        return version;
    }
};

class ResourceFormatLoaderPTCL : public ResourceFormatLoader
{
    GDCLASS(ResourceFormatLoaderPTCL, ResourceFormatLoader)

protected:
    static void _bind_methods()
    {
    }

public:
    PackedStringArray _get_recognized_extensions() const override;
    bool _handles_type(const StringName& type) const override;
    String _get_resource_type(const String& path) const override;
    Variant _load(const String& path, const String& original_path, bool use_sub_threads,
                  int32_t cache_mode) const override;
};

}
