#!/usr/bin/env g++
// NLM Explore Tool
// Interactive exploration of brain parameters and behavior

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <functional>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"

using namespace nlm;

struct ExplorationResult {
    std::string parameter;
    double currentValue;
    double previousValue;
    double changeAmount;
    double effectOnPerformance;
    std::string parameterType;
};

class NLMExplore {
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::vector<ExplorationResult> explorationHistory;
    bool interactiveMode;
    double explorationStep;
    std::mutex exploreMutex;
    std::function<void(const std::string&)> callback;
    
public:
    NLMExplore() {
        config = std::make_shared<Config>();
        config->loadFromFile("configs/default.cfg");
        brain = nullptr;
        interactiveMode = true;
        explorationStep = 0.1;
        callback = nullptr;
    }
    
    void initializeBrain() {
        std::cout << "=== Initializing Brain for Exploration ===" << std::endl;
        
        brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            std::cout << "✓ Brain initialized successfully" << std::endl;
            std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "  Regions: " << config->getOr<int64_t>("region_count", 2) << std::endl;
            std::cout << "  Plasticity: " << (config->getOr<std::string>("plasticity_enabled", "true") == "true" ? "enabled" : "disabled") << std::endl;
        } else {
            std::cout << "✗ Brain initialization failed" << std::endl;
        }
    }
    
    void exploreParameter(const std::string& parameter, double newValue, double stepSize = 0.0) {
        std::lock_guard<std::mutex> lock(exploreMutex);
        
        std::cout << "\n=== Exploring Parameter: " << parameter << " ===" << std::endl;
        
        // Get current value
        double currentValue = getParameterValue(parameter);
        
        // Store exploration result
        ExplorationResult result;
        result.parameter = parameter;
        result.currentValue = currentValue;
        result.previousValue = currentValue;
        result.changeAmount = newValue - currentValue;
        
        // Apply parameter change if specified
        if (stepSize > 0.0) {
            newValue = currentValue + stepSize;
            result.changeAmount = stepSize;
        }
        
        // Set parameter in brain or config
        bool parameterApplied = applyParameterChange(parameter, newValue);
        
        if (parameterApplied) {
            result.effectOnPerformance = testParameterEffect(parameter, currentValue, newValue);
            std::cout << "✓ Parameter changed: " << parameter << " = " << currentValue << " -> " << newValue << std::endl;
            std::cout << "  Performance effect: " << (result.effectOnPerformance > 0 ? "improvement" : "degradation") << " (delta: " << result.effectOnPerformance << ")" << std::endl;
        } else {
            std::cout << "✗ Failed to change parameter: " << parameter << std::endl;
        }
        
        // Record exploration
        explorationHistory.push_back(result);
        
        // Notify callback if set
        if (callback) {
            callback(parameter + " changed from " + std::to_string(currentValue) + " to " + std::to_string(newValue));
        }
    }
    
    void exploreParameterRange(const std::string& parameter, double startValue, double endValue, double stepSize) {
        std::cout << "\n=== Exploring Parameter Range: " << parameter << " ===" << std::endl;
        std::cout << "Range: " << startValue << " -> " << endValue << ", Step: " << stepSize << std::endl;
        
        double currentValue = startValue;
        while (currentValue <= endValue + 1e-9) {
            exploreParameter(parameter, currentValue, stepSize);
            currentValue += stepSize;
            
            // Small delay for observation
            if (interactiveMode) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        
        std::cout << "Exploration completed for parameter range." << std::endl;
    }
    
    void exploreBrainState() {
        std::cout << "\n=== Brain State Exploration ===" << std::endl;
        
        if (!brain) {
            std::cout << "No brain initialized. Use --init first." << std::endl;
            return;
        }
        
        std::cout << "Current Brain State:" << std::endl;
        brain->logStatus();
        
        // Explore key brain metrics
        std::cout << "\nExploration Metrics:" << std::endl;
        
        // Spike activity
        size_t totalSpikes = brain->getTotalSpikeCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        std::cout << "  Spike Activity:" << std::endl;
        std::cout << "    Total Spikes: " << totalSpikes << std::endl;
        std::cout << "    Firing Neurons: " << firingNeurons << std::endl;
        
        // Connectivity
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            size_t neuronCount = region->getAllNeurons().size();
            size_t synapseCount = region->getSynapses().size();
            std::cout << "  Neural Network:" << std::endl;
            std::cout << "    Neurons: " << neuronCount << std::endl;
            std::cout << "    Synapses: " << synapseCount << std::endl;
            std::cout << "    Connectivity: " << (neuronCount > 0 ? (synapseCount / (double)neuronCount * 100) : 0) << "% average" << std::endl;
        }
        
        // Plasticity status
        if (region) {
            bool plasticityActive = false;
            for (const auto& syn : region->getSynapses()) {
                if (syn->hasPlasticity()) {
                    plasticityActive = true;
                    break;
                }
            }
            std::cout << "  Plasticity:" << std::endl;
            std::cout << "    Active: " << (plasticityActive ? "yes" : "no") << std::endl;
        }
        
        // Memory systems
        bool workingMemory = (brain->getWorkingMemory() != nullptr);
        bool episodicMemory = (brain->getEpisodicMemory() != nullptr);
        std::cout << "  Memory Systems:" << std::endl;
        std::cout << "    Working Memory: " << (workingMemory ? "active" : "inactive") << std::endl;
        std::cout << "    Episodic Memory: " << (episodicMemory ? "active" : "inactive") << std::endl;
        
        // Neuromodulation
        auto* neuromodulator = brain->getNeuromodulator();
        if (neuromodulator) {
            std::cout << "  Neuromodulation:" << std::endl;
            std::cout << "    Dopamine: " << (neuromodulator->getModulatorState(ModulatorType::Dopamine) ? "active" : "inactive") << std::endl;
            std::cout << "    Curiosity: " << (neuromodulator->getModulatorState(ModulatorType::Curiosity) ? "active" : "inactive") << std::endl;
            std::cout << "    Novelty: " << (neuromodulator->getModulatorState(ModulatorType::Novelty) ? "active" : "inactive") << std::endl;
        }
    }
    
    void exploreLearningDynamics() {
        std::cout << "\n=== Learning Dynamics Exploration ===" << std::endl;
        
        if (!brain) {
            std::cout << "No brain initialized. Use --init first." << std::endl;
            return;
        }
        
        // Record baseline performance
        size_t initialSpikes = brain->getTotalSpikeCount();
        auto* spikeSystem = brain->getSpikeSystem();
        
        std::cout << "Baseline Performance:" << std::endl;
        std::cout << "  Total Spikes: " << initialSpikes << std::endl;
        if (spikeSystem) {
            auto mostActive = spikeSystem->getMostActiveNeurons(5);
            std::cout << "  Most Active Neurons: " << mostActive.size() << std::endl;
        }
        
        // Run learning trials
        std::cout << "\nRunning learning trials..." << std::endl;
        
        for (int trial = 1; trial <= 5 && interactiveMode; ++trial) {
            std::cout << "\n--- Trial " << trial << " ---" << std::endl;
            
            // Record performance before trial
            size_t spikesBefore = brain->getTotalSpikeCount();
            
            // Apply learning stimulus
            brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
            
            // Run simulation
            for (SimulationStep step = 0; step < 50; ++step) {
                brain->step(step, step * 0.001);
            }
            
            // Record performance after trial
            size_t spikesAfter = brain->getTotalSpikeCount();
            size_t spikesGenerated = spikesAfter - spikesBefore;
            
            std::cout << "  Spikes generated: " << spikesGenerated << std::endl;
            std::cout << "  Total spikes now: " << spikesAfter << std::endl;
            
            // Small delay
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        
        std::cout << "\nLearning dynamics exploration completed." << std::endl;
    }
    
    double getParameterValue(const std::string& parameter) {
        // Extract parameter value from config
        if (parameter == "random_seed") {
            return config->getOr<int64_t>("random_seed", 42);
        } else if (parameter == "neuron_count") {
            return config->getOr<int64_t>("neuron_count", 1000);
        } else if (parameter == "region_count") {
            return config->getOr<int64_t>("region_count", 2);
        } else if (parameter == "connection_probability") {
            return config->getOr<double>("connection_probability", 0.1);
        } else if (parameter == "plasticity_learning_rate") {
            return config->getOr<double>("plasticity_learning_rate", 0.01);
        } else if (parameter == "stdp_ltp_weight") {
            return config->getOr<double>("stdp_ltp_weight", 0.01);
        } else if (parameter == "log_level") {
            return config->getOr<std::string>("log_level", "INFO");
        } else if (parameter == "simulation_timestep") {
            return config->getOr<double>("simulation_timestep", 0.001);
        }
        
        return 0.0;
    }
    
    bool applyParameterChange(const std::string& parameter, double newValue) {
        // Apply parameter change
        if (parameter == "random_seed") {
            config->set("random_seed", static_cast<int64_t>(newValue), ConfigSource::Runtime);
            return true;
        } else if (parameter == "neuron_count") {
            config->set("neuron_count", static_cast<int64_t>(newValue), ConfigSource::Runtime);
            return true;
        } else if (parameter == "region_count") {
            config->set("region_count", static_cast<int64_t>(newValue), ConfigSource::Runtime);
            return true;
        } else if (parameter == "connection_probability") {
            config->set("connection_probability", newValue, ConfigSource::Runtime);
            return true;
        } else if (parameter == "plasticity_learning_rate") {
            config->set("plasticity_learning_rate", newValue, ConfigSource::Runtime);
            return true;
        } else if (parameter == "stdp_ltp_weight") {
            config->set("stdp_ltp_weight", newValue, ConfigSource::Runtime);
            return true;
        } else if (parameter == "log_level") {
            config->set("log_level", std::to_string(newValue), ConfigSource::Runtime);
            return true;
        } else if (parameter == "simulation_timestep") {
            config->set("simulation_timestep", newValue, ConfigSource::Runtime);
            return true;
        }
        
        return false;
    }
    
    double testParameterEffect(const std::string& parameter, double oldValue, double newValue) {
        // Simulate parameter effect on performance
        // For now, return a dummy value based on parameter
        if (parameter == "random_seed") {
            return (newValue == oldValue) ? 0.0 : -0.1;
        } else if (parameter == "neuron_count") {
            return (newValue > oldValue) ? -0.5 : 0.3;
        } else if (parameter == "region_count") {
            return (newValue > oldValue) ? -0.2 : 0.1;
        } else if (parameter == "connection_probability") {
            return (newValue > oldValue) ? -0.3 : 0.2;
        } else if (parameter == "plasticity_learning_rate") {
            return (newValue > oldValue) ? -0.4 : 0.4;
        } else if (parameter == "stdp_ltp_weight") {
            return (newValue > oldValue) ? -0.3 : 0.3;
        }
        
        return 0.0;
    }
    
    void printExplorationHistory() {
        std::cout << "=== Exploration History ===" << std::endl;
        
        if (explorationHistory.empty()) {
            std::cout << "No exploration history recorded." << std::endl;
            return;
        }
        
        std::cout << std::left << std::setw(20) << "Parameter"
                  << std::setw(15) << "Old Value"
                  << std::setw(15) << "New Value"
                  << std::setw(15) << "Change"
                  << std::setw(20) << "Performance Effect" << std::endl;
        std::cout << std::string(85, '-') << std::endl;
        
        for (const auto& result : explorationHistory) {
            std::cout << std::left << std::setw(20) << result.parameter
                      << std::setw(15) << result.previousValue
                      << std::setw(15) << result.currentValue
                      << std::setw(15) << result.changeAmount
                      << std::setw(20) << result.effectOnPerformance << std::endl;
        }
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --init                   Initialize brain for exploration" << std::endl;
        std::cout << "  --explore <parameter> <value>    Explore specific parameter" << std::endl;
        std::cout << "  --explore-range <param> <start> <end> <step>   Explore parameter range" << std::endl;
        std::cout << "  --brain-state            Display current brain state" << std::endl;
        std::cout << "  --learning-dynamics      Explore learning dynamics" << std::endl;
        std::cout << "  --history                Show exploration history" << std::endl;
        std::cout << "  --interactive-off       Disable interactive mode" << std::endl;
        std::cout << "  --step <size>            Set exploration step size" << std::endl;
        std::cout << "  --callback <function>    Set callback function" << std::endl;
        std::cout << "  --config <file>          Load configuration file" << std::endl;
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-explore";
    bool initBrain = false;
    std::string exploreParam;
    double exploreValue = 0.0;
    std::string exploreRangeParam;
    double exploreRangeStart = 0.0, exploreRangeEnd = 0.0, exploreRangeStep = 0.0;
    bool showBrainState = false;
    bool showLearningDynamics = false;
    bool showHistory = false;
    bool interactive = true;
    double stepSize = 0.1;
    std::string configFile;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            NLMExplore explore;
            explore.printUsage(programName);
            return 0;
        } else if (arg == "--init") {
            initBrain = true;
        } else if (arg == "--explore") {
            if (i + 2 < argc) {
                exploreParam = argv[++i];
                exploreValue = std::stod(argv[++i]);
            } else {
                std::cerr << "Error: --explore requires parameter and value arguments" << std::endl;
                return 1;
            }
        } else if (arg == "--explore-range") {
            if (i + 4 < argc) {
                exploreRangeParam = argv[++i];
                exploreRangeStart = std::stod(argv[++i]);
                exploreRangeEnd = std::stod(argv[++i]);
                exploreRangeStep = std::stod(argv[++i]);
            } else {
                std::cerr << "Error: --explore-range requires parameter, start, end, and step arguments" << std::endl;
                return 1;
            }
        } else if (arg == "--brain-state") {
            showBrainState = true;
        } else if (arg == "--learning-dynamics") {
            showLearningDynamics = true;
        } else if (arg == "--history") {
            showHistory = true;
        } else if (arg == "--interactive-off") {
            interactive = false;
        } else if (arg == "--step") {
            if (i + 1 < argc) {
                stepSize = std::stod(argv[++i]);
            } else {
                std::cerr << "Error: --step requires a value argument" << std::endl;
                return 1;
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a filename argument" << std::endl;
                return 1;
            }
        }
    }
    
    NLMExplore explore;
    explore.interactiveMode = interactive;
    explore.explorationStep = stepSize;
    
    if (!configFile.empty()) {
        if (!explore.config->loadFromFile(configFile)) {
            std::cerr << "Error: Failed to load configuration file: " << configFile << std::endl;
            return 1;
        }
    }
    
    if (initBrain) {
        explore.initializeBrain();
    }
    
    if (!exploreParam.empty() && exploreValue != 0.0) {
        explore.exploreParameter(exploreParam, exploreValue);
    }
    
    if (!exploreRangeParam.empty()) {
        explore.exploreParameterRange(exploreRangeParam, exploreRangeStart, exploreRangeEnd, exploreRangeStep);
    }
    
    if (showBrainState) {
        explore.exploreBrainState();
    }
    
    if (showLearningDynamics) {
        explore.exploreLearningDynamics();
    }
    
    if (showHistory) {
        explore.printExplorationHistory();
    }
    
    if (!initBrain && exploreParam.empty() && exploreRangeParam.empty() && 
        !showBrainState && !showLearningDynamics && !showHistory) {
        NLMExplore tempExplore;
        tempExplore.printUsage(programName);
    }
    
    return 0;
}
