#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>
#include "ErrorSystem.hpp"
#include "Validation/NeuralInputValidator.hpp"

namespace nlm {

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct Metrics {
        double cpuTime = 0.0;
        double wallTime = 0.0;
        size_t memoryUsage = 0;
        size_t memoryPeak = 0;
        size_t memoryAllocated = 0;
        std::vector<double> timestepTimes;
        std::vector<size_t> spikeCounts;
        std::vector<size_t> neuronCounts;
        std::vector<double> firingRates;
        std::vector<double> synapticWeights;
        
        // Anomaly detection
        std::vector<double> performanceAnomalies;
        std::vector<std::string> anomalyTypes;
    };
    
    struct Alert {
        std::string message;
        std::string severity; // "INFO", "WARNING", "ERROR", "CRITICAL"
        std::chrono::system_clock::time_point timestamp;
        std::string source;
        std::string metadata;
    };
    
    using AlertCallback = std::function<void(const Alert&)>
    
private:
    static inline std::unique_ptr<Metrics> currentMetrics;
    static inline std::chrono::high_resolution_clock::time_point startTime;
    static inline std::vector<Alert> alerts;
    static inline std::mutex alertsMutex;
    static inline AlertCallback alertCallback;
    static inline double baselineCpuUsage = 0.0;
    static inline double baselineMemoryUsage = 0.0;
    static inline double performanceThreshold = 0.8; // 80% of baseline
    static inline double memoryThreshold = 0.9; // 90% of baseline
    
public:
    static void startTiming() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    static double stopTiming() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000000.0; // Convert to seconds
    }
    
    static void recordStepMetrics(size_t spikeCount, size_t neuronCount, double timestep) {
        if (!currentMetrics) return;
        
        currentMetrics->timestepTimes.push_back(timestep);
        currentMetrics->spikeCounts.push_back(spikeCount);
        currentMetrics->neuronCounts.push_back(neuronCount);
        
        // Calculate firing rate
        double firingRate = static_cast<double>(spikeCount) / neuronCount;
        currentMetrics->firingRates.push_back(firingRate);
        
        // Check for performance anomalies
        detectAnomalies();
    }
    
    static void recordMemoryUsage(size_t memoryBytes) {
        if (!currentMetrics) return;
        
        currentMetrics->memoryUsage = memoryBytes;
        if (memoryBytes > currentMetrics->memoryPeak) {
            currentMetrics->memoryPeak = memoryBytes;
        }
        
        // Check for memory issues
        if (memoryBytes > baselineMemoryUsage * memoryThreshold) {
            triggerAlert("High memory usage", "ERROR",
                        "Memory usage: " + std::to_string(memoryBytes) + 
                        " bytes (threshold: " + std::to_string(baselineMemoryUsage * memoryThreshold) + " bytes)");
        }
    }
    
    static void recordWeight(double weight) {
        if (!currentMetrics) return;
        currentMetrics->synapticWeights.push_back(weight);
    }
    
    static Metrics getMetrics() {
        return currentMetrics ? *currentMetrics : Metrics();
    }
    
    static void reset() {
        currentMetrics = std::make_unique<Metrics>();
    }
    
    static void initialize(double baselineCpu = 0.0, double baselineMemory = 0.0) {
        currentMetrics = std::make_unique<Metrics>();
        startTiming();
        
        baselineCpuUsage = baselineCpu;
        baselineMemoryUsage = baselineMemory;
        
        // Initialize baseline measurements
        updateBaseline();
    }
    
    static std::vector<Alert> getAlerts() {
        std::lock_guard<std::mutex> lock(alertsMutex);
        return alerts;
    }
    
    static void clearAlerts() {
        std::lock_guard<std::mutex> lock(alertsMutex);
        alerts.clear();
    }
    
    static std::string getSummary() const {
        if (!currentMetrics) return "Performance monitor not initialized";
        
        std::ostringstream oss;
        oss << "=== Performance Metrics ===\n";
        oss << "CPU Time: " << currentMetrics->cpuTime << "s\n";
        oss << "Wall Time: " << currentMetrics->wallTime << "s\n";
        oss << "Memory Usage: " << currentMetrics->memoryUsage << " bytes\n";
        oss << "Memory Peak: " << currentMetrics->memoryPeak << " bytes\n";
        oss << "\nRecent Metrics:\n";
        
        if (!currentMetrics->timestepTimes.empty()) {
            double avgTimestep = std::accumulate(currentMetrics->timestepTimes.begin(),
                                                currentMetrics->timestepTimes.end(), 0.0) /
                                                currentMetrics->timestepTimes.size();
            oss << "Average timestep: " << avgTimestep << "s\n";
        }
        
        if (!currentMetrics->spikeCounts.empty()) {
            double avgSpikes = std::accumulate(currentMetrics->spikeCounts.begin(),
                                              currentMetrics->spikeCounts.end(), 0.0) /
                                              currentMetrics->spikeCounts.size();
            oss << "Average spikes per timestep: " << avgSpikes << "\n";
        }
        
        if (!currentMetrics->neuronCounts.empty()) {
            double avgNeurons = std::accumulate(currentMetrics->neuronCounts.begin(),
                                              currentMetrics->neuronCounts.end(), 0.0) /
                                              currentMetrics->neuronCounts.size();
            oss << "Average neuron count: " << static_cast<size_t>(avgNeurons) << "\n";
        }
        
        if (!currentMetrics->performanceAnomalies.empty()) {
            oss << "\nPerformance Anomalies Detected: " << currentMetrics->performanceAnomalies.size() << "\n";
            for (size_t i = 0; i < currentMetrics->performanceAnomalies.size(); ++i) {
                oss << "  " << (i + 1) << ". " << currentMetrics->anomalyTypes[i] << 
                    " (deviation: " << currentMetrics->performanceAnomalies[i] << ")\n";
            }
        }
        
        return oss.str();
    }
    
    static void setAlertCallback(AlertCallback callback) {
        alertCallback = callback;
    }
    
    static void triggerAlert(const std::string& message, const std::string& severity,
                             const std::string& metadata = "") {
        Alert alert;
        alert.message = message;
        alert.severity = severity;
        alert.timestamp = std::chrono::system_clock::now();
        alert.source = "performance_monitor";
        alert.metadata = metadata;
        
        std::lock_guard<std::mutex> lock(alertsMutex);
        alerts.push_back(alert);
        
        if (alertCallback) {
            alertCallback(alert);
        }
        
        // Log the alert
        if (severity == "WARNING") {
            NLM_LOG_WARNING(message + " (" + metadata + ")");
        } else if (severity == "ERROR") {
            NLM_LOG_ERROR(message + " (" + metadata + ")");
        } else if (severity == "CRITICAL") {
            NLM_LOG_CRITICAL(message + " (" + metadata + ")");
        }
    }
    
    static void detectAnomalies() {
        if (!currentMetrics) return;
        
        // Detect firing rate anomalies
        if (currentMetrics->firingRates.size() >= 5) {
            size_t windowSize = std::min<size_t>(5, currentMetrics->firingRates.size());
            double sum = std::accumulate(currentMetrics->firingRates.end() - windowSize,
                                        currentMetrics->firingRates.end(), 0.0);
            double avgFiringRate = sum / windowSize;
            
            double maxDev = 0.0;
            for (size_t i = currentMetrics->firingRates.size() - windowSize;
                 i < currentMetrics->firingRates.size(); ++i) {
                double dev = std::abs(currentMetrics->firingRates[i] - avgFiringRate);
                if (dev > maxDev) maxDev = dev;
            }
            
            if (maxDev > 2.0 * avgFiringRate && avgFiringRate > 0.0) {
                currentMetrics->performanceAnomalies.push_back(maxDev);
                currentMetrics->anomalyTypes.push_back("Firing rate anomaly");
                triggerAlert("Unusual firing rate detected", "WARNING",
                            "Average firing rate: " + std::to_string(avgFiringRate) + ", max deviation: " + std::to_string(maxDev));
            }
        }
        
        // Detect synaptic weight anomalies
        if (currentMetrics->synapticWeights.size() >= 10) {
            size_t windowSize = std::min<size_t>(10, currentMetrics->synapticWeights.size());
            double sum = std::accumulate(currentMetrics->synapticWeights.end() - windowSize,
                                        currentMetrics->synapticWeights.end(), 0.0);
            double avgWeight = sum / windowSize;
            
            double maxWeightDev = 0.0;
            for (size_t i = currentMetrics->synapticWeights.size() - windowSize;
                 i < currentMetrics->synapticWeights.size(); ++i) {
                double dev = std::abs(currentMetrics->synapticWeights[i] - avgWeight);
                if (dev > maxWeightDev) maxWeightDev = dev;
            }
            
            if (maxWeightDev > avgWeight * 2.0) {
                currentMetrics->performanceAnomalies.push_back(maxWeightDev);
                currentMetrics->anomalyTypes.push_back("Synaptic weight anomaly");
                triggerAlert("Unusual synaptic weight detected", "WARNING",
                            "Average weight: " + std::to_string(avgWeight) + ", max deviation: " + std::to_string(maxWeightDev));
            }
        }
    }
    
private:
    static void updateBaseline() {
        // This would be called periodically to update baseline performance
        // In a real implementation, you might use exponential moving averages
    }
};

// Neural connection validation
class NeuralConnectionValidator {
public:
    struct ConnectionValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::pair<NeuronId, NeuronId>> validConnections;
        std::unordered_map<NeuronId, size_t> inputCounts;
        std::unordered_map<NeuronId, size_t> outputCounts;
        
        ValidationResult toValidationResult() const {
            ValidationResult result;
            result.valid = valid;
            result.errors = errors;
            return result;
        }
    };
    
    // Validate neural connectivity
    static ConnectionValidationResult validateConnections(
        const std::vector<std::pair<NeuronId, NeuronId>>& connections,
        const std::unordered_map<NeuronId, bool>& neuronExists,
        size_t maxFanIn = 1000,
        size_t maxFanOut = 1000) {
        
        ConnectionValidationResult result;
        result.valid = true;
        
        // Check for self-connections
        for (const auto& conn : connections) {
            if (conn.first == conn.second) {
                result.errors.push_back("Self-connection detected: neuron " + std::to_string(conn.first));
                result.valid = false;
            }
        }
        
        // Validate neuron existence
        for (const auto& conn : connections) {
            auto [source, dest] = conn;
            
            if (!neuronExists.count(source)) {
                result.errors.push_back("Connection references non-existent source neuron: " + std::to_string(source));
                result.valid = false;
            }
            
            if (!neuronExists.count(dest)) {
                result.errors.push_back("Connection references non-existent destination neuron: " + std::to_string(dest));
                result.valid = false;
            }
        }
        
        if (!result.valid) {
            return result;
        }
        
        // Check fan-in and fan-out limits
        for (const auto& conn : connections) {
            result.inputCounts[conn.second]++;
            result.outputCounts[conn.first]++;
        }
        
        for (const auto& pair : result.inputCounts) {
            if (pair.second > maxFanIn) {
                result.errors.push_back("Neuron " + std::to_string(pair.first) + 
                                      " has too many inputs: " + std::to_string(pair.second) +
                                      " (limit: " + std::to_string(maxFanIn) + ")");
                result.valid = false;
            }
        }
        
        for (const auto& pair : result.outputCounts) {
            if (pair.second > maxFanOut) {
                result.errors.push_back("Neuron " + std::to_string(pair.first) + 
                                      " has too many outputs: " + std::to_string(pair.second) +
                                      " (limit: " + std::to_string(maxFanOut) + ")");
                result.valid = false;
            }
        }
        
        if (result.valid) {
            result.validConnections = connections;
        }
        
        return result;
    }
};

// Development stage validation
class DevelopmentValidator {
public:
    enum class DevelopmentStage {
        Initial,
        CriticalPeriod,
        Maturation,
        Adult,
        Unknown
    };
    
    struct DevelopmentalParameters {
        DevelopmentStage stage;
        double plasticityRate;
        double synaptogenesisRate;
        double pruningRate;
        double learningRate;
        double memoryConsolidationRate;
        
        ValidationResult validate() const {
            ValidationResult result;
            
            if (plasticityRate < 0.0 || plasticityRate > 1.0) {
                result.addError("Plasticity rate must be between 0.0 and 1.0: " + std::to_string(plasticityRate));
            }
            
            if (synaptogenesisRate < 0.0 || synaptogenesisRate > 0.01) {
                result.addError("Synaptogenesis rate must be between 0.0 and 0.01: " + std::to_string(synaptogenesisRate));
            }
            
            if (pruningRate < 0.0 || pruningRate > 0.001) {
                result.addError("Pruning rate must be between 0.0 and 0.001: " + std::to_string(pruningRate));
            }
            
            if (learningRate < 0.0 || learningRate > 1.0) {
                result.addError("Learning rate must be between 0.0 and 1.0: " + std::to_string(learningRate));
            }
            
            if (memoryConsolidationRate < 0.0 || memoryConsolidationRate > 1.0) {
                result.addError("Memory consolidation rate must be between 0.0 and 1.0: " + std::to_string(memoryConsolidationRate));
            }
            
            // Stage-specific validation
            if (stage == DevelopmentStage::CriticalPeriod) {
                if (plasticityRate < 0.5) {
                    result.addWarning("Critical period should have high plasticity rate: " + std::to_string(plasticityRate));
                }
            } else if (stage == DevelopmentStage::Adult) {
                if (plasticityRate > 0.3) {
                    result.addWarning("Adult stage should have lower plasticity rate: " + std::to_string(plasticityRate));
                }
            }
            
            return result;
        }
    };
    
    // Get developmental stage from configuration
    static DevelopmentStage getDevelopmentStage(const Config& config) {
        auto stageStr = config.get<std::string>("developmental_stage");
        if (!stageStr) {
            return DevelopmentStage::Unknown;
        }
        
        std::string stage = *stageStr;
        if (stage == "critical_period" || stage == "critical") {
            return DevelopmentStage::CriticalPeriod;
        } else if (stage == "maturation") {
            return DevelopmentStage::Maturation;
        } else if (stage == "adult") {
            return DevelopmentStage::Adult;
        } else if (stage == "initial" || stage == "early") {
            return DevelopmentStage::Initial;
        }
        
        return DevelopmentStage::Unknown;
    }
    
    // Get default developmental parameters
    static DevelopmentalParameters getDefaultParameters(DevelopmentStage stage) {
        DevelopmentalParameters params;
        params.stage = stage;
        
        switch (stage) {
            case DevelopmentStage::Initial:
                params.plasticityRate = 0.8;
                params.synaptogenesisRate = 0.001;
                params.pruningRate = 0.00001;
                params.learningRate = 0.05;
                params.memoryConsolidationRate = 0.1;
                break;
            case DevelopmentStage::CriticalPeriod:
                params.plasticityRate = 0.9;
                params.synaptogenesisRate = 0.005;
                params.pruningRate = 0.00002;
                params.learningRate = 0.1;
                params.memoryConsolidationRate = 0.15;
                break;
            case DevelopmentStage::Maturation:
                params.plasticityRate = 0.5;
                params.synaptogenesisRate = 0.0005;
                params.pruningRate = 0.00005;
                params.learningRate = 0.03;
                params.memoryConsolidationRate = 0.08;
                break;
            case DevelopmentStage::Adult:
                params.plasticityRate = 0.2;
                params.synaptogenesisRate = 0.0001;
                params.pruningRate = 0.00001;
                params.learningRate = 0.01;
                params.memoryConsolidationRate = 0.05;
                break;
            default:
                params.plasticityRate = 0.5;
                params.synaptogenesisRate = 0.0001;
                params.pruningRate = 0.00001;
                params.learningRate = 0.05;
                params.memoryConsolidationRate = 0.1;
        }
        
        return params;
    }
};

} // namespace nlm