#include "trinity_CollisionComponent.h"

using namespace godot;

void ModelShape::_bind_methods()
{
    GETTER_SETTER_BIND(ModelShape, path, Variant::STRING, PROPERTY_HINT_NONE)
}

void SphereShape::_bind_methods()
{
    GETTER_SETTER_BIND(SphereShape, pos, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SphereShape, radius, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void BoxShape::_bind_methods()
{
    GETTER_SETTER_BIND(BoxShape, pos, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoxShape, size, Variant::VECTOR3, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(BoxShape, rot, Variant::VECTOR3, PROPERTY_HINT_NONE)
}

void CapsuleShape::_bind_methods()
{
    GETTER_SETTER_BIND(CapsuleShape, pos, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(CapsuleShape, radius, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(CapsuleShape, height, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void PencilShape::_bind_methods()
{
    GETTER_SETTER_BIND(PencilShape, pos, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PencilShape, radius, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PencilShape, cyl_height, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PencilShape, tip_height, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PencilShape, slice_count, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PencilShape, rot, Variant::VECTOR3, PROPERTY_HINT_NONE)
}

void TrinityBodyCollision::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityBodyCollision, shape, Variant::OBJECT, PROPERTY_HINT_NONE)
}

void TrinityCharacterCollision::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityCharacterCollision, shape, Variant::OBJECT, PROPERTY_HINT_NONE)
}

void TrinityCollisionComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityCollisionComponent, collision, Variant::OBJECT, PROPERTY_HINT_NONE)
}

Ref<Resource> TrinityCollisionComponent::_ParseShapeType(Titan::TrinityScene::ColShapeType col_shape, const void* data)
{
    switch(col_shape)
    {
        case Titan::TrinityScene::ColShapeType::ColShapeType_SphereShape:
        {
            auto src = static_cast<const Titan::TrinityScene::SphereShape*>(data);
            Ref<SphereShape> sphere_shape;
            sphere_shape.instantiate();
            sphere_shape->set_pos(Utils::toGodotVec3(src->pos()));
            sphere_shape->set_radius(src->radius());
            return sphere_shape;
        }
        case Titan::TrinityScene::ColShapeType::ColShapeType_BoxShape:
        {
            auto src = static_cast<const Titan::TrinityScene::BoxShape*>(data);
            Ref<BoxShape> box_shape;
            box_shape.instantiate();
            box_shape->set_pos(Utils::toGodotVec3(src->pos()));
            box_shape->set_size(Utils::toGodotVec3(src->size()));
			box_shape->set_rot(Utils::toGodotVec3(src->rot()));
            return box_shape;
        }
        case Titan::TrinityScene::ColShapeType::ColShapeType_CapsuleShape:
        {
            auto src = static_cast<const Titan::TrinityScene::CapsuleShape*>(data);
            Ref<CapsuleShape> capsule_shape;
            capsule_shape.instantiate();
            capsule_shape->set_pos(Utils::toGodotVec3(src->pos()));
            capsule_shape->set_radius(src->radius());
            capsule_shape->set_height(src->height());
            return capsule_shape;
        }
        case Titan::TrinityScene::ColShapeType::ColShapeType_ModelShape:
        {
            auto src = static_cast<const Titan::TrinityScene::ModelShape*>(data);
            Ref<ModelShape> model_shape;
            model_shape.instantiate();
            model_shape->set_path(Utils::toGodotString(src->file_path()));
            return model_shape;
        }
        case Titan::TrinityScene::ColShapeType::ColShapeType_PencilShape:
        {
            auto src = static_cast<const Titan::TrinityScene::PencilShape*>(data);
            Ref<PencilShape> pencil_shape;
            pencil_shape.instantiate();
            pencil_shape->set_pos(Utils::toGodotVec3(src->pos()));
            pencil_shape->set_radius(src->radius());
            pencil_shape->set_cyl_height(src->cyl_height());
            pencil_shape->set_tip_height(src->tip_height());
            pencil_shape->set_slice_count(src->slice_count());
            pencil_shape->set_rot(Utils::toGodotVec3(src->rot()));
            return pencil_shape;
        }
        default:
            return Ref<Resource>();
    }
}

Ref<Resource> TrinityCollisionComponent::_ParseCollisionType(Titan::TrinityScene::Collision collision, const void* data)
{
    switch(collision)
    {
        case Titan::TrinityScene::Collision::Collision_BodyCollision:
        {
            auto src = static_cast<const Titan::TrinityScene::BodyCollision*>(data);
            Ref<TrinityBodyCollision> body_collision;
            body_collision.instantiate();

            Ref<Resource> shape = _ParseShapeType(src->shape_type(), src->shape());
            body_collision->set_shape(shape);
            return body_collision;
        }
        case Titan::TrinityScene::Collision::Collision_CharCollision:
        {
            return Ref<Resource>();
        }
        default:
            return Ref<Resource>();
    }

}

void TrinityCollisionComponent::LoadFromBuffer(const void* buffer)
{
    auto collision_component = Titan::TrinityScene::GetTrinityCollisionComponent(buffer);
    ERR_FAIL_COND_MSG(collision_component == nullptr, "Couldn't parse collision component");
    
    auto collision = _ParseCollisionType(collision_component->collision_type(), collision_component->collision());
    set_collision(collision);
}