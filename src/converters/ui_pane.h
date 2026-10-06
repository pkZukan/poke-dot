#pragma once

#include <godot_cpp/classes/control.hpp>

namespace godot
{

class TrinityPane : public Control {
    GDCLASS(TrinityPane, Control)

protected:
    static void _bind_methods();

    virtual Control *scope_root() const;

public:
    Control *get_pane(const String &name_or_path) const;
    TrinityPane *get_scope(const String &name_or_path) const;
    Error _set_text(const String &name_or_path, const String &text);
};

}
