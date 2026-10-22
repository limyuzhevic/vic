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
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/memory/AssociativeMemory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult: " << (result.success ? "SUCCESS" : "FAILURE") << ", " << result.reward << ", \"" << result.message << "\">";
            return ss.str();
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
        .def_readwrite("active", &WorldObject::active)
        .def("getTypeName", [](const WorldObject& obj) {
            switch(obj.type) {
                case WorldObjectType::Empty: return std::string("Empty");
                case WorldObjectType::Resource: return std::string("Resource");
                case WorldObjectType::Hazard: return std::string("Hazard");
                case WorldObjectType::Wall: return std::string("Wall");
                case WorldObjectType::Marker: return std::string("Marker");
                default: return std::string("Unknown");
            }
        });

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
        .def("getPosition", [](AgentBody& body) {
            return std::make_pair(body.x, body.y);
        })
        .def("setPosition", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        })
        .def("distanceTo", [](AgentBody& body, float x, float y) {
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx*dx + dy*dy);
        });

    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Base neuromodulator class)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Neuromodulator::getLevel)
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update)
        .def("getTypeName", [](const Neuromodulator& mod) {
            if (dynamic_cast<const Dopamine*>(&mod)) return std::string("Dopamine");
            if (dynamic_cast<const Curiosity*>(&mod)) return std::string("Curiosity");
            if (dynamic_cast<const Novelty*>(&mod)) return std::string("Novelty");
            if (dynamic_cast<const PredictionError*>(&mod)) return std::string("PredictionError");
            if (dynamic_cast<const Reward*>(&mod)) return std::string("Reward");
            return std::string("Neuromodulator");
        });

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Dopamine::getLevel)
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update);

    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel)
        .def("update", &Curiosity::update);

    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Novelty neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel)
        .def("update", &Novelty::update);

    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &PredictionError::getLevel)
        .def("getPredictionError", &PredictionError::getPredictionError)
        .def("update", &PredictionError::update);

    py::class_<Reward, Neuromodulator>(m, "Reward", R"pbdoc(Reward neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Reward::getLevel)
        .def("update", &Reward::update);

    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory system)pbdoc")
        .def(py::init<>())
        .def("storeToNeuron", &WorkingMemory::storeToNeuron)
        .def("getActiveTraces", &WorkingMemory::getActiveTraces)
        .def("update", &WorkingMemory::update)
        .def("clear", &WorkingMemory::clear)
        .def("getMemoryTrace", [](WorkingMemory& wm, NeuronId neuronId) {
            return wm.getMemoryTrace(neuronId);
        }, py::arg("neuron_id"));

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory system)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode)
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount)
        .def("getEpisodesForReplay", &EpisodicMemory::getEpisodesForReplay)
        .def("replayEpisode", &EpisodicMemory::replayEpisode)
        .def("consolidate", &EpisodicMemory::consolidate)
        .def("clear", &EpisodicMemory::clear);

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory system)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate)
        .def("recall", &AssociativeMemory::recall)
        .def("getAssocationStrength", [](AssociativeMemory& am, NeuronId pattern1, NeuronId pattern2) {
            return am.getAssociationStrength(pattern1, pattern2);
        }, py::arg("pattern1"), py::arg("pattern2"))
        .def("clear", &AssociativeMemory::clear);

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory)pbdoc")
        .def(py::init<>())
        .def("initialize", [](NeuralWorkingMemory& nwm, std::shared_ptr<Brain> brain) {
            nwm.initialize(brain.get());
        }, py::arg("brain"))
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron)
        .def("update", &NeuralWorkingMemory::update)
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons)
        .def("clear", &NeuralWorkingMemory::clear);

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory)pbdoc")
        .def(py::init<>())
        .def("initialize", [](NeuralEpisodicMemory& nem, std::shared_ptr<Brain> brain) {
            nem.initialize(brain.get());
        }, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay)
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode)
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount)
        .def("clear", &NeuralEpisodicMemory::clear);

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>())
        .def("update", [](PredictionSystem& ps, double timestep) {
            ps.update(timestep);
        }, py::arg("timestep"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("getPredictionConfidence", &PredictionSystem::getPredictionConfidence);

    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction)pbdoc")
        .def(py::init<>())
        .def("getPrediction", &NeuralPrediction::getPrediction)
        .def("getConfidence", &NeuralPrediction::getConfidence);

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system)pbdoc")
        .def(py::init<>())
        .def("update", &ConceptFormation::update)
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("getConceptSimilarity", &ConceptFormation::getConceptSimilarity);

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner)pbdoc")
        .def(py::init<>())
        .def("update", &NeuralPlanner::update)
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth)
        .def("getPlanLength", &NeuralPlanner::getPlanLength);

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system)pbdoc")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update)
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage)
        .def("getPlasticityRate", &DevelopmentSystem::getPlasticityRate);

    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Checkpoint manager)pbdoc")
        .def(py::init<>())
        .def("configure", &CheckpointManager::configure,
             py::arg("directory"), py::arg("maxCheckpoints"), py::arg("maxAge"), py::arg("autoSave") = false)
        .def("update", &CheckpointManager::update)
        .def("save", &CheckpointManager::save)
        .def("load", &CheckpointManager::load)
        .def("getCheckpointCount", &CheckpointManager::getCheckpointCount)
        .def("getLatestCheckpoint", &CheckpointManager::getLatestCheckpoint);

    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface)pbdoc")
        .def(py::init<>())
        .def("configure", &VisualizationInterface::configure,
             py::arg("width"), py::arg("height"), py::arg("title") = "NLM Visualization")
        .def("update", &VisualizationInterface::update)
        .def("render", &VisualizationInterface::render)
        .def("saveFrame", &VisualizationInterface::saveFrame)
                .def("configure", &VisualizationInterface::configure,
             py::arg("width"), py::arg("height"), py::arg("title") = "NLM Visualization")
        .def("update", &VisualizationInterface::update)
        .def("render", &VisualizationInterface::render)
        .def("saveFrame", &VisualizationInterface::saveFrame)
        .def("isRunning", &VisualizationInterface::isRunning)
        .def("stop", &VisualizationInterface::stop);

    // Add convenience factory functions for common setup patterns
    m.def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");

    m.def("createDefaultAgentBrain", []() -> std::shared_ptr<AgentBrain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        return std::make_shared<AgentBrain>(brain);
    }, "Create an agent brain with default configuration");

    m.def("createStandardWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8); // Standard 20x20 world with 8x8 vision
        return world;
    }, "Create a standard world with default settings");

    m.def("createAndInitializeAgent", 
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            return agent;
        }, py::arg("brain"), py::arg("world"), "Create and initialize an agent brain");

    m.def("enableAllFeatures", [](std::shared_ptr<AgentBrain> agent) {
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
    }, py::arg("agent"), "Enable all learning features in an agent");

    m.def("runStandardSimulation", 
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent,
           int numSteps, bool enableLearning = false) {
            if (enableLearning) {
                agent->enableRewardModulation(true);
                agent->enableCuriosity(true);
            }
            
            for (int step = 0; step < numSteps; ++step) {
                world->update(0.1); // 0.1 second timesteps
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                brain->step(step);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
        }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"), 
           py::arg("enableLearning") = false, "Run a standard simulation loop");

    m.def("getSystemInfo", []() {
        std::stringstream info;
        info << "NLM Python Bindings - Version 0.1.0\n";
        info << "Build: " << "CMAKE_CXX_STANDARD 20\n";
        info << "Features: LIF neurons, STDP, Hebbian learning, Structural plasticity\n";
        info << "Memory systems: Working, Episodic, Associative\n";
        info << "Neuromodulation: Dopamine, Curiosity, Novelty, PredictionError, Reward\n";
        info << "Cognition: NeuralPlanner, ConceptFormation\n";
        return info.str();
    }, "Get system information and version details");

    m.def("createFromCheckpoint", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->load(filepath);
        return brain;
    }, py::arg("filepath"), "Create a brain from a checkpoint file");

    m.def("validateConfiguration", [](std::shared_ptr<Config> config) {
        bool isValid = true;
        std::vector<std::string> errors;
        
        // Basic validation
        if (!config->has("brain.neuron_count")) {
            errors.push_back("Missing brain.neuron_count");
            isValid = false;
        }
        
        if (!config->has("brain.connection_probability")) {
            errors.push_back("Missing brain.connection_probability");
            isValid = false;
        }
        
        if (config->getOr<size_t>("brain.neuron_count", 0) == 0) {
            errors.push_back("brain.neuron_count must be > 0");
            isValid = false;
        }
        
        if (config->getOr<float>("brain.connection_probability", 1.0f) > 1.0f) {
            errors.push_back("brain.connection_probability must be <= 1.0");
            isValid = false;
        }
        
        return std::make_pair(isValid, errors);
    }, py::arg("config"), "Validate a configuration and return errors if any");

    m.def("createLearningPreset", [](const std::string& presetName) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        if (presetName == "fast_learning") {
            config->set("brain.neuron_count", 500);
            config->set("brain.connection_probability", 0.15f);
            config->set("plasticity.stdp.learning_rate", 0.02f);
            config->set("plasticity.hebbian.learning_rate", 0.01f);
            config->set("neuromod.dopamine.scale", 1.5f);
            config->set("neuromod.curiosity.enable", true);
            config->set("development.enable", true);
        }
        else if (presetName == "slow_stable") {
            config->set("brain.neuron_count", 2000);
            config->set("brain.connection_probability", 0.05f);
            config->set("plasticity.stdp.learning_rate", 0.001f);
            config->set("plasticity.hebbian.learning_rate", 0.0005f);
            config->set("neuromod.dopamine.scale", 0.5f);
            config->set("neuromod.curiosity.enable", false);
            config->set("development.enable", true);
        }
        else if (presetName == "curiosity_driven") {
            config->set("brain.neuron_count", 1000);
            config->set("brain.connection_probability", 0.1f);
            config->set("plasticity.stdp.learning_rate", 0.01f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("development.enable", true);
        }
        else {
            throw std::invalid_argument("Unknown preset: " + presetName);
        }
        
        return config;
    }, py::arg("presetName"), "Create a configuration preset");

    m.def("exportBrainStats", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        auto config = brain->getConfig();
        config->saveToFile(filepath);
        
        // Also save brain state
        brain->save(filepath + ".checkpoint");
        
        return true;
    }, py::arg("brain"), py::arg("filepath"), "Export brain statistics and checkpoint");

    m.def("getRandomSeed", []() {
        return static_cast<unsigned int>(std::random_device{}());
    }, "Generate a random seed for reproducible simulations");

    m.def("validateWorldSetup", [](std::shared_ptr<SimpleWorld> world) {
        bool isValid = true;
        std::vector<std::string> errors;
        
        if (world->getWidth() == 0 || world->getHeight() == 0) {
            errors.push_back("World dimensions must be > 0");
            isValid = false;
        }
        
        if (world->getVisionWidth() > world->getWidth()) {
            errors.push_back("Vision width cannot exceed world width");
            isValid = false;
        }
        
        if (world->getVisionHeight() > world->getHeight()) {
            errors.push_back("Vision height cannot exceed world height");
            isValid = false;
        }
        
        return std::make_pair(isValid, errors);
    }, py::arg("world"), "Validate world configuration");
}

} // namespace nlm
