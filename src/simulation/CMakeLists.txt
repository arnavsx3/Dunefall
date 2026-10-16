add_library(dunefall_simulation STATIC world.cpp)
target_include_directories(dunefall_simulation PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(dunefall_simulation PUBLIC dunefall_options)
