#include "NLMImprovedAPI.hpp"
#include <iostream>

namespace nlm {

struct AgentContextManager::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    bool initialized;
    std::vector<std::string> log_messages;
    std::vector<SensoryPercept> percept_history;
    
    Impl() : brain(nullptr), world(nullptr), agent(nullptr), initialized(false) {}
};

AgentContextManager::AgentContextManager() : pImpl(std::make_unique<Impl>()) {}

AgentContextManager::AgentContextManager(std::shared_ptr<Brain> brain,
                                         std::shared_ptr<SimpleWorld> world) 
    : pImpl(std::make_unique<Impl>()) {
    pImpl->brain = brain;
    pImpl->world = world;
}

AgentContextManager::~AgentContextManager() {
    if (pImpl->agent) {
        pImpl->agent->reset();
    }
}

AgentContextManager::AgentContextManager(AgentContextManager&& other) noexcept : pImpl(std::move(other.pImpl)) {}

AgentContextManager& AgentContextManager::operator=(AgentContextManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool AgentContextManager::enter() {
    if (!pImpl->brain || !pImpl->world) {
        pImpl->log_messages.push_back("ERROR: Missing brain or world");
        return false;
    }
    
    pImpl->agent = std::make_shared<AgentBrain>(pImpl->brain);
    
    if (!pImpl->agent) {
        pImpl->log_messages.push_back("ERROR: Failed to create agent");
        return false;
    }
    
    try {
        pImpl->agent->initialize(*pImpl->world);
        pImpl->initialized = true;
        pImpl->log_messages.push_back("AgentContextManager: Agent initialized successfully");
        return true;
    } catch (const std::exception& e) {
        pImpl->log_messages.push_back(std::string("ERROR: Agent initialization failed: ") + e.what());
        return false;
    }
}

void AgentContextManager::exit(int exception_code) {
    if (exception_code != 0 && pImpl->agent) {
        pImpl->agent->reset();
        pImpl->log_messages.push_back("AgentContextManager: Agent reset due to exception");
    }
    
    pImpl->initialized = false;
    pImpl->agent.reset();
}

std::shared_ptr<AgentBrain> AgentContextManager::agent() const {
    return pImpl->agent;
}

std::shared_ptr<Brain> AgentContextManager::brain() const {
    return pImpl->brain;
}

std::shared_ptr<SimpleWorld> AgentContextManager::world() const {
    return pImpl->world;
}

void AgentContextManager::initialize() {
    if (!pImpl->agent) {
        pImpl->log_messages.push_back("ERROR: No agent to initialize");
        return;
    }
    
    if (pImpl->initialized) {
        pImpl->agent->reset();
    }
    
    try {
        pImpl->agent->initialize(*pImpl->world);
        pImpl->initialized = true;
        pImpl->log_messages.push_back("AgentContextManager: Agent initialized");
    } catch (const std::exception& e) {
        pImpl->log_messages.push_back(std::string("ERROR: Agent initialization failed: ") + e.what());
    }
}

void AgentContextManager::reset() {
    if (pImpl->agent) {
        pImpl->agent->reset();
        pImpl->initialized = false;
        pImpl->log_messages.push_back("AgentContextManager: Agent reset");
    }
}

void AgentContextManager::process_sensor_data(const SensoryPercept& percept) {
    if (!pImpl->agent || !pImpl->initialized) {
        pImpl->log_messages.push_back("ERROR: Agent not initialized");
        return;
    }
    
    try {
        pImpl->agent->processSensoryInput(percept);
        pImpl->percept_history.push_back(percept);
        pImpl->log_messages.push_back("AgentContextManager: Processed sensor data");
    } catch (const std::exception& e) {
        pImpl->log_messages.push_back(std::string("ERROR: Sensor processing failed: ") + e.what());
    }
}

Action AgentContextManager::decode_motor_command() {
    if (!pImpl->agent || !pImpl->initialized) {
        pImpl->log_messages.push_back("ERROR: Agent not initialized");
        return Action(ActionType::Wait);
    }
    
    try {
        Action action = pImpl->agent->decodeMotorCommand();
        pImpl->log_messages.push_back("AgentContextManager: Decoded motor command");
        return action;
    } catch (const std::exception& e) {
        pImpl->log_messages.push_back(std::string("ERROR: Motor command decoding failed: ") + e.what());
        return Action(ActionType::Wait);
    }
}

void AgentContextManager::apply_reward(float reward, float predicted) {
    if (!pImpl->agent || !pImpl->initialized) {
        pImpl->log_messages.push_back("ERROR: Agent not initialized");
        return;
    }
    
    try {
        pImpl->agent->applyRewardModulation(reward, predicted);
        pImpl->log_messages.push_back("AgentContextManager: Applied reward modulation");
    } catch (const std::exception& e) {
        pImpl->log_messages.push_back(std::string("ERROR: Reward modulation failed: ") + e.what());
    }
}

void AgentContextManager::update_development(double timestep) {
    if (!pImpl->agent || !pImpl->initialized) {
        pImpl->log_messages.push_back("ERROR: Agent not initialized");
        return;
    }
    
    try {
        pImpl->agent->updateDevelopment(timestep);
        pImpl->log_messages.push_back("AgentContextManager: Updated development");
    } catch (const std::exception& e) {
        pImpl->log_messages.push_back(std::string("ERROR: Development update failed: ") + e.what());
    }
}

void AgentContextManager::process_sensor_batch(const std::vector<SensoryPercept>& percepts) {
    if (!pImpl->agent || !pImpl->initialized) {
        pImpl->log_messages.push_back("ERROR: Agent not initialized");
        return;
    }
    
    for (const auto& percept : percepts) {
        try {
            pImpl->agent->processSensoryInput(percept);
            pImpl->percept_history.push_back(percept);
        } catch (const std::exception& e) {
            pImpl->log_messages.push_back(std::string("ERROR: Batch sensor processing failed: ") + e.what());
        }
    }
    
    pImpl->log_messages.push_back("AgentContextManager: Processed " + 
                                 Utils::format_number(percepts.size()) + " sensor inputs");
}

std::vector<Action> AgentContextManager::decode_motor_batch(size_t count) {
    std::vector<Action> actions;
    
    if (!pImpl->agent || !pImpl->initialized) {
        pImpl->log_messages.push_back("ERROR: Agent not initialized");
        return actions;
    }
    
    for (size_t i = 0; i < count; ++i) {
        try {
            Action action = pImpl->agent->decodeMotorCommand();
            actions.push_back(action);
        } catch (const std::exception& e) {
            pImpl->log_messages.push_back(std::string("ERROR: Batch motor decoding failed: ") + e.what());
            actions.push_back(Action(ActionType::Wait));
        }
    }
    
    pImpl->log_messages.push_back("AgentContextManager: Decoded " + 
                                 Utils::format_number(count) + " motor commands");
    return actions;
}

} // namespace nlm