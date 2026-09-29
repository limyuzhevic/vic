# The test_dopamine.cpp file in the root directory has been moved to tests/test_dopamine.cpp
# All Dopamine tests are now properly integrated into the main test suite
# 
# The test_dopamine.cpp in the tests directory contains:
# - Implementation of Dopamine tests (testDopamineBasic, testDopamineRewardSignaling, etc.)
# - Test runner function (test_dopamine::runAll())
# 
# The tests/test_main.cpp contains the main() function that:
# - Runs all test suites including Dopamine tests
# - Provides proper test reporting and error handling
# - Integrates with the CMake test system