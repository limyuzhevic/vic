#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

// Include all NLM headers
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

// Enhanced exception class for Python bindings
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error(message) {}
    NLMError(const std::string& message, const std::string& context) 
        : std::runtime_error("NLM Error: " + message + " (Context: " + context + ")") {}
};

// Enhanced Config class with additional functionality
void setupEnhancedConfig(py::class_<Config>& cls) {
    cls
        // Enhanced configuration management
        .def("getAsString", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"), "Get configuration value as string")
        .def("getAsInt", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue"), "Get configuration value as integer")
        .def("getAsFloat", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"), "Get configuration value as float")
        .def("getAsBool", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue"), "Get configuration value as boolean")
        
        // Configuration validation
        .def("validate", &Config::validate, py::arg("category") = "all", "Validate configuration")
        .def("getErrors", &Config::getValidationErrors, py::arg("category") = "all", "Get configuration validation errors")
        
        // Configuration comparison
        .def("equals", &Config::equals, py::arg("other"), "Check if configuration equals another")
        .def("diff", &Config::diff, py::arg("other"), "Get differences between configurations")
        
        // Configuration statistics
        .def("getEntryCount", &Config::getEntryCount, "Get total entry count")
        .def("getEntryCountBySource", &Config::getEntryCount, py::arg("source"), "Get entry count by source")
        .def("getUsedPercentage", &Config::getUsedPercentage, "Get percentage of non-default values used")
        
        // Configuration backup and restore
        .def("backup", &Config::backup, py::arg("backupPath"), "Backup configuration to file")
        .def("restore", &Config::restore, py::arg("backupPath"), "Restore configuration from file")
        
        // Configuration templates
        .def_static("createDefaultConfig", &Config::createDefaultConfig, "Create a default configuration")
        .def_static("createMinimalConfig", &Config::createMinimalConfig, "Create a minimal configuration")
        .def_static("createResearchConfig", &Config::createResearchConfig, "Create a research configuration")
        .def_static("createDevelopmentConfig", &Config::createDevelopmentConfig, "Create a development configuration")
        .def_static("createProductionConfig", &Config::createProductionConfig, "Create a production configuration")
        
        // Configuration help
        .def_static("getCategoryDescription", &Config::getCategoryDescription, py::arg("category"), "Get category description")
        .def_static("getAllCategories", &Config::getAllCategories, "Get all available categories")
        
        // Enhanced setters with validation
        .def("setString", &Config::set<std::string>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set string value")
        .def("setInt", &Config::set<int>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set integer value")
        .def("setFloat", &Config::set<double>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set float value")
        .def("setBool", &Config::set<bool>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set boolean value")
        .def("setInt64", &Config::set<int64_t>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set int64 value")
        .def("setStringVector", &Config::set<std::vector<std::string>>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set string vector")
        .def("setFloatVector", &Config::set<std::vector<double>>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set float vector")
        .def("setIntVector", &Config::set<std::vector<int>>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set integer vector")
        
        // Configuration merging
        .def("merge", &Config::merge, py::arg("other"), py::arg("source") = ConfigSource::File, "Merge with another configuration")
        
        // Configuration search and analysis
        .def("findKeysByPattern", &Config::findKeysByPattern, py::arg("pattern"), "Find keys matching pattern")
        
        // Export/import functionality
        .def("exportToString", &Config::exportToString, py::arg("minSource") = ConfigSource::Runtime, "Export configuration to string")
        .def("importFromString", &Config::importFromString, py::arg("configText"), "Import configuration from string")
        
        // Enhanced getters with type safety
        .def("getAsType", [](const Config& self, const std::string& key) {
            auto keys = self.getKeys();
            if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
                throw py::key_error("Key not found: '" + key + "'");
            }
            
            // Try different types
            if (auto val = self.get<std::string>(key)) return *val;
            if (auto val = self.get<int64_t>(key)) return *val;
            if (auto val = self.get<double>(key)) return *val;
            if (auto val = self.get<bool>(key)) return *val;
            if (auto val = self.get<std::vector<std::string>>(key)) return *val;
            if (auto val = self.get<std::vector<double>>(key)) return *val;
            if (auto val = self.get<std::vector<int>>(key)) return *val;
            
            throw py::type_error("Cannot determine type for key: '" + key + "'");
        }, py::arg("key"), "Get configuration value as generic type (auto-detect)")
        
        // Configuration statistics and analysis
        .def("getStatistics", [](const Config& self) {
            py::dict stats;
            stats["total_entries"] = self.getEntryCount();
            stats["default_entries"] = self.getEntryCount(ConfigSource::Default);
            stats["runtime_entries"] = self.getEntryCount(ConfigSource::Runtime);
            stats["file_entries"] = self.getEntryCount(ConfigSource::File);
            stats["command_line_entries"] = self.getEntryCount(ConfigSource::CommandLine);
            stats["used_percentage"] = self.getUsedPercentage();
            return stats;
        }, "Get configuration statistics");
}

// Enhanced Brain class with additional functionality
void setupEnhancedBrain(py::class_<Brain>& cls) {
    cls
        // Enhanced simulation control
        .def("runSteps", [](Brain& self, size_t steps, bool collectMetrics = true) {
            py::dict metrics;
            float totalReward = 0.0f;
            float avgFiringRate = 0.0f;
            size_t activeNeurons = 0;
            
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * 0.001);
                
                if (collectMetrics) {
                    totalReward += self.getAverageFiringRate();
                    avgFiringRate += self.getAverageFiringRate();
                    if (self.getFiringNeuronCount() > 0) activeNeurons++;
                }
            }
            
            if (collectMetrics) {
                metrics["total_reward"] = totalReward;
                metrics["avg_firing_rate"] = avgFiringRate / steps;
                metrics["active_steps"] = activeNeurons;
                metrics["final_spike_count"] = self.getTotalSpikeCount();
                metrics["final_firing_neuron_count"] = self.getFiringNeuronCount();
            }
            
            return metrics;
        }, py::arg("steps"), py::arg("collectMetrics") = true, "Run multiple simulation steps and collect metrics")
        
        // Enhanced action production
        .def("produceActionWithMetrics", [](Brain& self, const SensoryPercept& percept) {
            py::dict actionInfo;
            actionInfo["action"] = self.produceAction();
            actionInfo["neuron_activity"] = self.getFiringNeuronCount();
            actionInfo["firing_rate"] = self.getAverageFiringRate();
            actionInfo["region_count"] = self.getRegionCount();
            actionInfo["total_synapses"] = self.getTotalSynapseCount();
            return actionInfo;
        }, py::arg("percept"), "Produce action with additional metrics")
        
        // Enhanced checkpoint functionality
        .def("saveWithTimestamp", [](Brain& self, const std::string& filepath) {
            auto timestamp = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(timestamp);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                timestamp.time_since_epoch()) % 1000;
            
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
            ss << "." << std::setfill('0') << std::setw(3) << ms.count() << "_" << filepath;
            
            bool success = self.save(ss.str());
            if (success) {
                py::print("Checkpoint saved: " + ss.str());
            }
            return success;
        }, py::arg("filepath"), "Save checkpoint with timestamp")
        
        // Enhanced brain analysis
        .def("analyzeNetworkTopology", [](Brain& self) {
            py::dict topology;
            topology["region_count"] = self.getRegionCount();
            topology["neuron_count"] = self.getTotalNeuronCount();
            topology["synapse_count"] = self.getTotalSynapseCount();
            topology["firing_ratio"] = static_cast<float>(self.getFiringNeuronCount()) / std::max(1.0f, static_cast<float>(self.getTotalNeuronCount()));
            topology["excitatory_inhibitory_ratio"] = self.getExcitationInhibitionRatio();
            topology["average_firing_rate"] = self.getAverageFiringRate();
            return topology;
        }, "Analyze brain network topology")
        
        // Enhanced development monitoring
        .def("monitorDevelopment", [](Brain& self) {
            py::dict devInfo;
            devInfo["stage"] = static_cast<int>(self.getDevelopmentalStage());
            devInfo["plasticity_enabled"] = self.getConfig()->getOr<bool>("plasticity_enabled", false);
            devInfo["critical_period_active"] = self.getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod;
            devInfo["sensitive_period"] = self.getConfig()->getOr<bool>("critical_period_enabled", false);
            return devInfo;
        }, "Monitor brain development state")
        
        // Enhanced learning analysis
        .def("analyzeLearningProgress", [](Brain& self, size_t steps) {
            py::dict learningMetrics;
            learningMetrics["spike_count_growth"] = self.getTotalSpikeCount();
            learningMetrics["firing_rate_stability"] = self.getAverageFiringRate();
            learningMetrics["network_activity"] = self.getFiringNeuronCount();
            learningMetrics["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            learningMetrics["time_steps"] = steps;
            return learningMetrics;
        }, py::arg("steps"), "Analyze learning progress over time");
}

// Enhanced SimpleWorld class with additional functionality
void setupEnhancedSimpleWorld(py::class_<SimpleWorld>& cls) {
    cls
        // Enhanced world simulation
        .def("runEpisode", [](SimpleWorld& self, std::shared_ptr<Brain> brain, 
                           std::shared_ptr<AgentBrain> agent, size_t steps) {
            py::dict episodeStats;
            float totalReward = 0.0f;
            size_t stepsCompleted = 0;
            
            for (size_t i = 0; i < steps; ++i) {
                // Update world
                self.update(0.1);
                
                // Get perception
                auto percept = self.getSensoryPercept();
                
                // Process in brain
                agent->processSensoryInput(percept);
                brain->step(i);
                
                // Get action
                auto action = agent->decodeMotorCommand();
                
                // Apply action
                self.applyMotorCommand(action, self.getSimulationTime());
                
                // Get reward
                float reward = self.getSensoryPercept().getInternal()[0] if self.getSensoryPercept().getInternal() else 0.0f;
                totalReward += reward;
                
                stepsCompleted++;
                
                // Periodic checkpoint
                if (i % 100 == 0) {
                    agent->reset();
                }
            }
            
            episodeStats["total_reward"] = totalReward;
            episodeStats["avg_reward"] = totalReward / stepsCompleted;
            episodeStats["steps_completed"] = stepsCompleted;
            episodeStats["final_firing_rate"] = brain->getAverageFiringRate();
            episodeStats["final_spike_count"] = brain->getTotalSpikeCount();
            
            return episodeStats;
        }, py::arg("brain"), py::arg("agent"), py::arg("steps"), "Run a complete world episode")
        
        // Enhanced world management
        .def("createRandomEnvironment", [](SimpleWorld& self, size_t width, size_t height) {
            self.configure(width, height, 8, 8);
            
            // Add some random resources
            for (size_t i = 0; i < width * height / 10; ++i) {
                WorldObject obj(
                    static_cast<float>(rand() % width),
                    static_cast<float>(rand() % height),
                    WorldObjectType::Resource,
                    1.0f,
                    0.3f
                );
                self.addObject(obj);
            }
            
            // Add some hazards
            for (size_t i = 0; i < width * height / 20; ++i) {
                WorldObject obj(
                    static_cast<float>(rand() % width),
                    static_cast<float>(rand() % height),
                    WorldObjectType::Hazard,
                    0.0f,
                    0.5f
                );
                self.addObject(obj);
            }
            
            py::print("Created random environment with " + 
                     std::to_string(width * height / 10) + " resources and " +
                     std::to_string(width * height / 20) + " hazards");
        }, py::arg("width"), py::arg("height"), "Create a random environment")
        
        // Enhanced world analysis
        .def("analyzeWorldState", [](SimpleWorld& self) {
            py::dict worldStats;
            worldStats["width"] = self.getWidth();
            worldStats["height"] = self.getHeight();
            worldStats["simulation_time"] = self.getSimulationTime();
            worldStats["max_energy"] = self.getMaxEnergy();
            worldStats["agent_position"] = py::make_tuple(self.getAgentBody().x, self.getAgentBody().y);
            worldStats["agent_orientation"] = self.getAgentBody().orientation;
            worldStats["agent_energy"] = self.getAgentBody().energy;
            worldStats["agent_health"] = self.getAgentBody().health;
            worldStats["agent_age"] = self.getAgentBody().age;
            return worldStats;
        }, "Analyze world state");
}

// Enhanced AgentBrain class with additional functionality
void setupEnhancedAgentBrain(py::class_<AgentBrain>& cls) {
    cls
        // Enhanced learning loops
        .def("runLearningLoop", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, 
                                 size_t steps, bool enableCuriosity = true, 
                                 bool enableReward = true, bool enableDevelopment = true) {
            py::dict learningStats;
            float totalCuriosity = 0.0f;
            float totalNovelty = 0.0f;
            float totalPredictionError = 0.0f;
            size_t curiositySteps = 0;
            size_t noveltySteps = 0;
            size_t predictionSteps = 0;
            
            for (size_t i = 0; i < steps; ++i) {
                // Update world
                world->update(0.1);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                self.processSensoryInput(percept);
                
                // Get action from brain (would need access to underlying brain)
                // For now, just update development
                self.updateDevelopment(0.1);
                
                // Collect metrics
                if (enableCuriosity) {
                    totalCuriosity += self.getCuriosityLevel();
                    curiositySteps++;
                }
                if (enableNovelty) {
                    totalNovelty += self.getNoveltyLevel();
                    noveltySteps++;
                }
                if (enableReward) {
                    totalPredictionError += self.getPredictionError();
                    predictionSteps++;
                }
                
                // Periodic reset
                if (i % 100 == 0) {
                    self.reset();
                }
            }
            
            learningStats["avg_curiosity"] = curiositySteps > 0 ? totalCuriosity / curiositySteps : 0.0f;
            learningStats["avg_novelty"] = noveltySteps > 0 ? totalNovelty / noveltySteps : 0.0f;
            learningStats["avg_prediction_error"] = predictionSteps > 0 ? totalPredictionError / predictionSteps : 0.0f;
            learningStats["steps_completed"] = steps;
            
            return learningStats;
        }, py::arg("world"), py::arg("steps"), py::arg("enableCuriosity") = true, 
               py::arg("enableReward") = true, py::arg("enableDevelopment") = true,
               "Run a complete learning loop")
        
        // Enhanced agent configuration
        .def("configureAgent", [](AgentBrain& self, bool enableAll = true) {
            self.enableRewardModulation(enableAll);
            self.enableStructuralPlasticity(enableAll);
            self.enableDevelopment(enableAll);
            self.enableCuriosity(enableAll);
            
            py::print("Agent configured with all subsystems: " + 
                     std::string(enableAll ? "enabled" : "disabled"));
        }, py::arg("enableAll") = true, "Configure all agent subsystems")
        
        // Enhanced agent monitoring
        .def("monitorAgent", [](AgentBrain& self) {
            py::dict agentStats;
            agentStats["reward_modulation"] = self.isRewardModulationEnabled();
            agentStats["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            agentStats["development"] = self.isDevelopmentEnabled();
            agentStats["curiosity"] = self.isCuriosityEnabled();
            agentStats["neuromodulation_level"] = self.getNeuromodulationLevel();
            agentStats["curiosity_level"] = self.getCuriosityLevel();
            agentStats["novelty_level"] = self.getNoveltyLevel();
            agentStats["prediction_error"] = self.getPredictionError();
            agentStats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return agentStats;
        }, "Monitor agent state");
}

// Enhanced utility functions
void setupEnhancedUtilities(py::module_& m) {
    // Advanced configuration creation
    m.def("createAdvancedConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set up advanced research configuration
        config->set("neuron_count", int64_t(5000), ConfigSource::Default);
        config->set("region_count", int64_t(5), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("plasticity_enabled", true, ConfigSource::Default);
        config->set("plasticity_learning_rate", 0.005, ConfigSource::Default);
        config->set("stdp_enabled", true, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.008f, ConfigSource::Default);
        config->set("synaptogenesis_enabled", true, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.00005f, ConfigSource::Default);
        config->set("pruning_enabled", true, ConfigSource::Default);
        config->set("pruning_rate", 0.000005f, ConfigSource::Default);
        config->set("memory_working_memory_enabled", true, ConfigSource::Default);
        config->set("memory_episodic_enabled", true, ConfigSource::Default);
        config->set("neuromodulation_enabled", true, ConfigSource::Default);
        config->set("dopamine_enabled", true, ConfigSource::Default);
        config->set("curiosity_enabled", true, ConfigSource::Default);
        config->set("novelty_enabled", true, ConfigSource::Default);
        config->set("prediction_enabled", true, ConfigSource::Default);
        config->set("development_enabled", true, ConfigSource::Default);
        config->set("attention_enabled", true, ConfigSource::Default);
        config->set("planner_enabled", true, ConfigSource::Default);
        config->set("concept_formation_enabled", true, ConfigSource::Default);
        
        return config;
    }, "Create an advanced research configuration");
    
    // Create a quick start configuration
    m.def("createQuickStartConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Minimal configuration for quick testing
        config->set("neuron_count", int64_t(100), ConfigSource::Default);
        config->set("region_count", int64_t(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("plasticity_enabled", false, ConfigSource::Default); // Disable for faster testing
        config->set("stdp_enabled", false, ConfigSource::Default);
        config->set("development_enabled", false, ConfigSource::Default);
        
        return config;
    }, "Create a quick start configuration for rapid testing");
    
    // Create a performance configuration
    m.def("createPerformanceConfig", []() {
        auto config = std::make_shared<Config>();
        
        // High-performance configuration
        config->set("neuron_count", int64_t(1000), ConfigSource::Default);
        config->set("region_count", int64_t(2), ConfigSource::Default);
        config->set("connection_probability", 0.02f, ConfigSource::Default);
        config->set("plasticity_enabled", true, ConfigSource::Default);
        config->set("stdp_enabled", true, ConfigSource::Default);
        config->set("simulation_timestep", 0.0005, ConfigSource::Default); // Smaller timestep for accuracy
        config->set("synaptogenesis_enabled", true, ConfigSource::Default);
        
        return config;
    }, "Create a performance-optimized configuration");
    
    // Create a development configuration
    m.def("createDevelopmentConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Development-friendly configuration with debugging
        config->set("neuron_count", int64_t(500), ConfigSource::Default);
        config->set("region_count", int64_t(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("plasticity_enabled", true, ConfigSource::Default);
        config->set("stdp_enabled", true, ConfigSource::Default);
        config->set("development_enabled", true, ConfigSource::Default);
        config->set("critical_period_enabled", true, ConfigSource::Default);
        config->set("sensitive_period_start", 100.0, ConfigSource::Default);
        config->set("sensitive_period_end", 1000.0, ConfigSource::Default);
        config->set("memory_working_memory_enabled", true, ConfigSource::Default);
        config->set("memory_episodic_enabled", true, ConfigSource::Default);
        
        return config;
    }, "Create a development configuration with debugging support");
    
    // Advanced simulation utilities
    m.def("runBenchmark", [](std::shared_ptr<Config> config, size_t steps) {
        py::dict benchmarkResults;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        benchmarkResults["total_steps"] = steps;
        benchmarkResults["time_taken_ms"] = static_cast<double>(duration.count());
        benchmarkResults["steps_per_second"] = steps / (duration.count() / 1000.0);
        benchmarkResults["final_firing_rate"] = brain->getAverageFiringRate();
        benchmarkResults["total_spikes"] = brain->getTotalSpikeCount();
        benchmarkResults["neurons_used"] = brain->getTotalNeuronCount();
        
        return benchmarkResults;
    }, py::arg("config"), py::arg("steps"), "Run a performance benchmark");
    
    m.def("createStandardEnvironment", [](size_t width, size_t height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        
        // Add standard objects
        for (size_t y = 0; y < height; y += 4) {
            for (size_t x = 0; x < width; x += 4) {
                WorldObject obj(static_cast<float>(x), static_cast<float>(y), 
                               WorldObjectType::Resource, 1.0f, 0.5f);
                world->addObject(obj);
            }
        }
        
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20, "Create a standard environment");
    
    m.def("createAgentWithWorld", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create an agent with a world");
    
    // Exception types for Python
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred in NLM");
        }
    });
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ---------------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        
        **Enhanced Configuration Management:**
        - Advanced configuration templates (research, development, production)
        - Configuration validation and error checking
        - Configuration comparison and merging
        - Configuration statistics and analysis
        
        **Enhanced Brain Simulation:**
        - Multi-step simulation runs with metrics collection
        - Network topology analysis
        - Learning progress monitoring
        - Enhanced checkpoint management with timestamps
        
        **Enhanced World Simulation:**
        - Complete episode running with statistics
        - Random environment generation
        - World state analysis
        
        **Enhanced Agent Interface:**
        - Learning loop automation
        - Agent configuration management
        - Comprehensive agent monitoring
        
        **Advanced Utilities:**
        - Performance benchmarking
        - Standard environment creation
        - Advanced configuration creation
        - Exception handling and translation
        
        **Key Features:**
        - Type-safe configuration access
        - Comprehensive error handling
        - Performance monitoring
        - Development-friendly debugging
        - Production-ready configurations
        
        The enhanced bindings provide a more intuitive and powerful interface
        for both beginners and advanced users of the NLM framework.
    )pbdoc";
    
    // Register exception types
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    
    // Basic type bindings
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

    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
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

    // Enhanced SensoryInput classes
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

    // Enhanced Action and related classes
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

    // Enhanced Agent and related classes
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

    // Enhanced SimpleWorld class
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

    // Enhanced Brain class
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

    // Enhanced AgentBrain class
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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm