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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_dict", &SensoryInput::getData, "Get sensory data as a dictionary for easy access")
        .def("__repr__", [](const SensoryInput& self) {
            return "<SensoryInput type=" + std::to_string(self.getType()) +
                   " dims=" + std::to_string(self.getDimensions()) + ">";
        });

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
        .def("clone", &Action::clone)
        .def("is_valid", &Action::getType, "Check if action is valid (non-zero type)")
        .def("get_parameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index < params.size()) return params[index];
            throw std::out_of_range("Parameter index out of range");
        }, py::arg("index"), "Get parameter at index (0-based)")
        .def("set_parameter", [](Action& self, size_t index, float value) {
            auto& params = self.getParameters();
            if (index >= params.size()) {
                params.resize(index + 1, 0.0f);
            }
            params[index] = value;
            self.setParameters(params);
        }, py::arg("index"), py::arg("value"), "Set parameter at index (0-based)")
        .def("__repr__", [](const Action& self) {
            return "<Action type=" + Action::getTypeName(self.getType()) +
                   " params=" + std::to_string(self.getParameters().size()) + ">";
        });

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
             "Log brain status");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("get_neuromodulation_summary", [](const AgentBrain& self) {
            return std::to_string(self.getNeuromodulationLevel()) + "/" +
                   std::to_string(self.getCuriosityLevel()) + "/" +
                   std::to_string(self.getNoveltyLevel()) + "/" +
                   std::to_string(self.getPredictionError());
        }, "Get a summary of neuromodulation levels (dopamine/curiosity/novelty/prediction_error)")
        .def("__repr__", [](const AgentBrain& self) {
            DevelopmentalStage stage = self.getDevelopmentalStage();
            const char* stageName = "";
            switch(stage) {
                case DevelopmentalStage::Initial: stageName = "Initial"; break;
                case DevelopmentalStage::CriticalPeriod: stageName = "CriticalPeriod"; break;
                case DevelopmentalStage::Maturation: stageName = "Maturation"; break;
                case DevelopmentalStage::Adult: stageName = "Adult"; break;
                case DevelopmentalStage::Aging: stageName = "Aging"; break;
                default: stageName = "Unknown"; break;
            }
            return "<AgentBrain stage=" + std::string(stageName) +
                   " curiosity=" + std::to_string(self.getCuriosityLevel()) + ">";
        });

    // Add convenience methods for Memory systems
    
    // Helper function to convert vector to Python list
    auto createVectorConverter = []() -> py::object { return py::object(); };
    
    m.def("createWorkingMemory", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralWorkingMemory> {
        auto wm = std::make_shared<NeuralWorkingMemory>();
        if (brain) {
            wm->initialize(brain.get());
        }
        return wm;
    }, py::arg("brain"), "Create a working memory system connected to a brain");
    
    m.def("createEpisodicMemory", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralEpisodicMemory> {
        auto em = std::make_shared<NeuralEpisodicMemory>();
        if (brain) {
            em->initialize(brain.get());
        }
        return em;
    }, py::arg("brain"), "Create an episodic memory system connected to a brain");
    
    m.def("createAssociativeMemory", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralAssociativeMemory> {
        auto am = std::make_shared<NeuralAssociativeMemory>();
        if (brain) {
            am->initialize(brain.get());
        }
        return am;
    }, py::arg("brain"), "Create an associative memory system connected to a brain");
    
    m.def("createDefaultMemoryConfig", []() {
        // Create default memory configuration
        py::dict config;
        config["working_memory_capacity"] = 1000;
        config["working_memory_decay_rate"] = 0.95f;
        config["episodic_memory_max_episodes"] = 10000;
        config["episodic_memory_replay_enabled"] = true;
        config["associative_memory_strength_threshold"] = 0.5f;
        return config;
    }, "Create default memory configuration for the brain");
    
    m.def("createMemorySystem", [](std::shared_ptr<Brain> brain) {
        // Create all memory systems at once
        py::dict memorySystems;
        memorySystems["working_memory"] = createWorkingMemory(brain);
        memorySystems["episodic_memory"] = createEpisodicMemory(brain);
        memorySystems["associative_memory"] = createAssociativeMemory(brain);
        return memorySystems;
    }, py::arg("brain"), "Create all memory systems together");
    
    m.def("store_episode_in_memory", [](std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                                       const std::vector<float>& sensoryPattern,
                                       float reward,
                                       float positionX,
                                       float positionY,
                                       ActionType action) {
        if (!episodicMemory) return;
        
        EpisodicMemoryItem episode;
        episode.timestamp = 0; // Would need actual timestamp from brain
        episode.sensoryState = sensoryPattern;
        episode.positionX = positionX;
        episode.positionY = positionY;
        episode.action = action;
        episode.reward = reward;
        episode.energy = 1.0f;
        episode.novelty = 0.0f;
        episode.resultingSensoryState = sensoryPattern;
        episode.resultingReward = reward;
        episode.age = 0;
        
        episodicMemory->storeEpisode(episode);
    }, py::arg("episodic_memory"), py::arg("sensory_pattern"), py::arg("reward"), 
        py::arg("position_x") = 0.0f, py::arg("position_y") = 0.0f,
        py::arg("action") = ActionType::Wait,
        "Store an episode in episodic memory");
    
    m.def("query_memory_by_location", [](std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                                        float x, float y, float radius,
                                        size_t maxResults = 5) {
        if (!episodicMemory) return std::vector<const EpisodicMemoryItem*>();
        
        return episodicMemory->retrieveByLocation(x, y, radius, maxResults);
    }, py::arg("episodic_memory"), py::arg("x"), py::arg("y"), py::arg("radius") = 1.0f,
        py::arg("max_results") = 5,
        "Retrieve episodes by location");
    
    m.def("query_memory_by_action", [](std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                                      ActionType action,
                                      size_t maxResults = 5) {
        if (!episodicMemory) return std::vector<const EpisodicMemoryItem*>();
        
        return episodicMemory->retrieveAfterAction(action, maxResults);
    }, py::arg("episodic_memory"), py::arg("action"), py::arg("max_results") = 5,
        "Retrieve episodes after a specific action");
    
    m.def("replay_memory_sequence", [](std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                                      const std::vector<size_t>& episodeIds) {
        if (!episodicMemory) return;
        
        episodicMemory->replaySequence(episodeIds);
    }, py::arg("episodic_memory"), py::arg("episode_ids"),
        "Replay a sequence of episodes for consolidation");
    
    m.def("store_association", [](std::shared_ptr<NeuralAssociativeMemory> associativeMemory,
                                const std::vector<float>& patternA,
                                const std::vector<float>& patternB,
                                float strength = 1.0f) {
        if (!associativeMemory) return;
        
        associativeMemory->associate(patternA, patternB, strength);
    }, py::arg("associative_memory"), py::arg("pattern_a"), py::arg("pattern_b"),
        py::arg("strength") = 1.0f, "Create association between two patterns");
    
    m.def("spread_activation", [](std::shared_ptr<NeuralAssociativeMemory> associativeMemory,
                                const std::vector<float>& cuePattern,
                                size_t steps = 2) {
        if (!associativeMemory) return std::vector<std::vector<float>>();
        
        return associativeMemory->spreadActivation(cuePattern, steps);
    }, py::arg("associative_memory"), py::arg("cue_pattern"), py::arg("steps") = 2,
        "Spread activation from a cue pattern to related patterns");
    
    m.def("get_memory_system_stats", [](std::shared_ptr<NeuralWorkingMemory> workingMemory,
                                      std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                                      std::shared_ptr<NeuralAssociativeMemory> associativeMemory) {
        py::dict stats;
        
        if (workingMemory) {
            py::dict wmStats;
            wmStats["active_traces"] = workingMemory->getActiveTraces();
            wmStats["capacity"] = workingMemory->getCapacity();
            wmStats["decay_rate"] = workingMemory->getDecayRate();
            wmStats["memory_activity"] = workingMemory->getMemoryActivity();
            stats["working_memory"] = wmStats;
        }
        
        if (episodicMemory) {
            py::dict emStats;
            emStats["episode_count"] = episodicMemory->getEpisodeCount();
            emStats["replay_enabled"] = episodicMemory->isReplayEnabled();
            emStats["average_reward"] = episodicMemory->getAverageReward();
            stats["episodic_memory"] = emStats;
        }
        
        if (associativeMemory) {
            py::dict amStats;
            amStats["association_count"] = associativeMemory->getAssociationCount();
            stats["associative_memory"] = amStats;
        }
        
        return stats;
    }, py::arg("working_memory"), py::arg("episodic_memory"), py::arg("associative_memory"),
        "Get statistics from all memory systems");
    
    // Add bindings for prediction and cognitive systems
    
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction and error computation)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState,
             py::arg("currentState"), "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"), "Update predictions based on actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction error history")
        .def("train", &PredictionSystem::train,
             py::arg("observation"), "Train prediction model")
        .def("get_recent_errors", [](const PredictionSystem& self, size_t count) {
            const auto& history = self.getErrorHistory();
            size_t start = std::max<size_t>(history.size() - count, 0);
            return std::vector<float>(history.begin() + start, history.end());
        }, py::arg("count") = 10, "Get recent prediction errors")
        .def("get_avg_error", [](const PredictionSystem& self) {
            const auto& history = self.getErrorHistory();
            if (history.empty()) return 0.0f;
            float sum = 0.0f;
            for (float error : history) sum += error;
            return sum / history.size();
        }, "Get average prediction error")
        .def("is_learning", [](const PredictionSystem& self) {
            return self.getErrorHistory().size() > 0;
        }, "Check if prediction system is learning");

    // Add bindings for neuromodulation systems
    
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get the name of the neuromodulator")
        .def("getLevel", &Neuromodulator::getLevel, "Get the current level/concentration")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set the current level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get the plasticity factor applied by this neuromodulator");

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine - reward and reinforcement signal)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"), "Signal reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity - exploration motivation based on novelty and prediction error)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("prediction_error"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"), "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"), "Set prediction error weight")
        .def("reset", &Curiosity::reset, "Reset curiosity state")
        .def("get_summary", [](const Curiosity& self) {
            return std::to_string(self.getLevel()) + " exploration_drive=" + std::to_string(self.getExplorationDrive());
        }, "Get curiosity system summary");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection signal for change detection)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Novelty::getLevel, "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& currentPattern,
                                const std::vector<float>& previousPattern) {
            self.detectNovelty(currentPattern, previousPattern);
        }, py::arg("current_pattern"), py::arg("previous_pattern"), "Detect novelty from pattern difference")
        .def("getHistory", &Novelty::getHistory, "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory, "Clear novelty history")
        .def("update", &Novelty::update, py::arg("dt"), "Decay novelty over time");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("getValue", &PredictionError::getValue, "Get current prediction error")
        .def("setValue", &PredictionError::setValue, py::arg("value"), "Set prediction error")
        .def("update", [](PredictionError& self, float prediction, float actual) {
            self.setValue(actual - prediction);  // Simple error calculation
        }, py::arg("prediction"), py::arg("actual"), "Update prediction error");

    m.def("createNeuromodulationSystem", []() {
        py::dict neuromodulation;
        neuromodulation["dopamine"] = std::make_shared<Dopamine>();
        neuromodulation["curiosity"] = std::make_shared<Curiosity>();
        neuromodulation["novelty"] = std::make_shared<Novelty>();
        neuromodulation["prediction_error"] = std::make_shared<PredictionError>();
        return neuromodulation;
    }, "Create all neuromodulation systems");

    // Add bindings for plasticity systems
    
    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"), "Set LTP weight")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get LTP weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"), "Set LTD weight")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get LTD weight")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"), "Set STDP time constant")
        .def("getTimeConstant", &STDP::getTimeConstant, "Get STDP time constant")
        .def("configure", &STDP::configure, py::arg("ltp"), py::arg("ltd"), py::arg("tau"),
             "Configure STDP parameters");

    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Hebbian learning)pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getLearningRate", &Hebbian::getLearningRate, "Get learning rate")
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"), "Set max weight")
        .def("getMaxWeight", &Hebbian::getMaxWeight, "Get max weight");

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity - creation/removal of synapses and neurons)pbdoc")
        .def(py::init<>())
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate, "Get synaptogenesis rate")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate,
             py::arg("rate"), "Set synaptogenesis rate")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate, "Get pruning rate")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"), "Set pruning rate")
        .def("update", &StructuralPlasticity::update, py::arg("brain"), py::arg("rng"),
             "Update structural plasticity");

    // Add bindings for SpikeSystem
    
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Spike system for neural dynamics)pbdoc")
        .def(py::init<>())
        .def("step", &SpikeSystem::step, py::arg("currentStep"), "Perform spike system step");

    // Add bindings for MotorSystem
    
    py::class_<MotorSystem>(m, "MotorSystem", R"pbdoc(Motor system for action production)pbdoc")
        .def(py::init<>())
        .def("step", &MotorSystem::step, py::arg("currentStep"), "Perform motor system step")
        .def("selectAction", &MotorSystem::selectAction, py::arg("motorRegion"), "Select action based on neural activity")
        .def("getAvailableActions", &MotorSystem::getAvailableActions, "Get available actions")
        .def("setActionPreference", &MotorSystem::setActionPreference, py::arg("action"), py::arg("preference"), "Set action preference for exploration")
        .def("getActionHistory", &MotorSystem::getActionHistory, "Get action history")
        .def("clearHistory", &MotorSystem::clearHistory, "Clear action history");

    // Add bindings for development systems
    
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system: orchestrates neural development and plasticity modulation)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage, "Advance to next developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName, "Get descriptive name of current stage")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier, "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod, "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress, "Get critical period completion progress")
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalAge, "Get developmental age in simulation time")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"), "Set developmental age in simulation time")
        .def("update", [](DevelopmentSystem& self, Brain& brain, SimulationStep currentStep) {
            self.update(&brain, currentStep);
        }, py::arg("brain"), py::arg("current_step"), "Update development system (simple version)")
        .def("update_full", [](DevelopmentSystem& self, Brain& brain, RandomGenerator& rng, TimestepDuration dt) {
            self.update(&brain, rng, dt);
        }, py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development system with full parameters")
        .def("get_summary", [](const DevelopmentSystem& self) {
            std::string summary = "Developmental Stage: " + std::string(self.getStageName()) + "\n";
            summary += "Age: " + std::to_string(self.getDevelopmentalAge()) + "\n";
            summary += "Plasticity: " + std::to_string(self.getPlasticityModifier()) + "\n";
            summary += "Critical Period: " + std::to_string(self.isCriticalPeriod()) + "\n";
            if (self.isCriticalPeriod()) {
                summary += "Critical Period Progress: " + std::to_string(self.getCriticalPeriodProgress()) + "\n";
            }
            return summary;
        }, "Get detailed development system summary");

    // Add factory functions for development systems
    
    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    }, "Create a development system");

    m.def("createDevelopmentConfig", []() {
        py::dict config;
        config["initial_stage"] = "Initial";
        config["critical_period_duration"] = 300.0;  // simulation seconds
        config["maturation_duration"] = 600.0;       // simulation seconds
        config["adult_plasticity"] = 0.2f;
        config["aging_plasticity"] = 0.1f;
        return config;
    }, "Create default development configuration");

    // Add convenience functions for development
    
    m.def("advance_development_stage", [](DevelopmentSystem& self) {
        self.advanceStage();
    }, py::arg("development_system"), "Advance development to next stage");

    m.def("set_development_to_critical_period", [](DevelopmentSystem& self) {
        self.setStage(DevelopmentalStage::CriticalPeriod);
    }, py::arg("development_system"), "Set development to critical period stage");

    m.def("set_development_to_adult", [](DevelopmentSystem& self) {
        self.setStage(DevelopmentalStage::Adult);
    }, py::arg("development_system"), "Set development to adult stage");

    // Add system statistics function
    
    m.def("get_development_system_stats", [](Brain& brain) {
        py::dict stats;
        
        DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
        if (devSystem) {
            py::dict dsStats;
            dsStats["stage"] = devSystem->getStageName();
            dsStats["age"] = devSystem->getDevelopmentalAge();
            dsStats["plasticity_modifier"] = devSystem->getPlasticityModifier();
            dsStats["is_critical_period"] = devSystem->isCriticalPeriod();
            if (devSystem->isCriticalPeriod()) {
                dsStats["critical_period_progress"] = devSystem->getCriticalPeriodProgress();
            }
            stats["development_system"] = dsStats;
        }
        
        // Add age-based predictions
        double age = devSystem ? devSystem->getDevelopmentalAge() : 0.0;
        if (age < 60.0) {
            stats["developmental_phase"] = "Initial (high plasticity)";
        } else if (age < 300.0) {
            stats["developmental_phase"] = "Critical Period (high learning)";
        } else if (age < 900.0) {
            stats["developmental_phase"] = "Maturation (moderate plasticity)";
        } else {
            stats["developmental_phase"] = "Adult (stable)";
        }
        
        return stats;
    }, py::arg("brain"), "Get development system statistics for a brain");

    m.def("get_development_report", [](Brain& brain) {
        DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
        if (!devSystem) {
            return std::string("No development system available");
        }
        
        std::string report = "Development System Report\n";
        report += "======================\n\n";
        report += "Current Stage: " + std::string(devSystem->getStageName()) + "\n";
        report += "Developmental Age: " + std::to_string(devSystem->getDevelopmentalAge()) + " simulation seconds\n";
        report += "Plasticity Modifier: " + std::to_string(devSystem->getPlasticityModifier()) + "\n";
        report += "Critical Period: " + std::string(devSystem->isCriticalPeriod() ? "Yes" : "No") + "\n";
        
        if (devSystem->isCriticalPeriod()) {
            report += "Critical Period Progress: " + std::to_string(devSystem->getCriticalPeriodProgress() * 100.0) + "%\n";
        }
        
        report += "\nDevelopmental Timeline:\n";
        double age = devSystem->getDevelopmentalAge();
        if (age < 60.0) {
            remaining = 60.0 - age;
            report += f"  - Initial stage: {remaining:.1f}s remaining (high plasticity)\n";
        } else if (age < 300.0) {
            remaining = 300.0 - age;
            report += f"  - Critical Period: {remaining:.1f}s remaining (critical learning)\n";
        } else if (age < 900.0) {
            remaining = 900.0 - age;
            report += f"  - Maturation: {remaining:.1f}s remaining (moderate plasticity)\n";
        } else {
            report += "  - Adult stage (stable plasticity)\n";
        }
        
        return report;
    }, py::arg("brain"), "Generate a human-readable development system report");
