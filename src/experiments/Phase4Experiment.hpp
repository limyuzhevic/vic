#pragma once

/**
 * NLM Phase 4: Emerging Cognitive Abilities
 * 
 * Phase 4 represents the development of advanced cognitive capabilities
 * in the NLM (Neural Learning Machine). This phase demonstrates how
 * artificial developmental systems can acquire sophisticated cognitive
 * functions including prediction, memory, planning, attention, and
 * social cognition through intensive learning and neural development.
 * 
 * Phase 4 capabilities include:
 * - Neural prediction and pattern recognition
 * - Working and episodic memory systems
 * - Concept formation and abstract reasoning
 * - Multi-step planning and goal-directed behavior
 * - Selective attention and distraction resistance
 * - Self-modeling and body awareness
 * - Social learning and imitation
 * - Continual learning and generalization
 * 
 * This phase establishes the cognitive foundation necessary for
 * higher-level functions like lifelong learning and social interaction,
 * preparing the system for the integration and specialization phase (Phase 5)
 * and the final integration phase (Phase 6).
 */

#include "../agent/AgentBody.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

/**
 * Phase4Results struct - Comprehensive experiment results for Phase 4
 * 
 * Complete results container for Phase 4 cognitive development experiments.
 * Captures all aspects of cognitive performance including prediction accuracy,
 * memory capabilities, concept formation, planning skills, and social cognition.
 * 
 * This struct provides a standardized way to evaluate and compare the
 * development of cognitive abilities across different experimental conditions
 * and time periods. It supports both simple performance measurement and
 * complex cognitive analysis.
 * 
 * Results categories:
 * - Prediction and inference (error reduction, accuracy)
 * - Memory systems (retention, capacity, recall)
 * - Cognitive formation (concepts, stability, generalization)
 * - Executive functions (attention, planning, self-model)
 * - Social cognition (imitation, observation, cooperation)
 * - Overall performance (reward, improvement, transfer)
 * 
 * Usage example:
 * @code
 * Phase4Results results;
 * results.predictionAccuracyImprovement = 0.85f;
 * results.workingMemoryCapacity = 25.0f;
 * results.conceptsFormed = 10;
 * results.planningAccuracy = 0.92f;
 * results.imitationAccuracy = 0.78f;
 * @endcode
 */
struct Phase4Results {
    // Prediction and inference metrics
    float predictionErrorInitial;           ///< Initial prediction error
    float predictionErrorFinal;             ///< Final prediction error
    float predictionAccuracyImprovement;     ///< Reduction in prediction error
    
    // Working memory metrics
    float workingMemoryRetentionInitial;     ///< Initial memory retention
    float workingMemoryRetentionFinal;       ///< Final memory retention
    float workingMemoryCapacity;            ///< Working memory capacity
    
    // Episodic memory metrics
    float episodicRecallAccuracy;           ///< Accuracy in episodic recall
    size_t episodesStored;                   ///< Number of episodes stored
    float episodicInfluence;                ///< Influence of episodic memory
    
    // Associative memory metrics
    float associationStrength;              ///< Strength of associative connections
    float associationTransfer;              ///< Transfer of associations across contexts
    
    // Concept formation metrics
    size_t conceptsFormed;                   ///< Number of concepts formed
    float conceptStability;                  ///< Stability of formed concepts
    float generalizationAbility;            ///< Ability to generalize learned concepts
    
    // Attention metrics
    float attentionSelectivity;             ///< Selectivity of attention
    float distractionResistance;             ///< Resistance to distractions
    
    // Planning metrics
    float planningAccuracy;                  ///< Accuracy of planning
    float multiStepSuccess;                  ///< Success rate in multi-step tasks
    float planningConfidence;                ///< Confidence in planned actions
    
    // Self-model metrics
    float selfPredictionAccuracy;           ///< Accuracy of self-prediction
    float bodyAwareness;                    ///< Awareness of body state
    float selfOtherDistinction;             ///< Ability to distinguish self from others
    
    // Social learning metrics
    float imitationAccuracy;                ///< Accuracy of imitation learning
    float socialPredictionAccuracy;         ///< Accuracy in predicting social behavior
    size_t observationsFromOthers;          ///< Number of observations from others
    
    // Overall performance metrics
    float totalReward;                      ///< Total reward accumulated
    float behaviorImprovement;              ///< Improvement in behavior
    float transferPerformance;              ///< Transfer of learning to new tasks
    
    /**
     * Default constructor
     * 
     * Initializes all result metrics to their default (zero) values.
     * Useful for creating empty result containers or resetting results.
     */
    Phase4Results();
    
    /**
     * Reset all results to initial state
     * 
     * Resets all result metrics to their starting values, preparing
     * the results container for a new experimental run.
     */
    void reset();
    
    /**
     * Convert results to CSV format
     * 
     * Generates a CSV-formatted string representation of all results
     * for easy storage, analysis, or import into external tools.
     * 
     * @return CSV-formatted string with all result values
     */
    std::string toCSV() const;
};

// ============================================================================
// Temporal Prediction Learning
// ============================================================================

/**
 * TemporalPredictionExperiment - Tests temporal predictive learning
 * 
 * This experiment tests whether the NLM can learn temporal sequences
 * where state A predicts state B, developing an understanding of
 * temporal relationships and causality in its environment.
 * 
 * The experiment presents the agent with correlated patterns A and B
 * (where A is always followed by B) and measures how well the agent
 * can predict future states based on past observations.
 * 
 * Key aspects:
 * - Sequential pattern learning
 * - Temporal prediction accuracy
 * - Error reduction over time
 * - Pattern recognition and generalization
 * 
 * Example: If pattern A = [1, 0, 1] always leads to pattern B = [0, 1, 0],
 * the agent learns that A predicts B and can anticipate B's occurrence.
 */
class TemporalPredictionExperiment {
public:
    /**
     * Default constructor
     */
    TemporalPredictionExperiment();
    
    /**
     * Destructor
     */
    ~TemporalPredictionExperiment();
    
    /**
     * Run the temporal prediction experiment
     * 
     * Executes the temporal prediction learning task, presenting
     * correlated patterns and measuring prediction accuracy.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Experiment results including prediction accuracy metrics
     */
    Phase4Results run(Brain* brain, size_t numTrials = 100);
    
    /**
     * Get prediction error history
     * 
     * Retrieves the prediction error values for each trial, showing
     * how prediction accuracy changes over time.
     * 
     * @return Vector of prediction errors for each trial
     */
    const std::vector<float>& getErrorHistory() const { return errorHistory_; }
    
private:
    std::vector<float> errorHistory_; ///< Prediction error for each trial
};

// ============================================================================
// Working Memory Test
// ============================================================================

/**
 * WorkingMemoryExperiment - Tests working memory capabilities
 * 
 * This experiment tests whether the NLM can maintain and manipulate
 * information in working memory despite the absence of the original
 * stimulus, a key aspect of human-like cognitive abilities.
 * 
 * The experiment uses a delayed response paradigm where the agent
 * must store a pattern in working memory and then reproduce it
 * after a delay period, testing memory retention and precision.
 * 
 * Key aspects:
 * - Memory storage without continuous input
 * - Delay period effects on memory retention
 * - Memory capacity limits
 * - Pattern similarity and distortion
 * 
 * This tests the agent's ability to form stable representations
 * that persist over time and resist interference.
 */
class WorkingMemoryExperiment {
public:
    WorkingMemoryExperiment();
    ~WorkingMemoryExperiment();
    
    /**
     * Run the working memory experiment
     * 
     * Executes the delayed response working memory task, testing
     * the agent's ability to maintain and retrieve patterns over time.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Experiment results including memory retention metrics
     */
    Phase4Results run(Brain* brain, size_t numTrials = 50);
    
    /**
     * Get retention by delay history
     * 
     * Retrieves retention accuracy measurements for different delay periods,
     * showing how memory performance degrades over time.
     * 
     * @return Vector of retention values for different delay periods
     */
    std::vector<float> getRetentionByDelay() const { return retentionByDelay_; }
    
private:
    std::vector<float> retentionByDelay_; ///< Retention accuracy by delay
};

// ============================================================================
// Episodic Memory Recall
// ============================================================================

/**
 * EpisodicRecallExperiment - Tests episodic memory capabilities
 * 
 * This experiment tests whether the NLM can form and retrieve episodic
 * memories - specific events with contextual details that can be
 * recalled later and influence future behavior.
 * 
 * The experiment stores various episodes with different characteristics
 * (sensory state, reward, action) and tests whether the agent can
 * retrieve similar episodes based on current sensory input.
 * 
 * Key aspects:
 * - Episodic memory formation
 * - Similarity-based retrieval
 * - Contextual memory influence
 * - Memory consolidation over time
 * 
 * This tests the agent's ability to create rich, contextual memories
 * that guide behavior based on past experiences.
 */
class EpisodicRecallExperiment {
public:
    EpisodicRecallExperiment();
    ~EpisodicRecallExperiment();
    
    /**
     * Run the episodic recall experiment
     * 
     * Executes the episodic memory recall task, testing the agent's
     * ability to form and retrieve episodic memories.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Experiment results including recall accuracy metrics
     */
    Phase4Results run(Brain* brain, size_t numTrials = 50);
    
    /**
     * Get recall accuracy history
     * 
     * Retrieves recall accuracy measurements for each trial,
     * showing how memory performance improves over time.
     * 
     * @return Vector of recall accuracy values for each trial
     */
    std::vector<float> getRecallAccuracyHistory() const { return recallHistory_; }
    
private:
    std::vector<float> recallHistory_; ///< Recall accuracy over trials
};

// ============================================================================
// Concept Formation
// ============================================================================

/**
 * ConceptFormationExperiment - Tests concept formation capabilities
 * 
 * This experiment tests whether the NLM can form stable concepts
 * from recurring patterns, abstracting away surface variations to
 * capture underlying similarities and relationships.
 * 
 * The experiment presents variations of the same underlying concept
 * and tests whether the agent can learn the abstract rules that
 * define the concept, applying them to novel instances.
 * 
 * Key aspects:
 * - Pattern abstraction and generalization
 * - Concept stability over time
 * - Learning from multiple instances
 * - Novel instance generalization
 * 
 * This tests the agent's ability to move from specific experiences
 * to general knowledge, a crucial aspect of intelligent cognition.
 */
class ConceptFormationExperiment {
public:
    ConceptFormationExperiment();
    ~ConceptFormationExperiment();
    
    /**
     * Run the concept formation experiment
     * 
     * Executes the concept formation task, testing the agent's
     * ability to form and generalize concepts from experience.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Experiment results including concept formation metrics
     */
    Phase4Results run(Brain* brain, size_t numTrials = 100);
    
    /**
     * Get concept stability history
     * 
     * Retrieves concept stability measurements for each checkpoint,
     * showing how concepts consolidate and stabilize over time.
     * 
     * @return Vector of concept stability values over time
     */
    std::vector<float> getConceptStabilityHistory() const { return stabilityHistory_; }
    
private:
    std::vector<float> stabilityHistory_; ///< Concept stability over time
};

// ============================================================================
// Attention Selection
// ============================================================================

/**
 * AttentionExperiment - Tests attention selection capabilities
 * 
 * This experiment tests whether the NLM can selectively process
 * competing stimuli, focusing on relevant information while
 * filtering out irrelevant or distracting input.
 * 
 * The experiment presents multiple competing stimuli with different
 * levels of salience and tests whether the agent can select the
 * most relevant stimulus based on bottom-up and top-down cues.
 * 
 * Key aspects:
 * - Competitive stimulus processing
 * * - Bottom-up salience processing
 * - Top-down attentional control
 * - Selective focus and filtering
 * 
 * This tests the agent's ability to prioritize information and
 * maintain focus on task-relevant inputs.
 */
class AttentionExperiment {
public:
    AttentionExperiment();
    ~AttentionExperiment();
    
    /**
     * Run the attention experiment
     * 
     * Executes the attention selection task, testing the agent's
     * ability to selectively process competing stimuli.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Experiment results including attention selectivity metrics
     */
    Phase4Results run(Brain* brain, size_t numTrials = 50);
    
    /**
     * Get attention selectivity history
     * 
     * Retrieves attention selectivity measurements for each trial,
     * showing how attentional control develops and stabilizes.
     * 
     * @return Vector of attention selectivity values over trials
     */
    std::vector<float> getSelectivityHistory() const { return selectivityHistory_; }
    
private:
    std::vector<float> selectivityHistory_; ///< Attention selectivity over trials
};

// ============================================================================
// Multi-Step Planning
// ============================================================================

/**
 * PlanningExperiment - Tests multi-step planning capabilities
 * 
 * This experiment tests whether the NLM can plan multi-step sequences
 * to achieve goals, demonstrating higher-order cognitive functions
 * like reasoning, strategy, and foresight.
 * 
 * The experiment requires the agent to plan actions that accomplish
 * goals requiring multiple steps, testing the agent's ability to
 * think ahead, anticipate consequences, and adapt plans based on
 * changing conditions.
 * 
 * Key aspects:
 * - Sequential planning and reasoning
 * - Goal-directed behavior
 * - Action quality evaluation
 * - Plan updating and adaptation
 * 
 * This tests the agent's ability to solve complex problems requiring
 * forward thinking and strategic planning.
 */
class PlanningExperiment {
public:
    PlanningExperiment();
    ~PlanningExperiment();
    
    /**
     * Run the planning experiment
     * 
     * Executes the multi-step planning task, testing the agent's
     * ability to plan and execute complex action sequences.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Experiment results including planning accuracy metrics
     */
    Phase4Results run(Brain* brain, size_t numTrials = 30);
    
    /**
     * Get planning success history
     * 
     * Retrieves planning success measurements for each trial,
     * showing how planning accuracy improves with experience.
     * 
     * @return Vector of planning success values over trials
     */
    std::vector<float> getSuccessHistory() const { return successHistory_; }
    
private:
    std::vector<float> successHistory_; ///< Planning success over trials
};

// ============================================================================
// Self-Action Prediction
// ============================================================================

/**
 * SelfModelExperiment - Tests self-modeling and prediction capabilities
 * 
 * This experiment tests whether the NLM can predict the consequences
 * of its own actions on its sensory state, developing an internal
 * model of cause and effect relationships.
 * 
 * The experiment requires the agent to record its actions and the
 * resulting sensory changes, then learn to predict those changes
 * before they occur, developing a sophisticated understanding of
 * its own influence on the environment.
 * 
 * Key aspects:
 * - Self-action consequence learning
 * - Predictive self-modeling
 * - Internal state representation
 * - Causal inference
 * 
 * This tests the agent's ability to develop an internal model
 * of its own effects on the world, crucial for effective planning.
 */
class SelfModelExperiment {
public:
    SelfModelExperiment();
    ~SelfModelExperiment();
    
    /**
     * Run the self-model experiment
     * 
     * Executes the self-action prediction task, testing the agent's
     * ability to predict its own action consequences.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Experiment results including self-prediction accuracy metrics
     */
    Phase4Results run(Brain* brain, size_t numTrials = 50);
    
    /**
     * Get self-prediction history
     * 
     * Retrieves self-prediction accuracy measurements for each trial,
     * showing how self-modeling accuracy improves with experience.
     * 
     * @return Vector of self-prediction accuracy values over trials
     */
    std::vector<float> getSelfPredictionHistory() const { return selfPredHistory_; }
    
private:
    std::vector<float> selfPredHistory_; ///< Self-prediction accuracy over trials
};

// ============================================================================
// Object Permanence
// ============================================================================

/**
 * ObjectPermanenceExperiment - Tests object permanence and hidden object search
 * 
 * This experiment tests whether the NLM can track and search for
 * hidden objects, demonstrating an understanding that objects
 * continue to exist even when not directly observable.
 * 
 * The experiment requires the agent to search for objects that
 * have been hidden or moved, testing spatial memory, object
 * tracking, and the ability to form and maintain mental representations
 * of object locations.
 * 
 * Key aspects:
 * - Object tracking and memory
 * - Hidden object search strategies
 * - Spatial reasoning
 * - Mental representation of object locations
 * 
 * This tests the agent's ability to form and maintain internal
 * representations of objects that are not currently visible.
 */
class ObjectPermanenceExperiment {
public:
    ObjectPermanenceExperiment();
    ~ObjectPermanenceExperiment();
    
    /**
     * Run the object permanence experiment
     * 
     * Executes the object permanence task, testing the agent's
     * ability to search for and track hidden objects.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Experiment results including object permanence metrics
     */
    Phase4Results run(Brain* brain, size_t numTrials = 30);
    
private:
    std::vector<float> searchBehaviorHistory_; ///< Search behavior over trials
};

// ============================================================================
// Social Observation
// ============================================================================

/**
 * SocialLearningExperiment - Tests social learning and imitation capabilities
 * 
 * This experiment tests whether the NLM can learn from observing
 * other agents perform actions and then imitate those actions,
 * demonstrating social cognition and cultural transmission abilities.
 * 
 * The experiment requires the agent to observe another agent's
 * behavior and its consequences, then reproduce similar actions
 * in similar situations, testing the agent's ability to learn
 * socially through observation and imitation.
 * 
 * Key aspects:
 * - Observational learning
 * - Imitation accuracy
 * - Social rule extraction
 * - Cultural transmission
 * 
 * This tests the agent's ability to learn from others and transmit
 * learned behaviors across individuals and generations.
 */
class SocialLearningExperiment {
public:
    SocialLearningExperiment();
    ~SocialLearningExperiment();
    
    /**
     * Run the social learning experiment
     * 
     * Executes the social learning task, testing the agent's
     * ability to learn from observation and imitate others.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Experiment results including imitation accuracy metrics
     */
    Phase4Results run(Brain* brain, size_t numTrials = 30);
    
    /**
     * Get imitation history
     * 
     * Retrieves imitation accuracy measurements for each trial,
     * showing how imitation learning accuracy develops over time.
     * 
     * @return Vector of imitation accuracy values over trials
     */
    std::vector<float> getImitationHistory() const { return imitationHistory_; }
    
private:
    std::vector<float> imitationHistory_; ///< Imitation accuracy over trials
};

// ============================================================================
// Continual Learning
// ============================================================================

/**
 * ContinualLearningExperiment - Tests continual learning capabilities
 * 
 * This experiment tests whether the NLM can learn new tasks
 * without forgetting previously learned ones, demonstrating
 * continual or lifelong learning abilities.
 * 
 * The experiment presents multiple tasks in sequence, testing
 * whether the agent's performance on earlier tasks degrades
 * as it learns new ones (catastrophic forgetting) and whether
 * it can maintain performance across a lifetime of learning.
 * 
 * Key aspects:
 * - Task sequence learning
 * - Catastrophic forgetting prevention
 * - Knowledge transfer across tasks
 * - Performance stability over time
 * 
 * This tests the agent's ability to maintain stable knowledge
 * while continuously acquiring new skills and information.
 */
class ContinualLearningExperiment {
public:
    ContinualLearningExperiment();
    ~ContinualLearningExperiment();
    
    /**
     * Run the continual learning experiment
     * 
     * Executes the continual learning task, testing the agent's
     * ability to learn multiple tasks without catastrophic forgetting.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Experiment results including continual learning metrics
     */
    Phase4Results run(Brain* brain, size_t numTrials = 100);
    
    /**
     * Get task performance history
     * 
     * Retrieves performance measurements for each task, showing
     * how performance evolves across the task sequence.
     * 
     * @return Vector of performance values for each task
     */
    std::vector<float> getTaskPerformance() const { return taskPerformance_; }
    
private:
    std::vector<float> taskPerformance_; ///< Performance across tasks
};

// ============================================================================
// Generalization
// ============================================================================

/**
 * GeneralizationExperiment - Tests knowledge generalization capabilities
 * 
 * This experiment tests whether the NLM can generalize learned knowledge
 * to novel situations, applying learned concepts to new contexts and
 * adapting to slight variations in input patterns.
 * 
 * The experiment trains the agent on one context and tests its
 * ability to apply that knowledge to similar but different contexts,
 * measuring how well the agent can transfer learned skills to new
 * situations with small variations.
 * 
 * Key aspects:
 * - Transfer of learned knowledge
 * - Generalization to novel instances
 * - Robustness to variations
 * - Cross-context learning application
 * 
 * This tests the agent's ability to move from specific training
 * to general application, a crucial aspect of intelligent behavior.
 */
class GeneralizationExperiment {
public:
    GeneralizationExperiment();
    ~GeneralizationExperiment();
    
    /**
     * Run the generalization experiment
     * 
     * Executes the generalization task, testing the agent's
     * ability to transfer learned knowledge to novel situations.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Experiment results including generalization metrics
     */
    Phase4Results run(Brain* brain, size_t numTrials = 50);
    
    /**
     * Get transfer ratio
     * 
     * Retrieves the ratio of performance on novel instances
     * compared to training instances, measuring generalization ability.
     * 
     * @return Transfer ratio (1.0 = perfect transfer, <1.0 = some loss)
     */
    float getTransferRatio() const { return transferRatio_; }
    
private:
    float transferRatio_; ///< Performance transfer ratio
};

// ============================================================================
// Integrated Phase 4 Experiment
// ============================================================================

/**
 * Phase4IntegratedExperiment - Comprehensive integration of all Phase 4 systems
 * 
 * This experiment represents the complete integration of all Phase 4 cognitive
 * capabilities in a complex environment, requiring the agent to coordinate
 * prediction, memory, concept formation, planning, and social cognition
 * simultaneously.
 * 
 * The experiment runs multiple episodes where the agent must apply all
 * its cognitive abilities to solve complex tasks, demonstrating the
 * ability to integrate and coordinate different cognitive functions
 * in real-time.
 * 
 * Key aspects:
 * - System integration and coordination
 * - Multi-cognitive function application
 * - Complex task solving
 * - Real-time cognitive processing
 * 
 * This tests the agent's ability to integrate and apply all its
 * cognitive capabilities in complex, dynamic environments.
 */
class Phase4IntegratedExperiment {
public:
    Phase4IntegratedExperiment();
    ~Phase4IntegratedExperiment();
    
    /**
     * Run the integrated Phase 4 experiment
     * 
     * Executes the complete Phase 4 integration experiment, applying
     * all cognitive capabilities in a complex environment.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numEpisodes Number of episodes to run
     * @param stepsPerEpisode Number of steps per episode
     * @return Experiment results including all cognitive performance metrics
     */
    Phase4Results run(Brain* brain, size_t numEpisodes = 100, size_t stepsPerEpisode = 200);
    
    /**
     * Get episode results
     * 
     * Retrieves the detailed results for each episode, showing
     * performance trends and development over the course of the experiment.
     * 
     * @return Vector of results for each episode
     */
    const std::vector<Phase4Results>& getEpisodeResults() const { return episodeResults_; }
    
    /**
     * Get before/after comparison
     * 
     * Provides a comparison of performance in early vs. late stages
     * of the experiment, showing learning and development progress.
     * 
     * @return Comparison results between early and late performance
     */
    Phase4Results compareBeforeAfter() const;
    
private:
    std::vector<Phase4Results> episodeResults_; ///< Results for each episode
};

// ============================================================================
// Ablation Experiment
// ============================================================================

/**
 * AblationExperiment - Tests cognitive component contributions
 * 
 * This experiment systematically tests the contribution of individual
 * cognitive components to overall performance by temporarily disabling
 * or ablating specific mechanisms.
 * 
 * The experiment runs baseline performance and then compares it with
 * performance when specific cognitive systems (prediction, memory,
 * planning, attention, etc.) are disabled, measuring the contribution
 * of each system to overall cognitive performance.
 * 
 * Key aspects:
 * - Systematic component testing
 * - Contribution measurement
 * - Performance baseline establishment
 * - Component importance evaluation
 * 
 * This helps understand which cognitive mechanisms are most critical
 * and how they interact to produce intelligent behavior.
 */
class AblationExperiment {
public:
    AblationExperiment();
    ~AblationExperiment();
    
    /**
     * Structure for ablation experiment results
     * 
     * Contains results for both baseline and ablated conditions,
     * allowing comparison of performance with and without specific
     * cognitive components.
     */
    struct AblationResult {
        std::string mechanism;           ///< Name of the ablated mechanism
        Phase4Results withMechanism;     ///< Performance with mechanism intact
        Phase4Results withoutMechanism;  ///< Performance with mechanism ablated
        float contribution;              ///< Measured contribution to performance
    };
    
    /**
     * Run the ablation experiment
     * 
     * Executes the systematic ablation study, testing the contribution
     * of individual cognitive components to overall performance.
     * 
     * @param brain Pointer to the brain to use for the experiment
     * @param numTrials Number of trials to run
     * @return Vector of results for each ablated mechanism
     */
    std::vector<AblationResult> run(Brain* brain, size_t numTrials = 30);
    
private:
    std::vector<AblationResult> results_; ///< Results for each ablation
};

// ============================================================================
// Representation Analysis Tools
// ============================================================================

/**
 * RepresentationAnalyzer - Tools for analyzing neural representations
 * 
 * This class provides a comprehensive set of tools for analyzing and
 * quantifying neural representations, including pattern similarity,
 * stability, clustering, selectivity, and temporal precision.
 * 
 * These analysis tools are essential for understanding how neural
 * activity represents information and how representations change
 * over time, supporting cognitive science and neuroscience research.
 * 
 * Key analysis capabilities:
 * - Pattern similarity measurement (cross-correlation)
 * - Representational stability over time
 * - Clustering and categorization analysis
 * - Neural response selectivity
 * - Temporal precision measurement
 * 
 * These tools support both basic and advanced analysis of neural
 * data, helping to understand the computational principles underlying
 * cognitive and neural processes.
 */
class RepresentationAnalyzer {
public:
    RepresentationAnalyzer();
    ~RepresentationAnalyzer();
    
    /**
     * Compute pattern similarity between two neural patterns
     * 
     * Measures how similar two neural activation patterns are using
     * Pearson correlation, providing a value between 0 (completely
     * dissimilar) and 1 (identical).
     * 
     * @param a First neural pattern (vector of activation values)
     * @param b Second neural pattern (vector of activation values)
     * @return Similarity value (0.0 to 1.0)
     */
    static float computePatternSimilarity(const std::vector<float>& a,
                                         const std::vector<float>& b);
    
    /**
     * Measure representational stability over time
     * 
     * Computes how stable neural representations are across multiple
     * time points, measuring the consistency of pattern representation.
     * 
     * @param patterns Vector of neural patterns across time
     * @return Stability measure (0.0 to 1.0)
     */
    static float measureRepresentationalStability(
        const std::vector<std::vector<float>>& patterns);
    
    /**
     * Measure clustering coefficient
     * 
     * Computes a simplified clustering measure for pattern analysis,
     * indicating how patterns group into distinct categories.
     * 
     * @param patterns Vector of neural patterns
     * @param numClusters Number of clusters to identify
     * @return Clustering measure (0.0 to 1.0)
     */
    static float measureClustering(const std::vector<std::vector<float>>& patterns,
                                   size_t numClusters);
    
    /**
     * Measure neural response selectivity
     * 
     * Computes how selective neural responses are, measuring the degree
     * to which neurons respond preferentially to specific patterns.
     * 
     * @param responses Vector of neural response patterns
     * @return Selectivity measure (0.0 to 1.0)
     */
    static float measureSelectivity(const std::vector<std::vector<float>>& responses);
    
    /**
     * Compute population vector for neural activities
     * 
     * Computes a population-level representation of neural activity
     * by taking a weighted average of individual neuron activations.
     * 
     * @param neuronActivities Vector of neuron ID and activation pairs
     * @return Population vector representation
     */
    static std::vector<float> computePopulationVector(
        const std::vector<std::pair<NeuronId, float>>& neuronActivities);
    
    /**
     * Measure temporal precision of neural patterns
     * 
     * Measures the consistency and precision of neural patterns
     * over time, indicating how reliably the network represents
     * information across temporal sequences.
     * 
     * @param temporalPatterns Vector of temporal neural patterns
     * @return Temporal precision measure
     */
    static float measureTemporalPrecision(
        const std::vector<std::vector<float>>& temporalPatterns);
    
private:
    /**
     * Compute dot product for pattern similarity
     * 
     * Helper function for computing dot product between two vectors
     * in pattern similarity calculations.
     */
    static float dotProduct(const std::vector<float>& a, const std::vector<float>& b);
    
    /**
     * Compute vector norm (magnitude)
     * 
     * Helper function for computing vector norm in pattern similarity calculations.
     */
    static float vectorNorm(const std::vector<float>& v);
};

} // namespace nlm