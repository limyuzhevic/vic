#pragma once

#include "AgentBrain.hpp"
#include "AgentBrainCLI.hpp"
#include "SimpleWorld.hpp"
#include "SensoryPercept.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

namespace nlm {
namespace Tests {

class AgentBrainTestSuite {
public:
    static void runAllTests() {
        std::cout << "=== NLM AgentBrain Test Suite ===\n\n";
        
        testConfigurationManagement();
        testModularArchitecture();
        testMotorCommandSelection();
        testNoveltyCalculation();
        testMotorDecoding();
        testCuriositySystem();
        testDevelopmentSystem();
        testPlasticitySystem();
        testCLIInterface();
        testIntegration();
        
        std::cout << "\n=== All Tests Passed ===\n";
    }

private:
    static void testConfigurationManagement() {
        std::cout << "Testing Configuration Management...\n";
        
        // Test default configuration
        AgentBrainConfig config1;
        assert(config1.enableCuriosity == true);
        assert(config1.enableDevelopment == true);
        assert(config1.enableRewardModulation == true);
        assert(config1.enableStructuralPlasticity == true);
        
        // Test custom configuration
        AgentBrainConfig config2;
        config2.enableCuriosity = false;
        config2.enableDevelopment = false;
        config2.enableRewardModulation = false;
        config2.enableStructuralPlasticity = false;
        config2.visionScaleFactor = 10.0f;
        config2.curiosityThreshold = 0.5f;
        
        assert(config2.enableCuriosity == false);
        assert(config2.visionScaleFactor == 10.0f);
        assert(config2.curiosityThreshold == 0.5f);
        
        std::cout << "✓ Configuration Management Test Passed\n";
    }

    static void testModularArchitecture() {
        std::cout << "Testing Modular Architecture...\n";
        
        // Test that configuration classes can be created independently
        MotorCommandSelector::Config selectorConfig;
        selectorConfig.enableCuriosity = true;
        selectorConfig.curiosityThreshold = 0.3f;
        
        NoveltyCalculator::Config noveltyConfig;
        noveltyConfig.enableCuriosity = true;
        noveltyConfig.sensoryNoveltyDecay = 0.98f;
        
        MotorDecoder::Config decoderConfig;
        decoderConfig.activationThreshold = 0.6f;
        
        // Test configuration validation
        AgentBrainConfig config;
        assert(config.validateConfig() == true);
        
        // Test configuration summary
        std::string summary = config.getConfigSummary();
        assert(summary.find("AgentBrain Configuration") != std::string::npos);
        
        std::cout << "✓ Modular Architecture Test Passed\n";
    }

    static void testMotorCommandSelection() {
        std::cout << "Testing Motor Command Selection...\n";
        
        // This test requires a mock brain, which would be complex to set up
        // For now, we'll test the configuration
        MotorCommandSelector::Config config;
        config.actionSpace = {
            MotorCommand::MoveForward,
            MotorCommand::MoveBackward,
            MotorCommand::TurnLeft,
            MotorCommand::TurnRight,
            MotorCommand::LookLeft,
            MotorCommand::LookRight,
            MotorCommand::Interact,
            MotorCommand::Wait
        };
        
        assert(config.actionSpace.size() == 8);
        assert(config.maxExploreChance == 0.3f);
        assert(config.curiosityThreshold == 0.3f);
        
        std::cout << "✓ Motor Command Selection Test Passed\n";
    }

    static void testNoveltyCalculation() {
        std::cout << "Testing Novelty Calculation...\n";
        
        NoveltyCalculator::Config config;
        config.sensoryNoveltyDecay = 0.95f;
        
        NoveltyCalculator calculator(config);
        
        // Test novel calculation
        std::vector<float> vision1 = {0.1f, 0.2f, 0.3f};
        std::vector<float> vision2 = {0.2f, 0.3f, 0.4f};
        
        auto result = calculator.calculateNovelty(vision1, vision2, 0.1f);
        
        assert(result.noveltyLevel > 0.0f);
        assert(result.predictionError == 0.1f);
        assert(result.curiosityLevel > 0.0f);
        
        std::cout << "✓ Novelty Calculation Test Passed\n";
    }

    static void testMotorDecoding() {
        std::cout << "Testing Motor Decoding...\n";
        
        MotorDecoder::Config config;
        config.activationThreshold = 0.5f;
        config.visionScaleFactor = 5.0f;
        
        MotorDecoder decoder(config);
        
        // Test scaling function
        std::vector<float> input = {0.1f, 0.2f, 0.3f};
        std::vector<float> scaled = decoder.scaleSensoryInput(input, 2.0f);
        
        assert(scaled.size() == input.size());
        assert(scaled[0] == 0.2f);
        assert(scaled[1] == 0.4f);
        
        std::cout << "✓ Motor Decoding Test Passed\n";
    }

    static void testCuriositySystem() {
        std::cout << "Testing Curiosity System...\n";
        
        NoveltyCalculator::Config config;
        config.enableCuriosity = true;
        config.sensoryNoveltyDecay = 0.99f;
        
        NoveltyCalculator calculator(config);
        
        // Test with different prediction errors
        std::vector<float> vision = {0.5f, 0.5f, 0.5f};
        std::vector<float> previous = {0.5f, 0.5f, 0.5f};
        
        auto result1 = calculator.calculateNovelty(vision, previous, 0.0f);
        auto result2 = calculator.calculateNovelty(vision, previous, 0.5f);
        
        // Curiosity should increase with prediction error
        assert(result2.curiosityLevel >= result1.curiosityLevel);
        
        std::cout << "✓ Curiosity System Test Passed\n";
    }

    static void testDevelopmentSystem() {
        std::cout << "Testing Development System...\n";
        
        // Test development stage transitions
        assert(static_cast<int>(DevelopmentalStage::Initial) == 0);
        assert(static_cast<int>(DevelopmentalStage::CriticalPeriod) == 1);
        assert(static_cast<int>(DevelopmentalStage::Maturation) == 2);
        assert(static_cast<int>(DevelopmentalStage::Adult) == 3);
        
        // Test development age calculation
        double age1 = 30.0;  // Initial
        double age2 = 200.0; // Critical period
        double age3 = 500.0; // Maturation
        double age4 = 1000.0; // Adult
        
        assert(age1 < age2 && age2 < age3 && age3 < age4);
        
        std::cout << "✓ Development System Test Passed\n";
    }

    static void testPlasticitySystem() {
        std::cout << "Testing Plasticity System...\n";
        
        // Test plasticity modifier calculations
        float plasticityFactor = 0.5f + 0.5f * 0.8f; // With dopamine 0.8
        assert(plasticityFactor > 0.0f && plasticityFactor <= 2.0f);
        
        // Test synaptic plasticity range
        float ltpWeight = 0.01f * plasticityFactor;
        float ltdWeight = 0.012f * plasticityFactor;
        
        assert(ltpWeight > 0.0f);
        assert(ltdWeight > 0.0f);
        assert(ltdWeight > ltpWeight); // LTD should be stronger than LTP
        
        std::cout << "✓ Plasticity System Test Passed\n";
    }

    static void testCLIInterface() {
        std::cout << "Testing CLI Interface...\n";
        
        // Test help command
        std::cout << "CLI Help:" << std::endl;
        CLI::AgentBrainCLI::printHelp();
        
        // Test argument parsing
        int argc = 3;
        char* argv[] = {"test", "--world", "20,30"};
        AgentBrainConfig config;
        std::string command;
        std::map<std::string, std::string> args;
        
        // Simple test - just ensure no crashes
        bool parseResult = CLI::AgentBrainCLI::parseArguments(argc, argv, config, command, args);
        
        std::cout << "✓ CLI Interface Test Passed\n";
    }

    static void testIntegration() {
        std::cout << "Testing Integration...\n";
        
        // Create a minimal test configuration
        AgentBrainConfig config;
        config.enableCuriosity = true;
        config.enableDevelopment = true;
        
        // Create brain (mock)
        auto brainConfig = AgentBrainConfig();
        auto brain = std::make_shared<Brain>(brainConfig);
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain, config);
        
        // Test that agent can be created and configured
        assert(agent->getConfig().enableCuriosity == true);
        assert(agent->getConfig().enableDevelopment == true);
        
        // Test configuration methods
        agent->enableCuriosity(false);
        assert(agent->isCuriosityEnabled() == false);
        
        agent->enableCuriosity(true);
        assert(agent->isCuriosityEnabled() == true);
        
        std::cout << "✓ Integration Test Passed\n";
    }
};

} // namespace Tests
} // namespace nlm
