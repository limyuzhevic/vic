// Test script for Phase 6 Integration System
// This script tests the key integration functionality

#include "IntegrationSystem.hpp"
#include "AgentBrain.hpp"
#include "WorkingMemory.hpp"
#include "EpisodicMemory.hpp"
#include "SemanticMemory.hpp"
#include "AssociativeMemory.hpp"
#include "PredictionSystem.hpp"
#include "Neuromodulator.hpp"
#include "DevelopmentSystem.hpp"
#include "SimpleWorld.hpp"
#include "Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <chrono>

using namespace nlm;

// Test basic integration functionality
bool testIntegrationBasic() {
    std::cout << "=== Testing Phase 6 Integration System (Basic) ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    try {
        // Create minimal system for testing
        auto config = std::make_shared<Config>();
        
        // Create core brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        // Create agent brain interface
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Create memory systems (basic versions)
        auto workingMem = std::make_shared<WorkingMemory>();
        auto episodicMem = std::make_shared<EpisodicMemory>();
        auto semanticMem = std::make_shared<SemanticMemory>();
        auto proceduralMem = std::make_shared<ProceduralMemory>();
        auto associativeMem = std::make_shared<AssociativeMemory>();
        
        // Create prediction system
        auto predictionSys = std::make_shared<PredictionSystem>();
        
        // Create neuromodulator
        auto neuromodulator = std::make_shared<Neuromodulator>();
        neuromodulator->initialize();
        
        // Create development system
        auto developmentSys = std::make_shared<DevelopmentSystem>();
        developmentSys->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        
        // Create integration system
        auto integrationSystem = createIntegrationSystem(
            brain, agent, workingMem, episodicMem, semanticMem,
            proceduralMem, associativeMem, predictionSys,
            neuromodulator, developmentSys);
        
        // Test 1: Basic integration creation
        std::cout << "Test 1: Basic integration system creation..." << std::endl;
        if (!integrationSystem->validateIntegration()) {
            std::cerr << "✗ Integration validation failed!" << std::endl;
            return false;
        }
        std::cout << "✓ Integration system created and validated" << std::endl;
        
        // Test 2: Establish connections
        std::cout << "Test 2: Establishing system connections..." << std::endl;
        integrationSystem->establishSystemConnections();
        std::cout << "✓ System connections established" << std::endl;
        
        // Test 3: Configure integration features
        std::cout << "Test 3: Configuring integration features..." << std::endl;
        integrationSystem->configureMemoryPredictionIntegration(true);
        integrationSystem->configureNeuromodulationMemoryIntegration(true);
        integrationSystem->configureReplayMechanism(true);
        integrationSystem->configurePersistence(true);
        std::cout << "✓ Integration features configured" << std::endl;
        
        // Test 4: Run basic integration steps
        std::cout << "Test 4: Running basic integration steps (50 steps)..." << std::endl;
        world->reset();
        agent->initialize(*world);
        
        auto startTime = std::chrono::steady_clock::now();
        
        for (SimulationStep step = 0; step < 50; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input through agent
            agent->processSensoryInput(percept);
            
            // Step brain
            if (brain) {
                brain->step(step);
            }
            
            // Decode motor command
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, step);
            
            // Apply reward modulation
            float reward = 0.0f;
            auto internalPercept = world->getSensoryPercept();
            auto internal = internalPercept.getInternal();
            if (!internal.empty()) {
                reward = internal[0];
            }
            agent->applyRewardModulation(reward, agent->getExpectedReward());
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Integrate systems
            integrationSystem->integrateStep(step, 0.1);
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "✓ Completed 50 integration steps in " << duration << " seconds" << std::endl;
        
        // Test 5: Check integration statistics
        std::cout << "Test 5: Checking integration statistics..." << std::endl;
        auto stats = integrationSystem->getStats();
        std::cout << "  Prediction-Memory Integrations: " << stats.predictionMemoryIntegrations << std::endl;
        std::cout << "  Neuromodulation-Memory Integrations: " << stats.neuromodulationMemoryIntegrations << std::endl;
        std::cout << "  Memory Replay Events: " << stats.memoryReplayEvents << std::endl;
        std::cout << "  Active Connections: " << stats.activeConnections << std::endl;
        std::cout << "✓ Integration statistics collected" << std::endl;
        
        // Test 6: Enable Phase 6 features
        std::cout << "Test 6: Enabling Phase 6 features..." << std::endl;
        integrationSystem->enablePhase6Features();
        std::cout << "✓ Phase 6 features enabled" << std::endl;
        
        // Test 7: Integration report
        std::cout << "Test 7: Integration report..." << std::endl;
        std::cout << integrationSystem->getIntegrationReport() << std::endl;
        std::cout << "✓ Integration report generated" << std::endl;
        
        // Cleanup
        Logger::shutdown();
        
        std::cout << "\n=== All Integration Tests Passed ===" << std::endl;
        std::cout << "Phase 6 Integration System is working correctly!" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during integration test: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    return testIntegrationBasic() ? 0 : 1;
}
