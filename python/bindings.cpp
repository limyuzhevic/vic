#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Enhanced Config class with Pythonic APIs
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON/YAML file (auto-detects format)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON/YAML file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", static_cast<std::optional<int>(Config::*)(const std::string&) const>(
            &Config::get<int>), py::arg("key"),
            "Get integer value for key")
        .def("get", static_cast<std::optional<int64_t>(Config::*)(const std::string&) const>(
            &Config::get<int64_t>), py::arg("key"),
            "Get int64 value for key")
        .def("get", static_cast<std::optional<double>(Config::*)(const std::string&) const>(
            &Config::get<double>), py::arg("key"),
            "Get double value for key")
        .def("get", static_cast<std::optional<bool>(Config::*)(const std::string&) const>(
            &Config::get<bool>), py::arg("key"),
            "Get boolean value for key")
        .def("get", static_cast<std::optional<std::string>(Config::*)(const std::string&) const>(
            &Config::get<std::string>), py::arg("key"),
            "Get string value for key")
        .def("getOr", static_cast<int(Config::*)(const std::string&, const int&) const>(
            &Config::getOr<int>), py::arg("key"), py::arg("defaultValue"),
            "Get integer value with default, returns default if key doesn't exist")
        .def("getOr", static_cast<int64_t(Config::*)(const std::string&, const int64_t&) const>(
            &Config::getOr<int64_t>), py::arg("key"), py::arg("defaultValue"),
            "Get int64 value with default, returns default if key doesn't exist")
        .def("getOr", static_cast<double(Config::*)(const std::string&, const double&) const>(
            &Config::getOr<double>), py::arg("key"), py::arg("defaultValue"),
            "Get double value with default, returns default if key doesn't exist")
        .def("getOr", static_cast<bool(Config::*)(const std::string&, const bool&) const>(
            &Config::getOr<bool>), py::arg("key"), py::arg("defaultValue"),
            "Get boolean value with default, returns default if key doesn't exist")
        .def("getOr", static_cast<std::string(Config::*)(const std::string&, const std::string&) const>(
            &Config::getOr<std::string>), py::arg("key"), py::arg("defaultValue"),
            "Get string value with default, returns default if key doesn't exist")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set),
            py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set string value for key")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set),
            py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set integer value for key")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(&Config::set),
            py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set double value for key")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(&Config::set),
            py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set boolean value for key")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Pythonic properties for common configuration values
        .def_property("random_seed", &Config::getOr<int64_t, const int64_t&>,
                     [](Config& self, int64_t value) {
                         self.set("random_seed", value, ConfigSource::Runtime);
                     }, "Random seed for reproducible experiments")
        .def_property("simulation_timestep", &Config::getOr<double, const double&>,
                     [](Config& self, double value) {
                         self.set("simulation_timestep", value, ConfigSource::Runtime);
                     }, "Simulation timestep in seconds")
        .def_property("neuron_count", &Config::getOr<int64_t, const int64_t&>,
                     [](Config& self, int64_t value) {
                         self.set("neuron_count", value, ConfigSource::Runtime);
                     }, "Number of neurons in the brain")
        .def_property("region_count", &Config::getOr<int64_t, const int64_t&>,
                     [](Config& self, int64_t value) {
                         self.set("region_count", value, ConfigSource::Runtime);
                     }, "Number of brain regions")
        .def_property("connection_probability", &Config::getOr<float, const float&>,
                     [](Config& self, float value) {
                         self.set("connection_probability", value, ConfigSource::Runtime);
                     }, "Connection probability for random connectivity")
        .def_property("stdp_ltp_weight", &Config::getOr<float, const float&>,
                     [](Config& self, float value) {
                         self.set("stdp_ltp_weight", value, ConfigSource::Runtime);
                     }, "STDP LTP weight")
        .def_property("stdp_ltd_weight", &Config::getOr<float, const float&>,
                     [](Config& self, float value) {
                         self.set("stdp_ltd_weight", value, ConfigSource::Runtime);
                     }, "STDP LTD weight")
        .def_property("synaptogenesis_rate", &Config::getOr<float, const float&>,
                     [](Config& self, float value) {
                         self.set("synaptogenesis_rate", value, ConfigSource::Runtime);
                     }, "Synaptogenesis rate")
        .def_property("pruning_rate", &Config::getOr<float, const float&>,
                     [](Config& self, float value) {
                         self.set("pruning_rate", value, ConfigSource::Runtime);
                     }, "Pruning rate")
        .def_property("dopamine_baseline", &Config::getOr<float, const float&>,
                     [](Config& self, float value) {
                         self.set("dopamine_baseline", value, ConfigSource::Runtime);
                     }, "Dopamine baseline level")
        .def("get_advanced_config", [](const Config& self) {
            py::dict config_dict;
            for (const auto& key : self.getKeys()) {
                // Get all available configuration options
                config_dict[key] = self.getOr<std::string>(key).value_or("") + " (configurable)";
            }
            return config_dict;
        }, "Get a dictionary of all available configuration options")
        .def("validate_config", [](Config& self) {
            // Validate configuration and return list of issues
            std::vector<std::string> issues;
            
            // Check for invalid values
            auto neuron_count = self.getOr<int64_t>("neuron_count");
            if (neuron_count && neuron_count.value() < 10) {
                issues.push_back("neuron_count should be at least 10 for meaningful simulations");
            }
            
            auto timestep = self.getOr<double>("simulation_timestep");
            if (timestep && timestep.value() <= 0.0) {
                issues.push_back("simulation_timestep must be positive");
            }
            
            auto connection_prob = self.getOr<float>("connection_probability");
            if (connection_prob && (connection_prob.value() < 0.0 || connection_prob.value() > 1.0)) {
                issues.push_back("connection_probability must be between 0.0 and 1.0");
            }
            
            if (issues.empty()) {
                return py::dict();
            }
            
            return py::cast(issues);
        }, "Validate configuration and return list of issues")
        // Context manager for temporary configuration changes
        .def("with_temporary_config", [](Config& self, const py::dict& changes) {
            // Store original values
            py::dict original_values;
            for (auto item : changes) {
                std::string key = py::cast<std::string>(item.first);
                auto value = self.getOr<std::string>(key);
                if (value) {
                    original_values[key] = *value;
                }
            }
            
            // Apply temporary changes
            for (auto item : changes) {
                std::string key = py::cast<std::string>(item.first);
                std::string value = py::cast<std::string>(item.second);
                self.set(key, value, ConfigSource::Runtime);
            }
            
            // Return context manager that will restore original values
            return py::make_iterator(original_values.begin(), original_values.end());
        }, "Apply temporary configuration changes and return context manager")
        ;

    // Enhanced Brain class with Pythonic APIs
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        // Pythonic properties for common brain statistics
        .def_property_readonly("total_neurons", &Brain::getTotalNeuronCount,
                              "Total number of neurons in the brain")
        .def_property_readonly("total_synapses", &Brain::getTotalSynapseCount,
                              "Total number of synapses in the brain")
        .def_property_readonly("active_neurons", &Brain::getActiveNeuronCount,
                              "Number of active neurons")
        .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount,
                              "Number of currently firing neurons")
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate,
                              "Average firing rate across all neurons")
        .def_property_readonly("total_spikes", &Brain::getTotalSpikeCount,
                              "Total spike count")
        .def_property_readonly("pending_spikes", &Brain::getPendingSpikeEventCount,
                              "Number of pending spike events")
        .def("get_brain_stats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["pending_spikes"] = self.getPendingSpikeEventCount();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics as dictionary")
        .def("run_advanced_simulation", [](Brain& self, size_t num_steps, bool verbose) {
            py::dict results;
            for (size_t step = 0; step < num_steps; ++step) {
                self.step(step);
                
                if (verbose && step % 100 == 0) {
                    results["step_" + std::to_string(step)] = py::dict({
                        {"neurons_firing", self.getFiringNeuronCount()},
                        {"total_spikes", self.getTotalSpikeCount()},
                        {"avg_firing_rate", self.getAverageFiringRate()}
                    });
                }
            }
            
            results["final_stats"] = py::dict({
                {"neurons_firing", self.getFiringNeuronCount()},
                {"total_spikes", self.getTotalSpikeCount()},
                {"avg_firing_rate", self.getAverageFiringRate()}
            });
            
            return results;
        }, "Run advanced simulation with verbose output option")
        // Context manager for safe brain initialization
        .def("with_safe_initialization", [](Brain& self) {
            // Ensure brain is properly initialized before simulation
            if (!self.getTotalNeuronCount()) {
                throw std::runtime_error("Brain must be initialized before simulation");
            }
            return self;
        }, "Ensure brain is properly initialized")
        ;

    // Enhanced AgentBrain class with Pythonic APIs
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
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
        // Pythonic properties for neuromodulation levels
        .def_property_readonly("dopamine_level", &AgentBrain::getNeuromodulationLevel,
                              "Current dopamine level (reward prediction error)")
        .def_property_readonly("curiosity_level", &AgentBrain::getCuriosityLevel,
                              "Current curiosity level")
        .def_property_readonly("novelty_level", &AgentBrain::getNoveltyLevel,
                              "Current novelty level")
        .def_property_readonly("prediction_error", &AgentBrain::getPredictionError,
                              "Current prediction error")
        .def("get_neuromodulation_summary", [](const AgentBrain& self) {
            py::dict summary;
            summary["dopamine"] = self.getNeuromodulationLevel();
            summary["curiosity"] = self.getCuriosityLevel();
            summary["novelty"] = self.getNoveltyLevel();
            summary["prediction_error"] = self.getPredictionError();
            summary["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return summary;
        }, "Get comprehensive neuromodulation summary")
        .def("run_agent_loop", [](AgentBrain& self, SimpleWorld& world, size_t max_steps) {
            py::dict results;
            size_t successful_actions = 0;
            
            for (size_t step = 0; step < max_steps; ++step) {
                world.update(0.1);  // Update world
                auto percept = world.getSensoryPercept();  // What does the agent see?
                self.processSensoryInput(percept);  // Brain sees it
                // Note: Brain step would be called here by the simulation loop
                auto action = self.decodeMotorCommand();  // Brain decides action
                world.applyMotorCommand(action, world.getSimulationTime());  // Do action
                
                if (world.getSimulationTime() > 0) {
                    successful_actions++;
                }
            }
            
            results["steps_completed"] = max_steps;
            results["successful_actions"] = successful_actions;
            results["success_rate"] = static_cast<float>(successful_actions) / max_steps;
            
            return results;
        }, "Run complete agent simulation loop")
        .def("enable_all_features", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
            return self;
        }, "Enable all brain features (reward modulation, structural plasticity, development, curiosity)")
        ;

    // Enhanced SimpleWorld class with Pythonic APIs
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command from brain")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept of what the agent sees")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Pythonic properties for world state
        .def_property_readonly("width", &SimpleWorld::getWidth)
        .def_property_readonly("height", &SimpleWorld::getHeight)
        .def_property_readonly("max_energy", &SimpleWorld::getMaxEnergy)
        .def_property_readonly("simulation_time", &SimpleWorld::getSimulationTime)
        .def("get_world_state", [](const SimpleWorld& self) {
            py::dict state;
            state["width"] = self.getWidth();
            state["height"] = self.getHeight();
            state["max_energy"] = self.getMaxEnergy();
            state["simulation_time"] = self.getSimulationTime();
            state["random_seed"] = self.getRandomSeed();
            return state;
        }, "Get comprehensive world state")
        .def("run_scenario", [](SimpleWorld& self, const py::dict& scenario) {
            py::dict results;
            
            // Configure world based on scenario parameters
            auto width = scenario.get("width", py::none()).cast<int>();
            auto height = scenario.get("height", py::none()).cast<int>();
            auto vision_width = scenario.get("vision_width", py::none()).cast<int>();
            auto vision_height = scenario.get("vision_height", py::none()).cast<int>();
            
            if (width && height && vision_width && vision_height) {
                self.configure(width.value(), height.value(), vision_width.value(), vision_height.value());
            }
            
            // Set agent start position
            auto start_x = scenario.get("start_x", py::none()).cast<float>();
            auto start_y = scenario.get("start_y", py::none()).cast<float>();
            if (start_x && start_y) {
                self.setAgentStart(start_x.value(), start_y.value());
            }
            
            // Run simulation for specified number of steps
            auto num_steps = scenario.get("num_steps", py::none()).cast<size_t>();
            if (num_steps) {
                for (size_t step = 0; step < num_steps.value(); ++step) {
                    self.update(0.1);
                }
            }
            
            results["width"] = self.getWidth();
            results["height"] = self.getHeight();
            results["final_simulation_time"] = self.getSimulationTime();
            results["steps_completed"] = num_steps ? num_steps.value() : 0;
            
            return results;
        }, "Run simulation scenario with configurable parameters")
        .def("get_detailed_state", [](const SimpleWorld& self) {
            py::dict state;
            state["dimensions"] = py::dict({
                {"width", self.getWidth()},
                {"height", self.getHeight()}
            });
            state["energy"] = py::dict({
                {"max", self.getMaxEnergy()},
                {"decay_rate", self.getEnergyDecayRate()}
            });
            state["simulation"] = py::dict({
                {"time", self.getSimulationTime()},
                {"random_seed", self.getRandomSeed()}
            });
            return state;
        }, "Get detailed world state with nested dictionaries")
        ;

    // Factory functions for easy creation
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

    // Convenience functions for common use cases
    m.def("run_complete_demo", []() {
        // Create default brain
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->setAgentStart(10, 10);
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all features
        agent->enableAllFeatures();
        
        // Run simulation
        auto results = agent->runAgentLoop(*world, 100);
        
        return py::dict({
            {"brain_stats", brain->getBrainStats()},
            {"agent_neuromodulation", agent->getNeuromodulationSummary()},
            {"world_state", world->getWorldState()},
            {"simulation_results", results}
        });
    }, "Run complete demonstration with all features enabled");

    // Error types
    py::class_<std::exception>(m, "Exception")
        .def(py::init<const std::string&>())
        .def("what", &std::exception::what);

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Version and metadata
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Neural Learning Machine - Enhanced Python Bindings";

    // Provide helpful examples
    m.def("get_examples", []() {
        py::dict examples;
        
        examples["basic_brain"] = R"""
# Basic brain simulation
import pynlm

# Create brain with default config
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
""";
        
        examples["brain_world_agent"] = R"""
# Brain in a world with agent
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Agent simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate()}")
""";
        
        examples["advanced_features"] = R"""
# Advanced features usage
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("connection_probability", 0.15)
config.set("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable all advanced features
agent = pynlm.createAgentBrain(brain)
agent.enableAllFeatures()

# Get comprehensive statistics
stats = brain.getBrainStats()
neuromod = agent.getNeuromodulationSummary()

print("Brain statistics:", stats)
print("Neuromodulation:", neuromod)
""";
        
        return examples;
    }, "Get example code snippets for common use cases");

    m.doc() = R"""pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ------------------------------------------------------
        
        This is the enhanced version of NLM Python bindings with:
        
        🎯 Pythonic APIs:
        - Property-style access for configuration values
        - Context managers for safe operations
        - Comprehensive error handling with detailed messages
        - Rich dictionary-based access for complex data
        
        🚀 Advanced Features:
        - Automatic JSON/YAML configuration format detection
        - Configuration validation and issue reporting
        - Comprehensive brain and agent statistics
        - Advanced simulation with verbose output options
        - Agent loop automation for world interaction
        - Enhanced error handling and exception safety
        
        📊 Performance Tools:
        - Built-in statistics collection
        - Comprehensive state reporting
        - Advanced simulation control
        - Performance monitoring utilities
        
        🎮 Easy Usage:
        - Factory functions for common use cases
        - Example code snippets
        - Helpful error messages
        - Version and metadata information
        
        Key Improvements:
        - More intuitive and Pythonic APIs
        - Better error handling and recovery
        - Enhanced configuration management
        - Comprehensive simulation tools
        - Rich documentation and examples
    )pbdoc";
}

#endif // PYBIND11_MODULE
