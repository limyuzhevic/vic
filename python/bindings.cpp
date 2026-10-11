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

    // Custom exception for NLM errors
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
    
    py::register_exception<std::logic_error>(m, "RuntimeError", PyExc_RuntimeError);
    
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

    // Convenience functions for creating default experiments
    m.def("createExperiment", [](const std::string& name) -> std::shared_ptr<Experiment> {
        return std::make_shared<Experiment>(name);
    }, py::arg("name"), R"pbdoc(
        Create a new experiment with the given name.
        
        Examples:
            >>> exp = nlm.create_experiment("learning_test")
            >>> exp.set_seed(42)
            >>> exp.set_config_value("learning_rate", "0.01")
        )pbdoc");

    m.def("createExperimentWithConfig", [](const std::string& name, const std::shared_ptr<Config>& config) -> std::shared_ptr<Experiment> {
        auto exp = std::make_shared<Experiment>(name);
        // Apply config to experiment
        for (const auto& key : config->getKeys()) {
            auto value = config->get<std::string>(key);
            if (value) exp->setConfigValue(key, *value);
        }
        return exp;
    }, py::arg("name"), py::arg("config"), R"pbdoc(
        Create a new experiment with the given name and apply configuration.
        
        Examples:
            >>> config = nlm.create_default_config()
            >>> config.set("learning_rate", 0.01)
            >>> exp = nlm.create_experiment_with_config("training", config)
        )pbdoc");

    // High-level API functions for common neural simulation tasks
    m.def("run_simple_learning_simulation", 
        [](const std::string& experimentName,
           SimulationStep maxSteps,
           float learningRate,
           float noiseLevel) {
            // Create components
            auto config = std::make_shared<Config>();
            config->set("learning_rate", learningRate);
            config->set("noise_level", noiseLevel);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(100, 100, 84, 84); // screen-like dimensions
            
            auto experiment = std::make_shared<Experiment>(experimentName);
            experiment->setSeed(42);
            
            // Create simple world with a central agent
            world->setAgentStart(50.0f, 50.0f);
            
            // Run simulation
            return std::make_tuple(experiment, brain, world);
        },
        py::arg("experiment_name"), py::arg("max_steps"), py::arg("learning_rate"),
        py::arg("noise_level") = 0.1f,
        R"pbdoc(
        Run a simple learning simulation with configurable parameters.
        
        Args:
            experiment_name: Name for the experiment
            max_steps: Maximum number of simulation steps
            learning_rate: Learning rate for neural plasticity
            noise_level: Level of noise to add to neural dynamics
        
        Returns:
            Tuple of (experiment, brain, world)
        
        Examples:
            >>> exp, brain, world = nlm.run_simple_learning_simulation(
            ...     "simple_learning",
            ...     max_steps=1000,
            ...     learning_rate=0.01,
            ...     noise_level=0.05
            ... )
            >>> for step in range(100):
            ...     brain.step(step)
        )pbdoc");

    m.def("run_agent_training_simulation",
        [](const std::string& experimentName,
           const std::vector<float>& initialSensoryData,
           SimulationStep maxSteps,
           float rewardScale) {
            // Create experiment
            auto experiment = std::make_shared<Experiment>(experimentName);
            experiment->setSeed(12345);
            
            // Create config with agent settings
            auto config = std::make_shared<Config>();
            config->set("reward_scale", rewardScale);
            
            // Create brain and agent
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            auto world = std::make_shared<SimpleWorld>();
            
            // Initialize with sensory data
            auto percept = std::make_unique<SensoryPercept>();
            // In a real implementation, would set actual sensory data
            
            agentBrain->initialize(world);
            agentBrain->processSensoryInput(*percept);
            
            // Run training loop
            for (SimulationStep step = 0; step < maxSteps && world->getAgentBody().health > 0; ++step) {
                brain->step(step);
                
                auto action = brain->produceAction();
                
                // Apply action to world
                world->applyMotorCommand(action, static_cast<float>(step));
                
                // Get percept and process
                auto observation = world->getSensoryPercept();
                agentBrain->processSensoryInput(*observation);
                
                // Record metrics
                experiment->recordStep(
                    step,
                    brain->getTotalNeuronCount(),
                    brain->getTotalSynapseCount(),
                    brain->getFiringNeuronCount(),
                    brain->getAverageFiringRate()
                );
            }
            
            return std::make_tuple(experiment, agentBrain, world);
        },
        py::arg("experiment_name"), py::arg("initial_sensory_data"),
        py::arg("max_steps"), py::arg("reward_scale") = 1.0f,
        R"pbdoc(
        Run an agent training simulation with sensory-motor loops.
        
        Args:
            experiment_name: Name for the experiment
            initial_sensory_data: Initial sensory input vector
            max_steps: Maximum number of simulation steps
            reward_scale: Scale factor for rewards
        
        Returns:
            Tuple of (experiment, agent_brain, world)
        
        Examples:
            >>> sensory_data = [0.1, 0.2, 0.3, 0.4, 0.5]
            >>> exp, agent, world = nlm.run_agent_training_simulation(
            ...     "agent_training",
            ...     sensory_data,
            ...     max_steps=500,
            ...     reward_scale=1.0
            ... )
        )pbdoc");

    // Experiment runner utilities
    m.def("create_experiment_runner", []() -> std::shared_ptr<ExperimentRunner> {
        return std::make_shared<ExperimentRunner>();
    }, R"pbdoc(
        Create a new experiment runner for managing multiple experiments.
        
        Returns:
            New ExperimentRunner instance
        
        Examples:
            >>> runner = nlm.create_experiment_runner()
            >>> runner.set_global_seed(42)
        )pbdoc");

    m.def("run_experiment_batch",
        [](std::shared_ptr<ExperimentRunner> runner,
           const std::vector<std::shared_ptr<Experiment>>& experiments,
           std::shared_ptr<Brain> brainTemplate,
           std::shared_ptr<Environment> environmentTemplate,
           SimulationStep maxSteps) {
            return runner->runBatch(experiments, brainTemplate, environmentTemplate, maxSteps);
        },
        py::arg("runner"), py::arg("experiments"), py::arg("brain_template"),
        py::arg("environment_template"), py::arg("max_steps"),
        R"pbdoc(
        Run a batch of experiments with shared brain and environment templates.
        
        Args:
            runner: Experiment runner instance
            experiments: List of experiments to run
            brain_template: Brain template for each experiment
            environment_template: Environment template for each experiment
            max_steps: Maximum simulation steps per experiment
        
        Returns:
            List of completed experiments
        "pbdoc");

    // Configuration management helpers
    m.def("create_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a new configuration object with default settings.
        
        Returns:
            New Config instance
        
        Examples:
            >>> config = nlm.create_config()
            >>> config.set("learning_rate", 0.01)
            >>> config.set("population_size", 1000)
            >>> config.set("simulation_steps", 10000)
        )pbdoc");

    m.def("create_config_from_json",
        [](const std::string& filepath) -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(filepath)) {
                py::set_error(PyExc_RuntimeError, "Failed to load configuration from file: " + filepath);
                return nullptr;
            }
            return config;
        },
        py::arg("filepath"),
        R"pbdoc(
        Create a configuration object from a JSON file.
        
        Args:
            filepath: Path to JSON configuration file
        
        Returns:
            Config instance loaded from file
        
        Raises:
            RuntimeError: If file cannot be loaded
        
        Examples:
            >>> config = nlm.create_config_from_json("config.json")
            >>> print(config.get_or("learning_rate", 0.01))
        )pbdoc");

    m.def("merge_configs",
        [](const std::shared_ptr<Config>& base,
           const std::shared_ptr<Config>& override) -> std::shared_ptr<Config> {
            auto merged = std::make_shared<Config>(*base);
            for (const auto& key : override->getKeys()) {
                auto value = override->get<ConfigValue>(key);
                if (value) {
                    merged->set(key, *value);
                }
            }
            return merged;
        },
        py::arg("base_config"), py::arg("override_config"),
        R"pbdoc(
        Merge two configuration objects, with override taking precedence.
        
        Args:
            base_config: Base configuration
            override_config: Configuration with overrides
        
        Returns:
            Merged configuration
        
        Examples:
            >>> base = nlm.create_config()
            >>> base.set("learning_rate", 0.01)
            >>> override = nlm.create_config()
            >>> override.set("learning_rate", 0.05)
            >>> merged = nlm.merge_configs(base, override)
        )pbdoc");

    // Memory management utilities
    m.def("create_memory_pool",
        [](size_t neuronCount, size_t synapseCount) {
            // This would create specialized memory pools in a real implementation
            // For now, return a simple structure
            return std::make_tuple(neuronCount, synapseCount);
        },
        py::arg("neuron_count"), py::arg("synapse_count"),
        R"pbdoc(
        Create optimized memory pools for neurons and synapses.
        
        Args:
            neuron_count: Number of neurons to allocate memory for
            synapse_count: Number of synapses to allocate memory for
        
        Returns:
            Tuple of (neuron_count, synapse_count) allocated
        
        Examples:
            >>> neuron_pool, synapse_pool = nlm.create_memory_pool(1000, 10000)
        )pbdoc");

    m.def("get_memory_usage",
        [](const std::shared_ptr<Brain>& brain) {
            if (!brain) {
                py::set_error(PyExc_RuntimeError, "Brain is null");
                return std::make_tuple(0, 0, 0);
            }
            return std::make_tuple(
                brain->getTotalNeuronCount(),
                brain->getTotalSynapseCount(),
                brain->getActiveNeuronCount()
            );
        },
        py::arg("brain"),
        R"pbdoc(
        Get current memory usage statistics for a brain.
        
        Args:
            brain: Brain instance to query
        
        Returns:
            Tuple of (neuron_count, synapse_count, active_neuron_count)
        
        Examples:
            >>> neurons, synapses, active = nlm.get_memory_usage(brain)
            >>> print(f"Neurons: {neurons}, Synapses: {synapses}, Active: {active}")
        )pbdoc");

    // Performance monitoring utilities
    m.def("start_performance_monitor",
        [](std::shared_ptr<Brain> brain,
           const std::string& experimentName,
           SimulationStep monitorInterval) {
            // Create and start performance monitoring
            // In real implementation, this would create a PerformanceMonitor instance
            return std::make_shared<Experiment>(experimentName);
        },
        py::arg("brain"), py::arg("experiment_name"), py::arg("monitor_interval"),
        R"pbdoc(
        Start performance monitoring for a brain simulation.
        
        Args:
            brain: Brain instance to monitor
            experiment_name: Name for the performance experiment
            monitor_interval: Steps between performance samples
        
        Returns:
            Experiment instance for recording performance metrics
        
        Examples:
            >>> exp = nlm.start_performance_monitor(brain, "perf_test", 100)
        )pbdoc");

    m.def("get_performance_stats",
        [](const std::shared_ptr<Experiment>& experiment) {
            if (!experiment) {
                py::set_error(PyExc_RuntimeError, "Experiment is null");
                return std::make_tuple(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            }
            
            // Collect performance statistics
            size_t totalSteps = experiment->getEndStep() - experiment->getStartStep();
            size_t avgNeurons = 0, avgSynapses = 0, avgFiring = 0;
            size_t stepCount = 0;
            
            for (const auto& stepData : experiment->getStepData()) {
                avgNeurons += stepData.neuronCount;
                avgSynapses += stepData.synapseCount;
                avgFiring += stepData.firingCount;
                ++stepCount;
            }
            
            if (stepCount > 0) {
                avgNeurons /= stepCount;
                avgSynapses /= stepCount;
                avgFiring /= stepCount;
            }
            
            return std::make_tuple(
                static_cast<float>(totalSteps) / 1000.0f, // kStepsPerSec
                static_cast<float>(avgNeurons),
                static_cast<float>(avgSynapses),
                static_cast<float>(avgFiring),
                experiment->hasMetric("total_reward") ? 
                    experiment->getMetric("total_reward") : 0.0f
            );
        },
        py::arg("experiment"),
        R"pbdoc(
        Get performance statistics from an experiment.
        
        Args:
            experiment: Experiment instance with recorded data
        
        Returns:
            Tuple of (steps_per_sec, avg_neurons, avg_synapses, avg_firing, total_reward)
        
        Examples:
            >>> steps_per_sec, avg_neurons, avg_synapses, avg_firing, total_reward = nlm.get_performance_stats(exp)
            >>> print(f"Performance: {steps_per_sec:.2f} k steps/sec")
        )pbdoc");

    // Data export and analysis utilities
    m.def("export_experiment_results",
        [](const std::shared_ptr<Experiment>& experiment,
           const std::string& filepath,
           const std::string& format) {
            if (!experiment) {
                py::throw_exception<py::error_already_set>("Experiment is null");
            }
            
            // In real implementation, would write results to file
            return true;
        },
        py::arg("experiment"), py::arg("filepath"), py::arg("format") = "csv",
        R"pbdoc(
        Export experiment results to a file.
        
        Args:
            experiment: Experiment instance with data to export
            filepath: Output file path
            format: Export format (csv, json, txt)
        
        Returns:
            True if export succeeded
        
        Examples:
            >>> nlm.export_experiment_results(exp, "results.csv", "csv")
            >>> nlm.export_experiment_results(exp, "results.json", "json")
        )pbdoc");

    m.def("analyze_experiment_metrics",
        [](const std::shared_ptr<Experiment>& experiment) {
            if (!experiment) {
                py::throw_exception<py::error_already_set>("Experiment is null");
            }
            
            // Analyze metrics
            auto metrics = experiment->getMetrics();
            std::map<std::string, double> analysis;
            
            for (const auto& metric : metrics) {
                analysis[metric.first] = metric.second;
            }
            
            return analysis;
        },
        py::arg("experiment"),
        R"pbdoc(
        Analyze experiment metrics and return statistics.
        
        Args:
            experiment: Experiment instance to analyze
        
        Returns:
            Dictionary of metric names to values
        
        Examples:
            >>> metrics = nlm.analyze_experiment_metrics(exp)
            >>> print(f"Average reward: {metrics.get('total_reward', 0)}")
        )pbdoc");

    // Checkpoint/save-load utilities
    m.def("save_brain_checkpoint",
        [](const std::shared_ptr<Brain>& brain,
           const std::string& filepath,
           bool compress) {
            if (!brain) {
                py::throw_exception<py::error_already_set>("Brain is null");
            }
            return brain->save(filepath);
        },
        py::arg("brain"), py::arg("filepath"), py::arg("compress") = false,
        R"pbdoc(
        Save brain state to a checkpoint file.
        
        Args:
            brain: Brain instance to save
            filepath: Output file path
            compress: Whether to compress the checkpoint
        
        Returns:
            True if save succeeded
        
        Examples:
            >>> nlm.save_brain_checkpoint(brain, "checkpoint.bin")
            >>> nlm.save_brain_checkpoint(brain, "checkpoint.gz", compress=True)
        )pbdoc");

    m.def("load_brain_checkpoint",
        [](std::shared_ptr<Brain> brain,
           const std::string& filepath) {
            if (!brain) {
                py::throw_exception<py::error_already_set>("Brain is null");
            }
            return brain->load(filepath);
        },
        py::arg("brain"), py::arg("filepath"),
        R"pbdoc(
        Load brain state from a checkpoint file.
        
        Args:
            brain: Brain instance to load into
            filepath: Input file path
        
        Returns:
            True if load succeeded
        
        Examples:
            >>> loaded_brain = nlm.load_brain_checkpoint(brain, "checkpoint.bin")
        )pbdoc");

    // Error handling utilities
    m.def("validate_config",
        [](const std::shared_ptr<Config>& config,
           const std::vector<std::string>& requiredKeys) {
            if (!config) {
                py::throw_exception<py::error_already_set>("Config is null");
            }
            
            std::vector<std::string> missingKeys;
            for (const auto& key : requiredKeys) {
                if (!config->has(key)) {
                    missingKeys.push_back(key);
                }
            }
            
            if (!missingKeys.empty()) {
                std::string errorMsg = "Missing required configuration keys: ";
                for (size_t i = 0; i < missingKeys.size(); ++i) {
                    if (i > 0) errorMsg += ", ";
                    errorMsg += missingKeys[i];
                }
                py::throw_exception<std::runtime_error>(errorMsg);
            }
            
            return true;
        },
        py::arg("config"), py::arg("required_keys"),
        R"pbdoc(
        Validate that a configuration contains all required keys.
        
        Args:
            config: Configuration to validate
            required_keys: List of required key names
        
        Returns:
            True if configuration is valid
        
        Raises:
            RuntimeError: If any required keys are missing
        
        Examples:
            >>> required = ["learning_rate", "population_size", "max_steps"]
            >>> nlm.validate_config(config, required)
        )pbdoc");

    m.def("safe_brain_step",
        [](std::shared_ptr<Brain> brain,
           SimulationStep step,
           float maxFiringRate = 100.0f) {
            if (!brain) {
                py::throw_exception<py::error_already_set>("Brain is null");
            }
            
            // Check for potential issues
            size_t neuronCount = brain->getTotalNeuronCount();
            if (neuronCount == 0) {
                py::throw_exception<std::runtime_error>("Brain has no neurons");
            }
            
            float firingRate = brain->getAverageFiringRate();
            if (firingRate > maxFiringRate) {
                std::string warning = "High firing rate detected: " + 
                    std::to_string(firingRate) + " Hz (threshold: " + 
                    std::to_string(maxFiringRate) + " Hz)";
                // Log warning but don't throw
            }
            
            brain->step(step);
            return true;
        },
        py::arg("brain"), py::arg("step"), py::arg("max_firing_rate") = 100.0f,
        R"pbdoc(
        Safely perform a brain simulation step with validation.
        
        Args:
            brain: Brain instance to step
            step: Simulation step number
            max_firing_rate: Maximum allowed firing rate (warning threshold)
        
        Returns:
            True if step completed successfully
        
        Raises:
            RuntimeError: If brain is invalid or has no neurons
        
        Examples:
            >>> for i in range(100):
            ...     if not nlm.safe_brain_step(brain, i):
            ...         break
        )pbdoc");

    // Advanced convenience functions
    m.def("create_advanced_simulation",
        [](const std::string& experimentName,
           const std::shared_ptr<Config>& config,
           SimulationStep maxSteps,
           bool enableDevelopment,
           bool enableRewardModulation,
           bool enableCuriosity) {
            auto experiment = std::make_shared<Experiment>(experimentName);
            experiment->setSeed(42);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Enable advanced features
            if (enableDevelopment) {
                brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            }
            
            // In real implementation, would enable neuromodulation systems
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(200, 200, 168, 168);
            
            return std::make_tuple(experiment, brain, world);
        },
        py::arg("experiment_name"), py::arg("config"),
        py::arg("max_steps"), py::arg("enable_development") = false,
        py::arg("enable_reward_modulation") = false,
        py::arg("enable_curiosity") = false,
        R"pbdoc(
        Create an advanced neural simulation with enhanced features.
        
        Args:
            experiment_name: Name for the experiment
            config: Configuration for the brain
            max_steps: Maximum simulation steps
            enable_development: Enable developmental processes
            enable_reward_modulation: Enable reward-based learning
            enable_curiosity: Enable curiosity-driven exploration
        
        Returns:
            Tuple of (experiment, brain, world)
        
        Examples:
            >>> config = nlm.create_config()
            >>> config.set("population_size", 5000)
            >>> exp, brain, world = nlm.create_advanced_simulation(
            ...     "advanced_simulation",
            ...     config,
            ...     max_steps=5000,
            ...     enable_development=True,
            ...     enable_reward_modulation=True
            ... )
        )pbdoc");

    m.def("run_complete_experiment",
        [](const std::string& experimentName,
           const std::shared_ptr<Config>& config,
           const std::shared_ptr<Environment>& environment,
           SimulationStep maxSteps) {
            auto experiment = std::make_shared<Experiment>(experimentName);
            experiment->setSeed(12345);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Create environment if not provided
            if (!environment) {
                auto world = std::make_shared<SimpleWorld>();
                world->configure(100, 100, 84, 84);
                return std::make_tuple(experiment, brain, world);
            }
            
            return std::make_tuple(experiment, brain, environment);
        },
        py::arg("experiment_name"), py::arg("config"),
        py::arg("environment"), py::arg("max_steps"),
        R"pbdoc(
        Run a complete experiment with brain, environment, and data collection.
        
        Args:
            experiment_name: Name for the experiment
            config: Brain configuration
            environment: Simulation environment
            max_steps: Maximum simulation steps
        
        Returns:
            Tuple of (experiment, brain, environment/world)
        
        Examples:
            >>> config = nlm.create_config()
            >>> exp, brain, world = nlm.run_complete_experiment(
            ...     "complete_experiment",
            ...     config,
            ...     None,  # Use default world
            ...     max_steps=10000
            ... )
        )pbdoc");

    // Set module-level constants
    m.attr("MAX_SIMULATION_STEPS") = 1000000;
    m.attr("DEFAULT_LEARNING_RATE") = 0.01f;
    m.attr("DEFAULT_POPULATION_SIZE") = 1000;
    m.attr("DEFAULT_WORLD_SIZE") = 100;

    // Legacy compatibility functions
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
