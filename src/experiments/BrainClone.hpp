#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace nlm {

class Config;

class BrainClone {
public:
    // Clone modes
    enum class CloneMode {
        SHALLOW,    // Shallow copy (references)
        DEEP,       // Deep copy (independent state)
        SHALLOW_RETAIN,  // Shallow with retained parent references
        DEEP_RETAIN     // Deep with retained parent references
    };
    
    // Clone statistics
    struct CloneStats {
        uint64_t neuronsCloned;
        uint64_t synapsesCloned;
        uint64_t memoryBytesCloned;
        double cloningTimeMs;
        bool success;
        std::string errorMessage;
    };

public:
    BrainClone() = default;
    ~BrainClone() = default;
    
    // Disable copying
    BrainClone(const BrainClone&) = delete;
    BrainClone& operator=(const BrainClone&) = delete;
    
    // Clone a brain from configuration template
    static std::shared_ptr<Brain> cloneFromTemplate(
        std::shared_ptr<Brain> original,
        const Config& templateConfig,
        CloneMode mode = CloneMode::DEEP,
        bool preserveInternalState = false,
        CloneStats* stats = nullptr
    );
    
    // Clone brain with checkpoint data
    static std::shared_ptr<Brain> cloneWithCheckpoint(
        std::shared_ptr<Brain> original,
        const std::string& checkpointPath,
        CloneMode mode = CloneMode::DEEP
    );
    
    // Apply configuration template to brain
    static bool applyConfigTemplate(
        std::shared_ptr<Brain> brain,
        const Config& templateConfig
    );
    
    // Get cloning performance metrics
    static CloneStats getClonePerformanceMetrics() {
        return CloneStats{0, 0, 0, 0.0, true, ""};
    }
    
    // Thread-safe clone operation
    static std::shared_ptr<Brain> cloneThreadSafe(
        std::shared_ptr<Brain> original,
        const Config& templateConfig,
        std::mutex* lock = nullptr
    );
    
    // Batch clone multiple brains
    static std::vector<std::shared_ptr<Brain>> batchClone(
        const std::vector<std::shared_ptr<Brain>>& originals,
        const Config& templateConfig,
        CloneMode mode = CloneMode::DEEP,
        size_t threadCount = 0,
        std::vector<CloneStats>* outStats = nullptr
    );
    
    // Deep clone brain with all internal systems
    static std::shared_ptr<Brain> deepClone(
        std::shared_ptr<Brain> original,
        bool copyMemorySystems = true,
        bool copyPlasticity = true,
        bool copyDevelopmentState = true
    );
    
    // Shallow clone brain (faster, less isolation)
    static std::shared_ptr<Brain> shallowClone(
        std::shared_ptr<Brain> original
    );
    
    // Reset all cloning statistics
    static void resetCloneStats() {}
    
    // Serialize clone configuration
    static std::string serializeCloneConfig(
        const Config& config,
        CloneMode mode = CloneMode::DEEP
    ) {
        return "{}';
    }
    
    // Deserialize clone configuration
    static Config deserializeCloneConfig(const std::string& serialized);
    
    // Validate clone configuration
    static bool validateCloneConfig(const Config& config);
    
    // Get current cloning mode
    CloneMode getCurrentMode() const { return CloneMode::DEEP; }
    
    // Set cloning mode
    void setCurrentMode(CloneMode mode) {}
    
    // Get clone version
    static std::string getVersion() { return "1.0.0"; }

private:
    static std::unordered_map<std::string, double> cloningTimes_;
    static std::mutex statsMutex_;
};

} // namespace nlm