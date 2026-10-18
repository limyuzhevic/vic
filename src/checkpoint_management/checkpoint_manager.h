#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <future>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <random>
#include <functional>
#include <zlib.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Checkpoint metadata
struct CheckpointMetadata {
    std::string id;
    SimulationStep step;
    std::string description;
    SimulationStep creationTime;
    SimulationStep lastAccessTime;
    size_t size;
    bool isCompressed;
    bool isEncrypted;
    std::string compressionAlgorithm;
    float integrityHash;
    std::unordered_map<std::string, std::string> metadataTags;
    std::vector<std::string> dependentCheckpoints;
    std::vector<std::string> requiredCheckpoints;
    
    CheckpointMetadata(const std::string& id, SimulationStep step, const std::string& desc = "")
        : id(id), step(step), description(desc), creationTime(getCurrentStep()),
          lastAccessTime(getCurrentStep()), size(0), isCompressed(false), 
          isEncrypted(false), integrityHash(0.0f) {}
};

// Differential checkpoint entry
struct DifferentialEntry {
    std::string targetCheckpointId;
    std::string sourceCheckpointId;
    std::vector<std::pair<std::string, std::string>> changes;
    SimulationStep creationTime;
    size_t size;
    bool isCompressed;
    
    DifferentialEntry(const std::string& target, const std::string& source)
        : targetCheckpointId(target), sourceCheckpointId(source), creationTime(getCurrentStep()),
          size(0), isCompressed(false) {}
};

// Parallel checkpoint operation
struct CheckpointOperation {
    std::string id;
    std::string type; // "create", "restore", "validate"
    std::string targetId;
    std::string baseId;
    std::promise<bool> promise;
    std::future<bool> future;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    CheckpointOperation(const std::string& id, const std::string& type, const std::string& target)
        : id(id), type(type), targetId(target), startTime(std::chrono::system_clock::now()) {
        future = promise.get_future();
    }
};

// Compression algorithms
enum class CompressionAlgorithm {
    None,
    Gzip,
    LZ4,
    Zstd,
    Brotli
};

// Encryption algorithms  
enum class EncryptionAlgorithm {
    None,
    AES128,
    AES256,
    ChaCha20
};

// Checkpoint validation result
struct ValidationResult {
    bool isValid;
    std::string validationReport;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    float integrityScore;
    
    ValidationResult() : isValid(true), integrityScore(1.0f) {}
};

// Forward declarations
class MemoryManager;

class CheckpointManager {
public:
    CheckpointManager();
    ~CheckpointManager();
    
    // Checkpoint Configuration
    void setCheckpointDirectory(const std::string& directory);
    void setCheckpointFrequency(size_t frequency);
    void setCheckpointCompression(bool enabled);
    void setCheckpointEncryption(bool enabled);
    void setMaxCheckpointSize(size_t size);
    
    // Checkpoint Operations
    std::string createCheckpoint(const std::string& description = "");
    bool restoreCheckpoint(const std::string& checkpointId);
    bool deleteCheckpoint(const std::string& checkpointId);
    std::vector<std::string> getCheckpointIds() const;
    CheckpointMetadata getCheckpointInfo(const std::string& checkpointId) const;
    
    // Incremental Checkpointing
    std::string createIncrementalCheckpoint(const std::string& baseCheckpointId);
    bool applyIncrementalCheckpoint(const std::string& checkpointId);
    
    // Differential Checkpointing
    std::string createDifferentialCheckpoint(const std::string& baseCheckpointId);
    bool applyDifferentialCheckpoint(const std::string& checkpointId);
    
    // Checkpoint Validation
    bool validateCheckpoint(const std::string& checkpointId) const;
    bool validateAllCheckpoints() const;
    std::string getCheckpointValidationReport() const;
    ValidationResult validateCheckpointDetails(const std::string& checkpointId) const;
    
    // Parallel Operations
    void enableParallelCheckpoints(bool enable);
    bool areParallelCheckpointsEnabled() const;
    std::vector<CheckpointOperation> getActiveOperations() const;
    
    // Compression and Encryption
    void setCompressionAlgorithm(CompressionAlgorithm algorithm);
    void setEncryptionAlgorithm(EncryptionAlgorithm algorithm);
    void setEncryptionKey(const std::string& key);
    void setEncryptionSalt(const std::string& salt);
    
    // Compression Operations
    std::vector<char> compressData(const std::vector<char>& data, CompressionAlgorithm algorithm) const;
    std::vector<char> decompressData(const std::vector<char>& data, CompressionAlgorithm algorithm) const;
    
    // Encryption Operations
    std::vector<char> encryptData(const std::vector<char>& data) const;
    std::vector<char> decryptData(const std::vector<char>& data) const;
    
    // Validation Operations
    float calculateIntegrityHash(const std::vector<char>& data) const;
    bool verifyIntegrity(const std::vector<char>& data, float expectedHash) const;
    
    // Memory Integration
    void setMemoryManager(MemoryManager* manager) { memoryManager_ = manager; }
    
    // Statistics and Monitoring
    size_t getTotalCheckpoints() const;
    size_t getActiveCheckpoints() const;
    size_t getTotalStorageSize() const;
    std::vector<CheckpointMetadata> getAllCheckpointsInfo() const;
    std::chrono::system_clock::time_point getLastCheckpointTime() const;
    size_t getCheckpointsSince(size_t minutes) const;
    
    // Cleanup and Maintenance
    void cleanupOldCheckpoints(size_t maxCheckpoints);
    void compactCheckpoints();
    void rebuildIndex();
    
    // Batch Operations
    std::vector<std::string> createBatchCheckpoints(size_t count, const std::string& baseDescription = "");
    bool restoreBatchCheckpoints(const std::vector<std::string>& checkpointIds);
    
    // Error Handling
    std::string getLastError() const { return lastError_; }
    void clearError() { lastError_.clear(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    MemoryManager* memoryManager_ = nullptr;
    mutable std::shared_mutex checkpointMutex_;
    
    std::string lastError_;
};

} // namespace nlm
