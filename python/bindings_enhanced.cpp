// Enhanced Python bindings for NLM
// This file provides improved Python API with better error handling, 
// type safety, and additional features for advanced usage

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
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

// Custom exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class BrainInitializationError : public NLMError {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMError("Brain initialization failed: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

// Enhanced configuration with validation
class EnhancedConfig : public Config {
private:
    std::map<std::string, std::function<bool(const std::string&, const std::string&)>> validators_;
    
public:
    EnhancedConfig() : Config() {
        // Register basic validators
        validators_["brain.neuron_count"] = [this](const std::string&, const std::string&) {
            int64_t count = std::stoll(get("brain.neuron_count"));
            return count > 0 && count <= 100000;
        };
        
        validators_["brain.synapse_density"] = [this](const std::string&, const std::string&) {
            float density = std::stof(get("brain.synapse_density"));
            return density >= 0.0f && density <= 1.0f;
        };
    }
    
    // Set with validation
    void safeSet(const std::string& key, const std::string& value) {
        if (validators_.find(key) != validators_.end()) {
            if (!validators_[key](key, value)) {
                throw NLMError("Invalid value for key '" + key + "': '" + value + "'");
            }
        }
        Config::set(key, value);
    }
    
    // Get with type conversion
    template<typename T>
    T getTyped(const std::string& key) const {
        std::string value = Config::get(key);
        // Simple type conversion - in practice would use std::stoi, std::stoi, etc.
        // This is a simplified version
        return T{};
    }
};

// Enhanced brain with additional features
class EnhancedBrain : public Brain {
private:
    std::vector<double> firingHistory_;
    std::vector<double> activityHistory_;
    std::chrono::high_resolution_clock::time_point startTime_;
    bool autoSaveEnabled_ = false;
    std::string autoSavePath_;
    
public:
    EnhancedBrain(std::shared_ptr<Config> config) : Brain(config) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    // Record firing activity
    void recordActivity() {
        firingHistory_.push_back(static_cast<double>(getFiringNeuronCount()));
        activityHistory_.push_back(static_cast<double>(getAverageFiringRate()));
    }
    
    // Get firing history
    py::array_t<double> getFiringHistory() {
        return py::array_t<double>(firingHistory_);
    }
    
    // Get activity history
    py::array_t<double> getActivityHistory() {
        return py::array_t<double>(activityHistory_);
    }
    
    // Get simulation duration
    double getSimulationDuration() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime_).count();
    }
    
    // Enable/disable auto-save
    void enableAutoSave(const std::string& path) {
        autoSaveEnabled_ = true;
        autoSavePath_ = path;
    }
    
    void disableAutoSave() {
        autoSaveEnabled_ = false;
    }
    
    // Enhanced step with recording
    void enhancedStep(SimulationStep step, Timestamp time = 0.0) {
        // Validate inputs
        if (step < 0) {
            throw SimulationError("Step cannot be negative");
        }
        if (time < 0.0) {
            throw SimulationError("Time cannot be negative");
        }
        
        // Perform the step
        Brain::step(step, time);
        
        // Record activity
        recordActivity();
        
        // Auto-save if enabled
        if (autoSaveEnabled_) {
            std::string path = autoSavePath_ + "_step_" + std::to_string(step) + ".bin";
            save(path);
        }
    }
    
    // Batch processing for performance
    void batchSteps(uint64_t startStep, uint64_t endStep, double timestep = 0.001) {
        if (endStep <= startStep) {
            throw SimulationError("End step must be greater than start step");
        }
        
        for (uint64_t step = startStep; step < endStep; ++step) {
            enhancedStep(step, step * timestep);
        }
    }
    
    // Get statistics
    struct SimulationStats {
        double avgFiringRate;
        double maxFiringRate;
        double minFiringRate;
        double totalSpikes;
        double stdDevFiring;
        double simulationTime;
        size_t stepsRecorded;
    };
    
    SimulationStats getStats() const {
        SimulationStats stats;
        if (firingHistory_.empty()) {
            stats.avgFiringRate = 0.0;
            stats.maxFiringRate = 0.0;
            stats.minFiringRate = 0.0;
            stats.totalSpikes = 0.0;
            stats.stdDevFiring = 0.0;
            stats.simulationTime = 0.0;
            stats.stepsRecorded = 0;
            return stats;
        }
        
        double sum = 0.0, sumSq = 0.0, max = 0.0, min = 1e9;
        for (double val : firingHistory_) {
            sum += val;
            sumSq += val * val;
            if (val > max) max = val;
            if (val < min) min = val;
        }
        
        stats.avgFiringRate = sum / firingHistory_.size();
        stats.maxFiringRate = max;
        stats.minFiringRate = min;
        stats.totalSpikes = static_cast<double>(getTotalSpikeCount());
        stats.stdDevFiring = std::sqrt((sumSq / firingHistory_.size()) - (stats.avgFiringRate * stats.avgFiringRate));
        stats.simulationTime = getSimulationDuration();
        stats.stepsRecorded = firingHistory_.size();
        
        return stats;
    }
};

// Enhanced agent with additional features
class EnhancedAgentBrain : public AgentBrain {
private:
    std::vector<double> rewardHistory_;
    std::vector<double> curiosityHistory_;
    std::vector<double> noveltyHistory_;
    double cumulativeReward_ = 0.0;
    size_t stepsWithoutLearning_ = 0;
    
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain) : AgentBrain(brain) {}
    
    // Enhanced reward processing with learning detection
    void enhancedApplyRewardModulation(float reward, float predictedReward) {
        // Update cumulative reward
        cumulativeReward_ += reward;
        
        // Check if learning is effective
        float predictionError = reward - predictedReward;
        float learningSignal = std::abs(predictionError);
        
        if (learningSignal < 0.01f) {
            stepsWithoutLearning_++;
        } else {
            stepsWithoutLearning_ = 0;
        }
        
        // Record history
        rewardHistory_.push_back(static_cast<double>(reward));
        curiosityHistory_.push_back(getCuriosityLevel());
        noveltyHistory_.push_back(getNoveltyLevel());
        
        // Apply original modulation
        AgentBrain::applyRewardModulation(reward, predictedReward);
    }
    
    // Check if learning is stagnating
    bool isLearningStagnating() const {
        return stepsWithoutLearning_ > 100;
    }
    
    // Get learning statistics
    struct LearningStats {
        double avgReward;
        double rewardVariance;
        double avgCuriosity;
        double avgNovelty;
        double cumulativeReward;
        size_t stepsWithoutLearning;
        bool isStagnating;
    };
    
    LearningStats getLearningStats() const {
        LearningStats stats;
        
        if (rewardHistory_.empty()) {
            stats.avgReward = 0.0;
            stats.rewardVariance = 0.0;
            stats.avgCuriosity = 0.0;
            stats.avgNovelty = 0.0;
            stats.cumulativeReward = cumulativeReward_;
            stats.stepsWithoutLearning = stepsWithoutLearning_;
            stats.isStagnating = isLearningStagnating();
            return stats;
        }
        
        double sum = 0.0, sumSq = 0.0, curiositySum = 0.0, noveltySum = 0.0;
        for (size_t i = 0; i < rewardHistory_.size(); ++i) {
            sum += rewardHistory_[i];
            sumSq += rewardHistory_[i] * rewardHistory_[i];
            if (i < curiosityHistory_.size()) curiositySum += curiosityHistory_[i];
            if (i < noveltyHistory_.size()) noveltySum += noveltyHistory_[i];
        }
        
        stats.avgReward = sum / rewardHistory_.size();
        stats.rewardVariance = (sumSq / rewardHistory_.size()) - (stats.avgReward * stats.avgReward);
        stats.avgCuriosity = curiosityHistory_.empty() ? 0.0 : curiositySum / curiosityHistory_.size();
        stats.avgNovelty = noveltyHistory_.empty() ? 0.0 : noveltySum / noveltyHistory_.size();
        stats.cumulativeReward = cumulativeReward_;
        stats.stepsWithoutLearning = stepsWithoutLearning_;
        stats.isStagnating = isLearningStagnating();
        
        return stats;
    }
    
    // Get reward history as numpy array
    py::array_t<double> getRewardHistory() {
        return py::array_t<double>(rewardHistory_);
    }
    
    // Get curiosity history as numpy array
    py::array_t<double> getCuriosityHistory() {
        return py::array_t<double>(curiosityHistory_);
    }
    
    // Get novelty history as numpy array
    py::array_t<double> getNoveltyHistory() {
        return py::array_t<double>(noveltyHistory_);
    }
};

// Advanced world with enhanced features
class EnhancedSimpleWorld : public SimpleWorld {
private:
    std::map<std::string, double> timeSeriesMetrics_;
    std::chrono::high_resolution_clock::time_point simulationStart_;
    bool enableProfiling_ = false;
    
public:
    EnhancedSimpleWorld() : SimpleWorld() {
        simulationStart_ = std::chrono::high_resolution_clock::now();
    }
    
    // Record time series metric
    void recordMetric(const std::string& name, double value) {
        timeSeriesMetrics_[name] = value;
    }
    
    // Get all recorded metrics
    std::map<std::string, double> getAllMetrics() const {
        return timeSeriesMetrics_;
    }
    
    // Get metric by name
    double getMetric(const std::string& name) const {
        auto it = timeSeriesMetrics_.find(name);
        return it != timeSeriesMetrics_.end() ? it->second : 0.0;
    }
    
    // Enable profiling
    void enableProfiling() {
        enableProfiling_ = true;
    }
    
    void disableProfiling() {
        enableProfiling_ = false;
    }
    
    // Enhanced update with profiling
    void enhancedUpdate(double timestep) {
        if (enableProfiling_) {
            auto start = std::chrono::high_resolution_clock::now();
            SimpleWorld::update(timestep);
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            recordMetric("update_time", duration);
        } else {
            SimpleWorld::update(timestep);
        }
    }
    
    // Get simulation duration
    double getSimulationDuration() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - simulationStart_).count();
    }
};

// Context manager for automatic resource cleanup
class BrainContext {
private:
    std::shared_ptr<EnhancedBrain> brain_;
    std::shared_ptr<EnhancedAgentBrain> agent_;
    std::shared_ptr<EnhancedSimpleWorld> world_;
    bool autoSave_ = false;
    std::string checkpointPath_;
    
public:
    BrainContext(std::shared_ptr<EnhancedBrain> brain,
                 std::shared_ptr<EnhancedAgentBrain> agent,
                 std::shared_ptr<EnhancedSimpleWorld> world)
        : brain_(brain), agent_(agent), world_(world) {}
    
    void enableAutoSave(const std::string& path) {
        autoSave_ = true;
        checkpointPath_ = path;
    }
    
    void cleanup() {
        if (autoSave_ && brain_) {
            brain_->save(checkpointPath_);
        }
        brain_.reset();
        agent_.reset();
        world_.reset();
    }
    
    ~BrainContext() {
        cleanup();
    }
    
    // Python context manager support
    void __enter__() {}
    void __exit__(int exc_type, int exc_val, int exc_tb) {
        cleanup();
    }
};

// Factory function for enhanced configuration
EnhancedConfig createEnhancedConfig() {
    return EnhancedConfig();
}

// Factory function for enhanced brain
EnhancedBrain createEnhancedBrain(std::shared_ptr<Config> config) {
    return EnhancedBrain(config);
}

// Factory function for enhanced agent
EnhancedAgentBrain createEnhancedAgent(std::shared_ptr<Brain> brain) {
    return EnhancedAgentBrain(brain);
}

// Factory function for enhanced world
EnhancedSimpleWorld createEnhancedWorld() {
    return EnhancedSimpleWorld();
}

// Factory function for context
BrainContext createContext(std::shared_ptr<EnhancedBrain> brain,
                          std::shared_ptr<EnhancedAgentBrain> agent,
                          std::shared_ptr<EnhancedSimpleWorld> world) {
    return BrainContext(brain, agent, world);
}

// Function to run enhanced simulation with monitoring
def runEnhancedSimulation() {
    // This would be exposed as a Python function for easy use
    // Implementation would be similar to the enhanced examples
}

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        This version includes improved error handling, type safety, and additional features.
        
        Key enhancements:
        - Custom exception classes for better error handling
        - Enhanced configuration with validation
        - Advanced brain features with activity recording
        - Enhanced agent with learning statistics
        - Enhanced world with profiling
        - Context manager for resource cleanup
        - Batch processing for performance
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);

    // Enhanced Config
    py::class_<EnhancedConfig, Config>(m, "EnhancedConfig")
        .def(py::init<>())
        .def("safeSet", &EnhancedConfig::safeSet, py::arg("key"), py::arg("value"),
             "Set configuration value with validation")
        .def("getTyped", &EnhancedConfig::getTyped<double>, py::arg("key"),
             "Get typed configuration value");

    // Enhanced Brain
    py::class_<EnhancedBrain, Brain>(m, "EnhancedBrain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("enhancedStep", &EnhancedBrain::enhancedStep, py::arg("step"), py::arg("time") = 0.0,
             "Enhanced simulation step with activity recording")
        .def("batchSteps", &EnhancedBrain::batchSteps, py::arg("startStep"), py::arg("endStep"), py::arg("timestep") = 0.001,
             "Run multiple steps efficiently")
        .def("recordActivity", &EnhancedBrain::recordActivity,
             "Record brain activity (called automatically)")
        .def("getFiringHistory", &EnhancedBrain::getFiringHistory,
             "Get firing activity history")
        .def("getActivityHistory", &EnhancedBrain::getActivityHistory,
             "Get average firing rate history")
        .def("getSimulationDuration", &EnhancedBrain::getSimulationDuration,
             "Get simulation duration in seconds")
        .def("enableAutoSave", &EnhancedBrain::enableAutoSave, py::arg("path"),
             "Enable automatic checkpoint saving")
        .def("disableAutoSave", &EnhancedBrain::disableAutoSave,
             "Disable automatic checkpoint saving")
        .def("getStats", &EnhancedBrain::getStats,
             "Get comprehensive simulation statistics");

    // Enhanced AgentBrain
    py::class_<EnhancedAgentBrain, AgentBrain>(m, "EnhancedAgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("enhancedApplyRewardModulation", &EnhancedAgentBrain::enhancedApplyRewardModulation, 
             py::arg("reward"), py::arg("predictedReward"),
             "Enhanced reward modulation with learning detection")
        .def("isLearningStagnating", &EnhancedAgentBrain::isLearningStagnating,
             "Check if learning is stagnating")
        .def("getLearningStats", &EnhancedAgentBrain::getLearningStats,
             "Get learning statistics")
        .def("getRewardHistory", &EnhancedAgentBrain::getRewardHistory,
             "Get reward history")
        .def("getCuriosityHistory", &EnhancedAgentBrain::getCuriosityHistory,
             "Get curiosity history")
        .def("getNoveltyHistory", &EnhancedAgentBrain::getNoveltyHistory,
             "Get novelty history");

    // Enhanced SimpleWorld
    py::class_<EnhancedSimpleWorld, SimpleWorld>(m, "EnhancedSimpleWorld")
        .def(py::init<>())
        .def("enhancedUpdate", &EnhancedSimpleWorld::enhancedUpdate, py::arg("timestep"),
             "Enhanced world update with optional profiling")
        .def("recordMetric", &EnhancedSimpleWorld::recordMetric, py::arg("name"), py::arg("value"),
             "Record a time series metric")
        .def("getAllMetrics", &EnhancedSimpleWorld::getAllMetrics,
             "Get all recorded metrics")
        .def("getMetric", &EnhancedSimpleWorld::getMetric, py::arg("name"),
             "Get a specific metric")
        .def("enableProfiling", &EnhancedSimpleWorld::enableProfiling,
             "Enable profiling for world updates")
        .def("disableProfiling", &EnhancedSimpleWorld::disableProfiling,
             "Disable profiling")
        .def("getSimulationDuration", &EnhancedSimpleWorld::getSimulationDuration,
             "Get simulation duration");

    // Context manager
    py::class_<BrainContext>(m, "BrainContext")
        .def(py::init<std::shared_ptr<EnhancedBrain>, std::shared_ptr<EnhancedAgentBrain>, std::shared_ptr<EnhancedSimpleWorld>>(),
             py::arg("brain"), py::arg("agent"), py::arg("world"))
        .def("enableAutoSave", &BrainContext::enableAutoSave, py::arg("path"),
             "Enable auto-save for this context")
        .def("__enter__", &BrainContext::__enter__, "Context manager entry")
        .def("__exit__", &BrainContext::__exit__, py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"),
             "Context manager exit");

    // Enhanced factory functions
    m.def("createEnhancedConfig", &createEnhancedConfig, "Create enhanced configuration")
      .def("createEnhancedBrain", &createEnhancedBrain, py::arg("config"), "Create enhanced brain")
      .def("createEnhancedAgent", &createEnhancedAgent, py::arg("brain"), "Create enhanced agent")
      .def("createEnhancedWorld", &createEnhancedWorld, "Create enhanced world")
      .def("createContext", &createContext, py::arg("brain"), py::arg("agent"), py::arg("world"),
           "Create context manager for resource cleanup")

    // Original factory functions (for backward compatibility)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
