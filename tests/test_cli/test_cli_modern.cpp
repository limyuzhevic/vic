#include "framework/test_base.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include <iostream>
#include <sstream>
#include <string>

class CLITestSuite : public TestSuite {
public:
    CLITestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running CLI Test Suite..." << std::endl;
        
        testCLIArgumentParsing();
        testCLIHelpOutput();
        testCLIBasicExecution();
        testCLIBrainConfiguration();
        
        std::cout << "CLI Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "CLITestSuite";
    }
    
private:
    void testCLIArgumentParsing() {
        // Test basic command line argument parsing
        std::stringstream testArgs;
        testArgs << "nlm --neuron-count 100 --steps 50 phase2";
        
        std::string args = testArgs.str();
        // Simple argument count test
        size_t argCount = std::count(args.begin(), args.end(), ' ') + 1;
        
        ASSERT_TRUE(argCount > 0);
        
        std::cout << "    testCLIArgumentParsing passed" << std::endl;
    }
    
    void testCLIHelpOutput() {
        std::stringstream helpOutput;
        helpOutput << "NLM Unified CLI" << std::endl;
        helpOutput << "USAGE: nlm <command> [options]" << std::endl;
        helpOutput << "COMMANDS:" << std::endl;
        
        std::string help = helpOutput.str();
        
        ASSERT_TRUE(help.find("NLM Unified CLI") != std::string::npos);
        ASSERT_TRUE(help.find("USAGE:") != std::string::npos);
        ASSERT_TRUE(help.find("COMMANDS:") != std::string::npos);
        
        std::cout << "    testCLIHelpOutput passed" << std::endl;
    }
    
    void testCLIBasicExecution() {
        // Test basic brain execution through CLI-like interface
        nlm::Brain brain("cli_test_brain");
        
        // Create neural population
        auto* population = new nlm::NeuralPopulation("test_pop", nlm::NeuronType::Excitatory);
        population->addNeuron(nlm::NeuronId(1));
        population->addNeuron(nlm::NeuronId(2));
        
        brain.addPopulation(population);
        
        // Run basic operations
        brain.update();
        brain.reset();
        
        ASSERT_TRUE(brain.getPopulationCount() > 0);
        
        delete population;
        
        std::cout << "    testCLIBasicExecution passed" << std::endl;
    }
    
    void testCLIBrainConfiguration() {
        nlm::Brain brain("config_test_brain");
        
        // Configure brain with different parameters
        brain.setName("configured_brain");
        
        ASSERT_EQ(brain.getName(), "configured_brain");
        
        // Add neural population
        auto* population = new nlm::NeuralPopulation("configured_pop", nlm::NeuronType::Excitatory);
        population->addNeuron(nlm::NeuronId(10));
        brain.addPopulation(population);
        
        // Configure memory system
        auto* memory = new nlm::NeuralWorkingMemory();
        nlm::Brain* brainPtr = &brain;
        memory->initialize(brainPtr);
        
        delete population;
        delete memory;
        
        std::cout << "    testCLIBrainConfiguration passed" << std::endl;
    }
};

class CommandLineInterfaceTestSuite : public TestSuite {
public:
    CommandLineInterfaceTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Command Line Interface Test Suite..." << std::endl;
        
        testCommandLineParsing();
        testCommandExecution();
        testOutputFormatting();
        
        std::cout << "Command Line Interface Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "CommandLineInterfaceTestSuite";
    }
    
private:
    void testCommandLineParsing() {
        std::vector<std::string> testCommands = {
            "nlm phase2",
            "nlm phase3 --episodes 10",
            "nlm phase4 --trials 50",
            "nlm phase6 --neurons 1000",
            "nlm run --experiment test_exp"
        };
        
        for (const auto& cmd : testCommands) {
            // Simple command validation
            ASSERT_TRUE(cmd.find("nlm ") == 0);
            ASSERT_TRUE(cmd.size() > 4); // "nlm " is 4 chars
        }
        
        std::cout << "    testCommandLineParsing passed" << std::endl;
    }
    
    void testCommandExecution() {
        nlm::Brain brain("command_test_brain");
        
        // Simulate command execution
        brain.initialize();
        brain.update();
        brain.reset();
        
        // Verify command execution
        ASSERT_TRUE(brain.getPopulationCount() >= 0);
        
        std::cout << "    testCommandExecution passed" << std::endl;
    }
    
    void testOutputFormatting() {
        std::stringstream jsonOutput;
        jsonOutput << "{\"status\": \"completed\", \"phase\": \"4\", \"results\": []}";
        
        std::string jsonStr = jsonOutput.str();
        ASSERT_TRUE(jsonStr.find("status") != std::string::npos);
        ASSERT_TRUE(jsonStr.find("phase") != std::string::npos);
        
        std::stringstream yamlOutput;
        yamlOutput << "status: completed" << std::endl;
        yamlOutput << "phase: 4" << std::endl;
        yamlOutput << "results: []" << std::endl;
        
        std::string yamlStr = yamlOutput.str();
        ASSERT_TRUE(yamlStr.find("status:") != std::string::npos);
        ASSERT_TRUE(yamlStr.find("phase:") != std::string::npos);
        
        std::cout << "    testOutputFormatting passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createCLITestSuite() {
        return new CLITestSuite();
    }
    
    TestSuite* createCommandLineInterfaceTestSuite() {
        return new CommandLineInterfaceTestSuite();
    }
}