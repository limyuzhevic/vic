#include "SimulationContext.hpp"
#include <iostream>
#include <chrono>

namespace nlm {

struct SimulationContext::Impl {
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<Brain> brain_;
    bool active_;
    size_t stepsRun_;
    double startTime_;
    std::chrono::high_resolution_clock::time_point startClock_;
    
    Impl(std::shared_ptr<SimpleWorld> world, 
         std::shared_ptr<AgentBrain> agent,
         std::shared_ptr<Brain> brain)
        : world_(std::move(world))
        , agent_(std::move(agent))
        , brain_(std::move(brain))
        , active_(false)
        , stepsRun_(0)
        , startTime_(0.0)
    {}
};

SimulationContext::SimulationContext(std::shared_ptr<SimpleWorld> world, 
                                     std::shared_ptr<AgentBrain> agent,
                                     std::shared_ptr<Brain> brain)
    : pImpl(std::make_unique<Impl>(std::move(world), std::move(agent), std::move(brain))) {
}

SimulationContext::~SimulationContext() {
    if (pImpl->active_) {
        exit();
    }
}

void SimulationContext::enter() {
    if (pImpl->active_) {
        throw std::runtime_error("Simulation context already active");
    }
    
    pImpl->active_ = true;
    pImpl->stepsRun_ = 0;
    pImpl->startClock_ = std::chrono::high_resolution_clock::now();
    pImpl->startTime_ = pImpl->world_->getSimulationTime();
    
    // Reset all components for fresh start
    pImpl->world_->reset();
    pImpl->agent_->reset();
    pImpl->brain_->reset();
    
    std::cout << "[SimulationContext] Started simulation" << std::endl;
}

void SimulationContext::exit() {
    if (!pImpl->active_) {
        return;
    }
    
    auto endClock = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endClock - pImpl->startClock_);
    
    pImpl->active_ = false;
    
    std::cout << "[SimulationContext] Stopped simulation (ran " << pImpl->stepsRun_ 
              << " steps in " << duration.count() << " ms)" << std::endl;
}

void SimulationContext::run(size_t steps) {
    if (!pImpl->active_) {
        throw std::runtime_error("Simulation context not active - call enter() first");
    }
    
    std::cout << "[SimulationContext] Running " << steps << " steps..." << std::endl;
    
    for (size_t i = 0; i < steps; ++i) {
        step(i);
    }
    
    std::cout << "[SimulationContext] Completed " << steps << " steps" << std::endl;
}

std::string SimulationContext::getStatus() const {
    std::ostringstream oss;
    oss << "SimulationContext Status:\n";
    oss << "  Active: " << (pImpl->active_ ? "Yes" : "No") << "\n";
    oss << "  Steps run: " << pImpl->stepsRun_ << "\n";
    oss << "  World time: " << pImpl->world_->getSimulationTime() << "\n";
    oss << "  World dimensions: " << pImpl->world_->getWidth() << " x " 
        << pImpl->world_->getHeight() << "\n";
    if (pImpl->brain_) {
        oss << "  Neurons: " << pImpl->brain_->getTotalNeuronCount() << "\n";
        oss << "  Firing: " << pImpl->brain_->getFiringNeuronCount() << "/" 
            << pImpl->brain_->getTotalNeuronCount() << "\n";
        oss << "  Total spikes: " << pImpl->brain_->getTotalSpikeCount() << "\n";
    }
    return oss.str();
}

void SimulationContext::step(size_t stepNumber) {
    if (!pImpl->active_) {
        throw std::runtime_error("Cannot step - simulation context not active");
    }
    
    // Update world
    pImpl->world_->update(0.1);
    
    // Get sensory input
    auto percept = pImpl->world_->getSensoryPercept();
    
    // Process in agent
    pImpl->agent_->processSensoryInput(percept);
    
    // Brain processes
    pImpl->brain_->step(static_cast<SimulationStep>(stepNumber));
    
    // Get motor command and apply to world
    auto action = pImpl->agent_->decodeMotorCommand();
    pImpl->world_->applyMotorCommand(action, pImpl->world_->getSimulationTime());
    
    pImpl->stepsRun_++;
    
    if (stepNumber % 100 == 0) {
        std::cout << "[SimulationContext] Step " << stepNumber << "/"
                  << (pImpl->stepsRun_) << " completed" << std::endl;
    }
}

} // namespace nlm
