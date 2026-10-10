#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace nlm {

/**
 * @file ConceptFormation.hpp
 * 
 * Cognitive system for concept formation and abstraction in NLM
 * 
 * This file provides the concept formation system for NLM, implementing:
 * - Pattern discovery through sensory experience
 * - Concept abstraction and categorization
 * - Category formation from recurring patterns
 * - Spatial and temporal concept relationships
 * - Hebbian learning for concept emergence
 * 
 * The concept formation system in NLM:
 * - **ConceptInstance**: Represents a single instance of a concept
 * - **DiscoveredConcept**: A concept that emerged from repeated experience
 * - **ConceptFormation**: Discovers recurring patterns from experience
 * - **SpatialRepresentation**: Learns spatial relationships from experience
 * - **TemporalRelation**: Learns temporal relationships and sequences
 * - **Integration**: Connects concept formation with neural dynamics
 * - **Learning**: Adapts concepts based on experience
 * 
 * Key features:
 * - **Pattern discovery**: Discovers recurring patterns from sensory input
 * - **Concept abstraction**: Forms abstract concepts from concrete instances
 * - **Categorization**: Groups similar patterns into concepts
 * - **Spatial awareness**: Learns spatial relationships and positions
 * - **Temporal processing**: Learns temporal sequences and relationships
 * - **Adaptive learning**: Continuously refines concepts based on experience
 * 
 * @note Critical for cognitive abstraction and understanding
 * @note Used extensively in perception and cognition
 * @note Essential for forming meaningful representations
 * @see Config.hpp - Configuration options for concept formation
 * @see Brain.hpp - Integration with brain architecture
 * @see NeuralPlanner.hpp - For planning with concepts
 * @see ConceptFormation.cpp - Implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @struct ConceptInstance
 * 
 * Represents a single instance of a concept, learned from experience.
 * 
 * This structure represents a specific occurrence of a concept, storing
 * the neural pattern, associated features, and statistics about this
 * instance. It is used by the concept formation system to:
 * - Track individual occurrences of concepts
 * - Measure concept stability and reliability
 * - Refine concept prototypes
 * - Learn from experience
 * 
 * Key features:
 * - **Neural pattern**: The neural activity pattern representing this instance
 * - **Sensory features**: Associated sensory features and characteristics
 * - **Observation count**: How many times this instance has been observed
 * - **Temporal tracking**: Last time this was observed and average reward
 * - **Stability measurement**: How consistent this pattern is
 * 
 * @note Used by ConceptFormation for instance tracking
 * @see DiscoveredConcept - For concept-level information
 * @see ConceptFormation - For concept formation system
 * 
 * @author NLM Development Team
 */
struct ConceptInstance {
    /** Neural activity pattern representing this concept instance */
    std::vector<float> pattern;
    
    /** Associated sensory features and characteristics */
    std::vector<float> features;
    
    /** Number of times this instance has been observed */
    size_t observationCount;
    
    /** Last simulation step when this instance was observed */
    SimulationStep lastObserved;
    
    /** Average reward associated with this instance */
    float avgReward;
    
    /** Stability measurement (0-1): how consistent this pattern is */
    float stability;
    
    /**
     * @brief Default constructor
     * 
     * Creates a default concept instance with empty values.
     * The concept instance is initialized with default values for
     * all fields.
     */
    ConceptInstance()
        : observationCount(0), lastObserved(0), avgReward(0), stability(0) {}
};

/**
 * @struct DiscoveredConcept
 * 
 * A concept that emerged from repeated experience and Hebbian clustering.
 * 
 * This structure represents an abstract concept that has formed through
 * repeated exposure to similar patterns. Unlike predefined categories,
 * discovered concepts emerge organically from the agent's experience.
 * It is used by the concept formation system to:
 * - Store discovered concepts and their properties
 * - Track concept instances and stability
 * - Maintain concept relationships
 * - Refine concepts based on experience
 * 
 * Key features:
 * - **Concept ID**: Unique identifier for the concept
 * - **Prototype**: Central pattern representing the concept
 * - **Instances**: All instances that belong to this concept
 * - **Category hint**: Derived category from properties, not predefined
 * - **Associations**: Related concepts through associative memory
 * - **Observation statistics**: Total observations and average stability
 * 
 * @note Concepts are not predefined - they emerge through experience
 * @see ConceptInstance - For individual instances
 * @see ConceptFormation - For concept discovery
 * 
 * @author NLM Development Team
 */
struct DiscoveredConcept {
    /** Unique identifier for this concept */
    size_t id;
    
    /** Central prototype pattern representing the concept */
    std::vector<float> prototype;
    
    /** All instances that belong to this concept */
    std::vector<ConceptInstance> instances;
    
    /** Category hint derived from properties (not predefined) */
    std::string categoryHint;
    
    /** Associated concepts through associative memory */
    std::vector<size_t> associatedConceptIds;
    
    /** Total number of different instances this concept has absorbed */
    size_t totalObservations;
    
    /** Average stability of all instances in this concept */
    float avgStability;
    
    /**
     * @brief Default constructor
     * 
     * Creates a default discovered concept with empty values.
     * The concept is initialized with default values for all fields.
     */
    DiscoveredConcept() 
        : id(0), totalObservations(0), avgStability(0) {}
};

/**
 * @class ConceptFormation
 * 
 * Neural implementation of concept discovery and abstraction.
 * 
 * This class implements neural concept discovery using Hebbian learning
 * and pattern clustering. It discovers recurring patterns from sensory
 * experience and forms abstract concepts through statistical regularities.
 * This is essential for:
 * - **Pattern discovery**: Finding recurring patterns in experience
 * - **Concept abstraction**: Forming abstract concepts from concrete data
 * - **Categorization**: Grouping similar experiences into concepts
 * - **Memory organization**: Organizing experiences into meaningful categories
 * - **Generalization**: Applying learned concepts to new experiences
 * 
 * Concept formation mechanisms in NLM:
 * - **Pattern clustering**: Groups similar patterns through similarity
 * - **Prototype formation**: Creates central prototypes through averaging
 * - **Instance tracking**: Tracks individual instances for stability measurement
 * - **Category discovery**: Discovers categories based on shared properties
 * - **Hebbian learning**: Uses Hebbian mechanisms for concept emergence
 * 
 * Key characteristics of concept formation in NLM:
 * - **Experience-based**: Concepts emerge from repeated sensory experience
 * - **No predefined categories**: Categories are derived from experience
 * - **Abstract representation**: Forms abstract prototypes from concrete instances
 * - **Adaptive learning**: Continuously refines concepts based on new data
 * - **Statistical learning**: Learns statistical regularities in experience
 * 
 * @note Concepts emerge through Hebbian clustering, not predefined labels
 * @see ConceptInstance - For concept instances
 * @see DiscoveredConcept - For discovered concepts
 * @see SpatialRepresentation - For spatial concept relationships
 * @see TemporalRelation - For temporal concept relationships
 * @see ConceptFormation.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class ConceptFormation {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new concept formation instance with default parameters.
     * The concept formation is initialized with default settings for
     * concept discovery and pattern clustering.
     */
    ConceptFormation();
    
    /**
     * @brief Destructor
     * 
     * Cleans up concept formation resources and performs any necessary
     * cleanup operations. The concept formation is left in a valid but
     * non-operational state after destruction.
     */
    ~ConceptFormation();
    
    // Disable copying - PImpl idiom
    ConceptFormation(const ConceptFormation&) = delete;
    ConceptFormation& operator=(const ConceptFormation&) = delete;
    ConceptFormation(ConceptFormation&&) noexcept;
    ConceptFormation& operator=(ConceptFormation&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the concept formation system with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Present a new experience to the concept formation system
     * 
     * Presents a new sensory experience to the concept formation system.
     * The system determines whether this experience belongs to an existing
     * concept or represents a novel concept. This is used for:
     * - Concept discovery from experience
     * - Pattern recognition and classification
     * - Concept formation and refinement
     * - Learning from sensory input
     */
    size_t presentExperience(const std::vector<float>& pattern,
                            const std::vector<float>& features,
                            float reward,
                            SimulationStep currentTime);
    
    /**
     * @brief Get the concept that best matches a pattern
     * 
     * Returns the concept ID that best matches a given pattern based on
     * similarity. This is used for:
     * - Pattern classification
     * - Concept retrieval
     * - Pattern matching
     * - Concept application
     */
    size_t getMatchingConcept(const std::vector<float>& pattern,
                              float similarityThreshold = 0.7f) const;
    
    /**
     * @brief Get all concepts
     * 
     * Returns all discovered concepts. This is used for:
     * - Concept enumeration
     * - Concept management
     * - Concept analysis
     * - Concept organization
     */
    const std::vector<DiscoveredConcept>& getConcepts() const { return concepts_; }
    
    /**
     * @brief Get a specific concept
     * 
     * Returns a specific concept by its ID. This is used for:
     * - Concept retrieval
     * - Concept access
     * - Concept analysis
     * - Concept management
     */
    const DiscoveredConcept* getConcept(size_t conceptId) const;
    
    /**
     * @brief Get concept prototype
     * 
     * Returns the prototype pattern for a specific concept. This is used for:
     * - Concept representation
     * - Pattern matching
     * - Concept analysis
     * - Concept refinement
     */
    std::vector<float> getConceptPrototype(size_t conceptId) const;
    
    /**
     * @brief Get concept instances
     * 
     * Returns all instances for a specific concept. This is used for:
     * - Instance analysis
     * - Concept refinement
     * - Instance management
     * - Concept debugging
     */
    const std::vector<ConceptInstance>& getConceptInstances(size_t conceptId) const;
    
    /**
     * @brief Measure concept stability (0-1)
     * 
     * Returns the stability measurement for a specific concept.
     * Stability represents how consistent and reliable the concept is.
     * This is used for:
     * - Concept quality assessment
     * - Concept validation
     * - Concept stability measurement
     * - Concept quality control
     */
    float getConceptStability(size_t conceptId) const;
    
    /**
     * @brief Get number of concepts discovered
     * 
     * Returns the total number of concepts discovered. This is used for:
     * - Concept counting
     * - Concept enumeration
     * - Concept analysis
     * - Concept management
     */
    size_t getConceptCount() const { return concepts_.size(); }
    
    /**
     * @brief Merge two concepts (when discovered they are actually the same)
     * 
     * Merges two concepts when it's discovered they are actually the same
     * concept. This is used for:
     * - Concept consolidation
     * - Concept merging
     * - Concept refinement
     * - Concept correction
     */
    void mergeConcepts(size_t conceptA, size_t conceptB);
    
    /**
     * @brief Update concept with new instance
     * 
     * Updates a concept with a new instance. This is used for:
     * - Concept refinement
     * - Concept updating
     * - Instance learning
     * - Concept adaptation
     */
    void updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                      const std::vector<float>& features, float reward);
    
    /**
     * @brief Compute similarity between two patterns
     * 
     * Computes the similarity between two patterns based on their
     * vector space representation. This is used for:
     * - Pattern comparison
     * - Concept matching
     * - Similarity measurement
     * - Pattern analysis
     */
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;
    
    /**
     * @brief Check if pattern is novel
     * 
     * Determines whether a pattern is novel (doesn't belong to any
     * existing concept). This is used for:
     * - Novelty detection
     * - Concept discovery
     * - Pattern analysis
     * - Concept formation
     */
    bool isNovel(const std::vector<float>& pattern,
                 float similarityThreshold = 0.7f) const;
    
    /**
     * @brief Get concept for a given pattern
     * 
     * Returns the concept ID for a given pattern. This is used for:
     * - Pattern classification
     * - Concept retrieval
     * - Pattern matching
     * - Concept application
     */
    size_t findConceptForPattern(const std::vector<float>& pattern) const;
    
    /**
     * @brief Clear all concepts
     * 
     * Clears all concepts, removing all stored concept information.
     * This is used for:
     * - Concept reset
     * - Concept management
     * - Resource management
     * - Concept clearing
     */
    void clear();
    
    /**
     * @brief Get generalization ability (can concept handle variations?)
     * 
     * Returns the generalization ability for a specific concept.
     * Generalization ability represents how well the concept can handle
     * variations of patterns. This is used for:
     * - Concept evaluation
     * - Concept quality assessment
     * - Concept analysis
     * - Concept refinement
     */
    float getGeneralizationAbility(size_t conceptId) const;
    
    /**
     * @brief Set parameters
     * 
     * Sets parameters for concept formation. This is used for:
     * - Parameter configuration
     * - Parameter management
     * - Parameter tuning
     * - Concept parameter control
     */
    void setFormationThreshold(float t) { formationThreshold_ = t; }
    void setStabilityWindow(size_t w) { stabilityWindow_ = w; }
    
private:
    /**
     * @brief Create new concept from pattern
     * 
     * Creates a new concept from a given pattern. This is used for:
     * - Concept creation
     * - Concept initialization
     * - Pattern-based concept formation
     * - Concept discovery
     */
    size_t createConcept(const std::vector<float>& pattern,
                         const std::vector<float>& features,
                         float reward);
    
    /**
     * @brief Update concept prototype using Hebbian averaging
     * 
     * Updates the concept prototype using Hebbian averaging.
     * This is used for:
     * - Prototype refinement
     * - Concept updating
     * - Hebbian learning
     * - Concept adaptation
     */
    void updatePrototype(size_t conceptId, const std::vector<float>& newInstance);
    
    /**
     * @brief Check if concept is stable enough to be considered "discovered"
     * 
     * Determines whether a concept is stable enough to be considered
     * "discovered". This is used for:
     * - Concept validation
     * - Concept stability checking
     * - Concept assessment
     * - Concept management
     */
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

/**
 * @class SpatialRepresentation
 * 
 * Neural implementation of spatial relationship learning and position tracking.
 * 
 * This class implements neural spatial representation, learning spatial
 * relationships and position tracking through neural mechanisms. It is
 * essential for:
 * - **Spatial awareness**: Understanding spatial relationships
 * - **Position tracking**: Tracking agent position in space
 * - **Place cell formation**: Forming place cells through exploration
 * - **Spatial memory**: Storing spatial information
 * - **Path integration**: Tracking position through movement
 * 
 * Spatial representation mechanisms in NLM:
 * - **Place cell formation**: Forms place cells through spatial exploration
 * - **Spatial encoding**: Encodes spatial relationships in neural patterns
 * - **Path integration**: Uses dead reckoning for position tracking
 * - **Landmark association**: Associates spatial locations with landmarks
 * - **Spatial memory**: Stores spatial relationships and positions
 * 
 * Key characteristics of spatial representation in NLM:
 * - **Place cells**: Forms place cells that fire at specific locations
 * - **Spatial encoding**: Encodes spatial information in neural patterns
 * - **Path integration**: Tracks position through movement integration
 * - **Landmark learning**: Learns associations with landmarks
 * - **Spatial awareness**: Maintains spatial understanding
 * 
 * @note Place neurons form through spatial exploration
 * @see SpatialRepresentation::PlaceCell - For place cell structure
 * @see ConceptFormation - For spatial concept relationships
 * @see TemporalRelation - For temporal-spatial relationships
 * @see SpatialRepresentation.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class SpatialRepresentation {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new spatial representation instance with default parameters.
     * The spatial representation is initialized with default settings for
     * spatial learning and position tracking.
     */
    SpatialRepresentation();
    
    /**
     * @brief Destructor
     * 
     * Cleans up spatial representation resources and performs any necessary
     * cleanup operations. The spatial representation is left in a valid but
     * non-operational state after destruction.
     */
    ~SpatialRepresentation();
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the spatial representation system with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Record current position experience
     * 
     * Records the current position experience with associated sensory cues.
     * This is used for:
     * - Place cell formation
     * - Spatial learning
     * - Position tracking
     * - Spatial memory
     */
    void recordPosition(float x, float y, const std::vector<float>& sensoryCues);
    
    /**
     * @brief Get predicted position based on path integration
     * 
     * Returns the predicted position based on path integration from the
     * current position. This is used for:
     * - Path integration
     * - Position prediction
     * - Movement tracking
     * - Position estimation
     */
    std::pair<float, float> getPredictedPosition() const;
    
    /**
     * @brief Update position estimate based on movement
     * 
     * Updates the position estimate based on movement in the x and y
     * directions. This is used for:
     * - Path integration
     * - Position tracking
     * - Movement integration
     * - Position update
     */
    void integrateMovement(float dx, float dy);
    
    /**
     * @brief Get position neuron activation for a given position
     * 
     * Returns the position neuron activation for a given position (x, y).
     * This is used for:
     * - Place cell lookup
     * - Position analysis
     * - Spatial query
     * - Position evaluation
     */
    float getPositionActivation(float x, float y) const;
    
    /**
     * @brief Find place neurons near a position
     * 
     * Returns the place neurons near a given position (x, y) within a
     * specific radius. This is used for:
     * - Place cell lookup
     * - Spatial query
     * - Position analysis
     * - Place cell management
     */
    std::vector<NeuronId> getPlaceNeuronsNear(float x, float y, float radius) const;
    
    /**
     * @brief Get grid spacing (if grid cells have formed)
     * 
     * Returns the grid spacing, which represents the spacing between
     * grid cells. This is used for:
     * - Grid cell analysis
     * - Spatial pattern analysis
     * - Position evaluation
     * - Grid cell management
     */
    float getGridSpacing() const { return gridSpacing_; }
    
    /**
     * @brief Clear spatial representations
     * 
     * Clears all spatial representations, removing all stored spatial information.
     * This is used for:
     * - Spatial reset
     * - Resource management
     * - Error recovery
     * - Spatial representation clearing
     */
    void clear();
    
    /**
     * @brief Have place cells formed?
     * 
     * Determines whether place cells have formed. This is used for:
     * - Place cell assessment
     * - Spatial representation evaluation
     * - Place cell management
     * - Spatial representation management
     */
    bool hasPlaceCells() const { return !placeCells_.empty(); }
    
private:
    /**
     * @brief Place cell structure
     * 
     * Represents a place cell with its associated properties.
     * This structure is used by the spatial representation system to:
     * - Store place cell information
     * - Track place cell properties
     * - Manage place cells
     * - Analyze place cells
     */
    struct PlaceCell {
        NeuronId neuron;
        float x;
        float y;
        float fieldRadius;
        float activation;
    };
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<PlaceCell> placeCells_;
    float currentX_;
    float currentY_;
    float gridSpacing_;
    std::vector<float> lastSensoryCue_;
};

/**
 * @class TemporalRelation
 * 
 * Neural implementation of temporal relationship and sequence learning.
 * 
 * This class implements neural temporal representation, learning temporal
 * relationships and sequences through neural mechanisms. It is essential
 * for:
 * - **Sequence learning**: Learning temporal sequences
 * - **Temporal prediction**: Predicting temporal events
 * - **Sequence memory**: Storing temporal sequences
 * - **Temporal reasoning**: Reasoning about temporal relationships
 * - **Temporal integration**: Integrating temporal information
 * 
 * Temporal relation mechanisms in NLM:
 * - **Sequence recording**: Records sequences of events
 * - **Temporal prediction**: Predicts temporal events
 * - **Sequence strength**: Maintains sequence strength and reliability
 * - **Temporal memory**: Stores temporal relationships and sequences
 * - **Temporal integration**: Integrates temporal information
 * 
 * Key characteristics of temporal relation in NLM:
 * - **Sequence recording**: Records sequences of events and their relationships
 * - **Temporal prediction**: Predicts temporal events based on sequences
 * - **Sequence strength**: Maintains sequence strength and observation count
 * - **Temporal memory**: Stores temporal relationships
 * - **Temporal awareness**: Maintains temporal understanding
 * 
 * @note Encodes temporal order in neural activity, not symbolic timestamps
 * @see TemporalRelation::SequenceLink - For sequence link structure
 * @see SpatialRepresentation - For spatial-temporal relationships
 * @see ConceptFormation - For temporal concept relationships
 * @see TemporalRelation.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class TemporalRelation {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new temporal relation instance with default parameters.
     * The temporal relation is initialized with default settings for
     * temporal learning and sequence tracking.
     */
    TemporalRelation();
    
    /**
     * @brief Destructor
     * 
     * Cleans up temporal relation resources and performs any necessary
     * cleanup operations. The temporal relation is left in a valid but
     * non-operational state after destruction.
     */
    ~TemporalRelation();
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the temporal relation system with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Record that event A was followed by event B
     * 
     * Records that event A was followed by event B with a specific reward.
     * This is used for:
     * - Sequence recording
     * - Temporal relationship learning
     * - Sequence tracking
     * - Temporal memory
     */
    void recordSequence(const std::vector<float>& eventA,
                      const std::vector<float>& eventB,
                      float reward);
    
    /**
     * @brief Given event A, predict what comes next
     * 
     * Returns the prediction for what comes next after event A.
     * This is used for:
     * - Temporal prediction
     * - Sequence prediction
     * - Temporal reasoning
     * - Temporal integration
     */
    std::vector<float> predictNext(const std::vector<float>& eventA) const;
    
    /**
     * @brief Given event B, predict what came before
     * 
     * Returns the prediction for what came before event B.
     * This is used for:
     * - Temporal prediction
     * - Sequence prediction
     * - Temporal reasoning
     * - Temporal integration
     */
    std::vector<float> predictPrevious(const std::vector<float>& eventB) const;
    
    /**
     * @brief Get confidence in prediction
     * 
     * Returns the confidence in a prediction for a given event.
     * This is used for:
     * - Prediction confidence assessment
     * - Sequence confidence
     * - Temporal confidence
     * - Prediction evaluation
     */
    float getPredictionConfidence(const std::vector<float>& event) const;
    
    /**
     * @brief Get sequence length
     * 
     * Returns the total number of recorded sequences. This is used for:
     * - Sequence counting
     * - Sequence enumeration
     * - Sequence analysis
     * - Sequence management
     */
    size_t getSequenceLength() const { return sequences_.size(); }
    
    /**
     * @brief Clear sequences
     * 
     * Clears all sequences, removing all stored temporal information.
     * This is used for:
     * - Sequence reset
     * - Resource management
     * - Error recovery
     * - Sequence clearing
     */
    void clear();
    
private:
    /**
     * @brief Sequence link structure
     * 
     * Represents a temporal sequence link between two events.
     * This structure is used by the temporal relation system to:
     * - Store sequence relationships
     * - Track sequence strength
     * - Manage temporal relationships
     * - Analyze temporal sequences
     */
    struct SequenceLink {
        std::vector<float> fromEvent;
        std::vector<float> toEvent;
        float strength;
        size_t observationCount;
    };
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<SequenceLink> sequences_;
};

} // namespace nlm
