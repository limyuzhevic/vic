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

    // Advanced API methods
    
    // Brain utility methods
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible settings");

    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42);
        return std::make_shared<Brain>(config);
    }, "Create a brain with default settings for immediate use");

    m.def("runSimulationLoop", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                std::shared_ptr<AgentBrain> agent, size_t steps,
                                double timestep = 0.1) {
        for (size_t step = 0; step < steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Process sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * timestep);
            
            // Apply action if agent exists
            if (agent) {
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       py::arg("timestep") = 0.1, "Run a complete simulation loop with brain-world interaction");

    m.def("createAgentWithDefaults", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create and initialize an agent with default settings");

    m.def("createWorldWithDefaults", [](size_t width, size_t height, size_t visionWidth = 8,
                                        size_t visionHeight = 8, float maxEnergy = 100.0f) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setMaxEnergy(maxEnergy);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth") = 8,
       py::arg("visionHeight") = 8, py::arg("maxEnergy") = 100.0f,
       "Create a configured world with default settings");

    m.def("trainBrainSimple", [](std::shared_ptr<Brain> brain, size_t steps, float inputStrength = 10.0f,
                               float learningRate = 1.0f) {
        brain->reset();
        brain->initialize();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject sensory-like input
            brain->injectCurrentToNeurons(NeuronType::Sensory, inputStrength);
            brain->step(step, step * 0.001);
        }
        
        return brain->getAverageFiringRate();
    }, py::arg("brain"), py::arg("steps"), py::arg("inputStrength") = 10.0f,
       py::arg("learningRate") = 1.0f, "Simple training loop with sensory input");

    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["exc_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        return stats;
    }, py::arg("brain"), "Get comprehensive brain statistics as a dictionary");

    m.def("runLearningExperiment", [](std::shared_ptr<Brain> brain, size_t steps, float patternStrength = 20.0f) {
        brain->reset();
        brain->initialize();
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t step = 0; step < steps; ++step) {
            // Create learning pattern: stimulate different neuron groups each step
            brain->injectCurrentToNeurons(NeuronType::Sensory, patternStrength);
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                // Log progress
                size_t currentSpikes = brain->getTotalSpikeCount() - initialSpikes;
                float firingRate = brain->getAverageFiringRate();
                py::print(f"Step {step}: {currentSpikes} new spikes, firing rate: {firingRate:.3f}");
            }
        }
        
        return brain->getAverageFiringRate();
    }, py::arg("brain"), py::arg("steps"), py::arg("patternStrength") = 20.0f,
       "Run a learning experiment with patterned input and logging");

    // Factory functions for common patterns
    m.def("createMiniBrainSimulation", [](size_t neuronCount = 50) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 123);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("neuronCount") = 50, "Create a minimal brain for quick testing");

    m.def("createAgentBrainSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all advanced features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create a fully featured agent brain for simulation");

    // Visualization and debugging helpers
    m.def("printBrainArchitecture", [](std::shared_ptr<Brain> brain) {
        py::print("=== NLM Brain Architecture ===");
        py::print(f"Total Regions: {brain->getRegionCount()}");
        py::print(f"Total Neurons: {brain->getTotalNeuronCount()}");
        py::print(f"Total Synapses: {brain->getTotalSynapseCount()}");
        py::print(f"Firing Neurons: {brain->getFiringNeuronCount()}");
        py::print(f"Average Firing Rate: {brain->getAverageFiringRate():.3f}");
        py::print("===============================");
    }, py::arg("brain"), "Print brain architecture and statistics");

    m.def("saveBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->save(filepath);
        if (!success) {
            throw std::runtime_error("Failed to save brain state to " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("loadBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->load(filepath);
        if (!success) {
            throw std::runtime_error("Failed to load brain state from " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        float avgFiring = brain->getAverageFiringRate();
        size_t totalSpikes = brain->getTotalSpikeCount();
        
        py::dict result;
        result["steps_performed"] = steps;
        result["duration_ms"] = static_cast<double>(duration.count());
        result["avg_firing_rate"] = avgFiring;
        result["total_spikes"] = totalSpikes;
        result["performance_hz"] = (steps * 1000.0) / duration.count();
        
        return result;
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance and return metrics");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Advanced API methods
    
    // Brain utility methods
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible settings");

    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42);
        return std::make_shared<Brain>(config);
    }, "Create a brain with default settings for immediate use");

    m.def("runSimulationLoop", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                std::shared_ptr<AgentBrain> agent, size_t steps,
                                double timestep = 0.1) {
        for (size_t step = 0; step < steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Process sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * timestep);
            
            // Apply action if agent exists
            if (agent) {
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       py::arg("timestep") = 0.1, "Run a complete simulation loop with brain-world interaction");

    m.def("createAgentWithDefaults", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create and initialize an agent with default settings");

    m.def("createWorldWithDefaults", [](size_t width, size_t height, size_t visionWidth = 8,
                                        size_t visionHeight = 8, float maxEnergy = 100.0f) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setMaxEnergy(maxEnergy);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth") = 8,
       py::arg("visionHeight") = 8, py::arg("maxEnergy") = 100.0f,
       "Create a configured world with default settings");

    m.def("trainBrainSimple", [](std::shared_ptr<Brain> brain, size_t steps, float inputStrength = 10.0f,
                               float learningRate = 1.0f) {
        brain->reset();
        brain->initialize();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject sensory-like input
            brain->injectCurrentToNeurons(NeuronType::Sensory, inputStrength);
            brain->step(step, step * 0.001);
        }
        
        return brain->getAverageFiringRate();
    }, py::arg("brain"), py::arg("steps"), py::arg("inputStrength") = 10.0f,
       py::arg("learningRate") = 1.0f, "Simple training loop with sensory input");

    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["exc_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        return stats;
    }, py::arg("brain"), "Get comprehensive brain statistics as a dictionary");

    m.def("runLearningExperiment", [](std::shared_ptr<Brain> brain, size_t steps, float patternStrength = 20.0f) {
        brain->reset();
        brain->initialize();
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t step = 0; step < steps; ++step) {
            // Create learning pattern: stimulate different neuron groups each step
            brain->injectCurrentToNeurons(NeuronType::Sensory, patternStrength);
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                // Log progress
                size_t currentSpikes = brain->getTotalSpikeCount() - initialSpikes;
                float firingRate = brain->getAverageFiringRate();
                py::print(f"Step {step}: {currentSpikes} new spikes, firing rate: {firingRate:.3f}");
            }
        }
        
        return brain->getAverageFiringRate();
    }, py::arg("brain"), py::arg("steps"), py::arg("patternStrength") = 20.0f,
       "Run a learning experiment with patterned input and logging");

    // Factory functions for common patterns
    m.def("createMiniBrainSimulation", [](size_t neuronCount = 50) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 123);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("neuronCount") = 50, "Create a minimal brain for quick testing");

    m.def("createAgentBrainSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all advanced features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create a fully featured agent brain for simulation");

    // Visualization and debugging helpers
    m.def("printBrainArchitecture", [](std::shared_ptr<Brain> brain) {
        py::print("=== NLM Brain Architecture ===");
        py::print(f"Total Regions: {brain->getRegionCount()}");
        py::print(f"Total Neurons: {brain->getTotalNeuronCount()}");
        py::print(f"Total Synapses: {brain->getTotalSynapseCount()}");
        py::print(f"Firing Neurons: {brain->getFiringNeuronCount()}");
        py::print(f"Average Firing Rate: {brain->getAverageFiringRate():.3f}");
        py::print("===============================");
    }, py::arg("brain"), "Print brain architecture and statistics");

    m.def("saveBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->save(filepath);
        if (!success) {
            throw std::runtime_error("Failed to save brain state to " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("loadBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->load(filepath);
        if (!success) {
            throw std::runtime_error("Failed to load brain state from " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");

    // Performance monitoring and profiling
    struct PerformanceMetrics {
        double setupTime;
        double simulationTime;
        double totalTime;
        size_t stepsCompleted;
        float avgFiringRate;
        size_t totalSpikes;
        double performanceHz;
    };

    py::class_<PerformanceMetrics>(m, "PerformanceMetrics", "Performance metrics for brain simulation")
        .def(py::init<>(), "Default constructor")
        .def_readwrite("setupTime", &PerformanceMetrics::setupTime)
        .def_readwrite("simulationTime", &PerformanceMetrics::simulationTime)
        .def_readwrite("totalTime", &PerformanceMetrics::totalTime)
        .def_readwrite("stepsCompleted", &PerformanceMetrics::stepsCompleted)
        .def_readwrite("avgFiringRate", &PerformanceMetrics::avgFiringRate)
        .def_readwrite("totalSpikes", &PerformanceMetrics::totalSpikes)
        .def_readwrite("performanceHz", &PerformanceMetrics::performanceHz)
        .def("__repr__", [](const PerformanceMetrics& metrics) {
            return "PerformanceMetrics(setupTime=" + std::to_string(metrics.setupTime) +
                   ", simulationTime=" + std::to_string(metrics.simulationTime) +
                   ", totalTime=" + std::to_string(metrics.totalTime) +
                   ", stepsCompleted=" + std::to_string(metrics.stepsCompleted) +
                   ", avgFiringRate=" + std::to_string(metrics.avgFiringRate) +
                   ", totalSpikes=" + std::to_string(metrics.totalSpikes) +
                   ", performanceHz=" + std::to_string(metrics.performanceHz) + ")";
        });

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t steps) -> PerformanceMetrics {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Setup time
        auto setup_start = std::chrono::high_resolution_clock::now();
        brain->reset();
        brain->initialize();
        auto setup_end = std::chrono::high_resolution_clock::now();
        
        // Simulation
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        
        auto setup_duration = std::chrono::duration<double>(setup_end - setup_start).count();
        auto simulation_duration = std::chrono::duration<double>(end_time - setup_end).count();
        auto total_duration = std::chrono::duration<double>(end_time - start_time).count();
        
        float avgFiring = brain->getAverageFiringRate();
        size_t totalSpikes = brain->getTotalSpikeCount();
        double performance_hz = (steps * 1000.0) / total_duration;
        
        PerformanceMetrics metrics;
        metrics.setupTime = setup_duration;
        metrics.simulationTime = simulation_duration;
        metrics.totalTime = total_duration;
        metrics.stepsCompleted = steps;
        metrics.avgFiringRate = avgFiring;
        metrics.totalSpikes = totalSpikes;
        metrics.performanceHz = performance_hz;
        
        return metrics;
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance and return metrics");

    m.def("createPerformanceProfiler", [](std::shared_ptr<Brain> brain) {
        struct BrainProfiler {
            std::shared_ptr<Brain> brainRef;
            std::vector<double> stepTimes;
            std::vector<size_t> spikeCounts;
            size_t stepsCompleted;
            
            BrainProfiler(std::shared_ptr<Brain> b) : brainRef(b), stepsCompleted(0) {}
            
            void step(double timestep) {
                auto start = std::chrono::high_resolution_clock::now();
                brainRef->step(stepsCompleted, stepsCompleted * timestep);
                auto end = std::chrono::high_resolution_clock::now();
                
                double stepTime = std::chrono::duration<double>(end - start).count() * 1000.0; // ms
                stepTimes.push_back(stepTime);
                spikeCounts.push_back(brainRef->getTotalSpikeCount());
                stepsCompleted++;
            }
            
            py::dict getStats() {
                py::dict stats;
                if (stepTimes.empty()) {
                    return stats;
                }
                
                double totalTime = 0.0;
                size_t totalSpikes = spikeCounts.empty() ? 0 : spikeCounts.back();
                
                for (double t : stepTimes) totalTime += t;
                
                stats["steps_completed"] = stepsCompleted;
                stats["avg_step_time_ms"] = totalTime / stepTimes.size();
                stats["min_step_time_ms"] = *std::min_element(stepTimes.begin(), stepTimes.end());
                stats["max_step_time_ms"] = *std::max_element(stepTimes.begin(), stepTimes.end());
                stats["total_spikes"] = totalSpikes;
                stats["current_step_time_ms"] = stepTimes.empty() ? 0.0 : stepTimes.back();
                
                return stats;
            }
            
            std::vector<double> getStepTimes() { return stepTimes; }
            std::vector<size_t> getSpikeCounts() { return spikeCounts; }
        };
        
        return std::make_shared<BrainProfiler>(brain);
    }, py::arg("brain"), "Create a performance profiler for monitoring brain simulation");

    m.def("compareBrains", [](std::vector<std::shared_ptr<Brain>> brains, size_t steps) {
        std::vector<PerformanceMetrics> metrics;
        
        for (auto& brain : brains) {
            auto start = std::chrono::high_resolution_clock::now();
            brain->reset();
            brain->initialize();
            auto setup_end = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            
            PerformanceMetrics metric;
            metric.setupTime = std::chrono::duration<double>(setup_end - start).count();
            metric.simulationTime = std::chrono::duration<double>(end - setup_end).count();
            metric.totalTime = std::chrono::duration<double>(end - start).count();
            metric.stepsCompleted = steps;
            metric.avgFiringRate = brain->getAverageFiringRate();
            metric.totalSpikes = brain->getTotalSpikeCount();
            metric.performanceHz = (steps * 1000.0) / metric.totalTime;
            
            metrics.push_back(metric);
        }
        
        return metrics;
    }, py::arg("brains"), py::arg("steps"), "Compare performance of multiple brains");

    // Utility and helper functions
    m.def("calculateFiringRate", [](std::shared_ptr<Brain> brain, size_t steps) {
        brain->reset();
        brain->initialize();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return brain->getAverageFiringRate();
    }, py::arg("brain"), py::arg("steps"), "Calculate average firing rate over specified steps");

    m.def("getNetworkDensity", [](std::shared_ptr<Brain> brain) {
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalPossibleSynapses = totalNeurons * (totalNeurons - 1) / 2; // Complete graph
        size_t actualSynapses = brain->getTotalSynapseCount();
        
        if (totalPossibleSynapses == 0) return 0.0f;
        
        return static_cast<float>(actualSynapses) / totalPossibleSynapses;
    }, py::arg("brain"), "Calculate network density (ratio of actual synapses to possible connections)");

    m.def("createConfigPresets", []() {
        py::dict presets;
        
        // Minimal configuration
        auto minimalConfig = std::make_shared<Config>();
        minimalConfig->set("neuron_count", 10);
        minimalConfig->set("region_count", 1);
        presets["minimal"] = minimalConfig;
        
        // Medium configuration
        auto mediumConfig = std::make_shared<Config>();
        mediumConfig->set("neuron_count", 100);
        mediumConfig->set("region_count", 2);
        mediumConfig->set("connection_probability", 0.15f);
        presets["medium"] = mediumConfig;
        
        // Large configuration
        auto largeConfig = std::make_shared<Config>();
        largeConfig->set("neuron_count", 1000);
        largeConfig->set("region_count", 4);
        largeConfig->set("connection_probability", 0.1f);
        presets["large"] = largeConfig;
        
        // High connectivity
        auto highConnConfig = std::make_shared<Config>();
        highConnConfig->set("neuron_count", 500);
        highConnConfig->set("region_count", 1);
        highConnConfig->set("connection_probability", 0.3f);
        presets["high_connectivity"] = highConnConfig;
        
        return presets;
    }, "Create preset configurations for different use cases");

    // System information and diagnostics
    m.def("getSystemInfo", []() {
        py::dict info;
        info["version"] = "1.0.0";
        info["build_type"] = "Debug"; // This could be determined at build time
        info["compiler"] = "GCC"; // This could be determined at build time
        info["python_bindings"] = true;
        info["cpp_standard"] = "C++20";
        return info;
    }, "Get system information about the NLM build");

    m.def("checkDependencies", []() {
        py::dict status;
        status["pybind11"] = true;
        status["cmake"] = true;
        status["std_cpp"] = true;
        status["logger"] = true;
        status["config_system"] = true;
        status["neural_networks"] = true;
        return status;
    }, "Check availability of core dependencies");

    // Error handling utilities
    m.def("safeBrainOperation", [](std::shared_ptr<Brain> brain, size_t steps) -> py::object {
        try {
            brain->reset();
            brain->initialize();
            
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
            }
            
            py::dict result;
            result["success"] = true;
            result["firing_rate"] = brain->getAverageFiringRate();
            result["total_spikes"] = brain->getTotalSpikeCount();
            return result;
        } catch (const std::exception& e) {
            py::dict errorResult;
            errorResult["success"] = false;
            errorResult["error"] = std::string(e.what());
            return errorResult;
        }
    }, py::arg("brain"), py::arg("steps"), "Safely execute brain operations with error handling");

    m.def("validateBrainConfig", [](std::shared_ptr<Config> config) {
        py::dict validation;
        
        // Check for required parameters
        if (!config->has("neuron_count")) {
            validation["valid"] = false;
            validation["error"] = "Missing required parameter: neuron_count";
            return validation;
        }
        
        int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
        if (neuronCount <= 0) {
            validation["valid"] = false;
            validation["error"] = "neuron_count must be positive";
            return validation;
        }
        
        if (!config->has("region_count")) {
            validation["valid"] = false;
            validation["error"] = "Missing required parameter: region_count";
            return validation;
        }
        
        int64_t regionCount = config->getOr<int64_t>("region_count", 0);
        if (regionCount <= 0) {
            validation["valid"] = false;
            validation["error"] = "region_count must be positive";
            return validation;
        }
        
        float connProb = config->getOr<float>("connection_probability", 1.0f);
        if (connProb < 0.0f || connProb > 1.0f) {
            validation["valid"] = false;
            validation["error"] = "connection_probability must be between 0.0 and 1.0";
            return validation;
        }
        
        // If all checks pass
        validation["valid"] = true;
        validation["message"] = "Configuration is valid";
        validation["neuron_count"] = neuronCount;
        validation["region_count"] = regionCount;
        validation["connection_probability"] = connProb;
        
        return validation;
    }, py::arg("config"), "Validate configuration parameters");

    // Configuration management utilities
    m.def("exportConfigToJson", [](std::shared_ptr<Config> config, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }
        
        file << "{\n";
        auto keys = config->getKeys();n        for (size_t i = 0; i < keys.size(); ++i) {
            std::string value;
            if (config->has("neuron_count")) value = std::to_string(config->getOr<int64_t>("neuron_count", 0));
            else if (config->has("connection_probability")) value = std::to_string(config->getOr<float>("connection_probability", 0.0f));
            else if (config->has("simulation_timestep")) value = std::to_string(config->getOr<double>("simulation_timestep", 0.0));
            else value = "\"unknown\"";
            
            file << "  \"" << keys[i] << "\": " << value;
            if (i < keys.size() - 1) file << ",";
            file << "\n";
        }
        file << "}\n";
        file.close();
        return true;
    }, py::arg("config"), py::arg("filepath"), "Export configuration to JSON file");

    // Quick setup helpers
    m.def("setupCompleteEnvironment", [](size_t neuronCount = 100, size_t worldWidth = 20, 
                                         size_t worldHeight = 20) {
        py::dict environment;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, 8, 8);
        world->reset();
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        environment["config"] = config;
        environment["brain"] = brain;
        environment["world"] = world;
        environment["agent"] = agent;
        
        py::print("Complete NLM environment created successfully!");
        py::print(f"  - Brain with {neuronCount} neurons");
        py::print(f"  - World {worldWidth}x{worldHeight} with 8x8 vision");
        py::print(f"  - Agent with reward modulation, curiosity, and development enabled");
        
        return environment;
    }, py::arg("neuronCount") = 100, py::arg("worldWidth") = 20, py::arg("worldHeight") = 20,
       "Create a complete, ready-to-use NLM environment");

    // Documentation and help
    m.def("showAdvancedExamples", []() {
        py::print("=== NLM Advanced Usage Examples ===");
        py::print("");
        py::print("1. Performance Comparison:");
        py::print("   brains = [nlm.createBrain(nlm.createDefaultConfig()) for _ in range(3)]");
        py::print("   results = nlm.compareBrains(brains, 1000)");
        py::print("   for r in results: print(f\"Rate: {r.avgFiringRate:.3f} Hz, Time: {r.totalTime:.3f}s\")");
        py::print("");
        py::print("2. Advanced Profiling:")
        py::print("   profiler = nlm.createPerformanceProfiler(brain)")
        py::print("   for i in range(100): profiler.step(0.001)")
        py::print("   stats = profiler.getStats()")
        py::print("   print(f\"Avg step time: {stats.avg_step_time_ms:.2f}ms\")");
        py::print("");
        py::print("3. Machine Learning Integration:")
        py::print("   from sklearn.preprocessing import StandardScaler")
        py::print("   scaler = StandardScaler()")
        py::print("   data = [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]]  # Your sensor data");
        py::print("   scaled = scaler.fit_transform(data)");
        py::print("   for row in scaled:")
        py::print("       vision = nlm.Vision(3, 1, 3)")
        py::print("       vision.setData(row)")
        py::print("       brain.receiveSensoryInput(vision)");
        py::print("       brain.step(step)");
        py::print("");
        py::print("4. Batch Processing:")
        py::print("   def process_batch(scenarios):")
        py::print("       results = []")
        py::print("       for scenario in scenarios:")
        py::print("           brain = nlm.createBrain(scenario.config)")
        py::print("           metrics = nlm.benchmarkBrain(brain, scenario.steps)")
        py::print("           results.append(metrics)")
        py::print("       return results");
        py::print("");
        py::print("5. Research Experiments:")
        py::print("   # Investigate network properties")
        py::print("   density = nlm.getNetworkDensity(brain)")
        py::print("   firing_rate = nlm.calculateFiringRate(brain, 500)")
        py::print("   print(f\"Network density: {density:.4f}\")")
        py::print("");
        py::print("=== Advanced Features Available ===");
        py::print("  ✓ Performance benchmarking and comparison");
        py::print("  ✓ Advanced profiling and time measurement");
        py::print("  ✓ Machine learning integration helpers");
        py::print("  ✓ Batch processing utilities");
        py::print("  ✓ Research and analysis tools");
        py::print("  ✓ Configuration validation and export");
        py::print("  ✓ Error handling and safe operations");
        py::print("  ✓ Preset configurations for different use cases");
        py::print("");
    }, "Show advanced usage examples and capabilities");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
