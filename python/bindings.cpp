#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

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
        NLM (Neural Learning Machine) Python Bindings v0.1.0
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

        NEW in v0.1.0:
        - Convenience functions for creating common brain configurations
        - Helper functions for running standard simulation patterns  
        - Configuration validation and parameter checking
        - Better error messages and examples

        Quick Start Examples:
        
        # Create a brain with optimized defaults
        brain = pynlm.createQuickBrain()
        
        # Create config for training simulations
        training_config = pynlm.createTrainingConfig()
        
        # Create config for exploration simulations  
        exploration_config = pynlm.createExplorationConfig()
        
        # Validate configuration
        issues, warnings, suggestions = pynlm.validateConfig(config)
        
        # Run a basic simulation
        neurons, avg_rate = pynlm.runBasicSimulation(brain, world, 1000)
        
        # Run a training episode
        neuron_count, avg_rate, total_reward = pynlm.runTrainingEpisode(brain, world, agent, 500)
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

    // Convenience functions for common use cases

    m.def("createQuickWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        // Configure with reasonable defaults
        world->configure(20, 20, 8, 8);
        world->reset();
        return world;
    }, "Create a simple world with reasonable default configuration");

    m.def("createAgentSetup", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::runtime_error("Brain is null - cannot create agent setup");
        }
        if (!world) {
            throw std::runtime_error("World is null - cannot create agent setup");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create and initialize an agent brain with the given world");

    m.def("runTrainingEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                std::shared_ptr<AgentBrain> agent, int steps,
                                bool enable_learning = true, bool verbose = false) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Null brain, world, or agent provided for training episode");
        }
        
        brain->initialize();
        
        if (enable_learning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }
        
        float total_reward = 0.0f;
        
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process input and let brain think
            agent->processSensoryInput(percept);
            brain->step(step);
            
            // Get action and execute
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward and apply learning
            float reward = 0.0f;
            if (enable_learning) {
                agent->applyRewardModulation(reward, 0.0f);  // No prediction error for basic training
            }
            total_reward += reward;
            
            if (verbose && step % 50 == 0) {
                std::cout << "Training step " << step << ": reward=" << reward 
                         << " | total_reward=" << total_reward
                         << " | neurons_active=" << brain->getFiringNeuronCount()
                         << " | avg_rate=" << brain->getAverageFiringRate() << std::endl;
            }
        }
        
        return std::make_tuple(brain->getTotalNeuronCount(), 
                             brain->getAverageFiringRate(),
                             total_reward);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
        py::arg("enable_learning") = true, py::arg("verbose") = false,
        "Run a complete training episode. Returns tuple of (neuron_count, avg_firing_rate, total_reward)");

    m.def("isBrainReady", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            return false;
        }
        return brain->getTotalNeuronCount() > 0 && brain->getTotalSynapseCount() >= 0;
    }, py::arg("brain"), "Check if a brain is properly initialized and ready for simulation");

    m.def("saveBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Brain is null - cannot save checkpoint");
        }
        if (filepath.empty()) {
            throw std::runtime_error("Filepath is empty - cannot save checkpoint");
        }
        
        bool success = brain->save(filepath);
        if (!success) {
            throw std::runtime_error("Failed to save brain checkpoint to " + filepath);
        }
        
        return true;
    }, py::arg("brain"), py::arg("filepath"), "Save brain checkpoint to file. Returns true on success.");

    m.def("loadBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Brain is null - cannot load checkpoint");
        }
        if (filepath.empty()) {
            throw std::runtime_error("Filepath is empty - cannot load checkpoint");
        }
        
        bool success = brain->load(filepath);
        if (!success) {
            throw std::runtime_error("Failed to load brain checkpoint from " + filepath);
        }
        
        return true;
    }, py::arg("brain"), py::arg("filepath"), "Load brain checkpoint from file. Returns true on success.");

    m.def("createExampleConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Create a minimal but functional configuration
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
        config->set("dt", 0.01, nlm::ConfigSource::Default);
        config->set("max_steps", 100, nlm::ConfigSource::Default);
        config->set("enable_reward_modulation", false, nlm::ConfigSource::Default);
        config->set("enable_development", false, nlm::ConfigSource::Default);
        config->set("enable_structural_plasticity", false, nlm::ConfigSource::Default);
        config->set("enable_curiosity", false, nlm::ConfigSource::Default);
        config->set("enable_prediction", false, nlm::ConfigSource::Default);
        return config;
    }, "Create a minimal functional configuration for basic simulations");

    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain is null - cannot get stats");
        }
        
        std::map<std::string, double> stats;
        stats["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["synapse_count"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["active_neuron_count"] = static_cast<double>(brain->getActiveNeuronCount());
        stats["firing_neuron_count"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spike_count"] = static_cast<double>(brain->getTotalSpikeCount());
        
        return stats;
    }, py::arg("brain"), "Get brain statistics as a dictionary. Includes neuron count, synapse count, active/firing neurons, firing rates, and other metrics.");

    m.def("createTrainingConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Configuration optimized for learning simulations
        config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.05, nlm::ConfigSource::Default);
        config->set("dt", 0.005, nlm::ConfigSource::Default);
        config->set("max_steps", 5000, nlm::ConfigSource::Default);
        config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
        config->set("enable_development", true, nlm::ConfigSource::Default);
        config->set("enable_structural_plasticity", true, nlm::ConfigSource::Default);
        config->set("enable_curiosity", true, nlm::ConfigSource::Default);
        config->set("enable_prediction", true, nlm::ConfigSource::Default);
        config->set("record_observations", true, nlm::ConfigSource::Default);
        config->set("save_checkpoints", true, nlm::ConfigSource::Default);
        config->set("checkpoint_interval", 500, nlm::ConfigSource::Default);
        return config;
    }, "Create a configuration optimized for learning/training simulations");

    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Configuration optimized for exploration and curiosity-driven learning
        config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.08, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.03, nlm::ConfigSource::Default);
        config->set("dt", 0.01, nlm::ConfigSource::Default);
        config->set("max_steps", 3000, nlm::ConfigSource::Default);
        config->set("enable_curiosity", true, nlm::ConfigSource::Default);
        config->set("enable_novelty_detection", true, nlm::ConfigSource::Default);
        config->set("enable_reward_prediction_error", true, nlm::ConfigSource::Default);
        config->set("exploration_bonus", 1.0, nlm::ConfigSource::Default);
        config->set("curiosity_strength", 2.0, nlm::ConfigSource::Default);
        config->set("enable_replay", true, nlm::ConfigSource::Default);
        config->set("replay_frequency", 100, nlm::ConfigSource::Default);
        config->set("record_observations", true, nlm::ConfigSource::Default);
        return config;
    }, "Create a configuration optimized for exploration simulations");

    m.def("runBasicSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                int steps, bool verbose = false) {
        if (!brain) {
            throw std::runtime_error("Brain is null - cannot run simulation");
        }
        if (!world) {
            throw std::runtime_error("World is null - cannot run simulation");
        }
        
        brain->initialize();
        
        if (verbose) {
            brain->logStatus();
        }
        
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
            if (verbose && step % 100 == 0) {
                std::cout << "Step: " << step 
                         << " | Neurons: " << brain->getTotalNeuronCount()
                         << " | Active: " << brain->getActiveNeuronCount()
                         << " | Firing: " << brain->getFiringNeuronCount()
                         << std::endl;
            }
        }
        
        if (verbose) {
            brain->logStatus();
        }
        
        return std::make_pair(brain->getTotalNeuronCount(), brain->getAverageFiringRate());
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("verbose") = false,
        "Run a basic simulation with the given brain and world for specified steps. "
        "Returns tuple of (total_neurons, average_firing_rate)");

    // Configuration specification for validation
    static const std::unordered_map<std::string, std::tuple<double, double, std::string>> CONFIG_SPEC = {
        {"neuron_count", std::make_tuple(1.0, 10000.0, "positive integer")},
        {"region_count", std::make_tuple(1.0, 100.0, "positive integer")},
        {"connection_probability", std::make_tuple(0.0, 1.0, "probability [0,1]")},
        {"learning_rate", std::make_tuple(0.0, 1.0, "learning rate [0,1]")},
        {"dt", std::make_tuple(0.001, 0.1, "time step [0.001,0.1]")},
        {"max_steps", std::make_tuple(10.0, 1000000.0, "simulation steps [10,1000000]")},
        {"random_seed", std::make_tuple(0.0, 2147483647.0, "random seed")},
        {"simulation_timestep", std::make_tuple(0.0, 1.0, "simulation timestep")},
        {"plasticity_learning_rate", std::make_tuple(0.0, 1.0, "plasticity learning rate")},
        {"stdp_ltp_weight", std::make_tuple(0.0, 1.0, "STDP LTP weight")},
        {"stdp_ltd_weight", std::make_tuple(0.0, 1.0, "STDP LTD weight")},
        {"stdp_time_constant", std::make_tuple(0.0, 1000.0, "STDP time constant")},
        {"development_synaptogenesis_rate", std::make_tuple(0.0, 1.0, "synaptogenesis rate")},
        {"development_pruning_rate", std::make_tuple(0.0, 1.0, "pruning rate")},
        {"dopamine_baseline", std::make_tuple(0.0, 10.0, "dopamine baseline")},
        {"reward_discount_factor", std::make_tuple(0.0, 1.0, "discount factor")},
        {"environment_width", std::make_tuple(1.0, 1000.0, "environment width")},
        {"environment_height", std::make_tuple(1.0, 1000.0, "environment height")},
        {"max_simulation_steps", std::make_tuple(1.0, 1000000.0, "max simulation steps")},
        {"simulation_time_limit", std::make_tuple(0.0, 10000.0, "simulation time limit")},
        {"visualization_update_rate", std::make_tuple(1.0, 1000.0, "visualization update rate")}
    };

    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw std::runtime_error("Config is null - cannot validate");
        }
        
        std::vector<std::string> issues;
        std::vector<std::string> warnings;
        std::vector<std::string> suggestions;
        
        // Check for duplicate keys (config doesn't store duplicates, but we can check for consistency)
        auto keys = config->getKeys();
        std::sort(keys.begin(), keys.end());
        auto last = std::unique(keys.begin(), keys.end());
        if (last != keys.end()) {
            // There are duplicate keys
            std::vector<std::string> duplicates;
            for (auto it = keys.begin(); it != last; ++it) {
                if (it + 1 != last && *it == *(it + 1)) {
                    duplicates.push_back(*it);
                }
            }
            for (const auto& dup : duplicates) {
                issues.push_back("Duplicate key found: '" + dup + "'");
            }
        }
        
        // Check for essential keys
        auto essential_keys = std::vector<std::string>{
            "neuron_count",
            "region_count",
            "connection_probability",
            "learning_rate",
            "dt",
            "max_steps"
        };
        
        for (const auto& key : essential_keys) {
            if (!config->has(key)) {
                warnings.push_back("Missing recommended key: \'" + key + "\'");
            }
        }
        
        // Validate ranges
        auto validate_range = [config, &issues, &warnings](
            const std::string& key, double min, double max, const std::string& type) {
            if (config->has(key)) {
                auto val = config->get<double>(key);
                if (val && (*val < min || *val > max)) {
                    issues.push_back("Invalid value for \'" + key + "\': " + std::to_string(*val) +
                                   ". Expected " + type + " in range [" + std::to_string(min) +
                                   ", " + std::to_string(max) + "]");
                }
            }
        };
        
        // Use config spec for validation
        for (const auto& [key, spec] : CONFIG_SPEC) {
            validate_range(key, std::get<0>(spec), std::get<1>(spec), std::get<2>(spec));
        }
        
        // Check consistency
        if (config->has("neuron_count") && config->has("region_count")) {
            auto neuron_count = config->get<int64_t>("neuron_count");
            auto region_count = config->get<int64_t>("region_count");
            if (neuron_count && region_count && *neuron_count < *region_count) {
                suggestions.push_back("neuron_count (" + std::to_string(*neuron_count) +
                                    ") should be greater than region_count (" +
                                    std::to_string(*region_count) + ")");
            }
        }
        
        return std::make_tuple(issues, warnings, suggestions);
    }, py::arg("config"), "Validate configuration parameters. "
        "Returns tuple of (issues, warnings, suggestions)");

    m.def("getCurrentTimestamp", []() {
        return nlm::SimulationClock::getCurrentTime();
    }, "Get current simulation timestamp");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
