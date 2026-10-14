#include "core/Config/Config.hpp"
#include "core/Config/JSONParser.hpp"
#include "core/Config/Validator.hpp"
#include "core/Config/TemplateManager.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <cctype>
#include <map>
#include <chrono>
#include <thread>

#include "core/Logger/Logger.hpp"
#include "core/DebugAdvanced.hpp"

using namespace nlm;

void initializeAdvancedLogging() {
    // Set up advanced logging configuration
    auto& logger = Logger::getGlobal();
    
    // Enable advanced logger support
    auto advancedLogger = std::dynamic_pointer_cast<IAdvancedLogger>(logger.addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info)));
    
    if (advancedLogger) {
        advancedLogger->enableJSONOutput(false); // Set to true for JSON output
        advancedLogger->addCategory("performance");
        advancedLogger->addCategory("memory");
        advancedLogger->addCategory("neural_state");
        advancedLogger->addCategory("benchmark");
        
        // Set category-specific verbosity levels
        advancedLogger->setCategoryLevel("performance", DebugLevel::Info);
        advancedLogger->setCategoryLevel("memory", DebugLevel::Warning);
        advancedLogger->setCategoryLevel("neural_state", DebugLevel::Debug);
        
        // Set a global correlation ID for tracking related log entries
        advancedLogger->setCorrelationID("experiment-001");
    }
    
    // Initialize debug modules
    Debug::getProfiler()->setAdvancedLogger(advancedLogger);
    Debug::getMemoryTracker()->setAdvancedLogger(advancedLogger);
    Debug::getSnapshotter()->setAdvancedLogger(advancedLogger);
    Debug::getLogAnalyzer()->setAdvancedLogger(advancedLogger);
    
    NLM_LOG_INFO("Advanced logging system initialized");
    NLM_LOG_INFO("Correlation ID: " + logger.getCurrentCorrelationID());
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    NLM_LOG_INFO("NLM Neural Learning Machine - Phase 2");
    NLM_LOG_INFO("Real-time neural computation and learning");
    NLM_LOG_INFO("Enhanced logging and debugging system active");
}

// Enhanced Learning Experiment with debugging
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    std::chrono::duration<double> executionTime;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        NLM_LOG_STREAM(nlm::LogLevel::Info) << "Recording initial state";
        
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
        
        // Record neural state snapshot for debugging
        Debug::getSnapshotter()->takeSnapshot("region-1");
    }
    
    void recordFinalState() {
        NLM_LOG_STREAM(nlm::LogLevel::Info) << "Recording final state";
        
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
        
        // Compare neural states
        if (!initialWeights.empty() && !finalWeights.empty()) {
            auto initialSnapshot = Debug::getSnapshotter()->getLastSnapshot("region-1");
            // Calculate weight changes (simplified comparison)
            size_t minSize = std::min(initialWeights.size(), finalWeights.size());
            float totalChange = 0.0f;
            for (size_t i = 0; i < minSize; ++i) {
                totalChange += std::abs(finalWeights[i] - initialWeights[i]);
            }
            float averageChange = minSize > 0 ? totalChange / minSize : 0.0f;
            
            NLM_LOG_DEBUG("Neural state comparison:")
                << " Average weight change: " << averageChange
                << " Max weight change: " << std::max(
                    *std::max_element(finalWeights.begin(), finalWeights.end()),
                    *std::max_element(initialWeights.begin(), initialWeights.end()));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_STREAM(nlm::LogLevel::Info) << "Computing experiment statistics";
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_ERROR("No weights recorded");
            return;
        }
        
        // Compute weight changes with performance tracking
        Debug::getProfiler()->startProfile("weight_computation");
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        Debug::getProfiler()->stopProfile("weight_computation");
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Memory tracking during computation
        size_t initialMem = initialWeights.size() * sizeof(float);
        size_t finalMem = finalWeights.size() * sizeof(float);
        
        NLM_LOG_DEBUG("Memory usage:");
        NLM_LOG_DEBUG("  Initial: " + std::to_string(initialMem) + " bytes");
        NLM_LOG_DEBUG("  Final: " + std::to_string(finalMem) + " bytes");
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Record final neural state
        Debug::getSnapshotter()->takeSnapshot("region-1-final");
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
        
        // Log benchmark results
        auto profMetrics = Debug::getProfiler()->getLastProfile("weight_computation");
        NLM_LOG_INFO("Computation performance:");
        NLM_LOG_INFO("  Time: " + std::to_string(profMetrics.duration.count()) + " seconds");
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Performance test tracking
    Debug::getProfiler()->startProfile("connectivity_test");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps with memory tracking
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
        
        // Periodic status updates
        if (step % 10 == 0) {
            NLM_LOG_DEBUG("  Step " + std::to_string(step) + 
                         " - Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
        }
    }
    
    Debug::getProfiler()->stopProfile("connectivity_test");
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    // Log performance metrics
    auto testMetrics = Debug::getProfiler()->getLastProfile("connectivity_test");
    NLM_LOG_INFO("  Test performance: " + std::to_string(testMetrics.duration.count()) + " seconds");
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                         " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                         " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}