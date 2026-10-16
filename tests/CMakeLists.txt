add_test(NAME cli.reject_noninteractive COMMAND dunefall)
set_tests_properties(cli.reject_noninteractive PROPERTIES WILL_FAIL TRUE)

add_test(NAME cli.help COMMAND dunefall --help)
set_tests_properties(cli.help PROPERTIES PASS_REGULAR_EXPRESSION "Usage: dunefall")

add_test(NAME cli.version COMMAND dunefall --version)
set_tests_properties(cli.version PROPERTIES PASS_REGULAR_EXPRESSION "Dunefall ${PROJECT_VERSION}")

add_test(NAME cli.reject_unknown_option COMMAND dunefall --unknown-option)
set_tests_properties(cli.reject_unknown_option PROPERTIES WILL_FAIL TRUE)

add_executable(world_tests simulation/world_tests.cpp)
target_link_libraries(world_tests PRIVATE dunefall_simulation)
add_test(NAME simulation.world COMMAND world_tests)

add_executable(input_tests terminal/input_tests.cpp)
target_link_libraries(input_tests PRIVATE dunefall_terminal)
add_test(NAME terminal.input COMMAND input_tests)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_executable(session_probe terminal/session_probe.cpp)
target_link_libraries(session_probe PRIVATE dunefall_terminal)
add_test(NAME terminal.session COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/terminal/session_test.py" "$<TARGET_FILE:session_probe>")
set_tests_properties(terminal.session PROPERTIES TIMEOUT 30)

add_test(NAME terminal.game COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/terminal/game_test.py" "$<TARGET_FILE:dunefall>")
set_tests_properties(terminal.game PROPERTIES TIMEOUT 30)

add_test(NAME app.cli COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/app/cli_test.py" "$<TARGET_FILE:dunefall>")
set_tests_properties(app.cli PROPERTIES TIMEOUT 45)

add_executable(renderer_tests terminal/renderer_tests.cpp)
target_link_libraries(renderer_tests PRIVATE dunefall_terminal)
add_test(NAME terminal.renderer COMMAND renderer_tests)
