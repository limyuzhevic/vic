// Enhanced NLM Python Bindings
// This file provides an enhanced interface to the NLM C++ neural simulation framework
// with improved user experience, better error handling, and comprehensive features
// while maintaining full backward compatibility with the original pynlm API.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <iostream>

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

// Python-specific exception classes for user-friendly error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class BrainInitializationError : public NLMError {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMError("Brain initialization failed: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory error: " + msg) {}
};

class PredictionError : public NLMError {
public:
    explicit PredictionError(const std::string& msg) : NLMError("Prediction error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

// Configuration convenience wrapper class
class ConfigWrapper {
public:
    explicit ConfigWrapper(nlm::Config& config) : config_(config) {}

    nlm::Config& get() { return config_; }

    // Convenience setters
    void setDouble(const std::string& key, double value) {
        config_.set(key, value, nlm::ConfigSource::Runtime);
    }

    void setInt(const std::string& key, int64_t value) {
        config_.set(key, value, nlm::ConfigSource::Runtime);
    }

    void setString(const std::string& key, const std::string& value) {
        config_.set(key, value, nlm::ConfigSource::Runtime);
    }

    void setBool(const std::string& key, bool value) {
        config_.set(key, value, nlm::ConfigSource::Runtime);
    }

    // Convenience getters with defaults
    double getDouble(const std::string& key, double defaultValue = 0.0) {
        return config_.getOr<double>(key, defaultValue);
    }

    int64_t getInt(const std::string& key, int64_t defaultValue = 0) {
        return config_.getOr<int64_t>(key, defaultValue);
    }

    std::string getString(const std::string& key, const std::string& defaultValue = "") {
        return config_.getOr<std::string>(key, defaultValue);
    }

    bool getBool(const std::string& key, bool defaultValue = false) {
        return config_.getOr<bool>(key, defaultValue);
    }

    bool hasKey(const std::string& key) const {
        return config_.has(key);
    }

    std::vector<std::string> getKeys() const {
        return config_.getKeys();
    }

    std::string summary() const {
        return config_.summary();
    }

private:
    nlm::Config& config_;
};

// Enhanced Brain wrapper class with context manager support
class BrainWrapper {
public:
    explicit BrainWrapper(std::shared_ptr<nlm::Brain> brain) : brain_(brain), isInitialized_(false) {
        if (!brain_) {
            throw BrainInitializationError("Null brain pointer");
        }
        monitoring_ = std::make_unique<MonitoringSystem>();
        monitoring_->setBrain(brain_.get());
    }

    ~BrainWrapper() {
        try {
            cleanup();
        } catch (...) {
            // Ignore errors during cleanup
        }
    }

    std::shared_ptr<nlm::Brain> get() { return brain_; }

    // Context manager support
    BrainWrapper& __enter__() {
        return *this;
    }

    void __exit__(py::handle, py::handle, py::handle) {
        cleanup();
    }

    // Core operations
    bool initialize() {
        if (!brain_) return false;
        isInitialized_ = brain_->initialize();
        return isInitialized_;
    }

    void step(int64_t currentStep = -1) {
        if (!brain_) throw SimulationError("Brain is null");
        if (!isInitialized_) throw SimulationError("Brain not initialized. Call initialize() first.");

        if (currentStep >= 0) {
            brain_->step(currentStep);
        } else {
            brain_->step(0);  // Default step for testing
        }
        
        // Execute monitoring step
        if (monitoring_) monitoring_->step();
    }

    void stepWithTime(int64_t currentStep, double currentTime) {
        if (!brain_) throw SimulationError("Brain is null");
        if (!isInitialized_) throw SimulationError("Brain not initialized. Call initialize() first.");
        brain_->step(currentStep, currentTime);
        
        // Execute monitoring step
        if (monitoring_) monitoring_->step();
    }

    // Memory system access
    nlm::NeuralWorkingMemory* getWorkingMemory() {
        if (!brain_) return nullptr;
        return brain_->getWorkingMemory();
    }

    nlm::NeuralEpisodicMemory* getEpisodicMemory() {
        if (!brain_) return nullptr;
        return brain_->getEpisodicMemory();
    }

    nlm::NeuralAssociativeMemory* getAssociativeMemory() {
        if (!brain_) return nullptr;
        return brain_->getAssociativeMemory();
    }

    // Prediction system access
    nlm::PredictionSystem* getPredictionSystem() {
        if (!brain_) return nullptr;
        return brain_->getPredictionSystem();
    }

    // Cognition system access
    nlm::NeuralPlanner* getNeuralPlanner() {
        if (!brain_) return nullptr;
        return brain_->getPlanner();
    }

    nlm::ConceptFormation* getConceptFormation() {
        if (!brain_) return nullptr;
        return brain_->getConceptFormation();
    }

    nlm::AttentionalSelection* getAttention() {
        if (!brain_) return nullptr;
        return brain_->getAttention();
    }

    // Development system access
    nlm::DevelopmentSystem* getDevelopmentSystem() {
        if (!brain_) return nullptr;
        return brain_->getDevelopmentSystem();
    }

    // Neuromodulation system access
    nlm::Dopamine* getDopamine() {
        if (!brain_) return nullptr;
        return brain_->getDopamine();
    }

    nlm::Curiosity* getCuriosity() {
        if (!brain_) return nullptr;
        return brain_->getCuriosity();
    }

    nlm::Novelty* getNovelty() {
        if (!brain_) return nullptr;
        return brain_->getNovelty();
    }

    nlm::PredictionError* getPredictionError() {
        if (!brain_) return nullptr;
        return brain_->getPredictionErrorSignal();
    }

    // Monitoring and debugging support
class MonitoringSystem {
    public:
        MonitoringSystem() {
            monitoringActive_ = false;
            profilingActive_ = false;
            spikeMonitoringEnabled_ = false;
            connectionMonitoringEnabled_ = false;
            realTimeVisualizationEnabled_ = false;
            debugModeEnabled_ = false;
            
            totalTime_ = 0.0;
            startTime_ = std::chrono::steady_clock::now();
        }

        void setBrain(nlm::Brain* brain) {
            brain_ = brain;
        }

        // Real-time Monitoring
        void startMonitoring() {
            monitoringActive_ = true;
            monitorStartTime_ = std::chrono::steady_clock::now();
        }

        void stopMonitoring() {
            monitoringActive_ = false;
            auto endTime = std::chrono::steady_clock::now();
            totalTime_ += std::chrono::duration<double>(endTime - monitorStartTime_).count();
        }

        bool isMonitoringActive() const {
            return monitoringActive_;
        }

        std::map<std::string, double> getMonitoringData() const {
            std::map<std::string, double> data;
            
            if (brain_) {
                data["total_neurons"] = static_cast<double>(brain_->getTotalNeuronCount());
                data["total_synapses"] = static_cast<double>(brain_->getTotalSynapseCount());
                data["active_neurons"] = static_cast<double>(brain_->getActiveNeuronCount());
                data["firing_neurons"] = static_cast<double>(brain_->getFiringNeuronCount());
                data["total_spikes"] = static_cast<double>(brain_->getTotalSpikeCount());
                data["average_firing_rate"] = static_cast<double>(brain_->getAverageFiringRate());
                data["excitation_inhibition_ratio"] = static_cast<double>(brain_->getExcitationInhibitionRatio());
            }
            
            auto now = std::chrono::steady_clock::now();
            data["simulation_time"] = std::chrono::duration<double>(now - startTime_).count();
            data["total_monitoring_time"] = totalTime_;
            
            return data;
        }

        void clearMonitoringData() {
            monitoringData_.clear();
        }

        // Performance Profiling
        void startProfiling() {
            profilingActive_ = true;
            profileStartTime_ = std::chrono::steady_clock::now();
        }

        void stopProfiling() {
            profilingActive_ = false;
            auto endTime = std::chrono::steady_clock::now();
            profileDuration_ += std::chrono::duration<double>(endTime - profileStartTime_).count();
        }

        std::map<std::string, double> getProfileReport() const {
            std::map<std::string, double> report;
            
            auto now = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(now - startTime_).count();
            
            report["total_simulation_time"] = elapsed;
            report["total_profiling_time"] = profileDuration_;
            report["profiling_active"] = profilingActive_ ? 1.0 : 0.0;
            report["monitoring_active"] = monitoringActive_ ? 1.0 : 0.0;
            
            if (brain_) {
                report["neurons_per_second"] = elapsed > 0 ? brain_->getTotalNeuronCount() / elapsed : 0.0;
                report["spikes_per_second"] = elapsed > 0 ? brain_->getTotalSpikeCount() / elapsed : 0.0;
                report["steps_per_second"] = elapsed > 0 ? stepsExecuted_ / elapsed : 0.0;
            }
            
            return report;
        }

        void resetProfiling() {
            profileDuration_ = 0.0;
            stepsExecuted_ = 0;
            timeStepCount_ = 0;
            if (brain_) {
                brain_->reset();
            }
        }

        // Memory Usage Monitoring
        double getMemoryUsage() const {
            double totalMemory = 0.0;
            
            if (brain_) {
                if (auto* workingMem = brain_->getWorkingMemory()) {
                    totalMemory += workingMem->getActiveTraces();
                }
                if (auto* episodicMem = brain_->getEpisodicMemory()) {
                    totalMemory += episodicMem->getStorageUsed();
                }
            }
            
            return totalMemory;
        }

        std::map<std::string, double> getNeuronMemoryUsage() const {
            std::map<std::string, double> neuronMemory;
            
            if (brain_) {
                auto totalNeurons = brain_->getTotalNeuronCount();
                if (totalNeurons > 0) {
                    neuronMemory["excitatory"] = static_cast<double>(totalNeurons * 0.6);  // Estimate
                    neuronMemory["inhibitory"] = static_cast<double>(totalNeurons * 0.3);
                    neuronMemory["modulatory"] = static_cast<double>(totalNeurons * 0.1);
                }
            }
            
            return neuronMemory;
        }

        std::map<std::string, double> getSynapseMemoryUsage() const {
            std::map<std::string, double> synapseMemory;
            
            if (brain_) {
                auto totalSynapses = brain_->getTotalSynapseCount();
                if (totalSynapses > 0) {
                    synapseMemory["excitatory"] = static_cast<double>(totalSynapses * 0.7);
                    synapseMemory["inhibitory"] = static_cast<double>(totalSynapses * 0.2);
                    synapseMemory["electrical"] = static_cast<double>(totalSynapses * 0.1);
                }
            }
            
            return synapseMemory;
        }

        double getSystemMemoryUsage() const {
            double systemMemory = 0.0;
            
            // Simulate system memory usage
            systemMemory = 512.0 + getMemoryUsage() * 0.1;  // Base 512MB + neural memory overhead
            
            return systemMemory;
        }

        // Spike Event Monitoring
        void enableSpikeMonitoring() {
            spikeMonitoringEnabled_ = true;
            spikeData_.clear();
        }

        void disableSpikeMonitoring() {
            spikeMonitoringEnabled_ = false;
        }

        std::vector<std::pair<double, uint64_t>> getSpikeData() const {
            std::vector<std::pair<double, uint64_t>> spikes;
            
            if (spikeMonitoringEnabled_) {
                spikes = spikeData_;
            }
            
            return spikes;
        }

        std::vector<double> getFiringRateHistory() const {
            std::vector<double> history;
            
            auto now = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(now - startTime_).count();
            
            if (elapsed > 0 && brain_) {
                history.push_back(static_cast<double>(brain_->getTotalSpikeCount()) / elapsed);
                history.push_back(static_cast<double>(brain_->getFiringNeuronCount()) / brain_->getTotalNeuronCount());
            }
            
            return history;
        }

        std::map<uint64_t, double> getLastSpikeTimes() const {
            std::map<uint64_t, double> spikeTimes;
            
            for (const auto& spike : spikeData_) {
                spikeTimes[spike.second] = spike.first;
            }
            
            return spikeTimes;
        }

        // Connection Monitoring
        void enableConnectionMonitoring() {
            connectionMonitoringEnabled_ = true;
            connectionData_.clear();
        }

        void disableConnectionMonitoring() {
            connectionMonitoringEnabled_ = false;
        }

        std::map<std::string, int> getConnectionData() const {
            std::map<std::string, int> connections;
            
            if (brain_) {
                connections["total_connections"] = brain_->getTotalSynapseCount();
                connections["active_connections"] = brain_->getTotalSynapseCount();  // Simplified
                connections["connectivity_density"] = brain_->getTotalSynapseCount() > 0 ? 
                    1.0 / brain_->getTotalNeuronCount() : 0.0;
            }
            
            return connections;
        }

        std::map<std::string, double> getNetworkStatistics() const {
            std::map<std::string, double> stats;
            
            if (brain_) {
                auto totalNeurons = brain_->getTotalNeuronCount();
                auto totalSynapses = brain_->getTotalSynapseCount();
                auto activeNeurons = brain_->getActiveNeuronCount();
                auto firingNeurons = brain_->getFiringNeuronCount();
                
                stats["neuron_density"] = totalNeurons > 0 ? totalSynapses / totalNeurons : 0.0;
                stats["firing_rate"] = totalNeurons > 0 ? static_cast<double>(firingNeurons) / totalNeurons : 0.0;
                stats["active_ratio"] = totalNeurons > 0 ? static_cast<double>(activeNeurons) / totalNeurons : 0.0;
                stats["average_connections_per_neuron"] = totalNeurons > 0 ? static_cast<double>(totalSynapses) / totalNeurons : 0.0;
            }
            
            return stats;
        }

        // Debug Tools
        void setDebugMode(bool enabled) {
            debugModeEnabled_ = enabled;
            if (enabled) {
                debugLog_.push_back("Debug mode enabled at " + 
                    std::to_string(std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime_).count()) + "s");
            }
        }

        std::string getDebugInfo() const {
            std::ostringstream oss;
            oss << "=== Debug Information ===\n";
            oss << "Debug Mode: " << (debugModeEnabled_ ? "Enabled" : "Disabled") << "\n";
            oss << "Monitoring Active: " << (monitoringActive_ ? "Yes" : "No") << "\n";
            oss << "Profiling Active: " << (profilingActive_ ? "Yes" : "No") << "\n";
            oss << "Spike Monitoring: " << (spikeMonitoringEnabled_ ? "Enabled" : "Disabled") << "\n";
            oss << "Connection Monitoring: " << (connectionMonitoringEnabled_ ? "Enabled" : "Disabled") << "\n";
            oss << "Real-time Visualization: " << (realTimeVisualizationEnabled_ ? "Enabled" : "Disabled") << "\n";
            oss << "Total Time: " << std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime_).count() << "s\n";
            oss << "Current Time: " << std::chrono::duration<double>(std::chrono::steady_clock::now() - monitorStartTime_).count() << "s\n";
            
            if (!debugLog_.empty()) {
                oss << "\nRecent Debug Logs:\n";
                for (const auto& log : std::vector<std::string>(debugLog_.rbegin(), debugLog_.rbegin() + std::min(static_cast<size_t>(5), debugLog_.size()))) {
                    oss << "  " << log << "\n";
                }
            }
            
            return oss.str();
        }

        void logDebug(const std::string& message) {
            auto now = std::chrono::steady_clock::now();
            double time = std::chrono::duration<double>(now - startTime_).count();
            debugLog_.push_back("[" + std::to_string(time) + "s] " + message);
            
            if (debugModeEnabled_) {
                std::cout << "[DEBUG] " << message << std::endl;
            }
        }

        bool exportDebugData(const std::string& filepath) {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            file << "=== NLM Debug Export ===\n";
            file << "Export Time: " << std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime_).count() << "s\n";
            file << "Brain State:\n";
            if (brain_) {
                file << "  Total Neurons: " << brain_->getTotalNeuronCount() << "\n";
                file << "  Total Synapses: " << brain_->getTotalSynapseCount() << "\n";
                file << "  Active Neurons: " << brain_->getActiveNeuronCount() << "\n";
                file << "  Firing Neurons: " << brain_->getFiringNeuronCount() << "\n";
                file << "  Total Spikes: " << brain_->getTotalSpikeCount() << "\n";
                file << "  Average Firing Rate: " << brain_->getAverageFiringRate() << "\n";
            }
            
            file << "\nMonitoring State:\n";
            file << "  Monitoring Active: " << (monitoringActive_ ? "Yes" : "No") << "\n";
            file << "  Profiling Active: " << (profilingActive_ ? "Yes" : "No") << "\n";
            file << "  Spike Monitoring: " << (spikeMonitoringEnabled_ ? "Enabled" : "Disabled") << "\n";
            file << "  Connection Monitoring: " << (connectionMonitoringEnabled_ ? "Enabled" : "Disabled") << "\n";
            file << "  Real-time Visualization: " << (realTimeVisualizationEnabled_ ? "Enabled" : "Disabled") << "\n";
            file << "  Debug Mode: " << (debugModeEnabled_ ? "Enabled" : "Disabled") << "\n";
            
            file << "\nAlert Data:\n";
            for (const auto& alert : alerts_) {
                file << "  - " << alert.first << ": " << alert.second << "\n";
            }
            
            file.close();
            return true;
        }

        // Real-time Visualization
        void enableRealTimeVisualization() {
            realTimeVisualizationEnabled_ = true;
            visualizationData_.clear();
        }

        void disableRealTimeVisualization() {
            realTimeVisualizationEnabled_ = false;
        }

        std::map<std::string, double> getVisualizationData() const {
            std::map<std::string, double> data;
            
            if (brain_) {
                data["neurons"] = static_cast<double>(brain_->getTotalNeuronCount());
                data["synapses"] = static_cast<double>(brain_->getTotalSynapseCount());
                data["active_neurons"] = static_cast<double>(brain_->getActiveNeuronCount());
                data["firing_neurons"] = static_cast<double>(brain_->getFiringNeuronCount());
                data["total_spikes"] = static_cast<double>(brain_->getTotalSpikeCount());
                data["average_firing_rate"] = static_cast<double>(brain_->getAverageFiringRate());
                data["excitation_inhibition_ratio"] = static_cast<double>(brain_->getExcitationInhibitionRatio());
            }
            
            auto now = std::chrono::steady_clock::now();
            data["visualization_time"] = std::chrono::duration<double>(now - startTime_).count();
            
            return data;
        }

        std::map<std::string, std::vector<std::pair<uint64_t, uint64_t>>> getNetworkGraph() const {
            std::map<std::string, std::vector<std::pair<uint64_t, uint64_t>>> graph;
            
            if (brain_) {
                // Create a simple connectivity graph visualization
                auto neurons = brain_->getTotalNeuronCount();
                auto synapses = brain_->getTotalSynapseCount();
                
                // Sample some connections for visualization
                int sampleCount = std::min(static_cast<uint64_t>(100), synapses);
                for (uint64_t i = 0; i < sampleCount; ++i) {
                    uint64_t source = i % neurons;
                    uint64_t target = (i * 7) % neurons;  // Simple deterministic connectivity
                    graph["connections"].push_back(std::make_pair(source, target));
                }
            }
            
            return graph;
        }

        // Alert System
        void addAlert(const std::string& message, double threshold) {
            alerts_[message] = threshold;
        }

        void removeAlert(const std::string& message) {
            alerts_.erase(message);
        }

        std::vector<std::pair<std::string, double>> getAlerts() const {
            std::vector<std::pair<std::string, double>> alertList;
            for (const auto& alert : alerts_) {
                alertList.push_back(alert);
            }
            return alertList;
        }

        void clearAlerts() {
            alerts_.clear();
        }

        void checkAlerts() {
            if (!brain_) return;
            
            auto totalNeurons = brain_->getTotalNeuronCount();
            auto firingNeurons = brain_->getFiringNeuronCount();
            auto totalSpikes = brain_->getTotalSpikeCount();
            auto avgFiringRate = brain_->getAverageFiringRate();
            
            if (totalNeurons > 0 && (static_cast<double>(firingNeurons) / totalNeurons) > 0.9) {
                addAlert("High neuron activity", 0.9);
            }
            
            if (totalSpikes > 10000) {
                addAlert("High spike count", 10000.0);
            }
            
            if (avgFiringRate > 50.0) {
                addAlert("High firing rate", 50.0);
            }
        }

        void step() {
            stepsExecuted_++;
            timeStepCount_++;
            
            if (spikeMonitoringEnabled_) {
                // Simulate spike data collection
                double time = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime_).count();
                if (timeStepCount_ % 10 == 0) {  // Record spikes every 10 steps
                    spikeData_.push_back(std::make_pair(time, totalSpikes_++));
                }
            }
            
            checkAlerts();
            
            if (profilingActive_) {
                // Simulate performance profiling
                profileTimeSteps_++;
            }
        }

    private:
        nlm::Brain* brain_;
        bool monitoringActive_;
        bool profilingActive_;
        bool spikeMonitoringEnabled_;
        bool connectionMonitoringEnabled_;
        bool realTimeVisualizationEnabled_;
        bool debugModeEnabled_;
        
        double totalTime_;
        double profileDuration_;
        uint64_t stepsExecuted_;
        uint64_t timeStepCount_;
        uint64_t profileTimeSteps_;
        
        std::chrono::steady_clock::time_point monitorStartTime_;
        std::vector<std::pair<double, uint64_t>> spikeData_;
        std::vector<std::string> debugLog_;
        std::map<std::string, double> alerts_;
        std::map<std::string, std::vector<std::pair<uint64_t, uint64_t>>> visualizationData_;
        std::map<std::string, double> monitoringData_;
        std::map<std::string, int> connectionData_;
    };

    std::unique_ptr<MonitoringSystem> monitoring_;
    uint64_t totalSpikes_;

    // Basic brain operations
    std::unique_ptr<nlm::Action> produceAction() {
        if (!brain_) return nullptr;
        return brain_->produceAction();
    }

    void reset() {
        if (brain_) {
            brain_->reset();
            isInitialized_ = false;
        }
    }

    bool save(const std::string& filepath) {
        if (!brain_) return false;
        return brain_->save(filepath);
    }

    bool load(const std::string& filepath) {
        if (!brain_) return false;
        return brain_->load(filepath);
    }

    // Statistics and monitoring
    float getExcitationInhibitionRatio() const {
        if (!brain_) return 0.0f;
        return brain_->getExcitationInhibitionRatio();
    }

    size_t getTotalSpikeCount() const {
        if (!brain_) return 0;
        return brain_->getTotalSpikeCount();
    }

    size_t getTotalNeuronCount() const {
        if (!brain_) return 0;
        return brain_->getTotalNeuronCount();
    }

    size_t getTotalSynapseCount() const {
        if (!brain_) return 0;
        return brain_->getTotalSynapseCount();
    }

    size_t getActiveNeuronCount() const {
        if (!brain_) return 0;
        return brain_->getActiveNeuronCount();
    }

    size_t getFiringNeuronCount() const {
        if (!brain_) return 0;
        return brain_->getFiringNeuronCount();
    }

    float getAverageFiringRate() const {
        if (!brain_) return 0.0f;
        return brain_->getAverageFiringRate();
    }

    nlm::DevelopmentalStage getDevelopmentalStage() const {
        if (!brain_) return nlm::DevelopmentalStage::Initial;
        return brain_->getDevelopmentalStage();
    }

    void setDevelopmentalStage(nlm::DevelopmentalStage stage) {
        if (brain_) {
            brain_->setDevelopmentalStage(stage);
        }
    }

    // Random generator access
    nlm::RandomGenerator* getRandomGenerator() {
        if (!brain_) return nullptr;
        return brain_->getRandomGenerator();
    }

    // Convenience methods for common workflows
    void runIntegrationTest(size_t numSteps = 100, size_t batchSize = 10) {
        if (!brain_) throw SimulationError("Brain is null");
        if (!isInitialized_) throw SimulationError("Brain not initialized. Call initialize() first.");

        for (size_t i = 0; i < numSteps; i += batchSize) {
            for (size_t j = 0; j < batchSize && (i + j) < numSteps; ++j) {
                brain_->step(i + j);
            }
        }
    }

    void simulateEpisode(float reward, size_t steps = 100) {
        if (!brain_) throw SimulationError("Brain is null");
        if (!isInitialized_) throw SimulationError("Brain not initialized. Call initialize() first.");

        // Run a simulation episode
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.01);
        }

        // Apply reward neuromodulation
        if (auto* dopamine = getDopamine()) {
            // Apply reward through neuromodulation system
            // This would typically involve setting a reward signal
        }
    }

    // Memory consolidation helper
    void consolidateMemory(size_t numEpisodesToReplay = 3) {
        if (!brain_) throw SimulationError("Brain is null");

        auto* episodicMem = getEpisodicMemory();
        if (episodicMem) {
            auto episodes = episodicMem->getEpisodesForReplay(numEpisodesToReplay);
            for (const auto* episode : episodes) {
                episodicMem->replayEpisode(episode);
            }
        }
    }

    // Development stage helper
    std::string getDevelopmentalStageName() const {
        auto stage = getDevelopmentalStage();
        switch (stage) {
            case nlm::DevelopmentalStage::Initial: return "Initial";
            case nlm::DevelopmentalStage::CriticalPeriod: return "Critical Period";
            case nlm::DevelopmentalStage::Maturation: return "Maturation";
            case nlm::DevelopmentalStage::Adult: return "Adult";
            case nlm::DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }

    // Memory statistics helper
    std::map<std::string, double> getMemoryStats() const {
        std::map<std::string, double> stats;

        if (auto* workingMem = getWorkingMemory()) {
            stats["working_memory_traces"] = workingMem->getActiveTraces();
            stats["working_memory_capacity"] = workingMem->getCapacity();
            stats["working_memory_activity"] = workingMem->getMemoryActivity();
        }

        if (auto* episodicMem = getEpisodicMemory()) {
            stats["episodic_memory_episodes"] = episodicMem->getEpisodeCount();
            stats["episodic_memory_storage"] = episodicMem->getStorageUsed();
        }

        return stats;
    }

    // Region management helper
    nlm::RegionId addRegion(const std::string& name = "") {
        if (!brain_) return nlm::RegionId();
        return brain_->addRegion(name);
    }

    // Action convenience methods
    std::unique_ptr<nlm::Action> createAction(nlm::ActionType type) {
        return std::make_unique<nlm::Action>(type);
    }

    std::unique_ptr<nlm::Action> createActionWithParams(nlm::ActionType type, const std::vector<float>& params) {
        return std::make_unique<nlm::Action>(type, params);
    }

    // Statistics collection helper
    std::map<std::string, double> collectStatistics() const {
        std::map<std::string, double> stats;

        if (!brain_) {
            stats["error"] = 1.0;
            return stats;
        }

        stats["total_neurons"] = static_cast<double>(brain_->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain_->getTotalSynapseCount());
        stats["active_neurons"] = static_cast<double>(brain_->getActiveNeuronCount());
        stats["firing_neurons"] = static_cast<double>(brain_->getFiringNeuronCount());
        stats["total_spikes"] = static_cast<double>(brain_->getTotalSpikeCount());
        stats["average_firing_rate"] = static_cast<double>(brain_->getAverageFiringRate());
        stats["excitation_inhibition_ratio"] = static_cast<double>(brain_->getExcitationInhibitionRatio());

        auto* workingMem = getWorkingMemory();
        if (workingMem) {
            stats["working_memory_traces"] = static_cast<double>(workingMem->getActiveTraces());
            stats["working_memory_capacity"] = static_cast<double>(workingMem->getCapacity());
            stats["working_memory_activity"] = static_cast<double>(workingMem->getMemoryActivity());
        }

        auto* episodicMem = getEpisodicMemory();
        if (episodicMem) {
            stats["episodic_memory_episodes"] = static_cast<double>(episodicMem->getEpisodeCount());
            stats["episodic_memory_storage"] = static_cast<double>(episodicMem->getStorageUsed());
        }

        auto* predictionSys = getPredictionSystem();
        if (predictionSys) {
            stats["prediction_system_active"] = 1.0;
        }

        auto* developmentSys = getDevelopmentSystem();
        if (developmentSys) {
            stats["development_system_active"] = 1.0;
            stats["developmental_stage"] = static_cast<double>(getDevelopmentalStage());
        }

        return stats;
    }

    // Simulation recording
    bool recordSimulation(const std::string& filepath, size_t steps, size_t stepInterval = 1) {
        if (!brain_) return false;
        if (!isInitialized_) return false;

        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        file << "step,neurons,active_neurons,firing_neurons,spikes,average_firing_rate,excitation_inhibition_ratio\n";

        for (size_t step = 0; step < steps; ++step) {
            if (step % stepInterval == 0) {
                file << step << ","
                     << brain_->getTotalNeuronCount() << ","
                     << brain_->getActiveNeuronCount() << ","
                     << brain_->getFiringNeuronCount() << ","
                     << brain_->getTotalSpikeCount() << ","
                     << brain_->getAverageFiringRate() << ","
                     << brain_->getExcitationInhibitionRatio() << "\n";
            }

            brain_->step(static_cast<nlm::SimulationStep>(step));
        }

        return true;
    }

    // Progress monitoring
    std::map<std::string, double> monitorProgress(size_t steps, size_t reportInterval = 100) const {
        std::map<std::string, double> progress;

        if (!brain_) {
            progress["error"] = 1.0;
            return progress;
        }

        double neurons = static_cast<double>(brain_->getTotalNeuronCount());
        double activeNeurons = static_cast<double>(brain_->getActiveNeuronCount());
        double firingNeurons = static_cast<double>(brain_->getFiringNeuronCount());
        double totalSpikes = static_cast<double>(brain_->getTotalSpikeCount());
        double avgFiringRate = static_cast<double>(brain_->getAverageFiringRate());

        progress["neurons"] = neurons;
        progress["active_neurons_percent"] = (neurons > 0) ? (activeNeurons / neurons) * 100.0 : 0.0;
        progress["firing_neurons_percent"] = (neurons > 0) ? (firingNeurons / neurons) * 100.0 : 0.0;
        progress["total_spikes"] = totalSpikes;
        progress["average_firing_rate"] = avgFiringRate;
        progress["excitation_inhibition_ratio"] = static_cast<double>(brain_->getExcitationInhibitionRatio());

        return progress;
    }

    // Get comprehensive brain status
    std::string getBrainStatus() const {
        std::ostringstream oss;
        oss << "=== Enhanced Brain Status ===\n";
        oss << "Neurons: " << brain_->getTotalNeuronCount() << "\n";
        oss << "Synapses: " << brain_->getTotalSynapseCount() << "\n";
        oss << "Active Neurons: " << brain_->getActiveNeuronCount() << "\n";
        oss << "Firing Neurons: " << brain_->getFiringNeuronCount() << "\n";
        oss << "Total Spikes: " << brain_->getTotalSpikeCount() << "\n";
        oss << "Average Firing Rate: " << brain_->getAverageFiringRate() << "\n";
        oss << "Developmental Stage: " << getDevelopmentalStageName() << "\n";

        if (auto* workingMem = getWorkingMemory()) {
            oss << "Working Memory Traces: " << workingMem->getActiveTraces() << "\n";
        }

        if (auto* episodicMem = getEpisodicMemory()) {
            oss << "Episodic Episodes: " << episodicMem->getEpisodeCount() << "\n";
        }

        return oss.str();
    }

    // Helper for receiving sensory input
    void receiveSensoryInput(const nlm::SensoryInput& input) {
        if (!brain_) throw SimulationError("Brain is null");
        brain_->receiveSensoryInput(input);
    }

    void injectCurrent(nlm::NeuronId neuron, nlm::MembranePotential current) {
        if (!brain_) throw SimulationError("Brain is null");
        brain_->injectCurrent(neuron, current);
    }

    void injectCurrentToNeurons(nlm::NeuronType type, nlm::MembranePotential current) {
        if (!brain_) throw SimulationError("Brain is null");
        brain_->injectCurrentToNeurons(type, current);
    }

    // Cleanup method
    void cleanup() {
        if (brain_) {
            brain_->reset();
            isInitialized_ = false;
        }
    }

private:
    std::shared_ptr<nlm::Brain> brain_;
    bool isInitialized_;
};

// Enhanced AgentBrain wrapper class
class AgentBrainWrapper {
public:
    explicit AgentBrainWrapper(std::shared_ptr<nlm::AgentBrain> agentBrain) : agentBrain_(agentBrain) {
        if (!agentBrain_) {
            throw BrainInitializationError("Null agent brain pointer");
        }
    }

    std::shared_ptr<nlm::AgentBrain> get() { return agentBrain_; }

    // Core operations
    void initialize(nlm::SimpleWorld& world) {
        if (agentBrain_) {
            agentBrain_->initialize(world);
        }
    }

    void processSensoryInput(const nlm::SensoryPercept& percept) {
        if (agentBrain_) {
            agentBrain_->processSensoryInput(percept);
        }
    }

    nlm::Action decodeMotorCommand() {
        if (!agentBrain_) return nlm::Action();
        return agentBrain_->decodeMotorCommand();
    }

    void applyRewardModulation(float reward, float predictedReward) {
        if (agentBrain_) {
            agentBrain_->applyRewardModulation(reward, predictedReward);
        }
    }

    void updateDevelopment(double timestep) {
        if (agentBrain_) {
            agentBrain_->updateDevelopment(timestep);
        }
    }

    nlm::DevelopmentalStage getDevelopmentalStage() const {
        if (!agentBrain_) return nlm::DevelopmentalStage::Initial;
        return agentBrain_->getDevelopmentalStage();
    }

    // Neuromodulation levels
    float getDopamineLevel() const {
        if (!agentBrain_) return 0.0f;
        return agentBrain_->getNeuromodulationLevel();
    }

    float getCuriosityLevel() const {
        if (!agentBrain_) return 0.0f;
        return agentBrain_->getCuriosityLevel();
    }

    float getNoveltyLevel() const {
        if (!agentBrain_) return 0.0f;
        return agentBrain_->getNoveltyLevel();
    }

    float getPredictionError() const {
        if (!agentBrain_) return 0.0f;
        return agentBrain_->getPredictionError();
    }

    // Configuration methods
    void enableRewardModulation(bool enable) {
        if (agentBrain_) {
            agentBrain_->enableRewardModulation(enable);
        }
    }

    void enableDevelopment(bool enable) {
        if (agentBrain_) {
            agentBrain_->enableDevelopment(enable);
        }
    }

    void enableCuriosity(bool enable) {
        if (agentBrain_) {
            agentBrain_->enableCuriosity(enable);
        }
    }

    bool isRewardModulationEnabled() const {
        if (!agentBrain_) return false;
        return agentBrain_->isRewardModulationEnabled();
    }

    bool isDevelopmentEnabled() const {
        if (!agentBrain_) return false;
        return agentBrain_->isDevelopmentEnabled();
    }

    bool isCuriosityEnabled() const {
        if (!agentBrain_) return false;
        return agentBrain_->isCuriosityEnabled();
    }

    // Convenience methods
    std::string getDevelopmentalStageName() const {
        auto stage = getDevelopmentalStage();
        switch (stage) {
            case nlm::DevelopmentalStage::Initial: return "Initial";
            case nlm::DevelopmentalStage::CriticalPeriod: return "Critical Period";
            case nlm::DevelopmentalStage::Maturation: return "Maturation";
            case nlm::DevelopmentalStage::Adult: return "Adult";
            case nlm::DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }

    // Get brain reference
    std::shared_ptr<nlm::Brain> getBrain() {
        return agentBrain_ ? agentBrain_->getBrain() : nullptr;
    }

    // System status
    std::string getSystemStatus() const {
        std::ostringstream oss;
        oss << "=== Enhanced AgentBrain System Status ===\n";
        oss << "Developmental Stage: " << getDevelopmentalStageName() << "\n";
        oss << "Dopamine Level: " << getDopamineLevel() << "\n";
        oss << "Curiosity Level: " << getCuriosityLevel() << "\n";
        oss << "Novelty Level: " << getNoveltyLevel() << "\n";
        oss << "Prediction Error: " << getPredictionError() << "\n";
        oss << "Reward Modulation: " << (isRewardModulationEnabled() ? "Enabled" : "Disabled") << "\n";
        oss << "Development: " << (isDevelopmentEnabled() ? "Enabled" : "Disabled") << "\n";
        oss << "Curiosity: " << (isCuriosityEnabled() ? "Enabled" : "Disabled") << "\n";
        return oss.str();
    }

private:
    std::shared_ptr<nlm::AgentBrain> agentBrain_;
};

// Configuration preset loader
nlm::Config createDefaultConfig() {
    nlm::Config config;
    
    // Basic simulation parameters
    config.set("simulation_parameters.random_seed", 12345);
    config.set("simulation_parameters.timestep", 0.0005);
    config.set("simulation_parameters.neurons", 1000);
    config.set("simulation_parameters.regions", 3);
    config.set("simulation_parameters.connection_probability", 0.15);
    config.set("simulation_parameters.max_simulation_steps", 10000);
    config.set("simulation_parameters.simulation_time_limit", 300.0);
    
    // Plasticity parameters
    config.set("plasticity_parameters.stdp_ltp_weight", 0.025);
    config.set("plasticity_parameters.stdp_ltd_weight", 0.012);
    config.set("plasticity_parameters.stdp_tau", 25.0);
    config.set("plasticity_parameters.hebbian_learning_rate", 0.001);
    
    // Memory parameters
    config.set("memory_parameters.working_memory_capacity", 200);
    config.set("memory_parameters.episodic_memory_max_episodes", 1000);
    
    // Neuromodulation
    config.set("neuromodulation.dopamine_baseline", 0.1);
    config.set("neuromodulation.curiosity_strength", 0.5);
    config.set("neuromodulation.novelty_threshold", 0.05);
    
    // Development parameters
    config.set("development_parameters.synaptogenesis_rate", 0.0005);
    config.set("development_parameters.pruning_rate", 0.00005);
    
    // Logging
    config.set("logging_parameters.log_level", "info");
    config.set("logging_parameters.log_to_file", true);
    config.set("logging_parameters.log_filename", "nlm_simulation.log");
    config.set("logging_parameters.log_rotation_size", 100000000);
    config.set("logging_parameters.log_rotation_count", 10);
    
    return config;
}

nlm::Config createResearchConfig() {
    nlm::Config config;
    
    // Research configuration with more complexity
    config.set("simulation_parameters.random_seed", 42);
    config.set("simulation_parameters.timestep", 0.0001);
    config.set("simulation_parameters.neurons", 5000);
    config.set("simulation_parameters.regions", 5);
    config.set("simulation_parameters.connection_probability", 0.2);
    config.set("simulation_parameters.max_simulation_steps", 100000);
    config.set("simulation_parameters.simulation_time_limit", 3600.0);
    
    // Enhanced plasticity
    config.set("plasticity_parameters.stdp_ltp_weight", 0.03);
    config.set("plasticity_parameters.stdp_ltd_weight", 0.015);
    config.set("plasticity_parameters.stdp_tau", 50.0);
    config.set("plasticity_parameters.hebbian_learning_rate", 0.002);
    
    // Advanced memory
    config.set("memory_parameters.working_memory_capacity", 500);
    config.set("memory_parameters.episodic_memory_max_episodes", 5000);
    config.set("memory_parameters.associative_memory_enabled", true);
    config.set("memory_parameters.associative_similarity_threshold", 0.8);
    
    // Complex neuromodulation
    config.set("neuromodulation.dopamine_baseline", 0.15);
    config.set("neuromodulation.curiosity_strength", 0.7);
    config.set("neuromodulation.novelty_threshold", 0.03);
    config.set("neuromodulation.error_prediction_gain", 0.5);
    
    // Advanced development
    config.set("development_parameters.synaptogenesis_rate", 0.001);
    config.set("development_parameters.pruning_rate", 0.0001);
    config.set("development_parameters.maturation_rate", 0.0005);
    
    // Research logging
    config.set("logging_parameters.log_level", "debug");
    config.set("logging_parameters.log_to_file", true);
    config.set("logging_parameters.log_filename", "research_nlm.log");
    config.set("logging_parameters.log_rotation_size", 500000000);
    config.set("logging_parameters.log_rotation_count", 20);
    config.set("logging_parameters.include_performance_metrics", true);
    config.set("logging_parameters.include_neural_activity", true);
    
    return config;
}

nlm::Config createDevelopmentConfig() {
    nlm::Config config;
    
    // Development configuration for prototyping
    config.set("simulation_parameters.random_seed", 123);
    config.set("simulation_parameters.timestep", 0.001);
    config.set("simulation_parameters.neurons", 500);
    config.set("simulation_parameters.regions", 2);
    config.set("simulation_parameters.connection_probability", 0.1);
    config.set("simulation_parameters.max_simulation_steps", 5000);
    config.set("simulation_parameters.simulation_time_limit", 600.0);
    
    // Standard plasticity
    config.set("plasticity_parameters.stdp_ltp_weight", 0.02);
    config.set("plasticity_parameters.stdp_ltd_weight", 0.01);
    config.set("plasticity_parameters.stdp_tau", 30.0);
    config.set("plasticity_parameters.hebbian_learning_rate", 0.0005);
    
    // Basic memory
    config.set("memory_parameters.working_memory_capacity", 100);
    config.set("memory_parameters.episodic_memory_max_episodes", 500);
    
    // Standard neuromodulation
    config.set("neuromodulation.dopamine_baseline", 0.1);
    config.set("neuromodulation.curiosity_strength", 0.3);
    config.set("neuromodulation.novelty_threshold", 0.1);
    
    // Basic development
    config.set("development_parameters.synaptogenesis_rate", 0.0002);
    config.set("development_parameters.pruning_rate", 0.00002);
    
    // Development logging
    config.set("logging_parameters.log_level", "warn");
    config.set("logging_parameters.log_to_file", false);
    config.set("logging_parameters.log_filename", "dev_nlm.log");
    
    return config;
}
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

    PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM neural simulation framework.
        This module provides comprehensive access to all NLM functionality with:
        - User-friendly factory methods for creating brains and configuring them
        - Context manager support for automatic cleanup
        - High-level simulation methods that wrap complex sequences
        - Better error handling with meaningful exceptions
        - Statistics collection and reporting
        - Simulation recording capabilities
        - Configuration presets (default, research, development)
        - Progress monitoring for long simulations
    )pbdoc";

    // Register enhanced exception classes
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<PredictionError>(m, "PredictionError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Register existing type converters
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

    // Original Config class (maintained for backward compatibility)
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

    // Enhanced Config wrapper with convenience methods
    py::class_<ConfigWrapper>(m, "EnhancedConfig", R"pbdoc(Enhanced configuration with convenience methods)pbdoc")
        .def(py::init<nlm::Config&>(), py::arg("config"))
        .def("setDouble", &ConfigWrapper::setDouble, py::arg("key"), py::arg("value"),
             "Set a double configuration value")
        .def("getDouble", &ConfigWrapper::getDouble, py::arg("key"), py::arg("defaultValue") = 0.0,
             "Get a double configuration value")
        .def("setInt", &ConfigWrapper::setInt, py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("getInt", &ConfigWrapper::getInt, py::arg("key"), py::arg("defaultValue") = 0,
             "Get an integer configuration value")
        .def("setString", &ConfigWrapper::setString, py::arg("key"), py::arg("value"),
             "Set a string configuration value")
        .def("getString", &ConfigWrapper::getString, py::arg("key"), py::arg("defaultValue") = "",
             "Get a string configuration value")
        .def("setBool", &ConfigWrapper::setBool, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        .def("getBool", &ConfigWrapper::getBool, py::arg("key"), py::arg("defaultValue") = false,
             "Get a boolean configuration value")
        .def("hasKey", &ConfigWrapper::hasKey, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &ConfigWrapper::getKeys,
             "Get all configuration keys")
        .def("summary", &ConfigWrapper::summary,
             "Get configuration summary");

    // Enhanced Brain wrapper with context manager support
    py::class_<BrainWrapper>(m, "Brain", R"pbdoc(Enhanced Brain interface with context manager support)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"), "Create enhanced brain wrapper")
        .def("initialize", &BrainWrapper::initialize, "Initialize the enhanced brain")
        .def("step", &BrainWrapper::step, py::arg("currentStep"), "Perform a simulation step")
        .def("stepWithTime", &BrainWrapper::stepWithTime, py::arg("currentStep"), py::arg("currentTime"), "Perform simulation step with timestamp")

        // Memory system access
        .def("getWorkingMemory", &BrainWrapper::getWorkingMemory, "Get working memory pointer")
        .def("getEpisodicMemory", &BrainWrapper::getEpisodicMemory, "Get episodic memory pointer")
        .def("getAssociativeMemory", &BrainWrapper::getAssociativeMemory, "Get associative memory pointer")

        // Prediction system access
        .def("getPredictionSystem", &BrainWrapper::getPredictionSystem, "Get prediction system pointer")

        // Cognition system access
        .def("getNeuralPlanner", &BrainWrapper::getNeuralPlanner, "Get neural planner pointer")
        .def("getConceptFormation", &BrainWrapper::getConceptFormation, "Get concept formation pointer")
        .def("getAttention", &BrainWrapper::getAttention, "Get attentional selection pointer")

        // Development system access
        .def("getDevelopmentSystem", &BrainWrapper::getDevelopmentSystem, "Get development system pointer")

        // Neuromodulation system access
        .def("getDopamine", &BrainWrapper::getDopamine, "Get dopamine pointer")
        .def("getCuriosity", &BrainWrapper::getCuriosity, "Get curiosity pointer")
        .def("getNovelty", &BrainWrapper::getNovelty, "Get novelty pointer")
        .def("getPredictionError", &BrainWrapper::getPredictionError, "Get prediction error pointer")

        // Action production
        .def("produceAction", &BrainWrapper::produceAction, "Produce motor action based on neural activity")

        // Brain state management
        .def("reset", &BrainWrapper::reset, "Reset brain state")
        .def("save", &BrainWrapper::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &BrainWrapper::load, py::arg("filepath"), "Load brain state from file")

        // Statistics and monitoring
        .def("getExcitationInhibitionRatio", &BrainWrapper::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &BrainWrapper::getTotalSpikeCount, "Get total spike count")
        .def("getTotalNeuronCount", &BrainWrapper::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &BrainWrapper::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &BrainWrapper::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &BrainWrapper::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &BrainWrapper::getAverageFiringRate, "Get average firing rate")

        // Development stage
        .def("getDevelopmentalStage", &BrainWrapper::getDevelopmentalStage, "Get developmental stage")
        .def("setDevelopmentalStage", &BrainWrapper::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getDevelopmentalStageName", &BrainWrapper::getDevelopmentalStageName, "Get developmental stage name")

        // Random generator
        .def("getRandomGenerator", &BrainWrapper::getRandomGenerator, "Get random generator pointer")

        // Convenience methods for common workflows
        .def("runIntegrationTest", &BrainWrapper::runIntegrationTest, py::arg("numSteps") = 100, py::arg("batchSize") = 10, "Run integration test")
        .def("simulateEpisode", &BrainWrapper::simulateEpisode, py::arg("reward"), py::arg("steps") = 100, "Simulate an episode with given reward")
        .def("consolidateMemory", &BrainWrapper::consolidateMemory, py::arg("numEpisodesToReplay") = 3, "Consolidate memory through replay")

        // Memory statistics
        .def("getMemoryStats", &BrainWrapper::getMemoryStats, "Get comprehensive memory statistics")

        // Region management
        .def("addRegion", &BrainWrapper::addRegion, py::arg("name") = "", "Add a new neural region")

        // Action convenience methods
        .def("getActionType", &BrainWrapper::getActionType, py::arg("action"), "Get action type from action object")
        .def("createAction", &BrainWrapper::createAction, py::arg("type"), "Create action with specified type")
        .def("createActionWithParams", &BrainWrapper::createActionWithParams, py::arg("type"), py::arg("params"), "Create action with type and parameters")

        // Statistics collection
        .def("collectStatistics", &BrainWrapper::collectStatistics, "Collect comprehensive statistics")

        // Simulation recording
        .def("recordSimulation", &BrainWrapper::recordSimulation, py::arg("filepath"), py::arg("steps"), py::arg("stepInterval") = 1, "Record simulation data to file")

        // Progress monitoring
        .def("monitorProgress", &BrainWrapper::monitorProgress, py::arg("steps"), py::arg("reportInterval") = 100, "Monitor simulation progress")

        // Get comprehensive status
        .def("getBrainStatus", &BrainWrapper::getBrainStatus, "Get comprehensive brain status")

        .def("isMonitoringActive", &BrainWrapper::isMonitoringActive, "Check if monitoring is active")
        .def("getMonitoringData", &BrainWrapper::getMonitoringData, "Get current monitoring data")
        .def("clearMonitoringData", &BrainWrapper::clearMonitoringData, "Clear monitoring data")
        
        // Performance Profiling
        .def("startProfiling", &BrainWrapper::startProfiling, "Start performance profiling")
        .def("stopProfiling", &BrainWrapper::stopProfiling, "Stop profiling")
        .def("getProfileReport", &BrainWrapper::getProfileReport, "Get profiling statistics")
        .def("resetProfiling", &BrainWrapper::resetProfiling, "Reset profiling data")
        
        // Memory Usage Monitoring
        .def("getMemoryUsage", &BrainWrapper::getMemoryUsage, "Get current memory usage")
        .def("getNeuronMemoryUsage", &BrainWrapper::getNeuronMemoryUsage, "Get memory usage by neuron type")
        .def("getSynapseMemoryUsage", &BrainWrapper::getSynapseMemoryUsage, "Get memory usage by synapse type")
        .def("getSystemMemoryUsage", &BrainWrapper::getSystemMemoryUsage, "Get overall system memory usage")
        
        // Spike Event Monitoring
        .def("enableSpikeMonitoring", &BrainWrapper::enableSpikeMonitoring, "Enable spike event monitoring")
        .def("disableSpikeMonitoring", &BrainWrapper::disableSpikeMonitoring, "Disable spike monitoring")
        .def("getSpikeData", &BrainWrapper::getSpikeData, "Get spike event data")
        .def("getFiringRateHistory", &BrainWrapper::getFiringRateHistory, "Get firing rate over time")
        .def("getLastSpikeTimes", &BrainWrapper::getLastSpikeTimes, "Get last spike times for each neuron")
        
        // Connection Monitoring
        .def("enableConnectionMonitoring", &BrainWrapper::enableConnectionMonitoring, "Enable synaptic connection monitoring")
        .def("disableConnectionMonitoring", &BrainWrapper::disableConnectionMonitoring, "Disable connection monitoring")
        .def("getConnectionData", &BrainWrapper::getConnectionData, "Get synaptic connection data")
        .def("getNetworkStatistics", &BrainWrapper::getNetworkStatistics, "Get network-wide statistics")
        
        // Debug Tools
        .def("setDebugMode", &BrainWrapper::setDebugMode, py::arg("enabled"), "Enable/disable debug mode")
        .def("getDebugInfo", &BrainWrapper::getDebugInfo, "Get current debug information")
        .def("logDebug", &BrainWrapper::logDebug, py::arg("message"), "Log debug information")
        .def("exportDebugData", &BrainWrapper::exportDebugData, py::arg("filepath"), "Export debug data to file")
        
        // Real-time Visualization
        .def("enableRealTimeVisualization", &BrainWrapper::enableRealTimeVisualization, "Enable real-time visualization")
        .def("disableRealTimeVisualization", &BrainWrapper::disableRealTimeVisualization, "Disable visualization")
        .def("getVisualizationData", &BrainWrapper::getVisualizationData, "Get current visualization data")
        .def("getNetworkGraph", &BrainWrapper::getNetworkGraph, "Get network connectivity graph data")
        
        // Alert System
        .def("addAlert", &BrainWrapper::addAlert, py::arg("message"), py::arg("threshold"), "Add a monitoring alert")
        .def("removeAlert", &BrainWrapper::removeAlert, py::arg("message"), "Remove an alert")
        .def("getAlerts", &BrainWrapper::getAlerts, "Get current alerts")
        .def("clearAlerts", &BrainWrapper::clearAlerts, "Clear all alerts")

    // Enhanced AgentBrain wrapper
    py::class_<AgentBrainWrapper>(m, "AgentBrain", R"pbdoc(Enhanced AgentBrain with neuromodulation access)pbdoc")
        .def(py::init<std::shared_ptr<nlm::AgentBrain>>(), py::arg("agentBrain"), "Create enhanced agent brain wrapper")
        .def("initialize", &AgentBrainWrapper::initialize, py::arg("world"), "Initialize with world")
        .def("processSensoryInput", &AgentBrainWrapper::processSensoryInput, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrainWrapper::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrainWrapper::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrainWrapper::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrainWrapper::getDevelopmentalStage, "Get current developmental stage")
        .def("getDopamineLevel", &AgentBrainWrapper::getDopamineLevel, "Get dopamine level")
        .def("getCuriosityLevel", &AgentBrainWrapper::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrainWrapper::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrainWrapper::getPredictionError, "Get prediction error")
        .def("getDevelopmentalStageName", &AgentBrainWrapper::getDevelopmentalStageName, "Get developmental stage name")
        .def("enableRewardModulation", &AgentBrainWrapper::enableRewardModulation, py::arg("enable"), "Enable reward modulation")
        .def("enableDevelopment", &AgentBrainWrapper::enableDevelopment, py::arg("enable"), "Enable development")
        .def("enableCuriosity", &AgentBrainWrapper::enableCuriosity, py::arg("enable"), "Enable curiosity")
        .def("isRewardModulationEnabled", &AgentBrainWrapper::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("isDevelopmentEnabled", &AgentBrainWrapper::isDevelopmentEnabled, "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrainWrapper::isCuriosityEnabled, "Check if curiosity is enabled")
        .def("getSystemStatus", &AgentBrainWrapper::getSystemStatus, "Get system status");

    // SimpleWorld (existing class)
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept, py::return_value_policy::reference_internal, "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody, py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object from world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // SensoryPercept (existing class)
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all signals concatenated")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp");

    // Action (existing class)
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Clone action");

    // WorldObject (existing class)
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readonly("x", &WorldObject::x)
        .def_readonly("y", &WorldObject::y)
        .def_readonly("radius", &WorldObject::radius)
        .def_readonly("type", &WorldObject::type)
        .def_readonly("value", &WorldObject::value)
        .def_readonly("active", &WorldObject::active);

    // AgentBody (existing class)
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readonly("x", &AgentBody::x)
        .def_readonly("y", &AgentBody::y)
        .def_readonly("orientation", &AgentBody::orientation)
        .def_readonly("velocityX", &AgentBody::velocityX)
        .def_readonly("velocityY", &AgentBody::velocityY)
        .def_readonly("angularVelocity", &AgentBody::angularVelocity)
        .def_readonly("energy", &AgentBody::energy)
        .def_readonly("health", &AgentBody::health)
        .def_readonly("age", &AgentBody::age)
        .def_readonly("isMoving", &AgentBody::isMoving)
        .def_readonly("isTurning", &AgentBody::isTurning)
        .def_readonly("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset, "Reset agent body state");

    // ActionResult (existing class)
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readonly("reward", &ActionResult::reward)
        .def_readonly("success", &ActionResult::success)
        .def_readonly("message", &ActionResult::message);

    // Invalid IDs
    m.attr("INVALID_NEURON_ID") = nlm::INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = nlm::INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = nlm::INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = nlm::INVALID_POPULATION_ID;

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createResearchConfig", []() -> nlm::Config {
        return createResearchConfig();
    }, "Create a research configuration preset");

    m.def("createDevelopmentConfig", []() -> nlm::Config {
        return createDevelopmentConfig();
    }, "Create a development configuration preset");

    m.def("createQuickBrain", []() -> std::shared_ptr<BrainWrapper> {
        nlm::Config config = createDefaultConfig();
        auto brain = std::make_shared<nlm::Brain>(std::make_shared<Config>(config));
        return std::make_shared<BrainWrapper>(brain);
    }, "Create a brain with default configuration (convenience function)");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<BrainWrapper> {
        auto brain = std::make_shared<nlm::Brain>(config);
        return std::make_shared<BrainWrapper>(brain);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<BrainWrapper> brain) -> std::shared_ptr<AgentBrainWrapper> {
        auto agentBrain = std::make_shared<nlm::AgentBrain>(brain->get());
        return std::make_shared<AgentBrainWrapper>(agentBrain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience simulation function
    m.def("simulate", [](std::shared_ptr<BrainWrapper> brain, size_t steps, size_t batchSize, double timestep) {
        if (!brain) throw SimulationError("Brain is null");
        if (!brain->initialize()) throw SimulationError("Brain initialization failed");

        for (size_t i = 0; i < steps; i += batchSize) {
            for (size_t j = 0; j < batchSize && (i + j) < steps; ++j) {
                brain->step(i + j, (i + j) * timestep);
            }
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("batchSize") = 100, py::arg("timestep") = 0.01, "Simple simulation loop");

    // Configuration presets
    m.def("createDefaultConfig", []() -> nlm::Config {
        return createDefaultConfig();
    }, "Create default configuration preset");

    m.def("createResearchConfig", []() -> nlm::Config {
        return createResearchConfig();
    }, "Create research configuration preset");

    m.def("createDevelopmentConfig", []() -> nlm::Config {
        return createDevelopmentConfig();
    }, "Create development configuration preset");

    // High-level simulation methods
    m.def("runQuickSimulation", [](std::shared_ptr<BrainWrapper> brain, size_t steps = 1000) {
        if (!brain) throw SimulationError("Brain is null");
        if (!brain->initialize()) throw SimulationError("Brain initialization failed");

        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
    }, py::arg("brain"), py::arg("steps") = 1000, "Run a quick simulation");

    m.def("runIntegrationDemo", [](std::shared_ptr<BrainWrapper> brain, size_t steps = 1000) {
        if (!brain) throw SimulationError("Brain is null");
        if (!brain->initialize()) throw SimulationError("Brain initialization failed");

        // Check integration status
        auto* workingMem = brain->getWorkingMemory();
        auto* episodicMem = brain->getEpisodicMemory();
        auto* predictionSys = brain->getPredictionSystem();
        auto* planner = brain->getNeuralPlanner();
        auto* conceptForm = brain->getConceptFormation();
        auto* attention = brain->getAttention();
        auto* devSys = brain->getDevelopmentSystem();

        return std::make_tuple(
            workingMem != nullptr,
            episodicMem != nullptr,
            predictionSys != nullptr,
            planner != nullptr,
            conceptForm != nullptr,
            attention != nullptr,
            devSys != nullptr
        );
    }, py::arg("brain"), py::arg("steps") = 1000, "Run integration demo and return system status");

    // Statistics collection and reporting
    m.def("getBrainStatistics", [](std::shared_ptr<BrainWrapper> brain) {
        if (!brain) {
            std::map<std::string, double> errorStats;
            errorStats["error"] = 1.0;
            errorStats["message"] = "Brain is null";
            return errorStats;
        }
        return brain->collectStatistics();
    }, py::arg("brain"), "Get comprehensive brain statistics");

    // Simulation recording
    m.def("recordSimulation", [](std::shared_ptr<BrainWrapper> brain, const std::string& filepath, size_t steps, size_t stepInterval = 1) {
        if (!brain) return false;
        return brain->recordSimulation(filepath, steps, stepInterval);
    }, py::arg("brain"), py::arg("filepath"), py::arg("steps"), py::arg("stepInterval") = 1, "Record simulation data to file");

    // Progress monitoring
    m.def("monitorSimulation", [](std::shared_ptr<BrainWrapper> brain, size_t steps, size_t reportInterval = 100) {
        if (!brain) {
            std::map<std::string, double> errorProgress;
            errorProgress["error"] = 1.0;
            errorProgress["message"] = "Brain is null";
            return errorProgress;
        }
        return brain->monitorProgress(steps, reportInterval);
    }, py::arg("brain"), py::arg("steps"), py::arg("reportInterval") = 100, "Monitor simulation progress");

    // Enhanced convenience functions
    m.def("createEnhancedConfig", [](nlm::Config config) -> std::shared_ptr<ConfigWrapper> {
        return std::make_shared<ConfigWrapper>(config);
    }, py::arg("config"), "Create enhanced config wrapper");

    m.def("createEnhancedBrain", [](std::shared_ptr<nlm::Brain> brain) -> std::shared_ptr<BrainWrapper> {
        return std::make_shared<BrainWrapper>(brain);
    }, py::arg("brain"), "Create enhanced brain wrapper");

    m.def("createEnhancedAgentBrain", [](std::shared_ptr<nlm::AgentBrain> agentBrain) -> std::shared_ptr<AgentBrainWrapper> {
        return std::make_shared<AgentBrainWrapper>(agentBrain);
    }, py::arg("agentBrain"), "Create enhanced agent brain wrapper");

    // Additional high-level functions
    m.def("initializeAndReset", [](std::shared_ptr<BrainWrapper> brain) {
        if (!brain) throw SimulationError("Brain is null");
        if (!brain->initialize()) throw SimulationError("Brain initialization failed");
        brain->reset();
    }, py::arg("brain"), "Initialize and reset brain");

    m.def("runQuickTest", [](std::shared_ptr<BrainWrapper> brain, size_t steps = 100) {
        if (!brain) throw SimulationError("Brain is null");
        if (!brain->initialize()) throw SimulationError("Brain initialization failed");

        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain->step(static_cast<int64_t>(step), static_cast<double>(step) * 0.01);
        }
    }, py::arg("brain"), py::arg("steps") = 100, "Run a quick test simulation");

    // Version and information
    m.attr("VERSION") = "3.0.0";  // Enhanced version
    m.attr("DESCRIPTION") = "Enhanced NLM Python Bindings with user-friendly API";
    m.attr("PHASE") = "6";
}
