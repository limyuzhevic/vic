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
        })
        .def("validate", &Config::validate,
             "Validate configuration and return list of errors")
        .def("getBeginnerHelp", &Config::getBeginnerHelp,
             "Get beginner-friendly documentation for configuration options");

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

    // ====================================================
    // NEW: BEGINNER-FRIENDLY API
    // ====================================================

    m.def("createBeginnerBrain", []() {
        auto config = std::make_shared<Config>();
        // Set sensible defaults for beginners
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a beginner-friendly brain with sensible defaults");

    m.def("createSimpleAgentWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 8, 8);
        world->reset();
        world->setAgentStart(10.0f, 10.0f);
        return world;
    }, "Create a simple, configured world for agent training");

    m.def("runSimpleEpisode", [](std::shared_ptr<Brain> brain,
                               std::shared_ptr<SimpleWorld> world,
                               std::shared_ptr<AgentBrain> agent,
                               int numSteps) {
        if (!agent) {
            throw std::runtime_error("Agent is null. Create an agent first.");
        }
        
        if (!world) {
            throw std::runtime_error("World is null. Create a world first.");
        }
        
        if (!brain) {
            throw std::runtime_error("Brain is null. Create a brain first.");
        }
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation based on world state
            auto& worldPercept = world->getSensoryPercept();
            auto internal = worldPercept.getInternal();
            float reward = internal.empty() ? 0.0f : internal[0];
            agent->applyRewardModulation(reward, 0.0f);
        }
        
        return std::make_tuple(brain->getAverageFiringRate(),
                              agent->getCuriosityLevel());
    }, "Run a simple agent episode. Returns (avg_firing_rate, curiosity_level).",
       py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps") = 100);

    m.def("simulateBrainThinking", [](std::shared_ptr<Brain> brain, int numSteps) {
        if (!brain) {
            throw std::runtime_error("Brain is null. Create a brain first.");
        }
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain.");
        }
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        
        return brain->getTotalSpikeCount();
    }, "Simulate a brain thinking for a number of steps.",
       py::arg("brain"), py::arg("numSteps") = 100);

    m.def("createBasicAgent", [](std::shared_ptr<Brain> brain,
                               std::shared_ptr<SimpleWorld> world) {
        if (!brain) {
            throw std::runtime_error("Brain is null. Create a brain first.");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable learning subsystems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return agent;
    }, "Create and initialize an agent brain with world.",
       py::arg("brain"), py::arg("world"));

    // ====================================================
    // IMPROVED ERROR HANDLING AND VALIDATION
    // ====================================================

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Enhanced Config with validation
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
        })
        .def("validate", &Config::validate,
             "Validate configuration and return list of errors")
        .def("getBeginnerHelp", &Config::getBeginnerHelp,
             "Get beginner-friendly documentation for configuration options");

    m.def("createValidatedConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set sensible defaults that validate
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.05f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("random_seed", 42);
        
        return config;
    }, "Create a validated configuration with beginner-friendly defaults");

    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors = config.validate();
        if (!errors.empty()) {
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw std::runtime_error(errorMsg);
        }
        return true;
    }, "Validate a configuration object. Throws RuntimeError on failure.",
       py::arg("config"));

    // ====================================================
    // BETTER DOCUMENTATION AND HELP
    // ====================================================

    m.attr("__version__") = "0.1.0-PHASE6";
    m.attr("__description__") = "NLM (Neural Learning Machine) Python API - Phase 6 Final Integration";
    m.attr("__website__") = "https://github.com/nlm-research/nlm";
    
    m.def("getVersion", []() {
        return std::make_tuple(std::string("NLM Python Bindings"), std::string("Phase 6 - Final Integration"), 0, 1, 0);
    }, "Get version information (name, description, major, minor, patch)");

    m.def("getQuickStartGuide", []() {
        return R"quickstart(
# Quick Start Guide for NLM Python API

## Creating Your First Brain
```python
import pynlm

# Method 1: Simplest possible
brain = pynlm.createBeginnerBrain()
brain.initialize()
for step in range(100):
    brain.step(step)

# Method 2: With agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
brain.step(step)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

## Common Usage Patterns
```python
# Run an agent in a world
brain = pynlm.createBeginnerBrain()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleAgentWorld()

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Common Errors and Solutions
| Error | Solution |
|-------|----------|
| "Brain not initialized" | Always call brain.initialize() before brain.step() |
| "ImportError: No module named pynlm" | pip install --force-reinstall . |
| "Segmentation fault" | Use smaller brain (500-1000 neurons for testing) |

## Configuration Help
Use config.getBeginnerHelp() for beginner-friendly parameter descriptions.
quickstart";
    }, "Get a quick start guide with examples and common patterns");

    m.def("getAPIExamples", []() {
        return R"examples(
# Core API Examples

## Basic Brain Usage
```python
import pynlm

# Create and initialize brain
config = pynlm.createValidatedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Agent with World
```python
import pynlm

# Setup complete agent system
config = pynlm.createValidatedConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Configuration Management
```python
import pynlm

# Custom configuration
config = pynlm.createValidatedConfig()
config.set("brain.neuron_count", 2000)  # More neurons
config.set("plasticity.stdp.learning_rate", 0.005)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createValidatedConfig()
config2.loadFromFile("my_config.json")
```

## Working with Statistics
```python
import pynlm

brain = pynlm.createBeginnerBrain()
brain.initialize()

for step in range(100):
    brain.step(step)

# Get brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

## Error Handling Examples
```python
try:
    brain = pynlm.createBrain(None)  # This will throw an error
except Exception as e:
    print(f"Error: {e}")

try:
    brain.initialize()
    brain.step(100)  # This will crash if not initialized
except Exception as e:
    print(f"Error: {e}")
```

## Research and Experiment Examples
```python
import pynlm
import json

# Run an experiment and collect metrics
brain = pynlm.createBeginnerBrain()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

results = []
for episode in range(10):
    reward_sum = 0.0
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        reward_sum += agent.getNeuromodulationLevel()
    
    results.append({
        'episode': episode,
        'avg_reward': reward_sum / 100,
        'curiosity': agent.getCuriosityLevel(),
        'firing_rate': brain.getAverageFiringRate()
    })

# Save results
with open('experiment_results.json', 'w') as f:
    json.dump(results, f, indent=2)
```
examples";
    }, "Get comprehensive API examples and documentation");

    // ====================================================
    // ADDITIONAL UTILITY FUNCTIONS
    // ====================================================

    m.def("runBeginnersTutorial", []() {
        std::cout << "=== NLM Beginners Tutorial ===\n"
                  << "\n"
                  << "This is a simplified tutorial for getting started with NLM:\n"
                  << "\n"
                  << "1. Create a brain\n"
                  << "   brain = pynlm.createBeginnerBrain()\n"
                  << "\n"
                  << "2. Initialize it (CRITICAL - never skip this!)\n"
                  << "   brain.initialize()\n"
                  << "\n"
                  << "3. Run some simulation steps\n"
                  << "   for step in range(100):\n"
                  << "       brain.step(step)\n"
                  << "\n"
                  << "4. Create an agent with the brain\n"
                  << "   agent = pynlm.createAgentBrain(brain)\n"
                  << "\n"
                  << "5. Create a world\n"
                  << "   world = pynlm.createSimpleWorld()\n"
                  << "\n"
                  << "6. Initialize the agent with the world\n"
                  << "   agent.initialize(world)\n"
                  << "\n"
                  << "7. Run an episode (the loop!)*\n"
                  << "   for step in range(500):\n"
                  << "       world.update(0.1)\n"
                  << "       agent.processSensoryInput(world.getSensoryPercept())\n"
                  << "       brain.step(step)\n"
                  << "       action = agent.decodeMotorCommand()\n"
                  << "       world.applyMotorCommand(action, world.getSimulationTime())\n"
                  << "\n"
                  << "*Tips for beginners:\n"
                  << "- Always initialize before using\n"
                  << "- Use smaller steps (100-1000) for testing\n"
                  << "- Enable learning subsystems for interesting behavior\n"
                  << "- Check statistics periodically\n"
                  << "\n"
                  << "For detailed documentation, visit: https://github.com/nlm-research/nlm\n"
                  << "\n"
                  << "Tutorial complete! You now have a working NLM agent.\n";
    }, "Run an interactive tutorial for beginners");

    m.def("createTestSetup", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 12345);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize test brain.");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, agent, world);
    }, "Create a pre-configured test setup for experimentation.",
       py::arg("brain") = nullptr, py::arg("agent") = nullptr, py::arg("world") = nullptr);

    m.def("runQuickTest", [](std::tuple<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>> setup) {
        auto [brain, agent, world] = setup;
        
        if (!brain || !agent || !world) {
            throw std::runtime_error("Invalid setup. All components must be provided.");
        }
        
        int steps = 200;
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain->getAverageFiringRate(),
                              agent->getCuriosityLevel(),
                              agent->getNoveltyLevel());
    }, "Run a quick test with pre-configured setup. Returns (firing_rate, curiosity, novelty).",
       py::arg("setup"));

    m.def("listAvailableCommands", []() {
        return R"commands(
Available NLM Python Commands:

=== Brain Creation ===
createBeginnerBrain() - Create brain with beginner-friendly settings
createBrain(config) - Create brain from configuration

=== World Creation ===
createSimpleWorld() - Create basic 2D world
createSimpleAgentWorld() - Create world pre-configured for agents

=== Agent Creation ===
createAgentBrain(brain) - Create agent interface

=== Simulation ===
runSimpleEpisode(brain, world, agent, numSteps) - Run complete agent episode
runQuickTest(setup) - Run quick test with pre-configured setup

=== Utilities ===
createValidatedConfig() - Create configuration with validation
createTestSetup() - Create complete test environment

=== Information ===
getVersion() - Get version information
runBeginnersTutorial() - Run interactive tutorial
listAvailableCommands() - List all available commands

=== Error Handling ===
All functions include comprehensive error handling and validation.
commands";
    }, "List all available Python commands and their descriptions");

    m.def("printHelp", []() {
        std::cout << "\n=== NLM Python API Help ===\n\n"
                  << "Welcome to the NLM (Neural Learning Machine) Python API!\n\n"
                  << "This API provides access to the Phase 6 final integrated artificial brain.\n"
                  << "The system includes neural processing, memory, prediction, cognition, and neuromodulation.\n\n"
                  << "For detailed documentation, visit:\n"
                  << "  https://github.com/nlm-research/nlm\n\n"
                  << "Getting Started:\n"
                  << "  1. Run: python -c \"import pynlm; pynlm.runBeginnersTutorial()\"\n"
                  << "  2. Or create a brain: brain = pynlm.createBeginnerBrain()\n"
                  << "  3. Initialize and simulate: brain.initialize(); brain.step(100)\n\n"
                  << "Key Features:\n"
                  << "  • Complete artificial brain implementation (Phase 6)\n"
                  << "  • Neural learning with STDP and Hebbian plasticity\n"
                  << "  • Working memory, episodic memory, and associative memory\n"
                  << "  • Prediction systems with error signals\n"
                  << "  • Neuromodulation (dopamine, curiosity, novelty)\n"
                  << "  • Developmental stages and structural plasticity\n"
                  << "  • Agent interface for world interaction\n\n"
                  << "Research Capabilities:\n"
                  << "  • Checkpoint/saving and loading brain states\n"
                  << "  • Experiment framework with metrics collection\n"
                  << "  • Statistical analysis tools for research\n"
                  << "  • Reproducible experiment execution\n\n"
                  << "For API reference, examples, and documentation:\n"
                  << "  https://github.com/nlm-research/nlm/blob/main/python/README.md\n"
                  << "\n"
                  << "=== End of Help ===\n";
    }, "Display comprehensive help information");

    // ====================================================
    // VALIDATION AND ERROR CHECKING
    // ====================================================

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        // ... existing Config class ...
        .def("validate", &Config::validate,
             "Validate configuration and return list of errors")
        .def("getBeginnerHelp", &Config::getBeginnerHelp,
             "Get beginner-friendly documentation for configuration options");

    m.def("createValidatedConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set sensible defaults that validate
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.05f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("random_seed", 42);
        
        return config;
    }, "Create a validated configuration with beginner-friendly defaults");

    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors = config.validate();
        if (!errors.empty()) {
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw std::runtime_error(errorMsg);
        }
        return true;
    }, "Validate a configuration object. Throws RuntimeError on failure.",
       py::arg("config"));

    m.def("getQuickStartGuide", []() {
        return R"quickstart(
# Quick Start Guide for NLM Python API

## Creating Your First Brain
```python
import pynlm

# Method 1: Simplest possible
brain = pynlm.createBeginnerBrain()
brain.initialize()
for step in range(100):
    brain.step(step)

# Method 2: With agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
brain.step(step)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

## Common Usage Patterns
```python
# Run an agent in a world
brain = pynlm.createBeginnerBrain()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleAgentWorld()

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Common Errors and Solutions
| Error | Solution |
|-------|----------|
| "Brain not initialized" | Always call brain.initialize() before brain.step() |
| "ImportError: No module named pynlm" | pip install --force-reinstall . |
| "Segmentation fault" | Use smaller brain (500-1000 neurons for testing) |

## Configuration Help
Use config.getBeginnerHelp() for beginner-friendly parameter descriptions.
quickstart";
    }, "Get a quick start guide with examples and common patterns");

    m.def("getAPIExamples", []() {
        return R"examples(
# Core API Examples

## Basic Brain Usage
```python
import pynlm

# Create and initialize brain
config = pynlm.createValidatedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Agent with World
```python
import pynlm

# Setup complete agent system
config = pynlm.createValidatedConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Configuration Management
```python
import pynlm

# Custom configuration
config = pynlm.createValidatedConfig()
config.set("brain.neuron_count", 2000)  # More neurons
config.set("plasticity.stdp.learning_rate", 0.005)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createValidatedConfig()
config2.loadFromFile("my_config.json")
```

## Working with Statistics
```python
import pynlm

brain = pynlm.createBeginnerBrain()
brain.initialize()

for step in range(100):
    brain.step(step)

# Get brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

## Error Handling Examples
```python
try:
    brain = pynlm.createBrain(None)  # This will throw an error
except Exception as e:
    print(f"Error: {e}")

try:
    brain.initialize()
    brain.step(100)  # This will crash if not initialized
except Exception as e:
    print(f"Error: {e}")
```

## Research and Experiment Examples
```python
import pynlm
import json

# Run an experiment and collect metrics
brain = pynlm.createBeginnerBrain()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

results = []
for episode in range(10):
    reward_sum = 0.0
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        reward_sum += agent.getNeuromodulationLevel()
    
    results.append({
        'episode': episode,
        'avg_reward': reward_sum / 100,
        'curiosity': agent.getCuriosityLevel(),
        'firing_rate': brain.getAverageFiringRate()
    })

# Save results
with open('experiment_results.json', 'w') as f:
    json.dump(results, f, indent=2)
```
examples";
    }, "Get comprehensive API examples and documentation");

    m.def("runBeginnersTutorial", []() {
        std::cout << "=== NLM Beginners Tutorial ===\n"
                  << "\n"
                  << "This is a simplified tutorial for getting started with NLM:\n"
                  << "\n"
                  << "1. Create a brain\n"
                  << "   brain = pynlm.createBeginnerBrain()\n"
                  << "\n"
                  << "2. Initialize it (CRITICAL - never skip this!)\n"
                  << "   brain.initialize()\n"
                  << "\n"
                  << "3. Run some simulation steps\n"
                  << "   for step in range(100):\n"
                  << "       brain.step(step)\n"
                  << "\n"
                  << "4. Create an agent with the brain\n"
                  << "   agent = pynlm.createAgentBrain(brain)\n"
                  << "\n"
                  << "5. Create a world\n"
                  << "   world = pynlm.createSimpleWorld()\n"
                  << "\n"
                  << "6. Initialize the agent with the world\n"
                  << "   agent.initialize(world)\n"
                  << "\n"
                  << "7. Run an episode (the loop!)*\n"
                  << "   for step in range(500):\n"
                  << "       world.update(0.1)\n"
                  << "       agent.processSensoryInput(world.getSensoryPercept())\n"
                  << "       brain.step(step)\n"
                  << "       action = agent.decodeMotorCommand()\n"
                  << "       world.applyMotorCommand(action, world.getSimulationTime())\n"
                  << "\n"
                  << "*Tips for beginners:\n"
                  << "- Always initialize before using\n"
                  << "- Use smaller steps (100-1000) for testing\n"
                  << "- Enable learning subsystems for interesting behavior\n"
                  << "- Check statistics periodically\n"
                  << "\n"
                  << "For detailed documentation, visit: https://github.com/nlm-research/nlm\n"
                  << "\n"
                  << "Tutorial complete! You now have a working NLM agent.\n";
    }, "Run an interactive tutorial for beginners");

    m.def("createTestSetup", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 12345);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize test brain.");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, agent, world);
    }, "Create a pre-configured test setup for experimentation.",
       py::arg("brain") = nullptr, py::arg("agent") = nullptr, py::arg("world") = nullptr);

    m.def("runQuickTest", [](std::tuple<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>> setup) {
        auto [brain, agent, world] = setup;
        
        if (!brain || !agent || !world) {
            throw std::runtime_error("Invalid setup. All components must be provided.");
        }
        
        int steps = 200;
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain->getAverageFiringRate(),
                              agent->getCuriosityLevel(),
                              agent->getNoveltyLevel());
    }, "Run a quick test with pre-configured setup. Returns (firing_rate, curiosity, novelty).",
       py::arg("setup"));

    m.def("listAvailableCommands", []() {
        return R"commands(
Available NLM Python Commands:

=== Brain Creation ===
createBeginnerBrain() - Create brain with beginner-friendly settings
createBrain(config) - Create brain from configuration

=== World Creation ===
createSimpleWorld() - Create basic 2D world
createSimpleAgentWorld() - Create world pre-configured for agents

=== Agent Creation ===
createAgentBrain(brain) - Create agent interface

=== Simulation ===
runSimpleEpisode(brain, world, agent, numSteps) - Run complete agent episode
runQuickTest(setup) - Run quick test with pre-configured setup

=== Utilities ===
createValidatedConfig() - Create configuration with validation
createTestSetup() - Create complete test environment

=== Information ===
getVersion() - Get version information
runBeginnersTutorial() - Run interactive tutorial
listAvailableCommands() - List all available commands

=== Error Handling ===
All functions include comprehensive error handling and validation.
commands";
    }, "List all available Python commands and their descriptions");

    m.def("printHelp", []() {
        std::cout << "\n=== NLM Python API Help ===\n\n"
                  << "Welcome to the NLM (Neural Learning Machine) Python API!\n\n"
                  << "This API provides access to the Phase 6 final integrated artificial brain.\n"
                  << "The system includes neural processing, memory, prediction, cognition, and neuromodulation.\n\n"
                  << "For detailed documentation, visit:\n"
                  << "  https://github.com/nlm-research/nlm\n\n"
                  << "Getting Started:\n"
                  << "  1. Run: python -c \"import pynlm; pynlm.runBeginnersTutorial()\"\n"
                  << "  2. Or create a brain: brain = pynlm.createBeginnerBrain()\n"
                  << "  3. Initialize and simulate: brain.initialize(); brain.step(100)\n\n"
                  << "Key Features:\n"
                  << "  • Complete artificial brain implementation (Phase 6)\n"
                  << "  • Neural learning with STDP and Hebbian plasticity\n"
                  << "  • Working memory, episodic memory, and associative memory\n"
                  << "  • Prediction systems with error signals\n"
                  << "  • Neuromodulation (dopamine, curiosity, novelty)\n"
                  << "  • Developmental stages and structural plasticity\n"
                  << "  • Agent interface for world interaction\n\n"
                  << "Research Capabilities:\n"
                  << "  • Checkpoint/saving and loading brain states\n"
                  << "  • Experiment framework with metrics collection\n"
                  << "  • Statistical analysis tools for research\n"
                  << "  • Reproducible experiment execution\n\n"
                  << "For API reference, examples, and documentation:\n"
                  << "  https://github.com/nlm-research/nlm/blob/main/python/README.md\n"
                  << "\n"
                  << "=== End of Help ===\n";
    }, "Display comprehensive help information");

    // ====================================================
    // VALIDATION AND ERROR CHECKING
    // ====================================================

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
        })
        .def("validate", &Config::validate,
             "Validate configuration and return list of errors")
        .def("getBeginnerHelp", &Config::getBeginnerHelp,
             "Get beginner-friendly documentation for configuration options");

    m.def("createValidatedConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set sensible defaults that validate
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.05f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("random_seed", 42);
        
        return config;
    }, "Create a validated configuration with beginner-friendly defaults");

    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors = config.validate();
        if (!errors.empty()) {
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw std::runtime_error(errorMsg);
        }
        return true;
    }, "Validate a configuration object. Throws RuntimeError on failure.",
       py::arg("config"));

    m.def("getQuickStartGuide", []() {
        return R"quickstart(
# Quick Start Guide for NLM Python API

## Creating Your First Brain
```python
import pynlm

# Method 1: Simplest possible
brain = pynlm.createBeginnerBrain()
brain.initialize()
for step in range(100):
    brain.step(step)

# Method 2: With agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
brain.step(step)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

## Common Usage Patterns
```python
# Run an agent in a world
brain = pynlm.createBeginnerBrain()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleAgentWorld()

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Common Errors and Solutions
| Error | Solution |
|-------|----------|
| "Brain not initialized" | Always call brain.initialize() before brain.step() |
| "ImportError: No module named pynlm" | pip install --force-reinstall . |
| "Segmentation fault" | Use smaller brain (500-1000 neurons for testing) |

## Configuration Help
Use config.getBeginnerHelp() for beginner-friendly parameter descriptions.
quickstart";
    }, "Get a quick start guide with examples and common patterns");

    m.def("getAPIExamples", []() {
        return R"examples(
# Core API Examples

## Basic Brain Usage
```python
import pynlm

# Create and initialize brain
config = pynlm.createValidatedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Agent with World
```python
import pynlm

# Setup complete agent system
config = pynlm.createValidatedConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Configuration Management
```python
import pynlm

# Custom configuration
config = pynlm.createValidatedConfig()
config.set("brain.neuron_count", 2000)  # More neurons
config.set("plasticity.stdp.learning_rate", 0.005)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createValidatedConfig()
config2.loadFromFile("my_config.json")
```

## Working with Statistics
```python
import pynlm

brain = pynlm.createBeginnerBrain()
brain.initialize()

for step in range(100):
    brain.step(step)

# Get brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

## Error Handling Examples
```python
try:
    brain = pynlm.createBrain(None)  # This will throw an error
except Exception as e:
    print(f"Error: {e}")

try:
    brain.initialize()
    brain.step(100)  # This will crash if not initialized
except Exception as e:
    print(f"Error: {e}")
```

## Research and Experiment Examples
```python
import pynlm
import json

# Run an experiment and collect metrics
brain = pynlm.createBeginnerBrain()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

results = []
for episode in range(10):
    reward_sum = 0.0
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        reward_sum += agent.getNeuromodulationLevel()
    
    results.append({
        'episode': episode,
        'avg_reward': reward_sum / 100,
        'curiosity': agent.getCuriosityLevel(),
        'firing_rate': brain.getAverageFiringRate()
    })

# Save results
with open('experiment_results.json', 'w') as f:
    json.dump(results, f, indent=2)
```
examples";
    }, "Get comprehensive API examples and documentation");

    m.def("runBeginnersTutorial", []() {
        std::cout << "=== NLM Beginners Tutorial ===\n"
                  << "\n"
                  << "This is a simplified tutorial for getting started with NLM:\n"
                  << "\n"
                  << "1. Create a brain\n"
                  << "   brain = pynlm.createBeginnerBrain()\n"
                  << "\n"
                  << "2. Initialize it (CRITICAL - never skip this!)\n"
                  << "   brain.initialize()\n"
                  << "\n"
                  << "3. Run some simulation steps\n"
                  << "   for step in range(100):\n"
                  << "       brain.step(step)\n"
                  << "\n"
                  << "4. Create an agent with the brain\n"
                  << "   agent = pynlm.createAgentBrain(brain)\n"
                  << "\n"
                  << "5. Create a world\n"
                  << "   world = pynlm.createSimpleWorld()\n"
                  << "\n"
                  << "6. Initialize the agent with the world\n"
                  << "   agent.initialize(world)\n"
                  << "\n"
                  << "7. Run an episode (the loop!)*\n"
                  << "   for step in range(500):\n"
                  << "       world.update(0.1)\n"
                  << "       agent.processSensoryInput(world.getSensoryPercept())\n"
                  << "       brain.step(step)\n"
                  << "       action = agent.decodeMotorCommand()\n"
                  << "       world.applyMotorCommand(action, world.getSimulationTime())\n"
                  << "\n"
                  << "*Tips for beginners:\n"
                  << "- Always initialize before using\n"
                  << "- Use smaller steps (100-1000) for testing\n"
                  << "- Enable learning subsystems for interesting behavior\n"
                  << "- Check statistics periodically\n"
                  << "\n"
                  << "For detailed documentation, visit: https://github.com/nlm-research/nlm\n"
                  << "\n"
                  << "Tutorial complete! You now have a working NLM agent.\n";
    }, "Run an interactive tutorial for beginners");

    m.def("createTestSetup", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 12345);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize test brain.");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, agent, world);
    }, "Create a pre-configured test setup for experimentation.",
       py::arg("brain") = nullptr, py::arg("agent") = nullptr, py::arg("world") = nullptr);

    m.def("runQuickTest", [](std::tuple<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>> setup) {
        auto [brain, agent, world] = setup;
        
        if (!brain || !agent || !world) {
            throw std::runtime_error("Invalid setup. All components must be provided.");
        }
        
        int steps = 200;
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain->getAverageFiringRate(),
                              agent->getCuriosityLevel(),
                              agent->getNoveltyLevel());
    }, "Run a quick test with pre-configured setup. Returns (firing_rate, curiosity, novelty).",
       py::arg("setup"));

    m.def("listAvailableCommands", []() {
        return R"commands(
Available NLM Python Commands:

=== Brain Creation ===
createBeginnerBrain() - Create brain with beginner-friendly settings
createBrain(config) - Create brain from configuration

=== World Creation ===
createSimpleWorld() - Create basic 2D world
createSimpleAgentWorld() - Create world pre-configured for agents

=== Agent Creation ===
createAgentBrain(brain) - Create agent interface

=== Simulation ===
runSimpleEpisode(brain, world, agent, numSteps) - Run complete agent episode
runQuickTest(setup) - Run quick test with pre-configured setup

=== Utilities ===
createValidatedConfig() - Create configuration with validation
createTestSetup() - Create complete test environment

=== Information ===
getVersion() - Get version information
runBeginnersTutorial() - Run interactive tutorial
listAvailableCommands() - List all available commands

=== Error Handling ===
All functions include comprehensive error handling and validation.
commands";
    }, "List all available Python commands and their descriptions");

    m.def("printHelp", []() {
        std::cout << "\n=== NLM Python API Help ===\n\n"
                  << "Welcome to the NLM (Neural Learning Machine) Python API!\n\n"
                  << "This API provides access to the Phase 6 final integrated artificial brain.\n"
                  << "The system includes neural processing, memory, prediction, cognition, and neuromodulation.\n\n"
                  << "For detailed documentation, visit:\n"
                  << "  https://github.com/nlm-research/nlm\n\n"
                  << "Getting Started:\n"
                  << "  1. Run: python -c \"import pynlm; pynlm.runBeginnersTutorial()\"\n"
                  << "  2. Or create a brain: brain = pynlm.createBeginnerBrain()\n"
                  << "  3. Initialize and simulate: brain.initialize(); brain.step(100)\n\n"
                  << "Key Features:\n"
                  << "  • Complete artificial brain implementation (Phase 6)\n"
                  << "  • Neural learning with STDP and Hebbian plasticity\n"
                  << "  • Working memory, episodic memory, and associative memory\n"
                  << "  • Prediction systems with error signals\n"
                  << "  • Neuromodulation (dopamine, curiosity, novelty)\n"
                  << "  • Developmental stages and structural plasticity\n"
                  << "  • Agent interface for world interaction\n\n"
                  << "Research Capabilities:\n"
                  << "  • Checkpoint/saving and loading brain states\n"
                  << "  • Experiment framework with metrics collection\n"
                  << "  • Statistical analysis tools for research\n"
                  << "  • Reproducible experiment execution\n\n"
                  << "For API reference, examples, and documentation:\n"
                  << "  https://github.com/nlm-research/nlm/blob/main/python/README.md\n"
                  << "\n"
                  << "=== End of Help ===\n";
    }, "Display comprehensive help information");

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
}

} // namespace nlm
