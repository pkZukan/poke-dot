#!/usr/bin/env python
import os
import sys
import glob as pyglob

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

fbs_files = pyglob.glob("schema/**/*.fbs", recursive=True)

# Generate flatc commands for each schema
generated_headers = []
for fbs in fbs_files:
    base = os.path.splitext(os.path.relpath(str(fbs), "schema"))[0]
    header = f"src/generated/{base}_generated.h"

    target = env.Command(
        header,
        fbs,
        f"flatc --cpp --keep-prefix --include-prefix generated -I schema -o {os.path.dirname(header)} $SOURCE"
    )
    generated_headers.append(target)

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/", "src/model/", "src/middleware/"])
sources = pyglob.glob("src/**/*.cpp", recursive=True)

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
