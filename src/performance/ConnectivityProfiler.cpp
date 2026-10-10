#include "performance/ConnectivityProfiler.hpp"

namespace nlm {

inline void ConnectivityProfiler::recordConnectivity(const std::string& operationName, size_t neuronCount,
                                                   size_t synapseCount, double averageWeight,
                                                   double weightVariance, size_t minConns, size_t maxConns) {
    if (!enabled_) return;
    
    // Calculate connection probability
    double connectionProbability = 0.0;
    if (neuronCount > 0) {
        connectionProbability = static_cast<double>(synapseCount) / (neuronCount * neuronCount);
    }
    
    ConnectivityResult result;
    result.operation = operationName;
    result.neuronCount = neuronCount;
    result.synapseCount = synapseCount;
    result.connectionProbability = connectionProbability;
    result.averageConnectionWeight = averageWeight;
    result.weightVariance = weightVariance;
    result.minConnectionsPerNeuron = minConns;
    result.maxConnectionsPerNeuron = maxConns;
    
    results_.push_back(result);
}

inline const ConnectivityProfiler::ConnectivityResult* ConnectivityProfiler::getResult(const std::string& operationName) const {
    for (const auto& result : results_) {
        if (result.operation == operationName) {
            return &result;
        }
    }
    return nullptr;
}

inline void ConnectivityProfiler::clear() {
    results_.clear();
}

inline void ConnectivityProfiler::printSummary() const {
    if (results_.empty()) {
        std::cout << "No connectivity profiling data available." << std::endl;
        return;
    }
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        Connectivity Profiler Summary                              ║" << std::endl;
    std::cout << "╠═════════════════╦═══════════════╦════════════╦════════════════╦════════════════════╣" << std::endl;
    std::cout << "║ Operation       ║ Connection %  ║ Synapses   ║ Avg Weight   ║ Conn/Neuron Range ║" << std::endl;
    std::cout << "╠═════════════════╬═══════════════╬════════════╬════════════════╬════════════════════╣" << std::endl;
    
    for (const auto& result : results_) {
        std::cout << "║ " << std::setw(15) << std::left << result.operation
                  << " ║ " << std::setw(15) << (result.connectionProbability * 100)
                  << "%         ║ " << std::setw(10) << result.synapseCount
                  << " ║ " << std::setw(12) << result.averageConnectionWeight
                  << " ║ " << std::setw(5) << result.minConnectionsPerNeuron
                  << " - " << std::setw(5) << result.maxConnectionsPerNeuron
                  << "         ║" << std::endl;
    }
    
    std::cout << "╚═════════════════╩═══════════════╩════════════╩════════════════╩════════════════════╝" << std::endl;
}

inline void ConnectivityProfiler::printOperationResults(const std::string& operationName) const {
    const auto* result = getResult(operationName);
    if (!result) {
        std::cout << "Operation \"" << operationName << "\" not found in connectivity profiling results." << std::endl;
        return;
    }
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      Connectivity Profile Results for: " << std::left << std::setw(39) << operationName << "║" << std::endl;
    std::cout << "╠═════════════════╦═════════════════╦════════════════╦════════════════════╣" << std::endl;
    std::cout << "║ Metric         ║ Value           ║              ║                 ║" << std::endl;
    std::cout << "╠═════════════════╬═════════════════╬════════════════╬════════════════════╣" << std::endl;
    std::cout << "║ Neurons        ║ " << std::setw(15) << result->neuronCount << "              ║" << std::endl;
    std::cout << "║ Synapses       ║ " << std::setw(15) << result->synapseCount << "              ║" << std::endl;
    std::cout << "║ Conn Probability║ " << std::setw(15) << (result->connectionProbability * 100) << "%         ║" << std::endl;
    std::cout << "║ Avg Weight     ║ " << std::setw(15) << result->averageConnectionWeight << "              ║" << std::endl;
    std::cout << "║ Weight Variance║ " << std::setw(15) << result->weightVariance << "              ║" << std::endl;
    std::cout << "║ Conn/Neuron    ║ " << std::setw(15) << result->minConnectionsPerNeuron << " - " << result->maxConnectionsPerNeuron << "      ║" << std::endl;
    std::cout << "╚═════════════════╩═════════════════╩════════════════╩════════════════════╝" << std::endl;
}

inline size_t ConnectivityProfiler::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& result : results_) {
        total += result.neuronCount;
    }
    return total;
}

inline size_t ConnectivityProfiler::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& result : results_) {
        total += result.synapseCount;
    }
    return total;
}

inline void ConnectivityProfiler::reset() {
    results_.clear();
}

} // namespace nlm
