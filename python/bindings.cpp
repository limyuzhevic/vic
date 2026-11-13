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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                bool result = self.loadFromFile(filepath);
                if (!result) {
                    PyErr_SetString(PyExc_RuntimeError(), ("Failed to load configuration from: " + filepath).c_str());
                }
                return result;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError(), ("Error loading config from " + filepath + ": " + std::string(e.what())).c_str());
                return false;
            }
        }, py::arg("filepath"), "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            try {
                std::vector<char*> argv;
                argv.reserve(args.size());
                for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
                return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError(), ("Error loading config from args: " + std::string(e.what())).c_str());
                return false;
            }
        }, py::arg("args"), "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                bool result = self.saveToFile(filepath);
                if (!result) {
                    PyErr_SetString(PyExc_RuntimeError(), ("Failed to save configuration to: " + filepath).c_str());
                }
                return result;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError(), ("Error saving config to " + filepath + ": " + std::string(e.what())).c_str());
                return false;
            }
        }, py::arg("filepath"), "Save configuration to a JSON file")
        .def("get", [](const Config& self, const std::string& key) {
            try {
                auto result = self.get(key);
                if (result.has_value()) {
                    return *result;
                }
                PyErr_SetString(PyExc_LookupError(), ("Key not found: " + key).c_str());
                return nullptr;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError(), ("Error getting config key " + key + ": " + std::string(e.what())).c_str());
                return nullptr;
            }
        }, py::arg("key"), "Get a configuration value (returns nullopt if not found)")
        .def("getWithDefault", [](const Config& self, const std::string& key, auto defaultValue) {
            return self.getOr(defaultValue, key);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("setInt", &Config::set, py::arg("key"), py::arg("value"), py::arg("source"),
             "Set an integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, float value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source"), "Set a float configuration value")
        .def("setDouble", [](Config& self, const std::string& key, double value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source"), "Set a double configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source"), "Set a string configuration value")
        .def("setBool", &Config::set, py::arg("key"), py::arg("value"), py::arg("source"),
             "Set a boolean configuration value")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal);

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", &Vision::getData)
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

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp");

    py::enum_<SensoryInputType>(m, "SensoryInputType", R"pbdoc(Sensory input type enumeration)pbdoc")
        .value("Vision", SensoryInputType::Vision)
        .value("Audio", SensoryInputType::Audio)
        .value("Internal", SensoryInputType::Internal)
        .export_values();

    // Add memory systems
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory system for NLM)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuronId"), py::arg("activation"))
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons)
        .def("getMemoryActivations", &NeuralWorkingMemory::getMemoryActivations)
        .def("getMemoryTimestamps", &NeuralWorkingMemory::getMemoryTimestamps)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces)
        .def("update", &NeuralWorkingMemory::update, py::arg("timestep"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"));

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory system for NLM)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("getEpisodes", &NeuralEpisodicMemory::getEpisodes)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("maxEpisodes"))
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"))
        .def("setMaxEpisodes", &NeuralEpisodicMemory::setMaxEpisodes, py::arg("maxEpisodes"))
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"))
        .def("clear", &NeuralEpisodicMemory::clear);

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory system for NLM)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"))
        .def("addAssociation", &NeuralAssociativeMemory::addAssociation,
             py::arg("pattern"), py::arg("associatedPattern"), py::arg("strength"))
        .def("recall", &NeuralAssociativeMemory::recall, py::arg("pattern"))
        .def("clear", &NeuralAssociativeMemory::clear);

    // Add prediction system
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for NLM)pbdoc")
        .def(py::init<>())
        .def("update", &PredictionSystem::update, py::arg("brain"), py::arg("timestep"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("reset", &PredictionSystem::reset);

    // Add cognition systems
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning system for NLM)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"))
        .def("planAction", &NeuralPlanner::planAction, py::arg("sensoryInput"))
        .def("getAction", &NeuralPlanner::getAction)
        .def("getConfidence", &NeuralPlanner::getConfidence)
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("reset", &NeuralPlanner::reset);

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system for NLM)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"))
        .def("processPattern", &ConceptFormation::processPattern, py::arg("pattern"))
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("pattern1"), py::arg("pattern2"))
        .def("getConceptBiasCount", &ConceptFormation::getConceptBiasCount)
        .def("reset", &ConceptFormation::reset);

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attention system for NLM)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"))
        .def("update", &AttentionalSelection::update, py::arg("timestep"))
        .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("neurons"))
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"))
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"))
        .def("reset", &AttentionalSelection::reset);

    // Add neuromodulation systems
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for NLM)pbdoc")
        .def(py::init<>())
        .def("initialize", &Dopamine::initialize, py::arg("brain"))
        .def("update", &Dopamine::update, py::arg("timestep"))
        .def("getLevel", &Dopamine::getLevel)
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("reset", &Dopamine::reset);

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator for NLM)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"))
        .def("update", &Curiosity::update, py::arg("noveltyLevel"), py::arg("predictionError"), py::arg("timestep"))
        .def("getLevel", &Curiosity::getLevel)
        .def("reset", &Curiosity::reset);

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator for NLM)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"))
        .def("update", &Novelty::update, py::arg("timestep"))
        .def("getLevel", &Novelty::getLevel)
        .def("reset", &Novelty::reset);

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator for NLM)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"))
        .def("update", &PredictionError::update, py::arg("prediction"), py::arg("sensory"), py::arg("timestep"))
        .def("getError", &PredictionError::getError)
        .def("reset", &PredictionError::reset);

    // Add development system
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system for NLM)pbdoc")
        .def(py::init<>())
        .def("initialize", &DevelopmentSystem::initialize, py::arg("brain"))
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("timestep"))
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage)
        .def("setDevelopmentalStage", &DevelopmentSystem::setDevelopmentalStage, py::arg("stage"))
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier)
        .def("reset", &DevelopmentSystem::reset);

    // Add plasticity systems
    py::class_<STDP>(m, "STDP", R"pbdoc(Spiking timing dependent plasticity for NLM)pbdoc")
        .def(py::init<>())
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"))
        .def("update", &STDP::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("timestep"))
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"))
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"))
        .def("setTau", &STDP::setTau, py::arg("tau"))
        .def("reset", &STDP::reset);

    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian plasticity for NLM)pbdoc")
        .def(py::init<>())
        .def("update", &Hebbian::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("timestep"))
        .def("reset", &Hebbian::reset);

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity for NLM)pbdoc")
        .def(py::init<>())
        .def("update", &StructuralPlasticity::update, py::arg("brain"), py::arg("rng"))
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate, py::arg("rate"))
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"))
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate)
        .def("getPruningRate", &StructuralPlasticity::getPruningRate)
        .def("reset", &StructuralPlasticity::reset);

    // Add spike system
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Spike system for NLM)pbdoc")
        .def(py::init<>())
        .def("registerHandler", &SpikeSystem::registerHandler)
        .def("registerDelayedHandler", &SpikeSystem::registerDelayedHandler)
        .def("queueSpike", &SpikeSystem::queueSpike)
        .def("queueDelayedSpike", &SpikeSystem::queueDelayedSpike)
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("step"))
        .def("processDelayedSpikes", &SpikeSystem::processDelayedSpikes, py::arg("step"), py::arg("time"))
        .def("getPendingSpikeCount", &SpikeSystem::getPendingSpikeCount)
        .def("getPendingDelayedCount", &SpikeSystem::getPendingDelayedCount)
        .def("getPendingSpikeEvents", &SpikeSystem::getPendingSpikeEvents)
        .def("getPendingDelayedEvents", &SpikeSystem::getPendingDelayedEvents)
        .def("reset", &SpikeSystem::reset);

    // Add experiment system
    py::class_<Experiment>(m, "Experiment", R"pbdoc(Experiment system for NLM)pbdoc")
        .def(py::init<>())
        .def("run", &Experiment::run)
        .def("reset", &Experiment::reset);

    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(Experiment runner for NLM)pbdoc")
        .def(py::init<>())
        .def("addExperiment", &ExperimentRunner::addExperiment, py::arg("experiment"))
        .def("runAll", &ExperimentRunner::runAll)
        .def("clear", &ExperimentRunner::clear);

    // Add checkpoint system
    py::class_<CheckpointSystem>(m, "CheckpointSystem", R"pbdoc(Checkpoint system for NLM)pbdoc")
        .def(py::init<>())
        .def("configure", &CheckpointSystem::configure,
             py::arg("directory"), py::arg("maxCheckpoints"), py::arg("maxHistory"), py::arg("autoSave"))
        .def("save", &CheckpointSystem::save, py::arg("filepath"))
        .def("load", &CheckpointSystem::load, py::arg("filepath"))
        .def("update", &CheckpointSystem::update, py::arg("step"), py::arg("time"))
        .def("getCheckpointCount", &CheckpointSystem::getCheckpointCount)
        .def("getCheckpointHistory", &CheckpointSystem::getCheckpointHistory)
        .def("clear", &CheckpointSystem::clear);

    // Add SelfModel and SocialLearning classes
    py::class_<SelfModel>(m, "SelfModel", R"pbdoc(Self-model for NLM)pbdoc")
        .def(py::init<>())
        .def("update", &SelfModel::update, py::arg("percept"), py::arg("action"), py::arg("reward"), py::arg("timestep"))
        .def("getConfidence", &SelfModel::getConfidence)
        .def("getPattern", &SelfModel::getPattern)
        .def("reset", &SelfModel::reset);

    py::class_<SocialLearning>(m, "SocialLearning", R"pbdoc(Social learning system for NLM)pbdoc")
        .def(py::init<>())
        .def("observeOtherAgent", &SocialLearning::observeOtherAgent,
             py::arg("action"), py::arg("beforeVision"), py::arg("afterVision"))
        .def("getSocialObservationCount", &SocialLearning::getSocialObservationCount)
        .def("getSelfModelConfidence", &SocialLearning::getSelfModelConfidence)
        .def("reset", &SocialLearning::reset);

    // Add optional development helpers
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

    // Add constant values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
             "Log brain status");

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

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

    // Python convenience functions
    m.def("validate_config", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors = config->validate();
        if (!errors.empty()) {
            std::stringstream ss;
            ss << "Configuration validation errors:";
            for (const auto& error : errors) {
                ss << "\n  - " << error;
            }
            PyErr_SetString(PyExc_ValueError(), ss.str().c_str());
            return false;
        }
        return true;
    }, py::arg("config"), "Validate configuration and return True if valid");

    m.def("apply_config_defaults", [](std::shared_ptr<Config> config) {
        config->applyDefaults();
    }, py::arg("config"), "Apply default values for missing configuration keys");

    m.def("get_config_documentation", [](const std::string& key = "") {
        return Config::getConfigDocumentation(key);
    }, py::arg("key") = "", "Get documentation for configuration key or all keys");

    m.def("get_all_config_keys", []() {
        return Config::getAllConfigKeys();
    }, "Get all available configuration keys");

    m.def("run_simple_demo", [](size_t neuronCount = 500, size_t steps = 1000) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("neuronCount") = 500, py::arg("steps") = 1000,
       "Run a simple demo and return total spike count");

    m.def("run_learning_experiment", [](size_t neuronCount = 500, size_t steps = 1000) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Enable plasticity
        for (const auto& region : brain->getRegions()) {
            for (auto& synapse : region->getSynapses()) {
                synapse->enablePlasticity(true, true, false);
            }
        }
        
        for (size_t step = 0; step < steps; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        // Return statistics
        py::dict stats;
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        
        return stats;
    }, py::arg("neuronCount") = 500, py::arg("steps") = 1000,
       "Run a learning experiment and return statistics");

    m.def("export_brain_state", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("import_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");

    m.def("print_brain_stats", [](const std::shared_ptr<Brain>& brain) {
        brain->logStatus();
    }, py::arg("brain"), "Print brain statistics");
}

} // namespace nlm


