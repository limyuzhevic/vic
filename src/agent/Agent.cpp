#include "Agent.hpp"
#include "AgentBrain.hpp"
#include "AgentCommands.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

Agent::Agent(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , agentBrain_(brain)
    , agentBody_()
    , isAlive_(true)
    , totalReward_(0.0)
    , totalSteps_(0)
    , totalActions_(0)
    , lastStepTime_(0.0)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
{
    if (!brain_) {
        NLM_LOG_ERROR("Agent: Cannot create agent with null brain");
        isAlive_ = false;
    }
}

Agent::~Agent() = default;

bool Agent::initialize(const SimpleWorld& world) {
    if (!isAlive_) {
        NLM_LOG_ERROR("Agent: Cannot initialize dead agent");
        return false;
    }
    
    // Initialize agent brain with world
    agentBrain_.initialize(world);
    
    // Reset agent body state
    agentBody_.reset();
    
    // Reset counters
    totalReward_ = 0.0;
    totalSteps_ = 0;
    totalActions_ = 0;
    lastStepTime_ = 0.0;
    
    // Generate initial random position within world bounds
    const float worldWidth = static_cast<float>(world.getWorldWidth());
    const float worldHeight = static_cast<float>(world.getWorldHeight());
    
    agentBody_.x = (std::rand() % static_cast<int>(worldWidth * 10)) / 10.0f;
    agentBody_.y = (std::rand() % static_cast<int>(worldHeight * 10)) / 10.0f;
    
    // Initial orientation
    agentBody_.orientation = (std::rand() % 628) / 100.0f;  // -6.28 to 6.28
    
    NLM_LOG_INFO("Agent initialized at position (" + 
                 std::to_string(agentBody_.x) + ", " + 
                 std::to_string(agentBody_.y) + ") with orientation " +
                 std::to_string(agentBody_.orientation) + " rad");
    
    return true;
}

void Agent::step(double timestep) {
    if (!isAlive_) return;
    
    lastStepTime_ = timestep;
    totalSteps_++;
    
    // Get current world state
    const SimpleWorld& world = SimpleWorld::getInstance();
    
    // Update sensory input
    updateSensoryInput(world);
    
    // Let brain process sensory input
    agentBrain_.processSensoryInput(currentPercept_);
    
    // Generate motor output based on brain activity
    generateMotorOutput();
    
    // Apply motor command to agent body
    applyPhysics(timestep);
    
    // Update internal state
    updateState(timestep);
    
    // Check if agent should die
    checkAgentHealth(timestep);
    
    // Log every 100 steps
    if (totalSteps_ % 100 == 0) {
        NLM_LOG_INFO("Agent step " + std::to_string(totalSteps_) + 
                     ", pos: (" + std::to_string(agentBody_.x) + ", " +
                     std::to_string(agentBody_.y) + "), " +
                     "reward: " + std::to_string(totalReward_));
    }
}

SensoryPercept Agent::getSensoryPercept() const {
    return currentPercept_;
}

std::shared_ptr<Action> Agent::getAction() const {
    return currentAction_;
}

void Agent::applyReward(float reward) {
    if (!isAlive_) return;
    
    totalReward_ += reward;
    
    // Apply reward modulation through agent brain
    agentBrain_.applyRewardModulation(reward, 0.0f);  // Assume no predicted reward
    
    // Update agent's internal reward processing
    if (reward > 0.1f) {
        agentBody_.health = std::min(1.0f, agentBody_.health + 0.05f);
    } else if (reward < -0.1f) {
        agentBody_.health = std::max(0.0f, agentBody_.health - 0.1f);
    }
    
    // Log significant rewards
    if (std::abs(reward) > 1.0f) {
        NLM_LOG_INFO("Agent received significant reward: " + std::to_string(reward) +
                     ", total: " + std::to_string(totalReward_));
    }
}

bool Agent::isAlive() const {
    return isAlive_ && agentBody_.health > 0.0f && agentBody_.energy > 0.0f;
}

const AgentBody& Agent::getAgentBody() const {
    return agentBody_;
}

std::shared_ptr<Brain> Agent::getBrain() const {
    return brain_;
}

void Agent::reset() {
    // Reset agent body
    agentBody_.reset();
    
    // Reset agent brain
    agentBrain_.reset();
    
    // Reset counters
    totalReward_ = 0.0;
    totalSteps_ = 0;
    totalActions_ = 0;
    
    // Reset neural state through brain
    if (brain_) {
        brain_->reset();
    }
    
    isAlive_ = true;
    
    NLM_LOG_INFO("Agent reset to initial state");
}

void Agent::enableRewardModulation(bool enable) {
    rewardModulationEnabled_ = enable;
    agentBrain_.enableRewardModulation(enable);
}

void Agent::enableStructuralPlasticity(bool enable) {
    structuralPlasticityEnabled_ = enable;
    agentBrain_.enableStructuralPlasticity(enable);
}

void Agent::enableDevelopment(bool enable) {
    developmentEnabled_ = enable;
    agentBrain_.enableDevelopment(enable);
}

void Agent::enableCuriosity(bool enable) {
    curiosityEnabled_ = enable;
    agentBrain_.enableCuriosity(enable);
}

bool Agent::isRewardModulationEnabled() const {
    return rewardModulationEnabled_;
}

bool Agent::isStructuralPlasticityEnabled() const {
    return structuralPlasticityEnabled_;
}

bool Agent::isDevelopmentEnabled() const {
    return developmentEnabled_;
}

bool Agent::isCuriosityEnabled() const {
    return curiosityEnabled_;
}

float Agent::getDopamineLevel() const {
    return agentBrain_.getNeuromodulationLevel();
}

float Agent::getNoveltyLevel() const {
    return agentBrain_.getNoveltyLevel();
}

float Agent::getCuriosityLevel() const {
    return agentBrain_.getCuriosityLevel();
}

float Agent::getPredictionError() const {
    return agentBrain_.getPredictionError();
}

DevelopmentalStage Agent::getDevelopmentalStage() const {
    return agentBrain_.getDevelopmentalStage();
}

double Agent::getTotalReward() const {
    return totalReward_;
}

size_t Agent::getTotalSteps() const {
    return totalSteps_;
}

size_t Agent::getTotalActions() const {
    return totalActions_;
}

void Agent::updateSensoryInput(const SimpleWorld& world) {
    // Create sensory percept from world state
    currentPercept_.setTimestamp(lastStepTime_);
    
    // Get vision data (simplified: check nearby cells)
    std::vector<float> vision;
    const float agentX = agentBody_.x;
    const float agentY = agentBody_.y;
    const float visionRange = 2.0f;  // 2 cell radius
    const size_t visionWidth = world.getVisionWidth();
    const size_t visionHeight = world.getVisionHeight();
    
    for (size_t y = 0; y < visionHeight; ++y) {
        for (size_t x = 0; x < visionWidth; ++x) {
            // Calculate world position
            float worldX = static_cast<float>(x) - static_cast<float>(visionWidth) / 2.0f;
            float worldY = static_cast<float>(y) - static_cast<float>(visionHeight) / 2.0f;
            
            // Rotate based on agent orientation
            float cosA = std::cos(agentBody_.orientation);
            float sinA = std::sin(agentBody_.orientation);
            float rotatedX = worldX * cosA - worldY * sinA;
            float rotatedY = worldX * sinA + worldY * cosA;
            
            // Scale and translate to world coordinates
            rotatedX = rotatedX * visionRange + agentX;
            rotatedY = rotatedY * visionRange + agentY;
            
            // Check if within world bounds
            if (rotatedX >= 0 && rotatedX < world.getWorldWidth() &&
                rotatedY >= 0 && rotatedY < world.getWorldHeight()) {
                
                // Get cell type
                WorldObjectType cellType = world.getCellType(
                    static_cast<size_t>(rotatedX),
                    static_cast<size_t>(rotatedY)
                );
                
                // Convert to intensity
                float intensity = 0.0f;
                switch (cellType) {
                    case WorldObjectType::Empty: intensity = 0.1f; break;
                    case WorldObjectType::Resource: intensity = 0.8f; break;
                    case WorldObjectType::Hazard: intensity = 0.3f; break;
                    case WorldObjectType::Wall: intensity = 0.9f; break;
                    case WorldObjectType::Marker: intensity = 0.5f; break;
                }
                
                vision.push_back(intensity);
            } else {
                vision.push_back(0.0f);  // Out of bounds
            }
        }
    }
    
    currentPercept_.setVision(vision);
    
    // Get touch sensors (check nearby cells for collisions)
    std::vector<float> touch;
    const float touchRadius = 0.3f;
    
    // Check 8 surrounding cells
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            float checkX = agentBody_.x + dx * 0.5f;
            float checkY = agentBody_.y + dy * 0.5f;
            
            if (checkX >= 0 && checkX < world.getWorldWidth() &&
                checkY >= 0 && checkY < world.getWorldHeight()) {
                
                WorldObjectType cellType = world.getCellType(
                    static_cast<size_t>(checkX),
                    static_cast<size_t>(checkY)
                );
                
                // Touch if it's not empty
                touch.push_back((cellType != WorldObjectType::Empty) ? 1.0f : 0.0f);
            } else {
                touch.push_back(0.0f);
            }
        }
    }
    
    currentPercept_.setTouch(touch);
    
    // Internal state signals
    std::vector<float> internal;
    internal.push_back(agentBody_.health);      // Health
    internal.push_back(agentBody_.energy);      // Energy
    internal.push_back(std::abs(agentBody_.velocityX));  // Movement speed
    internal.push_back(std::abs(agentBody_.angularVelocity));  // Turning speed
    
    currentPercept_.setInternal(internal);
    
    // Proprioception (body state)
    std::vector<float> proprioception;
    proprioception.push_back(agentBody_.x / world.getWorldWidth());
    proprioception.push_back(agentBody_.y / world.getWorldHeight());
    proprioception.push_back(agentBody_.orientation / (2.0f * M_PI));  // Normalize to [0, 1]
    proprioception.push_back(agentBody_.velocityX / 2.0f);
    proprioception.push_back(agentBody_.velocityY / 2.0f);
    proprioception.push_back(agentBody_.angularVelocity / (2.0f * M_PI));
    
    currentPercept_.setProprioception(proprioception);
}

void Agent::generateMotorOutput() {
    // Get action from agent brain
    MotorCommand motorCmd = agentBrain_.decodeMotorCommand();
    
    // Convert motor command to Action object
    currentAction_ = std::make_shared<Action>(ActionType::Wait);  // Default
    
    switch (motorCmd) {
        case MotorCommand::MoveForward:
            currentAction_ = std::make_shared<Action>(ActionType::MoveForward);
            break;
        case MotorCommand::MoveBackward:
            currentAction_ = std::make_shared<Action>(ActionType::MoveBackward);
            break;
        case MotorCommand::TurnLeft:
            currentAction_ = std::make_shared<Action>(ActionType::TurnLeft);
            break;
        case MotorCommand::TurnRight:
            currentAction_ = std::make_shared<Action>(ActionType::TurnRight);
            break;
        case MotorCommand::Interact:
            currentAction_ = std::make_shared<Action>(ActionType::Interact);
            break;
        case MotorCommand::LookLeft:
            currentAction_ = std::make_shared<Action>(ActionType::Look);
            // Add parameter for direction
            {
                std::vector<float> params = { -1.0f };  // Look left
                currentAction_->setParameters(params);
            }
            break;
        case MotorCommand::LookRight:
            currentAction_ = std::make_shared<Action>(ActionType::Look);
            // Add parameter for direction
            {
                std::vector<float> params = { 1.0f };  // Look right
                currentAction_->setParameters(params);
            }
            break;
        case MotorCommand::Wait:
        default:
            currentAction_ = std::make_shared<Action>(ActionType::Wait);
            break;
    }
    
    totalActions_++;
}

void Agent::applyPhysics(double timestep) {
    if (!currentAction_) return;
    
    ActionType actionType = currentAction_->getType();
    const std::vector<float>& params = currentAction_->getParameters();n
    
    // Store old position for collision detection
    float oldX = agentBody_.x;
    float oldY = agentBody_.y;
    float oldOrientation = agentBody_.orientation;
    
    // Apply action effects
    switch (actionType) {
        case ActionType::MoveForward:
            agentBody_.velocityX += std::cos(agentBody_.orientation) * 0.2f;
            agentBody_.velocityY += std::sin(agentBody_.orientation) * 0.2f;
            break;
        case ActionType::MoveBackward:
            agentBody_.velocityX -= std::cos(agentBody_.orientation) * 0.2f;
            agentBody_.velocityY -= std::sin(agentBody_.orientation) * 0.2f;
            break;
        case ActionType::MoveLeft:
            agentBody_.velocityX -= std::sin(agentBody_.orientation) * 0.2f;
            agentBody_.velocityY += std::cos(agentBody_.orientation) * 0.2f;
            break;
        case ActionType::MoveRight:
            agentBody_.velocityX += std::sin(agentBody_.orientation) * 0.2f;
            agentBody_.velocityY -= std::cos(agentBody_.orientation) * 0.2f;
            break;
        case ActionType::TurnLeft:
            agentBody_.angularVelocity += 0.2f;  // Positive = turn left
            break;
        case ActionType::TurnRight:
            agentBody_.angularVelocity -= 0.2f;  // Negative = turn right
            break;
        case ActionType::Look:
            if (!params.empty()) {
                // Look in direction
                agentBody_.orientation += params[0] * 0.3f;
            }
            break;
        case ActionType::Interact:
            // Consume some energy
            agentBody_.energy = std::max(0.0f, agentBody_.energy - 0.1f);
            break;
        case ActionType::Rest:
            // Stop all motion
            agentBody_.velocityX = 0.0f;
            agentBody_.velocityY = 0.0f;
            agentBody_.angularVelocity = 0.0f;
            // Regenerate energy slowly
            agentBody_.energy = std::min(1.0f, agentBody_.energy + 0.05f);
            break;
        case ActionType::Eat:
            // Find food and eat it
            agentBody_.energy = std::min(1.0f, agentBody_.energy + 0.3f);
            break;
        case ActionType::Drink:
            // Restore hydration/health
            agentBody_.health = std::min(1.0f, agentBody_.health + 0.2f);
            break;
        case ActionType::Wait:
        default:
            // Only apply friction
            agentBody_.velocityX *= 0.95f;
            agentBody_.velocityY *= 0.95f;
            agentBody_.angularVelocity *= 0.9f;
            break;
    }
    
    // Apply physics integration
    const float dt = static_cast<float>(timestep);
    
    // Update position
    agentBody_.x += agentBody_.velocityX * dt;
    agentBody_.y += agentBody_.velocityY * dt;
    
    // Update orientation
    agentBody_.orientation += agentBody_.angularVelocity * dt;
    
    // Normalize orientation to [0, 2π]
    while (agentBody_.orientation < 0.0f) agentBody_.orientation += 2.0f * M_PI;
    while (agentBody_.orientation >= 2.0f * M_PI) agentBody_.orientation -= 2.0f * M_PI;
    
    // Apply friction
    agentBody_.velocityX *= 0.95f;
    agentBody_.velocityY *= 0.95f;
    agentBody_.angularVelocity *= 0.9f;
    
    // Update age
    agentBody_.age += dt;
    
    // Check world boundaries
    const SimpleWorld& world = SimpleWorld::getInstance();
    const float worldWidth = static_cast<float>(world.getWorldWidth());
    const float worldHeight = static_cast<float>(world.getWorldHeight());
    
    // Wrap around edges (toroidal world)
    if (agentBody_.x < 0.0f) agentBody_.x = worldWidth - fmod(agentBody_.x, worldWidth);
    else if (agentBody_.x >= worldWidth) agentBody_.x = fmod(agentBody_.x, worldWidth);
    
    if (agentBody_.y < 0.0f) agentBody_.y = worldHeight - fmod(agentBody_.y, worldHeight);
    else if (agentBody_.y >= worldHeight) agentBody_.y = fmod(agentBody_.y, worldHeight);
    
    // Energy consumption for movement
    float movementEnergy = std::sqrt(agentBody_.velocityX * agentBody_.velocityX +
                                     agentBody_.velocityY * agentBody_.velocityY) * 0.01f;
    movementEnergy += std::abs(agentBody_.angularVelocity) * 0.01f;
    
    agentBody_.energy = std::max(0.0f, agentBody_.energy - movementEnergy);
    
    // Age-related energy consumption
    agentBody_.energy = std::max(0.0f, agentBody_.energy - 0.001f);
    
    agentBody_.isMoving = (movementEnergy > 0.001f);
    agentBody_.isTurning = (std::abs(agentBody_.angularVelocity) > 0.01f);
    agentBody_.lastActionTime = static_cast<float>(timestep);
}

void Agent::updateState(double timestep) {
    // Update agent brain with current state
    if (brain_) {
        brain_->updateDevelopment(static_cast<float>(timestep));
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        // This will be updated in the brain processing loop
    }
    
    // Update energy decay over time
    agentBody_.energy = std::max(0.0f, agentBody_.energy - 0.001f * static_cast<float>(timestep));
    
    // Natural health recovery
    if (agentBody_.health < 1.0f) {
        agentBody_.health = std::min(1.0f, agentBody_.health + 0.001f * static_cast<float>(timestep));
    }
}

void Agent::checkAgentHealth(double timestep) {
    // Check energy level
    if (agentBody_.energy <= 0.0f) {
        NLM_LOG_INFO("Agent energy depleted - agent dying");
        isAlive_ = false;
        return;
    }
    
    // Check health level
    if (agentBody_.health <= 0.0f) {
        NLM_LOG_INFO("Agent health depleted - agent dying");
        isAlive_ = false;
        return;
    }
    
    // Check if agent has been inactive too long
    if (timestep - lastStepTime_ > 100.0) {
        // Starvation/death from inactivity
        agentBody_.health = std::max(0.0f, agentBody_.health - 0.1f);
        NLM_LOG_INFO("Agent starving from inactivity, health: " + std::to_string(agentBody_.health));
    }
}

} // namespace nlm