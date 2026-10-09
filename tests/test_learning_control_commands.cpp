// Test for Learning Control Commands
#include "LearningControlCommands.hpp"
#include <cassert>
#include <iostream>

void testPlasticityEnableCommand() {
    std::cout << "Testing PlasticityEnableCommand...\n";
    
    nlm::PlasticityEnableCommand cmd;
    
    assert(cmd.getName() == "plasticity_enable");
    assert(!cmd.getDescription().empty());
    
    // Test argument validation
    std::map<std::string, nlm::CommandArgument> args;
    args["hebbian"] = true;
    args["stdp"] = true;
    args["structural"] = false;
    assert(cmd.validateArguments(args));
    
    // Test execution
    nlm::CommandContext context;
    auto result = cmd.execute(context, args);
    
    std::cout << "PlasticityEnableCommand tests passed.\n";
}

void testLearningRateCommand() {
    std::cout << "Testing LearningRateCommand...\n";
    
    nlm::LearningRateCommand cmd;
    
    assert(cmd.getName() == "learning_rate");
    assert(!cmd.getDescription().empty());
    
    // Test argument validation
    std::map<std::string, nlm::CommandArgument> args;
    args["hebbian"] = 0.01;
    args["stdp"] = 0.02;
    args["structural"] = 0.001;
    assert(cmd.validateArguments(args));
    
    // Test execution with invalid values
    std::map<std::string, nlm::CommandArgument> invalidArgs;
    invalidArgs["hebbian"] = 1.5;  // Invalid (>1.0)
    assert(!cmd.validateArguments(invalidArgs));
    
    std::cout << "LearningRateCommand tests passed.\n";
}

int main() {
    try {
        testPlasticityEnableCommand();
        testLearningRateCommand();
        std::cout << "All learning control command tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
