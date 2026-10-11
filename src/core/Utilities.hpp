#pragma once

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <functional>
#include <chrono>
#include <fstream>
#include <filesystem>

namespace nlm {

class NLMException;

// Progress reporting interface
class IProgressReporter {
public:
    virtual ~IProgressReporter() = default;
    
    virtual void onProgress(double progress, const std::string& message) = 0;
    virtual bool shouldCancel() const { return false; }
};

// Default progress reporter that does nothing
class NullProgressReporter : public IProgressReporter {
public:
    void onProgress(double progress, const std::string& message) override {}
    bool shouldCancel() const override { return false; }
};

// Serialization utility for progress tracking
class ProgressReporter : public IProgressReporter {
public:
    ProgressReporter(std::function<void(double, const std::string&)> callback)
        : callback_(callback) {}
    
    void onProgress(double progress, const std::string& message) override {
        if (callback_) {
            callback_(progress, message);
        }
    }
    
    bool shouldCancel() const override {
        return shouldCancel_;
    }
    
    void setCancel(bool cancel) {
        shouldCancel_ = cancel;
    }
    
private:
    std::function<void(double, const std::string&)> callback_;
    bool shouldCancel_ = false;
};

// File utilities with progress reporting
class FileUtils {
public:
    static bool copyFile(const std::string& source, const std::string& destination,
                        std::shared_ptr<IProgressReporter> reporter = nullptr,
                        const std::string& description = "Copying");
    
    static bool backupFile(const std::string& filepath,
                          std::shared_ptr<IProgressReporter> reporter = nullptr);
    
    static bool validateFile(const std::string& filepath,
                             const std::vector<std::string>& allowedExtensions = {});
    
    static bool isFileLocked(const std::string& filepath);
    
    static std::string getFileBackupPath(const std::string& filepath);
};

// Serialization utilities
class SerializationUtils {
public:
    // String encoding/decoding
    static std::string encodeString(const std::string& str);
    static std::string decodeString(const std::string& encoded);
    
    // File encoding detection
    static std::string detectFileEncoding(const std::string& filepath);
    static bool isTextFile(const std::string& filepath);
    static bool isBinaryFile(const std::string& filepath);
    
    // Safe file writing with atomic operations
    static bool writeAtomic(const std::string& filepath, const std::string& content,
                           std::shared_ptr<IProgressReporter> reporter = nullptr);
    
    // Checksum utilities
    static std::string calculateMD5(const std::string& filepath);
    static std::string calculateSHA256(const std::string& filepath);
    
    // File size and space checking
    static uint64_t getFileSize(const std::string& filepath);
    static bool hasDiskSpace(const std::string& filepath, uint64_t requiredBytes);
    
    // Path validation and sanitization
    static std::string sanitizePath(const std::string& path);
    static std::string normalizePath(const std::string& path);
    static bool isPathSafe(const std::string& path);
};

// Configuration utilities
class ConfigUtils {
public:
    // Configuration validation
    static bool validateConfigKey(const std::string& key);
    static bool validateConfigValue(const std::string& key, const std::string& value);
    static std::string suggestSimilarKey(const std::string& key, const std::vector<std::string>& availableKeys);
    
    // Configuration merging
    static bool mergeConfigs(const std::string& baseFile, const std::string& overrideFile,
                           const std::string& outputFile, bool backupOriginal = true);
    
    // Configuration comparison
    static bool configsEqual(const std::string& file1, const std::string& file2);
    static std::string diffConfigs(const std::string& file1, const std::string& file2);
    
    // Configuration optimization
    static std::string optimizeConfig(const std::string& filepath,
                                     const std::map<std::string, std::string>& optimizations);
    
    // Configuration security
    static bool isConfigSecure(const std::string& filepath);
    static bool sanitizeConfigFile(const std::string& filepath);
};

// Neural simulation utilities with error recovery
class NeuralUtils {
public:
    // Error recovery and rollback
    static bool canRecover(const std::string& errorContext);
    static bool performRecovery(const std::string& errorContext);
    static std::string getRecoveryPlan(const std::string& errorContext);
    
    // Validation utilities for neural components
    static bool validateNeuronConfiguration(const std::map<std::string, double>& params);
    static bool validateSynapseConfiguration(const std::map<std::string, double>& params);
    static bool validatePlasticityParameters(const std::map<std::string, double>& params);
    
    // Performance monitoring
    static double getSimulationSpeed() { return simulationSpeed_; }
    static void setSimulationSpeed(double speed) { simulationSpeed_ = speed; }
    static double getMemoryUsage() { return memoryUsage_; }
    static void setMemoryUsage(double usage) { memoryUsage_ = usage; }
    
    // Diagnostic tools
    static std::string getSystemInfo();
    static std::vector<std::string> getDiagnosticReport();
    static bool runDiagnostics();
    
    // Checkpoint and recovery utilities
    static bool validateCheckpoint(const std::string& filepath);
    static bool restoreFromCheckpoint(const std::string& filepath, bool verify = true);
    static bool createCheckpoint(const std::string& filepath, bool compress = false);
    
private:
    static double simulationSpeed_;
    static double memoryUsage_;
};

// Advanced string utilities
class StringUtils {
public:
    // String formatting
    static std::string format(const std::string& format, ...);
    static std::string vformat(const std::string& format, va_list args);
    
    // String manipulation
    static std::string trim(const std::string& str);
    static std::string ltrim(const std::string& str);
    static std::string rtrim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
    
    // Pattern matching
    static bool matchesPattern(const std::string& str, const std::string& pattern);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    
    // Case conversion
    static std::string toUpper(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Hash and encoding
    static std::string sha256(const std::string& str);
    static std::string md5(const std::string& str);
    static std::string base64Encode(const std::string& str);
    static std::string base64Decode(const std::string& str);
    
    // UUID and unique identifiers
    static std::string generateUUID();
    static std::string generateSessionId();
    
    // Text analysis
    static size_t countOccurrences(const std::string& str, const std::string& substring);
    static bool containsWhitespace(const std::string& str);
    static bool isPrintable(const std::string& str);
    static bool isNumeric(const std::string& str);
    
    // Date and time formatting
    static std::string getCurrentTimestamp();
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& timepoint);
    static std::chrono::system_clock::time_point parseTimestamp(const std::string& timestamp);
};

// Math utilities
class MathUtils {
public:
    // Numerical operations
    static double clamp(double value, double min, double max);
    static double lerp(double a, double b, double t);
    static double normalize(double value, double min, double max);
    static double denormalize(double normalized, double min, double max);
    
    // Statistical functions
    static double mean(const std::vector<double>& values);
    static double variance(const std::vector<double>& values);
    static double standardDeviation(const std::vector<double>& values);
    static double median(const std::vector<double>& values);
    
    // Probability and random number generation
    static double random(double min = 0.0, double max = 1.0);
    static int randomInt(int min, int max);
    static bool randomBool(double probability = 0.5);
    
    // Vector operations
    static double dotProduct(const std::vector<double>& a, const std::vector<double>& b);
    static double magnitude(const std::vector<double>& v);
    static double normalizeVector(std::vector<double>& v);
    static double angleBetween(const std::vector<double>& a, const std::vector<double>& b);
    
    // Matrix operations (for neural network computations)
    static std::vector<std::vector<double>> matrixMultiply(const std::vector<std::vector<double>>& A,
                                                          const std::vector<std::vector<double>>& B);
    static std::vector<std::vector<double>> transpose(const std::vector<std::vector<double>>& matrix);
    static std::vector<std::vector<double>> identityMatrix(size_t size);
    
    // Numerical stability
    static bool isNear(double a, double b, double epsilon = 1e-10);
    static bool isPositive(double value, double epsilon = 1e-10);
    static bool isNegative(double value, double epsilon = 1e-10);
    
    // Rounding and precision
    static double roundToPrecision(double value, int precision);
    static double floorToPrecision(double value, int precision);
    static double ceilToPrecision(double value, int precision);
    
    // Safe math operations
    static double safeDivide(double numerator, double denominator, double defaultValue = 0.0);
    static double safeLog(double value, double defaultValue = 0.0);
    static double safeSqrt(double value, double defaultValue = 0.0);
    static double safeExp(double value, double defaultValue = 0.0);
    
    // Error function (for neural computations)
    static double erf(double x);
    static double erfc(double x);
    static double phi(double x); // Normal PDF
    static double Phi(double x); // Normal CDF
    
    // Neural network activation functions
    static double relu(double x);
    static double leakyRelu(double x, double alpha = 0.01);
    static double sigmoid(double x);
    static double tanh(double x);
    static double softmax(double x);
    
    // Loss functions
    static double mse(double prediction, double target);
    static double crossEntropy(double prediction, double target);
    static double hingeLoss(double prediction, double target);
};

// Memory management utilities
class MemoryUtils {
public:
    // Memory allocation tracking
    static void enableMemoryTracking(bool enable = true);
    static bool isMemoryTrackingEnabled();
    static size_t getTotalAllocatedMemory();
    static size_t getCurrentAllocatedMemory();
    static std::vector<std::string> getMemoryLeaks();
    
    // Memory pools
    static void createMemoryPool(size_t size, size_t alignment = 16);
    static void destroyMemoryPool();
    static void* allocateFromPool(size_t size);
    static void deallocateFromPool(void* ptr);
    
    // Memory analysis
    static std::vector<std::string> analyzeMemoryUsage();
    static bool detectMemoryLeaks();
    static void forceGarbageCollection();
    
    // Memory protection
    static bool enableMemoryProtection();
    static void disableMemoryProtection();
    static bool isMemoryProtectionEnabled();
    
    // Memory compression/decompression
    static std::string compressMemory(const std::string& data);
    static std::string decompressMemory(const std::string& compressed);
    static size_t getCompressedSize(const std::string& data);
    static size_t getUncompressedSize(const std::string& compressed);
    
    // Memory copying and swapping
    static bool copyMemory(void* dest, const void* src, size_t size);
    static bool swapMemory(void* ptr1, void* ptr2, size_t size);
    static bool compareMemory(const void* ptr1, const void* ptr2, size_t size);
    
    // Memory reporting
    static std::string getMemoryReport();
    static std::vector<std::string> getAllocationStackTrace();
    static std::string getCurrentThreadId();
};

// Threading and concurrency utilities
class ThreadUtils {
public:
    // Thread management
    static size_t getThreadCount();
    static size_t getHardwareThreadCount();
    static size_t getCurrentThreadId();
    static bool setThreadAffinity(size_t threadId, size_t cpuMask);
    static std::string getThreadName();
    static bool setThreadName(const std::string& name);
    
    // Synchronization primitives
    static void spinLock(std::atomic_flag& lock);
    static void spinUnlock(std::atomic_flag& lock);
    static bool tryLock(std::atomic_flag& lock);
    static void atomicIncrement(std::atomic<size_t>& counter);
    static size_t atomicDecrement(std::atomic<size_t>& counter);
    static size_t atomicRead(const std::atomic<size_t>& counter);
    static void atomicWrite(std::atomic<size_t>& counter, size_t value);
    
    // Mutex wrappers
    static void lockMutex(std::mutex& mutex);
    static bool tryLockMutex(std::mutex& mutex);
    static void unlockMutex(std::mutex& mutex);
    
    // Condition variables
    static void waitCondition(std::condition_variable& cv, std::mutex& mutex,
                              std::function<bool()> predicate);
    static void notifyCondition(std::condition_variable& cv);
    static void notifyAllCondition(std::condition_variable& cv);
    
    // Thread-local storage
    static void setThreadLocal(const std::string& key, const std::string& value);
    static std::string getThreadLocal(const std::string& key);
    static void removeThreadLocal(const std::string& key);
    
    // Thread pools
    static void createThreadPool(size_t numThreads);
    static void destroyThreadPool();
    static bool submitTask(std::function<void()> task);
    static std::future<void> submitTaskAndGetFuture(std::function<void()> task);
    
    // Parallel processing
    static std::vector<double> parallelForEach(const std::vector<double>& input,
                                              std::function<double(double)> func,
                                              size_t numThreads = 0);
    static std::vector<double> parallelReduce(const std::vector<double>& input,
                                             std::function<double(double, double)> func,
                                             double initialValue = 0.0,
                                             size_t numThreads = 0);
    
    // Performance monitoring
    static double getThreadCPUTime(size_t threadId);
    static double getThreadWallTime(size_t threadId);
    static std::vector<double> getAllThreadCPUTimes();
    static std::vector<double> getAllThreadWallTimes();
    
    // Thread safety
    static bool isThreadSafe();
    static bool checkThreadSafety(std::function<bool()> test);
    static std::vector<std::string> getThreadSafetyViolations();
};

// Validation utilities
class ValidationUtils {
public:
    // Type validation
    static bool isValidInteger(const std::string& str);
    static bool isValidDouble(const std::string& str);
    static bool isValidBoolean(const std::string& str);
    static bool isValidString(const std::string& str);
    static bool isValidIPv4(const std::string& str);
    static bool isValidIPv6(const std::string& str);
    static bool isValidMAC(const std::string& str);
    static bool isValidUUID(const std::string& str);
    
    // Range validation
    static bool isInRange(double value, double min, double max);
    static bool isInRange(int value, int min, int max);
    static bool isInRange(const std::string& str, size_t min, size_t max);
    
    // Format validation
    static bool isValidEmail(const std::string& email);
    static bool isValidURL(const std::string& url);
    static bool isValidPath(const std::string& path);
    static bool isValidFilename(const std::string& filename);
    
    // Content validation
    static bool containsOnlyAllowedCharacters(const std::string& str,
                                              const std::string& allowedChars);
    static bool isASCII(const std::string& str);
    static bool isPrintable(const std::string& str);
    
    // Configuration validation
    static bool validateNeuralConfig(const std::map<std::string, double>& config);
    static bool validatePlasticityConfig(const std::map<std::string, double>& config);
    static bool validateMemoryConfig(const std::map<std::string, size_t>& config);
    
    // Performance validation
    static bool isPerformanceAcceptable(double value, double threshold);
    static bool isMemoryUsageAcceptable(size_t memoryBytes, size_t limitBytes);
    static bool isCPULoadAcceptable(double cpuLoad, double threshold);
    
    // Error validation
    static bool isRecoverableError(const NLMException& error);
    static std::string getErrorSeverity(const NLMException& error);
    static bool isCriticalError(const NLMException& error);
};

} // namespace nlm
