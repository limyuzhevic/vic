#pragma once

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "AdvancedTypes.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <algorithm>

namespace nlm {

// Advanced Simulation Controller
class Simulation {
public:
    Simulation(std::shared_ptr<Brain> brain) : brain_(brain) {
        // Initialize with standard settings
        mode_ = SimulationMode::Standard;
        maxStepsPerEpisode_ = 1000;
        autoSaveInterval_ = 100;
        performanceMonitoringEnabled_ = false;
        currentEpisode_ = 0;
        currentStep_ = 0;
        isRunning_ = false;
        isPaused_ = false;
    }
    
    ~Simulation() {
        // Cleanup resources
        clearHistory();
    }
    
    // Run the simulation
    bool run() {
        if (!brain_) return false;
        
        isRunning_ = true;
        currentStep_ = 0;
        currentEpisode_++;
        
        // Create new episode
        EpisodeData episode;
        episode.episodeId = currentEpisode_;
        
        // Record start step
        if (brain_->getDevelopmentalStage() == DevelopmentalStage::Initial) {
            brain_->setStartStep(currentStep_);
        }
        
        while (isRunning_ && !isPaused_ && 
               (maxStepsPerEpisode_ == 0 || currentStep_ < maxStepsPerEpisode_)) {
            
            // Get brain status for monitoring
            if (performanceMonitoringEnabled_) {
                brain_->logStatus();
            }
            
            // Perform simulation step
            brain_->step(currentStep_);
            
            // Record step data
            EpisodeData::StepData stepData;
            stepData.step = currentStep_;
            stepData.neuronCount = brain_->getTotalNeuronCount();
            stepData.synapseCount = brain_->getTotalSynapseCount();
            stepData.firingCount = brain_->getFiringNeuronCount();
            stepData.avgFiringRate = brain_->getAverageFiringRate();
            episode.stepData_.push_back(stepData);
            
            // Auto-save if interval reached
            if (autoSaveInterval_ > 0 && currentStep_ % autoSaveInterval_ == 0) {
                save(std::string("checkpoint_step_") + std::to_string(currentStep_) + ".bin");
            }
            
            currentStep_++;
            
            // Update performance metrics
            if (performanceMonitoringEnabled_) {
                updatePerformanceMetrics(currentStep_);
            }
        }
        
        isRunning_ = false;
        brain_->setEndStep(currentStep_ - 1);
        episodeHistory_.push_back(episode);
        
        return true;
    }
    
    // Run a single step
    bool step(size_t stepNumber) {
        if (!brain_ || !isRunning_) return false;
        
        currentStep_ = stepNumber;
        brain_->step(currentStep_);
        
        // Update history
        if (currentStep_ < episodeHistory_.size() && 
            currentStep_ < episodeHistory_.back().stepData_.size()) {
            updatePerformanceMetrics(currentStep_);
        }
        
        return true;
    }
    
    // Pause simulation
    void pause() {
        isPaused_ = true;
    }
    
    // Resume simulation
    void resume() {
        isPaused_ = false;
    }
    
    // Step through manually (debug mode)
    bool stepThrough() {
        if (isRunning_) pause();
        
        // Perform one manual step
        if (brain_ && currentStep_ < maxStepsPerEpisode_) {
            brain_->step(currentStep_);
            currentStep_++;
            return true;
        }
        
        return false;
    }
    
    // Set simulation mode
    void setMode(SimulationMode mode) {
        mode_ = mode;
        switch (mode_) {
            case SimulationMode::Episode:
                maxStepsPerEpisode_ = 1000;
                break;
            case SimulationMode::ReinforcementLearning:
                maxStepsPerEpisode_ = 500;
                break;
            case SimulationMode::Benchmark:
                maxStepsPerEpisode_ = 10000;
                break;
            default:
                maxStepsPerEpisode_ = 0; // Run forever
                break;
        }
    }
    
    // Get current simulation mode
    SimulationMode getMode() const {
        return mode_;
    }
    
    // Enable/disable performance monitoring
    void enablePerformanceMonitoring(bool enable) {
        performanceMonitoringEnabled_ = enable;
        if (enable) {
            performanceMetrics_.avgStepTimeMs = 0.0;
            performanceMetrics_.stepCount = 0;
        }
    }
    
    // Set max steps per episode
    void setMaxStepsPerEpisode(size_t maxSteps) {
        maxStepsPerEpisode_ = maxSteps;
    }
    
    // Set auto-save interval
    void setAutosaveInterval(size_t interval) {
        autoSaveInterval_ = interval;
    }
    
    // Get performance metrics
    PerformanceMetrics getPerformanceMetrics() const {
        return performanceMetrics_;
    }
    
    // Get simulation history
    const std::vector<EpisodeData>& getHistory() const {
        return episodeHistory_;
    }
    
    // Clear history
    void clearHistory() {
        episodeHistory_.clear();
        performanceMetrics_.clear();
    }
    
    // Save simulation state
    bool save(const std::string& filepath) {
        if (!brain_) return false;
        return brain_->save(filepath);
    }
    
    // Load simulation state
    bool load(const std::string& filepath) {
        if (!brain_) return false;
        bool loaded = brain_->load(filepath);
        if (loaded) {
            // Reset episode data
            currentStep_ = 0;
            clearHistory();
        }
        return loaded;
    }
    
    // Export to JSON
    bool exportToJson(const std::string& filepath) {
        // Implementation for JSON export
        return false;
    }
    
    // Import from JSON
    bool importFromJson(const std::string& filepath) {
        // Implementation for JSON import
        return false;
    }
    
    // Replay a specific episode
    bool replayEpisode(size_t episodeIndex) {
        if (episodeIndex >= episodeHistory_.size()) return false;
        
        const EpisodeData& episode = episodeHistory_[episodeIndex];
        
        // Reset brain
        brain_->reset();
        
        // Replay steps
        currentStep_ = 0;
        for (const auto& stepData : episode.stepData_) {
            // Reconstruct world state if available
            brain_->step(stepData.step);
            currentStep_++;
        }
        
        return true;
    }
    
    // Update performance metrics
    void updatePerformanceMetrics(size_t stepNumber) {
        // Simple performance tracking
        performanceMetrics_.stepCount = stepNumber;
        // In real implementation, track timing, memory, etc.
    }
    
    // Check if simulation is running
    bool isRunning() const {
        return isRunning_ && !isPaused_;
    }
    
    // Check if simulation is paused
    bool isPaused() const {
        return isPaused_;
    }
    
    // Get current step
    size_t getCurrentStep() const {
        return currentStep_;
    }
    
    // Get current episode
    size_t getCurrentEpisode() const {
        return currentEpisode_;
    }
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() {
        return brain_;
    }
    
    // Get world reference if available
    std::shared_ptr<SimpleWorld> getWorld() {
        return world_;
    }
    
    // Set world reference
    void setWorld(std::shared_ptr<SimpleWorld> world) {
        world_ = world;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    
    SimulationMode mode_;
    size_t maxStepsPerEpisode_;
    size_t autoSaveInterval_;
    bool performanceMonitoringEnabled_;
    
    size_t currentEpisode_;
    size_t currentStep_;
    bool isRunning_;
    bool isPaused_;
    
    std::vector<EpisodeData> episodeHistory_;
    PerformanceMetrics performanceMetrics_;
};

} // namespace nlm
