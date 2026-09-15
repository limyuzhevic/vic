#include "Profiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>

namespace nlm {

struct Profiler::Impl {
    struct Sample {
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        std::string operationName;
    };
    
    std::vector<Sample> activeSamples;
    std::unordered_map<std::string, std::vector<PerformanceSample>> samples;
    std::mutex mutex;
};

Profiler::Profiler() : pImpl(std::make_unique<Impl>()), active_(true) {}

Profiler::~Profiler() = default;

void Profiler::startProfile(const std::string& operationName) {
    if (!active_) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    Impl::Sample sample;
    sample.startTime = std::chrono::high_resolution_clock::now();
    sample.operationName = operationName;
    pImpl->activeSamples.push_back(std::move(sample));
}

void Profiler::stopProfile(const std::string& operationName) {
    if (!active_) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->activeSamples.empty()) return;
    
    // Find the most recent sample for this operation
    auto it = std::find_if(pImpl->activeSamples.rbegin(), pImpl->activeSamples.rend(),
                          [&operationName](const Impl::Sample& sample) {
                              return sample.operationName == operationName;
                          });
    
    if (it == pImpl->activeSamples.rend()) return;
    
    auto& sample = *it;
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - sample.startTime);
    
    double durationMs = static_cast<double>(duration.count()) / 1000.0;
    
    // Update or create performance sample
    if (pImpl->samples.find(operationName) == pImpl->samples.end()) {
        pImpl->samples[operationName].emplace_back(operationName, durationMs);
    } else {
        auto& sampleList = pImpl->samples[operationName];
        if (!sampleList.empty()) {
            auto& lastSample = sampleList.back();
            lastSample.durationMs = durationMs;
            lastSample.callCount++;
            lastSample.totalTime += durationMs;
            lastSample.averageTime = lastSample.totalTime / lastSample.callCount;
            
            if (durationMs < lastSample.minTime || lastSample.callCount == 1) {
                lastSample.minTime = durationMs;
            }
            if (durationMs > lastSample.maxTime || lastSample.callCount == 1) {
                lastSample.maxTime = durationMs;
            }
        }
    }
    
    // Remove the active sample
    pImpl->activeSamples.erase(std::next(it).base());
}

const std::vector<PerformanceSample>& Profiler::getSamples(const std::string& operationName) const {
    static const std::vector<PerformanceSample> emptyVector;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->samples.find(operationName);
    if (it != pImpl->samples.end()) {
        return it->second;
    }
    
    return emptyVector;
}

std::vector<std::string> Profiler::getOperationNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> names;
    names.reserve(pImpl->samples.size());
    
    for (const auto& pair : pImpl->samples) {
        names.push_back(pair.first);
    }
    
    return names;
}

double Profiler::getTotalTime(const std::string& operationName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->samples.find(operationName);
    if (it != pImpl->samples.end() && !it->second.empty()) {
        return it->second.back().totalTime;
    }
    
    return 0.0;
}

double Profiler::getAverageTime(const std::string& operationName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->samples.find(operationName);
    if (it != pImpl->samples.end() && !it->second.empty()) {
        return it->second.back().averageTime;
    }
    
    return 0.0;
}

int Profiler::getCallCount(const std::string& operationName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->samples.find(operationName);
    if (it != pImpl->samples.end() && !it->second.empty()) {
        return it->second.back().callCount;
    }
    
    return 0;
}

void Profiler::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->samples.clear();
    pImpl->activeSamples.clear();
}

std::string Profiler::getSummary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Profiler Summary (" << pImpl->samples.size() << " operations):\n";
    
    std::vector<PerformanceSample> allSamples;
    for (const auto& pair : pImpl->samples) {
        if (!pair.second.empty()) {
            allSamples.push_back(pair.second.back());
        }
    }
    
    // Sort by total time
    std::sort(allSamples.begin(), allSamples.end(),
              [](const PerformanceSample& a, const PerformanceSample& b) {
                  return a.totalTime > b.totalTime;
              });
    
    for (const auto& sample : allSamples) {
        oss << "  " << std::left << std::setw(30) << sample.operationName 
            << "total: " << std::fixed << std::setprecision(3) << sample.totalTime << "ms"
            << " (avg: " << sample.averageTime << "ms, " 
            << sample.callCount << " calls, "
            << "min: " << sample.minTime << "ms, "
            << "max: " << sample.maxTime << "ms)\n";
    }
    
    return oss.str();
}

std::vector<PerformanceSample> Profiler::getTopSlowOperations(size_t n) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<PerformanceSample> allSamples;
    for (const auto& pair : pImpl->samples) {
        if (!pair.second.empty()) {
            allSamples.push_back(pair.second.back());
        }
    }
    
    // Sort by total time
    std::sort(allSamples.begin(), allSamples.end(),
              [](const PerformanceSample& a, const PerformanceSample& b) {
                  return a.totalTime > b.totalTime;
              });
    
    if (n > allSamples.size()) {
        n = allSamples.size();
    }
    
    return std::vector<PerformanceSample>(allSamples.begin(), allSamples.begin() + n);
}

} // namespace nlm