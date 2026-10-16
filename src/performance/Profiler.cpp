// src/performance/Profiler.cpp
#include "Profiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>

namespace nlm {

struct Profiler::Impl {
    struct TimingData {
        double totalDuration;
        double sumSquares;
        size_t count;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        
        TimingData() : totalDuration(0.0), sumSquares(0.0), count(0) {}
    };
    
    std::unordered_map<std::string, TimingData> measurements;
    double totalTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> sessionStart;
    
    Impl() : totalTime(0.0) {
        sessionStart = std::chrono::high_resolution_clock::now();
    }
};

Profiler::Profiler() {
    pImpl = std::make_unique<Impl>();
    verbose_ = false;
    threshold_ = 0.001; // 1ms threshold
}

Profiler::~Profiler() = default;

void Profiler::start(const std::string& name) {
    auto& data = pImpl->measurements[name];
    data.startTime = std::chrono::high_resolution_clock::now();
}

double Profiler::stop(const std::string& name) {
    auto it = pImpl->measurements.find(name);
    if (it == pImpl->measurements.end()) {
        return 0.0;
    }
    
    auto& data = it->second;
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - data.startTime).count();
    
    data.totalDuration += duration;
    data.sumSquares += duration * duration;
    data.count++;
    
    pImpl->totalTime += duration;
    
    if (verbose_ && duration >= threshold_) {
        Logger::getGlobal().info("Profile: " + name + " took " + std::to_string(duration * 1000) + " ms");
    }
    
    return duration;
}

double Profiler::getTotalDuration(const std::string& name) const {
    auto it = pImpl->measurements.find(name);
    return (it != pImpl->measurements.end()) ? it->second.totalDuration : 0.0;
}

double Profiler::getAverageDuration(const std::string& name) const {
    auto it = pImpl->measurements.find(name);
    if (it == pImpl->measurements.end() || it->second.count == 0) {
        return 0.0;
    }
    return it->second.totalDuration / it->second.count;
}

size_t Profiler::getMeasurementCount(const std::string& name) const {
    auto it = pImpl->measurements.find(name);
    return (it != pImpl->measurements.end()) ? it->second.count : 0;
}

std::vector<std::string> Profiler::getOperationNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->measurements.size());
    for (const auto& pair : pImpl->measurements) {
        names.push_back(pair.first);
    }
    return names;
}

void Profiler::clear() {
    pImpl->measurements.clear();
    pImpl->totalTime = 0.0;
    pImpl->sessionStart = std::chrono::high_resolution_clock::now();
}

double Profiler::getTotalTime() const {
    return pImpl->totalTime;
}

std::string Profiler::getReport() const {
    std::stringstream ss;
    ss << "Profiler Report\n";
    ss << "===============\n";
    ss << "Total session time: " << pImpl->totalTime * 1000 << " ms\n\n";
    
    for (const auto& pair : pImpl->measurements) {
        const auto& data = pair.second;
        if (data.count > 0) {
            double avg = data.totalDuration / data.count;
            double stdDev = std::sqrt((data.sumSquares / data.count) - (avg * avg));
            
            ss << "Operation: " << pair.first << "\n";
            ss << "  Total: " << data.totalDuration * 1000 << " ms\n";
            ss << "  Average: " << avg * 1000 << " ms\n";
            ss << "  Std Dev: " << stdDev * 1000 << " ms\n";
            ss << "  Count: " << data.count << "\n\n";
        }
    }
    
    return ss.str();
}

} // namespace nlm