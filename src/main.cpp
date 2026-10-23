// NLM Test Runner Main
//
// This is the new main.cpp file that serves as the entry point for the NLM test suite.
// It uses the TestRunner class to orchestrate all tests with improved error handling,
// command-line argument parsing, and logging capabilities. The test sequence maintains
// backward compatibility while adding modularity and configurability.

#include "tests/TestRunner.hpp"

int main(int argc, char** argv) {
    nlm::tests::TestRunner runner(argc, argv);
    return runner.run();
}
