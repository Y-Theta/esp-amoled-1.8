file(GLOB_RECURSE HAL_SRCS
    ./display/*.c
    ./display/*.cpp
)

set(APP_LAYER_INCS
    
)

idf_component_register(SRCS "main.cpp" ${HAL_SRCS}
                    INCLUDE_DIRS "." ${APP_LAYER_INCS})

set_source_files_properties(
    ${LV_DEMOS_SOURCES}
    PROPERTIES COMPILE_OPTIONS
    -DLV_LVGL_H_INCLUDE_SIMPLE)
