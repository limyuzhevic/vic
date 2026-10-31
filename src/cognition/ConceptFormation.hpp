#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <string>
#include <limits>

namespace nlm {

// ConceptFormation: Discovers recurring patterns from experience
// NOT a predefined taxonomy - concepts emerge through repeated experience
//
// Key mechanisms:
// - Pattern clustering through similarity
// - Prototype formation through averaging
// - Instance tracking for stability measurement
// - Category discovery based on shared properties
// - No predefined labels or semantic categories

class ConceptFormation {
public:
    ConceptFormation();
    ~ConceptFormation();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Present a new experience to the concept formation system
    // Returns concept ID if this experience belongs to an existing concept,
    // or 0 if it's too early to tell, or new concept ID if novel
    size_t presentExperience(const std::vector<float>& pattern,
                            const std::vector<float>& features,
                            float reward,
                            SimulationStep currentTime);

    // Get the concept that best matches a pattern
    size_t getMatchingConcept(const std::vector<float>& pattern,
                              float similarityThreshold = 0.7f) const;

    // Get all concepts
    const std::vector<DiscoveredConcept>& getConcepts() const { return concepts_; }

    // Get a specific concept
    const DiscoveredConcept* getConcept(size_t conceptId) const;

    // Get concept prototype
    std::vector<float> getConceptPrototype(size_t conceptId) const;

    // Get concept instances
    const std::vector<ConceptInstance>& getConceptInstances(size_t conceptId) const;

    // Measure concept stability (0-1)
    float getConceptStability(size_t conceptId) const;

    // Get number of concepts discovered
    size_t getConceptCount() const { return concepts_.size(); }

    // Merge two concepts (when discovered they are actually the same)
    void mergeConcepts(size_t conceptA, size_t conceptB);

    // Update concept with new instance
    void updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                      const std::vector<float>& features, float reward);

    // Compute similarity between two patterns
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;

    // Check if pattern is novel (doesn't belong to any existing concept)
    bool isNovel(const std::vector<float>& pattern,
                 float similarityThreshold = 0.7f) const;

    // Get concept for a given pattern
    size_t findConceptForPattern(const std::vector<float>& pattern) const;

    // Clear all concepts
    void clear();

    // Get generalization ability (can concept handle variations?)
    float getGeneralizationAbility(size_t conceptId) const;

    // Set parameters
    void setFormationThreshold(float t) { formationThreshold_ = t; }
    void setStabilityWindow(size_t w) { stabilityWindow_ = w; }

private:
    // Create new concept from pattern
    size_t createConcept(const std::vector<float>& pattern,
                        const std::vector<float>& features,
                        float reward);

    // Update concept prototype using Hebbian averaging
    void updatePrototype(size_t conceptId, const std::vector<float>& newInstance);

    // Check if concept is stable enough to be considered "discovered"
    bool isConceptStable(size_t conceptId) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<DiscoveredConcept> concepts_;
    size_t nextConceptId_;

    // Parameters
    float formationThreshold_;  // Similarity threshold for forming new concept
    float stabilityThreshold_;   // Stability needed to be considered stable
    size_t stabilityWindow_;    // How many observations to consider for stability
};

} // namespace nlm