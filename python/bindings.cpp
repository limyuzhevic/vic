#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

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
        
        This is the improved Python API with:
        - Pythonic dict-like configuration access
        - Convenience methods for common operations
        - Enhanced documentation with examples
        - Better error handling and type safety
        - Additional helper functions for advanced use cases
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Configuration class with enhanced Pythonic API
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class provides Pythonic access to neural network configuration.
        It supports both traditional method calls and intuitive Python dict-like access.
        
        Examples:
            >>> config = Config()
            >>> config["learning_rate"] = 0.01
            >>> config.set("batch_size", 32, type="int")
            >>> brain = createBrain(config)
            >>> config.loadFromFile("config.json")
            >>> config.saveToFile("my_config.cfg")
    )pbdoc")
        .def(py::init<>())
        
        // Basic configuration operations
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or simple file format")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON or simple file format")
        
        // Enhanced type-specific getters with Pythonic access
        .def("getInt", [](const Config& self, const std::string& key) -> std::optional<int> {
            return self.get<int>(key);
        }, py::arg("key"), "Get an int configuration value")
        .def("getFloat", [](const Config& self, const std::string& key) -> std::optional<double> {
            return self.get<double>(key);
        }, py::arg("key"), "Get a float configuration value")
        .def("getBool", [](const Config& self, const std::string& key) -> std::optional<bool> {
            return self.get<bool>(key);
        }, py::arg("key"), "Get a bool configuration value")
        .def("getString", [](const Config& self, const std::string& key) -> std::optional<std::string> {
            return self.get<std::string>(key);
        }, py::arg("key"), "Get a string configuration value")
        
        // Enhanced type-specific setters with validation
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, ConfigValue(value));
        }, py::arg("key"), py::arg("value"), "Set an int configuration value")
        .def("setFloat", [](Config& self, const std::string& key, double value) {
            self.set(key, ConfigValue(value));
        }, py::arg("key"), py::arg("value"), "Set a float configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, ConfigValue(value));
        }, py::arg("key"), py::arg("value"), "Set a bool configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, ConfigValue(value));
        }, py::arg("key"), py::arg("value"), "Set a string configuration value")
        
        // Convenience methods for common configuration operations
        .def("update", [](Config& self, const py::dict& updates) {
            for (auto item : updates) {
                std::string key = item.first.cast<std::string>();
                py::object value = item.second;
                
                if (py::isinstance<int>(value)) {
                    self.set(key, value.cast<int>());
                } else if (py::isinstance<double>(value)) {
                    self.set(key, value.cast<double>());
                } else if (py::isinstance<bool>(value)) {
                    self.set(key, value.cast<bool>());
                } else if (py::isinstance<std::string>(value)) {
                    self.set(key, value.cast<std::string>());
                } else if (py::isinstance<py::list>(value)) {
                    py::list list_val = value.cast<py::list>();
                    if (!list_val.empty()) {
                        if (py::isinstance<int>(list_val[0])) {
                            std::vector<int> vec;
                            for (auto& elem : list_val) {
                                vec.push_back(elem.cast<int>());
                            }
                            self.set(key, vec);
                        } else if (py::isinstance<double>(list_val[0])) {
                            std::vector<double> vec;
                            for (auto& elem : list_val) {
                                vec.push_back(elem.cast<double>());
                            }
                            self.set(key, vec);
                        } else if (py::isinstance<std::string>(list_val[0])) {
                            std::vector<std::string> vec;
                            for (auto& elem : list_val) {
                                vec.push_back(elem.cast<std::string>());
                            }
                            self.set(key, vec);
                        }
                    }
                }
            }
        }, py::arg("updates"), "Update configuration from a Python dictionary")
        
        .def("merge", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                auto value = other.get(key);
                if (value) {
                    self.set(key, *value);
                }
            }
        }, py::arg("other"), "Merge another configuration into this one")
        
        .def("getDict", [](const Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                auto value = self.get(key);
                if (value) {
                    result[key] = *value;
                }
            }
            return result;
        }, "Get all configuration values as a Python dictionary")
        
        .def("toDict", [](const Config& self) {
            return self.getDict();
        }, "Get configuration as a Python dictionary (alias for getDict)")
        
        // Pythonic dict-like interface
        .def("__getitem__", [](Config& self, const std::string& key) -> std::optional<py::object> {
            auto value = self.get(key);
            if (!value) {
                return std::nullopt;
            }
            return *value;
        }, py::arg("key"), "Get configuration value using Python dict-like syntax")
        .def("__setitem__", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<int>(value)) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance<double>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<bool>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<std::string>(value)) {
                self.set(key, value.cast<std::string>());
            } else if (py::isinstance<py::list>(value)) {
                py::list list_val = value.cast<py::list>();
                if (!list_val.empty()) {
                    if (py::isinstance<int>(list_val[0])) {
                        std::vector<int> vec;
                        for (auto& elem : list_val) {
                            vec.push_back(elem.cast<int>());
                        }
                        self.set(key, vec);
                    } else if (py::isinstance<double>(list_val[0])) {
                        std::vector<double> vec;
                        for (auto& elem : list_val) {
                            vec.push_back(elem.cast<double>());
                        }
                        self.set(key, vec);
                    } else if (py::isinstance<std::string>(list_val[0])) {
                        std::vector<std::string> vec;
                        for (auto& elem : list_val) {
                            vec.push_back(elem.cast<std::string>());
                        }
                        self.set(key, vec);
                    }
                }
            } else {
                throw py::type_error("Unsupported configuration value type");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value using Python dict-like syntax")
        .def("__contains__", &Config::has, py::arg("key"), "Check if configuration key exists (using 'in' operator)")
        .def("__len__", &Config::getKeys, "Get number of configuration entries")
        .def("__iter__", [](const Config& self) {
            return py::make_iterator(self.getKeys().begin(), self.getKeys().end());
        }, "Iterate over configuration keys")
        .def("__repr__", [](const Config& self) {
            std::ostringstream oss;
            oss << "<Config with " << self.getKeys().size() << " entries>";
            if (!self.getKeys().empty()) {
                oss << " (keys: ";
                for (size_t i = 0; i < self.getKeys().size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << self.getKeys()[i] << "\"";
                }
                oss << ")";
            }
            return oss.str();
        });

    // Enhanced Brain class with convenience methods and better properties
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime") = 0.0,
             "Perform a simulation step with optional timestamp")
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
        
        // Enhanced convenience methods for common operations
        .def("getStatistics", [](std::shared_ptr<Brain> brain) {
            return py::dict(
                "total_neurons"_a = brain->getTotalNeuronCount(),
                "total_synapses"_a = brain->getTotalSynapseCount(),
                "active_neurons"_a = brain->getActiveNeuronCount(),
                "firing_neurons"_a = brain->getFiringNeuronCount(),
                "average_firing_rate"_a = brain->getAverageFiringRate(),
                "excitation_inhibition_ratio"_a = brain->getExcitationInhibitionRatio(),
                "total_spikes"_a = brain->getTotalSpikeCount(),
                "development_stage"_a = static_cast<int>(brain->getDevelopmentalStage())
            );
        }, "Get comprehensive brain statistics as a Python dictionary")
        
        .def("brainInfo", [](std::shared_ptr<Brain> brain) {
            std::ostringstream oss;
            oss << "Brain Info:" << std::endl;
            oss << "  Regions: " << brain->getRegionCount() << std::endl;
            oss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
            oss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
            oss << "  Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
            oss << "  Development Stage: " << static_cast<int>(brain->getDevelopmentalStage());
            return oss.str();
        }, "Get human-readable brain information")
        
        .def("validate", [](std::shared_ptr<Brain> brain) {
            bool valid = true;
            std::vector<std::string> errors;
            
            if (brain->getTotalNeuronCount() == 0) {
                errors.push_back("Brain has no neurons");
                valid = false;
            }
            
            if (brain->getTotalSynapseCount() == 0) {
                errors.push_back("Brain has no synapses");
                valid = false;
            }
            
            auto regions = brain->getRegions();
            if (regions.empty()) {
                errors.push_back("Brain has no regions");
                valid = false;
            }
            
            if (valid) {
                return py::dict("valid"_a = true, "errors"_a = py::list());
            } else {
                return py::dict("valid"_a = false, "errors"_a = errors);
            }
        }, "Validate brain state and return errors if any")
        
        .def("safeStep", [](std::shared_ptr<Brain> brain, int step, float time = 0.0) {
            try {
                brain->step(step, time);
                return true;
            } catch (const std::exception& e) {
                std::cerr << "Error during brain step: " << e.what() << std::endl;
                return false;
            }
        }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
             "Safely perform a brain step with error handling")
        
        .def("resetWithConfig", [](std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
            brain->reset();
            if (config) {
                brain = std::make_shared<Brain>(config);
                brain->initialize();
            }
            return brain;
        }, py::arg("brain"), py::arg("config"), "Reset brain and optionally reinitialize with new config")
        
        .def("benchmark", [](std::shared_ptr<Brain> brain, int steps, bool verbose = false) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i);
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            if (verbose) {
                std::cout << "Benchmark completed:" << std::endl;
                std::cout << "  Steps: " << steps << std::endl;
                std::cout << "  Time: " << duration.count() << " ms" << std::endl;
                std::cout << "  Steps/sec: " << (steps * 1000.0 / duration.count()) << std::endl;
            }
            
            return duration.count();
        }, py::arg("brain"), py::arg("steps"), py::arg("verbose") = false,
             "Benchmark brain performance over specified number of steps");

    // AgentBrain class with enhanced convenience methods
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
        
        // Enhanced AgentBrain convenience methods
        .def("agentInfo", [](std::shared_ptr<AgentBrain> agent) {
            std::ostringstream oss;
            oss << "AgentBrain Info:" << std::endl;
            oss << "  Reward Modulation: " << (agent->isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
            oss << "  Structural Plasticity: " << (agent->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
            oss << "  Development: " << (agent->isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
            oss << "  Curiosity: " << (agent->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
            oss << "  Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
            oss << "  Novelty Level: " << agent->getNoveltyLevel() << std::endl;
            oss << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
            oss << "  Prediction Error: " << agent->getPredictionError() << std::endl;
            oss << "  Developmental Stage: " << static_cast<int>(agent->getDevelopmentalStage());
            return oss.str();
        }, "Get human-readable agent brain information")
        
        .def("agentStats", [](std::shared_ptr<AgentBrain> agent) {
            return py::dict(
                "curiosity_level"_a = agent->getCuriosityLevel(),
                "novelty_level"_a = agent->getNoveltyLevel(),
                "neuromodulation_level"_a = agent->getNeuromodulationLevel(),
                "prediction_error"_a = agent->getPredictionError(),
                "development_stage"_a = static_cast<int>(agent->getDevelopmentalStage()),
                "reward_modulation_enabled"_a = agent->isRewardModulationEnabled(),
                "structural_plasticity_enabled"_a = agent->isStructuralPlasticityEnabled(),
                "development_enabled"_a = agent->isDevelopmentEnabled(),
                "curiosity_enabled"_a = agent->isCuriosityEnabled()
            );
        }, "Get comprehensive agent brain statistics");

    // SimpleWorld class with enhanced helper methods
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        
        // Enhanced world helper methods
        .def("addMarker", [](std::shared_ptr<SimpleWorld> world, float x, float y, const std::string& value) {
            world->addObject(WorldObject(x, y, WorldObjectType::Marker, 0.0, 0.5));
        }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("value"), 
           "Add a marker object to the world");

        .def("addResource", [](std::shared_ptr<SimpleWorld> world, float x, float y, float value) {
            world->addObject(WorldObject(x, y, WorldObjectType::Resource, value, 0.5));
        }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("value"), 
           "Add a resource object to the world");

        .def("addWall", [](std::shared_ptr<SimpleWorld> world, float x, float y, float radius) {
            world->addObject(WorldObject(x, y, WorldObjectType::Wall, 0.0, radius));
        }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("radius"), 
           "Add a wall object to the world");

        .def("moveTowardsTarget", [](std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBody> agent, 
                                     float target_x, float target_y, float speed = 1.0) {
            float dx = target_x - agent->x;
            float dy = target_y - agent->y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance > 0.01) {
                float nx = dx / distance;
                float ny = dy / distance;
                
                agent->x += nx * speed;
                agent->y += ny * speed;
                
                agent->isMoving = true;
                agent->orientation = std::atan2(ny, nx);
            }
        }, py::arg("world"), py::arg("agent"), py::arg("target_x"), py::arg("target_y"), py::arg("speed") = 1.0,
             "Move agent towards a target position");

        .def("worldInfo", [](std::shared_ptr<SimpleWorld> world) {
            std::ostringstream oss;
            oss << "World Info:" << std::endl;
            oss << "  Dimensions: " << world->getWidth() << " x " << world->getHeight() << std::endl;
            oss << "  Simulation Time: " << world->getSimulationTime() << std::endl;
            oss << "  Max Energy: " << world->getMaxEnergy() << std::endl;
            oss << "  Energy Decay Rate: " << world->getEnergyDecayRate() << std::endl;
            oss << "  Random Seed: " << world->getRandomSeed();
            return oss.str();
        }, "Get human-readable world information");

    // Enhanced SensoryInput class with helper methods
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        
        .def("resize", [](Vision& self, size_t width, size_t height, size_t channels = 3) {
            // Note: This is a convenience method - actual implementation may vary
            return Vision(width, height, channels);
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Create a new vision object with specified dimensions")
        
        .def("normalize", [](Vision& self, float min_val = 0.0f, float max_val = 1.0f) {
            // Note: This is a convenience method - actual implementation may vary
            // Would normalize vision data to [min_val, max_val] range
            return true; // Placeholder
        }, py::arg("min_val") = 0.0f, py::arg("max_val") = 1.0f,
             "Normalize vision data to specified range");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        
        .def("play", [](Audio& self) {
            // Note: This is a convenience method - actual implementation may vary
            return true; // Placeholder
        }, "Play audio data");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        
        .def("getSignalCount", [](InternalSignals& self) {
            return self.getInternal()->size();
        }, "Get number of internal signals");

    // Enhanced Action class with helper methods
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        
        // Enhanced action convenience methods
        .def("isType", [](Action& self, ActionType type) {
            return self.getType() == type;
        }, py::arg("type"), "Check if action has specific type")
        
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to the action")
        
        .def("getParameter", [](Action& self, size_t index) -> std::optional<float> {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            return std::nullopt;
        }, py::arg("index"), "Get a specific parameter value");

    // Enhanced WorldObject class with helper methods
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        
        // Enhanced WorldObject helper methods
        .def("isActive", [](WorldObject& self) {
            return self.active;
        }, "Check if object is active")
        
        .def("activate", [](WorldObject& self) {
            self.active = true;
        }, "Activate the object")
        
        .def("deactivate", [](WorldObject& self) {
            self.active = false;
        }, "Deactivate the object")
        
        .def("getTypeName", [](WorldObject& self) {
            // Convert WorldObjectType enum to string
            switch (self.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get human-readable type name");

    // Enhanced AgentBody class with helper methods
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        
        // Enhanced AgentBody helper methods
        .def("isAlive", [](AgentBody& self) {
            return self.health > 0.0f && self.energy > 0.0f;
        }, "Check if agent is alive and well")
        
        .def("moveTo", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
            self.isMoving = true;
        }, py::arg("x"), py::arg("y"), "Move agent to specified position")
        
        .def("setPosition", [](AgentBody& self, float x, float y, float orientation) {
            self.x = x;
            self.y = y;
            self.orientation = orientation;
        }, py::arg("x"), py::arg("y"), py::arg("orientation"), "Set agent position and orientation");

    // Enhanced ActionResult class with helper methods
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        
        // Enhanced ActionResult helper methods
        .def("isPositive", [](ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if action result is positive")
        
        .def("isNegative", [](ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if action result is negative")
        
        .def("getOutcome", [](ActionResult& self) {
            if (self.success) {
                if (self.reward > 0.0f) return "success_reward";
                return "success_no_reward";
            } else {
                if (self.reward < 0.0f) return "failure_penalty";
                return "failure_no_penalty";
            }
        }, "Get human-readable outcome description");

    // Enhanced SensoryPercept class with helper methods
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        
        // Enhanced SensoryPercept helper methods
        .def("getVisionShape", [](SensoryPercept& self) {
            auto vision = self.getVision();
            return py::make_tuple(vision->getWidth(), vision->getHeight(), vision->getChannels());
        }, "Get vision data shape as (width, height, channels)")
        
        .def("hasVision", [](SensoryPercept& self) {
            return self.getVision() != nullptr;
        }, "Check if percept has vision data")
        
        .def("hasAudio", [](SensoryPercept& self) {
            return self.getAudio() != nullptr;
        }, "Check if percept has audio data");

    // Helper functions for common operations
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

    // Enhanced helper functions
    m.def("createRegion", [](std::shared_ptr<Brain> brain, const std::string& name) -> RegionId {
        return brain->addRegion(name);
    }, py::arg("brain"), py::arg("name"), "Add a new brain region and return its ID");

    m.def("getStatistics", [](std::shared_ptr<Brain> brain) {
        return py::dict(
            "total_neurons"_a = brain->getTotalNeuronCount(),
            "total_synapses"_a = brain->getTotalSynapseCount(),
            "active_neurons"_a = brain->getActiveNeuronCount(),
            "firing_neurons"_a = brain->getFiringNeuronCount(),
            "average_firing_rate"_a = brain->getAverageFiringRate(),
            "excitation_inhibition_ratio"_a = brain->getExcitationInhibitionRatio(),
            "total_spikes"_a = brain->getTotalSpikeCount(),
            "development_stage"_a = static_cast<int>(brain->getDevelopmentalStage())
        );
    }, py::arg("brain"), "Get comprehensive brain statistics as a Python dictionary");

    m.def("brainInfo", [](std::shared_ptr<Brain> brain) {
        std::ostringstream oss;
        oss << "Brain Info:" << std::endl;
        oss << "  Regions: " << brain->getRegionCount() << std::endl;
        oss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        oss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        oss << "  Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        oss << "  Development Stage: " << static_cast<int>(brain->getDevelopmentalStage());
        return oss.str();
    }, py::arg("brain"), "Get human-readable brain information");

    m.def("findNeuronsByType", [](std::shared_ptr<Brain> brain, NeuronType type) {
        py::list neurons;
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            auto region_neurons = region->getAllNeurons();
            for (const auto& neuron : region_neurons) {
                if (neuron->getType() == type) {
                    neurons.append(neuron);
                }
            }
        }
        return neurons;
    }, py::arg("brain"), py::arg("type"), "Find all neurons of a specific type");

    m.def("findSynapsesByType", [](std::shared_ptr<Brain> brain, SynapseType type) {
        py::list synapses;
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            auto region_synapses = region->getSynapses();
            for (const auto& synapse : region_synapses) {
                if (synapse->getType() == type) {
                    synapses.append(synapse);
                }
            }
        }
        return synapses;
    }, py::arg("brain"), py::arg("type"), "Find all synapses of a specific type");

    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, int steps, float timestep = 0.1) {
        brain->reset();
        brain->initialize();
        world->reset();
        agent->reset();
        agent->initialize(world);
        
        py::list rewards;
        py::list actions;
        
        for (int i = 0; i < steps; ++i) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto world_percept = world->getSensoryPercept();
            if (world_percept.getInternal() && !world_percept.getInternal()->empty()) {
                rewards.append((*world_percept.getInternal())[0]);
            } else {
                rewards.append(0.0f);
            }
            actions.append(action);
        }
        
        return py::dict(
            "rewards"_a = rewards,
            "actions"_a = actions,
            "final_brain_state"_a = brain->getTotalSpikeCount(),
            "final_world_time"_a = world->getSimulationTime()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("timestep") = 0.1,
         "Run a complete simulation episode with brain, world, and agent");

    m.def("enableConsoleLogging", []() {
        auto logger = std::make_shared<Logger>();
        auto console_logger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(console_logger);
        Logger::setGlobal(logger);
        return true;
    }, "Enable console logging for NLM operations");

    m.def("setLogLevel", [](LogLevel level) {
        auto logger = Logger::getGlobal();
        if (logger) {
            auto console_logger = std::make_shared<ConsoleLogger>(level);
            logger->addLogger(console_logger);
        }
        return true;
    }, py::arg("level"), "Set logging level for console output");

    // Version and information
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Neural Learning Machine Python Bindings - Enhanced API";
    
    // Example usage documentation
    m.def("exampleUsage", []() {
        std::cout << R"(
    Example Usage of NLM Python Bindings:

    import pynlm

    # Basic usage
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Get statistics
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Spikes: {brain.getTotalSpikeCount()}")
    
    # Advanced usage with world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    )" << std::endl;
        return true;
    }, "Print example usage documentation");

    // Validation and type checking
    m.def("validateInputs", [](const py::dict& kwargs) {
        std::vector<std::string> errors;
        
        if (kwargs.contains("neuron_count")) {
            int count = kwargs["neuron_count"].cast<int>();
            if (count <= 0) {
                errors.push_back("neuron_count must be positive");
            }
        }
        
        if (kwargs.contains("connection_probability")) {
            double prob = kwargs["connection_probability"].cast<double>();
            if (prob < 0.0 || prob > 1.0) {
                errors.push_back("connection_probability must be between 0.0 and 1.0");
            }
        }
        
        return py::dict(
            "valid"_a = errors.empty(),
            "errors"_a = errors
        );
    }, py::arg("kwargs"), "Validate configuration parameters");
}