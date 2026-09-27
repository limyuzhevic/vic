#pragma once

/**
 * Memory State Validator
 * 
 * Comprehensive validation of memory system state for NLM brain simulation.
 * Validates memory encoding, storage, retrieval, consolidation, and replay.
 * 
 * Features:
 * - Memory encoding validation
 * - Memory storage validation
 * - Memory retrieval validation
 * - Memory consolidation validation
 * - Memory replay validation
 * - Integration with neural subsystems
 * - Real-time validation during simulation
 * - Thread-safe validation operations
 */

#include "../memory/Memory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <mutex>
#include <vector>
#include <string>

namespace nlm {
namespace validation {

/**
 * Memory system validation result
 */
struct MemorySystemValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t workingMemoryItemsValidated;
    size_t episodicMemoryItemsValidated;
    size_t associativeMemoryItemsValidated;
    
    MemorySystemValidationResult()
        : isValid(true), validationTime(0.0), workingMemoryItemsValidated(0),
          episodicMemoryItemsValidated(0), associativeMemoryItemsValidated(0) {}
};

/**
 * Memory system state validator
 */
class MemoryStateValidator {
public:
    MemoryStateValidator();
    ~MemoryStateValidator();
    
    // Disable copying, enable moving
    MemoryStateValidator(const MemoryStateValidator&) = delete;
    MemoryStateValidator& operator=(const MemoryStateValidator&) = delete;
    MemoryStateValidator(MemoryStateValidator&&) noexcept = default;
    MemoryStateValidator& operator=(MemoryStateValidator&&) noexcept = default;
    
    // Core validation operations
    MemorySystemValidationResult validate(const Brain& brain) const;
    
    // Configuration
    void setMemoryEncodingValidationEnabled(bool enabled) { 
        validateMemoryEncoding_ = enabled; 
    }
    
    void setMemoryStorageValidationEnabled(bool enabled) { 
        validateMemoryStorage_ = enabled; 
    }
    
    void setMemoryRetrievalValidationEnabled(bool enabled) { 
        validateMemoryRetrieval_ = enabled; 
    }
    
    void setMemoryConsolidationValidationEnabled(bool enabled) { 
        validateMemoryConsolidation_ = enabled; 
    }
    
    void setMemoryReplayValidationEnabled(bool enabled) { 
        validateMemoryReplay_ = enabled; 
    }
    
    // Validation statistics
    size_t getTotalValidations() const { return totalValidations_; }
    double getAverageValidationTime() const { return totalValidationTime_ / totalValidations_; }
    size_t getErrorCount() const { return totalErrors_; }
    size_t getWarningCount() const { return totalWarnings_; }
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    
    bool validateMemoryEncoding_ = true;
    bool validateMemoryStorage_ = true;
    bool validateMemoryRetrieval_ = true;
    bool validateMemoryConsolidation_ = true;
    bool validateMemoryReplay_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    MemorySystemValidationResult validateMemoryEncoding(const Brain& brain) const;
    MemorySystemValidationResult validateMemoryStorage(const Brain& brain) const;
    MemorySystemValidationResult validateMemoryRetrieval(const Brain& brain) const;
    MemorySystemValidationResult validateMemoryConsolidation(const Brain& brain) const;
    MemorySystemValidationResult validateMemoryReplay(const Brain& brain) const;
};

} // namespace validation
} // namespace nlm
