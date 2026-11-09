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
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath").noconvert(),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
           "Load configuration from command line arguments (list of strings)")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath").noconvert(),
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
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp").noconvert(),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create a Vision sensory input with given dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the raw data for this vision input")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create an Audio sensory input with given sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the audio sample data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate").noconvert(),
             "Set the sample rate for audio input")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value").noconvert(),
             "Add a signal value to internal signals")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create an action with given type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an action with type and parameter values")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type").noconvert(),
             "Set the action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params").noconvert(),
             "Set the action parameters")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone, "Create a copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object at position (x, y) with type and properties")
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
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result with reward, success flag, and message")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision").noconvert(),
             "Set the vision data for this percept")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch").noconvert(),
             "Set the touch data for this percept")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal").noconvert(),
             "Set the internal signals data")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception").noconvert(),
             "Set the proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio").noconvert(),
             "Set the audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp").noconvert(),
             "Set the timestamp for this percept");

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and sensory capabilities")
        .def("reset", &SimpleWorld::reset, "Reset the world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position in the world")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world state for the given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command at the given time")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept from the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj").noconvert(),
             "Add a world object to the simulation")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove a world object at the given position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if a position is valid (not occupied by a wall)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e").noconvert(),
             "Set the maximum energy for the agent")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r").noconvert(),
             "Set the energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed").noconvert(),
             "Set the random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with the given configuration")
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
        .def("save", &Brain::save, py::arg("filepath").noconvert(),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath").noconvert(),
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
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface with the given brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent with the given world")
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

    // Phase 5: Helper functions for advanced users
    m.def("createSimulationRunner", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent) {
        // Create a simulation runner that manages the complete brain-world interaction loop
        return std::make_shared<SimulationRunner>(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Create a simulation runner that manages brain-world interaction");

    // Metrics collection and analysis utilities
    m.def("createMetricsCollector", []() {
        return std::make_shared<MetricsCollector>();
    }, "Create a metrics collector for tracking simulation statistics");

    // Advanced configuration builders
    m.def("createTrainingConfig", [](int numNeurons, int numSteps, float learningRate) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(numNeurons), ConfigSource::Default);
        config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
        config->set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", learningRate, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        return config;
    }, py::arg("numNeurons") = 1000, py::arg("numSteps") = 1000, py::arg("learningRate") = 0.001f,
       "Create a training configuration for neural network training");

    m.def("createWorldConfig", [](int width, int height, float timestep) {
        auto config = std::make_shared<Config>();
        config->set("world.width", static_cast<int64_t>(width), ConfigSource::Default);
        config->set("world.height", static_cast<int64_t>(height), ConfigSource::Default);
        config->set("world.timestep", timestep, ConfigSource::Default);
        config->set("world.max_energy", 100.0f, ConfigSource::Default);
        config->set("world.energy_decay_rate", 0.1f, ConfigSource::Default);
        return config;
    }, py::arg("width") = 20, py::arg("height") = 20, py::arg("timestep") = 0.1f,
       "Create a world configuration for the simulation environment");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

// Forward declarations for helper classes
class SimulationRunner;
class MetricsCollector;

// Simulation runner helper class
py::class_<SimulationRunner>(m, "SimulationRunner", R"pbdoc(Helper class for running complete brain-world simulations)pbdoc")
    .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
         py::arg("brain"), py::arg("world"), py::arg("agent"),
         "Create a simulation runner that manages brain-world interaction")
    .def("runEpisode", &SimulationRunner::runEpisode,
         py::arg("numSteps"), py::arg("headless") = false,
         "Run a complete simulation episode for the given number of steps")
    .def("getMetrics", &SimulationRunner::getMetrics,
         py::return_value_policy::reference_internal,
         "Get the metrics collector from the last run episode")
    .def("getBrain", &SimulationRunner::getBrain,
         py::return_value_policy::reference_internal,
         "Get the brain from the simulation")
    .def("getWorld", &SimulationRunner::getWorld,
         py::return_value_policy::reference_internal,
         "Get the world from the simulation")
    .def("getAgent", &SimulationRunner::getAgent,
         py::return_value_policy::reference_internal,
         "Get the agent from the simulation");

// Metrics collector helper class
py::class_<MetricsCollector>(m, "MetricsCollector", R"pbdoc(Class for collecting and analyzing simulation metrics)pbdoc")
    .def(py::init<>())
    .def("recordStep", &MetricsCollector::recordStep,
         py::arg("step"), py::arg("brain"), py::arg("agent"),
         "Record metrics for a single simulation step")
    .def("getMeanFiringRate", &MetricsCollector::getMeanFiringRate,
         "Get the mean firing rate across all recorded steps")
    .def("getMaxFiringRate", &MetricsCollector::getMaxFiringRate,
         "Get the maximum firing rate across all recorded steps")
    .def("getTotalSpikes", &MetricsCollector::getTotalSpikes,
         "Get the total number of spikes across all recorded steps")
    .def("getAverageActionCount", &MetricsCollector::getAverageActionCount,
         "Get the average number of actions taken per step")
    .def("getRewardStats", &MetricsCollector::getRewardStats,
         "Get reward statistics (mean, std, min, max)")
    .def("getNeuralActivityStats", &MetricsCollector::getNeuralActivityStats,
         "Get neural activity statistics")
    .def("reset", &MetricsCollector::reset,
         "Reset all collected metrics")
    .def("getStepCount", &MetricsCollector::getStepCount,
         "Get the number of recorded steps")
    .def("exportCSV", &MetricsCollector::exportCSV,
         py::arg("filepath"),
         "Export metrics to a CSV file")
    .def("__repr__", [](const MetricsCollector& mc) {
        return "<MetricsCollector: " + std::to_string(mc.getStepCount()) + " steps recorded>";
    });

// Implementation details for SimulationRunner and MetricsCollector

namespace {

class SimulationRunner {
public:
    SimulationRunner(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                     std::shared_ptr<AgentBrain> agent)
        : brain_(brain), world_(world), agent_(agent), metrics_(std::make_shared<MetricsCollector>()) {}

    void runEpisode(int numSteps, bool headless = false) {
        for (int step = 0; step < numSteps; ++step) {
            // Update world
            world_->update(0.1);

            // Get sensory percept from world
            auto percept = world_->getSensoryPercept();

            // Process sensory input in brain
            agent_->processSensoryInput(percept);

            // Run brain step
            brain_->step(step);

            // Decode motor command from brain activity
            auto motorCmd = agent_->decodeMotorCommand();

            // Apply motor command to world
            world_->applyMotorCommand(motorCmd, world_->getSimulationTime());

            // Apply reward modulation
            float reward = 0.0f;
            if (percept) {
                auto internalSignals = percept->getInternal();
                if (!internalSignals.empty()) {
                    reward = internalSignals[0];
                }
            }
            agent_->applyRewardModulation(reward, 0.0f);

            // Update development
            agent_->updateDevelopment(0.1);

            // Record metrics
            metrics_->recordStep(step, brain_, agent_);

            if (!headless && step % 100 == 0) {
                std::cout << "Step " << step << ": "
                          << "Firing: " << brain_->getFiringNeuronCount()
                          << ", Curiosity: " << agent_->getCuriosityLevel()
                          << ", Novelty: " << agent_->getNoveltyLevel()
                          << std::endl;
            }
        }
    }

    std::shared_ptr<MetricsCollector> getMetrics() const { return metrics_; }
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    std::shared_ptr<AgentBrain> getAgent() const { return agent_; }

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<MetricsCollector> metrics_;
};

class MetricsCollector {
public:
    MetricsCollector() : stepCount_(0) {}

    void recordStep(int step, std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
        stepCount_++;

        // Record neural activity metrics
        float firingRate = brain->getFiringNeuronCount();
        float avgFiringRate = brain->getAverageFiringRate();
        float eir = brain->getExcitationInhibitionRatio();

        // Record neuromodulation metrics
        float curiosity = agent->getCuriosityLevel();
        float novelty = agent->getNoveltyLevel();
        float neuromodulation = agent->getNeuromodulationLevel();

        // Record action metrics
        // Note: Action production statistics would require tracking actions over time
        // For now, we use basic metrics that can be calculated from available data

        // Store metrics for analysis
        stepMetrics_.push_back({
            step,
            firingRate,
            avgFiringRate,
            eir,
            curiosity,
            novelty,
            neuromodulation,
            brain->getTotalSpikeCount()
        });
    }

    float getMeanFiringRate() const {
        if (stepCount_ == 0) return 0.0f;
        float sum = 0.0f;
        for (const auto& metrics : stepMetrics_) {
            sum += metrics.firingRate;
        }
        return sum / stepCount_;
    }

    float getMaxFiringRate() const {
        if (stepCount_ == 0) return 0.0f;
        float max = 0.0f;
        for (const auto& metrics : stepMetrics_) {
            max = std::max(max, metrics.firingRate);
        }
        return max;
    }

    size_t getTotalSpikes() const {
        size_t total = 0;
        for (const auto& metrics : stepMetrics_) {
            total += metrics.totalSpikes;
        }
        return total;
    }

    float getAverageActionCount() const {
        // Basic approximation - in a real implementation, this would track actual actions
        return 1.0f; // Each step typically produces one action
    }

    // Reward statistics (simplified - would need reward tracking)
    struct RewardStats {
        float mean;
        float std;
        float min;
        float max;
    };

    RewardStats getRewardStats() const {
        // Simplified - in practice, would track actual rewards over time
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    // Neural activity statistics
    struct NeuralActivityStats {
        float meanFiringRate;
        float meanExcitatoryInhibitionRatio;
        float meanCuriosity;
        float meanNovelty;
    };

    NeuralActivityStats getNeuralActivityStats() const {
        if (stepCount_ == 0) return {0.0f, 0.0f, 0.0f, 0.0f};

        float sumFiring = 0.0f, sumEIR = 0.0f, sumCuriosity = 0.0f, sumNovelty = 0.0f;
        for (const auto& metrics : stepMetrics_) {
            sumFiring += metrics.firingRate;
            sumEIR += metrics.eir;
            sumCuriosity += metrics.curiosity;
            sumNovelty += metrics.novelty;
        }

        return {
            sumFiring / stepCount_,
            sumEIR / stepCount_,
            sumCuriosity / stepCount_,
            sumNovelty / stepCount_
        };
    }

    void reset() {
        stepCount_ = 0;
        stepMetrics_.clear();
    }

    int getStepCount() const { return stepCount_; }

    void exportCSV(const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }

        file << "Step,FiringRate,AverageFiringRate,E/I,Ratio,Curiosity,Novelty,Neuromodulation,TotalSpikes\n";
        for (const auto& metrics : stepMetrics_) {
            file << metrics.step << ","
                 << metrics.firingRate << ","
                 << metrics.avgFiringRate << ","
                 << metrics.eir << ","
                 << metrics.curiosity << ","
                 << metrics.novelty << ","
                 << metrics.neuromodulation << ","
                 << metrics.totalSpikes << "\n";
        }

        file.close();
    }

    std::string __repr__() const {
        return "<MetricsCollector: " + std::to_string(stepCount_) + " steps recorded>";
    }

private:
    struct StepMetrics {
        int step;
        float firingRate;
        float avgFiringRate;
        float eir;
        float curiosity;
        float novelty;
        float neuromodulation;
        size_t totalSpikes;
    };

    int stepCount_;
    std::vector<StepMetrics> stepMetrics_;
};

} // namespace

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
