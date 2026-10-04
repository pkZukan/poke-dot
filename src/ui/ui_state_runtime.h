#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <map>
#include <vector>

namespace godot {

// Owned by TrinityUI. Bindings use paths, never owning or dangling node pointers.
class UIStateRuntime {
    struct Instance {
        String layout_name;
        std::map<String, std::vector<NodePath>> panes;
        std::map<String, std::vector<NodePath>> materials;
    };
    std::map<String, Instance> instances;
    std::map<String, std::vector<NodePath>> components;
    Dictionary animations;
    Dictionary animation_files;
    bool initialized = false;

    void index(Node *owner, Control *node, Instance *instance);
    static double sample(const Dictionary &track, double frame);
    static void apply_track(Node *owner, Control *pane, Polygon2D *picture,
        const String &kind, const Dictionary &track, double value, const PackedStringArray &textures);

public:
    void reset(Node *owner, Control *layout);
    Error apply(Node *owner, Control *layout, const String &component, const String &state, double frame);
};
}
