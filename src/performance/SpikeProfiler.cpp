#include "performance/SpikeProfiler.hpp"

namespace nlm {

inline void SpikeProfiler::recordSpikeActivity(const std::string& operationName, size_t spikeCount,
                                             size_t neuronsSpiking, const std::chrono::microseconds& processingTime) {
    if (!enabled_) return;
    
    auto it = spikeData_.find(operationName);
    if (it == spikeData_.end()) {
        spikeData_[operationName] = {spikeCount, neuronsSpiking, processingTime, 1};
    } else {
        it->second.spikeCount += spikeCount;
        if (it->second.neuronsSpiking < neuronsSpiking) {
            it->second.neuronsSpiking = neuronsSpiking;
        }
        it->second.processingTime += processingTime;
        it->second.stepCount++;
    }
}

inline const SpikeProfiler::SpikeActivityResult* SpikeProfiler::getResult(const std::string& operationName) const {
    for (const auto& result : results_) {
        if (result.operation == operationName) {
            return &result;
        }
    }
    return nullptr;
}

inline void SpikeProfiler::clear() {
    spikeData_.clear();
    results_.clear();
}

inline void SpikeProfiler::printSummary() const {
    if (results_.empty()) {
        std::cout << "No spike profiling data available." << std::endl;
        return;
    }
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        Spike Profiler Summary                                     ║" << std::endl;
    std::cout << "╠═════════════════╦═════════════════╦═════════════════╦════════════╦════════════╣" << std::endl;
    std::cout << "║ Operation       ║ Spike Count     ║ Spikes/Step    ║ Spike Rate ║ Max Neurons ║" << std::endl;
    std::cout << "╠═════════════════╬═════════════════╬═════════════════╬════════════╬════════════╣" << std::endl;
    
    for (const auto& result : results_) {
        std::cout << "║ " << std::setw(15) << std::left << result.operation
                  << " ║ " << std::setw(15) << result.spikeCount
                  << " ║ " << std::setw(15) << result.averageSpikesPerStep
                  << " ║ " << std::setw(10) << result.spikeRateHz
                  << " ║ " << std::setw(10) << result.maxNeuronsSpiking
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═════════════════╩═════════════════╩═════════════════╩════════════╩════════════╝" << std::endl;
}

inline void SpikeProfiler::printOperationResults(const std::string& operationName) const {
    const auto* result = getResult(operationName);
    if (!result) {
        std::cout << "Operation \"" << operationName << "\" not found in spike profiling results." << std::endl;
        return;
    }
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      Spike Profile Results for: " << std::left << std::setw(41) << operationName << "║" << std::endl;
    std::cout << "╠═════════════════╦═════════════════╦═════════════════╦════════════╦════════════╣" << std::endl;
    std::cout << "║ Metric         ║ Value           ║                ║           ║            ║" << std::endl;
    std::cout << "╠═════════════════╬═════════════════╬═════════════════╬════════════╬════════════╣" << std::endl;
    std::cout << "║ Spike Count    ║ " << std::setw(15) << result->spikeCount << "                ║" << std::endl;
    std::cout << "║ Spikes/Step    ║ " << std::setw(15) << result->averageSpikesPerStep << "                ║" << std::endl;
    std::cout << "║ Spike Rate     ║ " << std::setw(15) << result->spikeRateHz << " Hz            ║" << std::endl;
    std::cout << "║ Max Neurons    ║ " << std::setw(15) << result->maxNeuronsSpiking << "            ║" << std::endl;
    std::cout << "║ Processing Time║ " << std::setw(15) << result->processingTime.count() << " μs       ║" << std::endl;
    std::cout << "╚═════════════════╩═════════════════╩═════════════════╩════════════╩════════════╝" << std::endl;
}

inline size_t SpikeProfiler::getTotalSpikeCount() const {
    size_t total = 0;
    for (const auto& result : results_) {
        total += result.spikeCount;
    }
    return total;
}

inline std::chrono::microseconds SpikeProfiler::getTotalProcessingTime() const {
    std::chrono::microseconds total{0};
    for (const auto& result : results_) {
        total += result.processingTime;
    }
    return total;
}

inline void SpikeProfiler::reset() {
    for (auto& pair : spikeData_) {
        pair.second = SpikeActivityData{};
    }
}

} // namespace nlm
