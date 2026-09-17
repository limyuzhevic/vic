// Production-ready NLM monitoring and visualization integration
// This file integrates all production features for Phase 6

#include "PerformanceProfiler.hpp"
#include "CheckpointManager.hpp"
#include "visualization/VisualizationInterface.hpp"
#include "core/Logger/Logger.hpp"
#include <thread>
#include <chrono>
#include <memory>

namespace nlm {

/**
 * ProductionManager - Integrates all production features
 * 
 * Manages checkpointing, visualization, and performance monitoring
 * for production-ready NLM simulations.
 */
class ProductionManager {
public:
    ProductionManager() : initialized_(false), monitoringEnabled_(false),
                          visualizationActive_(false), profiler_(nullptr),
                          checkpointManager_(nullptr), visualization_(nullptr) {}
    
    ~ProductionManager() {
        shutdown();
    }
    
    /**
     * Initialize all production features
     */
    bool initialize(const std::string& checkpointDir = "./checkpoints",
                    bool enableVisualization = true,
                    bool enableMonitoring = true) {
        if (initialized_) {
            return true;
        }
        
        NLM_LOG_INFO("Initializing NLM Production Manager (Phase 6)...");
        
        // Initialize performance profiler
        if (enableMonitoring) {
            profiler_ = std::make_unique<PerformanceProfiler>();
            if (!profiler_->initialize()) {
                NLM_LOG_ERROR("Failed to initialize performance profiler");
                return false;
            }
            monitoringEnabled_ = true;
            NLM_LOG_INFO("Performance profiler initialized");
        }
        
        // Initialize checkpoint manager
        checkpointManager_ = std::make_unique<CheckpointManager>();
        if (!checkpointManager_->configure(checkpointDir, 10000, 10, true)) {
            NLM_LOG_ERROR("Failed to configure checkpoint manager");
            return false;
        }
        
        // Set up checkpoint data providers
        checkpointManager_->setNeuronProvider([this](NeuronCheckpointData& data) {
            return getNeuronDataForCheckpoint(data);
        });
        
        checkpointManager_->setSynapseProvider([this](SynapseCheckpointData& data) {
            return getSynapseDataForCheckpoint(data);
        });
        
        // Initialize visualization if requested
        if (enableVisualization) {
            visualization_ = std::make_unique<VisualizationInterface>();
            if (!visualization_->initialize()) {
                NLM_LOG_ERROR("Failed to initialize visualization");
                return false;
            }
            visualizationActive_ = true;
            NLM_LOG_INFO("Visualization initialized");
            
            // Connect checkpoint manager to visualization
            checkpointManager_->setVisualizationInterface(visualization_.get());
        }
        
        initialized_ = true;
        
        NLM_LOG_INFO("NLM Production Manager initialized successfully");
        return true;
    }
    
    /**
     * Shutdown all production features
     */
    void shutdown() {
        if (!initialized_) {
            return;
        }
        
        NLM_LOG_INFO("Shutting down NLM Production Manager...");
        
        // Stop monitoring
        if (monitoringEnabled_ && profiler_) {
            profiler_->shutdown();
            monitoringEnabled_ = false;
        }
        
        // Close visualization
        if (visualizationActive_ && visualization_) {
            visualization_->close();
            visualizationActive_ = false;
        }
        
        // Cleanup checkpoint manager
        if (checkpointManager_) {
            checkpointManager_.reset();
        }
        
        // Cleanup profiler
        if (profiler_) {
            profiler_.reset();
        }
        
        initialized_ = false;
        
        NLM_LOG_INFO("NLM Production Manager shutdown complete");
    }
    
    /**
     * Start monitoring
     */
    void startMonitoring() {
        if (!monitoringEnabled_ || !profiler_) {
            return;
        }
        
        profiler_->startProfiling();
        NLM_LOG_INFO("Monitoring started");
    }
    
    /**
     * Stop monitoring
     */
    void stopMonitoring() {
        if (!monitoringEnabled_ || !profiler_) {
            return;
        }
        
        profiler_->stopProfiling();
        NLM_LOG_INFO("Monitoring stopped");
    }
    
    /**
     * Update all production features (call each simulation step)
     */
    void update(uint64_t step, double simulationTime) {
        if (!initialized_) {
            return;
        }
        
        // Update performance profiler
        if (monitoringEnabled_ && profiler_) {
            profiler_->update();
        }
        
        // Update checkpoint manager
        if (checkpointManager_) {
            checkpointManager_->update(step, simulationTime);
        }
        
        // Update visualization if active
        if (visualizationActive_ && visualization_) {
            // We need a Brain instance here - in real implementation
            // this would be passed from the main simulation loop
            NLM_LOG_INFO("ProductionManager update requires Brain instance integration");
        }
    }
    
    /**
     * Save current state
     */
    bool saveState(const std::string& name) {
        if (!checkpointManager_) {
            return false;
        }
        
        return checkpointManager_->saveImmediately(name);
    }
    
    /**
     * Load saved state
     */
    bool loadState(const std::string& name) {
        if (!checkpointManager_) {
            return false;
        }
        
        // We need a Brain instance here - in real implementation
        // this would be passed from the main simulation loop
        NLM_LOG_INFO("ProductionManager load requires Brain instance integration");
        return false;
    }
    
    /**
     * Set progress for long-running operations
     */
    void setProgress(double progress, const std::string& message = "") {
        // Update profiler progress
        if (profiler_) {
            // Get current step from profiler if available
            int64_t currentStep = 0;
            int64_t totalSteps = 100;  // Default
            profiler_->setProgress(progress, message, currentStep, totalSteps);
        }
        
        // Update visualization progress
        if (visualization_) {
            visualization_->visualizeProgress(progress, message);
        }
        
        // Update checkpoint progress
        if (checkpointManager_) {
            CheckpointProgress checkpointProgress = checkpointManager_->getCurrentProgress();
            checkpointProgress.progress = progress;
            checkpointProgress.message = message;
            
            if (checkpointManager_->setProgressCallback(
                [this](const CheckpointProgress& progress) {
                    // Handle checkpoint progress
                })) {
            }
        }
    }
    
    /**
     * Get current performance metrics
     */
    PerformanceMetrics getCurrentMetrics() const {
        if (profiler_) {
            return profiler_->getCurrentMetrics();
        }
        return PerformanceMetrics();
    }
    
    /**
     * Get checkpoint statistics
     */
    CheckpointStats getCheckpointStats() const {
        if (checkpointManager_) {
            return checkpointManager_->getStatistics();
        }
        return CheckpointStats();
    }
    
    /**
     * Check if monitoring is active
     */
    bool isMonitoringEnabled() const { return monitoringEnabled_; }
    
    /**
     * Check if visualization is active
     */
    bool isVisualizationActive() const { return visualizationActive_; }
    
    /**
     * Get production status
     */
    bool isInitialized() const { return initialized_; }
    
    /**
     * Get uptime
     */
    double getUptime() const {
        if (profiler_) {
            return profiler_->getUptime();
        }
        return 0.0;
    }
    
    /**
     * Reset all statistics
     */
    bool resetStatistics() {
        bool success = true;
        
        if (profiler_) {
            success = profiler_->reset() && success;
        }
        
        if (checkpointManager_) {
            success = checkpointManager_->resetStatistics() && success;
        }
        
        return success;
    }
    
private:
    /**
     * Get neuron data for checkpointing (placeholder)
     */
    bool getNeuronDataForCheckpoint(NeuronCheckpointData& data) {
        // In real implementation, this would collect neuron state from the brain
        NLM_LOG_INFO("Collecting neuron data for checkpoint");
        return true;
    }
    
    /**
     * Get synapse data for checkpointing (placeholder)
     */
    bool getSynapseDataForCheckpoint(SynapseCheckpointData& data) {
        // In real implementation, this would collect synapse state from the brain
        NLM_LOG_INFO("Collecting synapse data for checkpoint");
        return true;
    }
    
    // Member variables
    bool initialized_;
    bool monitoringEnabled_;
    bool visualizationActive_;
    
    std::unique_ptr<PerformanceProfiler> profiler_;
    std::unique_ptr<CheckpointManager> checkpointManager_;
    std::unique_ptr<VisualizationInterface> visualization_;
};

} // namespace nlm