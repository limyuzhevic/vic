// AgentBrain initializer header
// Contains initialization logic for AgentBrain

#pragma once

#include "../core/Neuron.hpp"

namespace nlm {

class AgentBrain;
class Brain;
class SimpleWorld;

/**
 * @class AgentBrainInitializer
 * @brief Handles initialization and setup of AgentBrain components
 * 
 * This module provides static methods for:
 * - Distributing motor neurons into action groups (forward, backward, turn left/right, interact, wait)
 * - Distributing sensory neurons into modality groups (vision, touch, internal, proprioception)
 * - Initializing all neuron groups from brain regions
 * - Setting up the agent brain with a world instance
 * 
 * The initialization process ensures proper neuron distribution and setup for
 * the agent's interaction with the environment, forming the foundation for
 * sensory processing and motor control.
 */
class AgentBrainInitializer {
public:
    /**
     * @brief Distribute motor neurons into action groups
     * 
     * This method takes a collection of motor neurons and assigns them to
     * different action groups in a round-robin fashion. This creates the
     * motor neuron populations that will be controlled by the brain.
     * 
     * @param neurons Collection of motor neurons to distribute
     * @param motorForward Reference to motor forward group output vector
     * @param motorBackward Reference to motor backward group output vector
     * @param motorTurnLeft Reference to motor turn left group output vector
     * @param motorTurnRight Reference to motor turn right group output vector
     * @param motorInteract Reference to motor interact group output vector
     * @param motorWait Reference to motor wait group output vector
     */
    static void distributeMotorNeurons(
        const std::vector<std::shared_ptr<Neuron>>& neurons,
        std::vector<Neuron*>& motorForward,
        std::vector<Neuron*>& motorBackward,
        std::vector<Neuron*>& motorTurnLeft,
        std::vector<Neuron*>& motorTurnRight,
        std::vector<Neuron*>& motorInteract,
        std::vector<Neuron*>& motorWait);
    
    /**
     * @brief Distribute sensory neurons into modality groups
     * 
     * This method takes a collection of sensory neurons and assigns them to
     * different sensory modality groups based on their type. This creates
     * the sensory neuron populations for vision, touch, proprioception, etc.
     * 
     * @param neurons Collection of sensory neurons to distribute
     * @param visionNeurons Reference to vision sensory group output vector
     * @param touchNeurons Reference to touch sensory group output vector
     * @param internalNeurons Reference to internal sensory group output vector
     * @param proprioceptionNeurons Reference to proprioception sensory group output vector
     */
    static void distributeSensoryNeurons(
        const std::vector<std::shared_ptr<Neuron>>& neurons,
        std::vector<Neuron*>& visionNeurons,
        std::vector<Neuron*>& touchNeurons,
        std::vector<Neuron*>& internalNeurons,
        std::vector<Neuron*>& proprioceptionNeurons);
    
    /**
     * @brief Initialize all neuron groups from brain regions
     * 
     * This is the main initialization method that processes all brain regions
     * and populates the motor and sensory neuron groups in the AgentBrain.
     * It calls the specific distribution methods for motor and sensory neurons.
     * 
     * @param agentBrain Reference to the AgentBrain being initialized
     * @param brain Constant reference to the Brain containing regions and populations
     * @param motorForward Reference to motor forward group output vector
     * @param motorBackward Reference to motor backward group output vector
     * @param motorTurnLeft Reference to motor turn left group output vector
     * @param motorTurnRight Reference to motor turn right group output vector
     * @param motorInteract Reference to motor interact group output vector
     * @param motorWait Reference to motor wait group output vector
     * @param internalNeurons Reference to internal neurons output vector
     * @param visionNeurons Reference to vision neurons output vector
     * @param touchNeurons Reference to touch neurons output vector
     * @param proprioceptionNeurons Reference to proprioception neurons output vector
     */
    static void initializeMotorGroups(
        AgentBrain& agentBrain,
        const Brain& brain,
        std::vector<Neuron*>& motorForward,
        std::vector<Neuron*>& motorBackward,
        std::vector<Neuron*>& motorTurnLeft,
        std::vector<Neuron*>& motorTurnRight,
        std::vector<Neuron*>& motorInteract,
        std::vector<Neuron*>& motorWait,
        std::vector<Neuron*>& internalNeurons,
        std::vector<Neuron*>& visionNeurons,
        std::vector<Neuron*>& touchNeurons,
        std::vector<Neuron*>& proprioceptionNeurons);
    
    /**
     * @brief Initialize agent brain with world
     * 
     * This method sets up the AgentBrain for interaction with a SimpleWorld,
     * clearing any previous state and preparing the agent for new episodes.
     * It also logs initialization information for debugging purposes.
     * 
     * @param agentBrain Reference to the AgentBrain to initialize
     * @param world Constant reference to the SimpleWorld to initialize with
     */
    static void initializeAgentBrain(AgentBrain& agentBrain, const SimpleWorld& world);
};

} // namespace nlm
