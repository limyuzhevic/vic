#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

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
        
        Key Features:
        - Comprehensive brain simulation with spiking neurons
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Neuromodulation systems (dopamine, curiosity, novelty)
        - Memory systems (working, episodic, associative)
        - Cognitive mechanisms (concept formation, planning)
        - Agent-world interaction framework
        - Development and maturation systems
        - Configurable simulation environments
        
        Usage Examples:
        ```python
        import pynlm
        
        # Create a default configuration
        config = pynlm.createDefaultConfig()
        
        # Create and initialize a brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for i in range(1000):
            brain.step(i)
        
        # Create an agent with world
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run complete simulation
        for step in range(500):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Basic type definitions
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

    // Enumerations
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

    // Configuration class
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

    // Sensory input classes
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

    // Action and world object classes
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

    // Simple world class
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

    // Brain class
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

    // Agent brain class
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

    // Convenience functions for common simulation patterns
    m.def("run_silent_brain", [](std::shared_ptr<Config> config, int steps) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        return brain;
    }, py::arg("config"), py::arg("steps"), "Run a silent brain simulation (no world interaction)");

    m.def("run_agent_simulation", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world, int steps) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("config"), py::arg("world"), py::arg("steps"), "Run a complete agent simulation");

    m.def("run_learning_experiment", [](std::shared_ptr<Config> config, int steps, bool enable_learning = true) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Create a simple world for the experiment
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        if (enable_learning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward system based on agent position
            if (percept.getInternal().size() > 0) {
                float reward = percept.getInternal()[0];
                agent->applyRewardModulation(reward, 0.0);
            }
            
            agent->updateDevelopment(0.1);
        }
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("config"), py::arg("steps"), py::arg("enable_learning") = true, "Run a learning experiment with reward-based learning");

    m.def("create_experimental_setup", [](const std::string& scenario) {
        auto config = std::make_shared<Config>();
        
        if (scenario == "curiosity") {
            config->set("neuromodulation.curiosity.enable", 1.0f);
            config->set("neuromodulation.novelty.enable", 1.0f);
            config->set("brain.neuron_count", 800);
        } else if (scenario == "reward") {
            config->set("neuromodulation.dopamine.scale", 2.0f);
            config->set("neuromodulation.reward.scale", 1.5f);
            config->set("brain.neuron_count", 1000);
        } else if (scenario == "balanced") {
            config->set("neuromodulation.curiosity.enable", 0.7f);
            config->set("neuromodulation.dopamine.scale", 1.0f);
            config->set("brain.neuron_count", 600);
        } else if (scenario == "development") {
            config->set("development.enabled", 1);
            config->set("development.initial.plasticity", 1.5f);
            config->set("brain.neuron_count", 700);
        } else if (scenario == "memory") {
            config->set("memory.working_memory.capacity", 50);
            config->set("memory.episodic.capacity", 200);
            config->set("brain.neuron_count", 900);
        }
        
        return config;
    }, py::arg("scenario"), "Create an experimental setup with predefined parameters");

    m.def("get_brain_stats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return stats;
    }, py::arg("brain"), "Get comprehensive brain statistics");

    m.def("compare_brains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) {
        std::map<std::string, std::pair<double, double>> comparison;
        
        comparison["neuron_count"] = std::make_pair(brain1->getTotalNeuronCount(), brain2->getTotalNeuronCount());
        comparison["synapse_count"] = std::make_pair(brain1->getTotalSynapseCount(), brain2->getTotalSynapseCount());
        comparison["firing_neurons"] = std::make_pair(brain1->getFiringNeuronCount(), brain2->getFiringNeuronCount());
        comparison["firing_rate"] = std::make_pair(brain1->getAverageFiringRate(), brain2->getAverageFiringRate());
        comparison["spike_count"] = std::make_pair(brain1->getTotalSpikeCount(), brain2->getTotalSpikeCount());
        
        return comparison;
    }, py::arg("brain1"), py::arg("brain2"), "Compare two brains and return statistics comparison");

    m.def("create_simulation_preset", [](const std::string& preset_name) {
        auto config = std::make_shared<Config>();
        
        if (preset_name == "fast") {
            config->set("brain.neuron_count", 100);
            config->set("simulation.timestep", 0.01);
            config->set("plasticity.stdp.learning_rate", 0.001);
        } else if (preset_name == "medium") {
            config->set("brain.neuron_count", 500);
            config->set("simulation.timestep", 0.001);
            config->set("plasticity.stdp.learning_rate", 0.001);
        } else if (preset_name == "comprehensive") {
            config->set("brain.neuron_count", 2000);
            config->set("simulation.timestep", 0.0005);
            config->set("plasticity.stdp.learning_rate", 0.0015);
            config->set("plasticity.hebbian.learning_rate", 0.0005);
            config->set("neuromodulation.curiosity.enable", 1.0f);
            config->set("neuromodulation.novelty.enable", 1.0f);
            config->set("development.enabled", 1);
        }
        
        return config;
    }, py::arg("preset_name"), "Create a simulation preset with predefined parameters");

    // Enumerate preset names
    py::class_<py::dict>(m, "PresetDict", R"pbdoc(Dictionary of preset names and descriptions)pbdoc")
        .def(py::init<>())
        .def("__getitem__", [](const py::dict& d, const std::string& key) {
            if (key == "fast") return "Fast simulation with minimal resources";
            if (key == "medium") return "Balanced simulation with moderate complexity";
            if (key == "comprehensive") return "Comprehensive simulation with all features enabled";
            return std::string();
        });

    m.attr("PRESET_NAMES") = py::dict()
        .def("__setitem__", [](py::dict& d, const std::string& key, const std::string& value) {
            d[key] = value;
        })
        .attr("fast")("Fast simulation with minimal resources")
        .attr("medium")("Balanced simulation with moderate complexity")
        .attr("comprehensive")("Comprehensive simulation with all features enabled");

    m.def("run_multiple_steps", [](std::shared_ptr<Brain> brain, int steps, bool verbose = false) {
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
            if (verbose && i % 100 == 0) {
                std::cout << "Step " << i << ": " << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
            }
        }
        return brain;
    }, py::arg("brain"), py::arg("steps"), py::arg("verbose") = false, "Run multiple steps on a brain");

    m.def("extract_sensory_features", [](const SensoryPercept& percept) {
        std::map<std::string, double> features;
        
        const auto& vision = percept.getVision();
        if (!vision.empty()) {
            double sum = std::accumulate(vision.begin(), vision.end(), 0.0);
            double avg = sum / vision.size();
            double min = *std::min_element(vision.begin(), vision.end());
            double max = *std::max_element(vision.begin(), vision.end());
            
            features["vision_average"] = avg;
            features["vision_min"] = min;
            features["vision_max"] = max;
            features["vision_range"] = max - min;
        }
        
        const auto& touch = percept.getTouch();
        if (!touch.empty()) {
            double sum = std::accumulate(touch.begin(), touch.end(), 0.0);
            features["touch_total"] = sum;
        }
        
        const auto& internal = percept.getInternal();
        if (!internal.empty()) {
            features["internal_mean"] = std::accumulate(internal.begin(), internal.end(), 0.0) / internal.size();
        }
        
        return features;
    }, py::arg("percept"), "Extract meaningful features from sensory percept");

    m.def("create_brain_from_preset", [](const std::string& preset_name, bool randomize = false) {
        auto config = std::make_shared<Config>();
        
        if (preset_name == "fast") {
            config->set("brain.neuron_count", randomize ? (rand() % 100 + 50) : 100);
            config->set("simulation.timestep", 0.01);
            config->set("plasticity.stdp.learning_rate", 0.001);
        } else if (preset_name == "medium") {
            config->set("brain.neuron_count", 500);
            config->set("simulation.timestep", 0.001);
            config->set("plasticity.stdp.learning_rate", 0.001);
        } else if (preset_name == "comprehensive") {
            config->set("brain.neuron_count", 2000);
            config->set("simulation.timestep", 0.0005);
            config->set("plasticity.stdp.learning_rate", 0.0015);
            config->set("plasticity.hebbian.learning_rate", 0.0005);
            config->set("neuromodulation.curiosity.enable", 1.0f);
            config->set("neuromodulation.novelty.enable", 1.0f);
            config->set("development.enabled", 1);
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("preset_name"), py::arg("randomize") = false, "Create a brain from a preset with optional randomization");

    m.def("standardize_configuration", [](std::shared_ptr<Config> config) {
        // Apply standard configuration for reproducible experiments
        config->set("random_seed", 42);
        config->set("simulation.timestep", 0.001);
        config->set("brain.neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        config->set("plasticity.stdp.enable", 1);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.hebbian.enable", 1);
        config->set("plasticity.hebbian.learning_rate", 0.0001);
        config->set("neuromodulation.curiosity.enable", 1);
        config->set("neuromodulation.novelty.enable", 1);
        config->set("neuromodulation.dopamine.scale", 1.0f);
        config->set("development.enabled", 1);
        config->set("development.initial.plasticity", 1.0f);
        
        return config;
    }, py::arg("config"), "Standardize a configuration for reproducible experiments");

    m.def("batch_process_brains", [](const std::vector<std::shared_ptr<Config>>& configs, int steps_per_brain) {
        std::vector<std::tuple<std::shared_ptr<Brain>, std::map<std::string, double>>> results;
        
        for (const auto& config : configs) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            for (int i = 0; i < steps_per_brain; ++i) {
                brain->step(i);
            }
            
            // Collect statistics
            std::map<std::string, double> stats;
            stats["total_neurons"] = brain->getTotalNeuronCount();
            stats["total_synapses"] = brain->getTotalSynapseCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            
            results.push_back(std::make_tuple(brain, stats));
        }
        
        return results;
    }, py::arg("configs"), py::arg("steps_per_brain"), "Batch process multiple brains with different configurations");

    m.def("create_research_protocol", [](const std::string& protocol_name, const std::map<std::string, std::string>& parameters) {
        auto config = std::make_shared<Config>();
        
        if (protocol_name == "curiosity_experiment") {
            config->set("experiment.type", "curiosity");
            config->set("experiment.duration", 3000);
            config->set("neuromodulation.curiosity.enable", 1.0f);
            config->set("neuromodulation.novelty.enable", 1.0f);
            config->set("brain.neuron_count", 800);
            
            if (parameters.find("curiosity_weight") != parameters.end()) {
                config->set("plasticity.curiosity.weight", std::stof(parameters.at("curiosity_weight")));
            }
        } else if (protocol_name == "reward_learning") {
            config->set("experiment.type", "reward_learning");
            config->set("experiment.duration", 2000);
            config->set("neuromodulation.dopamine.scale", 2.0f);
            config->set("neuromodulation.reward.scale", 1.5f);
            config->set("brain.neuron_count", 1000);
            
            if (parameters.find("learning_rate") != parameters.end()) {
                config->set("plasticity.stdp.learning_rate", std::stof(parameters.at("learning_rate")));
            }
        } else if (protocol_name == "development_study") {
            config->set("experiment.type", "development");
            config->set("experiment.duration", 4000);
            config->set("development.enabled", 1);
            config->set("brain.neuron_count", 700);
            
            if (parameters.find("plasticity_schedule") != parameters.end()) {
                config->set("development.plasticity_schedule", parameters.at("plasticity_schedule"));
            }
        }
        
        return config;
    }, py::arg("protocol_name"), py::arg("parameters"), "Create a research protocol with predefined parameters");

    m.def("export_config_to_json", [](std::shared_ptr<Config> config, const std::string& filename) {
        config->saveToFile(filename);
        return true;
    }, py::arg("config"), py::arg("filename"), "Export configuration to JSON file");

    m.def("import_config_from_json", [](const std::string& filename) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filename);
        return config;
    }, py::arg("filename"), "Import configuration from JSON file");

    m.def("validate_config", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        // Check for required keys
        std::vector<std::string> required_keys = {
            "brain.neuron_count",
            "simulation.timestep",
            "random_seed"
        };
        
        for (const auto& key : required_keys) {
            if (!config->has(key)) {
                errors.push_back("Missing required configuration key: " + key);
            }
        }
        
        // Check for valid ranges
        if (config->has("brain.neuron_count")) {
            int64_t neuron_count = config->getOr<int64_t>("brain.neuron_count", 0);
            if (neuron_count <= 0 || neuron_count > 100000) {
                errors.push_back("Invalid neuron_count: must be between 1 and 100000");
            }
        }
        
        if (config->has("simulation.timestep")) {
            double timestep = config->getOr<double>("simulation.timestep", 0.0);
            if (timestep <= 0.0 || timestep > 1.0) {
                errors.push_back("Invalid simulation.timestep: must be between 0.0 and 1.0");
            }
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");

    m.def("config_summary", [](std::shared_ptr<Config> config) {
        return config->summary();
    }, py::arg("config"), "Get a summary of the configuration");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm