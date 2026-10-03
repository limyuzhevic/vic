#include "SimpleEnvironment.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

SimpleEnvironment::SimpleEnvironment()
    : config_(std::make_shared<Config>())
    , brain_(nullptr)
    , world_(nullptr)
    , agent_(nullptr)
    , episodeStepsCompleted_(0)
    , episodeTotalReward_(0.0f)
    , episodeAvgFiringRate_(0.0f)
    , initialized_(false) {
}

SimpleEnvironment::~SimpleEnvironment() = default;

void SimpleEnvironment::initialize(float width, float height,
                                   float visionWidth, float visionHeight) {
    // Initialize world first
    world_ = std::make_shared<SimpleWorld>();
    world_->configure(width, height, visionWidth, visionHeight);
    world_->reset();
    world_->setAgentStart(width / 2.0f, height / 2.0f);
    
    // Initialize brain with configuration
    config_ = std::make_shared<Config>();
    configureBrain(); // Use default configuration
    
    brain_ = std::make_shared<Brain>(config_);
    if (!brain_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        throw std::runtime_error("Brain initialization failed");
    }
    
    // Create agent brain interface
    agent_ = std::make_shared<AgentBrain>(brain_);
    agent_->initialize(*world_);
    
    // Enable all common subsystems
    enableAllSubsystems();
    
    initialized_ = true;
    reset();
    
    NLM_LOG_INFO("SimpleEnvironment initialized successfully");
}

void SimpleEnvironment::configureBrain(size_t neuronCount, size_t regionCount,
                                      float connectionProbability) {
    config_->set("neuron_count", neuronCount);
    config_->set("region_count", regionCount);
    config_->set("connection_probability", connectionProbability);
    
    // Set reasonable defaults for a balanced simulation
    config_->set("stdp_ltp_weight", 0.01f);
    config_->set("stdp_ltd_weight", 0.012f);
    config_->set("stdp_tau", 20.0f);
    config_->set("simulation_timestep", 0.001);
    config_->set("replay_interval", 100);
    config_->set("consolidation_interval", 1000);
    
    // Reset brain if already created
    if (brain_) {
        brain_.reset();
        brain_ = std::make_shared<Brain>(config_);
        if (brain_->initialize()) {
            agent_->reset();
            agent_->initialize(*world_);
        }
    }
}

void SimpleEnvironment::enableAllSubsystems() {
    if (!agent_) return;
    
    // Enable commonly used subsystems
    agent_->enableRewardModulation(true);
    agent_->enableStructuralPlasticity(true);
    agent_->enableDevelopment(true);
    agent_->enableCuriosity(true);
    
    // Set curiosity to a reasonable starting level
    agent_->getCuriosityLevel();
}

void SimpleEnvironment::runEpisode(size_t numSteps, bool render) {
    if (!isInitialized()) {
        throw std::runtime_error("Environment not initialized. Call initialize() first.");
    }
    
    NLM_LOG_INFO(std::to_string("Starting episode with " + std::to_string(numSteps) + " steps"));
    
    // Reset episode statistics
    episodeStepsCompleted_ = 0;
    episodeTotalReward_ = 0.0f;
    episodeAvgFiringRate_ = 0.0f;
    
    for (size_t step = 0; step < numSteps; ++step) {
        // Step the world
        world_->update(0.1);
        
        // Get sensory percept from world
        const SensoryPercept& percept = world_->getSensoryPercept();
        
        // Process sensory input in agent brain
        agent_->processSensoryInput(percept);
        
        // Run brain simulation step
        brain_->step(step, step * 0.1);
        
        // Get motor command from agent brain
        const Action& action = agent_->decodeMotorCommand();
        
        // Apply motor command to world
        world_->applyMotorCommand(action, world_->getSimulationTime());
        
        // Update development
        agent_->updateDevelopment(0.1);
        
        // Collect reward if any (simplified reward computation)
        float reward = 0.0f;
        const auto& percept = world_->getSensoryPercept();
        if (!percept.getInternal().empty()) {
            reward = percept.getInternal()[0];
        }
        agent_->applyRewardModulation(reward, 0.0f);
        
        episodeTotalReward_ += reward;
        
        // Update statistics
        episodeAvgFiringRate_ = (episodeAvgFiringRate_ * episodeStepsCompleted_ + 
                                brain_->getAverageFiringRate()) / (episodeStepsCompleted_ + 1);
        episodeStepsCompleted_++;
        
        if (render && step % 100 == 0) {
            NLM_LOG_INFO(std::to_string("Step " + std::to_string(step) +
                            " | Reward: " + std::to_string(reward) +
                            " | Firing: " + std::to_string(brain_->getAverageFiringRate()) +
                            " | Curiosity: " + std::to_string(agent_->getCuriosityLevel())));
        }
    }
    
    NLM_LOG_INFO("Episode completed: " + std::to_string(episodeStepsCompleted_) + " steps, " +
                 std::to_string(episodeTotalReward_) + " total reward, " +
                 std::to_string(episodeAvgFiringRate_) + " avg firing rate");
}

void SimpleEnvironment::reset() {
    if (world_) {
        world_->reset();
    }
    if (agent_) {
        agent_->reset();
    }
    if (brain_) {
        brain_->reset();
    }
    
    // Reset episode statistics
    episodeStepsCompleted_ = 0;
    episodeTotalReward_ = 0.0f;
    episodeAvgFiringRate_ = 0.0f;
    
    NLM_LOG_INFO("Environment reset completed");
}

SimpleEnvironment::EpisodeStats SimpleEnvironment::getEpisodeStats() const {
    EpisodeStats stats;
    stats.stepsCompleted = episodeStepsCompleted_;
    stats.totalReward = episodeTotalReward_;
    stats.avgFiringRate = episodeAvgFiringRate_;
    stats.activeNeurons = (brain_ ? brain_->getFiringNeuronCount() : 0);
    stats.curiosityLevel = (agent_ ? agent_->getCuriosityLevel() : 0.0f);
    stats.noveltyLevel = (agent_ ? agent_->getNoveltyLevel() : 0.0f);
    stats.developmentStage = (brain_ ? brain_->getDevelopmentalStage() : DevelopmentalStage::Initial);
    return stats;
}

bool SimpleEnvironment::step(double timestep) {
    if (!isInitialized()) {
        return false;
    }
    
    // Step the world
    world_->update(static_cast<float>(timestep));
    
    // Get sensory percept
    const SensoryPercept& percept = world_->getSensoryPercept();
    
    // Process sensory input
    agent_->processSensoryInput(percept);
    
    // Run brain step
    brain_->step(episodeStepsCompleted_, episodeStepsCompleted_ * timestep);
    
    // Get motor command
    const Action& action = agent_->decodeMotorCommand();
    
    // Apply action
    world_->applyMotorCommand(action, world_->getSimulationTime());
    
    // Update development
    agent_->updateDevelopment(static_cast<float>(timestep));
    
    // Simple reward computation
    float reward = 0.0f;
    if (!percept.getInternal().empty()) {
        reward = percept.getInternal()[0];
    }
    agent_->applyRewardModulation(reward, 0.0f);
    
    // Update statistics
    episodeTotalReward_ += reward;
    episodeAvgFiringRate_ = (episodeAvgFiringRate_ * episodeStepsCompleted_ + 
                            brain_->getAverageFiringRate()) / (episodeStepsCompleted_ + 1);
    episodeStepsCompleted_++;
    
    return true;
}

std::string SimpleEnvironment::getStateDescription() const {
    std::stringstream ss;
    ss << "SimpleEnvironment State:" << std::endl;
    ss << "  Initialized: " << (initialized_ ? "Yes" : "No") << std::endl;
    if (brain_) {
        ss << "  Brain: " << brain_->getTotalNeuronCount() << " neurons, "
           << brain_->getTotalSynapseCount() << " synapses" << std::endl;
        ss << "  Development Stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
        ss << "  Firing Rate: " << brain_->getAverageFiringRate() << std::endl;
    }
    if (world_) {
        ss << "  World: " << world_->getWidth() << "x" << world_->getHeight() << std::endl;
        ss << "  Simulation Time: " << world_->getSimulationTime() << std::endl;
    }
    if (agent_) {
        ss << "  Agent: Curiosity=" << agent_->getCuriosityLevel() << ", Novelty=" << agent_->getNoveltyLevel() << std::endl;
    }
    ss << "  Episode Steps: " << episodeStepsCompleted_ << std::endl;
    ss << "  Episode Reward: " << episodeTotalReward_ << std::endl;
    
    return ss.str();
}

} // namespace nlm