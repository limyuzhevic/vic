#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_set>
#include <memory>

namespace nlm {

// FunctionCallProfiler: Track individual function execution times
class FunctionCallProfiler {
public:
    FunctionCallProfiler();
    ~FunctionCallProfiler();
    
    // Configuration
    void setProfilingInterval(double interval) { profilingInterval_ = interval; }
    void setMinSamples(size_t minSamples) { minSamples_ = minSamples; }
    void setEnableCallStack(bool enable) { enableCallStack_ = enable; }
    void setMaxFunctionDepth(size_t depth) { maxFunctionDepth_ = depth; }
    
    // Function call tracking
    void recordFunctionCall(const std::string& functionName, double time, 
                           const std::string& context = "",
                           const std::string& file = "", int line = 0,
                           const std::vector<std::string>& callStack = {});
    
    // Query results
    double getAverageTime(const std::string& functionName) const;
    double getMinTime(const std::string& functionName) const;
    double getMaxTime(const std::string& functionName) const;
    double getStandardDeviation(const std::string& functionName) const;
    
    // Function listing
    std::vector<std::string> getFunctionCalls() const;
    size_t getCallCount(const std::string& functionName) const;
    
    // Analysis
    std::string analyzeFunctionComplexity() const;
    std::string analyzeCallPatterns() const;
    std::string identifySlowFunctions() const;
    
    // Statistics
    size_t getTotalCalls() const { return totalCalls_; }
    size_t getActiveFunctions() const { return activeFunctions_.size(); }
    
    // Export
    bool exportProfile(const std::string& filepath) const;
    std::string exportProfileAsString() const;
    
private:
    struct FunctionCall {
        std::chrono::system_clock::time_point timestamp;
        std::string functionName;
        double time;
        std::string context;
        std::string file;
        int line;
        std::vector<std::string> callStack;
        size_t recursionDepth;
        
        FunctionCall(const std::string& name, double time, const std::string& ctx,
                    const std::string& file, int line,
                    const std::vector<std::string>& stack, size_t depth)
            : functionName(name), time(time), context(ctx), file(file), line(line),
              callStack(stack), recursionDepth(depth) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    std::vector<FunctionCall> calls_;
    mutable std::mutex callsMutex_;
    
    std::unordered_map<std::string, std::unordered_set<std::string>> activeFunctions_;
    mutable std::mutex activeMutex_;
    
    double profilingInterval_;
    bool enableCallStack_;
    size_t minSamples_;
    size_t maxFunctionDepth_;
    size_t totalCalls_;
    
    // Helper methods
    void cleanupOldCalls();
    double calculateCallComplexity(const FunctionCall& call) const;
    std::string getCallPattern(const FunctionCall& call) const;
    bool isSlowFunction(const std::string& functionName, double avgTime) const;
};

} // namespace nlm
