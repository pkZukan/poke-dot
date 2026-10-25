# poke-dot
Pokemon ZA asset file support for Godot

## Screenshot
![godot](https://i.imgur.com/5L7Uw4y.png)

## Usage
1. Dump asset files somewhere in your project folder (a symlink to them works too)
2. Copy over the gflib folder from this repo into the root of your project. It must stay at `res://gflib/`, since the converted materials reference its shaders. Build or provide the library for each platform/build type you target (see `gflib.gdextension`)
3. Go into Project Settings -> Gflib -> Romfs Root and set the asset folder, then restart the editor (the setting is read at startup)
4. To load a map, add a `TrinitySceneLoader`, set `scene_path` to a `.trscn` from the assets, and call `load_scene()`
5. If using TrinityEnvironment, point the master light file to a `.trlgt` from the assets (e.g. `light/master/master/master.trlgt`). Assign the loaded resource as-is: probe, sky and shadow textures are resolved relative to the `.trlgt`'s path, so a `duplicate()`d or script-built copy silently loses them. Use `set_layer()` to stack changes on top of it instead.
