// RemoteStorage.hpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <filesystem>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <iomanip>

namespace nlm {

/**
 * Remote storage system for distributed checkpointing
 * 
 * Features:
 * - S3-compatible storage (S3, GCS, Azure Blob Storage)
 * - NFS support for network file systems
 * - Distributed file systems (HDFS, Ceph, etc.)
 * - Cloud storage integration
 * - Local fallback storage
 * - Cache management for remote storage
 * - Network resilience and error handling
 * - Bandwidth throttling and compression
 * - Storage quota management
 * - Storage lifecycle management
 * 
 * Storage Types:
 * - Local: Local file system
 * - S3: Amazon S3 compatible
 * - GCS: Google Cloud Storage
 * - Azure: Microsoft Azure Blob Storage
 * - NFS: Network File System
 * - HDFS: Hadoop Distributed File System
 * - Custom: Custom storage backend
 */

enum class StorageType {
    Local,
    S3,
    GCS,
    Azure,
    NFS,
    HDFS,
    Custom
};

enum class StorageAccessMode {
    ReadOnly,
    WriteOnly,
    ReadWrite,
    ReadWriteAppend
};

enum class StorageError {
    Success,
    ConnectionError,
    AuthenticationError,
    AuthorizationError,
    NotFound,
    AlreadyExists,
    QuotaExceeded,
    NetworkError,
    Timeout,
    NotImplemented,
    Unknown
};

struct StorageMetadata {
    std::string uri;                    // Storage URI
    StorageType type;                   // Storage type
    StorageAccessMode accessMode;       // Access mode
    uint64_t maxStorage{0};             // Maximum storage capacity (bytes)
    uint64_t usedStorage{0};            // Used storage (bytes)
    uint64_t objectCount{0};            // Number of objects
    time_t lastModified{0};             // Last modification time
    std::string region;                 // Geographic region
    std::string endpoint;               // Storage endpoint
    std::string bucket;                 // Storage bucket
    std::string prefix;                 // Object prefix
    std::map<std::string, std::string> credentials; // Authentication credentials
    std::map<std::string, std::string> config;     // Configuration options
    bool encryptionEnabled{false};      // Is encryption enabled
    bool versioningEnabled{false};      // Is versioning enabled
    std::string defaultStorageClass;   // Default storage class
    double bandwidthLimit{0.0};        // Bandwidth limit (bytes/second)
    double transferRate{0.0};          // Transfer rate (bytes/second)
    
    StorageMetadata() : type(StorageType::Local), accessMode(StorageAccessMode::ReadWrite),
                       maxStorage(0), usedStorage(0), objectCount(0), lastModified(0) {}
};

struct StorageObject {
    std::string key;                    // Object key
    uint64_t size{0};                   // Object size (bytes)
    std::string etag;                   // Entity tag
    std::string contentType;            // Content type
    time_t lastModified{0};             // Last modification time
    std::string storageClass;           // Storage class
    std::map<std::string, std::string> metadata; // Object metadata
    bool isEncrypted{false};           // Is encrypted
    bool isVersioned{false};           // Is versioned
    std::string versionId;             // Version ID
    
    StorageObject() : size(0), lastModified(0), isEncrypted(false), isVersioned(false) {}
};

struct TransferStats {
    uint64_t bytesTransferred{0};      // Bytes transferred
    uint64_t transferTime{0};          // Transfer time (milliseconds)
    uint64_t transferRate{0};          // Transfer rate (bytes/second)
    uint64_t errorCount{0};            // Error count
    uint64_t retryCount{0};            // Retry count
    double efficiency{0.0};           // Transfer efficiency
    time_t startTime{0};               // Transfer start time
    time_t endTime{0};                 // Transfer end time
    
    void addTransfer(uint64_t bytes, uint64_t time) {
        bytesTransferred += bytes;
        transferTime += time;
        retryCount++;
        
        if (transferTime > 0) {
            transferRate = (bytesTransferred * 1000) / transferTime; // bytes per second
        }
        
        if (startTime == 0) {
            startTime = std::time(nullptr);
        }
        endTime = std::time(nullptr);
        
        double totalTimeSeconds = (endTime - startTime) * 1.0;
        if (totalTimeSeconds > 0) {
            efficiency = static_cast<double>(bytesTransferred) / (totalTimeSeconds * bandwidthLimit);
        }
    }
    
    double getTransferTimeSeconds() const {
        return (endTime - startTime) * 1.0;
    }
    
    double getAverageSpeed() const {
        if (transferTime == 0) return 0.0;
        return (bytesTransferred * 1000) / transferTime; // bytes per second
    }
    
    double bandwidthLimit{0.0};       // Bandwidth limit (bytes/second)
};

class RemoteStorage {
public:
    RemoteStorage();
    ~RemoteStorage();
    
    // Disable copying
    RemoteStorage(const RemoteStorage&) = delete;
    RemoteStorage& operator=(const RemoteStorage&) = delete;
    
    /**
     * Connect to remote storage
     * @param metadata Storage metadata
     * @return true if connected successfully
     */
    bool connect(const StorageMetadata& metadata);
    
    /**
     * Disconnect from remote storage
     */
    void disconnect();
    
    /**
     * Check if connected to storage
     */
    bool isConnected() const { return connected_; }
    
    /**
     * Get storage metadata
     */
    const StorageMetadata& getMetadata() const { return metadata_; }
    
    /**
     * Set storage metadata
     */
    void setMetadata(const StorageMetadata& metadata) { metadata_ = metadata; }
    
    /**
     * Get storage type
     */
    StorageType getStorageType() const { return metadata_.type; }
    
    /**
     * Get storage access mode
     */
    StorageAccessMode getAccessMode() const { return metadata_.accessMode; }
    
    /**
     * Set storage access mode
     */
    void setAccessMode(StorageAccessMode mode) { metadata_.accessMode = mode; }
    
    /**
     * Store object to storage
     * @param key Object key
     * @param data Object data
     * @param metadata Object metadata
     * @return StorageError
     */
    StorageError putObject(const std::string& key, const std::vector<uint8_t>& data,
                          const std::map<std::string, std::string>& metadata = {});
    
    /**
     * Get object from storage
     * @param key Object key
     * @return Object data or empty vector if not found
     */
    std::vector<uint8_t> getObject(const std::string& key);
    
    /**
     * Get object metadata
     * @param key Object key
     * @return Object metadata
     */
    StorageObject getObjectMetadata(const std::string& key);
    
    /**
     * Delete object from storage
     * @param key Object key
     * @return StorageError
     */
    StorageError deleteObject(const std::string& key);
    
    /**
     * List objects in storage
     * @param prefix Object prefix
     * @param limit Maximum number of objects
     * @return Vector of storage objects
     */
    std::vector<StorageObject> listObjects(const std::string& prefix = "", size_t limit = 1000);
    
    /**
     * Check if object exists
     * @param key Object key
     * @return true if object exists
     */
    bool doesObjectExist(const std::string& key);
    
    /**
     * Copy object
     * @param sourceKey Source object key
     * @param destinationKey Destination object key
     * @return StorageError
     */
    StorageError copyObject(const std::string& sourceKey, const std::string& destinationKey);
    
    /**
     * Get storage statistics
     */
    struct StorageStatistics {
        uint64_t totalObjects{0};          // Total objects
        uint64_t totalSize{0};             // Total size (bytes)
        uint64_t availableSize{0};        // Available size (bytes)
        uint64_t usedSize{0};             // Used size (bytes)
        uint32_t errorCount{0};           // Error count
        uint32_t retryCount{0};           // Retry count
        double usageRatio{0.0};           // Usage ratio (0.0-1.0)
        time_t lastOperationTime{0};     // Last operation time
        double averageLatency{0.0};      // Average latency (milliseconds)
        std::string lastError;            // Last error message
        
        void addTransfer(const TransferStats& stats) {
            totalObjects++;
            totalSize += stats.bytesTransferred;
            usedSize += stats.bytesTransferred;
            errorCount += stats.errorCount;
            retryCount += stats.retryCount;
            
            if (totalSize > 0) {
                usageRatio = static_cast<double>(usedSize) / totalSize;
            }
            
            lastOperationTime = std::time(nullptr);
        }
    };
    
    StorageStatistics getStatistics() const { return stats_; }
    
    /**
     * Get transfer statistics
     */
    TransferStats getTransferStatistics() const { return transferStats_; }
    
    /**
     * Set bandwidth limit
     */
    void setBandwidthLimit(double limit) { metadata_.bandwidthLimit = limit; }
    
    /**
     * Get bandwidth limit
     */
    double getBandwidthLimit() const { return metadata_.bandwidthLimit; }
    
    /**
     * Set transfer rate
     */
    void setTransferRate(double rate) { metadata_.transferRate = rate; }
    
    /**
     * Get transfer rate
     */
    double getTransferRate() const { return metadata_.transferRate; }
    
    /**
     * Enable/disable caching
     */
    void setCacheEnabled(bool enabled) { cacheEnabled_ = enabled; }
    
    /**
     * Get cache enabled
     */
    bool isCacheEnabled() const { return cacheEnabled_; }
    
    /**
     * Set cache size
     */
    void setCacheSize(size_t cacheSize) { cacheSize_ = cacheSize; }
    
    /**
     * Get cache size
     */
    size_t getCacheSize() const { return cacheSize_; }
    
    /**
     * Set local fallback storage path
     */
    void setLocalFallbackPath(const std::string& path) { localFallbackPath_ = path; }
    
    /**
     * Get local fallback storage path
     */
    const std::string& getLocalFallbackPath() const { return localFallbackPath_; }
    
    /**
     * Enable/disable local fallback
     */
    void setLocalFallbackEnabled(bool enabled) { localFallbackEnabled_ = enabled; }
    
    /**
     * Get local fallback enabled
     */
    bool isLocalFallbackEnabled() const { return localFallbackEnabled_; }
    
    /**
     * Set retry policy
     */
    void setRetryPolicy(int maxRetries, double retryDelay) {
        maxRetries_ = maxRetries;
        retryDelay_ = retryDelay;
    }
    
    /**
     * Get max retries
     */
    int getMaxRetries() const { return maxRetries_; }
    
    /**
     * Get retry delay
     */
    double getRetryDelay() const { return retryDelay_; }
    
    /**
     * Set connection timeout
     */
    void setConnectionTimeout(double timeout) { connectionTimeout_ = timeout; }
    
    /**
     * Get connection timeout
     */
    double getConnectionTimeout() const { return connectionTimeout_; }
    
    /**
     * Test connection
     */
    bool testConnection();
    
    /**
     * Get storage endpoint
     */
    std::string getEndpoint() const { return metadata_.endpoint; }
    
    /**
     * Get storage bucket
     */
    std::string getBucket() const { return metadata_.bucket; }
    
    /**
     * Get storage prefix
     */
    std::string getPrefix() const { return metadata_.prefix; }
    
    /**
     * Set storage prefix
     */
    void setPrefix(const std::string& prefix) { metadata_.prefix = prefix; }
    
    /**
     * Get version string
     */
    static std::string getVersion() { return "RemoteStorage v1.0"; }
    
protected:
    // Storage backends
    bool connectLocal(const StorageMetadata& metadata);
    bool connectS3(const StorageMetadata& metadata);
    bool connectGCS(const StorageMetadata& metadata);
    bool connectAzure(const StorageMetadata& metadata);
    bool connectNFS(const StorageMetadata& metadata);
    bool connectHDFS(const StorageMetadata& metadata);
    bool connectCustom(const StorageMetadata& metadata);
    
    // Object operations
    StorageError putObjectLocal(const std::string& key, const std::vector<uint8_t>& data,
                               const std::map<std::string, std::string>& metadata);
    std::vector<uint8_t> getObjectLocal(const std::string& key);
    StorageObject getObjectMetadataLocal(const std::string& key);
    StorageError deleteObjectLocal(const std::string& key);
    std::vector<StorageObject> listObjectsLocal(const std::string& prefix = "", size_t limit = 1000);
    
    StorageError putObjectS3(const std::string& key, const std::vector<uint8_t>& data,
                             const std::map<std::string, std::string>& metadata);
    std::vector<uint8_t> getObjectS3(const std::string& key);
    StorageObject getObjectMetadataS3(const std::string& key);
    StorageError deleteObjectS3(const std::string& key);
    std::vector<StorageObject> listObjectsS3(const std::string& prefix = "", size_t limit = 1000);
    
    // Cache management
    bool addToCache(const std::string& key, const std::vector<uint8_t>& data);
    std::vector<uint8_t> getFromCache(const std::string& key) const;
    bool isInCache(const std::string& key) const;
    void clearCache();
    size_t getCacheSize() const;
    
    // Error handling
    StorageError handleError(const std::string& error) const;
    bool shouldRetry(const StorageError& error) const;
    
    // Transfer management
    void recordTransfer(const TransferStats& stats);
    
    // Statistics
    mutable std::mutex statsMutex_;
    StorageMetadata metadata_;
    bool connected_{false};
    bool cacheEnabled_{true};
    size_t cacheSize_{100};
    std::string localFallbackPath_;
    bool localFallbackEnabled_{true};
    int maxRetries_{3};
    double retryDelay_{1.0};
    double connectionTimeout_{30.0};
    
    // Cache
    mutable std::unordered_map<std::string, std::vector<uint8_t>> cache_;
    
    // Statistics
    StorageStatistics stats_;
    TransferStats transferStats_;
};