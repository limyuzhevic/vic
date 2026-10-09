// Test for Monitoring Commands
#include "MonitoringCommands.hpp"
#include <cassert>
#include <iostream>

void testBrainStatusCommand() {
    std::cout << "Testing BrainStatusCommand...\n";
    
    nlm::BrainStatusCommand cmd;
    
    assert(cmd.getName() == "status");
    assert(!cmd.getDescription().empty());
    assert(!cmd.getUsage().empty());
    
    // Test argument validation
    std::map<std::string, nlm::CommandArgument> args;
    args["region"] = 1;
    assert(cmd.validateArguments(args));
    
    // Test execution
    nlm::CommandContext context;
    auto result = cmd.execute(context, args);
    
    std::cout << "BrainStatusCommand tests passed.\n";
}

void testBrainMetricsCommand() {
    std::cout << "Testing BrainMetricsCommand...\n";
    
    nlm::BrainMetricsCommand cmd;
    
    assert(cmd.getName() == "metrics");
    assert(!cmd.getDescription().empty());
    
    // Test execution
    nlm::CommandContext context;
    std::map<std::string, nlm::CommandArgument> args;
    args["detailed"] = true;
    auto result = cmd.execute(context, args);
    
    std::cout << "BrainMetricsCommand tests passed.\n";
}

int main() {
    try {
        testBrainStatusCommand();
        testBrainMetricsCommand();
        std::cout << "All monitoring command tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
