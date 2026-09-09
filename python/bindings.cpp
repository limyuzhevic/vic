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

    // Add loadFromArgs wrapper for command line configuration
    m.def("loadConfigFromArgs", [](std::shared_ptr<Config> config, int argc, char** argv) {
        return config->loadFromArgs(argc, argv);
    }, py::arg("config"), py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // Enhanced configuration creation with environment-specific presets
    m.def("createConfigForEnvironment", [](const std::string& environment, const std::string& mode) {
        auto config = std::make_shared<Config>();
        
        if (environment == "gridworld" || environment == "GridWorld") {
            config->set("environment_name", std::string("GridWorld"));
            config->set("environment_width", 20);
            config->set("environment_height", 20);
            config->set("connection_probability", 0.05f);
        } else if (environment == "maze") {
            config->set("environment_name", std::string("Maze"));
            config->set("environment_width", 30);
            config->set("environment_height", 30);
            config->set("connection_probability", 0.03f);
        }
        
        if (mode == "exploration") {
            config->set("curiosity_enabled", true);
            config->set("novelty_detection", 0.8f);
            config->set("max_simulation_steps", static_cast<int64_t>(50000));
        } else if (mode == "memory") {
            config->set("episodic_memory_max_episodes", static_cast<int64_t>(5000));
            config->set("working_memory_capacity", static_cast<int64_t>(2000));
            config->set("max_simulation_steps", static_cast<int64_t>(100000));
        } else if (mode == "learning") {
            config->set("reward_modulation_enabled", true);
            config->set("plasticity_learning_rate", 0.05f);
            config->set("max_simulation_steps", static_cast<int64_t>(20000));
        }
        
        // Set core parameters
        config->set("random_seed", int64_t(42 + std::hash<std::string>{}(environment) % 1000));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(2000));
        config->set("region_count", static_cast<int64_t>(3));
        
        return config;
    }, py::arg("environment"), py::arg("mode"), 
         R"pbdoc(
        Create a configuration optimized for a specific environment and mode.
        
        Args:
            environment: Environment type ("gridworld", "maze", etc.)
            mode: Operating mode ("exploration", "memory", "learning", "default")
        
        Returns:
            Config: Pre-configured NLM configuration
        
        This provides preset configurations for different use cases, adjusting
        network size, connectivity, and system parameters based on the target
        environment and desired mode of operation.
    )pbdoc");

    // Quick setup for common experiment scenarios
    m.def("quickSetupExploration", []() {
        auto config = std::make_shared<Config>();
        
        // Exploration-focused settings
        config->set("random_seed", int64_t(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(1500));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.08f);
        
        // Enable exploration features
        config->set("curiosity_enabled", true);
        config->set("novelty_detection", 0.7f);
        config->set("exploration_rate", 0.3f);
        config->set("reward_modulation_enabled", true);
        config->set("structural_plasticity_enabled", true);
        config->set("development_enabled", true);
        
        // Set up world for exploration
        config->set("environment_name", std::string("GridWorld"));
        config->set("environment_width", 15);
        config->set("environment_height", 15);
        config->set("vision_width", 8);
        config->set("vision_height", 8);
        
        // Configure for long exploration
        config->set("max_simulation_steps", static_cast<int64_t>(100000));
        config->set("simulation_time_limit", 0.0);
        
        return std::make_shared<Brain>(config);
    }, R"pbdoc(
        Quick setup for exploration experiments.
        
        Returns:
            Brain: A pre-configured NLM brain ready for exploration experiments.
        
        This is a convenience function that creates a complete, ready-to-use
        NLM brain configured for exploration and discovery behavior, including
        curiosity-driven exploration and reward-based learning.
    )pbdoc");

    m.def("quickSetupMemory", []() {
        auto config = std::make_shared<Config>();
        
        // Memory-focused settings
        config->set("random_seed", int64_t(123));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(3000));
        config->set("region_count", static_cast<int64_t>(4));
        config->set("connection_probability", 0.06f);
        
        // Enable memory features
        config->set("episodic_memory_max_episodes", static_cast<int64_t>(3000));
        config->set("working_memory_capacity", static_cast<int64_t>(1500));
        config->set("associative_memory_enabled", true);
        config->set("reward_modulation_enabled", true);
        config->set("plasticity_learning_rate", 0.03f);
        
        // Configure environment
        config->set("environment_name", std::string("GridWorld"));
        config->set("environment_width", 25);
        config->set("environment_height", 25);
        config->set("vision_width", 10);
        config->set("vision_height", 10);
        
        // Configure for memory-intensive experiments
        config->set("max_simulation_steps", static_cast<int64_t>(200000));
        config->set("consolidation_interval", static_cast<int64_t>(2000));
        
        return std::make_shared<Brain>(config);
    }, R"pbdoc(
        Quick setup for memory experiments.
        
        Returns:
            Brain: A pre-configured NLM brain ready for memory experiments.
        
        This creates a brain optimized for episodic and working memory experiments,
        with larger network capacity and memory system configuration for
        studying memory formation, consolidation, and retrieval.
    )pbdoc");

    m.def("quickSetupLearning", []() {
        auto config = std::make_shared<Config>();
        
        // Learning-focused settings
        config->set("random_seed", int64_t(456));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(2000));
        config->set("region_count", static_cast<int64_t>(3));
        config->set("connection_probability", 0.12f);
        
        // Enable learning features
        config->set("reward_modulation_enabled", true);
        config->set("structural_plasticity_enabled", true);
        config->set("plasticity_learning_rate", 0.08f);
        config->set("stdp_ltp_weight", 0.03f);
        config->set("stdp_ltd_weight", 0.028f);
        config->set("development_enabled", true);
        config->set("curiosity_enabled", true);
        
        // Configure for RL-style learning
        config->set("environment_name", std::string("GridWorld"));
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 6);
        config->set("vision_height", 6);
        
        // Configure for intensive learning
        config->set("max_simulation_steps", static_cast<int64_t>(50000));
        config->set("reward_discount_factor", 0.95f);
        
        return std::make_shared<Brain>(config);
    }, R"pbdoc(
        Quick setup for reinforcement learning experiments.
        
        Returns:
            Brain: A pre-configured NLM brain ready for reinforcement learning.
        
        This creates a brain optimized for reward-based learning and
        reinforcement learning experiments, with strong plasticity and
        neuromodulation for value-based learning.
    )pbdoc");

    // Batch simulation utilities
    m.def("runMultipleSimulations", [](const std::vector<std::shared_ptr<Config>>& configs, 
                                       const std::vector<int>& stepsPerConfig,
                                       bool runInParallel = false) {
        std::vector<std::shared_ptr<Brain>> brains;
        std::vector<std::vector<double>> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            auto brain = std::make_shared<Brain>(configs[i]);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain for simulation " + std::to_string(i));
            }
            brains.push_back(brain);
            
            // Run simulation
            for (int step = 0; step < stepsPerConfig[i]; ++step) {
                brain->step(step);
            }
            
            // Collect results
            std::vector<double> simulationResults;
            simulationResults.push_back(static_cast<double>(brain->getTotalSpikeCount()));
            simulationResults.push_back(static_cast<double>(brain->getAverageFiringRate()));
            
            if (auto episodicMemory = brain->getEpisodicMemory()) {
                simulationResults.push_back(static_cast<double>(episodicMemory->getEpisodeCount()));
            }
            
            results.push_back(simulationResults);
        }
        
        return std::make_tuple(brains, results);
    }, py::arg("configs"), py::arg("stepsPerConfig"), py::arg("runInParallel"), 
         R"pbdoc(
        Run multiple simulations with different configurations.
        
        Args:
            configs: List of configuration objects
            stepsPerConfig: Number of steps for each simulation
            runInParallel: Whether to run simulations in parallel (if supported)
        
        Returns:
            tuple: (brains, results) where brains is list of brain objects and
                   results contains performance metrics for each simulation
        
        This utility is useful for running comparative experiments or
        parameter sweeps across different configuration settings.
    )pbdoc");

    // Create default config with all standard parameters initialized
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set all standard configuration parameters with proper defaults
        config->set("random_seed", int64_t(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_time_constant", 20.0f);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", 0.001f);
        config->set("pruning_rate", 0.0001f);
        
        // Neuromodulation
        config->set("dopamine_baseline", 0.1f);
        config->set("reward_discount_factor", 0.99f);
        
        // Environment
        config->set("environment_name", std::string("GridWorld"));
        config->set("environment_width", 10);
        config->set("environment_height", 10);
        
        // Logging
        config->set("log_level", std::string("INFO"));
        config->set("log_to_file", false);
        config->set("log_filename", std::string("nlm.log"));
        
        // Simulation
        config->set("max_simulation_steps", static_cast<int64_t>(10000));
        config->set("simulation_time_limit", 0.0);
        
        // Visualization
        config->set("visualization_enabled", false);
        config->set("visualization_update_rate", 30.0f);
        
        return config;
    }, R"pbdoc(
        Create a default NLM configuration with all standard parameters initialized.
        
        Returns:
            Config: A configuration object with sensible defaults for Phase 2 simulation.
        
        The configuration includes:
        - 1000 neurons across 2 regions
        - Standard STDP parameters for learning
        - Structural plasticity settings
        - Neuromodulation baseline
        - Environment parameters for GridWorld
        - Logging configuration
    )pbdoc");
