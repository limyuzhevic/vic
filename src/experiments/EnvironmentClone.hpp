#pragma once

#include "../environment/Environment.hpp"
#include <memory>
#include <string>
#include <mutex>
#include <vector>
#include <chrono>

namespace nlm {

class EnvironmentClone {
public:
    // Clone modes
    enum class CloneMode {
        SHALLOW,
        DEEP,
        CHECKPOINT
    };
    
    // Clone statistics
    struct CloneStats {
        uint64_t stateSize;
        double cloningTimeMs;
        bool success;
        std::string errorMessage;
    };

public:
    EnvironmentClone() = default;
    ~EnvironmentClone() = default;
    
    // Disable copying
    EnvironmentClone(const EnvironmentClone&) = delete;
    EnvironmentClone& operator=(const EnvironmentClone&) = delete;
    
    // Clone environment from template
    static std::shared_ptr<Environment> cloneFromTemplate(
        std::shared_ptr<Environment> original,
        CloneMode mode = CloneMode::DEEP,
        bool preserveState = true,
        CloneStats* stats = nullptr
    );
    
    // Clone environment with checkpoint
    static std::shared_ptr<Environment> cloneWithCheckpoint(
        std::shared_ptr<Environment> original,
        const std::string& checkpointPath
    );
    
    // Clone environment with specific parameters
    static std::shared_ptr<Environment> cloneWithParams(
        std::shared_ptr<Environment> original,
        const std::string& paramName,
        const std::string& paramValue
    );
    
    // Get clone performance metrics
    static CloneStats getClonePerformanceMetrics() {
        return CloneStats{0, 0.0, true, ""};
    }
    
    // Thread-safe clone operation
    static std::shared_ptr<Environment> cloneThreadSafe(
        std::shared_ptr<Environment> original,
        CloneMode mode = CloneMode::DEEP,
        std::mutex* lock = nullptr
    );
    
    // Batch clone multiple environments
    static std::vector<std::shared_ptr<Environment>> batchClone(
        const std::vector<std::shared_ptr<Environment>>& originals,
        CloneMode mode = CloneMode::DEEP,
        size_t threadCount = 0,
        std::vector<CloneStats>* outStats = nullptr
    );
    
    // Clone environment with state preservation
    static std::shared_ptr<Environment> clonePreserveState(
        std::shared_ptr<Environment> original,
        const EnvironmentState& preservedState
    );
    
    // Reset all cloning statistics
    static void resetCloneStats() {}
    
    // Validate clone parameters
    static bool validateCloneParams(const std::string& paramName, const std::string& paramValue);
    
    // Serialize clone configuration
    static std::string serializeCloneConfig(const std::string& mode = "DEEP") {
        return "{}';
    }
    
    // Deserialize clone configuration
    static std::string deserializeCloneConfig(const std::string& serialized);
    
    // Get current clone version
    static std::string getVersion() { return "1.0.0"; }

private:
    static std::vector<double> cloningTimes_;
    static std::mutex statsMutex_;
};

} // namespace nlm