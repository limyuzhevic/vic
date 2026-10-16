// NLM (熙然) - Neural Learning Machine
// Main entry point with improved structure and CLI framework

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>

using namespace nlm;

class CommandLineOptions {
public:
    enum class Mode {
        SIMULATION,
        CONNECTIVITY_TEST,
        PLASTICITY_TEST,
        STDP_TEST,
        BENCHMARK,
        CONFIG_EXPORT,
        HELP
    };
    
    struct Options {
        Mode mode = Mode::SIMULATION;
        std::string configFile = "configs/default.cfg";
        std::string outputFile = "brain_checkpoint.bin";
        std::string testMode = "all";
        size_t neuronCount = 1000;
        size_t duration = 1000;
        size_t stepsPerSecond = 1000;
        double timestep = 0.001;
        bool verbose = false;
        bool headless = false;
        bool saveOnExit = false;
        std::map<std::string, std::string> customArgs;
    };
    
    static Options parse(int argc, char** argv);
    static void printHelp(Mode context = Mode::HELP);
    static bool validateOptions(const Options& opts);
};

class NLMApplication {
public:
    NLMApplication(const CommandLineOptions::Options& options);
    ~NLMApplication() = default;
    
    bool initialize();
    void run();
    void cleanup();
    
    // Mode handlers
    void runSimulationMode();
    void runConnectivityTest();
    void runPlasticityTest();
    void runSTDPTest();
    void runBenchmark();
    void exportConfiguration();
    
private:
    CommandLineOptions::Options options_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<SimulationClock> clock_;
    
    void setupLogging();
    void setupConfiguration();
    void setupBrain();
    void runStandardTests();
    void printPerformanceStats();
    
    // Statistics
    struct PerfStats {
        size_t totalSteps = 0;
        size_t totalSpikes = 0;
        double startTime = 0.0;
        double endTime = 0.0;
    } stats_;
};

// Learning Experiment structure (extracted from main.cpp)
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
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
        
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

// Test functions (extracted from main.cpp)
void printBanner();
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

int main(int argc, char** argv) {
    auto options = CommandLineOptions::parse(argc, argv);
    
    if (!CommandLineOptions::validateOptions(options)) {
        return 1;
    }
    
    try {
        NLMApplication app(options);
        if (!app.initialize()) {
            NLM_LOG_ERROR("Failed to initialize application");
            return 1;
        }
        app.run();
        app.cleanup();
        
        NLM_LOG_INFO("Application completed successfully");
        return 0;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unhandled exception: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception occurred");
        return 1;
    }
}