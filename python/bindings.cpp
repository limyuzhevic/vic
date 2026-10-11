#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
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
    )pbdoc";

    // Register custom Python exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
    py::register_exception<std::logic_error>(m, "LogicError", PyExc_RuntimeError);
    
    // User-friendly error messages
    auto user_friendly_runtime_error = [](const std::string& message) {
        std::stringstream ss;
        ss << "NLM Runtime Error: ";
        if (message.find("cannot be null") != std::string::npos) {
            ss << "Invalid argument provided. ";
        } else if (message.find("not initialized") != std::string::npos) {
            ss << "The system has not been initialized. ";
        } else if (message.find("failed") != std::string::npos) {
            ss << "Operation failed. ";
        }
        ss << message;
        return ss.str();
    };

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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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

    // Enhanced Config class with better error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with enhanced error handling)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                if (!self.loadFromFile(filepath)) {
                    throw std::runtime_error("Failed to load configuration from file: " + filepath);
                }
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("filepath"), "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                if (!self.saveToFile(filepath)) {
                    throw std::runtime_error("Failed to save configuration to file: " + filepath);
                }
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("filepath"), "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("get", [](Config& self, const std::string& key) {
            auto result = self.get<std::string>(key);
            if (!result) {
                throw std::out_of_range("Configuration key not found: " + key);
            }
            return *result;
        }, py::arg("key"), "Get configuration value (string)")
        .def("getInt", [](Config& self, const std::string& key) {
            auto result = self.get<int>(key);
            if (!result) {
                throw std::out_of_range("Configuration key not found: " + key);
            }
            return *result;
        }, py::arg("key"), "Get configuration value (integer)")
        .def("getFloat", [](Config& self, const std::string& key) {
            auto result = self.get<float>(key);
            if (!result) {
                throw std::out_of_range("Configuration key not found: " + key);
            }
            return *result;
        }, py::arg("key"), "Get configuration value (float)")
        .def("getBool", [](Config& self, const std::string& key) {
            auto result = self.get<bool>(key);
            if (!result) {
                throw std::out_of_range("Configuration key not found: " + key);
            }
            return *result;
        }, py::arg("key"), "Get configuration value (boolean)")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set<std::string>(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (string)")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set<int>(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (integer)")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set<float>(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (float)")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set<bool>(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (boolean)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput classes with better error messages
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input with error handling)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            std::stringstream ss;
            ss << "<SensoryInput type=" << input.getType() << ">";
            return ss.str();
        });

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw std::invalid_argument("Cannot set empty vision data");
            }
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("isValid", [](const Vision& self) {
            return !self.getData().empty() && self.getWidth() > 0 && self.getHeight() > 0;
        }, "Check if vision data is valid");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw std::invalid_argument("Cannot set empty audio data");
            }
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("isValid", [](const Audio& self) {
            return !self.getData().empty() && self.getSampleRate() > 0;
        }, "Check if audio data is valid");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

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
        .def("isValid", [](const Action& action) {
            return action.getType() != ActionType::Custom || !action.getParameters().empty();
        }, "Check if action is valid");

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
        .def("isValid", [](const WorldObject& obj) {
            return obj.radius > 0.0f;
        }, "Check if world object is valid");

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
        .def("isHealthy", [](AgentBody& body) {
            return body.energy > 0.0f && body.health > 0.0f;
        }, "Check if agent body is healthy");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", [](const ActionResult& result) {
            return result.success;
        }, "Check if action was successful")
        .def("isPositiveReward", [](const ActionResult& result) {
            return result.reward > 0.0f;
        }, "Check if action yielded positive reward");

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
        .def("isValid", [](const SensoryPercept& percept) {
            return !percept.getVision().empty() || !percept.getTouch().empty() ||
                   !percept.getInternal().empty() || !percept.getProprioception().empty();
        }, "Check if sensory percept contains valid data");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", [](SimpleWorld& self, const Action& cmd, Timestamp time) {
            auto result = self.applyMotorCommand(cmd, time);
            if (!result.success) {
                throw std::runtime_error("Failed to apply motor command: " + result.message);
            }
            return result;
        }, py::arg("cmd"), py::arg("currentTime"), "Apply motor command and return result")
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
        .def("isRunning", [](const SimpleWorld& world) {
            return world.getSimulationTime() > 0.0;
        }, "Check if world simulation is running");

    // Enhanced Brain class with comprehensive error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced error handling)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            try {
                if (!self.initialize()) {
                    throw std::runtime_error("Brain initialization failed");
                }
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", [](Brain& self, const SensoryInput& input) {
            try {
                self.receiveSensoryInput(input);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", [](Brain& self) {
            try {
                auto action = self.produceAction();
                if (!action) {
                    throw std::runtime_error("Brain produced no action");
                }
                return action;
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Produce motor action based on neural activity")
        .def("reset", [](Brain& self) {
            try {
                self.reset();
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Reset brain state")
        .def("save", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.save(filepath)) {
                    throw std::runtime_error("Failed to save brain state to file: " + filepath);
                }
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("filepath"), "Save brain state to file")
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.load(filepath)) {
                    throw std::runtime_error("Failed to load brain state from file: " + filepath);
                }
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("filepath"), "Load brain state from file")
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
        .def("logStatus", [](Brain& self) {
            try {
                self.logStatus();
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Log brain status")
        // Enhanced convenience methods with validation
        .def("createPopulation", [](Brain& self, NeuronType type, size_t count) {
            try {
                if (count == 0) {
                    throw std::invalid_argument("Cannot create population with zero neurons");
                }
                RegionId regionId = self.getRegionIds().empty() ? 
                    self.addRegion() : self.getRegionIds()[0];
                NeuralRegion* region = self.getRegion(regionId);
                return region->createPopulation(type, count);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("type"), py::arg("count"),
             "Create a population of neurons")
        .def("connectRegions", [](Brain& self, RegionId source, RegionId target, 
                                 float weight = 0.1f, Delay delay = 1) {
            try {
                self.addInterRegionConnection(source, target, weight, delay);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("source"), py::arg("target"), py::arg("weight") = 0.1f, py::arg("delay") = 1,
             "Connect two regions with a synapse")
        .def("getLearningRate", [](Brain& self) {
            try {
                auto stdp = self.getSTDP();
                return stdp ? stdp->getLTPWeight() : 0.01f;
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Get current learning rate")
        .def("setLearningRate", [](Brain& self, float rate) {
            try {
                if (rate < 0.0f || rate > 1.0f) {
                    throw std::invalid_argument("Learning rate must be between 0.0 and 1.0");
                }
                auto stdp = self.getSTDP();
                if (stdp) stdp->setLTPWeight(rate);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("rate"), "Set learning rate")
        .def("getStatistics", [](Brain& self) {
            try {
                return std::map<std::string, double>{{
                    {"total_neurons", static_cast<double>(self.getTotalNeuronCount())},
                    {"total_synapses", static_cast<double>(self.getTotalSynapseCount())},
                    {"active_neurons", static_cast<double>(self.getActiveNeuronCount())},
                    {"firing_neurons", static_cast<double>(self.getFiringNeuronCount())},
                    {"avg_firing_rate", self.getAverageFiringRate()},
                    {"spike_count", static_cast<double>(self.getTotalSpikeCount())},
                    {"e_i_ratio", self.getExcitationInhibitionRatio()},
                    {"developmental_stage", static_cast<double>(self.getDevelopmentalStage())}
                }};
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Get comprehensive brain statistics")
        .def("enableAllLearning", [](Brain& self) {
            try {
                auto stdp = self.getSTDP();
                auto hebbian = self.getHebbian();
                auto structural = self.getStructuralPlasticity();
                if (stdp) stdp->setEnabled(true);
                if (hebbian) hebbian->setEnabled(true);
                if (structural) structural->setEnabled(true);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Enable all plasticity rules")
        .def("disableAllLearning", [](Brain& self) {
            try {
                auto stdp = self.getSTDP();
                auto hebbian = self.getHebbian();
                auto structural = self.getStructuralPlasticity();
                if (stdp) stdp->setEnabled(false);
                if (hebbian) hebbian->setEnabled(false);
                if (structural) structural->setEnabled(false);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Disable all plasticity rules")
        .def("getPlasticityStatus", [](Brain& self) {
            try {
                auto stdp = self.getSTDP();
                auto hebbian = self.getHebbian();
                auto structural = self.getStructuralPlasticity();
                return std::map<std::string, bool>{{
                    {"stdp_enabled", stdp ? stdp->isEnabled() : false},
                    {"hebbian_enabled", hebbian ? hebbian->isEnabled() : false},
                    {"structural_enabled", structural ? structural->isEnabled() : false}
                }};
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Get status of all plasticity rules")
        .def("isHealthy", [](const Brain& self) {
            return self.getTotalNeuronCount() > 0 && 
                   self.getTotalSynapseCount() > 0 &&
                   self.getActiveNeuronCount() <= self.getTotalNeuronCount();
        }, "Check if brain is in a healthy state")
        .def("getMemoryStats", [](Brain& self) {
            try {
                auto working = self.getWorkingMemory();
                auto episodic = self.getEpisodicMemory();
                auto associative = self.getAssociativeMemory();
                return std::map<std::string, double>{{
                    {"working_memory_capacity", working ? working->getCapacity() : 0},
                    {"episodic_memory_size", episodic ? episodic->getSize() : 0},
                    {"associative_memory_size", associative ? associative->getSize() : 0}
                }};
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Get memory system statistics")
        .def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration")
        .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration")
        .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, "Create a new simple world")
        .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a new agent brain interface")
        .attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);

    // Enhanced AgentBrain class with comprehensive error handling
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with enhanced error handling)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", [](AgentBrain& self, const SimpleWorld& world) {
            try {
                self.initialize(world);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", [](AgentBrain& self, const SensoryPercept& percept) {
            try {
                self.processSensoryInput(percept);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", [](AgentBrain& self) {
            try {
                return self.decodeMotorCommand();
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", [](AgentBrain& self, float reward, float predictedReward) {
            try {
                if (std::isnan(reward) || std::isinf(reward)) {
                    throw std::invalid_argument("Reward must be a valid number");
                }
                self.applyRewardModulation(reward, predictedReward);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", [](AgentBrain& self, double timestep) {
            try {
                if (std::isnan(timestep) || timestep < 0.0) {
                    throw std::invalid_argument("Timestep must be a positive number");
                }
                self.updateDevelopment(timestep);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("timestep"), "Update development system")
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
        .def("reset", [](AgentBrain& self) {
            try {
                self.reset();
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, "Reset agent for new episode")
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
        // Enhanced convenience methods
        .def("getAllNeuromodulationLevels", [](AgentBrain& self) {
            return std::map<std::string, double>{{
                {"dopamine", self.getNeuromodulationLevel()},
                {"curiosity", self.getCuriosityLevel()},
                {"novelty", self.getNoveltyLevel()},
                {"prediction_error", self.getPredictionError()}
            }};
        }, "Get all neuromodulation levels")
        .def("setDevelopmentStage", [](AgentBrain& self, DevelopmentalStage stage) {
            try {
                self.getBrain()->setDevelopmentalStage(stage);
            } catch (const std::exception& e) {
                throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
            }
        }, py::arg("stage"), "Set brain developmental stage")
        .def("getLearningState", [](AgentBrain& self) {
            return std::map<std::string, bool>{{
                {"reward_modulation", self.isRewardModulationEnabled()},
                {"structural_plasticity", self.isStructuralPlasticityEnabled()},
                {"development", self.isDevelopmentEnabled()},
                {"curiosity", self.isCuriosityEnabled()}
            }};
        }, "Get current learning configuration")
        .def("updateLearningState", [](AgentBrain& self, bool enableRewardModulation,
                                       bool enableStructuralPlasticity, bool enableDevelopment,
                                       bool enableCuriosity) {
            self.enableRewardModulation(enableRewardModulation);
            self.enableStructuralPlasticity(enableStructuralPlasticity);
            self.enableDevelopment(enableDevelopment);
            self.enableCuriosity(enableCuriosity);
        }, py::arg("enableRewardModulation"), py::arg("enableStructuralPlasticity"),
             py::arg("enableDevelopment"), py::arg("enableCuriosity"),
             "Update all learning configuration flags");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);

    // Enhanced convenience functions with validation and error handling
    m.def("quickSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                               std::shared_ptr<AgentBrain> agent, int steps) {
        try {
            if (!brain || !world || !agent) {
                throw std::invalid_argument("All parameters must be provided and non-null");
            }
            if (steps <= 0) {
                throw std::invalid_argument("Number of steps must be positive");
            }
            
            for (int i = 0; i < steps; ++i) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                if (!percept.isValid()) {
                    throw std::runtime_error("Invalid sensory percept received");
                }
                agent->processSensoryInput(percept);
                brain->step(i);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            return std::map<std::string, double>{{
                {"steps_completed", static_cast<double>(steps)},
                {"neurons_firing", static_cast<double>(brain->getFiringNeuronCount())},
                {"total_spikes", static_cast<double>(brain->getTotalSpikeCount())},
                {"firing_rate", brain->getAverageFiringRate()}
            }};
        } catch (const std::exception& e) {
            throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
         "Run a complete simulation with brain-world-agent interaction");

    m.def("createQuickLearningBrain", [](bool enableCuriosity, bool enableRewardModulation,
                                        bool enableDevelopment, bool enableStructuralPlasticity) {
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 8, 8);
            world->reset();
            agent->initialize(*world);
            
            if (enableCuriosity) agent->enableCuriosity(true);
            if (enableRewardModulation) agent->enableRewardModulation(true);
            if (enableDevelopment) agent->enableDevelopment(true);
            if (enableStructuralPlasticity) agent->enableStructuralPlasticity(true);
            
            return std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(
                brain, world, agent);
        } catch (const std::exception& e) {
            throw std::runtime_error(user_friendly_runtime_error(std::string(e.what())));
        }
    }, py::arg("enableCuriosity") = true, py::arg("enableRewardModulation") = true,
         py::arg("enableDevelopment") = true, py::arg("enableStructuralPlasticity") = true,
         "Create a brain configured for advanced learning with all features enabled");

    // Error handling utility function
    m.def("nlmError", [](const std::string& message) {
        throw std::runtime_error(user_friendly_runtime_error(message));
    }, py::arg("message"), "Raise a user-friendly NLM runtime error");
}

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
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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
        .def_readwrite("active", &WorldObject::active);

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
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

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
        // Enhanced convenience methods
        .def("createPopulation", [](Brain& self, NeuronType type, size_t count) {
            RegionId regionId = self.getRegionIds().empty() ? 
                self.addRegion() : self.getRegionIds()[0];
            NeuralRegion* region = self.getRegion(regionId);
            return region->createPopulation(type, count);
        }, py::arg("type"), py::arg("count"),
             "Create a population of neurons")
        .def("connectRegions", [](Brain& self, RegionId source, RegionId target, 
                                 float weight = 0.1f, Delay delay = 1) {
            self.addInterRegionConnection(source, target, weight, delay);
        }, py::arg("source"), py::arg("target"), py::arg("weight") = 0.1f, py::arg("delay") = 1,
             "Connect two regions with a synapse")
        .def("getLearningRate", [](Brain& self) {
            auto stdp = self.getSTDP();
            return stdp ? stdp->getLTPWeight() : 0.01f;
        }, "Get current learning rate")
        .def("setLearningRate", [](Brain& self, float rate) {
            auto stdp = self.getSTDP();
            if (stdp) stdp->setLTPWeight(rate);
        }, py::arg("rate"), "Set learning rate")
        .def("getStatistics", [](Brain& self) {
            return std::map<std::string, double>{{
                {"total_neurons", static_cast<double>(self.getTotalNeuronCount())},
                {"total_synapses", static_cast<double>(self.getTotalSynapseCount())},
                {"active_neurons", static_cast<double>(self.getActiveNeuronCount())},
                {"firing_neurons", static_cast<double>(self.getFiringNeuronCount())},
                {"avg_firing_rate", self.getAverageFiringRate()},
                {"spike_count", static_cast<double>(self.getTotalSpikeCount())},
                {"e_i_ratio", self.getExcitationInhibitionRatio()},
                {"developmental_stage", static_cast<double>(self.getDevelopmentalStage())}
            }};
        }, "Get comprehensive brain statistics")
        .def("enableAllLearning", [](Brain& self) {
            auto stdp = self.getSTDP();
            auto hebbian = self.getHebbian();
            auto structural = self.getStructuralPlasticity();
            if (stdp) stdp->setEnabled(true);
            if (hebbian) hebbian->setEnabled(true);
            if (structural) structural->setEnabled(true);
        }, "Enable all plasticity rules")
        .def("disableAllLearning", [](Brain& self) {
            auto stdp = self.getSTDP();
            auto hebbian = self.getHebbian();
            auto structural = self.getStructuralPlasticity();
            if (stdp) stdp->setEnabled(false);
            if (hebbian) hebbian->setEnabled(false);
            if (structural) structural->setEnabled(false);
        }, "Disable all plasticity rules")
        .def("getPlasticityStatus", [](Brain& self) {
            auto stdp = self.getSTDP();
            auto hebbian = self.getHebbian();
            auto structural = self.getStructuralPlasticity();
            return std::map<std::string, bool>{{
                {"stdp_enabled", stdp ? stdp->isEnabled() : false},
                {"hebbian_enabled", hebbian ? hebbian->isEnabled() : false},
                {"structural_enabled", structural ? structural->isEnabled() : false}
            }};
        }, "Get status of all plasticity rules")
        .def("isHealthy", [](const Brain& self) {
            return self.getTotalNeuronCount() > 0 && 
                   self.getTotalSynapseCount() > 0 &&
                   self.getActiveNeuronCount() <= self.getTotalNeuronCount();
        }, "Check if brain is in a healthy state")
        .def("getMemoryStats", [](Brain& self) {
            auto working = self.getWorkingMemory();
            auto episodic = self.getEpisodicMemory();
            auto associative = self.getAssociativeMemory();
            return std::map<std::string, double>{{
                {"working_memory_capacity", working ? working->getCapacity() : 0},
                {"episodic_memory_size", episodic ? episodic->getSize() : 0},
                {"associative_memory_size", associative ? associative->getSize() : 0}
            }};
        }, "Get memory system statistics")
        .def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration")
        .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration")
        .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, "Create a new simple world")
        .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a new agent brain interface")
        .attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);

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
        // Enhanced convenience methods for AgentBrain
        .def("getAllNeuromodulationLevels", [](AgentBrain& self) {
            return std::map<std::string, double>{{
                {"dopamine", self.getNeuromodulationLevel()},
                {"curiosity", self.getCuriosityLevel()},
                {"novelty", self.getNoveltyLevel()},
                {"prediction_error", self.getPredictionError()}
            }};
        }, "Get all neuromodulation levels")
        .def("setDevelopmentStage", [](AgentBrain& self, DevelopmentalStage stage) {
            self.getBrain()->setDevelopmentalStage(stage);
        }, py::arg("stage"), "Set brain developmental stage")
        .def("getLearningState", [](AgentBrain& self) {
            return std::map<std::string, bool>{{
                {"reward_modulation", self.isRewardModulationEnabled()},
                {"structural_plasticity", self.isStructuralPlasticityEnabled()},
                {"development", self.isDevelopmentEnabled()},
                {"curiosity", self.isCuriosityEnabled()}
            }};
        }, "Get current learning configuration")
        .def("updateLearningState", [](AgentBrain& self, bool enableRewardModulation,
                                       bool enableStructuralPlasticity, bool enableDevelopment,
                                       bool enableCuriosity) {
            self.enableRewardModulation(enableRewardModulation);
            self.enableStructuralPlasticity(enableStructuralPlasticity);
            self.enableDevelopment(enableDevelopment);
            self.enableCuriosity(enableCuriosity);
        }, py::arg("enableRewardModulation"), py::arg("enableStructuralPlasticity"),
             py::arg("enableDevelopment"), py::arg("enableCuriosity"),
             "Update all learning configuration flags");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);

    // Enhanced convenience functions
    m.def("quickSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                               std::shared_ptr<AgentBrain> agent, int steps) {
        for (int i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        return std::map<std::string, double>{{
            {"steps_completed", static_cast<double>(steps)},
            {"neurons_firing", static_cast<double>(brain->getFiringNeuronCount())},
            {"total_spikes", static_cast<double>(brain->getTotalSpikeCount())},
            {"firing_rate", brain->getAverageFiringRate()}
        }};
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
         "Run a complete simulation with brain-world-agent interaction");

    m.def("createQuickLearningBrain", [](bool enableCuriosity, bool enableRewardModulation,
                                        bool enableDevelopment, bool enableStructuralPlasticity) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        if (enableCuriosity) agent->enableCuriosity(true);
        if (enableRewardModulation) agent->enableRewardModulation(true);
        if (enableDevelopment) agent->enableDevelopment(true);
        if (enableStructuralPlasticity) agent->enableStructuralPlasticity(true);
        
        return std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(
            brain, world, agent);
    }, py::arg("enableCuriosity") = true, py::arg("enableRewardModulation") = true,
         py::arg("enableDevelopment") = true, py::arg("enableStructuralPlasticity") = true,
         "Create a brain configured for advanced learning with all features enabled");
}