#include "Environment.hpp"

namespace nlm {

struct EnvironmentState::Impl {
    std::unique_ptr<EnvironmentStateManager> stateManager;
    
    Impl() {
        // Initialize with default conditions
        EnvironmentalConditions conditions;
        conditions.temperature = 20.0f;
        conditions.humidity = 50.0f;
        conditions.pressure = 1013.25f;
        conditions.lightLevel = 1.0f;
        conditions.windSpeed = 0.0f;
        conditions.windDirection = 0.0f;
        conditions.timeOfDay = 12.0f;
        conditions.dayNightCycle = 1.0f;
        conditions.season = 2.0f;
        conditions.radiationLevel = 0.0f;
        conditions.pollutionLevel = 0.0f;
        conditions.visibility = 100.0f;
        
        stateManager.reset(new EnvironmentStateManager());
        stateManager->initialize(conditions);
    }
};

EnvironmentState::EnvironmentState() : pImpl(new Impl) {}

EnvironmentState::~EnvironmentState() = default;

struct RealEnvironmentWrapper::Impl {
    std::unique_ptr<RealEnvironment> realEnv;
    float lastReward;
    bool done;
    
    Impl(float width, float height) : lastReward(0.0f), done(false) {
        realEnv.reset(new RealEnvironment());
        realEnv->initialize(width, height);
    }
};

RealEnvironmentWrapper::RealEnvironmentWrapper(float width, float height) 
    : pImpl(new Impl(width, height)) {}

RealEnvironmentWrapper::~RealEnvironmentWrapper() = default;

std::unique_ptr<SensoryInput> RealEnvironmentWrapper::reset() {
    pImpl->realEnv->reset();
    pImpl->lastReward = 0.0f;
    pImpl->done = false;
    
    return std::make_unique<InternalSignals>();
}

std::unique_ptr<SensoryInput> RealEnvironmentWrapper::step(const Action& action) {
    // Convert Action to MotorCommand
    MotorCommand motorCmd;
    switch (action.getType()) {
        case ActionType::MoveForward:
            motorCmd = MotorCommand::MoveForward;
            break;
        case ActionType::MoveBackward:
            motorCmd = MotorCommand::MoveBackward;
            break;
        case ActionType::MoveLeft:
            motorCmd = MotorCommand::LookLeft;  // No left/right movement in current system
            break;
        case ActionType::MoveRight:
            motorCmd = MotorCommand::LookRight;  // No left/right movement in current system
            break;
        case ActionType::LookLeft:
            motorCmd = MotorCommand::LookLeft;
            break;
        case ActionType::LookRight:
            motorCmd = MotorCommand::LookRight;
            break;
        case ActionType::Interact:
            motorCmd = MotorCommand::Interact;
            break;
        case ActionType::Wait:
            motorCmd = MotorCommand::Wait;
            break;
        default:
            motorCmd = MotorCommand::Wait;
            break;
    }
    
    // Apply motor command
    ActionResult result = pImpl->realEnv->applyMotorCommand(motorCmd, pImpl->realEnv->getSimulationTime());
    
    // Update state
    pImpl->lastReward = result.reward;
    
    // Generate sensory input
    auto sensory = std::make_unique<InternalSignals>();
    
    // Add agent state to sensory input
    const AgentBody& agent = pImpl->realEnv->getAgentBody();
    sensory->addSignal(agent.x);
    sensory->addSignal(agent.y);
    sensory->addSignal(agent.orientation);
    sensory->addSignal(agent.energy);
    sensory->addSignal(agent.health);
    
    return sensory;
}

const EnvironmentState& RealEnvironmentWrapper::getState() const {
    // Return a static state (could be enhanced)
    static EnvironmentState state;
    return state;
}

float RealEnvironmentWrapper::getLastReward() const {
    return pImpl->lastReward;
}

bool RealEnvironmentWrapper::isDone() const {
    return pImpl->done;
}

const char* RealEnvironmentWrapper::getName() const {
    return "RealEnvironmentWrapper";
}

size_t RealEnvironmentWrapper::getActionSpaceSize() const {
    return 8; // 8 possible motor commands
}

size_t RealEnvironmentWrapper::getObservationSpaceSize() const {
    return 6; // x, y, orientation, energy, health, internal signals
}

struct GridWorldEnvironment::Impl {
    float agentX;
    float agentY;
    float goalX;
    float goalY;
    float lastReward;
    bool done;
    EnvironmentState environmentState;
    
    Impl(int width, int height) 
        : agentX(0.0f), agentY(0.0f), goalX(width - 1), goalY(height - 1),
          lastReward(0.0f), done(false) {
        // Initialize environment state
        EnvironmentalConditions conditions;
        conditions.temperature = 20.0f;
        conditions.humidity = 50.0f;
        environmentState.initialize(conditions);
    }
};

GridWorldEnvironment::GridWorldEnvironment(int width, int height) 
    : pImpl(new Impl(width, height)) {}

GridWorldEnvironment::~GridWorldEnvironment() = default;

std::unique_ptr<SensoryInput> GridWorldEnvironment::reset() {
    pImpl->agentX = 0;
    pImpl->agentY = 0;
    pImpl->lastReward = 0.0f;
    pImpl->done = false;
    
    // Update environment state
    EnvironmentalConditions conditions = pImpl->environmentState.getEnvironmentalConditions();
    conditions.timeOfDay = 0.0f;
    pImpl->environmentState.initialize(conditions);
    
    auto observation = std::make_unique<InternalSignals>();
    observation->addSignal(static_cast<float>(pImpl->agentX) / (pImpl->goalX + 1));
    observation->addSignal(static_cast<float>(pImpl->agentY) / (pImpl->goalY + 1));
    observation->addSignal(static_cast<float>(pImpl->goalX) / (pImpl->goalX + 1));
    observation->addSignal(static_cast<float>(pImpl->goalY) / (pImpl->goalY + 1));
    return observation;
}

std::unique_ptr<SensoryInput> GridWorldEnvironment::step(const Action& action) {
    if (pImpl->done) {
        return reset();
    }
    
    // Move agent based on action
    switch (action.getType()) {
        case ActionType::MoveForward:
            pImpl->agentY = std::min(pImpl->goalY, pImpl->agentY + 1);
            break;
        case ActionType::MoveBackward:
            pImpl->agentY = std::max(0.0f, pImpl->agentY - 1);
            break;
        case ActionType::MoveLeft:
            pImpl->agentX = std::max(0.0f, pImpl->agentX - 1);
            break;
        case ActionType::MoveRight:
            pImpl->agentX = std::min(pImpl->goalX, pImpl->agentX + 1);
            break;
        default:
            break;
    }
    
    // Check if reached goal
    float reward = -0.01f;  // Small negative reward for each step
    if (pImpl->agentX == pImpl->goalX && pImpl->agentY == pImpl->goalY) {
        reward = 1.0f;
        pImpl->done = true;
        pImpl->environmentState.set_terminal(true);
    }
    
    // Check if max steps reached
    static int steps = 0;
    steps++;
    if (steps >= 1000) {
        pImpl->done = true;
        pImpl->environmentState.set_terminal(true);
    }
    
    pImpl->lastReward = reward;
    
    // Update environment state
    EnvironmentalConditions conditions = pImpl->environmentState.getEnvironmentalConditions();
    conditions.timeOfDay = steps * 0.001f;  // Advance time
    pImpl->environmentState.initialize(conditions);
    
    // Return observation
    auto observation = std::make_unique<InternalSignals>();
    observation->addSignal(static_cast<float>(pImpl->agentX) / (pImpl->goalX + 1));
    observation->addSignal(static_cast<float>(pImpl->agentY) / (pImpl->goalY + 1));
    observation->addSignal(static_cast<float>(pImpl->goalX) / (pImpl->goalX + 1));
    observation->addSignal(static_cast<float>(pImpl->goalY) / (pImpl->goalY + 1));
    return observation;
}

const EnvironmentState& GridWorldEnvironment::getState() const {
    return pImpl->environmentState;
}

float GridWorldEnvironment::getLastReward() const {
    return pImpl->lastReward;
}

bool GridWorldEnvironment::isDone() const {
    return pImpl->done;
}

const char* GridWorldEnvironment::getName() const {
    return "GridWorldEnvironment";
}

size_t GridWorldEnvironment::getActionSpaceSize() const {
    return 4;  // 4 movement actions
}

size_t GridWorldEnvironment::getObservationSpaceSize() const {
    return 4;  // agent_x, agent_y, goal_x, goal_y
}

} // namespace nlm