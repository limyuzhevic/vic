/*
 * Environment.cpp - Implementation of Environment interface
 * 
 * This file implements the Environment class and GridWorldEnvironment for world simulation.
 * The environment provides the interface between the agent's brain and the external world,
 * handling sensory input, actions, rewards, and world state. This is a placeholder
 * implementation for Phase 1, with more sophisticated features planned for Phase 2.
 */

#include "Environment.hpp"
#include "../agent/AgentBody.hpp"
#include "../sensory/InternalSignals.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace nlm {

// Environment implementation
Environment::Environment()
    : state_(nullptr)
    , lastReward_(0.0f)
    , done_(false) {
}

// GridWorldEnvironment implementation
GridWorldEnvironment::GridWorldEnvironment(int width, int height)
    : pImpl(new Impl(width, height)) {
}

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