// Enhanced Python bindings for NLM (Neural Learning Machine)
// 
// This file provides a comprehensive Python interface to the NLM C++ neural simulation framework.
// Key improvements:
// - Comprehensive convenience functions for common operations
// - Enhanced documentation and examples
// - Advanced features for experienced users
// - Better error handling and type conversions
// - Modular API design for ease of use

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
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

// =============================================================================
// UTILITY FUNCTIONS AND CONVENIENCE CLASSES
// =============================================================================

namespace utils {
    
    // Configuration convenience functions
    std::shared_ptr<Config> createDemoConfig(int neuronCount = 1000, 
                                           float connectionProbability = 0.1f) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("connection_probability", connectionProbability);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", static_cast<int64_t>(42));
        return config;
    }
    
    // Brain initialization with error checking
    std::shared_ptr<Brain> createAndInitializeBrain(std::shared_ptr<Config> config) {
        if (!config) {
            config = createDemoConfig();
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }
    
    // Calculate firing rate statistics
    std::pair<float, float> calculateFiringStats(const std::vector<float>& firingRates) {
        if (firingRates.empty()) {
            return {0.0f, 0.0f};
        }
        
        float sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f);
        float mean = sum / firingRates.size();
        
        float variance = 0.0f;
        for (float rate : firingRates) {
            variance += (rate - mean) * (rate - mean);
        }
        variance /= firingRates.size();
        
        float stddev = std::sqrt(variance);
        
        return {mean, stddev};
    }
    
    // Create standardized sensory input
    std::shared_ptr<Vision> createVisionInput(size_t width, size_t height, 
                                               const std::vector<float>& data = {}) {
        auto vision = std::make_shared<Vision>(width, height);
        if (!data.empty() && data.size() == width * height) {
            vision->setData(data);
        }
        return vision;
    }
    
    // Create standardized action
    Action createAction(ActionType type, const std::vector<float>& parameters = {}) {
        Action action(type, parameters);
        return action;
    }
    
    // Convert action type to string
    std::string actionTypeToString(ActionType type) {
        switch (type) {
            case ActionType::MoveForward: return "MoveForward";
            case ActionType::MoveBackward: return "MoveBackward";
            case ActionType::MoveLeft: return "MoveLeft";
            case ActionType::MoveRight: return "MoveRight";
            case ActionType::TurnLeft: return "TurnLeft";
            case ActionType::TurnRight: return "TurnRight";
            case ActionType::Look: return "Look";
            case ActionType::LookUp: return "LookUp";
            case ActionType::LookDown: return "LookDown";
            case ActionType::Interact: return "Interact";
            case ActionType::Eat: return "Eat";
            case ActionType::Drink: return "Drink";
            case ActionType::Rest: return "Rest";
            case ActionType::Wait: return "Wait";
            case ActionType::Custom: return "Custom";
            default: return "Unknown";
        }
    }
}

// =============================================================================
// ENHANCED CONFIGURATION CLASS
// =============================================================================

py::class_<Config> enhancedConfig(py::class_<Config> cls) {
    return cls
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Automatically detects JSON vs simple key=value format.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file (formatted for readability)")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get string value with default")
        .def("getOr", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get integer value with default")
        .def("getOr", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get float value with default")
        .def("getOr", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean value with default")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
}

// =============================================================================
// ENHANCED BRAIN CLASS
// =============================================================================

py::class_<Brain> enhancedBrain(py::class_<Brain> cls) {
    return cls
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a new brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with default timestep")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with specified timestamp")
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
             "Log brain status")
        // Additional convenience methods
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attention system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine neuromodulator")
        .def("getPredictionError", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error signal");
}

// =============================================================================
// ENHANCED AGENT BRAIN CLASS
// =============================================================================

py::class_<AgentBrain> enhancedAgentBrain(py::class_<AgentBrain> cls) {
    return cls
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create a new agent brain interface")
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
        // Subsystem enable/disable methods
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        // Query methods
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Convenience methods
        .def("runEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                           const std::function<void(AgentBrain&, std::shared_ptr<SimpleWorld>, size_t)>& agentFunction,
                           size_t numSteps) {
            self.initialize(*world);
            for (size_t step = 0; step < numSteps; ++step) {
                agentFunction(self, world, step);
            }
        }, py::arg("world"), py::arg("agentFunction"), py::arg("numSteps"),
            "Run a complete episode with custom agent function")
        .def("createActionSequence", [](AgentBrain& self, const std::vector<ActionType>& actionTypes) {
            std::vector<Action> actions;
            for (ActionType type : actionTypes) {
                actions.push_back(Action(type));
            }
            return actions;
        }, py::arg("actionTypes"),
            "Create a sequence of actions from action types");
}

// =============================================================================
// ENHANCED SIMPLE WORLD CLASS
// =============================================================================

py::class_<SimpleWorld> enhancedSimpleWorld(py::class_<SimpleWorld> cls) {
    return cls
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensory input")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Convenience methods
        .def("runSimulation", [](SimpleWorld& self, std::shared_ptr<Brain> brain,
                                const std::function<void(std::shared_ptr<SimpleWorld>, std::shared_ptr<Brain>, size_t)>& worldFunction,
                                size_t numSteps) {
            self.reset();
            for (size_t step = 0; step < numSteps; ++step) {
                self.update(0.1);  // Default timestep
                worldFunction(&self, brain, step);
            }
        }, py::arg("brain"), py::arg("worldFunction"), py::arg("numSteps"),
            "Run simulation with custom world function")
        .def("createExplorationPattern", [](SimpleWorld& self) {
            // Create a simple exploration pattern
            std::vector<py::object> actions;
            for (int i = 0; i < 10; ++i) {
                actions.push_back(py::cast(Action(ActionType::MoveForward)));
            }
            return actions;
        }, "Create a simple exploration pattern");
}

// =============================================================================
// CONVENIENCE FUNCTIONS AND FACTORIES
// =============================================================================

void addConvenienceFunctions(py::module_& m) {
    // Configuration factories
    m.def("createDemoConfig", &utils::createDemoConfig,
          py::arg("neuronCount") = 1000,
          py::arg("connectionProbability") = 0.1f,
          "Create a demo configuration with sensible defaults");
    
    m.def("createAndInitializeBrain", &utils::createAndInitializeBrain,
          py::arg("config"),
          "Create and initialize a brain with error checking");
    
    // Utility functions
    m.def("calculateFiringStats", &utils::calculateFiringStats,
          py::arg("firingRates"),
          "Calculate mean and standard deviation of firing rates");
    
    m.def("createVisionInput", &utils::createVisionInput,
          py::arg("width"), py::arg("height"), py::arg("data") = std::vector<float>(),
          "Create a vision sensory input");
    
    m.def("createAction", &utils::createAction,
          py::arg("type"), py::arg("parameters") = std::vector<float>(),
          "Create an action with type and parameters");
    
    m.def("actionTypeToString", &utils::actionTypeToString,
          py::arg("type"),
          "Convert action type to string representation");
    
    // Configuration loading shortcuts
    m.def("loadConfigJSON", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load config from " + filepath);
        }
        return config;
    }, py::arg("filepath"),
        "Load configuration from JSON file with error checking");
    
    m.def("loadConfigSimple", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load config from " + filepath);
        }
        return config;
    }, py::arg("filepath"),
        "Load configuration from simple key=value format with error checking");
    
    // Brain testing utilities
    m.def("runBrainTestSuite", [](std::shared_ptr<Brain> brain) {
        SimpleTest::runAll(brain);
    }, py::arg("brain"),
        "Run comprehensive test suite on brain");
    
    // Serialization utilities
    m.def("saveBrainWithTimestamp", [](std::shared_ptr<Brain> brain, const std::string& basePath) {
        auto timestamp = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        std::string filename = basePath + "_" + ss.str() + ".bin";
        if (!brain->save(filename)) {
            throw std::runtime_error("Failed to save brain to " + filename);
        }
        return filename;
    }, py::arg("brain"), py::arg("basePath"),
        "Save brain with timestamp in filename");
    
    // Comparison and analysis utilities
    m.def("compareBrains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) {
        std::map<std::string, double> comparison;
        comparison["neuron_count_diff"] = abs(static_cast<int>(brain1->getTotalNeuronCount() - 
                                                           brain2->getTotalNeuronCount()));
        comparison["synapse_count_diff"] = abs(static_cast<int>(brain1->getTotalSynapseCount() - 
                                                             brain2->getTotalSynapseCount()));
        comparison["firing_rate_diff"] = abs(brain1->getAverageFiringRate() - 
                                             brain2->getAverageFiringRate());
        comparison["e_i_ratio_diff"] = abs(brain1->getExcitationInhibitionRatio() - 
                                           brain2->getExcitationInhibitionRatio());
        comparison["total_spikes_diff"] = abs(static_cast<int>(brain1->getTotalSpikeCount() - 
                                                             brain2->getTotalSpikeCount()));
        return comparison;
    }, py::arg("brain1"), py::arg("brain2"),
        "Compare two brains and return key differences");
    
    // Experimental utilities
    m.def("createLearningExperiment", [](std::shared_ptr<Brain> brain, uint64_t seed = 42) {
        struct LearningExperiment {
            std::shared_ptr<Brain> brain;
            uint64_t seed;
            size_t initialSynapseCount;
            std::vector<float> initialWeights;
            std::vector<float> finalWeights;
            std::vector<NeuronId> mostActiveNeurons;
            
            LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
                : brain(b), seed(s), initialSynapseCount(0) {}
            
            void recordInitialState() {
                initialSynapseCount = brain->getTotalSynapseCount();
                initialWeights.clear();
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        initialWeights.push_back(syn->getWeight());
                    }
                }
            }
            
            void recordFinalState() {
                finalWeights.clear();
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        finalWeights.push_back(syn->getWeight());
                    }
                }
                mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
            }
            
            double calculateLearningScore() {
                if (initialWeights.empty() || finalWeights.empty()) {
                    return 0.0;
                }
                float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float initialMean = initialSum / initialWeights.size();
                float finalMean = finalSum / finalWeights.size();
                return std::abs(finalMean - initialMean);
            }
            
            std::string getLearningDescription() {
                double score = calculateLearningScore();
                if (score > 0.01) {
                    return "Significant learning detected";
                } else if (score > 0.001) {
                    return "Moderate learning detected";
                } else {
                    return "Minimal learning detected";
                }
            }
        };
        
        return LearningExperiment(brain, seed);
    }, py::arg("brain"), py::arg("seed") = 42,
        "Create a learning experiment structure");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------------------
        
        An enhanced Python interface for the NLM C++ neural simulation framework.
        
        Key improvements over the standard bindings:
        
        • Comprehensive convenience functions for common operations
        • Enhanced error handling and type safety
        • Advanced features for experienced users
        • Better documentation and examples
        • Modular API design for easy extension
        
        New features in this enhanced version:
        
        1. Configuration Utilities:
           - createDemoConfig() for quick setup
           - Enhanced error checking in file loading
           - Type-safe configuration setters
        
        2. Brain Testing:
           - SimpleTest framework for comprehensive testing
           - Built-in test suites (connectivity, plasticity, STDP)
           - Detailed test reporting
        
        3. Agent Development:
           - Enhanced AgentBrain with convenience methods
           - runEpisode() for easy agent training
           - Action sequence creation utilities
        
        4. World Simulation:
           - Enhanced SimpleWorld with utilities
           - runSimulation() for custom world logic
           - Exploration pattern generation
        
        5. Advanced Features:
           - Timestamped brain saving
           - Brain comparison utilities
           - Learning experiment structures
           - Statistical analysis functions
        
        6. Error Handling:
           - Comprehensive exception handling
           - Validation functions
           - Detailed error messages
        
        Usage Examples:
        
        Basic Usage:
            import pynlm
            
            # Create configuration
            config = pynlm.createDemoConfig(2000, 0.1)
            
            # Create and initialize brain
            brain = pynlm.createAndInitializeBrain(config)
            
            # Run simulation
            for step in range(1000):
                brain.step(step)
        
        Advanced Agent Training:
            import pynlm
            
            # Setup agent
            config = pynlm.createDemoConfig()
            brain = pynlm.createAndInitializeBrain(config)
            world = pynlm.createSimpleWorld()
            world.configure(20, 20, 8, 8)
            agent = pynlm.createAgentBrain(brain)
            
            # Run episode
            def agent_function(agent, world, step):
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.runEpisode(world, agent_function, 500)
        
        Learning Experiments:
            import pynlm
            
            config = pynlm.createDemoConfig()
            brain = pynlm.createAndInitializeBrain(config)
            
            # Create learning experiment
            experiment = pynlm.createLearningExperiment(brain)
            
            # Enable plasticity
            brain->enablePlasticity(true, true, false)
            
            # Run learning trial
            for step in range(1000):
                brain->step(step)
            
            # Record results
            experiment.recordFinalState()
            print(f"Learning score: {experiment.calculateLearningScore()}")
            print(f"Learning description: {experiment.getLearningDescription()}")
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // Register exception classes for Python
    m.def("raise_error", [](const std::string& msg) {
        throw std::runtime_error(msg);
    }, py::arg("message"), "Raise a runtime error");
    
    m.def("raise_value_error", [](const std::string& msg) {
        throw std::invalid_argument(msg);
    }, py::arg("message"), "Raise a value error");

    // Register all enhanced classes
    enhancedConfig(py::class_<Config>(m, "Config", "Enhanced configuration class"));
    enhancedBrain(py::class_<Brain>(m, "Brain", "Enhanced brain class"));
    enhancedAgentBrain(py::class_<AgentBrain>(m, "AgentBrain", "Enhanced agent brain class"));
    enhancedSimpleWorld(py::class_<SimpleWorld>(m, "SimpleWorld", "Enhanced simple world class"));

    // Register original classes for backward compatibility
    py::class_<NeuronId>(m, "NeuronId", "Neuron identifier")
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

    py::class_<SynapseId>(m, "SynapseId", "Synapse identifier")
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

    py::class_<RegionId>(m, "RegionId", "Region identifier")
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

    py::class_<PopulationId>(m, "PopulationId", "Population identifier")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", "Neuron type enumeration")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", "Synapse type enumeration")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", "Firing state enumeration")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", "Action type enumeration")
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

    py::enum_<MotorCommand>(m, "MotorCommand", "Motor command enumeration")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", "World object type enumeration")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Original sensory input classes
    py::class_<SensoryInput>(m, "SensoryInput", "Base class for sensory input")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", "Vision sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", "Audio sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", "Internal signals sensory input")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", "Action representation for motor output")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", "World object representation")
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

    py::class_<AgentBody>(m, "AgentBody", "Agent body state")
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

    py::class_<ActionResult>(m, "ActionResult", "Action result from world")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", "Sensory percept data")
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

    py::class_<SimpleWorld>(m, "SimpleWorld", "Simple 2D world for NLM simulation")
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

    // Factory functions
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

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add convenience functions
    addConvenienceFunctions(m);

    // Module metadata
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------------------
        
        A comprehensive Python interface to the NLM C++ neural simulation framework.
        This enhanced version provides additional convenience functions, better error handling,
        and advanced features for both beginners and experienced users.
        
        Key Features:
        • Comprehensive convenience functions for common operations
        • Enhanced error handling with detailed error messages
        • Advanced configuration utilities
        • Enhanced brain testing framework
        • Agent development utilities
        • World simulation enhancements
        • Advanced features for experienced users
        • Better documentation and examples
        
        Installation:
            pip install .
        
        Basic Usage:
            import pynlm
            
            # Create and initialize brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            
            # Enable plasticity
            brain.enablePlasticity(true, true, false)
            
            # Run simulation
            for step in range(1000):
                brain.step(step)
        
        Advanced Agent Usage:
            import pynlm
            
            # Setup agent
            config = pynlm.createDemoConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            world.configure(20, 20, 8, 8)
            agent = pynlm.createAgentBrain(brain)
            
            # Run episode
            def agent_function(agent, world, step):
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.runEpisode(world, agent_function, 500)
        
        Learning Experiments:
            import pynlm
            
            # Create learning experiment
            config = pynlm.createDemoConfig()
            brain = pynlm.createBrain(config)
            experiment = pynlm.createLearningExperiment(brain)
            
            # Run learning trial
            for step in range(1000):
                brain.step(step)
            
            # Analyze results
            experiment.recordFinalState()
            print(f"Learning score: {experiment.calculateLearningScore()}")
            print(f"Learning description: {experiment.getLearningDescription()}")
    )pbdoc";

    // Version information
    m.attr("__version__") = "6.0.0-enhanced";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Enhanced NLM Python bindings with comprehensive convenience functions and advanced features";
}

} // namespace nlm
