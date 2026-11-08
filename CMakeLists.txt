# standalone colour-science: sRGB EOTF/OETF, luminance, OKLab/OKLCh, Planckian + CCT
# include/bencolour/colour.glsl mirrors the maths for shaders (sRGB, OKLab, luminance, XYZ)
# pure C, float32, deterministic, only depends on libm

add_library(libbencolour SHARED
    src/srgb.c
    src/oklab.c
    src/planckian.c
)
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
