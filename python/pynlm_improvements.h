#ifndef PYNLM_IMPROVEMENTS_H
#define PYNLM_IMPROVEMENTS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <map>
#include <functional>

// Forward declarations
namespace py = pybind11;

namespace nlm {
    // Forward declarations for improved bindings
    class Brain;
    class Config;
    class AgentBrain;
    class SimpleWorld;
    class SensoryInput;
    class Action;
    class AgentBody;
    class SensoryPercept;
}

// Context manager for Brain simulation
class BrainSimulationManager {
private:
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::AgentBrain> agent;
    std::shared_ptr<nlm::SimpleWorld> world;
    bool auto_reset;
    size_t steps_per_iteration;
    
public:
    BrainSimulationManager(std::shared_ptr<nlm::Brain> b, 
                          std::shared_ptr<nlm::AgentBrain> a,
                          std::shared_ptr<nlm::SimpleWorld> w,
                          bool autoReset = true,
                          size_t steps = 1)
        : brain(b), agent(a), world(w), auto_reset(autoReset), steps_per_iteration(steps) {
        
        // Initialize if needed
        if (brain && !brain->getTotalNeuronCount()) {
            brain->initialize();
        }
        if (agent && world) {
            agent->initialize(world);
        }
    }
    
    ~BrainSimulationManager() {
        if (auto_reset && brain) {
            brain->reset();
        }
    }
    
    // Simulate one iteration
    void simulate() {
        if (!brain || !agent || !world) return;
        
        for (size_t i = 0; i < steps_per_iteration; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, world->getSimulationTime());
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }
    
    // Simulate multiple iterations
    void simulate(size_t iterations) {
        for (size_t i = 0; i < iterations; ++i) {
            simulate();
        }
    }
    
    // Get current statistics
    py::dict getStats() const {
        py::dict stats;
        if (brain) {
            stats["neuron_count"] = brain->getTotalNeuronCount();
            stats["synapse_count"] = brain->getTotalSynapseCount();
            stats["firing_count"] = brain->getFiringNeuronCount();
            stats["spike_count"] = brain->getTotalSpikeCount();
            stats["avg_firing_rate"] = brain->getAverageFiringRate();
            stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        }
        if (agent) {
            stats["curiosity"] = agent->getCuriosityLevel();
            stats["novelty"] = agent->getNoveltyLevel();
            stats["prediction_error"] = agent->getPredictionError();
            stats["neuromodulation"] = agent->getNeuromodulationLevel();
        }
        if (world) {
            stats["world_time"] = world->getSimulationTime();
            stats["world_width"] = world->getWidth();
            stats["world_height"] = world->getHeight();
        }
        return stats;
    }
    
    // Get brain reference for advanced operations
    std::shared_ptr<nlm::Brain> getBrain() { return brain; }
    std::shared_ptr<nlm::AgentBrain> getAgent() { return agent; }
    std::shared_ptr<nlm::SimpleWorld> getWorld() { return world; }
};

// Exception types for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message) : NLMError("Simulation Error: " + message) {}
};

// Enhanced configuration utilities
class AdvancedConfig {
private:
    std::shared_ptr<nlm::Config> config;
    
public:
    explicit AdvancedConfig(std::shared_ptr<nlm::Config> cfg) : config(cfg) {}
    
    // Apply preset configurations
    static std::shared_ptr<nlm::Config> createMiniBrain() {
        auto cfg = std::make_shared<nlm::Config>();
        cfg->set("brain.neuron_count", 100);
        cfg->set("brain.synapse_density", 0.05);
        cfg->set("brain.v_thresh", -50.0);
        cfg->set("plasticity.stdp.enable", true);
        cfg->set("neuromod.dopamine.scale", 1.0);
        return cfg;
    }
    
    static std::shared_ptr<nlm::Config> createStandardBrain() {
        auto cfg = std::make_shared<nlm::Config>();
        cfg->set("brain.neuron_count", 1000);
        cfg->set("brain.synapse_density", 0.1);
        cfg->set("plasticity.stdp.enable", true);
        cfg->set("plasticity.hebbian.enable", true);
        cfg->set("neuromod.curiosity.enable", true);
        cfg->set("neuromod.novelty.enable", true);
        cfg->set("development.enable", true);
        return cfg;
    }
    
    static std::shared_ptr<nlm::Config> createAdvancedBrain() {
        auto cfg = std::make_shared<nlm::Config>();
        cfg->set("brain.neuron_count", 5000);
        cfg->set("brain.synapse_density", 0.15);
        cfg->set("plasticity.stdp.enable", true);
        cfg->set("plasticity.hebbian.enable", true);
        cfg->set("plasticity.structural.enable", true);
        cfg->set("neuromod.dopamine.scale", 1.5);
        cfg->set("neuromod.curiosity.enable", true);
        cfg->set("neuromod.novelty.enable", true);
        cfg->set("neuromod.prediction.error.enabled", true);
        cfg->set("development.enable", true);
        cfg->set("development.synaptogenesis", 1.0);
        return cfg;
    }
    
    // Apply configuration validation
    bool validate(std::map<std::string, std::string>& errors) {
        if (!config) return true;
        
        errors.clear();
        
        // Check neuron count
        auto neuronCount = config->get<int>("brain.neuron_count");
        if (neuronCount && (*neuronCount < 10 || *neuronCount > 100000)) {
            errors["neuron_count"] = "Neuron count must be between 10 and 100,000";
        }
        
        // Check synapse density
        auto synapseDensity = config->get<double>("brain.synapse_density");
        if (synapseDensity && (*synapseDensity < 0.0 || *synapseDensity > 1.0)) {
            errors["synapse_density"] = "Synapse density must be between 0.0 and 1.0";
        }
        
        // Check consistency
        if (config->has("plasticity.stdp.enable") && 
            config->has("plasticity.hebbian.enable") &&
            config->get<bool>("plasticity.stdp.enable") &&
            config->get<bool>("plasticity.hebbian.enable")) {
            // Both enabled, fine
        }
        
        return errors.empty();
    }
    
    // Merge configurations
    static std::shared_ptr<nlm::Config> mergeConfigs(const std::vector<std::shared_ptr<nlm::Config>>& configs) {
        auto result = std::make_shared<nlm::Config>();
        for (const auto& cfg : configs) {
            auto keys = cfg->getKeys();
            for (const auto& key : keys) {
                if (cfg->has(key)) {
                    auto value = cfg->get<nlm::ConfigValue>(key);
                    // Use runtime source for merged values
                    result->set(key, value, nlm::ConfigSource::Runtime);
                }
            }
        }
        return result;
    }
    
    // Export configuration to Python-friendly format
    py::dict exportToDict() const {
        py::dict result;
        if (!config) return result;
        
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            // Try to get appropriate type
            if (config->has(key)) {
                // For simplicity, convert to string representation
                result[key] = config->get<nlm::ConfigValue>(key);
            }
        }
        return result;
    }
};

// Performance monitoring utilities
class PerformanceMonitor {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    size_t steps_completed;
    double total_time;
    std::vector<double> step_times;
    
public:
    PerformanceMonitor() : steps_completed(0), total_time(0.0) {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    void recordStep(double step_time) {
        steps_completed++;
        total_time += step_time;
        step_times.push_back(step_time);
    }
    
    py::dict getStats() const {
        py::dict stats;
        
        if (step_times.empty()) {
            return stats;
        }
        
        double avg_step_time = total_time / steps_completed;
        double min_step_time = *std::min_element(step_times.begin(), step_times.end());
        double max_step_time = *std::max_element(step_times.begin(), step_times.end());
        
        stats["steps_completed"] = steps_completed;
        stats["total_time"] = total_time;
        stats["average_step_time_ms"] = avg_step_time * 1000.0;
        stats["min_step_time_ms"] = min_step_time * 1000.0;
        stats["max_step_time_ms"] = max_step_time * 1000.0;
        stats["steps_per_second"] = 1.0 / avg_step_time;
        
        // Performance analysis
        if (step_times.size() >= 10) {
            double variance = 0.0;
            for (double t : step_times) {
                variance += (t - avg_step_time) * (t - avg_step_time);
            }
            variance /= step_times.size();
            double std_dev = std::sqrt(variance);
            stats["step_time_std_dev_ms"] = std_dev * 1000.0;
            
            // Detect performance degradation (more than 20% variance)
            double cv = std_dev / avg_step_time; // Coefficient of variation
            stats["coefficient_of_variation"] = cv;
            stats["performance_stable"] = cv < 0.2;
        }
        
        return stats;
    }
    
    void reset() {
        start_time = std::chrono::high_resolution_clock::now();
        steps_completed = 0;
        total_time = 0.0;
        step_times.clear();
    }
    
    double getElapsedTime() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - start_time);
        return duration.count();
    }
};

// Neural network visualization utilities (placeholder for advanced features)
class VisualizationHelper {
public:
    // Generate simple network statistics
    static py::dict analyzeNetwork(const std::shared_ptr<nlm::Brain>& brain) {
        py::dict analysis;
        if (!brain) return analysis;
        
        size_t total_neurons = brain->getTotalNeuronCount();
        size_t total_synapses = brain->getTotalSynapseCount();
        size_t firing_neurons = brain->getFiringNeuronCount();
        size_t spike_count = brain->getTotalSpikeCount();
        
        analysis["total_neurons"] = total_neurons;
        analysis["total_synapses"] = total_synapses;
        analysis["connectivity_density"] = total_neurons > 0 ? 
            static_cast<double>(total_synapses) / (total_neurons * total_neurons) : 0.0;
        
        if (total_neurons > 0) {
            analysis["firing_percentage"] = static_cast<double>(firing_neurons) / total_neurons * 100.0;
            analysis["spike_per_neuron"] = static_cast<double>(spike_count) / total_neurons;
        }
        
        // Estimate network size in memory (rough approximation)
        size_t memory_estimate = total_neurons * 64 + // Rough estimate per neuron
                                total_synapses * 32 +     // Rough estimate per synapse
                                sizeof(nlm::Brain);
        
        analysis["estimated_memory_kb"] = memory_estimate / 1024;
        
        return analysis;
    }
    
    // Generate connection statistics (placeholder for advanced visualization)
    static py::dict getConnectionStats(const std::shared_ptr<nlm::Brain>& brain) {
        py::dict stats;
        if (!brain) return stats;
        
        // Placeholder - in a real implementation, this would analyze
        // the actual neural connectivity patterns
        stats["connection_analysis"] = "Feature requires advanced connectivity data structures";
        stats["enable_detailed_analysis"] = false;
        
        return stats;
    }
};

// Experimental features for advanced users
namespace experimental {
    // Advanced synaptic manipulation
    class SynapticManipulation {
    public:
        static void strengthenConnections(std::shared_ptr<nlm::Brain> brain, 
                                         double strength_factor = 1.5,
                                         nlm::NeuronType target_type = nlm::NeuronType::Excitatory) {
            // This would require access to internal synapse management
            // Placeholder for advanced synaptic control
        }
        
        static void createPattern(std::shared_ptr<nlm::Brain> brain,
                                  const std::vector<nlm::NeuronId>& pattern,
                                  bool synchronous = false) {
            // Create specific neural patterns
            // Placeholder for advanced pattern creation
        }
    };
    
    // Custom plasticity rules
    class CustomPlasticity {
    public:
        using PlasticityRule = std::function<double(double, double, double, double)>;
        
        static void applyCustomSTDP(std::shared_ptr<nlm::Brain> brain,
                                   PlasticityRule pre_rule,
                                   PlasticityRule post_rule) {
            // Apply custom STDP rules
            // Placeholder for advanced plasticity customization
        }
    };
    
    // Advanced memory manipulation
    class AdvancedMemory {
    public:
        static void replayExperience(std::shared_ptr<nlm::Brain> brain,
                                     const std::vector<float>& pattern,
                                     size_t replay_iterations = 10,
                                     double replay_strength = 1.0) {
            // Replay specific memory patterns
            // Placeholder for advanced memory replay
        }
    };
}

#endif // PYNLM_IMPROVEMENTS_H