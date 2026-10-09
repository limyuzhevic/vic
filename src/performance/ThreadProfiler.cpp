// Thread profiler implementation

#include "ThreadProfiler.hpp"
#include <csignal>
#include <execinfo.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

ThreadProfiler::ThreadProfiler()
    : isProfiling_(false)
    , samplingInterval_(0.01)  // 10 ms default
    , stopSampling_(false)
{
    threadId_ = std::hash<std::thread::id>{}(std::this_thread::get_id());
}

ThreadProfiler::~ThreadProfiler() {
    stopProfiling();
}

void ThreadProfiler::startProfiling(const std::string& threadName) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (isProfiling_) return;
    
    isProfiling_ = true;
    threadName_ = threadName;
    stopSampling_ = false;
    
    // Start sampling thread
    samplingThread_ = std::thread(&ThreadProfiler::samplingWorker, this);
}

void ThreadProfiler::stopProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isProfiling_) return;
    
    isProfiling_ = false;
    stopSampling_ = true;
    
    if (samplingThread_.joinable()) {
        samplingThread_.join();
    }
}

void ThreadProfiler::recordFunctionEntry(const std::string& functionName,
                                       const std::string& filePath,
                                       uint32_t lineNumber,
                                       uint64_t instructionPointer) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    uint64_t timestamp = getCurrentTimestamp();
    callStack_.push_back(CallStackEntry(timestamp, instructionPointer, functionName,
                                       filePath, lineNumber, true));
}

void ThreadProfiler::recordFunctionExit(uint64_t instructionPointer) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!callStack_.empty() && !callStack_.back().isEntry) {
        return; // Not on a call stack
    }
    
    // Find the matching entry (simple stack handling)
    callStack_.push_back(CallStackEntry(getCurrentTimestamp(), instructionPointer,
                                       "", "", 0, false));
}

std::vector<ProfileFrame> ThreadProfiler::getCurrentProfile() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buildCompleteProfile();
}

double ThreadProfiler::calculateHotness(const std::string& functionName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    buildCompleteProfile();
    
    double totalSamples = 0.0;
    for (const auto& frame : completeProfile_) {
        totalSamples += frame.samples;
    }
    
    if (totalSamples == 0.0) return 0.0;
    
    double functionSamples = 0.0;
    for (const auto& frame : completeProfile_) {
        if (frame.functionName == functionName) {
            functionSamples += frame.samples;
        }
    }
    
    return functionSamples / totalSamples;
}

std::vector<std::string> ThreadProfiler::getTopHotFunctions(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    buildCompleteProfile();
    
    // Sort by samples
    std::vector<std::pair<std::string, double>> functionHotness;
    for (const auto& frame : completeProfile_) {
        functionHotness.emplace_back(frame.functionName, frame.samples);
    }
    
    std::sort(functionHotness.begin(), functionHotness.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> topFunctions;
    size_t limit = std::min(count, functionHotness.size());
    for (size_t i = 0; i < limit; ++i) {
        topFunctions.push_back(functionHotness[i].first);
    }
    
    return topFunctions;
}

std::string ThreadProfiler::generateFlameGraph(size_t maxDepth) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    buildCompleteProfile();
    
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head><title>NLM Thread Flame Graph</title></head>\n";
    html << "<body>\n";
    html << "<h1>Thread Flame Graph for " << threadName_ << "</h1>\n";
    html << "<pre style=\"font-family: monospace; font-size: 12px;\">\n";
    
    double totalSamples = 0.0;
    for (const auto& frame : completeProfile_) {
        totalSamples += frame.samples;
    }
    
    // Simple depth-first flame graph generation
    std::vector<std::pair<std::string, double>> stack;
    double currentDepth = 0.0;
    
    for (const auto& frame : completeProfile_) {
        if (frame.samples <= 0.0) continue;
        
        double depth = frame.percentage / 100.0 * 80.0;
        
        // Close tags for deeper levels
        while (stack.size() > static_cast<size_t>(currentDepth)) {
            html << std::string(2 * currentDepth, ' ') << "</div>\n";
            stack.pop_back();
            currentDepth = stack.size();
        }
        
        // Open tags for shallower levels
        while (stack.size() < static_cast<size_t>(depth)) {
            html << std::string(2 * currentDepth, ' ') << "<div class=\"flame\">\n";
            stack.push_back(std::make_pair(frame.functionName, frame.percentage));
            currentDepth = stack.size();
        }
        
        // Add function at this depth
        html << std::string(2 * currentDepth, ' ') << frame.functionName
             << " (" << std::fixed << std::setprecision(1) << frame.percentage << "%)" << std::endl;
    }
    
    // Close remaining tags
    while (!stack.empty()) {
        html << std::string(2 * currentDepth, ' ') << "</div>\n";
        stack.pop_back();
        currentDepth = stack.size();
    }
    
    html << "</pre>\n";
    html << "<style>\n";
    html << "  .flame { background-color: #ffaaaa; border: 1px solid #cc0000; }\n";
    html << "</style>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

void ThreadProfiler::samplingWorker() {
    while (!stopSampling_) {
        if (isProfiling_) {
            // Sample current stack trace
            void* callStack[32];
            int frames = backtrace(callStack, 32);
            
            char** symbols = backtrace_symbols(callStack, frames);
            if (symbols) {
                // Parse stack trace (simplified)
                for (int i = 0; i < frames; ++i) {
                    std::string symbol(symbols[i]);
                    // Extract function name from symbol
                    size_t pos = symbol.find("(_Z");  // mangled C++ function
                    if (pos != std::string::npos) {
                        size_t end = symbol.find('+', pos);
                        if (end != std::string::npos) {
                            std::string functionName = symbol.substr(pos + 3, end - pos - 3);
                            // Demangle function name (simplified)
                            recordFunctionEntry(functionName, "unknown", 0, 0);
                        }
                    }
                }
                free(symbols);
            }
        }
        
        std::this_thread::sleep_for(std::chrono::duration<double>(samplingInterval_));
    }
}

void ThreadProfiler::buildCompleteProfile() {
    completeProfile_.clear();
    
    // Count samples per function
    std::unordered_map<std::string, double> functionSamples;
    for (const auto& entry : callStack_) {
        if (entry.isEntry && !entry.functionName.empty()) {
            functionSamples[entry.functionName] += 1.0;
        }
    }
    
    // Build profile frames
    double totalSamples = std::accumulate(functionSamples.begin(), functionSamples.end(), 0.0,
                                          [](double acc, const auto& pair) { return acc + pair.second; });
    
    for (const auto& pair : functionSamples) {
        ProfileFrame frame(pair.first, "unknown", 0, 0);
        frame.samples = pair.second;
        if (totalSamples > 0.0) {
            frame.percentage = (pair.second / totalSamples) * 100.0;
        }
        completeProfile_.push_back(frame);
    }
}

uint64_t ThreadProfiler::getCurrentTimestamp() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto epoch = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
}

} // namespace nlm