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
        // Get typed values
        .def("getInt", [](Config& self, const std::string& key) {
            auto val = self.get<int>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            } else {
                throw py::cast_error("Key not found or wrong type");
            }
        }, py::arg("key"), "Get an integer value by key")
        .def("getDouble", [](Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            } else {
                throw py::cast_error("Key not found or wrong type");
            }
        }, py::arg("key"), "Get a double value by key")
        .def("getString", [](Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            } else {
                throw py::cast_error("Key not found or wrong type");
            }
        }, py::arg("key"), "Get a string value by key")
        .def("getBool", [](Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            } else {
                throw py::cast_error("Key not found or wrong type");
            }
        }, py::arg("key"), "Get a boolean value by key")
        // Get with default values
        .def("getIntOr", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get an integer value or default if not found")
        .def("getDoubleOr", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get a double value or default if not found")
        .def("getStringOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get a string value or default if not found")
        .def("getBoolOr", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get a boolean value or default if not found")
        // Set methods
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set an integer value")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set a double value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set a string value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set a boolean value")
        .def("remove", &Config::remove, py::arg("key"), "Remove a configuration key")
        // Type checking methods
        .def("getIntIfExists", [](Config& self, const std::string& key) -> py::object {
            auto val = self.get<int>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            } else {
                return py::none();
            }
        }, py::arg("key"), "Get an integer value if it exists, otherwise None")
        .def("getDoubleIfExists", [](Config& self, const std::string& key) -> py::object {
            auto val = self.get<double>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            } else {
                return py::none();
            }
        }, py::arg("key"), "Get a double value if it exists, otherwise None")
        .def("getStringIfExists", [](Config& self, const std::string& key) -> py::object {
            auto val = self.get<std::string>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            } else {
                return py::none();
            }
        }, py::arg("key"), "Get a string value if it exists, otherwise None")
        .def("getBoolIfExists", [](Config& self, const std::string& key) -> py::object {
            auto val = self.get<bool>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            } else {
                return py::none();
            }
        }, py::arg("key"), "Get a boolean value if it exists, otherwise None")
        .def("getAllAsDict", [](const Config& self) {
            std::vector<std::string> keys = self.getKeys();
            py::dict result;
            for (const auto& key : keys) {
                result[key] = self.get<std::string>(key).value_or("");
            }
            return result;
        }, "Get all configuration as a Python dictionary")
        // Advanced convenience methods
        .def("merge", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                auto val = other.get<std::string>(key);
                if (val.has_value()) {
                    self.set(key, val.value(), nlm::ConfigSource::Runtime);
                }
            }
        }, py::arg("other"), "Merge another configuration into this one")
        .def("copy", [](const Config& self) {
            auto newConfig = std::make_shared<Config>();
            for (const auto& key : self.getKeys()) {
                // Try all possible types
                auto intVal = self.get<int>(key);
                if (intVal.has_value()) {
                    newConfig->set(key, intVal.value(), nlm::ConfigSource::Runtime);
                    continue;
                }
                auto doubleVal = self.get<double>(key);
                if (doubleVal.has_value()) {
                    newConfig->set(key, doubleVal.value(), nlm::ConfigSource::Runtime);
                    continue;
                }
                auto stringVal = self.get<std::string>(key);
                if (stringVal.has_value()) {
                    newConfig->set(key, stringVal.value(), nlm::ConfigSource::Runtime);
                    continue;
                }
                auto boolVal = self.get<bool>(key);
                if (boolVal.has_value()) {
                    newConfig->set(key, boolVal.value(), nlm::ConfigSource::Runtime);
                }
            }
            return newConfig;
        }, "Create a copy of this configuration")
        .def("updateFrom", [](Config& self, const py::dict& data) {
            for (auto item : data) {
                std::string key = py::cast<std::string>(item.first);
                auto value = item.second;
                if (value.is(py::int_())) {
                    self.set(key, py::cast<int>(value), nlm::ConfigSource::Runtime);
                } else if (value.is(py::float_())) {
                    self.set(key, py::cast<double>(value), nlm::ConfigSource::Runtime);
                } else if (value.is(py::str())) {
                    self.set(key, py::cast<std::string>(value), nlm::ConfigSource::Runtime);
                } else if (value.is(py::bool_())) {
                    self.set(key, py::cast<bool>(value), nlm::ConfigSource::Runtime);
                }
            }
        }, py::arg("data"), "Update configuration from a Python dictionary")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Enhanced convenience methods for Brain
    
    // Get brain status and statistics
    .def("getStatistics", [](const Brain& brain) {
        py::dict stats;
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["active_neurons"] = brain.getActiveNeuronCount();
        stats["firing_neurons"] = brain.getFiringNeuronCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["pending_spike_events"] = brain.getPendingSpikeEventCount();
        stats["region_count"] = brain.getRegionCount();
        return stats;
    }, "Get brain statistics as a dictionary")
    
    // Developmental stage convenience methods
    .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
         py::arg("stage"), "Set developmental stage")
    .def("advanceDevelopment", [](Brain& brain, double amount) {
        auto currentStage = brain.getDevelopmentalStage();
        int stageInt = static_cast<int>(currentStage);
        // Simple stage advancement (in real implementation, this would be more sophisticated)
        if (stageInt < 4) { // Don't go beyond Adult
            brain.setDevelopmentalStage(static_cast<DevelopmentalStage>(stageInt + 1));
        }
    }, py::arg("amount"), "Advance developmental stage")
    
    // Check if brain has specific system
    .def("hasSystem", [](const Brain& brain, const std::string& systemName) {
        if (systemName == "working_memory") {
            return brain.getWorkingMemory() != nullptr;
        } else if (systemName == "episodic_memory") {
            return brain.getEpisodicMemory() != nullptr;
        } else if (systemName == "associative_memory") {
            return brain.getAssociativeMemory() != nullptr;
        } else if (systemName == "prediction") {
            return brain.getPredictionSystem() != nullptr;
        } else if (systemName == "planner") {
            return brain.getPlanner() != nullptr;
        } else if (systemName == "concept_formation") {
            return brain.getConceptFormation() != nullptr;
        } else if (systemName == "attention") {
            return brain.getAttention() != nullptr;
        } else if (systemName == "development") {
            return brain.getDevelopmentSystem() != nullptr;
        } else if (systemName == "dopamine") {
            return brain.getDopamine() != nullptr;
        } else if (systemName == "curiosity") {
            return brain.getCuriosity() != nullptr;
        } else if (systemName == "novelty") {
            return brain.getNovelty() != nullptr;
        } else if (systemName == "prediction_error") {
            return brain.getPredictionErrorSignal() != nullptr;
        }
        return false;
    }, py::arg("system_name"), "Check if brain has a specific system")
    
    // Get specific system (returns nullptr if not available)
    .def("getWorkingMemory", [](Brain& brain) {
        auto* mem = brain.getWorkingMemory();n
        // Return as py::object (could be extended with specific type)
        return py::cast(mem);
    }, "Get working memory system")
    .def("getEpisodicMemory", [](Brain& brain) {
        auto* mem = brain.getEpisodicMemory();
        return py::cast(mem);
    }, "Get episodic memory system")
    .def("getAssociativeMemory", [](Brain& brain) {
        auto* mem = brain.getAssociativeMemory();
        return py::cast(mem);
    }, "Get associative memory system")
    .def("getPredictionSystem", [](Brain& brain) {
        auto* pred = brain.getPredictionSystem();
        return py::cast(pred);
    }, "Get prediction system")
    .def("getPlanner", [](Brain& brain) {
        auto* planner = brain.getPlanner();
        return py::cast(planner);
    }, "Get neural planner")
    .def("getConceptFormation", [](Brain& brain) {
        auto* concept = brain.getConceptFormation();
        return py::cast(concept);
    }, "Get concept formation system")
    .def("getAttention", [](Brain& brain) {
        auto* attention = brain.getAttention();
        return py::cast(attention);
    }, "Get attentional selection system")
    .def("getDevelopmentSystem", [](Brain& brain) {
        auto* dev = brain.getDevelopmentSystem();
        return py::cast(dev);
    }, "Get development system")
    .def("getDopamine", [](Brain& brain) {
        auto* dopamine = brain.getDopamine();
        return py::cast(dopamine);
    }, "Get dopamine neuromodulator")
    .def("getCuriosity", [](Brain& brain) {
        auto* curiosity = brain.getCuriosity();
        return py::cast(curiosity);
    }, "Get curiosity neuromodulator")
    .def("getNovelty", [](Brain& brain) {
        auto* novelty = brain.getNovelty();
        return py::cast(novelty);
    }, "Get novelty neuromodulator")
    .def("getPredictionErrorSignal", [](Brain& brain) {
        auto* predError = brain.getPredictionErrorSignal();
        return py::cast(predError);
    }, "Get prediction error signal")
    
    // Configuration convenience methods
    .def("getConfig", &Brain::getConfig,
         py::return_value_policy::reference_internal,
         "Get brain configuration")
    .def("setConfigValue", [](Brain& brain, const std::string& key, 
                             const std::string& value) {
        auto config = brain.getConfig();
        if (config) {
            config->set(key, value, nlm::ConfigSource::Runtime);
        }
    }, py::arg("key"), py::arg("value"), "Set configuration value")
    .def("getConfigValue", [](Brain& brain, const std::string& key) -> py::object {
        auto config = brain.getConfig();
        if (config) {
            auto val = config->get<std::string>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            }
        }
        return py::none();
    }, py::arg("key"), "Get configuration value")
    
    // Random generator convenience methods
    .def("getRandomGenerator", &Brain::getRandomGenerator,
         py::return_value_policy::reference_internal,
         "Get random generator")
    .def("setRandomSeed", [](Brain& brain, uint64_t seed) {
        auto* rng = brain.getRandomGenerator();
        if (rng) {
            // In real implementation, would call rng->setSeed(seed)
        }
    }, py::arg("seed"), "Set random seed")
    
    // Inter-region connection convenience methods
    .def("addConnection", [](Brain& brain, const std::string& sourceRegion,
                           const std::string& targetRegion,
                           float weight = 0.0f, int delay = 1) {
        auto regions = brain.getRegions();
        RegionId sourceId, targetId;
        
        // Find region IDs
        for (const auto& region : regions) {
            if (region->getName() == sourceRegion) {
                sourceId = region->getId();
            }
            if (region->getName() == targetRegion) {
                targetId = region->getId();
            }
        }
        
        if (!sourceId.value && !targetId.value) {
            throw py::value_error("Could not find specified regions");
        }
        
        brain.addInterRegionConnection(sourceId, targetId, weight, delay);
    }, py::arg("source_region"), py::arg("target_region"),
       py::arg("weight") = 0.0f, py::arg("delay") = 1,
       "Add connection between brain regions")
    
    // Get brain status as string
    .def("logStatus", &Brain::logStatus,
         "Log brain status to console")
    
    // Enhanced plasticity methods
    .def("updatePlasticity", &Brain::updatePlasticity,
         "Update plasticity rules (called automatically in step)")
    .def("develop", &Brain::develop,
         "Apply developmental changes (called automatically in step)")
    
    // Enhanced injection methods
    .def("injectCurrentToNeuron", &Brain::injectCurrent,
         py::arg("neuron_id"), py::arg("current"),
         "Inject current directly into a specific neuron")
    .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
         py::arg("neuron_type"), py::arg("current"),
         "Inject current into all neurons of a specific type")
    
    // Sensory input convenience methods
    .def("receiveVisionInput", [](Brain& brain, const std::vector<float>& visionData,
                                 size_t width, size_t height, size_t channels) {
        if (width * height * channels != visionData.size()) {
            throw py::value_error("Vision data size doesn't match dimensions");
        }
        
        Vision vision(width, height, channels);
        vision.setData(visionData);
        
        SensoryInput sensoryInput;
        sensoryInput.setVision(vision);
        
        brain.receiveSensoryInput(sensoryInput);
    }, py::arg("vision_data"), py::arg("width"),
       py::arg("height"), py::arg("channels"),
       "Process vision input data directly")
    .def("receiveAudioInput", [](Brain& brain, const std::vector<float>& audioData,
                                size_t sampleRate, size_t numSamples) {
        if (numSamples != audioData.size()) {
            throw py::value_error("Audio data size doesn't match sample count");
        }
        
        Audio audio(sampleRate, numSamples);
        audio.setData(audioData);
        
        SensoryInput sensoryInput;
        sensoryInput.setAudio(audio);
        
        brain.receiveSensoryInput(sensoryInput);
    }, py::arg("audio_data"), py::arg("sample_rate"),
       py::arg("num_samples"), "Process audio input data directly")
    .def("receiveInternalInput", [](Brain& brain, const std::vector<float>& internalData) {
        InternalSignals internalSignals;
        for (float value : internalData) {
            internalSignals.addSignal(value);
        }
        
        SensoryInput sensoryInput;
        sensoryInput.setInternal(internalSignals);
        
        brain.receiveSensoryInput(sensoryInput);
    }, py::arg("internal_data"), "Process internal signals data directly")
    
    // Enhanced world interaction methods
    .def("getOrCreateRegion", [](Brain& brain, const std::string& name) {
        auto regions = brain.getRegionIds();
        for (const auto& rid : regions) {
            auto* region = brain.getRegion(rid);
            if (region && region->getName() == name) {
                return rid; // Region exists
            }
        }
        // Create new region
        return brain.addRegion(name);
    }, py::arg("name"), "Get existing region or create new one")
    
    // Region management convenience methods
    .def("getRegionNames", [](const Brain& brain) {
        std::vector<std::string> names;
        auto regions = brain.getRegions();
        for (const auto& region : regions) {
            names.push_back(region->getName());
        }
        return names;
    }, "Get names of all brain regions")
    
    // Get region by name
    .def("getRegionByName", [](const Brain& brain, const std::string& name) -> py::object {
        auto regions = brain.getRegions();
        for (const auto& region : regions) {
            if (region->getName() == name) {
                return py::cast(region.get());
            }
        }
        return py::none();
    }, py::arg("name"), "Get region by name")
    
    // Get brain configuration as dictionary
    .def("getConfigAsDict", [](const Brain& brain) {
        auto config = brain.getConfig();
        if (!config) {
            return py::dict();
        }
        
        py::dict configDict;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            // Try different types
            auto intVal = config->get<int>(key);
            if (intVal.has_value()) {
                configDict[key] = intVal.value();
                continue;
            }
            auto doubleVal = config->get<double>(key);
            if (doubleVal.has_value()) {
                configDict[key] = doubleVal.value();
                continue;
            }
            auto stringVal = config->get<std::string>(key);
            if (stringVal.has_value()) {
                configDict[key] = stringVal.value();
                continue;
            }
            auto boolVal = config->get<bool>(key);
            if (boolVal.has_value()) {
                configDict[key] = boolVal.value();
            }
        }
        return configDict;
    }, "Get brain configuration as Python dictionary")
    
    // Set multiple configuration values from dictionary
    .def("updateConfigFromDict", [](Brain& brain, const py::dict& configDict) {
        auto config = brain.getConfig();
        if (!config) {
            return;
        }
        
        for (auto item : configDict) {
            std::string key = py::cast<std::string>(item.first);
            auto value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, py::cast<int>(value), nlm::ConfigSource::Runtime);
            } else if (value.is(py::float_())) {
                config->set(key, py::cast<double>(value), nlm::ConfigSource::Runtime);
            } else if (value.is(py::str())) {
                config->set(key, py::cast<std::string>(value), nlm::ConfigSource::Runtime);
            } else if (value.is(py::bool_())) {
                config->set(key, py::cast<bool>(value), nlm::ConfigSource::Runtime);
            }
        }
    }, py::arg("config_dict"), "Update brain configuration from Python dictionary")
    
    // Reset brain with specific stage
    .def("resetWithStage", [](Brain& brain, DevelopmentalStage stage) {
        brain.reset();
        brain.setDevelopmentalStage(stage);
    }, py::arg("stage"), "Reset brain and set specific developmental stage")
    
    // Enhanced action production
    .def("produceActionWithFallback", [](Brain& brain) -> py::object {
        auto action = brain.produceAction();
        if (action) {
            return py::cast(*action);
        }
        return py::none();
    }, "Produce action or None if no action available")
    
    // Get all system names
    .def("getSystemNames", []() {
        return std::vector<std::string>{
            "working_memory", "episodic_memory", "associative_memory",
            "prediction", "planner", "concept_formation", "attention",
            "development", "dopamine", "curiosity", "novelty",
            "prediction_error", "spike_system", "stdp", "hebbian",
            "structural_plasticity"
        };
    }, "Get list of all system names")

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
        // Convenience methods
        .def("enableLearning", [](AgentBrain& self, bool enable) {
            self.enableRewardModulation(enable);
            self.enableCuriosity(enable);
            self.enableDevelopment(enable);
        }, py::arg("enable"), "Enable/disable all learning systems")
        .def("enableAllPlasticity", [](AgentBrain& self, bool enable) {
            self.enableStructuralPlasticity(enable);
            self.enableDevelopment(enable);
        }, py::arg("enable"), "Enable/disable structural and developmental plasticity")
        .def("getStatus", [](AgentBrain& self) {
            py::dict status;
            status["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            status["neuromodulation_level"] = self.getNeuromodulationLevel();
            status["curiosity_level"] = self.getCuriosityLevel();
            status["novelty_level"] = self.getNoveltyLevel();
            status["prediction_error"] = self.getPredictionError();
            return status;
        }, "Get agent status as a dictionary")
        .def("isLearning", [](AgentBrain& self) {
            return self.isRewardModulationEnabled() && 
                   self.isCuriosityEnabled() && 
                   self.isDevelopmentEnabled();
        }, "Check if learning is enabled")
        .def("getEffectivePlasticity", [](AgentBrain& self) {
            float plasticity = 1.0f;
            if (self.isDevelopmentEnabled()) {
                plasticity *= 0.8f;  // Reduced plasticity during development
            }
            if (self.isStructuralPlasticityEnabled()) {
                plasticity *= 1.2f;  // Enhanced with structural plasticity
            }
            return plasticity;
        }, "Calculate effective plasticity factor")
        .def("createActionFromString", [](AgentBrain& self, const std::string& actionName) -> Action {
            Action action;
            // Map string to action type
            if (actionName == "MoveForward") {
                action.setType(ActionType::MoveForward);
            } else if (actionName == "MoveBackward") {
                action.setType(ActionType::MoveBackward);
            } else if (actionName == "TurnLeft") {
                action.setType(ActionType::TurnLeft);
            } else if (actionName == "TurnRight") {
                action.setType(ActionType::TurnRight);
            } else if (actionName == "Look") {
                action.setType(ActionType::Look);
            } else if (actionName == "LookUp") {
                action.setType(ActionType::LookUp);
            } else if (actionName == "LookDown") {
                action.setType(ActionType::LookDown);
            } else if (actionName == "Interact") {
                action.setType(ActionType::Interact);
            } else if (actionName == "Eat") {
                action.setType(ActionType::Eat);
            } else if (actionName == "Drink") {
                action.setType(ActionType::Drink);
            } else if (actionName == "Rest") {
                action.setType(ActionType::Rest);
            } else if (actionName == "Wait") {
                action.setType(ActionType::Wait);
            } else if (actionName == "Custom") {
                action.setType(ActionType::Custom);
            }
            return action;
        }, py::arg("action_name"), "Create an action from a string name")
        .def("getActionName", [](const Action& action) {
            switch (action.getType()) {
                case ActionType::MoveForward: return std::string("MoveForward");
                case ActionType::MoveBackward: return std::string("MoveBackward");
                case ActionType::TurnLeft: return std::string("TurnLeft");
                case ActionType::TurnRight: return std::string("TurnRight");
                case ActionType::Look: return std::string("Look");
                case ActionType::LookUp: return std::string("LookUp");
                case ActionType::LookDown: return std::string("LookDown");
                case ActionType::Interact: return std::string("Interact");
                case ActionType::Eat: return std::string("Eat");
                case ActionType::Drink: return std::string("Drink");
                case ActionType::Rest: return std::string("Rest");
                case ActionType::Wait: return std::string("Wait");
                case ActionType::Custom: return std::string("Custom");
                default: return std::string("Unknown");
            }
        }, "Get action name from ActionType enum")
        .def("processVisionInput", [](AgentBrain& self, const std::vector<float>& visionData, 
                                      size_t width, size_t height, size_t channels) {
            if (width * height * channels != visionData.size()) {
                throw py::value_error("Vision data size doesn't match dimensions");
            }
            SensoryPercept percept;
            Vision vision(width, height, channels);
            vision.setData(visionData);
            percept.setVision(vision);
            self.processSensoryInput(percept);
        }, py::arg("vision_data"), py::arg("width"), py::arg("height"), py::arg("channels"),
           "Process vision input data directly")
        .def("processTouchInput", [](AgentBrain& self, const std::vector<float>& touchData) {
            SensoryPercept percept;
            // Create touch data (simplified - normally would be more complex)
            std::vector<float> touch = touchData;
            // In a real implementation, this would set up touch neurons
            self.processSensoryInput(percept);
        }, py::arg("touch_data"), "Process touch input data directly")
        .def("processInternalInput", [](AgentBrain& self, const std::vector<float>& internalData) {
            SensoryPercept percept;
            // Create internal signals data (simplified)
            std::vector<float> internal = internalData;
            // In a real implementation, this would set up internal neurons
            self.processSensoryInput(percept);
        }, py::arg("internal_data"), "Process internal signals data directly")
        .def("getActionStatistics", [](const AgentBrain& self) {
            py::dict stats;
            // Get brain reference to access statistics
            auto* brain = self.getBrain();
            if (brain) {
                stats["firing_neurons"] = brain->getFiringNeuronCount();
                stats["total_neurons"] = brain->getTotalNeuronCount();
                stats["total_synapses"] = brain->getTotalSynapseCount();
                stats["average_firing_rate"] = brain->getAverageFiringRate();
                stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
                stats["total_spike_count"] = brain->getTotalSpikeCount();
                stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            }
            return stats;
        }, "Get detailed action and brain statistics")
        .def("enableAdaptiveExploration", [](AgentBrain& self, bool enable) {
            self.enableCuriosity(enable);
            if (enable) {
                // Initialize curiosity-related state
                // This would normally involve more complex setup
            }
        }, py::arg("enable"), "Enable adaptive exploration behavior")
        .def("getExplorationScore", [](const AgentBrain& self) {
            float curiosity = self.getCuriosityLevel();
            float novelty = self.getNoveltyLevel();
            float prediction_error = self.getPredictionError();
            
            // Calculate exploration score (higher means more exploration)
            float score = (curiosity * 0.5f + novelty * 0.3f + std::abs(prediction_error) * 0.2f);
            return std::clamp(score, 0.0f, 1.0f);
        }, "Get exploration score (0-1, higher means more exploration)")
        .def("shouldExplore", [](const AgentBrain& self, float threshold) {
            return self.getExplorationScore() > threshold;
        }, py::arg("threshold"), "Check if the agent should explore based on threshold")
        .def("resetWithState", [](AgentBrain& self, const py::dict& state) {
            self.reset();
            // Restore state from dictionary (simplified)
            // In a real implementation, this would restore more complex state
        }, py::arg("state"), "Reset agent with specific state")
        .def("saveState", [](const AgentBrain& self) {
            py::dict state;
            state["neuromodulation_level"] = self.getNeuromodulationLevel();
            state["curiosity_level"] = self.getCuriosityLevel();
            state["novelty_level"] = self.getNoveltyLevel();
            state["prediction_error"] = self.getPredictionError();
            state["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            state["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            state["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            state["development_enabled"] = self.isDevelopmentEnabled();
            state["curiosity_enabled"] = self.isCuriosityEnabled();
            return state;
        }, "Save agent state to dictionary")
        .def("loadState", [](AgentBrain& self, const py::dict& state) {
            // Simplified state loading - in real implementation would be more complete
            self.reset();
            for (auto item : state) {
                std::string key = py::cast<std::string>(item.first);
                auto value = item.second;
                if (key == "reward_modulation_enabled") {
                    self.enableRewardModulation(py::cast<bool>(value));
                } else if (key == "structural_plasticity_enabled") {
                    self.enableStructuralPlasticity(py::cast<bool>(value));
                } else if (key == "development_enabled") {
                    self.enableDevelopment(py::cast<bool>(value));
                } else if (key == "curiosity_enabled") {
                    self.enableCuriosity(py::cast<bool>(value));
                }
            }
        }, py::arg("state"), "Load agent state from dictionary");

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
