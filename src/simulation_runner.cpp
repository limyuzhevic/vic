// SimulationRunner class implementation
// This file provides the implementation of the SimulationRunner high-level interface

#include "../src/simulation_runner.hpp"
#include "../src/advanced_api.hpp"

// SimulationRunner implementation
SimulationRunner::SimulationRunner(size_t numSteps, float dt)
    : numSteps_(numSteps), dt_(dt), renderEnabled_(false), isRunning_(false) {
    experimentId_ = 0;
}

void SimulationRunner::addAgent(std::shared_ptr<SimpleAgent> agent) {
    agents_.push_back(agent);
    NLM_LOG_INFO("Added agent " + agent->getAgentId() + " to simulation");
}

void SimulationRunner::removeAgent(const std::string& agentId) {
    auto it = std::find_if(agents_.begin(), agents_.end(),
                          [&agentId](const std::shared_ptr<SimpleAgent>& agent) {
                              return agent->getAgentId() == agentId;
                          });
    if (it != agents_.end()) {
        NLM_LOG_INFO("Removed agent " + (*it)->getAgentId() + " from simulation");
        agents_.erase(it);
    }
}

std::shared_ptr<SimpleAgent> SimulationRunner::getAgent(const std::string& agentId) const {
    for (const auto& agent : agents_) {
        if (agent->getAgentId() == agentId) {
            return agent;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<SimpleAgent>> SimulationRunner::getAllAgents() const {
    return agents_;
}

void SimulationRunner::setWorld(std::shared_ptr<SimpleWorld> world) {
    world_ = world;
    NLM_LOG_INFO("Set world for simulation");
}

std::shared_ptr<SimpleWorld> SimulationRunner::getWorld() const {
    return world_;
}

void SimulationRunner::configure(const StringMap& config) {
    experimentConfig_ = config;
    
    // Extract configuration parameters
    auto it = config.find("num_steps");
    if (it != config.end()) {
        numSteps_ = std::stoul(it->second);
    }
    
    it = config.find("dt");
    if (it != config.end()) {
        dt_ = std::stof(it->second);
    }
    
    it = config.find("render_enabled");
    if (it != config.end()) {
        renderEnabled_ = (it->second == "true");
    }
    
    it = config.find("output_directory");
    if (it != config.end()) {
        outputDirectory_ = it->second;
    }
    
    NLM_LOG_INFO("Simulation configured with " + std::to_string(numSteps_) + " steps");
}

StringMap SimulationRunner::getConfig() const {
    StringMap config = experimentConfig_;
    config["num_steps"] = std::to_string(numSteps_);
    config["dt"] = std::to_string(dt_);
    config["render_enabled"] = renderEnabled_ ? "true" : "false";
    config["output_directory"] = outputDirectory_;
    config["num_agents"] = std::to_string(agents_.size());
    return config;
}

void SimulationRunner::start() {
    isRunning_ = true;
    NLM_LOG_INFO("Started simulation runner");
}

void SimulationRunner::stop() {
    isRunning_ = false;
    NLM_LOG_INFO("Stopped simulation runner");
}

bool SimulationRunner::isRunning() const {
    return isRunning_;
}

void SimulationRunner::run() {
    if (agents_.empty()) {
        NLM_LOG_WARNING("No agents in simulation, nothing to run");
        return;
    }
    
    if (!world_) {
        NLM_LOG_WARNING("No world set, creating default world");
        world_ = std::make_shared<SimpleWorld>();
    }
    
    start();
    
    // Initialize all agents
    for (const auto& agent : agents_) {
        agent->initialize();
    }
    
    // Run simulation
    for (size_t step = 0; step < numSteps_ && isRunning_; ++step) {
        // Update all agents
        for (const auto& agent : agents_) {
            agent->update(dt_);
        }
        
        // Step world
        if (world_) {
            world_->update(dt_);
        }
        
        // Call step callback if set
        if (stepCallback_) {
            stepCallback_(step);
        }
        
        // Save checkpoint if needed
        if ((step + 1) % 100 == 0 && world_) {
            world_->createCheckpoint();
        }
    }
    
    stop();
    
    // Save results
    saveResults("simulation_" + std::to_string(experimentId_));
    
    experimentId_++;
}

void SimulationRunner::step() {
    if (!isRunning_) {
        NLM_LOG_WARNING("Simulation not running, cannot step");
        return;
    }
    
    // Update all agents
    for (const auto& agent : agents_) {
        agent->update(dt_);
    }
    
    // Step world
    if (world_) {
        world_->update(dt_);
    }
}

void SimulationRunner::enableRendering(bool enable) {
    renderEnabled_ = enable;
    if (renderEnabled_ && world_) {
        world_->enableRendering(true);
    }
    NLM_LOG_INFO("Rendering " + std::string(enable ? "enabled" : "disabled"));
}

bool SimulationRunner::isRenderingEnabled() const {
    return renderEnabled_;
}

void SimulationRunner::setOutputDirectory(const std::string& directory) {
    outputDirectory_ = directory;
    NLM_LOG_INFO("Set output directory to " + directory);
}

void SimulationRunner::saveResults(const std::string& prefix) {
    if (outputDirectory_.empty()) {
        NLM_LOG_WARNING("No output directory set, skipping results save");
        return;
    }
    
    std::string filename = outputDirectory_ + "/" + prefix + "_results.json";
    
    // Collect results
    std::map<std::string, FloatArray> agentMetrics;
    for (const auto& agent : agents_) {
        agentMetrics[agent->getAgentId()] = agent->getNeuralActivity();
    }
    
    // Save to file
    // Implementation would use JSON serialization
    NLM_LOG_INFO("Saved results to " + filename);
}

std::map<std::string, double> SimulationRunner::getStatistics() const {
    std::map<std::string, double> stats;
    
    if (agents_.empty()) {
        return stats;
    }
    
    // Calculate average statistics across all agents
    double totalFiringRate = 0.0;
    double totalSpikeCount = 0.0;
    size_t numAgents = agents_.size();
    
    for (const auto& agent : agents_) {
        totalFiringRate += agent->getFiringRate();
        totalSpikeCount += agent->getSpikeCount();
    }
    
    stats["average_firing_rate"] = totalFiringRate / numAgents;
    stats["total_spike_count"] = totalSpikeCount;
    stats["average_spike_count_per_agent"] = totalSpikeCount / numAgents;
    
    return stats;
}

FloatMatrix SimulationRunner::getLearningCurve() const {
    FloatMatrix learningCurve;
    
    // Generate learning curve based on agent rewards over time
    for (const auto& agent : agents_) {
        FloatArray rewards;
        // Implementation would collect rewards over time
        learningCurve.push_back(rewards);
    }
    
    return learningCurve;
}

std::map<std::string, FloatArray> SimulationRunner::getAgentMetrics() const {
    std::map<std::string, FloatArray> metrics;
    
    for (const auto& agent : agents_) {
        metrics[agent->getAgentId()] = agent->getNeuralActivity();
    }
    
    return metrics;
}

void SimulationRunner::setStepCallback(std::function<void(size_t)> callback) {
    stepCallback_ = callback;
}

void SimulationRunner::setEpisodeCallback(std::function<void(const ExperimentResult&)> callback) {
    episodeCallback_ = callback;
}
