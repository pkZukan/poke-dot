#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/Ikkaku/TrinityScene/trinity_LightApplierComponent_generated.h"
#include "utils.h"

namespace godot
{

class TrinityLightApplierComponent : public Resource
{
    GDCLASS(TrinityLightApplierComponent, Resource)
protected:
    static void _bind_methods();

public:
    void LoadFromBuffer(const void* buffer);

    GETTER_SETTER_DEFINE(String, FilePath)
    GETTER_SETTER_DEFINE(double, TimeSpan)
    GETTER_SETTER_DEFINE(double, CurrentTime)
    GETTER_SETTER_DEFINE(bool, StartWithOffsetData)
    GETTER_SETTER_DEFINE(String, OffsetDataName)
    GETTER_SETTER_DEFINE(double, OffsetBlendWeight)
    GETTER_SETTER_DEFINE(bool, ApplyGameObjectSRT)
    GETTER_SETTER_DEFINE(int, Priority)
    GETTER_SETTER_DEFINE(bool, UniqueFlag)
    GETTER_SETTER_DEFINE(bool, UseInstancerSRT)

private:
    String FilePath = String();
    double TimeSpan = 0;
    double CurrentTime = 0;
    bool StartWithOffsetData = false;
    String OffsetDataName = String();
    double OffsetBlendWeight = 0;
    bool ApplyGameObjectSRT = false;
    int Priority = 0;
    bool UniqueFlag = false;
    bool UseInstancerSRT = false;
};

}
