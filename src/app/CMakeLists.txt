add_library(dunefall_app STATIC app.cpp game.cpp options.cpp headless.cpp)
target_include_directories(dunefall_app PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(dunefall_app PUBLIC dunefall_options dunefall_simulation dunefall_terminal)
target_compile_definitions(dunefall_app PRIVATE DUNEFALL_VERSION="${PROJECT_VERSION}")

add_executable(dunefall main.cpp)
target_link_libraries(dunefall PRIVATE dunefall_app)
set_target_properties(dunefall PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
