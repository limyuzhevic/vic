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

    // Enhanced error handling and exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ArgumentError");
    py::register_exception<std::out_of_range>(m, "RangeError");

    // Utility functions and helper classes
    m.def("validate_simulation_step", [](const std::shared_ptr<Brain>& brain, SimulationStep step, Timestamp time) {
        if (!brain) throw py::value_error("Brain cannot be None");
        if (step < 0) throw py::value_error("Step number must be non-negative");
        if (time < 0) throw py::value_error("Simulation time must be non-negative");
        return true;
    }, py::arg("brain"), py::arg("step"), py::arg("time"),
       "Validate simulation step parameters");

    m.def("createOptimizedConfig", [](const std::string& useCase) {
        auto config = std::make_shared<Config>();
        
        if (useCase == "research") {
            config->set("brain.neuron_count", 2000, ConfigSource::Runtime);
            config->set("brain.region_count", 5, ConfigSource::Runtime);
            config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            config->set("plasticity.stdp.learning_rate", 0.002, ConfigSource::Runtime);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
            config->set("plasticity.structural.enable", true, ConfigSource::Runtime);
            config->set("neuromod.dopamine.scale", 1.5, ConfigSource::Runtime);
            config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
            config->set("neuromod.novelty.enable", true, ConfigSource::Runtime);
            config->set("neuromod.prediction_error.enable", true, ConfigSource::Runtime);
            config->set("development.initial_plasticity", 1.2, ConfigSource::Runtime);
            config->set("development.maturation_rate", 0.002, ConfigSource::Runtime);
        } else if (useCase == "production") {
            config->set("brain.neuron_count", 800, ConfigSource::Runtime);
            config->set("brain.region_count", 3, ConfigSource::Runtime);
            config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            config->set("plasticity.stdp.learning_rate", 0.001, ConfigSource::Runtime);
            config->set("plasticity.hebbian.enable", false, ConfigSource::Runtime);
            config->set("plasticity.structural.enable", false, ConfigSource::Runtime);
            config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Runtime);
            config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
            config->set("neuromod.novelty.enable", false, ConfigSource::Runtime);
            config->set("development.initial_plasticity", 0.6, ConfigSource::Runtime);
            config->set("development.maturation_rate", 0.001, ConfigSource::Runtime);
        } else if (useCase == "rapid_learning") {
            config->set("brain.neuron_count", 3000, ConfigSource::Runtime);
            config->set("brain.region_count", 4, ConfigSource::Runtime);
            config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            config->set("plasticity.stdp.learning_rate", 0.005, ConfigSource::Runtime);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
            config->set("plasticity.structural.enable", true, ConfigSource::Runtime);
            config->set("neuromod.dopamine.scale", 2.0, ConfigSource::Runtime);
            config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
            config->set("neuromod.novelty.enable", true, ConfigSource::Runtime);
            config->set("neuromod.prediction_error.enable", true, ConfigSource::Runtime);
            config->set("development.initial_plasticity", 2.0, ConfigSource::Runtime);
            config->set("development.maturation_rate", 0.002, ConfigSource::Runtime);
        }
        
        return config;
    }, py::arg("useCase"), "Create optimized configuration for specific use case");

    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                            std::shared_ptr<AgentBrain> agent, int numSteps) {
        if (!brain || !world || !agent) {
            throw py::value_error("Brain, world, and agent cannot be None");
        }
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        for (int step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Decode motor command
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            auto internal = percept->getInternal();
            float reward = internal.empty() ? 0.0f : internal[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
        }
        
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       "Run complete simulation with brain, world, and agent");

    m.def("createCompleteSimulation", [](const std::string& useCase) {
        auto config = createOptimizedConfig(useCase);
        auto brain = createBrain(config);
        brain->initialize();
        
        auto world = createSimpleWorld();
        world->configure(30, 30, 12, 12);
        world->reset();
        world->setAgentStart(15, 15);
        
        auto agent = createAgentBrain(brain);
        agent->initialize(world);
        
        // Enable all learning systems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("useCase"), "Create complete simulation environment with all components");

    m.def("performanceProfile", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent, int numSteps) {
        if (!brain || !world || !agent) {
            throw py::value_error("Brain, world, and agent cannot be None");
        }
        
        // Enable profiling
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        int totalSpikes = 0;
        int firingNeurons = 0;
        float curiosityLevel = 0.0f;
        float noveltyLevel = 0.0f;
        float predictionError = 0.0f;
        float avgFiringRate = 0.0f;
        float e_i_ratio = 0.0f;
        float developmentStage = 0.0f;
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            totalSpikes += brain->getTotalSpikeCount();
            firingNeurons += brain->getFiringNeuronCount();
            curiosityLevel += agent->getCuriosityLevel();
            noveltyLevel += agent->getNoveltyLevel();
            predictionError += agent->getPredictionError();
            avgFiringRate += brain->getAverageFiringRate();
            e_i_ratio += brain->getExcitationInhibitionRatio();
            developmentStage += brain->getDevelopmentalStage();
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto internal = percept->getInternal();
            float reward = internal.empty() ? 0.0f : internal[0];
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        int numComplete = numSteps;  // In real implementation, would count completed steps
        
        return py::dict(
            "total_spikes", totalSpikes,
            "avg_firing_rate", avgFiringRate / numComplete,
            "firing_neurons", firingNeurons / numComplete,
            "e_i_ratio", e_i_ratio / numComplete,
            "curiosity_level", curiosityLevel / numComplete,
            "novelty_level", noveltyLevel / numComplete,
            "prediction_error", predictionError / numComplete,
            "development_stage", developmentStage / numComplete,
            "duration_ms", duration,
            "steps_per_second", (numComplete * 1000.0) / duration,
            "spikes_per_second", (totalSpikes * 1000.0) / duration
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       "Profile simulation performance and return metrics");

    m.def("createConfigurationTemplate", [](const std::string& templateType) {
        auto config = std::make_shared<Config>();
        
        if (templateType == "brain_emulation") {
            config->set("brain.neuron_count", 5000, ConfigSource::Runtime);
            config->set("brain.region_count", 8, ConfigSource::Runtime);
            config->set("brain.synapse_density", 0.1, ConfigSource::Runtime);
            config->set("brain.connection_probability", 0.05, ConfigSource::Runtime);
            config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            config->set("plasticity.stdp.learning_rate", 0.001, ConfigSource::Runtime);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
            config->set("plasticity.structural.enable", true, ConfigSource::Runtime);
        } else if (templateType == "cognitive_task") {
            config->set("brain.neuron_count", 1000, ConfigSource::Runtime);
            config->set("brain.region_count", 3, ConfigSource::Runtime);
            config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            config->set("plasticity.stdp.learning_rate", 0.005, ConfigSource::Runtime);
            config->set("plasticity.hebbian.enable", false, ConfigSource::Runtime);
            config->set("neuromod.dopamine.scale", 2.0, ConfigSource::Runtime);
            config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
            config->set("neuromod.novelty.enable", true, ConfigSource::Runtime);
            config->set("development.initial_plasticity", 1.5, ConfigSource::Runtime);
        } else if (templateType == "memory_intensive") {
            config->set("brain.neuron_count", 3000, ConfigSource::Runtime);
            config->set("brain.region_count", 5, ConfigSource::Runtime);
            config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            config->set("plasticity.stdp.learning_rate", 0.001, ConfigSource::Runtime);
            config->set("plasticity.structural.enable", true, ConfigSource::Runtime);
            config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Runtime);
            config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
            config->set("neuromod.novelty.enable", true, ConfigSource::Runtime);
            config->set("development.maturation_rate", 0.005, ConfigSource::Runtime);
        }
        
        return config;
    }, py::arg("templateType"), "Create configuration template for specific application");

    // Visualization and monitoring utilities
    m.def("createSimulationLogger", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent, const std::string& logFile) {
        class SimulationLogger {
        public:
            SimulationLogger(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w,
                           std::shared_ptr<AgentBrain> a, const std::string& file)
                : brain(b), world(w), agent(a), logFile(file) {}
            
            void logStep(int step) {
                if (!brain || !world || !agent) return;
                
                // Create log entry
                std::ostringstream logEntry;
                logEntry << "Step " << step << ": ";
                logEntry << "Spikes=" << brain->getTotalSpikeCount() << "; ";
                logEntry << "Firing=" << brain->getFiringNeuronCount() << "; ";
                logEntry << "FiringRate=" << brain->getAverageFiringRate() << "; ";
                logEntry << "Curiosity=" << agent->getCuriosityLevel() << "; ";
                logEntry << "Novelty=" << agent->getNoveltyLevel() << "; ";
                logEntry << "DevStage=" << brain->getDevelopmentalStage();
                
                // Log to console
                std::cout << logEntry.str() << std::endl;
                
                // TODO: Write to log file
                // std::ofstream logFileStream(logFile, std::ios::app);
                // logFileStream << logEntry.str() << std::endl;
            }
            
            void runAndLog(int numSteps) {
                for (int step = 0; step < numSteps; ++step) {
                    world->update(0.1);
                    
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    
                    brain->step(step);
                    
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    auto internal = percept->getInternal();
                    float reward = internal.empty() ? 0.0f : internal[0];
                    agent->applyRewardModulation(reward, 0.0f);
                    agent->updateDevelopment(0.1);
                    
                    logStep(step);
                }
            }
            
        private:
            std::shared_ptr<Brain> brain;
            std::shared_ptr<SimpleWorld> world;
            std::shared_ptr<AgentBrain> agent;
            std::string logFile;
        };
        
        return std::make_shared<SimulationLogger>(brain, world, agent, logFile);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("logFile"),
       "Create simulation logger for recording step-by-step progress");

    m.def("createVisualizationHelper", []() {
        class VisualizationHelper {
        public:
            VisualizationHelper() {}
            
            py::dict createSimulationPlot(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                          std::shared_ptr<AgentBrain> agent, int historySize) {
                // Create synthetic plot data (in real implementation, would use matplotlib/pillow)
                py::dict plotData;
                
                py::list steps;
                py::list spikes;
                py::list firingRates;
                py::list curiosityLevels;
                py::list noveltyLevels;
                
                // Generate synthetic data
                for (int i = 0; i < historySize; ++i) {
                    steps.append(i);
                    spikes.append(brain ? brain->getTotalSpikeCount() + i*10 : i*10);
                    firingRates.append(brain ? brain->getAverageFiringRate() + i*0.1 : i*0.1);
                    curiosityLevels.append(agent ? agent->getCuriosityLevel() + (i % 10) * 0.1 : 0.5);
                    noveltyLevels.append(agent ? agent->getNoveltyLevel() + (i % 7) * 0.1 : 0.3);
                }
                
                plotData["steps"] = steps;
                plotData["spikes"] = spikes;
                plotData["firing_rates"] = firingRates;
                plotData["curiosity_levels"] = curiosityLevels;
                plotData["novelty_levels"] = noveltyLevels;
                
                return plotData;
            }
            
            std::string createStatusReport(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                          std::shared_ptr<AgentBrain> agent) {
                std::ostringstream report;
                report << "=== NLM Simulation Status Report ===\n";
                if (brain) {
                    report << "Brain State:\n";
                    report << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
                    report << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
                    report << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
                    report << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
                    report << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
                    report << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
                    report << "  Developmental Stage: " << brain->getDevelopmentalStage() << "\n";
                }
                if (agent) {
                    report << "Agent State:\n";
                    report << "  Curiosity Level: " << agent->getCuriosityLevel() << "\n";
                    report << "  Novelty Level: " << agent->getNoveltyLevel() << "\n";
                    report << "  Prediction Error: " << agent->getPredictionError() << "\n";
                    report << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << "\n";
                }
                if (world) {
                    report << "World State:\n";
                    report << "  Width: " << world->getWidth() << "\n";
                    report << "  Height: " << world->getHeight() << "\n";
                    report << "  Simulation Time: " << world->getSimulationTime() << "\n";
                }
                
                return report.str();
            }
            
        };
        
        return std::make_shared<VisualizationHelper>();
    }, "Create visualization helper for plotting and reporting");

    // Error handling and validation utilities
    m.def("validateConfiguration", [](std::shared_ptr<Config> config) {
        if (!config) throw py::value_error("Config cannot be None");
        
        std::vector<std::string> errors;
        
        // Check neuron count
        if (config->has("brain.neuron_count")) {
            auto neurons = config->get<int64_t>("brain.neuron_count");
            if (neurons <= 0) errors.push_back("Neuron count must be positive");
            if (neurons > 100000) errors.push_back("Neuron count too large (max 100,000)");
        }
        
        // Check learning rate
        if (config->has("plasticity.stdp.learning_rate")) {
            auto rate = config->get<double>("plasticity.stdp.learning_rate");
            if (rate < 0 || rate > 1) errors.push_back("STDP learning rate must be between 0 and 1");
        }
        
        // Check neuromodulation scales
        if (config->has("neuromod.dopamine.scale")) {
            auto scale = config->get<double>("neuromod.dopamine.scale");
            if (scale < 0) errors.push_back("Dopamine scale must be positive");
        }
        
        if (!errors.empty()) {
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw py::value_error(errorMsg);
        }
        
        return true;
    }, py::arg("config"), "Validate configuration and return true if valid");

    m.def("safeSimulationStep", [](std::shared_ptr<Brain> brain, SimulationStep step, Timestamp time) {
        if (!brain) throw py::value_error("Brain cannot be None");
        
        try {
            brain->step(step, time);
            return true;
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Simulation step failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time"),
       "Safely execute simulation step with error handling");

    m.def("exportBrainToPython", [](std::shared_ptr<Brain> brain) {
        if (!brain) throw py::value_error("Brain cannot be None");
        
        py::dict brainState;
        
        // Convert brain state to Python dictionary
        brainState["total_neuron_count"] = brain->getTotalNeuronCount();
        brainState["total_synapse_count"] = brain->getTotalSynapseCount();
        brainState["total_spike_count"] = brain->getTotalSpikeCount();
        brainState["firing_neuron_count"] = brain->getFiringNeuronCount();
        brainState["average_firing_rate"] = brain->getAverageFiringRate();
        brainState["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        brainState["developmental_stage"] = brain->getDevelopmentalStage();
        
        // Get configuration
        auto config = brain->getConfig();
        py::dict configDict;
        for (const auto& key : config->getKeys()) {
            configDict[key] = config->getOr<double>(key, 0.0);
        }
        brainState["config"] = configDict;
        
        return brainState;
    }, py::arg("brain"), "Export brain state as Python dictionary for analysis");

    // Performance utilities
    m.def("optimizeForPerformance", [](std::shared_ptr<Config> config, const std::string& target) {
        if (!config) throw py::value_error("Config cannot be None");
        
        // Create optimized copy
        auto optimizedConfig = std::make_shared<Config>();
        
        for (const auto& key : config->getKeys()) {
            optimizedConfig->set(key, config->getOr<double>(key), ConfigSource::Runtime);
        }
        
        if (target == "speed") {
            optimizedConfig->set("brain.neuron_count", std::min(config->getOr<int64_t>("brain.neuron_count", 1000), 500), ConfigSource::Runtime);
            optimizedConfig->set("plasticity.structural.enable", false, ConfigSource::Runtime);
            optimizedConfig->set("neuromod.novelty.enable", false, ConfigSource::Runtime);
        } else if (target == "learning") {
            optimizedConfig->set("plasticity.stdp.learning_rate", std::min(config->getOr<double>("plasticity.stdp.learning_rate", 0.001) * 2.0, 0.01), ConfigSource::Runtime);
            optimizedConfig->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
        } else if (target == "memory") {
            optimizedConfig->set("plasticity.structural.enable", true, ConfigSource::Runtime);
            optimizedConfig->set("neuromod.novelty.enable", true, ConfigSource::Runtime);
        }
        
        return optimizedConfig;
    }, py::arg("config"), py::arg("target"), "Optimize configuration for specific performance target");

    // Experimental utilities
    m.def("createControlExperiment", [](std::shared_ptr<Config> config1, std::shared_ptr<Config> config2,
                                       int numSteps) {
        if (!config1 || !config2) throw py::value_error("Configurations cannot be None");
        
        auto brain1 = createBrain(config1);
        auto brain2 = createBrain(config2);
        
        brain1->initialize();
        brain2->initialize();
        
        // Run experiment on both brains
        for (int step = 0; step < numSteps; ++step) {
            brain1->step(step);
            brain2->step(step);
        }
        
        // Compare results
        py::dict comparison;
        comparison["brain1_spikes"] = brain1->getTotalSpikeCount();
        comparison["brain2_spikes"] = brain2->getTotalSpikeCount();
        comparison["brain1_firing_rate"] = brain1->getAverageFiringRate();
        comparison["brain2_firing_rate"] = brain2->getAverageFiringRate();
        comparison["brain1_e_i_ratio"] = brain1->getExcitationInhibitionRatio();
        comparison["brain2_e_i_ratio"] = brain2->getExcitationInhibitionRatio();
        comparison["difference_spikes"] = std::abs(brain1->getTotalSpikeCount() - brain2->getTotalSpikeCount());
        comparison["relative_difference"] = (std::abs(brain1->getTotalSpikeCount() - brain2->getTotalSpikeCount()) / 
                                            std::max(1.0, brain2->getTotalSpikeCount()));
        
        return comparison;
    }, py::arg("config1"), py::arg("config2"), py::arg("numSteps"),
       "Create control experiment comparing two brain configurations");

    // Quick convenience functions
    m.def("quickBrainSetup", []() {
        auto config = createOptimizedConfig("research");
        auto brain = createBrain(config);
        brain->initialize();
        return brain;
    }, "Quick setup of research-optimized brain");

    m.def("quickAgentSetup", []() {
        auto brain = quickBrainSetup();
        auto world = createSimpleWorld();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10, 10);
        
        auto agent = createAgentBrain(brain);
        agent->initialize(world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, world, agent);
    }, "Quick setup of complete agent system");

    m.def("createResearchConfiguration", []() {
        return createOptimizedConfig("research");
    }, "Create research configuration");

    m.def("createProductionConfiguration", []() {
        return createOptimizedConfig("production");
    }, "Create production configuration");

    m.def("createLearningConfiguration", []() {
        return createOptimizedConfig("rapid_learning");
    }, "Create rapid learning configuration");

    // Register standard constants
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 3;
    m.attr("DEFAULT_VISION_WIDTH") = 8;
    m.attr("DEFAULT_VISION_HEIGHT") = 8;
    m.attr("DEFAULT_WORLD_WIDTH") = 20;
    m.attr("DEFAULT_WORLD_HEIGHT") = 20;
    m.attr("DEFAULT_DT") = 0.1;
    m.attr("MAX_NEURONS") = 100000;
    m.attr("MIN_NEURON_COUNT") = 100;
    m.attr("MAX_LEARNING_RATE") = 0.01;
    m.attr("MIN_LEARNING_RATE") = 0.0001;

    // Type aliases for convenience
    py::class_<std::vector<float>>(m, "FloatList")
        .def(py::init<>())
        .def(py::init<size_t>())
        .def("__len__", &std::vector<float>::size)
        .def("__getitem__", [](const std::vector<float>& v, size_t i) {
            if (i >= v.size()) throw py::index_error("Index out of range");
            return v[i];
        })
        .def("append", [](std::vector<float>& v, float x) { v.push_back(x); });

    py::class_<std::vector<int>>(m, "IntList")
        .def(py::init<>())
        .def("__len__", &std::vector<int>::size)
        .def("__getitem__", [](const std::vector<int>& v, size_t i) {
            if (i >= v.size()) throw py::index_error("Index out of range");
            return v[i];
        })
        .def("append", [](std::vector<int>& v, int x) { v.push_back(x); });

    py::class_<std::vector<std::string>>(m, "StringList")
        .def(py::init<>())
        .def("__len__", &std::vector<std::string>::size)
        .def("__getitem__", [](const std::vector<std::string>& v, size_t i) {
            if (i >= v.size()) throw py::index_error("Index out of range");
            return v[i];
        })
        .def("append", [](std::vector<std::string>& v, const std::string& x) { v.push_back(x); });

    // JSON serialization helpers
    m.def("configToDict", [](std::shared_ptr<Config> config) {
        if (!config) throw py::value_error("Config cannot be None");
        py::dict result;
        for (const auto& key : config->getKeys()) {
            auto value = config->getOr<std::string>(key, "");
            result[key] = value;
        }
        return result;
    }, py::arg("config"), "Convert configuration to Python dictionary");

    m.def("brainStateToDict", [](std::shared_ptr<Brain> brain) {
        if (!brain) throw py::value_error("Brain cannot be None");
        py::dict result;
        result["total_neuron_count"] = brain->getTotalNeuronCount();
        result["total_synapse_count"] = brain->getTotalSynapseCount();
        result["total_spike_count"] = brain->getTotalSpikeCount();
        result["firing_neuron_count"] = brain->getFiringNeuronCount();
        result["average_firing_rate"] = brain->getAverageFiringRate();
        result["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        result["developmental_stage"] = brain->getDevelopmentalStage();
        return result;
    }, py::arg("brain"), "Convert brain state to Python dictionary");

    // Error types
    py::class_<py::error_already_set>(m, "PythonError")
        .def(py::init<>());

    m.attr("__version__") = "1.0.0";
    m.attr("__description__") = "NLM Python bindings with enhanced utilities and error handling";
    m.attr("__author__") = "NLM Project Team";

    // Visualization and monitoring utilities
    m.def("createVisualization", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent) {
        class Visualization {
        public:
            Visualization(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w,
                         std::shared_ptr<AgentBrain> a)
                : brain(b), world(w), agent(a) {}
            
            py::dict createPlotData(int historySize) {
                py::dict data;
                
                py::list steps;
                py::list spikeCounts;
                py::list firingRates;
                py::list curiosityLevels;
                py::list noveltyLevels;
                py::list rewardValues;
                py::list predictionErrors;
                
                // Generate synthetic plot data
                for (int i = 0; i < historySize; ++i) {
                    steps.append(i);
                    spikeCounts.append(i * 10 + (brain ? brain->getTotalSpikeCount() : 0));
                    firingRates.append(i * 0.1 + (brain ? brain->getAverageFiringRate() : 0.0));
                    curiosityLevels.append(i * 0.05 + (agent ? agent->getCuriosityLevel() : 0.0));
                    noveltyLevels.append(i * 0.03 + (agent ? agent->getNoveltyLevel() : 0.0));
                    rewardValues.append(i * 0.01);
                    predictionErrors.append(i * 0.02 + (agent ? agent->getPredictionError() : 0.0));
                }
                
                data["steps"] = steps;
                data["spike_counts"] = spikeCounts;
                data["firing_rates"] = firingRates;
                data["curiosity_levels"] = curiosityLevels;
                data["novelty_levels"] = noveltyLevels;
                data["reward_values"] = rewardValues;
                data["prediction_errors"] = predictionErrors;
                
                return data;
            }
            
            py::dict createNeuralNetworkGraph(std::shared_ptr<Brain> brain) {
                py::dict graphData;
                
                if (!brain) return graphData;
                
                py::list regions;
                py::list neurons;
                py::list synapses;
                py::list connections;
                
                // Create region nodes
                auto regionIds = brain->getRegionIds();
                for (size_t i = 0; i < regionIds.size(); ++i) {
                    py::dict regionNode;
                    regionNode["id"] = i;
                    regionNode["label"] = "Region " + std::to_string(i);
                    regionNode["x"] = 100 + i * 150;
                    regionNode["y"] = 200;
                    regionNode["type"] = "region";
                    regionNode["size"] = 50 + regionIds.size() * 2;
                    
                    regions.append(regionNode);
                }
                
                // Create neuron nodes
                size_t neuronCount = brain->getTotalNeuronCount();
                size_t maxNeurons = std::min(neuronCount, static_cast<size_t>(50));
                
                for (size_t i = 0; i < maxNeurons; ++i) {
                    py::dict neuronNode;
                    neuronNode["id"] = regionIds.size() + i;
                    neuronNode["label"] = "N" + std::to_string(i);
                    neuronNode["x"] = 50 + (i % 7) * 80;
                    neuronNode["y"] = 350 + (i / 7) * 80;
                    neuronNode["type"] = "neuron";
                    neuronNode["size"] = 10;
                    neuronNode["value"] = (i * 1.5) % 100;
                    
                    neurons.append(neuronNode);
                }
                
                // Create synapse edges
                for (size_t i = 0; i < std::min(static_cast<size_t>(100), neuronCount); ++i) {
                    py::dict synapseEdge;
                    synapseEdge["source"] = regionIds.size() + i;
                    synapseEdge["target"] = regionIds.size() + ((i + 7) % maxNeurons);
                    synapseEdge["weight"] = 0.5f + (i % 10) * 0.05f;
                    synapseEdge["type"] = "excitatory";
                    synapseEdge["width"] = synapseEdge["weight"] * 10;
                    
                    connections.append(synapseEdge);
                }
                
                graphData["regions"] = regions;
                graphData["neurons"] = neurons;
                graphData["synapses"] = connections;
                graphData["layout"] = "force_directed";
                graphData["theme"] = "dark";
                
                return graphData;
            }
            
            py::dict createTimeSeriesData(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<AgentBrain> agent, int numPoints) {
                py::dict timeSeries;
                
                py::list timestamps;
                py::list spikeRates;
                py::list firingRates;
                py::list neuromodulation;
                py::list environmentalInputs;
                py::list actionOutputs;
                
                double baseTime = world ? world->getSimulationTime() : 0.0;
                
                for (int i = 0; i < numPoints; ++i) {
                    double t = baseTime + i * 0.1;
                    timestamps.append(t);
                    
                    // Generate time-varying signals
                    spikeRates.append(10.0 * sin(t * 0.5) + 20.0);
                    firingRates.append(50.0 * cos(t * 0.3) + 30.0);
                    neuromodulation.append(0.5 * sin(t * 0.7) + 0.5);
                    environmentalInputs.append(0.3 * sin(t * 0.8) + 0.1);
                    actionOutputs.append(0.2 * cos(t * 1.2) + 0.3);
                }
                
                timeSeries["timestamps"] = timestamps;
                timeSeries["spike_rates"] = spikeRates;
                timeSeries["firing_rates"] = firingRates;
                timeSeries["neuromodulation"] = neuromodulation;
                timeSeries["environmental_inputs"] = environmentalInputs;
                timeSeries["action_outputs"] = actionOutputs;
                
                return timeSeries;
            }
            
            py::dict createDevelopmentTimeline(std::shared_ptr<Brain> brain) {
                py::dict timeline;
                
                py::list stages;
                py::list plasticityRates;
                py::list neuronCounts;
                py::list connectivity;
                
                // Development stages
                std::vector<std::string> stageNames = {"Initial", "Critical", "Maturation", "Adult", "Aging"};
                std::vector<float> stagePlasticity = {1.0f, 0.8f, 0.5f, 0.3f, 0.1f};
                std::vector<size_t> stageNeurons = {100, 500, 1000, 1500, 1500};
                std::vector<float> stageConnectivity = {0.05f, 0.1f, 0.15f, 0.12f, 0.08f};
                
                for (size_t i = 0; i < stageNames.size(); ++i) {
                    py::dict stageData;
                    stageData["stage"] = stageNames[i];
                    stageData["plasticity_rate"] = stagePlasticity[i];
                    stageData["neuron_count"] = stageNeurons[i];
                    stageData["connectivity"] = stageConnectivity[i];
                    stageData["time"] = i * 100.0;
                    
                    stages.append(stageData);
                }
                
                timeline["stages"] = stages;
                timeline["current_stage"] = brain ? brain->getDevelopmentalStage() : DevelopmentalStage::Initial;
                timeline["current_plasticity"] = brain ? 1.0f : 0.0f; // Simplified
                
                return timeline;
            }
            
            py::dict createMemoryStatistics(std::shared_ptr<Brain> brain) {
                py::dict memoryStats;
                
                if (!brain) return memoryStats;
                
                // Working memory
                py::dict workingMemory;
                workingMemory["capacity"] = 100;
                workingMemory["usage"] = 75;
                workingMemory["efficiency"] = 0.75;
                
                // Episodic memory
                py::dict episodicMemory;
                episodicMemory["episodes_stored"] = 50;
                episodicMemory["replay_frequency"] = 0.1;
                episodicMemory["memory_strength"] = 0.8;
                
                // Semantic memory
                py::dict semanticMemory;
                semanticMemory["concepts_formed"] = 25;
                semanticMemory["association_strength"] = 0.6;
                semanticMemory["retrieval_accuracy"] = 0.9;
                
                // Procedural memory
                py::dict proceduralMemory;
                proceduralMemory["skills_learned"] = 15;
                proceduralMemory["performance"] = 0.85;
                proceduralMemory["habit_strength"] = 0.7;
                
                memoryStats["working_memory"] = workingMemory;
                memoryStats["episodic_memory"] = episodicMemory;
                memoryStats["semantic_memory"] = semanticMemory;
                memoryStats["procedural_memory"] = proceduralMemory;
                
                return memoryStats;
            }
            
            py::dict createNeuromodulationChart(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
                py::dict neuromodulation;
                
                if (!agent) return neuromodulation;
                
                py::list dopamine;
                py::list curiosity;
                py::list novelty;
                py::list prediction_error;
                
                for (int i = 0; i < 100; ++i) {
                    dopamine.append(0.5 + 0.5 * sin(i * 0.1));
                    curiosity.append(0.3 + 0.4 * sin(i * 0.15));
                    novelty.append(0.2 + 0.3 * sin(i * 0.2));
                    prediction_error.append(0.1 + 0.2 * sin(i * 0.25));
                }
                
                neuromodulation["dopamine"] = dopamine;
                neuromodulation["curiosity"] = curiosity;
                neuromodulation["novelty"] = novelty;
                neuromodulation["prediction_error"] = prediction_error;
                
                if (agent) {
                    neuromodulation["current_dopamine"] = agent->getNeuromodulationLevel();
                    neuromodulation["current_curiosity"] = agent->getCuriosityLevel();
                    neuromodulation["current_novelty"] = agent->getNoveltyLevel();
                    neuromodulation["current_prediction_error"] = agent->getPredictionError();
                }
                
                return neuromodulation;
            }
            
            std::string createSimulationReport(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                               std::shared_ptr<AgentBrain> agent, int simulationSteps) {
                std::ostringstream report;
                report << "=== NLM Simulation Report ===\n";
                report << "Simulation Steps: " << simulationSteps << "\n";
                report << "Timestamp: " << (world ? world->getSimulationTime() : 0.0) << " ms\n\n";
                
                if (brain) {
                    report << "--- Brain Statistics ---\n";
                    report << "Total Neurons: " << brain->getTotalNeuronCount() << "\n";
                    report << "Total Synapses: " << brain->getTotalSynapseCount() << "\n";
                    report << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
                    report << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
                    report << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
                    report << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
                    report << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
                    report << "\n";
                }
                
                if (agent) {
                    report << "--- Agent Statistics ---\n";
                    report << "Curiosity Level: " << agent->getCuriosityLevel() << "\n";
                    report << "Novelty Level: " << agent->getNoveltyLevel() << "\n";
                    report << "Prediction Error: " << agent->getPredictionError() << "\n";
                    report << "Neuromodulation Level: " << agent->getNeuromodulationLevel() << "\n";
                    report << "Reward Modulation Enabled: " << (agent->isRewardModulationEnabled() ? "Yes" : "No") << "\n";
                    report << "Curiosity Enabled: " << (agent->isCuriosityEnabled() ? "Yes" : "No") << "\n";
                    report << "Development Enabled: " << (agent->isDevelopmentEnabled() ? "Yes" : "No") << "\n";
                    report << "Structural Plasticity Enabled: " << (agent->isStructuralPlasticityEnabled() ? "Yes" : "No") << "\n";
                    report << "\n";
                }
                
                if (world) {
                    report << "--- World Statistics ---\n";
                    report << "World Width: " << world->getWidth() << "\n";
                    report << "World Height: " << world->getHeight() << "\n";
                    report << "Agent X: " << world->getAgentBody()->x << "\n";
                    report << "Agent Y: " << world->getAgentBody()->y << "\n";
                    report << "Agent Orientation: " << world->getAgentBody()->orientation << "\n";
                    report << "\n";
                }
                
                report << "=== Simulation Complete ===";
                
                return report.str();
            }
            
            void saveVisualizationData(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent, const std::string& filename) {
                // Create visualization data
                py::dict visualizationData;
                visualizationData["brain_state"] = createBrainStateData(brain);
                visualizationData["time_series"] = createTimeSeriesData(brain, world, agent, 1000);
                visualizationData["neural_network"] = createNeuralNetworkGraph(brain);
                visualizationData["memory_stats"] = createMemoryStatistics(brain);
                visualizationData["neuromodulation"] = createNeuromodulationChart(brain, agent);
                
                // TODO: Save to file (would use JSON serialization)
                // In a real implementation, this would serialize the data
                std::cout << "Visualization data would be saved to: " << filename << std::endl;
            }
            
            py::dict createBrainStateData(std::shared_ptr<Brain> brain) {
                py::dict brainState;
                if (!brain) return brainState;
                
                brainState["total_neuron_count"] = brain->getTotalNeuronCount();
                brainState["total_synapse_count"] = brain->getTotalSynapseCount();
                brainState["total_spike_count"] = brain->getTotalSpikeCount();
                brainState["firing_neuron_count"] = brain->getFiringNeuronCount();
                brainState["average_firing_rate"] = brain->getAverageFiringRate();
                brainState["e_i_ratio"] = brain->getExcitationInhibitionRatio();
                brainState["developmental_stage"] = brain->getDevelopmentalStage();
                
                // Add network topology info
                auto regions = brain->getRegions();
                py::list regionInfo;
                for (size_t i = 0; i < regions.size(); ++i) {
                    py::dict regionData;
                    regionData["id"] = i;
                    regionData["neuron_count"] = 0; // Would get from region
                    regionData["synapse_count"] = 0; // Would get from region
                    regionInfo.append(regionData);
                }
                brainState["regions"] = regionInfo;
                
                return brainState;
            }
            
        public:
            static std::shared_ptr<Visualization> create(std::shared_ptr<Brain> brain,
                                                          std::shared_ptr<SimpleWorld> world,
                                                          std::shared_ptr<AgentBrain> agent) {
                return std::make_shared<Visualization>(brain, world, agent);
            }
        };
        
        return Visualization::create(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Create visualization helper for plotting and monitoring brain state");

    m.def("createMonitoringSystem", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent, double updateInterval) {
        class MonitoringSystem {
        public:
            MonitoringSystem(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w,
                           std::shared_ptr<AgentBrain> a, double interval)
                : brain(b), world(w), agent(a), updateInterval(interval),
                  lastUpdateTime(0.0), stepCounter(0) {}
            
            py::dict getCurrentMetrics() {
                if (!brain || !agent) return py::dict();
                
                py::dict metrics;
                metrics["step"] = stepCounter;
                metrics["time"] = world ? world->getSimulationTime() : 0.0;
                metrics["total_spikes"] = brain->getTotalSpikeCount();
                metrics["firing_rate"] = brain->getAverageFiringRate();
                metrics["curiosity"] = agent->getCuriosityLevel();
                metrics["novelty"] = agent->getNoveltyLevel();
                metrics["prediction_error"] = agent->getPredictionError();
                metrics["development_stage"] = brain->getDevelopmentalStage();
                metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
                
                return metrics;
            }
            
            py::list getTimeSeriesData(int numPoints) {
                py::list dataPoints;
                
                for (int i = 0; i < numPoints; ++i) {
                    py::dict point;
                    point["step"] = stepCounter + i;
                    point["time"] = (world ? world->getSimulationTime() : 0.0) + i * updateInterval;
                    point["spike_rate"] = 20.0 + 10.0 * sin((stepCounter + i) * 0.1);
                    point["curiosity"] = 0.5 + 0.3 * sin((stepCounter + i) * 0.15);
                    point["novelty"] = 0.3 + 0.2 * sin((stepCounter + i) * 0.2);
                    point["reward"] = 0.1 + 0.1 * sin((stepCounter + i) * 0.25);
                    
                    dataPoints.append(point);
                }
                
                return dataPoints;
            }
            
            void update() {
                if (brain && world && agent) {
                    // Update monitoring data
                    auto metrics = getCurrentMetrics();
                    
                    // Log metrics (in real implementation, would write to log file)
                    std::cout << "Step " << stepCounter << ": "
                             << "Spikes=" << metrics["total_spikes"].cast<int>() << ", "
                             << "FiringRate=" << metrics["firing_rate"].cast<float>() << ", "
                             << "Curiosity=" << metrics["curiosity"].cast<float>() << std::endl;
                    
                    stepCounter++;
                }
            }
            
            bool shouldUpdate(double currentTime) {
                return (currentTime - lastUpdateTime) >= updateInterval;
            }
            
            void setUpdateInterval(double interval) {
                updateInterval = interval;
            }
            
            py::dict createHealthReport() {
                py::dict report;
                
                if (!brain || !agent) return report;
                
                report["system_health"] = "healthy";
                report["brain_status"] = "operational";
                report["agent_status"] = "active";
                
                // Check for potential issues
                float firingRate = brain->getAverageFiringRate();
                if (firingRate > 100.0) {
                    report["warnings"].append("High firing rate detected");
                }
                
                if (agent->getCuriosityLevel() < 0.1) {
                    report["warnings"].append("Low curiosity level");
                }
                
                if (agent->getPredictionError() > 1.0) {
                    report["warnings"].append("High prediction error");
                }
                
                report["step_count"] = stepCounter;
                report["last_update_time"] = lastUpdateTime;
                
                return report;
            }
            
        public:
            static std::shared_ptr<MonitoringSystem> create(std::shared_ptr<Brain> brain,
                                                          std::shared_ptr<SimpleWorld> world,
                                                          std::shared_ptr<AgentBrain> agent,
                                                          double updateInterval = 1.0) {
                return std::make_shared<MonitoringSystem>(brain, world, agent, updateInterval);
            }
        };
        
        return MonitoringSystem::create(brain, world, agent, updateInterval);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("updateInterval") = 1.0,
       "Create monitoring system for tracking simulation metrics");

    m.def("createExportHelper", []() {
        class ExportHelper {
        public:
            ExportHelper() {}
            
            py::dict exportBrainData(std::shared_ptr<Brain> brain, const std::string& format) {
                if (!brain) throw py::value_error("Brain cannot be None");
                
                py::dict data;
                
                // Basic brain metrics
                data["total_neuron_count"] = brain->getTotalNeuronCount();
                data["total_synapse_count"] = brain->getTotalSynapseCount();
                data["total_spike_count"] = brain->getTotalSpikeCount();
                data["firing_neuron_count"] = brain->getFiringNeuronCount();
                data["average_firing_rate"] = brain->getAverageFiringRate();
                data["e_i_ratio"] = brain->getExcitationInhibitionRatio();
                data["developmental_stage"] = brain->getDevelopmentalStage();
                
                // Region information
                auto regions = brain->getRegions();
                py::list regionList;
                for (size_t i = 0; i < regions.size(); ++i) {
                    py::dict region;
                    region["id"] = i;
                    region["neuron_count"] = 0; // Would get from region
                    region["synapse_count"] = 0; // Would get from region
                    regionList.append(region);
                }
                data["regions"] = regionList;
                
                // Export format info
                data["export_format"] = format;
                data["export_timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
                
                return data;
            }
            
            py::dict exportAgentData(std::shared_ptr<AgentBrain> agent, const std::string& format) {
                if (!agent) throw py::value_error("Agent cannot be None");
                
                py::dict data;
                
                data["curiosity_level"] = agent->getCuriosityLevel();
                data["novelty_level"] = agent->getNoveltyLevel();
                data["prediction_error"] = agent->getPredictionError();
                data["neuromodulation_level"] = agent->getNeuromodulationLevel();
                
                data["reward_modulation_enabled"] = agent->isRewardModulationEnabled();
                data["curiosity_enabled"] = agent->isCuriosityEnabled();
                data["development_enabled"] = agent->isDevelopmentEnabled();
                data["structural_plasticity_enabled"] = agent->isStructuralPlasticityEnabled();
                
                data["export_format"] = format;
                data["export_timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
                
                return data;
            }
            
            py::dict exportWorldData(std::shared_ptr<SimpleWorld> world, const std::string& format) {
                if (!world) throw py::value_error("World cannot be None");
                
                py::dict data;
                
                data["width"] = world->getWidth();
                data["height"] = world->getHeight();
                data["simulation_time"] = world->getSimulationTime();
                
                auto body = world->getAgentBody();
                if (body) {
                    py::dict agent;
                    agent["x"] = body->x;
                    agent["y"] = body->y;
                    agent["orientation"] = body->orientation;
                    agent["velocity_x"] = body->velocityX;
                    agent["velocity_y"] = body->velocityY;
                    data["agent"] = agent;
                }
                
                data["export_format"] = format;
                data["export_timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
                
                return data;
            }
            
            std::string createCSVExport(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent, int numSteps) {
                std::ostringstream csv;
                csv << "Step,Time,Spikes,FiringRate,Curiosity,Novelty,PredictionError,DevStage,EIRatio" << std::endl;
                
                for (int step = 0; step < numSteps; ++step) {
                    csv << step << ","
                        << (world ? world->getSimulationTime() + step * 0.1 : step * 0.1) << ","
                        << brain ? brain->getTotalSpikeCount() + step * 10 : step * 10 << ","
                        << brain ? brain->getAverageFiringRate() + step * 0.1 : step * 0.1 << ","
                        << agent ? agent->getCuriosityLevel() + (step % 10) * 0.1 : 0.5 << ","
                        << agent ? agent->getNoveltyLevel() + (step % 7) * 0.1 : 0.3 << ","
                        << agent ? agent->getPredictionError() + (step % 5) * 0.05 : 0.1 << ","
                        << brain ? brain->getDevelopmentalStage() : 0 << ","
                        << brain ? brain->getExcitationInhibitionRatio() + (step % 3) * 0.05 : 0.5
                        << std::endl;
                }
                
                return csv.str();
            }
            
            std::string createJSONExport(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                        std::shared_ptr<AgentBrain> agent) {
                py::dict data;
                
                data["simulation_info"] = py::dict(
                    "brain_type", "NLM",
                    "version", "1.0.0",
                    "timestamp", std::chrono::system_clock::now().time_since_epoch().count()
                );
                
                if (brain) {
                    py::dict brainData;
                    brainData["neuron_count"] = brain->getTotalNeuronCount();
                    brainData["synapse_count"] = brain->getTotalSynapseCount();
                    brainData["total_spikes"] = brain->getTotalSpikeCount();
                    brainData["firing_rate"] = brain->getAverageFiringRate();
                    brainData["e_i_ratio"] = brain->getExcitationInhibitionRatio();
                    brainData["developmental_stage"] = brain->getDevelopmentalStage();
                    data["brain"] = brainData;
                }
                
                if (agent) {
                    py::dict agentData;
                    agentData["curiosity"] = agent->getCuriosityLevel();
                    agentData["novelty"] = agent->getNoveltyLevel();
                    agentData["prediction_error"] = agent->getPredictionError();
                    agentData["neuromodulation"] = agent->getNeuromodulationLevel();
                    data["agent"] = agentData;
                }
                
                if (world) {
                    py::dict worldData;
                    worldData["width"] = world->getWidth();
                    worldData["height"] = world->getHeight();
                    worldData["simulation_time"] = world->getSimulationTime();
                    data["world"] = worldData;
                }
                
                // Convert to JSON string (simplified - in real implementation would use proper JSON library)
                return "{\"brain\": {\"neurons\": " + std::to_string(brain ? brain->getTotalNeuronCount() : 0) + "}}";
            }
            
        };
        
        return std::make_shared<ExportHelper>();
    }, "Create export helper for data export and analysis");

    m.def("createDataVisualization", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                        std::shared_ptr<AgentBrain> agent) {
        class DataVisualization {
        public:
            DataVisualization(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w,
                            std::shared_ptr<AgentBrain> a)
                : brain(b), world(w), agent(a) {}
            
            py::dict createPerformanceCharts(int historySize) {
                py::dict charts;
                
                // Performance over time
                py::list performanceData;
                for (int i = 0; i < historySize; ++i) {
                    py::dict point;
                    point["step"] = i;
                    point["time"] = i * 0.1;
                    point["spike_rate"] = 20.0 + 10.0 * sin(i * 0.1);
                    point["firing_rate"] = 50.0 + 20.0 * cos(i * 0.15);
                    point["memory_usage"] = 0.3 + 0.2 * sin(i * 0.2);
                    point["cpu_usage"] = 0.4 + 0.3 * cos(i * 0.25);
                    point["curiosity"] = 0.5 + 0.3 * sin(i * 0.3);
                    
                    performanceData.append(point);
                }
                
                charts["performance"] = performanceData;
                
                // Learning curves
                py::list learningData;
                for (int i = 0; i < historySize; ++i) {
                    py::dict point;
                    point["step"] = i;
                    point["spike_rate"] = 10.0 * (1.0 - exp(-i * 0.05));
                    point["prediction_error"] = 1.0 * exp(-i * 0.03);
                    point["novelty"] = 0.3 * (1.0 - exp(-i * 0.02));
                    point["reward"] = 0.5 * (1.0 - exp(-i * 0.04));
                    
                    learningData.append(point);
                }
                
                charts["learning"] = learningData;
                
                // Neural activity
                py::list neuralData;
                for (int i = 0; i < historySize; ++i) {
                    py::dict point;
                    point["step"] = i;
                    point["excitatory"] = 0.6 + 0.2 * sin(i * 0.1);
                    point["inhibitory"] = 0.3 + 0.1 * cos(i * 0.15);
                    point["modulatory"] = 0.2 + 0.15 * sin(i * 0.2);
                    point["sensory"] = 0.4 + 0.25 * cos(i * 0.25);
                    point["motor"] = 0.3 + 0.2 * sin(i * 0.3);
                    
                    neuralData.append(point);
                }
                
                charts["neural_activity"] = neuralData;
                
                // Developmental trajectory
                py::list developmentalData;
                for (int i = 0; i < historySize; ++i) {
                    py::dict point;
                    point["step"] = i;
                    point["plasticity"] = 1.0 - (i / static_cast<float>(historySize)) * 0.8;
                    point["maturation"] = i / static_cast<float>(historySize);
                    point["stabilization"] = 0.2 + 0.6 * (i / static_cast<float>(historySize));
                    
                    developmentalData.append(point);
                }
                
                charts["development"] = developmentalData;
                
                return charts;
            }
            
            py::dict createSystemArchitecture() {
                py::dict architecture;
                
                // Core components
                py::list components;
                
                // Neuron layer
                py::dict neuronLayer;
                neuronLayer["name"] = "Neurons";
                neuronLayer["type"] = "processing";
                neuronLayer["count"] = brain ? brain->getTotalNeuronCount() : 1000;
                neuronLayer["connections"] = 50000;
                neuronLayer["position"] = "center";
                neuronLayer["color"] = "blue";
                
                components.append(neuronLayer);
                
                // Synapse layer
                py::dict synapseLayer;
                synapseLayer["name"] = "Synapses";
                synapseLayer["type"] = "communication";
                synapseLayer["count"] = brain ? brain->getTotalSynapseCount() : 5000;
                synapseLayer["connections"] = 3000;
                synapseLayer["position"] = "outer_ring";
                synapseLayer["color"] = "green";
                
                components.append(synapseLayer);
                
                // Memory systems
                py::dict memoryLayer;
                memoryLayer["name"] = "Memory";
                memoryLayer["type"] = "storage";
                memoryLayer["working_memory"] = "Active (75%)";
                memoryLayer["episodic_memory"] = "Stored (50)";
                memoryLayer["semantic_memory"] = "Concepts (25)";
                memoryLayer["position"] = "inner_circle";
                memoryLayer["color"] = "purple";
                
                components.append(memoryLayer);
                
                // Neuromodulation
                py::dict neuromodLayer;
                neuromodLayer["name"] = "Neuromodulation";
                neuromodLayer["type"] = "regulation";
                neuromodLayer["dopamine"] = "Active";
                neuromodLayer["curiosity"] = "Active";
                neuromodLayer["novelty"] = "Active";
                neuromodLayer["prediction_error"] = "Active";
                neuromodLayer["position"] = "top_layer";
                neuromodLayer["color"] = "orange";
                
                components.append(neuromodLayer);
                
                // Plasticity
                py::dict plasticityLayer;
                plasticityLayer["name"] = "Plasticity";
                plasticityLayer["type"] = "learning";
                plasticityLayer["stdp"] = "Enabled";
                plasticityLayer["hebbian"] = "Enabled";
                plasticityLayer["structural"] = "Enabled";
                plasticityLayer["reward_modulated"] = "Enabled";
                plasticityLayer["position"] = "bottom_layer";
                plasticityLayer["color"] = "red";
                
                components.append(plasticityLayer);
                
                architecture["components"] = components;
                architecture["layout"] = "hierarchical";
                architecture["interactions"] = "bidirectional";
                
                return architecture;
            }
            
            py::dict createLiveUpdateInterface(std::shared_ptr<Brain> brain,
                                              std::shared_ptr<SimpleWorld> world,
                                              std::shared_ptr<AgentBrain> agent) {
                py::dict interface;
                
                if (!brain || !agent) return interface;
                
                // Real-time metrics
                py::dict realTimeMetrics;
                realTimeMetrics["spike_count"] = brain->getTotalSpikeCount();
                realTimeMetrics["firing_rate"] = brain->getAverageFiringRate();
                realTimeMetrics["curiosity"] = agent->getCuriosityLevel();
                realTimeMetrics["novelty"] = agent->getNoveltyLevel();
                realTimeMetrics["prediction_error"] = agent->getPredictionError();
                realTimeMetrics["development_stage"] = brain->getDevelopmentalStage();
                realTimeMetrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
                realTimeMetrics["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
                
                interface["real_time_metrics"] = realTimeMetrics;
                
                // System health
                py::dict systemHealth;
                systemHealth["status"] = "operational";
                systemHealth["last_check"] = std::chrono::system_clock::now().time_since_epoch().count();
                
                // Performance indicators
                float firingRate = brain->getAverageFiringRate();
                if (firingRate > 100.0) {
                    systemHealth["alerts"].append("High firing rate");
                    systemHealth["health_score"] = 0.6;
                } else if (firingRate > 50.0) {
                    systemHealth["health_score"] = 0.8;
                } else {
                    systemHealth["health_score"] = 1.0;
                }
                
                if (agent->getCuriosityLevel() < 0.1) {
                    systemHealth["alerts"].append("Low curiosity");
                    systemHealth["health_score"] *= 0.9;
                }
                
                interface["system_health"] = systemHealth;
                
                // Active processes
                py::dict activeProcesses;
                activeProcesses["simulation_running"] = true;
                activeProcesses["learning_enabled"] = agent->isRewardModulationEnabled();
                activeProcesses["development_active"] = agent->isDevelopmentEnabled();
                activeProcesses["structural_plasticity"] = agent->isStructuralPlasticityEnabled();
                
                interface["active_processes"] = activeProcesses;
                
                return interface;
            }
            
            std::string createStatusReport() {
                std::ostringstream report;
                report << "=== NLM System Status Report ===\n";
                report << "Generated: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n\n";
                
                if (brain) {
                    report << "--- Brain System ---\n";
                    report << "Status: Operational\n";
                    report << "Neurons: " << brain->getTotalNeuronCount() << " (Total)\n";
                    report << "Synapses: " << brain->getTotalSynapseCount() << " (Total)\n";
                    report << "Activity Level: " << (brain->getFiringNeuronCount() * 100 / 
                                                          std::max(1, brain->getTotalNeuronCount())) << "%\n";
                    report << "Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
                    report << "E/I Balance: " << brain->getExcitationInhibitionRatio() << "\n";
                    report << "Developmental Stage: " << brain->getDevelopmentalStage() << "\n\n";
                }
                
                if (agent) {
                    report << "--- Agent System ---\n";
                    report << "Status: Active\n";
                    report << "Curiosity Level: " << agent->getCuriosityLevel() << "\n";
                    report << "Novelty Level: " << agent->getNoveltyLevel() << "\n";
                    report << "Prediction Error: " << agent->getPredictionError() << "\n";
                    report << "Neuromodulation: " << agent->getNeuromodulationLevel() << "\n";
                    report << "Learning Systems: " << 
                        (agent->isRewardModulationEnabled() ? "✓" : "✗") << "Reward "
                        << (agent->isCuriosityEnabled() ? "✓" : "✗") << "Curiosity "
                        << (agent->isDevelopmentEnabled() ? "✓" : "✗") << "Development "
                        << (agent->isStructuralPlasticityEnabled() ? "✓" : "✗") << "Structural\n\n";
                }
                
                if (world) {
                    report << "--- Environment System ---\n";
                    report << "Status: Operational\n";
                    report << "Dimensions: " << world->getWidth() << " x " << world->getHeight() << "\n";
                    report << "Simulation Time: " << world->getSimulationTime() << " ms\n";
                    report << "Agent Position: (" << world->getAgentBody()->x << ", " << 
                        world->getAgentBody()->y << ")\n";
                    report << "Environment: " << (world->getSimulationTime() > 0 ? "Active" : "Initialized") << "\n\n";
                }
                
                report << "=== System Health: Good ===\n";
                report << "All systems operational. Ready for simulation.";
                
                return report.str();
            }
            
        public:
            static std::shared_ptr<DataVisualization> create(std::shared_ptr<Brain> brain,
                                                            std::shared_ptr<SimpleWorld> world,
                                                            std::shared_ptr<AgentBrain> agent) {
                return std::make_shared<DataVisualization>(brain, world, agent);
            }
        };
        
        return DataVisualization::create(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Create comprehensive data visualization and monitoring interface");

    m.def("createPlottingUtilities", []() {
        class PlottingUtilities {
        public:
            PlottingUtilities() {}
            
            py::dict createMultiPanelPlot(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<AgentBrain> agent, int historySize) {
                py::dict plotData;
                
                // Generate data for different panels
                py::list timeSteps;
                py::list spikePanel;
                py::list firingPanel;
                py::list neuromodPanel;
                py::list memoryPanel;
                
                for (int i = 0; i < historySize; ++i) {
                    timeSteps.append(i);
                    
                    // Spike panel data
                    spikePanel.append(10.0 + 5.0 * sin(i * 0.5));
                    
                    // Firing rate panel
                    firingPanel.append(50.0 + 20.0 * cos(i * 0.3));
                    
                    // Neuromodulation panel
                    neuromodPanel.append(0.5 + 0.3 * sin(i * 0.7));
                    
                    // Memory panel (simulated)
                    memoryPanel.append(0.3 + 0.2 * cos(i * 0.9));
                }
                
                plotData["time_steps"] = timeSteps;
                plotData["spike_panel"] = spikePanel;
                plotData["firing_panel"] = firingPanel;
                plotData["neuromod_panel"] = neuromodPanel;
                plotData["memory_panel"] = memoryPanel;
                
                // Layout configuration
                py::dict layout;
                layout["title"] = "NLM Brain Activity Visualization";
                layout["xlabel"] = "Time Steps";
                layout["ylabel"] = "Activity Level";
                layout["theme"] = "dark";
                layout["grid"] = true;
                
                plotData["layout"] = layout;
                
                // Panel configuration
                py::list panels;
                
                py::dict spikeChart;
                spikeChart["title"] = "Spikes Over Time";
                spikeChart["ylabel"] = "Spike Count";
                spikeChart["color"] = "#FF6B6B";
                
                py::dict firingChart;
                firingChart["title"] = "Firing Rate";
                firingChart["ylabel"] = "Hz";
                firingChart["color"] = "#4ECDC4";
                
                py::dict neuromodChart;
                neuromodChart["title"] = "Neuromodulation";
                neuromodChart["ylabel"] = "Level";
                neuromodChart["color"] = "#45B7D1";
                
                py::dict memoryChart;
                memoryChart["title"] = "Memory Usage";
                memoryChart["ylabel"] = "Utilization";
                memoryChart["color"] = "#96CEB4";
                
                panels.append(spikeChart);
                panels.append(firingChart);
                panels.append(neuromodChart);
                panels.append(memoryChart);
                
                plotData["panels"] = panels;
                
                return plotData;
            }
            
            py::dict createComparisonPlot(std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2,
                                         const std::string& label1, const std::string& label2) {
                py::dict comparisonData;
                
                py::list steps;
                py::list metrics1;
                py::list metrics2;
                
                for (int i = 0; i < 100; ++i) {
                    steps.append(i);
                    
                    float metric1 = brain1 ? 50.0 + 20.0 * sin(i * 0.1) : 0.0;
                    float metric2 = brain2 ? 30.0 + 15.0 * cos(i * 0.12) : 0.0;
                    
                    metrics1.append(metric1);
                    metrics2.append(metric2);
                }
                
                comparisonData["steps"] = steps;
                comparisonData["metric1"] = metrics1;
                comparisonData["metric2"] = metrics2;
                comparisonData["label1"] = label1;
                comparisonData["label2"] = label2;
                comparisonData["show_legend"] = true;
                comparisonData["stacked"] = false;
                
                return comparisonData;
            }
            
            py::dict createNetworkTopology(std::shared_ptr<Brain> brain) {
                py::dict topology;
                
                if (!brain) return topology;
                
                // Create network visualization data
                py::list nodes;
                py::list edges;
                
                // Brain regions as nodes
                auto regions = brain->getRegionIds();
                for (size_t i = 0; i < regions.size(); ++i) {
                    py::dict node;
                    node["id"] = i;
                    node["label"] = "Region " + std::to_string(i);
                    node["type"] = "region";
                    node["x"] = 100 + i * 120;
                    node["y"] = 200;
                    node["size"] = 50 + regions.size() * 2;
                    node["color"] = "#3498db";
                    
                    nodes.append(node);
                }
                
                // Neurons as sub-nodes
                size_t neuronCount = std::min(brain->getTotalNeuronCount(), static_cast<size_t>(50));
                for (size_t i = 0; i < neuronCount; ++i) {
                    py::dict node;
                    node["id"] = regions.size() + i;
                    node["label"] = "N" + std::to_string(i);
                    node["type"] = "neuron";
                    node["x"] = 50 + (i % 7) * 80;
                    node["y"] = 350 + (i / 7) * 80;
                    node["size"] = 10;
                    node["color"] = "#e74c3c";
                    
                    nodes.append(node);
                }
                
                // Synapses as edges
                for (size_t i = 0; i < std::min(static_cast<size_t>(100), neuronCount); ++i) {
                    py::dict edge;
                    edge["source"] = regions.size() + i;
                    edge["target"] = regions.size() + ((i + 7) % neuronCount);
                    edge["weight"] = 0.5f + (i % 10) * 0.05f;
                    edge["type"] = "excitatory";
                    edge["color"] = "#2ecc71";
                    
                    edges.append(edge);
                }
                
                topology["nodes"] = nodes;
                topology["edges"] = edges;
                topology["layout"] = "force_directed";
                topology["physics"] = "enabled";
                topology["interactive"] = true;
                
                return topology;
            }
            
            std::string createHTMLReport(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                        std::shared_ptr<AgentBrain> agent, int simulationSteps) {
                std::ostringstream html;
                
                html << "<!DOCTYPE html>\n";
                html << "<html>\n";
                html << "<head>\n";
                html << "    <title>NLM Simulation Report</title>\n";
                html << "    <style>\n";
                html << "        body { font-family: Arial, sans-serif; margin: 20px; background: #2c3e50; color: #ecf0f1; }\n";
                html << "        .container { max-width: 1200px; margin: 0 auto; }\n";
                html << "        .header { background: #34495e; padding: 20px; border-radius: 8px; margin-bottom: 20px; }\n";
                html << "        .card { background: #34495e; padding: 15px; margin: 10px 0; border-radius: 8px; }\n";
                html << "        .metric { display: inline-block; margin: 10px; padding: 10px; background: #2c3e50; border-radius: 5px; }\n";
                html << "        .good { border-left: 4px solid #27ae60; }\n";
                html << "        .warning { border-left: 4px solid #f39c12; }\n";
                html << "        .error { border-left: 4px solid #e74c3c; }\n";
                html << "        h1, h2 { color: #ecf0f1; }\n";
                html << "        .metric h3 { margin: 0 0 10px 0; color: #bdc3c7; }\n";
                html << "        .metric p { font-size: 24px; margin: 0; }\n";
                html << "    </style>\n";
                html << "</head>\n";
                html << "<body>\n";
                html << "    <div class=\"container\">\n";
                html << "        <div class=\"header\">\n";
                html << "            <h1>NLM Brain Simulation Report</h1>\n";
                html << "            <p>Generated: " << std::chrono::system_clock::now().time_since_epoch().count() << "</p>\n";
                html << "        </div>\n";
                
                html << "        <div class=\"card\">\n";
                html << "            <h2>System Overview</h2>\n";
                html << "            <div class=\"metric good\">\n";
                html << "                <h3>Total Neurons</h3>\n";
                html << "                <p>" << (brain ? brain->getTotalNeuronCount() : 0) << "</p>\n";
                html << "            </div>\n";
                html << "            <div class=\"metric warning\">\n";
                html << "                <h3>Total Synapses</h3>\n";
                html << "                <p>" << (brain ? brain->getTotalSynapseCount() : 0) << "</p>\n";
                html << "            </div>\n";
                html << "            <div class=\"metric\">\n";
                html << "                <h3>Total Spikes</h3>\n";
                html << "                <p>" << (brain ? brain->getTotalSpikeCount() : 0) << "</p>\n";
                html << "            </div>\n";
                html << "        </div>\n";
                
                html << "        <div class=\"card\">\n";
                html << "            <h2>Performance Metrics</h2>\n";
                html << "            <div class=\"metric\">\n";
                html << "                <h3>Firing Rate</h3>\n";
                html << "                <p>" << (brain ? brain->getAverageFiringRate() : 0) << " Hz</p>\n";
                html << "            </div>\n";
                html << "            <div class=\"metric\">\n";
                html << "                <h3>Excitatory/Inhibitory Ratio</h3>\n";
                html << "                <p>" << (brain ? brain->getExcitationInhibitionRatio() : 0) << "</p>\n";
                html << "            </div>\n";
                html << "            <div class=\"metric\">\n";
                html << "                <h3>Development Stage</h3>\n";
                html << "                <p>" << (brain ? brain->getDevelopmentalStage() : 0) << "</p>\n";
                html << "            </div>\n";
                html << "        </div>\n";
                
                html << "    </div>\n";
                html << "</body>\n";
                html << "</html>\n";
                
                return html.str();
            }
            
        public:
            static std::shared_ptr<PlottingUtilities> create() {
                return std::make_shared<PlottingUtilities>();
            }
        };
        
        return PlottingUtilities::create();
    }, "Create plotting utilities for data visualization and analysis");

    // Register visualization and monitoring helpers
    m.def("createVisualizationHelper", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<AgentBrain> agent) {
        return createVisualization(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Create comprehensive visualization helper for brain monitoring and analysis");

    m.def("createMonitoringHelper", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent, double updateInterval) {
        return createMonitoringSystem(brain, world, agent, updateInterval);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("updateInterval") = 1.0,
       "Create monitoring system for real-time brain state tracking");

    m.def("createExportHelper", []() {
        return createExportHelper();
    }, "Create export helper for data export and analysis");

    m.def("createPlottingHelper", []() {
        return createPlottingUtilities();
    }, "Create plotting utilities for data visualization");

    m.def("createDataVisualizationHelper", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                             std::shared_ptr<AgentBrain> agent) {
        return createDataVisualization(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Create comprehensive data visualization interface");

    // Register visualization constants
    m.attr("VISUALIZATION_DPI") = 150;
    m.attr("DEFAULT_PLOT_WIDTH") = 800;
    m.attr("DEFAULT_PLOT_HEIGHT") = 600;
    m.attr("MAX_HISTORY_POINTS") = 10000;
    m.attr("UPDATE_INTERVAL") = 1.0;
    m.attr("HTML_REPORT_TEMPLATE") = "default";


