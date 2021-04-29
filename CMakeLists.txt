cmake_minimum_required(VERSION 3.0)
project(CIMC)

# Host utilities
add_subdirectory(toolchain)
add_subdirectory(emulator)
add_subdirectory(utils)

# TODO: Cross-compiled utilities
