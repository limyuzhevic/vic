#include "SimulationScheduler.hpp"
#include "../core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <random>

namespace nlm {

// ============= SIMULATION SCHEDULER IMPLEMENTATION =============

SimulationScheduler::SimulationScheduler() {
    // Initialize system resources
    systemResources_ = ResourceConfig(); // Default
    availableResources_ = systemResources_;
    
    // Initialize scheduler state
    running_ = true;
    simulationIdCounter_ = 0;
    
    NLM_LOG_INFO("SimulationScheduler initialized with resources: " +
                std::to_string(systemResources_.cpuCores) + " CPU cores, " +
                std::to_string(systemResources_.ramMemory) + " GB RAM");
}

bool SimulationScheduler::scheduleSimulation(const ExperimentConfig& experiment) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if experiment is already scheduled
    for (const auto& scheduled : scheduledSimulations_) {
        if (scheduled.experimentId == experiment.experimentName) {
            NLM_LOG_WARN("Experiment already scheduled: " + experiment.experimentName);
            return false;
        }
    }
    
    // Check if resources are available
    ResourceConfig required = calculateRequiredResources(experiment);
    if (!canScheduleExperiment(experiment)) {
        NLM_LOG_WARN("Cannot schedule experiment - insufficient resources: " + experiment.experimentName);
        return false;
    }
    
    // Create simulation schedule
    SimulationSchedule schedule;
    schedule.experimentId = experiment.experimentName;
    schedule.algorithmName = ReinforcementLearnerFactory::algorithmToString(experiment.algorithm);
    schedule.algorithm = experiment.algorithm;
    schedule.brain = experiment.config ? std::make_shared<Brain>(*experiment.config) : nullptr;
    schedule.agent = experiment.config ? std::make_shared<AgentBrain>(schedule.brain) : nullptr;
    schedule.world = experiment.config ? std::make_shared<SimpleWorld>() : nullptr;
    schedule.episodeCount = experiment.episodeCount;
    schedule.estimatedDuration = calculateEstimatedDuration(experiment);
    schedule.resources = required;
    schedule.active = false;
    schedule.completed = false;
    schedule.progress = 0.0f;
    schedule.currentReward = 0.0f;
    schedule.currentSteps = 0;
    
    // Add to scheduled simulations
    scheduledSimulations_.push_back(schedule);
    
    NLM_LOG_INFO("Scheduled simulation: " + experiment.experimentName +
                " (" + schedule.algorithmName + ") - " + std::to_string(schedule.estimatedDuration) + "s estimated");
    
    return true;
}

bool SimulationScheduler::scheduleMultipleExperiments(const std::vector<ExperimentConfig>& experiments) {
    bool allScheduled = true;
    
    for (const auto& experiment : experiments) {
        if (!scheduleSimulation(experiment)) {
            allScheduled = false;
        }
    }
    
    return allScheduled;
}

bool SimulationScheduler::allocateResources(const ResourceConfig& resources) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if requested resources are available
    if (resources.cpuCores > availableResources_.cpuCores) {
        NLM_LOG_WARN("Insufficient CPU cores: required " + std::to_string(resources.cpuCores) +
                    ", available " + std::to_string(availableResources_.cpuCores));
        return false;
    }
    
    if (resources.gpuMemory > availableResources_.gpuMemory) {
        NLM_LOG_WARN("Insufficient GPU memory: required " + std::to_string(resources.gpuMemory) +
                    ", available " + std::to_string(availableResources_.gpuMemory));
        return false;
    }
    
    if (resources.ramMemory > availableResources_.ramMemory) {
        NLM_LOG_WARN("Insufficient RAM: required " + std::to_string(resources.ramMemory) +
                    ", available " + std::to_string(availableResources_.ramMemory));
        return false;
    }
    
    // Allocate resources
    availableResources_.cpuCores -= resources.cpuCores;
    availableResources_.gpuMemory -= resources.gpuMemory;
    availableResources_.ramMemory -= resources.ramMemory;
    
    // Call resource allocation callback
    if (resourceAllocationCallback_) {
        resourceAllocationCallback_(resources);
    }
    
    NLM_LOG_INFO("Allocated resources: " + std::to_string(resources.cpuCores) + " CPU cores, " +
                std::to_string(resources.gpuMemory) + " GPU memory, " +
                std::to_string(resources.ramMemory) + " RAM");
    
    return true;
}

ResourceConfig SimulationScheduler::getAvailableResources() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return availableResources_;
}

std::vector<ResourceConfig> SimulationScheduler::getResourceUsage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ResourceConfig> usage;
    
    for (const auto& simulation : activeSimulations_) {
        usage.push_back(simulation.resources);
    }
    
    return usage;
}

SimulationSchedule* SimulationScheduler::getSimulationSchedule(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check scheduled simulations
    for (auto& scheduled : scheduledSimulations_) {
        if (scheduled.experimentId == experimentId) {
            return &scheduled;
        }
    }
    
    // Check active simulations
    for (auto& active : activeSimulations_) {
        if (active.experimentId == experimentId) {
            return &active;
        }
    }
    
    // Check completed simulations
    for (auto& completed : completedSimulations_) {
        if (completed.experimentId == experimentId) {
            return &completed;
        }
    }
    
    return nullptr;
}

std::vector<SimulationSchedule> SimulationScheduler::getActiveSimulations() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeSimulations_;
}

std::vector<SimulationSchedule> SimulationScheduler::getCompletedSimulations() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return completedSimulations_;
}

std::vector<TrainingJob> SimulationScheduler::getDistributedJobs(const std::string& experimentId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<TrainingJob> jobs;
    
    // Find the simulation
    const SimulationSchedule* simulation = nullptr;
    for (const auto& scheduled : scheduledSimulations_) {
        if (scheduled.experimentId == experimentId) {
            simulation = &scheduled;
            break;
        }
    }
    if (!simulation) {
        for (const auto& active : activeSimulations_) {
            if (active.experimentId == experimentId) {
                simulation = &active;
                break;
            }
        }
    }
    
    if (!simulation) {
        return jobs;
    }
    
    // For now, return empty jobs list (would be populated with actual distributed jobs)
    return jobs;
}

bool SimulationScheduler::startSimulation(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the simulation in scheduled simulations
    auto it = std::find_if(scheduledSimulations_.begin(), scheduledSimulations_.end(),
                          [&experimentId](const SimulationSchedule& s) { return s.experimentId == experimentId; });
    
    if (it == scheduledSimulations_.end()) {
        NLM_LOG_WARN("Cannot start simulation - not found: " + experimentId);
        return false;
    }
    
    SimulationSchedule& simulation = *it;
    
    // Check if resources are available
    ResourceConfig required = calculateRequiredResources(
        ExperimentConfig{simulation.experimentId, "", simulation.algorithm, simulation.episodeCount});
    
    if (!hasAvailableResources(required)) {
        NLM_LOG_WARN("Cannot start simulation - insufficient resources: " + experimentId);
        return false;
    }
    
    // Allocate resources
    if (!allocateResources(required)) {
        NLM_LOG_WARN("Failed to allocate resources for simulation: " + experimentId);
        return false;
    }
    
    // Move to active simulations
    activeSimulations_.push_back(simulation);
    scheduledSimulations_.erase(it);
    
    simulation.active = true;
    simulation.startTime = getCurrentTime();
    simulation.progress = 0.0f;
    
    NLM_LOG_INFO("Started simulation: " + experimentId);
    
    // Call simulation start callback
    if (simulationStartCallback_) {
        simulationStartCallback_(simulation);
    }
    
    return true;
}

bool SimulationScheduler::pauseSimulation(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the simulation in active simulations
    auto it = std::find_if(activeSimulations_.begin(), activeSimulations_.end(),
                          [&experimentId](const SimulationSchedule& s) { return s.experimentId == experimentId; });
    
    if (it == activeSimulations_.end()) {
        NLM_LOG_WARN("Cannot pause simulation - not found or not active: " + experimentId);
        return false;
    }
    
    SimulationSchedule& simulation = *it;
    simulation.active = false;
    
    // Release resources
    ResourceConfig required = calculateRequiredResources(
        ExperimentConfig{simulation.experimentId, "", simulation.algorithm, simulation.episodeCount});
    
    availableResources_.cpuCores += required.cpuCores;
    availableResources_.gpuMemory += required.gpuMemory;
    availableResources_.ramMemory += required.ramMemory;
    
    NLM_LOG_INFO("Paused simulation: " + experimentId);
    
    return true;
}

bool SimulationScheduler::resumeSimulation(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the simulation in scheduled simulations
    auto it = std::find_if(scheduledSimulations_.begin(), scheduledSimulations_.end(),
                          [&experimentId](const SimulationSchedule& s) { return s.experimentId == experimentId; });
    
    if (it == scheduledSimulations_.end()) {
        NLM_LOG_WARN("Cannot resume simulation - not found: " + experimentId);
        return false;
    }
    
    SimulationSchedule& simulation = *it;
    
    // Check if resources are available
    ResourceConfig required = calculateRequiredResources(
        ExperimentConfig{simulation.experimentId, "", simulation.algorithm, simulation.episodeCount});
    
    if (!hasAvailableResources(required)) {
        NLM_LOG_WARN("Cannot resume simulation - insufficient resources: " + experimentId);
        return false;
    }
    
    // Allocate resources
    if (!allocateResources(required)) {
        NLM_LOG_WARN("Failed to allocate resources for resuming simulation: " + experimentId);
        return false;
    }
    
    // Move to active simulations
    activeSimulations_.push_back(simulation);
    scheduledSimulations_.erase(it);
    
    simulation.active = true;
    simulation.progress = 0.0f;
    
    NLM_LOG_INFO("Resumed simulation: " + experimentId);
    
    return true;
}

bool SimulationScheduler::cancelSimulation(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find and remove from all lists
    auto removeFromList = [&experimentId](std::vector<SimulationSchedule>& list) {
        auto it = std::remove_if(list.begin(), list.end(),
                               [&experimentId](const SimulationSchedule& s) {
                                   return s.experimentId == experimentId;
                               });
        list.erase(it, list.end());
    };
    
    removeFromList(scheduledSimulations_);
    removeFromList(activeSimulations_);
    removeFromList(completedSimulations_);
    
    NLM_LOG_INFO("Cancelled simulation: " + experimentId);
    
    return true;
}

bool SimulationScheduler::completeSimulation(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the simulation in active simulations
    auto it = std::find_if(activeSimulations_.begin(), activeSimulations_.end(),
                          [&experimentId](const SimulationSchedule& s) { return s.experimentId == experimentId; });
    
    if (it == activeSimulations_.end()) {
        NLM_LOG_WARN("Cannot complete simulation - not found or not active: " + experimentId);
        return false;
    }
    
    SimulationSchedule& simulation = *it;
    
    // Release resources
    ResourceConfig required = calculateRequiredResources(
        ExperimentConfig{simulation.experimentId, "", simulation.algorithm, simulation.episodeCount});
    
    availableResources_.cpuCores += required.cpuCores;
    availableResources_.gpuMemory += required.gpuMemory;
    availableResources_.ramMemory += required.ramMemory;
    
    // Move to completed simulations
    simulation.completed = true;
    simulation.endTime = getCurrentTime();
    simulation.progress = 1.0f;
    
    completedSimulations_.push_back(simulation);
    activeSimulations_.erase(it);
    
    NLM_LOG_INFO("Completed simulation: " + experimentId);
    
    // Call simulation complete callback
    if (simulationCompleteCallback_) {
        simulationCompleteCallback_(simulation);
    }
    
    return true;
}

bool SimulationScheduler::updateSimulationProgress(const std::string& experimentId, float progress) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the simulation
    SimulationSchedule* simulation = nullptr;
    
    auto checkList = [&experimentId, &simulation](std::vector<SimulationSchedule>& list) {
        for (auto& sim : list) {
            if (sim.experimentId == experimentId) {
                simulation = &sim;
                return true;
            }
        }
        return false;
    };
    
    bool found = checkList(scheduledSimulations_) || 
                 checkList(activeSimulations_) || 
                 checkList(completedSimulations_);
    
    if (!found || !simulation) {
        NLM_LOG_WARN("Cannot update progress - simulation not found: " + experimentId);
        return false;
    }
    
    simulation->progress = std::max(0.0f, std::min(1.0f, progress));
    
    // Call progress callback if progress significant
    if (simulationProgressCallback_ && simulation->progress > 0.0f && 
        simulation->progress < 1.0f) {
        simulationProgressCallback_(*simulation);
    }
    
    return true;
}

bool SimulationScheduler::updateJobProgress(const std::string& experimentId, size_t jobIndex, float progress) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // For now, return false (would need to track distributed jobs separately)
    NLM_LOG_WARN("Job progress tracking not yet implemented: " + experimentId);
    return false;
}

bool SimulationScheduler::adjustSchedule(const std::string& experimentId, const ResourceConfig& newResources) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the simulation
    SimulationSchedule* simulation = nullptr;
    
    auto checkList = [&experimentId, &simulation](std::vector<SimulationSchedule>& list) {
        for (auto& sim : list) {
            if (sim.experimentId == experimentId) {
                simulation = &sim;
                return true;
            }
        }
        return false;
    };
    
    bool found = checkList(scheduledSimulations_) || 
                 checkList(activeSimulations_);
    
    if (!found || !simulation) {
        NLM_LOG_WARN("Cannot adjust schedule - simulation not found: " + experimentId);
        return false;
    }
    
    // For now, just update resources
    simulation->resources = newResources;
    
    NLM_LOG_INFO("Adjusted schedule for simulation: " + experimentId);
    
    return true;
}

bool SimulationScheduler::rescheduleSimulation(const std::string& experimentId, size_t newEpisodeCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the simulation
    SimulationSchedule* simulation = nullptr;
    
    auto checkList = [&experimentId, &simulation](std::vector<SimulationSchedule>& list) {
        for (auto& sim : list) {
            if (sim.experimentId == experimentId) {
                simulation = &sim;
                return true;
            }
        }
        return false;
    };
    
    bool found = checkList(scheduledSimulations_) || 
                 checkList(activeSimulations_);
    
    if (!found || !simulation) {
        NLM_LOG_WARN("Cannot reschedule simulation - not found: " + experimentId);
        return false;
    }
    
    simulation->episodeCount = newEpisodeCount;
    simulation->estimatedDuration = calculateEstimatedDuration(
        ExperimentConfig{simulation->experimentId, "", simulation->algorithm, newEpisodeCount});
    
    NLM_LOG_INFO("Rescheduled simulation " + experimentId + " to " + std::to_string(newEpisodeCount) + " episodes");
    
    return true;
}

bool SimulationScheduler::optimizeResourceAllocation() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Simple optimization: try to consolidate simulations if possible
    if (activeSimulations_.size() < 2) {
        return true; // Nothing to optimize
    }
    
    // Sort simulations by estimated duration
    std::sort(activeSimulations_.begin(), activeSimulations_.end(),
              [](const SimulationSchedule& a, const SimulationSchedule& b) {
                  return a.estimatedDuration < b.estimatedDuration;
              });
    
    // Try to run shorter simulations first
    NLM_LOG_INFO("Optimized resource allocation for " + std::to_string(activeSimulations_.size()) + " simulations");
    
    return true;
}

bool SimulationScheduler::isSimulationActive(const std::string& experimentId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    return std::any_of(activeSimulations_.begin(), activeSimulations_.end(),
                      [&experimentId](const SimulationSchedule& s) {
                          return s.experimentId == experimentId && s.active;
                      });
}

bool SimulationScheduler::isSimulationCompleted(const std::string& experimentId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    return std::any_of(completedSimulations_.begin(), completedSimulations_.end(),
                      [&experimentId](const SimulationSchedule& s) {
                          return s.experimentId == experimentId && s.completed;
                      });
}

SimulationScheduler& SimulationScheduler::setSimulationStartCallback(
    std::function<void(const SimulationSchedule&)> callback) {
    simulationStartCallback_ = callback;
    return *this;
}

SimulationScheduler& SimulationScheduler::setSimulationProgressCallback(
    std::function<void(const SimulationSchedule&)> callback) {
    simulationProgressCallback_ = callback;
    return *this;
}

SimulationScheduler& SimulationScheduler::setSimulationCompleteCallback(
    std::function<void(const SimulationSchedule&)> callback) {
    simulationCompleteCallback_ = callback;
    return *this;
}

SimulationScheduler& SimulationScheduler::setResourceAllocationCallback(
    std::function<void(const ResourceConfig&)> callback) {
    resourceAllocationCallback_ = callback;
    return *this;
}

std::string SimulationScheduler::getSchedulerStatus() const {
    std::stringstream ss;
    ss << "=== Simulation Scheduler Status ===\n";
    ss << "Active Simulations: " << activeSimulations_.size() << "\n";
    ss << "Scheduled Simulations: " << scheduledSimulations_.size() << "\n";
    ss << "Completed Simulations: " << completedSimulations_.size() << "\n";
    ss << "Available Resources: " << availableResources_.cpuCores << " CPU cores, "
       << availableResources_.gpuMemory << " GPU memory, "
       << availableResources_.ramMemory << " RAM\n";
    ss << "System Resources: " << systemResources_.cpuCores << " CPU cores, "
       << systemResources_.gpuMemory << " GPU memory, "
       << systemResources_.ramMemory << " RAM\n";
    ss << "======================================\n";
    
    return ss.str();
}

std::string SimulationScheduler::getResourceReport() const {
    std::stringstream ss;
    ss << "=== Resource Allocation Report ===\n";
    ss << "Available CPU Cores: " << availableResources_.cpuCores << " / " << systemResources_.cpuCores << "\n";
    ss << "Available GPU Memory: " << availableResources_.gpuMemory << " / " << systemResources_.gpuMemory << " GB\n";
    ss << "Available RAM: " << availableResources_.ramMemory << " / " << systemResources_.ramMemory << " GB\n";
    ss << "\nResource Usage by Active Simulations:\n";
    
    for (const auto& simulation : activeSimulations_) {
        ss << "  " << simulation.experimentId << ": "
           << simulation.resources.cpuCores << " cores, "
           << simulation.resources.gpuMemory << " GB GPU, "
           << simulation.resources.ramMemory << " GB RAM\n";
    }
    
    ss << "==========================================\n";
    
    return ss.str();
}

std::string SimulationScheduler::getSimulationReport(const std::string& experimentId) const {
    std::stringstream ss;
    ss << "=== Simulation Report ===\n";
    
    // Find the simulation
    const SimulationSchedule* simulation = nullptr;
    
    auto checkList = [&experimentId, &simulation](const std::vector<SimulationSchedule>& list) {
        for (const auto& sim : list) {
            if (sim.experimentId == experimentId) {
                simulation = &sim;
                return true;
            }
        }
        return false;
    };
    
    bool found = checkList(scheduledSimulations_) || 
                 checkList(activeSimulations_) || 
                 checkList(completedSimulations_);
    
    if (!found || !simulation) {
        ss << "Simulation not found: " << experimentId << "\n";
        ss << "===========================================\n";
        return ss.str();
    }
    
    ss << "Experiment ID: " << simulation->experimentId << "\n";
    ss << "Algorithm: " << simulation->algorithmName << "\n";
    ss << "Status: " << (simulation->active ? "Active" : "Inactive") << "\n";
    ss << "Progress: " << simulation->progress * 100 << "%\n";
    ss << "Current Reward: " << simulation->currentReward << "\n";
    ss << "Current Steps: " << simulation->currentSteps << " / " << simulation->episodeCount << "\n";
    ss << "Estimated Duration: " << simulation->estimatedDuration << "s\n";
    if (simulation->startTime.time_since_epoch().count() > 0) {
        ss << "Start Time: " << std::chrono::system_clock::to_time_t(simulation->startTime) << "\n";
    }
    if (simulation->endTime.time_since_epoch().count() > 0) {
        ss << "End Time: " << std::chrono::system_clock::to_time_t(simulation->endTime) << "\n";
    }
    
    ss << "=========================================\n";
    
    return ss.str();
}

std::string SimulationScheduler::getOverallScheduleReport() const {
    std::stringstream ss;
    ss << "=== Overall Schedule Report ===\n";
    ss << "Total Scheduled: " << scheduledSimulations_.size() << "\n";
    ss << "Total Active: " << activeSimulations_.size() << "\n";
    ss << "Total Completed: " << completedSimulations_.size() << "\n";
    ss << "\nSimulation Details:\n";
    
    for (const auto& simulation : scheduledSimulations_) {
        ss << "  Scheduled: " << simulation.experimentId << " (" << simulation.algorithmName << ")\n";
    }
    
    for (const auto& simulation : activeSimulations_) {
        ss << "  Active: " << simulation.experimentId << " (" << simulation.algorithmName << ") - "
           << simulation.progress * 100 << "% complete\n";
    }
    
    for (const auto& simulation : completedSimulations_) {
        ss << "  Completed: " << simulation.experimentId << " (" << simulation.algorithmName << ")\n";
    }
    
    ss << "======================================\n";
    
    return ss.str();
}

void SimulationScheduler::cleanupCompletedSimulations() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Remove completed simulations that have been completed for a while
    auto it = std::remove_if(completedSimulations_.begin(), completedSimulations_.end(),
                            [](const SimulationSchedule& s) {
                                return s.completed;
                            });
    completedSimulations_.erase(it, completedSimulations_.end());
    
    NLM_LOG_INFO("Cleaned up completed simulations");
}

void SimulationScheduler::stopAllSimulations() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Pause all active simulations
    for (auto& simulation : activeSimulations_) {
        simulation.active = false;
    }
    
    // Release all resources
    ResourceConfig totalAllocated = ResourceConfig(); // Reset to default
    availableResources_ = totalAllocated;
    
    NLM_LOG_INFO("Stopped all simulations");
}

bool SimulationScheduler::canScheduleExperiment(const ExperimentConfig& experiment) const {
    ResourceConfig required = calculateRequiredResources(experiment);
    return hasAvailableResources(required);
}

ResourceConfig SimulationScheduler::calculateRequiredResources(const ExperimentConfig& experiment) const {
    ResourceConfig required;
    
    // Base resource requirements
    required.cpuCores = 1;
    required.gpuMemory = (experiment.requiresGPU ? 2 : 0); // 2 GB if GPU required
    required.ramMemory = 8; // 8 GB base
    
    // Adjust based on algorithm complexity
    switch (experiment.algorithm) {
        case RLAlgorithm::DEEP_Q_NETWORK:
            required.cpuCores = 2;
            required.gpuMemory = 4;
            required.ramMemory = 16;
            break;
        case RLAlgorithm::POLICY_GRADIENT:
            required.cpuCores = 2;
            required.gpuMemory = 2;
            required.ramMemory = 12;
            break;
        case RLAlgorithm::PPO:
            required.cpuCores = 2;
            required.gpuMemory = 4;
            required.ramMemory = 16;
            break;
        case RLAlgorithm::SAC:
            required.cpuCores = 2;
            required.gpuMemory = 4;
            required.ramMemory = 16;
            break;
    }
    
    // Adjust based on episode count (longer training = more memory)
    required.ramMemory += (experiment.episodeCount / 100) * 2; // 2 GB per 100 episodes
    
    // Adjust based on distributed training
    if (experiment.distributedTraining) {
        required.cpuCores *= experiment.config ? experiment.config->getOr<int>("num_workers", 2) : 2;
        required.ramMemory *= experiment.config ? experiment.config->getOr<int>("num_workers", 2) : 2;
    }
    
    return required;
}

bool SimulationScheduler::hasAvailableResources(const ResourceConfig& required) const {
    if (required.cpuCores > availableResources_.cpuCores) return false;
    if (required.gpuMemory > availableResources_.gpuMemory) return false;
    if (required.ramMemory > availableResources_.ramMemory) return false;
    return true;
}

void SimulationScheduler::allocateResourcesToSimulation(SimulationSchedule& simulation) {
    ResourceConfig required = calculateRequiredResources(
        ExperimentConfig{simulation.experimentId, "", simulation.algorithm, simulation.episodeCount});
    
    if (allocateResources(required)) {
        simulation.resources = required;
    }
}

void SimulationScheduler::releaseResourcesFromSimulation(SimulationSchedule& simulation) {
    ResourceConfig required = simulation.resources;
    
    availableResources_.cpuCores += required.cpuCores;
    availableResources_.gpuMemory += required.gpuMemory;
    availableResources_.ramMemory += required.ramMemory;
}

std::vector<TrainingJob> SimulationScheduler::createDistributedJobs(const SimulationSchedule& simulation) {
    std::vector<TrainingJob> jobs;
    
    // For now, return empty (would need distributed training implementation)
    return jobs;
}

std::chrono::system_clock::time_point SimulationScheduler::getCurrentTime() const {
    return std::chrono::system_clock::now();
}

double SimulationScheduler::calculateEstimatedDuration(const ExperimentConfig& experiment) const {
    // Very rough estimation based on algorithm and episode count
    double baseTimePerEpisode = 10.0; // seconds per episode
    
    switch (experiment.algorithm) {
        case RLAlgorithm::TABULAR_Q_LEARNING:
            baseTimePerEpisode = 5.0;
            break;
        case RLAlgorithm::DEEP_Q_NETWORK:
            baseTimePerEpisode = 15.0;
            break;
        case RLAlgorithm::POLICY_GRADIENT:
            baseTimePerEpisode = 20.0;
            break;
        case RLAlgorithm::PPO:
            baseTimePerEpisode = 25.0;
            break;
        case RLAlgorithm::SAC:
            baseTimePerEpisode = 30.0;
            break;
    }
    
    return baseTimePerEpisode * experiment.episodeCount;
}

bool SimulationScheduler::runSimulation(SimulationSchedule& simulation) {
    // Simplified simulation run (would integrate with actual training)
    double duration = calculateEstimatedDuration(
        ExperimentConfig{simulation.experimentId, "", simulation.algorithm, simulation.episodeCount});
    
    simulation.startTime = getCurrentTime();
    simulation.progress = 1.0f;
    simulation.currentReward = 100.0f + simulation.episodeCount; // Dummy reward
    simulation.currentSteps = simulation.episodeCount;
    
    // Simulate some time passing
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(duration * 100)));
    
    simulation.endTime = getCurrentTime();
    
    return true;
}

bool SimulationScheduler::runDistributedSimulation(SimulationSchedule& simulation) {
    // Distributed simulation (would use multiple workers)
    return runSimulation(simulation);
}

// Implementation of factory methods
std::unique_ptr<SimulationScheduler> SimulationSchedulerFactory::createLocalScheduler(
    const ResourceConfig& systemResources) {
    
    auto scheduler = std::make_unique<SimulationScheduler>();
    scheduler->systemResources_ = systemResources;
    scheduler->availableResources_ = systemResources;
    
    NLM_LOG_INFO("Created local scheduler with resources: " +
                std::to_string(systemResources.cpuCores) + " CPU cores, " +
                std::to_string(systemResources.ramMemory) + " GB RAM");
    
    return scheduler;
}

std::unique_ptr<SimulationScheduler> SimulationSchedulerFactory::createClusterScheduler(
    const std::vector<ResourceConfig>& workerResources,
    const ResourceConfig& masterResources) {
    
    // This would create a cluster scheduler with master and worker nodes
    return createLocalScheduler(masterResources);
}

std::unique_ptr<SimulationScheduler> SimulationSchedulerFactory::createCloudScheduler(
    const std::string& provider,
    const std::string& region,
    const std::string& credentials) {
    
    // This would create a cloud-based scheduler
    return createLocalScheduler(ResourceConfig()); // Default resources
}

std::vector<ExperimentConfig> SimulationSchedulerFactory::optimizeExperimentSchedule(
    const std::vector<ExperimentConfig>& experiments,
    const ResourceConfig& availableResources) {
    
    std::vector<ExperimentConfig> optimized;
    
    // Simple optimization: sort by resource requirements and priority
    std::vector<ExperimentConfig> sortedExperiments = experiments;
    
    std::sort(sortedExperiments.begin(), sortedExperiments.end(),
              [&availableResources](const ExperimentConfig& a, const ExperimentConfig& b) {
                  ResourceConfig requiredA = SimulationScheduler::calculateRequiredResources(a);
                  ResourceConfig requiredB = SimulationScheduler::calculateRequiredResources(b);
                  
                  bool aFits = SimulationScheduler::hasAvailableResources(requiredA);
                  bool bFits = SimulationScheduler::hasAvailableResources(requiredB);
                  
                  if (aFits && !bFits) return true;
                  if (!aFits && bFits) return false;
                  
                  // Both fit or both don't fit, sort by priority
                  return a.priority > b.priority;
              });
    
    optimized = sortedExperiments;
    
    NLM_LOG_INFO("Optimized experiment schedule for " + std::to_string(experiments.size()) + " experiments");
    
    return optimized;
}

std::string SimulationSchedulerFactory::generateExperimentId(const std::string& prefix) {
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, 999999);
    
    return prefix + "_" + std::to_string(dist(rng));
}

std::chrono::system_clock::duration SimulationSchedulerFactory::calculatePriority(
    const ExperimentConfig& experiment, const ResourceConfig& resources) {
    
    // Calculate priority based on experiment importance and resource requirements
    std::chrono::seconds basePriority = std::chrono::seconds(experiment.episodeCount / 100);
    
    // Prefer experiments with higher episode counts
    if (experiment.episodeCount > 500) {
        basePriority += std::chrono::seconds(60);
    }
    
    // Prefer experiments that require more resources (use them efficiently)
    if (experiment.requiresGPU) {
        basePriority += std::chrono::seconds(30);
    }
    
    // Respect original priority
    return basePriority + experiment.priority;
}

} // namespace nlm
