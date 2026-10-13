#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

using namespace nlm;

void printUsage(const char* programName) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --verify-only        Quick integration verification" << std::endl;
    std::cout << "  --detailed          Detailed system analysis" << std::endl;
    std::cout << "  --benchmark         Performance profiling" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --verify-only" << std::endl;
    std::cout << "  " << programName << " --detailed" << std::endl;
    std::cout << "  " << programName << " --benchmark" << std::endl;
}

void runQuickVerification(Phase6IntegratedExperiment& experiment) {
    std::cout << "--- QUICK INTEGRATION VERIFICATION ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "Some brain systems are not properly integrated." << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: All critical systems are properly integrated!" << std::endl;
    std::cout << std::endl;
}

void runDetailedAnalysis(Phase6IntegratedExperiment& experiment) {
    std::cout << "--- DETAILED SYSTEM ANALYSIS ---" << std::endl;
    
    // Create minimal brain for analysis
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain for analysis" << std::endl;
        return;
    }
    
    std::cout << "Brain Configuration:" << std::endl;
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
    std::cout << std::endl;
    
    std::cout << "System Integration Status:" << std::endl;
    std::cout << "  Working Memory: " << (brain->getWorkingMemory() ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (brain->getEpisodicMemory() ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Prediction System: " << (brain->getPredictionSystem() ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Neural Planner: " << (brain->getPlanner() ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Concept Formation: " << (brain->getConceptFormation() ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Attention System: " << (brain->getAttention() ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Development System: " << (brain->getDevelopmentSystem() ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation (Dopamine): " << (brain->getDopamine() ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << std::endl;
    
    // Run a few steps to collect metrics
    std::cout << "Running simulation for metrics collection..." << std::endl;
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        // Inject some sensory activity to trigger learning
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        }
    }
    
    std::cout << "Metrics After 100 Steps:" << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    if (brain->getWorkingMemory()) {
        std::cout << "  Working memory traces: " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
    }
    
    if (brain->getEpisodicMemory()) {
        std::cout << "  Episodic memory episodes: " << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
    }
    
    if (brain->getPredictionSystem()) {
        std::cout << "  Prediction error: " << brain->getPredictionSystem()->getPredictionError() << std::endl;
        std::cout << "  Prediction confidence: " << brain->getPredictionSystem()->getConfidence() << std::endl;
        std::cout << "  Prediction error history size: " << brain->getPredictionSystem()->getErrorHistory().size() << std::endl;
    }
    
    std::cout << std::endl;
}

void runBenchmark(Phase6IntegratedExperiment& experiment) {
    std::cout << "--- PERFORMANCE BENCHMARK ---" << std::endl;
    
    // Create experiment with larger configuration
    Phase6Config config;
    config.neuronCount = 1000;
    config.maxSteps = 5000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = experiment.run(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Benchmark Results (" << config.maxSteps << " steps, " << config.neuronCount << " neurons):" << std::endl;
    std::cout << "  Wall clock time: " << duration.count() / 1000.0 << " seconds" << std::endl;
    std::cout << "  Steps per second: " << config.maxSteps / (duration.count() / 1000.0) << std::endl;
    std::cout << "  Total reward per step: " << result.totalReward << std::endl;
    std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "  E/I ratio: " << result.avgSynapticWeight << std::endl;
    std::cout << "  Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << std::endl;
    
    std::cout << "Integration Status Summary:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Prediction: " << (result.predictionIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✅ WORKING" : "❌ NOT WORKING") << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 0;
    }
    
    std::string mode = argv[1];
    
    if (mode == "--help") {
        printUsage(argv[0]);
        return 0;
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    if (mode == "--verify-only") {
        runQuickVerification(experiment);
    } else if (mode == "--detailed") {
        runDetailedAnalysis(experiment);
    } else if (mode == "--benchmark") {
        runBenchmark(experiment);
    } else {
        std::cerr << "ERROR: Unknown option '" << mode << "'" << std::endl;
        std::cerr << "Use '--help' to see available options." << std::endl;
        return 1;
    }
    
    return 0;
}
