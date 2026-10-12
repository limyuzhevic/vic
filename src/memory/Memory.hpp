#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Validation/ValidationSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <functional>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

// Custom exceptions for memory validation
class MemoryValidationError : public ValidationError {
public:
    explicit MemoryValidationError(const std::string &message) : ValidationError(message) {}
    virtual const char *category() const noexcept override { return "MemoryValidationError"; }
};

class MemoryCapacityError : public MemoryValidationError {
public:
    explicit MemoryCapacityError(const std::string &message) : MemoryValidationError(message) {}
    virtual const char *category() const noexcept override { return "MemoryCapacityError"; }
};

class MemoryParameterError : public MemoryValidationError {
public:
    MemoryParameterError(const std::string &message, const std::string &parameterName)
        : MemoryValidationError(message), parameterName_(parameterName) {}
    const std::string &getParameterName() const { return parameterName_; }
    virtual const char *category() const noexcept override { return "MemoryParameterError"; }
private:
    std::string parameterName_;
};

// Validation macros for memory classes
#define NLM_VALIDATE_MEMORY_PARAM(param, condition, message) \
    do { \
        if (!(condition)) { \
            throw nlm::MemoryParameterError(message, #param); \
        } \
    } while(0)

#define NLM_VALIDATE_MEMORY_STATE(condition, message) \
    do { \
        if (!(condition)) { \
            throw nlm::MemoryValidationError(message); \
        } \
    } while(0)

// Validation result for memory operations
struct MemoryOperationResult {
    bool success;
    std::string message;
    std::string operation;
    uint64_t timestamp;
    
    MemoryOperationResult(bool s = true, const std::string &msg = "", const std::string &op = "")
        : success(s), message(msg), operation(op), timestamp(std::chrono::system_clock::now().time_since_epoch().count()) {}
    
    static MemoryOperationResult success(const std::string &operation = "") {
        return MemoryOperationResult(true, "Operation succeeded", operation);
    }
    
    static MemoryOperationResult failure(const std::string &message, const std::string &operation = "") {
        return MemoryOperationResult(false, message, operation);
    }
};

// Working memory: temporary active storage of information
// PLACEHOLDER - Phase 2 will implement real working memory

class WorkingMemory {
public:
  WorkingMemory();
  ~WorkingMemory();

  // Store item
  // TODO PHASE 2: Implement real storage
  void store(NeuronId neuron, float value);

  // Retrieve item
  float retrieve(NeuronId neuron) const;

  // Check if item exists
  bool contains(NeuronId neuron) const;

  // Clear all
  void clear();

  // Get capacity
  size_t getCapacity() const;
  size_t getCurrentSize() const;

  // Decay all items
  void decay(float decayRate);

  // Validation methods
  ValidationResult validate() const;
  ValidationResult validateCapacity() const;
  ValidationResult validateContents() const;
  void validateOrThrow() const;

  // Recovery methods
  bool attemptRecovery();
  void setAutoRecovery(bool enable);
  bool isAutoRecoveryEnabled() const;

  // Performance monitoring
  void enablePerformanceMonitoring();
  void disablePerformanceMonitoring();
  bool isPerformanceMonitoringEnabled() const;
  struct WorkingMemoryStats {
    uint64_t storeCount = 0;
    uint64_t retrieveCount = 0;
    uint64_t clearCount = 0;
    uint64_t validationCount = 0;
    uint64_t validationFailureCount = 0;
    uint64_t memoryErrorCount = 0;
    double averageValidationTime = 0.0;
  } stats_;

private:
  struct Impl;
  Impl *pImpl;
};

// Episodic memory: storage of events and experiences
// PLACEHOLDER - Phase 2 will implement real episodic memory

struct EpisodicMemoryItem {
  SimulationStep timestamp;
  std::vector<NeuronId> neurons;
  std::vector<float> values;
  std::string metadata;

  EpisodicMemoryItem() : timestamp(0) {}

  // Validate episodic memory item
  ValidationResult validate() const {
    ValidationResult result = ValidationResult::success("EpisodicMemoryItem");

    // Validate neurons vector
    if (!NLM_VALIDATE_MEMORY_PARAM(neurons, !neurons.empty(), "Episodic memory item must have at least one neuron")) {
      return ValidationResult::failure("Invalid episodic memory item neurons", "EpisodicMemoryItem");
    }

    // Validate values vector size matches neurons
    if (!NLM_VALIDATE_MEMORY_PARAM(values, values.size() == neurons.size(), "Values must match neurons count")) {
      return ValidationResult::failure("Invalid episodic memory item values", "EpisodicMemoryItem");
    }

    // Validate value ranges
    for (size_t i = 0; i < values.size(); ++i) {
      if (!NLM_VALIDATE_MEMORY_PARAM(values[i], values[i] >= -10.0f && values[i] <= 10.0f, "Value out of reasonable range")) {
        return ValidationResult::failure("Invalid episodic memory item value", "EpisodicMemoryItem");
      }
    }

    return result;
  }
};

class EpisodicMemory {
public:
  EpisodicMemory();
  ~EpisodicMemory();

  // Store episode
  void storeEpisode(const EpisodicMemoryItem &episode);

  // Retrieve episode
  EpisodicMemoryItem retrieveEpisode(size_t index) const;

  // Get episode count
  size_t getEpisodeCount() const;

  // Get recent episodes
  std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;

  // Clear all
  void clear();

  // Memory consolidation (move to long-term)
  void consolidate(float relevanceThreshold);

  // Validation methods
  ValidationResult validate() const;
  ValidationResult validateCapacity() const;
  ValidationResult validateRecentAccess() const;
  void validateOrThrow() const;

  // Recovery methods
  bool attemptRecovery();
  void setAutoRecovery(bool enable);
  bool isAutoRecoveryEnabled() const;

  // Performance monitoring
  void enablePerformanceMonitoring();
  void disablePerformanceMonitoring();
  bool isPerformanceMonitoringEnabled() const;
  struct EpisodicMemoryStats {
    uint64_t storeCount = 0;
    uint64_t retrieveCount = 0;
    uint64_t consolidateCount = 0;
    uint64_t validationCount = 0;
    uint64_t validationFailureCount = 0;
    uint64_t memoryErrorCount = 0;
    double averageValidationTime = 0.0;
  } stats_;

private:
  struct Impl;
  Impl *pImpl;
};

// Semantic memory: gradually acquired knowledge
// PLACEHOLDER - Phase 2 will implement real semantic memory

class SemanticMemory {
public:
  SemanticMemory();
  ~SemanticMemory();

  // Store fact
  void storeFact(const std::string &key, const std::string &value);

  // Retrieve fact
  std::string retrieveFact(const std::string &key) const;

  // Check if fact exists
  bool hasFact(const std::string &key) const;

  // Get all facts
  std::vector<std::pair<std::string, std::string>> getAllFacts() const;

  // Clear all
  void clear();

  // Validation methods
  ValidationResult validate() const;
  ValidationResult validateKeyValue() const;
  void validateOrThrow() const;

  // Recovery methods
  bool attemptRecovery();
  void setAutoRecovery(bool enable);
  bool isAutoRecoveryEnabled() const;

  // Performance monitoring
  void enablePerformanceMonitoring();
  void disablePerformanceMonitoring();
  bool isPerformanceMonitoringEnabled() const;
  struct SemanticMemoryStats {
    uint64_t storeCount = 0;
    uint64_t retrieveCount = 0;
    uint64_t clearCount = 0;
    uint64_t validationCount = 0;
    uint64_t validationFailureCount = 0;
    uint64_t memoryErrorCount = 0;
    double averageValidationTime = 0.0;
  } stats_;

private:
  struct Impl;
  Impl *pImpl;
};

// Procedural memory: learned skills and habits
// PLACEHOLDER - Phase 2 will implement real procedural memory

struct Skill {
  std::string name;
  std::vector<NeuronId> neuralPattern;
  float proficiency;

  Skill() : proficiency(0.0f) {}

  // Validate skill
  ValidationResult validate() const {
    ValidationResult result = ValidationResult::success("Skill");

    if (!NLM_VALIDATE_MEMORY_PARAM(name, !name.empty(), "Skill must have a name")) {
      return ValidationResult::failure("Invalid skill name", "Skill");
    }

    if (!NLM_VALIDATE_MEMORY_PARAM(neuralPattern, !neuralPattern.empty(), "Skill must have a neural pattern")) {
      return ValidationResult::failure("Invalid skill neural pattern", "Skill");
    }

    if (!NLM_VALIDATE_MEMORY_PARAM(proficiency, proficiency >= 0.0f && proficiency <= 10.0f, "Proficiency must be reasonable (0-10)")) {
      return ValidationResult::failure("Invalid skill proficiency", "Skill");
    }

    return result;
  }
};

class ProceduralMemory {
public:
  ProceduralMemory();
  ~ProceduralMemory();

  // Learn skill
  void learnSkill(const std::string &name,
                  const std::vector<NeuronId> &pattern);

  // Retrieve skill
  Skill *getSkill(const std::string &name);

  // Get all skills
  std::vector<Skill *> getAllSkills();

  // Update proficiency
  void updateProficiency(const std::string &name, float delta);

  // Clear all
  void clear();

  // Validation methods
  ValidationResult validate() const;
  ValidationResult validateSkill(const Skill &skill) const;
  void validateOrThrow() const;

  // Recovery methods
  bool attemptRecovery();
  void setAutoRecovery(bool enable);
  bool isAutoRecoveryEnabled() const;

  // Performance monitoring
  void enablePerformanceMonitoring();
  void disablePerformanceMonitoring();
  bool isPerformanceMonitoringEnabled() const;
  struct ProceduralMemoryStats {
    uint64_t learnCount = 0;
    uint64_t retrieveCount = 0;
    uint64_t updateCount = 0;
    uint64_t clearCount = 0;
    uint64_t validationCount = 0;
    uint64_t validationFailureCount = 0;
    uint64_t memoryErrorCount = 0;
    double averageValidationTime = 0.0;
  } stats_;

private:
  struct Impl;
  Impl *pImpl;
};

// Associative memory: relationships between representations
// PLACEHOLDER - Phase 2 will implement real associative memory

class AssociativeMemory {
public:
  AssociativeMemory();
  ~AssociativeMemory();

  // Create association
  void associate(NeuronId a, NeuronId b, float strength);

  // Get associated neurons
  std::vector<NeuronId> getAssociations(NeuronId neuron) const;

  // Get association strength
  float getAssociationStrength(NeuronId a, NeuronId b) const;

  // Update association
  void updateAssociation(NeuronId a, NeuronId b, float delta);

  // Clear all
  void clear();

  // Validation methods
  ValidationResult validate() const;
  ValidationResult validateAssociation(const NeuronId &a, const NeuronId &b, float strength) const;
  void validateOrThrow() const;

  // Recovery methods
  bool attemptRecovery();
  void setAutoRecovery(bool enable);
  bool isAutoRecoveryEnabled() const;

  // Performance monitoring
  void enablePerformanceMonitoring();
  void disablePerformanceMonitoring();
  bool isPerformanceMonitoringEnabled() const;
  struct AssociativeMemoryStats {
    uint64_t associateCount = 0;
    uint64_t retrieveCount = 0;
    uint64_t updateCount = 0;
    uint64_t clearCount = 0;
    uint64_t validationCount = 0;
    uint64_t validationFailureCount = 0;
    uint64_t memoryErrorCount = 0;
    double averageValidationTime = 0.0;
  } stats_;

private:
  struct Impl;
  Impl *pImpl;
};

// Memory validation helpers
ValidationResult validateNeuronId(NeuronId neuron);
ValidationResult validateMemoryOperation(const std::string &operation, size_t currentSize, size_t capacity);
ValidationResult validateMemoryAccess(const std::unordered_map<NeuronId, bool> &validNeurons, const std::vector<NeuronId> &neurons);

} // namespace nlm
