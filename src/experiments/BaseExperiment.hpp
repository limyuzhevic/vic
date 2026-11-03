// Experiment Framework Base Class
#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include <memory>
#include <string>
#include <functional>
#include <chrono>

namespace nlm {

class BaseExperiment {
public:
    BaseExperiment(const std::string& name, const std::string& description);
    virtual ~BaseExperiment();
    
    // Experiment metadata
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    
    // Experiment configuration
    void setConfig(const std::shared_ptr<Config>& config) { config_ = config; }
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Execution control
    virtual bool initialize() { return true; }
    virtual bool run() = 0;
    virtual bool cleanup() { return true; }
    
    // Configuration convenience methods
    template<typename T>
    void setConfigValue(const std::string& key, const T& value) {
        if (config_) {
            config_->set(key, value);
        }
    }
    
    template<typename T>
    T getConfigValue(const std::string& key, const T& defaultValue) const {
        if (config_) {
            auto value = config_->getOr<T>(key, defaultValue);
            return value ? *value : defaultValue;
        }
        return defaultValue;
    }
    
    // Execution time tracking
    void startTiming() { start_time_ = std::chrono::high_resolution_clock::now(); }
    double getElapsedTime() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start_time_).count();
    }
    
    // Progress reporting
    void setProgressCallback(std::function<void(double)> callback) {
        progress_callback_ = callback;
    }
    
protected:
    // Helper methods for common experiment patterns
    std::shared_ptr<Brain> createBrain() {
        auto brain = std::make_shared<Brain>(config_);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }
    
    void configureWorld(SimpleWorld& world, int width = 16, int height = 16, 
                       int depth = 16, int time_steps = 16) {
        world.configure(width, height, depth, time_steps);
        world.reset();
    }
    
    void configureAgent(AgentBrain& agent, const SimpleWorld& world) {
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
    }
    
    // Standard experiment step pattern
    template<typename WorldType, typename AgentType>
    void runExperimentSteps(WorldType& world, AgentType& agent, 
                           std::shared_ptr<Brain> brain,
                           SimulationStep maxSteps,
                           std::function<void(SimulationStep)> stepCallback = nullptr) {
        for (SimulationStep step = 0; step < maxSteps; ++step) {
            // Update world
            world.update(0.001);
            
            // Get sensory percept
            SensoryPercept percept = world.getSensoryPercept();
            
            // Process input
            agent.processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply action
            ActionResult actionResult = world.applyMotorCommand(cmd, step * 0.001);
            
            // Apply neuromodulation
            agent.applyRewardModulation(actionResult.reward, 0.0f);
            
            // Update development
            if (agent.isDevelopmentEnabled()) {
                agent.updateDevelopment(0.001);
            }
            
            // Progress callback
            if (progress_callback_) {
                progress_callback_(static_cast<double>(step) / maxSteps);
            }
            
            if (stepCallback) {
                stepCallback(step);
            }
        }
    }
    
    // Metrics collection helper
    struct ExperimentMetrics {
        double totalReward = 0.0;
        double avgFiringRate = 0.0;
        double noveltyLevel = 0.0;
        double curiosityLevel = 0.0;
        double dopamineLevel = 0.0;
        size_t memoryEpisodesStored = 0;
        double totalWallTime = 0.0;
    };
    
    ExperimentMetrics collectMetrics(AgentBrain& agent, std::shared_ptr<Brain> brain) {
        ExperimentMetrics metrics;
        metrics.totalReward = agent.getTotalReward();
        metrics.avgFiringRate = brain->getAverageFiringRate();
        metrics.noveltyLevel = agent.getNoveltyLevel();
        metrics.curiosityLevel = agent.getCuriosityLevel();
        metrics.dopamineLevel = agent.getNeuromodulationLevel();
        metrics.memoryEpisodesStored = brain->getEpisodicMemory() ? 
            brain->getEpisodicMemory()->getEpisodeCount() : 0;
        metrics.totalWallTime = getElapsedTime();
        return metrics;
    }
    
private:
    std::string name_;
    std::string description_;
    std::shared_ptr<Config> config_;
    std::function<void(double)> progress_callback_;
    std::chrono::high_resolution_clock::time_point start_time_;
};

} // namespace nlm
