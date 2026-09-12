#pragma once

#include "PerformanceMonitor.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Neural Activity Sample for profiling
struct NeuralActivitySample {
    double timestamp;           // Simulation time when sample was taken
    size_t neuronCount;          // Total neurons in brain
    size_t synapseCount;         // Total synapses in brain  
    float firingRate;            // Average firing rate in Hz
    size_t spikeCount;           // Total spikes in this step
    size_t activeNeurons;       // Number of neurons actively firing
    size_t firingNeurons;        // Number of neurons that fired in this step
    
    NeuralActivitySample()
        : timestamp(0.0), neuronCount(0), synapseCount(0),
          firingRate(0.0f), spikeCount(0), activeNeurons(0), firingNeurons(0) {}
    
    NeuralActivitySample(double ts, size_t neurons, size_t synapses, float rate,
                         size_t spikes, size_t active, size_t firing)
        : timestamp(ts), neuronCount(neurons), synapseCount(synapses),
          firingRate(rate), spikeCount(spikes), activeNeurons(active), firingNeurons(firing) {}
};

// Neural Activity Profiler class
class NeuralActivityProfiler {
public:
    NeuralActivityProfiler(PerformanceMonitor& monitor) : performanceMonitor(monitor) {
        startTime = monitor.getCurrentTimeAsDouble();
        samplesCollected = 0;
        active = false;
    }
    
    ~NeuralActivityProfiler() {
        if (active) {
            stop();
        }
    }
    
    // Start recording neural activity
    void start() {
        active = true;
        startTime = performanceMonitor.getCurrentTimeAsDouble();
        samplesCollected = 0;
        NLM_LOG_INFO("NeuralActivityProfiler started");
    }
    
    // Stop recording
    void stop() {
        active = false;
        NLM_LOG_INFO("NeuralActivityProfiler stopped");
    }
    
    // Record a snapshot of neural activity
    void record(const Brain* brain, size_t stepNumber) {
        if (!active || !brain) return;
        
        auto currentTime = performanceMonitor.getCurrentTimeAsDouble();
        auto metrics = performanceMonitor.getCurrentMetrics();
        
        NeuralActivitySample sample;
        sample.timestamp = currentTime;
        sample.neuronCount = brain->getTotalNeuronCount();
        sample.synapseCount = brain->getTotalSynapseCount();
        sample.firingRate = brain->getAverageFiringRate();
        sample.spikeCount = brain->getTotalSpikeCount();
        sample.activeNeurons = brain->getActiveNeuronCount();
        sample.firingNeurons = brain->getFiringNeuronCount();
        
        // Add to samples vector
        {
            std::lock_guard<std::mutex> lock(mutex);
            neuralActivitySamples.push_back(sample);
            samplesCollected++;
            
            // Limit sample size to prevent memory issues
            if (neuralActivitySamples.size() > 10000) {
                neuralActivitySamples.erase(neuralActivitySamples.begin(),
                                           neuralActivitySamples.begin() + 1000);
            }
        }
        
        // Record to performance monitor
        performanceMonitor.recordStep(
            static_cast<double>(stepNumber),
            brain->getTotalSpikeCount(),
            brain->getActiveNeuronCount()
        );
    }
    
    // Get all recorded samples
    std::vector<NeuralActivitySample> getSamples() const {
        std::lock_guard<std::mutex> lock(mutex);
        return neuralActivitySamples;
    }
    
    // Get number of samples collected
    size_t getSampleCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return samplesCollected;
    }
    
    // Get samples within a time range
    std::vector<NeuralActivitySample> getSamplesInRange(double startTime, double endTime) const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<NeuralActivitySample> result;
        
        for (const auto& sample : neuralActivitySamples) {
            if (sample.timestamp >= startTime && sample.timestamp <= endTime) {
                result.push_back(sample);
            }
        }
        
        return result;
    }
    
    // Calculate average firing rate
    double getAverageFiringRate() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (neuralActivitySamples.empty()) return 0.0;
        
        double totalRate = 0.0;
        for (const auto& sample : neuralActivitySamples) {
            totalRate += sample.firingRate;
        }
        return totalRate / neuralActivitySamples.size();
    }
    
    // Calculate average spike rate per step
    double getAverageSpikesPerStep() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (neuralActivitySamples.empty()) return 0.0;
        
        double totalSpikes = 0.0;
        for (const auto& sample : neuralActivitySamples) {
            totalSpikes += sample.spikeCount;
        }
        return totalSpikes / neuralActivitySamples.size();
    }
    
    // Get maximum neurons observed
    size_t getMaxNeurons() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (neuralActivitySamples.empty()) return 0;
        
        size_t maxNeurons = 0;
        for (const auto& sample : neuralActivitySamples) {
            if (sample.neuronCount > maxNeurons) maxNeurons = sample.neuronCount;
        }
        return maxNeurons;
    }
    
    // Get maximum synapses observed
    size_t getMaxSynapses() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (neuralActivitySamples.empty()) return 0;
        
        size_t maxSynapses = 0;
        for (const auto& sample : neuralActivitySamples) {
            if (sample.synapseCount > maxSynapses) maxSynapses = sample.synapseCount;
        }
        return maxSynapses;
    }
    
    // Get neural activity trend (positive = increasing, negative = decreasing)
    double getActivityTrend() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (neuralActivitySamples.size() < 2) return 0.0;
        
        // Simple linear regression on firing rate over time
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
        double firstTime = neuralActivitySamples.front().timestamp;
        double timeRange = neuralActivitySamples.back().timestamp - firstTime;
        
        if (timeRange < 0.001) return 0.0;
        
        for (const auto& sample : neuralActivitySamples) {
            double normalizedTime = (sample.timestamp - firstTime) / timeRange;
            double rate = sample.firingRate;
            
            sumX += normalizedTime;
            sumY += rate;
            sumXY += normalizedTime * rate;
            sumX2 += normalizedTime * normalizedTime;
        }
        
        size_t n = neuralActivitySamples.size();
        double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        
        return slope; // Firing rate change per normalized time unit
    }
    
    // Export neural activity data to CSV
    bool exportToCSV(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open neural activity CSV file: " + filepath);
            return false;
        }
        
        // Write header
        file << "timestamp,neuron_count,synapse_count,firing_rate,spike_count,active_neurons,firing_neurons\n";
        
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& sample : neuralActivitySamples) {
            file << std::fixed << std::setprecision(3)
                 << sample.timestamp << ","
                 << sample.neuronCount << ","
                 << sample.synapseCount << ","
                 << sample.firingRate << ","
                 << sample.spikeCount << ","
                 << sample.activeNeurons << ","
                 << sample.firingNeurons << "\n";
        }
        
        file.close();
        NLM_LOG_INFO("Neural activity data exported to CSV: " + filepath);
        return true;
    }
    
    // Generate neural activity report
    std::string generateReport() const {
        std::stringstream ss;
        ss << "=== Neural Activity Profiler Report ===\n\n";
        
        ss << "Collection Summary:\n";
        ss << "  Samples collected: " << getSampleCount() << "\n";
        ss << "  Duration: " << (getDuration() / 1000.0) << " seconds\n";
        ss << "  Average firing rate: " << getAverageFiringRate() << " Hz\n";
        ss << "  Average spikes per step: " << getAverageSpikesPerStep() << "\n";
        ss << "  Max neurons observed: " << getMaxNeurons() << "\n";
        ss << "  Max synapses observed: " << getMaxSynapses() << "\n\n";
        
        ss << "Activity Trend:\n";
        double trend = getActivityTrend();
        if (trend > 0.01) {
            ss << "  Increasing (rising activity)\n";
        } else if (trend < -0.01) {
            ss << "  Decreasing (declining activity)\n";
        } else {
            ss << "  Stable\n";
        }
        ss << "  Trend value: " << trend << "\n\n";
        
        ss << "Neural Load:\n";
        ss << "  Current neuron count: " << getCurrentNeurons() << "\n";
        ss << "  Current synapse count: " << getCurrentSynapses() << "\n";
        ss << "  Activity density: " << getActivityDensity() << "%\n\n";
        
        ss << "Performance Indicators:\n";
        ss << "  Firing efficiency: " << getFiringEfficiency() << "\n";
        ss << "  Network connectivity: " << getConnectivityRatio() << "\n";
        ss << "  Spike regularity: " << getSpikeRegularity() << "\n\n";
        
        return ss.str();
    }
    
    // Get current neural state
    struct CurrentNeuralState {
        size_t neuronCount;
        size_t synapseCount;
        float firingRate;
        size_t activeNeurons;
        double timeOffset;
    };
    
    CurrentNeuralState getCurrentNeuralState() const {
        std::lock_guard<std::mutex> lock(mutex);
        CurrentNeuralState state;
        
        if (neuralActivitySamples.empty()) {
            state.neuronCount = 0;
            state.synapseCount = 0;
            state.firingRate = 0.0f;
            state.activeNeurons = 0;
            state.timeOffset = 0.0;
            return state;
        }
        
        const auto& latestSample = neuralActivitySamples.back();
        state.neuronCount = latestSample.neuronCount;
        state.synapseCount = latestSample.synapseCount;
        state.firingRate = latestSample.firingRate;
        state.activeNeurons = latestSample.activeNeurons;
        state.timeOffset = latestSample.timestamp - startTime;
        
        return state;
    }
    
    // Reset profiler data
    void reset() {
        std::lock_guard<std::mutex> lock(mutex);
        neuralActivitySamples.clear();
        samplesCollected = 0;
        startTime = performanceMonitor.getCurrentTimeAsDouble();
        NLM_LOG_INFO("NeuralActivityProfiler reset");
    }
    
private:
    PerformanceMonitor& performanceMonitor;
    mutable std::mutex mutex;
    std::vector<NeuralActivitySample> neuralActivitySamples;
    size_t samplesCollected;
    double startTime;
    bool active;
    
    // Helper functions
    double getDuration() const {
        return performanceMonitor.getCurrentTimeAsDouble() - startTime;
    }
    
    size_t getCurrentNeurons() const {
        auto state = getCurrentNeuralState();
        return state.neuronCount;
    }
    
    size_t getCurrentSynapses() const {
        auto state = getCurrentNeuralState();
        return state.synapseCount;
    }
    
    double getActivityDensity() const {
        auto state = getCurrentNeuralState();
        if (state.neuronCount == 0) return 0.0;
        return (state.activeNeurons / static_cast<double>(state.neuronCount)) * 100.0;
    }
    
    double getFiringEfficiency() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (neuralActivitySamples.empty()) return 0.0;
        
        double totalSpikes = 0.0;
        double totalNeurons = 0.0;
        
        for (const auto& sample : neuralActivitySamples) {
            totalSpikes += sample.spikeCount;
            totalNeurons += static_cast<double>(sample.neuronCount);
        }
        
        return (totalSpikes / totalNeurons) * 1000.0; // Spikes per 1000 neurons
    }
    
    double getConnectivityRatio() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (neuralActivitySamples.empty()) return 0.0;
        
        double totalSynapses = 0.0;
        double totalNeurons = 0.0;
        
        for (const auto& sample : neuralActivitySamples) {
            totalSynapses += static_cast<double>(sample.synapseCount);
            totalNeurons += static_cast<double>(sample.neuronCount);
        }
        
        if (totalNeurons < 1.0) return 0.0;
        return (totalSynapses / totalNeurons);
    }
    
    double getSpikeRegularity() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (neuralActivitySamples.size() < 2) return 0.0;
        
        std::vector<double> spikeRates;
        for (const auto& sample : neuralActivitySamples) {
            spikeRates.push_back(sample.spikeCount);
        }
        
        // Calculate coefficient of variation
        double mean = getAverageSpikesPerStep();
        if (mean < 0.001) return 0.0;
        
        double variance = 0.0;
        for (double rate : spikeRates) {
            variance += (rate - mean) * (rate - mean);
        }
        variance /= spikeRates.size();
        
        return std::sqrt(variance) / mean; // Coefficient of variation
    }
};

} // namespace nlm
