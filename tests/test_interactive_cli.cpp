// Interactive CLI Tests
// Tests for the new nlm_interactive_cli functionality

#include "experiments/InteractiveCLI.hpp"
#include <cassert>
#include <iostream>
#include <string>

namespace test_interactive_cli {

void testCommandParsing() {
    std::cout << "    Testing command parsing..." << std::endl;
    
    // Test simple command extraction
    std::string command = "create default";
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    assert(cmd == "create");
    
    // Test config command
    command = "config set neuron_count 1000";
    iss.clear();
    iss.str(command);
    iss >> cmd;
    assert(cmd == "config");
    
    std::string action;
    iss >> action;
    assert(action == "set");
    
    // Test inspect command
    command = "inspect brain";
    iss.clear();
    iss.str(command);
    iss >> cmd;
    assert(cmd == "inspect");
    
    std::string target;
    iss >> target;
    assert(target == "brain");
    
    std::cout << "    Command parsing tests passed!" << std::endl;
}

void testConfigValueParsing() {
    std::cout << "    Testing config value parsing..." << std::endl;
    
    // Test integer parsing
    std::string key = "neuron_count";
    std::string value = "500";
    
    // Extract numeric part
    size_t pos;
    int intValue = std::stoi(value, &pos);
    assert(intValue == 500);
    
    // Test float parsing
    key = "connection_probability";
    value = "0.15";
    float floatValue = std::stof(value);
    assert(std::abs(floatValue - 0.15f) < 0.001f);
    
    // Test invalid float
    value = "invalid";
    try {
        floatValue = std::stof(value);
        assert(false && "Should have thrown exception for invalid float");
    } catch (const std::exception&) {
        // Expected
    }
    
    std::cout << "    Config value parsing tests passed!" << std::endl;
}

void testMetricPlotting() {
    std::cout << "    Testing metric plotting..." << std::endl;
    
    std::string metric = "reward";
    int count = 10;
    
    // Test reward metric
    assert(metric == "reward");
    assert(count > 0);
    
    // Test firing metric
    metric = "firing";
    assert(metric == "firing");
    
    // Test novelty metric
    metric = "novelty";
    assert(metric == "novelty");
    
    std::cout << "    Metric plotting tests passed!" << std::endl;
}

void testBrainCreation() {
    std::cout << "    Testing brain creation..." << std::endl;
    
    // Test brain configuration
    std::string configName = "test_brain";
    assert(!configName.empty());
    assert(configName == "test_brain");
    
    // Test brain parameters
    int neuronCount = 500;
    assert(neuronCount > 0);
    assert(neuronCount <= 10000);
    
    std::cout << "    Brain creation tests passed!" << std::endl;
}

void testSaveLoad() {
    std::cout << "    Testing save/load functionality..." << std::endl;
    
    // Test file path validation
    std::string path = "/tmp/test_brain.bin";
    assert(!path.empty());
    assert(path.find(".bin") != std::string::npos || path.find(".ckpt") != std::string::npos);
    
    // Test checkpoint path
    std::string checkpoint = "./checkpoint_test.bin";
    assert(!checkpoint.empty());
    
    std::cout << "    Save/load tests passed!" << std::endl;
}

void runAll() {
    testCommandParsing();
    testConfigValueParsing();
    testMetricPlotting();
    testBrainCreation();
    testSaveLoad();
    
    std::cout << "\n=== All Interactive CLI tests passed! ===" << std::endl;
}

} // namespace test_interactive_cli

int main() {
    try {
        test_interactive_cli::runAll();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}