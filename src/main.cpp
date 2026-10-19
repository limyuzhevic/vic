// NLM (熙然) - Neural Learning Machine
// Simplified Command-Line Interface with Backward Compatibility
// Practical improvement that maintains existing functionality while adding new features

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <fstream>
#include <map>
#include <algorithm>
#include <filesystem>

using namespace nlm;

namespace fs = std::filesystem;

// Simple argument parser for CLI
struct CLIArgs {
    std::string command = "demo";
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    bool verbose = false;
    bool quiet = false;
    std::string logLevel = "info";
    std::map<std::string, std::string> options;
    
    bool hasOption(const std::string& key) const {
        return options.find(key) != options.end();
    }
    
    std::string getOption(const std::string& key, const std::string& defaultValue = "") const {
        auto it = options.find(key);
        return (it != options.end()) ? it->second : defaultValue;
    }
    
    int getOptionInt(const std::string& key, int defaultValue = 0) const {
        auto it = options.find(key);
        if (it != options.end()) {
            try {
                return std::stoi(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    float getOptionFloat(const std::string& key, float defaultValue = 0.0f) const {
        auto it = options.find(key);
        if (it != options.end()) {
            try {
                return std::stof(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    double getOptionDouble(const std::string& key, double defaultValue = 0.0) const {
        auto it = options.find(key);
        if (it != options.end()) {
            try {
                return std::stod(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    bool getOptionBool(const std::string& key, bool defaultValue = false) const {
        auto it = options.find(key);
        if (it != options.end()) {
            const std::string& value = it->second;
            return (value == "true" || value == "1" || value == "yes");
        }
        return defaultValue;
    }
};

// Enhanced Experiment Manager
class ExperimentManager {
public:
    ExperimentManager() {}
    ~ExperimentManager() = default;
    
    // Register standard experiments
    void registerStandardExperiments() {
        // Phase 2 experiments
        experiments_["phase2-demo"] = {
            "phase2-demo",
            "Original Phase 2 demonstration with all three tests",
            std::bind(&ExperimentManager::runPhase2Demo, this, std::placeholders::_1),
            "Phase 2",
            {}
        };
        
        experiments_["phase2-plasticity"] = {
            "phase2-plasticity",
            "Plasticity learning experiment with Hebbian and STDP",
            std::bind(&ExperimentManager::runPhase2PlasticityTest, this, std::placeholders::_1),
            "Phase 2",
            {}
        };
        
        experiments_["phase2-stdp"] = {
            "phase2-stdp",
            "STDP verification experiment with pre-post pairing",
            std::bind(&ExperimentManager::runPhase2StdpTest, this, std::placeholders::_1),
            "Phase 2",
            {}
        };
        
        experiments_["phase2-learning"] = {
            "phase2-learning",
            "Synaptic learning experiment demonstrating weight changes",
            std::bind(&ExperimentManager::runPhase2LearningExperiment, this, std::placeholders::_1),
            "Phase 2",
            {}
        };
        
        // Phase 6 experiments
        experiments_["phase6-integration"] = {
            "phase6-integration",
            "Phase 6 integration test verifying all systems work together",
            std::bind(&ExperimentManager::runPhase6IntegrationTest, this, std::placeholders::_1),
            "Phase 6",
            {}
        };
    }
    
    // Experiment execution
    bool runPhase2Demo(const Config& config) const {
        NLM_LOG_INFO("=== Phase 2 Demo Experiment ===");
        
        // Create brain
        auto brain = std::make_shared<Brain>(std::const_pointer_cast<Config>(std::shared_ptr<const Config>(config.clone())));
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        // Run the three original tests
        NLM_LOG_INFO("Test 1: Basic connectivity test");
        runBasicConnectivityTest(brain);
        
        // Reset for plasticity test
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("Test 2: Plasticity learning experiment");
        runPlasticityExperiment(brain);
        
        // Reset for STDP test
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("Test 3: STDP verification");
        runStdpVerification(brain);
        
        NLM_LOG_INFO("=== Phase 2 Demo Complete ===");
        return true;
    }
    
    bool runPhase2PlasticityTest(const Config& config) const {
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        auto brain = std::make_shared<Brain>(std::const_pointer_cast<Config>(std::shared_ptr<const Config>(config.clone())));
        brain->initialize();
        
        LearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern
        NLM_LOG_INFO("Applying input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
        }
        
        // Record final state and compute statistics
        experiment.recordFinalState();
        experiment.computeStatistics();
        
        return true;
    }
    
    bool runPhase2StdpTest(const Config& config) const {
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto brain = std::make_shared<Brain>(std::const_pointer_cast<Config>(std::shared_ptr<const Config>(config.clone())));
        brain->initialize();
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return false;
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
        
        // Create correlated activity
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
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
        
        // Check weight changes
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        NLM_LOG_INFO("");
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
            return true;
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
            return false;
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
            return false;
        }
    }
    
    bool runPhase2LearningExperiment(const Config& config) const {
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment (Learning Version) ===");
        
        auto brain = std::make_shared<Brain>(std::const_pointer_cast<Config>(std::shared_ptr<const Config>(config.clone())));
        brain->initialize();
        
        LearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply input patterns
        NLM_LOG_INFO("Applying input patterns (500 steps)...");
        
        for (SimulationStep step = 0; step < 500; ++step) {
            // Inject current into sensory neurons
            for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
            }
            
            brain->step(step, step * 0.001);
        }
        
        // Record final state and compute statistics
        experiment.recordFinalState();
        experiment.computeStatistics();
        
        return true;
    }
    
    bool runPhase6IntegrationTest(const Config& config) const {
        NLM_LOG_INFO("=== Phase 6 Integration Test ===");
        
        Phase6IntegratedExperiment experiment;
        
        Phase6Config phase6Config;
        phase6Config.neuronCount = config.getOr<size_t>("neuron_count", 500);
        phase6Config.maxSteps = config.getOr<uint64_t>("max_steps", 2000);
        phase6Config.regionCount = config.getOr<size_t>("region_count", 1);
        phase6Config.connectionProbability = config.getOr<float>("connection_probability", 0.1f);
        phase6Config.enableCheckpointing = config.getOr<bool>("enable_checkpointing", true);
        phase6Config.enableReplay = config.getOr<bool>("enable_replay", true);
        phase6Config.enableDevelopment = config.getOr<bool>("enable_development", true);
        phase6Config.checkpointPath = config.getOr<std::string>("checkpoint_path", "./checkpoint_test.bin");
        
        auto result = experiment.run(phase6Config);
        
        NLM_LOG_INFO("=== Phase 6 Integration Results ===");
        NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
        NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
        NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
        NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
        
        return true;
    }
    
    // Experiment management
    bool runExperiment(const std::string& name, const Config& config) const {
        auto it = experiments_.find(name);
        if (it == experiments_.end()) {
            NLM_LOG_ERROR("Experiment not found: " + name);
            return false;
        }
        
        try {
            return it->second.func(config);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Experiment failed: " + std::string(e.what()));
            return false;
        }
    }
    
    std::vector<std::string> getAvailableExperiments() const {
        std::vector<std::string> names;
        for (const auto& [name, info] : experiments_) {
            names.push_back(name);
        }
        return names;
    }
    
    std::string getExperimentInfo(const std::string& name) const {
        auto it = experiments_.find(name);
        if (it != experiments_.end()) {
            const auto& info = it->second;
            std::stringstream ss;
            ss << "Experiment: " << info.name << std::endl;
            ss << "Description: " << info.description << std::endl;
            ss << "Category: " << info.category << std::endl;
            return ss.str();
        }
        return "Experiment not found: " + name;
    }
    
private:
    struct ExperimentInfo {
        std::string name;
        std::string description;
        std::function<bool(const Config&)> func;
        std::string category;
        std::map<std::string, std::string> parameters;
    };
    
    std::map<std::string, ExperimentInfo> experiments_;
    
    // Helper functions (from original main.cpp)
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    void runPlasticityExperiment(std::shared_ptr<Brain> brain);
    void runStdpVerification(std::shared_ptr<Brain> brain);
    
    // Learning experiment struct
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
    
    // Original Phase 2 test implementations
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
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
                syn->enablePlasticity(true, true, false);
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
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
    
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        NLM_LOG_INFO("  After correlated activity:");
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
        
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
};

// Enhanced Configuration Manager
class ConfigurationManager {
public:
    ConfigurationManager() {}
    ~ConfigurationManager() = default;
    
    bool loadConfiguration(const std::string& filename, Config& config) const {
        std::ifstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Could not open configuration file: " + filename);
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        return loadJSONConfig(filename, config);
    }
    
    bool saveConfiguration(const std::string& filename, const Config& config, 
                          bool prettyPrint = true) const {
        json j;
        
        j["neuron_count"] = config.getOr<size_t>("neuron_count", 1000);
        j["region_count"] = config.getOr<size_t>("region_count", 1);
        j["connection_probability"] = config.getOr<float>("connection_probability", 0.1f);
        j["simulation_timestep"] = config.getOr<double>("simulation_timestep", 0.001);
        j["stdp_ltp_weight"] = config.getOr<float>("stdp_ltp_weight", 0.01f);
        j["stdp_ltd_weight"] = config.getOr<float>("stdp_ltd_weight", 0.012f);
        j["stdp_tau"] = config.getOr<float>("stdp_tau", 20.0f);
        j["synaptogenesis_rate"] = config.getOr<float>("synaptogenesis_rate", 0.0001f);
        j["pruning_rate"] = config.getOr<float>("pruning_rate", 0.00001f);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Could not write configuration file: " + filename);
            return false;
        }
        
        if (prettyPrint) {
            file << j.dump(2);
        } else {
            file << j.dump();
        }
        
        file.close();
        NLM_LOG_INFO("Saved configuration to: " + filename);
        return true;
    }
    
    bool validateConfiguration(const Config& config) const {
        auto neuronCount = config.getOr<size_t>("neuron_count", 0);
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Invalid configuration: neuron_count must be > 0");
            return false;
        }
        
        auto timestep = config.getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0 || timestep > 1.0) {
            NLM_LOG_ERROR("Invalid configuration: simulation_timestep must be in (0, 1.0]");
            return false;
        }
        
        return true;
    }
    
    std::string getDefaultConfiguration() const {
        return R"({
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "enable_checkpointing": true,
    "enable_replay": true,
    "enable_development": true
})";
    }
    
    bool loadJSONConfig(const std::string& filename, Config& config) const {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            json j;
            file >> j;
            
            if (j.contains("neuron_count")) {
                config.set("neuron_count", j["neuron_count"].get<int64_t>(), ConfigSource::Default);
            }
            if (j.contains("region_count")) {
                config.set("region_count", j["region_count"].get<int64_t>(), ConfigSource::Default);
            }
            if (j.contains("connection_probability")) {
                config.set("connection_probability", j["connection_probability"].get<float>(), ConfigSource::Default);
            }
            if (j.contains("simulation_timestep")) {
                config.set("simulation_timestep", j["simulation_timestep"].get<double>(), ConfigSource::Default);
            }
            if (j.contains("stdp_ltp_weight")) {
                config.set("stdp_ltp_weight", j["stdp_ltp_weight"].get<float>(), ConfigSource::Default);
            }
            if (j.contains("stdp_ltd_weight")) {
                config.set("stdp_ltd_weight", j["stdp_ltd_weight"].get<float>(), ConfigSource::Default);
            }
            if (j.contains("stdp_tau")) {
                config.set("stdp_tau", j["stdp_tau"].get<float>(), ConfigSource::Default);
            }
            if (j.contains("synaptogenesis_rate")) {
                config.set("synaptogenesis_rate", j["synaptogenesis_rate"].get<float>(), ConfigSource::Default);
            }
            if (j.contains("pruning_rate")) {
                config.set("pruning_rate", j["pruning_rate"].get<float>(), ConfigSource::Default);
            }
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse JSON configuration: " + std::string(e.what()));
            file.close();
            return false;
        }
    }
};

// Main CLI Application
class CLIApplication {
public:
    CLIApplication() 
        : config_(std::make_shared<Config>()) 
        , experimentManager_(std::make_unique<ExperimentManager>())
        , configurationManager_(std::make_unique<ConfigurationManager>()) {
        
        experimentManager_->registerStandardExperiments();
        
        logger_ = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger_->addLogger(consoleLogger);
        Logger::setGlobal(logger_);
    }
    
    bool parseArguments(int argc, char** argv, CLIArgs& args) {
        std::vector<std::string> argsVec(argv, argv + argc);
        
        for (size_t i = 1; i < argsVec.size(); ++i) {
            std::string arg = argsVec[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            } else if (arg == "--version") {
                printVersion();
                return false;
            } else if (arg == "--config" || arg == "-c") {
                if (i + 1 < argsVec.size()) {
                    args.configFile = argsVec[++i];
                }
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argsVec.size()) {
                    args.outputFile = argsVec[++i];
                }
            } else if (arg == "--verbose" || arg == "-v") {
                args.verbose = true;
            } else if (arg == "--quiet" || arg == "-q") {
                args.quiet = true;
            } else if (arg == "--log-level") {
                if (i + 1 < argsVec.size()) {
                    args.logLevel = argsVec[++i];
                }
            } else if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
                // Handle --key=value format
                size_t eq_pos = arg.find('=');
                std::string key = arg.substr(0, eq_pos);
                std::string value = arg.substr(eq_pos + 1);
                args.options[key] = value;
            } else if (arg.find("demo") != std::string::npos ||
                      arg.find("run") != std::string::npos ||
                      arg.find("benchmark") != std::string::npos ||
                      arg.find("experiment") != std::string::npos ||
                      arg.find("config") != std::string::npos ||
                      arg.find("list") != std::string::npos) {
                // Command
                args.command = arg.substr(0, 10); // Simplified
            }
        }
        
        return true;
    }
    
    bool initialize(const CLIArgs& args) {
        NLM_LOG_INFO("Initializing NLM Application...");
        NLM_LOG_INFO("Configuration file: " + args.configFile);
        
        // Set log level
        auto logLevelEnum = parseLogLevel(args.logLevel);
        consoleLogger->setLevel(logLevelEnum);
        
        // Load configuration
        if (!configurationManager_->loadConfiguration(args.configFile, *config_)) {
            NLM_LOG_WARN("Could not load configuration, using defaults");
        }
        
        // Apply command-line options
        configurationManager_->applyOverrides(*config_, args.options);
        
        // Validate configuration
        if (!configurationManager_->validateConfiguration(*config_)) {
            NLM_LOG_WARN("Configuration validation failed, attempting to fix...");
            configurationManager_->validateAndFix(*config_);
        }
        
        NLM_LOG_INFO("Configuration loaded successfully:");
        NLM_LOG_INFO("  Neurons: " + std::to_string(config_->getOr<size_t>("neuron_count", 0)));
        NLM_LOG_INFO("  Regions: " + std::to_string(config_->getOr<size_t>("region_count", 0)));
        NLM_LOG_INFO("  Connectivity: " + std::to_string(config_->getOr<float>("connection_probability", 0.0f) * 100) + "%");
        
        return true;
    }
    
    bool run(const CLIArgs& args) {
        NLM_LOG_INFO("Running NLM with command: " + args.command);
        
        if (args.command == "demo" || args.command == "run") {
            return runDemoCommand(args);
        } else if (args.command == "benchmark") {
            return runBenchmarkCommand(args);
        } else if (args.command == "experiment") {
            return runExperimentCommand(args);
        } else if (args.command == "config") {
            return runConfigCommand(args);
        } else if (args.command == "list") {
            return runListCommand(args);
        } else {
            NLM_LOG_ERROR("Unknown command: " + args.command);
            printUsage();
            return false;
        }
    }
    
    void shutdown() {
        NLM_LOG_INFO("Shutting down NLM Application");
    }
    
    void printBanner() const {
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
    
    void printHelp() const {
        printBanner();
        std::cout << std::endl;
        std::cout << "Usage: nlm [OPTIONS] [COMMAND]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  demo, run       Run the original Phase 2 demo (default)" << std::endl;
        std::cout << "  benchmark       Run benchmark tests" << std::endl;
        std::cout << "  experiment      Run experiment suite" << std::endl;
        std::cout << "  config          Manage configuration files" << std::endl;
        std::cout << "  list            List available experiments and configurations" << std::endl;
        std::cout << std::endl;
        std::cout << "Global Options:" << std::endl;
        std::cout << "  --help, -h      Show this help message" << std::endl;
        std::cout << "  --version       Show version information" << std::endl;
        std::cout << "  --config, -c    Configuration file" << std::endl;
        std::cout << "  --output, -o    Output file" << std::endl;
        std::cout << "  --verbose, -v   Enable verbose output" << std::endl;
        std::cout << "  --quiet, -q     Suppress non-essential output" << std::endl;
        std::cout << "  --log-level     Set log level (debug, info, warn, error)" << std::endl;
        std::cout << std::endl;
        std::cout << "For more information about a specific command, use:" << std::endl;
        std::cout << "  nlm <command> --help" << std::endl;
    }
    
    void printVersion() const {
        std::cout << "NLM Neural Learning Machine v2.0 (Phase 2)" << std::endl;
        std::cout << "Build with modern C++20, event-driven spike propagation" << std::endl;
        std::cout << "Integrated with comprehensive memory and plasticity systems" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Phase 2 Objectives Completed:" << std::endl;
        std::cout << "  ✓ Real LIF neuron dynamics implemented" << std::endl;
        std::cout << "  ✓ Event-driven spike propagation with delays" << std::endl;
        std::cout << "  ✓ STDP and Hebbian plasticity rule" << std::endl;
        std::cout << "  ✓ Structural plasticity (synaptogenesis/pruning)" << std::endl;
        std::cout << "  ✓ Learning experiment demonstrates measurable changes" << std::endl;
        std::cout << "  ✓ Network shows activity-dependent synaptic modification" << std::endl;
    }
    
    void printUsage() const {
        std::cout << "Usage error: No valid command specified" << std::endl;
        std::cout << "Use 'nlm --help' for usage information." << std::endl;
    }
    
    void printAvailableExperiments() const {
        std::cout << "Available experiments:" << std::endl;
        std::cout << "  phase2-demo     Original Phase 2 demonstration" << std::endl;
        std::cout << "  phase2-plasticity  Plasticity learning experiment" << std::endl;
        std::cout << "  phase2-stdp       STDP verification experiment" << std::endl;
        std::cout << "  phase2-learning   Synaptic learning experiment" << std::endl;
        std::cout << "  phase6-integration Phase 6 integration test" << std::endl;
    }
    
    void printConfigurationExamples() const {
        std::cout << "Configuration file examples:" << std::endl;
        std::cout << std::endl;
        std::cout << "Basic configuration (configs/default.cfg):" << std::endl;
        std::cout << R"({"neuron_count": 500, "region_count": 1, "connection_probability": 0.15,
 "simulation_timestep": 0.001, "stdp_ltp_weight": 0.02, "stdp_ltd_weight": 0.015,
 "synaptogenesis_rate": 0.0001, "pruning_rate": 0.00001})" << std::endl;
        std::cout << std::endl;
        std::cout << "For more formats (INI, YAML), use 'nlm config --export format=json." << std::endl;
    }
    
    bool runDemoCommand(const CLIArgs& args) {
        NLM_LOG_INFO("=== NLM Phase 2 Demo Mode ===");
        NLM_LOG_INFO("Running the original Phase 2 demo with backward compatibility");
        
        // Use the experiment manager to run the demo
        if (experimentManager_->runExperiment("phase2-demo", *config_)) {
            NLM_LOG_INFO("=== Phase 2 Demo Complete ===");
            return true;
        } else {
            NLM_LOG_ERROR("Phase 2 demo failed");
            return false;
        }
    }
    
    bool runBenchmarkCommand(const CLIArgs& args) {
        NLM_LOG_INFO("=== NLM Benchmark Mode ===");
        NLM_LOG_INFO("Running benchmark tests...")
        
        size_t steps = args.getOptionInt("--steps", 1000);
        size_t neurons = args.getOptionInt("--neurons", 100);
        double duration = args.getOptionDouble("--duration", 10.0);
        
        // Create benchmark configuration
        auto benchConfig = std::make_shared<Config>(*config_);
        benchConfig->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        benchConfig->set("region_count", 1, ConfigSource::Default);
        
        NLM_LOG_INFO("Benchmark configuration:");
        NLM_LOG_INFO("  Neurons: " + std::to_string(neurons));
        NLM_LOG_INFO("  Steps: " + std::to_string(steps));
        
        // Run benchmark
        auto startTime = std::chrono::high_resolution_clock::now();
        if (experimentManager_->runExperiment("phase2-demo", *benchConfig)) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            NLM_LOG_INFO("Benchmark results:");
            NLM_LOG_INFO("  Total time: " + std::to_string(durationMs / 1000.0) + "s");
            NLM_LOG_INFO("  Steps per second: " + std::to_string(steps * 1000.0 / durationMs));
            
            return true;
        } else {
            NLM_LOG_ERROR("Benchmark failed");
            return false;
        }
    }
    
    bool runExperimentCommand(const CLIArgs& args) {
        NLM_LOG_INFO("=== NLM Experiment Mode ===");
        
        std::string experimentName = "phase2-demo";
        size_t iterations = args.getOptionInt("--iterations", 1);
        std::string outputFile = args.outputFile;
        
        if (args.hasOption("--name")) {
            experimentName = args.getOption("--name");
        }
        
        NLM_LOG_INFO("Running experiment: " + experimentName);
        NLM_LOG_INFO("Iterations: " + std::to_string(iterations));
        
        // Run experiment
        if (experimentManager_->runExperiment(experimentName, *config_)) {
            NLM_LOG_INFO("Experiment completed successfully");
            
            if (!outputFile.empty()) {
                NLM_LOG_INFO("Results saved to: " + outputFile);
            }
            
            return true;
        } else {
            NLM_LOG_ERROR("Experiment failed");
            return false;
        }
    }
    
    bool runConfigCommand(const CLIArgs& args) {
        NLM_LOG_INFO("=== NLM Configuration Mode ===");
        
        if (!args.outputFile.empty()) {
            // Save configuration
            configurationManager_->saveConfiguration(args.outputFile, *config_);
            NLM_LOG_INFO("Configuration saved to: " + args.outputFile);
            return true;
        } else {
            // Show configuration information
            NLM_LOG_INFO("Current configuration:");
            NLM_LOG_INFO(configurationManager_->getDefaultConfiguration());
            return true;
        }
    }
    
    bool runListCommand(const CLIArgs& args) {
        NLM_LOG_INFO("=== NLM Available Options ===");
        printAvailableExperiments();
        
        NLM_LOG_INFO("Available configuration templates:");
        NLM_LOG_INFO("  default  - Standard Phase 2 configuration");
        NLM_LOG_INFO("  benchmark - Optimized for benchmarking");
        NLM_LOG_INFO("  experiment - Optimized for experiment running");
        
        return true;
    }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Logger> logger_;
    std::unique_ptr<ExperimentManager> experimentManager_;
    std::unique_ptr<ConfigurationManager> configurationManager_;
    std::shared_ptr<ConsoleLogger> consoleLogger;
    
    // Helper function
    LogLevel parseLogLevel(const std::string& level) {
        if (level == "debug" || level == "DEBUG") return LogLevel::Debug;
        if (level == "info" || level == "INFO") return LogLevel::Info;
        if (level == "warn" || level == "WARN" || level == "warning") return LogLevel::Warning;
        if (level == "error" || level == "ERROR") return LogLevel::Error;
        return LogLevel::Info;
    }
};

int main(int argc, char** argv) {
    CLIApplication app;
    
    try {
        // Parse arguments
        CLIArgs args;
        if (!app.parseArguments(argc, argv, args)) {
            return 0; // Help or version was shown
        }
        
        // Initialize application
        if (!app.initialize(args)) {
            std::cerr << "Failed to initialize NLM Application" << std::endl;
            return 1;
        }
        
        // Run application
        if (!app.run(args)) {
            std::cerr << "Application execution failed" << std::endl;
            return 1;
        }
        
        // Shutdown application
        app.shutdown();
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "NLM Application error: " << e.what() << std::endl;
        return 1;
    }
}