#idf_component_register(SRCS "LCD.c" "sht31d_sensor.c" "main.c"
#                        INCLUDE_DIRS ".")

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(
    SRCS ${app_sources}
    REQUIRES esp_driver_i2c
)
