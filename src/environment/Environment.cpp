#include "Environment.hpp"
#include "../core/ErrorHandling.hpp"

namespace nlm {

struct EnvironmentState::Impl {
    bool terminal;
    
    Impl() : terminal(false) {}
};

EnvironmentState::EnvironmentState() : pImpl(new Impl) {}

EnvironmentState::~EnvironmentState() = default;

bool EnvironmentState::is_terminal() const {
    return pImpl->terminal;
}

void EnvironmentState::set_terminal(bool terminal) {
    pImpl->terminal = terminal;
}

Environment::Environment() : state_(nullptr), lastReward_(0.0f), done_(false) {}

struct GridWorldEnvironment::Impl {
    int width;
    int height;
    int agentX;
    int agentY;
    int goalX;
    int goalY;
    int steps;
    int maxSteps;
    bool done;
    EnvironmentState state;
    
    Impl(int w, int h) : width(w), height(h), agentX(0), agentY(0), 
                         goalX(w-1), goalY(h-1), steps(0), maxSteps(1000), done(false) {
        // Validate initialization parameters
        if (w <= 0 || h <= 0) {
            throw NLMError(NLMError::ErrorCode::INVALID_PARAMETER, "Invalid grid dimensions: width and height must be positive");
        }
        if (maxSteps <= 0) {
            throw NLMError(NLMError::ErrorCode::INVALID_PARAMETER, "Invalid max steps: must be positive");
        }
    }
};

GridWorldEnvironment::GridWorldEnvironment(int width, int height) 
    : pImpl(new Impl(width, height)) {}

GridWorldEnvironment::~GridWorldEnvironment() = default;

std::unique_ptr<SensoryInput> GridWorldEnvironment::reset() {
    NLM_LOG_DEBUG("Resetting GridWorldEnvironment");
    
    try {
        // Reset agent position and state
        pImpl->agentX = 0;
        pImpl->agentY = 0;
        pImpl->steps = 0;
        pImpl->done = false;
        pImpl->state.set_terminal(false);
        done_ = false;
        lastReward_ = 0.0f;
        
        // Validate reset state
        NLM_VALIDATE_PARAM(pImpl->agentX >= 0 && pImpl->agentX < pImpl->width, "Reset agentX out of bounds");
        NLM_VALIDATE_PARAM(pImpl->agentY >= 0 && pImpl->agentY < pImpl->height, "Reset agentY out of bounds");
        
        // Return observation (agent position as simple vector)
        auto observation = std::make_unique<InternalSignals>();
        
        // Validate observation creation
        NLM_CHECK_NULL(observation.get(), "Failed to create observation in reset");
        
        observation->addSignal(static_cast<float>(pImpl->agentX) / pImpl->width);
        observation->addSignal(static_cast<float>(pImpl->agentY) / pImpl->height);
        
        NLM_LOG_DEBUG("GridWorldEnvironment reset completed");
        return observation;
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("NLM GridWorldEnvironment reset error (code: ") + std::to_string(static_cast<int>(e.getCode())) + "): " + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during GridWorldEnvironment reset: ") + e.what());
        throw;
    }
}

std::unique_ptr<SensoryInput> GridWorldEnvironment::step(const Action& action) {
    NLM_LOG_DEBUG("GridWorldEnvironment step called");
    
    try {
        if (pImpl->done) {
            NLM_LOG_WARNING("Step called after environment is done, resetting instead");
            return reset();
        }
        
        ++pImpl->steps;
        
        // Validate steps count
        NLM_VALIDATE_PARAM(pImpl->steps >= 0, "Invalid step count after increment");
        
        // Move agent based on action
        switch (action.getType()) {
            case ActionType::MoveForward:
                pImpl->agentY = std::min(pImpl->height - 1, pImpl->agentY + 1);
                break;
            case ActionType::MoveBackward:
                pImpl->agentY = std::max(0, pImpl->agentY - 1);
                break;
            case ActionType::MoveLeft:
                pImpl->agentX = std::max(0, pImpl->agentX - 1);
                break;
            case ActionType::MoveRight:
                pImpl->agentX = std::min(pImpl->width - 1, pImpl->agentX + 1);
                break;
            default:
                NLM_LOG_WARNING("Unknown action type in step, treating as no-op");
                break;
        }
        
        // Validate agent position
        NLM_VALIDATE_PARAM(pImpl->agentX >= 0 && pImpl->agentX < pImpl->width, "agentX out of bounds after movement");
        NLM_VALIDATE_PARAM(pImpl->agentY >= 0 && pImpl->agentY < pImpl->height, "agentY out of bounds after movement");
        
        // Check if reached goal
        float reward = -0.01f;  // Small negative reward for each step
        if (pImpl->agentX == pImpl->goalX && pImpl->agentY == pImpl->goalY) {
            reward = 1.0f;
            pImpl->done = true;
            pImpl->state.set_terminal(true);
            done_ = true;
            NLM_LOG_DEBUG("Agent reached goal! Reward: " + std::to_string(reward));
        }
        
        // Check if max steps reached
        if (pImpl->steps >= pImpl->maxSteps) {
            pImpl->done = true;
            pImpl->state.set_terminal(true);
            done_ = true;
            NLM_LOG_DEBUG("Max steps reached, episode ended");
        }
        
        lastReward_ = reward;
        
        // Return observation
        auto observation = std::make_unique<InternalSignals>();
        
        // Validate observation creation
        NLM_CHECK_NULL(observation.get(), "Failed to create observation in step");
        
        observation->addSignal(static_cast<float>(pImpl->agentX) / pImpl->width);
        observation->addSignal(static_cast<float>(pImpl->agentY) / pImpl->height);
        observation->addSignal(static_cast<float>(pImpl->goalX) / pImpl->width);
        observation->addSignal(static_cast<float>(pImpl->goalY) / pImpl->height);
        
        NLM_LOG_DEBUG("GridWorldEnvironment step completed, reward: " + std::to_string(reward));
        return observation;
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("NLM GridWorldEnvironment step error (code: ") + std::to_string(static_cast<int>(e.getCode())) + "): " + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during GridWorldEnvironment step: ") + e.what());
        throw;
    }
}

const EnvironmentState& GridWorldEnvironment::getState() const {
    return pImpl->state;
}

float GridWorldEnvironment::getLastReward() const {
    return lastReward_;
}

bool GridWorldEnvironment::isDone() const {
    return done_;
}

const char* GridWorldEnvironment::getName() const {
    return "GridWorld";
}

size_t GridWorldEnvironment::getActionSpaceSize() const {
    return 4;  // 4 movement actions
}

size_t GridWorldEnvironment::getObservationSpaceSize() const {
    return 4;  // agent_x, agent_y, goal_x, goal_y
}

} // namespace nlm
