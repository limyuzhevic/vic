// Memory persistence - memory system persistence
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <mutex>
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/SemanticMemory.hpp"
#include "../memory/ProceduralMemory.hpp"
#include "../memory/AssociativeMemory.hpp"

namespace nlm {

// Memory system types
enum class MemorySystemType {
    Working,
    Episodic,
    Semantic,
    Procedural,
    Associative
};

// Memory persistence result
struct MemoryPersistenceResult {
    bool success;
    std::string message;
    uint64_t totalItems;
    uint64_t totalSize;
    double elapsedTime;
    time_t timestamp;
    
    MemoryPersistenceResult() : success(false), totalItems(0), totalSize(0), 
                                 elapsedTime(0.0), timestamp(0) {}
};

// Memory persistence options
struct MemoryPersistenceOptions {
    std::vector<MemorySystemType> systemsToPersist;
    bool compress;
    bool encrypt;
    std::string outputPath;
    std::string format; // json, binary, xml
    bool validateAfterSave;
    std::chrono::seconds timeout;
    
    MemoryPersistenceOptions() : compress(true), encrypt(false), 
                                 format("binary"), validateAfterSave(true),
                                 timeout(std::chrono::seconds(30)) {
        systemsToPersist = {
            MemorySystemType::Working,
            MemorySystemType::Episodic,
            MemorySystemType::Semantic,
            MemorySystemType::Procedural,
            MemorySystemType::Associative
        };
    }
};

// Memory persistence statistics
struct MemoryPersistenceStats {
    uint64_t workingMemorySaved;
    uint64_t episodicMemorySaved;
    uint64_t semanticMemorySaved;
    uint64_t proceduralMemorySaved;
    uint64_t associativeMemorySaved;
    uint64_t totalItemsSaved;
    uint64_t totalSizeSaved;
    double totalTimeSpent;
    uint32_t errorCount;
    
    MemoryPersistenceStats() : workingMemorySaved(0), episodicMemorySaved(0),
                               semanticMemorySaved(0), proceduralMemorySaved(0),
                               associativeMemorySaved(0), totalItemsSaved(0),
                               totalSizeSaved(0), totalTimeSpent(0.0), errorCount(0) {}
};

// Memory persistence status
enum class MemoryPersistenceStatus {
    Inactive,
    Initializing,
    Active,
    Saving,
    Loading,
    Error,
    Shutdown
};

// Main memory persistence class
class MemoryPersistence {
public:
    MemoryPersistence();
    ~MemoryPersistence();
    
    // Configuration
    bool configure(const std::shared_ptr<Config>& config);
    void shutdown();
    
    // Persistence operations
    bool saveWorkingMemory(const std::string& name, const MemoryPersistenceOptions& options = MemoryPersistenceOptions());
    bool saveEpisodicMemory(const std::string& name, const MemoryPersistenceOptions& options = MemoryPersistenceOptions());
    bool saveSemanticMemory(const std::string& name, const MemoryPersistenceOptions& options = MemoryPersistenceOptions());
    bool saveProceduralMemory(const std::string& name, const MemoryPersistenceOptions& options = MemoryPersistenceOptions());
    bool saveAssociativeMemory(const std::string& name, const MemoryPersistenceOptions& options = MemoryPersistenceOptions());
    
    // Load operations
    bool loadWorkingMemory(const std::string& name);
    bool loadEpisodicMemory(const std::string& name);
    bool loadSemanticMemory(const std::string& name);
    bool loadProceduralMemory(const std::string& name);
    bool loadAssociativeMemory(const std::string& name);
    
    // Bulk operations
    bool saveAllMemories(const std::string& name, const MemoryPersistenceOptions& options = MemoryPersistenceOptions());
    bool loadAllMemories(const std::string& name);
    bool deleteMemories(const std::string& name);
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory() const { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() const { return episodicMemory_.get(); }
    SemanticMemory* getSemanticMemory() const { return semanticMemory_.get(); }
    ProceduralMemory* getProceduralMemory() const { return proceduralMemory_.get(); }
    AssociativeMemory* getAssociativeMemory() const { return associativeMemory_.get(); }
    
    // Memory system integration
    void setWorkingMemory(std::unique_ptr<NeuralWorkingMemory> memory) {
        workingMemory_ = std::move(memory);
    }
    
    void setEpisodicMemory(std::unique_ptr<NeuralEpisodicMemory> memory) {
        episodicMemory_ = std::move(memory);
    }
    
    void setSemanticMemory(std::unique_ptr<SemanticMemory> memory) {
        semanticMemory_ = std::move(memory);
    }
    
    void setProceduralMemory(std::unique_ptr<ProceduralMemory> memory) {
        proceduralMemory_ = std::move(memory);
    }
    
    void setAssociativeMemory(std::unique_ptr<AssociativeMemory> memory) {
        associativeMemory_ = std::move(memory);
    }
    
    // Statistics and monitoring
    MemoryPersistenceStats getStats() const { return stats_; }
    
    bool isHealthy() const { return errorCount_ == 0; }
    
    // Event handling
    using MemoryEventCallback = std::function<void(MemorySystemType, const std::string&, const MemoryPersistenceResult&)>;
    void setMemoryEventCallback(MemoryEventCallback callback) { 
        memoryEventCallback_ = callback; 
    }
    
    // Validation
    bool validateConfiguration() const;
    bool validateMemoryIntegrity() const;
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    bool saveMemorySystem(MemorySystemType type, const std::string& name, 
                         const MemoryPersistenceOptions& options);
    bool loadMemorySystem(MemorySystemType type, const std::string& name);
    
    MemoryPersistenceResult executeMemoryOperation(MemorySystemType type,
                                                     const std::string& operation,
                                                     const std::string& name,
                                                     const MemoryPersistenceOptions& options);
    
    void updateStats(const MemoryPersistenceResult& result, MemorySystemType type);
    void logEvent(MemorySystemType type, const std::string& operation,
                  const std::string& name, const MemoryPersistenceResult& result);
    
    std::string getMemoryPath(MemorySystemType type, const std::string& name) const;
    
    // Component references
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<SemanticMemory> semanticMemory_;
    std::unique_ptr<ProceduralMemory> proceduralMemory_;
    std::unique_ptr<AssociativeMemory> associativeMemory_;
    
    // Configuration
    std::shared_ptr<Config> config_;
    
    // State
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    MemoryPersistenceStats stats_;
    
    // Events
    MemoryEventCallback memoryEventCallback_;
    
    // Validation
    std::vector<std::string> validationErrors_;
};

} // namespace nlm
