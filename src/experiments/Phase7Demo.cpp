#include "Phase7Demo.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>
#include <iomanip>

namespace nlm {

Phase7Demo::Phase7Demo() : running_(false), paused_(false) {
    startWallTime_ = std::chrono::high_resolution_clock::now();
    lastStepTime_ = startWallTime_;
    totalSteps_ = 0;
}

Phase7Demo::~Phase7Demo() {
    stopMonitoring();
}

void Phase7Demo::run(
    const Phase7Config& config,
    const std::shared_ptr<class Brain>& brain,
    const std::shared_ptr<class SimpleWorld>& world,
    const std::shared_ptr<class AgentBrain>& agent,
    const std::shared_ptr<ConsoleVisualization>& visualization
) {
    if (!brain || !world || !agent || !visualization) {
        std::cerr << "ERROR: Invalid parameters for Phase 7 demo!" << std::endl;
        return;
    }
    
    running_ = true;
    totalSteps_ = config.numSteps;
    
    std::cout << "Starting Phase 7 demo with " << totalSteps_ << " steps..." << std::endl;
    std::cout << "Press 'h' for help, 'q' to quit, 'p' to pause during simulation." << std::endl;
    std::cout << std::endl;
    
    startWallTime_ = std::chrono::high_resolution_clock::now();
    lastStepTime_ = startWallTime_;
    
    // Start monitoring
    startMonitoring();
    
    // Run simulation loop
    for (size_t step = 0; step < totalSteps_ && running_; ++step) {
        // Check for pause
        if (paused_) {
            std::cout << "PAUSED. Press 'p' to resume or 'q' to quit." << std::endl;
            char input = waitForInput();
            if (input == 'p' || input == 'P') {
                paused_ = false;
                std::cout << "Resuming..." << std::endl;
            } else if (input == 'q' || input == 'Q') {
                running_ = false;
                break;
            }
            continue;
        }
        
        // Update visualization
        if (visualization->isActive()) {
            visualization->update(brain, step, 0.1);
        }
        
        // Run simulation step
        auto stepStart = std::chrono::high_resolution_clock::now();
        
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation
        float reward = world->getSensoryPercept().getInternal()[0] if 
            world->getSensoryPercept().getInternal() else 0.0f;
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Record step
        recordStep(step, 0.1, brain);
        
        // Calculate step time
        auto stepEnd = std::chrono::high_resolution_clock::now();
        auto stepDuration = std::chrono::duration_cast<std::chrono::duration<double>>(stepEnd - stepStart).count();
        
        // Display progress every 100 steps or on user request
        if (step % 100 == 0 || config.exportData) {
            displayProgress(step, totalSteps_, stepDuration, brain, visualization);
        }
        
        // Check for user input (non-blocking)
        if (!config.exportData) {
            char input = getNonBlockingInput();
            if (input == 'q' || input == 'Q') {
                std::cout << "\nUser requested quit..." << std::endl;
                running_ = false;
                break;
            } else if (input == 'p' || input == 'P') {
                paused_ = !paused_;
                if (paused_) {
                    std::cout << "PAUSED. Press 'p' to resume or 'q' to quit." << std::endl;
                }
            } else if (input == 'h' || input == 'H') {
                printHelp();
            } else if (input == 's' || input == 'S') {
                toggleNetworkVisualization();
            } else if (input == 'a' || input == 'A') {
                toggleActivityVisualization();
            } else if (input == 'r' || input == 'R') {
                toggleSpikeVisualization();
            } else if (input == 'm' || input == 'M') {
                toggleMetricsVisualization();
            }
        }
        
        // Small delay to maintain simulation rate
        double remainingTime = 0.1 - stepDuration;  // 0.1 second timestep
        if (remainingTime > 0.001) {
            std::this_thread::sleep_for(std::chrono::duration<double>(remainingTime));
        }
    }
    
    // Stop monitoring
    stopMonitoring();
    
    // Display final statistics
    displayFinalStatistics(brain, visualization);
    
    // Render final frame
    if (visualization->isActive()) {
        visualization->render();
        std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
    }
    
    std::cout << "\nPhase 7 demo completed successfully!" << std::endl;
}

void Phase7Demo::displayProgress(
    size_t step,
    size_t totalSteps,
    double stepDuration,
    const std::shared_ptr<class Brain>& brain,
    const std::shared_ptr<ConsoleVisualization>& visualization
) {
    double progress = static_cast<double>(step) / totalSteps;
    size_t barWidth = 30;
    size_t filledWidth = static_cast<size_t>(progress * barWidth);
    
    std::cout << "\rStep: " << step << "/" << totalSteps 
              << " (" << std::fixed << std::setprecision(1) << progress * 100 << "%)" << std::endl;
    
    std::cout << "  Progress: [";
    for (size_t i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            std::cout << "=";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "]" << std::endl;
    
    if (brain) {
        std::cout << "  Brain Stats: " << brain->getFiringNeuronCount() << " firing, "
                  << brain->getTotalSpikeCount() << " total spikes, "
                  << std::setprecision(2) << brain->getAverageFiringRate() << " Hz avg rate" << std::endl;
    }
    
    std::cout << "  Step Time: " << std::fixed << std::setprecision(4) << stepDuration * 1000 << " ms" << std::endl;
    std::cout << "  Sim Rate: " << std::fixed << std::setprecision(1) << 1.0 / stepDuration << " Hz" << std::endl;
    
    if (visualization && visualization->isActive()) {
        visualization->render();
    }
}

void Phase7Demo::displayFinalStatistics(
    const std::shared_ptr<class Brain>& brain,
    const std::shared_ptr<ConsoleVisualization>& visualization
) {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startWallTime_).count();
    
    std::cout << std::endl;
    std::cout << "=== FINAL STATISTICS ===" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total Steps: " << totalSteps_ << std::endl;
    std::cout << "Total Time: " << totalDuration << " seconds" << std::endl;
    std::cout << "Simulation Rate: " << totalSteps_ / totalDuration << " Hz" << std::endl;
    std::cout << std::endl;
    
    if (brain) {
        std::cout << "Brain Statistics:" << std::endl;
        std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Visualization Statistics:" << std::endl;
    std::cout << "  Frames Rendered: " << getFrameCount() << std::endl;
    std::cout << "  Average Frame Time: " << getAverageFrameTime() << " ms" << std::endl;
    
    if (visualization) {
        std::cout << "  Active: " << (visualization->isActive() ? "Yes" : "No") << std::endl;
        std::cout << "  Brain Stats: " << visualization->getBrainStats() << std::endl;
        std::cout << "  Performance Stats: " << visualization->getPerformanceStats() << std::endl;
    }
}

void Phase7Demo::startMonitoring() {
    // TODO: Implement performance monitoring
    monitoring_ = true;
}

void Phase7Demo::stopMonitoring() {
    monitoring_ = false;
}

void Phase7Demo::recordStep(
    uint64_t step,
    double timestep,
    const std::shared_ptr<class Brain>& brain
) {
    std::lock_guard<std::mutex> lock(monitorMutex_);
    
    stepCount_++;
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastStepTime_).count();
    lastStepTime_ = currentTime;
    
    totalStepTime_ += duration;
    
    if (brain) {
        double firingRate = brain->getAverageFiringRate();
        totalFiringRate_ += firingRate;
        maxFiringRate_ = std::max(maxFiringRate_, firingRate);
        minFiringRate_ = std::min(minFiringRate_, firingRate);
        
        totalSpikes_ += brain->getTotalSpikeCount();
        activeNeurons_ += brain->getActiveNeuronCount();
    }
    
    // Record for history
    stepHistory_.push_back({step, duration, timestep, brain ? brain->getAverageFiringRate() : 0.0});
    if (stepHistory_.size() > maxHistory_) {
        stepHistory_.erase(stepHistory_.begin());
    }
}

std::string Phase7Demo::getPerformanceStats() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    if (stepCount_ > 0) {
        ss << "Steps/sec: " << stepCount_ / totalStepTime_ << " ";
        ss << "Avg Step Time: " << totalStepTime_ / stepCount_ * 1000 << " ms ";
        ss << "Firing Rate: " << totalFiringRate_ / stepCount_ << " Hz ";
        ss << "Avg Spikes: " << totalSpikes_ / stepCount_;
    }
    
    return ss.str();
}

size_t Phase7Demo::getMemoryUsage() const {
    // TODO: Implement memory usage tracking
    return 0;
}

double Phase7Demo::getCPUUsage() const {
    // TODO: Implement CPU usage tracking
    return 0.0;
}

double Phase7Demo::getSimulationRate() const {
    return stepCount_ / totalStepTime_;
}

void Phase7Demo::printHelp() const {
    std::cout << "=== Phase 7 Demo Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Keyboard Commands:" << std::endl;
    std::cout << "  'q' or 'Q' : Quit the demo" << std::endl;
    std::cout << "  'p' or 'P' : Pause/resume simulation" << std::endl;
    std::cout << "  'h' or 'H' : Show this help" << std::endl;
    std::cout << "  's' or 'S' : Toggle network visualization" << std::endl;
    std::cout << "  'a' or 'A' : Toggle activity visualization" << std::endl;
    std::cout << "  'r' or 'R' : Toggle spike visualization" << std::endl;
    std::cout << "  'm' or 'M' : Toggle metrics visualization" << std::endl;
    std::cout << "  '+'         : Increase simulation speed (placeholder)" << std::endl;
    std::cout << "  '-'         : Decrease simulation speed (placeholder)" << std::endl;
    std::cout << std::endl;
    std::cout << "Visualization Features:" << std::endl;
    std::cout << "  - Network topology display" << std::endl;
    std::cout << "  - Activity heatmap" << std::endl;
    std::endl;
    << "  - Spike raster plot" << std::endl;
    std::cout << "  - Real-time metrics display" << std::endl;
    std::cout << "  - Performance monitoring" << std::endl;
    std::cout << "  - Export capabilities" << std::endl;
}

char Phase7Demo::getNonBlockingInput() {
    // TODO: Implement non-blocking input (platform-specific)
    return 0;
}

char Phase7Demo::waitForInput() {
    // TODO: Implement blocking input (platform-specific)
    return 0;
}

void Phase7Demo::toggleNetworkVisualization() {
    // TODO: Implement network visualization toggle
}

void Phase7Demo::toggleActivityVisualization() {
    // TODO: Implement activity visualization toggle
}

void Phase7Demo::toggleSpikeVisualization() {
    // TODO: Implement spike visualization toggle
}

void Phase7Demo::toggleMetricsVisualization() {
    // TODO: Implement metrics visualization toggle
}

void Phase7Demo::saveVisualizationFrames() {
    // TODO: Implement frame saving
}

void Phase7Demo::exportData(const std::string& outputDir) {
    // TODO: Implement data export
}

std::string Phase7Demo::generateReport() const {
    std::stringstream ss;
    ss << "=== NLM Phase 7 Demo Report ===" << std::endl;
    ss << std::endl;
    ss << "Simulation Configuration:" << std::endl;
    ss << "  Total Steps: " << totalSteps_ << std::endl;
    ss << "  Max History: " << maxHistory_ << std::endl;
    ss << "  Update Interval: " << updateInterval_ << std::endl;
    ss << "  Color Scheme: " << colorScheme_ << std::endl;
    ss << std::endl;
    ss << "Performance Metrics:" << std::endl;
    ss << "  Simulation Rate: " << getSimulationRate() << " Hz" << std::endl;
    ss << "  Average Firing Rate: " << (stepCount_ > 0 ? totalFiringRate_ / stepCount_ : 0.0) << " Hz" << std::endl;
    ss << "  Max Firing Rate: " << maxFiringRate_ << " Hz" << std::endl;
    ss << "  Min Firing Rate: " << minFiringRate_ << " Hz" << std::endl;
    ss << "  Total Steps Recorded: " << stepCount_ << std::endl;
    ss << "  Total Time: " << totalStepTime_ << " seconds" << std::endl;
    ss << std::endl;
    ss << "Visualization Status:" << std::endl;
    ss << "  Running: " << (running_ ? "Yes" : "No") << std::endl;
    ss << "  Paused: " << (paused_ ? "Yes" : "No") << std::endl;
    ss << "  Network: " << (showNetwork_ ? "Visible" : "Hidden") << std::endl;
    ss << "  Activity: " << (showActivity_ ? "Visible" : "Hidden") << std::endl;
    ss << "  Spikes: " << (showSpikes_ ? "Visible" : "Hidden") << std::endl;
    ss << "  Metrics: " << (showMetrics_ ? "Visible" : "Hidden") << std::endl;
    ss << std::endl;
    
    return ss.str();
}

} // namespace nlm