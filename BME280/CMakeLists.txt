add_library(bme280 bme280.c)

target_link_libraries(bme280 pico_stdlib hardware_i2c)

target_include_directories(bme280 PUBLIC ${CMAKE_CURRENT_LIST_DIR})