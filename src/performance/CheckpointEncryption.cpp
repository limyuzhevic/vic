#include "CheckpointEncryption.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandler.hpp"

namespace nlm {

// KeyDerivation implementation

std::vector<uint8_t> KeyDerivation::deriveKey(const std::string& password,
                                            const std::vector<uint8_t>& salt,
                                            uint32_t iterations,
                                            size_t keyLength) {
    // Simple PBKDF2 implementation (placeholder - would use proper OpenSSL PBKDF2)
    std::vector<uint8_t> key(keyLength, 0);
    
    // This is a simplified placeholder - real implementation would use OpenSSL's
    // EVP_PBE_derive or similar for proper PBKDF2
    
    // For now, use a simple hash-based derivation
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }
    
    // Initialize HMAC-SHA256
    const EVP_MD* md = EVP_sha256();
    
    if (EVP_DigestInit_ex(ctx, md, nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize digest");
    }
    
    // Derive key using PBKDF2 (simplified)
    unsigned char derived_key[EVP_MAX_KEY_LENGTH];
    unsigned int derived_len = 0;
    
    // Use EVP_PBE_derive for proper PBKDF2
    if (EVP_PBE_derive(ctx, derived_key, &derived_len,
                       reinterpret_cast<const unsigned char*>(password.data()),
                       password.size(),
                       reinterpret_cast<const unsigned char*>(salt.data()),
                       salt.size(),
                       EVP_sha256(), iterations) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("PBKDF2 key derivation failed");
    }
    
    // Copy derived key
    size_t copy_len = std::min<size_t>(derived_len, keyLength);
    std::copy(derived_key, derived_key + copy_len, key.begin());
    
    EVP_MD_CTX_free(ctx);
    return key;
}

std::vector<uint8_t> KeyDerivation::generateSalt(size_t length) {
    std::vector<uint8_t> salt(length);
    if (RAND_bytes(salt.data(), static_cast<int>(length)) != 1) {
        throw std::runtime_error("Failed to generate random salt");
    }
    return salt;
}

std::vector<uint8_t> KeyDerivation::generateIV(size_t length) {
    std::vector<uint8_t> iv(length);
    if (RAND_bytes(iv.data(), static_cast<int>(length)) != 1) {
        throw std::runtime_error("Failed to generate random IV");
    }
    return iv;
}

std::vector<uint8_t> KeyDerivation::hashPassword(const std::string& password,
                                                 const std::vector<uint8_t>& salt,
                                                 uint32_t iterations) {
    // Derive a backup key from password (similar to main key but with fewer iterations)
    return deriveKey(password, salt, iterations, 32);
}

// EncryptionEngine implementation

EncryptionEngine::EncryptionEngine(const std::vector<uint8_t>& key,
                                 const std::vector<uint8_t>& iv,
                                 EncryptionMode mode,
                                 const std::vector<uint8_t>& aad)
    : key_(key), iv_(iv), mode_(mode), aad_(aad) {}

bool EncryptionEngine::encrypt(const void* data, size_t size, std::vector<uint8_t>& output) {
    if (!data || size == 0) {
        return false;
    }
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return false;
    }
    
    // Select cipher based on mode
    const EVP_CIPHER* cipher = nullptr;
    switch (mode_) {
        case EncryptionMode::CBC:
            cipher = EVP_AES_256_CBC();
            break;
        case EncryptionMode::GCM:
            cipher = EVP_AES_256_GCM();
            break;
        case EncryptionMode::XTS:
            cipher = EVP_AES_256_XTS();
            break;
        default:
            EVP_CIPHER_CTX_free(ctx);
            return false;
    }
    
    // Initialize encryption
    if (EVP_EncryptInit_ex(ctx, cipher, nullptr, key_.data(), iv_.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // Handle additional authenticated data for GCM
    if (mode_ == EncryptionMode::GCM && !aad_.empty()) {
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_AAD,
                                static_cast<int>(aad_.size()),
                                const_cast<unsigned char*>(aad_.data())) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
    }
    
    // Calculate output size (can be larger than input)
    int outlen = 0;
    if (EVP_EncryptUpdate(ctx, nullptr, &outlen, nullptr, 0) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // Output includes IV, tag (for GCM), and ciphertext
    output.resize(iv_.size());
    if (mode_ == EncryptionMode::GCM) {
        output.resize(iv_.size() + 16); // Add tag
    }
    output.insert(output.end(), size, 0);
    
    // Encrypt data
    unsigned char* output_data = output.data() + iv_.size();
    if (mode_ == EncryptionMode::GCM) {
        output_data = output.data(); // Tag is first for GCM
    }
    
    int ciphertext_len = 0;
    if (EVP_EncryptUpdate(ctx, output_data, &ciphertext_len,
                          static_cast<const unsigned char*>(data), size) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    output.resize(iv_.size() + ciphertext_len);
    if (mode_ == EncryptionMode::GCM) {
        output.resize(iv_.size() + 16 + ciphertext_len);
    }
    
    // Finalization
    int final_len = 0;
    if (EVP_EncryptFinal_ex(ctx, output_data + ciphertext_len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    output.resize(iv_.size() + ciphertext_len + final_len);
    if (mode_ == EncryptionMode::GCM) {
        output.resize(iv_.size() + 16 + ciphertext_len + final_len);
    }
    
    // Get authentication tag for GCM
    if (mode_ == EncryptionMode::GCM && tag_.empty()) {
        tag_.resize(16);
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag_.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
    }
    
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool EncryptionEngine::decrypt(const void* data, size_t size, std::vector<uint8_t>& output) {
    if (!data || size == 0) {
        return false;
    }
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return false;
    }
    
    // Select cipher based on mode
    const EVP_CIPHER* cipher = nullptr;
    switch (mode_) {
        case EncryptionMode::CBC:
            cipher = EVP_AES_256_CBC();
            break;
        case EncryptionMode::GCM:
            cipher = EVP_AES_256_GCM();
            break;
        case EncryptionMode::XTS:
            cipher = EVP_AES_256_XTS();
            break;
        default:
            EVP_CIPHER_CTX_free(ctx);
            return false;
    }
    
    // For GCM, the tag is at the end
    size_t tag_size = 0;
    size_t ciphertext_size = size;
    
    if (mode_ == EncryptionMode::GCM) {
        if (size < 16) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        tag_size = 16;
        ciphertext_size = size - tag_size;
    }
    
    // Copy IV and ciphertext to output buffer
    std::vector<unsigned char> data_copy(static_cast<const unsigned char*>(data),
                                          static_cast<const unsigned char*>(data) + size);
    
    // Initialize decryption
    unsigned char* iv_ptr = nullptr;
    unsigned char* ciphertext_ptr = nullptr;
    
    if (mode_ == EncryptionMode::GCM) {
        iv_ptr = data_copy.data();
        ciphertext_ptr = data_copy.data() + iv_.size();
    } else {
        iv_ptr = data_copy.data();
        ciphertext_ptr = data_copy.data();
    }
    
    if (EVP_DecryptInit_ex(ctx, cipher, nullptr, key_.data(), iv_ptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // Set tag for GCM
    if (mode_ == EncryptionMode::GCM) {
        if (tag_.empty()) {
            tag_ = std::vector<uint8_t>(data_copy.end() - tag_size, data_copy.end());
        }
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag_size, tag_.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
    }
    
    // Handle additional authenticated data for GCM
    if (mode_ == EncryptionMode::GCM && !aad_.empty()) {
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_AAD,
                                static_cast<int>(aad_.size()),
                                const_cast<unsigned char*>(aad_.data())) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
    }
    
    // Calculate output size
    int outlen = 0;
    if (EVP_DecryptUpdate(ctx, nullptr, &outlen, nullptr, 0) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    output.resize(outlen + 1); // +1 for finalization
    
    // Decrypt data
    int plaintext_len = 0;
    if (EVP_DecryptUpdate(ctx, output.data(), &plaintext_len,
                          ciphertext_ptr, ciphertext_size) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // Finalization
    int final_len = 0;
    if (EVP_DecryptFinal_ex(ctx, output.data() + plaintext_len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    output.resize(plaintext_len + final_len);
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

// CheckpointEncryption implementation

CheckpointEncryption::CheckpointEncryption() : isConfigured_(false), isEncrypted_(false),
                                           mode_(EncryptionMode::GCM), keyDerivationIterations_(100000) {}

bool CheckpointEncryption::configure(const std::string& password,
                                    EncryptionMode mode,
                                    uint32_t iterations,
                                    bool enableBackupEncryption) {
    if (password.empty()) {
        ErrorHandler::getInstance().logError(
            ErrorCode::InvalidParameter,
            "Password cannot be empty for encryption configuration",
            __FILE__, __LINE__, "CheckpointEncryption::configure",
            LogLevel::Critical
        );
        return false;
    }
    
    password_ = password;
    mode_ = mode;
    keyDerivationIterations_ = iterations;
    isConfigured_ = true;
    
    // Generate backup password if requested
    if (enableBackupEncryption) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist('a', 'z');
        
        std::string backup = "backup_";
        for (int i = 0; i < 16; ++i) {
            backup += static_cast<char>(dist(gen));
        }
        
        backupPassword_ = backup;
        NLM_LOG_INFO("Backup password generated: " + backupPassword_);
    }
    
    // Initialize metadata
    metadata_.version = 1;
    metadata_.mode = mode;
    metadata_.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    metadata_.salt = KeyDerivation::generateSalt();
    metadata_.iv = KeyDerivation::generateIV();
    metadata_.keyDerivationIterations = iterations;
    metadata_.checksum = 0;
    
    return true;
}

bool CheckpointEncryption::encrypt(const void* data, size_t size,
                                   std::vector<uint8_t>& output,
                                   const std::string& backupPassword) {
    if (!isConfigured_ || !data || size == 0) {
        return false;
    }
    
    bool useBackupKey = !backupPassword.empty();
    return encryptInternal(data, size, output, useBackupKey);
}

bool CheckpointEncryption::decrypt(const void* data, size_t size,
                                   std::vector<uint8_t>& output,
                                   const std::string& password) {
    if (!isConfigured_ || !data || size == 0) {
        return false;
    }
    
    // Try primary password first
    if (!password.empty() && password == password_) {
        if (decryptInternal(data, size, output, false)) {
            return true;
        }
    }
    
    // Try backup password
    if (!backupPassword_.empty() && (!password.empty() && password == backupPassword_)) {
        if (decryptInternal(data, size, output, true)) {
            return true;
        }
    }
    
    // Try with provided password if different from stored
    if (!password.empty() && password != password_ && password != backupPassword_) {
        // Temporarily configure with new password and try decryption
        std::string tempPassword = password_;
        password_ = password;
        bool result = decryptInternal(data, size, output, false);
        password_ = tempPassword;
        return result;
    }
    
    return false;
}

bool CheckpointEncryption::encryptInternal(const void* data, size_t size,
                                          std::vector<uint8_t>& output,
                                          bool useBackupKey) {
    // Derive key
    const std::vector<uint8_t>& currentPassword = useBackupKey ? backupPassword_ : password_;
    currentKey_ = KeyDerivation::deriveKey(currentPassword, metadata_.salt, keyDerivationIterations_);
    
    // Create encryption engine
    EncryptionEngine engine(currentKey_, metadata_.iv, mode_);
    
    // Encrypt data
    if (!engine.encrypt(data, size, output)) {
        return false;
    }
    
    // Set tag for GCM mode
    if (mode_ == EncryptionMode::GCM) {
        metadata_.tag = engine.getTag();
    }
    
    // Compute checksum of encrypted data
    metadata_.checksum = computeChecksum(output.data(), output.size());
    
    // Mark as encrypted
    isEncrypted_ = true;
    
    return true;
}

bool CheckpointEncryption::decryptInternal(const void* data, size_t size,
                                          std::vector<uint8_t>& output,
                                          bool useBackupKey) {
    // Create encryption engine
    EncryptionEngine engine(currentKey_, metadata_.iv, mode_);
    
    // Decrypt data
    if (!engine.decrypt(data, size, output)) {
        return false;
    }
    
    // Validate checksum
    if (metadata_.checksum != computeChecksum(output.data(), output.size())) {
        ErrorHandler::getInstance().logError(
            ErrorCode::DataCorruption,
            "Decryption failed: data checksum validation failed",
            __FILE__, __LINE__, "CheckpointEncryption::decryptInternal",
            LogLevel::Error
        );
        return false;
    }
    
    return true;
}

std::string CheckpointEncryption::generateBackupKey() const {
    return backupPassword_;
}

void CheckpointEncryption::setBackupKey(const std::string& backupKey) {
    backupPassword_ = backupKey;
}

bool CheckpointEncryption::rotateKey(const std::string& newPassword) {
    if (!isConfigured_ || newPassword.empty()) {
        return false;
    }
    
    // Generate new metadata
    EncryptionMetadata newMetadata;
    newMetadata.version = metadata_.version;
    newMetadata.mode = mode_;
    newMetadata.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    newMetadata.salt = KeyDerivation::generateSalt();
    newMetadata.iv = KeyDerivation::generateIV();
    newMetadata.keyDerivationIterations = keyDerivationIterations_;
    newMetadata.tag = {}; // Will be regenerated on re-encryption
    newMetadata.checksum = 0;
    
    // Create temporary encryption with new key
    CheckpointEncryption tempEncryption;
    if (!tempEncryption.configure(newPassword, mode_, keyDerivationIterations_)) {
        return false;
    }
    
    // Re-encrypt all existing data (if any)
    // For now, just update configuration
    metadata_ = newMetadata;
    password_ = newPassword;
    
    // Clear backup key on rotation for security
    backupPassword_.clear();
    
    return true;
}

bool CheckpointEncryption::decryptWithFallback(const void* data, size_t size,
                                              std::vector<uint8_t>& output,
                                              const std::string& primaryPassword,
                                              const std::string& backupPassword) {
    // Try primary password
    if (!primaryPassword.empty()) {
        std::string tempPassword = password_;
        password_ = primaryPassword;
        bool result = decryptInternal(data, size, output, false);
        password_ = tempPassword;
        if (result) {
            return true;
        }
    }
    
    // Try backup password
    if (!backupPassword.empty()) {
        std::string tempPassword = password_;
        password_ = backupPassword;
        bool result = decryptInternal(data, size, output, false);
        password_ = tempPassword;
        if (result) {
            return true;
        }
    }
    
    // Try with original password
    if (!password_.empty()) {
        std::string tempPassword = password_;
        password_ = password_;
        bool result = decryptInternal(data, size, output, false);
        password_ = tempPassword;
        if (result) {
            return true;
        }
    }
    
    return false;
}

uint64_t CheckpointEncryption::computeChecksum(const void* data, size_t size) {
    // Simple checksum implementation (would use CRC-64 or similar)
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    uint64_t checksum = 0xFFFFFFFFFFFFFFFFULL;
    
    for (size_t i = 0; i < size; ++i) {
        checksum = (checksum >> 8) ^ CRC64_TABLE[(checksum ^ bytes[i]) & 0xFF];
    }
    
    return checksum ^ 0xFFFFFFFFFFFFFFFFULL;
}

bool CheckpointEncryption::validateMetadata(const EncryptionMetadata& metadata) {
    // Validate metadata structure and consistency
    if (metadata.version == 0 || metadata.salt.empty() || metadata.iv.empty()) {
        return false;
    }
    
    if (metadata.mode != EncryptionMode::CBC && metadata.mode != EncryptionMode::GCM &&
        metadata.mode != EncryptionMode::XTS) {
        return false;
    }
    
    return true;
}

// EncryptedCheckpointWriter implementation

bool EncryptedCheckpointWriter::create(const std::string& filepath,
                                       const std::string& password,
                                       EncryptionMode mode,
                                       bool compress,
                                       uint32_t iterations) {
    if (filepath.empty() || password.empty()) {
        return false;
    }
    
    filepath_ = filepath;
    
    // Configure encryption
    if (!encryption_.configure(password, mode, iterations, true)) {
        return false;
    }
    
    // Open file for writing
    std::ios_base::openmode openMode = std::ios::out | std::ios::binary;
    if (!stream_.open(filepath_, openMode)) {
        ErrorHandler::getInstance().logError(
            ErrorCode::FileIOError,
            "Failed to open checkpoint file for writing: " + filepath_,
            __FILE__, __LINE__, "EncryptedCheckpointWriter::create",
            LogLevel::Error
        );
        return false;
    }
    
    // Write encryption metadata header
    std::vector<uint8_t> metadataBytes;
    // In real implementation, would serialize metadata properly
    metadataBytes.resize(sizeof(CheckpointEncryption::EncryptionMetadata));
    
    return true;
}

bool EncryptedCheckpointWriter::writeData(const void* data, size_t size) {
    if (!stream_.is_open() || !data || size == 0) {
        return false;
    }
    
    // Encrypt data
    std::vector<uint8_t> encryptedData;
    if (!encryption_.encrypt(data, size, encryptedData)) {
        ErrorHandler::getInstance().logError(
            ErrorCode::EncryptionError,
            "Failed to encrypt checkpoint data",
            __FILE__, __LINE__, "EncryptedCheckpointWriter::writeData",
            LogLevel::Error
        );
        return false;
    }
    
    // Write encrypted data to file
    if (!encryptedData.empty()) {
        stream_.write(reinterpret_cast<const char*>(encryptedData.data()),
                     encryptedData.size());
        if (!stream_.good()) {
            ErrorHandler::getInstance().logError(
                ErrorCode::FileIOError,
                "Failed to write encrypted data to checkpoint file",
                __FILE__, __LINE__, "EncryptedCheckpointWriter::writeData",
                LogLevel::Error
            );
            return false;
        }
    }
    
    return true;
}

bool EncryptedCheckpointWriter::finalize() {
    if (!stream_.is_open()) {
        return false;
    }
    
    stream_.close();
    return true;
}

void EncryptedCheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    // Clean up partial file
    std::remove(filepath_.c_str());
}

// EncryptedCheckpointReader implementation

bool EncryptedCheckpointReader::open(const std::string& filepath, const std::string& password) {
    if (filepath.empty() || password.empty()) {
        return false;
    }
    
    filepath_ = filepath;
    
    // Open file for reading
    if (!stream_.open(filepath_, std::ios::in | std::ios::binary)) {
        ErrorHandler::getInstance().logError(
            ErrorCode::FileIOError,
            "Failed to open checkpoint file for reading: " + filepath_,
            __FILE__, __LINE__, "EncryptedCheckpointReader::open",
            LogLevel::Error
        );
        return false;
    }
    
    // Configure encryption
    if (!encryption_.configure(password)) {
        close();
        return false;
    }
    
    // Read metadata header (simplified)
    // In real implementation, would read and deserialize metadata
    
    return true;
}

bool EncryptedCheckpointReader::readData(std::vector<uint8_t>& output, size_t size) {
    if (!stream_.is_open()) {
        return false;
    }
    
    // Read encrypted data from file
    std::vector<uint8_t> encryptedData(size);
    stream_.read(reinterpret_cast<char*>(encryptedData.data()), size);
    
    if (!stream_.good()) {
        ErrorHandler::getInstance().logError(
            ErrorCode::FileIOError,
            "Failed to read encrypted data from checkpoint file",
            __FILE__, __LINE__, "EncryptedCheckpointReader::readData",
            LogLevel::Error
        );
        return false;
    }
    
    // Decrypt data
    if (!encryption_.decrypt(encryptedData.data(), encryptedData.size(), output)) {
        ErrorHandler::getInstance().logError(
            ErrorCode::EncryptionError,
            "Failed to decrypt checkpoint data",
            __FILE__, __LINE__, "EncryptedCheckpointReader::readData",
            LogLevel::Error
        );
        return false;
    }
    
    return true;
}

} // namespace nlm
