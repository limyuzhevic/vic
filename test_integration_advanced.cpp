// Integration Test for Neural Prediction and Memory Systems
// This test verifies that the integration system properly connects
// NeuralPrediction with WorkingMemory and EpisodicMemory

#include "IntegrationSystem.hpp"
#include "AgentBrain.hpp"
#include "WorkingMemory.hpp"
#include "EpisodicMemory.hpp"
#include "SemanticMemory.hpp"
#include "ProceduralMemory.hpp>
#include "AssociativeMemory.hpp>
#include "PredictionSystem.hpp"
#include "Neuromodulator.hpp"
#include "DevelopmentSystem.hpp"
#include "SimpleWorld.hpp>
#include "Logger.hpp"
#include "core/Config/Config.hpp>
#include "brain/Brain.hpp>
#include <iostream>
#include <memory>
#include <chrono>

using namespace nlm;

// Test Neural Prediction integration with memory systems
bool testNeuralPredictionMemoryIntegration() {
    std::cout << "=== Testing Neural Prediction - Memory Integration ===" << std::endl;
    
    try {
        // Initialize minimal logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Create core systems
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Create memory systems
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
        world->configure(15, 15, 8, 8);
        
        // Create integration system
        auto integration = createIntegrationSystem(
            brain, agent, workingMem, episodicMem, semanticMem,
            proceduralMem, associativeMem, predictionSys,
            neuromodulator, developmentSys);
        
        std::cout << "✓ All systems created" << std::endl;
        
        // Test 1: Basic integration setup
        std::cout << "Test 1: Basic integration setup..." << std::endl;
        integration->configureMemoryPredictionIntegration(true);
        integration->configureNeuromodulationMemoryIntegration(true);
        integration->enablePhase6Features();
        std::cout << "✓ Integration system configured" << std::endl;
        
        // Test 2: Establish system connections
        std::cout << "Test 2: Establishing system connections..." << std::endl;
        integration->establishSystemConnections();
        std::cout << "✓ System connections established" << std::endl;
        
        // Test 3: Run integration simulation
        std::cout << "Test 3: Running integration simulation..." << std::endl;
        world->reset();
        agent->initialize(*world);
        
        auto startTime = std::chrono::steady_clock::now();
        
        for (SimulationStep step = 0; step < 100; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process through agent
            agent->processSensoryInput(percept);
            
            // Step brain
            brain->step(step);
            
            // Decode action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, step);
            
            // Get reward and apply neuromodulation
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
            integration->integrateStep(step, 0.1);
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "✓ Integration simulation completed in " << duration << " seconds" << std::endl;
        
        // Test 4: Check integration statistics
        std::cout << "Test 4: Integration statistics..." << std::endl;
        auto stats = integration->getStats();
        std::cout << "  Memory-Prediction Integrations: " << stats.predictionMemoryIntegrations << std::endl;
        std::cout << "  Neuromodulation-Memory Integrations: " << stats.neuromodulationMemoryIntegrations << std::endl;
        std::cout << "  Memory Replay Events: " << stats.memoryReplayEvents << std::endl;
        std::cout << "  Active Connections: " << stats.activeConnections << std::endl;
        std::cout << "✓ Integration statistics collected" << std::endl;
        
        // Test 5: Validation
        std::cout << "Test 5: Integration validation..." << std::endl;
        if (integration->validateIntegration()) {
            std::cout << "✓ Integration validation passed" << std::endl;
        } else {
            std::cout << "✗ Integration validation failed" << std::endl;
            return false;
        }
        
        // Test 6: Integration report
        std::cout << "Test 6: Integration report..." << std::endl;
        std::cout << integration->getIntegrationReport() << std::endl;
        std::cout << "✓ Integration report generated" << std::endl;
        
        // Cleanup
        Logger::shutdown();
        
        std::cout << "\n=== Neural Prediction - Memory Integration Test PASSED ===" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during integration test: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    return testNeuralPredictionMemoryIntegration() ? 0 : 1;
}
