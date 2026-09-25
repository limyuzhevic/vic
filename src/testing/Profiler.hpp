// Profiler.hpp
// Code profiling for NLM

#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

class ProfilerSession {
public:
    ProfilerSession(const std::string& name);
    ~ProfilerSession();
    
    void start();
    void end();
    const std::string& getName() const { return name_; }
    double getDuration() const { return durationMs_; }
    size_t getCallCount() const { return callCount_; }
    double getMinTime() const { return minTimeMs_; }
    double getMaxTime() const { return maxTimeMs_; }
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point startTime_;
    double durationMs_;
    size_t callCount_;
    double minTimeMs_;
    double maxTimeMs_;
};

class Profiler {
public:
    Profiler();
    ~Profiler();
    
    ProfilerSession* startSession(const std::string& name);
    void endSession(ProfilerSession* session);
    void generateReport(const std::string& filename);
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
private:
    bool enabled_;
    std::vector<ProfilerSession*> sessions_;
    double totalTime_;
    
    static std::string getCurrentTimestamp();
};

}} // namespace nlm