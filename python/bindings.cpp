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

    // Python utility functions and advanced features
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, int steps, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_pair(brain->getTotalSpikeCount(), agent->getCuriosityLevel());
    }, py::arg("brain"), py::arg("steps"), py::arg("world"),
       "Run a complete simulation with brain and world");

    m.def("create_learning_config", [](int neuron_count, float connection_probability) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.synapse_density", connection_probability);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        return config;
    }, py::arg("neuron_count") = 1000, py::arg("connection_probability") = 0.1f,
       "Create a learning configuration with specified parameters");

    m.def("create_advanced_config", [](int neuron_count, int region_count, bool enable_all_features) {
        auto config = std::make_shared<Config>();
        
        // Basic configuration
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.region_count", region_count);
        config->set("brain.connection_probability", 0.1f);
        
        // Plasticity settings
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.0005f);
        config->set("plasticity.structural.enable", true);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Neuromodulation settings
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.prediction_error.scale", 0.5f);
        
        // Development settings
        config->set("development.enable", true);
        config->set("development.critical_period_end", 300.0f);
        
        // Memory system
        config->set("memory.working_capacity", neuron_count / 10);
        config->set("memory.episodic_max_episodes", 1000);
        
        // Performance
        config->set("simulation.timestep", 0.001);
        config->set("checkpoint.enable", true);
        
        if (enable_all_features) {
            config->set("cognition.planner.enabled", true);
            config->set("cognition.attention.enabled", true);
            config->set("cognition.concept_formation.enabled", true);
            config->set("prediction.enabled", true);
            config->set("visualization.enable", false); // Performance intensive
        }
        
        return config;
    }, py::arg("neuron_count") = 1000, py::arg("region_count") = 1, py::arg("enable_all_features") = true,
       "Create an advanced configuration with learning and development features");

    m.def("create_agent_config", [](const std::string& behavior_mode) {
        auto config = std::make_shared<Config>();
        
        // Set base configuration
        config->set("brain.neuron_count", 2000);
        config->set("brain.region_count", 2);
        
        // Configure based on behavior mode
        if (behavior_mode == "exploratory") {
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.curiosity.intensity", 0.8f);
            config->set("neuromod.novelty.sensitivity", 0.9f);
            config->set("plasticity.structural.enabled", true);
            config->set("development.critical_period", true);
        } else if (behavior_mode == "exploitive") {
            config->set("neuromod.dopamine.scale", 2.0f);
            config->set("neuromod.curiosity.enable", false);
            config->set("plasticity.reward_modulated.enable", true);
            config->set("development.critical_period", false);
        } else if (behavior_mode == "balanced") {
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("plasticity.structural.enabled", true);
            config->set("development.critical_period", true);
        }
        
        return config;
    }, py::arg("behavior_mode") = "balanced", "Create an agent-specific configuration for different behaviors");

    // Performance monitoring utilities
    m.def("monitor_performance", [](std::shared_ptr<Brain> brain, int steps, std::shared_ptr<SimpleWorld> world) {
        std::vector<float> firing_rates;
        std::vector<float> spike_counts;
        std::vector<float> neuromod_levels;
        std::vector<float> curiosity_levels;
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Record metrics
            firing_rates.push_back(brain->getAverageFiringRate());
            spike_counts.push_back(static_cast<float>(brain->getTotalSpikeCount()));
            neuromod_levels.push_back(agent->getNeuromodulationLevel());
            curiosity_levels.push_back(agent->getCuriosityLevel());
        }
        
        // Calculate statistics
        float avg_firing_rate = 0.0f, avg_spike_count = 0.0f;
        float avg_neuromod = 0.0f, avg_curiosity = 0.0f;
        
        for (float rate : firing_rates) avg_firing_rate += rate;
        for (float count : spike_counts) avg_spike_count += count;
        for (float neuromod : neuromod_levels) avg_neuromod += neuromod;
        for (float curiosity : curiosity_levels) avg_curiosity += curiosity;
        
        avg_firing_rate /= firing_rates.size();
        avg_spike_count /= spike_counts.size();
        avg_neuromod /= neuromod_levels.size();
        avg_curiosity /= curiosity_levels.size();
        
        return std::make_tuple(
            std::make_pair("avg_firing_rate", avg_firing_rate),
            std::make_pair("avg_spike_count", avg_spike_count),
            std::make_pair("avg_neuromod_level", avg_neuromod),
            std::make_pair("avg_curiosity_level", avg_curiosity)
        );
    }, py::arg("brain"), py::arg("steps"), py::arg("world"),
       "Monitor performance metrics over a simulation");

    // Error handling utilities
    m.def("safe_step", [](std::shared_ptr<Brain> brain, int step, std::shared_ptr<SimpleWorld> world) {
        try {
            // Validate brain state
            if (!brain) {
                throw std::runtime_error("Brain is null");
            }
            
            // Validate world state
            if (!world) {
                throw std::runtime_error("World is null");
            }
            
            // Update world
            world->update(0.1);
            
            // Get and process sensory input
            auto percept = world->getSensoryPercept();
            
            // Create agent and process
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            agent->processSensoryInput(percept);
            
            // Perform brain step
            brain->step(step);
            
            // Get and apply action
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Return success with metrics
            return std::make_pair(true, std::make_pair(brain->getTotalSpikeCount(), agent->getCuriosityLevel()));
            
        } catch (const std::exception& e) {
            // Return error information
            return std::make_pair(false, std::make_pair(step, std::string(e.what())));
        }
    }, py::arg("brain"), py::arg("step"), py::arg("world"),
       "Safely perform a simulation step with error handling");

    // Version and system information
    m.attr("version") = "0.1.0";
    m.attr("phase") = "6";
    m.attr("description") = "NLM Phase 6: Integrated Artificial Brain";

    m.def("get_system_info", []() {
        py::dict info;
        info["version"] = "0.1.0";
        info["phase"] = "6";
        info["description"] = "NLM Phase 6: Integrated Artificial Brain";
        info["python_module"] = "pynlm";
        info["build_type"] = "Release";
        info["compiler_support"] = std::vector<std::string>{"C++20", "GCC 10+", "Clang 12+", "MSVC 2019+"};
        info["features"] = std::vector<std::string>{
            "Real spiking neural computation",
            "Event-driven dynamics",
            "Memory systems (working, episodic, associative)",
            "Prediction system",
            "Cognitive functions (planning, attention, concept formation)",
            "Neuromodulation (dopamine, curiosity, novelty)",
            "Development (critical periods, maturation, aging)",
            "Plasticity (STDP, Hebbian, structural)",
            "Checkpoint/restoration",
            "Performance monitoring"
        };
        return info;
    }, "Get system and feature information");

    m.def("benchmark", [](std::shared_ptr<Brain> brain, int steps, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Measure performance
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict results;
        results["total_steps"] = steps;
        results["duration_ms"] = duration.count();
        results["steps_per_second"] = (steps * 1000.0) / duration.count();
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["avg_firing_rate"] = brain->getAverageFiringRate();
        results["final_curiosity"] = agent->getCuriosityLevel();
        results["neurons"] = brain->getTotalNeuronCount();
        results["synapses"] = brain->getTotalSynapseCount();
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("world"),
       "Benchmark simulation performance");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Python utility functions and advanced features
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, int steps, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_pair(brain->getTotalSpikeCount(), agent->getCuriosityLevel());
    }, py::arg("brain"), py::arg("steps"), py::arg("world"),
       "Run a complete simulation with brain and world");

    m.def("create_learning_config", [](int neuron_count, float connection_probability) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.synapse_density", connection_probability);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        return config;
    }, py::arg("neuron_count") = 1000, py::arg("connection_probability") = 0.1f,
       "Create a learning configuration with specified parameters");

    m.def("create_advanced_config", [](int neuron_count, int region_count, bool enable_all_features) {
        auto config = std::make_shared<Config>();
        
        // Basic configuration
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.region_count", region_count);
        config->set("brain.connection_probability", 0.1f);
        
        // Plasticity settings
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.0005f);
        config->set("plasticity.structural.enable", true);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Neuromodulation settings
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.prediction_error.scale", 0.5f);
        
        // Development settings
        config->set("development.enable", true);
        config->set("development.critical_period_end", 300.0f);
        
        // Memory system
        config->set("memory.working_capacity", neuron_count / 10);
        config->set("memory.episodic_max_episodes", 1000);
        
        // Performance
        config->set("simulation.timestep", 0.001);
        config->set("checkpoint.enable", true);
        
        if (enable_all_features) {
            config->set("cognition.planner.enabled", true);
            config->set("cognition.attention.enabled", true);
            config->set("cognition.concept_formation.enabled", true);
            config->set("prediction.enabled", true);
            config->set("visualization.enable", false); // Performance intensive
        }
        
        return config;
    }, py::arg("neuron_count") = 1000, py::arg("region_count") = 1, py::arg("enable_all_features") = true,
       "Create an advanced configuration with learning and development features");

    m.def("create_agent_config", [](const std::string& behavior_mode) {
        auto config = std::make_shared<Config>();
        
        // Set base configuration
        config->set("brain.neuron_count", 2000);
        config->set("brain.region_count", 2);
        
        // Configure based on behavior mode
        if (behavior_mode == "exploratory") {
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.curiosity.intensity", 0.8f);
            config->set("neuromod.novelty.sensitivity", 0.9f);
            config->set("plasticity.structural.enabled", true);
            config->set("development.critical_period", true);
        } else if (behavior_mode == "exploitive") {
            config->set("neuromod.dopamine.scale", 2.0f);
            config->set("neuromod.curiosity.enable", false);
            config->set("plasticity.reward_modulated.enable", true);
            config->set("development.critical_period", false);
        } else if (behavior_mode == "balanced") {
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("plasticity.structural.enabled", true);
            config->set("development.critical_period", true);
        }
        
        return config;
    }, py::arg("behavior_mode") = "balanced", "Create an agent-specific configuration for different behaviors");

    // Performance monitoring utilities
    m.def("monitor_performance", [](std::shared_ptr<Brain> brain, int steps, std::shared_ptr<SimpleWorld> world) {
        std::vector<float> firing_rates;
        std::vector<float> spike_counts;
        std::vector<float> neuromod_levels;
        std::vector<float> curiosity_levels;
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Record metrics
            firing_rates.push_back(brain->getAverageFiringRate());
            spike_counts.push_back(static_cast<float>(brain->getTotalSpikeCount()));
            neuromod_levels.push_back(agent->getNeuromodulationLevel());
            curiosity_levels.push_back(agent->getCuriosityLevel());
        }
        
        // Calculate statistics
        float avg_firing_rate = 0.0f, avg_spike_count = 0.0f;
        float avg_neuromod = 0.0f, avg_curiosity = 0.0f;
        
        for (float rate : firing_rates) avg_firing_rate += rate;
        for (float count : spike_counts) avg_spike_count += count;
        for (float neuromod : neuromod_levels) avg_neuromod += neuromod;
        for (float curiosity : curiosity_levels) avg_curiosity += curiosity;
        
        avg_firing_rate /= firing_rates.size();
        avg_spike_count /= spike_counts.size();
        avg_neuromod /= neuromod_levels.size();
        avg_curiosity /= curiosity_levels.size();
        
        return std::make_tuple(
            std::make_pair("avg_firing_rate", avg_firing_rate),
            std::make_pair("avg_spike_count", avg_spike_count),
            std::make_pair("avg_neuromod_level", avg_neuromod),
            std::make_pair("avg_curiosity_level", avg_curiosity)
        );
    }, py::arg("brain"), py::arg("steps"), py::arg("world"),
       "Monitor performance metrics over a simulation");

    // Error handling utilities
    m.def("safe_step", [](std::shared_ptr<Brain> brain, int step, std::shared_ptr<SimpleWorld> world) {
        try {
            // Validate brain state
            if (!brain) {
                throw std::runtime_error("Brain is null");
            }
            
            // Validate world state
            if (!world) {
                throw std::runtime_error("World is null");
            }
            
            // Update world
            world->update(0.1);
            
            // Get and process sensory input
            auto percept = world->getSensoryPercept();
            
            // Create agent and process
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            agent->processSensoryInput(percept);
            
            // Perform brain step
            brain->step(step);
            
            // Get and apply action
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Return success with metrics
            return std::make_pair(true, std::make_pair(brain->getTotalSpikeCount(), agent->getCuriosityLevel()));
            
        } catch (const std::exception& e) {
            // Return error information
            return std::make_pair(false, std::make_pair(step, std::string(e.what())));
        }
    }, py::arg("brain"), py::arg("step"), py::arg("world"),
       "Safely perform a simulation step with error handling");

    // Version and system information
    m.attr("version") = "0.1.0";
    m.attr("phase") = "6";
    m.attr("description") = "NLM Phase 6: Integrated Artificial Brain";

    m.def("get_system_info", []() {
        py::dict info;
        info["version"] = "0.1.0";
        info["phase"] = "6";
        info["description"] = "NLM Phase 6: Integrated Artificial Brain";
        info["python_module"] = "pynlm";
        info["build_type"] = "Release";
        info["compiler_support"] = std::vector<std::string>{"C++20", "GCC 10+", "Clang 12+", "MSVC 2019+"};
        info["features"] = std::vector<std::string>{
            "Real spiking neural computation",
            "Event-driven dynamics",
            "Memory systems (working, episodic, associative)",
            "Prediction system",
            "Cognitive functions (planning, attention, concept formation)",
            "Neuromodulation (dopamine, curiosity, novelty)",
            "Development (critical periods, maturation, aging)",
            "Plasticity (STDP, Hebbian, structural)",
            "Checkpoint/restoration",
            "Performance monitoring"
        };
        return info;
    }, "Get system and feature information");

    m.def("benchmark", [](std::shared_ptr<Brain> brain, int steps, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Measure performance
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict results;
        results["total_steps"] = steps;
        results["duration_ms"] = duration.count();
        results["steps_per_second"] = (steps * 1000.0) / duration.count();
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["avg_firing_rate"] = brain->getAverageFiringRate();
        results["final_curiosity"] = agent->getCuriosityLevel();
        results["neurons"] = brain->getTotalNeuronCount();
        results["synapses"] = brain->getTotalSynapseCount();
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("world"),
       "Benchmark simulation performance");

    // Configuration templates for common use cases
    m.def("get_exploratory_config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.intensity", 0.8f);
        config->set("neuromod.novelty.sensitivity", 0.9f);
        config->set("development.critical_period", true);
        config->set("plasticity.structural.enabled", true);
        return config;
    }, "Get a configuration optimized for exploratory behavior");

    m.def("get_stable_config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1f);
        config->set("neuromod.dopamine.scale", 2.0f);
        config->set("neuromod.curiosity.enable", false);
        config->set("development.critical_period", false);
        config->set("plasticity.reward_modulated.enable", true);
        return config;
    }, "Get a configuration optimized for stable behavior");

    m.def("get_balanced_config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1500);
        config->set("brain.synapse_density", 0.12f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("development.critical_period", true);
        config->set("plasticity.structural.enabled", true);
        config->set("plasticity.stdp.enable", true);
        return config;
    }, "Get a configuration optimized for balanced behavior");

    // Python-style convenience functions
    m.def("create_simple_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.05f);
        return std::make_shared<Brain>(config);
    }, "Create a simple brain with minimal configuration");

    m.def("create_agent_world_pair", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        return world;
    }, "Create and configure a world for agent simulation");

    m.def("simulate_step_by_step", 
          [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, int steps) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        py::list step_results;
        
        for (int step = 0; step < steps; ++step) {
            // Execute world update
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
            
            // Store step results
            py::dict step_result;
            step_result["step"] = step;
            step_result["spike_count"] = brain->getTotalSpikeCount();
            step_result["firing_rate"] = brain->getAverageFiringRate();
            step_result["curiosity"] = agent->getCuriosityLevel();
            step_result["action_type"] = static_cast<int>(action.getType());
            step_result["world_x"] = world->getAgentBody().x;
            step_result["world_y"] = world->getAgentBody().y;
            
            step_results.append(step_result);
        }
        
        return step_results;
    }, py::arg("brain"), py::arg("world"), py::arg("steps") = 100,
       "Simulate step by step and return detailed results");

    // Utility functions for working with simulation results
    m.def("find_step_with_action", 
          [](const py::list& steps, const std::string& action_type) {
        py::list matching_steps;
        
        for (auto item : steps) {
            py::dict step_result = item.cast<py::dict>();
            std::string step_action = step_result["action_type"].cast<std::string>();
            if (step_action == action_type) {
                matching_steps.append(item);
            }
        }
        
        return matching_steps;
    }, py::arg("steps"), py::arg("action_type") = "Wait",
       "Find simulation steps that produced a specific action");

    m.def("average_value", 
          [](const py::list& steps, const std::string& key) {
        double sum = 0.0;
        int count = 0;
        
        for (auto item : steps) {
            py::dict step_result = item.cast<py::dict>();
            sum += step_result[key].cast<double>();
            count++;
        }
        
        return (count > 0) ? sum / count : 0.0;
    }, py::arg("steps"), py::arg("key") = "firing_rate",
       "Calculate average value of a specific metric across steps");

    m.def("max_value", 
          [](const py::list& steps, const std::string& key) {
        double max_val = std::numeric_limits<double>::lowest();
        
        for (auto item : steps) {
            py::dict step_result = item.cast<py::dict>();
            double value = step_result[key].cast<double>();
            max_val = std::max(max_val, value);
        }
        
        return max_val;
    }, py::arg("steps"), py::arg("key") = "spike_count",
       "Find maximum value of a specific metric across steps");

    // Checkpoint utilities
    m.def("save_brain_checkpoint", 
          [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Save brain state to checkpoint file");

    m.def("load_brain_checkpoint", 
          [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Load brain state from checkpoint file");

    // Memory system utilities
    m.def("get_working_memory_size", 
          [](std::shared_ptr<Brain> brain) {
        if (brain->getWorkingMemory()) {
            return brain->getWorkingMemory()->getActiveTraces();
        }
        return 0;
    }, py::arg("brain"),
       "Get number of active working memory traces");

    m.def("get_episodic_memory_count", 
          [](std::shared_ptr<Brain> brain) {
        if (brain->getEpisodicMemory()) {
            return brain->getEpisodicMemory()->getEpisodeCount();
        }
        return 0;
    }, py::arg("brain"),
       "Get number of episodic memory episodes");

    // Development utilities
    m.def("get_development_stage", 
          [](std::shared_ptr<Brain> brain) {
        if (brain->getDevelopmentSystem()) {
            return static_cast<int>(brain->getDevelopmentSystem()->getCurrentStage());
        }
        return 0;
    }, py::arg("brain"),
       "Get current developmental stage (0=Initial, 1=CriticalPeriod, 2=Maturation, 3=Adult, 4=Aging)");

    m.def("get_plasticity_modifier", 
          [](std::shared_ptr<AgentBrain> agent) {
        return agent->getPlasticityModifier();
    }, py::arg("agent"),
       "Get current plasticity modifier (1.0=high plasticity, 0.2=stable)");

    m.def("get_developmental_age", 
          [](std::shared_ptr<AgentBrain> agent) {
        return agent->getDevelopmentalAge();
    }, py::arg("agent"),
       "Get current developmental age in time units");

    // Neuromodulation utilities
    m.def("get_dopamine_level", 
          [](std::shared_ptr<AgentBrain> agent) {
        return agent->getNeuromodulationLevel();
    }, py::arg("agent"),
       "Get current dopamine level (-1.0=-negative, 1.0=positive)");

    m.def("get_prediction_error", 
          [](std::shared_ptr<AgentBrain> agent) {
        return agent->getPredictionError();
    }, py::arg("agent"),
       "Get prediction error (difference between expected and actual reward)");

    m.def("get_novelty_level", 
          [](std::shared_ptr<AgentBrain> agent) {
        return agent->getNoveltyLevel();
    }, py::arg("agent"),
       "Get current novelty level (0.0=common, 1.0=new)");

    m.def("get_curiosity_level", 
          [](std::shared_ptr<AgentBrain> agent) {
        return agent->getCuriosityLevel();
    }, py::arg("agent"),
       "Get current curiosity level (0.0=inactive, 1.0=high)");

    // Advanced configuration helpers
    m.def("enable_all_learning_systems", 
          [](std::shared_ptr<Config> config) {
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.reward_modulated.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.prediction_error.scale", 0.5f);
        config->set("development.enable", true);
        config->set("cognition.planner.enabled", true);
        config->set("cognition.attention.enabled", true);
        config->set("cognition.concept_formation.enabled", true);
        config->set("prediction.enabled", true);
        return config;
    }, py::arg("config"),
       "Enable all learning and cognitive systems in a configuration");

    m.def("disable_all_learning_systems", 
          [](std::shared_ptr<Config> config) {
        config->set("plasticity.stdp.enable", false);
        config->set("plasticity.hebbian.enable", false);
        config->set("plasticity.structural.enable", false);
        config->set("plasticity.reward_modulated.enable", false);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("neuromod.prediction_error.scale", 0.0f);
        config->set("development.enable", false);
        config->set("cognition.planner.enabled", false);
        config->set("cognition.attention.enabled", false);
        config->set("cognition.concept_formation.enabled", false);
        config->set("prediction.enabled", false);
        return config;
    }, py::arg("config"),
       "Disable all learning and cognitive systems in a configuration");

    // Simulation monitoring and debugging
    m.def("watch_simulation", 
          [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, int steps) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        py::dict monitor;
        
        for (int step = 0; step < steps; ++step) {
            // Execute world update
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
            
            // Record state
            monitor["step_" + std::to_string(step)] = py::make_tuple(
                brain->getTotalSpikeCount(),
                brain->getAverageFiringRate(),
                agent->getCuriosityLevel(),
                static_cast<int>(action.getType()),
                world->getAgentBody().x,
                world->getAgentBody().y
            );
        }
        
        return monitor;
    }, py::arg("brain"), py::arg("world"), py::arg("steps") = 100,
       "Run simulation and return detailed state monitoring data");

    // Add type-safe getter methods with proper error handling
    m.def("get_config_int", [](std::shared_ptr<Config> config, const std::string& key, int default_value) {
        auto value = config->get<int>(key);
        return value.has_value() ? value.value() : default_value;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = 0,
       "Get integer value from config with default fallback");

    m.def("get_config_float", [](std::shared_ptr<Config> config, const std::string& key, float default_value) {
        auto value = config->get<float>(key);
        return value.has_value() ? value.value() : default_value;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = 0.0f,
       "Get float value from config with default fallback");

    m.def("get_config_bool", [](std::shared_ptr<Config> config, const std::string& key, bool default_value) {
        auto value = config->get<bool>(key);
        return value.has_value() ? value.value() : default_value;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = false,
       "Get boolean value from config with default fallback");

    m.def("get_config_string", [](std::shared_ptr<Config> config, const std::string& key, const std::string& default_value) {
        auto value = config->get<std::string>(key);
        return value.has_value() ? value.value() : default_value;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = "",
       "Get string value from config with default fallback");

    // Add validation functions for configuration
    m.def("validate_config", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        // Required keys check
        std::vector<std::string> required_keys = {
            "brain.neuron_count",
            "brain.synapse_density",
            "brain.connection_probability"
        };
        
        for (const auto& key : required_keys) {
            if (!config->has(key)) {
                errors.push_back("Missing required configuration key: " + key);
            }
        }
        
        // Value validation
        if (config->has("brain.neuron_count")) {
            auto neuron_count = config->get<size_t>("brain.neuron_count");
            if (neuron_count.has_value() && neuron_count.value() < 100) {
                errors.push_back("brain.neuron_count should be at least 100");
            }
        }
        
        if (config->has("brain.synapse_density")) {
            auto density = config->get<float>("brain.synapse_density");
            if (density.has_value() && (density.value() < 0.0f || density.value() > 1.0f)) {
                errors.push_back("brain.synapse_density should be between 0.0 and 1.0");
            }
        }
        
        return errors;
    }, py::arg("config"),
       "Validate configuration and return list of errors (empty if valid)");

    // Add comprehensive error checking for simulation parameters
    m.def("validate_simulation_params", [](int steps, double timestep) {
        std::vector<std::string> errors;
        
        if (steps <= 0) {
            errors.push_back("Steps must be positive (got " + std::to_string(steps) + ")");
        }
        
        if (steps > 1000000) {
            errors.push_back("Steps seems excessively large (got " + std::to_string(steps) + ")");
        }
        
        if (timestep <= 0.0) {
            errors.push_back("Timestep must be positive (got " + std::to_string(timestep) + ")");
        }
        
        if (timestep > 1.0) {
            errors.push_back("Timestep seems excessively large (got " + std::to_string(timestep) + ")");
        }
        
        return errors;
    }, py::arg("steps") = 1000, py::arg("timestep") = 0.1,
       "Validate simulation parameters and return list of errors (empty if valid)");

    // Add safer versions of brain methods with comprehensive error checking
    m.def("safe_step_brain", [](std::shared_ptr<Brain> brain, int step, double timestep) {
        try {
            if (!brain) {
                throw std::runtime_error("Brain is null");
            }
            
            if (step < 0) {
                throw std::runtime_error("Step must be non-negative (got " + std::to_string(step) + ")");
            }
            
            if (timestep <= 0.0) {
                throw std::runtime_error("Timestep must be positive (got " + std::to_string(timestep) + ")");
            }
            
            brain->step(step, timestep * step);
            return std::make_pair(true, std::string("Step " + std::to_string(step) + " completed successfully"));
            
        } catch (const std::exception& e) {
            return std::make_pair(false, std::string(e.what()));
        }
    }, py::arg("brain"), py::arg("step") = 0, py::arg("timestep") = 0.1,
       "Safely execute a brain step with comprehensive parameter validation");

    m.def("safe_receive_input", [](std::shared_ptr<Brain> brain, std::shared_ptr<SensoryInput> input) {
        try {
            if (!brain) {
                throw std::runtime_error("Brain is null");
            }
            
            if (!input) {
                throw std::runtime_error("Sensory input is null");
            }
            
            // Validate input data
            auto data = input->getData();
            if (data.empty()) {
                throw std::runtime_error("Input data is empty");
            }
            
            brain->receiveSensoryInput(*input);
            return std::make_pair(true, std::string("Input processed successfully"));
            
        } catch (const std::exception& e) {
            return std::make_pair(false, std::string(e.what()));
        }
    }, py::arg("brain"), py::arg("input"),
       "Safely process sensory input with validation");

    m.def("safe_apply_action", [](std::shared_ptr<SimpleWorld> world, std::shared_ptr<Action> action, double time) {
        try {
            if (!world) {
                throw std::runtime_error("World is null");
            }
            
            if (!action) {
                throw std::runtime_error("Action is null");
            }
            
            if (time < 0.0) {
                throw std::runtime_error("Time must be non-negative (got " + std::to_string(time) + ")");
            }
            
            auto result = world->applyMotorCommand(action->getType(), time);
            
            py::dict result_dict;
            result_dict["success"] = result.success;
            result_dict["reward"] = result.reward;
            result_dict["message"] = result.message;
            
            return std::make_pair(true, result_dict);
            
        } catch (const std::exception& e) {
            return std::make_pair(false, std::string(e.what()));
        }
    }, py::arg("world"), py::arg("action"), py::arg("time") = 0.0,
       "Safely apply motor command with comprehensive parameter validation");

    // Add type checking and conversion utilities
    m.def("create_sensory_from_vision", [](const py::list& vision_data, int width, int height) {
        if (vision_data.size() != static_cast<size_t>(width * height)) {
            throw std::runtime_error("Vision data size doesn't match dimensions (expected " + 
                                   std::to_string(width * height) + ", got " + 
                                   std::to_string(vision_data.size()) + ")");
        }
        
        std::vector<float> vision(width * height);
        for (size_t i = 0; i < vision.size(); ++i) {
            vision[i] = vision_data[i].cast<float>();
            
            // Clamp values to valid range
            if (vision[i] < 0.0f) vision[i] = 0.0f;
            if (vision[i] > 1.0f) vision[i] = 1.0f;
        }
        
        auto sensory = std::make_shared<Vision>();
        sensory->setData(vision);
        return sensory;
    }, py::arg("vision_data"), py::arg("width") = 16, py::arg("height") = 16,
       "Create Vision sensory input from Python list with validation");

    m.def("create_sensory_from_internal", [](const py::list& internal_data) {
        if (internal_data.size() != 4) {
            throw std::runtime_error("Internal signals must have exactly 4 values (got " + 
                                   std::to_string(internal_data.size()) + ")");
        }
        
        std::vector<float> internal(4);
        for (size_t i = 0; i < internal.size(); ++i) {
            internal[i] = internal_data[i].cast<float>();
            
            // Clamp values to reasonable ranges
            if (internal[i] < 0.0f) internal[i] = 0.0f;
            if (internal[i] > 1.0f) internal[i] = 1.0f;
        }
        
        auto sensory = std::make_shared<InternalSignals>();
        for (size_t i = 0; i < internal.size(); ++i) {
            sensory->addSignal(internal[i]);
        }
        
        return sensory;
    }, py::arg("internal_data"),
       "Create InternalSignals sensory input from Python list with validation");

    // Add configuration validation utilities
    m.def("validate_brain_config", [](std::shared_ptr<Config> config) {
        py::dict validation_result;
        validation_result["valid"] = true;
        validation_result["warnings"] = py::list();
        validation_result["errors"] = py::list();
        
        // Check for common configuration issues
        if (config->has("brain.neuron_count")) {
            auto neuron_count = config->get<size_t>("brain.neuron_count");
            if (neuron_count.has_value()) {
                size_t count = neuron_count.value();
                if (count < 100) {
                    validation_result["warnings"].attr("append")("Neuron count is low (<100), may affect learning performance");
                } else if (count > 10000) {
                    validation_result["warnings"].attr("append")("Neuron count is high (>10000), may impact performance");
                }
            }
        }
        
        if (config->has("plasticity.stdp.enable")) {
            auto stdp_enabled = config->get<bool>("plasticity.stdp.enable");
            if (stdp_enabled.has_value() && !stdp_enabled.value()) {
                validation_result["warnings"].attr("append")("STDP is disabled - learning may be less effective");
            }
        }
        
        if (config->has("neuromod.curiosity.enable")) {
            auto curiosity_enabled = config->get<bool>("neuromod.curiosity.enable");
            if (curiosity_enabled.has_value() && !curiosity_enabled.value()) {
                validation_result["warnings"].attr("append")("Curiosity is disabled - exploration motivation will be reduced");
            }
        }
        
        return validation_result;
    }, py::arg("config"),
       "Validate brain configuration and return validation result with warnings");

    // Add simulation monitoring and statistics
    m.def("simulate_with_monitoring", 
          [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
             int steps, double timestep) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        py::dict monitoring;
        monitoring["start_time"] = 0.0;
        monitoring["steps_completed"] = 0;
        monitoring["total_spikes"] = 0;
        monitoring["average_firing_rate"] = 0.0;
        monitoring["curiosity_levels"] = py::list();
        monitoring["action_types"] = py::list();
        monitoring["rewards"] = py::list();
        
        double current_time = 0.0;
        
        for (int step = 0; step < steps; ++step) {
            try {
                // Update world
                world->update(timestep);
                current_time += timestep;
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Run brain step
                brain->step(step, current_time);
                
                // Get action
                auto action = agent->decodeMotorCommand();
                
                // Apply action
                world->applyMotorCommand(action, current_time);
                
                // Record metrics
                monitoring["total_spikes"] = brain->getTotalSpikeCount();
                monitoring["average_firing_rate"] = brain->getAverageFiringRate();
                
                // Add to lists
                monitoring["curiosity_levels"].attr("append")(agent->getCuriosityLevel());
                monitoring["action_types"].attr("append")(static_cast<int>(action.getType()));
                monitoring["rewards"].attr("append")(action.getReward());
                
                monitoring["steps_completed"] = step + 1;
                
            } catch (const std::exception& e) {
                monitoring["error"] = std::string("Step " + std::to_string(step) + ": " + e.what());
                break;
            }
        }
        
        // Calculate final statistics
        double avg_curiosity = 0.0, sum_curiosity = 0.0;
        auto curiosity_list = monitoring["curiosity_levels"].cast<py::list>();
        if (!curiosity_list.empty()) {
            for (auto item : curiosity_list) {
                sum_curiosity += item.cast<float>();
            }
            avg_curiosity = sum_curiosity / curiosity_list.size();
        }
        
        monitoring["final_curiosity"] = avg_curiosity;
        
        return monitoring;
    }, py::arg("brain"), py::arg("world"), py::arg("steps") = 100, py::arg("timestep") = 0.1,
       "Run simulation with comprehensive monitoring and statistics collection");

    // Add utility for working with simulation checkpoints
    m.def("backup_simulation_state", 
          [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
             const std::string& prefix) {
        try {
            if (!brain) {
                throw std::runtime_error("Brain is null");
            }
            
            if (!world) {
                throw std::runtime_error("World is null");
            }
            
            if (prefix.empty()) {
                throw std::runtime_error("Backup prefix cannot be empty");
            }
            
            // Save brain state
            std::string brain_file = prefix + "_brain_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".bin";
            if (!brain->save(brain_file)) {
                throw std::runtime_error("Failed to save brain state to " + brain_file);
            }
            
            // Save world state (create world snapshot)
            std::string world_file = prefix + "_world_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json";
            // Note: SimpleWorld doesn't currently have a save method
            // This would need to be implemented in SimpleWorld.cpp
            
            py::dict backup_info;
            backup_info["brain_file"] = brain_file;
            backup_info["world_file"] = world_file;
            backup_info["brain_neurons"] = brain->getTotalNeuronCount();
            backup_info["brain_synapses"] = brain->getTotalSynapseCount();
            backup_info["world_width"] = world->getWidth();
            backup_info["world_height"] = world->getHeight();
            
            return backup_info;
            
        } catch (const std::exception& e) {
            return std::make_pair(false, std::string(e.what()));
        }
    }, py::arg("brain"), py::arg("world"), py::arg("prefix"),
       "Backup simulation state to files");

    // Add comprehensive error recovery utilities
    m.def("recover_from_error", 
          [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
             int max_recovery_attempts = 3) {
        try {
            if (!brain || !world) {
                throw std::runtime_error("Brain or world is null");
            }
            
            py::dict recovery_result;
            recovery_result["success"] = false;
            recovery_result["attempts_made"] = 0;
            recovery_result["final_spike_count"] = 0;
            recovery_result["error_message"] = "";
            
            for (int attempt = 0; attempt < max_recovery_attempts; ++attempt) {
                recovery_result["attempts_made"] = attempt + 1;
                
                try {
                    // Reset brain to clean state
                    brain->reset();
                    
                    // Reset world
                    world->reset();
                    
                    // Re-initialize components
                    auto agent = std::make_shared<AgentBrain>(brain);
                    agent->initialize(*world);
                    
                    // Validate state
                    if (brain->getTotalNeuronCount() == 0) {
                        throw std::runtime_error("Brain initialization failed - zero neurons");
                    }
                    
                    if (!world->isValidPosition(world->getAgentBody().x, world->getAgentBody().y)) {
                        throw std::runtime_error("World initialization failed - invalid agent position");
                    }
                    
                    recovery_result["success"] = true;
                    recovery_result["final_spike_count"] = brain->getTotalSpikeCount();
                    recovery_result["error_message"] = "Recovery successful";
                    return recovery_result;
                    
                } catch (const std::exception& e) {
                    recovery_result["error_message"] = std::string("Attempt " + std::to_string(attempt + 1) + ": " + e.what());
                    
                    // Small delay before retry
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            
            return recovery_result;
            
        } catch (const std::exception& e) {
            py::dict result;
            result["success"] = false;
            result["attempts_made"] = 0;
            result["final_spike_count"] = 0;
            result["error_message"] = std::string("Recovery setup failed: " + std::string(e.what()));
            return result;
        }
    }, py::arg("brain"), py::arg("world"), py::arg("max_recovery_attempts") = 3,
       "Attempt to recover simulation from error state");

    // Add performance profiling utilities
    m.def("profile_performance", 
          [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
             int steps, double timestep, const std::string& profile_type = "detailed") {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        py::dict profile_results;
        profile_results["total_steps"] = steps;
        profile_results["timestep"] = timestep;
        profile_results["start_time"] = 0.0;
        profile_results["end_time"] = 0.0;
        profile_results["total_duration_ms"] = 0.0;
        profile_results["steps_per_second"] = 0.0;
        profile_results["peak_firing_rate"] = 0.0;
        profile_results["average_firing_rate"] = 0.0;
        profile_results["total_spikes"] = 0;
        profile_results["final_curiosity"] = 0.0;
        profile_results["memory_usage"] = py::dict();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        double max_firing_rate = 0.0;
        double sum_firing_rate = 0.0;
        int step_count = 0;
        
        for (int step = 0; step < steps; ++step) {
            auto step_start = std::chrono::high_resolution_clock::now();
            
            try {
                // Execute simulation step
                world->update(timestep);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                brain->step(step);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Record performance metrics
                double firing_rate = brain->getAverageFiringRate();
                sum_firing_rate += firing_rate;
                max_firing_rate = std::max(max_firing_rate, firing_rate);
                step_count++;
                
                auto step_end = std::chrono::high_resolution_clock::now();
                auto step_duration = std::chrono::duration_cast<std::chrono::microseconds>(step_end - step_start);
                
                // Store detailed step information if requested
                if (profile_type == "detailed") {
                    // Store step timing
                    if (!profile_results["step_times"].is_none()) {
                        auto step_times = profile_results["step_times"].cast<py::list>();
                        step_times.append(static_cast<double>(step_duration.count()) / 1000.0);  // Convert to ms
                        profile_results["step_times"] = step_times;
                    } else {
                        py::list step_times;
                        step_times.append(static_cast<double>(step_duration.count()) / 1000.0);
                        profile_results["step_times"] = step_times;
                    }
                }
                
            } catch (const std::exception& e) {
                profile_results["error_at_step"] = step;
                profile_results["error_message"] = std::string(e.what());
                break;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        profile_results["end_time"] = total_duration.count() / 1000.0;
        profile_results["total_duration_ms"] = total_duration.count();
        profile_results["steps_per_second"] = (step_count * 1000.0) / total_duration.count();
        profile_results["peak_firing_rate"] = max_firing_rate;
        profile_results["average_firing_rate"] = (step_count > 0) ? (sum_firing_rate / step_count) : 0.0;
        profile_results["total_spikes"] = brain->getTotalSpikeCount();
        profile_results["final_curiosity"] = agent->getCuriosityLevel();
        
        // Memory usage estimation
        profile_results["memory_usage"]["neurons"] = brain->getTotalNeuronCount();
        profile_results["memory_usage"]["synapses"] = brain->getTotalSynapseCount();
        
        return profile_results;
    }, py::arg("brain"), py::arg("world"), py::arg("steps") = 100, 
          py::arg("timestep") = 0.1, py::arg("profile_type") = "detailed",
       "Profile simulation performance with timing and metrics collection");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
