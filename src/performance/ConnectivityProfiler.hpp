#ifndef NLM_CONNECTIVITYPROFILER_H
#define NLM_CONNECTIVITYPROFILER_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace nlm {

class ConnectivityProfiler {
public:
    struct ConnectivityResult {
        std::string operation;
        size_t neuronCount;
        size_t synapseCount;
        double connectionProbability;
        double averageConnectionWeight;
        double weightVariance;
        size_t minConnectionsPerNeuron;
        size_t maxConnectionsPerNeuron;
    };

    ConnectivityProfiler() = default;
    ~ConnectivityProfiler() = default;

    // Record connectivity statistics
    void recordConnectivity(const std::string& operationName, size_t neuronCount, 
                          size_t synapseCount, double averageWeight, double weightVariance,
                          size_t minConns, size_t maxConns);

    // Get results for a specific operation
    const ConnectivityResult* getResult(const std::string& operationName) const;

    // Get all results
    const std::vector<ConnectivityResult>& getAllResults() const { return results_; }

    // Clear all profiling data
    void clear();

    // Print profiling summary to stdout
    void printSummary() const;

    // Print results for a specific operation
    void printOperationResults(const std::string& operationName) const;

    // Get total neuron count
    size_t getTotalNeuronCount() const;

    // Get total synapse count
    size_t getTotalSynapseCount() const;

    // Reset counters
    void reset();

    // Set whether profiling is enabled
    void setEnabled(bool enabled) { enabled_ = enabled; }

    // Check if profiling is enabled
    bool isEnabled() const { return enabled_; }

private:
    std::vector<ConnectivityResult> results_;
    bool enabled_{true};
};

} // namespace nlm

#endif // NLM_CONNECTIVITYPROFILER_H
