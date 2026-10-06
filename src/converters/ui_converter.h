#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include "middleware/sarc.h"
#include "ui/ui_state_runtime.h"
#include "ui_builder.h"
#include "ui_pane.h"

namespace godot
{

class TrinityUI : public TrinityPane {
    GDCLASS(TrinityUI, TrinityPane)

protected:
    static void _bind_methods();
    void _notification(int what);
    Control *scope_root() const override;

public:
    Error load_ui(const String &truiv_path, const String &arc_path, const NodePath &parent_path);
    Ref<Font> get_font(const String &p_name);
    Error apply_state(const String &component, const String &state, double frame = 0.0);
    PackedStringArray get_warnings() const;

private:
    Control *layout_node() const;
    void clear_loaded_ui();
    void fit_layout();

    UIStateRuntime state_runtime;
    UIFontCache fonts;
    PackedStringArray warnings;
    Control *layout = nullptr;
    uint64_t layout_id = 0;
};

}
