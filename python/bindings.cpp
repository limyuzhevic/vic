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

    // AgentBrain: Connects NLM brain to the world
    // Handles sensory transduction and motor decoding
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create AgentBrain with a brain instance")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size. Returns total sensory dimensions (vision + touch + internal + proprioception)")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size. Returns number of motor commands (typically 6: forward, backward, turn left/right, interact, wait)")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain. percept must contain vision, touch, internal, and proprioception data")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command based on neural activity patterns")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation. Updates prediction error and dopamine levels for learning")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system. Advances developmental age and adjusts plasticity based on time")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage (Initial, CriticalPeriod, Maturation, Adult, or Aging)")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level (dopamine, -1.0 to 1.0)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (0.0 to 1.0, drives exploration behavior)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level (0.0 to 1.0, based on sensory input changes)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error (reward - expected reward, drives learning)")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode. Clears neuromodulation state and sensory history")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying Brain instance")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation system")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity (neuron/synapse changes)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable developmental processes")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        
        // ===== BEGINNER-FRIENDLY HELPER FUNCTIONS =====
        .def("createSimpleAction", [](const AgentBrain& self, const std::string& actionType, const std::vector<float>& params) {
            Action act;
            if (actionType == "move_forward") act = Action(ActionType::MoveForward, params);
            else if (actionType == "move_backward") act = Action(ActionType::MoveBackward, params);
            else if (actionType == "turn_left") act = Action(ActionType::TurnLeft, params);
            else if (actionType == "turn_right") act = Action(ActionType::TurnRight, params);
            else if (actionType == "interact") act = Action(ActionType::Interact, params);
            else if (actionType == "wait") act = Action(ActionType::Wait, params);
            else act = Action(ActionType::Custom, params);
            return act;
        }, py::arg("actionType"), py::arg("params") = std::vector<float>(),
             R"pbdoc(Create a common action type for convenience. Common types: "move_forward", "move_backward", "turn_left", "turn_right", "interact", "wait"")pbdoc")
        .def("createVisionInput", [](size_t width, size_t height, size_t channels, const std::vector<float>& data) {
            Vision v(width, height, channels);
            v.setData(data);
            return v;
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, py::arg("data") = std::vector<float>(),
             R"pbdoc(Create vision sensory input. Common defaults: width=16, height=16, channels=3 for grayscale")pbdoc")
        .def("createAudioInput", [](size_t sampleRate, size_t numSamples, const std::vector<float>& data) {
            Audio a(sampleRate, numSamples);
            a.setData(data);
            return a;
        }, py::arg("sampleRate"), py::arg("numSamples"), py::arg("data") = std::vector<float>(),
             R"pbdoc(Create audio sensory input")pbdoc")
        .def("simulateStep", [](AgentBrain& self, double timestep, const SensoryPercept& percept) {
            self.processSensoryInput(percept);
            self.updateDevelopment(timestep);
            MotorCommand action = self.decodeMotorCommand();
            return action;
        }, py::arg("timestep"), py::arg("percept"),
             "Complete simulation step: process sensory input, update development, and decode action. Convenience method for simple agent control")
        
        // ===== ADVANCED POWER USER FEATURES =====
        .def("getMotorGroups", [](AgentBrain& self) {
            std::map<std::string, std::vector<Neuron*>> groups;
            groups["forward"] = self.motorForward_;
            groups["backward"] = self.motorBackward_;
            groups["turn_left"] = self.motorTurnLeft_;
            groups["turn_right"] = self.motorTurnRight_;
            groups["interact"] = self.motorInteract_;
            groups["wait"] = self.motorWait_;
            return groups;
        }, "Get motor neuron groups as a dictionary. Advanced access to internal neural architecture")
        .def("getSensoryGroups", [](AgentBrain& self) {
            std::map<std::string, std::vector<Neuron*>> groups;
            groups["vision"] = self.sensoryVision_;
            groups["touch"] = self.sensoryTouch_;
            groups["internal"] = self.sensoryInternal_;
            groups["proprioception"] = self.sensoryProprioception_;
            return groups;
        }, "Get sensory neuron groups as a dictionary. Advanced access to internal neural architecture")
        .def("getNeuromodulationState", [](AgentBrain& self) {
            std::map<std::string, float> state;
            state["dopamine"] = self.dopamineLevel_;
            state["novelty"] = self.noveltyLevel_;
            state["curiosity"] = self.curiosityLevel_;
            state["prediction_error"] = self.predictionError_;
            state["expected_reward"] = self.expectedReward_;
            state["plasticity_modifier"] = self.plasticityModifier_;
            state["developmental_age"] = static_cast<float>(self.developmentalAge_);
            return state;
        }, "Get complete neuromodulation state dictionary. Advanced debugging and analysis")
        .def("setNeuromodulationState", [](AgentBrain& self, const std::map<std::string, float>& state) {
            if (state.count("dopamine")) self.dopamineLevel_ = state.at("dopamine");
            if (state.count("novelty")) self.noveltyLevel_ = state.at("novelty");
            if (state.count("curiosity")) self.curiosityLevel_ = state.at("curiosity");
            if (state.count("prediction_error")) self.predictionError_ = state.at("prediction_error");
            if (state.count("expected_reward")) self.expectedReward_ = state.at("expected_reward");
            if (state.count("plasticity_modifier")) self.plasticityModifier_ = state.at("plasticity_modifier");
            if (state.count("developmental_age")) self.developmentalAge_ = state.at("developmental_age");
        }, py::arg("state"), "Set neuromodulation state from dictionary. Advanced control for power users")
        .def("batchProcessSensory", [](AgentBrain& self, const std::vector<SensoryPercept>& percepts) {
            for (const auto& percept : percepts) {
                self.processSensoryInput(percept);
            }
        }, py::arg("percepts"), "Process multiple sensory inputs in batch. More efficient for processing sequences")
        .def("getStatistics", [](const AgentBrain& self) {
            std::map<std::string, double> stats;
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get agent statistics as dictionary. Useful for monitoring and logging")
        .def("printNeuralArchitecture", [](const AgentBrain& self) {
            std::cout << "=== AgentBrain Neural Architecture ===\n";
            std::cout << "Motor groups: " << self.motorForward_.size() << " forward, " 
                      << self.motorBackward_.size() << " backward, "
                      << self.motorTurnLeft_.size() << " turn_left, "
                      << self.motorTurnRight_.size() << " turn_right, "
                      << self.motorInteract_.size() << " interact, "
                      << self.motorWait_.size() << " wait\n";
            std::cout << "Sensory groups: " << self.sensoryVision_.size() << " vision, "
                      << self.sensoryTouch_.size() << " touch, "
                      << self.sensoryInternal_.size() << " internal, "
                      << self.sensoryProprioception_.size() << " proprioception\n";
            std::cout << "Neuromodulation: dopamine=" << self.dopamineLevel_ 
                      << ", curiosity=" << self.curiosityLevel_ 
                      << ", novelty=" << self.noveltyLevel_ << "\n";
        }, "Print neural architecture to stdout. Debugging and analysis tool")
        .def("getRandomBehavior", [](AgentBrain& self) {
            float r = self.brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < 0.1f) return MotorCommand::MoveForward;
            else if (r < 0.2f) return MotorCommand::MoveBackward;
            else if (r < 0.3f) return MotorCommand::TurnLeft;
            else if (r < 0.4f) return MotorCommand::TurnRight;
            else if (r < 0.5f) return MotorCommand::Interact;
            else if (r < 0.6f) return MotorCommand::LookLeft;
            else if (r < 0.7f) return MotorCommand::LookRight;
            else if (r < 0.8f) return MotorCommand::LookUp;
            else if (r < 0.9f) return MotorCommand::LookDown;
            else return MotorCommand::Wait;
        }, "Generate random motor command. Useful for testing and exploration")
        .def("setExplorationMode", [](AgentBrain& self, bool enable, float curiosityLevel) {
            self.enableCuriosity(enable);
            if (enable) self.curiosityLevel_ = curiosityLevel;
        }, py::arg("enable"), py::arg("curiosityLevel") = 0.5f,
             "Set exploration mode with specific curiosity level. Convenience for behavior control");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults for neural simulation")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world for agent simulation")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")

    m.def("runSimpleEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world,
                               const std::vector<SensoryPercept>& initialPercepts,
                               size_t steps, double timestep) {
        agent->initialize(*world);
        std::vector<MotorCommand> actions;
        for (size_t i = 0; i < steps; ++i) {
            agent->processSensoryInput(initialPercepts[i % initialPercepts.size()]);
            MotorCommand action = agent->decodeMotorCommand();
            actions.push_back(action);
            world->applyMotorCommand(action, i * timestep);
            agent->updateDevelopment(timestep);
        }
        return actions;
    }, py::arg("agent"), py::arg("world"), py::arg("initialPercepts"), py::arg("steps"), py::arg("timestep"),
        "Run a complete simulation episode. Convenience function for simple agent training/ testing")

    m.def("getAllConstants", []() {
        std::map<std::string, float> constants;
        constants["DEFAULT_DOPAMINE_CLAMP_MIN"] = -1.0f;
        constants["DEFAULT_DOPAMINE_CLAMP_MAX"] = 1.0f;
        constants["DEFAULT_PLASTICITY_MODIFIER"] = 1.0f;
        constants["DEFAULT_NOVELTY_DECAY"] = 0.99f;
        constants["DEFAULT_SENSORY_NOVELTY_DECAY"] = 0.99f;
        constants["DEFAULT_EXPLORATION_CHANCE"] = 0.3f;
        constants["DEFAULT_DEVELOPMENT_STAGE_AGE_INITIAL"] = 60.0;
        constants["DEFAULT_DEVELOPMENT_STAGE_AGE_CRITICAL"] = 300.0;
        constants["DEFAULT_DEVELOPMENT_STAGE_AGE_MATURE"] = 900.0;
        return constants;
    }, "Get all default constants used in the simulation. Useful for configuration and debugging")

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Error types for Python exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
}

} // namespace nlm
