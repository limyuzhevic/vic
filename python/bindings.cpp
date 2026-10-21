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
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    py::register_exception<std::logic_error>(m, "LogicError");

    // Exception classes for NLM
    py::class_<nlm::ConfigError>(m, "ConfigError", py::module_local())
        .def(py::init<const std::string&>(), py::arg("message"),
             "Configuration error")
        .def("what", &std::runtime_error::what);

    py::class_<nlm::SimulationError>(m, "SimulationError", py::module_local())
        .def(py::init<const std::string&>(), py::arg("message"),
             "Simulation error")
        .def("what", &std::runtime_error::what);

    py::class_<nlm::VisualizationError>(m, "VisualizationError", py::module_local())
        .def(py::init<const std::string&>(), py::arg("message"),
             "Visualization error")
        .def("what", &std::runtime_error::what);

    // Input validation helpers
    namespace {
        void validateConfigValue(const std::string& key, const std::string& value) {
            if (key.empty()) {
                throw std::invalid_argument("Configuration key cannot be empty");
            }
            if (key.length() > 100) {
                throw std::invalid_argument("Configuration key too long (max 100 characters)");
            }
            if (value.length() > 10000) {
                throw std::invalid_argument("Configuration value too large (max 10000 characters)");
            }
            // Check for valid key format
            for (char c : key) {
                if (!std::isalnum(c) && c != '_' && c != '-') {
                    throw std::invalid_argument("Invalid configuration key format: " + key);
                }
            }
        }

        void validateStepRange(SimulationStep step) {
            if (step > 1000000000) {
                throw std::out_of_range("Step number too large (max 1,000,000,000)");
            }
            if (step == 0) {
                throw std::invalid_argument("Step number cannot be zero");
            }
        }

        void validateWorldSize(int width, int height) {
            if (width <= 0 || width > 10000) {
                throw std::invalid_argument("World width must be between 1 and 10000");
            }
            if (height <= 0 || height > 10000) {
                throw std::invalid_argument("World height must be between 1 and 10000");
            }
        }

        void validateVisionDimensions(int width, int height, int channels) {
            if (width <= 0 || width > 1000) {
                throw std::invalid_argument("Vision width must be between 1 and 1000");
            }
            if (height <= 0 || height > 1000) {
                throw std::invalid_argument("Vision height must be between 1 and 1000");
            }
            if (channels <= 0 || channels > 10) {
                throw std::invalid_argument("Vision channels must be between 1 and 10");
            }
        }
    }

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

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Pythonic convenience methods for Config with validation
    .def("get_value", [](Config& self, const std::string& key) {
        validateConfigValue(key, "");  // Empty value for get
        auto value = self.get<std::string>(key);
        if (!value) {
            auto intValue = self.get<int>(key);
            if (intValue) {
                return py::make_tuple("int", *intValue);
            }
            auto floatValue = self.get<double>(key);
            if (floatValue) {
                return py::make_tuple("float", *floatValue);
            }
            auto boolValue = self.get<bool>(key);
            if (boolValue) {
                return py::make_tuple("bool", *boolValue);
            }
            throw nlm::ConfigError("Configuration key not found: " + key);
        }
        return py::make_tuple("string", *value);
    }, py::arg("key"), "Get configuration value with type checking and validation")
    .def("set_value", [](Config& self, const std::string& key, const std::string& value) {
        validateConfigValue(key, value);
        self.set(key, value);
    }, py::arg("key"), py::arg("value"), "Set configuration value as string with validation")
    .def("set_value_int", [](Config& self, const std::string& key, int value) {
        validateConfigValue(key, std::to_string(value));
        self.set(key, value);
    }, py::arg("key"), py::arg("value"), "Set configuration value as integer with validation")
    .def("set_value_float", [](Config& self, const std::string& key, double value) {
        validateConfigValue(key, std::to_string(value));
        self.set(key, value);
    }, py::arg("key"), py::arg("value"), "Set configuration value as float with validation")
    .def("set_value_bool", [](Config& self, const std::string& key, bool value) {
        validateConfigValue(key, value ? "true" : "false");
        self.set(key, value);
    }, py::arg("key"), py::arg("value"), "Set configuration value as boolean with validation")
    .def("get_all_as_dict", [](const Config& self) {
        py::dict result;
        auto keys = self.getKeys();
        for (const auto& key : keys) {
            auto value = self.get<std::string>(key);
            if (value) {
                result[key] = *value;
            } else {
                // Try to get other types
                auto intValue = self.get<int>(key);
                if (intValue) {
                    result[key] = *intValue;
                } else {
                    auto floatValue = self.get<double>(key);
                    if (floatValue) {
                        result[key] = *floatValue;
                    } else {
                        auto boolValue = self.get<bool>(key);
                        if (boolValue) {
                            result[key] = *boolValue;
                        }
                    }
                }
            }
        }
        return result;
    }, "Get all configuration values as a Python dictionary")
    .def("apply_preset", [](Config& self, const std::string& preset) {
        if (preset.empty()) {
            throw std::invalid_argument("Preset name cannot be empty");
        }
        if (preset.length() > 50) {
            throw std::invalid_argument("Preset name too long (max 50 characters)");
        }
        // Validate preset name
        for (char c : preset) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                throw std::invalid_argument("Invalid preset name format: " + preset);
            }
        }
        // Call the original preset logic
        if (preset == "default") {
            self.set("neuronCount", 1000);
            self.set("visionWidth", 32);
            self.set("visionHeight", 32);
            self.set("visionChannels", 3);
            self.set("actionCount", 10);
            self.set("maxEnergy", 100.0);
            self.set("energyDecayRate", 0.01);
        } else if (preset == "debug") {
            self.set("neuronCount", 100);
            self.set("visionWidth", 16);
            self.set("visionHeight", 16);
            self.set("visionChannels", 3);
            self.set("actionCount", 5);
            self.set("maxEnergy", 50.0);
        } else if (preset == "high_performance") {
            self.set("neuronCount", 5000);
            self.set("visionWidth", 64);
            self.set("visionHeight", 64);
            self.set("visionChannels", 3);
            self.set("actionCount", 20);
            self.set("maxEnergy", 200.0);
        } else {
            throw nlm::ConfigError("Unknown preset: " + preset);
        }
    }, py::arg("preset"), "Apply a predefined configuration preset with validation");

    // Helper functions for configuration
    .def("create_brain_config", []() {
        auto config = std::make_shared<Config>();
        config->set("neuronCount", 1000);
        config->set("visionWidth", 32);
        config->set("visionHeight", 32);
        config->set("visionChannels", 3);
        config->set("actionCount", 10);
        config->set("maxEnergy", 100.0);
        return config;
    }, "Create a default brain configuration")
    .def("create_agent_config", []() {
        auto config = std::make_shared<Config>();
        config->set("agentNeuronCount", 500);
        config->set("sensorySize", 96); // 32 * 32 * 3
        config->set("motorSize", 10);
        config->set("maxEnergy", 100.0);
        return config;
    }, "Create a default agent configuration")
    .def("create_world_config", []() {
        auto config = std::make_shared<Config>();
        config->set("worldWidth", 100);
        config->set("worldHeight", 100);
        config->set("maxEnergy", 100.0);
        config->set("energyDecayRate", 0.01);
        return config;
    }, "Create a default world configuration")
    .def("merge_configs", [](std::vector<std::shared_ptr<Config>> configs) {
        auto merged = std::make_shared<Config>();
        for (const auto& config : configs) {
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                auto value = config->get<std::string>(key);
                if (value) {
                    merged->set(key, *value);
                }
            }
        }
        return merged;
    }, py::arg("configs"), "Merge multiple configurations into one");

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
        
        // Pythonic convenience methods
        .def("status", [](const std::shared_ptr<Brain>& brain) {
            brain->logStatus();
        }, "Print brain status (alias for logStatus)")
        .def("get_stats", [](const std::shared_ptr<Brain>& brain) {
            py::dict stats;
            stats["total_neurons"] = brain->getTotalNeuronCount();
            stats["total_synapses"] = brain->getTotalSynapseCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["development_stage"] = brain->getDevelopmentalStage();
            return stats;
        }, "Get brain statistics as a dictionary")
        .def("is_initialized", [](const std::shared_ptr<Brain>& brain) {
            // We can't directly check initialization, but we can check neuron count
            return brain->getTotalNeuronCount() > 0;
        }, "Check if brain is initialized (has neurons)")
        .def("reset_with_config", [](std::shared_ptr<Brain>& brain, std::shared_ptr<Config> config) {
            brain->reset();
            // Note: We would need to reconstruct the brain with new config
            // This is a convenience wrapper for typical use
            brain = std::make_shared<Brain>(config);
        }, py::arg("config"), "Reset brain with new configuration (convenience method)")
        .def("save_as_json", &Brain::save, py::arg("filepath"),
              "Save brain state to JSON file (alias for save)")
        .def("load_from_json", &Brain::load, py::arg("filepath"),
              "Load brain state from JSON file (alias for load)");

    // Enhanced AgentBrain interface with advanced features
    py::class_<AgentBrain>(m, "AgentBrain", py::module_local(), R"pbdoc(Advanced agent brain interface with enhanced control and debugging)pbdoc")
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

        // Advanced agent features
        .def("set_action_mode", [](AgentBrain& self, const std::string& mode) {
            if (mode == "exploration") {
                self.enableCuriosity(true);
                self.enableRewardModulation(false);
            } else if (mode == "exploitation") {
                self.enableCuriosity(false);
                self.enableRewardModulation(true);
            } else if (mode == "balanced") {
                self.enableCuriosity(true);
                self.enableRewardModulation(true);
            } else {
                throw std::invalid_argument("Unknown action mode: " + mode);
            }
        }, py::arg("mode"), "Set agent action mode (exploration, exploitation, balanced)")

        .def("get_action_mode", [](const AgentBrain& self) {
            // Determine mode based on enabled features
            bool curiosity = self.isCuriosityEnabled();
            bool reward = self.isRewardModulationEnabled();
            
            if (curiosity && !reward) return "exploration";
            if (!curiosity && reward) return "exploitation";
            if (curiosity && reward) return "balanced";
            return "default";
        }, "Get current agent action mode")

        .def("set_behavioral_state", [](AgentBrain& self, const std::string& state) {
            if (state == "active") {
                self.enableDevelopment(true);
                self.enableCuriosity(true);
            } else if (state == "passive") {
                self.enableDevelopment(false);
                self.enableCuriosity(false);
            } else if (state == "curious") {
                self.enableDevelopment(true);
                self.enableCuriosity(true);
            } else {
                throw std::invalid_argument("Unknown behavioral state: " + state);
            }
        }, py::arg("state"), "Set agent behavioral state (active, passive, curious)")

        .def("get_behavioral_state", [](const AgentBrain& self) {
            bool development = self.isDevelopmentEnabled();
            bool curiosity = self.isCuriosityEnabled();
            
            if (development && curiosity) return "active";
            if (!development && !curiosity) return "passive";
            if (development && curiosity) return "curious";
            return "default";
        }, "Get current agent behavioral state")

        .def("get_agent_stats", [](const AgentBrain& self) {
            py::dict stats;
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["development_stage"] = self.getDevelopmentalStage();
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            return stats;
        }, "Get agent statistics and state")

        .def("enable_agent_mode", [](AgentBrain& self, const std::string& mode, bool enable) {
            if (mode == "reward_modulation") {
                self.enableRewardModulation(enable);
            } else if (mode == "structural_plasticity") {
                self.enableStructuralPlasticity(enable);
            } else if (mode == "development") {
                self.enableDevelopment(enable);
            } else if (mode == "curiosity") {
                self.enableCuriosity(enable);
            } else {
                throw std::invalid_argument("Unknown agent mode: " + mode);
            }
        }, py::arg("mode"), py::arg("enable"), "Enable/disable specific agent mode")

        .def("is_mode_enabled", [](const AgentBrain& self, const std::string& mode) {
            if (mode == "reward_modulation") {
                return self.isRewardModulationEnabled();
            } else if (mode == "structural_plasticity") {
                return self.isStructuralPlasticityEnabled();
            } else if (mode == "development") {
                return self.isDevelopmentEnabled();
            } else if (mode == "curiosity") {
                return self.isCuriosityEnabled();
            } else {
                throw std::invalid_argument("Unknown agent mode: " + mode);
            }
        }, py::arg("mode"), "Check if agent mode is enabled")

    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface for brain debugging and analysis)pbdoc")
        .def(py::init<>())
        .def("initialize", &VisualizationInterface::initialize,
             "Initialize visualization")
        .def("update", &VisualizationInterface::update,
             "Update visualization data")
        .def("render", &VisualizationInterface::render,
             "Render current state")
        .def("close", &VisualizationInterface::close,
             "Close visualization")
        .def("isActive", &VisualizationInterface::isActive,
             "Check if visualization is active")
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate, py::arg("hz"),
             "Set update rate (Hz)")
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate,
             "Get update rate (Hz)")
        .def("visualizeNetwork", &VisualizationInterface::visualizeNetwork, py::arg("brain"),
             "Visualize neural network structure")
        .def("visualizeActivity", &VisualizationInterface::visualizeActivity, py::arg("brain"),
             "Visualize neural activity")
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes, py::arg("spikes"),
             "Visualize spike events")
        .def("visualizeWeights", &VisualizationInterface::visualizeWeights, py::arg("brain"),
             "Visualize synaptic weights");

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

    m.def("createVisualization", []() -> std::shared_ptr<VisualizationInterface> {
        return std::make_shared<VisualizationInterface>();
    }, "Create a visualization interface for brain debugging");

    // Performance monitoring and metrics classes
    py::class_<BenchmarkMetrics>(m, "BenchmarkMetrics", R"pbdoc(Performance metrics for benchmarking)pbdoc")
        .def(py::init<>())
        .def_readwrite("neuronsPerSecond", &BenchmarkMetrics::neuronsPerSecond)
        .def_readwrite("synapsesPerSecond", &BenchmarkMetrics::synapsesPerSecond)
        .def_readwrite("spikesPerSecond", &BenchmarkMetrics::spikesPerSecond)
        .def_readwrite("avgStepTimeMs", &BenchmarkMetrics::avgStepTimeMs)
        .def_readwrite("minStepTimeMs", &BenchmarkMetrics::minStepTimeMs)
        .def_readwrite("maxStepTimeMs", &BenchmarkMetrics::maxStepTimeMs)
        .def_readwrite("totalMemoryMB", &BenchmarkMetrics::totalMemoryMB)
        .def_readwrite("peakMemoryMB", &BenchmarkMetrics::peakMemoryMB)
        .def_readwrite("activeNeurons", &BenchmarkMetrics::activeNeurons)
        .def_readwrite("firingNeurons", &BenchmarkMetrics::firingNeurons)
        .def_readwrite("avgFiringRateHz", &BenchmarkMetrics::avgFiringRateHz)
        .def_readwrite("realTimeFactor", &BenchmarkMetrics::realTimeFactor)
        .def("toCSV", &BenchmarkMetrics::toCSV, "Export metrics to CSV format")
        .def("toJSON", &BenchmarkMetrics::toJSON, "Export metrics to JSON format")
        .def("__repr__", [](const BenchmarkMetrics& m) {
            return "<BenchmarkMetrics: " + std::to_string(m.totalMemoryMB) + " MB, " +
                   std::to_string(m.avgStepTimeMs) + " ms/step, " +
                   std::to_string(m.neuronsPerSecond) + " neurons/sec";
        });

    py::class_<MemoryProfiler>(m, "MemoryProfiler", R"pbdoc(Memory usage profiler for NLM simulations)pbdoc")
        .def(py::init<>())
        .def("record", &MemoryProfiler::record,
             "Record current memory usage")
        .def("getPeakMemoryMB", &MemoryProfiler::getPeakMemoryMB,
             "Get peak memory usage")
        .def("getCurrentMemoryMB", &MemoryProfiler::getCurrentMemoryMB,
             "Get current memory usage")
        .def("getBreakdown", &MemoryProfiler::getBreakdown,
             "Get memory breakdown")
        .def("resetPeak", &MemoryProfiler::resetPeak,
             "Reset peak memory tracking");

    py::class_<Metrics>(m, "Metrics", R"pbdoc(Metric collection and statistics for experiments)pbdoc")
        .def(py::init<>())
        .def("record", &Metrics::record, py::arg("name"), py::arg("value"),
             "Record a metric value")
        .def("getValues", &Metrics::getValues, py::arg("name"),
             "Get all values for a metric")
        .def("getMean", &Metrics::getMean, py::arg("name"),
             "Get mean value of a metric")
        .def("getStdDev", &Metrics::getStdDev, py::arg("name"),
             "Get standard deviation of a metric")
        .def("getMin", &Metrics::getMin, py::arg("name"),
             "Get minimum value of a metric")
        .def("getMax", &Metrics::getMax, py::arg("name"),
             "Get maximum value of a metric")
        .def("getMetricNames", &Metrics::getMetricNames,
             "Get all metric names")
        .def("clear", &Metrics::clear,
             "Clear all metrics")
        .def("summary", &Metrics::summary,
             "Get summary report of all metrics");

    // Factory functions for performance monitoring
    m.def("createPerformanceMonitor", []() -> std::shared_ptr<PerformanceMonitor> {
        return std::make_shared<PerformanceMonitor>();
    }, "Create a performance monitor for tracking simulation performance")
    .def("createMemoryProfiler", []() -> std::shared_ptr<MemoryProfiler> {
        return std::make_shared<MemoryProfiler>();
    }, "Create a memory profiler for tracking memory usage")
    .def("createMetricsCollector", []() -> std::shared_ptr<Metrics> {
        return std::make_shared<Metrics>();
    }, "Create a metrics collector for recording experiment metrics");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
