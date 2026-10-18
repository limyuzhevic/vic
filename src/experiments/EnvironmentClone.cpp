#include "EnvironmentClone.hpp"
#include "../environment/EnvironmentState.hpp"
#include <chrono>
#include <thread>
#include <algorithm>

namespace nlm {

std::vector<double> EnvironmentClone::cloningTimes_;
std::mutex EnvironmentClone::statsMutex_;

std::shared_ptr<Environment> EnvironmentClone::cloneFromTemplate(
    std::shared_ptr<Environment> original,
    CloneMode mode,
    bool preserveState,
    CloneStats* stats
) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CloneStats localStats;
    localStats.stateSize = 0;
    localStats.cloningTimeMs = 0.0;
    localStats.success = false;
    localStats.errorMessage = "";
    
    try {
        if (!original) {
            localStats.errorMessage = "Original environment is null";
            if (stats) *stats = localStats;
            return nullptr;
        }
        
        switch (mode) {
            case CloneMode::DEEP:
            case CloneMode::CHECKPOINT:
                // Create new environment based on original type
                // In real implementation, would copy:
                // - Complete environment state
                // - Configuration parameters
                // - Internal data structures
                // - Simulation state
                
                // For now, create a new environment of same type
                // This is a simplified implementation
                if (std::string(original->getName()) == "GridWorld") {
                    // Create new GridWorldEnvironment
                    auto gridEnv = std::make_shared<GridWorldEnvironment>();
                    localStats.stateSize = sizeof(EnvironmentState);
                    localStats.success = true;
                    return gridEnv;
                } else {
                    localStats.errorMessage = "Unsupported environment type for cloning";
                    if (stats) *stats = localStats;
                    return nullptr;
                }
                break;
                
            case CloneMode::SHALLOW:
                // Shallow clone
                // In real implementation, would share some internal structures
                if (std::string(original->getName()) == "GridWorld") {
                    auto gridEnv = std::make_shared<GridWorldEnvironment>();
                    localStats.stateSize = sizeof(EnvironmentState) / 2; // Approximate
                    localStats.success = true;
                    return gridEnv;
                } else {
                    localStats.errorMessage = "Unsupported environment type for cloning";
                    if (stats) *stats = localStats;
                    return nullptr;
                }
                break;
        }
        
    } catch (const std::exception& e) {
        localStats.errorMessage = std::string("Exception during clone: ") + e.what();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    localStats.cloningTimeMs = duration;
    
    // Update global stats
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        cloningTimes_.push_back(duration);
    }
    
    if (stats) *stats = localStats;
    return nullptr;
}

std::shared_ptr<Environment> EnvironmentClone::cloneWithCheckpoint(
    std::shared_ptr<Environment> original,
    const std::string& checkpointPath
) {
    // In real implementation, would load from checkpoint file
    // For now, just return a clone
    if (!original) return nullptr;
    
    return cloneFromTemplate(original, CloneMode::CHECKPOINT);
}

std::shared_ptr<Environment> EnvironmentClone::cloneWithParams(
    std::shared_ptr<Environment> original,
    const std::string& paramName,
    const std::string& paramValue
) {
    if (!original) return nullptr;
    
    // In real implementation, would apply parameters to the clone
    return cloneFromTemplate(original, CloneMode::DEEP);
}

std::shared_ptr<Environment> EnvironmentClone::cloneThreadSafe(
    std::shared_ptr<Environment> original,
    CloneMode mode,
    std::mutex* lock
) {
    if (lock) {
        std::lock_guard<std::mutex> guard(*lock);
    }
    
    CloneStats stats;
    return cloneFromTemplate(original, mode, true, &stats);
}

std::vector<std::shared_ptr<Environment>> EnvironmentClone::batchClone(
    const std::vector<std::shared_ptr<Environment>>& originals,
    CloneMode mode,
    size_t threadCount,
    std::vector<CloneStats>* outStats
) {
    std::vector<std::shared_ptr<Environment>> results;
    results.reserve(originals.size());
    
    if (threadCount == 0) {
        threadCount = std::thread::hardware_concurrency();
    }
    
    std::vector<std::thread> threads;
    std::mutex resultsMutex;
    
    size_t itemsPerThread = originals.size() / threadCount;
    size_t remainder = originals.size() % threadCount;
    
    for (size_t i = 0; i < threadCount; ++i) {
        size_t start = i * itemsPerThread;
        size_t count = itemsPerThread + (i == threadCount - 1 ? remainder : 0);
        
        threads.emplace_back([&, start, count]() {
            std::vector<std::shared_ptr<Environment>> threadResults;
            std::vector<CloneStats> threadStats;
            
            for (size_t j = 0; j < count; ++j) {
                auto original = originals[start + j];
                CloneStats stats;
                auto cloned = cloneFromTemplate(original, mode, true, &stats);
                
                if (cloned) {
                    threadResults.push_back(cloned);
                    threadStats.push_back(stats);
                }
            }
            
            std::lock_guard<std::mutex> lock(resultsMutex);
            results.insert(results.end(), threadResults.begin(), threadResults.end());
            if (outStats) {
                outStats->insert(outStats->end(), threadStats.begin(), threadStats.end());
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return results;
}

std::shared_ptr<Environment> EnvironmentClone::clonePreserveState(
    std::shared_ptr<Environment> original,
    const EnvironmentState& preservedState
) {
    if (!original) return nullptr;
    
    // In real implementation, would preserve specific state
    return cloneFromTemplate(original, CloneMode::DEEP);
}

bool EnvironmentClone::validateCloneParams(const std::string& paramName, const std::string& paramValue) {
    // In real implementation, would validate parameters
    return !paramName.empty() && !paramValue.empty();
}

std::string EnvironmentClone::deserializeCloneConfig(const std::string& serialized) {
    // In real implementation, would parse serialized config
    return "{}";
}

} // namespace nlm