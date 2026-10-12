#include "NLMImprovedAPI.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>

namespace nlm {

struct BrainContextManager::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    bool initialized;
    std::vector<std::string> log_messages;
    double start_time;
    
    Impl() : config(nullptr), brain(nullptr), initialized(false), start_time(0.0) {}
};

BrainContextManager::BrainContextManager() : pImpl(std::make_unique<Impl>()) {}

BrainContextManager::BrainContextManager(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
}

BrainContextManager::~BrainContextManager() {
    if (pImpl->brain) {
        pImpl->brain->reset();
    }
}

BrainContextManager::BrainContextManager(BrainContextManager&& other) noexcept : pImpl(std::move(other.pImpl)) {}

BrainContextManager& BrainContextManager::operator=(BrainContextManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool BrainContextManager::enter() {
    if (!pImpl->config) {
        pImpl->config = std::make_shared<Config>();
    }
    
    pImpl->brain = std::make_shared<Brain>(pImpl->config);
    
    if (!pImpl->brain->initialize()) {
        pImpl->log_messages.push_back("ERROR: Brain initialization failed");
        return false;
    }
    
    pImpl->initialized = true;
    pImpl->start_time = Utils::current_time_seconds();
    pImpl->log_messages.push_back("BrainContextManager: Brain initialized successfully");
    
    return true;
}

void BrainContextManager::exit(int exception_code) {
    if (exception_code != 0 && pImpl->brain) {
        pImpl->brain->reset();
        pImpl->log_messages.push_back("BrainContextManager: Brain reset due to exception");
    }
    
    pImpl->initialized = false;
    pImpl->brain.reset();
}

std::shared_ptr<Brain> BrainContextManager::brain() const {
    return pImpl->brain;
}

std::shared_ptr<Config> BrainContextManager::config() const {
    return pImpl->config;
}

void BrainContextManager::initialize() {
    if (!pImpl->brain) {
        pImpl->log_messages.push_back("ERROR: No brain to initialize");
        return;
    }
    
    if (pImpl->initialized) {
        pImpl->brain->reset();
    }
    
    if (!pImpl->brain->initialize()) {
        pImpl->log_messages.push_back("ERROR: Brain initialization failed");
        return;
    }
    
    pImpl->initialized = true;
    pImpl->start_time = Utils::current_time_seconds();
    pImpl->log_messages.push_back("BrainContextManager: Brain initialized");
}

void BrainContextManager::step(size_t step_count) {
    if (!pImpl->brain || !pImpl->initialized) {
        pImpl->log_messages.push_back("ERROR: Brain not initialized");
        return;
    }
    
    for (size_t i = 0; i < step_count; ++i) {
        pImpl->brain->step(static_cast<SimulationStep>(i));
    }
    
    pImpl->log_messages.push_back("BrainContextManager: Completed " + 
                                 Utils::format_number(step_count) + " steps");
}

void BrainContextManager::reset() {
    if (pImpl->brain) {
        pImpl->brain->reset();
        pImpl->initialized = false;
        pImpl->log_messages.push_back("BrainContextManager: Brain reset");
    }
}

void BrainContextManager::save(const std::string& filepath) const {
    if (!pImpl->brain) {
        pImpl->log_messages.push_back("ERROR: No brain to save");
        return;
    }
    
    if (pImpl->brain->save(filepath)) {
        pImpl->log_messages.push_back("BrainContextManager: Brain saved to " + filepath);
    } else {
        pImpl->log_messages.push_back("ERROR: Failed to save brain to " + filepath);
    }
}

void BrainContextManager::load(const std::string& filepath) {
    if (!pImpl->brain) {
        pImpl->log_messages.push_back("ERROR: No brain to load into");
        return;
    }
    
    if (pImpl->brain->load(filepath)) {
        pImpl->initialized = true;
        pImpl->log_messages.push_back("BrainContextManager: Brain loaded from " + filepath);
    } else {
        pImpl->log_messages.push_back("ERROR: Failed to load brain from " + filepath);
    }
}

void BrainContextManager::run_simulation(size_t steps, double timestep) {
    if (!pImpl->brain || !pImpl->initialized) {
        pImpl->log_messages.push_back("ERROR: Brain not initialized");
        return;
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < steps; ++i) {
        pImpl->brain->step(static_cast<SimulationStep>(i), 
                         static_cast<Timestamp>(i * timestep));
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    pImpl->log_messages.push_back("BrainContextManager: Simulation completed " + 
                                 Utils::format_number(steps) + " steps in " + 
                                 Utils::format_duration(duration.count()) + 
                                 " (avg: " + Utils::format_number(static_cast<size_t>(steps / duration.count())) + 
                                 " steps/sec)");
}

void BrainContextManager::run_episode(std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent,
                                      size_t steps, double timestep) {
    if (!pImpl->brain || !agent || !world) {
        pImpl->log_messages.push_back("ERROR: Missing brain, agent, or world");
        return;
    }
    
    world->reset();
    
    for (size_t i = 0; i < steps; ++i) {
        world->update(timestep);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        pImpl->brain->step(static_cast<SimulationStep>(i), 
                         static_cast<Timestamp>(i * timestep));
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
    
    pImpl->log_messages.push_back("BrainContextManager: Episode completed " + 
                                 Utils::format_number(steps) + " steps with agent");
}

} // namespace nlm