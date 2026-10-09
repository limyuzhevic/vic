// Test for Network Exploration Commands
#include "NetworkExplorationCommands.hpp"
#include <cassert>
#include <iostream>

void testSpikeAnalysisCommand() {
    std::cout << "Testing SpikeAnalysisCommand...\n";
    
    nlm::SpikeAnalysisCommand cmd;
    
    assert(cmd.getName() == "spike_analysis");
    assert(!cmd.getDescription().empty());
    
    // Test argument validation
    std::map<std::string, nlm::CommandArgument> args;
    args["neuron"] = 1;
    args["time"] = 10.0;
    assert(cmd.validateArguments(args));
    
    // Test execution
    nlm::CommandContext context;
    auto result = cmd.execute(context, args);
    
    std::cout << "SpikeAnalysisCommand tests passed.\n";
}

void testConnectivityCommand() {
    std::cout << "Testing ConnectivityCommand...\n";
    
    nlm::ConnectivityCommand cmd;
    
    assert(cmd.getName() == "connectivity");
    assert(!cmd.getDescription().empty());
    
    // Test execution
    nlm::CommandContext context;
    std::map<std::string, nlm::CommandArgument> args;
    args["source"] = 1;
    args["target"] = 2;
    args["threshold"] = 0.5;
    auto result = cmd.execute(context, args);
    
    std::cout << "ConnectivityCommand tests passed.\n";
}

int main() {
    try {
        testSpikeAnalysisCommand();
        testConnectivityCommand();
        std::cout << "All network exploration command tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
