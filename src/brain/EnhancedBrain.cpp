#include "EnhancedBrain.hpp"

namespace nlm {

// EnhancedBrain implementation

EnhancedBrain::EnhancedBrain(std::shared_ptr<Config> config,
                             bool enableProfiling,
                             bool enableMemoryTracking,
                             bool enableErrorDetection)
    : brain_(std::make_shared<Brain>(config))
    , profilingEnabled_(enableProfiling)
    , memoryTrackingEnabled_(enableMemoryTracking)
    , errorDetectionEnabled_(enableErrorDetection)
    , initializationTime_(std::chrono::high_resolution_clock::now())
    , healthMonitorActive_(false) {
    
    // Initialize monitoring systems
    if (profilingEnabled_) {
        simdProfiler_ = std::make_unique<EnhancedSIMDVectorization>();
    }
    
    // Create enhanced memory pools
    if (memoryTrackingEnabled_) {
        neuronPool_ = std::make_unique<EnhancedNeuronPool>(1024, true);
        synapsePool_ = std::make_unique<EnhancedSynapsePool>(5000, true);
    }
    
    // Start health monitoring
    if (errorDetectionEnabled_) {
        startHealthMonitoring();
    }
}

bool EnhancedBrain::initialize() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Validate configuration
    if (!validateConfiguration()) {
        return false;
    }
    
    // Initialize original brain
    bool initSuccess = brain_->initialize();
    if (!initSuccess) {
        return false;
    }
    
    // Update initialization time
    initializationTime_ = std::chrono::high_resolution_clock::now();
    
    // Record initialization statistics
    if (profilingEnabled_) {
        recordOperation("initialization", startTime);
    }
    
    // Check for initialization errors
    if (errorDetectionEnabled_) {
        checkSystemHealth();
    }
    
    return true;
}

void EnhancedBrain::step(SimulationStep step, Timestamp time) {
    auto stepStartTime = std::chrono::high_resolution_clock::now();
    
    // Record step start
    if (profilingEnabled_) {
        recordOperation("step_start", stepStartTime);
    }
    
    // Perform step
    brain_->step(step, time);
    
    // Record step completion
    if (profilingEnabled_) {
        recordOperation("step_end", stepStartTime);
    }
    
    // Update memory pools
    if (memoryTrackingEnabled_) {
        updateMemoryPools();
    }
    
    // Check for errors
    if (errorDetectionEnabled_) {
        checkForErrors();
    }
    
    // Update system health
    if (errorDetectionEnabled_) {
        updateSystemHealth();
    }
}

bool EnhancedBrain::validateConfiguration() const {
    std::vector<std::string> errors;
    
    // Check required configuration keys
    if (!brain_->getConfig()->has("brain.neuron_count")) {
        errors.push_back("Missing brain.neuron_count configuration");
    }
    
    if (!brain_->getConfig()->has("brain.synapse_density")) {
        errors.push_back("Missing brain.synapse_density configuration");
    }
    
    // Validate values
    size_t neuronCount = brain_->getConfig()->getOr<size_t>("brain.neuron_count", 0);
    if (neuronCount < 10) {
        errors.push_back("Neuron count too low (minimum 10 recommended)");
    }
    
    float connectionProbability = brain_->getConfig()->getOr<float>("brain.connection_probability", 0.0f);
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        errors.push_back("Connection probability must be between 0.0 and 1.0");
    }
    
    // Report errors
    if (!errors.empty()) {
        for (const auto& error : errors) {
            NLM_LOG_ERROR("Configuration validation error: " + error);
        }
        return false;
    }
    
    return true;
}

void EnhancedBrain::startHealthMonitoring() {
    healthMonitorActive_ = true;
    healthMonitorThread_ = std::thread(&EnhancedBrain::healthMonitorLoop, this);
}

void EnhancedBrain::stopHealthMonitoring() {
    healthMonitorActive_ = false;
    if (healthMonitorThread_.joinable()) {
        healthMonitorThread_.join();
    }
}

void EnhancedBrain::healthMonitorLoop() {
    while (healthMonitorActive_) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        if (errorDetectionEnabled_) {
            checkSystemHealth();
        }
    }
}

void EnhancedBrain::checkSystemHealth() {
    SystemHealth newHealth;
    newHealth.cpuUsage = getCPUUsage();
    newHealth.memoryUsage = getMemoryUsage();
    newHealth.performanceScore = computePerformanceScore();
    
    // Check for issues
    if (newHealth.cpuUsage > 0.9) {
        newHealth.warnings.push_back("High CPU usage: " + std::to_string(newHealth.cpuUsage * 100.0) + "%");
        newHealth.status = SystemHealth::Status::DEGRADED;
    }
    
    if (newHealth.memoryUsage > getMemoryLimit() * 0.9) {
        newHealth.warnings.push_back("High memory usage: " + std::to_string(newHealth.memoryUsage) + " bytes");
        newHealth.status = SystemHealth::Status::DEGRADED;
    }
    
    if (newHealth.performanceScore < 0.5) {
        newHealth.errors.push_back("Low performance score: " + std::to_string(newHealth.performanceScore * 100.0) + "%");
        newHealth.status = SystemHealth::Status::CRITICAL;
    }
    
    // Check for critical errors
    if (!newHealth.errors.empty()) {
        for (const auto& error : newHealth.errors) {
            NLM_LOG_ERROR("Critical system error: " + error);
        }
    }
    
    systemHealth_ = newHealth;
}

void EnhancedBrain::checkForErrors() {
    // Check for performance issues
    double avgStepTime = getAverageStepTime();
    if (avgStepTime > getTargetStepTime() * 2.0) {
        systemHealth_.warnings.push_back("Step execution time exceeded target by more than 2x");
    }
    
    // Check for memory issues
    if (neuronPool_) {
        std::string neuronHealth = neuronPool_->checkHealth();
        if (!neuronHealth.empty()) {
            systemHealth_.warnings.push_back("Neuron pool health issues: " + neuronHealth);
        }
    }
    
    if (synapsePool_) {
        std::string synapseHealth = synapsePool_->checkHealth();
        if (!synapseHealth.empty()) {
            systemHealth_.warnings.push_back("Synapse pool health issues: " + synapseHealth);
        }
    }
}

void EnhancedBrain::updateSystemHealth() {
    // Update performance score based on various metrics
    double stepEfficiency = computeStepEfficiency();
    double memoryEfficiency = computeMemoryEfficiency();
    
    systemHealth_.performanceScore = (stepEfficiency + memoryEfficiency) / 2.0;
    
    if (systemHealth_.performanceScore > 0.8) {
        systemHealth_.status = SystemHealth::Status::HEALTHY;
    } else if (systemHealth_.performanceScore > 0.5) {
        systemHealth_.status = SystemHealth::Status::DEGRADED;
    } else {
        systemHealth_.status = SystemHealth::Status::CRITICAL;
    }
}

void EnhancedBrain::recordOperation(const std::string& operationName,
                                    const std::chrono::high_resolution_clock::time_point& startTime) {
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    operationStats_[operationName].recordExecution(duration, 0, 0); // Simplified
    operationCounts_[operationName]++;
}

void EnhancedBrain::updateMemoryPools() {
    if (neuronPool_) {
        // Update neuron pool statistics
        // In real implementation, would track actual neuron allocations
    }
    
    if (synapsePool_) {
        // Update synapse pool statistics
        // In real implementation, would track actual synapse allocations
    }
}

void EnhancedBrain::printPerformanceReport() const {
    std::cout << getPerformanceStats() << std::endl;
    std::cout << getMemoryPoolStats() << std::endl;
    std::cout << "System Health: " << systemHealth_.getStatusString() << std::endl;
    
    if (!systemHealth_.warnings.empty()) {
        std::cout << "Warnings:" << std::endl;
        for (const auto& warning : systemHealth_.warnings) {
            std::cout << "  - " << warning << std::endl;
        }
    }
    
    if (!systemHealth_.errors.empty()) {
        std::cout << "Errors:" << std::endl;
        for (const auto& error : systemHealth_.errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
}

// Helper function implementations

double EnhancedBrain::getCPUUsage() const {
    return 0.5; // Simplified - would use system-specific implementation
}

double EnhancedBrain::getMemoryUsage() const {
    return 0.0; // Simplified - would use system-specific implementation
}

double EnhancedBrain::getMemoryLimit() const {
    return 1024.0 * 1024.0 * 1024.0; // 1GB simplified
}

double EnhancedBrain::computePerformanceScore() const {
    double stepEfficiency = computeStepEfficiency();
    double memoryEfficiency = computeMemoryEfficiency();
    return (stepEfficiency + memoryEfficiency) / 2.0;
}

double EnhancedBrain::getAverageStepTime() const {
    auto it = operationStats_.find("step");
    if (it == operationStats_.end() || it->second.executionCount == 0) {
        return 0.0;
    }
    return it->second.avgExecutionTime;
}

double EnhancedBrain::getTargetStepTime() const {
    return 0.001; // 1ms target
}

double EnhancedBrain::computeStepEfficiency() const {
    double avgStepTime = getAverageStepTime();
    double targetStepTime = getTargetStepTime();
    
    if (avgStepTime > targetStepTime) {
        return targetStepTime / avgStepTime;
    }
    return 1.0;
}

double EnhancedBrain::computeMemoryEfficiency() const {
    // Simplified - would track actual memory usage
    return 1.0;
}

} // namespace nlm