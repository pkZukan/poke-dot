#pragma once

#include <godot_cpp/classes/control.hpp>

namespace godot {

// A layout pane is also a lookup scope for its descendants.
class TrinityPane : public Control {
    GDCLASS(TrinityPane, Control)
protected:
    static void _bind_methods();
    virtual Control *scope_root() const;
public:
    // Bare names must be unique; paths are relative to this scope.
    Control *get_pane(const String &name_or_path) const;
    TrinityPane *get_scope(const String &name_or_path) const;
    Error _set_text(const String &name_or_path, const String &text);
};

}
