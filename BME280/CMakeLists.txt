add_library(BME280 BME280.cpp)

target_link_libraries(BME280 pico_stdlib hardware_i2c)

target_include_directories(BME280 PUBLIC ${CMAKE_CURRENT_LIST_DIR})