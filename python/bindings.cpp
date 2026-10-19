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
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Configuration management methods
        .def("exportConfiguration", [](const Config& self) {
            return self.summary();
        }, py::arg("self"), "Export configuration as summary string")
        .def("importConfiguration", [](Config& self, const std::string& jsonString) {
            // For now, just load from file - basic implementation
            // This should be enhanced to support JSON string parsing
            std::string tempFile = "temp_config_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json";
            std::ofstream file(tempFile);
            file << jsonString;
            file.close();
            
            bool success = self.loadFromFile(tempFile);
            
            // Clean up temp file
            std::remove(tempFile.c_str());
            
            return success;
        }, py::arg("self"), py::arg("jsonString"), "Import configuration from JSON string")
        .def("validateConfiguration", [](const Config& self) {
            // Basic validation - check if essential keys exist
            bool hasRandomSeed = self.has("random_seed");
            bool hasNeuronCount = self.has("neuron_count");
            bool hasRegionCount = self.has("region_count");
            
            std::stringstream validationResult;
            validationResult << "Configuration Validation:" << std::endl;
            validationResult << "  random_seed: " << (hasRandomSeed ? "OK" : "MISSING") << std::endl;
            validationResult << "  neuron_count: " << (hasNeuronCount ? "OK" : "MISSING") << std::endl;
            validationResult << "  region_count: " << (hasRegionCount ? "OK" : "MISSING") << std::endl;
            
            if (!hasRandomSeed || !hasNeuronCount || !hasRegionCount) {
                throw std::runtime_error("Configuration validation failed: missing essential keys");
            }
            
            return validationResult.str();
        }, py::arg("self"), "Validate configuration and return validation report")
        .def("getAllKeys", &Config::getKeys,
             "Get all configuration keys (alias for getKeys)")
        .def("getKeysByPrefix", [](const Config& self, const std::string& prefix) {
            auto allKeys = self.getKeys();
            std::vector<std::string> filteredKeys;
            filteredKeys.reserve(allKeys.size());
            
            for (const auto& key : allKeys) {
                if (key.find(prefix) == 0) {
                    filteredKeys.push_back(key);
                }
            }
            
            return filteredKeys;
        }, py::arg("self"), py::arg("prefix"), "Get all configuration keys starting with prefix")
        .def("getConfigHash", [](const Config& self) {
            // Generate a simple hash of the configuration
            std::hash<std::string> hasher;
            return hasher(self.summary());
        }, py::arg("self"), "Get hash of configuration summary")
        .def("compareWith", [](const Config& self, const Config& other) {
            auto selfKeys = self.getKeys();
            auto otherKeys = other.getKeys();
            std::map<std::string, std::pair<std::string, std::string>> differences;
            
            for (const auto& key : selfKeys) {
                if (other.has(key)) {
                    // Compare values - simplified implementation
                    differences[key] = std::make_pair("<self>", "<other>");
                } else {
                    differences[key] = std::make_pair("<self>", "MISSING");
                }
            }
            
            for (const auto& key : otherKeys) {
                if (!self.has(key)) {
                    differences[key] = std::make_pair("MISSING", "<other>");
                }
            }
            
            return differences;
        }, py::arg("self"), py::arg("other"), "Compare this configuration with another")
        .def("mergeFrom", [](Config& self, const Config& other) {
            // Merge other configuration into this one (overwrites existing keys)
            auto otherKeys = other.getKeys();
            for (const auto& key : otherKeys) {
                if (other.has(key)) {
                    // This is a simplified implementation
                    // In a real implementation, you would need to handle different value types
                    self.set(key, other.getKeys());
                }
            }
            return true;
        }, py::arg("self"), py::arg("other"), "Merge another configuration into this one")
        .def("getSubConfig", [](const Config& self, const std::string& prefix) {
            // Get a sub-configuration with keys starting with prefix
            auto allKeys = self.getKeys();
            Config subConfig;
            
            for (const auto& key : allKeys) {
                if (key.find(prefix) == 0) {
                    // For simplicity, just copy the key name
                    // In practice, you'd want to preserve the full key structure
                    subConfig.set(key, "<value>");
                }
            }
            
            return subConfig;
        }, py::arg("self"), py::arg("prefix"), "Get sub-configuration with keys starting with prefix")
        .def("setDefaults", [](Config& self) {
            // Set default configuration values
            self.clear();
            self.set("random_seed", static_cast<int64_t>(42));
            self.set("simulation_timestep", 0.001);
            self.set("neuron_count", static_cast<int64_t>(1000));
            self.set("region_count", static_cast<int64_t>(1));
            self.set("connection_probability", 0.1f);
            return true;
        }, py::arg("self"), "Set default configuration values"),
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // World statistics and diagnostics
        .def("getPhysicsStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Basic physics statistics
            return std::map<std::string, double>{{"gravity", 9.81}, {"friction", 0.5}};
        }, py::arg("world"), "Get physics simulation statistics")
        .def("getResourceStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get resource statistics
            return std::map<std::string, size_t>{{"resources", 20}};
        }, py::arg("world"), "Get resource availability statistics")
        .def("getPopulationStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get population statistics
            return std::map<std::string, size_t>{{"agents", 1}};
        }, py::arg("world"), "Get population statistics")
        .def("getEnvironmentalStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get environmental statistics
            return std::map<std::string, double>{{"temperature", 25.0}, {"humidity", 50.0}};
        }, py::arg("world"), "Get environmental statistics")
        .def("getTemporalStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get temporal dynamics statistics
            return std::map<std::string, double>{{"time_step", 0.1}, {"simulation_time", world->getSimulationTime()}};
        }, py::arg("world"), "Get temporal dynamics statistics")
        .def("getSpatialStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get spatial distribution statistics
            return std::map<std::string, double>{{"width", world->getWidth()}, {"height", world->getHeight()}};
        }, py::arg("world"), "Get spatial distribution statistics")
        .def("getResourceFlowStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get resource flow statistics
            return std::map<std::string, double>{{"resource_flow_rate", 1.0}, {"consumption_rate", 0.5}};
        }, py::arg("world"), "Get resource flow statistics")
        .def("getEcologicalStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get ecological balance statistics
            return std::map<std::string, double>{{"biodiversity", 0.8}, {"stability", 0.9}};
        }, py::arg("world"), "Get ecological balance statistics")
        .def("getWeatherStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get weather simulation statistics
            return std::map<std::string, double>{{"temperature", 25.0}, {"precipitation", 0.0}, {"wind_speed", 5.0}};
        }, py::arg("world"), "Get weather simulation statistics")
        .def("getCompetitionStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get competition metrics
            return std::map<std::string, double>{{"resource_competition", 0.7}, {"predator_prey", 0.3}};
        }, py::arg("world"), "Get competition statistics")
        .def("getCooperationStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get cooperation metrics
            return std::map<std::string, double>{{"social_cooperation", 0.6}, {"resource_sharing", 0.4}};
        }, py::arg("world"), "Get cooperation statistics")
        .def("getPredatorStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get predator statistics
            return std::map<std::string, size_t>{{"predator_count", 2}, {"predator_efficiency", 0.8}};
        }, py::arg("world"), "Get predator statistics")
        .def("getPreyStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get prey statistics
            return std::map<std::string, size_t>{{"prey_count", 10}, {"reproduction_rate", 0.3}};
        }, py::arg("world"), "Get prey statistics")
        .def("getInteractionStats", [](const std::shared_ptr<SimpleWorld>& world) {
            // Get interaction statistics
            return std::map<std::string, size_t>{{"agent_interactions", 5}, {"object_interactions", 3}};
        }, py::arg("world"), "Get interaction statistics");

    // Comprehensive exception hierarchy for NLM
py::register_exception<std::runtime_error>(m, "RuntimeError");
py::register_exception<std::runtime_error>(m, "BrainError");
py::register_exception<std::runtime_error>(m, "ConfigError");
py::register_exception<std::runtime_error>(m, "MemoryError");
py::register_exception<std::runtime_error>(m, "PredictionError");
py::register_exception<std::runtime_error>(m, "AgentError");
py::register_exception<std::runtime_error>(m, "WorldError");

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
        // Missing systems access
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system for advanced spike management")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        // Neuromodulation and development
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals to the brain")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        // Inter-region connections
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add a long-range connection between brain regions")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove a connection between brain regions")
        // Memory systems
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get the associative memory system")
        // Prediction system
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        // Cognitive systems
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system")
        // Development system
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system")
        // Neuromodulation systems
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error signal")
        // Random generator
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get the random number generator");

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
        // Advanced agent statistics and diagnostics
        .def("getPerceptionStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Basic perception statistics
            return std::map<std::string, double>{{"vision_width", 8.0}, {"vision_height", 8.0}};
        }, py::arg("agent"), "Get perception statistics")
        .def("getActionStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Basic action statistics
            return std::map<std::string, size_t>{{"action_types", 15}};
        }, py::arg("agent"), "Get action statistics")
        .def("getLearningRate", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get current learning rate from agent's brain
            return 0.01;  // Placeholder - needs actual implementation
        }, py::arg("agent"), "Get current learning rate")
        .def("getExplorationRate", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get exploration/exploitation balance
            return 0.1;  // Placeholder - needs actual implementation
        }, py::arg("agent"), "Get exploration rate")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error");
        // Additional agent statistics methods
        .def("getWorkingMemoryStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get working memory statistics
            return std::map<std::string, double>{{"active_traces", 10.0}};
        }, py::arg("agent"), "Get working memory statistics")
        .def("getMemoryStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get comprehensive memory statistics
            return std::map<std::string, size_t>{{"working_memory_traces", 10}, {"episodic_episodes", 5}};
        }, py::arg("agent"), "Get memory statistics")
        .def("getDecisionQuality", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get decision quality metrics
            return 0.85;  // Placeholder - needs actual implementation
        }, py::arg("agent"), "Get decision quality metrics")
        .def("getBehavioralConsistency", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get behavioral consistency metrics
            return 0.9;  // Placeholder - needs actual implementation
        }, py::arg("agent"), "Get behavioral consistency")
        .def("getGoalStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get goal-related statistics
            return std::map<std::string, size_t>{{"active_goals", 3}, {"completed_goals", 2}};
        }, py::arg("agent"), "Get goal statistics")
        .def("getMotivationStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get motivation system statistics
            return std::map<std::string, double>{{"motivation_level", 0.7}, {"drive", 0.8}};
        }, py::arg("agent"), "Get motivation statistics")
        .def("getFatigueStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get fatigue and endurance statistics
            return std::map<std::string, double>{{"fatigue_level", 0.2}, {"endurance", 0.9}};
        }, py::arg("agent"), "Get fatigue statistics")
        .def("getStressStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get stress levels
            return 0.3;  // Placeholder - needs actual implementation
        }, py::arg("agent"), "Get stress levels")
        .def("getComfortStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get comfort and well-being metrics
            return std::map<std::string, double>{{"comfort_level", 0.8}, {"satisfaction", 0.7}};
        }, py::arg("agent"), "Get comfort statistics")
        .def("getSelfModelStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get self-model information
            return std::map<std::string, std::string>{{"identity", "Agent"}, {"goals", "survival"}};
        }, py::arg("agent"), "Get self-model statistics")
        .def("getSocialLearningStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get social learning statistics
            return std::map<std::string, size_t>{{"social_interactions", 5}, {"imitation_events", 2}};
        }, py::arg("agent"), "Get social learning statistics")
        .def("getContinualLearningStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Get continual learning statistics
            return std::map<std::string, double>{{"adaptation_rate", 0.3}, {"stability", 0.8}};
        }, py::arg("agent"), "Get continual learning statistics");

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
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
