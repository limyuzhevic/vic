// AgentBrain motor processing header
// Handles motor decoding and command selection

#pragma once

#include <vector>
#include <string>

namespace nlm {

class Neuron;
class MotorCommand;

struct MotorGroupActivity;

/**
 * @class AgentBrainMotor
 * @brief Manages motor command decoding and selection
 * 
 * This module handles the complete motor system:
 * - Motor command selection with curiosity and exploration
 * - Winner-take-all lateral inhibition for decision making
 * - Curiosity-modulated action selection
 * - Neural decoding from motor neuron groups
 * - Motor bias application (reflex, planner, concept, self-model, social)
 * 
 * The motor processing module converts neural activity patterns into
 * actual motor commands for agent control, incorporating various biases
 * and regulatory mechanisms to produce adaptive behavior.
 */
class AgentBrainMotor {
public:
    /**
     * @brief Select motor command using curiosity and exploration
     * 
     * This is the main motor command selection function that chooses between
     * different motor actions based on neural firing rates and various
     * regulatory factors including curiosity, repetition, and movement history.
     * 
     * @param motorForward Collection of forward-moving neurons
     * @param motorBackward Collection of backward-moving neurons
     * @param motorTurnLeft Collection of left-turn neurons
     * @param motorTurnRight Collection of right-turn neurons
     * @param motorInteract Collection of interact neurons
     * @param motorWait Collection of wait neurons
     * @param curiosityEnabled Whether curiosity-based exploration is enabled
     * @param defaultCmd Default motor command to return if no clear winner
     * @param curiosityLevel Current curiosity level (0.0 to 1.0)
     * @param hasLastDecoded Whether there was a previous decoded command
     * @param lastDecoded Previous decoded motor command
     * @param stepsSinceTurn Steps since last turning action
     * @param repeatCount Times the same movement has repeated
     * @param consecutiveWinnerCount Consecutive wins by same group
     * @param lastWinnerMargin Margin of victory for last winner
     * @param lastWinnerGroup Index of last winning motor group
     * @return Selected motor command
     */
    static MotorCommand selectMotorCommand(
        const std::vector<Neuron*>& motorForward,
        const std::vector<Neuron*>& motorBackward,
        const std::vector<Neuron*>& motorTurnLeft,
        const std::vector<Neuron*>& motorTurnRight,
        const std::vector<Neuron*>& motorInteract,
        const std::vector<Neuron*>& motorWait,
        bool curiosityEnabled,
        const MotorCommand& defaultCmd,
        float curiosityLevel,
        bool hasLastDecoded,
        const MotorCommand& lastDecoded,
        int stepsSinceTurn,
        int repeatCount,
        int consecutiveWinnerCount,
        float lastWinnerMargin,
        int lastWinnerGroup);
    
    /**
     * @brief Apply winner-take-all lateral inhibition
     * 
     * Implements Phase 5 winner-take-all inhibition where the winning motor
     * group suppresses competing groups with inhibitory pulses. This creates
     * decisive action selection while maintaining competition when appropriate.
     * 
     * @param allMotorNeurons All motor neurons in the system
     * @param lastWinnerGroup Index of the last winning motor group
     * @param lastWinnerMargin Margin by which the winner won
     * @param noveltyLevel Current environmental novelty level
     * @param consecutiveWinnerCount How many times the same group has won
     * @param wtaPulseCount Output reference to WTA pulse counter
     * @param motorForward Reference to forward motor group
     * @param motorBackward Reference to backward motor group
     * @param motorTurnLeft Reference to left turn motor group
     * @param motorTurnRight Reference to right turn motor group
     * @param motorInteract Reference to interact motor group
     * @param motorWait Reference to wait motor group
     */
    static void applyWTAInhibition(
        const std::vector<Neuron*>& allMotorNeurons,
        int lastWinnerGroup,
        float lastWinnerMargin,
        float noveltyLevel,
        int consecutiveWinnerCount,
        int& wtaPulseCount,
        std::vector<Neuron*>& motorForward,
        std::vector<Neuron*>& motorBackward,
        std::vector<Neuron*>& motorTurnLeft,
        std::vector<Neuron*>& motorTurnRight,
        std::vector<Neuron*>& motorInteract,
        std::vector<Neuron*>& motorWait);
    
    /**
     * @brief Curiosity-modulated motor command selection
     * 
     * Adds exploratory behavior based on curiosity levels. When curiosity is
     * high, the agent is more likely to select novel or random actions rather
     * than repeating known successful behaviors.
     * 
     * @param defaultCmd Default motor command to potentially override
     * @param curiosityLevel Current curiosity level (0.0 to 1.0)
     * @return Potentially different motor command based on curiosity
     */
    static MotorCommand selectWithCuriosity(MotorCommand defaultCmd, float curiosityLevel);
    
    /**
     * @brief Decode motor command from neural activity
     * 
     * Core neural decoding function that converts motor neuron firing rates
     * into specific motor commands. This implements the spike-based decoding
     * mechanism where the group with the highest firing rate wins.
     * 
     * @param motorForward Collection of forward-moving neurons
     * @param motorBackward Collection of backward-moving neurons
     * @param motorTurnLeft Collection of left-turn neurons
     * @param motorTurnRight Collection of right-turn neurons
     * @param motorInteract Collection of interact neurons
     * @param motorWait Collection of wait neurons
     * @param activities Output vector for motor group activities
     * @param refractoryCounts Output vector for refractory counts
     * @param sizes Output vector for group sizes
     * @param firingRates Output vector for firing rates
     * @return Decoded motor command based on neural activity
     */
    static MotorCommand decodeFromMotorNeurons(
        const std::vector<Neuron*>& motorForward,
        const std::vector<Neuron*>& motorBackward,
        const std::vector<Neuron*>& motorTurnLeft,
        const std::vector<Neuron*>& motorTurnRight,
        const std::vector<Neuron*>& motorInteract,
        const std::vector<Neuron*>& motorWait,
        std::vector<float>& activities,
        std::vector<size_t>& refractoryCounts,
        std::vector<size_t>& sizes,
        std::vector<float>& firingRates);
};

} // namespace nlm
