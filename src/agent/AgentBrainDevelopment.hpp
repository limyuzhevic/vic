// AgentBrain development and neuromodulation header
// Handles development processes, neuromodulation, and learning systems

#pragma once

namespace nlm {

class Brain;
class DevelopmentalStage;

/**
 * @class AgentBrainDevelopment
 * @brief Manages developmental processes and neuromodulation
 * 
 * This module handles all aspects of development and learning:
 * - Developmental stage progression based on age
 * - Reward-modulated learning and synaptic plasticity
 * - Novelty detection and curiosity computation
 * - Neuromodulator (dopamine) effects on learning
 * - Structural plasticity changes with development
 * 
 * The development module implements a biologically inspired developmental
 * timeline where the agent's learning capacity changes over time, mimicking
 * critical periods in biological development. It also handles reward-based
 * learning through dopaminergic signaling.
 */
class AgentBrainDevelopment {
public:
    /**
     * @brief Update developmental stage based on age
     * 
     * Implements a biologically inspired developmental timeline where the
     * agent's learning capacity and behavior change at different life stages:
     * - Initial stage (0-60s): High plasticity, rapid learning
     * - Critical period (60-300s): Modulated plasticity, refined skills
     * - Maturation (300-900s): Reduced plasticity, stable behavior
     * - Adult (900s+): Low plasticity, conservative responses
     * 
     * @param brain Reference to the Brain object being developed
     * @param timestep Current simulation timestep
     * @param developmentalAge Reference to current developmental age
     * @param plasticityModifier Output parameter for plasticity factor
     * @param developmentEnabled Whether development processes are active
     * @param structuralPlasticityEnabled Whether structural changes occur
     */
    static void updateDevelopment(
        Brain& brain,
        double timestep,
        double& developmentalAge,
        float& plasticityModifier,
        bool developmentEnabled,
        bool structuralPlasticityEnabled);
    
    /**
     * @brief Apply reward modulation and learning
     * 
     * Implements reward-modulated learning using dopaminergic signals.
     * When reward prediction error is positive, synaptic weights are increased
     * for activated synapses (learning what's good). When negative, weights
     * are decreased (learning what's bad).
     * 
     * @param brain Reference to the Brain containing synapses to modulate
     * @param reward Actual reward received from environment
     * @param predictedReward Expected reward (prediction)
     * @param predictionError Output parameter for prediction error
     * @param expectedReward Output parameter for expected reward (EMA)
     * @param dopamineLevel Output parameter for dopaminergic signal
     * @param plasticityModifier Current plasticity factor
     * @param rewardModulationEnabled Whether reward learning is active
     */
    static void applyRewardModulation(
        Brain& brain,
        float reward,
        float predictedReward,
        float& predictionError,
        float& expectedReward,
        float& dopamineLevel,
        float plasticityModifier,
        bool rewardModulationEnabled);
    
    /**
     * @brief Update novelty detection and curiosity
     * 
     * Computes novelty as the prediction error between current sensory input
     * and the brain's predicted input. This drives curiosity and exploration
     * behavior by signaling when the environment is novel or unexpected.
     * 
     * @param vision Current visual input from world
     * @param touch Current tactile input from world
     * @param predictedVision Output parameter for predicted vision (EMA)
     * @param noveltyLevel Output parameter for calculated novelty (0.0 to 1.0)
     * @param curiosityLevel Output parameter for curiosity level (0.0 to 1.0)
     * @param predictionError Current prediction error signal
     * @param developmentalAge Reference to current developmental age
     * @param plasticityModifier Output parameter for plasticity factor
     * @param curiosityEnabled Whether curiosity computation is active
     * @param sensoryNoveltyDecay Decay factor for novelty calculation
     */
    static void updateNoveltyAndCuriosity(
        const std::vector<float>& vision,
        const std::vector<float>& touch,
        std::vector<float>& predictedVision,
        float& noveltyLevel,
        float& curiosityLevel,
        float predictionError,
        float& developmentalAge,
        float& plasticityModifier,
        bool curiosityEnabled,
        float sensoryNoveltyDecay);
};

} // namespace nlm
