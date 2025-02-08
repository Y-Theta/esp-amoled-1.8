option(ENABLE_LVGL_DEMOS "Enable LVGL demos" OFF)

file(GLOB_RECURSE HAL_SRCS
    ./utils/*.c
    ./utils/*.cpp
    ./app/*.c
    ./app/*.cpp
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

set(DIR_RESOURCES "./resources")
set(Drive_PNG_PNG   "${CMAKE_BINARY_DIR}/spiffs")
file(MAKE_DIRECTORY ${Drive_PNG_PNG})

file(GLOB_RECURSE SOURCE_FILES ${DIR_RESOURCES}/*.png)
foreach(FILE ${SOURCE_FILES})
    file(COPY ${FILE} DESTINATION ${Drive_PNG_PNG})
endforeach()

spiffs_create_partition_assets(
    spiffs
    ${Drive_PNG_PNG}
    FLASH_IN_PROJECT
    MMAP_FILE_SUPPORT_FORMAT ".png" 
    MMAP_SUPPORT_SPNG
    MMAP_SPLIT_HEIGHT 16
)