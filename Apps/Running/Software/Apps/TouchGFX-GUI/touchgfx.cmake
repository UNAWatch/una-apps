file(GLOB_RECURSE TOUCHGFX_SOURCES
    gui/src/*.cpp
    generated/fonts/src/*.cpp
    generated/gui_generated/src/*.cpp
    generated/images/src/*.cpp
    generated/texts/src/*.cpp
)

set(TOUCHGFX_INCLUDE_DIRS
    touchgfx/framework/include
    generated/fonts/include
    generated/gui_generated/include
    generated/images/include
    generated/texts/include
    generated/videos/include
    gui/include
)