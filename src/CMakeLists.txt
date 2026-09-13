#idf_component_register(SRCS "main.c"
#                        INCLUDE_DIRS ".")

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(
    SRCS ${app_sources}
    REQUIRES esp_driver_i2c
)
