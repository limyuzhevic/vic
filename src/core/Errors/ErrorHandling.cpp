#include "ErrorHandling.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>

namespace nlm {

// File operations with error handling
bool fileExists(const std::string& path) {
    try {
        return std::filesystem::exists(path);
    } catch (const std::exception& e) {
        throw FileError("Error checking file existence: " + std::string(e.what()),
                       FileError::ErrorType::ReadFailed,
                       __FILE__, __LINE__, __func__);
    }
}

std::string getFileExtension(const std::string& path) {
    try {
        std::filesystem::path p(path);
        std::string ext = p.extension().string();
        if (!ext.empty() && ext[0] == '.') {
            ext = ext.substr(1);
        }
        return ext;
    } catch (const std::exception& e) {
        throw FileError("Error getting file extension: " + std::string(e.what()),
                       FileError::ErrorType::InvalidPath,
                       __FILE__, __LINE__, __func__);
    }
}

bool isPathTraversal(const std::string& path) {
    try {
        // Check for ".." components
        size_t pos = 0;
        while ((pos = path.find("..", pos)) != std::string::npos) {
            // Check if ".." is not part of a filename
            bool isTraversal = false;
            if (pos == 0) {
                isTraversal = true;
            } else if (path[pos - 1] == '/') {
                isTraversal = true;
            } else if (pos + 2 < path.length() && path[pos + 2] == '/') {
                isTraversal = true;
            }
            
            if (isTraversal) {
                return true;
            }
            pos += 2;
        }
        return false;
    } catch (const std::exception& e) {
        throw FileError("Error checking for path traversal: " + std::string(e.what()),
                       FileError::ErrorType::InvalidPath,
                       __FILE__, __LINE__, __func__);
    }
}

std::string sanitizePath(const std::string& path) {
    try {
        // Remove any ".." components to prevent traversal
        std::string result;
        size_t pos = 0;
        while (pos < path.length()) {
            size_t nextDot = path.find('.', pos);
            if (nextDot == std::string::npos) {
                result += path.substr(pos);
                break;
            }
            
            // Check if this is a ".." 
            if (nextDot + 1 < path.length() && path[nextDot + 1] == '.') {
                // Skip ".." and following separator
                size_t skipTo = nextDot + 2;
                if (skipTo < path.length() && (path[skipTo] == '/' || path[skipTo] == '\\')) {
                    skipTo++;
                }
                pos = skipTo;
                continue;
            }
            
            // Copy everything up to the dot
            result += path.substr(pos, nextDot - pos);
            pos = nextDot;
        }
        
        // Normalize path separators
        std::replace(result.begin(), result.end(), '\\', '/');
        
        return result;
    } catch (const std::exception& e) {
        throw FileError("Error sanitizing path: " + std::string(e.what()),
                       FileError::ErrorType::InvalidPath,
                       __FILE__, __LINE__, __func__);
    }
}

// Validation functions
bool validateFilePath(const std::string& path) {
    if (path.empty()) {
        throw ValidationError("File path cannot be empty", ValidationError::ErrorType::EmptyInput);
    }
    
    if (isPathTraversal(path)) {
        throw ValidationError("Path traversal detected in file path", ValidationError::ErrorType::InvalidFormat);
    }
    
    // Check for null bytes
    if (path.find('\0') != std::string::npos) {
        throw ValidationError("File path contains null byte", ValidationError::ErrorType::InvalidFormat);
    }
    
    // Check path length
    if (path.length() > 4096) {
        throw ValidationError("File path too long", ValidationError::ErrorType::OutOfRange);
    }
    
    return true;
}

bool validateConfigValue(const std::string& key, const std::string& value) {
    if (key.empty()) {
        throw ValidationError("Config key cannot be empty", ValidationError::ErrorType::EmptyInput);
    }
    
    if (value.empty()) {
        throw ValidationError("Config value cannot be empty for key: " + key, ValidationError::ErrorType::EmptyInput);
    }
    
    // Check for duplicates
    if (key.find_first_of("()[]{}<> ") != std::string::npos) {
        throw ValidationError("Config key contains invalid characters: " + key, ValidationError::ErrorType::InvalidFormat);
    }
    
    // Check key length
    if (key.length() > 128) {
        throw ValidationError("Config key too long: " + key, ValidationError::ErrorType::OutOfRange);
    }
    
    // Check value length
    if (value.length() > 1024 * 1024) {  // 1MB max
        throw ValidationError("Config value too large for key: " + key, ValidationError::ErrorType::OutOfRange);
    }
    
    return true;
}

bool validateMemorySize(size_t size) {
    if (size > SIZE_MAX / 4) {  // Leave room for alignment and overhead
        throw MemoryError("Requested memory size too large", MemoryError::ErrorType::OutOfMemory);
    }
    return true;
}

bool validateNeuronCount(size_t count) {
    if (count == 0) {
        throw ValidationError("Neuron count must be positive", ValidationError::ErrorType::OutOfRange);
    }
    if (count > 1000000) {  // 1 million neurons max
        throw ValidationError("Neuron count too large", ValidationError::ErrorType::OutOfRange);
    }
    return true;
}

bool validateLearningRate(float rate) {
    if (std::isnan(rate) || std::isinf(rate)) {
        throw ValidationError("Learning rate must be a finite number", ValidationError::ErrorType::InvalidFormat);
    }
    if (rate < -1.0f || rate > 1.0f) {
        throw ValidationError("Learning rate out of valid range [-1.0, 1.0]", ValidationError::ErrorType::OutOfRange);
    }
    return true;
}

bool validateConnectionProbability(float prob) {
    if (std::isnan(prob) || std::isinf(prob)) {
        throw ValidationError("Connection probability must be a finite number", ValidationError::ErrorType::InvalidFormat);
    }
    if (prob < 0.0f || prob > 1.0f) {
        throw ValidationError("Connection probability must be in range [0.0, 1.0]", ValidationError::ErrorType::OutOfRange);
    }
    return true;
}

bool validateTimeStep(double timestep) {
    if (std::isnan(timestep) || std::isinf(timestep)) {
        throw ValidationError("Time step must be a finite number", ValidationError::ErrorType::InvalidFormat);
    }
    if (timestep <= 0.0 || timestep > 1.0) {
        throw ValidationError("Time step must be in range (0.0, 1.0]", ValidationError::ErrorType::OutOfRange);
    }
    return true;
}

bool validateLearningRateRange(float rate) {
    if (std::isnan(rate) || std::isinf(rate)) {
        throw ValidationError("Learning rate must be a finite number", ValidationError::ErrorType::InvalidFormat);
    }
    if (rate < 0.0f) {
        throw ValidationError("Learning rate cannot be negative", ValidationError::ErrorType::OutOfRange);
    }
    return true;
}

bool validateNeuronId(const NeuronId& id) {
    if (id.value == 0) {
        throw ValidationError("Neuron ID cannot be zero", ValidationError::ErrorType::InvalidFormat);
    }
    if (id.value == UINT64_MAX) {
        throw ValidationError("Neuron ID is invalid (max value)", ValidationError::ErrorType::InvalidFormat);
    }
    return true;
}

bool validateSynapseId(const SynapseId& id) {
    if (id.value == 0) {
        throw ValidationError("Synapse ID cannot be zero", ValidationError::ErrorType::InvalidFormat);
    }
    if (id.value == UINT64_MAX) {
        throw ValidationError("Synapse ID is invalid (max value)", ValidationError::ErrorType::InvalidFormat);
    }
    return true;
}

bool validateConfigKey(const std::string& key) {
    if (key.empty()) {
        throw ValidationError("Config key cannot be empty", ValidationError::ErrorType::EmptyInput);
    }
    
    // Check for reserved characters
    static const std::string reservedChars = "()[]{}<>:;=,\"'`~!@#$%^&*|\\?/ \\t\\n\\r";
    for (char c : key) {
        if (reservedChars.find(c) != std::string::npos) {
            throw ValidationError("Config key contains invalid character: " + std::string(1, c), ValidationError::ErrorType::InvalidFormat);
        }
    }
    
    return true;
}

bool validateConfigValueType(const std::string& key, const ConfigValue& value) {
    // Basic type validation - could be extended based on key-specific requirements
    return true;
}

// Configuration validation
bool validateConfig(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        validateConfigKey(key);
        validateConfigValue(key, value);
        
        // Type-specific validation
        if (key == "learning_rate") {
            try {
                float rate = std::stof(value);
                validateLearningRate(rate);
            } catch (const std::exception&) {
                throw ConfigError("Invalid learning rate format: " + value, ConfigError::ErrorType::InvalidValue);
            }
        } else if (key == "neuron_count") {
            try {
                size_t count = std::stoull(value);
                validateNeuronCount(count);
            } catch (const std::exception&) {
                throw ConfigError("Invalid neuron count format: " + value, ConfigError::ErrorType::InvalidValue);
            }
        } else if (key == "connection_probability") {
            try {
                float prob = std::stof(value);
                validateConnectionProbability(prob);
            } catch (const std::exception&) {
                throw ConfigError("Invalid connection probability format: " + value, ConfigError::ErrorType::InvalidValue);
            }
        }
        
        return true;
    } catch (const ConfigError& e) {
        throw;
    } catch (const ValidationError& e) {
        throw ConfigError(e.getMessage(), ConfigError::ErrorType::ValidationFailed);
    }
}

// Memory allocation safety
void* safeAlloc(size_t size) {
    if (size == 0) {
        throw MemoryError("Cannot allocate zero bytes", MemoryError::ErrorType::InvalidAddress);
    }
    
    if (!validateMemorySize(size)) {
        throw MemoryError("Memory size validation failed", MemoryError::ErrorType::OutOfMemory);
    }
    
    void* ptr = std::aligned_alloc(64, size);  // 64-byte alignment for performance
    if (!ptr) {
        throw MemoryError("Memory allocation failed: " + std::to_string(size) + " bytes",
                         MemoryError::ErrorType::AllocationFailed);
    }
    
    return ptr;
}

template<typename T>
T* safeAllocTyped(size_t count) {
    if (count == 0) {
        throw MemoryError("Cannot allocate zero elements", MemoryError::ErrorType::InvalidAddress);
    }
    
    size_t size = count * sizeof(T);
    if (size / sizeof(T) != count) {  // Check for overflow
        throw MemoryError("Memory allocation size overflow", MemoryError::ErrorType::OutOfMemory);
    }
    
    return static_cast<T*>(safeAlloc(size));
}

// Error handling utilities
std::string getErrorMessage(const NLMBasicError& e) {
    std::ostringstream oss;
    oss << e.errorType() << ": " << e.what();
    if (!e.getFile().empty()) {
        oss << " (" << e.getFile();
        if (e.getLine() > 0) {
            oss << ":" << e.getLine();
        }
        if (!e.getFunction().empty()) {
            oss << " in " << e.getFunction();
        }
        oss << ")";
    }
    return oss.str();
}

void logError(const NLMBasicError& e) {
    std::cerr << "ERROR: " << getErrorMessage(e) << std::endl;
    // Could be extended to write to log file
}

void logWarning(const std::string& message) {
    std::cerr << "WARNING: " << message << std::endl;
}

void logInfo(const std::string& message) {
    std::cout << "INFO: " << message << std::endl;
}

// SafeFile implementation
SafeFile::SafeFile(const std::string& path, Mode mode) : file_(nullptr), path_(path) {
    try {
        validateFilePath(path_);
        
        const char* modeStr;
        switch (mode) {
            case Mode::Read: modeStr = "rb"; break;
            case Mode::Write: modeStr = "wb"; break;
            case Mode::Append: modeStr = "ab"; break;
            default: throw ValidationError("Invalid file mode");
        }
        
        file_ = std::fopen(path_.c_str(), modeStr);
        if (!file_) {
            throw FileError("Failed to open file: " + path_, FileError::ErrorType::FileNotFound);
        }
    } catch (const FileError& e) {
        throw;
    } catch (const ValidationError& e) {
        throw FileError(e.getMessage(), FileError::ErrorType::InvalidPath);
    }
}

SafeFile::~SafeFile() {
    close();
}

std::string SafeFile::readLine() {
    if (!file_) {
        throw FileError("File not open", FileError::ErrorType::FileNotFound);
    }
    
    std::string line;
    char buffer[4096];
    if (std::fgets(buffer, sizeof(buffer), file_)) {
        line = buffer;
        // Remove trailing newline
        if (!line.empty() && line.back() == '\n') {
            line.pop_back();
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
        }
    } else if (feof(file_)) {
        return "";  // EOF reached
    } else {
        throw FileError("Failed to read line from file", FileError::ErrorType::ReadFailed);
    }
    
    return line;
}

bool SafeFile::readLine(std::string& line) {
    line.clear();
    if (!file_) {
        return false;
    }
    
    char buffer[4096];
    if (std::fgets(buffer, sizeof(buffer), file_)) {
        line = buffer;
        // Remove trailing newline
        if (!line.empty() && line.back() == '\n') {
            line.pop_back();
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
        }
        return true;
    } else if (feof(file_)) {
        return false;  // EOF reached
    } else {
        return false;  // Error
    }
}

bool SafeFile::writeLine(const std::string& line) {
    if (!file_) {
        return false;
    }
    
    std::string lineWithNewline = line;
    if (!lineWithNewline.empty() && lineWithNewline.back() != '\n') {
        lineWithNewline += '\n';
    }
    
    size_t written = std::fwrite(lineWithNewline.c_str(), 1, lineWithNewline.length(), file_);
    return written == lineWithNewline.length();
}

bool SafeFile::write(const std::string& data) {
    if (!file_) {
        return false;
    }
    
    size_t written = std::fwrite(data.c_str(), 1, data.length(), file_);
    return written == data.length();
}

size_t SafeFile::write(const void* data, size_t size) {
    if (!file_) {
        return 0;
    }
    
    return std::fwrite(data, 1, size, file_);
}

size_t SafeFile::read(void* data, size_t size) {
    if (!file_) {
        return 0;
    }
    
    return std::fread(data, 1, size, file_);
}

bool SafeFile::seek(long offset, int whence) {
    if (!file_) {
        return false;
    }
    
    return std::fseek(file_, offset, whence) == 0;
}

long SafeFile::tell() {
    if (!file_) {
        return -1;
    }
    
    return std::ftell(file_);
}

bool SafeFile::close() {
    if (file_) {
        int result = std::fclose(file_);
        file_ = nullptr;
        return result == 0;
    }
    return true;
}

SafeFile SafeFile::createBackup(const std::string& path) {
    try {
        validateFilePath(path);
        
        std::string backupPath = path + ".backup";
        SafeFile backup(backupPath, Mode::Write);
        
        // Copy original file to backup
        SafeFile source(path, Mode::Read);
        char buffer[4096];
        size_t bytesRead;
        while ((bytesRead = source.read(buffer, sizeof(buffer))) > 0) {
            backup.write(buffer, bytesRead);
        }
        
        backup.close();
        source.close();
        
        return backup;
    } catch (const FileError& e) {
        throw;
    } catch (const ValidationError& e) {
        throw FileError(e.getMessage(), FileError::ErrorType::InvalidPath);
    }
}

SafeFile SafeFile::openForRead(const std::string& path) {
    return SafeFile(path, Mode::Read);
}

SafeFile SafeFile::openForWrite(const std::string& path, bool createBackup) {
    if (createBackup && fileExists(path)) {
        createBackup(path).close();  // Create backup if file exists
    }
    return SafeFile(path, Mode::Write);
}

// Atomic file operations
bool atomicWrite(const std::string& path, const std::string& content) {
    try {
        validateFilePath(path);
        
        std::string tempPath = path + ".tmp";
        
        // Write to temporary file
        SafeFile::openForWrite(tempPath).write(content);
        
        // If target exists, rename (atomic on POSIX)
        if (fileExists(path)) {
            std::remove(path.c_str());
        }
        
        std::rename(tempPath.c_str(), path.c_str());
        
        return true;
    } catch (const FileError& e) {
        throw;
    } catch (const ValidationError& e) {
        throw FileError(e.getMessage(), FileError::ErrorType::InvalidPath);
    }
}

bool atomicWrite(const std::string& path, const std::vector<char>& data) {
    try {
        validateFilePath(path);
        
        std::string tempPath = path + ".tmp";
        
        // Write to temporary file
        SafeFile::openForWrite(tempPath).write(data.data(), data.size());
        
        // If target exists, rename (atomic on POSIX)
        if (fileExists(path)) {
            std::remove(path.c_str());
        }
        
        std::rename(tempPath.c_str(), path.c_str());
        
        return true;
    } catch (const FileError& e) {
        throw;
    } catch (const ValidationError& e) {
        throw FileError(e.getMessage(), FileError::ErrorType::InvalidPath);
    }
}

std::string atomicRead(const std::string& path) {
    try {
        validateFilePath(path);
        
        if (!fileExists(path)) {
            throw FileError("File not found: " + path, FileError::ErrorType::FileNotFound);
        }
        
        SafeFile file(path, SafeFile::Mode::Read);
        
        std::string content;
        std::string line;
        while (file.readLine(line)) {
            content += line + '\n';
        }
        
        return content;
    } catch (const FileError& e) {
        throw;
    } catch (const ValidationError& e) {
        throw FileError(e.getMessage(), FileError::ErrorType::InvalidPath);
    }
}

} // namespace nlm
