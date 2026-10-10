// Checkpoint validator - checkpoint integrity validation
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include "CheckpointSystem.hpp"

namespace nlm {

// Checkpoint validation result
struct CheckpointValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    time_t timestamp;
    
    CheckpointValidationResult() : valid(false), validationTime(0.0), timestamp(0) {}
    
    bool isValid() const { return valid && errors.empty(); }
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Valid: " << (valid ? "yes" : "no");
        if (!errors.empty()) ss << ", Errors: " << errors.size();
        if (!warnings.empty()) ss << ", Warnings: " << warnings.size();
        return ss.str();
    }
};

// Validation options
struct ValidationOptions {
    bool validateHeader;
    bool validateSections;
    bool validateChecksums;
    bool validateVersion;
    bool validateDataIntegrity;
    std::chrono::seconds timeout;
    
    ValidationOptions() : validateHeader(true), validateSections(true), validateChecksums(true),
                         validateVersion(true), validateDataIntegrity(true),
                         timeout(std::chrono::seconds(30)) {}
};

// Validation statistics
struct ValidationStats {
    uint64_t checkpointsValidated;
    uint64_t validCheckpoints;
    uint64_t invalidCheckpoints;
    uint64_t totalValidationTime;
    uint32_t errorCount;
    
    ValidationStats() : checkpointsValidated(0), validCheckpoints(0), invalidCheckpoints(0),
                       totalValidationTime(0), errorCount(0) {}
};

// Main checkpoint validator
class CheckpointValidator {
public:
    CheckpointValidator();
    ~CheckpointValidator();
    
    // Validation operations
    bool validate(const std::string& checkpointPath, std::vector<std::string>& errors,
                  const ValidationOptions& options = ValidationOptions());
    
    bool validateHeader(const CheckpointHeader& header, std::vector<std::string>& errors);
    bool validateSections(const std::vector<SectionHeader>& sections, const CheckpointHeader& header,
                         std::vector<std::string>& errors);
    bool validateChecksums(const CheckpointHeader& header, const std::vector<SectionHeader>& sections,
                          std::vector<std::string>& errors);
    bool validateVersion(const CheckpointHeader& header, std::vector<std::string>& errors);
    bool validateDataIntegrity(const std::string& checkpointPath, const CheckpointHeader& header,
                              const std::vector<SectionHeader>& sections, std::vector<std::string>& errors);
    
    // Quick validation
    bool quickValidate(const std::string& checkpointPath);
    
    // Statistics and monitoring
    ValidationStats getStats() const { return stats_; }
    
    bool isHealthy() const { return errorCount_ == 0; }
    
    // Event handling
    using ValidationEventCallback = std::function<void(const std::string&, const CheckpointValidationResult&)>;
    void setValidationEventCallback(ValidationEventCallback callback) { 
        validationEventCallback_ = callback; 
    }
    
    // Configuration
    void configure(const std::string& dataDir, const std::shared_ptr<Config>& config);
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    bool loadCheckpointData(const std::string& checkpointPath, CheckpointHeader& header,
                           std::vector<SectionHeader>& sections);
    
    void updateStats(const CheckpointValidationResult& result);
    void logEvent(const std::string& checkpointPath, const CheckpointValidationResult& result);
    
    // Components
    std::unique_ptr<CheckpointReader> checkpointReader_;
    
    // Configuration
    std::string dataDir_;
    std::shared_ptr<Config> config_;
    
    // State
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    ValidationStats stats_;
    
    // Events
    ValidationEventCallback validationEventCallback_;
};

} // namespace nlm
