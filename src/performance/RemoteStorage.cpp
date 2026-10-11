// RemoteStorage.cpp
#include "RemoteStorage.hpp"
#include <thread>
#include <future>
#include <shared_mutex>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {
    
    RemoteStorage::RemoteStorage()
        : cacheEnabled_(true)
        , cacheSize_(100)
        , localFallbackEnabled_(true)
        , maxRetries_(3)
        , retryDelay_(1.0)
        , connectionTimeout_(30.0)
        , connected_(false) {}
    
    RemoteStorage::~RemoteStorage() {
        disconnect();
    }
    
    bool RemoteStorage::connect(const StorageMetadata& metadata) {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        // Disconnect existing connection
        if (connected_) {
            disconnect();
        }
        
        // Validate metadata
        if (metadata.type == StorageType::None) {
            NLM_LOG_ERROR("Storage type cannot be None");
            return false;
        }
        
        // Store metadata
        metadata_ = metadata;
        
        // Try to connect based on storage type
        switch (metadata_.type) {
            case StorageType::Local:
                connected_ = connectLocal(metadata_);
                break;
            case StorageType::S3:
                connected_ = connectS3(metadata_);
                break;
            case StorageType::GCS:
                connected_ = connectGCS(metadata_);
                break;
            case StorageType::Azure:
                connected_ = connectAzure(metadata_);
                break;
            case StorageType::NFS:
                connected_ = connectNFS(metadata_);
                break;
            case StorageType::HDFS:
                connected_ = connectHDFS(metadata_);
                break;
            case StorageType::Custom:
                connected_ = connectCustom(metadata_);
                break;
            default:
                NLM_LOG_ERROR("Unsupported storage type");
                connected_ = false;
                break;
        }
        
        if (connected_) {
            NLM_LOG_INFO("Connected to storage: " + metadata_.uri);
        } else {
            NLM_LOG_ERROR("Failed to connect to storage: " + metadata_.uri);
        }
        
        return connected_;
    }
    
    void RemoteStorage::disconnect() {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        if (connected_) {
            // Cleanup based on storage type
            switch (metadata_.type) {
                case StorageType::Local:
                    // Cleanup local storage
                    break;
                case StorageType::S3:
                    // Cleanup S3 storage
                    break;
                case StorageType::GCS:
                    // Cleanup GCS storage
                    break;
                case StorageType::Azure:
                    // Cleanup Azure storage
                    break;
                case StorageType::NFS:
                    // Cleanup NFS storage
                    break;
                case StorageType::HDFS:
                    // Cleanup HDFS storage
                    break;
                case StorageType::Custom:
                    // Cleanup custom storage
                    break;
                default:
                    break;
            }
            
            connected_ = false;
            clearCache();
            
            NLM_LOG_INFO("Disconnected from storage: " + metadata_.uri);
        }
    }
    
    StorageError RemoteStorage::putObject(
        const std::string& key,
        const std::vector<uint8_t>& data,
        const std::map<std::string, std::string>& metadata) {
        
        if (!connected_) {
            return StorageError::ConnectionError;
        }
        
        // Try primary storage
        StorageError result = StorageError::Success;
        
        switch (metadata_.type) {
            case StorageType::Local:
                result = putObjectLocal(key, data, metadata);
                break;
            case StorageType::S3:
                result = putObjectS3(key, data, metadata);
                break;
            case StorageType::GCS:
                result = putObjectGCS(key, data, metadata);
                break;
            case StorageType::Azure:
                result = putObjectAzure(key, data, metadata);
                break;
            case StorageType::NFS:
                result = putObjectNFS(key, data, metadata);
                break;
            case StorageType::HDFS:
                result = putObjectHDFS(key, data, metadata);
                break;
            case StorageType::Custom:
                result = putObjectCustom(key, data, metadata);
                break;
            default:
                result = StorageError::NotImplemented;
                break;
        }
        
        // If primary storage fails and local fallback is enabled, try fallback
        if (result != StorageError::Success && localFallbackEnabled_) {
            NLM_LOG_WARNING("Primary storage failed, trying local fallback: " + key);
            result = putObjectLocal(key, data, metadata);
        }
        
        // Record transfer statistics
        if (result == StorageError::Success) {
            TransferStats stats;
            stats.addTransfer(data.size(), 10); // Estimate time
            recordTransfer(stats);
        }
        
        return result;
    }
    
    std::vector<uint8_t> RemoteStorage::getObject(const std::string& key) {
        if (!connected_) {
            return std::vector<uint8_t>();
        }
        
        // Check cache first
        if (cacheEnabled_) {
            std::vector<uint8_t> cached = getFromCache(key);
            if (!cached.empty()) {
                return cached;
            }
        }
        
        // Try primary storage
        std::vector<uint8_t> result;
        
        switch (metadata_.type) {
            case StorageType::Local:
                result = getObjectLocal(key);
                break;
            case StorageType::S3:
                result = getObjectS3(key);
                break;
            case StorageType::GCS:
                result = getObjectGCS(key);
                break;
            case StorageType::Azure:
                result = getObjectAzure(key);
                break;
            case StorageType::NFS:
                result = getObjectNFS(key);
                break;
            case StorageType::HDFS:
                result = getObjectHDFS(key);
                break;
            case StorageType::Custom:
                result = getObjectCustom(key);
                break;
            default:
                break;
        }
        
        // If primary storage fails and local fallback is enabled, try fallback
        if (result.empty() && localFallbackEnabled_) {
            NLM_LOG_WARNING("Primary storage failed, trying local fallback: " + key);
            result = getObjectLocal(key);
        }
        
        // Cache result if enabled
        if (cacheEnabled_ && !result.empty()) {
            addToCache(key, result);
        }
        
        return result;
    }
    
    StorageObject RemoteStorage::getObjectMetadata(const std::string& key) {
        if (!connected_) {
            return StorageObject();
        }
        
        StorageObject result;
        
        switch (metadata_.type) {
            case StorageType::Local:
                result = getObjectMetadataLocal(key);
                break;
            case StorageType::S3:
                result = getObjectMetadataS3(key);
                break;
            case StorageType::GCS:
                result = getObjectMetadataGCS(key);
                break;
            case StorageType::Azure:
                result = getObjectMetadataAzure(key);
                break;
            case StorageType::NFS:
                result = getObjectMetadataNFS(key);
                break;
            case StorageType::HDFS:
                result = getObjectMetadataHDFS(key);
                break;
            case StorageType::Custom:
                result = getObjectMetadataCustom(key);
                break;
            default:
                break;
        }
        
        return result;
    }
    
    StorageError RemoteStorage::deleteObject(const std::string& key) {
        if (!connected_) {
            return StorageError::ConnectionError;
        }
        
        // Try primary storage
        StorageError result = StorageError::Success;
        
        switch (metadata_.type) {
            case StorageType::Local:
                result = deleteObjectLocal(key);
                break;
            case StorageType::S3:
                result = deleteObjectS3(key);
                break;
            case StorageType::GCS:
                result = deleteObjectGCS(key);
                break;
            case StorageType::Azure:
                result = deleteObjectAzure(key);
                break;
            case StorageType::NFS:
                result = deleteObjectNFS(key);
                break;
            case StorageType::HDFS:
                result = deleteObjectHDFS(key);
                break;
            case StorageType::Custom:
                result = deleteObjectCustom(key);
                break;
            default:
                result = StorageError::NotImplemented;
                break;
        }
        
        // If primary storage fails and local fallback is enabled, try fallback
        if (result != StorageError::Success && localFallbackEnabled_) {
            NLM_LOG_WARNING("Primary storage failed, trying local fallback: " + key);
            result = deleteObjectLocal(key);
        }
        
        return result;
    }
    
    std::vector<StorageObject> RemoteStorage::listObjects(
        const std::string& prefix,
        size_t limit) {
        
        std::vector<StorageObject> result;
        
        if (!connected_) {
            return result;
        }
        
        switch (metadata_.type) {
            case StorageType::Local:
                result = listObjectsLocal(prefix, limit);
                break;
            case StorageType::S3:
                result = listObjectsS3(prefix, limit);
                break;
            case StorageType::GCS:
                result = listObjectsGCS(prefix, limit);
                break;
            case StorageType::Azure:
                result = listObjectsAzure(prefix, limit);
                break;
            case StorageType::NFS:
                result = listObjectsNFS(prefix, limit);
                break;
            case StorageType::HDFS:
                result = listObjectsHDFS(prefix, limit);
                break;
            case StorageType::Custom:
                result = listObjectsCustom(prefix, limit);
                break;
            default:
                break;
        }
        
        return result;
    }
    
    bool RemoteStorage::doesObjectExist(const std::string& key) {
        if (!connected_) {
            return false;
        }
        
        switch (metadata_.type) {
            case StorageType::Local:
                return doesObjectExistLocal(key);
            case StorageType::S3:
                return doesObjectExistS3(key);
            case StorageType::GCS:
                return doesObjectExistGCS(key);
            case StorageType::Azure:
                return doesObjectExistAzure(key);
            case StorageType::NFS:
                return doesObjectExistNFS(key);
            case StorageType::HDFS:
                return doesObjectExistHDFS(key);
            case StorageType::Custom:
                return doesObjectExistCustom(key);
            default:
                return false;
        }
    }
    
    StorageError RemoteStorage::copyObject(
        const std::string& sourceKey,
        const std::string& destinationKey) {
        
        if (!connected_) {
            return StorageError::ConnectionError;
        }
        
        // Get source object
        std::vector<uint8_t> data = getObject(sourceKey);
        if (data.empty()) {
            return StorageError::NotFound;
        }
        
        // Store destination object
        return putObject(destinationKey, data);
    }
    
    bool RemoteStorage::testConnection() {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        if (!connected_) {
            return false;
        }
        
        switch (metadata_.type) {
            case StorageType::Local:
                return testConnectionLocal();
            case StorageType::S3:
                return testConnectionS3();
            case StorageType::GCS:
                return testConnectionGCS();
            case StorageType::Azure:
                return testConnectionAzure();
            case StorageType::NFS:
                return testConnectionNFS();
            case StorageType::HDFS:
                return testConnectionHDFS();
            case StorageType::Custom:
                return testConnectionCustom();
            default:
                return false;
        }
    }
    
    bool RemoteStorage::addToCache(
        const std::string& key,
        const std::vector<uint8_t>& data) const {
        if (!cacheEnabled_) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        if (cache_.size() >= cacheSize_) {
            // Remove oldest entry (simple FIFO)
            auto it = cache_.begin();
            cache_.erase(it);
        }
        
        cache_[key] = data;
        return true;
    }
    
    std::vector<uint8_t> RemoteStorage::getFromCache(const std::string& key) const {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        
        return std::vector<uint8_t>();
    }
    
    bool RemoteStorage::isInCache(const std::string& key) const {
        std::lock_guard<std::mutex> lock(statsMutex_);
        return cache_.find(key) != cache_.end();
    }
    
    void RemoteStorage::clearCache() {
        std::lock_guard<std::mutex> lock(statsMutex_);
        cache_.clear();
    }
    
    size_t RemoteStorage::getCacheSize() const {
        std::lock_guard<std::mutex> lock(statsMutex_);
        return cache_.size();
    }
    
    StorageError RemoteStorage::handleError(const std::string& error) const {
        // Simple error handling - in real implementation would be more sophisticated
        if (error.find("connection") != std::string::npos ||
            error.find("network") != std::string::npos) {
            return StorageError::ConnectionError;
        } else if (error.find("auth") != std::string::npos ||
                   error.find("permission") != std::string::npos) {
            return StorageError::AuthorizationError;
        } else if (error.find("not found") != std::string::npos) {
            return StorageError::NotFound;
        } else if (error.find("quota") != std::string::npos) {
            return StorageError::QuotaExceeded;
        } else {
            return StorageError::Unknown;
        }
    }
    
    bool RemoteStorage::shouldRetry(const StorageError& error) const {
        switch (error) {
            case StorageError::ConnectionError:
            case StorageError::NetworkError:
            case StorageError::Timeout:
                return true;
            case StorageError::AuthorizationError:
            case StorageError::NotFound:
            case StorageError::NotImplemented:
                return false;
            case StorageError::QuotaExceeded:
                return false; // Should not retry quota errors
            case StorageError::Success:
                return false;
            default:
                return false;
        }
    }
    
    void RemoteStorage::recordTransfer(const TransferStats& stats) {
        std::lock_guard<std::mutex> lock(statsMutex_);
        transferStats_.addTransfer(stats);
        stats_.addTransfer(stats);
    }
    
    // Local storage implementations
    bool RemoteStorage::connectLocal(const StorageMetadata& metadata) {
        // Create local directory if needed
        try {
            std::filesystem::create_directories(metadata.uri);
            metadata_ = metadata;
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to connect to local storage: ") + e.what());
            return false;
        }
    }
    
    StorageError RemoteStorage::putObjectLocal(
        const std::string& key,
        const std::vector<uint8_t>& data,
        const std::map<std::string, std::string>& metadata) {
        
        try {
            std::string filepath = metadata_.uri + "/" + key;
            
            // Create directory if needed
            std::filesystem::create_directories(std::filesystem::path(filepath).parent_path());
            
            // Write file
            std::ofstream file(filepath, std::ios::binary);
            if (!file) {
                return StorageError::ConnectionError;
            }
            
            file.write(reinterpret_cast<const char*>(data.data()), data.size());
            if (!file.good()) {
                return StorageError::ConnectionError;
            }
            
            file.close();
            return StorageError::Success;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in putObjectLocal: ") + e.what());
            return StorageError::Unknown;
        }
    }
    
    std::vector<uint8_t> RemoteStorage::getObjectLocal(const std::string& key) {
        try {
            std::string filepath = metadata_.uri + "/" + key;
            
            std::ifstream file(filepath, std::ios::binary);
            if (!file) {
                return std::vector<uint8_t>();
            }
            
            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            
            std::vector<uint8_t> data(fileSize);
            if (!file.read(reinterpret_cast<char*>(data.data()), fileSize)) {
                return std::vector<uint8_t>();
            }
            
            return data;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in getObjectLocal: ") + e.what());
            return std::vector<uint8_t>();
        }
    }
    
    StorageObject RemoteStorage::getObjectMetadataLocal(const std::string& key) {
        StorageObject obj;
        
        try {
            std::string filepath = metadata_.uri + "/" + key;
            
            if (!std::filesystem::exists(filepath)) {
                return obj;
            }
            
            auto fileTime = std::filesystem::last_write_time(std::filesystem::path(filepath));
            auto fileTime_t = std::chrono::system_clock::to_time_t(fileTime);
            
            obj.key = key;
            obj.size = std::filesystem::file_size(filepath);
            obj.lastModified = fileTime_t;
            obj.contentType = "application/octet-stream";
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in getObjectMetadataLocal: ") + e.what());
        }
        
        return obj;
    }
    
    StorageError RemoteStorage::deleteObjectLocal(const std::string& key) {
        try {
            std::string filepath = metadata_.uri + "/" + key;
            
            if (std::filesystem::exists(filepath)) {
                std::filesystem::remove(filepath);
            }
            
            return StorageError::Success;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in deleteObjectLocal: ") + e.what());
            return StorageError::Unknown;
        }
    }
    
    std::vector<StorageObject> RemoteStorage::listObjectsLocal(
        const std::string& prefix,
        size_t limit) {
        
        std::vector<StorageObject> result;
        
        try {
            for (const auto& entry : std::filesystem::directory_iterator(metadata_.uri)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();;
                    
                    if (!prefix.empty() && filename.find(prefix) != 0) {
                        continue;
                    }
                    
                    StorageObject obj;
                    obj.key = filename;
                    obj.size = entry.file_size();
                    
                    auto fileTime = entry.last_write_time();
                    auto fileTime_t = std::chrono::system_clock::to_time_t(fileTime);
                    obj.lastModified = fileTime_t;
                    
                    result.push_back(obj);
                    
                    if (result.size() >= limit) {
                        break;
                    }
                }
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in listObjectsLocal: ") + e.what());
        }
        
        return result;
    }
    
    bool RemoteStorage::doesObjectExistLocal(const std::string& key) {
        try {
            std::string filepath = metadata_.uri + "/" + key;
            return std::filesystem::exists(filepath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in doesObjectExistLocal: ") + e.what());
            return false;
        }
    }
    
    bool RemoteStorage::testConnectionLocal() {
        try {
            return std::filesystem::exists(metadata_.uri);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in testConnectionLocal: ") + e.what());
            return false;
        }
    }
    
    // Stub implementations for other storage types
    bool RemoteStorage::connectS3(const StorageMetadata& metadata) {
        // In real implementation, this would initialize S3 client
        NLM_LOG_INFO("Connecting to S3 storage: " + metadata.bucket + " at " + metadata.endpoint);
        metadata_ = metadata;
        return true;
    }
    
    StorageError RemoteStorage::putObjectS3(
        const std::string& key,
        const std::vector<uint8_t>& data,
        const std::map<std::string, std::string>& metadata) {
        // In real implementation, this would upload to S3
        NLM_LOG_INFO("Uploading to S3: " + key);
        return StorageError::Success;
    }
    
    std::vector<uint8_t> RemoteStorage::getObjectS3(const std::string& key) {
        // In real implementation, this would download from S3
        NLM_LOG_INFO("Downloading from S3: " + key);
        return std::vector<uint8_t>();
    }
    
    StorageObject RemoteStorage::getObjectMetadataS3(const std::string& key) {
        // In real implementation, this would get S3 metadata
        NLM_LOG_INFO("Getting S3 metadata: " + key);
        return StorageObject();
    }
    
    StorageError RemoteStorage::deleteObjectS3(const std::string& key) {
        // In real implementation, this would delete from S3
        NLM_LOG_INFO("Deleting from S3: " + key);
        return StorageError::Success;
    }
    
    std::vector<StorageObject> RemoteStorage::listObjectsS3(
        const std::string& prefix,
        size_t limit) {
        // In real implementation, this would list S3 objects
        NLM_LOG_INFO("Listing S3 objects with prefix: " + prefix);
        return std::vector<StorageObject>();
    }
    
    bool RemoteStorage::connectGCS(const StorageMetadata& metadata) {
        NLM_LOG_INFO("Connecting to GCS storage: " + metadata.bucket + " at " + metadata.endpoint);
        metadata_ = metadata;
        return true;
    }
    
    StorageError RemoteStorage::putObjectGCS(
        const std::string& key,
        const std::vector<uint8_t>& data,
        const std::map<std::string, std::string>& metadata) {
        NLM_LOG_INFO("Uploading to GCS: " + key);
        return StorageError::Success;
    }
    
    std::vector<uint8_t> RemoteStorage::getObjectGCS(const std::string& key) {
        NLM_LOG_INFO("Downloading from GCS: " + key);
        return std::vector<uint8_t>();
    }
    
    StorageObject RemoteStorage::getObjectMetadataGCS(const std::string& key) {
        NLM_LOG_INFO("Getting GCS metadata: " + key);
        return StorageObject();
    }
    
    StorageError RemoteStorage::deleteObjectGCS(const std::string& key) {
        NLM_LOG_INFO("Deleting from GCS: " + key);
        return StorageError::Success;
    }
    
    std::vector<StorageObject> RemoteStorage::listObjectsGCS(
        const std::string& prefix,
        size_t limit) {
        NLM_LOG_INFO("Listing GCS objects with prefix: " + prefix);
        return std::vector<StorageObject>();
    }
    
    bool RemoteStorage::connectAzure(const StorageMetadata& metadata) {
        NLM_LOG_INFO("Connecting to Azure storage: " + metadata.bucket + " at " + metadata.endpoint);
        metadata_ = metadata;
        return true;
    }
    
    StorageError RemoteStorage::putObjectAzure(
        const std::string& key,
        const std::vector<uint8_t>& data,
        const std::map<std::string, std::string>& metadata) {
        NLM_LOG_INFO("Uploading to Azure: " + key);
        return StorageError::Success;
    }
    
    std::vector<uint8_t> RemoteStorage::getObjectAzure(const std::string& key) {
        NLM_LOG_INFO("Downloading from Azure: " + key);
        return std::vector<uint8_t>();
    }
    
    StorageObject RemoteStorage::getObjectMetadataAzure(const std::string& key) {
        NLM_LOG_INFO("Getting Azure metadata: " + key);
        return StorageObject();
    }
    
    StorageError RemoteStorage::deleteObjectAzure(const std::string& key) {
        NLM_LOG_INFO("Deleting from Azure: " + key);
        return StorageError::Success;
    }
    
    std::vector<StorageObject> RemoteStorage::listObjectsAzure(
        const std::string& prefix,
        size_t limit) {
        NLM_LOG_INFO("Listing Azure objects with prefix: " + prefix);
        return std::vector<StorageObject>();
    }
    
    bool RemoteStorage::connectNFS(const StorageMetadata& metadata) {
        NLM_LOG_INFO("Connecting to NFS storage: " + metadata.uri);
        metadata_ = metadata;
        return true;
    }
    
    StorageError RemoteStorage::putObjectNFS(
        const std::string& key,
        const std::vector<uint8_t>& data,
        const std::map<std::string, std::string>& metadata) {
        NLM_LOG_INFO("Uploading to NFS: " + key);
        return StorageError::Success;
    }
    
    std::vector<uint8_t> RemoteStorage::getObjectNFS(const std::string& key) {
        NLM_LOG_INFO("Downloading from NFS: " + key);
        return std::vector<uint8_t>();
    }
    
    StorageObject RemoteStorage::getObjectMetadataNFS(const std::string& key) {
        NLM_LOG_INFO("Getting NFS metadata: " + key);
        return StorageObject();
    }
    
    StorageError RemoteStorage::deleteObjectNFS(const std::string& key) {
        NLM_LOG_INFO("Deleting from NFS: " + key);
        return StorageError::Success;
    }
    
    std::vector<StorageObject> RemoteStorage::listObjectsNFS(
        const std::string& prefix,
        size_t limit) {
        NLM_LOG_INFO("Listing NFS objects with prefix: " + prefix);
        return std::vector<StorageObject>();
    }
    
    bool RemoteStorage::connectHDFS(const StorageMetadata& metadata) {
        NLM_LOG_INFO("Connecting to HDFS storage: " + metadata.uri);
        metadata_ = metadata;
        return true;
    }
    
    StorageError RemoteStorage::putObjectHDFS(
        const std::string& key,
        const std::vector<uint8_t>& data,
        const std::map<std::string, std::string>& metadata) {
        NLM_LOG_INFO("Uploading to HDFS: " + key);
        return StorageError::Success;
    }
    
    std::vector<uint8_t> RemoteStorage::getObjectHDFS(const std::string& key) {
        NLM_LOG_INFO("Downloading from HDFS: " + key);
        return std::vector<uint8_t>();
    }
    
    StorageObject RemoteStorage::getObjectMetadataHDFS(const std::string& key) {
        NLM_LOG_INFO("Getting HDFS metadata: " + key);
        return StorageObject();
    }
    
    StorageError RemoteStorage::deleteObjectHDFS(const std::string& key) {
        NLM_LOG_INFO("Deleting from HDFS: " + key);
        return StorageError::Success;
    }
    
    std::vector<StorageObject> RemoteStorage::listObjectsHDFS(
        const std::string& prefix,
        size_t limit) {
        NLM_LOG_INFO("Listing HDFS objects with prefix: " + prefix);
        return std::vector<StorageObject>();
    }
    
    bool RemoteStorage::connectCustom(const StorageMetadata& metadata) {
        NLM_LOG_INFO("Connecting to custom storage: " + metadata.uri);
        metadata_ = metadata;
        return true;
    }
    
    StorageError RemoteStorage::putObjectCustom(
        const std::string& key,
        const std::vector<uint8_t>& data,
        const std::map<std::string, std::string>& metadata) {
        NLM_LOG_INFO("Uploading to custom storage: " + key);
        return StorageError::Success;
    }
    
    std::vector<uint8_t> RemoteStorage::getObjectCustom(const std::string& key) {
        NLM_LOG_INFO("Downloading from custom storage: " + key);
        return std::vector<uint8_t>();
    }
    
    StorageObject RemoteStorage::getObjectMetadataCustom(const std::string& key) {
        NLM_LOG_INFO("Getting custom storage metadata: " + key);
        return StorageObject();
    }
    
    StorageError RemoteStorage::deleteObjectCustom(const std::string& key) {
        NLM_LOG_INFO("Deleting from custom storage: " + key);
        return StorageError::Success;
    }
    
    std::vector<StorageObject> RemoteStorage::listObjectsCustom(
        const std::string& prefix,
        size_t limit) {
        NLM_LOG_INFO("Listing custom storage objects with prefix: " + prefix);
        return std::vector<StorageObject>();
    }
    
} // namespace nlm