#pragma once

/**
 * Encryption utilities for secure checkpointing
 * 
 * Provides AES-256 encryption with password-based key derivation.
 * Supports multiple encryption modes (CBC, GCM) and backup strategies.
 * 
 * Features:
 * - AES-256 encryption for checkpoint files
 * - PBKDF2 key derivation from passwords
 * - Multiple encryption modes (CBC, GCM)
 * - Key management and backup strategies
 * - Graceful fallback for decryption failures
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include <stdexcept>

// OpenSSL headers for encryption (simplified for portability)
#if defined(__linux__) || defined(__APPLE__)
    #include <openssl/evp.h>
    #include <openssl/aes.h>
    #include <openssl/rand.h>
    #include <openssl/kdf.h>
#else
    // Fallback for platforms without OpenSSL
    #include "openssl/aes.h"
    #include "openssl/rand.h"
    #include "openssl/evp.h"
    #include "openssl/kdf.h"
#endif

namespace nlm {

/**
 * Encryption modes
 */
enum class EncryptionMode {
    CBC,  // Cipher Block Chaining
    GCM,  // Galois/Counter Mode (authenticated encryption)
    XTS   // XOR-Threshold mode (for random access)
};

/**
 * Key derivation functions
 */
class KeyDerivation {
public:
    /**
     * Derive encryption key from password using PBKDF2
     * @param password User password
     * @param salt Salt for key derivation
     * @param iterations Number of iterations (higher = more secure)
     * @param keyLength Desired key length in bytes
     * @return Derived key
     */
    static std::vector<uint8_t> deriveKey(const std::string& password,
                                          const std::vector<uint8_t>& salt,
                                          uint32_t iterations = 100000,
                                          size_t keyLength = 32); // 32 bytes = 256 bits
    
    /**
     * Generate random salt
     */
    static std::vector<uint8_t> generateSalt(size_t length = 16);
    
    /**
     * Generate random initialization vector
     */
    static std::vector<uint8_t> generateIV(size_t length = 16);
    
    /**
     * Hash password for backup key derivation
     */
    static std::vector<uint8_t> hashPassword(const std::string& password,
                                             const std::vector<uint8_t>& salt,
                                             uint32_t iterations = 10000);
};

/**
 * AES-256 encryption engine
 */
class EncryptionEngine {
public:
    EncryptionEngine(const std::vector<uint8_t>& key,
                    const std::vector<uint8_t>& iv,
                    EncryptionMode mode = EncryptionMode::CBC,
                    const std::vector<uint8_t>& aad = {});
    
    /**
     * Encrypt data
     */
    bool encrypt(const void* data, size_t size, std::vector<uint8_t>& output);
    
    /**
     * Decrypt data
     */
    bool decrypt(const void* data, size_t size, std::vector<uint8_t>& output);
    
    /**
     * Get tag for GCM mode
     */
    const std::vector<uint8_t>& getTag() const { return tag_; }
    
    /**
     * Set tag for decryption (GCM mode)
     */
    void setTag(const std::vector<uint8_t>& tag) { tag_ = tag; }
    
    /**
     * Get current IV
     */
    const std::vector<uint8_t>& getIV() const { return iv_; }

private:
    std::vector<uint8_t> key_;
    std::vector<uint8_t> iv_;
    EncryptionMode mode_;
    std::vector<uint8_t> aad_;
    std::vector<uint8_t> tag_; // For GCM mode
};

/**
 * Checkpoint encryption with password management
 */
class CheckpointEncryption {
public:
    struct EncryptionMetadata {
        uint32_t version;
        EncryptionMode mode;
        uint64_t timestamp;
        std::vector<uint8_t> salt;
        std::vector<uint8_t> iv;
        uint64_t keyDerivationIterations;
        std::vector<uint8_t> tag; // For GCM mode
        uint64_t checksum; // Checksum of encrypted data
    };

public:
    CheckpointEncryption();
    
    /**
     * Configure encryption with password
     */
    bool configure(const std::string& password,
                   EncryptionMode mode = EncryptionMode::GCM,
                   uint32_t iterations = 100000,
                   bool enableBackupEncryption = true);
    
    /**
     * Encrypt checkpoint data
     */
    bool encrypt(const void* data, size_t size,
                 std::vector<uint8_t>& output,
                 const std::string& backupPassword = "");
    
    /**
     * Decrypt checkpoint data
     */
    bool decrypt(const void* data, size_t size,
                 std::vector<uint8_t>& output,
                 const std::string& password = "");
    
    /**
     * Get encryption metadata
     */
    const EncryptionMetadata& getMetadata() const { return metadata_; }
    
    /**
     * Generate backup key from password
     */
    std::string generateBackupKey() const;
    
    /**
     * Set backup key for emergency recovery
     */
    void setBackupKey(const std::string& backupKey);
    
    /**
     * Check if checkpoint is encrypted
     */
    bool isEncrypted() const { return isConfigured_ && isEncrypted_; }
    
    /**
     * Get current encryption mode
     */
    EncryptionMode getEncryptionMode() const { return mode_; }
    
    /**
     * Check if encryption is configured
     */
    bool isConfigured() const { return isConfigured_; }
    
    /**
     * Rotate encryption key (re-encrypt with new key)
     */
    bool rotateKey(const std::string& newPassword);
    
    /**
     * Decrypt with fallback strategies
     */
    bool decryptWithFallback(const void* data, size_t size,
                            std::vector<uint8_t>& output,
                            const std::string& primaryPassword = "",
                            const std::string& backupPassword = "");

private:
    /**
     * Encrypt with current key
     */
    bool encryptInternal(const void* data, size_t size,
                        std::vector<uint8_t>& output,
                        bool useBackupKey = false);
    
    /**
     * Decrypt with current key
     */
    bool decryptInternal(const void* data, size_t size,
                        std::vector<uint8_t>& output,
                        bool useBackupKey = false);
    
    /**
     * Compute checksum of data
     */
    uint64_t computeChecksum(const void* data, size_t size);
    
    /**
     * Validate metadata integrity
     */
    bool validateMetadata(const EncryptionMetadata& metadata);

private:
    bool isConfigured_;
    bool isEncrypted_;
    EncryptionMode mode_;
    std::string password_;
    std::string backupPassword_;
    EncryptionMetadata metadata_;
    std::vector<uint8_t> currentKey_;
    std::vector<uint8_t> backupKey_;
    uint32_t keyDerivationIterations_;
};

/**
 * Encrypted checkpoint writer
 */
class EncryptedCheckpointWriter {
public:
    EncryptedCheckpointWriter() = default;
    
    /**
     * Create encrypted checkpoint file
     */
    bool create(const std::string& filepath,
                const std::string& password,
                EncryptionMode mode = EncryptionMode::GCM,
                bool compress = true,
                uint32_t iterations = 100000);
    
    /**
     * Write encrypted data
     */
    bool writeData(const void* data, size_t size);
    
    /**
     * Finalize encrypted checkpoint
     */
    bool finalize();
    
    /**
     * Abort encrypted checkpoint
     */
    void abort();
    
    /**
     * Check if file is open
     */
    bool isOpen() const { return stream_.is_open(); }

private:
    std::ofstream stream_;
    std::string filepath_;
    CheckpointEncryption encryption_;
    std::vector<uint8_t> buffer_;
};

/**
 * Encrypted checkpoint reader
 */
class EncryptedCheckpointReader {
public:
    EncryptedCheckpointReader() = default;
    
    /**
     * Open encrypted checkpoint file
     */
    bool open(const std::string& filepath, const std::string& password);
    
    /**
     * Read encrypted data
     */
    bool readData(std::vector<uint8_t>& output, size_t size);
    
    /**
     * Close encrypted checkpoint
     */
    void close() {
        if (stream_.is_open()) {
            stream_.close();
        }
    }
    
    /**
     * Check if file is open
     */
    bool isOpen() const { return stream_.is_open(); }
    
    /**
     * Get encryption metadata
     */
    const CheckpointEncryption::EncryptionMetadata& getMetadata() const { return metadata_; }
    
    /**
     * Get last error
     */
    const std::string& getError() const { return error_; }

private:
    std::ifstream stream_;
    std::string filepath_;
    CheckpointEncryption encryption_;
    CheckpointEncryption::EncryptionMetadata metadata_;
    std::string error_;
};

} // namespace nlm
