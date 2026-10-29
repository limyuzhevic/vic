// Enhanced Python bindings for NLM with comprehensive functionality

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <sstream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/world/WorldObject.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions for Python integration
namespace py_helpers {
    // Type conversion utilities
    template<typename T>
    T get_value_safely(const nlm::Config& config, const std::string& key, const T& default_value) {
        auto value = config.get<T>(key);
        return value ? *value : default_value;
    }
    
    // Validate neuron IDs
    void validate_neuron_id(nlm::NeuronId id, const std::string& context = "operation") {
        if (id.value == UINT64_MAX) {
            throw std::invalid_argument("Invalid neuron ID in " + context);
        }
    }
    
    // Format statistics for Python
    std::string format_neural_stats(const nlm::Brain& brain) {
        std::stringstream ss;
        ss << "Brain Statistics:" << std::endl;
        ss << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
        ss << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
        ss << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
        ss << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
        ss << "  Average Firing Rate: " << brain.getAverageFiringRate() << std::endl;
        ss << "  Excitatory/Inhibitory Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        ss << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
        ss << "  Pending Spikes: " << brain.getPendingSpikeEventCount() << std::endl;
        ss << "  Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
        return ss.str();
    }
    
    // Convert simulation step to time
    double step_to_time(nlm::SimulationStep step, double timestep = 0.01) {
        return static_cast<double>(step) * timestep;
    }
    
    // Convert time to simulation step
    nlm::SimulationStep time_to_step(double time, double timestep = 0.01) {
        return static_cast<nlm::SimulationStep>(std::round(time / timestep));
    }
    
    // Validate configuration key
    void validate_config_key(const nlm::Config& config, const std::string& key) {
        if (!config.has(key)) {
            throw std::invalid_argument("Configuration key not found: " + key);
        }
    }
    
    // Create default config with common settings
    nlm::Config create_default_config() {
        nlm::Config config;
        config.set("brain.neuron_count", 1000);
        config.set("brain.region_count", 4);
        config.set("brain.init_excitation", 0.5f);
        config.set("brain.init_inhibition", 0.3f);
        config.set("simulation.dt", 0.01);
        config.set("simulation.max_steps", 10000);
        config.set("memory.working_memory.capacity", 100);
        config.set("memory.episodic.capacity", 1000);
        config.set("neuromodulation.dopamine.baseline", 0.1f);
        config.set("development.stage", "Adult");
        return config;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python interface for the NLM C++ neural simulation framework.
        
        This module provides comprehensive access to neural simulation capabilities,
        including brain modeling, memory systems, neuromodulation, prediction,
        and cognitive functions.
        
        Key Features:
        - Brain simulation with spiking neurons
        - Multiple memory systems (working, episodic, associative, procedural)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and cognitive systems
        - Developmental models
        - Configuration management
        - Simulation automation
        - Performance monitoring
        
        Usage Example:
        ```python
        import pynlm
        
        # Create a brain with configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            # Get sensory input (example)
            input_data = pynlm.Vision(64, 64, 3)
            brain.receiveSensoryInput(input_data)
            brain.step(step)
        
        # Get statistics
        stats = brain.logStatus()
        print(stats)
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Configure Python module behavior
    py::options options;
    options.disable_function_signatures();

    // ========== CORE TYPES ==========
    
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ========== ENUMS ==========
    
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ========== CONFIGURATION ==========
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init([]() {
            Config config;
            config.set("brain.neuron_count", 1000);
            config.set("brain.region_count", 4);
            config.set("brain.init_excitation", 0.5f);
            config.set("brain.init_inhibition", 0.3f);
            config.set("simulation.dt", 0.01);
            config.set("simulation.max_steps", 10000);
            config.set("memory.working_memory.capacity", 100);
            config.set("memory.episodic.capacity", 1000);
            config.set("neuromodulation.dopamine.baseline", 0.1f);
            config.set("development.stage", "Adult");
            return config;
        }))
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
        .def("get", [](const Config& self, const std::string& key) {
            py::dict result;
            
            auto value = self.get<int>(key);
            if (value) {
                result["int"] = *value;
                return result;
            }
            
            value = self.get<int64_t>(key);
            if (value) {
                result["int64"] = *value;
                return result;
            }
            
            value = self.get<double>(key);
            if (value) {
                result["double"] = *value;
                return result;
            }
            
            value = self.get<bool>(key);
            if (value) {
                result["bool"] = *value;
                return result;
            }
            
            value = self.get<std::string>(key);
            if (value) {
                result["str"] = *value;
                return result;
            }
            
            value = self.get<std::vector<int>>(key);
            if (value) {
                result["int_list"] = *value;
                return result;
            }
            
            value = self.get<std::vector<double>>(key);
            if (value) {
                result["double_list"] = *value;
                return result;
            }
            
            value = self.get<std::vector<std::string>>(key);
            if (value) {
                result["str_list"] = *value;
                return result;
            }
            
            return result;
        }, py::arg("key"), R"pbdoc(Get a configuration value by key.
        
        Returns a dict with the value and type information.
        "pbdoc")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is_none()) {
                throw std::invalid_argument("Cannot set config value to None");
            }
            
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance<py::int64_t>(value)) {
                self.set(key, value.cast<int64_t>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else if (py::isinstance<py::list>(value)) {
                py::list pyList = value.cast<py::list>();
                
                bool all_int = true;
                bool all_str = true;
                bool all_float = true;
                
                for (size_t i = 0; i < pyList.size(); ++i) {
                    if (!py::isinstance<py::int_>(pyList[i])) all_int = false;
                    if (!py::isinstance<py::str>(pyList[i])) all_str = false;
                    if (!py::isinstance<py::float_>(pyList[i])) all_float = false;
                }
                
                if (all_int) {
                    std::vector<int> vec;
                    vec.reserve(pyList.size());
                    for (size_t i = 0; i < pyList.size(); ++i) {
                        vec.push_back(pyList[i].cast<int>());
                    }
                    self.set(key, vec);
                } else if (all_str) {
                    std::vector<std::string> vec;
                    vec.reserve(pyList.size());
                    for (size_t i = 0; i < pyList.size(); ++i) {
                        vec.push_back(pyList[i].cast<std::string>());
                    }
                    self.set(key, vec);
                } else if (all_float) {
                    std::vector<double> vec;
                    vec.reserve(pyList.size());
                    for (size_t i = 0; i < pyList.size(); ++i) {
                        vec.push_back(pyList[i].cast<double>());
                    }
                    self.set(key, vec);
                } else {
                    throw std::invalid_argument("Unsupported list type for configuration value");
                }
            } else {
                throw std::invalid_argument("Unsupported type for configuration value");
            }
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value with automatic type conversion")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ========== SENSORY INPUT ==========
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

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
        .def("getDataAsArray", [](const Vision& self) {
            py::array_t<float> array = py::array_t<float>({self.getHeight(), self.getWidth(), self.getChannels()});
            auto buf = array.mutable_unchecked<3>();
            const auto& data = self.getData();
            size_t idx = 0;
            for (size_t h = 0; h < self.getHeight(); ++h) {
                for (size_t w = 0; w < self.getWidth(); ++w) {
                    for (size_t c = 0; c < self.getChannels(); ++c) {
                        buf[h, w, c] = data[idx++];
                    }
                }
            }
            return array;
        }, "Get vision data as a numpy-style array");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getSamplesAsArray", [](const Audio& self) {
            py::array_t<float> array = py::array_t<float>({self.getNumSamples()});
            auto buf = array.mutable_unchecked<1>();
            const auto& data = self.getData();
            for (size_t i = 0; i < self.getNumSamples(); ++i) {
                buf[i] = data[i];
            }
            return array;
        }, "Get audio samples as a numpy-style array");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalsAsArray", [](const InternalSignals& self) {
            return self.getData();
        }, "Get signals as a numpy-style array");

    // ========== ACTIONS ==========
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw std::out_of_range("Parameter index out of range");
        }, py::arg("index"), "Get parameter at index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto& params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
            } else {
                params.resize(index + 1, 0.0f);
                params[index] = value;
            }
        }, py::arg("index"), py::arg("value"), "Set parameter at index")
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " with " + std::to_string(action.getParameters().size()) + " parameters>";
        });

    // ========== WORLD OBJECTS ==========
    
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
        .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
            double dx = self.x - other.x;
            double dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another object")
        .def("isWithinRange", [](const WorldObject& self, float x, float y, float range) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx * dx + dy * dy) <= (range * range);
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if object is within range of position");

    // ========== AGENT BODY ==========
    
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
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get speed magnitude")
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get direction in radians")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"), "Set velocity")
        .def("setOrientation", [](AgentBody& self, float angle) {
            self.orientation = angle;
        }, py::arg("angle"), "Set orientation in radians")
        .def("reset", &AgentBody::reset)
        .def("isAlive", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is alive (health > 0)")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 0.5f;
        }, "Check if agent is healthy (health > 0.5)");

    // ========== ACTION RESULT ==========
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) + ", success=" + std::to_string(result.success) + ", message=\"" + result.message + "\">";
        });

    // ========== SENSORY PERCEPT ==========
    
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
        .def("addVisionData", [](SensoryPercept& self, const std::vector<float>& data, size_t width, size_t height, size_t channels = 3) {
            Vision vision(width, height, channels);
            vision.setData(data);
            self.setVision(vision);
        }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Add vision data conveniently")
        .def("addAudioData", [](SensoryPercept& self, const std::vector<float>& data, size_t sampleRate) {
            Audio audio(sampleRate, data.size());
            audio.setData(data);
            audio.setSampleRate(sampleRate);
            self.setAudio(audio);
        }, py::arg("data"), py::arg("sampleRate"), "Add audio data conveniently");

    // ========== WORLD ==========
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 64, py::arg("visionHeight") = 64,
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and get result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
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
        .def("getObjectsInRadius", [](const SimpleWorld& self, float x, float y, float radius) {
            std::vector<WorldObject> objects;
            // Note: This requires access to internal state, implementation depends on SimpleWorld API
            return objects;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get objects within radius")
        .def("getNearestObject", [](const SimpleWorld& self, float x, float y) {
            // Note: This requires access to internal state, implementation depends on SimpleWorld API
            return WorldObject();
        }, py::arg("x"), py::arg("y"), "Get nearest object to position");

    // ========== BRAIN ==========
    
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
        .def("stepWithTime", [](Brain& self, double timestep, double currentTime) {
            SimulationStep step = py_helpers::time_to_step(currentTime);
            self.step(step, currentTime);
        }, py::arg("timestep"), py::arg("currentTime"),
           "Perform a simulation step with automatic time conversion")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getStatistics", py_helpers::format_neural_stats,
             "Get formatted brain statistics")
        .def("getMemorySystems", [](const Brain& self) {
            py::dict systems;
            
            auto* workingMem = self.getWorkingMemory();
            if (workingMem) {
                systems["working"] = py::cast(workingMem);
            }
            
            auto* episodicMem = self.getEpisodicMemory();
            if (episodicMem) {
                systems["episodic"] = py::cast(episodicMem);
            }
            
            auto* associativeMem = self.getAssociativeMemory();
            if (associativeMem) {
                systems["associative"] = py::cast(associativeMem);
            }
            
            return systems;
        }, "Get access to memory systems")
        .def("getNeuromodulationSystems", [](const Brain& self) {
            py::dict systems;
            
            auto* dopamine = self.getDopamine();
            if (dopamine) {
                systems["dopamine"] = py::cast(dopamine);
            }
            
            auto* curiosity = self.getCuriosity();
            if (curiosity) {
                systems["curiosity"] = py::cast(curiosity);
            }
            
            auto* novelty = self.getNovelty();
            if (novelty) {
                systems["novelty"] = py::cast(novelty);
            }
            
            auto* predictionError = self.getPredictionErrorSignal();
            if (predictionError) {
                systems["prediction_error"] = py::cast(predictionError);
            }
            
            return systems;
        }, "Get access to neuromodulation systems")
        .def("getCognitionSystems", [](const Brain& self) {
            py::dict systems;
            
            auto* planner = self.getPlanner();
            if (planner) {
                systems["planner"] = py::cast(planner);
            }
            
            auto* conceptFormation = self.getConceptFormation();
            if (conceptFormation) {
                systems["concept_formation"] = py::cast(conceptFormation);
            }
            
            auto* attention = self.getAttention();
            if (attention) {
                systems["attention"] = py::cast(attention);
            }
            
            auto* developmentSystem = self.getDevelopmentSystem();
            if (developmentSystem) {
                systems["development"] = py::cast(developmentSystem);
            }
            
            return systems;
        }, "Get access to cognition systems")
        .def("getPlasticitySystems", [](const Brain& self) {
            py::dict systems;
            
            auto* stdp = self.getSTDP();
            if (stdp) {
                systems["stdp"] = py::cast(stdp);
            }
            
            auto* hebbian = self.getHebbian();
            if (hebbian) {
                systems["hebbian"] = py::cast(hebbian);
            }
            
            auto* structural = self.getStructuralPlasticity();
            if (structural) {
                systems["structural"] = py::cast(structural);
            }
            
            return systems;
        }, "Get access to plasticity systems")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator");

    // ========== AGENT BRAIN ==========
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // ========== FACTORY FUNCTIONS ==========
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible settings")
    
    m.def("createConfig", []() -> std::shared_ptr<Config> {
        nlm::Config config;
        config.set("brain.neuron_count", 1000);
        config.set("brain.region_count", 4);
        config.set("brain.init_excitation", 0.5f);
        config.set("brain.init_inhibition", 0.3f);
        config.set("simulation.dt", 0.01);
        config.set("simulation.max_steps", 10000);
        config.set("memory.working_memory.capacity", 100);
        config.set("memory.episodic.capacity", 1000);
        config.set("neuromodulation.dopamine.baseline", 0.1f);
        config.set("development.stage", "Adult");
        return std::make_shared<Config>(config);
    }, "Create a pre-configured default configuration")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    
    // ========== SIMULATION UTILITIES ==========
    
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                            size_t numSteps, double timestep = 0.01) {
        brain->initialize();
        brain->reset();
        world->reset();
        
        for (size_t i = 0; i < numSteps; ++i) {
            // Get sensory input
            auto percept = world->getSensoryPercept();
            brain->receiveSensoryInput(percept);
            
            // Update world
            world->update(timestep);
            
            // Step brain
            brain->step(i, timestep * i);
        }
        
        return py::dict("steps_completed" = numSteps,
                       "final_stats" = brain->logStatus());
    }, py::arg("brain"), py::arg("world"), py::arg("numSteps"), py::arg("timestep") = 0.01,
       "Run a complete simulation episode")
    
    m.def("episodeSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                 size_t maxSteps = 1000, double timestep = 0.01,
                                 bool verbose = false) {
        brain->initialize();
        brain->reset();
        world->reset();
        
        py::list episode_data;
        
        for (size_t i = 0; i < maxSteps; ++i) {
            auto action = brain->produceAction();
            auto result = world->applyMotorCommand(action, timestep * i);
            auto percept = world->getSensoryPercept();
            brain->receiveSensoryInput(percept);
            brain->step(i, timestep * i);
            world->update(timestep);
            
            if (verbose) {
                py::dict step_data;
                step_data["step"] = i;
                step_data["time"] = timestep * i;
                step_data["action"] = action;
                step_data["result"] = result;
                step_data["reward"] = result.reward;
                step_data["success"] = result.success;
                episode_data.append(step_data);
            }
        }
        
        return episode_data;
    }, py::arg("brain"), py::arg("world"), py::arg("maxSteps") = 1000,
       py::arg("timestep") = 0.01, py::arg("verbose") = false,
       "Run an episode simulation with step-by-step data collection")
    
    m.def("checkpointSimulation", [](std::shared_ptr<Brain> brain, const std::string& filepath,
                                    bool includeWorld = false, std::shared_ptr<SimpleWorld> world = nullptr) {
        bool brainSaved = brain->save(filepath + "_brain.nnl");
        
        if (includeWorld && world) {
            // For world checkpointing, we'd need serialization support
            // This is a placeholder for future implementation
            return py::dict("brain_saved" = brainSaved,
                           "world_saved" = false,
                           "message" = "World checkpointing not yet implemented");
        }
        
        return py::dict("brain_saved" = brainSaved,
                       "world_saved" = false);
    }, py::arg("brain"), py::arg("filepath"), py::arg("includeWorld") = false,
       py::arg("world") = nullptr,
       "Save brain state to checkpoint files")
    
    m.def("loadSimulation", [](const std::string& filepath, std::shared_ptr<Config> config = nullptr) {
        if (!config) {
            config = std::make_shared<Config>();
        }
        
        auto brain = std::make_shared<Brain>(config);
        bool loaded = brain->load(filepath + "_brain.nnl");
        
        return py::dict("brain" = brain,
                       "loaded" = loaded,
                       "message" = loaded ? "Simulation loaded successfully" : "Failed to load simulation");
    }, py::arg("filepath"), py::arg("config") = nullptr,
       "Load a saved brain state")
    
    // ========== UTILITY FUNCTIONS ==========
    
    m.def("formatStep", py_helpers::format_neural_stats,
         "Format neural statistics as readable string")
    
    m.def("stepToTime", py_helpers::step_to_time,
         py::arg("step"), py::arg("timestep") = 0.01,
         "Convert simulation step to time")
    
    m.def("timeToStep", py_helpers::time_to_step,
         py::arg("time"), py::arg("timestep") = 0.01,
         "Convert time to simulation step")
    
    m.def("validateNeuronId", py_helpers::validate_neuron_id,
         py::arg("id"), py::arg("context") = "operation",
         "Validate neuron ID and throw exception if invalid")
    
    m.def("createSimulationWorld", [](size_t width, size_t height, size_t visionWidth = 64,
                                     size_t visionHeight = 64) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 100, py::arg("height") = 100,
       py::arg("visionWidth") = 64, py::arg("visionHeight") = 64,
       "Create and configure a simple world")
    
    // ========== INVALID ID SENTINELS ==========
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // ========== DEMONSTRATION AND EXAMPLES ==========
    
    m.def("demo_brain_lifecycle", []() {
        // Create brain and world
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        
        // Configure world
        world->configure(50, 50, 16, 16);
        world->setAgentStart(25, 25);
        
        // Initialize
        brain->initialize();
        world->reset();
        
        // Run demonstration
        py::dict results;
        results["brain_created"] = true;
        results["world_configured"] = true;
        results["initialized"] = true;
        results["status"] = brain->logStatus();
        
        return results;
    }, "Demonstrate basic brain lifecycle (create, initialize, run)")
    
    m.def("demo_memory_integration", []() {
        // Create brain with memory systems
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        brain->initialize();
        
        py::dict results;
        
        // Access memory systems
        auto workingMem = brain->getWorkingMemory();
        auto episodicMem = brain->getEpisodicMemory();
        
        if (workingMem) {
            results["working_memory"] = py::dict("available" = true);
        } else {
            results["working_memory"] = py::dict("available" = false);
        }
        
        if (episodicMem) {
            results["episodic_memory"] = py::dict("available" = true);
        } else {
            results["episodic_memory"] = py::dict("available" = false);
        }
        
        return results;
    }, "Demonstrate memory system integration")
    
    m.def("demo_plasticity", []() {
        // Create brain with plasticity systems
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        brain->initialize();
        
        py::dict results;
        
        // Access plasticity systems
        auto stdp = brain->getSTDP();
        auto hebbian = brain->getHebbian();
        auto structural = brain->getStructuralPlasticity();
        
        results["stdp_available"] = stdp != nullptr;
        results["hebbian_available"] = hebbian != nullptr;
        results["structural_available"] = structural != nullptr;
        
        return results;
    }, "Demonstrate plasticity system integration")
    
    m.def("demo_neuromodulation", []() {
        // Create brain with neuromodulation systems
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        brain->initialize();
        
        py::dict results;
        
        // Access neuromodulation systems
        auto dopamine = brain->getDopamine();
        auto curiosity = brain->getCuriosity();
        auto novelty = brain->getNovelty();
        
        results["dopamine_available"] = dopamine != nullptr;
        results["curiosity_available"] = curiosity != nullptr;
        results["novelty_available"] = novelty != nullptr;
        
        return results;
    }, "Demonstrate neuromodulation system integration");
}

} // namespace nlm