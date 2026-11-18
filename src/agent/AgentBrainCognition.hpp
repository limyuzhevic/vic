// AgentBrain cognition and thought header
// Handles cognitive functions including planner interaction, concepts, and self-model

#pragma once

#include <string>
#include <vector>

namespace nlm {

class Brain;
class SelfModel;
class SocialLearning;

/**
 * @class AgentBrainCognition
 * @brief Manages cognitive functions and higher-level processing
 * 
 * This module handles all cognitive aspects of the agent:
 * - Self-model development for action prediction
 * - Planner interaction and thought generation
 * - Concept formation from experience
 * - Social learning from other agents
 * - Thought trace generation for debugging and analysis
 * 
 * The cognition module implements the agent's higher-level processing,
 * including learning from experience, predicting others' actions, and
 * forming abstract concepts that guide behavior.
 */
class AgentBrainCognition {
public:
    /**
     * @brief Update self-model for action prediction
     * 
     * Develops a self-model that predicts the consequences of actions by
     * recording the relationship between actions and sensory changes. This
     * allows the agent to anticipate what its own actions will do.
     * 
     * @param selfModel Reference to the SelfModel to update
     * @param selfModelReady Output parameter indicating if self-model is ready
     * @param lastVision Previous vision input (before action)
     * @param currentVision Current vision input (after action)
     * @param lastDecoded Previous decoded motor command
     * @param brain Reference to brain containing the self-model
     */
    static void updateSelfModel(
        SelfModel& selfModel,
        bool& selfModelReady,
        const std::vector<float>& lastVision,
        const std::vector<float>& currentVision,
        MotorCommand lastDecoded,
        Brain* brain);
    
    /**
     * @brief Update planner interaction and thought generation
     * 
     * Interfaces with the brain's planner to get suggested actions based
     * on learned action quality. The planner's suggestions bias the motor
     * decoding process, allowing learned preferences to influence behavior.
     * 
     * @param brain Reference to brain containing the planner
     * @param hasThoughtPlanned Output parameter indicating if thought was planned
     * @param thoughtPlanned Output parameter for planned action
     * @param plannerDecisionCount Output counter for planner decisions
     * @param plannerAgreementCount Output counter for planner agreements
     * @param plannerOverrideCount Output counter for planner overrides
     * @param lastVision Current visual input for planner context
     * @param lastText Current text input for context
     */
    static void updatePlannerInteraction(
        Brain* brain,
        bool& hasThoughtPlanned,
        MotorCommand& thoughtPlanned,
        int& plannerDecisionCount,
        int& plannerAgreementCount,
        int& plannerOverrideCount,
        std::vector<float>& lastVision,
        const std::string& lastText);
    
    /**
     * @brief Update concept learning from experience
     * 
     * Forms abstract concepts from experienced pattern/reward pairings.
     * Concepts capture the relationship between sensory patterns and their
     * associated rewards, allowing the agent to generalize from experience
     * and make predictions about future outcomes.
     * 
     * @param brain Reference to brain containing concept formation
     * @param conceptBiasCount Output counter for concept influence
     * @param lastVision Current visual input for concept matching
     * @param lastTouch Current tactile input for concept matching
     * @param reward Current reward for experience
     * @param developmentalStage Current developmental stage for concept limits
     */
    static void updateConceptFormation(
        Brain* brain,
        int& conceptBiasCount,
        const std::vector<float>& lastVision,
        const std::vector<float>& lastTouch,
        float reward,
        DevelopmentalStage developmentalStage);
    
    /**
     * @brief Update social learning from other agents
     * 
     * Observes other agents' actions and learns from their behavior through
     * imitation and social influence. This enables cultural transmission and
     * learning from others' experiences.
     * 
     * @param socialLearning Reference to social learning system
     * @param socialReady Output parameter indicating if social learning is ready
     * @param socialObservationCount Output counter for social observations
     * @param lastText Current text input (may include social cues)
     */
    static void updateSocialLearning(
        SocialLearning& socialLearning,
        bool& socialReady,
        size_t& socialObservationCount,
        const std::string& lastText);
    
    /**
     * @brief Get command name for debugging
     * 
     * Utility function that returns human-readable names for motor commands.
     * Used in thought traces and debugging output.
     * 
     * @param cmd Motor command to name
     * @return Human-readable command name
     */
    static const char* phase5CmdName(MotorCommand cmd);
    
    /**
     * @brief Generate concept pattern from perception
     * 
     * Converts sensory input (vision + touch) into a concept pattern that
     * can be matched against learned concepts. The pattern uses closeness
     * encoding (1 - vision) to emphasize differences from background.
     * 
     * @param lastVision Visual input for pattern generation
     * @param lastTouch Tactile input for pattern generation
     * @return Concept pattern vector for matching
     */
    static std::vector<float> conceptPattern(
        const std::vector<float>& lastVision,
        const std::vector<float>& lastTouch);
};

} // namespace nlm
