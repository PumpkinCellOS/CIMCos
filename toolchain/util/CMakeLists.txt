set(SOURCES
    "exec.cpp"
    "args.cpp")

add_library(scc-util ${SOURCES})
target_include_directories(scc-util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
