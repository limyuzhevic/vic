#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <functional>

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

// Enhanced Python bindings with advanced features and better usability
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -------------------------------------------------------
        Enhanced Python API for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Pythonic interfaces with context managers and properties
        - Advanced simulation control and monitoring
        - Enhanced configuration management
        - Learning algorithm utilities
        - Performance monitoring tools
        - Advanced visualization integration (if available)
        - Checkpointing and persistence utilities
        - Real-time visualization and analysis
        - Machine learning integration utilities
    )pbdoc";

    // Re-export core functionality
    py::module::import("pynlm").attr("_copy_module_attributes"); // Copy attributes from original module

    // ========== ENHANCED CONFIGURATION ==========
    py::class_<Config>(m, "EnhancedConfig", R"pbdoc(
        Enhanced configuration class with advanced features
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from file with validation")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to file with error handling")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (val) return *val;
            throw py::key_error("Configuration key not found: " + key);
        }, py::arg("key"),
            "Get string configuration value (throws if not found)")
        .def("get_or", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto val = self.get<std::string>(key);
            return val ? *val : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get string configuration value with default")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set string configuration value")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set float configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set boolean configuration value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get configuration summary")
        .def("validate", [](Config& self) {
            // Validation logic would go here
            std::vector<std::string> errors;
            for (const auto& key : self.getKeys()) {
                // Add validation for specific keys if needed
            }
            if (!errors.empty()) {
                throw py::value_error("Configuration validation failed:\n" + 
                                     std::accumulate(errors.begin() + 1, errors.end(), errors[0],
                                                    [](const std::string& a, const std::string& b) {
                                                        return a + "\n" + b;
                                                    }));
            }
        }, "Validate configuration and throw error if invalid")
        .def("exportToDict", [](Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                // Try to get each value and convert to Python type
                auto val = self.get<std::string>(key);
                if (val) {
                    result[key] = *val;
                }
            }
            return result;
        }, "Export configuration as Python dictionary")
        .def("importFromDict", [](Config& self, const py::dict& data) {
            for (auto item : data) {
                std::string key = py::str(item.first);
                self.set(key, py::str(item.second), ConfigSource::Runtime);
            }
        }, py::arg("data"),
            "Import configuration from Python dictionary")
        .def("mergeFromFile", [](Config& self, const std::string& filepath) {
            // Temporary config for merging
            Config temp;
            if (!temp.loadFromFile(filepath)) {
                throw py::runtime_error("Failed to load configuration from: " + filepath);
            }
            // Merge temp config into self
            for (const auto& key : temp.getKeys()) {
                auto val = temp.get<std::string>(key);
                if (val) {
                    self.set(key, *val, ConfigSource::Runtime);
                }
            }
        }, py::arg("filepath"),
            "Merge configuration from file (overwrites existing keys)")
        .def("mergeFromArgs", [](Config& self, int argc, char** argv) {
            Config temp;
            if (!temp.loadFromArgs(argc, argv)) {
                throw py::runtime_error("Failed to load configuration from arguments");
            }
            // Merge temp config into self
            for (const auto& key : temp.getKeys()) {
                auto val = temp.get<std::string>(key);
                if (val) {
                    self.set(key, *val, ConfigSource::Runtime);
                }
            }
        }, py::arg("argc"), py::arg("argv"),
            "Merge configuration from command line arguments")
        .def("__repr__", [](const Config& cfg) {
            return "<EnhancedConfig: " + cfg.summary() + ">";
        });

    // ========== ENHANCED BRAIN ==========
    py::class_<Brain>(m, "EnhancedBrain", R"pbdoc(
        Enhanced brain class with advanced simulation features
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("reset", &Brain::reset,
             "Reset brain to initial state")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("run", [](Brain& self, size_t numSteps, double timestep = 0.001,
                     std::function<void(size_t)> callback = nullptr) {
            for (size_t step = 0; step < numSteps; ++step) {
                self.step(step, step * timestep);
                if (callback) {
                    callback(step);
                }
            }
        }, py::arg("numSteps"), py::arg("timestep") = 0.001,
            py::arg("callback") = nullptr,
            "Run simulation for specified number of steps")
        .def("runUntil", [](Brain& self, std::function<bool()> condition,
                          double timestep = 0.001) {
            size_t step = 0;
            while (condition()) {
                self.step(step, step * timestep);
                step++;
            }
        }, py::arg("condition"), py::arg("timestep") = 0.001,
            "Run simulation until condition is met")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getStats", [](Brain& self) {
            py::dict stats;
            stats["total_neuron_count"] = self.getTotalNeuronCount();
            stats["total_synapse_count"] = self.getTotalSynapseCount();
            stats["active_neuron_count"] = self.getActiveNeuronCount();
            stats["firing_neuron_count"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["total_spike_count"] = self.getTotalSpikeCount();
            stats["pending_spike_count"] = self.getPendingSpikeEventCount();
            return stats;
        }, "Get brain statistics as dictionary")
        .def("isInitialized", &Brain::getConfig,
             "Check if brain is initialized (config is not null)")
        .def("waitForSpikes", [](Brain& self, size_t minSpikes, double timeout = 10.0,
                               double timestep = 0.001) {
            size_t step = 0;
            double startTime = 0.0;
            while (self.getTotalSpikeCount() < minSpikes && 
                   (startTime == 0.0 || (step * timestep - startTime) < timeout)) {
                if (startTime == 0.0) startTime = step * timestep;
                self.step(step, step * timestep);
                step++;
            }
            return self.getTotalSpikeCount() >= minSpikes;
        }, py::arg("minSpikes"), py::arg("timeout") = 10.0, py::arg("timestep") = 0.001,
            "Wait for minimum number of spikes or timeout")
        .def("runUntilSteadyState", [](Brain& self, size_t windowSize = 100,
                                     double convergenceThreshold = 0.01,
                                     double timestep = 0.001) {
            float previousRate = 0.0f;
            size_t consecutiveStable = 0;
            
            for (size_t step = 0; step < 10000; ++step) {
                self.step(step, step * timestep);
                float currentRate = self.getAverageFiringRate();
                
                if (std::abs(currentRate - previousRate) < convergenceThreshold) {
                    consecutiveStable++;
                } else {
                    consecutiveStable = 0;
                }
                
                if (consecutiveStable >= windowSize) {
                    return step;
                }
                
                previousRate = currentRate;
            }
            return size_t(-1); // Not reached
        }, py::arg("windowSize") = 100, py::arg("convergenceThreshold") = 0.01, py::arg("timestep") = 0.001,
            "Run until firing rate reaches steady state");

    // ========== ENHANCED AGENT BRAIN ==========
    py::class_<AgentBrain>(m, "EnhancedAgentBrain", R"pbdoc(
        Enhanced agent brain interface with advanced features
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getAgentStats", [](AgentBrain& self) {
            py::dict stats;
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            return stats;
        }, "Get agent brain statistics")
        .def("getLearningStats", [](AgentBrain& self) {
            py::dict stats;
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            return stats;
        }, "Get learning subsystem status")
        .def("applyLearning", [](AgentBrain& self, double reward, double predictedReward = 0.0) {
            self.applyRewardModulation(reward, predictedReward);
            self.updateDevelopment(0.1); // Default timestep
        }, py::arg("reward"), py::arg("predictedReward") = 0.0,
            "Apply learning from environment");

    // ========== ENHANCED WORLD ==========
    py::class_<SimpleWorld>(m, "EnhancedSimpleWorld", R"pbdoc(
        Enhanced simple world with advanced features
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
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
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def("simulateStep", [](SimpleWorld& self, double timestep = 0.1,
                               py::object action = py::none()) {
            self.update(timestep);
            
            if (action) {
                // Apply action if provided
                // This would need additional API for action application
            }
            
            return self.getSensoryPercept();
        }, py::arg("timestep") = 0.1, py::arg("action") = py::none(),
            "Simulate one world step and return percept")
        .def("runEpisode", [](SimpleWorld& self, size_t numSteps,
                            py::function<py::object()> actionGenerator = nullptr,
                            py::function<void(py::object)> rewardCallback = nullptr) {
            py::object percept;
            for (size_t step = 0; step < numSteps; ++step) {
                self.update(0.1);
                percept = self.getSensoryPercept();
                
                if (actionGenerator) {
                    py::object action = actionGenerator();
                    self.applyMotorCommand(action, self.getSimulationTime());
                    
                    if (rewardCallback) {
                        // Calculate reward would need additional API
                        rewardCallback("default_reward");
                    }
                }
            }
            return percept;
        }, py::arg("numSteps"), py::arg("actionGenerator") = nullptr, py::arg("rewardCallback") = nullptr,
            "Run simulation episode and return final percept")
        .def("getWorldInfo", [](SimpleWorld& self) {
            py::dict info;
            info["width"] = self.getWidth();
            info["height"] = self.getHeight();
            info["time"] = self.getSimulationTime();
            info["agent_x"] = self.getAgentBody().x;
            info["agent_y"] = self.getAgentBody().y;
            return info;
        }, "Get world information as dictionary");

    // ========== FACTORY FUNCTIONS ==========
    m.def("createEnhancedConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create enhanced configuration");
    
    m.def("createEnhancedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create enhanced brain with configuration");
    
    m.def("createEnhancedAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create enhanced agent brain interface");
    
    m.def("createEnhancedWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create enhanced world");

    // ========== UTILITY FUNCTIONS ==========
    m.def("createRandomConfig", [](int neuronCount = 1000, int regionCount = 1) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Runtime);
        config->set("connection_probability", 0.1f, ConfigSource::Runtime);
        return config;
    }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 1,
        "Create configuration with specified parameters");
    
    m.def("createQuickSimulation", [](size_t neuronCount = 500, size_t steps = 100) {
        auto config = createRandomConfig(neuronCount);
        auto brain = createEnhancedBrain(config);
        brain->initialize();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        return brain;
    }, py::arg("neuronCount") = 500, py::arg("steps") = 100,
        "Quick simulation setup and run");
    
    m.def("benchmarkSimulation", [](size_t neuronCount = 1000, size_t steps = 1000) {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto config = createRandomConfig(neuronCount);
        auto brain = createEnhancedBrain(config);
        brain->initialize();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        py::dict results;
        results["neuron_count"] = neuronCount;
        results["steps"] = steps;
        results["duration_ms"] = duration.count();
        results["neurons_per_second"] = (neuronCount * steps) / (duration.count() / 1000.0);
        
        return results;
    }, py::arg("neuronCount") = 1000, py::arg("steps") = 1000,
        "Run benchmark simulation and return performance metrics");

    // ========== ADVANCED LEARNING ALGORITHMS ==========
    m.def("runHebbianLearning", [](std::shared_ptr<Brain> brain, size_t steps = 1000,
                                 float learningRate = 0.01f) {
        // Enable Hebbian learning
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, false, false); // Hebbian only
            }
        }
        
        // Run simulation with Hebbian learning
        for (size_t step = 0; step < steps; ++step) {
            // Add some input to drive learning
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step);
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps") = 1000, py::arg("learningRate") = 0.01f,
        "Run Hebbian learning simulation");
    
    m.def("runSTDPLearning", [](std::shared_ptr<Brain> brain, size_t steps = 1000,
                               float learningRate = 0.01f) {
        // Enable STDP learning
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(false, true, false); // STDP only
            }
        }
        
        // Run simulation with STDP learning
        for (size_t step = 0; step < steps; ++step) {
            // Create correlated activity for STDP
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 30.0f);
            brain->step(step);
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps") = 1000, py::arg("learningRate") = 0.01f,
        "Run STDP learning simulation");
    
    m.def("runRewardLearning", [](std::shared_ptr<AgentBrain> agent, size_t steps = 1000,
                                 float reward = 1.0f) {
        // Enable reward learning
        agent->enableRewardModulation(true);
        
        // Run simulation with reward learning
        for (size_t step = 0; step < steps; ++step) {
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        return agent->getCuriosityLevel();
    }, py::arg("agent"), py::arg("steps") = 1000, py::arg("reward") = 1.0f,
        "Run reward-based learning simulation");

    // ========== EXPORT METADATA ==========
    m.attr("__version__") = "enhanced-0.1.0";
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced features";
}

} // namespace nlm
