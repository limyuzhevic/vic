#include "FunctionCallProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <unordered_set>

namespace nlm {

FunctionCallProfiler::FunctionCallProfiler()
    : profilingInterval_(0.1),
      enableCallStack_(true),
      minSamples_(10),
      maxFunctionDepth_(10),
      totalCalls_(0)
{
}

FunctionCallProfiler::~FunctionCallProfiler() = default;

void FunctionCallProfiler::recordFunctionCall(const std::string& functionName, double time, 
                                             const std::string& context,
                                             const std::string& file, int line,
                                             const std::vector<std::string>& callStack) {
    size_t recursionDepth = 0;
    if (enableCallStack_ && !callStack.empty()) {
        recursionDepth = std::min(callStack.size(), maxFunctionDepth_);
    }
    
    FunctionCall call(functionName, time, context, file, line, callStack, recursionDepth);
    
    std::lock_guard<std::mutex> lock(callsMutex_);
    calls_.push_back(call);
    totalCalls_++;
    
    // Track active functions
    std::lock_guard<std::mutex> activeLock(activeMutex_);
    activeFunctions_[functionName].insert(context);
    
    // Cleanup old calls periodically
    if (calls_.size() % 1000 == 0) {
        cleanupOldCalls();
    }
}

double FunctionCallProfiler::getAverageTime(const std::string& functionName) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(callsMutex_);
        for (const auto& call : calls_) {
            if (call.functionName == functionName) {
                times.push_back(call.time);
            }
        }
    }
    
    if (times.empty()) return 0.0;
    
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / times.size();
}

double FunctionCallProfiler::getMinTime(const std::string& functionName) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(callsMutex_);
        for (const auto& call : calls_) {
            if (call.functionName == functionName) {
                times.push_back(call.time);
            }
        }
    }
    
    if (times.empty()) return 0.0;
    
    return *std::min_element(times.begin(), times.end());
}

double FunctionCallProfiler::getMaxTime(const std::string& functionName) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(callsMutex_);
        for (const auto& call : calls_) {
            if (call.functionName == functionName) {
                times.push_back(call.time);
            }
        }
    }
    
    if (times.empty()) return 0.0;
    
    return *std::max_element(times.begin(), times.end());
}

double FunctionCallProfiler::getStandardDeviation(const std::string& functionName) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(callsMutex_);
        for (const auto& call : calls_) {
            if (call.functionName == functionName) {
                times.push_back(call.time);
            }
        }
    }
    
    if (times.empty()) return 0.0;
    
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    double mean = sum / times.size();
    
    double sqSum = 0.0;
    for (double time : times) {
        sqSum += (time - mean) * (time - mean);
    }
    
    return std::sqrt(sqSum / times.size());
}

std::vector<std::string> FunctionCallProfiler::getFunctionCalls() const {
    std::vector<std::string> functions;
    {
        std::lock_guard<std::mutex> lock(callsMutex_);
        std::unordered_set<std::string> uniqueFuncs;
        for (const auto& call : calls_) {
            uniqueFuncs.insert(call.functionName);
        }
        functions.assign(uniqueFuncs.begin(), uniqueFuncs.end());
    }
    return functions;
}

size_t FunctionCallProfiler::getCallCount(const std::string& functionName) const {
    std::lock_guard<std::mutex> lock(callsMutex_);
    size_t count = 0;
    for (const auto& call : calls_) {
        if (call.functionName == functionName) {
            count++;
        }
    }
    return count;
}

std::string FunctionCallProfiler::analyzeFunctionComplexity() const {
    std::ostringstream oss;
    oss << "Function Complexity Analysis:\n";
    oss << "==========================\n\n";
    
    std::lock_guard<std::mutex> lock(callsMutex_);
    
    std::unordered_map<std::string, size_t> functionComplexity;
    std::unordered_map<std::string, double> avgTimes;
    
    // Calculate complexity metrics for each function
    for (const auto& call : calls_) {
        double& avgTime = avgTimes[call.functionName];
        size_t& complexity = functionComplexity[call.functionName];
        
        // Simple complexity: time + stack depth
        double timeScore = call.time * 1000; // Scale to milliseconds
        size_t stackScore = call.callStack.size() * 10;
        double totalScore = timeScore + stackScore;
        
        if (complexity == 0) {
            complexity = static_cast<size_t>(totalScore);
        } else {
            complexity = static_cast<size_t>((complexity + totalScore) / 2);
        }
    }
    
    // Sort by complexity
    std::vector<std::pair<std::string, size_t>> sortedComplexity(
        functionComplexity.begin(), functionComplexity.end());
    std::sort(sortedComplexity.begin(), sortedComplexity.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    oss << "Function Complexity Ranking:\n";
    for (size_t i = 0; i < std::min(sortedComplexity.size(), size_t(10)); ++i) {
        const auto& [func, complexity] = sortedComplexity[i];
        double avgTime = avgTimes[func];
        size_t callCount = getCallCount(func);
        
        oss << i + 1 << ". " << func << "\n";
        oss << "   Complexity Score: " << complexity << "\n";
        oss << "   Average Time: " << avgTime << "s\n";
        oss << "   Call Count: " << callCount << "\n";
        
        if (complexity > 1000) {
            oss << "   *** HIGH COMPLEXITY ***\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string FunctionCallProfiler::analyzeCallPatterns() const {
    std::ostringstream oss;
    oss << "Function Call Pattern Analysis:\n";
    oss << "============================\n\n";
    
    std::lock_guard<std::mutex> lock(callsMutex_);
    
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> callGraph;
    
    // Build call graph
    for (const auto& call : calls_) {
        if (!call.callStack.empty()) {
            std::string caller = call.callStack.back();
            std::string callee = call.functionName;
            callGraph[caller][callee]++;
        }
    }
    
    oss << "Call Graph (Top Functions):\n";
    std::vector<std::pair<std::string, size_t>> functionStats;
    
    for (const auto& [caller, callees] : callGraph) {
        size_t totalCalls = 0;
        for (const auto& [callee, count] : callees) {
            totalCalls += count;
        }
        functionStats.emplace_back(caller, totalCalls);
    }
    
    std::sort(functionStats.begin(), functionStats.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (size_t i = 0; i < std::min(functionStats.size(), size_t(10)); ++i) {
        const auto& [func, count] = functionStats[i];
        oss << i + 1 << ". " << func << ": " << count << " calls\n";
    }
    
    oss << "\nMost Called Functions:\n";
    std::vector<std::pair<std::string, size_t>> mostCalled;
    for (const auto& [func, callees] : callGraph) {
        size_t totalCalled = 0;
        for (const auto& [callee, count] : callees) {
            totalCalled += count;
        }
        mostCalled.emplace_back(func, totalCalled);
    }
    
    std::sort(mostCalled.begin(), mostCalled.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (size_t i = 0; i < std::min(mostCalled.size(), size_t(10)); ++i) {
        const auto& [func, count] = mostCalled[i];
        oss << i + 1 << ". " << func << ": " << count << " calls\n";
    }
    
    return oss.str();
}

std::string FunctionCallProfiler::identifySlowFunctions() const {
    std::ostringstream oss;
    oss << "Slow Function Identification:\n";
    oss << "==========================\n\n";
    
    auto functionCalls = getFunctionCalls();
    
    oss << "Functions with High Average Call Times:\n";
    for (const auto& func : functionCalls) {
        double avgTime = getAverageTime(func);
        size_t count = getCallCount(func);
        
        if (count >= minSamples_ && avgTime > profilingInterval_ * 5) {
            oss << "  " << func << " (avg: " << avgTime << "s, calls: " << count << ")\n";
            
            // Suggest optimizations
            if (avgTime > profilingInterval_ * 10) {
                oss << "    *** CRITICAL - Optimization needed ***\n";
            } else if (avgTime > profilingInterval_ * 7) {
                oss << "    *** HIGH PRIORITY - Optimization recommended ***\n";
            } else {
                oss << "    *** MEDIUM PRIORITY - Consider optimization ***\n";
            }
        }
    }
    
    oss << "\nFunctions with High Variance:\n";
    for (const auto& func : functionCalls) {
        double stddev = getStandardDeviation(func);
        double avgTime = getAverageTime(func);
        size_t count = getCallCount(func);
        
        if (count >= minSamples_ && avgTime > 0 && (stddev / avgTime) > 0.5) {
            oss << "  " << func << " (avg: " << avgTime << "s, stddev: " << stddev << "s, cv: " << (stddev / avgTime) << ")\n";
            oss << "    *** HIGH VARIANCE - Inconsistent performance ***\n";
        }
    }
    
    return oss.str();
}

bool FunctionCallProfiler::exportProfile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportProfileAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string FunctionCallProfiler::exportProfileAsString() const {
    std::ostringstream oss;
    oss << "Function Call Profile\n";
    oss << "=====================\n\n";
    
    oss << "Total Function Calls: " << totalCalls_ << "\n";
    oss << "Active Functions: " << activeFunctions_.size() << "\n\n";
    
    auto functionCalls = getFunctionCalls();
    
    oss << "Function Statistics:\n";
    for (const auto& func : functionCalls) {
        size_t count = getCallCount(func);
        double avgTime = getAverageTime(func);
        double minTime = getMinTime(func);
        double maxTime = getMaxTime(func);
        double stddev = getStandardDeviation(func);
        
        oss << "  " << func << ":\n";
        oss << "    Calls: " << count << "\n";
        oss << "    Average time: " << avgTime << "s\n";
        oss << "    Min time: " << minTime << "s\n";
        oss << "    Max time: " << maxTime << "s\n";
        oss << "    Std Dev: " << stddev << "s\n";
        oss << "    Coefficient of Variation: " << (avgTime > 0 ? (stddev / avgTime) : 0) << "\n";
        oss << "\n";
    }
    
    return oss.str();
}

void FunctionCallProfiler::cleanupOldCalls() {
    // Remove calls older than 1 hour
    auto now = std::chrono::system_clock::now();
    auto oneHourAgo = now - std::chrono::hours(1);
    
    std::lock_guard<std::mutex> lock(callsMutex_);
    auto it = std::remove_if(calls_.begin(), calls_.end(),
                             [oneHourAgo](const FunctionCall& call) {
                                 return call.timestamp < oneHourAgo;
                             });
    calls_.erase(it, calls_.end());
}

double FunctionCallProfiler::calculateCallComplexity(const FunctionCall& call) const {
    double timeScore = call.time * 1000; // Scale to milliseconds
    size_t stackScore = call.callStack.size() * 10;
    return timeScore + stackScore;
}

std::string FunctionCallProfiler::getCallPattern(const FunctionCall& call) const {
    if (call.callStack.empty()) {
        return "direct";
    }
    
    std::string caller = call.callStack.back();
    if (caller == "main" || caller == "__init__") {
        return "top-level";
    } else if (caller.find('.') != std::string::npos) {
        return "nested";
    } else {
        return "recursive";
    }
}

bool FunctionCallProfiler::isSlowFunction(const std::string& functionName, double avgTime) const {
    return avgTime > profilingInterval_ * 10;
}

} // namespace nlm
