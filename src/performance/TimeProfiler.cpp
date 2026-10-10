#include "performance/TimeProfiler.hpp"

namespace nlm {

inline void TimeProfiler::startProfile(const std::string& operationName) {
    if (!enabled_) return;
    
    auto it = activeProfiles_.find(operationName);
    if (it == activeProfiles_.end()) {
        activeProfiles_[operationName].startTime = 
            std::chrono::high_resolution_clock::now();
    } else {
        it->second.startTime = std::chrono::high_resolution_clock::now();
    }
}

inline void TimeProfiler::stopProfile(const std::string& operationName) {
    if (!enabled_) return;
    
    auto it = activeProfiles_.find(operationName);
    if (it == activeProfiles_.end()) return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - it->second.startTime);
    
    it->second.accumulatedTime += duration;
    it->second.callCount++;
    
    // Add to results vector for accumulation
    bool found = false;
    for (auto& result : results_) {
        if (result.operation == operationName) {
            result.duration = std::chrono::microseconds(static_cast<long>(result.duration.count() + duration.count()));
            result.callCount += it->second.callCount;
            found = true;
            break;
        }
    }
    
    if (!found) {
        ProfileResult result;
        result.operation = operationName;
        result.duration = duration;
        result.callCount = it->second.callCount;
        results_.push_back(result);
    }
}

inline const TimeProfiler::ProfileResult* TimeProfiler::getResult(const std::string& operationName) const {
    for (const auto& result : results_) {
        if (result.operation == operationName) {
            return &result;
        }
    }
    return nullptr;
}

inline void TimeProfiler::clear() {
    activeProfiles_.clear();
    results_.clear();
}

inline void TimeProfiler::printSummary() const {
    if (results_.empty()) {
        std::cout << "No profiling data available." << std::endl;
        return;
    }
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        Time Profiler Summary                                     ║" << std::endl;
    std::cout << "╠═════════════════╦═════════════════╦════════════╦════════════╦═══════════════╣" << std::endl;
    std::cout << "║ Operation       ║ Total Time (μs) ║ Calls      ║ Avg Time (μs) ║ Std Dev (μs)   ║" << std::endl;
    std::cout << "╠═════════════════╬═════════════════╬════════════╬════════════╬═══════════════╣" << std::endl;
    
    for (const auto& result : results_) {
        std::cout << "║ " << std::setw(15) << std::left << result.operation
                  << " ║ " << std::setw(15) << result.duration.count()
                  << " ║ " << std::setw(10) << result.callCount
                  << " ║ " << std::setw(11) << result.averageTimeUs
                  << " ║ " << std::setw(13) << result.stdDevUs
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═════════════════╩═════════════════╩════════════╩════════════╩═══════════════╝" << std::endl;
}

inline void TimeProfiler::printOperationResults(const std::string& operationName) const {
    const auto* result = getResult(operationName);
    if (!result) {
        std::cout << "Operation \"" << operationName << "\" not found in profiling results." << std::endl;
        return;
    }
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        Profile Results for: " << std::left << std::setw(36) << operationName << "║" << std::endl;
    std::cout << "╠═════════════════╦═════════════════╦════════════╦════════════╦═══════════════╣" << std::endl;
    std::cout << "║ Metric         ║ Value           ║           ║           ║              ║" << std::endl;
    std::cout << "╠═════════════════╬═════════════════╬════════════╬════════════╬═══════════════╣" << std::endl;
    std::cout << "║ Total Time     ║ " << std::setw(15) << result->duration.count() << " μs          ║" << std::endl;
    std::cout << "║ Calls          ║ " << std::setw(15) << result->callCount << "             ║" << std::endl;
    std::cout << "║ Average Time   ║ " << std::setw(15) << result->averageTimeUs << " μs          ║" << std::endl;
    std::cout << "║ Min Time       ║ " << std::setw(15) << result->minTimeUs << " μs          ║" << std::endl;
    std::cout << "║ Max Time       ║ " << std::setw(15) << result->maxTimeUs << " μs          ║" << std::endl;
    std::cout << "║ Std Dev        ║ " << std::setw(15) << result->stdDevUs << " μs          ║" << std::endl;
    std::cout << "╚═════════════════╩═════════════════╩════════════╩════════════╩═══════════════╝" << std::endl;
}

inline double TimeProfiler::getTotalTime() const {
    double total = 0.0;
    for (const auto& result : results_) {
        total += result.averageTimeUs * result.callCount;
    }
    return total;
}

inline size_t TimeProfiler::getTotalCallCount() const {
    size_t total = 0;
    for (const auto& result : results_) {
        total += result.callCount;
    }
    return total;
}

inline void TimeProfiler::reset() {
    for (auto& pair : activeProfiles_) {
        pair.second.accumulatedTime = std::chrono::microseconds{0};
        pair.second.callCount = 0;
    }
}

} // namespace nlm
