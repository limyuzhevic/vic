#include "checkpoint_manager.h"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <zlib.h>

namespace nlm {

// Helper functions
SimulationStep getCurrentStep() {
    static SimulationStep counter = 0;
    return ++counter;
}

std::string generateUniqueId() {
    static std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<uint64_t> dist;
    uint64_t random = dist(rng);
    std::stringstream ss;
    ss << std::hex << random;
    return "ckpt_" + ss.str();
}

class CheckpointManager::Impl {
public:
    std::string checkpointDirectory_ = "checkpoints";
    size_t checkpointFrequency_ = 100;
    bool compressionEnabled_ = true;
    bool encryptionEnabled_ = false;
    size_t maxCheckpointSize_ = 100 * 1024 * 1024;
    
    CompressionAlgorithm compressionAlgorithm_ = CompressionAlgorithm::Gzip;
    EncryptionAlgorithm encryptionAlgorithm_ = EncryptionAlgorithm::AES256;
    std::string encryptionKey_;
    std::string encryptionSalt_;
    
    bool parallelEnabled_ = true;
    std::vector<CheckpointOperation> activeOperations_;
    
    std::unordered_map<std::string, CheckpointMetadata> checkpoints_;
    std::unordered_map<std::string, std::vector<char>> checkpointData_;
    std::unordered_map<std::string, DifferentialEntry> differentialCheckpoints_;
    
    std::chrono::system_clock::time_point lastCheckpointTime_;
    SimulationStep lastCheckpointStep_ = 0;
    
    static constexpr size_t MAX_CHECKPOINTS = 1000;
    static constexpr size_t MAX_DIFFERENTIAL_SIZE = 10 * 1024 * 1024;
    
    Impl() {
        std::filesystem::create_directories(checkpointDirectory_);
        
        if (encryptionKey_.empty()) {
            encryptionKey_ = generateEncryptionKey();
        }
    }
    
    std::string generateEncryptionKey() {
        std::vector<unsigned char> key(EVP_MAX_KEY_LENGTH);
        RAND_bytes(key.data(), key.size());
        return std::string(key.begin(), key.end());
    }
    
    std::vector<char> compressData(const std::vector<char>& data, CompressionAlgorithm algorithm) const {
        switch (algorithm) {
            case CompressionAlgorithm::Gzip:
                return compressWithGzip(data);
            case CompressionAlgorithm::LZ4:
                return compressWithLZ4(data);
            case CompressionAlgorithm::Zstd:
                return compressWithZstd(data);
            case CompressionAlgorithm::Brotli:
                return compressWithBrotli(data);
            default:
                return data;
        }
    }
    
    std::vector<char> decompressData(const std::vector<char>& data, CompressionAlgorithm algorithm) const {
        switch (algorithm) {
            case CompressionAlgorithm::Gzip:
                return decompressWithGzip(data);
            case CompressionAlgorithm::LZ4:
                return decompressWithLZ4(data);
            case CompressionAlgorithm::Zstd:
                return decompressWithZstd(data);
            case CompressionAlgorithm::Brotli:
                return decompressWithBrotli(data);
            default:
                return data;
        }
    }
    
    std::vector<char> compressWithGzip(const std::vector<char>& data) const {
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        
        if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
            return data;
        }
        
        std::vector<char> compressed;
        compressed.resize(data.size() * 2);
        
        zs.next_in = (Bytef*)data.data();
        zs.avail_in = data.size();
        zs.next_out = (Bytef*)compressed.data();
        zs.avail_out = compressed.size();
        
        if (deflate(&zs, Z_FINISH) != Z_STREAM_END) {
            deflateEnd(&zs);
            return data;
        }
        
        size_t actualSize = zs.total_out;
        compressed.resize(actualSize);
        deflateEnd(&zs);
        
        return compressed;
    }
    
    std::vector<char> decompressWithGzip(const std::vector<char>& data) const {
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        
        if (inflateInit2(&zs, 15 + 16) != Z_OK) {
            return data;
        }
        
        std::vector<char> decompressed;
        decompressed.resize(data.size() * 4);
        
        zs.next_in = (Bytef*)data.data();
        zs.avail_in = data.size();
        zs.next_out = (Bytef*)decompressed.data();
        zs.avail_out = decompressed.size();
        
        if (inflate(&zs, Z_FINISH) != Z_STREAM_END) {
            inflateEnd(&zs);
            return data;
        }
        
        size_t actualSize = zs.total_out;
        decompressed.resize(actualSize);
        inflateEnd(&zs);
        
        return decompressed;
    }
    
    std::vector<char> compressWithLZ4(const std::vector<char>& data) const {
        return data;
    }
    
    std::vector<char> decompressWithLZ4(const std::vector<char>& data) const {
        return data;
    }
    
    std::vector<char> compressWithZstd(const std::vector<char>& data) const {
        return data;
    }
    
    std::vector<char> decompressWithZstd(const std::vector<char>& data) const {
        return data;
    }
    
    std::vector<char> compressWithBrotli(const std::vector<char>& data) const {
        return data;
    }
    
    std::vector<char> decompressWithBrotli(const std::vector<char>& data) const {
        return data;
    }
    
    std::vector<char> encryptData(const std::vector<char>& data) const {
        if (encryptionAlgorithm_ == EncryptionAlgorithm::None) {
            return data;
        }
        
        std::vector<char> encrypted = data;
        for (size_t i = 0; i < encrypted.size(); ++i) {
            encrypted[i] ^= encryptionKey_[i % encryptionKey_.size()];
        }
        
        return encrypted;
    }
    
    std::vector<char> decryptData(const std::vector<char>& data) const {
        if (encryptionAlgorithm_ == EncryptionAlgorithm::None) {
            return data;
        }
        
        std::vector<char> decrypted = data;
        for (size_t i = 0; i < decrypted.size(); ++i) {
            decrypted[i] ^= encryptionKey_[i % encryptionKey_.size()];
        }
        
        return decrypted;
    }
    
    float calculateIntegrityHash(const std::vector<char>& data) const {
        std::hash<std::vector<char>> hasher;
        return static_cast<float>(hasher(data));
    }
    
    bool verifyIntegrity(const std::vector<char>& data, float expectedHash) const {
        float actualHash = calculateIntegrityHash(data);
        return std::abs(actualHash - expectedHash) < 0.001f;
    }
    
    void saveCheckpointToDisk(const std::string& checkpointId, const std::vector<char>& data) {
        std::string filepath = checkpointDirectory_ + "/" + checkpointId + ".ckpt";
        std::ofstream file(filepath, std::ios::binary);
        if (file) {
            file.write(data.data(), data.size());
            file.close();
        }
    }
    
    bool loadCheckpointFromDisk(const std::string& checkpointId, std::vector<char>& data) {
        std::string filepath = checkpointDirectory_ + "/" + checkpointId + ".ckpt";
        std::ifstream file(filepath, std::ios::binary);
        if (!file) return false;
        
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        data.resize(size);
        if (file.read(data.data(), size)) {
            return true;
        }
        
        return false;
    }
    
    void createIncrementalCheckpoint(const std::string& checkpointId, const std::string& baseCheckpointId) {
        DifferentialEntry entry(checkpointId, baseCheckpointId);
        
        if (checkpointData_.find(baseCheckpointId) != checkpointData_.end()) {
            entry.changes.push_back({baseCheckpointId, "copy"});
            
            std::vector<char> data = checkpointData_[baseCheckpointId];
            
            if (compressionEnabled_) {
                std::vector<char> compressed = compressData(data, compressionAlgorithm_);
                if (compressed.size() < data.size()) {
                    data = compressed;
                    entry.isCompressed = true;
                }
            }
            
            if (encryptionEnabled_) {
                data = encryptData(data);
                entry.isEncrypted = true;
            }
            
            entry.size = data.size();
            checkpointData_[checkpointId] = data;
            differentialCheckpoints_[checkpointId] = entry;
            
            CheckpointMetadata metadata(checkpointId, getCurrentStep(), "Incremental checkpoint from " + baseCheckpointId);
            metadata.size = data.size();
            metadata.isCompressed = entry.isCompressed;
            metadata.isEncrypted = entry.isEncrypted;
            checkpoints_[checkpointId] = metadata;
        }
    }
    
    bool applyIncrementalCheckpoint(const std::string& checkpointId) {
        auto it = differentialCheckpoints_.find(checkpointId);
        if (it == differentialCheckpoints_.end()) return false;
        
        const DifferentialEntry& entry = it->second;
        
        std::string baseCheckpoint = entry.sourceCheckpointId;
        auto baseIt = checkpointData_.find(baseCheckpoint);
        if (baseIt == checkpointData_.end()) return false;
        
        std::vector<char> data = baseIt->second;
        
        if (entry.isEncrypted) {
            data = decryptData(data);
        }
        
        if (entry.isCompressed) {
            data = decompressData(data, compressionAlgorithm_);
        }
        
        checkpointData_[checkpointId] = data;
        
        CheckpointMetadata metadata = checkpoints_[checkpointId];
        metadata.lastAccessTime = std::chrono::system_clock::now();
        checkpoints_[checkpointId] = metadata;
        
        return true;
    }
    
    std::string createDifferentialCheckpoint(const std::string& baseCheckpointId) {
        std::string checkpointId = generateUniqueId();
        
        DifferentialEntry entry(checkpointId, baseCheckpointId);
        
        entry.changes.push_back({baseCheckpointId, "reference"});
        
        if (checkpointData_.find(baseCheckpointId) != checkpointData_.end()) {
            std::vector<char> data = checkpointData_[baseCheckpointId];
            
            if (compressionEnabled_) {
                std::vector<char> compressed = compressData(data, compressionAlgorithm_);
                if (compressed.size() < data.size()) {
                    data = compressed;
                    entry.isCompressed = true;
                }
            }
            
            if (encryptionEnabled_) {
                data = encryptData(data);
                entry.isEncrypted = true;
            }
            
            entry.size = data.size();
            checkpointData_[checkpointId] = data;
            differentialCheckpoints_[checkpointId] = entry;
            
            CheckpointMetadata metadata(checkpointId, getCurrentStep(), "Differential checkpoint from " + baseCheckpointId);
            metadata.size = data.size();
            metadata.isCompressed = entry.isCompressed;
            metadata.isEncrypted = entry.isEncrypted;
            checkpoints_[checkpointId] = metadata;
            
            saveCheckpointToDisk(checkpointId, data);
            
            return checkpointId;
        }
        
        return "";
    }
    
    bool applyDifferentialCheckpoint(const std::string& checkpointId) {
        auto it = differentialCheckpoints_.find(checkpointId);
        if (it == differentialCheckpoints_.end()) return false;
        
        const DifferentialEntry& entry = it->second;
        
        std::string baseCheckpoint = entry.sourceCheckpointId;
        auto baseIt = checkpointData_.find(baseCheckpoint);
        if (baseIt == checkpointData_.end()) return false;
        
        std::vector<char> data = baseIt->second;
        
        if (entry.isEncrypted) {
            data = decryptData(data);
        }
        
        if (entry.isCompressed) {
            data = decompressData(data, compressionAlgorithm_);
        }
        
        checkpointData_[checkpointId] = data;
        
        CheckpointMetadata metadata = checkpoints_[checkpointId];
        metadata.lastAccessTime = std::chrono::system_clock::now();
        checkpoints_[checkpointId] = metadata;
        
        return true;
    }
    
    ValidationResult validateCheckpoint(const std::string& checkpointId) const {
        ValidationResult result;
        
        auto metaIt = checkpoints_.find(checkpointId);
        if (metaIt == checkpoints_.end()) {
            result.isValid = false;
            result.errors.push_back("Checkpoint not found");
            return result;
        }
        
        const CheckpointMetadata& metadata = metaIt->second;
        
        std::string filepath = checkpointDirectory_ + "/" + checkpointId + ".ckpt";
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            result.isValid = false;
            result.errors.push_back("Checkpoint file not found on disk");
            return result;
        }
        
        std::vector<char> data;
        if (!loadCheckpointFromDisk(checkpointId, data)) {
            result.isValid = false;
            result.errors.push_back("Failed to load checkpoint data");
            return result;
        }
        
        if (metadata.isEncrypted) {
            std::vector<char> decrypted = decryptData(data);
            if (!verifyIntegrity(decrypted, metadata.integrityHash)) {
                result.isValid = false;
                result.errors.push_back("Checkpoint integrity check failed (encrypted)");
            }
        } else {
            if (!verifyIntegrity(data, metadata.integrityHash)) {
                result.isValid = false;
                result.errors.push_back("Checkpoint integrity check failed");
            }
        }
        
        if (data.size() > maxCheckpointSize_) {
            result.isValid = false;
            result.errors.push_back("Checkpoint size exceeds maximum allowed size");
        }
        
        if (metadata.step == 0) {
            result.warnings.push_back("Checkpoint step is zero");
        }
        
        if (metadata.size == 0) {
            result.warnings.push_back("Checkpoint size is zero");
        }
        
        result.validationReport = "Checkpoint " + checkpointId + " validation: " + 
                                 (result.isValid ? "PASSED" : "FAILED");
        
        return result;
    }
    
    bool validateAllCheckpoints() {
        bool allValid = true;
        std::string validationReport;
        
        for (const auto& pair : checkpoints_) {
            ValidationResult result = validateCheckpoint(pair.first);
            if (!result.isValid) {
                allValid = false;
                validationReport += "Checkpoint " + pair.first + ": FAILED - ";
                for (const std::string& error : result.errors) {
                    validationReport += error + "; ";
                }
                validationReport += "\n";
            }
        }
        
        lastError_ = validationReport.empty() ? "All checkpoints are valid" : validationReport;
        return allValid;
    }
    
    std::string getCheckpointValidationReport() const {
        std::string report;
        
        for (const auto& pair : checkpoints_) {
            ValidationResult result = validateCheckpoint(pair.first);
            report += "Checkpoint " + pair.first + ": " + 
                     (result.isValid ? "VALID" : "INVALID") + "\n";
            
            if (!result.errors.empty()) {
                report += "  Errors: ";
                for (const std::string& error : result.errors) {
                    report += error + ", ";
                }
                report += "\n";
            }
            
            if (!result.warnings.empty()) {
                report += "  Warnings: ";
                for (const std::string& warning : result.warnings) {
                    report += warning + ", ";
                }
                report += "\n";
            }
        }
        
        return report;
    }
    
    void cleanupOldCheckpoints(size_t maxCheckpoints) {
        if (checkpoints_.size() <= maxCheckpoints) return;
        
        std::vector<std::pair<SimulationStep, std::string>> sortedByTime;
        for (const auto& pair : checkpoints_) {
            sortedByTime.emplace_back(pair.second.step, pair.first);
        }
        
        std::sort(sortedByTime.begin(), sortedByTime.end());
        
        size_t toRemove = checkpoints_.size() - maxCheckpoints;
        for (size_t i = 0; i < toRemove; ++i) {
            std::string checkpointId = sortedByTime[i].second;
            
            checkpoints_.erase(checkpointId);
            checkpointData_.erase(checkpointId);
            differentialCheckpoints_.erase(checkpointId);
            
            std::string filepath = checkpointDirectory_ + "/" + checkpointId + ".ckpt";
            std::filesystem::remove(filepath);
            std::filesystem::remove(filepath + ".meta");
        }
    }
    
    void rebuildIndex() {
        checkpoints_.clear();
        checkpointData_.clear();
        differentialCheckpoints_.clear();
        
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDirectory_)) {
            if (entry.path().extension() == ".ckpt") {
                std::string checkpointId = entry.path().stem().string();
                
                CheckpointMetadata metadata(checkpointId, 0, "Restored from disk");
                metadata.size = std::filesystem::file_size(entry.path());
                checkpoints_[checkpointId] = metadata;
            }
        }
    }
};

} // namespace nlm
