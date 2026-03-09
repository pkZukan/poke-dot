#include "pokemon_character.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void PokemonCharacter::_bind_methods() 
{
    GETTER_SETTER_BIND(PokemonCharacter, species, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PokemonCharacter, form, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PokemonCharacter, gender, Variant::INT, PROPERTY_HINT_NONE)

    ClassDB::bind_method(D_METHOD("Initialize"), &PokemonCharacter::Initialize);

    ClassDB::bind_method(D_METHOD("Idle"), &PokemonCharacter::Idle);
    ClassDB::bind_method(D_METHOD("Walk"), &PokemonCharacter::Walk);
    ClassDB::bind_method(D_METHOD("Run"), &PokemonCharacter::Run);
    ClassDB::bind_method(D_METHOD("Roar"), &PokemonCharacter::Roar);
}

void PokemonCharacter::_enter_tree()
{
    CollisionShape3D* col = memnew(CollisionShape3D);
    Ref<CapsuleShape3D> shape;
    shape.instantiate();
    shape->set_height(1.8f);
    shape->set_radius(0.4f);
    col->set_shape(shape);
    add_child(col);

    _actor = memnew(PokemonActor);
    add_child(_actor);
}

void PokemonCharacter::_ready()
{
    //
}

void PokemonCharacter::_process(double delta)
{
    //
}

void PokemonCharacter::Initialize()
{
    Ref<CatalogEntry> catEnt = PokemonCatalog::get_singleton()->GetCatalogEntry(species, form, gender);
    _actor->SetInfo(catEnt);
    _actor->Initialize();
}

void PokemonCharacter::Idle()
{
    _actor->PlayAnim("00010_defaultidle01");
}

void PokemonCharacter::Walk()
{
    _actor->PlayAnim("00030_walk01_loop");
}

void PokemonCharacter::Run()
{
    _actor->PlayAnim("00100_run01_loop");
}

void PokemonCharacter::Roar()
{
    _actor->PlayAnim("00300_roar01");
}