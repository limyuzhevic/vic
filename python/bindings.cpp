#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced Python exception types for better error handling
class NLMBrainError : public std::runtime_error {
public:
    explicit NLMBrainError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigurationError : public NLMBrainError {
public:
    explicit NLMConfigurationError(const std::string& msg) : NLMBrainError(msg) {}
};

class NLMSimulationError : public NLMBrainError {
public:
    explicit NLMSimulationError(const std::string& msg) : NLMBrainError(msg) {}
};

// Advanced configuration management class for Python
class PythonConfigManager {
public:
    PythonConfigManager() : config_(std::make_shared<Config>()) {}
    
    // Create brain with advanced configuration
    std::shared_ptr<Brain> createAdvancedBrain(std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }
    
    // Load and validate configuration from JSON with error checking
    std::shared_ptr<Config> loadValidatedConfig(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw NLMConfigurationError("Failed to load configuration from " + filepath);
        }
        
        // Validate configuration
        validateConfig(config);
        return config;
    }
    
    // Load configuration from Python dictionary
    std::shared_ptr<Config> configFromDict(const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        
        // Convert Python dict to C++ config
        for (auto it = configDict.begin(); it != configDict.end(); ++it) {
            std::string key = py::str(it->first);
            py::object value = it->second;
            
            // Handle different value types
            if (value.is(py::int_())) {
                config->set(key, static_cast<int64_t>(value.cast<int64_t>()));
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<float>());
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            } else if (value.is(py::list())) {
                // Handle list of floats
                auto list = value.cast<std::vector<float>>();
                config->set(key, list);
            }
        }
        
        validateConfig(config);
        return config;
    }
    
    // Get configuration summary in Python-friendly format
    py::dict getConfigSummary(const std::shared_ptr<Config>& config) {
        py::dict summary;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            py::object value;
            if (config->has(key)) {
                if (key.find("neuron_count") != std::string::npos) {
                    value = py::int_(config->get<int64_t>(key).value_or(0));
                } else if (key.find("float") != std::string::npos || 
                          key.find("rate") != std::string::npos ||
                          key.find("weight") != std::string::npos ||
                          key.find("alpha") != std::string::npos) {
                    value = py::float_(config->get<float>(key).value_or(0.0f));
                } else if (key.find("bool") != std::string::npos) {
                    value = py::bool_(config->get<bool>(key).value_or(false));
                } else {
                    value = py::str(config->get<std::string>(key).value_or(""));
                }
                summary[key] = value;
            }
        }
        return summary;
    }
    
    // Advanced simulation manager with event-driven execution
    class AdvancedSimulation {
    public:
        AdvancedSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) 
            : brain_(brain), world_(world), agent_(agent) {}
        
        // Run simulation with advanced controls
        void runAdvancedSimulation(int steps, 
                                 const py::dict& options,
                                 std::function<void(int, double)> progressCallback = nullptr) {
            
            // Parse advanced options
            bool enableLogging = options.get("enable_logging", py::bool_(true)).cast<bool>();
            bool enableCheckpointing = options.get("enable_checkpointing", py::bool_(false)).cast<bool>();
            bool adaptiveTimestep = options.get("adaptive_timestep", py::bool_(false)).cast<bool>();
            float maxTimeStep = options.get("max_timestep", py::float_(0.01)).cast<float>();
            float minTimeStep = options.get("min_timestep", py::float_(0.001)).cast<float>();
            
            double currentTime = 0.0;
            double timestep = 0.001;
            
            if (enableLogging) {
                NLM_LOG_INFO("Starting advanced simulation with " + std::to_string(steps) + " steps");
            }
            
            for (int step = 0; step < steps; ++step) {
                // Progress callback
                if (progressCallback) {
                    progressCallback(step, currentTime);
                }
                
                // Adaptive timestep if enabled
                if (adaptiveTimestep) {
                    timestep = calculateAdaptiveTimestep(brain_, currentTime);
                    timestep = std::max(minTimeStep, std::min(maxTimeStep, timestep));
                }
                
                // Run world update
                world_->update(timestep);
                
                // Process sensory input
                auto percept = world_->getSensoryPercept();
                agent_->processSensoryInput(percept);
                
                // Run brain simulation
                brain_->step(step, currentTime);
                
                // Get action and apply it
                auto action = agent_->decodeMotorCommand();
                world_->applyMotorCommand(action, currentTime);
                
                // Update development
                agent_->updateDevelopment(timestep);
                
                // Apply reward modulation if enabled
                if (options.get("enable_reward_modulation", py::bool_(true)).cast<bool>()) {
                    double reward = percept.getInternal()[0];
                    agent_->applyRewardModulation(reward, 0.0);
                }
                
                // Enable checkpointing
                if (enableCheckpointing && step % 1000 == 0) {
                    std::string checkpointPath = "./checkpoints/step_" + std::to_string(step) + ".bin";
                    brain_->save(checkpointPath);
                }
                
                currentTime += timestep;
                
                if (enableLogging && step % 100 == 0) {
                    NLM_LOG_INFO(std::to_string(step) + ": time=" + std::to_string(currentTime) + 
                                ", neurons_firing=" + std::to_string(brain_->getFiringNeuronCount()) +
                                ", total_spikes=" + std::to_string(brain_->getTotalSpikeCount()));
                }
            }
            
            if (enableLogging) {
                NLM_LOG_INFO("Advanced simulation completed successfully");
            }
        }
        
    private:
        double calculateAdaptiveTimestep(std::shared_ptr<Brain> brain, double currentTime) {
            // Calculate timestep based on neural activity
            float firingRate = brain->getAverageFiringRate();
            float maxRate = 100.0f; // Maximum expected firing rate
            
            // Return smaller timestep for high firing rates
            double adaptiveTimestep = 0.001 * (1.0 - firingRate / maxRate) + 0.0001;
            return std::max(0.0001, std::min(0.01, adaptiveTimestep));
        }
        
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<SimpleWorld> world_;
        std::shared_ptr<AgentBrain> agent_;
    };
    
    // Create advanced simulation manager
    AdvancedSimulation createAdvancedSimulation(std::shared_ptr<Brain> brain, 
                                                std::shared_ptr<SimpleWorld> world, 
                                                std::shared_ptr<AgentBrain> agent) {
        return AdvancedSimulation(brain, world, agent);
    }
    
    // Advanced analysis and debugging tools
    class AdvancedAnalyzer {
    public:
        AdvancedAnalyzer(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) 
            : brain_(brain), agent_(agent) {}
        
        // Get detailed neural activity analysis
        py::dict analyzeNeuralActivity() {
            py::dict analysis;
            
            // Get brain statistics
            analysis["total_neurons"] = py::int_(brain_->getTotalNeuronCount());
            analysis["total_synapses"] = py::int_(brain_->getTotalSynapseCount());
            analysis["firing_neurons"] = py::int_(brain_->getFiringNeuronCount());
            analysis["total_spikes"] = py::int_(brain_->getTotalSpikeCount());
            analysis["e_i_ratio"] = py::float_(brain_->getExcitationInhibitionRatio());
            
            // Get neural population analysis
            auto regions = brain_->getRegions();
            py::list regionAnalysis;
            
            for (const auto& region : regions) {
                py::dict regionData;
                regionData["id"] = py::int_(region->getId().index());
                regionData["name"] = py::str(region->getName());
                regionData["neuron_count"] = py::int_(region->getTotalNeuronCount());
                regionData["synapse_count"] = py::int_(region->getSynapseCount());
                regionData["avg_firing_rate"] = py::float_(region->getAverageFiringRate());
                regionAnalysis.append(regionData);
            }
            analysis["regions"] = regionAnalysis;
            
            // Get connectivity analysis
            py::dict connectivityAnalysis;
            connectivityAnalysis["avg_synapse_weight"] = py::float_(brain_->getExcitationInhibitionRatio());
            analysis["connectivity"] = connectivityAnalysis;
            
            // Get agent brain analysis
            if (agent_) {
                py::dict agentAnalysis;
                agentAnalysis["has_planner"] = agent_->getBrain() && agent_->getBrain()->getPlanner();
                agentAnalysis["has_concept_formation"] = agent_->getBrain() && agent_->getBrain()->getConceptFormation();
                agentAnalysis["has_attention"] = agent_->getBrain() && agent_->getBrain()->getAttention();
                agentAnalysis["has_reward_modulation"] = agent_->isRewardModulationEnabled();
                agentAnalysis["has_curiosity"] = agent_->isCuriosityEnabled();
                analysis["agent_capabilities"] = agentAnalysis;
            }
            
            return analysis;
        }
        
        // Pattern analysis for concept formation
        py::dict analyzePatterns(const std::vector<float>& pattern) {
            py::dict patternAnalysis;
            
            if (pattern.empty()) {
                return patternAnalysis;
            }
            
            // Calculate pattern statistics
            float sum = 0.0f;
            float sumSquares = 0.0f;
            float min = pattern[0];
            float max = pattern[0];
            
            for (float value : pattern) {
                sum += value;
                sumSquares += value * value;
                min = std::min(min, value);
                max = std::max(max, value);
            }
            
            int size = static_cast<int>(pattern.size());
            patternAnalysis["size"] = py::int_(size);
            patternAnalysis["mean"] = py::float_(sum / size);
            patternAnalysis["variance"] = py::float_(sumSquares / size - (sum / size) * (sum / size));
            patternAnalysis["std_dev"] = py::float_(std::sqrt(patternAnalysis["variance"].cast<float>()));
            patternAnalysis["min"] = py::float_(min);
            patternAnalysis["max"] = py::float_(max);
            patternAnalysis["range"] = py::float_(max - min);
            
            return patternAnalysis;
        }
        
        // Developmental stage analysis
        py::dict analyzeDevelopment(const std::shared_ptr<Brain>& brain) {
            py::dict developmentAnalysis;
            
            if (!brain) {
                return developmentAnalysis;
            }
            
            developmentAnalysis["stage"] = static_cast<int>(brain->getDevelopmentalStage());
            developmentAnalysis["plasticity_modifier"] = py::float_(1.0f); // Default value
            
            // Analyze different developmental aspects
            developmentAnalysis["maturation_level"] = py::float_(0.5f); // Placeholder
            developmentAnalysis["critical_period_active"] = false; // Placeholder
            developmentAnalysis["stable_phase"] = false; // Placeholder
            
            return developmentAnalysis;
        }
        
        // Neuromodulation analysis
        py::dict analyzeNeuromodulation(const std::shared_ptr<Brain>& brain) {
            py::dict neuromodulationAnalysis;
            
            if (!brain) {
                return neuromodulationAnalysis;
            }
            
            if (brain->getDopamine()) {
                neuromodulationAnalysis["dopamine_level"] = py::float_(brain->getDopamine()->getLevel());
                neuromodulationAnalysis["dopamine_plasticity_factor"] = py::float_(brain->getDopamine()->getPlasticityFactor());
            }
            
            if (brain->getCuriosity()) {
                neuromodulationAnalysis["has_curiosity"] = true;
                neuromodulationAnalysis["curiosity_level"] = py::float_(agent_ ? agent_->getCuriosityLevel() : 0.0f);
            }
            
            if (brain->getNovelty()) {
                neuromodulationAnalysis["has_novelty"] = true;
                neuromodulationAnalysis["novelty_level"] = py::float_(agent_ ? agent_->getNoveltyLevel() : 0.0f);
            }
            
            if (brain->getPredictionErrorSignal()) {
                neuromodulationAnalysis["has_prediction_error"] = true;
                neuromodulationAnalysis["prediction_error"] = py::float_(agent_ ? agent_->getPredictionError() : 0.0f);
            }
            
            return neuromodulationAnalysis;
        }
        
        // Advanced simulation diagnostics
        py::dict runDiagnostics() {
            py::dict diagnostics;
            
            // Basic brain diagnostics
            diagnostics["brain_alive"] = brain_ != nullptr;
            if (brain_) {
                diagnostics["neuron_count"] = py::int_(brain_->getTotalNeuronCount());
                diagnostics["synapse_count"] = py::int_(brain_->getTotalSynapseCount());
                diagnostics["active_neurons"] = py::int_(brain_->getActiveNeuronCount());
                diagnostics["firing_neurons"] = py::int_(brain_->getFiringNeuronCount());
                diagnostics["total_spikes"] = py::int_(brain_->getTotalSpikeCount());
            }
            
            // Agent diagnostics
            diagnostics["agent_alive"] = agent_ != nullptr;
            if (agent_) {
                diagnostics["has_planner"] = agent_->getBrain() && agent_->getBrain()->getPlanner();
                diagnostics["has_concept_formation"] = agent_->getBrain() && agent_->getBrain()->getConceptFormation();
                diagnostics["has_attention"] = agent_->getBrain() && agent_->getBrain()->getAttention();
                diagnostics["has_reward_modulation"] = agent_->isRewardModulationEnabled();
                diagnostics["has_curiosity"] = agent_->isCuriosityEnabled();
                diagnostics["has_development"] = agent_->isDevelopmentEnabled();
            }
            
            // World diagnostics
            diagnostics["world_alive"] = world_ != nullptr;
            if (world_) {
                diagnostics["world_width"] = py::int_(world_->getWidth());
                diagnostics["world_height"] = py::int_(world_->getHeight());
                diagnostics["simulation_time"] = py::float_(world_->getSimulationTime());
            }
            
            // Overall health check
            bool overallHealthy = diagnostics["brain_alive"].cast<bool>() && 
                                 diagnostics["agent_alive"].cast<bool>() &&
                                 diagnostics["world_alive"].cast<bool>();
            diagnostics["system_healthy"] = overallHealthy;
            
            return diagnostics;
        }
        
        // Run comprehensive benchmark
        py::dict runBenchmark(int steps, int warmupSteps = 100) {
            py::dict benchmarkResults;
            
            if (!brain_ || !agent_ || !world_) {
                benchmarkResults["error"] = "Missing required components";
                return benchmarkResults;
            }
            
            NLM_LOG_INFO("Starting benchmark simulation with " + std::to_string(steps) + " steps");
            
            // Warmup phase
            for (int step = 0; step < warmupSteps; ++step) {
                world_->update(0.001);
                auto percept = world_->getSensoryPercept();
                agent_->processSensoryInput(percept);
                brain_->step(step, step * 0.001);
            }
            
            // Benchmark phase
            double startTime = world_->getSimulationTime();
            int startSpikes = brain_->getTotalSpikeCount();
            
            for (int step = 0; step < steps; ++step) {
                world_->update(0.001);
                auto percept = world_->getSensoryPercept();
                agent_->processSensoryInput(percept);
                brain_->step(step, step * 0.001);
            }
            
            double endTime = world_->getSimulationTime();
            int endSpikes = brain_->getTotalSpikeCount();
            
            // Calculate results
            double elapsedTime = endTime - startTime;
            int spikesGenerated = endSpikes - startSpikes;
            double spikeRate = static_cast<double>(spikesGenerated) / elapsedTime;
            double stepsPerSecond = static_cast<double>(steps) / elapsedTime;
            
            benchmarkResults["total_steps"] = py::int_(steps);
            benchmarkResults["elapsed_time"] = py::float_(elapsedTime);
            benchmarkResults["spikes_generated"] = py::int_(spikesGenerated);
            benchmarkResults["spike_rate_hz"] = py::float_(spikeRate);
            benchmarkResults["steps_per_second"] = py::float_(stepsPerSecond);
            benchmarkResults["efficiency_score"] = py::float_(spikeRate / stepsPerSecond);
            
            NLM_LOG_INFO("Benchmark completed: " + std::to_string(stepsPerSecond) + " steps/sec, " +
                        std::to_string(spikeRate) + " spikes/sec");
            
            return benchmarkResults;
        }
        
    private:
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<AgentBrain> agent_;
        std::shared_ptr<SimpleWorld> world_;
    };
    
    // Create advanced analyzer
    AdvancedAnalyzer createAdvancedAnalyzer(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
        return AdvancedAnalyzer(brain, agent);
    }
    
private:
    void validateConfig(const std::shared_ptr<Config>& config) {
        // Validate essential configuration parameters
        if (config->get<int64_t>("neuron_count").value_or(1000) <= 0) {
            throw NLMConfigurationError("Invalid neuron_count: must be positive");
        }
        
        if (config->get<float>("connection_probability").value_or(0.1f) < 0.0f || 
            config->get<float>("connection_probability").value_or(0.1f) > 1.0f) {
            throw NLMConfigurationError("Invalid connection_probability: must be between 0.0 and 1.0");
        }
        
        if (config->get<float>("plasticity.stdp.learning_rate").value_or(0.001f) < 0.0f) {
            throw NLMConfigurationError("Invalid STDP learning rate: must be non-negative");
        }
    }
    
    std::shared_ptr<Config> config_;
};

PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        NLM Advanced Python Bindings
        ----------------------------
        Enhanced Python bindings for NLM with advanced features, better error handling,
        and comprehensive analysis tools for researchers and advanced users.
    )pbdoc";
    
    // Register advanced exception types
    py::register_exception<NLMConfigurationError>(m, "ConfigurationError");
    py::register_exception<NLMSimulationError>(m, "SimulationError");
    
    // Register main NLM classes (same as in pynlm)
    // This would include all the bindings from the original pynlm module
    // For brevity, only key classes are shown here
    
    // Register PythonConfigManager
    py::class_<PythonConfigManager>(m, "PythonConfigManager", R"pbdoc(Advanced configuration manager with validation)pbdoc")
        .def(py::init<>())
        .def("createBrain", &PythonConfigManager::createAdvancedBrain,
             py::arg("config"),
             "Create an advanced brain with configuration")
        .def("loadConfig", &PythonConfigManager::loadValidatedConfig,
             py::arg("filepath"),
             "Load and validate configuration from file")
        .def("loadConfigFromDict", &PythonConfigManager::configFromDict,
             py::arg("config_dict"),
             "Load configuration from Python dictionary")
        .def("getConfigSummary", &PythonConfigManager::getConfigSummary,
             py::arg("config"),
             "Get configuration summary in Python-friendly format")
        .def("createAdvancedSimulation", &PythonConfigManager::createAdvancedSimulation,
             py::arg("brain"), py::arg("world"), py::arg("agent"),
             "Create an advanced simulation manager")
        .def("createAdvancedAnalyzer", &PythonConfigManager::createAdvancedAnalyzer,
             py::arg("brain"), py::arg("agent"),
             "Create an advanced analyzer for neural activity");
    
    // Register AdvancedSimulation class
    py::class_<PythonConfigManager::AdvancedSimulation>(m, "AdvancedSimulation", R"pbdoc(Advanced simulation manager with adaptive controls)pbdoc")
        .def("runAdvancedSimulation", &PythonConfigManager::AdvancedSimulation::runAdvancedSimulation,
             py::arg("steps"), py::arg("options"),
             py::arg("progress_callback") = nullptr,
             "Run advanced simulation with enhanced controls");
    
    // Register AdvancedAnalyzer class
    py::class_<PythonConfigManager::AdvancedAnalyzer>(m, "AdvancedAnalyzer", R"pbdoc(Advanced neural activity analyzer)pbdoc")
        .def("analyzeNeuralActivity", &PythonConfigManager::AdvancedAnalyzer::analyzeNeuralActivity,
             "Analyze neural activity and return detailed statistics")
        .def("analyzePatterns", &PythonConfigManager::AdvancedAnalyzer::analyzePatterns,
             py::arg("pattern"),
             "Analyze a neural pattern and return statistics")
        .def("analyzeDevelopment", &PythonConfigManager::AdvancedAnalyzer::analyzeDevelopment,
             py::arg("brain"),
             "Analyze brain development stage")
        .def("analyzeNeuromodulation", &PythonConfigManager::AdvancedAnalyzer::analyzeNeuromodulation,
             py::arg("brain"),
             "Analyze neuromodulation state")
        .def("runDiagnostics", &PythonConfigManager::AdvancedAnalyzer::runDiagnostics,
             "Run comprehensive system diagnostics")
        .def("runBenchmark", &PythonConfigManager::AdvancedAnalyzer::runBenchmark,
             py::arg("steps"), py::arg("warmup_steps") = 100,
             "Run benchmark simulation with detailed metrics");
    
    // Register convenience functions
    m.def("createAdvancedConfigManager", []() {
        return PythonConfigManager();
    }, "Create an advanced configuration manager");
    
    // Enhanced error classes
    py::register_exception_translator([](const NLMConfigurationError& e) {
        return py::error_already_set();
    });
}

} // namespace nlm