// Profiling Base Classes
//
// This file provides base classes for different types of profilers.

#pragma once

#include "ProfilingFramework.hpp"
#include <map>
#include <vector>
#include <string>
#include <functional>

namespace nlm {

class ProfilerBase {
public:
    virtual ~ProfilerBase() = default;
    
    virtual bool initialize(const Config& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isAvailable() const = 0;
    virtual std::string getName() const = 0;
    
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
    virtual void recordSample(const std::string& sampleName, 
                             const std::map<std::string, double>& metrics,
                             const std::map<std::string, std::string>& metadata = {}) = 0;
    
    virtual std::vector<std::shared_ptr<ProfileData>> getSamples() const = 0;
    virtual std::vector<std::shared_ptr<ProfileData>> getSamplesByTag(const std::string& tag) const = 0;
    
    virtual void addTag(const std::string& tag) = 0;
    virtual void removeTag(const std::string& tag) = 0;
    virtual std::vector<std::string> getTags() const = 0;
    
    virtual double getTotalDuration() const = 0;
    virtual size_t getSampleCount() const = 0;
};

class CPUProfiler : public ProfilerBase {
public:
    virtual ~CPUProfiler() = default;
    
    virtual void setCPUCores(const std::vector<int>& cores) = 0;
    virtual std::vector<int> getCPUCores() const = 0;
    
    virtual void setSamplingRate(double rate) = 0;
    virtual double getSamplingRate() const = 0;
    
    virtual void enableBranchPrediction(bool enable) = 0;
    virtual bool isBranchPredictionEnabled() const = 0;
    
    virtual void enableCacheAnalysis(bool enable) = 0;
    virtual bool isCacheAnalysisEnabled() const = 0;
};

class MemoryProfiler : public ProfilerBase {
public:
    virtual ~MemoryProfiler() = default;
    
    virtual void setTrackingAllocations(bool track) = 0;
    virtual bool isTrackingAllocations() const = 0;
    
    virtual void setDetectLeaks(bool detect) = 0;
    virtual bool isLeakDetectionEnabled() const = 0;
    
    virtual void setHeapProfiling(bool profile) = 0;
    virtual bool isHeapProfilingEnabled() const = 0;
    
    virtual void setStackProfiling(bool profile) = 0;
    virtual bool isStackProfilingEnabled() const = 0;
    
    virtual void setObjectLifecycleProfiling(bool profile) = 0;
    virtual bool isObjectLifecycleProfilingEnabled() const = 0;
};

class IOProfiler : public ProfilerBase {
public:
    virtual ~IOProfiler() = default;
    
    virtual void setDiskIoprofileing(bool profile) = 0;
    virtual bool isDiskProfilingEnabled() const = 0;
    
    virtual void setNetworkIoprofileing(bool profile) = 0;
    virtual bool isNetworkProfilingEnabled() const = 0;
    
    virtual void setFileIoprofileing(bool profile) = 0;
    virtual bool isFileProfilingEnabled() const = 0;
    
    virtual void setDatabaseIoprofileing(bool profile) = 0;
    virtual bool isDatabaseProfilingEnabled() const = 0;
    
    virtual void setStreamIoprofileing(bool profile) = 0;
    virtual bool isStreamProfilingEnabled() const = 0;
};

class TimeProfiler : public ProfilerBase {
public:
    virtual ~TimeProfiler() = default;
    
    virtual void setElapsedTimeProfiling(bool profile) = 0;
    virtual bool isElapsedTimeProfilingEnabled() const = 0;
    
    virtual void setCPUTimeProfiling(bool profile) = 0;
    virtual bool isCPUTimeProfilingEnabled() const = 0;
    
    virtual void setWallClockProfiling(bool profile) = 0;
    virtual bool isWallClockProfilingEnabled() const = 0;
    
    virtual void setSystemTimeProfiling(bool profile) = 0;
    virtual bool isSystemTimeProfilingEnabled() const = 0;
    
    virtual void setUserTimeProfiling(bool profile) = 0;
    virtual bool isUserTimeProfilingEnabled() const = 0;
    
    virtual void setWaitTimeProfiling(bool profile) = 0;
    virtual bool isWaitTimeProfilingEnabled() const = 0;
    
    virtual void setLatencyProfiling(bool profile) = 0;
    virtual bool isLatencyProfilingEnabled() const = 0;
    
    virtual void setThroughputProfiling(bool profile) = 0;
    virtual bool isThroughputProfilingEnabled() const = 0;
};

} // namespace nlm
