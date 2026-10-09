#include "CallStackProfiler.hpp"
#include <cxxabi.h>
#include <sstream>
#include <unistd.h>

namespace nlm {
namespace profiling {

// CallStackProfiler implementation

void CallStackProfiler::startProfiling() {
    if (profiling_) return;
    profiling_ = true;
    pImpl_->startTime = std::chrono::steady_clock::now();
    
    if (samplingInterval_ > 0.0) {
        pImpl_->samplingThread = std::thread(&CallStackProfiler::samplingThreadFunc, this);
    }
}

void CallStackProfiler::stopProfiling() {
    if (!profiling_) return;
    profiling_ = false;
    
    if (pImpl_->samplingThread.joinable()) {
        pImpl_->samplingThread.join();
    }
}

bool CallStackProfiler::isProfiling() const {
    return profiling_;
}

void CallStackProfiler::captureStack(const std::string& operation) {
    if (!profiling_) return;
    
    // Simplified stack capture - could be enhanced with actual stack walking
    CallStackSample sample;
    sample.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now() - pImpl_->startTime).count();
    sample.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    sample.operation = operation;
    
    // Add some dummy stack frames for demonstration
    sample.stackFrames.push_back("CallStackProfiler::captureStack");
    sample.stackFrames.push_back("SomeClass::someMethod");
    sample.stackFrames.push_back("main");
    
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    stackSamples_.push_back(sample);
}

void CallStackProfiler::setMaxStackDepth(size_t depth) {
    maxStackDepth_ = depth;
}

void CallStackProfiler::setSamplingInterval(double interval) {
    samplingInterval_ = interval;
}

std::vector<CallStackProfiler::CallStackSample> CallStackProfiler::getMostCommonStacks(size_t limit) const {
    std::map<std::string, size_t> stackCounts;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (const auto& sample : stackSamples_) {
        std::stringstream ss;
        for (size_t i = 0; i < std::min(sample.stackFrames.size(), size_t(5)); ++i) {
            if (i > 0) ss << "->";
            ss << sample.stackFrames[i];
        }
        std::string stackKey = ss.str();
        stackCounts[stackKey]++;
    }
    
    std::vector<std::pair<std::string, size_t>> sortedStacks(stackCounts.begin(), stackCounts.end());
    std::sort(sortedStacks.begin(), sortedStacks.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<CallStackSample> result;
    result.reserve(std::min(limit, sortedStacks.size()));
    
    for (size_t i = 0; i < std::min(limit, sortedStacks.size()); ++i) {
        CallStackSample sample;
        sample.stackFrames = parseStackFramesFromKey(sortedStacks[i].first);
        sample.timestamp = 0.0; // Simplified
        sample.threadId = "thread_0";
        sample.operation = "common_stack";
        result.push_back(sample);
    }
    
    return result;
}

std::vector<std::string> CallStackProfiler::getTopCallers(size_t limit) const {
    std::map<std::string, double> functionTimes;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (const auto& sample : stackSamples_) {
        for (const auto& frame : sample.stackFrames) {
            functionTimes[frame] += 1.0; // Simplified time calculation
        }
    }
    
    std::vector<std::pair<std::string, double>> sortedFunctions(functionTimes.begin(), functionTimes.end());
    std::sort(sortedFunctions.begin(), sortedFunctions.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> result;
    result.reserve(std::min(limit, sortedFunctions.size()));
    for (size_t i = 0; i < std::min(limit, sortedFunctions.size()); ++i) {
        result.push_back(sortedFunctions[i].first);
    }
    
    return result;
}

double CallStackProfiler::calculateTimeInFunction(const std::string& functionName) const {
    double totalTime = 0.0;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (const auto& sample : stackSamples_) {
        for (const auto& frame : sample.stackFrames) {
            if (frame == functionName) {
                totalTime += 1.0; // Simplified time calculation
                break;
            }
        }
    }
    
    return totalTime;
}

std::vector<std::string> CallStackProfiler::findCallStackPatterns(const std::string& pattern) const {
    std::vector<std::string> matchingStacks;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (const auto& sample : stackSamples_) {
        std::stringstream ss;
        for (size_t i = 0; i < sample.stackFrames.size(); ++i) {
            if (i > 0) ss << "->";
            ss << sample.stackFrames[i];
        }
        std::string stackKey = ss.str();
        if (stackKey.find(pattern) != std::string::npos) {
            matchingStacks.push_back(stackKey);
        }
    }
    
    return matchingStacks;
}

bool CallStackProfiler::exportToProfileFile(const std::string& filepath) const {
    // TODO: Implement profile file export
    return false;
}

bool CallStackProfiler::exportToCallGraph(const std::string& filepath) const {
    // TODO: Implement call graph export
    return false;
}

void CallStackProfiler::integrateWithProfiler(PerformanceProfiler& profiler) {
    // TODO: Integrate with PerformanceProfiler
}

void CallStackProfiler::samplingThreadFunc() {
    while (profiling_) {
        std::this_thread::sleep_for(std::chrono::duration<double>(samplingInterval_));
        if (profiling_) {
            captureStack("sampling");
        }
    }
}

std::vector<std::string> CallStackProfiler::parseStackFramesFromKey(const std::string& stackKey) const {
    std::vector<std::string> frames;
    std::stringstream ss(stackKey);
    std::string segment;
    while (std::getline(ss, segment, '>')) {
        if (segment != "CallStackProfiler::captureStack") {
            frames.push_back(segment);
        }
    }
    return frames;
}

} // namespace profiling
} // namespace nlm
