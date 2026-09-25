// Performance Analysis Implementation for NLM
// Performance measurement and profiling utilities

#include "core/PerformanceAnalysis.hpp"
#include <iostream>
#include <mutex>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Performance profiler implementation
PerformanceAnalysis PerformanceProfiler::analyzeProfile(const ProfileInfo& profile) const {
    PerformanceAnalysis analysis;
    analysis.profileName = profile.name;
    
    if (profile.duration.count() > 0.0) {
        analysis.totalTime = profile.duration.count();
        analysis.samplesPerSecond = profile.getSamplesPerSecond();
        analysis.minTime = profile.getMinSampleTime();
        analysis.maxTime = profile.getMaxSampleTime();
        analysis.avgTime = profile.getAverageSampleTime();
        analysis.stdDevTime = profile.getSampleTimeStdDev();
        
        if (profile.sampleTimes.size() > 1) {
            analysis.throughput = static_cast<double>(profile.sampleTimes.size() - 1) / analysis.totalTime;
        }
        
        // Calculate efficiency metrics (mock implementation)
        analysis.cpuEfficiency = std::min(100.0, analysis.samplesPerSecond * 100.0);
        analysis.memoryEfficiency = std::min(100.0, profile.peakMemory / (1024 * 1024));
        
        analysis.cpuTimePerOperation = profile.cpuTime / profile.sampleTimes.size();
        analysis.memoryPerOperation = profile.peakMemory / (profile.sampleTimes.size() > 0 ? profile.sampleTimes.size() : 1);
    }
    
    return analysis;
}

} // namespace nlm
