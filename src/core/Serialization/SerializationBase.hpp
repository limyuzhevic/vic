#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <functional>
#include <variant>
#include <optional>

namespace nlm {

// Serialization versions
struct SerializationVersion {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    
    SerializationVersion() : major(1), minor(0), patch(0) {}
    SerializationVersion(uint32_t m, uint32_t n, uint32_t p) : major(m), minor(n), patch(p) {}
    
    bool operator==(const SerializationVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    bool operator!=(const SerializationVersion& other) const {
        return !(*this == other);
    }
};

// Compression levels
enum class CompressionLevel {
    None = 0,
    Fast = 1,
    Balanced = 2,
    Best = 3
};

// Serialization flags
enum class SerializationFlags : uint32_t {
    None = 0,
    Compressed = (1 << 0),
    Encrypted = (1 << 1),
    Incremental = (1 << 2),
    Validate = (1 << 3),
    ThreadSafe = (1 << 4)
};

// Plugin interface for custom serializers
class ISerializer {
public:
    virtual ~ISerializer() = default;
    
    virtual SerializationVersion getVersion() const = 0;
    virtual const std::string& getName() const = 0;
    virtual bool canSerialize(const std::type_info& type) const = 0;
    virtual std::vector<uint8_t> serialize(const void* data) = 0;
    virtual bool deserialize(const std::vector<uint8_t>& data, void* out) = 0;
};

// Base serializer with common functionality
class BaseSerializer {
protected:
    SerializationVersion version_;
    CompressionLevel compressionLevel_;
    uint32_t flags_;
    mutable std::mutex mutex_;
    
public:
    BaseSerializer(SerializationVersion version = SerializationVersion(), 
                  CompressionLevel compression = CompressionLevel::Balanced,
                  uint32_t flags = 0)
        : version_(version), compressionLevel_(compression), flags_(flags) {}
    
    virtual ~BaseSerializer() = default;
    
    SerializationVersion getVersion() const { return version_; }
    CompressionLevel getCompressionLevel() const { return compressionLevel_; }
    uint32_t getFlags() const { return flags_; }
    
    void setVersion(SerializationVersion version) { version_ = version; }
    void setCompressionLevel(CompressionLevel level) { compressionLevel_ = level; }
    void setFlags(uint32_t flags) { flags_ = flags; }
    
    // Thread-safe wrapper
    template<typename T, typename Func>
    T executeSafely(Func&& func) const {
        if (flags_ & static_cast<uint32_t>(SerializationFlags::ThreadSafe)) {
            std::lock_guard<std::mutex> lock(mutex_);
        }
        return func();
    }
};

// Serialization statistics
struct SerializationStats {
    uint64_t bytesWritten = 0;
    uint64_t bytesRead = 0;
    uint64_t serializationTimeMs = 0;
    uint64_t deserializationTimeMs = 0;
    uint32_t checkpointCount = 0;
    uint32_t compressionRatio = 0;
    uint32_t errorCount = 0;
    
    void reset() {
        bytesWritten = 0;
        bytesRead = 0;
        serializationTimeMs = 0;
        deserializationTimeMs = 0;
        checkpointCount = 0;
        compressionRatio = 0;
        errorCount = 0;
    }
};

// Plugin manager for custom serializers
class SerializerPluginManager {
private:
    std::vector<std::unique_ptr<ISerializer>> serializers_;
    std::unordered_map<std::type_index, ISerializer*> serializerMap_;
    
public:
    void registerSerializer(std::unique_ptr<ISerializer> serializer) {
        ISerializer* ptr = serializer.get();
        serializers_.push_back(std::move(serializer));
        // Register for all types this serializer can handle
        for (const auto& type : ptr->getSupportedTypes()) {
            serializerMap_[type] = ptr;
        }
    }
    
    ISerializer* getSerializer(const std::type_info& type) const {
        auto it = serializerMap_.find(std::type_index(type));
        return it != serializerMap_.end() ? it->second : nullptr;
    }
    
    std::vector<ISerializer*> getAllSerializers() const {
        std::vector<ISerializer*> result;
        for (const auto& serializer : serializers_) {
            result.push_back(serializer.get());
        }
        return result;
    }
};

// Schema validation for data integrity
class SchemaValidator {
public:
    virtual ~SchemaValidator() = default;
    
    virtual bool validate(const std::vector<uint8_t>& data, SerializationVersion version) = 0;
    virtual std::string getValidationError() const = 0;
};

// Error handling for serialization
class SerializationError : public std::runtime_error {
public:
    enum class ErrorType {
        Unknown,
        VersionMismatch,
        CorruptedData,
        MissingRequiredData,
        InvalidFormat,
        CompressionError,
        EncryptionError,
        Timeout,
        MemoryUsageExceeded
    };
    
    SerializationError(ErrorType type, const std::string& message)
        : std::runtime_error(message), errorType_(type) {}
    
    ErrorType getErrorType() const { return errorType_; }
    
private:
    ErrorType errorType_;
};

} // namespace nlm
