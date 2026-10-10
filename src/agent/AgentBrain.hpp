#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @file AgentBrain.hpp
 * 
 * Agent brain interface for NLM
 * 
 * This file provides the agent brain interface for NLM, implementing:
 * - Agent brain initialization
 * - Sensory transduction and motor decoding
 * - Neuromodulation and development
 * - Agent-environment interaction
 * - Curiosity and novelty processing
 * 
 * The agent brain interface in NLM:
 * - **AgentBrain**: Connects NLM brain to the world
 * - **Sensory transduction**: Converts sensory input to neural signals
 * - **Motor decoding**: Converts neural activity to motor commands
 * - **Neuromodulation**: Manages neuromodulation and learning
 * - **Development**: Manages developmental processes
 * - **Curiosity**: Manages curiosity and exploration
 * 
 * Key features:
 * - **Agent brain interface**: Connects NLM brain to the world
 * - **Sensory transduction**: Converts sensory input to neural signals
 * - **Motor decoding**: Converts neural activity to motor commands
 * - **Neuromodulation**: Manages neuromodulation and learning
 * - **Development**: Manages developmental processes
 * - **Curiosity**: Manages curiosity and exploration
 * 
 * Agent brain characteristics in NLM:
 * - **Agent brain interface**: Connects NLM brain to the world
 * - **Sensory transduction**: Converts sensory input to neural signals
 * - **Motor decoding**: Converts neural activity to motor commands
 * - **Neuromodulation**: Manages neuromodulation and learning
 * - **Development**: Manages developmental processes
 * - **Curiosity**: Manages curiosity and exploration
 * 
 * @note Critical for agent-environment interaction
 * @note Used extensively in agent brain systems
 * @note Essential for agent functionality
 * @see AgentBrain.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class AgentBrain
 * 
 * Neural implementation of agent brain interface.
 * 
 * This class implements the agent brain interface, connecting the NLM brain
 * to the world and handling sensory transduction and motor decoding. It is
 * essential for:
 * - **Agent brain interface**: Connects NLM brain to the world
 * - **Sensory transduction**: Converts sensory input to neural signals
 * - **Motor decoding**: Converts neural activity to motor commands
 * - **Neuromodulation**: Manages neuromodulation and learning
 * - **Development**: Manages developmental processes
 * - **Curiosity**: Manages curiosity and exploration
 * 
 * Agent brain characteristics in NLM:
 * - **Agent brain interface**: Connects NLM brain to the world
 * - **Sensory transduction**: Converts sensory input to neural signals
 * - **Motor decoding**: Converts neural activity to motor commands
 * - **Neuromodulation**: Manages neuromodulation and learning
 * - **Development**: Manages developmental processes
 * - **Curiosity**: Manages curiosity and exploration
 * 
 * Key components:
 * - **Initialization**: Initializes agent brain with world
 * - **Sensory processing**: Processes sensory input
 * - **Motor decoding**: Decodes brain motor activity
 * - **Neuromodulation**: Applies neuromodulation based on reward
 * - **Development**: Updates development system
 * - **Curiosity**: Manages curiosity and novelty
 * 
 * @note Critical for agent-environment interaction
 * @see AgentBrain.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class AgentBrain {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default agent brain instance with default parameters.
     * The agent brain is initialized with default values for
     * all agent brain fields.
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destructor
     * 
     * Cleans up agent brain resources and performs any necessary
     * cleanup operations. The agent brain is left in a valid but
     * non-operational state after destruction.
     */
    ~AgentBrain();
    
    /**
     * @brief Initialize with world
     * 
     * Initializes the agent brain with a reference to the world.
     * This is essential for neural integration and interaction with other
     * neural systems. The world reference is used for:
     * - Accessing world state
     * - Establishing neural connections
     * - Integrating with world dynamics
     * - Coordinating with other systems
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get sensory input size expected by brain
     * 
     * Returns the sensory input size expected by the brain. This is used for:
     * - Sensory processing
     * - Sensory access
     * - Sensory processing
     * - Sensory management
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get motor output size expected
     * 
     * Returns the motor output size expected by the brain. This is used for:
     * - Motor output access
     * - Motor output management
     * - Motor output processing
     * - Motor output control
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * Processes a sensory percept and injects it into the brain. This is used for:
     * - Sensory transduction
     * - Sensory processing
     * - Brain integration
     * - Neural integration
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into motor command
     * 
     * Decodes brain motor activity into a motor command. This is used for:
     * - Motor decoding
     * - Motor command selection
     * - Motor output generation
     * - Motor command control
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply neuromodulation based on reward
     * 
     * Applies neuromodulation based on reward. This is used for:
     * - Neuromodulation
     * - Reward processing
     * - Learning enhancement
     * - Neuromodulation management
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update development system
     * 
     * Updates the development system. This is used for:
     * - Development update
     * - Development management
     * - Development processing
     * - Development control
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * 
     * Returns the current developmental stage. This is used for:
     * - Developmental stage access
     * - Developmental stage management
     * - Developmental stage processing
     * - Developmental stage control
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get neuromodulation level
     * 
     * Returns the neuromodulation level. This is used for:
     * - Neuromodulation level access
     * - Neuromodulation level management
     * - Neuromodulation level processing
     * - Neuromodulation level control
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get curiosity level
     * 
     * Returns the curiosity level. This is used for:
     * - Curiosity level access
     * - Curiosity level management
     * - Curiosity level processing
     * - Curiosity level control
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get novelty level
     * 
     * Returns the novelty level. This is used for:
     * - Novelty level access
     * - Novelty level management
     * - Novelty level processing
     * - Novelty level control
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get prediction error
     * 
     * Returns the prediction error. This is used for:
     * - Prediction error access
     * - Prediction error management
     * - Prediction error processing
     * - Prediction error control
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset agent for new episode
     * 
     * Resets the agent for a new episode. This is used for:
     * - Episode reset
     * - Episode management
     * - Episode processing
     * - Episode control
     */
    void reset();
    
    /**
     * @brief Get brain pointer
     * 
     * Returns the brain pointer. This is used for:
     * - Brain access
     * - Brain management
     * - Brain processing
     * - Brain control
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @brief Enable reward modulation
     * 
     * Enables reward modulation. This is used for:
     * - Reward modulation enable
     * - Reward modulation management
     * - Reward modulation processing
     * - Reward modulation control
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enable structural plasticity
     * 
     * Enables structural plasticity. This is used for:
     * - Structural plasticity enable
     * - Structural plasticity management
     * - Structural plasticity processing
     * - Structural plasticity control
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enable development
     * 
     * Enables development. This is used for:
     * - Development enable
     * - Development management
     * - Development processing
     * - Development control
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * @brief Enable curiosity
     * 
     * Enables curiosity. This is used for:
     * - Curiosity enable
     * - Curiosity management
     * - Curiosity processing
     * - Curiosity control
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * @brief Check if reward modulation is enabled
     * 
     * Checks if reward modulation is enabled. This is used for:
     * - Reward modulation checking
     * - Reward modulation management
     * - Reward modulation processing
     * - Reward modulation control
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * @brief Check if structural plasticity is enabled
     * 
     * Checks if structural plasticity is enabled. This is used for:
     * - Structural plasticity checking
     * - Structural plasticity management
     * - Structural plasticity processing
     * - Structural plasticity control
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Check if development is enabled
     * 
     * Checks if development is enabled. This is used for:
     * - Development checking
     * - Development management
     * - Development processing
     * - Development control
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * @brief Check if curiosity is enabled
     * 
     * Checks if curiosity is enabled. This is used for:
     * - Curiosity checking
     * - Curiosity management
     * - Curiosity processing
     * - Curiosity control
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    /**
     * @brief Decode from motor neurons
     * 
     * Decodes from motor neurons. This is used for:
     * - Motor neuron decoding
     * - Motor decoding
     * - Motor output generation
     * - Motor command selection
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief Select with curiosity
     * 
     * Selects with curiosity. This is used for:
     * - Curiosity-based selection
     * - Motor command selection
     * - Exploration enhancement
     * - Curiosity management
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    /** Brain pointer */
    std::shared_ptr<Brain> brain_;
    
    /** Motor neuron groups */
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    /** Sensory neuron groups */
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    /** Neuromodulation state */
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    /** Development state */
    double developmentalAge_;
    float plasticityModifier_;
    
    /** Configuration flags */
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    /** Previous sensory state for novelty detection */
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
