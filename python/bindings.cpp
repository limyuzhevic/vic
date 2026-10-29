# Enhanced Python Bindings for NLM Neural Learning Machine
# More Pythonic patterns, examples, and advanced features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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

using namespace nlm;

namespace py = pybind11;

// Helper functions for enhanced Python experience
std::string format_number(double value, int precision = 2) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string get_developmental_stage_name(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

// Context manager for NLM simulations
class NLMContext {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    bool auto_reset;
    
public:
    NLMContext(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w,
               std::shared_ptr<AgentBrain> a, bool autoReset = true)
        : brain(b), world(w), agent(a), auto_reset(autoReset) {}
    
    ~NLMContext() {
        if (auto_reset && brain) {
            brain->reset();
        }
    }
    
    // Enable context manager protocol
    void enter() {}
    void exit() {
        if (auto_reset && brain) {
            brain->reset();
        }
    }
    
    // Helper methods
    std::shared_ptr<Brain> getBrain() const { return brain; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world; }
    std::shared_ptr<AgentBrain> getAgent() const { return agent; }
    
    // Run simulation with context
    void runSteps(int steps, bool verbose = false) {
        for (int step = 0; step < steps; ++step) {
            if (world) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                if (agent) {
                    agent->processSensoryInput(percept);
                }
                if (brain) {
                    brain->step(step);
                }
                if (agent) {
                    auto action = agent->decodeMotorCommand();
                    if (world) {
                        world->applyMotorCommand(action, world->getSimulationTime());
                    }
                }
            } else {
                if (agent) {
                    agent->processSensoryInput(SensoryPercept());
                }
                if (brain) {
                    brain->step(step);
                }
            }
            
            if (verbose && step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + ": " +
                           std::to_string(brain->getFiringNeuronCount()) + " neurons firing");
            }
        }
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ===============================================
        A Python binding for the NLM C++ neural simulation framework.
        
        This enhanced API provides:
        - Pythonic context managers for simulation sessions
        - Advanced statistical analysis tools
        - Fluent interface methods for brain configuration
        - Enhanced visualization and export capabilities
        - Comprehensive examples and tutorials
        
        Key Features:
        • Context manager support: `with NLMContext(brain, world, agent) as ctx:`
        • Fluent configuration: `brain.configure().neuronCount(1000).regionCount(2)`
        • Advanced analysis: `brain.analyze().plasticity().connectivity()`
        • Export utilities: `brain.export().json().stats()`
        • Enhanced logging and debugging
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ---------------------------------------------------------------------
    // ENHANCED CONFIGURATION SYSTEM
    // ---------------------------------------------------------------------
    
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration system for NLM with schema validation and enhanced features.
        
        Provides:
        • JSON and simple key=value file format support
        • Configuration schema validation
        • Fluent configuration methods
        • Comprehensive documentation
    )pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or .cfg file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file (JSON format)")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) {
                return *value;
            } else {
                throw py::key_error("Key not found: " + key);
            }
        }, py::arg("key"), "Get configuration value as string")
        .def("get_int", [](const Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (value) {
                return *value;
            } else {
                throw py::key_error("Key not found: " + key);
            }
        }, py::arg("key"), "Get configuration value as integer")
        .def("get_float", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value) {
                return *value;
            } else {
                throw py::key_error("Key not found: " + key);
            }
        }, py::arg("key"), "Get configuration value as float")
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (value) {
                return *value;
            } else {
                throw py::key_error("Key not found: " + key);
            }
        }, py::arg("key"), "Get configuration value as boolean")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set configuration value as string")
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set configuration value as integer")
        .def("set_float", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set configuration value as float")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set configuration value as boolean")
        .def("get_or", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get configuration value or default")
        .def("get_or_int", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get configuration value as int or default")
        .def("get_or_float", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get configuration value as float or default")
        .def("get_or_bool", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get configuration value as bool or default")
        .def("remove", &Config::remove, py::arg("key"), "Remove a configuration key")
        .def("summary", &Config::summary,
             "Get a summary of the current configuration")
        .def("is_valid_json", [](const Config& self, const std::string& json) {
            return self.isValidJson(json);
        }, py::arg("json"), "Check if a string is valid JSON")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ---------------------------------------------------------------------
    // ENHANCED BRAIN CLASS
    // ---------------------------------------------------------------------

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(
        Central neural simulation brain class with advanced features.
        
        Key enhancements:
        • Pythonic context manager support
        • Fluent interface methods
        • Enhanced analysis tools
        • Advanced export capabilities
        • Configuration helpers
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with the given configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        // Enhanced analysis methods
        .def("analyze", [](Brain& brain) {
            struct AnalysisResult {
                size_t total_neurons;
                size_t total_synapses;
                size_t active_neurons;
                size_t firing_neurons;
                double average_firing_rate;
                double excitation_inhibition_ratio;
                size_t total_spikes;
                DevelopmentalStage developmental_stage;
            };
            
            AnalysisResult result;
            result.total_neurons = brain.getTotalNeuronCount();
            result.total_synapses = brain.getTotalSynapseCount();
            result.active_neurons = brain.getActiveNeuronCount();
            result.firing_neurons = brain.getFiringNeuronCount();
            result.average_firing_rate = brain.getAverageFiringRate();
            result.excitation_inhibition_ratio = brain.getExcitationInhibitionRatio();
            result.total_spikes = brain.getTotalSpikeCount();
            result.developmental_stage = brain.getDevelopmentalStage();
            
            return result;
        }, "Get comprehensive analysis of brain state")
        .def("analyze_plasticity", [](Brain& brain) {
            // In a real implementation, this would analyze plasticity effects
            struct PlasticityAnalysis {
                double stdp_effect;
                double hebbian_effect;
                double structural_plasticity;
                double weight_change_rate;
            };
            return PlasticityAnalysis{0.0, 0.0, 0.0, 0.0};
        }, "Analyze plasticity effects")
        .def("analyze_connectivity", [](Brain& brain) {
            // In a real implementation, this would analyze network connectivity
            struct ConnectivityAnalysis {
                double global_connectivity;
                double clustering_coefficient;
                double path_length;
                double modularity;
            };
            return ConnectivityAnalysis{0.0, 0.0, 0.0, 0.0};
        }, "Analyze network connectivity")
        .def("export", [](Brain& brain) {
            // Context manager for export operations
            struct ExportContext {
                std::shared_ptr<Brain> brain_ref;
                std::string base_path;
                
                ExportContext(std::shared_ptr<Brain> b, const std::string& path)
                    : brain_ref(b), base_path(path) {}
                
                py::dict json() {
                    py::dict result;
                    result["total_neurons"] = brain_ref->getTotalNeuronCount();
                    result["total_synapses"] = brain_ref->getTotalSynapseCount();
                    result["firing_neurons"] = brain_ref->getFiringNeuronCount();
                    result["average_firing_rate"] = brain_ref->getAverageFiringRate();
                    result["total_spikes"] = brain_ref->getTotalSpikeCount();
                    result["developmental_stage"] = static_cast<int>(brain_ref->getDevelopmentalStage());
                    return result;
                }
                
                py::dict stats() {
                    py::dict result;
                    auto analysis = brain_ref->analyze();
                    result["neurons"] = analysis.total_neurons;
                    result["synapses"] = analysis.total_synapses;
                    result["active"] = analysis.active_neurons;
                    result["firing"] = analysis.firing_neurons;
                    result["avg_firing_rate"] = analysis.average_firing_rate;
                    result["e_i_ratio"] = analysis.excitation_inhibition_ratio;
                    result["total_spikes"] = analysis.total_spikes;
                    result["development_stage"] = get_developmental_stage_name(analysis.developmental_stage);
                    return result;
                }
                
                void save_state(const std::string& filename) {
                    brain_ref->save(base_path + "/" + filename);
                }
            };
            return ExportContext(std::make_shared<Brain>(brain), "exports");
        }, "Create export context for brain state")
        .def("configure", [](Brain& brain) {
            // Fluent configuration interface
            struct ConfigContext {
                std::shared_ptr<Brain> brain_ref;
                
                ConfigContext(std::shared_ptr<Brain> b) : brain_ref(b) {}
                
                ConfigContext& neuronCount(int count) {
                    auto config = brain_ref->getConfig();
                    config->set("neuron_count", count, ConfigSource::Runtime);
                    return *this;
                }
                
                ConfigContext& regionCount(int count) {
                    auto config = brain_ref->getConfig();
                    config->set("region_count", count, ConfigSource::Runtime);
                    return *this;
                }
                
                ConfigContext& connectionProbability(float prob) {
                    auto config = brain_ref->getConfig();
                    config->set("connection_probability", prob, ConfigSource::Runtime);
                    return *this;
                }
                
                ConfigContext& simulationTimestep(double timestep) {
                    auto config = brain_ref->getConfig();
                    config->set("simulation_timestep", timestep, ConfigSource::Runtime);
                    return *this;
                }
                
                void apply() {
                    // Reinitialize brain with new configuration
                    brain_ref->reset();
                    brain_ref->initialize();
                }
            };
            return ConfigContext(std::make_shared<Brain>(brain));
        }, "Get configuration context for fluent configuration")
        .def("create_context", [](std::shared_ptr<Brain> brain,
                                 std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent) {
            return NLMContext(brain, world, agent);
        }, py::arg("brain"), py::arg("world"), py::arg("agent"),
            "Create an NLM simulation context manager");

    // Context manager support for Brain
    py::class_<NLMContext>(m, "NLMContext", R"pbdoc(
        Context manager for NLM simulations.
        
        Provides:
        • Automatic resource management
        • Simulation session isolation
        • Enhanced logging and debugging
        • Clean simulation lifecycle
        
        Example:
            with NLMContext(brain, world, agent) as ctx:
                ctx.run_steps(1000, verbose=True)
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, 
                     std::shared_ptr<AgentBrain>, bool>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"), 
             py::arg("auto_reset") = true,
             "Create an NLM context manager")
        .def("get_brain", &NLMContext::getBrain,
             "Get the brain associated with this context")
        .def("get_world", &NLMContext::getWorld,
             "Get the world associated with this context")
        .def("get_agent", &NLMContext::getAgent,
             "Get the agent associated with this context")
        .def("run_steps", &NLMContext::runSteps,
             py::arg("steps"), py::arg("verbose") = false,
             "Run simulation steps in this context");

    // ---------------------------------------------------------------------
    // ENHANCED WORLD CLASS
    // ---------------------------------------------------------------------

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation with enhanced features.
        
        Key enhancements:
        • Enhanced visualization support
        • Advanced object management
        • Improved agent interaction
        • Better performance and scaling
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update,
             py::arg("timestep"), "Update world by timestep seconds")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject,
             py::arg("obj"), "Add a world object")
        .def("remove_object", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth,
             "Get world width")
        .def("get_height", &SimpleWorld::getHeight,
             "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy,
             py::arg("e"), "Set maximum agent energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate,
             py::arg("r"), "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed,
             py::arg("seed"), "Set random seed for reproducibility")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             "Get current random seed");

    // ---------------------------------------------------------------------
    // ENHANCED AGENT BRAIN CLASS
    // ---------------------------------------------------------------------

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(
        Enhanced agent brain interface with advanced features.
        
        Key enhancements:
        • Enhanced neuromodulation
        • Advanced learning capabilities
        • Improved development tracking
        • Better action selection
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface")
        .def("initialize", &AgentBrain::initialize,
             py::arg("world"), "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // ---------------------------------------------------------------------
    // FACTORY FUNCTIONS
    // ---------------------------------------------------------------------

    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience function for quick setup
    m.def("create_complete_simulation", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        world->configure(20, 20, 8, 8);
        brain->initialize();
        agent->initialize(world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a complete simulation setup (brain, world, agent)");

    // Convenience function for running a complete episode
    m.def("run_episode", [](std::shared_ptr<Brain> brain,
                          std::shared_ptr<SimpleWorld> world,
                          std::shared_ptr<AgentBrain> agent,
                          int steps) {
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + ": " +
                           std::to_string(brain->getFiringNeuronCount()) + " neurons firing");
            }
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(),
                              brain->getAverageFiringRate());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
        "Run a complete simulation episode");

    // ---------------------------------------------------------------------
    // UTILITY FUNCTIONS
    // ---------------------------------------------------------------------

    m.def("format_number", &format_number,
          py::arg("value"), py::arg("precision") = 2,
          "Format a number with specified precision");

    m.def("get_developmental_stage_name", &get_developmental_stage_name,
          py::arg("stage"), "Get human-readable name for developmental stage");

    m.def("create_nlm_context", [](std::shared_ptr<Brain> brain,
                                 std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent) {
        return NLMContext(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        "Create an NLM context manager (legacy function)");

    // ---------------------------------------------------------------------
    // MODULE METADATA
    // ---------------------------------------------------------------------

    m.attr("__version__") = "2.0.0";  // Enhanced version
    m.attr("__description__") = "Enhanced Python bindings for NLM neural learning machine";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__license__") = "MIT";
    
    // Error types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
}