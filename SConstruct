#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

fbs_files = Glob("#schema/*.fbs")

# Generate flatc commands for each schema
generated_headers = []
for fbs in fbs_files:
    base = os.path.splitext(os.path.basename(str(fbs)))[0]
    header = f"src/generated/{base}_generated.h"

    target = env.Command(
        header,
        fbs,
        "flatc --cpp -o src/generated $SOURCE"
    )
    generated_headers.append(target)

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/", "src/model/", "src/middleware/"])
sources = [Glob("src/*.cpp"), Glob("src/**/*.cpp")]

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/gflib/gflib.{}.{}.framework/gflib.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "demo/gflib/gflib{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Clean(library, generated_headers)
Default(library)
