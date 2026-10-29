#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Context manager for simulation runs
class SimulationContext {
public:
    SimulationContext(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                      std::shared_ptr<AgentBrain> agent, size_t steps)
        : brain_(brain), world_(world), agent_(agent), steps_(steps) {
    }

    void __enter__() {
        // Store initial state
        initialSpikeCount_ = brain_->getTotalSpikeCount();
        initialFiringCount_ = brain_->getFiringNeuronCount();
        initialTime_ = world_->getSimulationTime();
        
        // Reset and initialize
        if (brain_) brain_->reset();
        if (agent_) agent_->reset();
        
        if (world_) world_->reset();
        if (agent_) agent_->initialize(*world_);
        if (brain_) brain_->initialize();
    }

    py::dict __exit__(py::args) {
        // Capture final state
        py::dict result;
        if (brain_) {
            result["spike_count"] = brain_->getTotalSpikeCount() - initialSpikeCount_;
            result["firing_count"] = brain_->getFiringNeuronCount();
            result["avg_firing_rate"] = brain_->getAverageFiringRate();
        }
        if (world_) {
            result["world_time"] = world_->getSimulationTime() - initialTime_;
            result["last_reward"] = world_->getLastReward();
        }
        return result;
    }

    // Execute simulation with custom step function
    py::dict run_simulation(py::function<void(size_t, float)> stepFunc) {
        py::dict results;
        if (!brain_ || !world_ || !agent_) {
            throw std::runtime_error("Brain, world, or agent not initialized");
        }

        for (size_t step = 0; step < steps_; ++step) {
            // Get observation
            SensoryPercept percept = world_->observe(brain_->getRegions()[0].get());
            
            // Process sensory input
            agent_->processSensoryInput(percept);
            
            // Call custom step function
            stepFunc(step, step * 0.001f);
            
            // Brain step
            brain_->step(step, step * 0.001f);
            
            // Get motor command
            MotorCommand cmd = agent_->decodeMotorCommand();
            
            // Apply action to world
            world_->applyAction(brain_->getRegions()[0].get(), cmd);
            
            // Store results every 100 steps
            if (step % 100 == 0) {
                results[std::to_string(step)] = py::make_tuple(
                    brain_->getTotalSpikeCount(),
                    brain_->getFiringNeuronCount(),
                    brain_->getAverageFiringRate(),
                    world_->getLastReward()
                );
            }
        }
        
        return results;
    }

    // Run with automatic step function
    py::dict run() {
        return run_simulation([](size_t step, float time) {
            // Default step function - just advances time
        });
    }

    // Get current state
    py::dict get_state() const {
        py::dict state;
        if (brain_) {
            state["spike_count"] = brain_->getTotalSpikeCount();
            state["firing_count"] = brain_->getFiringNeuronCount();
            state["avg_firing_rate"] = brain_->getAverageFiringRate();
            state["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        }
        if (world_) {
            state["world_time"] = world_->getSimulationTime();
            state["last_reward"] = world_->getLastReward();
            state["is_done"] = world_->isDone();
        }
        if (agent_) {
            state["curiosity"] = agent_->getCuriosityLevel();
            state["novelty"] = agent_->getNoveltyLevel();
            state["prediction_error"] = agent_->getPredictionError();
            state["neuromodulation"] = agent_->getNeuromodulationLevel();
            state["developmental_stage"] = static_cast<int>(agent_->getDevelopmentalStage());
        }
        return state;
    }

    // Set up learning environment
    void setup_learning(bool enableRewardModulation = true, bool enableCuriosity = true,
                       bool enableDevelopment = true, bool enableStructuralPlasticity = true) {
        if (agent_) {
            if (enableRewardModulation) agent_->enableRewardModulation(true);
            if (enableCuriosity) agent_->enableCuriosity(true);
            if (enableDevelopment) agent_->enableDevelopment(true);
            if (enableStructuralPlasticity) agent_->enableStructuralPlasticity(true);
        }
    }

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    size_t steps_;
    size_t initialSpikeCount_ = 0;
    size_t initialFiringCount_ = 0;
    float initialTime_ = 0.0f;
};

// Enhanced Brain wrapper with additional methods
class EnhancedBrain {
public:
    EnhancedBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}

    void initialize() {
        if (!brain_->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
    }

    void step_with_input(size_t step, float time, const SensoryPercept& percept) {
        brain_->receiveSensoryInput(percept);
        brain_->step(step, time);
    }

    py::dict get_detailed_stats() const {
        py::dict stats;
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["active_neurons"] = brain_->getActiveNeuronCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["average_firing_rate"] = brain_->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["pending_spike_events"] = brain_->getPendingSpikeEventCount();
        stats["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
        
        // Get neuromodulation levels if available
        if (brain_->getDopamine()) {
            stats["dopamine_level"] = brain_->getDopamine()->getCurrentValue();
        }
        if (brain_->getCuriosity()) {
            stats["curiosity_level"] = brain_->getCuriosity()->getCurrentValue();
        }
        
        return stats;
    }

    bool save_checkpoint(const std::string& filepath) const {
        return brain_->save(filepath);
    }

    bool load_checkpoint(const std::string& filepath) {
        return brain_->load(filepath);
    }

    // Get brain as shared_ptr for use with other components
    std::shared_ptr<Brain> get_brain() { return brain_; }

    // Context manager support
    SimulationContext run_simulation(size_t steps, std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent) {
        return SimulationContext(brain_, world, agent, steps);
    }

private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced AgentBrain wrapper
class EnhancedAgentBrain {
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain) : agent_(std::make_shared<AgentBrain>(brain)) {}

    void initialize(SimpleWorld& world) {
        agent_->initialize(world);
    }

    py::dict process_and_step(const SensoryPercept& percept, size_t step, float time) {
        agent_->processSensoryInput(percept);
        MotorCommand cmd = agent_->decodeMotorCommand();
        
        py::dict result;
        result["action"] = static_cast<int>(cmd);
        result["curiosity"] = agent_->getCuriosityLevel();
        result["novelty"] = agent_->getNoveltyLevel();
        result["prediction_error"] = agent_->getPredictionError();
        result["neuromodulation"] = agent_->getNeuromodulationLevel();
        return result;
    }

    void apply_reward_modulation(float reward, float predicted) {
        agent_->applyRewardModulation(reward, predicted);
    }

    void update_development(float timestep) {
        agent_->updateDevelopment(timestep);
    }

    py::dict get_advanced_metrics() const {
        py::dict metrics;
        metrics["curiosity"] = agent_->getCuriosityLevel();
        metrics["novelty"] = agent_->getNoveltyLevel();
        metrics["prediction_error"] = agent_->getPredictionError();
        metrics["neuromodulation"] = agent_->getNeuromodulationLevel();
        metrics["developmental_stage"] = static_cast<int>(agent_->getDevelopmentalStage());
        metrics["is_reward_enabled"] = agent_->isRewardModulationEnabled();
        metrics["is_curiosity_enabled"] = agent_->isCuriosityEnabled();
        metrics["is_development_enabled"] = agent_->isDevelopmentEnabled();
        metrics["is_structural_plasticity_enabled"] = agent_->isStructuralPlasticityEnabled();
        return metrics;
    }

    std::shared_ptr<AgentBrain> get_agent() { return agent_; }

private:
    std::shared_ptr<AgentBrain> agent_;
};

// Helper functions for easier Python API
namespace python {

    // Create configuration from JSON string
    std::shared_ptr<Config> config_from_json(const std::string& jsonStr) {
        auto config = std::make_shared<Config>();
        // Parse JSON string directly - would need nlohmann::json support
        // For now, return empty config
        return config;
    }

    // Create configuration from YAML string
    std::shared_ptr<Config> config_from_yaml(const std::string& yamlStr) {
        auto config = std::make_shared<Config>();
        // Parse YAML string directly - would need YAML::Node support
        // For now, return empty config
        return config;
    }

    // Create default configuration with specific settings
    std::shared_ptr<Config> create_config(size_t neuronCount = 1000, 
                                          size_t regionCount = 1,
                                          float connectionProbability = 0.1) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config->set("connection_probability", connectionProbability, ConfigSource::Default);
        return config;
    }

    // Create a simple simulation setup
    std::tuple<std::shared_ptr<EnhancedBrain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<EnhancedAgentBrain>>
    create_simple_simulation(size_t neuronCount = 500, size_t regionCount = 1) {
        // Create config
        auto config = create_config(neuronCount, regionCount);
        
        // Create brain
        auto brain = std::make_shared<EnhancedBrain>(config);
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->initialize(16, 16);
        
        // Create agent
        auto agent = std::make_shared<EnhancedAgentBrain>(brain->get_brain());
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }

    // Run a simulation with automatic cleanup
    py::dict run_complete_simulation(size_t steps, bool enableLearning = true) {
        auto [brain, world, agent] = create_simple_simulation();
        
        // Set up learning
        if (enableLearning) {
            auto enhancedAgent = std::dynamic_pointer_cast<EnhancedAgentBrain>(agent);
            if (enhancedAgent) {
                enhancedAgent->setup_learning(true, true, true, true);
            }
        }
        
        // Run simulation
        auto context = brain->run_simulation(steps, world, agent);
        context.__enter__();
        
        py::dict result = context.run();
        
        // Get final stats
        result["final_stats"] = brain->get_detailed_stats();
        
        return result;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -------------------------------------------------------
        An enhanced Python interface for the NLM neural simulation framework.
        Provides advanced features for easier and more powerful usage.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register exception classes
    py::register_exception<std::runtime_error>(m, "NLMError");

    // Configuration class enhancements
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class with JSON/YAML support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from file (JSON, YAML, or .cfg format)")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to file")
        .def("toJSON", &Config::toJSON, "Export configuration as JSON string")
        .def("toYAML", &Config::toYAML, "Export configuration as YAML string")
        .def("validate", &Config::validate, py::arg("schemaFile") = "",
             "Validate configuration against schema")
        .def("merge", &Config::merge, py::arg("other"),
             "Merge another configuration into this one")
        .def("getAllStringValues", &Config::getAllStringValues,
             "Get all configuration values as strings")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Brain class
    py::class_<EnhancedBrain>(m, "EnhancedBrain", R"pbdoc(Enhanced brain interface with additional features)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &EnhancedBrain::initialize, "Initialize the brain")
        .def("step_with_input", &EnhancedBrain::step_with_input,
             py::arg("step"), py::arg("time"), py::arg("percept"),
             "Step brain with sensory input")
        .def("get_detailed_stats", &EnhancedBrain::get_detailed_stats,
             "Get detailed brain statistics")
        .def("save_checkpoint", &EnhancedBrain::save_checkpoint, py::arg("filepath"),
             "Save brain state to checkpoint")
        .def("load_checkpoint", &EnhancedBrain::load_checkpoint, py::arg("filepath"),
             "Load brain state from checkpoint")
        .def("run_simulation", &EnhancedBrain::run_simulation,
             py::arg("steps"), py::arg("world"), py::arg("agent"),
             "Context manager for simulation runs");

    // Enhanced AgentBrain class
    py::class_<EnhancedAgentBrain>(m, "EnhancedAgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &EnhancedAgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("process_and_step", &EnhancedAgentBrain::process_and_step,
             py::arg("percept"), py::arg("step"), py::arg("time"),
             "Process percept and step simulation")
        .def("apply_reward_modulation", &EnhancedAgentBrain::apply_reward_modulation,
             py::arg("reward"), py::arg("predicted"),
             "Apply reward-based neuromodulation")
        .def("update_development", &EnhancedAgentBrain::update_development, py::arg("timestep"),
             "Update development system")
        .def("get_advanced_metrics", &EnhancedAgentBrain::get_advanced_metrics,
             "Get advanced agent metrics");

    // Simulation context
    py::class_<SimulationContext>(m, "SimulationContext", R"pbdoc(Context manager for simulation runs)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, 
                  std::shared_ptr<AgentBrain>, size_t>())
        .def("__enter__", &SimulationContext::__enter__,
             "Enter context and setup simulation")
        .def("__exit__", &SimulationContext::__exit__,
             "Exit context and cleanup")
        .def("run", &SimulationContext::run,
             "Run simulation with default step function")
        .def("run_with_step", &SimulationContext::run_simulation,
             py::arg("stepFunc"), "Run simulation with custom step function")
        .def("get_state", &SimulationContext::get_state,
             "Get current simulation state")
        .def("setup_learning", &SimulationContext::setup_learning,
             py::arg("enableRewardModulation") = true,
             py::arg("enableCuriosity") = true,
             py::arg("enableDevelopment") = true,
             py::arg("enableStructuralPlasticity") = true,
             "Setup learning environment");

    // Helper functions
    m.def("create_default_config", &python::create_config,
          py::arg("neuronCount") = 1000,
          py::arg("regionCount") = 1,
          py::arg("connectionProbability") = 0.1,
          "Create default configuration with custom settings");

    m.def("create_simple_simulation", &python::create_simple_simulation,
          "Create a simple simulation setup");

    m.def("run_complete_simulation", &python::run_complete_simulation,
          py::arg("steps") = 1000,
          py::arg("enableLearning") = true,
          "Run a complete simulation with automatic setup and cleanup");

    // Original factory functions (for backward compatibility)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and info
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "Enhanced NLM Python Bindings with advanced features";
}

} // namespace nlm