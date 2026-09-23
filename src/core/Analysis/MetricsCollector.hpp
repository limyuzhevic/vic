#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

class MetricsCollector {
public:
    struct BrainMetrics {
        std::string simulationPhase;
        SimulationStep step;
        Timestamp timestamp;
        
        // Neural activity metrics
        size_t neuronCount;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        size_t totalSpikeCount;
        float averageFiringRate;
        float excitationInhibitionRatio;
        
        // Synaptic metrics
        size_t synapseCount;
        float averageSynapseWeight;
        size_t pendingSpikeCount;
        
        // Memory system metrics
        size_t workingMemoryTraces;
        size_t episodicMemoryEpisodes;
        
        // Neuromodulation metrics
        float dopamineLevel;
        float curiosityLevel;
        
        // Performance metrics
        double simulationTime;
        size_t memoryUsageMB;
        
        // Plasticity metrics
        size_t plasticityUpdates;
        float totalWeightChange;
        
        BrainMetrics() : step(0), timestamp(0.0), neuronCount(0), activeNeuronCount(0),
                        firingNeuronCount(0), totalSpikeCount(0), averageFiringRate(0.0),
                        excitationInhibitionRatio(0.0), synapseCount(0), averageSynapseWeight(0.0),
                        pendingSpikeCount(0), workingMemoryTraces(0), episodicMemoryEpisodes(0),
                        dopamineLevel(0.0), curiosityLevel(0.0), simulationTime(0.0),
                        memoryUsageMB(0), plasticityUpdates(0), totalWeightChange(0.0) {}
    };
    
    MetricsCollector() = default;
    
    // Add metrics for current simulation state
    void addMetrics(const BrainMetrics& metrics) {
        collectedMetrics.push_back(metrics);
        
        // Update rolling averages for performance tracking
        updateRollingMetrics(metrics);
    }
    
    // Get all collected metrics
    const std::vector<BrainMetrics>& getMetrics() const {
        return collectedMetrics;
    }
    
    // Get metrics for specific simulation phase
    std::vector<BrainMetrics> getMetricsForPhase(const std::string& phase) const {
        std::vector<BrainMetrics> phaseMetrics;
        for (const auto& metrics : collectedMetrics) {
            if (metrics.simulationPhase == phase) {
                phaseMetrics.push_back(metrics);
            }
        }
        return phaseMetrics;
    }
    
    // Get latest metrics
    BrainMetrics getLatestMetrics() const {
        if (collectedMetrics.empty()) return BrainMetrics{};
        return collectedMetrics.back();
    }
    
    // Generate comprehensive metrics report
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== NLM Brain Performance Metrics ===" << std::endl;
        oss << "Total simulation steps: " << collectedMetrics.size() << std::endl;
        oss << "Simulation duration: " << getSimulationDuration() << " steps" << std::endl;
        oss << std::endl;
        
        // Overall statistics
        oss << "=== Overall Statistics ===" << std::endl;
        oss << "Peak neurons: " << getMaxNeurons() << std::endl;
        oss << "Total spikes: " << getTotalSpikes() << std::endl;
        oss << "Final firing rate: " << getLatestMetrics().averageFiringRate << " Hz" << std::endl;
        oss << "Final E/I ratio: " << getLatestMetrics().excitationInhibitionRatio << std::endl;
        oss << std::endl;
        
        // Neural activity trends
        oss << "=== Neural Activity Trends ===" << std::endl;
        oss << "Average firing rate: " << calculateAverage("averageFiringRate") << " Hz" << std::endl;
        oss << "Average E/I ratio: " << calculateAverage("excitationInhibitionRatio") << std::endl;
        oss << "Average active neurons: " << calculateAverage("activeNeuronCount") << std::endl;
        oss << std::endl;
        
        // Memory system trends
        oss << "=== Memory System Trends ===" << std::endl;
        oss << "Average working memory traces: " << calculateAverage("workingMemoryTraces") << std::endl;
        oss << "Average episodic episodes: " << calculateAverage("episodicMemoryEpisodes") << std::endl;
        oss << std::endl;
        
        // Neuromodulation trends
        oss << "=== Neuromodulation Trends ===" << std::endl;
        oss << "Average dopamine level: " << calculateAverage("dopamineLevel") << std::endl;
        oss << "Average curiosity level: " << calculateAverage("curiosityLevel") << std::endl;
        oss << std::endl;
        
        // Plasticity trends
        oss << "=== Plasticity Trends ===" << std::endl;
        oss << "Total plasticity updates: " << getTotalPlasticityUpdates() << std::endl;
        oss << "Total weight change: " << getTotalWeightChange() << std::endl;
        oss << "Average weight change per update: " << calculateAverageWeightChange() << std::endl;
        oss << std::endl;
        
        // Performance summary
        oss << "=== Performance Summary ===" << std::endl;
        if (!collectedMetrics.empty()) {
            const auto& latest = getLatestMetrics();
            oss << "Last step metrics:" << std::endl;
            oss << "  Neurons: " << latest.neuronCount << std::endl;
            oss << "  Spikes: " << latest.totalSpikeCount << std::endl;
            oss << "  Synapses: " << latest.synapseCount << std::endl;
            oss << "  Memory usage: " << latest.memoryUsageMB << " MB" << std::endl;
        }
        
        return oss.str();
    }
    
    // Get simulation duration
    size_t getSimulationDuration() const {
        if (collectedMetrics.size() < 2) return collectedMetrics.size();
        
        auto first = collectedMetrics.front().step;
        auto last = collectedMetrics.back().step;
        return last - first + 1;
    }
    
    // Get maximum neurons
    size_t getMaxNeurons() const {
        size_t maxNeurons = 0;
        for (const auto& metrics : collectedMetrics) {
            maxNeurons = std::max(maxNeurons, metrics.neuronCount);
        }
        return maxNeurons;
    }
    
    // Get total spikes
    size_t getTotalSpikes() const {
        size_t total = 0;
        for (const auto& metrics : collectedMetrics) {
            total += metrics.totalSpikeCount;
        }
        return total;
    }
    
    // Get total plasticity updates
    size_t getTotalPlasticityUpdates() const {
        size_t total = 0;
        for (const auto& metrics : collectedMetrics) {
            total += metrics.plasticityUpdates;
        }
        return total;
    }
    
    // Get total weight change
    float getTotalWeightChange() const {
        float total = 0.0f;
        for (const auto& metrics : collectedMetrics) {
            total += metrics.totalWeightChange;
        }
        return total;
    }
    
    // Calculate average for specific metric
    template<typename T>
    T calculateAverage(const std::string& metricName) const {
        if (collectedMetrics.empty()) return T();
        
        std::vector<T> values;
        for (const auto& metrics : collectedMetrics) {
            if (metricName == "averageFiringRate") {
                values.push_back(static_cast<T>(metrics.averageFiringRate));
            } else if (metricName == "excitationInhibitionRatio") {
                values.push_back(static_cast<T>(metrics.excitationInhibitionRatio));
            } else if (metricName == "activeNeuronCount") {
                values.push_back(static_cast<T>(metrics.activeNeuronCount));
            } else if (metricName == "workingMemoryTraces") {
                values.push_back(static_cast<T>(metrics.workingMemoryTraces));
            } else if (metricName == "episodicMemoryEpisodes") {
                values.push_back(static_cast<T>(metrics.episodicMemoryEpisodes));
            } else if (metricName == "dopamineLevel") {
                values.push_back(static_cast<T>(metrics.dopamineLevel));
            } else if (metricName == "curiosityLevel") {
                values.push_back(static_cast<T>(metrics.curiosityLevel));
            }
        }
        
        if (values.empty()) return T();
        
        T sum = std::accumulate(values.begin(), values.end(), T(0));
        return sum / values.size();
    }
    
    // Calculate average weight change
    float calculateAverageWeightChange() const {
        const auto& updates = getTotalPlasticityUpdates();
        if (updates == 0) return 0.0f;
        return getTotalWeightChange() / updates;
    }
    
    // Clear all collected metrics
    void clear() {
        collectedMetrics.clear();
        rollingMetrics.clear();
    }
    
private:
    void updateRollingMetrics(const BrainMetrics& metrics) {
        // Simple rolling average implementation
        // In a real system, this would be more sophisticated
        if (rollingMetrics.find("averageFiringRate") == rollingMetrics.end()) {
            rollingMetrics["averageFiringRate"] = std::vector<float>();
        }
        rollingMetrics["averageFiringRate"].push_back(metrics.averageFiringRate);
        
        if (rollingMetrics["averageFiringRate"].size() > 100) {
            rollingMetrics["averageFiringRate"].erase(
                rollingMetrics["averageFiringRate"].begin());
        }
    }
    
    std::vector<BrainMetrics> collectedMetrics;
    std::unordered_map<std::string, std::vector<float>> rollingMetrics;
};

} // namespace nlm