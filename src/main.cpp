// NLM (熙然) - Neural Learning Machine
// Enhanced CLI and Modular Main
//
// This phase implements real spiking neural computation with enhanced:
// - Proper command-line argument parsing
// - Modular test execution
// - Enhanced configuration management
// - Better error handling and user feedback

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <algorithm>
#include <chrono>

using namespace nlm;

// Forward declarations for CLI commands
class CLInterface;
class ConfigManager;
class TestRunner;

// Command result structure
struct CommandResult {
    bool success;
    std::string message;
    std::chrono::duration<double> executionTime;
    
    CommandResult(bool s, const std::string& msg) 
        : success(s), message(msg), executionTime(0.0) {}
};

// Configuration for CLI behavior
struct CLIConfig {
    std::string configFile = "configs/default.cfg";
    std::string logFile;
    bool verbose = false;
    bool quiet = false;
    std::string outputFile;
    bool jsonOutput = false;
    std::string experimentType = "full";
    size_t maxSteps = 1000;
    bool saveBrain = false;
    std::string brainSavePath;
};

// Enhanced banner with version info
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     Enhanced CLI with Modular Tests                          ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Pretty print for JSON output
std::string jsonString(const std::string& str) {
    std::ostringstream oss;
    oss << "\"";
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
                break;
        }
    }
    oss << "\"";
    return oss.str();
}

void printJsonResult(const std::string& key, const std::string& value) {
    std::cout << "{\"" << key << "\": " << jsonString(value) << "}";
}

void printJsonResult(const std::string& key, double value) {
    std::cout << "{\"" << key << "\": " << value << "}";
}

void printJsonResult(const std::string& key, size_t value) {
    std::cout << "{\"" << key << "\": " << value << "}";
}

void printJsonResult(const std::string& key, bool value) {
    std::cout << "{\"" << key << "\": " << (value ? "true" : "false") << "}";
}

// Output formatter for JSON and regular output
class OutputFormatter {
private:
    bool jsonMode;
    bool quiet;
    bool verbose;
    
public:
    OutputFormatter(bool json, bool quiet, bool verbose) 
        : jsonMode(json), quiet(quiet), verbose(verbose) {}
    
    template<typename T>
    void print(const std::string& label, const T& value, bool newline = true) {
        if (jsonMode) {
            printJsonResult(label, value);
            if (newline) std::cout << "\n";
        } else {
            if (!quiet) {
                if (verbose) {
                    std::cout << "[DEBUG] " << label << ": ";
                }
                std::cout << label << ": " << value;
                if (newline) std::cout << "\n";
            }
        }
    }
    
    void print(const std::string& message) {
        if (jsonMode) {
            std::cout << jsonString(message) << "\n";
        } else if (!quiet) {
            std::cout << message << "\n";
        }
    }
};

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
    
    void computeStatistics(OutputFormatter& output) {
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
        
        output.print("initial_mean_weight", initialMean);
        output.print("final_mean_weight", finalMean);
        output.print("weight_change", finalMean - initialMean);
        
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
        
        output.print("strengthened_synapses", strengthened);
        output.print("weakened_synapses", weakened);
        output.print("unchanged_synapses", unchanged);
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        output.print("total_spikes", brain->getTotalSpikeCount());
        output.print("most_active_neurons", mostActiveNeurons.size());
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
        
        output.print("learning_occurred", learningOccurred);
    }
};

// Test runner for modular execution
class TestRunner {
public:
    TestRunner(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    CommandResult runBasicConnectivityTest() {
        auto start = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain_->getRegion(RegionId(1));
        if (!region) {
            return CommandResult(false, "No region found");
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            return CommandResult(false, "No neurons found");
        }
        
        // Get initial spike count
        size_t initialSpikes = brain_->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain_->step(step, step * 0.001);
        }
        
        size_t spikes = brain_->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
            auto end = std::chrono::high_resolution_clock::now();
            return CommandResult(true, "Basic connectivity test passed, generated " + std::to_string(spikes) + " spikes");
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                            " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                            " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
            auto end = std::chrono::high_resolution_clock::now();
            return CommandResult(false, "No spikes generated");
        }
    }
    
    CommandResult runPlasticityExperiment() {
        auto start = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        LearningExperiment experiment(brain_, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain_->getTotalNeuronCount() / 4; ++i) {
                brain_->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain_->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain_->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain_->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        OutputFormatter formatter(false, false, true); // Verbose for intermediate output
        experiment.computeStatistics(formatter);
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        
        return CommandResult(true, "Plasticity experiment completed in " + std::to_string(duration.count()) + "s");
    }
    
    CommandResult runStdpVerification() {
        auto start = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain_->getRegion(RegionId(1));
        if (!region) {
            return CommandResult(false, "No region found");
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            return CommandResult(false, "Not enough synapses for STDP test");
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
                brain_->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain_->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights
        NLM_LOG_INFO("  After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
            totalDelta += delta;
        }
        
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        
        CommandResult result(totalDelta > 0.001f, 
                           "STDP test completed in " + std::to_string(duration.count()) + "s, delta: " + std::to_string(totalDelta));
        return result;
    }
    
    CommandResult runAllTests() {
        auto start = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("=== Running All Tests ===");
        
        // Reset brain between tests
        brain_->reset();
        brain_->initialize();
        
        CommandResult result1 = runBasicConnectivityTest();
        if (!result1.success) return result1;
        
        brain_->reset();
        brain_->initialize();
        
        CommandResult result2 = runPlasticityExperiment();
        if (!result2.success) return result2;
        
        brain_->reset();
        brain_->initialize();
        
        CommandResult result3 = runStdpVerification();
        if (!result3.success) return result3;
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== All Tests Completed Successfully ===");
        NLM_LOG_INFO("Total execution time: " + std::to_string(duration.count()) + " seconds");
        
        return CommandResult(true, "All tests completed successfully in " + std::to_string(duration.count()) + "s");
    }
    
    CommandResult runSingleTest(const std::string& testName) {
        brain_->reset();
        brain_->initialize();
        
        if (testName == "basic" || testName == "connectivity") {
            return runBasicConnectivityTest();
        } else if (testName == "plasticity" || testName == "learning") {
            return runPlasticityExperiment();
        } else if (testName == "stdp") {
            return runStdpVerification();
        } else {
            return CommandResult(false, "Unknown test: " + testName);
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
};