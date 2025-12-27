cmake_minimum_required(VERSION 3.12)

set(APP_NAME "fixbrot")

# Pull in PICO SDK (must be before project)
set(PICOSYSTEM_DIR $ENV{PICOSYSTEM_DIR})
include(${PICOSYSTEM_DIR}/pico_sdk_import.cmake)

# Initialize the SDK
pico_sdk_init()

project(${APP_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Find the PicoSystem SDK
find_package(PICOSYSTEM REQUIRED)

# Set up your project and sources
picosystem_executable(
  ${APP_NAME}
  src/fixbrot.cpp
)

add_definitions(-DFIXBROT_ITER_12BIT=1)
add_definitions(-DFIXBROT_ARGB4444_BSWAP=1)

target_include_directories(${APP_NAME} PRIVATE
  ../../lib/include
  ../../submodules/Adafruit-GFX-Library
)

target_link_libraries(${APP_NAME} 
  pico_multicore
)

# Example build options
# pixel_double(${APP_NAME})
no_spritesheet(${APP_NAME})
disable_startup_logo(${APP_NAME})
