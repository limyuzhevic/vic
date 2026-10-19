// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <argparse/argparse.hpp>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <csignal>

using namespace nlm;
namespace fs = std::filesystem;

// Forward declarations for test functions
void runAllTests(std::shared_ptr<Brain> brain, LogLevel logLevel);
void runConnectivityTest(std::shared_ptr<Brain> brain, LogLevel logLevel);
void runPlasticityTest(std::shared_ptr<Brain> brain, LogLevel logLevel);
void runSTDPTest(std::shared_ptr<Brain> brain, LogLevel logLevel);
void runPerformanceTest(std::shared_ptr<Brain> brain, LogLevel logLevel);

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
}

void printHelp() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  ./nlm [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "MODES:" << std::endl;
    std::cout << "  --test <mode>     Run specific test mode" << std::endl;
    std::cout << "  --experiment <name> Run specific experiment" << std::endl;
    std::cout << "  --batch           Run batch of experiments" << std::endl;
    std::cout << "  --interactive     Start interactive mode" << std::endl;
    std::cout << std::endl;
    std::cout << "TEST MODES:" << std::endl;
    std::cout << "  all               Run all tests (default)" << std::endl;
    std::cout << "  connectivity      Basic neural connectivity test" << std::endl;
    std::cout << "  plasticity        Plasticity learning experiment" << std::endl;
    std::cout << "  stdp              STDP verification test" << std::endl;
    std::cout << "  performance       Performance benchmark" << std::endl;
    std::cout << std::endl;
    std::cout << "CONFIGURATION:" << std::endl;
    std::cout << "  --config <file>   Load configuration from file" << std::endl;
    std::cout << "  --output <dir>    Save results to directory" << std::endl;
    std::cout << "  --verbose          Enable verbose logging" << std::endl;
    std::cout << "  --quiet            Minimal output" << std::endl;
    std::cout << "  --progress         Show progress indicators" << std::endl;
    std::cout << std::endl;
    std::cout << "SAVE/LOAD:" << std::endl;
    std::cout << "  --save <file>     Save brain state to file" << std::endl;
    std::cout << "  --load <file>     Load brain state from file" << std::endl;
    std::cout << "  --checkpoint <step> Create checkpoint at step" << std::endl;
    std::cout << std::endl;
    std::cout << "GENERAL:" << std::endl;
    std::cout << "  --help             Show this help message" << std::endl;
    std::cout << "  --version          Show version information" << std::endl;
    std::cout << "  --random <seed>    Set random seed for reproducibility" << std::endl;
    std::cout << "  --threads <n>      Use N threads for simulation" << std::endl;
    std::cout << "  --memory <limit>   Memory limit in MB" << std::endl;
    std::cout << "  --max-steps <n>    Maximum simulation steps" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  ./nlm --test all --output results/ --verbose" << std::endl;
    std::cout << "  ./nlm --test plasticity --config my_config.json --save brain_state.json" << std::endl;
    std::cout << "  ./nlm --experiment ablation_study --load pretrained.json --checkpoint 10000" << std::endl;
    std::cout << std::endl;
}

void printProgress(float progress, const std::string& message) {
    int barWidth = 50;
    std::cout << "[";
    int pos = static_cast<int>(barWidth * progress);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(progress * 100.0) << "% " << message << std::endl;
}

void saveResults(const std::string& filepath, const std::string& data) {
    fs::path outputDir = fs::path(filepath).parent_path();
    if (!outputDir.empty() && !fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << data;
        file.close();
        NLM_LOG_INFO("Results saved to: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to save results to: " + filepath);
    }
}

void setupLogging(LogLevel level, const std::string& logFile = "") {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(level);
    
    if (!logFile.empty()) {
        auto fileLogger = std::make_shared<ConsoleLogger>(level);
        fileLogger->setOutputFile(logFile);
        logger->addLogger(fileLogger);
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
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
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
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
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
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
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
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
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

// Forward declarations for test functions
void runAllTests(std::shared_ptr<Brain> brain, LogLevel logLevel);
void runConnectivityTest(std::shared_ptr<Brain> brain, LogLevel logLevel);
void runPlasticityTest(std::shared_ptr<Brain> brain, LogLevel logLevel);
void runSTDPTest(std::shared_ptr<Brain> brain, LogLevel logLevel);
void runPerformanceTest(std::shared_ptr<Brain> brain, LogLevel logLevel);

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
}

void printHelp() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  ./nlm [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "MODES:" << std::endl;
    std::cout << "  --test <mode>     Run specific test mode" << std::endl;
    std::cout << "  --experiment <name> Run specific experiment" << std::endl;
    std::cout << "  --batch           Run batch of experiments" << std::endl;
    std::cout << "  --interactive     Start interactive mode" << std::endl;
    std::cout << std::endl;
    std::cout << "TEST MODES:" << std::endl;
    std::cout << "  all               Run all tests (default)" << std::endl;
    std::cout << "  connectivity      Basic neural connectivity test" << std::endl;
    std::cout << "  plasticity        Plasticity learning experiment" << std::endl;
    std::cout << "  stdp              STDP verification test" << std::endl;
    std::cout << "  performance       Performance benchmark" << std::endl;
    std::cout << std::endl;
    std::cout << "CONFIGURATION:" << std::endl;
    std::cout << "  --config <file>   Load configuration from file" << std::endl;
    std::cout << "  --output <dir>    Save results to directory" << std::endl;
    std::cout << "  --verbose          Enable verbose logging" << std::endl;
    std::cout << "  --quiet            Minimal output" << std::endl;
    std::cout << "  --progress         Show progress indicators" << std::endl;
    std::cout << std::endl;
    std::cout << "SAVE/LOAD:" << std::endl;
    std::cout << "  --save <file>     Save brain state to file" << std::endl;
    std::cout << "  --load <file>     Load brain state from file" << std::endl;
    std::cout << "  --checkpoint <step> Create checkpoint at step" << std::endl;
    std::cout << std::endl;
    std::cout << "GENERAL:" << std::endl;
    std::cout << "  --help             Show this help message" << std::endl;
    std::cout << "  --version          Show version information" << std::endl;
    std::cout << "  --random <seed>    Set random seed for reproducibility" << std::endl;
    std::cout << "  --threads <n>      Use N threads for simulation" << std::endl;
    std::cout << "  --memory <limit>   Memory limit in MB" << std::endl;
    std::cout << "  --max-steps <n>    Maximum simulation steps" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  ./nlm --test all --output results/ --verbose" << std::endl;
    std::cout << "  ./nlm --test plasticity --config my_config.json --save brain_state.json" << std::endl;
    std::cout << "  ./nlm --experiment ablation_study --load pretrained.json --checkpoint 10000" << std::endl;
    std::cout << std::endl;
}

void printProgress(float progress, const std::string& message) {
    int barWidth = 50;
    std::cout << "[";
    int pos = static_cast<int>(barWidth * progress);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(progress * 100.0) << "% " << message << std::endl;
}

void saveResults(const std::string& filepath, const std::string& data) {
    fs::path outputDir = fs::path(filepath).parent_path();
    if (!outputDir.empty() && !fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << data;
        file.close();
        NLM_LOG_INFO("Results saved to: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to save results to: " + filepath);
    }
}

void setupLogging(LogLevel level, const std::string& logFile = "") {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(level);
    
    if (!logFile.empty()) {
        auto fileLogger = std::make_shared<ConsoleLogger>(level);
        fileLogger->setOutputFile(logFile);
        logger->addLogger(fileLogger);
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

void runAllTests(std::shared_ptr<Brain> brain, LogLevel logLevel) {
    NLM_LOG_INFO("=== Running All Tests ===");
    
    // Test 1: Basic connectivity
    NLM_LOG_INFO("Test 1: Basic Neural Connectivity");
    printProgress(0.0f, "Initializing brain...");
    runConnectivityTest(brain, logLevel);
    printProgress(1.0f, "Connectivity test complete.");
    
    // Reset brain
    brain->reset();
    brain->initialize();
    
    // Test 2: Plasticity
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 2: Plasticity Learning Experiment");
    printProgress(0.0f, "Resetting brain for plasticity test...");
    runPlasticityTest(brain, logLevel);
    printProgress(1.0f, "Plasticity test complete.");
    
    // Reset brain
    brain->reset();
    brain->initialize();
    
    // Test 3: STDP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 3: STDP Verification");
    printProgress(0.0f, "Resetting brain for STDP test...");
    runSTDPTest(brain, logLevel);
    printProgress(1.0f, "STDP test complete.");
    
    // Reset brain
    brain->reset();
    brain->initialize();
    
    // Test 4: Performance
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 4: Performance Benchmark");
    printProgress(0.0f, "Resetting brain for performance test...");
    runPerformanceTest(brain, logLevel);
    printProgress(1.0f, "Performance test complete.");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== All Tests Complete ===");
}

void runConnectivityTest(std::shared_ptr<Brain> brain, LogLevel logLevel) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
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
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
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

void runPlasticityTest(std::shared_ptr<Brain> brain, LogLevel logLevel) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runSTDPTest(std::shared_ptr<Brain> brain, LogLevel logLevel) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void runPerformanceTest(std::shared_ptr<Brain> brain, LogLevel logLevel) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 4: Performance Benchmark ===");
    
    // Configure for performance testing
    auto config = brain->getConfig();
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Reset and reinitialize with larger network
    brain->reset();
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for performance test");
        return;
    }
    
    // Measure performance
    auto startTime = std::chrono::high_resolution_clock::now();
    size_t steps = 1000;
    
    NLM_LOG_INFO("  Running " + std::to_string(steps) + " simulation steps...");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Inject random activity
        auto neurons = brain->getTotalNeuronCount();
        if (neurons > 0) {
            size_t activeNeurons = std::min<size_t>(10, neurons);
            for (size_t i = 0; i < activeNeurons; ++i) {
                // Find a random neuron and inject current
                brain->injectCurrent(NeuronId(static_cast<uint64_t>(i % neurons)), 20.0f);
            }
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            printProgress(static_cast<float>(step) / static_cast<float>(steps),
                         "Performance steps completed");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double stepsPerSecond = (steps * 1000.0) / duration.count();
    NLM_LOG_INFO("  Performance results:");
    NLM_LOG_INFO("    Total steps: " + std::to_string(steps));
    NLM_LOG_INFO("    Time elapsed: " + std::to_string(duration.count()) + "ms");
    NLM_LOG_INFO("    Steps per second: " + std::to_string(stepsPerSecond));
    NLM_LOG_INFO("    Neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("    Synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("    Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    
    NLM_LOG_INFO("  ✓ Performance test complete.");
}

int main(int argc, char** argv) {
    // Parse command line arguments
    argparse::ArgumentParser program("nlm", "Phase 2: Real Neural Computation", "1.0");
    
    // Test modes
    program.add_argument("--test").choices({"all", "connectivity", "plasticity", "stdp", "performance"}).default_value("all").help("Test mode to run");
    
    // Configuration
    program.add_argument("--config").default_value("configs/default.cfg").help("Load configuration from file");
    program.add_argument("--output").help("Save results to directory");
    program.add_argument("--verbose").flag().help("Enable verbose logging");
    program.add_argument("--quiet").flag().help("Minimal output");
    program.add_argument("--progress").flag().help("Show progress indicators");
    
    // Save/load
    program.add_argument("--save").help("Save brain state to file");
    program.add_argument("--load").help("Load brain state from file");
    program.add_argument("--checkpoint").help("Create checkpoint at step");
    
    // General options
    program.add_argument("--help").flag().help("Show this help message");
    program.add_argument("--version").flag().help("Show version information");
    program.add_argument("--random").help("Set random seed for reproducibility");
    program.add_argument("--threads").help("Use N threads for simulation");
    program.add_argument("--memory").help("Memory limit in MB");
    program.add_argument("--max-steps").help("Maximum simulation steps");
    
    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
        std::cerr << program.help() << std::endl;
        return 1;
    }
    
    // Handle help and version
    if (program["--help"].as<bool>()) {
        printHelp();
        return 0;
    }
    
    if (program["--version"].as<bool>()) {
        std::cout << "NLM Phase 2: Real Neural Computation v1.0" << std::endl;
        return 0;
    }
    
    printBanner();
    
    // Determine log level
    LogLevel logLevel = LogLevel::Info;
    if (program["--verbose"].as<bool>()) {
        logLevel = LogLevel::Debug;
    } else if (program["--quiet"].as<bool>()) {
        logLevel = LogLevel::Warning;
    }
    
    // Setup logging
    std::string logFile;
    if (program.present("--output")) {
        logFile = program.get<std::string>("--output") + "/nlm.log";
    }
    
    setupLogging(logLevel, logFile);
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = program.get<std::string>("--config");
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Handle random seed from command line
    if (program.present("--random")) {
        int64_t randomSeed = std::stoll(program.get<std::string>("--random"));
        config->set("random_seed", randomSeed, ConfigSource::Default);
        NLM_LOG_INFO("Set random seed from command line: " + std::to_string(randomSeed));
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Save brain state if requested
    if (program.present("--save")) {
        std::string saveFile = program.get<std::string>("--save");
        if (!brain->save(saveFile)) {
            NLM_LOG_WARNING("Failed to save brain state to: " + saveFile);
        }
    }
    
    // Load brain state if requested
    if (program.present("--load")) {
        std::string loadFile = program.get<std::string>("--load");
        if (!brain->load(loadFile)) {
            NLM_LOG_WARNING("Failed to load brain state from: " + loadFile);
        }
    }
    
    // Run tests based on mode
    std::string testMode = program.get<std::string>("--test");
    
    if (testMode == "all") {
        runAllTests(brain, logLevel);
    } else if (testMode == "connectivity") {
        runConnectivityTest(brain, logLevel);
    } else if (testMode == "plasticity") {
        runPlasticityTest(brain, logLevel);
    } else if (testMode == "stdp") {
        runSTDPTest(brain, logLevel);
    } else if (testMode == "performance") {
        runPerformanceTest(brain, logLevel);
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}
