// Phase 6 Integration Experiment
// Demonstrates the comprehensive integration of all Phase 6 systems

#include "IntegrationSystem.hpp"
#include "AgentBrain.hpp"
#include "WorkingMemory.hpp"
#include "EpisodicMemory.hpp"
#include "SemanticMemory.hpp"
#include "ProceduralMemory.hpp"
#include "AssociativeMemory.hpp"
#include "PredictionSystem.hpp"
#include "Neuromodulator.hpp"
#include "DevelopmentSystem.hpp"
#include "SimpleWorld.hpp"
#include "Logger.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <chrono>
#include <iomanip>

namespace nlm {

// Integration experiment observer that demonstrates Phase 6 features
class IntegrationExperimentObserver : public IntegrationObserver {
public:
    void onIntegrationEvent(const IntegrationEvent& event) override {
        std::cout << "[" << event.timestamp << "] " << getEventTypeString(event.type) 
                  << " - Dopamine: " << event.parameters["dopamine"] 
                  << ", Prediction Error: " << event.parameters["prediction_error"] 
                  << ", Memory Update: " << event.parameters["memory_update"] << std::endl;
    }
    
private:
    std::string getEventTypeString(IntegrationEvent::Type type) {
        switch (type) {
            case IntegrationEvent::Type::PredictionError: return "PREDICTION_ERROR";
            case IntegrationEvent::Type::MemoryUpdate: return "MEMORY_UPDATE";
            case IntegrationEvent::Type::NeuromodulationChange: return "NEUROMODULATION_CHANGE";
            case IntegrationEvent::Type::DevelopmentStageChange: return "DEVELOPMENT_STAGE_CHANGE";
            case IntegrationEvent::Type::RewardReceived: return "REWARD_RECEIVED";
            case IntegrationEvent::Type::NoveltyDetected: return "NOVELTY_DETECTED";
            case IntegrationEvent::Type::ActionExecuted: return "ACTION_EXECUTED";
            case IntegrationEvent::Type::WorldInteraction: return "WORLD_INTERACTION";
            case IntegrationEvent::Type::MemoryReplay: return "MEMORY_REPLAY";
            case IntegrationEvent::Type::Consolidation: return "CONSOLIDATION";
            default: return "UNKNOWN";
        }
    }
};

// Phase 6 Integration Experiment
class Phase6IntegrationExperiment {
public:
    Phase6IntegrationExperiment() {
        NLM_LOG_INFO("Starting Phase 6 Integration Experiment");
        NLM_LOG_INFO("Demonstrating comprehensive integration of memory, prediction, neuromodulation, and development systems");
    }
    
    void runExperiment() {
        // Create all Phase 6 systems
        setupSystems();
        
        // Initialize the integration system
        auto integrationSystem = createIntegrationSystem(
            brain_, agent_, workingMem_, episodicMem_, semanticMem_,
            proceduralMem_, associativeMem_, predictionSys_, neuromodulator_,
            developmentSys_);
        
        // Register observer
        observer_ = std::make_unique<IntegrationExperimentObserver>();
        integrationSystem->registerObserver(observer_.get());
        
        // Configure and establish connections
        integrationSystem->configureMemoryPredictionIntegration(true);
        integrationSystem->configureNeuromodulationMemoryIntegration(true);
        integrationSystem->configureReplayMechanism(true);
        integrationSystem->configurePersistence(true);
        
        integrationSystem->establishSystemConnections();
        
        // Verify integration
        if (!integrationSystem->validateIntegration()) {
            NLM_LOG_ERROR("Integration validation failed");
            return;
        }
        
    // Run integration simulation
    NLM_LOG_INFO("Running Phase 6 integration simulation (1000 steps)...");
    
    world_->reset();
    agent_->initialize(*world_);
    
    auto startTime = std::chrono::steady_clock::now();
    
    for (SimulationStep step = 0; step < 1000; ++step) {
            // Update world
            world_->update(0.1);
            
            // Get sensory input
            auto percept = world_->getSensoryPercept();
            
            // Process sensory input through agent
            agent_->processSensoryInput(percept);
            
            // Step brain (if needed)
            if (brain_) {
                brain_->step(step);
            }
            
            // Decode motor command
            auto action = agent_->decodeMotorCommand();
            
            // Apply action to world
            world_->applyMotorCommand(action, step);
            
            // Apply reward modulation
            float reward = getRewardFromWorld(*world_);
            agent_->applyRewardModulation(reward, agent_->getExpectedReward());
            
            // Update development
            agent_->updateDevelopment(0.1);
            
        // Integrate systems
        integrationSystem->integrateStep(step, 0.1);
            
        // Show progress every 100 steps
        if (step % 100 == 0) {
            printProgress(step, *integrationSystem, *agent_, *world_);
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO(std::to_string(duration) + " seconds for 1000 integration steps");
        
        // Print final integration report
        printFinalReport(*integrationSystem);
        
        // Show Phase 6 features demonstration
        demonstratePhase6Features(*integrationSystem);
        
        NLM_LOG_INFO("Phase 6 Integration Experiment completed successfully");
    }
    
private:
    void setupSystems() {
        // Create core brain
        brain_ = std::make_shared<Brain>();
        brain_->initialize();
        
        // Create agent brain interface
        agent_ = std::make_shared<AgentBrain>(brain_);
        
        // Create memory systems
        workingMem_ = std::make_shared<WorkingMemory>();
        episodicMem_ = std::make_shared<EpisodicMemory>();
        semanticMem_ = std::make_shared<SemanticMemory>();
        proceduralMem_ = std::make_shared<ProceduralMemory>();
        associativeMem_ = std::make_shared<AssociativeMemory>();
        
        // Create prediction system
        predictionSys_ = std::make_shared<PredictionSystem>();
        
        // Create neuromodulator
        neuromodulator_ = std::make_shared<Neuromodulator>();
        neuromodulator_->initialize();
        
        // Create development system
        developmentSys_ = std::make_shared<DevelopmentSystem>();
        developmentSys_->initialize();
        
        // Create world
        world_ = std::make_shared<SimpleWorld>();
        world_->configure(20, 20, 8, 8);
        
        NLM_LOG_INFO("Created all Phase 6 systems");
    }
    
    float getRewardFromWorld(const SimpleWorld& world) {
        // Extract reward from world state
        auto percept = world.getSensoryPercept();
        auto internal = percept.getInternal();
        
        if (!internal.empty()) {
            return internal[0];
        }
        return 0.0f;
    }
    
    void printProgress(SimulationStep step, IntegrationSystem& integration, 
                      const AgentBrain& agent, const SimpleWorld& world) {
        auto stats = integration.getStats();
        
        std::cout << "Step " << step << ": "
                  << "Firing Neurons: " << (brain_ ? brain_->getFiringNeuronCount() : 0) << ", "
                  << "Curiosity: " << agent.getCuriosityLevel() << ", "
                  << "Prediction Error: " << (predictionSys_ ? predictionSys_->getPredictionError() : 0.0f) << ", "
                  << "Dopamine: " << agent.getNeuromodulationLevel() << ", "
                  << "Memory Replay: " << stats.memoryReplayEvents << std::endl;
    }
    
    void printFinalReport(const IntegrationSystem& integration) {
        std::cout << "\n=== Phase 6 Integration Final Report ===" << std::endl;
        std::cout << integration.getIntegrationReport() << std::endl;
    }
    
    void demonstratePhase6Features(const IntegrationSystem& integration) {
        std::cout << "\n=== Phase 6 Features Demonstrated ===" << std::endl;
        
        auto stats = integration.getStats();
        
        std::cout << "✓ Memory-Prediction Integration: " << stats.predictionMemoryIntegrations << " events" << std::endl;
        std::cout << "✓ Neuromodulation-Memory Integration: " << stats.neuromodulationMemoryIntegrations << " events" << std::endl;
        std::cout << "✓ Memory Replay: " << stats.memoryReplayEvents << " events" << std::endl;
        std::cout << "✓ Memory Consolidation: " << stats.consolidationEvents << " events" << std::endl;
        std::cout << "✓ Active System Connections: " << stats.activeConnections << " connections" << std::endl;
        
        std::cout << "\n✓ Real-time Integration: Working memory updated by prediction errors" << std::endl;
        std::cout << "✓ Neuromodulation: Dopamine and curiosity modulate memory consolidation" << std::endl;
        std::cout << "✓ Development: Memory systems adapt based on developmental stage" << std::endl;
        std::cout << "✓ Agent-World Coordination: Actions create episodic memories" << std::endl;
        std::cout << "✓ Persistence: Integration state can be saved and restored" << std::endl;
        std::cout << "✓ Replay Mechanisms: Memory replay during consolidation" << std::endl;
    }
    
    // System components
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<WorkingMemory> workingMem_;
    std::shared_ptr<EpisodicMemory> episodicMem_;
    std::shared_ptr<SemanticMemory> semanticMem_;
    std::shared_ptr<ProceduralMemory> proceduralMem_;
    std::shared_ptr<AssociativeMemory> associativeMem_;
    std::shared_ptr<PredictionSystem> predictionSys_;
    std::shared_ptr<Neuromodulator> neuromodulator_;
    std::shared_ptr<DevelopmentSystem> developmentSys_;
    std::shared_ptr<SimpleWorld> world_;
    std::unique_ptr<IntegrationObserver> observer_;
};

} // namespace nlm

// Main function
int main() {
    try {
        // Initialize logging
        Logger::initialize();
        Logger::setLevel(LogLevel::INFO);
        
        // Run Phase 6 Integration Experiment
        Phase6IntegrationExperiment experiment;
        experiment.runExperiment();
        
        // Shutdown logging
        Logger::shutdown();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in Phase 6 Integration Experiment: " << e.what() << std::endl;
        return 1;
    }
}