// MemorySystemProfiler class header
// This file provides the header declaration for the MemorySystemProfiler class

#ifndef MEMORY_SYSTEM_PROFILER_HPP
#define MEMORY_SYSTEM_PROFILER_HPP

#include <string>
#include <vector>
#include <map>

// NLM includes
#include "../src/advanced_api.hpp"

/**
 * @brief Profile memory system activity
 */
class MemorySystemProfiler {
private:
    std::shared_ptr<Brain> brain_;
    std::chrono::steady_clock::time_point startTime_;
    
public:
    MemorySystemProfiler(std::shared_ptr<Brain> brain);
    
    ~MemorySystemProfiler() = default;
    
    // Memory analysis
    FloatArray getWorkingMemoryUsage() const;
    FloatArray getEpisodicMemoryUsage() const;
    FloatArray getSemanticMemoryUsage() const;
    FloatArray getProceduralMemoryUsage() const;
    
    // Activity patterns
    std::map<std::string, FloatArray> getMemoryAccessPatterns() const;
    std::map<std::string, FloatArray> getMemoryRetrievalDynamics() const;
    FloatArray computeMemoryStability() const;
    
    // Performance metrics
    double getMemoryRetrievalSpeed() const;
    double getMemoryStorageCapacity() const;
    double getMemoryEfficiency() const;
    
    // Export
    void exportMemoryProfile(const std::string& filepath) const;
    std::string getMemorySummary() const;
};

#endif // MEMORY_SYSTEM_PROFILER_HPP
