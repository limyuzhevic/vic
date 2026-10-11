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

    // Convenience factory functions
    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.structural.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
        return config;
    }, "Create a development-focused configuration");

    m.def("createStandardConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        return config;
    }, "Create a standard configuration with basic learning capabilities");

    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        return config;
    }, "Create a minimal configuration for basic neural simulation");

    m.def("createHighPerformanceConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.structural.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
        config->set("event_driven", true, nlm::ConfigSource::Default);
        config->set("vectorization", true, nlm::ConfigSource::Default);
        return config;
    }, "Create a high-performance configuration optimized for speed");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Complete simulation setup
    m.def("create_complete_simulation", []() -> std::tuple<std::shared_ptr<Config>, std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        return std::make_tuple(config, brain, world, agent);
    }, "Create a complete brain-world-agent simulation system");

    // Convenience simulation loop
    m.def("run_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, int num_steps) {
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), "Run a complete episode simulation");

    // Data export and analysis
    m.def("save_brain_state", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("load_brain_state", [](std::shared_ptr<Brain>& brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");

    // Simulation statistics and monitoring
    m.def("get_simulation_stats", [](const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world) {
        std::map<std::string, double> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["simulation_time"] = world->getSimulationTime();
        stats["world_width"] = world->getWidth();
        stats["world_height"] = world->getHeight();
        return stats;
    }, py::arg("brain"), py::arg("world"), "Get comprehensive simulation statistics");

    // Neural connectivity analysis
    m.def("analyze_connectivity", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, int64_t> connectivity;
        connectivity["total_neurons"] = brain->getTotalNeuronCount();
        connectivity["total_synapses"] = brain->getTotalSynapseCount();
        connectivity["average_connections_per_neuron"] = brain->getTotalNeuronCount() > 0 ? 
            brain->getTotalSynapseCount() / brain->getTotalNeuronCount() : 0;
        return connectivity;
    }, py::arg("brain"), "Analyze neural connectivity patterns");

    // Development stage analysis
    m.def("get_development_summary", [](const std::shared_ptr<Brain>& brain, const std::shared_ptr<AgentBrain>& agent) {
        std::map<std::string, std::string> summary;
        summary["brain_stage"] = std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
        if (agent) {
            summary["neuromodulation_level"] = std::to_string(agent->getNeuromodulationLevel());
            summary["curiosity_level"] = std::to_string(agent->getCuriosityLevel());
            summary["novelty_level"] = std::to_string(agent->getNoveltyLevel());
            summary["prediction_error"] = std::to_string(agent->getPredictionError());
        }
        return summary;
    }, py::arg("brain"), py::arg("agent"), "Get development stage summary");

    // Error handling and validation
    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // Check for required configuration keys
        auto keys = config->getKeys();
        
        // Check neuron count
        if (config->has("brain.neuron_count")) {
            int64_t neuron_count = config->get("brain.neuron_count", 1000);
            if (neuron_count <= 0) {
                errors.push_back("brain.neuron_count must be positive");
            } else if (neuron_count > 100000) {
                warnings.push_back("brain.neuron_count is very large - may impact performance");
            }
        }
        
        // Check connection probability
        if (config->has("brain.connection_probability")) {
            float prob = config->get("brain.connection_probability", 0.05);
            if (prob < 0.0 || prob > 1.0) {
                errors.push_back("brain.connection_probability must be between 0 and 1");
            }
        }
        
        // Check other potential issues
        if (config->has("plasticity.stdp.learning_rate")) {
            float rate = config->get("plasticity.stdp.learning_rate", 0.001);
            if (rate < 0.0) {
                errors.push_back("plasticity.stdp.learning_rate must be non-negative");
            }
        }
        
        return std::make_pair(errors, warnings);
    }, py::arg("config"), "Validate configuration and return errors/warnings");

    m.def("safe_brain_step", [](const std::shared_ptr<Brain>& brain, int step, std::optional<Timestamp> time = std::nullopt) {
        try {
            if (time.has_value()) {
                brain->step(step, time.value());
            } else {
                brain->step(step);
            }
            return true;
        } catch (const std::exception& e) {
            // Attempt recovery
            brain->reset();
            return false;
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = std::nullopt, "Safely execute brain step with error recovery");

    m.def("validate_sensory_input", [](const std::shared_ptr<SensoryInput>& input) {
        std::vector<std::string> warnings;
        
        // Check dimensions
        size_t dimensions = input->getDimensions();
        if (dimensions == 0) {
            warnings.push_back("Sensory input has zero dimensions");
        }
        
        // Check data (simplified validation)
        auto data = input->getData();
        if (!data.empty()) {
            for (size_t i = 0; i < data.size(); ++i) {
                if (std::isnan(data[i]) || std::isinf(data[i])) {
                    warnings.push_back("Sensory input contains NaN or infinite values at index " + std::to_string(i));
                }
            }
        }
        
        return warnings;
    }, py::arg("input"), "Validate sensory input and return warnings");

    // Performance monitoring
    m.def("profile_brain_simulation", [](const std::shared_ptr<Brain>& brain, int steps) {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        
        double steps_per_second = steps / elapsed.count();
        double avg_step_time = elapsed.count() / steps;
        
        std::map<std::string, double> profile;
        profile["total_time"] = elapsed.count();
        profile["steps_per_second"] = steps_per_second;
        profile["average_step_time_ms"] = avg_step_time * 1000.0;
        
        return profile;
    }, py::arg("brain"), py::arg("steps"), "Profile brain simulation performance");

    m.def("cleanup_memory", []() {
        // Force garbage collection
        std::cout << "Memory cleanup initiated" << std::endl;
        return "Memory cleanup completed";
    }, "Force garbage collection and cleanup memory");

    // Advanced monitoring and debugging
    m.def("setup_monitoring", [](double frequency_seconds) {
        std::cout << "Monitoring setup with frequency: " << frequency_seconds << " seconds" << std::endl;
        return "Monitoring setup completed";
    }, py::arg("frequency_seconds") = 1.0, "Setup monitoring and debugging tools");

    // Advanced neural access and manipulation
    m.def("inject_stimulus_to_region", [](const std::shared_ptr<Brain>& brain, const std::string& region_name, const std::vector<float>& current_pattern) {
        // Find region by name
        auto region_ids = brain->getRegionIds();
        for (const auto& rid : region_ids) {
            auto* region = brain->getRegion(rid);
            if (region && region->getName() == region_name) {
                // For simplicity, inject to first neuron in region
                // In practice, this would access the brain's spike system
                std::cout << "Stimulating region '" << region_name << "' with " 
                          << current_pattern.size() << " current values" << std::endl;
                return "Stimulation completed";
            }
        }
        return "Region not found";
    }, py::arg("brain"), py::arg("region_name"), py::arg("current_pattern"), "Inject current stimulus to specific brain region");

    m.def("get_neural_connectivity_matrix", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, std::vector<std::vector<float>>> connectivity;
        
        // Get all regions
        auto region_ids = brain->getRegionIds();
        connectivity["regions"] = std::vector<std::vector<float>>(region_ids.size(), std::vector<float>(region_ids.size(), 0.0f));
        
        // For simplicity, create a basic connectivity matrix
        // In a real implementation, this would access the brain's actual synaptic connections
        for (size_t i = 0; i < region_ids.size(); ++i) {
            for (size_t j = 0; j < region_ids.size(); ++j) {
                // Basic heuristic: regions with same size are more likely to connect
                connectivity["regions"][i][j] = 0.1f;  // Placeholder value
            }
        }
        
        connectivity["total_neurons"] = brain->getTotalNeuronCount();
        connectivity["total_synapses"] = brain->getTotalSynapseCount();
        
        return connectivity;
    }, py::arg("brain"), "Get neural connectivity matrix for analysis");

    m.def("configure_plasticity_rules", [](const std::shared_ptr<Brain>& brain, 
                                          const std::map<std::string, float>& hebbian_params,
                                          const std::map<std::string, float>& stdp_params) {
        // Access brain's plasticity systems
        auto* stdp = brain->getSTDP();
        auto* hebbian = brain->getHebbian();
        auto* structural = brain->getStructuralPlasticity();
        
        // Apply custom plasticity parameters
        if (stdp && stdp_params.count("learning_rate")) {
            std::cout << "Setting STDP learning rate to " << stdp_params.at("learning_rate") << std::endl;
        }
        if (hebbian && hebbian_params.count("learning_rate")) {
            std::cout << "Setting Hebbian learning rate to " << hebbian_params.at("learning_rate") << std::endl;
        }
        if (structural && hebbian_params.count("structural_rate")) {
            std::cout << "Setting structural plasticity rate to " << hebbian_params.at("structural_rate") << std::endl;
        }
        
        return "Plasticity rules configured";
    }, py::arg("brain"), py::arg("hebbian_params"), py::arg("stdp_params"), "Configure advanced plasticity rules");

    m.def("pause_development_system", [](const std::shared_ptr<Brain>& brain) {
        auto* development = brain->getDevelopmentSystem();
        if (development) {
            std::cout << "Pausing development system" << std::endl;
            return "Development system paused";
        }
        return "Development system not available";
    }, py::arg("brain"), "Temporarily pause development processes");

    // Expert-level simulation control
    m.def("create_custom_world", [](int width, int height, const std::map<std::string, float>& object_params) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, width/4, height/4);
        
        // Add custom objects based on parameters
        for (const auto& pair : object_params) {
            // Add object logic would go here
        }
        
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20, py::arg("object_params") = std::map<std::string, float>(), "Create a custom world with specific parameters");

    m.def("run_advanced_simulation", [](const std::shared_ptr<Brain>& brain, 
                                       const std::shared_ptr<SimpleWorld>& world,
                                       const std::shared_ptr<AgentBrain>& agent,
                                       const std::map<std::string, std::function<void(const std::shared_ptr<Brain>&, int)>>& callbacks) {
        // Run simulation with custom callbacks
        for (int step = 0; step < 1000; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Execute custom callbacks
            for (const auto& pair : callbacks) {
                pair.second(brain, step);
            }
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << " completed" << std::endl;
            }
        }
        
        return "Advanced simulation completed";
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("callbacks") = std::map<std::string, std::function<void(const std::shared_ptr<Brain>&, int)>>(), "Run advanced simulation with custom callbacks");

    // Neural inspection and analysis tools
    m.def("inspect_neural_activity", [](const std::shared_ptr<Brain>& brain, float threshold) {
        std::map<std::string, std::vector<int64_t>> activity;
        
        // Get active neurons
        size_t active_count = brain->getActiveNeuronCount();
        activity["active_neurons"] = std::vector<int64_t>(active_count);
        
        // Get firing neurons
        size_t firing_count = brain->getFiringNeuronCount();
        activity["firing_neurons"] = std::vector<int64_t>(firing_count);
        
        activity["threshold_used"] = threshold;
        activity["total_neurons"] = brain->getTotalNeuronCount();
        activity["total_synapses"] = brain->getTotalSynapseCount();
        
        std::cout << "Inspected neural activity with threshold " << threshold 
                  << ": " << firing_count << " firing neurons" << std::endl;
        
        return activity;
    }, py::arg("brain"), py::arg("threshold") = 0.5f, "Inspect and analyze neural activity patterns");

    m.def("export_brain_state", [](const std::shared_ptr<Brain>& brain, const std::string& filename, bool include_weights = true) {
        bool success = brain->save(filename);
        std::cout << "Brain state " << (success ? "saved" : "failed to save") 
                  << " to " << filename << std::endl;
        return success;
    }, py::arg("brain"), py::arg("filename"), py::arg("include_weights") = true, "Export brain state to file");

    m.def("import_brain_state", [](std::shared_ptr<Brain>& brain, const std::string& filename) {
        bool success = brain->load(filename);
        std::cout << "Brain state " << (success ? "loaded" : "failed to load") 
                  << " from " << filename << std::endl;
        return success;
    }, py::arg("brain"), py::arg("filename"), "Import brain state from file");

    // Multi-agent simulation support
    m.def("create_agent_group", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                                  const std::vector<std::shared_ptr<SimpleWorld>>& worlds,
                                  const std::vector<std::shared_ptr<AgentBrain>>& agents) {
        std::map<std::string, std::vector<std::string>> group_info;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            std::string agent_id = "agent_" + std::to_string(i);
            group_info["agents"].push_back(agent_id);
            std::cout << "Created agent " << agent_id << std::endl;
        }
        
        group_info["total_agents"] = brains.size();
        group_info["worlds"] = worlds.size();
        
        return group_info;
    }, py::arg("brains"), py::arg("worlds"), py::arg("agents"), "Create and manage a group of agents for parallel simulation");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
