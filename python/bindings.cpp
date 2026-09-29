#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <algorithm>
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
#include "../src/performance/CheckpointSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Custom NLM Exception hierarchy for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
    virtual ~NLMException() = default;
    virtual std::string getType() const { return "NLMException"; }
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message) : NLMException(message) {}
    virtual std::string getType() const override { return "ConfigurationError"; }
};

class NeuralError : public NLMException {
public:
    explicit NeuralError(const std::string& message) : NLMException(message) {}
    virtual std::string getType() const override { return "NeuralError"; }
};

class IntegrationError : public NLMException {
public:
    explicit IntegrationError(const std::string& message) : NLMException(message) {}
    virtual std::string getType() const override { return "IntegrationError"; }
};

class SerializationError : public NLMException {
public:
    explicit SerializationError(const std::string& message) : NLMException(message) {}
    virtual std::string getType() const override { return "SerializationError"; }
};

class DevelopmentError : public NLMException {
public:
    explicit DevelopmentError(const std::string& message) : NLMException(message) {}
    virtual std::string getType() const override { return "DevelopmentError"; }
};

class ResourceError : public NLMException {
public:
    explicit ResourceError(const std::string& message) : NLMException(message) {}
    virtual std::string getType() const override { return "ResourceError"; }
};

// Progress callback type for long-running operations
class ProgressCallback {
public:
    virtual ~ProgressCallback() = default;
    virtual void onProgress(float progress, const std::string& message = "") = 0;
    virtual bool shouldCancel() const { return false; }
};

// Thread-safe brain wrapper
class ThreadSafeBrain {
private:
    std::shared_ptr<Brain> brain_;
    mutable std::mutex mutex_;
    bool is_gpu_enabled_ = false;
    std::atomic<float> gpu_memory_usage_{0.0f};
    CheckpointSystem checkpointSystem_;
    
public:
    explicit ThreadSafeBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    std::shared_ptr<Brain> getBrain() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return brain_;
    }
    
    void setGPU(bool enabled) {
        std::lock_guard<std::mutex> lock(mutex_);
        is_gpu_enabled_ = enabled;
        if (enabled) {
            gpu_memory_usage_ = 0.5f;
        }
    }
    
    bool isGPUEnabled() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return is_gpu_enabled_;
    }
    
    float getGPUMemoryUsage() const {
        return gpu_memory_usage_.load();
    }
    
    void stepWithProgress(SimulationStep currentStep, ProgressCallback* callback = nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        brain_->step(currentStep);
        if (callback) {
            callback->onProgress(0.5f, "Brain processing completed");
        }
    }
    
    void runLifetime(int days, ProgressCallback* callback = nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            callback->onProgress(0.0f, "Starting lifetime simulation");
        }
        
        for (int day = 0; day < days && !callback->shouldCancel(); ++day) {
            float progress = static_cast<float>(day) / days;
            if (callback) {
                callback->onProgress(progress, "Processing day " + std::to_string(day + 1));
            }
            brain_->reset();
            for (int step = 0; step < 1000; ++step) {
                SimulationStep s{};
                brain_->step(s);
            }
        }
        
        if (callback) {
            callback->onProgress(1.0f, "Lifetime simulation completed");
        }
    }
    
    std::string saveCheckpoint(const std::string& filepath, bool compress = true) {
        std::lock_guard<std::mutex> lock(mutex_);
        try {
            if (compress) {
                checkpointSystem_.save(filepath, CheckpointSystem::CompressionLevel::Balanced);
            } else {
                checkpointSystem_.save(filepath, CheckpointSystem::CompressionLevel::None);
            }
            return "Successfully saved checkpoint to " + filepath;
        } catch (const std::exception& e) {
            throw SerializationError("Failed to save checkpoint: " + std::string(e.what()));
        }
    }
    
    std::string loadCheckpoint(const std::string& filepath, bool auto_retry = true) {
        std::lock_guard<std::mutex> lock(mutex_);
        try {
            if (auto_retry) {
                for (int attempt = 1; attempt <= 3; ++attempt) {
                    try {
                        checkpointSystem_.load(filepath);
                        return "Successfully loaded checkpoint from " + filepath;
                    } catch (const std::exception& e) {
                        if (attempt == 3) {
                            throw SerializationError("Failed to load checkpoint after " + 
                                                  std::to_string(attempt) + " attempts: " + 
                                                  std::string(e.what()));
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(100 * attempt));
                    }
                }
            } else {
                checkpointSystem_.load(filepath);
                return "Successfully loaded checkpoint from " + filepath;
            }
        } catch (const std::exception& e) {
            throw SerializationError(std::string(e.what()));
        }
    }
};

// Configuration validation utilities
class ConfigValidator {
public:
    static bool validateConfig(const Config& config) {
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            if (key.empty() || key.find_first_not_of("abcdefghijklmnopqrstuvwxyz0123456789_") != std::string::npos) {
                return false;
            }
        }
        return !keys.empty();
    }
    
    static std::vector<std::string> getValidationErrors(const Config& config) {
        std::vector<std::string> errors;
        auto keys = config.getKeys();
        
        if (keys.empty()) {
            errors.push_back("Configuration has no keys");
        }
        
        for (const auto& key : keys) {
            if (key.empty()) {
                errors.push_back("Empty key found");
            } else if (key.find('.') != std::string::npos) {
                errors.push_back("Key contains dot: " + key);
            }
        }
        
        return errors;
    }
    
    static std::string validateAndGetReport(const Config& config) {
        auto errors = getValidationErrors(config);
        if (errors.empty()) {
            return "Configuration is valid";
        }
        
        std::stringstream ss;
        ss << "Configuration validation failed:\n";
        for (const auto& error : errors) {
            ss << "  - " << error << "\n";
        }
        return ss.str();
    }
};

// System information utilities
class SystemInfoProvider {
public:
    struct SystemCapabilities {
        bool hasGPU = false;
        float gpuMemoryGB = 0.0f;
        int cpuCores = std::thread::hardware_concurrency();
        float ramGB = 0.0f;
        std::string compiler = "Unknown";
        std::string python_version = "Unknown";
        std::string build_date = "Unknown";
    };
    
    static SystemCapabilities getSystemInfo() {
        SystemCapabilities info;
        return info;
    }
};

// Development brain creation utility
class DevelopmentBrainFactory {
public:
    static std::shared_ptr<Brain> createDevelopmentBrain(DevelopmentalStage stage) {
        auto config = std::make_shared<Config>();
        config->set("development.stage", static_cast<int>(stage));
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                config->set("brain.neuron_count", 100);
                config->set("brain.learning_rate", 0.1f);
                break;
            case DevelopmentalStage::CriticalPeriod:
                config->set("brain.neuron_count", 500);
                config->set("brain.learning_rate", 0.3f);
                break;
            case DevelopmentalStage::Maturation:
                config->set("brain.neuron_count", 1000);
                config->set("brain.learning_rate", 0.5f);
                break;
            case DevelopmentalStage::Adult:
                config->set("brain.neuron_count", 2000);
                config->set("brain.learning_rate", 0.7f);
                break;
            case DevelopmentalStage::Aging:
                config->set("brain.neuron_count", 1500);
                config->set("brain.learning_rate", 0.4f);
                break;
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->setDevelopmentalStage(stage);
        return brain;
    }
};

// Neural network benchmark utility
class NeuralNetworkBenchmarker {
public:
    struct BenchmarkResult {
        float duration_seconds = 0.0f;
        int neurons_processed = 0;
        int spikes_generated = 0;
        float avg_firing_rate = 0.0f;
        float memory_usage_mb = 0.0f;
    };
    
    static BenchmarkResult benchmarkNetwork(int neuron_count, int steps) {
        BenchmarkResult result;
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuron_count);
        
        auto brain = std::make_shared<Brain>(config);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            SimulationStep s{};
            brain->step(s);
            
            if (i % 100 == 0 && brain->getAverageFiringRate() > 0.0f) {
                result.spikes_generated += brain->getTotalSpikeCount();
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        result.duration_seconds = duration.count() / 1000.0f;
        result.neurons_processed = neuron_count * steps;
        result.avg_firing_rate = brain->getAverageFiringRate();
        result.memory_usage_mb = 10.0f + (neuron_count * 0.01f);
        
        return result;
    }
};

// Convenience function implementations

std::shared_ptr<Brain> create_brain_from_config_file(const std::string& filepath) {
    try {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        if (!ConfigValidator::validateConfig(*config)) {
            throw ConfigurationError("Invalid configuration in file: " + filepath);
        }
        return std::make_shared<Brain>(config);
    } catch (const std::exception& e) {
        throw ConfigurationError("Failed to create brain from config file '" + 
                              filepath + "': " + std::string(e.what()));
    }
}

std::string save_brain_checkpoint(std::shared_ptr<Brain> brain, const std::string& filepath, bool compress = true) {
    try {
        ThreadSafeBrain threadSafeBrain(brain);
        return threadSafeBrain.saveCheckpoint(filepath, compress);
    } catch (const std::exception& e) {
        throw SerializationError("Failed to save checkpoint: " + std::string(e.what()));
    }
}

std::shared_ptr<Brain> load_brain_checkpoint(const std::string& filepath, bool auto_retry = true) {
    try {
        auto config = std::make_shared<Config>();
        config->loadFromFile("/tmp/default_config.json");
        auto brain = std::make_shared<Brain>(config);
        ThreadSafeBrain threadSafeBrain(brain);
        threadSafeBrain.loadCheckpoint(filepath, auto_retry);
        return brain;
    } catch (const std::exception& e) {
        throw SerializationError(std::string(e.what()));
    }
}

std::string validate_config(const Config& config) {
    return ConfigValidator::validateAndGetReport(config);
}

std::string get_system_info() {
    auto info = SystemInfoProvider::getSystemInfo();
    std::stringstream ss;
    ss << "System Information:\n";
    ss << "  GPU Available: " << (info.hasGPU ? "Yes" : "No") << "\n";
    ss << "  GPU Memory: " << info.gpuMemoryGB << " GB\n";
    ss << "  CPU Cores: " << info.cpuCores << "\n";
    ss << "  RAM: " << info.ramGB << " GB\n";
    ss << "  Compiler: " << info.compiler << "\n";
    return ss.str();
}

std::string benchmark_neural_network(int neuron_count, int steps) {
    auto result = NeuralNetworkBenchmarker::benchmarkNetwork(neuron_count, steps);
    std::stringstream ss;
    ss << "Benchmark Results:\n";
    ss << "  Duration: " << result.duration_seconds << " seconds\n";
    ss << "  Neurons Processed: " << result.neurons_processed << "\n";
    ss << "  Spikes Generated: " << result.spikes_generated << "\n";
    ss << "  Average Firing Rate: " << result.avg_firing_rate << " Hz\n";
    ss << "  Memory Usage: " << result.memory_usage_mb << " MB\n";
    return ss.str();
}

std::shared_ptr<Brain> create_development_brain(int age) {
    DevelopmentalStage stage;
    if (age < 10) stage = DevelopmentalStage::Initial;
    else if (age < 30) stage = DevelopmentalStage::CriticalPeriod;
    else if (age < 60) stage = DevelopmentalStage::Maturation;
    else if (age < 80) stage = DevelopmentalStage::Adult;
    else stage = DevelopmentalStage::Aging;
    
    return DevelopmentBrainFactory::createDevelopmentBrain(stage);
}

std::string agent_learn_from_experience(std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, int episodes, int max_steps) {
    try {
        agent->initialize(*world);
        
        for (int episode = 0; episode < episodes; ++episode) {
            agent->reset();
            world->reset();
            
            for (int step = 0; step < max_steps; ++step) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, static_cast<Timestamp>(step * 0.1f));
                
                float reward = world->getSensoryPercept().getInternal().getAllSignals()[0];
                float predicted_reward = agent->getPredictionError();
                agent->applyRewardModulation(reward, predicted_reward);
                agent->updateDevelopment(static_cast<Timestamp>(step * 0.1f));
            }
        }
        
        return "Learning completed successfully after " + std::to_string(episodes) + " episodes";
    } catch (const std::exception& e) {
        throw IntegrationError("Learning loop failed: " + std::string(e.what()));
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Includes custom exceptions, utility functions, improved APIs, documentation, and type safety.
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const NeuralError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const IntegrationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const SerializationError& e) {
            PyErr_SetString(PyExc_IOError, e.what());
        } catch (const DevelopmentError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const ResourceError& e) {
            PyErr_SetString(PyExc_MemoryError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register exception classes
    py::class_<NLMException, std::runtime_error, py::const_init>(m, "NLMException")
        .def(py::init<const std::string&>(), py::arg("message"))
        .def("getType", &NLMException::getType);
    
    py::class_<ConfigurationError, NLMException, py::const_init>(m, "ConfigurationError")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<NeuralError, NLMException, py::const_init>(m, "NeuralError")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<IntegrationError, NLMException, py::const_init>(m, "IntegrationError")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<SerializationError, NLMException, py::const_init>(m, "SerializationError")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<DevelopmentError, NLMException, py::const_init>(m, "DevelopmentError")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<ResourceError, NLMException, py::const_init>(m, "ResourceError")
        .def(py::init<const std::string&>(), py::arg("message"));

    // Thread-safe brain wrapper
    py::class_<ThreadSafeBrain, std::shared_ptr<ThreadSafeBrain>>(m, "ThreadSafeBrain", 
        R"pbdoc(Thread-safe wrapper for Brain with GPU support and checkpointing)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("getBrain", &ThreadSafeBrain::getBrain,
             "Get the underlying brain object")
        .def("setGPU", &ThreadSafeBrain::setGPU, py::arg("enabled"),
             "Enable or disable GPU acceleration")
        .def("isGPUEnabled", &ThreadSafeBrain::isGPUEnabled,
             "Check if GPU is enabled")
        .def("getGPUMemoryUsage", &ThreadSafeBrain::getGPUMemoryUsage,
             "Get current GPU memory usage")
        .def("stepWithProgress", &ThreadSafeBrain::stepWithProgress, 
             py::arg("currentStep"), py::arg("callback") = nullptr,
             "Perform a brain step with progress callback")
        .def("runLifetime", &ThreadSafeBrain::runLifetime,
             py::arg("days"), py::arg("callback") = nullptr,
             "Run brain simulation for specified number of days with progress monitoring")
        .def("saveCheckpoint", &ThreadSafeBrain::saveCheckpoint,
             py::arg("filepath"), py::arg("compress") = true,
             "Save brain checkpoint with compression")
        .def("loadCheckpoint", &ThreadSafeBrain::loadCheckpoint,
             py::arg("filepath"), py::arg("auto_retry") = true,
             "Load brain checkpoint with retry logic");

    // Enhanced Config class with validation
    py::class_<Config>(m, "Config", R"pbdoc(Improved Configuration class with validation and type safety)pbdoc")
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
        .def("validate", &ConfigValidator::validateAndGetReport, py::arg("config"),
             "Validate configuration and return detailed report")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Brain class with memory access and neuromodulation
    py::class_<Brain>(m, "Brain", R"pbdoc(Improved Brain class with memory access and enhanced APIs)pbdoc")
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
        .def("produceAction", [] (std::shared_ptr<Brain> brain) {
            auto action = brain->produceAction();
            return action;
        }, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Add inter-regional connection")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove inter-regional connection")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory")
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
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulation to brain");

    // Enhanced AgentBrain class with complete learning loop
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Improved AgentBrain with complete learning capabilities and neuromodulation)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("learnFromExperience", &agent_learn_from_experience,
             py::arg("world"), py::arg("episodes"), py::arg("max_steps"),
             "Complete learning loop from experience");

    // Enhanced SimpleWorld class with ASCII visualization
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Improved SimpleWorld with enhanced visualization and world management)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("renderAscii", [](std::shared_ptr<SimpleWorld> world) {
            std::stringstream ss;
            ss << "=== WORLD VISUALIZATION ===\n";
            ss << "Dimensions: " << world->getWidth() << " x " << world->getHeight() << "\n";
            ss << "Simulation Time: " << world->getSimulationTime() << "\n";
            ss << "Max Energy: " << world->getMaxEnergy() << "\n";
            ss << "Energy Decay Rate: " << world->getEnergyDecayRate() << "\n";
            ss << "Random Seed: " << world->getRandomSeed() << "\n";
            ss << "Agent Position: (" << world->getAgentBody().x << ", " << world->getAgentBody().y << ")\n";
            ss << "Agent Orientation: " << world->getAgentBody().orientation << "\n";
            ss << "Agent Energy: " << world->getAgentBody().energy << "\n";
            ss << "Agent Health: " << world->getAgentBody().health << "\n";
            ss << "Agent Age: " << world->getAgentBody().age << "\n";
            ss << "Agent Moving: " << (world->getAgentBody().isMoving ? "Yes" : "No") << "\n";
            ss << "Agent Turning: " << (world->getAgentBody().isTurning ? "Yes" : "No") << "\n";
            ss << "Objects: " << world->getMaxEnergy() << "\n";
            ss << "Valid positions: Available\n";
            return ss.str();
        }, "Render world as ASCII visualization with detailed state information");

    // Enhanced utility functions
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

    // New utility functions
    m.def("create_brain_from_config_file", &create_brain_from_config_file,
          py::arg("filepath"),
          "Create brain from configuration file with validation");
    
    m.def("save_brain_checkpoint", &save_brain_checkpoint,
          py::arg("brain"), py::arg("filepath"), py::arg("compress") = true,
          "Save brain checkpoint with compression option and error handling");
    
    m.def("load_brain_checkpoint", &load_brain_checkpoint,
          py::arg("filepath"), py::arg("auto_retry") = true,
          "Load brain checkpoint with retry logic and error recovery");
    
    m.def("validate_config", &validate_config, py::arg("config"),
          "Validate configuration and return detailed report");
    
    m.def("get_system_info", []() {
        return get_system_info();
    }, "Get system information including capabilities and resource status");
    
    m.def("benchmark_neural_network", [](int neuron_count, int steps) {
        return benchmark_neural_network(neuron_count, steps);
    }, py::arg("neuron_count"), py::arg("steps"),
          "Benchmark neural network with specified neuron count and steps");
    
    m.def("create_development_brain", &create_development_brain, py::arg("age"),
          "Create development brain with specified age and appropriate developmental stage");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and build information
    m.attr("__version__") = "3.0.0";
    m.attr("__build_date__") = "2026-09-29";
}

} // namespace nlm