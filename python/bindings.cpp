// Python bindings for NLM (Neural Learning Machine)
// Comprehensive API for neural simulation, configuration, and experiment management
//
// Usage:
//   import nlm
//   brain = nlm.Brain()
//   brain.initialize()
//   result = brain.run_experiment(1000)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "performance/CheckpointSystem.hpp"

namespace py = pybind11;

namespace nlm_py {

// Wrapper for Config class
class ConfigWrapper {
public:
    std::shared_ptr<nlm::Config> config;
    
    ConfigWrapper() : config(std::make_shared<nlm::Config>()) {
        nlm::Config::initializeSchema();
    }
    
    ConfigWrapper(const std::string& filepath) {
        config = std::make_shared<nlm::Config>();
        config->loadFromFile(filepath);
        nlm::Config::initializeSchema();
    }
    
    // Set values with type conversion
    void set(const std::string& key, int value) {
        config->set(key, value);
    }
    
    void set(const std::string& key, int64_t value) {
        config->set(key, value);
    }
    
    void set(const std::string& key, double value) {
        config->set(key, value);
    }
    
    void set(const std::string& key, bool value) {
        config->set(key, value);
    }
    
    void set(const std::string& key, const std::string& value) {
        config->set(key, value);
    }
    
    // Get values
    int get_int(const std::string& key) const {
        auto val = config->get<int>(key);
        return val ? *val : 0;
    }
    
    int64_t get_int64(const std::string& key) const {
        auto val = config->get<int64_t>(key);
        return val ? *val : 0;
    }
    
    double get_double(const std::string& key) const {
        auto val = config->get<double>(key);
        return val ? *val : 0.0;
    }
    
    bool get_bool(const std::string& key) const {
        auto val = config->get<bool>(key);
        return val ? *val : false;
    }
    
    std::string get_string(const std::string& key) const {
        auto val = config->get<std::string>(key);
        return val ? *val : "";
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config->has(key);
    }
    
    // Get all keys
    std::vector<std::string> get_keys() const {
        return config->getKeys();
    }
    
    // Get configuration summary
    std::string summary() const {
        return config->summary();
    }
    
    // Load from JSON
    bool load_from_json(const std::string& json_string) {
        return config->loadFromJSON(json_string);
    }
    
    // Load from YAML
    bool load_from_yaml(const std::string& yaml_string) {
        return config->loadFromYAML(yaml_string);
    }
    
    // Save to JSON
    std::string to_json() const {
        return config->toJSON();
    }
    
    // Save to YAML
    std::string to_yaml() const {
        return config->toYAML();
    }
    
    // Validate configuration
    py::dict validate() {
        auto result = config->validateAll();
        py::dict py_result;
        py_result["valid"] = result.valid;
        py_result["error"] = result.errorMessage;
        py_result["warnings"] = result.warnings;
        return py_result;
    }
};

// Wrapper for Brain class
class BrainWrapper {
public:
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::AgentBrain> agent;
    std::shared_ptr<nlm::SimpleWorld> world;
    
    BrainWrapper() {
        auto config = std::make_shared<nlm::Config>();
        // Set sensible defaults
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("random_seed", 42);
        
        brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        world = std::make_shared<nlm::SimpleWorld>();
        world->initialize(16, 16);
        
        agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
    }
    
    BrainWrapper(const ConfigWrapper& config_wrapper) {
        brain = std::make_shared<nlm::Brain>(config_wrapper.config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain with given config");
        }
        
        world = std::make_shared<nlm::SimpleWorld>();
        world->initialize(16, 16);
        
        agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
    }
    
    // Run simulation for specified number of steps
    py::dict run_steps(size_t steps) {
        py::dict result;
        
        float total_reward = 0.0f;
        float total_firing_rate = 0.0f;
        size_t firing_count = 0;
        
        for (size_t i = 0; i < steps; ++i) {
            // Get observation
            auto percept = world->observe(brain->getRegions()[0].get());
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(i, i * 0.001);
            
            // Get motor command
            auto cmd = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyAction(brain->getRegions()[0].get(), cmd);
            
            // Compute reward
            float reward = world->computeReward(brain->getRegions()[0].get());
            total_reward += reward;
            
            // Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.001);
            
            // Collect metrics
            total_firing_rate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firing_count++;
        }
        
        result["total_reward"] = total_reward / steps;
        result["average_firing_rate"] = total_firing_rate / steps;
        result["active_steps"] = static_cast<float>(firing_count) / steps;
        result["total_spikes"] = brain->getTotalSpikeCount();
        result["neuron_count"] = brain->getTotalNeuronCount();
        
        return result;
    }
    
    // Get brain statistics
    py::dict get_stats() const {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["firing_neuron_count"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        
        return stats;
    }
    
    // Save checkpoint
    bool save_checkpoint(const std::string& filepath) const {
        return brain->save(filepath);
    }
    
    // Load checkpoint
    bool load_checkpoint(const std::string& filepath) {
        auto config = std::make_shared<nlm::Config>();
        auto new_brain = std::make_shared<nlm::Brain>(config);
        if (!new_brain->initialize()) {
            return false;
        }
        
        if (!new_brain->load(filepath)) {
            return false;
        }
        
        brain = new_brain;
        agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
        
        return true;
    }
    
    // Get working memory
    py::dict get_working_memory() const {
        auto wm = brain->getWorkingMemory();
        py::dict memory;
        
        if (wm) {
            memory["active_traces"] = wm->getActiveTraces();
            memory["capacity"] = wm->getCapacity();
            memory["current_load"] = wm->getCurrentLoad();
        } else {
            memory["available"] = false;
        }
        
        return memory;
    }
    
    // Get episodic memory
    py::dict get_episodic_memory() const {
        auto em = brain->getEpisodicMemory();
        py::dict memory;
        
        if (em) {
            memory["episode_count"] = em->getEpisodeCount();
            memory["max_capacity"] = em->getMaxCapacity();
            memory["current_usage"] = em->getCurrentUsage();
        } else {
            memory["available"] = false;
        }
        
        return memory;
    }
    
    // Get neuromodulation levels
    py::dict get_neuromodulation() const {
        py::dict neuromod;
        
        auto dopamine = brain->getDopamine();
        if (dopamine) neuromod["dopamine"] = dopamine->getLevel();
        
        auto curiosity = brain->getCuriosity();
        if (curiosity) neuromod["curiosity"] = curiosity->getLevel();
        
        auto novelty = brain->getNovelty();
        if (novelty) neuromod["novelty"] = novelty->getLevel();
        
        auto prediction_error = brain->getPredictionErrorSignal();
        if (prediction_error) neuromod["prediction_error"] = prediction_error->getError();
        
        return neuromod;
    }
    
    // Get development stage
    std::string get_development_stage() const {
        auto stage = brain->getDevelopmentStage();
        return "Stage_" + std::to_string(static_cast<int>(stage));
    }
};

// Wrapper for Phase6IntegratedExperiment
class Phase6ExperimentWrapper {
public:
    nlm::Phase6IntegratedExperiment experiment;
    
    py::dict run_experiment(py::dict config_dict) {
        nlm::Phase6Config config;
        
        // Convert py::dict to Phase6Config
        if (config_dict.contains("neuronCount")) {
            config.neuronCount = config_dict["neuronCount"].cast<size_t>();
        }
        if (config_dict.contains("maxSteps")) {
            config.maxSteps = config_dict["maxSteps"].cast<uint64_t>();
        }
        if (config_dict.contains("regionCount")) {
            config.regionCount = config_dict["regionCount"].cast<size_t>();
        }
        if (config_dict.contains("connectionProbability")) {
            config.connectionProbability = config_dict["connectionProbability"].cast<float>();
        }
        if (config_dict.contains("enableCheckpointing")) {
            config.enableCheckpointing = config_dict["enableCheckpointing"].cast<bool>();
        }
        if (config_dict.contains("enableReplay")) {
            config.enableReplay = config_dict["enableReplay"].cast<bool>();
        }
        if (config_dict.contains("enableDevelopment")) {
            config.enableDevelopment = config_dict["enableDevelopment"].cast<bool>();
        }
        if (config_dict.contains("checkpointPath")) {
            config.checkpointPath = config_dict["checkpointPath"].cast<std::string>();
        }
        
        // Run experiment
        auto result = experiment.run(config);
        
        // Convert to py::dict
        py::dict py_result;
        py_result["totalReward"] = result.totalReward;
        py_result["avgFiringRate"] = result.avgFiringRate;
        py_result["memoryEpisodesStored"] = result.memoryEpisodesStored;
        py_result["noveltyLevel"] = result.noveltyLevel;
        py_result["curiosityLevel"] = result.curiosityLevel;
        py_result["dopamineLevel"] = result.dopamineLevel;
        
        // Integration status
        py::dict integration;
        integration["workingMemory"] = result.memoryWorkingMemoryIntegrated;
        integration["episodicMemory"] = result.memoryEpisodicMemoryIntegrated;
        integration["neuromodulation"] = result.neuromodulationIntegrated;
        integration["prediction"] = result.predictionIntegrated;
        integration["development"] = result.developmentIntegrated;
        integration["checkpointing"] = result.checkpointingWorks;
        integration["replay"] = result.replayWorks;
        
        py_result["integration"] = integration;
        py_result["wallClockTime"] = result.totalWallClockTime;
        
        return py_result;
    }
    
    bool verify_integration() {
        return experiment.verifyIntegration();
    }
    
    bool test_memory_integration() {
        return experiment.testMemoryIntegration();
    }
    
    bool test_neuromodulation_integration() {
        return experiment.testNeuromodulationIntegration();
    }
    
    bool test_checkpointing() {
        return experiment.testCheckpointing();
    }
    
    bool test_replay() {
        return experiment.testReplay();
    }
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct PerformanceStats {
        double cpu_usage;
        double memory_usage;
        size_t thread_count;
        double simulation_speed;
        size_t spike_events_per_second;
    };
    
    static PerformanceStats get_system_stats() {
        PerformanceStats stats;
        // TODO: Implement actual system monitoring
        stats.cpu_usage = 0.0;
        stats.memory_usage = 0.0;
        stats.thread_count = std::thread::hardware_concurrency();
        stats.simulation_speed = 0.0;
        stats.spike_events_per_second = 0;
        return stats;
    }
    
    static std::vector<double> benchmark_simulation(size_t steps, size_t neuron_count) {
        std::vector<double> timings;
        // TODO: Implement benchmark with actual timing
        return timings;
    }
};

// Main Python module definition
PYBIND11_MODULE(nlm, m) {
    m.doc() = "NLM (Neural Learning Machine) - Experimental Artificial Developmental Brain";
    
    // Version info
    m.attr("__version__") = "6.0.0";
    m.attr("__codename__") = "FINAL INTEGRATION";
    
    // Configuration classes
    py::class_<ConfigWrapper>(m, "Config")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("set", (void (ConfigWrapper::*)(const std::string&, int)) &ConfigWrapper::set)
        .def("set", (void (ConfigWrapper::*)(const std::string&, int64_t)) &ConfigWrapper::set)
        .def("set", (void (ConfigWrapper::*)(const std::string&, double)) &ConfigWrapper::set)
        .def("set", (void (ConfigWrapper::*)(const std::string&, bool)) &ConfigWrapper::set)
        .def("set", (void (ConfigWrapper::*)(const std::string&, const std::string&)) &ConfigWrapper::set)
        .def("get_int", &ConfigWrapper::get_int)
        .def("get_int64", &ConfigWrapper::get_int64)
        .def("get_double", &ConfigWrapper::get_double)
        .def("get_bool", &ConfigWrapper::get_bool)
        .def("get_string", &ConfigWrapper::get_string)
        .def("has", &ConfigWrapper::has)
        .def("get_keys", &ConfigWrapper::get_keys)
        .def("summary", &ConfigWrapper::summary)
        .def("load_from_json", &ConfigWrapper::load_from_json)
        .def("load_from_yaml", &ConfigWrapper::load_from_yaml)
        .def("to_json", &ConfigWrapper::to_json)
        .def("to_yaml", &ConfigWrapper::to_yaml)
        .def("validate", &ConfigWrapper::validate);
    
    // Brain simulation classes
    py::class_<BrainWrapper>(m, "Brain")
        .def(py::init<>())
        .def(py::init<const ConfigWrapper&>())
        .def("run_steps", &BrainWrapper::run_steps)
        .def("get_stats", &BrainWrapper::get_stats)
        .def("save_checkpoint", &BrainWrapper::save_checkpoint)
        .def("load_checkpoint", &BrainWrapper::load_checkpoint)
        .def("get_working_memory", &BrainWrapper::get_working_memory)
        .def("get_episodic_memory", &BrainWrapper::get_episodic_memory)
        .def("get_neuromodulation", &BrainWrapper::get_neuromodulation)
        .def("get_development_stage", &BrainWrapper::get_development_stage);
    
    // Experiment classes
    py::class_<Phase6ExperimentWrapper>(m, "Phase6Experiment")
        .def(py::init<>())
        .def("run_experiment", &Phase6ExperimentWrapper::run_experiment)
        .def("verify_integration", &Phase6ExperimentWrapper::verify_integration)
        .def("test_memory_integration", &Phase6ExperimentWrapper::test_memory_integration)
        .def("test_neuromodulation_integration", &Phase6ExperimentWrapper::test_neuromodulation_integration)
        .def("test_checkpointing", &Phase6ExperimentWrapper::test_checkpointing)
        .def("test_replay", &Phase6ExperimentWrapper::test_replay);
    
    // Performance monitoring
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor")
        .def_static("get_system_stats", &PerformanceMonitor::get_system_stats)
        .def_static("benchmark_simulation", &PerformanceMonitor::benchmark_simulation);
    
    // Utility functions
    m.def("print_version", []() {
        std::cout << "NLM " << nlm::NLM_VERSION << " (" << nlm::NLM_CODENAME << ")" << std::endl;
    });
    
    m.def("create_config", []() {
        return ConfigWrapper();
    });
    
    m.def("create_brain", []() {
        return BrainWrapper();
    });
    
    m.def("create_experiment", []() {
        return Phase6ExperimentWrapper();
    });
}

} // namespace nlm_py