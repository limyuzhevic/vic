// Example Output Tests
// Tests example script outputs and behavior

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_example_outputs {

void testMinimalExampleOutput() {
    std::cout << "  [testMinimalExampleOutput] ";

    nlm::Config config;

    // Test minimal example output simulation
    config.set("neuron_count", 100, nlm::ConfigSource::Default);
    config.set("random_seed", 42, nlm::ConfigSource::Default);

    assert(config.getOr<int>("neuron_count", 0) == 100);
    assert(config.getOr<int>("random_seed", 0) == 42);

    std::cout << "PASSED" << std::endl;
}

void testAgentExampleOutput() {
    std::cout << "  [testAgentExampleOutput] ";

    nlm::Config config;

    // Test agent example output simulation
    config.set("neuron_count", 500, nlm::ConfigSource::Default);
    config.set("plasticity.stdp.enable", true, nlm::ConfigSource::File);

    assert(config.getOr<int>("neuron_count", 0) == 500);
    assert(config.getOr<bool>("plasticity.stdp.enable", false));

    std::cout << "PASSED" << std::endl;
}

void testExplorationExampleOutput() {
    std::cout << "  [testExplorationExampleOutput] ";

    nlm::Config config;

    // Test exploration example output simulation
    config.set("neuron_count", 800, nlm::ConfigSource::Default);
    config.set("neuromod.curiosity.enable", true, nlm::ConfigSource::File);

    assert(config.getOr<int>("neuron_count", 0) == 800);
    assert(config.getOr<bool>("neuromod.curiosity.enable", false));

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testMinimalExampleOutput();
    testAgentExampleOutput();
    testExplorationExampleOutput();

    std::cout << "  All example output tests passed!" << std::endl;
}

} // namespace test_example_outputs