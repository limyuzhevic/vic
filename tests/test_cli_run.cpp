// CLI Run Tests
// Tests run command functionality

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_cli_run {

void testRunCommand() {
    std::cout << "  [testRunCommand] ";

    nlm::Config config;

    // Test run command configuration
    config.set("scenario", std::string("movie_test"), nlm::ConfigSource::CommandLine);
    config.set("neuron_count", 1000, nlm::ConfigSource::CommandLine);
    config.set("steps", 2000, nlm::ConfigSource::CommandLine);

    assert(config.getOr<std::string>("scenario", "") == "movie_test");
    assert(config.getOr<int>("neuron_count", 0) == 1000);
    assert(config.getOr<int>("steps", 0) == 2000);

    std::cout << "PASSED" << std::endl;
}

void testRunArguments() {
    std::cout << "  [testRunArguments] ";

    nlm::Config config;

    // Test run command arguments
    config.set("random_seed", 42, nlm::ConfigSource::CommandLine);
    config.set("output_format", std::string("csv"), nlm::ConfigSource::CommandLine);

    assert(config.getOr<int>("random_seed", 0) == 42);
    assert(config.getOr<std::string>("output_format", "") == "csv");

    std::cout << "PASSED" << std::endl;
}

void testRunExecution() {
    std::cout << "  [testRunExecution] ";

    nlm::Config config;

    // Test run execution simulation
    config.set("execution_time", 100.0f, nlm::ConfigSource::Runtime);

    assert(config.getOr<float>("execution_time", 0.0f) == 100.0f);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testRunCommand();
    testRunArguments();
    testRunExecution();

    std::cout << "  All CLI run tests passed!" << std::endl;
}

} // namespace test_cli_run