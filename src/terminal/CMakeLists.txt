add_library(dunefall_terminal STATIC input.cpp session.cpp renderer.cpp)
target_include_directories(dunefall_terminal PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(dunefall_terminal PUBLIC dunefall_options dunefall_simulation)
