#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace nlm {

// ConceptInstance: Represents a single instance of a concept
struct ConceptInstance {
    // The neural activity pattern representing this instance
    std::vector<float> pattern;
    
    // Associated sensory features
    std::vector<float> features;
    
    // How many times this instance has been observed
    size_t observationCount;
    
    // Last time this was observed
    SimulationStep lastObserved;
    
    // Average reward when this was observed
    float avgReward;
    
    // Stability: how consistent is this pattern
    float stability;
    
    ConceptInstance()
        : observationCount(0), lastObserved(0), avgReward(0), stability(0) {}
};

// DiscoveredConcept: A concept that emerged from repeated experience
// NOT a predefined label - created through Hebbian clustering
struct DiscoveredConcept {
    // Unique identifier
    size_t id;
    
    // Central prototype pattern
    std::vector<float> prototype;
    
    // Instances that belong to this concept
    std::vector<ConceptInstance> instances;
    
    // Category label (derived from properties, not predefined)
    // e.g., "edible", "dangerous", "rewarding"
    std::string categoryHint;
    
    // Associated concepts (through associative memory)
    std::vector<size_t> associatedConceptIds;
    
    // How many different instances this concept has absorbed
    size_t totalObservations;
    
    // Average stability of instances
    float avgStability;
    
    // Concept activation level (current relevance)
    float activationLevel;
    
    // Last time concept was activated
    SimulationStep lastActivationTime;
    
    // How many times recently activated (for temporal attention)
    size_t recentActivationCount;
    
    DiscoveredConcept() 
        : id(0), totalObservations(0), avgStability(0), activationLevel(0.0f), 
          lastActivationTime(0), recentActivationCount(0) {}
};

struct Cluster {
    size_t centerConceptId;
    std::vector<size_t> conceptIds;
    std::vector<float> centerPrototype;
    float radius;
    
    Cluster()
        : centerConceptId(0), radius(0.0f) {}
};

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

    // Get concept activation level
    float getConceptActivation(size_t conceptId) const;

    // Get concept cluster assignment
    size_t getConceptCluster(size_t conceptId) const;

    // Get all clusters
    const std::vector<Cluster>& getClusters() const { return clusters_; }

    // Set parameters
    void setFormationThreshold(float t) { formationThreshold_ = t; }
    void setStabilityWindow(size_t w) { stabilityWindow_ = w; }
    void setMaxClusters(size_t m) { maxClusters_ = m; }

private:
    // Create new concept from pattern
    size_t createConcept(const std::vector<float>& pattern,
                        const std::vector<float>& features,
                        float reward);

    // Update concept prototype using Hebbian averaging
    void updatePrototype(size_t conceptId, const std::vector<float>& newInstance);

    // Check if concept is stable enough to be considered "discovered"
    bool isConceptStable(size_t conceptId) const;

    // Perform concept clustering
    void clusterConcepts();

    // Update concept activation based on recent exposure
    void updateConceptActivation(size_t conceptId, SimulationStep currentTime);

    // Retrieve concept from episodic memory
    size_t retrieveConceptFromEpisodicMemory(const std::vector<float>& pattern) const;

    // Generalize concept from episodic memory patterns
    void generalizeConceptFromEpisodicMemory(const std::vector<const EpisodicMemoryItem*>& episodes);

    // Integrate concept with NeuralPlanner
    void integrateWithNeuralPlanner(size_t conceptId);

    // Link concept to PredictionSystem
    void linkToPredictionSystem(size_t conceptId);

    // Connect concept to WorkingMemory
    void connectToWorkingMemory(size_t conceptId);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<DiscoveredConcept> concepts_;
    std::vector<Cluster> clusters_;
    std::vector<size_t> clusterAssignments_;
    std::vector<std::vector<float>> conceptCenters_;
    size_t nextConceptId_;

    // Parameters
    float formationThreshold_;  // Similarity threshold for forming new concept
    float stabilityThreshold_;   // Stability needed to be considered stable
    size_t stabilityWindow_;    // How many observations to consider for stability
    size_t maxClusters_;        // Maximum number of clusters for organization
};

} // namespace nlm
