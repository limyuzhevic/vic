#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class MotorSystem
 * @brief Motor system: converts neural activity to actions
 * @details The MotorSystem serves as the output interface of the neural control system,
 * translating neural activity patterns from motor neurons into executable motor commands.
 * It acts as the final stage of the sensorimotor loop, bridging neural computation
 * with environmental interaction.
 * 
 * Action Selection:
 * - Real-time action selection based on neural population activity
 * - Implementation of basal ganglia-like action selection mechanisms
 * - Integration with neuromodulatory signals for exploration/exploitation balance
 * - Support for both reflexive and planned behaviors
 * 
 * Architecture:
 * - Neural input interface: Receives activity from motor neural regions
 * - Action mapping: Transforms activity patterns into specific commands
 * - Policy representation: Maintains learned action preferences
 * - Output interface: Produces Action objects for the agent
 * 
 * Thread Safety: Not thread-safe for concurrent action selection. Action selection
 * operations should be synchronized with neural updates and environment control.
 * 
 * Performance: O(n) for action selection where n is number of motor neurons.
 * O(1) for preference updates and history management.
 * 
 * Biological Connection:
 * - Mirrors motor cortex output organization
 * - Implements basal ganglia direct/indirect pathways
 * - Supports action vigor modulation
 * - Implements motor planning and execution
 * 
 * Note: This is a placeholder implementation. Phase 2 will implement real action
 * selection algorithms including:
 * - Reinforcement learning based policies
 * - Actor-critic architectures
 * - Policy gradient methods
 * - Evolutionary strategies
 * 
 * @code
 * // Motor system integration
 * MotorSystem motorSystem;
 * NeuralRegion* motorRegion = brain->getRegion(motorRegionId);
 * 
 * // Select action based on current neural state
 * std::unique_ptr<Action> action = motorSystem.selectAction(*motorRegion);
 * 
 * // Process action
 * if (action) {
 *     // Apply action to environment
 *     world.applyAction(*action);
 * }
 * 
 * // Update action preferences based on outcomes
 * motorSystem.setActionPreference(ActionType::MoveForward, 0.8f);
 * motorSystem.setActionPreference(ActionType::Wait, 0.2f);
 * @endcode
 * 
 * @see Action
 * @see NeuralRegion
 * @see ActionType
 * @see AgentBrain
 * @see Brain
 */
class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity
    // TODO PHASE 2: Implement real action selection
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
