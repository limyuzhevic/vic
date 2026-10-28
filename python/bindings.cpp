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

    // Version information
    m.attr("__version__") = "0.1.0";

    // Pythonic configuration helpers
    m.def("configureBrainDefaults", [](std::shared_ptr<Config> config) {
        if (!config) return;
        // Set sensible defaults for brain configuration
        config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
        config->set("brain.v_thresh", -50.0f, ConfigSource::Default);
        config->set("brain.v_rest", -70.0f, ConfigSource::Default);
        config->set("brain.v_reset", -75.0f, ConfigSource::Default);
    }, py::arg("config"), "Apply default brain configuration values");

    m.def("configureFastSimulation", [](std::shared_ptr<Config> config) {
        if (!config) return;
        // Optimized configuration for fast simulation
        config->set("brain.neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("brain.connection_probability", 0.02f, ConfigSource::Default);
        config->set("brain.v_thresh", -45.0f, ConfigSource::Default);
        config->set("brain.v_rest", -65.0f, ConfigSource::Default);
        config->set("brain.v_reset", -70.0f, ConfigSource::Default);
    }, py::arg("config"), "Configure for fast simulation with fewer neurons");

    // Convenience factory methods
    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");

    m.def("createSimpleWorldConfigured", [](size_t width, size_t height,
                                           size_t visionWidth = 8, size_t visionHeight = 8) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       "Create and configure a simple world with specified dimensions");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Advanced simulation helpers
    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain,
                                    std::shared_ptr<SimpleWorld> world,
                                    std::shared_ptr<AgentBrain> agent,
                                    size_t num_steps,
                                    double timestep = 0.1,
                                    bool enable_learning = false,
                                    bool enable_development = false) {
        if (!brain || !world || !agent) return;

        brain->initialize();
        world->reset();
        agent->initialize(*world);

        // Enable subsystems if requested
        if (enable_learning) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
        }
        if (enable_development) {
            agent->enableDevelopment(true);
        }

        for (size_t step = 0; step < num_steps; ++step) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       py::arg("timestep") = 0.1, py::arg("enable_learning") = false, py::arg("enable_development") = false,
       "Run a complete agent simulation with brain, world, and agent brain interface");

    m.def("create_brain_and_world", [](size_t width = 20, size_t height = 20,
                                      size_t visionWidth = 8, size_t visionHeight = 8,
                                      size_t neuronCount = 1000) -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);

        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);

        auto agent = std::make_shared<AgentBrain>(brain);

        return std::make_tuple(brain, world, agent);
    }, py::arg("width") = 20, py::arg("height") = 20, py::arg("visionWidth") = 8, py::arg("visionHeight") = 8, py::arg("neuronCount") = 1000,
       "Create a complete brain-world-agent setup with sensible defaults");

    m.def("run_development_simulation", [](std::shared_ptr<Brain> brain,
                                           std::shared_ptr<SimpleWorld> world,
                                           std::shared_ptr<AgentBrain> agent,
                                           size_t num_steps = 500,
                                           double timestep = 0.1) {
        if (!brain || !world || !agent) return;

        brain->initialize();
        world->reset();
        agent->initialize(*world);

        // Enable development features
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);

        for (size_t step = 0; step < num_steps; ++step) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->updateDevelopment(timestep);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps") = 500,
       py::arg("timestep") = 0.1, "Run a development simulation focusing on brain maturation");

    m.def("quick_test_simulation", [](std::shared_ptr<Brain> brain, size_t num_steps = 50) {
        if (!brain) return;

        brain->initialize();

        for (size_t step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("num_steps") = 50, "Run a quick test simulation with just the brain");

    // World configuration helpers
    m.def("createRichWorld", [](size_t width = 30, size_t height = 30) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 12, 12);
        world->setRandomSeed(42);
        return world;
    }, py::arg("width") = 30, py::arg("height") = 30, "Create a rich world with random seed for reproducible experiments");

    // Visualization integration helpers
    m.def("create_visualization", []() -> std::shared_ptr<VisualizationInterface> {
        return std::make_shared<VisualizationInterface>();
    }, "Create a visualization interface for brain state visualization");

    m.def("visualize_brain", [](std::shared_ptr<Brain> brain,
                               std::shared_ptr<VisualizationInterface> viz,
                               bool show_network = true,
                               bool show_activity = true,
                               bool show_spikes = true,
                               bool show_weights = false) {
        if (!brain || !viz) return;

        viz->visualizeNetwork(*brain);
        viz->visualizeActivity(*brain);
        viz->visualizeWeights(*brain);
        // Note: spike visualization requires spike event collection
        // viz->visualizeSpikes(brain->getSpikeSystem()->getRecentSpikes());
    }, py::arg("brain"), py::arg("viz"), py::arg("show_network") = true,
       py::arg("show_activity") = true, py::arg("show_spikes") = true, py::arg("show_weights") = false,
       "Visualize brain state using the visualization interface");

    // Experiment management helpers
    m.def("create_experiment", [](const std::string& name = "") -> std::shared_ptr<Experiment> {
        return std::make_shared<Experiment>(name);
    }, py::arg("name") = "", "Create an experiment for recording data");

    m.def("create_experiment_with_seed", [](uint64_t seed, const std::string& name = "") -> std::shared_ptr<Experiment> {
        auto exp = std::make_shared<Experiment>(name);
        exp->setSeed(seed);
        return exp;
    }, py::arg("seed"), py::arg("name") = "", "Create an experiment with random seed for reproducible results");

    m.def("run_experiment", [](std::shared_ptr<Brain> brain,
                               std::shared_ptr<Experiment> experiment,
                               size_t num_steps,
                               bool enable_visualization = false,
                               std::shared_ptr<VisualizationInterface> viz = nullptr) {
        if (!brain || !experiment) return;

        brain->initialize();
        experiment->setStartStep(0);

        if (enable_visualization && viz) {
            viz->initialize();
        }

        for (size_t step = 0; step < num_steps; ++step) {
            brain->step(step);

            // Record experiment metrics
            auto activeNeurons = brain->getActiveNeuronCount();
            auto firingNeurons = brain->getFiringNeuronCount();
            auto totalSpikes = brain->getTotalSpikeCount();
            auto avgFiringRate = brain->getAverageFiringRate();

            experiment->recordStep(step, brain->getTotalNeuronCount(),
                                  brain->getTotalSynapseCount(),
                                  firingNeurons, avgFiringRate);

            // Record additional metrics
            experiment->recordMetric("active_neurons", static_cast<double>(activeNeurons));
            experiment->recordMetric("firing_neurons", static_cast<double>(firingNeurons));
            experiment->recordMetric("total_spikes", static_cast<double>(totalSpikes));
            experiment->recordMetric("firing_rate", static_cast<double>(avgFiringRate));

            // Update visualization if requested
            if (enable_visualization && viz) {
                viz->update();
            }
        }

        experiment->setEndStep(num_steps - 1);

        if (enable_visualization && viz) {
            viz->close();
        }
    }, py::arg("brain"), py::arg("experiment"), py::arg("num_steps"),
       py::arg("enable_visualization") = false, py::arg("viz") = nullptr,
       "Run an experiment recording brain metrics and optionally visualization");

    // High-level experiment helpers
    m.def("run_development_experiment", [](size_t neuron_count = 500,
                                           size_t steps = 200,
                                           uint64_t seed = 42) -> std::shared_ptr<Experiment> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
        config->set("brain.connection_probability", 0.02f, ConfigSource::Default);

        auto brain = std::make_shared<Brain>(config);
        auto experiment = std::make_shared<Experiment>("Development Experiment");
        experiment->setSeed(seed);

        brain->initialize();
        experiment->setStartStep(0);

        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);

            experiment->recordStep(step, brain->getTotalNeuronCount(),
                                  brain->getTotalSynapseCount(),
                                  brain->getFiringNeuronCount(),
                                  brain->getAverageFiringRate());

            // Record development metrics
            experiment->recordMetric("developmental_stage", static_cast<double>(brain->getDevelopmentalStage()));
            experiment->recordMetric("neurons_active", static_cast<double>(brain->getActiveNeuronCount()));
        }

        experiment->setEndStep(steps - 1);
        return experiment;
    }, py::arg("neuron_count") = 500, py::arg("steps") = 200, py::arg("seed") = 42,
       "Run a complete development experiment and return results");

    m.def("run_agent_experiment", [](std::shared_ptr<Brain> brain,
                                    std::shared_ptr<SimpleWorld> world,
                                    std::shared_ptr<AgentBrain> agent,
                                    size_t num_steps = 300,
                                    uint64_t seed = 42) -> std::shared_ptr<Experiment> {
        if (!brain || !world || !agent) return nullptr;

        auto experiment = std::make_shared<Experiment>("Agent Experiment");
        experiment->setSeed(seed);

        brain->initialize();
        world->reset();
        agent->initialize(*world);

        // Enable learning features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);

        for (size_t step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());

            // Record agent metrics
            experiment->recordStep(step, brain->getTotalNeuronCount(),
                                  brain->getTotalSynapseCount(),
                                  brain->getFiringNeuronCount(),
                                  brain->getAverageFiringRate());

            experiment->recordMetric("step_count", static_cast<double>(step));
            experiment->recordMetric("curiosity", agent->getCuriosityLevel());
            experiment->recordMetric("novelty", agent->getNoveltyLevel());
            experiment->recordMetric("reward_modulation", agent->getNeuromodulationLevel());
            experiment->recordMetric("action_success", 1.0); // Simplified
        }

        experiment->setEndStep(num_steps - 1);
        return experiment;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps") = 300,
       py::arg("seed") = 42, "Run an agent experiment and return recorded data");

    // Utility functions for experiment analysis
    m.def("analyze_experiment_metrics", [](const std::shared_ptr<Experiment>& experiment) {
        if (!experiment) {
            py::print("Error: Invalid experiment");
            return py::dict();
        }

        auto metrics = experiment->getMetrics();
        py::dict result;

        for (const auto& metric : metrics) {
            result[metric.first] = metric.second;
        }

        result["step_count"] = static_cast<double>(experiment->getEndStep() - experiment->getStartStep() + 1);
        return result;
    }, py::arg("experiment"), "Analyze experiment metrics and return summary statistics");

    m.def("export_experiment_results", [](const std::shared_ptr<Experiment>& experiment,
                                          const std::string& filename) -> bool {
        if (!experiment) return false;

        // Create a simple text export
        std::ofstream file(filename);
        if (!file.is_open()) return false;

        file << "Experiment Results\n";
        file << "==================\n\n";

        file << "Name: " << experiment->getName() << "\n";
        file << "Seed: " << experiment->getSeed() << "\n";
        file << "Steps: " << (experiment->getEndStep() - experiment->getStartStep() + 1) << "\n\n";

        file << "Metrics:\n";
        auto metrics = experiment->getMetrics();
        for (const auto& metric : metrics) {
            file << "  " << metric.first << ": " << metric.second << "\n";
        }

        file << "\nStep Data:\n";
        auto stepData = experiment->getStepData();
        for (const auto& step : stepData) {
            file << "  Step " << step.step << ": "
                 << "neurons=" << step.neuronCount
                 << " synapses=" << step.synapseCount
                 << " firing=" << step.firingCount
                 << " rate=" << step.avgFiringRate << "\n";
        }

        file.close();
        return true;
    }, py::arg("experiment"), py::arg("filename"), "Export experiment results to a text file");

    // Error handling and validation
    m.def("validate_simulation_setup", [](std::shared_ptr<Brain> brain,
                                         std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<AgentBrain> agent) -> std::string {
        std::vector<std::string> errors;

        if (!brain) errors.push_back("Brain is null or not initialized");
        if (!world) errors.push_back("World is null or not initialized");
        if (!agent) errors.push_back("Agent is null or not initialized");

        if (brain && brain->getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons");
        }

        if (world && (world->getWidth() == 0 || world->getHeight() == 0)) {
            errors.push_back("World has invalid dimensions");
        }

        if (errors.empty()) {
            return "Setup is valid";
        } else {
            std::string result = "Setup validation errors:\n";
            for (const auto& error : errors) {
                result += "  - " + error + "\n";
            }
            return result;
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), "Validate simulation setup and return error messages");

    m.def("quick_benchmark", [](size_t neuron_count = 1000, size_t steps = 100) -> double {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);

        auto brain = std::make_shared<Brain>(config);
        brain->initialize();

        auto start_time = std::chrono::high_resolution_clock::now();

        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time);

        return duration.count();
    }, py::arg("neuron_count") = 1000, py::arg("steps") = 100,
       "Run a quick benchmark and return execution time in seconds");
}

} // namespace nlm
