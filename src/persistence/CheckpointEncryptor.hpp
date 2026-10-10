// Checkpoint encryptor - checkpoint encryption
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <chrono>
#include <functional>
#include "CheckpointSystem.hpp"

namespace nlm {

// Encryption result
struct EncryptionResult {
    bool success;
    std::string message;
    uint64_t originalSize;
    uint64_t encryptedSize;
    std::string algorithm;
    std::string keyId;
    time_t timestamp;
    
    EncryptionResult() : success(false), originalSize(0), encryptedSize(0), timestamp(0) {}
};

// Encryption statistics
struct EncryptionStats {
    uint64_t totalOperations;
    uint64_t successfulOperations;
    uint64_t failedOperations;
    uint64_t totalEncryptedSize;
    uint32_t errorCount;
    
    EncryptionStats() : totalOperations(0), successfulOperations(0), failedOperations(0),
                       totalEncryptedSize(0), errorCount(0) {}
};

// Main checkpoint encryptor
class CheckpointEncryptor {
public:
    CheckpointEncryptor();
    ~CheckpointEncryptor();
    
    // Encryption operations
    bool encrypt(const void* data, uint64_t dataSize, std::vector<uint8_t>& encryptedData,
                 EncryptionAlgorithm algorithm = EncryptionAlgorithm::AES256);
    bool encryptFile(const std::string& inputPath, const std::string& outputPath,
                    EncryptionAlgorithm algorithm = EncryptionAlgorithm::AES256);
    
    // Decryption operations
    bool decrypt(const void* encryptedData, uint64_t encryptedSize,
                  std::vector<uint8_t>& decryptedData);
    bool decryptFile(const std::string& inputPath, const std::string& outputPath);
    
    // Encryption management
    bool setEncryptionAlgorithm(EncryptionAlgorithm algorithm);
    EncryptionAlgorithm getEncryptionAlgorithm() const { return algorithm_; }
    bool setKey(const std::string& keyId, const std::vector<uint8_t>& key);
    bool removeKey(const std::string& keyId);
    
    // Key management
    bool generateKey(const std::string& keyId, EncryptionAlgorithm algorithm);
    bool importKey(const std::string& keyId, const std::vector<uint8_t>& keyData);
    std::vector<uint8_t> exportKey(const std::string& keyId);
    
    // Statistics and monitoring
    EncryptionStats getStats() const { return stats_; }
    
    bool isHealthy() const { return errorCount_ == 0; }
    
    // Configuration
    void configure(const std::string& keyDir, const std::shared_ptr<Config>& config);
    
    // Event handling
    using EncryptionEventCallback = std::function<void(const std::string&, const EncryptionResult&)>;
    void setEncryptionEventCallback(EncryptionEventCallback callback) { 
        encryptionEventCallback_ = callback; 
    }
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    EncryptionResult executeEncryptionOperation(const std::string& operation,
                                                const std::string& sourcePath,
                                                const std::string& targetPath,
                                                EncryptionAlgorithm algorithm);
    
    void updateStats(const EncryptionResult& result);
    void logEvent(const std::string& operation, const std::string& sourcePath,
                  const EncryptionResult& result);
    
    // Encryption implementations
    bool encryptWithAES(const void* data, uint64_t dataSize, std::vector<uint8_t>& encryptedData,
                       const std::vector<uint8_t>& key);
    bool encryptWithChaCha20(const void* data, uint64_t dataSize, std::vector<uint8_t>& encryptedData,
                            const std::vector<uint8_t>& key);
    
    bool decryptWithAES(const void* encryptedData, uint64_t encryptedSize,
                        std::vector<uint8_t>& decryptedData, const std::vector<uint8_t>& key);
    bool decryptWithChaCha20(const void* encryptedData, uint64_t encryptedSize,
                            std::vector<uint8_t>& decryptedData, const std::vector<uint8_t>& key);
    
    // Components
    std::unique_ptr<CheckpointReader> checkpointReader_;
    std::unique_ptr<CheckpointWriter> checkpointWriter_;
    
    // Key management
    std::unordered_map<std::string, std::pair<EncryptionAlgorithm, std::vector<uint8_t>>> keys_;
    
    // Configuration
    std::string keyDir_;
    std::shared_ptr<Config> config_;
    EncryptionAlgorithm algorithm_;
    
    // State
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    EncryptionStats stats_;
    
    // Events
    EncryptionEventCallback encryptionEventCallback_;
};

} // namespace nlm
