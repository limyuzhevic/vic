// PerformanceAnalyzer implementation
#include "PerformanceAnalyzer.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

void PerformanceAnalyzer::recordBrainOperation(const std::string& operation, 
                                               const PerformanceMetrics& metrics,
                                               size_t neuronCount,
                                               size_t synapseCount) {
    auto it = operationStats_.find(operation);
    if (it == operationStats_.end()) {
        operationStats_[operation] = PerformanceStats();
    }
    
    operationStats_[operation].addSample(metrics.wallClockTime);
}

PerformanceStats PerformanceAnalyzer::getOperationStats(const std::string& operation) const {
    auto it = operationStats_.find(operation);
    if (it != operationStats_.end()) {
        return it->second;
    }
    return PerformanceStats();
}

std::vector<std::string> PerformanceAnalyzer::getAllOperations() const {
    std::vector<std::string> operations;
    operations.reserve(operationStats_.size());
    for (const auto& pair : operationStats_) {
        operations.push_back(pair.first);
    }
    return operations;
}

std::string PerformanceAnalyzer::generateReport() const {
    std::ostringstream oss;
    oss << "=== NLM Performance Analysis ===\n";
    oss << "Recorded " << operationStats_.size() << " operations\n\n";
    
    for (const auto& pair : operationStats_) {
        const auto& name = pair.first;
        const auto& stats = pair.second;
        
        oss << "Operation: " << name << "\n";
        oss << "  Samples: " << stats.getCount() << "\n";
        oss << "  Mean Time: " << std::fixed << std::setprecision(6) 
            << stats.getMean() << "s\n";
        oss << "  Std Dev: " << std::fixed << std::setprecision(6) 
            << stats.getStdDev() << "s\n";
        oss << "  Min Time: " << std::fixed << std::setprecision(6) 
            << stats.getMin() << "s\n";
        oss << "  Max Time: " << std::fixed << std::setprecision(6) 
            << stats.getMax() << "s\n";
        oss << "  Sample Rate: " << std::fixed << std::setprecision(2) 
            << stats.getSampleRate() << " samples/s\n\n";
    }
    
    return oss.str();
}

void PerformanceAnalyzer::clear() {
    operationStats_.clear();
}

void PerformanceAnalyzer::runBenchmarks() {
    // Implementation for running built-in benchmarks
    // This would benchmark various operations like neuron creation,
    // spike propagation, plasticity, etc.
}

} // namespace nlm
