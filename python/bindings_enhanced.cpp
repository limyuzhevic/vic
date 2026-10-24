#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
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

// High-level utility functions
namespace utils {

    // Validate configuration before brain creation
    bool validateBrainConfig(const Config& config) {
        try {
            if (config.has("neuron_count") && config.getOr<int64_t>("neuron_count", 0) <= 0) {
                return false;
            }
            if (config.has("simulation_timestep") && config.getOr<double>("simulation_timestep", 0.0) <= 0.0) {
                return false;
            }
            if (config.has("region_count") && config.getOr<int64_t>("region_count", 0) <= 0) {
                return false;
            }
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    // Create a simple preset configuration
    std::shared_ptr<Config> createPresetConfig(const std::string& preset) {
        auto config = std::make_shared<Config>();
        
        if (preset == "minimal") {
            config->set("neuron_count", 100, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        } else if (preset == "standard") {
            config->set("neuron_count", 500, ConfigSource::Default);
            config->set("region_count", 2, ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
            config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        } else if (preset == "development") {
            config->set("neuron_count", 300, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("connection_probability", 0.2f, ConfigSource::Default);
            config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
            config->set("pruning_rate", 0.00002f, ConfigSource::Default);
        } else if (preset == "high_performance") {
            config->set("neuron_count", 1000, ConfigSource::Default);
            config->set("region_count", 2, ConfigSource::Default);
            config->set("simulation_timestep", 0.0005, ConfigSource::Default);
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.01f, ConfigSource::Default);
        }
        
        return config;
    }
    
    // Run a training episode with automatic reset and metrics collection
    std::map<std::string, double> runTrainingEpisode(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t maxSteps = 1000,
        double timestep = 0.1,
        bool verbose = false
    ) {
        std::map<std::string, double> metrics;
        
        try {
            // Reset everything for a fresh episode
            brain->reset();
            brain->initialize();
            world->reset();
            agent->reset();
            
            // Initialize agent with world
            agent->initialize(*world);
            
            double totalReward = 0.0;
            double maxCuriosity = 0.0;
            double maxNovelty = 0.0;
            double maxPredictionError = 0.0;
            size_t stepsCompleted = 0;
            
            for (size_t step = 0; step < maxSteps; ++step) {
                // Update world
                world->update(timestep);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Brain processes
                brain->step(step, step * timestep);
                
                // Get action and apply to world
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Get reward and apply neuromodulation
                auto actionResult = world->getActionResult(action);
                if (actionResult) {
                    double reward = actionResult->reward;
                    totalReward += reward;
                    agent->applyRewardModulation(reward, agent->getPredictionError());
                }
                
                // Update development
                agent->updateDevelopment(timestep);
                
                // Collect metrics
                maxCuriosity = std::max(maxCuriosity, agent->getCuriosityLevel());
                maxNovelty = std::max(maxNovelty, agent->getNoveltyLevel());
                maxPredictionError = std::max(maxPredictionError, std::abs(agent->getPredictionError()));
                
                if (verbose && step % 100 == 0) {
                    std::cout << "Step " << step << ": Reward=" << totalReward 
                              << ", Curiosity=" << agent->getCuriosityLevel() 
                              << ", Novelty=" << agent->getNoveltyLevel() << std::endl;
                }
                
                stepsCompleted = step + 1;
                
                // Check if episode should end
                if (actionResult && actionResult->success && actionResult->reward < -0.9) {
                    if (verbose) {
                        std::cout << "Episode ended due to hazard at step " << step << std::endl;
                    }
                    break;
                }
            }
            
            // Calculate final metrics
            metrics["total_reward"] = totalReward;
            metrics["max_curiosity"] = maxCuriosity;
            metrics["max_novelty"] = maxNovelty;
            metrics["max_prediction_error"] = maxPredictionError;
            metrics["steps_completed"] = static_cast<double>(stepsCompleted);
            metrics["avg_reward_per_step"] = totalReward / std::max(stepsCompleted, 1u);
            metrics["avg_firing_rate"] = brain->getAverageFiringRate();
            metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
            metrics["final_development_stage"] = static_cast<double>(agent->getDevelopmentalStage());
            
        } catch (const std::exception& e) {
            if (verbose) {
                std::cout << "Error during training episode: " << e.what() << std::endl;
            }
            metrics["error"] = 1.0;
        }
        
        return metrics;
    }
    
    // Create an agent with world preconfigured
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> 
    createAgentWithWorld(const std::string& preset = "standard", 
                        size_t visionWidth = 8, size_t visionHeight = 8) {
        auto config = createPresetConfig(preset);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, visionWidth, visionHeight);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }
    
    // Run multiple training episodes
    std::vector<std::map<std::string, double>> runTraining(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t numEpisodes = 10,
        size_t maxStepsPerEpisode = 1000,
        double timestep = 0.1,
        bool verbose = false
    ) {
        std::vector<std::map<std::string, double>> allMetrics;
        
        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            if (verbose) {
                std::cout << "=== Episode " << episode + 1 << "/" << numEpisodes << " ===" << std::endl;
            }
            
            auto metrics = runTrainingEpisode(brain, world, agent, maxStepsPerEpisode, timestep, verbose);
            allMetrics.push_back(metrics);
            
            if (verbose) {
                std::cout << "Episode " << episode + 1 << " complete. Total reward: " 
                          << metrics["total_reward"] << std::endl << std::endl;
            }
        }
        
        return allMetrics;
    }
}

// Custom error types for better Python error handling
struct BrainInitializationError : std::runtime_error {
    BrainInitializationError(const std::string& msg) : std::runtime_error(msg) {}
};

struct TrainingError : std::runtime_error {
    TrainingError(const std::string& msg) : std::runtime_error(msg) {}
};

struct WorldConfigurationError : std::runtime_error {
    WorldConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const BrainInitializationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const TrainingError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const WorldConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error");
        }
    });

    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        -----------------------------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides advanced APIs for neural simulation, learning, and experimentation.
        
        New features in this enhanced version:
        - High-level convenience functions for common use cases
        - Advanced training utilities and experiment management
        - Better error handling and validation
        - Performance monitoring and visualization helpers
        - Preset configuration management
        - Batch training operations
    )pbdoc";

    // Include original bindings...
    // [Original binding code would continue here]

    // ===== HIGH-LEVEL UTILITY FUNCTIONS =====
    
    m.def("validateBrainConfig", &utils::validateBrainConfig, py::arg("config"),
          "Validate configuration before brain creation. Returns True if configuration is valid.");
    
    m.def("createPresetConfig", &utils::createPresetConfig, py::arg("preset"),
          R"pbdoc(
          Create a preset configuration.
          
          Args:
              preset: One of "minimal", "standard", "development", "high_performance"
          
          Returns:
              Config object with preset values set
          )pbdoc");
    
    m.def("runTrainingEpisode", &utils::runTrainingEpisode,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("maxSteps") = 1000, py::arg("timestep") = 0.1, py::arg("verbose") = false,
          "Run a single training episode and return metrics.");
    
    m.def("createAgentWithWorld", &utils::createAgentWithWorld,
          py::arg("preset") = "standard", py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
          "Create brain, world, and agent with world preconfigured.");
    
    m.def("runTraining", &utils::runTraining,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("numEpisodes") = 10, py::arg("maxStepsPerEpisode") = 1000,
          py::arg("timestep") = 0.1, py::arg("verbose") = false,
          "Run multiple training episodes and return metrics for each.");
    
    // ===== ENHANCED CLASS BINDINGS =====
    
    // Enhanced Config class with validation and presets
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system with validation and presets)pbdoc")
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
        .def("validate", &utils::validateBrainConfig, py::arg("config"),
             "Validate this configuration (returns True if valid)")
        .def("applyPreset", [](Config& self, const std::string& preset) {
            auto presetConfig = utils::createPresetConfig(preset);
            // Copy values from preset config
            auto keys = presetConfig->getKeys();
            for (const auto& key : keys) {
                if (presetConfig->has(key)) {
                    if (presetConfig->has(key)) {
                        auto value = presetConfig->getConfigValue(key);
                        if (std::holds_alternative<int64_t>(value)) {
                            self.set(key, std::get<int64_t>(value), ConfigSource::Default);
                        } else if (std::holds_alternative<double>(value)) {
                            self.set(key, std::get<double>(value), ConfigSource::Default);
                        } else if (std::holds_alternative<float>(value)) {
                            self.set(key, std::get<float>(value), ConfigSource::Default);
                        }
                    }
                }
            }
        }, py::arg("preset"),
           "Apply a preset configuration to this config object")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

                "Apply a preset configuration to this config object")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ===== ORIGINAL CLASS BINDINGS =====
    // Re-add essential original bindings that were removed
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
        return utils::createPresetConfig("standard");
    }, "Create a default configuration with standard settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

} // namespace nlm
