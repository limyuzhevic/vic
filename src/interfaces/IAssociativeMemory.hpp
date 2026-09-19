// IAssociativeMemory.hpp
// Abstract interface for associative memory systems
// Forms associations between patterns and enables pattern completion
// Supports content-addressable memory retrieval

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;

// Associative memory pattern with meta-data
struct AssociativePattern {
    uint64_t id;
    std::vector<float> content;
    std::vector<float> context;
    float activationStrength;
    SimulationStep lastAccessed;
    float predictionError;
    bool isActive;
    
    AssociativePattern() : id(0), activationStrength(0.0f), 
                          lastAccessed(0), predictionError(0.0f), isActive(false) {}
    AssociativePattern(uint64_t i, const std::vector<float>& cont,
                      const std::vector<float>& ctx, float strength)
        : id(i), content(cont), context(ctx), activationStrength(strength),
          lastAccessed(0), predictionError(0.0f), isActive(true) {}
};

class IAssociativeMemory {
public:
    virtual ~IAssociativeMemory() = default;

    // Initialize associative memory with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Learn an association between content and context
    virtual void learnAssociation(const std::vector<float>& content,
                                  const std::vector<float>& context,
                                  float strength = 1.0f) = 0;

    // Learn association with explicit ID
    virtual void learnAssociation(uint64_t id,
                                  const std::vector<float>& content,
                                  const std::vector<float>& context,
                                  float strength = 1.0f) = 0;

    // Retrieve context for given content (pattern completion)
    virtual std::vector<float> retrieveContext(
        const std::vector<float>& content,
        float threshold = 0.5f) const = 0;

    // Retrieve content for given context (pattern completion)
    virtual std::vector<float> retrieveContent(
        const std::vector<float>& context,
        float threshold = 0.5f) const = 0;

    // Get strongest association for content
    virtual float getAssociationStrength(
        const std::vector<float>& content) const = 0;

    // Update associative memory (strengthening/weakening)
    virtual void update(TimestepDuration dt) = 0;

    // Clear all associations
    virtual void clear() = 0;

    // Get number of associations
    virtual size_t getAssociationCount() const = 0;

    // Get memory capacity
    virtual size_t getCapacity() const = 0;
    virtual void setCapacity(size_t cap) = 0;

    // Check if association exists for content
    virtual bool hasAssociation(const std::vector<float>& content) const = 0;

    // Get association ID for content
    virtual uint64_t getAssociationId(const std::vector<float>& content) const = 0;

    // Get activation level of pattern
    virtual float getPatternActivation(uint64_t id) const = 0;
    virtual void setPatternActivation(uint64_t id, float activation) = 0;

    // Get all active patterns
    virtual std::vector<AssociativePattern> getActivePatterns() const = 0;

    // Get patterns sorted by activation strength
    virtual std::vector<AssociativePattern> getPatternsSortedByActivation() const = 0;

    // Update association based on prediction error
    virtual void updateFromPredictionError(
        const std::vector<float>& content,
        float predictionError) = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IAssociativeMemory"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Thread safety for concurrent access
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual bool isLocked() const = 0;

    // Callback pattern for learning events
    using AssociationLearnCallback = std::function<void(const AssociativePattern& pattern)>;
    virtual void setAssociationLearnCallback(const AssociationLearnCallback& callback) = 0;

    using AssociationRetrieveCallback = std::function<void(const std::vector<float>& content,
                                                           const std::vector<float>& result)>;
    virtual void setAssociationRetrieveCallback(const AssociationRetrieveCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Performance statistics
    virtual float getAverageActivation() const = 0;
    virtual float getMaximumActivation() const = 0;
    virtual SimulationStep getLastAccessTime() const = 0;

protected:
    IAssociativeMemory() = default;
};

} // namespace nlm
