add_test(NAME cli.workspace COMMAND dunefall)
set_tests_properties(cli.workspace PROPERTIES PASS_REGULAR_EXPRESSION "Workspace ready")

add_test(NAME cli.help COMMAND dunefall --help)
set_tests_properties(cli.help PROPERTIES PASS_REGULAR_EXPRESSION "Usage: dunefall")

add_test(NAME cli.version COMMAND dunefall --version)
set_tests_properties(cli.version PROPERTIES PASS_REGULAR_EXPRESSION "Dunefall ${PROJECT_VERSION}")

add_test(NAME cli.reject_unknown_option COMMAND dunefall --unknown-option)
set_tests_properties(cli.reject_unknown_option PROPERTIES WILL_FAIL TRUE)

add_executable(world_tests simulation/world_tests.cpp)
target_link_libraries(world_tests PRIVATE dunefall_simulation)
add_test(NAME simulation.world COMMAND world_tests)
