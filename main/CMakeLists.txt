option(ENABLE_LVGL_DEMOS "Enable LVGL demos" OFF)

file(GLOB_RECURSE HAL_SRCS
    ./utils/*.c
    ./utils/*.cpp
)

set(APP_LAYER_INCS

)

idf_component_register(SRCS "main.cpp" ${HAL_SRCS}
    INCLUDE_DIRS "." ${APP_LAYER_INCS})

if(ENABLE_LVGL_DEMOS)
    set_source_files_properties(
        ${LV_DEMOS_SOURCES}
        PROPERTIES COMPILE_OPTIONS
        -DLV_LVGL_H_INCLUDE_SIMPLE)
endif()
