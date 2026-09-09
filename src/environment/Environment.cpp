#include "Environment.hpp"

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
        // Validate dimensions
        if (width <= 0 || height <= 0) {
            width = 10;  // Default to 10x10 if invalid
            height = 10;
            goalX = width - 1;
            goalY = height - 1;
        }
        // Ensure goal is within bounds
        goalX = std::max(0, std::min(width - 1, goalX));
        goalY = std::max(0, std::min(height - 1, goalY));
    }
};

GridWorldEnvironment::GridWorldEnvironment(int width, int height) 
    : pImpl(new Impl(width, height)) {}

GridWorldEnvironment::~GridWorldEnvironment() = default;

std::unique_ptr<SensoryInput> GridWorldEnvironment::reset() {
    pImpl->agentX = 0;
    pImpl->agentY = 0;
    pImpl->steps = 0;
    pImpl->done = false;
    pImpl->state.set_terminal(false);
    done_ = false;
    lastReward_ = 0.0f;
    
    // Return observation (agent position as simple vector)
    auto observation = std::make_unique<InternalSignals>();
    observation->addSignal(static_cast<float>(pImpl->agentX) / pImpl->width);
    observation->addSignal(static_cast<float>(pImpl->agentY) / pImpl->height);
    return observation;
}

std::unique_ptr<SensoryInput> GridWorldEnvironment::step(const Action& action) {
    if (pImpl->done) {
        return reset();
    }
    
    ++pImpl->steps;
    
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
            break;
    }
    
    // Check if reached goal
    float reward = -0.01f;  // Small negative reward for each step
    if (pImpl->agentX == pImpl->goalX && pImpl->agentY == pImpl->goalY) {
        reward = 1.0f;
        pImpl->done = true;
        pImpl->state.set_terminal(true);
        done_ = true;
    }
    
    // Check if max steps reached
    if (pImpl->steps >= pImpl->maxSteps) {
        pImpl->done = true;
        pImpl->state.set_terminal(true);
        done_ = true;
    }
    
    lastReward_ = reward;
    
    // Return observation
    auto observation = std::make_unique<InternalSignals>();
    observation->addSignal(static_cast<float>(pImpl->agentX) / pImpl->width);
    observation->addSignal(static_cast<float>(pImpl->agentY) / pImpl->height);
    observation->addSignal(static_cast<float>(pImpl->goalX) / pImpl->width);
    observation->addSignal(static_cast<float>(pImpl->goalY) / pImpl->height);
    return observation;
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
