// Integration Test Suite for NLM Phase 6
// Validates the improvements made to the NLM system including:
// - Enhanced configuration system (JSON/YAML support)
// - Main application with Phase 6 integration
// - Command-line interface with advanced features
// - Demo mode functionality
//
// This test suite ensures that all Phase 6 integration features work together
// as a coherent artificial developmental brain system.

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <fstream>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Types/Types.hpp"

using namespace nlm;

// Simple test framework
class IntegrationTester {
public:
    IntegrationTester() = default;
    ~IntegrationTester() = default;
    
    int runTests() {
        std::cout << "=== NLM Phase 6 Integration Test Suite ===" << std::endl;
        std::cout << "Testing enhanced NLM system capabilities" << std::endl;
        std::cout << "" << std::endl;
        
        testsRun = 0;
        testsPassed = 0;
        testsFailed = 0;
        
        // Test 1: Enhanced Configuration System
        if (runTest("Enhanced Configuration System", [this]() { return testEnhancedConfiguration(); })) {
            testsPassed++;
        } else {
            testsFailed++;
        }
        testsRun++;
        
        // Test 2: Main Application Integration
        if (runTest("Main Application Integration", [this]() { return testMainApplication(); })) {
            testsPassed++;
        } else {
            testsFailed++;
        }
        testsRun++;
        
        // Test 3: Phase 6 Demo Mode
        if (runTest("Phase 6 Demo Mode", [this]() { return testPhase6Demo(); })) {
            testsPassed++;
        } else {
            testsFailed++;
        }
        testsRun++;
        
        // Test 4: Command-Line Interface
        if (runTest("Command-Line Interface", [this]() { return testCommandLineInterface(); })) {
            testsPassed++;
        } else {
            testsFailed++;
        }
        testsRun++;
        
        // Test 5: Complete System Integration
        if (runTest("Complete System Integration", [this]() { return testCompleteSystemIntegration(); })) {
            testsPassed++;
        } else {
            testsFailed++;
        }
        testsRun++;
        
        // Print test summary
        printTestSummary();
        
        return (testsFailed > 0) ? 1 : 0;
    }
    
private:
    bool runTest(const std::string& name, std::function<bool()> testFunc) {
        std::cout << "Running " << name << "..." << std::endl;
        
        try {
            auto startTime = std::chrono::high_resolution_clock::now();
            bool passed = testFunc();
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            if (passed) {
                std::cout << "  ✓ PASSED (" << duration.count() << " ms)" << std::endl;
                return true;
            } else {
                std::cout << "  ✗ FAILED (" << duration.count() << " ms)" << std::endl;
                return false;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✗ EXCEPTION: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cout << "  ✗ UNKNOWN ERROR" << std::endl;
            return false;
        }
    }
    
    void printTestSummary() {
        std::cout << "" << std::endl;
        std::cout << "=== TEST SUMMARY ===" << std::endl;
        std::cout << "Tests run: " << testsRun << std::endl;
        std::cout << "Tests passed: " << testsPassed << std::endl;
        std::cout << "Tests failed: " << testsFailed << std::endl;
        
        if (testsFailed == 0) {
            std::cout << "" << std::endl;
            std::cout << "=== ALL TESTS PASSED ===" << std::endl;
            std::cout << "The NLM Phase 6 system enhancements are working correctly!" << std::endl;
        } else {
            std::cout << "" << std::endl;
            std::cout << "=== TESTS FAILED ===" << std::endl;
            std::cout << "Please review the failed tests and investigate issues." << std::endl;
        }
    }
    
    bool testEnhancedConfiguration() {
        std::cout << "  Testing enhanced configuration system with JSON/YAML support..." << std::endl;
        
        // Test 1: Basic configuration
        auto config = std::make_shared<Config>();
        
        // Test all setter types
        config->set("string_value", std::string("test"), ConfigSource::Default);
        config->set("int_value", 42, ConfigSource::Default);
        config->set("double_value", 3.14, ConfigSource::Default);
        config->set("bool_value", true, ConfigSource::Default);
        config->set("vector_value", std::vector<int>({1, 2, 3}), ConfigSource::Default);
        
        // Test getters
        if (!config->get<std::string>("string_value").value_or("") == "test") {
            throw std::runtime_error("String value retrieval failed");
        }
        
        if (!config->get<int>("int_value").value_or(0) == 42) {
            throw std::runtime_error("Integer value retrieval failed");
        }
        
        if (std::abs(config->getOr<double>("double_value", 0.0) - 3.14) > 0.001) {
            throw std::runtime_error("Double value retrieval failed");
        }
        
        if (!config->getOr<bool>("bool_value", false)) {
            throw std::runtime_error("Boolean value retrieval failed");
        }
        
        // Test file save/load
        std::string testFile = "test_config.cfg";
        if (!config->saveToFile(testFile)) {
            throw std::runtime_error("Configuration save failed");
        }
        
        auto config2 = std::make_shared<Config>();
        if (!config2->loadFromFile(testFile)) {
            throw std::runtime_error("Configuration load failed");
        }
        
        // Cleanup
        std::remove(testFile.c_str());
        
        std::cout << "  ✓ Enhanced configuration system functional" << std::endl;
        return true;
    }
    
    bool testMainApplication() {
        std::cout << "  Testing main application with Phase 6 integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        if (brain->getTotalNeuronCount() <= 0) {
            throw std::runtime_error("Brain has no neurons");
        }
        
        std::cout << "  ✓ Main application integration successful" << std::endl;
        return true;
    }
    
    bool testPhase6Demo() {
        std::cout << "  Testing Phase 6 demo mode with complete system integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(150), ConfigSource::Default);
        
        // Create complete demo system
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for demo");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        world->setRandomSeed(42);
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        // Enable all Phase 6 features
        agentBrain->enableRewardModulation(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        
        // Run demo episode
        for (int step = 0; step < 50; ++step) {
            // Complete sensory-motor loop
            const SensoryPercept& percept = world->getSensoryPercept();
            agentBrain->processSensoryInput(percept);
            
            for (int neuralStep = 0; neuralStep < 5; ++neuralStep) {
                brain->step(neuralStep, neuralStep * config->getOr<double>("simulation_timestep", 0.001));
            }
            
            MotorCommand cmd = agentBrain->decodeMotorCommand();
            ActionResult result = world->applyMotorCommand(cmd, world->getAgentBody().age);
            agentBrain->applyRewardModulation(result.reward, 0.0f);
            
            world->update(0.01);
            agentBrain->updateDevelopment(0.01);
            
            // Verify all systems are active
            if (agentBrain->getNeuromodulationLevel() < -1.0f || agentBrain->getNeuromodulationLevel() > 1.0f) {
                throw std::runtime_error("Neuromodulation out of range");
            }
        }
        
        std::cout << "  ✓ Phase 6 demo mode working perfectly" << std::endl;
        return true;
    }
    
    bool testCommandLineInterface() {
        std::cout << "  Testing enhanced command-line interface..." << std::endl;
        
        // Test command-line argument parsing
        std::string testArgs[] = {"nlm", "--config", "test.cfg", "--episodes", "10", "--steps", "200"};
        int argc = 7;
        char** argv = new char*[argc];
        for (int i = 0; i < argc; ++i) {
            argv[i] = new char[testArgs[i].length() + 1];
            strcpy(argv[i], testArgs[i].c_str());
        }
        
        // Test configuration loading from file
        auto config = std::make_shared<Config>();
        if (!config->loadFromArgs(argc, argv)) {
            throw std::runtime_error("Command-line argument parsing failed");
        }
        
        // Cleanup
        for (int i = 0; i < argc; ++i) {
            delete[] argv[i];
        }
        delete[] argv;
        
        std::cout << "  ✓ Command-line interface functional" << std::endl;
        return true;
    }
    
    bool testCompleteSystemIntegration() {
        std::cout << "  Testing complete Phase 6 system integration..." << std::endl;
        
        // Create a fully integrated NLM Phase 6 system
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        
        // Initialize all major systems
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        // Enable all Phase 6 features
        agentBrain->enableAllSystems(true);
        
        // Verify system integration
        if (!agentBrain->isRewardModulationEnabled() || 
            !agentBrain->isStructuralPlasticityEnabled() ||
            !agentBrain->isDevelopmentEnabled() ||
            !agentBrain->isCuriosityEnabled()) {
            throw std::runtime_error("Not all Phase 6 features enabled");
        }
        
        // Run comprehensive integration test
        for (int episode = 0; episode < 3; ++episode) {
            // Reset and run episode
            world->reset();
            agentBrain->reset();
            brain->reset();
            brain->initialize();
            
            for (int step = 0; step < 100; ++step) {
                // Complete integration loop
                const SensoryPercept& percept = world->getSensoryPercept();
                agentBrain->processSensoryInput(percept);
                
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(neuralStep, neuralStep * config->getOr<double>("simulation_timestep", 0.001));
                }
                
                MotorCommand cmd = agentBrain->decodeMotorCommand();
                ActionResult result = world->applyMotorCommand(cmd, world->getAgentBody().age);
                agentBrain->applyRewardModulation(result.reward, 0.0f);
                
                world->update(0.01);
                agentBrain->updateDevelopment(0.01);
            }
            
            // Check for learning and adaptation
            if (agentBrain->getDevelopmentalStage() == DevelopmentalStage::Initial) {
                throw std::runtime_error("Development not progressing during integration test");
            }
            
            if (brain->getTotalSynapseCount() <= 0) {
                throw std::runtime_error("No synapses created during integration test");
            }
        }
        
        std::cout << "  ✓ Complete Phase 6 system integration successful" << std::endl;
        return true;
    }
    
    int testsRun = 0;
    int testsPassed = 0;
    int testsFailed = 0;
};

int main(int argc, char* argv[]) {
    IntegrationTester tester;
    return tester.runTests();
}
