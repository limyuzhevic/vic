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
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/sensory/SensoryPercept.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        Complete Python bindings for the NLM neural simulation framework.
        Includes all integrated memory, cognition, prediction, and neuromodulation systems.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Neuron types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse types
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental stages
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing states
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Action types
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

    // Motor commands
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

    // World object types
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ==========================================
    // Core Types
    // ==========================================

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

    // ==========================================
    // Configuration
    // ==========================================

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

    // ==========================================
    // Sensory Processing
    // ==========================================

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

    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction mechanism)pbdoc")
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"),
             "Initialize neural prediction")
        .def("setPredictor", &NeuralPrediction::setPredictor,
             py::arg("predictor"),
             "Set prediction function")
        .def("predict", &NeuralPrediction::predict,
             py::arg("state")("State to predict"))
        .def("getPrediction", &NeuralPrediction::getPrediction, py::arg("timeStep"),
             "Get prediction for time step")
        .def("update", &NeuralPrediction::update, py::arg("newState"),
             "Update prediction with new state");

    // ==========================================
    // Brain
    // ==========================================

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with integrated systems)pbdoc")
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
        // Memory systems
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        // Prediction system
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        // Cognition systems
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner system")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system")
        // Development system
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        // Neuromodulation systems
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty detection")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        // Plasticity
        .def("getSTDP", &Brain::getSTDP,
             "Get STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian,
             "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             "Get structural plasticity system")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             "Get spike system")
        .def("__repr__", [](const Brain& b) {
            return "<Brain: " + std::to_string(b.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(b.getTotalSynapseCount()) + 
                   " synapses>";
        });

    // ==========================================
    // AgentBrain
    // ==========================================

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // ==========================================
    // World
    // ==========================================

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

    // ==========================================
    // Memory Systems
    // ==========================================

    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory system for active information)pbdoc")
        .def("initialize", &WorkingMemory::initialize, py::arg("brain"),
             "Initialize working memory with brain reference")
        .def("storeToNeuron", &WorkingMemory::storeToNeuron,
             py::arg("neuronId"), py::arg("activation"),
             "Store neural activation in working memory")
        .def("getActiveTraces", &WorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("update", &WorkingMemory::update, py::arg("timestep"),
             "Update working memory")
        .def("clear", &WorkingMemory::clear,
             "Clear all working memory traces")
        .def("setCapacity", &WorkingMemory::setCapacity, py::arg("capacity"),
             "Set working memory capacity");

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural implementation of working memory)pbdoc")
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize neural working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuronId"), py::arg("activation"),
             "Store neural activation in working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active working memory traces")
        .def("update", &NeuralWorkingMemory::update, py::arg("timestep"),
             "Update working memory dynamics")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear all working memory traces")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"),
             "Set working memory capacity")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory system for experience storage)pbdoc")
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize episodic memory")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store an episode in memory")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults"),
             "Retrieve similar episodes by pattern")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults"),
             "Retrieve episodes by time range")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults"),
             "Retrieve episodes by spatial location")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults"),
             "Retrieve episodes after specific action")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode,
             py::arg("episode"),
             "Replay an episode for consolidation")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"),
             "Get an episode by index")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay,
             py::arg("count"),
             "Get episodes for replay (consolidation)")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             "Get average reward across all episodes")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear all episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Consolidate and prune episodes");

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory for pattern associations)pbdoc")
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize associative memory")
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength"),
             "Associate two patterns together")
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience,
             py::arg("episode"),
             "Associate patterns from experience")
        .def("retrieve", &NeuralAssociativeMemory::retrieve,
             py::arg("queryPattern"), py::arg("maxResults"),
             "Retrieve associated patterns")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"),
             "Get association strength between patterns")
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
             "Update association strength")
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation,
             py::arg("cuePattern"), py::arg("steps"),
             "Perform activation spreading")
        .def("clear", &NeuralAssociativeMemory::clear,
             "Clear all associations");

    // ==========================================
    // Neuromodulation Systems
    // ==========================================

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward signaling)pbdoc")
        .def("initialize", &Dopamine::initialize, py::arg("brain"),
             "Initialize dopamine system")
        .def("update", &Dopamine::update, py::arg("timestep"),
             "Update dopamine level")
        .def("getLevel", &Dopamine::getLevel,
             "Get current dopamine level")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"),
             "Set dopamine level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get plasticity modulation factor");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator for exploration)pbdoc")
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             "Initialize curiosity system")
        .def("update", &Curiosity::update, py::arg("timestep"),
             "Update curiosity level")
        .def("getLevel", &Curiosity::getLevel,
             "Get current curiosity level")
        .def("setLevel", &Curiosity::setLevel, py::arg("level"),
             "Set curiosity level");

    py::class_<Novelty>(m, "Novelty", R"pbdoc( novelty detection neuromodulator)pbdoc")
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             "Initialize novelty detector")
        .def("update", &Novelty::update, py::arg("timestep"),
             "Update novelty level")
        .def("getLevel", &Novelty::getLevel,
             "Get current novelty level");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             "Initialize prediction error system")
        .def("update", &PredictionError::update, py::arg("timestep"),
             "Update prediction error")
        .def("getLevel", &PredictionError::getLevel,
             "Get current prediction error level")
        .def("setLevel", &PredictionError::setLevel, py::arg("level"),
             "Set prediction error level");

    // ==========================================
    // Cognition Systems
    // ==========================================

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize planner")
        .def("planAction", &NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("maxDepth"),
             "Plan action sequence")
        .def("evaluateActionSequence", &NeuralPlanner::evaluateActionSequence,
             py::arg("sequence"),
             "Evaluate action sequence quality")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get planning horizon depth")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning horizon depth");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from experience patterns)pbdoc")
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize concept formation")
        .def("processPattern", &ConceptFormation::processPattern,
             py::arg("pattern"), py::arg("isPositive"),
             "Process a pattern to form concept")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get current concepts")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"),
             "Get concept stability measure")
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("conceptId"), py::arg("updateVector"),
             "Update concept representation");

    // ==========================================
    // Prediction Systems
    // ==========================================

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for temporal sequences)pbdoc")
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"),
             "Initialize prediction system")
        .def("predictNextState", &PredictionSystem::predictNextState,
             py::arg("currentState"),
             "Predict next state")
        .def("getPrediction", &PredictionSystem::getPrediction, py::arg("timeStep"),
             "Get prediction for specific time step")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("updatePrediction", &PredictionSystem::updatePrediction,
             py::arg("actualState"),
             "Update prediction with actual state")
        .def("getConfidence", &PredictionSystem::getConfidence, py::arg("timeStep"),
             "Get prediction confidence")
        .def("getPredictionHistory", &PredictionSystem::getPredictionHistory,
             "Get prediction history");

    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction mechanism)pbdoc")
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"),
             "Initialize neural prediction")
        .def("setPredictor", &NeuralPrediction::setPredictor,
             py::arg("predictor"),
             "Set prediction function")
        .def("predict", &NeuralPrediction::predict,
             py::arg("state")("State to predict")
        .def("getPrediction", &NeuralPrediction::getPrediction, py::arg("timeStep"),
             "Get prediction for time step")
        .def("update", &NeuralPrediction::update, py::arg("newState"),
             "Update prediction with new state");

    // ==========================================
    // Development System
    // ==========================================

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Developmental system for brain maturation)pbdoc")
        .def("initialize", &DevelopmentSystem::initialize, py::arg("brain"),
             "Initialize development system")
        .def("update", &DevelopmentSystem::update,
             py::arg("brain"), py::arg("rng"), py::arg("timestep"),
             "Update development")
        .def("getCurrentStage", &DevelopmentSystem::getCurrentStage,
             "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set developmental stage")
        .def("getSynaptogenesisRate", &DevelopmentSystem::getSynaptogenesisRate,
             "Get synaptogenesis rate")
        .def("getPruningRate", &DevelopmentSystem::getPruningRate,
             "Get pruning rate")
        .def("getMaturationLevel", &DevelopmentSystem::getMaturationLevel,
             "Get maturation level");

    // ==========================================
    // Plasticity Systems
    // ==========================================

    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def("initialize", &STDP::initialize, py::arg("brain"),
             "Initialize STDP")
        .def("update", &STDP::update,
             py::arg("synapse"), py::arg("preHistory"), py::arg("postHistory"),
             py::arg("timestep"), "Update synapse weights using STDP")
        .def("configure", &STDP::configure,
             py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             "Configure STDP parameters")
        .def("getLTPWeight", &STDP::getLTPWeight,
             "Get long-term potentiation weight")
        .def("getLTDWeight", &STDP::getLTDWeight,
             "Get long-term depression weight");

    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def("initialize", &Hebbian::initialize, py::arg("brain"),
             "Initialize Hebbian learning")
        .def("update", &Hebbian::update,
             py::arg("synapse"), py::arg("preHistory"), py::arg("postHistory"),
             py::arg("timestep"), "Update synapse weights using Hebbian rule")
        .def("getLearningRate", &Hebbian::getLearningRate,
             "Get learning rate");

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity for synapse formation/removal)pbdoc")
        .def("initialize", &StructuralPlasticity::initialize, py::arg("brain"),
             "Initialize structural plasticity")
        .def("update", &StructuralPlasticity::update,
             py::arg("brain"), py::arg("rng"),
             "Update structural plasticity")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate, py::arg("rate"),
             "Set synaptogenesis rate")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"),
             "Set pruning rate")
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate,
             "Get synaptogenesis rate")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate,
             "Get pruning rate");

    // ==========================================
    // Performance Infrastructure
    // ==========================================

    py::class_<MemoryPoolBase>(m, "MemoryPoolBase", R"pbdoc(Base class for memory pools)")
        .def("allocate", &MemoryPoolBase::allocate,
             "Allocate memory from pool")
        .def("deallocate", &MemoryPoolBase::deallocate,
             py::arg("ptr"), "Return memory to pool")
        .def("empty", &MemoryPoolBase::empty,
             "Check if pool is empty")
        .def("allocatedCount", &MemoryPoolBase::allocatedCount,
             "Get number of allocated objects")
        .def("availableCount", &MemoryPoolBase::availableCount,
             "Get number of available objects")
        .def("memoryUsage", &MemoryPoolBase::memoryUsage,
             "Get memory usage in bytes")
        .def("reserve", &MemoryPoolBase::reserve, py::arg("count"),
             "Pre-allocate memory");

    // ==========================================
    // Factories and Utils
    // ==========================================

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

    m.def("createWorkingMemory", []() -> std::shared_ptr<WorkingMemory> {
        return std::make_shared<WorkingMemory>();
    }, "Create a working memory system");

    m.def("createEpisodicMemory", []() -> std::shared_ptr<NeuralEpisodicMemory> {
        return std::make_shared<NeuralEpisodicMemory>();
    }, "Create an episodic memory system");

    m.def("createAssociativeMemory", []() -> std::shared_ptr<NeuralAssociativeMemory> {
        return std::make_shared<NeuralAssociativeMemory>();
    }, "Create an associative memory system");

    m.def("createDopamine", []() -> std::shared_ptr<Dopamine> {
        return std::make_shared<Dopamine>();
    }, "Create a dopamine neuromodulator");

    m.def("createCuriosity", []() -> std::shared_ptr<Curiosity> {
        return std::make_shared<Curiosity>();
    }, "Create a curiosity neuromodulator");

    m.def("createNovelty", []() -> std::shared_ptr<Novelty> {
        return std::make_shared<Novelty>();
    }, "Create a novelty detector");

    m.def("createPredictionError", []() -> std::shared_ptr<PredictionError> {
        return std::make_shared<PredictionError>();
    }, "Create a prediction error system");

    m.def("createNeuralPlanner", []() -> std::shared_ptr<NeuralPlanner> {
        return std::make_shared<NeuralPlanner>();
    }, "Create a neural planner");

    m.def("createConceptFormation", []() -> std::shared_ptr<ConceptFormation> {
        return std::make_shared<ConceptFormation>();
    }, "Create concept formation system");

    m.def("createPredictionSystem", []() -> std::shared_ptr<PredictionSystem> {
        return std::make_shared<PredictionSystem>();
    }, "Create prediction system");

    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    }, "Create development system");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    m.attr("INVALID_SPIKE_HISTORY_SIZE") = MAX_SPIKE_HISTORY;
    m.attr("DEFAULT_MEMPOOL_BLOCK_SIZE") = 1024;
    m.attr("DEFAULT_MEMPOOL_INITIAL_BLOCKS") = 4;

    // Utility functions
    m.def("createLearningExperiment", [](std::shared_ptr<Brain> brain, uint64_t seed) {
        return std::make_shared<LearningExperiment>(brain, seed);
    }, py::arg("brain"), py::arg("seed"),
       "Create a learning experiment for testing");

    m.def("runBasicConnectivityTest", &runBasicConnectivityTest,
          py::arg("brain"), "Run basic neural connectivity test");

    m.def("runPlasticityExperiment", &runPlasticityExperiment,
          py::arg("brain"), "Run plasticity learning experiment");

    m.def("runStdpVerification", &runStdpVerification,
          py::arg("brain"), "Run STDP verification test");

    // ==========================================
    // Error types
    // ==========================================

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");

    // ==========================================
    // Module documentation
    // ==========================================

    m.doc() = R"pbdoc(
        Complete NLM Python API with all integrated systems:

        **Core Brain Components:**
        - Brain: Central neural simulation with integrated memory, prediction, cognition, neuromodulation
        - AgentBrain: Interface connecting brain to world environment
        - SimpleWorld: 2D simulation environment

        **Memory Systems:**
        - WorkingMemory: Active information maintenance
        - NeuralEpisodicMemory: Experience storage and retrieval
        - NeuralAssociativeMemory: Pattern association learning

        **Cognition Systems:**
        - NeuralPlanner: Action planning and evaluation
        - ConceptFormation: Pattern discovery and categorization

        **Prediction System:**
        - PredictionSystem: Temporal sequence prediction
        - NeuralPrediction: Neural mechanism for prediction

        **Neuromodulation:**
        - Dopamine: Reward signaling and reinforcement
        - Curiosity: Exploration motivation
        - Novelty: Novelty detection
        - PredictionError: Learning error signals

        **Development:**
        - DevelopmentSystem: Brain maturation and stage transitions

        **Plasticity:**
        - STDP: Spike-timing-dependent plasticity
        - Hebbian: Hebbian learning
        - StructuralPlasticity: Synapse formation and pruning

        **Performance:**
        - MemoryPool: Efficient memory allocation pools
        - OptimizedBrain: High-performance simulation with SIMD, multithreading, sparse connectivity

        **Configuration:**
        - Config: Centralized configuration management

        **Integration:**
        All systems are fully integrated into the Phase 6 brain loop, providing a complete artificial brain implementation.
    )pbdoc";

} // namespace nlm