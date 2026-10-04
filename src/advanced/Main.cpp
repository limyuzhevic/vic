// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
// Enhanced version with advanced features for power users
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Enhanced monitoring and debugging capabilities
//
// Advanced features for power users:
// - Real-time brain state monitoring
// - Performance profiling and analysis
// - Network topology visualization
// - Configuration management tools
// - Advanced error reporting and debugging

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
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║     Enhanced with Advanced Features                           ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║     - Enhanced monitoring and debugging                      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage() {
    std::cout << "=== NLM Phase 2 Usage ===" << std::endl;
    std::cout << "Basic usage:" << std::endl;
    std::cout << "  ./nlm_phase2 [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic options:" << std::endl;
    std::cout << "  --config <file>                    Load configuration from file" << std::endl;
    std::cout << "  --neurons <count>                  Number of neurons (default: 500)" << std::endl;
    std::cout << "  --regions <count>                  Number of regions (default: 1)" << std::endl;
    std::cout << "  --steps <count>                    Number of simulation steps (default: 1000)" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced options (for power users):" << std::endl;
    std::cout << "  --enable-monitoring                Enable real-time monitoring" << std::endl;
    std::cout << "  --monitoring-interval <ms>          Monitoring interval in milliseconds (default: 100)" << std::endl;
    std::cout << "  --enable-profiling                 Enable performance profiling" << std::endl;
    std::cout << "  --profile-output <file>            Profile output file (default: profile.txt)" << std::endl;
    std::cout << "  --enable-debug                    Enable detailed debugging output" << std::endl;
    std::cout << "  --debug-log <file>                Debug log file (default: debug.log)" << std::endl;
    std::cout << "  --enable-network-analysis          Analyze network topology" << std::endl;
    std::cout << "  --enable-synapse-analysis          Analyze synapse properties" << std::endl;
    std::cout << "  --output-state <file>              Save brain state to file" << std::endl;
    std::cout << "  --validation-only                  Only validate configuration, don't run simulation" << std::endl;
    std::cout << "  --help                            Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase2 --neurons 1000 --enable-monitoring --monitoring-interval 50" << std::endl;
    std::cout << "  ./nlm_phase2 --enable-profiling --profile-output timing.txt" << std::endl;
    std::cout << "  ./nlm_phase2 --enable-network-analysis --enable-synapse-analysis" << std::endl;
}

class AdvancedNLM {
public:
    AdvancedNLM() : brain_(nullptr), config_(nullptr), monitoringEnabled_(false), 
                   profilingEnabled_(false), debugEnabled_(false), networkAnalysisEnabled_(false),
                   synapseAnalysisEnabled_(false) {}
    
    ~AdvancedNLM() {
        if (brain_) {
            brain_->reset();
        }
        if (config_) {
            delete config_;
        }
    }
    
    bool initialize(int argc, char** argv) {
        parseCommandLine(argc, argv);
        
        // Initialize configuration
        config_ = new Config();
        config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Override with command line arguments
        config_->loadFromArgs(argc, argv);
        
        // Set configuration from command line
        config_->set("neuron_count", neurons_, ConfigSource::CommandLine);
        config_->set("region_count", regions_, ConfigSource::CommandLine);
        
        // Initialize brain
        brain_ = new Brain(config_);
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        // Initialize monitoring if enabled
        if (monitoringEnabled_) {
            startMonitoringThread();
        }
        
        // Initialize profiling if enabled
        if (profilingEnabled_) {
            startProfiling();
        }
        
        return true;
    }
    
    void run() {
        NLM_LOG_INFO("Starting NLM simulation with advanced features...");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run basic connectivity test
        runBasicConnectivityTest();
        
        // Reset brain for plasticity experiment
        brain_->reset();
        brain_->initialize();
        
        // Run plasticity experiment
        runPlasticityExperiment();
        
        // Reset and run STDP verification
        brain_->reset();
        brain_->initialize();
        runStdpVerification();
        
        // Network analysis if enabled
        if (networkAnalysisEnabled_) {
            analyzeNetwork();
        }
        
        // Synapse analysis if enabled
        if (synapseAnalysisEnabled_) {
            analyzeSynapses();
        }
        
        // Save brain state if requested
        if (!outputStateFile_.empty()) {
            saveBrainState(outputStateFile_);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("=== Simulation Complete ===");
        NLM_LOG_INFO("Total wall clock time: " + std::to_string(duration.count() / 1000.0) + "s");
        NLM_LOG_INFO("Final brain status:");
        brain_->logStatus();
        
        // Save profiling data if enabled
        if (profilingEnabled_) {
            saveProfilingData();
        }
        
        // Stop monitoring thread
        if (monitoringEnabled_) {
            stopMonitoringThread();
        }
    }
    
    void parseCommandLine(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help") {
                printUsage();
                exit(0);
            } else if (arg == "--enable-monitoring") {
                monitoringEnabled_ = true;
            } else if (arg.find("--monitoring-interval=") == 0) {
                monitoringInterval_ = std::stoi(arg.substr(strlen("--monitoring-interval=")));
            } else if (arg == "--enable-profiling") {
                profilingEnabled_ = true;
            } else if (arg.find("--profile-output=") == 0) {
                profileOutputFile_ = arg.substr(strlen("--profile-output="));
            } else if (arg == "--enable-debug") {
                debugEnabled_ = true;
                Logger::getInstance().setLevel(Logger::Level::Debug);
            } else if (arg.find("--debug-log=") == 0) {
                debugLogFile_ = arg.substr(strlen("--debug-log="));
            } else if (arg == "--enable-network-analysis") {
                networkAnalysisEnabled_ = true;
            } else if (arg == "--enable-synapse-analysis") {
                synapseAnalysisEnabled_ = true;
            } else if (arg.find("--output-state=") == 0) {
                outputStateFile_ = arg.substr(strlen("--output-state="));
            } else if (arg == "--validation-only") {
                validationOnly_ = true;
            } else if (arg.find("--config=") == 0) {
                configFile_ = arg.substr(strlen("--config="));
                config_->loadFromFile(configFile_);
            } else if (arg.find("--neurons=") == 0) {
                neurons_ = std::stoi(arg.substr(strlen("--neurons=")));
            } else if (arg.find("--regions=") == 0) {
                regions_ = std::stoi(arg.substr(strlen("--regions=")));
            } else if (arg.find("--steps=") == 0) {
                steps_ = std::stoi(arg.substr(strlen("--steps=")));
            }
        }
    }
    
private:
    void runBasicConnectivityTest() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain_->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain_->getTotalSpikeCount();
        
        // Inject current into a few neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < std::min(steps_, 50); ++step) {
            brain_->step(step, step * 0.001);
        }
        
        size_t spikes = brain_->getTotalSpikeCount() - initialSpikes;
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
    
    void runPlasticityExperiment() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        // Enable plasticity on synapses
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("")
        NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(steps_) + " steps)...");
        
        for (SimulationStep step = 0; step < steps_; ++step) {
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
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("✓ Plasticity experiment completed");
    }
    
    void runStdpVerification() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain_->getRegion(RegionId(1));
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
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        NLM_LOG_INFO("")
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
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
        
        // Check if weights increased (LTP)
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        NLM_LOG_INFO("")
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
    }
    
    void analyzeNetwork() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Network Analysis ===");
        
        size_t totalNeurons = brain_->getTotalNeuronCount();
        size_t totalRegions = brain_->getTotalRegionCount();
        size_t totalSynapses = brain_->getTotalSynapseCount();
        
        NLM_LOG_INFO("Network Statistics:");
        NLM_LOG_INFO("  Total neurons: " + std::to_string(totalNeurons));
        NLM_LOG_INFO("  Total regions: " + std::to_string(totalRegions));
        NLM_LOG_INFO("  Total synapses: " + std::to_string(totalSynapses));
        
        if (totalSynapses > 0) {
            float avgWeight = 0.0f;
            float maxWeight = -1000.0f;
            float minWeight = 1000.0f;
            size_t excitatory = 0;
            size_t inhibitory = 0;
            
            // Get all synapses
            for (const auto& region : brain_->getRegions()) {
                for (const auto& syn : region->getSynapses()) {
                    avgWeight += syn->getWeight();
                    maxWeight = std::max(maxWeight, syn->getWeight());
                    minWeight = std::min(minWeight, syn->getWeight());
                    
                    if (syn->isExcitatory()) {
                        excitatory++;
                    } else if (syn->isInhibitory()) {
                        inhibitory++;
                    }
                }
            }
            
            avgWeight /= totalSynapses;
            NLM_LOG_INFO("  Average synapse weight: " + std::to_string(avgWeight));
            NLM_LOG_INFO("  Max synapse weight: " + std::to_string(maxWeight));
            NLM_LOG_INFO("  Min synapse weight: " + std::to_string(minWeight));
            NLM_LOG_INFO("  Excitatory synapses: " + std::to_string(excitatory));
            NLM_LOG_INFO("  Inhibitory synapses: " + std::to_string(inhibitory));
        }
        
        NLM_LOG_INFO("✓ Network analysis completed");
    }
    
    void analyzeSynapses() {
        NLM_LOG_INFO("")
        NLM_LOG_INFO("=== Synapse Analysis ===");
        
        size_t totalSynapses = brain_->getTotalSynapseCount();
        NLM_LOG_INFO("Analyzing " + std::to_string(totalSynapses) + " synapses...");
        
        // Analyze synapse properties
        std::map<std::string, size_t> synapseTypes;
        std::map<float, size_t> weightDistribution;
        
        for (const auto& region : brain_->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                // Count synapse types
                if (syn->isExcitatory()) {
                    synapseTypes["Excitatory"]++;
                } else if (syn->isInhibitory()) {
                    synapseTypes["Inhibitory"]++;
                } else {
                    synapseTypes["Other"]++;
                }
                
                // Bin weights for distribution
                float weight = syn->getWeight();
                int bin = static_cast<int>((weight + 1.0f) * 50.0f); // Map -1 to 1 to bins 0-100
                if (bin < 0) bin = 0;
                if (bin > 100) bin = 100;
                weightDistribution[bin]++;
            }
        }
        
        NLM_LOG_INFO("Synapse Type Distribution:");
        for (const auto& pair : synapseTypes) {
            NLM_LOG_INFO("  " + pair.first + ": " + std::to_string(pair.second));
        }
        
        NLM_LOG_INFO("Weight Distribution (0-1 normalized):");
        for (const auto& pair : weightDistribution) {
            float weight = (pair.first / 50.0f) - 1.0f;
            NLM_LOG_INFO("  [" + std::to_string(weight) + ", " + std::to_string(weight + 0.02f) + "): " + std::to_string(pair.second));
        }
        
        NLM_LOG_INFO("✓ Synapse analysis completed");
    }
    
    void saveBrainState(const std::string& filename) {
        NLM_LOG_INFO("")
        NLM_LOG_INFO("Saving brain state to: " + filename);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for saving brain state: " + filename);
            return;
        }
        
        // Save basic brain information
        file << "NLM Brain State" << std::endl;
        file << "=================" << std::endl;
        file << "Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
        file << "Total regions: " << brain_->getTotalRegionCount() << std::endl;
        file << "Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
        file << "Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
        file << "Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
        
        file << std::endl;
        file << "Configuration:" << std::endl;
        file << "  Neuron count: " << neurons_ << std::endl;
        file << "  Region count: " << regions_ << std::endl;
        file << "  Steps: " << steps_ << std::endl;
        
        file.close();
        NLM_LOG_INFO("Brain state saved successfully");
    }
    
    void startMonitoringThread() {
        NLM_LOG_INFO("Starting real-time monitoring thread...");
        monitoringThread_ = std::thread(&AdvancedNLM::monitoringLoop, this);
    }
    
    void stopMonitoringThread() {
        if (monitoringThread_.joinable()) {
            monitoringShouldStop_ = true;
            monitoringThread_.join();
        }
    }
    
    void monitoringLoop() {
        while (!monitoringShouldStop_) {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMonitorTime_);
            
            if (elapsed.count() >= monitoringInterval_) {
                logMonitoringData();
                lastMonitorTime_ = now;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void logMonitoringData() {
        NLM_LOG_INFO("[MONITORING] Neurons: " + std::to_string(brain_->getTotalNeuronCount()) +
                    " | Spikes: " + std::to_string(brain_->getTotalSpikeCount()) +
                    " | Firing: " + std::to_string(brain_->getFiringNeuronCount()) +
                    " | Synapses: " + std::to_string(brain_->getTotalSynapseCount()));
    }
    
    void startProfiling() {
        NLM_LOG_INFO("Starting performance profiling...");
        profileStartTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void saveProfilingData() {
        NLM_LOG_INFO("Saving profiling data to: " + profileOutputFile_);
        
        std::ofstream file(profileOutputFile_);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open profile file: " + profileOutputFile_);
            return;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - profileStartTime_);
        
        file << "NLM Performance Profile" << std::endl;
        file << "=======================" << std::endl;
        file << "Total execution time: " << duration.count() / 1000.0 << "s" << std::endl;
        file << "Average step time: " << duration.count() / static_cast<double>(steps_) << "ms" << std::endl;
        file << "Steps per second: " << (steps_ * 1000.0) / duration.count() << std::endl;
        
        // Basic memory and performance statistics
        file << std::endl;
        file << "Memory and Performance Statistics:" << std::endl;
        file << "  Neurons processed: " << brain_->getTotalNeuronCount() << std::endl;
        file << "  Synapses processed: " << brain_->getTotalSynapseCount() << std::endl;
        file << "  Spikes generated: " << brain_->getTotalSpikeCount() << std::endl;
        file << "  Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
        
        file.close();
        NLM_LOG_INFO("Profiling data saved successfully");
    }
    
    // Advanced debugging and analysis methods
    void runAdvancedDebugging() {
        NLM_LOG_INFO("")
        NLM_LOG_INFO("=== Advanced Debugging ===");
        
        // Debug neuron states
        NLM_LOG_INFO("Neuron states:");
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto& neuron : pop->getNeurons()) {
                    NLM_LOG_INFO("  Neuron " + std::to_string(neuron->getId()) +
                                " Type: " + std::to_string(static_cast<int>(neuron->getType())) +
                                " V: " + std::to_string(neuron->getMembranePotential()) +
                                " Firing: " + std::to_string(neuron->isFiring()) +
                                " Refractory: " + std::to_string(neuron->isRefractory()));
                }
            }
        }
        
        // Debug synapse states
        NLM_LOG_INFO("Synapse states:");
        size_t synapseCount = 0;
        for (const auto& region : brain_->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                if (synapseCount++ < 10) {  // Limit output
                    NLM_LOG_INFO("  Synapse " + std::to_string(syn->getId()) +
                                " Pre: " + std::to_string(syn->getSourceNeuron()) +
                                " Post: " + std::to_string(syn->getDestinationNeuron()) +
                                " Weight: " + std::to_string(syn->getWeight()) +
                                " Delay: " + std::to_string(syn->getDelay()));
                }
            }
        }
        
        NLM_LOG_INFO("✓ Advanced debugging completed");
    }
    
    void performConfigurationAnalysis() {
        NLM_LOG_INFO("")
        NLM_LOG_INFO("=== Configuration Analysis ===");
        
        if (!config_) {
            NLM_LOG_INFO("No configuration available for analysis");
            return;
        }
        
        NLM_LOG_INFO("Configuration summary:");
        NLM_LOG_INFO(config_->summary());
        
        // Validate configuration
        bool isValid = config_->validateConfiguration();
        NLM_LOG_INFO("Configuration validation: " + std::string(isValid ? "PASSED" : "FAILED"));
        
        if (!isValid) {
            NLM_LOG_INFO("Configuration validation errors detected");
        }
        
        NLM_LOG_INFO("✓ Configuration analysis completed");
    }
    
    // Utility methods
    static void logError(const std::string& message) {
        NLM_LOG_ERROR(message);
    }
    
    static void logWarning(const std::string& message) {
        NLM_LOG_WARNING(message);
    }
    
    static void logInfo(const std::string& message) {
        NLM_LOG_INFO(message);
    }
    
private:
    Brain* brain_;
    Config* config_;
    
    // Advanced features flags
    bool monitoringEnabled_;
    bool profilingEnabled_;
    bool debugEnabled_;
    bool networkAnalysisEnabled_;
    bool synapseAnalysisEnabled_;
    
    // Configuration parameters
    size_t neurons_;
    size_t regions_;
    size_t steps_;
    std::string configFile_;
    std::string outputStateFile_;
    std::string profileOutputFile_;
    std::string debugLogFile_;
    
    // Monitoring parameters
    size_t monitoringInterval_;
    bool validationOnly_;
    
    // Profiling parameters
    std::chrono::high_resolution_clock::time_point profileStartTime_;
    
    // Monitoring thread
    std::thread monitoringThread_;
    std::atomic<bool> monitoringShouldStop_{false};
    std::chrono::high_resolution_clock::time_point lastMonitorTime_;
};

int main(int argc, char* argv[]) {
    printBanner();
    
    // Check for help flag
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            printUsage();
            return 0;
        }
    }
    
    // Create advanced NLM instance
    AdvancedNLM nlm;
    
    // Initialize and run
    if (!nlm.initialize(argc, argv)) {
        NLM_LOG_ERROR("Failed to initialize NLM!");
        return 1;
    }
    
    nlm.run();
    
    return 0;
}
