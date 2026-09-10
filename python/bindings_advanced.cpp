// Advanced Python API Extensions for NLM
// Enhanced Python bindings for expert users

#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/experiments/Phase6Demo.hpp"

namespace py = pybind11;
namespace nlm {

// Batch processing capabilities for large-scale simulations
class AdvancedBrainOperations {
public:
    static void batchStep(Brain& brain, size_t steps, size_t logInterval = 100) {
        for (size_t step = 0; step < steps; ++step) {
            brain.step(step);
            if (logInterval > 0 && step % logInterval == 0) {
                NLM_LOG_INFO("Batch step " + std::to_string(step) + "/" + std::to_string(steps));
            }
        }
    }
    
    static void batchStepWithTime(Brain& brain, size_t steps, double timestep, 
                                 size_t logInterval = 100) {
        for (size_t step = 0; step < steps; ++step) {
            double time = step * timestep;
            brain.step(step, time);
            if (logInterval > 0 && step % logInterval == 0) {
                NLM_LOG_INFO("Batch step " + std::to_string(step) + "/" + std::to_string(steps) + 
                           " time=" + std::to_string(time) + "s");
            }
        }
    }
    
    static void batchProcessSequence(std::shared_ptr<Brain> brain, 
                                    const std::vector<std::shared_ptr<SensoryInput>>& inputs,
                                    const std::vector<ActionType>& expectedActions = {}) {
        for (size_t i = 0; i < inputs.size(); ++i) {
            brain->receiveSensoryInput(*inputs[i]);
            brain->step(i);
            
            if (!expectedActions.empty() && i < expectedActions.size()) {
                auto action = brain->produceAction();
                // Could add validation logic here
            }
        }
    }
};

// Performance monitoring for expert users
class PerformanceMonitor {
public:
    struct Metrics {
        double totalSimulationTime;
        size_t totalStepsExecuted;
        size_t totalSpikesGenerated;
        double averageFiringRate;
        double memoryUsage;
        double cpuUsage;
        size_t checkpointCount;
        
        Metrics() : totalSimulationTime(0.0), totalStepsExecuted(0), 
                   totalSpikesGenerated(0), averageFiringRate(0.0),
                   memoryUsage(0.0), cpuUsage(0.0), checkpointCount(0) {}
    };
    
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    void startMonitoring() {
        startTime = std::chrono::high_resolution_clock::now();
        currentStep = 0;
        totalSpikes = 0;
    }
    
    void recordStep(Brain& brain) {
        ++currentStep;
        totalSpikes += brain.getTotalSpikeCount();
    }
    
    Metrics stopMonitoring() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        Metrics metrics;
        metrics.totalSimulationTime = duration.count() / 1000.0;
        metrics.totalStepsExecuted = currentStep;
        metrics.totalSpikesGenerated = totalSpikes;
        metrics.averageFiringRate = (currentStep > 0) ? 
            static_cast<double>(totalSpikes) / currentStep : 0.0;
        
        return metrics;
    }
    
    void logMetrics(const Metrics& metrics, const std::string& label = "Performance") {
        NLM_LOG_INFO(label + " Metrics:");
        NLM_LOG_INFO("  Total simulation time: " + std::to_string(metrics.totalSimulationTime) + "s");
        NLM_LOG_INFO("  Total steps: " + std::to_string(metrics.totalStepsExecuted));
        NLM_LOG_INFO("  Total spikes: " + std::to_string(metrics.totalSpikesGenerated));
        NLM_LOG_INFO("  Average firing rate: " + std::to_string(metrics.averageFiringRate) + " Hz");
        NLM_LOG_INFO("  Memory usage: " + std::to_string(metrics.memoryUsage) + " MB");
        NLM_LOG_INFO("  CPU usage: " + std::to_string(metrics.cpuUsage) + "%");
        NLM_LOG_INFO("  Checkpoints: " + std::to_string(metrics.checkpointCount));
    }
    
private:
    PerformanceMonitor() {}
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    size_t currentStep;
    size_t totalSpikes;
};

// Advanced configuration management for expert users
class AdvancedConfig {
public:
    static std::shared_ptr<Config> optimizeForPerformance(std::shared_ptr<Config> config) {
        // Optimize configuration for high-performance simulations
        config->set("brain.neuron_count", 5000, ConfigSource::Runtime);
        config->set("brain.synapse_density", 0.2f, ConfigSource::Runtime);
        config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Runtime);
        config->set("neuromod.dopamine.scale", 1.5f, ConfigSource::Runtime);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
        config->set("neuromod.novelty.enable", true, ConfigSource::Runtime);
        config->set("development.synaptogenesis_rate", 0.002f, ConfigSource::Runtime);
        
        return config;
    }
    
    static std::shared_ptr<Config> optimizeForLearning(std::shared_ptr<Config> config) {
        // Optimize configuration for maximum learning capability
        config->set("brain.neuron_count", 2000, ConfigSource::Runtime);
        config->set("brain.synapse_density", 0.3f, ConfigSource::Runtime);
        config->set("plasticity.stdp.learning_rate", 0.005f, ConfigSource::Runtime);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
        config->set("neuromod.dopamine.scale", 2.0f, ConfigSource::Runtime);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
        config->set("neuromod.novelty.enable", true, ConfigSource::Runtime);
        config->set("development.synaptogenesis_rate", 0.005f, ConfigSource::Runtime);
        
        return config;
    }
    
    static std::shared_ptr<Config> validateSafetyConstraints(std::shared_ptr<Config> config) {
        // Validate configuration meets safety constraints
        auto neuronCount = config->getOr<size_t>("brain.neuron_count", 1000);
        if (neuronCount > 10000) {
            NLM_LOG_WARNING("Neuron count exceeds safe limit (10000)");
            config->set("brain.neuron_count", 10000, ConfigSource::Runtime);
        }
        
        auto connectionProb = config->getOr<float>("connection_probability", 0.1f);
        if (connectionProb > 0.5) {
            NLM_LOG_WARNING("Connection probability exceeds safe limit (0.5)");
            config->set("connection_probability", 0.5f, ConfigSource::Runtime);
        }
        
        return config;
    }
    
    static std::string getOptimizationRecommendations(std::shared_ptr<Config> config) {
        std::ostringstream oss;
        oss << "Optimization Recommendations:" << std::endl;
        
        auto neuronCount = config->getOr<size_t>("brain.neuron_count", 1000);
        if (neuronCount < 1000) {
            oss << "  - Increase neuron count for better performance (currently " << neuronCount << ")" << std::endl;
        }
        
        auto stdpRate = config->getOr<float>("plasticity.stdp.learning_rate", 0.001f);
        if (stdpRate < 0.001f) {
            oss << "  - Increase STDP learning rate for better learning (currently " << stdpRate << ")" << std::endl;
        }
        
        auto dopamineScale = config->getOr<float>("neuromod.dopamine.scale", 1.0f);
        if (dopamineScale < 1.0f) {
            oss << "  - Increase dopamine scale for better reward processing (currently " << dopamineScale << ")" << std::endl;
        }
        
        return oss.str();
    }
};

// Batch checkpoint operations for expert users
class BatchCheckpointManager {
public:
    static bool batchSave(std::shared_ptr<Brain> brain, 
                         const std::vector<std::string>& filepaths) {
        bool allSuccess = true;
        for (const auto& filepath : filepaths) {
            if (!brain->save(filepath)) {
                NLM_LOG_ERROR("Failed to save checkpoint: " + filepath);
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    static bool batchLoad(std::shared_ptr<Brain> brain,
                         const std::vector<std::string>& filepaths) {
        bool allSuccess = true;
        for (const auto& filepath : filepaths) {
            if (!brain->load(filepath)) {
                NLM_LOG_ERROR("Failed to load checkpoint: " + filepath);
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    static std::vector<std::string> createCheckpointNames(const std::string& prefix,
                                                          size_t startIndex,
                                                          size_t count) {
        std::vector<std::string> names;
        for (size_t i = 0; i < count; ++i) {
            names.push_back(prefix + "_step" + std::to_string(startIndex + i) + ".chk");
        }
        return names;
    }
};

// Memory profiling for expert users
class MemoryProfiler {
public:
    struct MemoryStats {
        size_t totalMemoryUsage;
        size_t allocatedNeurons;
        size_t allocatedSynapses;
        size_t memoryPoolUsage;
        double fragmentationRatio;
        
        MemoryStats() : totalMemoryUsage(0), allocatedNeurons(0), 
                       allocatedSynapses(0), memoryPoolUsage(0),
                       fragmentationRatio(0.0) {}
    };
    
    static MemoryStats profileMemoryUsage(std::shared_ptr<Brain> brain) {
        MemoryStats stats;
        
        // Estimate neuron memory usage
        stats.allocatedNeurons = brain->getTotalNeuronCount() * sizeof(Neuron);
        
        // Estimate synapse memory usage  
        stats.allocatedSynapses = brain->getTotalSynapseCount() * sizeof(Synapse);
        
        // Estimate total memory
        stats.totalMemoryUsage = stats.allocatedNeurons + stats.allocatedSynapses;
        
        // Estimate memory pool usage (placeholder)
        stats.memoryPoolUsage = stats.totalMemoryUsage * 0.2; // Assume 20% overhead
        
        // Calculate fragmentation ratio
        if (stats.totalMemoryUsage > 0) {
            stats.fragmentationRatio = static_cast<double>(stats.memoryPoolUsage) / 
                                      stats.totalMemoryUsage;
        }
        
        NLM_LOG_INFO("Memory Profile:");
        NLM_LOG_INFO("  Total neurons: " + std::to_string(stats.allocatedNeurons));
        NLM_LOG_INFO("  Total synapses: " + std::to_string(stats.allocatedSynapses));
        NLM_LOG_INFO("  Total memory: " + std::to_string(stats.totalMemoryUsage) + " bytes");
        NLM_LOG_INFO("  Memory pool usage: " + std::to_string(stats.memoryPoolUsage) + " bytes");
        NLM_LOG_INFO("  Fragmentation ratio: " + std::to_string(stats.fragmentationRatio));
        
        return stats;
    }
};

// Advanced Python bindings module
PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        Advanced NLM Python API
        ------------------------
        Enhanced Python bindings for expert users with batch processing,
        performance monitoring, and advanced configuration management.
    )pbdoc";
    
    // Advanced brain operations
    m.def("batch_step", &AdvancedBrainOperations::batchStep,
          py::arg("brain"), py::arg("steps"), py::arg("log_interval") = 100,
          "Execute multiple simulation steps efficiently");
    
    m.def("batch_step_with_time", &AdvancedBrainOperations::batchStepWithTime,
          py::arg("brain"), py::arg("steps"), py::arg("timestep"), py::arg("log_interval") = 100,
          "Execute multiple simulation steps with time tracking");
    
    m.def("batch_process_sequence", &AdvancedBrainOperations::batchProcessSequence,
          py::arg("brain"), py::arg("inputs"), py::arg("expected_actions") = py::list(),
          "Process a sequence of sensory inputs and actions");
    
    // Performance monitoring
    m.def("start_performance_monitoring", &PerformanceMonitor::getInstance",
          "Start performance monitoring for the current simulation");
    
    m.def("record_step", &PerformanceMonitor::getInstance",
          "Record a simulation step for performance tracking");
    
    m.def("stop_performance_monitoring", &PerformanceMonitor::getInstance",
          "Stop performance monitoring and return metrics");
    
    m.def("log_performance_metrics", &PerformanceMonitor::getInstance",
          py::arg("metrics"), py::arg("label") = "Performance",
          "Log performance metrics");
    
    // Advanced configuration management
    m.def("optimize_for_performance", &AdvancedConfig::optimizeForPerformance,
          py::arg("config"), "Optimize configuration for performance");
    
    m.def("optimize_for_learning", &AdvancedConfig::optimizeForLearning,
          py::arg("config"), "Optimize configuration for learning");
    
    m.def("validate_safety_constraints", &AdvancedConfig::validateSafetyConstraints,
          py::arg("config"), "Validate configuration against safety constraints");
    
    m.def("get_optimization_recommendations", &AdvancedConfig::getOptimizationRecommendations,
          py::arg("config"), "Get optimization recommendations for configuration");
    
    // Batch checkpoint operations
    m.def("batch_save_checkpoints", &BatchCheckpointManager::batchSave,
          py::arg("brain"), py::arg("filepaths"), "Save multiple checkpoints");
    
    m.def("batch_load_checkpoints", &BatchCheckpointManager::batchLoad,
          py::arg("brain"), py::arg("filepaths"), "Load multiple checkpoints");
    
    m.def("create_checkpoint_names", &BatchCheckpointManager::createCheckpointNames,
          py::arg("prefix"), py::arg("start_index"), py::arg("count"),
          "Create a series of checkpoint filenames");
    
    // Memory profiling
    m.def("profile_memory_usage", &MemoryProfiler::profileMemoryUsage,
          py::arg("brain"), "Profile memory usage of the brain");
    
    // Advanced agent operations
    m.def("create_advanced_agent", [](std::shared_ptr<Brain> brain, 
                                     std::shared_ptr<SimpleWorld> world,
                                     bool enableRewardModulation,
                                     bool enableCuriosity,
                                     bool enableDevelopment,
                                     bool enableStructuralPlasticity) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        if (enableRewardModulation) agent->enableRewardModulation(true);
        if (enableCuriosity) agent->enableCuriosity(true);
        if (enableDevelopment) agent->enableDevelopment(true);
        if (enableStructuralPlasticity) agent->enableStructuralPlasticity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"), 
           py::arg("enable_reward_modulation") = true,
           py::arg("enable_curiosity") = true,
           py::arg("enable_development") = true,
           py::arg("enable_structural_plasticity") = true,
           "Create an advanced agent with enabled subsystems");
    
    // Advanced simulation utilities
    m.def("create_experiment_environment", [](size_t numAgents, size_t worldSize, 
                                            const std::string& type) {
        // Create a complex experimental environment
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldSize, worldSize, 8, 8);
        world->setRandomSeed(std::time(nullptr));
        
        // Create multiple agents
        std::vector<std::shared_ptr<AgentBrain>> agents;
        for (size_t i = 0; i < numAgents; ++i) {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(world);
            agents.push_back(agent);
        }
        
        return std::make_tuple(world, agents);
    }, py::arg("num_agents") = 5, py::arg("world_size") = 20, py::arg("type") = "simple",
       "Create an advanced experimental environment with multiple agents");
    
    // Utility functions for advanced users
    m.def("simulate_episode", [](std::shared_ptr<Brain> brain,
                               std::shared_ptr<SimpleWorld> world,
                               std::shared_ptr<AgentBrain> agent,
                               size_t maxSteps,
                               bool verbose) {
        for (size_t step = 0; step < maxSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << "Firing: " << brain->getFiringNeuronCount()
                         << ", Total spikes: " << brain->getTotalSpikeCount()
                         << std::endl;
            }
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
           py::arg("max_steps") = 1000, py::arg("verbose") = false,
           "Run a complete simulation episode");
}

} // namespace nlm