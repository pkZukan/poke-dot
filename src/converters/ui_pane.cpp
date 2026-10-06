#include "ui_pane.h"
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

void TrinityPane::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_pane", "name_or_path"), &TrinityPane::get_pane);
    ClassDB::bind_method(D_METHOD("get_scope", "name_or_path"), &TrinityPane::get_scope);
    ClassDB::bind_method(D_METHOD("_set_text", "name_or_path", "text"), &TrinityPane::_set_text);
}

Control *TrinityPane::scope_root() const {
    return const_cast<TrinityPane *>(this);
}

Control *TrinityPane::get_pane(const String &name_or_path) const {
    Control *root = scope_root();
    if (!root) return nullptr;
    if (name_or_path == ".") return root;
    if (name_or_path.contains("/"))
    {
        NodePath path(name_or_path);
        if (path.is_absolute() || path.get_subname_count() != 0) return nullptr;
        Control *pane = Object::cast_to<Control>(root->get_node_or_null(path));
        return pane && pane->has_meta("bflyt_pane") && (pane == root || root->is_ancestor_of(pane)) ? pane : nullptr;
    }

    Control *match = nullptr;
    Vector<Node *> pending;
    pending.push_back(root);
    while (!pending.is_empty()) {
        Node *node = pending[pending.size() - 1];
        pending.resize(pending.size() - 1);
        Dictionary metadata = node->get_meta("bflyt_pane", Dictionary());
        if (!metadata.is_empty() && String(metadata.get("name", "")) == name_or_path) {
            Control *pane = Object::cast_to<Control>(node);
            ERR_FAIL_COND_V_MSG(match && pane, nullptr,
                "Ambiguous UI pane '" + name_or_path + "'; use get_scope() or a path relative to this scope.");
            if (pane) match = pane;
        }
        for (int i = 0; i < node->get_child_count(); ++i) pending.push_back(node->get_child(i));
    }
    return match;
}

TrinityPane *TrinityPane::get_scope(const String &name_or_path) const {
    return Object::cast_to<TrinityPane>(get_pane(name_or_path));
}

Error TrinityPane::_set_text(const String &name_or_path, const String &text) {
    Control *pane = get_pane(name_or_path);
    if (!pane) return ERR_DOES_NOT_EXIST;
    Label *label = Object::cast_to<Label>(pane);
    ERR_FAIL_NULL_V_MSG(label, ERR_INVALID_PARAMETER, "UI pane is not a text label: " + name_or_path);
    label->set_text(text);
    return OK;
}
