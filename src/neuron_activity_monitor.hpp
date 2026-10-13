// NeuronActivityMonitor class header
// This file provides the header declaration for the NeuronActivityMonitor class

#ifndef NEURON_ACTIVITY_MONITOR_HPP
#define NEURON_ACTIVITY_MONITOR_HPP

#include <string>
#include <map>
#include <vector>

// NLM includes
#include "../src/advanced_api.hpp"

/**
 * @brief Monitor and analyze neuron activity
 */
class NeuronActivityMonitor {
private:
    std::shared_ptr<Brain> brain_;
    std::vector<FloatArray> activityHistory_;
    std::chrono::steady_clock::time_point startTime_;
    
public:
    NeuronActivityMonitor(std::shared_ptr<Brain> brain);
    
    ~NeuronActivityMonitor() = default;
    
    // Monitoring control
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Analysis methods
    FloatArray getActivityStatistics(size_t neuronIndex) const;
    FloatMatrix getActivityTimeSeries(size_t neuronIndex) const;
    std::map<size_t, double> getFiringRates() const;
    std::map<size_t, double> getBursts() const;
    
    // State analysis
    FloatArray computeCoherence() const;
    std::vector<size_t> findSynchronyClusters() const;
    std::map<size_t, FloatArray> getSpatialDistribution() const;
    
    // Export
    void exportActivityData(const std::string& filepath) const;
    std::string getActivitySummary() const;
};

#endif // NEURON_ACTIVITY_MONITOR_HPP
