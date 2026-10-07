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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Configuration presets for common use cases
    m.def("createSmallConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500);
        config->set("brain.region_count", 2);
        config->set("brain.connection_probability", 0.1f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        return config;
    }, "Create a small configuration suitable for quick testing");

    m.def("createMediumConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000);
        config->set("brain.region_count", 4);
        config->set("brain.connection_probability", 0.05f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.prediction_error.enable", true);
        config->set("development.enable", true);
        return config;
    }, "Create a medium configuration for balanced simulation");

    m.def("createLargeConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 10000);
        config->set("brain.region_count", 8);
        config->set("brain.connection_probability", 0.02f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.5f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.prediction_error.enable", true);
        config->set("development.enable", true);
        config->set("development.critical_period", true);
        return config;
    }, "Create a large configuration for advanced simulations");

    // Convenience methods for agent-world interaction
    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                    std::shared_ptr<AgentBrain> agent, int num_steps) {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        if (!agent->initialize(*world)) {
            throw std::runtime_error("Failed to initialize agent");
        }
        
        for (int step = 0; step < num_steps; ++step) {
            // Get sensory input from world
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process sensory input in brain
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Get motor command from brain
            MotorCommand motor_cmd = agent->decodeMotorCommand();
            
            // Apply motor command to world
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(), 
                              brain->getAverageFiringRate(),
                              agent->getCuriosityLevel());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       "Run a complete agent simulation with brain and world interaction");

    // Helper function for creating typical agent configurations
    m.def("create_learning_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable typical learning subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"),
       "Create a learning-enabled agent brain for world interaction");

    // Brain configuration presets
    m.def("configure_for_reproduction", [](std::shared_ptr<Config> config) {
        config->set("plasticity.stdp.ltp_weight", 0.01f);
        config->set("plasticity.stdp.ltd_weight", 0.012f);
        config->set("plasticity.stdp.tau", 20.0f);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", false); // Focus on reproduction
        config->set("development.enable", true);
        config->set("development.critical_period", true);
        return config;
    }, py::arg("config"),
       "Configure for reproduction-focused learning");

    m.def("configure_for_exploration", [](std::shared_ptr<Config> config) {
        config->set("plasticity.stdp.ltp_weight", 0.005f);
        config->set("plasticity.stdp.ltd_weight", 0.015f);
        config->set("plasticity.stdp.tau", 30.0f);
        config->set("neuromod.dopamine.scale", 0.5f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("development.enable", true);
        config->set("development.critical_period", true);
        config->set("development.maturation_rate", 0.8f);
        return config;
    }, py::arg("config"),
       "Configure for exploration and curiosity-driven learning");

    m.def("configure_for_skill_formation", [](std::shared_ptr<Config> config) {
        config->set("brain.neuron_count", 3000);
        config->set("plasticity.stdp.ltp_weight", 0.002f);
        config->set("plasticity.stdp.ltd_weight", 0.003f);
        config->set("plasticity.stdp.tau", 50.0f);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 2.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.prediction_error.enable", true);
        config->set("development.enable", true);
        config->set("development.pruning.rate", 0.00005f);
        config->set("development.maturation_rate", 0.6f);
        return config;
    }, py::arg("config"),
       "Configure for skill formation and procedural memory");

    // Status and debugging helpers
    m.def("brain_status", [](std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Brain Status:" << std::endl;
        ss << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        ss << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        ss << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        ss << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        return ss.str();
    }, py::arg("brain"),
       "Get a formatted status string for the brain");

    m.def("world_status", [](std::shared_ptr<SimpleWorld> world) {
        std::stringstream ss;
        ss << "World Status:" << std::endl;
        ss << "  Dimensions: " << world->getWidth() << "x" << world->getHeight() << std::endl;
        ss << "  Simulation time: " << world->getSimulationTime() << std::endl;
        ss << "  Max energy: " << world->getMaxEnergy() << std::endl;
        ss << "  Energy decay rate: " << world->getEnergyDecayRate() << std::endl;
        ss << "  Random seed: " << world->getRandomSeed() << std::endl;
        auto percept = world->getSensoryPercept();
        ss << "  Vision: " << percept.getVisionWidth() << "x" << percept.getVisionHeight() << std::endl;
        ss << "  Touch sensors: " << percept.getTouch().size() << std::endl;
        ss << "  Internal signals: " << percept.getInternal().size() << std::endl;
        return ss.str();
    }, py::arg("world"),
       "Get a formatted status string for the world");

    m.def("agent_status", [](std::shared_ptr<AgentBrain> agent) {
        std::stringstream ss;
        ss << "Agent Status:" << std::endl;
        ss << "  Curiosity level: " << agent->getCuriosityLevel() << std::endl;
        ss << "  Novelty level: " << agent->getNoveltyLevel() << std::endl;
        ss << "  Prediction error: " << agent->getPredictionError() << std::endl;
        ss << "  Neuromodulation level: " << agent->getNeuromodulationLevel() << std::endl;
        ss << "  Developmental stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        ss << "  Reward modulation enabled: " << agent->isRewardModulationEnabled() << std::endl;
        ss << "  Structural plasticity enabled: " << agent->isStructuralPlasticityEnabled() << std::endl;
        ss << "  Development enabled: " << agent->isDevelopmentEnabled() << std::endl;
        ss << "  Curiosity enabled: " << agent->isCuriosityEnabled() << std::endl;
        return ss.str();
    }, py::arg("agent"),
       "Get a formatted status string for the agent");

    // Diagnostic and troubleshooting helpers
    m.def("validate_configuration", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config->has("brain.neuron_count")) {
            errors.push_back("Missing 'brain.neuron_count' configuration");
        } else {
            auto neuron_count = config->getOr<size_t>("brain.neuron_count", 0);
            if (neuron_count == 0) {
                errors.push_back("brain.neuron_count must be greater than 0");
            }
        }
        
        if (config->has("brain.connection_probability")) {
            auto prob = config->getOr<float>("brain.connection_probability", 0.0f);
            if (prob < 0.0f || prob > 1.0f) {
                errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
            }
        }
        
        if (!errors.empty()) {
            throw std::runtime_error("Configuration validation failed:\n" + 
                                   std::accumulate(errors.begin(), errors.end(), std::string(),
                                                 [](const std::string& a, const std::string& b) {
                                                     return a + "\n  - " + b;
                                                 }));
        }
        
        return "Configuration is valid";
    }, py::arg("config"),
       "Validate configuration and throw error if invalid");

    m.def("configure_for_experiment", [](const std::string& experiment_name, std::shared_ptr<Config> config) {
        if (experiment_name == "reproduction") {
            configure_for_reproduction(config);
        } else if (experiment_name == "exploration") {
            configure_for_exploration(config);
        } else if (experiment_name == "skill_formation") {
            configure_for_skill_formation(config);
        } else {
            throw std::runtime_error("Unknown experiment type: " + experiment_name);
        }
        return config;
    }, py::arg("experiment_name"), py::arg("config"),
       "Configure for a specific experiment type");

    // Enhanced configuration validation and diagnostic tools
    m.def("validate_configuration", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config->has("brain.neuron_count")) {
            errors.push_back("Missing 'brain.neuron_count' configuration");
        } else {
            auto neuron_count = config->getOr<size_t>("brain.neuron_count", 0);
            if (neuron_count == 0) {
                errors.push_back("brain.neuron_count must be greater than 0");
            } else if (neuron_count > 1000000) {
                errors.push_back("brain.neuron_count is extremely large, may cause performance issues");
            }
        }
        
        if (config->has("brain.connection_probability")) {
            auto prob = config->getOr<float>("brain.connection_probability", 0.0f);
            if (prob < 0.0f || prob > 1.0f) {
                errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
            }
        }
        
        if (config->has("plasticity.stdp.ltp_weight")) {
            auto ltp = config->getOr<float>("plasticity.stdp.ltp_weight", 0.0f);
            if (ltp < 0.0f || ltp > 1.0f) {
                errors.push_back("plasticity.stdp.ltp_weight must be between 0.0 and 1.0");
            }
        }
        
        if (config->has("plasticity.stdp.ltd_weight")) {
            auto ltd = config->getOr<float>("plasticity.stdp.ltd_weight", 0.0f);
            if (ltd < 0.0f || ltd > 1.0f) {
                errors.push_back("plasticity.stdp.ltd_weight must be between 0.0 and 1.0");
            }
        }
        
        if (!errors.empty()) {
            throw std::runtime_error("Configuration validation failed:\n" + 
                                   std::accumulate(errors.begin(), errors.end(), std::string(),
                                                 [](const std::string& a, const std::string& b) {
                                                     return a + "\n  - " + b;
                                                 }));
        }
        
        return "Configuration is valid";
    }, py::arg("config"),
       "Validate configuration and throw error if invalid");

    m.def("analyze_configuration", [](std::shared_ptr<Config> config) {
        std::stringstream ss;
        ss << "Configuration Analysis:" << std::endl;
        
        // Brain parameters
        ss << "Brain:" << std::endl;
        ss << "  Neuron count: " << (config->has("brain.neuron_count") ? 
                                  std::to_string(config->getOr<size_t>("brain.neuron_count", 0)) : "not set") << std::endl;
        ss << "  Region count: " << (config->has("brain.region_count") ? 
                                  std::to_string(config->getOr<size_t>("brain.region_count", 0)) : "not set") << std::endl;
        ss << "  Connection probability: " << (config->has("brain.connection_probability") ? 
                                          std::to_string(config->getOr<float>("brain.connection_probability", 0.0f)) : "not set") << std::endl;
        
        // Plasticity
        ss << "Plasticity:" << std::endl;
        ss << "  STDP enabled: " << (config->has("plasticity.stdp.enable") && 
                                   config->getOr<bool>("plasticity.stdp.enable", false) ? "yes" : "no") << std::endl;
        ss << "  Hebbian enabled: " << (config->has("plasticity.hebbian.enable") && 
                                     config->getOr<bool>("plasticity.hebbian.enable", false) ? "yes" : "no") << std::endl;
        ss << "  Structural plasticity enabled: " << (config->has("plasticity.structural.enable") && 
                                                 config->getOr<bool>("plasticity.structural.enable", false) ? "yes" : "no") << std::endl;
        
        // Neuromodulation
        ss << "Neuromodulation:" << std::endl;
        ss << "  Dopamine scale: " << (config->has("neuromod.dopamine.scale") ? 
                                    std::to_string(config->getOr<float>("neuromod.dopamine.scale", 1.0f)) : "not set") << std::endl;
        ss << "  Curiosity enabled: " << (config->has("neuromod.curiosity.enable") && 
                                         config->getOr<bool>("neuromod.curiosity.enable", false) ? "yes" : "no") << std::endl;
        ss << "  Novelty enabled: " << (config->has("neuromod.novelty.enable") && 
                                      config->getOr<bool>("neuromod.novelty.enable", false) ? "yes" : "no") << std::endl;
        
        // Development
        ss << "Development:" << std::endl;
        ss << "  Enabled: " << (config->has("development.enable") && 
                              config->getOr<bool>("development.enable", false) ? "yes" : "no") << std::endl;
        ss << "  Critical period: " << (config->has("development.critical_period") && 
                                      config->getOr<bool>("development.critical_period", false) ? "yes" : "no") << std::endl;
        
        // Simulation
        ss << "Simulation:" << std::endl;
        ss << "  Timestep: " << (config->has("simulation_timestep") ? 
                               std::to_string(config->getOr<double>("simulation_timestep", 0.001)) : "not set") << "s" << std::endl;
        ss << "  Random seed: " << (config->has("random_seed") ? 
                                  std::to_string(config->getOr<int64_t>("random_seed", 42)) : "not set") << std::endl;
        
        // System status
        ss << "System:" << std::endl;
        ss << "  Keys count: " << config->getKeys().size() << std::endl;
        ss << "  Default keys: " << (config->has("brain.neuron_count") && config->has("brain.region_count") ? "yes" : "no") << std::endl;
        
        return ss.str();
    }, py::arg("config"),
       "Analyze and report configuration details");

    m.def("optimize_configuration", [](std::shared_ptr<Config> config, const std::string& target) {
        // Apply optimization based on target
        if (target == "performance") {
            // Optimize for performance - moderate size, fewer connections
            config->set("brain.neuron_count", 2000);
            config->set("brain.region_count", 4);
            config->set("brain.connection_probability", 0.05f);
            config->set("plasticity.structural.enable", false); // Disable structural for performance
            config->set("development.enable", false); // Disable development for stability
        } else if (target == "learning") {
            // Optimize for learning - more connections, structural plasticity
            config->set("brain.neuron_count", 5000);
            config->set("brain.region_count", 8);
            config->set("brain.connection_probability", 0.02f);
            config->set("plasticity.structural.enable", true);
            config->set("development.enable", true);
            config->set("development.critical_period", true);
        } else if (target == "stability") {
            // Optimize for stability - more neurons, lower learning rates
            config->set("brain.neuron_count", 3000);
            config->set("brain.region_count", 6);
            config->set("brain.connection_probability", 0.03f);
            config->set("plasticity.stdp.ltp_weight", 0.001f);
            config->set("plasticity.stdp.ltd_weight", 0.002f);
            config->set("development.enable", false);
        }
        
        return config;
    }, py::arg("config"), py::arg("target"),
       "Optimize configuration for a target (performance, learning, or stability)");

    m.def("batch_configure", [](const std::vector<std::pair<std::string, std::string>>& settings) {
        auto config = std::make_shared<Config>();
        
        for (const auto& setting : settings) {
            config->set(setting.first, setting.second);
        }
        
        return config;
    }, py::arg("settings"),
       "Batch configure settings from key-value pairs");

    m.def("import_config_from_dict", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            
            // Try to parse as different types
            try {
                std::string str_value = py::cast<std::string>(item.second);
                config->set(key, str_value);
            } catch (...) {
                try {
                    int int_value = py::cast<int>(item.second);
                    config->set(key, int_value);
                } catch (...) {
                    try {
                        double float_value = py::cast<double>(item.second);
                        config->set(key, float_value);
                    } catch (...) {
                        try {
                            bool bool_value = py::cast<bool>(item.second);
                            config->set(key, bool_value);
                        } catch (...) {
                            // If all conversions fail, treat as string
                            config->set(key, py::cast<std::string>(item.second));
                        }
                    }
                }
            }
        }
        
        return config;
    }, py::arg("config_dict"),
       "Import configuration from Python dictionary");

    m.def("export_config_to_dict", [](std::shared_ptr<Config> config) {
        py::dict config_dict;
        
        for (const auto& key : config->getKeys()) {
            try {
                config_dict[key] = config->getOr<std::string>(key, "");
            } catch (...) {
                try {
                    config_dict[key] = config->getOr<int>(key, 0);
                } catch (...) {
                    try {
                        config_dict[key] = config->getOr<double>(key, 0.0);
                    } catch (...) {
                        try {
                            config_dict[key] = config->getOr<bool>(key, false);
                        } catch (...) {
                            // Keep as-is if all conversions fail
                            config_dict[key] = py::cast<std::string>(key);
                        }
                    }
                }
            }
        }
        
        return config_dict;
    }, py::arg("config"),
       "Export configuration to Python dictionary");

    // Error handling improvements
    m.def("safe_brain_step", [](std::shared_ptr<Brain> brain, int step) {
        try {
            brain->step(step);
            return true;
        } catch (const std::exception& e) {
            Logger::getGlobal().error(std::string("Brain step error: ") + e.what());
            return false;
        }
    }, py::arg("brain"), py::arg("step"),
       "Safely perform a brain step with error handling");

    m.def("safe_world_update", [](std::shared_ptr<SimpleWorld> world, double timestep) {
        try {
            world->update(timestep);
            return true;
        } catch (const std::exception& e) {
            Logger::getGlobal().error(std::string("World update error: ") + e.what());
            return false;
        }
    }, py::arg("world"), py::arg("timestep"),
       "Safely update world with error handling");

    m.def("safe_agent_process", [](std::shared_ptr<AgentBrain> agent, const SensoryPercept& percept) {
        try {
            agent->processSensoryInput(percept);
            return true;
        } catch (const std::exception& e) {
            Logger::getGlobal().error(std::string("Agent process error: ") + e.what());
            return false;
        }
    }, py::arg("agent"), py::arg("percept"),
       "Safely process sensory input with error handling");

    // Performance and logging improvements
    m.def("enable_detailed_logging", [](bool enable_console, bool enable_file) {
        auto& logger = Logger::getGlobal();
        logger.setLevel(enable_console ? LogLevel::Info : LogLevel::Error);
        
        if (enable_file) {
            // This would require more implementation in the Logger class
            // For now, just enable console logging
            logger.setLevel(LogLevel::Info);
        }
        
        return "Detailed logging enabled";
    }, py::arg("enable_console") = true, py::arg("enable_file") = false,
       "Enable detailed logging for debugging");

    m.def("get_logging_level", []() {
        return static_cast<int>(Logger::getGlobal().getLevel());
    }, "Get current logging level");

    m.def("set_logging_level", [](int level) {
        Logger::getGlobal().setLevel(static_cast<LogLevel>(level));
    }, py::arg("level"), "Set logging level (0=Debug, 1=Info, 2=Warning, 3=Error, 4=Critical)");

    // Performance monitoring helpers
    m.def("monitor_performance", [](std::shared_ptr<Brain> brain, int duration_steps) {
        std::vector<double> firing_rates;
        std::vector<size_t> spike_counts;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < duration_steps; ++step) {
            brain->step(step);
            firing_rates.push_back(brain->getAverageFiringRate());
            spike_counts.push_back(brain->getTotalSpikeCount());
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double avg_firing = 0.0;
        for (double rate : firing_rates) avg_firing += rate;
        avg_firing /= firing_rates.size();
        
        size_t total_spikes = 0;
        for (size_t count : spike_counts) total_spikes += count;
        
        std::stringstream ss;
        ss << "Performance Summary:" << std::endl;
        ss << "  Duration: " << duration.count() << " ms" << std::endl;
        ss << "  Steps per second: " << (duration_steps * 1000.0 / duration.count()) << std::endl;
        ss << "  Average firing rate: " << avg_firing << std::endl;
        ss << "  Total spikes per step: " << (total_spikes / duration_steps) << std::endl;
        ss << "  Firing rate std dev: " << "[calculate standard deviation]" << std::endl;
        
        return ss.str();
    }, py::arg("brain"), py::arg("duration_steps"),
       "Monitor brain performance over a duration");

    m.def("benchmark_brain", [](std::shared_ptr<Config> config, int num_steps) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double steps_per_second = (num_steps * 1000.0) / duration.count();
        
        std::stringstream ss;
        ss << "Benchmark Results:" << std::endl;
        ss << "  Steps: " << num_steps << std::endl;
        ss << "  Duration: " << duration.count() << " ms" << std::endl;
        ss << "  Steps per second: " << steps_per_second << std::endl;
        ss << "  Average time per step: " << (duration.count() / num_steps) << " ms" << std::endl;
        
        return ss.str();
    }, py::arg("config"), py::arg("num_steps"),
       "Benchmark brain performance with given configuration");
}

} // namespace nlm
