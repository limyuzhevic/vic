// IConceptFormation.hpp
// Abstract interface for concept formation systems
// Forms abstract concepts from concrete experiences and patterns
// Integrates with memory and attention systems

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>
#include <unordered_set>

namespace nlm {

// Forward declarations
class Brain;

// Concept representation
struct Concept {
    uint64_t id;
    std::string name;
    std::vector<float> prototype;  // Representative pattern
    std::vector<std::vector<float>> exemplars;  // Instance examples
    float activation;
    float coherence;  // How well concept fits its exemplars
    SimulationStep lastUpdated;
    std::map<uint64_t, float> categoryLinks;  // Links to parent/child concepts
    
    Concept() : id(0), activation(0), coherence(0), lastUpdated(0) {}
};

// Feature extractor function type
using FeatureExtractor = std::function<std::vector<float>(const std::vector<float>& input)>;

class IConceptFormation {
public:
    virtual ~IConceptFormation() = default;

    // Initialize concept formation with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Process input to form or update concepts
    virtual void processInput(const std::vector<float>& input) = 0;

    // Retrieve concept matching input
    virtual Concept findConcept(const std::vector<float>& input, float threshold = 0.5f) const = 0;

    // Get concept activation level
    virtual float getConceptActivation(uint64_t conceptId) const = 0;

    // Update concept formation dynamics
    virtual void update(TimestepDuration dt) = 0;

    // Reset concept formation
    virtual void reset() = 0;

    // Get current concept statistics
    virtual std::string getConceptStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IConceptFormation"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for concept events
    using ConceptFormedCallback = std::function<void(const Concept& concept)>;
    virtual void setConceptFormedCallback(const ConceptFormedCallback& callback) = 0;

    using ConceptActivatedCallback = std::function<void(uint64_t conceptId, float activation)>;
    virtual void setConceptActivatedCallback(const ConceptActivatedCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Concept management
    virtual void addConcept(const Concept& concept) = 0;
    virtual void removeConcept(uint64_t conceptId) = 0;
    virtual bool hasConcept(uint64_t conceptId) const = 0;
    virtual Concept getConcept(uint64_t conceptId) const = 0;

    // Get all concepts sorted by activation
    virtual std::vector<Concept> getConceptsSortedByActivation() const = 0;

    // Concept parameters
    virtual size_t getMaxConcepts() const = 0;
    virtual void setMaxConcepts(size_t max) = 0;
    virtual float getConceptThreshold() const = 0;
    virtual void setConceptThreshold(float threshold) = 0;

    // Feature extraction
    virtual void setFeatureExtractor(const FeatureExtractor& extractor) = 0;
    virtual std::vector<float> extractFeatures(const std::vector<float>& input) const = 0;

    // Performance metrics
    virtual float getConceptFormationAccuracy() const = 0;
    virtual float getConceptCohesion() const = 0;  // How cohesive concepts are
    virtual uint64_t getTotalConceptsFormed() const = 0;

protected:
    IConceptFormation() = default;
};

} // namespace nlm
