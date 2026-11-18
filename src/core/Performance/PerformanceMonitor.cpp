// Performance monitoring and debugging implementation
#include "PerformanceMonitor.hpp"
#include "ProgressReporter.hpp"
#include "OutputFormatter.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace nlm {

struct PerformanceMonitor::Impl {
    std::map<std::string, std::chrono::steady_clock::time_point> startTimes_;
    std::map<std::string, double> durations_;
    std::map<std::string, std::chrono::steady_clock::time_point> epoch_;
    std::map<std::string, size_t> operationCounts_;
    std::map<std::string, double> operationTotalTimes_;
    std::map<std::string, double> operationAverageTimes_;
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {
    pImpl->epoch_["total"] = std::chrono::steady_clock::now();
}

void PerformanceMonitor::start(const std::string& operation) {
    pImpl->startTimes_[operation] = std::chrono::steady_clock::now();
    pImpl->operationCounts_[operation] = 0;
}

void PerformanceMonitor::stop(const std::string& operation) {
    auto end = std::chrono::steady_clock::now();
    auto start = pImpl->startTimes_[operation];
    double duration = std::chrono::duration<double>(end - start).count();
    
    pImpl->durations_[operation] = duration;
    pImpl->operationCounts_[operation]++;
    pImpl->operationTotalTimes_[operation] += duration;
    
    if (pImpl->operationCounts_[operation] > 0) {
        pImpl->operationAverageTimes_[operation] = 
            pImpl->operationTotalTimes_[operation] / pImpl->operationCounts_[operation];
    }
}

double PerformanceMonitor::getDuration(const std::string& operation) const {
    auto it = pImpl->durations_.find(operation);
    return it != pImpl->durations_.end() ? it->second : 0.0;
}

void PerformanceMonitor::logProfile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "NLM Performance Profile\n";
    file << "========================\n\n";
    
    file << "Operation Profiling:\n";
    file << "-------------------\n";
    
    for (const auto& pair : pImpl->durations_) {
        const std::string& operation = pair.first;
        double duration = pair.second;
        size_t count = pImpl->operationCounts_[operation];
        double average = pImpl->operationAverageTimes_[operation];
        
        file << operation << ":\n";
        file << "  Duration: " << OutputFormatter::formatDuration(duration) << "\n";
        file << "  Count: " << count << "\n";
        file << "  Average: " << OutputFormatter::formatDuration(average) << "\n";
        file << "  Percentage: " << (duration / getDuration("total")) * 100.0 << "%\n\n";
    }
    
    // Memory usage estimation (simplified)
    file << "Memory Usage Estimate:\n";
    file << "-------------------\n";
    file << "This profile does not include detailed memory analysis.\n";
    file << "For detailed memory profiling, use system tools like valgrind or address sanitizer.\n\n";
    
    file << "Performance Recommendations:\n";
    file << "----------------------\n";
    
    // Analyze performance bottlenecks
    std::vector<std::pair<std::string, double>> sortedOps;
    for (const auto& pair : pImpl->durations_) {
        sortedOps.push_back({pair.first, pair.second});
    }
    
    std::sort(sortedOps.begin(), sortedOps.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (const auto& pair : sortedOps) {
        if (pair.first != "total" && pair.second > getDuration("total") * 0.1) {
            file << "- " << pair.first << " takes " << 
                OutputFormatter::formatDuration(pair.second) << " ("
                << (pair.second / getDuration("total")) * 100.0 << "% of total)\n";
        }
    }
    
    file << std::endl;
}

} // namespace nlm