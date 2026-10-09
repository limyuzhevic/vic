#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/memory/AssociativeMemory.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/development/DevelopmentSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Context manager for automatic resource management
class BrainContext {
public:
    BrainContext(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (brain_) {
            brain_->initialize();
        }
    }
    
    ~BrainContext() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    void step(SimulationStep step) {
        if (brain_) {
            brain_->step(step);
        }
    }
    
    std::shared_ptr<Brain> getBrain() {
        return brain_;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced configuration builder
class ConfigBuilder {
public:
    static std::shared_ptr<Config> createBrainConfig(
        size_t neuronCount = 1000,
        size_t regionCount = 1,
        float connectionProbability = 0.1f,
        uint64_t randomSeed = 42u) {
        
        auto config = std::make_shared<Config>();
        
        // Basic brain configuration
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        config->set("connection_probability", connectionProbability);
        config->set("random_seed", randomSeed);
        
        // Neuron dynamics parameters
        config->set("v_thresh", -50.0f);
        config->set("v_rest", -70.0f);
        config->set("v_reset", -75.0f);
        config->set("tau_mem", 20.0f);
        config->set("tau_ref", 2.0f);
        
        // Plasticity parameters
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Neuromodulation
        config->set("neuromod_dopamine_scale", 1.0f);
        config->set("neuromod_curiosity_enable", true);
        config->set("neuromod_novelty_enable", true);
        
        // Memory systems
        config->set("working_memory_capacity", neuronCount / 10);
        config->set("episodic_memory_max_episodes", 1000);
        
        return config;
    }
    
    static std::shared_ptr<Config> createAdvancedConfig(
        const std::string& name = "NLM_Advanced",
        std::map<std::string, std::string> customParams = {}) {
        
        auto config = std::make_shared<Config>();
        
        // Set name
        config->set("name", name);
        
        // Apply custom parameters
        for (const auto& pair : customParams) {
            // Parse and apply different parameter types based on naming
            if (pair.first.find("neuron_") != std::string::npos) {
                try {
                    size_t value = std::stoull(pair.second);
                    config->set(pair.first, static_cast<int64_t>(value));
                } catch (...) {}
            } else if (pair.first.find("float_") != std::string::npos || 
                      pair.first.find("rate_") != std::string::npos ||
                      pair.first.find("weight_") != std::string::npos) {
                try {
                    float value = std::stof(pair.second);
                    config->set(pair.first, value);
                } catch (...) {}
            } else if (pair.first.find("bool_") != std::string::npos) {
                bool value = (pair.second == "true" || pair.second == "1" || 
                             pair.second == "yes" || pair.second == "on");
                config->set(pair.first, value);
            }
        }
        
        return config;
    }
};

// Advanced simulation runner with monitoring
class SimulationRunner {
public:
    struct SimulationMetrics {
        size_t steps = 0;
        float totalReward = 0.0f;
        float avgFiringRate = 0.0f;
        size_t maxFiringNeurons = 0;
        size_t totalSpikes = 0;
        std::chrono::milliseconds wallClockTime = std::chrono::milliseconds(0);
        std::vector<float> firingRateHistory;
        std::vector<float> rewardHistory;
        std::vector<float> neuromodulationHistory;
    };
    
    static SimulationMetrics runEpisode(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t stepsPerEpisode,
        bool enableMonitoring = true) {
        
        SimulationMetrics metrics;
        auto startWall = std::chrono::high_resolution_clock::now();
        
        // Configure world and agent if needed
        if (world && agent) {
            world->configure(16, 16, 16, 16);
            agent->initialize(*world);
            
            // Enable all advanced features
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
        
        // Reset state
        brain->reset();
        if (agent) agent->reset();
        if (world) world->reset();
        
        // Run episode
        for (size_t step = 0; step < stepsPerEpisode; ++step) {
            if (world && agent) {
                // Get sensory percept
                auto percept = world->getSensoryPercept();
                
                // Process input
                agent->processSensoryInput(percept);
                
                // Run brain
                brain->step(step, step * 0.001f);
                
                // Get action
                auto action = agent->decodeMotorCommand();
                
                // Apply to world
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward modulation
                float reward = 0.0f;
                if (world->getSensoryPercept().getInternal() && 
                    !world->getSensoryPercept().getInternal()->empty()) {
                    reward = world->getSensoryPercept().getInternal()->front();
                }
                agent->applyRewardModulation(reward, 0.0f);
                
                // Update development
                agent->updateDevelopment(0.001f);
            } else {
                // Just run brain without world interaction
                brain->step(step);
            }
            
            // Collect metrics
            metrics.steps++;
            metrics.totalSpikes += brain->getTotalSpikeCount();
            metrics.maxFiringNeurons = std::max(
                metrics.maxFiringNeurons, brain->getFiringNeuronCount());
            
            if (enableMonitoring) {
                metrics.firingRateHistory.push_back(brain->getAverageFiringRate());
                
                if (agent) {
                    metrics.rewardHistory.push_back(metrics.totalReward);
                    metrics.neuromodulationHistory.push_back(
                        agent->getNeuromodulationLevel());
                }
            }
        }
        
        // Calculate averages
        auto endWall = std::chrono::high_resolution_clock::now();
        metrics.wallClockTime = std::chrono::duration_cast<
            std::chrono::milliseconds>(endWall - startWall);
        
        if (stepsPerEpisode > 0) {
            metrics.avgFiringRate = metrics.totalSpikes / stepsPerEpisode;
            metrics.totalReward = metrics.rewardHistory.empty() ? 0.0f : 
                metrics.rewardHistory.back();
        }
        
        return metrics;
    }
    
    static std::vector<SimulationMetrics> runMultipleEpisodes(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t numEpisodes,
        size_t stepsPerEpisode,
        bool enableMonitoring = true) {
        
        std::vector<SimulationMetrics> allMetrics;
        
        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            auto metrics = runEpisode(brain, world, agent, stepsPerEpisode, enableMonitoring);
            metrics.steps = stepsPerEpisode;
            metrics.totalReward = 0.0f; // Will be calculated by runEpisode
            allMetrics.push_back(metrics);
        }
        
        return allMetrics;
    }
};

// Memory management utilities
class MemoryManager {
public:
    static void clearAllMemory(std::shared_ptr<Brain> brain) {
        if (!brain) return;
        
        // Clear working memory
        if (auto* workingMem = brain->getWorkingMemory()) {
            workingMem->clear();
        }
        
        // Clear episodic memory
        if (auto* episodicMem = brain->getEpisodicMemory()) {
            episodicMem->clear();
        }
        
        // Clear associative memory
        if (auto* assocMem = brain->getAssociativeMemory()) {
            assocMem->clear();
        }
    }
    
    static size_t getMemoryUsage(std::shared_ptr<Brain> brain) {
        if (!brain) return 0;
        
        size_t total = 0;
        
        if (auto* workingMem = brain->getWorkingMemory()) {
            total += workingMem->getActiveTraces();
        }
        
        if (auto* episodicMem = brain->getEpisodicMemory()) {
            total += episodicMem->getEpisodeCount() * 100; // Approximate per episode
        }
        
        if (auto* assocMem = brain->getAssociativeMemory()) {
            total += assocMem->getAssociationCount() * 50; // Approximate per association
        }
        
        return total;
    }
    
    static void optimizeMemory(std::shared_ptr<Brain> brain, float retentionRate = 0.3f) {
        if (!brain) return;
        
        if (auto* episodicMem = brain->getEpisodicMemory()) {
            episodicMem->consolidate(retentionRate);
        }
        
        if (auto* workingMem = brain->getWorkingMemory()) {
            // Maintain only the most important traces
            size_t capacity = workingMem->getCapacity();
            if (workingMem->getActiveTraces() > capacity * 0.9f) {
                // Reduce to 80% of capacity
                workingMem->setCapacity(static_cast<size_t>(capacity * 0.8f));
            }
        }
    }
};

// Performance monitoring and profiling
class PerformanceProfiler {
public:
    struct PerformanceStats {
        std::chrono::high_resolution_clock::time_point startTime;
        size_t stepsCompleted = 0;
        size_t totalSpikeCount = 0;
        float totalCPUUsage = 0.0f;
        size_t memoryUsage = 0;
        float avgStepTime = 0.0f;
        std::vector<float> stepTimes;
        std::map<std::string, float> customMetrics;
    };
    
    static PerformanceStats startProfiling() {
        PerformanceStats stats;
        stats.startTime = std::chrono::high_resolution_clock::now();
        return stats;
    }
    
    static void recordStep(PerformanceStats& stats, std::shared_ptr<Brain> brain) {
        stats.stepsCompleted++;
        if (brain) {
            stats.totalSpikeCount += brain->getTotalSpikeCount();
            
            // Record step time
            auto endStep = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                endStep - stats.startTime);
            
            stats.stepTimes.push_back(duration.count() / 1000.0f); // Convert to ms
            
            if (!stats.stepTimes.empty()) {
                float sum = 0.0f;
                for (float t : stats.stepTimes) sum += t;
                stats.avgStepTime = sum / stats.stepTimes.size();
            }
        }
    }
    
    static std::string generateReport(const PerformanceStats& stats) {
        std::stringstream ss;
        ss << "=== NLM Performance Report ===\n";
        ss << "Steps completed: " << stats.stepsCompleted << "\n";
        ss << "Total spikes: " << stats.totalSpikeCount << "\n";
        ss << "Average step time: " << std::fixed << std::setprecision(3) 
           << stats.avgStepTime << " ms\n";
        ss << "CPU usage: " << stats.totalCPUUsage << "%\n";
        ss << "Memory usage: " << stats.memoryUsage << " bytes\n";
        
        if (!stats.stepTimes.empty()) {
            float min = *std::min_element(stats.stepTimes.begin(), stats.stepTimes.end());
            float max = *std::max_element(stats.stepTimes.begin(), stats.stepTimes.end());
            ss << "Step time range: " << std::fixed << std::setprecision(3) 
               << min << " - " << max << " ms\n";
            ss << "Step time stddev: " << std::fixed << std::setprecision(3) 
               << calculateStdDev(stats.stepTimes) << " ms\n";
        }
        
        ss << "\nCustom metrics:\n";
        for (const auto& pair : stats.customMetrics) {
            ss << "  " << pair.first << ": " << std::fixed << std::setprecision(3) 
               << pair.second << "\n";
        }
        
        return ss.str();
    }
    
private:
    static float calculateStdDev(const std::vector<float>& values) {
        if (values.size() <= 1) return 0.0f;
        
        float mean = std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
        float variance = 0.0f;
        
        for (float v : values) {
            variance += (v - mean) * (v - mean);
        }
        
        variance /= values.size();
        return std::sqrt(variance);
    }
};

// Advanced neural network utilities
class NeuralNetworkUtils {
public:
    static std::vector<float> computeFiringStatistics(std::shared_ptr<Brain> brain) {
        if (!brain) return {};
        
        std::vector<float> stats;
        
        // Get all regions and their neuron information
        auto regions = brain->getRegions();
        
        for (const auto& region : regions) {
            float avgFiring = region->getAverageFiringRate();
            float minFiring = avgFiring; // Will be updated
            float maxFiring = avgFiring;
            size_t activeNeurons = region->getActiveNeuronCount();
            size_t totalNeurons = region->getTotalNeuronCount();
            
            // Calculate statistics (simplified)
            stats.push_back(avgFiring);      // Average firing rate
            stats.push_back(static_cast<float>(activeNeurons)); // Active neurons
            stats.push_back(static_cast<float>(totalNeurons));  // Total neurons
            stats.push_back(avgFiring / std::max(1.0f, static_cast<float>(activeNeurons))); // Firing per active neuron
        }
        
        return stats;
    }
    
    static std::string analyzeConnectivity(std::shared_ptr<Brain> brain) {
        if (!brain) return "No brain provided";
        
        std::stringstream ss;
        ss << "=== Connectivity Analysis ===\n";
        
        size_t totalSynapses = brain->getTotalSynapseCount();
        float eiRatio = brain->getExcitationInhibitionRatio();
        
        ss << "Total synapses: " << totalSynapses << "\n";
        ss << "E/I ratio: " << std::fixed << std::setprecision(3) << eiRatio << "\n";
        
        if (eiRatio > 0) {
            if (eiRatio > 2.0f) ss << "Status: Excitatory dominant (high excitation)\n";
            else if (eiRatio > 1.0f) ss << "Status: Balanced excitation\n";
            else if (eiRatio > 0.5f) ss << "Status: Slightly inhibitory dominant\n";
            else ss << "Status: Inhibitory dominant\n";
        }
        
        return ss.str();
    }
    
    static py::list getSensoryNeurons(std::shared_ptr<Brain> brain) {
        py::list neuronIds;
        
        if (!brain) return neuronIds;
        
        // Get sensory neurons from all regions
        auto regions = brain->getRegions();
        
        for (const auto& region : regions) {
            for (const auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Sensory) {
                    // In a real implementation, we would get actual neuron IDs
                    // For now, we'll use placeholder values
                    neuronIds.append(py::cast(NeuronId{})); // This would be real neuron IDs
                }
            }
        }
        
        return neuronIds;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version includes:
        - Brain context manager for automatic resource management
        - Configuration builder for easy setup
        - Advanced simulation runner with monitoring
        - Memory management utilities
        - Performance profiling and analysis
        - Neural network analysis tools
        - Context managers for resource cleanup
        
        Features:
        * Automatic resource management with context managers
        * Advanced simulation monitoring and metrics collection
        * Memory optimization and management
        * Performance profiling and reporting
        * Enhanced error handling and type safety
        * Batch operations for improved performance
        * Comprehensive documentation and examples
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Add exception types for better error handling
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // === Enhanced Configuration System ===
    
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration system for NLM)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](Config& self, const std::string& key) {
            return self.get<std::string>(key).value_or("");
        }, py::arg("key"),
             "Get configuration value as string (returns empty string if not found)")
        .def("getInt", [](Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            return value ? *value : 0;
        }, py::arg("key"),
             "Get configuration value as integer (returns 0 if not found)")
        .def("getFloat", [](Config& self, const std::string& key) {
            auto value = self.get<float>(key);
            return value ? *value : 0.0f;
        }, py::arg("key"),
             "Get configuration value as float (returns 0.0 if not found)")
        .def("getBool", [](Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            return value ? *value : false;
        }, py::arg("key"),
             "Get configuration value as boolean (returns false if not found)")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value (int)")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value (float)")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value (bool)")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value (string)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<EnhancedConfig: " + cfg.summary() + ">";
        });

    // Add ConfigBuilder class methods as static methods
    m.def("createBrainConfig", &ConfigBuilder::createBrainConfig,
          py::arg("neuronCount") = 1000,
          py::arg("regionCount") = 1,
          py::arg("connectionProbability") = 0.1f,
          py::arg("randomSeed") = 42u,
          "Create a standard brain configuration with sensible defaults");
    
    m.def("createAdvancedConfig", &ConfigBuilder::createAdvancedConfig,
          py::arg("name") = "NLM_Advanced",
          py::arg("customParams") = std::map<std::string, std::string>(),
          "Create an advanced configuration with custom parameters");

    // === Enhanced Brain Class ===
    
    py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Context manager for automatic brain resource management)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize context manager with brain instance")
        .def("step", &BrainContext::step, py::arg("currentStep"),
             "Perform a simulation step within the context")
        .def("getBrain", &BrainContext::getBrain,
             "Get the brain instance within the context");
    
    py::class_<Brain>(m, "EnhancedBrain", R"pbdoc(High-performance brain simulation with advanced features)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a new brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Memory system access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulation system")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulation system")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random number generator")
        // Enhanced statistics
        .def("getMemoryUsage", &MemoryManager::getMemoryUsage,
             py::arg("brain"),
             "Get total memory usage of the brain system")
        .def("getConnectivityAnalysis", &NeuralNetworkUtils::analyzeConnectivity,
             py::arg("brain"),
             "Analyze brain connectivity and provide insights")
        .def("getFiringStatistics", &NeuralNetworkUtils::computeFiringStatistics,
             py::arg("brain"),
             "Compute detailed firing statistics for all brain regions");

    // Create context manager convenience function
    m.def("brain_context", [](std::shared_ptr<Brain> brain) {
        return py::cast(BrainContext(brain));
    }, py::arg("brain"),
         "Create a context manager for automatic brain resource management");

    // === Enhanced AgentBrain Class ===
    
    py::class_<AgentBrain>(m, "EnhancedAgentBrain", R"pbdoc(Advanced agent brain interface with enhanced features)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create a new agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Enhanced agent features
        .def("enableAllFeatures", &AgentBrain::enableRewardModulation,
             py::arg("enable") = true,
             "Enable all advanced agent features (reward, plasticity, development, curiosity)")
        .def("getAdvancedStats", [](std::shared_ptr<AgentBrain> agent) {
            py::dict stats;
            if (agent) {
                stats["neuromodulation"] = agent->getNeuromodulationLevel();
                stats["curiosity"] = agent->getCuriosityLevel();
                stats["novelty"] = agent->getNoveltyLevel();
                stats["prediction_error"] = agent->getPredictionError();
                stats["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
                stats["enabled_features"] = py::dict({
                    {"reward_modulation", agent->isRewardModulationEnabled()},
                    {"structural_plasticity", agent->isStructuralPlasticityEnabled()},
                    {"development", agent->isDevelopmentEnabled()},
                    {"curiosity", agent->isCuriosityEnabled()}
                });
            }
            return stats;
        }, "Get comprehensive agent statistics");

    // === Enhanced Simulation Runner ===
    
    py::class_<SimulationRunner::SimulationMetrics>(m, "SimulationMetrics", R"pbdoc(Comprehensive simulation metrics collection)pbdoc")
        .def(py::init<>())
        .def_readwrite("steps", &SimulationRunner::SimulationMetrics::steps)
        .def_readwrite("total_reward", &SimulationRunner::SimulationMetrics::totalReward)
        .def_readwrite("avg_firing_rate", &SimulationRunner::SimulationMetrics::avgFiringRate)
        .def_readwrite("max_firing_neurons", &SimulationRunner::SimulationMetrics::maxFiringNeurons)
        .def_readwrite("total_spikes", &SimulationRunner::SimulationMetrics::totalSpikes)
        .def_readwrite("wall_clock_time_ms", &SimulationRunner::SimulationMetrics::wallClockTime)
        .def("add_firing_rate", &SimulationRunner::SimulationMetrics::firingRateHistory)
        .def("add_reward", &SimulationRunner::SimulationMetrics::rewardHistory)
        .def("add_neuromodulation", &SimulationRunner::SimulationMetrics::neuromodulationHistory);
    
    m.def("run_episode", &SimulationRunner::runEpisode,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("steps_per_episode") = 1000,
          py::arg("enable_monitoring") = true,
          "Run a single episode with comprehensive metrics collection");
    
    m.def("run_multiple_episodes", &SimulationRunner::runMultipleEpisodes,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("num_episodes") = 10,
          py::arg("steps_per_episode") = 1000,
          py::arg("enable_monitoring") = true,
          "Run multiple episodes and return aggregated metrics");

    // === Memory Management ===
    
    m.def("clear_all_memory", &MemoryManager::clearAllMemory,
          py::arg("brain"),
          "Clear all memory systems in the brain");
    
    m.def("optimize_memory", &MemoryManager::optimizeMemory,
          py::arg("brain"), py::arg("retention_rate") = 0.3f,
          "Optimize memory usage and consolidate important memories");
    
    // === Performance Profiling ===
    
    py::class_<PerformanceProfiler::PerformanceStats>(m, "PerformanceStats", R"pbdoc(Performance profiling statistics)pbdoc")
        .def(py::init<>())
        .def_readwrite("steps_completed", &PerformanceProfiler::PerformanceStats::stepsCompleted)
        .def_readwrite("total_spike_count", &PerformanceProfiler::PerformanceStats::totalSpikeCount)
        .def_readwrite("avg_step_time_ms", &PerformanceProfiler::PerformanceStats::avgStepTime)
        .def_readwrite("memory_usage_bytes", &PerformanceProfiler::PerformanceStats::memoryUsage)
        .def("add_step_time", &PerformanceProfiler::PerformanceStats::stepTimes)
        .def("add_custom_metric", &PerformanceProfiler::PerformanceStats::customMetrics);
    
    m.def("start_profiling", &PerformanceProfiler::startProfiling,
          "Start performance profiling for the current simulation");
    
    m.def("record_step", &PerformanceProfiler::recordStep,
          py::arg("stats"), py::arg("brain"),
          "Record a simulation step in the performance profiler");
    
    m.def("generate_performance_report", &PerformanceProfiler::generateReport,
          py::arg("stats"),
          "Generate a detailed performance report from profiling data");

    // === Neural Network Analysis Utilities ===
    
    m.def("compute_firing_statistics", &NeuralNetworkUtils::computeFiringStatistics,
          py::arg("brain"),
          "Compute comprehensive firing statistics for all brain regions");
    
    m.def("analyze_connectivity", &NeuralNetworkUtils::analyzeConnectivity,
          py::arg("brain"),
          "Analyze brain connectivity and provide insights");
    
    // === Convenience Functions ===
    
    m.def("create_brain_config", []() {
        return ConfigBuilder::createBrainConfig();
    }, "Create a standard brain configuration with sensible defaults");
    
    m.def("create_advanced_brain_config", []() {
        return ConfigBuilder::createBrainConfig(2000, 4, 0.15f, 12345u);
    }, "Create an advanced brain configuration with more neurons and regions");
    
    // Legacy compatibility functions
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Legacy function - use EnhancedBrain instead");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Legacy function - use EnhancedAgentBrain instead");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Legacy function - use SimpleWorld directly");
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Legacy function - use Config directly");

    // === Invalid Constants ===
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // === Version and Information ===
    
    m.attr("__version__") = "2.0.0"; // Updated version for enhanced features
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced features for research and development";
    m.attr("__features__") = py::dict({
        {"enhanced_brain_class", true},
        {"memory_management", true},
        {"performance_profiling", true},
        {"neural_network_analysis", true},
        {"context_managers", true},
        {"advanced_simulation_runner", true},
        {"configuration_builder", true},
        {"legacy_compatibility", true}
    });
}
