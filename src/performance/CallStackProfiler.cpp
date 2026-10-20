// CallStackProfiler.cpp implementation
#include "CallStackProfiler.cpp"
#include <execinfo.h>
#include <cstring>
#include <sstream>

namespace nlm {

CallStackProfiler::CallStackProfiler() : sampling_(false), samplingInterval_(0.01), stopSampling_(false) {}
CallStackProfiler::~CallStackProfiler() {
    stopSampling();
}

void CallStackProfiler::startSampling(double intervalSeconds) {
    if (sampling_) return;
    
    sampling_ = true;
    samplingInterval_ = intervalSeconds;
    stopSampling_ = false;
    
    samplingThread_ = std::thread(&CallStackProfiler::samplingLoop, this);
}

void CallStackProfiler::stopSampling() {
    if (!sampling_) return;
    
    sampling_ = false;
    stopSampling_ = true;
    
    if (samplingThread_.joinable()) {
        samplingThread_.join();
    }
}

void CallStackProfiler::samplingLoop() {
    while (!stopSampling_) {
        std::this_thread::sleep_for(std::chrono::duration<double>(samplingInterval_));
        
        auto stack = captureStack();
        if (!stack.empty()) {
            std::lock_guard<std::mutex> lock(mutex_);
            CallStackSample sample(
                std::chrono::duration<double>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()
                ).count(),
                stack,
                std::hash<std::thread::id>{}(std::this_thread::get_id()),
                0.0 // Will be updated later
            );
            samples_.push_back(sample);
        }
    }
}

std::vector<std::string> CallStackProfiler::captureStack() const {
    const size_t maxDepth = 100;
    void* stack[maxDepth];
    
    int frames = backtrace(stack, maxDepth);
    
    char** symbols = backtrace_symbols(stack, frames);
    if (!symbols) return {};
    
    std::vector<std::string> result;
    
    // Skip our own profiling functions
    for (int i = 2; i < frames; ++i) { // Skip this function and samplingLoop
        std::string symbol = symbols[i];
        
        // Try to demangle C++ function names
        std::string functionName = symbol;
        if (symbol.find("_Z") != std::string::npos) {
            // Simple demangling for C++ functions
            size_t pos = symbol.find('(');
            if (pos != std::string::npos) {
                functionName = symbol.substr(0, pos);
            }
        }
        
        result.push_back(functionName);
    }
    
    free(symbols);
    return result;
}

std::vector<std::vector<std::string>> CallStackProfiler::getStacksForFunction(
    const std::string& functionName) const {
    std::vector<std::vector<std::string>> result;
    
    for (const auto& sample : samples_) {
        for (const auto& frame : sample.stackFrames) {
            if (frame.find(functionName) != std::string::npos) {
                result.push_back(sample.stackFrames);
                break;
            }
        }
    }
    
    return result;
}

std::vector<CallStackProfiler::StackAnalysis> CallStackProfiler::analyzeStackSamples() const {
    std::unordered_map<std::string, StackAnalysis> analysisMap;
    
    for (const auto& sample : samples_) {
        for (const auto& frame : sample.stackFrames) {
            if (analysisMap.find(frame) == analysisMap.end()) {
                analysisMap[frame].functionName = frame;
                analysisMap[frame].totalSamples = 0;
            }
            
            analysisMap[frame].totalSamples++;
        }
    }
    
    std::vector<StackAnalysis> result;
    for (auto& pair : analysisMap) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::string CallStackProfiler::generateCallGraph() const {
    std::stringstream ss;
    ss << "Call Graph\n";
    ss << "==========\n\n";
    
    auto analyses = analyzeStackSamples();
    
    // Sort by frequency
    std::sort(analyses.begin(), analyses.end(),
              [](const StackAnalysis& a, const StackAnalysis& b) {
                  return a.totalSamples > b.totalSamples;
              });
    
    ss << "Functions by call frequency:\n";
    for (const auto& analysis : analyses) {
        ss << "  " << analysis.functionName << ": " << analysis.totalSamples << " calls\n";
    }
    
    return ss.str();
}

bool CallStackProfiler::saveSamples(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    j["samples"] = json::array();
    
    for (const auto& sample : samples_) {
        json jSample;
        jSample["timestamp"] = sample.timestamp;
        jSample["threadId"] = sample.threadId;
        jSample["cpuTime"] = sample.cpuTime;
        jSample["stackFrames"] = sample.stackFrames;
        j["samples"].push_back(jSample);
    }
    
    file << j.dump(2);
    file.close();
    return true;
}

bool CallStackProfiler::loadSamples(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        file.close();
        return false;
    }
    
    clearSamples();
    
    for (const auto& jSample : j["samples"]) {
        CallStackSample sample(
            jSample["timestamp"].get<double>(),
            jSample["stackFrames"].get<std::vector<std::string>>(),
            jSample.value("threadId", 0ULL),
            jSample.value("cpuTime", 0.0)
        );
        samples_.push_back(sample);
    }
    
    file.close();
    return true;
}

void CallStackProfiler::clearSamples() {
    std::lock_guard<std::mutex> lock(mutex_);
    samples_.clear();
}

} // namespace nlm
