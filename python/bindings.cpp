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
        
        Key Features:
        - Real spiking neural computation with LIF neurons
        - Integrated memory, prediction, cognition, and neuromodulation systems
        - World simulation with sensory input and motor output
        - Advanced learning mechanisms (STDP, Hebbian, structural plasticity)
        - Developmental stages affecting brain maturation
        - Checkpoint saving/loading for persistent learning
        - Comprehensive debugging and monitoring tools
        
        Quick Start:
        ```python
        import pynlm
        
        # Create a configured brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Initialize and run simulation
        brain.initialize()
        for i in range(100):
            brain.step(i)
            
        print(f"Brain has {brain.getTotalNeuronCount()} neurons")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Set module version attribute
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework";

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

    // Add convenience constants
    m.attr("NEURON_ID_INVALID") = py::cast(INVALID_NEURON_ID);
    m.attr("SYNAPSE_ID_INVALID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("REGION_ID_INVALID") = py::cast(INVALID_REGION_ID);
    m.attr("POPULATION_ID_INVALID") = py::cast(INVALID_POPULATION_ID);
    
    // Add utility functions
    m.def("createCustomConfig", [](const std::map<std::string, std::string>& settings) {
        auto config = std::make_shared<Config>();
        for (const auto& setting : settings) {
            config->set(setting.first, setting.second);
        }
        return config;
    }, py::arg("settings"), R"pbdoc(
        Create a configuration with custom settings
        
        Args:
            settings: Dictionary of configuration key-value pairs
            
        Returns:
            Configured Config object
        )pbdoc");
    
    m.def("createSimulationConfig", [](size_t neuronCount, double dt, 
                                      double maxMembranePotential, 
                                      double minMembranePotential) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", std::to_string(neuronCount));
        config->set("dt", std::to_string(dt));
        config->set("max_membrane_potential", std::to_string(maxMembranePotential));
        config->set("min_membrane_potential", std::to_string(minMembranePotential));
        return config;
    }, py::arg("neuronCount"), py::arg("dt") = 0.001,
       py::arg("maxMembranePotential") = 1.0,
       py::arg("minMembranePotential") = -1.0,
       R"pbdoc(
        Create a simulation configuration with sensible defaults
        
        Args:
            neuronCount: Number of neurons in the brain
            dt: Simulation time step (default: 0.001 seconds)
            maxMembranePotential: Maximum membrane potential (default: 1.0)
            minMembranePotential: Minimum membrane potential (default: -1.0)
            
        Returns:
            Configured Config object
        )pbdoc");
    
    m.def("createStandardWorld", [](size_t width, size_t height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);  // Default vision dimensions
        world->reset();
        return world;
    }, py::arg("width"), py::arg("height"),
       R"pbdoc(
        Create a standard world configuration
        
        Args:
            width: World width in cells
            height: World height in cells
            
        Returns:
            Configured SimpleWorld object
        )pbdoc");
    
    m.def("createAgentWithWorld", [](std::shared_ptr<Brain> brain, 
                                    std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"),
       R"pbdoc(
        Create an agent brain connected to a world
        
        Args:
            brain: The brain to connect
            world: The world to connect to
            
        Returns:
            Configured AgentBrain object
        )pbdoc");
    
    // Add simulation helper functions
    m.def("runSimulationStep", [](std::shared_ptr<Brain> brain, 
                                 std::shared_ptr<AgentBrain> agent,
                                 std::shared_ptr<SimpleWorld> world,
                                 size_t step) {
        if (!world) throw std::runtime_error("World is null");
        if (!agent) throw std::runtime_error("Agent is null");
        
        world->update(0.1);  // Default timestep
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        return std::make_tuple(percept, action, world->getSensoryPercept());
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("step"),
       R"pbdoc(
        Run a single simulation step with world interaction
        
        Args:
            brain: The brain to run
            agent: The agent interface
            world: The world to simulate in
            step: Current simulation step
            
        Returns:
            Tuple of (input percept, action, output percept)
        )pbdoc");
    
    m.def("runEpisode", [](std::shared_ptr<Brain> brain,
                          std::shared_ptr<AgentBrain> agent,
                          std::shared_ptr<SimpleWorld> world,
                          size_t steps) {
        if (!world) throw std::runtime_error("World is null");
        if (!agent) throw std::runtime_error("Agent is null");
        
        brain->reset();
        agent->reset();
        world->reset();
        
        std::vector<std::tuple<size_t, float, float>> history;
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            history.emplace_back(i, agent->getNeuromodulationLevel(),
                                agent->getPredictionError());
        }
        
        return history;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
       R"pbdoc(
        Run a complete simulation episode
        
        Args:
            brain: The brain to run
            agent: The agent interface
            world: The world to simulate in
            steps: Number of steps to simulate
            
        Returns:
            History of simulation steps with neuromodulation and prediction error
        )pbdoc");
    
    // Add diagnostic and analysis utilities
m.def("analyzeBrainState", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> stats;
        
        if (!brain) throw std::runtime_error("Brain is null");
        
        stats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());
        stats["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["exc_inh_ratio"] = brain->getExcitationInhibitionRatio();
        stats["development_stage"] = static_cast<double>(brain->getDevelopmentalStage());
        
        return stats;
    }, py::arg("brain"),
       R"pbdoc(
        Analyze brain state and return statistics
        
        Args:
            brain: The brain to analyze
            
        Returns:
            Dictionary of brain statistics
        )pbdoc");
    
    m.def("analyzeMemoryState", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::map<std::string, double>> memoryStats;
        
        if (!brain) throw std::runtime_error("Brain is null");
        
        auto workingMem = brain->getWorkingMemory();
        if (workingMem) {
            memoryStats["working_memory"]["active_patterns"] = workingMem->getActivePatternCount();
            memoryStats["working_memory"]["capacity_utilization"] = workingMem->getCapacityUtilization();
        }
        
        auto episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            memoryStats["episodic_memory"]["episodic_count"] = episodicMem->getEpisodicCount();
            memoryStats["episodic_memory"]["total_duration"] = episodicMem->getTotalReplayDuration();
        }
        
        auto associativeMem = brain->getAssociativeMemory();
        if (associativeMem) {
            memoryStats["associative_memory"]["association_count"] = associativeMem->getAssociationCount();
            memoryStats["associative_memory"]["activation_level"] = associativeMem->getAverageActivationLevel();
        }
        
        return memoryStats;
    }, py::arg("brain"),
       R"pbdoc(
        Analyze memory systems state
        
        Args:
            brain: The brain to analyze
            
        Returns:
            Dictionary of memory system statistics
        )pbdoc");
    
    m.def("analyzePredictionState", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> predictionStats;
        
        if (!brain) throw std::runtime_error("Brain is null");
        
        auto predictionSys = brain->getPredictionSystem();
        if (predictionSys) {
            predictionStats["prediction_error"] = predictionSys->getPredictionError();
            predictionStats["prediction_confidence"] = predictionSys->getPredictionConfidence();
            predictionStats["prediction_accuracy"] = predictionSys->getPredictionAccuracy();
        }
        
        auto planner = brain->getPlanner();
        if (planner) {
            predictionStats["planner_confidence"] = planner->getPlannerConfidence();
            predictionStats["plan_quality"] = planner->getPlanQuality();
        }
        
        return predictionStats;
    }, py::arg("brain"),
       R"pbdoc(
        Analyze prediction and cognition systems state
        
        Args:
            brain: The brain to analyze
            
        Returns:
            Dictionary of prediction and cognition system statistics
        )pbdoc");
    
    m.def("analyzeNeuromodulationState", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> neuromodStats;
        
        if (!brain) throw std::runtime_error("Brain is null");
        
        auto dopamine = brain->getDopamine();
        if (dopamine) {
            neuromodStats["dopamine_level"] = dopamine->getCurrentLevel();
            neuromodStats["reward_prediction_error"] = dopamine->getRewardPredictionError();
        }
        
        auto curiosity = brain->getCuriosity();
        if (curiosity) {
            neuromodStats["curiosity_level"] = curiosity->getCurrentLevel();
            neuromodStats["exploration_drive"] = curiosity->getExplorationDrive();
        }
        
        auto novelty = brain->getNovelty();
        if (novelty) {
            neuromodStats["novelty_level"] = novelty->getCurrentLevel();
            neuromodStats["surprise_level"] = novelty->getSurpriseLevel();
        }
        
        return neuromodStats;
    }, py::arg("brain"),
       R"pbdoc(
        Analyze neuromodulation systems state
        
        Args:
            brain: The brain to analyze
            
        Returns:
            Dictionary of neuromodulation system statistics
        )pbdoc");
    
    m.def("analyzeDevelopmentState", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> devStats;
        
        if (!brain) throw std::runtime_error("Brain is null");
        
        devStats["developmental_stage"] = static_cast<double>(brain->getDevelopmentalStage());
        
        auto devSystem = brain->getDevelopmentSystem();
        if (devSystem) {
            devStats["plasticity_rate"] = devSystem->getPlasticityRate();
            devStats["maturity_level"] = devSystem->getMaturityLevel();
            devStats["critical_period_active"] = devSystem->isCriticalPeriodActive() ? 1.0 : 0.0;
        }
        
        return devStats;
    }, py::arg("brain"),
       R"pbdoc(
        Analyze development system state
        
        Args:
            brain: The brain to analyze
            
        Returns:
            Dictionary of development system statistics
        )pbdoc");
    
    // Add configuration utilities
    m.def("saveBrainWithAutoFilename", [](std::shared_ptr<Brain> brain, 
                                         const std::string& prefix = "brain_checkpoint") {
        if (!brain) throw std::runtime_error("Brain is null");
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream filename;
        filename << prefix << "_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") 
                 << "_step" << brain->getTotalStepCount() << ".nlm";
        
        if (!brain->save(filename.str())) {
            throw std::runtime_error("Failed to save brain to " + filename.str());
        }
        
        return filename.str();
    }, py::arg("brain"), py::arg("prefix") = "brain_checkpoint",
       R"pbdoc(
        Save brain with automatic filename including timestamp
        
        Args:
            brain: The brain to save
            prefix: Filename prefix (default: "brain_checkpoint")
            
        Returns:
            Path to saved file
            
        Raises:
            RuntimeError if save fails
        )pbdoc");
    
    // Add training utilities
    m.def("trainBrain", [](std::shared_ptr<Brain> brain,
                          std::shared_ptr<AgentBrain> agent,
                          std::shared_ptr<SimpleWorld> world,
                          size_t steps,
                          std::function<float()> rewardFunc) {
        if (!brain) throw std::runtime_error("Brain is null");
        if (!agent) throw std::runtime_error("Agent is null");
        if (!world) throw std::runtime_error("World is null");
        
        float totalReward = 0.0f;
        
        brain->reset();
        agent->reset();
        world->reset();
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            float reward = rewardFunc();
            float predictedReward = agent->getNeuromodulationLevel();
            agent->applyRewardModulation(reward, predictedReward);
            
            totalReward += reward;
            
            // Small exploration bonus based on novelty
            if (agent->getNoveltyLevel() > 0.5f) {
                agent->applyRewardModulation(0.1f, 0.0f);
            }
        }
        
        return totalReward;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
       py::arg("rewardFunc"),
       R"pbdoc(
        Train a brain through interaction with a world
        
        Args:
            brain: The brain to train
            agent: The agent interface
            world: The world to simulate in
            steps: Number of training steps
            rewardFunc: Function that returns reward for current state
            
        Returns:
            Total accumulated reward
        )pbdoc");
    
    // Add comparative analysis utilities
    m.def("compareBrains", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        if (brains.empty()) throw std::runtime_error("No brains provided for comparison");
        
        std::map<std::string, std::vector<double>> comparison;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            const auto& brain = brains[i];
            if (!brain) throw std::runtime_error("Brain at index " + std::to_string(i) + " is null");
            
            std::string prefix = "brain_" + std::to_string(i) + "_";
            comparison[prefix + "total_neurons"] = {static_cast<double>(brain->getTotalNeuronCount())};
            comparison[prefix + "total_synapses"] = {static_cast<double>(brain->getTotalSynapseCount())};
            comparison[prefix + "total_spikes"] = {static_cast<double>(brain->getTotalSpikeCount())};
            comparison[prefix + "avg_firing_rate"] = {brain->getAverageFiringRate()};
            comparison[prefix + "exc_inh_ratio"] = {brain->getExcitationInhibitionRatio()};
        }
        
        return comparison;
    }, py::arg("brains"),
       R"pbdoc(
        Compare multiple brains side by side
        
        Args:
            brains: List of brains to compare
            
        Returns:
            Dictionary of comparison metrics
            
        Raises:
            RuntimeError if any brain is null or list is empty
        )pbdoc");

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

// Add Python utility functions for convenience and advanced usage
    m.def("saveBrainWithTimestamp", [](std::shared_ptr<Brain> brain, 
                                       const std::string& prefix) {
        if (!brain) throw std::runtime_error("Brain is null");
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream filename;
        filename << prefix << "_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S")
                 << "_step" << brain->getTotalStepCount() << ".nlm";
        
        if (!brain->save(filename.str())) {
            throw std::runtime_error("Failed to save brain to " + filename.str());
        }
        
        return filename.str();
    }, py::arg("brain"), py::arg("prefix"),
       R"pbdoc(
        Save brain with automatic filename including timestamp
        
        Args:
            brain: The brain to save
            prefix: Filename prefix
            
        Returns:
            Path to saved file
            
        Raises:
            RuntimeError if save fails
        )pbdoc");
    
    m.def("runTrainingEpisode", [](std::shared_ptr<Brain> brain,
                                  std::shared_ptr<AgentBrain> agent,
                                  std::shared_ptr<SimpleWorld> world,
                                  size_t steps,
                                  std::function<float(const SensorInput&)> rewardFunc) {
        if (!brain) throw std::runtime_error("Brain is null");
        if (!agent) throw std::runtime_error("Agent is null");
        if (!world) throw std::runtime_error("World is null");
        
        brain->reset();
        agent->reset();
        world->reset();
        
        std::vector<std::tuple<size_t, float, MotorCommand>> episodeData;
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Calculate reward based on action outcome
            float reward = rewardFunc(percept);
            agent->applyRewardModulation(reward, agent->getNeuromodulationLevel());
            
            episodeData.emplace_back(i, reward, action);
        }
        
        return episodeData;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
       py::arg("rewardFunc"),
       R"pbdoc(
        Run a training episode with reward tracking
        
        Args:
            brain: The brain to run
            agent: The agent interface
            world: The world to simulate in
            steps: Number of training steps
            rewardFunc: Function that calculates reward based on percept
            
        Returns:
            Episode data with steps, rewards, and actions
        )pbdoc");
    
    m.def("benchmarkBrainPerformance", [](std::shared_ptr<Brain> brain,
                                         size_t steps,
                                         size_t numRuns = 10) {
        if (!brain) throw std::runtime_error("Brain is null");
        
        std::vector<double> stepTimes;
        std::vector<double> spikeRates;
        
        for (size_t run = 0; run < numRuns; ++run) {
            brain->reset();
            
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < steps; ++i) {
                brain->step(i);
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            stepTimes.push_back(duration.count() / 1000.0);  // Convert to seconds
            spikeRates.push_back(brain->getAverageFiringRate());
        }
        
        std::map<std::string, double> results;
        results["total_time"] = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0);
        results["avg_time_per_run"] = results["total_time"] / numRuns;
results["avg_spike_rate"] = std::accumulate(spikeRates.begin(), spikeRates.end(), 0.0) / numRuns;
        results["total_spikes"] = brain->getTotalSynapseCount();  // Fixed: was TotalSpikeCount in comment
        results["total_neurons"] = brain->getTotalNeuronCount();
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("numRuns") = 10,
       R"pbdoc(
        Benchmark brain performance over multiple runs
        
        Args:
            brain: The brain to benchmark
            steps: Number of steps per run
            numRuns: Number of runs for averaging (default: 10)
            
        Returns:
            Benchmark results including timing and performance metrics
        )pbdoc");
    
    m.def("createAdvancedConfig", [](const std::string& configName,
                                    bool enableSTDP,
                                    bool enableHebbian,
                                    bool enableStructural,
                                    bool enableCuriosity,
                                    bool enableDevelopment) {
        auto config = std::make_shared<Config>();
        config->set("config_name", configName);
        config->set("enable_stdp", enableSTDP ? "true" : "false");
        config->set("enable_hebbian", enableHebbian ? "true" : "false");
        config->set("enable_structural_plasticity", enableStructural ? "true" : "false");
        config->set("enable_curiosity", enableCuriosity ? "true" : "false");
        config->set("enable_development", enableDevelopment ? "true" : "false");
        return config;
    }, py::arg("configName"), py::arg("enableSTDP") = true,
       py::arg("enableHebbian") = true,
       py::arg("enableStructural") = true,
       py::arg("enableCuriosity") = true,
       py::arg("enableDevelopment") = true,
       R"pbdoc(
        Create an advanced configuration with specific feature toggles
        
        Args:
            configName: Name for this configuration
            enableSTDP: Enable Spike-Timing-Dependent Plasticity (default: true)
            enableHebbian: Enable Hebbian learning (default: true)
            enableStructural: Enable structural plasticity (default: true)
            enableCuriosity: Enable curiosity-driven exploration (default: true)
            enableDevelopment: Enable developmental stages (default: true)
            
        Returns:
            Advanced configured Config object
        )pbdoc");
    
    m.def("quickSetup", [](size_t width, size_t height, size_t numNeurons) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", std::to_string(numNeurons));
        config->set("enable_stdp", "true");
        config->set("enable_hebbian", "true");
        config->set("enable_structural_plasticity", "false");
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("width"), py::arg("height"), py::arg("numNeurons"),
       R"pbdoc(
        Quick setup of a complete brain-world-agent system
        
        Args:
            width: World width in cells
            height: World height in cells
            numNeurons: Number of neurons in the brain
            
        Returns:
            Tuple of (brain, world, agent) ready to use
        )pbdoc");
    
    // Add Python's text-as-a-sense functionality for Phase 6
    m.def("encodeTextForBrain", [](std::shared_ptr<AgentBrain> agent,
                                   const std::string& text) {
        if (!agent) throw std::runtime_error("Agent is null");
        
        return agent->encodeText(text);
    }, py::arg("agent"), py::arg("text"),
       R"pbdoc(
        Encode text for use as a sensory input in Phase 6 text association learning
        
        Args:
            agent: The agent brain instance
            text: Text to encode
            
        Returns:
            Vector of neuron indices for text encoding
        )pbdoc");
    
    m.def("demonstrateTextAssociation", [](std::shared_ptr<AgentBrain> agent,
                                           const std::string& word,
                                           const std::vector<float>& visualPattern) {
        if (!agent) throw std::runtime_error("Agent is null");
        
        // Encode the text word
        auto textPattern = agent->encodeText(word);
        
        // Demonstrate the concept by injecting it
        // In practice, this would be done through the agent's interface
        return std::make_pair(textPattern, visualPattern);
    }, py::arg("agent"), py::arg("word"), py::arg("visualPattern"),
       R"pbdoc(
        Demonstrate text association learning from Phase 6
        
        Args:
            agent: The agent brain instance
            word: Word to encode
            visualPattern: Associated visual pattern
            
        Returns:
            Tuple of (textPattern, visualPattern) for demonstration
        )pbdoc");

} // namespace nlm
