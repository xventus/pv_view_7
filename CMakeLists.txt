# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(PV_VIEW_7)

# Partition table
idf_build_set_property(PARTITION_TABLE_FILE "${CMAKE_SOURCE_DIR}/partition_table/partition_table.csv")

# Check
message(STATUS "Using partition table file: ${PARTITION_TABLE_FILE}")

# SPIFFS partition image
spiffs_create_partition_image(spiffs ${CMAKE_SOURCE_DIR}/data FLASH_IN_PROJECT)


