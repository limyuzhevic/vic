// SimpleAgent class implementation
// This file provides the implementation of the SimpleAgent high-level interface

#include "../src/simple_agent.hpp"
#include "../src/advanced_api.hpp"

// SimpleAgent implementation
SimpleAgent::SimpleAgent(std::shared_ptr<Brain> brain, 
                        std::shared_ptr<SimpleWorld> world,
                        const std::string& agentId)
    : brain_(brain), world_(world), agentId_(agentId), isInitialized_(false) {
}

bool SimpleAgent::initialize() {
    if (!brain_ || !world_) {
        NLM_LOG_ERROR("Cannot initialize agent: brain or world is null");
        return false;
    }
    
    // Initialize brain
    brain_->initialize();
    
    // Setup world for agent
    if (world_) {
        world_->setupForAgent(agentId_);
    }
    
    isInitialized_ = true;
    NLM_LOG_INFO("Agent " + agentId_ + " initialized successfully");
    return true;
}

void SimpleAgent::update(float dt) {
    if (!isInitialized_) {
        NLM_LOG_WARNING("Agent not initialized, skipping update");
        return;
    }
    
    // Process sensory input
    if (world_) {
        auto sensoryInput = world_->getSensoryInput();
        processInput(sensoryInput);
    }
    
    // Step brain
    if (brain_) {
        brain_->step(dt);
    }
    
    // Update world
    if (world_) {
        world_->update(dt);
    }
}

void SimpleAgent::reset() {
    if (brain_) {
        brain_->reset();
    }
    if (world_) {
        world_->reset();
    }
    isInitialized_ = false;
}

void SimpleAgent::processInput(const FloatArray& input) {
    // Store input in brain's working memory
    if (brain_) {
        brain_->getWorkingMemory()->store(input, 1.0f);
    }
}

FloatArray SimpleAgent::getOutput() const {
    FloatArray output;
    if (brain_) {
        // Generate output based on brain state
        output = brain_->getOutputVector();
    }
    return output;
}

double SimpleAgent::getReward() const {
    double reward = 0.0;
    if (brain_) {
        auto dopamine = brain_->getDopamine();
        reward = dopamine->getLevel();
    }
    return reward;
}

void SimpleAgent::receiveReward(double reward) {
    if (brain_) {
        auto dopamine = brain_->getDopamine();
        dopamine->signalReward(reward);
    }
}

bool SimpleAgent::isActive() const {
    return isInitialized_ && brain_ && brain_->isActive();
}

std::string SimpleAgent::getAgentId() const {
    return agentId_;
}

std::shared_ptr<Brain> SimpleAgent::getBrain() const {
    return brain_;
}

std::shared_ptr<SimpleWorld> SimpleAgent::getWorld() const {
    return world_;
}

bool SimpleAgent::configure(const StringMap& config) {
    // Apply configuration to brain and world
    if (brain_) {
        brain_->configure(config);
    }
    if (world_) {
        world_->configure(config);
    }
    return true;
}

StringMap SimpleAgent::getConfig() const {
    StringMap config;
    if (brain_) {
        config["brain"] = "configured";
    }
    if (world_) {
        config["world"] = "configured";
    }
    config["agent_id"] = agentId_;
    config["initialized"] = isInitialized_ ? "true" : "false";
    return config;
}

double SimpleAgent::getFiringRate() const {
    double firingRate = 0.0;
    if (brain_) {
        firingRate = brain_->getAverageFiringRate();
    }
    return firingRate;
}

size_t SimpleAgent::getSpikeCount() const {
    size_t spikeCount = 0;
    if (brain_) {
        spikeCount = brain_->getTotalSpikeCount();
    }
    return spikeCount;
}

FloatArray SimpleAgent::getNeuralActivity() const {
    FloatArray activity;
    if (brain_) {
        activity = brain_->getNeuralActivityVector();
    }
    return activity;
}

void SimpleAgent::learnFromExperience(const FloatArray& experience) {
    if (brain_) {
        // Learn from experience using brain's learning systems
        brain_->learnFromExperience(experience);
    }
}

void SimpleAgent::develop(float dt) {
    if (brain_) {
        // Development processes are handled internally
        brain_->updateDevelopment(dt);
    }
}

void SimpleAgent::saveState(const std::string& filepath) const {
    // Save agent state to file
    // Implementation would use serialization
}

void SimpleAgent::loadState(const std::string& filepath) {
    // Load agent state from file
    // Implementation would use deserialization
}

std::string SimpleAgent::getDebugInfo() const {
    std::string info = "Agent Debug Info:\n";
    info += "  ID: " + agentId_ + "\n";
    info += "  Initialized: " + (isInitialized_ ? "Yes" : "No") + "\n";
    if (brain_) {
        info += "  Brain State: " + brain_->getBrainStateString() + "\n";
    }
    if (world_) {
        info += "  World State: " + world_->getWorldStateString() + "\n";
    }
    return info;
}

void SimpleAgent::profile(float duration) {
    // Profile agent performance
    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::duration<float>(duration);
    
    while (std::chrono::steady_clock::now() < end) {
        if (isInitialized_) {
            update(1.0f);
        }
    }
}
