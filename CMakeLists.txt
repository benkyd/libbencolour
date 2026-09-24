# standalone CPU colour-science: sRGB EOTF/OETF, OKLab, Planckian + CCT
# pure C, float32, deterministic, only depends on libm

# Emscripten has no installed-shared-library model (no SONAME/dylib
# story a browser could resolve at runtime) - a single self-contained
# static .wasm binary is the natural shape there, unlike the SIDE_MODULE
# dynamic-linking mode which would be unnecessary complexity for a
# 3-file pure-math library.
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    add_library(libbencolour STATIC
        src/srgb.c
        src/oklab.c
        src/planckian.c
    )
else()
    add_library(libbencolour SHARED
        src/srgb.c
        src/oklab.c
        src/planckian.c
    )
endif()
target_include_directories(libbencolour PUBLIC include)
target_compile_definitions(libbencolour PRIVATE BENCOLOUR_BUILDING_LIB)
if(WIN32)
    target_compile_definitions(libbencolour PUBLIC BENCOLOUR_SHARED)
endif()
set_target_properties(libbencolour PROPERTIES
    OUTPUT_NAME bencolour
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    VERSION 1.0.1
    SOVERSION 1
)
if(APPLE)
    set_target_properties(libbencolour PROPERTIES
        INSTALL_NAME_DIR "@rpath"
        BUILD_WITH_INSTALL_NAME_DIR TRUE
    )
endif()
