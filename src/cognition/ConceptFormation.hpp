#pragma once

/**
 * @file ConceptFormation.hpp
 * @brief Pattern discovery and concept formation system
 * 
 * This header defines the ConceptFormation class for the brain's pattern discovery
 * system. The concept formation system discovers recurring patterns from experience
 * through Hebbian clustering, prototype formation, and stability measurement,
 * creating emergent concepts without predefined labels or semantic categories.
 * 
 * Concept Formation Theory:
 * - Concepts emerge through repeated Hebbian clustering of patterns
 * - Prototype formation occurs through averaging of similar instances
 * - Stability measurement ensures concepts are well-established
 * - Category discovery based on shared properties, not predefined labels
 * - No semantic categories - concepts emerge from experience patterns
 * 
 * Concept Functions:
 * - Pattern clustering: Groups similar patterns through similarity metrics
 * - Prototype formation: Creates central prototypes from averaged instances
 * - Instance tracking: Maintains history of concept instances for stability
 * - Stability measurement: Determines when concepts are "discovered"
 * - Category hints: Suggests labels based on emergent properties
 * 
 * Integration Points:
 * - Connected to sensory and prediction systems for pattern input
 * - Modulated by neuromodulation systems (dopamine, curiosity)
 * - Affects attention system for feature selection
 * - Influences planning system for concept-based decision making
 * - Coordinated with memory systems for pattern storage
 * 
 * @author Enhanced Documentation Agent
 * @date Current session
 * @version 1.0
 */

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace nlm {

// Forward declarations
class ConceptFormation;

/**
 * @brief Represents a single instance of a discovered concept
 * 
 * Represents a single instance of a concept, storing the neural activity
 * pattern that constitutes this instance along with associated features,
 * occurrence statistics, and stability measurements.
 * 
 * ConceptInstance Structure:
 * - pattern: Neural activity pattern representing this instance
 * - features: Associated sensory and perceptual features
 * - observationCount: Number of times this instance has been observed
 * - lastObserved: Time when this was last observed
 * - avgReward: Average reward when this was observed
 * - stability: How consistent this pattern is over time
 * 
 * Instance Properties:
 * - Pattern representation: Neural activity encoding of experience
 * - Feature association: Links to sensory and perceptual features
 * - Statistical tracking: Counts and averages for stability
 * - Temporal tracking: Time-based decay and update mechanisms
 * - Reward association: Links to reward prediction errors
 */
struct ConceptInstance {
    /**
     * @brief The neural activity pattern representing this instance
     * 
     * This vector contains the neural activation levels that constitute
     * this concept instance. It represents how the brain encodes this
     * particular experience or pattern.
     */
    std::vector<float> pattern;
    
    /**
     * @brief Associated sensory features
     * 
     * Vector of sensory and perceptual features that are associated with
     * this concept instance. These features help characterize the concept
     * and support generalization.
     */
    std::vector<float> features;
    
    /**
     * @brief How many times this instance has been observed
     * 
     * Count of how many times this specific pattern has been presented
     * to the concept formation system. Higher counts indicate more
     * frequent or stable concept instances.
     */
    size_t observationCount;
    
    /**
     * @brief Last time this was observed
     * 
     * Simulation step when this concept instance was most recently observed.
     * Used for temporal decay and recency effects in concept maintenance.
     */
    SimulationStep lastObserved;
    
    /**
     * @brief Average reward when this was observed
     * 
     * Average reward value associated with observations of this concept
     * instance. Used for reward-based concept selection and evaluation.
     */
    float avgReward;
    
    /**
     * @brief Stability: how consistent is this pattern
     * 
     * Stability measure (0.0-1.0) indicating how consistent this pattern
     * has been over time. Higher stability means the concept is well-established
     * and reliable.
     */
    float stability;
    
    /**
     * @brief Default constructor
     */
    ConceptInstance()
        : observationCount(0), lastObserved(0), avgReward(0), stability(0) {}
};

/**
 * @brief A concept that emerged from repeated experience
 * 
 * Represents a discovered concept that has emerged through repeated
 * Hebbian clustering of similar patterns. Concepts are not predefined
 * labels but emerge organically from experience through pattern clustering
 * and prototype formation.
 * 
 * DiscoveredConcept Structure:
 * - id: Unique identifier for this concept
 * - prototype: Central prototype pattern representing the concept
 * - instances: All instances that belong to this concept
 * - categoryHint: Suggested label derived from properties (not predefined)
 * - associatedConceptIds: Related concepts through associative memory
 * - totalObservations: Total number of instances absorbed
 * - avgStability: Average stability of all instances
 * 
 * Concept Properties:
 * - Emergent nature: Concepts created through experience, not predefined
 * - Prototype-based: Each concept has a central prototype pattern
 * - Instance-based: Maintains all individual observations
 * - Relational: Concepts can be associated with other concepts
 * - Dynamic: Concepts can merge when discovered they are the same
 * 
 * Concept Dynamics:
 * - Formation: Concepts form through pattern clustering and averaging
 * - Stability: Concepts require sufficient stability to be "discovered"
 * - Merging: Similar concepts merge when identified as redundant
 * - Evolution: Concepts evolve and adapt based on new instances
 * - Generalization: Concepts generalize from instances to new patterns
 */
struct DiscoveredConcept {
    /**
     * @brief Unique identifier for this concept
     * 
     * Sequential ID assigned when concept is created. Used for referencing
     * concepts internally and in associative memory links.
     */
    size_t id;
    
    /**
     * @brief Central prototype pattern
     * 
     * The average or representative pattern for this concept, created through
     * Hebbian averaging of all instances. Represents the core identity of the concept.
     */
    std::vector<float> prototype;
    
    /**
     * @brief Instances that belong to this concept
     * 
     * All individual observations that have been assigned to this concept
     * through pattern similarity matching. Each instance maintains its
     * own characteristics and statistics.
     */
    std::vector<ConceptInstance> instances;
    
    /**
     * @brief Category label (derived from properties, not predefined)
     * 
     * Suggested label for this concept based on its properties (e.g., "edible",
     * "dangerous", "rewarding"). Labels emerge from properties rather than
     * being predefined categories.
     */
    std::string categoryHint;
    
    /**
     * @brief Associated concepts (through associative memory)
     * 
     * Vector of concept IDs that are associated with this concept through
     * associative memory connections. Represents conceptual relationships
     * and knowledge structure.
     */
    std::vector<size_t> associatedConceptIds;
    
    /**
     * @brief How many different instances this concept has absorbed
     * 
     * Total number of observations that have been assigned to this concept.
     * Indicates the concept's coverage and stability.
     */
    size_t totalObservations;
    
    /**
     * @brief Average stability of instances
     * 
     * Average stability measure across all instances belonging to this concept.
     * Higher average stability indicates more robust and well-established concept.
     */
    float avgStability;
    
    /**
     * @brief Default constructor
     */
    DiscoveredConcept() 
        : id(0), totalObservations(0), avgStability(0) {}
};

/**
 * @brief Discovers recurring patterns from experience
 * 
 * Implements the brain's pattern discovery system that creates concepts
 * through Hebbian clustering, prototype formation, and stability measurement.
 * The system discovers patterns without predefined labels or semantic categories,
 * allowing concepts to emerge organically from experience.
 * 
 * Concept Formation Architecture:
 * 1. Pattern Input: Receive experiences as neural activity patterns
 * 2. Similarity Matching: Find similar patterns to existing concepts
 * 3. Clustering: Create new concepts or assign to existing ones
 * 4. Prototype Updating: Update concept prototypes with new instances
 * 5. Stability Assessment: Determine concept stability for "discovery"
 * 6. Category Suggestion: Derive labels from emergent properties
 * 
 * Concept Formation Mechanisms:
 * - Pattern clustering: Groups similar patterns through similarity metrics
 * - Prototype formation: Creates prototypes through Hebbian averaging
 * - Instance tracking: Maintains historical statistics for stability
 * - Category discovery: Infers categories from shared properties
 * - Concept merging: Combines concepts when discovered they are redundant
 * 
 * Thread Safety: ConceptFormation is thread-safe for concurrent access
 * from different brain components. Pattern presentation and concept
 * operations are protected with mutexes.
 * 
 * Memory Management: ConceptFormation uses pImpl pattern with pointer storage
 * for efficient memory usage and to reduce compilation dependencies.
 * 
 * @note ConceptFormation is essential for intelligent behavior and learning.
 *       It allows the system to create its own taxonomy based on experience
 *       rather than relying on predefined categories.
 * @see PredictionSystem for pattern input
 * @see Neuromodulator for learning modulation
 * @see Attention for feature selection
 * @see Memory for pattern storage
 */
class ConceptFormation {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a concept formation system with initial state. The concept
     * formation implementation uses the pImpl pattern for efficient memory usage
     * and to reduce compilation dependencies.
     */
    ConceptFormation();
    
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper cleanup of ConceptFormation's pImpl implementation
     * and maintains the pImpl pattern's exception safety guarantees.
     */
    virtual ~ConceptFormation();
    
    /**
     * @brief Initialize concept formation system with brain reference
     * 
     * Initializes the concept formation system with reference to the brain,
     * enabling access to brain's sensory input, prediction system, and other
     * components for context-aware concept formation.
     * 
     * @param brain Pointer to the brain that contains this concept formation system
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Present a new experience to the concept formation system
     * 
     * Presents a new experience (pattern and features) to the concept formation
     * system. Returns concept ID if this experience belongs to an existing concept,
     * or 0 if it's too early to tell, or new concept ID if novel.
     * 
     * Presentation Algorithm:
     * 1. Compute similarity to existing concept prototypes
     * 2. Find best matching concept (highest similarity)
     * 3. If similarity > threshold: assign to existing concept
     * 4. Else: create new concept from pattern
     * 5. Update concept prototype (Hebbian averaging)
     * 6. Update instance statistics
     * 7. Return concept ID
     * 
     * @param pattern Neural activity pattern of new experience
     * @param features Sensory features of new experience
     * @param reward Reward associated with new experience
     * @param currentTime Current simulation time
     * @return Concept ID: existing concept ID, 0 if uncertain, >0 if new concept
     * 
     * @note Presentation determines whether a pattern belongs to an existing
     *       concept, belongs to a new concept, or is too early to decide.
     *       Early presentations (few observations) return 0 to allow more data.
     */
    size_t presentExperience(const std::vector<float>& pattern,
                            const std::vector<float>& features,
                            float reward,
                            SimulationStep currentTime);
    
    /**
     * @brief Get the concept that best matches a pattern
     * 
     * Returns the concept ID that best matches the given pattern based on
     * similarity measures. Used for concept-based decision making and classification.
     * 
     * @param pattern Neural activity pattern to match
     * @param similarityThreshold Similarity threshold for matching (0.0-1.0)
     * @return Concept ID of best matching concept, or 0 if no good match
     * 
     * @note Returns 0 if no concept meets the similarity threshold, allowing
     *       for uncertain classifications.
     */
    size_t getMatchingConcept(const std::vector<float>& pattern,
                              float similarityThreshold = 0.7f) const;
    
    /**
     * @brief Get all concepts
     * 
     * Returns the complete list of discovered concepts. Order is preserved
     * as concept creation order.
     * 
     * @return Vector of all discovered concepts
     */
    const std::vector<DiscoveredConcept>& getConcepts() const { return concepts_; }
    
    /**
     * @brief Get a specific concept
     * 
     * Returns a pointer to a specific concept by ID. Returns nullptr if
     * concept ID is invalid.
     * 
     * @param conceptId ID of concept to retrieve
     * @return Pointer to concept, or nullptr if not found
     */
    const DiscoveredConcept* getConcept(size_t conceptId) const;
    
    /**
     * @brief Get concept prototype
     * 
     * Returns the prototype pattern for a specific concept. The prototype
     * represents the central identity of the concept.
     * 
     * @param conceptId ID of concept
     * @return Concept prototype pattern
     */
    std::vector<float> getConceptPrototype(size_t conceptId) const;
    
    /**
     * @brief Get concept instances
     * 
     * Returns all instances belonging to a specific concept.
     * 
     * @param conceptId ID of concept
     * @return Vector of concept instances
     */
    const std::vector<ConceptInstance>& getConceptInstances(size_t conceptId) const;
    
    /**
     * @brief Measure concept stability (0-1)
     * 
     * Returns the stability measure for a specific concept (0.0-1.0).
     * Stability indicates how well-established and reliable the concept is.
     * 
     * @param conceptId ID of concept
     * @return Concept stability measure
     */
    float getConceptStability(size_t conceptId) const;
    
    /**
     * @brief Get number of concepts discovered
     * 
     * Returns the total number of concepts that have been discovered.
     * 
     * @return Number of concepts
     */
    size_t getConceptCount() const { return concepts_.size(); }
    
    /**
     * @brief Merge two concepts (when discovered they are actually the same)
     * 
     * Merges two concepts when discovered they are actually the same concept
     * (redundant classification). The merged concept combines all instances
     * and updates statistics.
     * 
     * @param conceptA ID of first concept
     * @param conceptB ID of second concept
     */
    void mergeConcepts(size_t conceptA, size_t conceptB);
    
    /**
     * @brief Update concept with new instance
     * 
     * Adds a new instance to an existing concept and updates concept statistics.
     * This is used for concept evolution and refinement over time.
     * 
     * @param conceptId ID of concept to update
     * @param newPattern New instance pattern
     * @param features New instance features
     * @param reward New instance reward
     */
    void updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                      const std::vector<float>& features, float reward);
    
    /**
     * @brief Compute similarity between two patterns
     * 
     * Computes the similarity between two patterns using cosine similarity
     * or other appropriate metric. Returns value between 0.0 (dissimilar)
     * and 1.0 (identical).
     * 
     * @param a First pattern
     * @param b Second pattern
     * @return Similarity score (0.0-1.0)
     */
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;
    
    /**
     * @brief Check if pattern is novel (doesn't belong to any existing concept)
     * 
     * Determines whether a pattern is novel based on similarity to existing
     * concept prototypes. Returns true if pattern is sufficiently novel.
     * 
     * @param pattern Pattern to check
     * @param similarityThreshold Similarity threshold for novelty (0.0-1.0)
     * @return true if pattern is novel, false if it matches existing concept
     */
    bool isNovel(const std::vector<float>& pattern,
                 float similarityThreshold = 0.7f) const;
    
    /**
     * @brief Get concept for a given pattern
     * 
     * Returns the concept ID for a pattern using the best matching concept
     * or 0 if uncertain. This is an alias for getMatchingConcept().
     * 
     * @param pattern Pattern to classify
     * @return Concept ID or 0 if uncertain
     */
    size_t findConceptForPattern(const std::vector<float>& pattern) const;
    
    /**
     * @brief Clear all concepts
     * 
     * Removes all concepts and resets to empty state. This is used to
     * clear concept memory or when concept formation should restart.
     * 
     * @note Thread-safe with mutex protection.
     *       This operation resets the entire concept formation system.
     */
    void clear();
    
    /**
     * @brief Get generalization ability (can concept handle variations?)
     * 
     * Returns a measure of how well a concept can generalize to variations
     * of its pattern. Higher values indicate better generalization.
     * 
     * @param conceptId ID of concept
     * @return Generalization ability (0.0-1.0)
     */
    float getGeneralizationAbility(size_t conceptId) const;
    
    /**
     * @brief Set formation threshold
     * 
     * Sets the similarity threshold for forming new concepts. Lower values
     * make concept formation more sensitive (create concepts more easily).
     * 
     * @param threshold Similarity threshold (0.0-1.0)
     */
    void setFormationThreshold(float threshold) { formationThreshold_ = threshold; }
    
    /**
     * @brief Set stability window
     * 
     * Sets the number of observations to consider for stability calculation.
     * Larger values require more observations for a concept to be considered stable.
     * 
     * @param window Number of observations for stability calculation
     */
    void setStabilityWindow(size_t window) { stabilityWindow_ = window; }
    
    /**
     * @brief Get current developmental stage multiplier
     * 
     * Returns how the current developmental stage affects concept formation.
     * Different developmental stages have different concept formation sensitivities
     * and patterns.
     * 
     * @return Developmental stage multiplier for concept formation
     */
    float getDevelopmentalStageMultiplier() const;
    
    /**
     * @brief Get current concept formation parameters
     * 
     * Returns current concept formation parameters.
     * 
     * @return Vector containing current parameters [formationThreshold, stabilityThreshold, stabilityWindow]
     */
    std::vector<float> getParameters() const;
    
    /**
     * @brief Set concept formation parameters
     * 
     * Sets internal parameters for concept formation.
     * 
     * @param formationThreshold Similarity threshold for concept formation
     * @param stabilityThreshold Minimum stability for concept discovery
     * @param stabilityWindow Number of observations for stability calculation
     */
    void setParameters(float formationThreshold, float stabilityThreshold,
                       size_t stabilityWindow);
    
private:
    /**
     * @brief Create new concept from pattern
     * 
     * Creates a new concept from a pattern and its features. This is the
     * core mechanism for concept formation.
     * 
     * @param pattern Prototype pattern for new concept
     * @param features Features for new concept
     * @param reward Reward associated with new concept
     * @return ID of newly created concept
     */
    size_t createConcept(const std::vector<float>& pattern,
                        const std::vector<float>& features,
                        float reward);
    
    /**
     * @brief Update concept prototype using Hebbian averaging
     * 
     * Updates the prototype of a concept using Hebbian averaging with a new
     * instance. This is the core Hebbian learning mechanism for concept evolution.
     * 
     * @param conceptId ID of concept to update
     * @param newInstance New instance pattern
     */
    void updatePrototype(size_t conceptId, const std::vector<float>& newInstance);
    
    /**
     * @brief Check if concept is stable enough to be considered "discovered"
     * 
     * Determines if a concept meets stability criteria to be considered
     * "discovered" (fully established). This includes checking stability
     * over time window and sufficient observations.
     * 
     * @param conceptId ID of concept to check
     * @return true if concept is stable enough to be discovered
     */
    bool isConceptStable(size_t conceptId) const;
    
    /**
     * @brief Implementation details (pImpl pattern)
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Reference to brain for concept formation context
     */
    Brain* brain_;
    
    /**
     * @brief Discovered concepts
     */
    std::vector<DiscoveredConcept> concepts_;
    
    /**
     * @brief Next concept ID to assign
     */
    size_t nextConceptId_;
    
    /**
     * @brief Parameters
     */
    float formationThreshold_;  // Similarity threshold for forming new concept
    float stabilityThreshold_;   // Stability needed to be considered stable
    size_t stabilityWindow_;    // How many observations to consider for stability
};

} // namespace nlm