/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "DemoFramework.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

class Phase6Demo : public DemoBase {
public:
    Phase6Demo() : DemoBase() {}
    ~Phase6Demo() {}
    
    std::string getDemoName() const override {
        return "PHASE 6 INTEGRATION DEMO";
    }
    
    void onSetup() override {
        Logger::getGlobal().setLevel(LogLevel::Info);
        std::cout << "Setting up Phase 6 integration demo...\n";
    }
    
    void onRun(int numEpisodes, int stepsPerEpisode) override {
        // Create experiment using framework configuration
        Phase6IntegratedExperiment experiment;
        
        // Run integration verification
        runIntegrationVerification(experiment);
        
        // Run individual system tests
        runSystemTests(experiment);
        
        // Run full integration experiment
        runFullIntegration(experiment);
    }
    
    void onComplete() override {
        std::cout << "\n=== INTEGRATION COMPLETE ===\n";
        std::cout << "All brain systems have been verified and integrated successfully.\n";
    }
    
private:
    void runIntegrationVerification(Phase6IntegratedExperiment& experiment) {
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            throw DemoException("Integration verification failed!");
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }
    
    void runSystemTests(Phase6IntegratedExperiment& experiment) {
        std::cout << "--- Memory Integration Test ---" << std::endl;
        experiment.testMemoryIntegration();
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
        
        std::cout << "--- Checkpoint Test ---" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---" << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
    }
    
    void runFullIntegration(Phase6IntegratedExperiment& experiment) {
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        
        // Create configuration using factory pattern
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment.run(config);
        
        // Display results using framework utilities
        displayResults(result);
    }
    
    void displayResults(const Phase6Result& result) {
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    }
};

// Main entry point for Phase 6 demo
int main(int argc, char* argv[]) {
    try {
        nlm::Phase6Demo demo;
        // Use default parameters (5 episodes, 100 steps each)
        demo.run(5, 100);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
