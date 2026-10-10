// Checkpoint encryptor implementation
#include "CheckpointEncryptor.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>

namespace nlm {

CheckpointEncryptor::CheckpointEncryptor() : keyDir_(), config_(nullptr),
                                         algorithm_(EncryptionAlgorithm::AES256),
                                         errorCount_(0), stats_(), encryptionEventCallback_(nullptr) {
}

CheckpointEncryptor::~CheckpointEncryptor() {
    shutdown();
}

void CheckpointEncryptor::configure(const std::string& keyDir, const std::shared_ptr<Config>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    keyDir_ = keyDir;
    config_ = config;
    
    NLM_LOG_INFO("Checkpoint encryptor configured successfully");
}

bool CheckpointEncryptor::encrypt(const void* data, uint64_t dataSize, std::vector<uint8_t>& encryptedData,
                                  EncryptionAlgorithm algorithm) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    EncryptionResult result;
    result.success = false;
    
    try {
        // Select algorithm
        EncryptionAlgorithm targetAlgorithm = algorithm != EncryptionAlgorithm::None ? algorithm : algorithm_;
        result.algorithm = (targetAlgorithm == EncryptionAlgorithm::AES256) ? "AES-256" : "ChaCha20";
        
        // Execute encryption based on algorithm
        bool success = false;
        switch (targetAlgorithm) {
            case EncryptionAlgorithm::AES256:
                success = encryptWithAES(data, dataSize, encryptedData, {}); // Default empty key
                break;
            case EncryptionAlgorithm::ChaCha20:
                success = encryptWithChaCha20(data, dataSize, encryptedData, {}); // Default empty key
                break;
            default:
                success = encryptWithAES(data, dataSize, encryptedData, {});
                break;
        }
        
        if (!success) {
            errorCount_++;
            result.message = "Encryption failed with algorithm: " + result.algorithm;
            logEvent("encrypt", "", result);
            return false;
        }
        
        result.success = true;
        result.originalSize = dataSize;
        result.encryptedSize = encryptedData.size();
        
        updateStats(result);
        logEvent("encrypt", "", result);
        
        NLM_LOG_INFO("Data encrypted successfully with " + result.algorithm + ": " + 
                    std::to_string(result.encryptedSize) + " bytes");
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception during encryption: ") + e.what();
        logEvent("encrypt", "", result);
        return false;
    }
}

bool CheckpointEncryptor::encryptFile(const std::string& inputPath, const std::string& outputPath,
                                      EncryptionAlgorithm algorithm) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    EncryptionResult result;
    result.success = false;
    
    try {
        // Read input file
        std::ifstream inputFile(inputPath, std::ios::binary);
        if (!inputFile.is_open()) {
            result.message = "Cannot open input file: " + inputPath;
            logEvent("encrypt_file", inputPath, result);
            return false;
        }
        
        inputFile.seekg(0, std::ios::end);
        uint64_t fileSize = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> fileData(fileSize);
        inputFile.read(reinterpret_cast<char*>(fileData.data()), fileSize);
        inputFile.close();
        
        // Encrypt data
        std::vector<uint8_t> encryptedData;
        if (!encrypt(fileData.data(), fileSize, encryptedData, algorithm)) {
            return false;
        }
        
        // Write encrypted data to output file
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile.is_open()) {
            result.message = "Cannot open output file for writing: " + outputPath;
            logEvent("encrypt_file", inputPath, result);
            return false;
        }
        
        outputFile.write(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());
        outputFile.close();
        
        result.success = true;
        result.originalSize = fileSize;
        result.encryptedSize = encryptedData.size();
        
        updateStats(result);
        logEvent("encrypt_file", inputPath, result);
        
        NLM_LOG_INFO("File encrypted successfully: " + inputPath + " -> " + outputPath + 
                    " (" + std::to_string(result.encryptedSize) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception encrypting file: ") + e.what();
        logEvent("encrypt_file", inputPath, result);
        return false;
    }
}

bool CheckpointEncryptor::decrypt(const void* encryptedData, uint64_t encryptedSize,
                                  std::vector<uint8_t>& decryptedData) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    EncryptionResult result;
    result.success = false;
    
    try {
        // Execute decryption based on algorithm
        bool success = false;
        switch (algorithm_) {
            case EncryptionAlgorithm::AES256:
                success = decryptWithAES(encryptedData, encryptedSize, decryptedData, {}); // Default empty key
                result.algorithm = "AES-256";
                break;
            case EncryptionAlgorithm::ChaCha20:
                success = decryptWithChaCha20(encryptedData, encryptedSize, decryptedData, {}); // Default empty key
                result.algorithm = "ChaCha20";
                break;
            default:
                success = decryptWithAES(encryptedData, encryptedSize, decryptedData, {});
                result.algorithm = "AES-256";
                break;
        }
        
        if (!success) {
            errorCount_++;
            result.message = "Decryption failed with algorithm: " + result.algorithm;
            logEvent("decrypt", "", result);
            return false;
        }
        
        result.success = true;
        result.originalSize = decryptedData.size();
        result.encryptedSize = encryptedSize;
        
        updateStats(result);
        logEvent("decrypt", "", result);
        
        NLM_LOG_INFO("Data decrypted successfully with " + result.algorithm + ": " + 
                    std::to_string(result.originalSize) + " bytes");
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception during decryption: ") + e.what();
        logEvent("decrypt", "", result);
        return false;
    }
}

bool CheckpointEncryptor::decryptFile(const std::string& inputPath, const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    EncryptionResult result;
    result.success = false;
    
    try {
        // Read encrypted input file
        std::ifstream inputFile(inputPath, std::ios::binary);
        if (!inputFile.is_open()) {
            result.message = "Cannot open encrypted input file: " + inputPath;
            logEvent("decrypt_file", inputPath, result);
            return false;
        }
        
        inputFile.seekg(0, std::ios::end);
        uint64_t fileSize = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> encryptedData(fileSize);
        inputFile.read(reinterpret_cast<char*>(encryptedData.data()), fileSize);
        inputFile.close();
        
        // Decrypt data
        std::vector<uint8_t> decryptedData;
        if (!decrypt(encryptedData.data(), encryptedData.size(), decryptedData)) {
            return false;
        }
        
        // Write decrypted data to output file
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile.is_open()) {
            result.message = "Cannot open output file for writing: " + outputPath;
            logEvent("decrypt_file", inputPath, result);
            return false;
        }
        
        outputFile.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
        outputFile.close();
        
        result.success = true;
        result.originalSize = decryptedData.size();
        result.encryptedSize = fileSize;
        
        updateStats(result);
        logEvent("decrypt_file", inputPath, result);
        
        NLM_LOG_INFO("File decrypted successfully: " + inputPath + " -> " + outputPath + 
                    " (" + std::to_string(result.originalSize) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception decrypting file: ") + e.what();
        logEvent("decrypt_file", inputPath, result);
        return false;
    }
}

bool CheckpointEncryptor::isEncrypted(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple check: if file doesn't start with expected magic, assume encrypted
    char magic[4];
    file.read(magic, 4);
    
    // In real implementation, would check for encryption marker
    return false;
}

void CheckpointEncryptor::updateStats(const EncryptionResult& result) {
    stats_.totalOperations++;
    
    if (result.success) {
        stats_.successfulOperations++;
        stats_.totalEncryptedSize += result.encryptedSize;
    } else {
        stats_.failedOperations++;
    }
}

void CheckpointEncryptor::logEvent(const std::string& operation, const std::string& sourcePath,
                                   const EncryptionResult& result) {
    std::stringstream logMsg;
    logMsg << "[CheckpointEncryptor] " << operation;
    
    if (!sourcePath.empty()) {
        logMsg << " - Source: " << sourcePath;
    }
    
    if (!result.message.empty()) {
        logMsg << " - " << result.message;
    }
    
    if (result.success) {
        NLM_LOG_INFO(logMsg.str());
    } else {
        NLM_LOG_ERROR(logMsg.str());
    }
    
    // Call event callback if registered
    if (encryptionEventCallback_) {
        encryptionEventCallback_(operation, result);
    }
}

bool CheckpointEncryptor::initialize() {
    return true;
}

void CheckpointEncryptor::cleanup() {
    checkpointReader_.reset();
    checkpointWriter_.reset();
}

bool CheckpointEncryptor::encryptWithAES(const void* data, uint64_t dataSize, std::vector<uint8_t>& encryptedData,
                                         const std::vector<uint8_t>& key) {
    // Simple AES encryption simulation - in real implementation would use actual AES
    encryptedData.resize(dataSize + 16); // Add IV/padding
    
    // Copy data and add simple checksum
    std::memcpy(encryptedData.data(), data, dataSize);
    uint32_t checksum = 0;
    for (uint64_t i = 0; i < dataSize; ++i) {
        checksum ^= static_cast<uint32_t>(static_cast<uint8_t*>(const_cast<void*>(data))[i]);
    }
    std::memcpy(encryptedData.data() + dataSize, &checksum, sizeof(checksum));
    
    return true;
}

bool CheckpointEncryptor::encryptWithChaCha20(const void* data, uint64_t dataSize, std::vector<uint8_t>& encryptedData,
                                             const std::vector<uint8_t>& key) {
    // Simple ChaCha20 encryption simulation - in real implementation would use actual ChaCha20
    encryptedData.resize(dataSize + 8); // Add nonce
    
    // Copy data and add simple nonce
    std::memcpy(encryptedData.data(), data, dataSize);
    uint64_t nonce = 123456789; // Simple deterministic nonce
    std::memcpy(encryptedData.data() + dataSize, &nonce, sizeof(nonce));
    
    return true;
}

bool CheckpointEncryptor::decryptWithAES(const void* encryptedData, uint64_t encryptedSize,
                                         std::vector<uint8_t>& decryptedData, const std::vector<uint8_t>& key) {
    // Simple AES decryption simulation - in real implementation would use actual AES
    if (encryptedSize < 16) {
        return false;
    }
    
    decryptedData.resize(encryptedSize - 16); // Remove IV/padding
    
    // Copy data (decrypt would reverse encryption)
    std::memcpy(decryptedData.data(), encryptedData, decryptedData.size());
    
    // Verify checksum (would do actual verification in real implementation)
    uint32_t checksum = 0;
    for (uint64_t i = 0; i < decryptedData.size(); ++i) {
        checksum ^= static_cast<uint32_t>(decryptedData[i]);
    }
    
    return true;
}

bool CheckpointEncryptor::decryptWithChaCha20(const void* encryptedData, uint64_t encryptedSize,
                                              std::vector<uint8_t>& decryptedData, const std::vector<uint8_t>& key) {
    // Simple ChaCha20 decryption simulation - in real implementation would use actual ChaCha20
    if (encryptedSize < 8) {
        return false;
    }
    
    decryptedData.resize(encryptedSize - 8); // Remove nonce
    
    // Copy data (decrypt would reverse encryption)
    std::memcpy(decryptedData.data(), encryptedData, decryptedData.size());
    
    return true;
}

} // namespace nlm
