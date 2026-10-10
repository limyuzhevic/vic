#include "BrainSimulator.hpp"
#include "BrainDebugger.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

BrainSimulator::BrainSimulator(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger)
    : brain_(brain), debugger_(debugger), config_(nullptr), clock_(nullptr), random_(nullptr),
      isRunning_(false), isPaused_(false), currentStep_(0), currentTime_(0.0), 
      wallClockStart_(0.0), lastStepTime_(0.0), stepsCompleted_(0), totalStepsExecuted_(0),
      realTimeFactor_(1.0), avgStepTime_(0.0), stepsPerSecond_(0), loggingEnabled_(true) {
    
    // Try to get configuration from brain if available
    NLM_LOG_INFO("BrainSimulator initialized");
}

BrainSimulator::~BrainSimulator() {
    shutdown();
}

bool BrainSimulator::initialize() {
    if (!brain_) {
        NLM_LOG_ERROR("BrainSimulator: No brain reference available");
        return false;
    }
    
    // Initialize simulation clock
    clock_ = std::make_shared<SimulationClock>();
    
    // Initialize random generator
    random_ = std::make_shared<RandomGenerator>();
    
    // Initialize debugger if not attached
    if (!debugger_) {
        NLM_LOG_INFO("Creating default BrainDebugger");
        debugger_ = std::make_shared<BrainDebugger>(brain_, nullptr, nullptr);
        debugger_->initializeAnalysis();
    }
    
    // Reset simulation state
    reset();
    
    NLM_LOG_INFO("BrainSimulator initialized successfully");
    return true;
}

void BrainSimulator::shutdown() {
    if (isRunning()) {
        stop();
    }
    
    if (debugger_) {
        debugger_->shutdownAnalysis();
    }
    
    cleanup();
    
    NLM_LOG_INFO("BrainSimulator shutdown completed");
}

void BrainSimulator::cleanup() {
    if (simulationThread_.joinable()) {
        simulationThread_.join();
    }
    
    snapshots_.clear();
    snapshotHistory_.clear();
    
    currentStep_ = 0;
    currentTime_ = 0.0;
    wallClockStart_ = 0.0;
    lastStepTime_ = 0.0;
    stepsCompleted_ = 0;
    totalStepsExecuted_ = 0;
}

void BrainSimulator::run(double duration, double timestep) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot run simulation: no brain reference");
        return;
    }
    
    if (!isRunning()) {
        isRunning_ = true;
        isPaused_ = false;
        wallClockStart_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        
        simulationThread_ = std::thread(&BrainSimulator::runSimulationLoop, this, CustomSimulationConfig());
        simulationThread_.detach();
    }
    
    NLM_LOG_INFO("Brain simulation started: duration=" + std::to_string(duration) + 
                 "s, timestep=" + std::to_string(timestep) + "s");
}

void BrainSimulator::runToStep(size_t targetStep) {
    if (!brain_) return;
    
    CustomSimulationConfig config;
    config.maxSteps = targetStep;
    config.stepCallback = [this, targetStep](BrainSimulator& sim, SimulationStep step, double time) {
        if (step >= targetStep) {
            sim.stop();
        }
    };
    
    runCustomSimulation(config);
}

void BrainSimulator::runUntil(double timepoint) {
    if (!brain_) return;
    
    CustomSimulationConfig config;
    config.stepCallback = [this, timepoint](BrainSimulator& sim, SimulationStep step, double time) {
        if (time >= timepoint) {
            sim.stop();
        }
    };
    
    runCustomSimulation(config);
}

void BrainSimulator::pause() {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    isPaused_ = true;
    NLM_LOG_INFO("Brain simulation paused");
}

void BrainSimulator::resume() {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    isPaused_ = false;
    NLM_LOG_INFO("Brain simulation resumed");
}

void BrainSimulator::stop() {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    isRunning_ = false;
    isPaused_ = false;
    
    if (simulationThread_.joinable()) {
        simulationThread_.join();
    }
    
    NLM_LOG_INFO("Brain simulation stopped");
}

void BrainSimulator::step() {
    if (!brain_ || !isRunning_) {
        return;
    }
    
    double timestep = 0.01; // Default timestep
    step(timestep);
}

void BrainSimulator::step(double timestep) {
    if (!brain_ || !isRunning_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(simulationMutex_);
    
    auto startTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    executeStep(timestep);
    
    auto endTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    double stepTime = endTime - startTime;
    
    // Update performance metrics
    lastStepTime_ = stepTime;
    stepsCompleted_++;
    totalStepsExecuted_++;
    
    if (loggingEnabled_) {
        logStep(captureState());
    }
}

void BrainSimulator::step(SimulationStep step, Timestamp time) {
    if (!brain_) return;
    
    std::lock_guard<std::mutex> lock(simulationMutex_);
    
    currentStep_ = step;
    currentTime_ = time;
    
    // Execute step with given parameters
    executeStep(0.01); // Use default timestep for now
}

bool BrainSimulator::isRunning() const {
    return isRunning_.load();
}

bool BrainSimulator::isPaused() const {
    return isPaused_.load();
}

void BrainSimulator::runCustomSimulation(const CustomSimulationConfig& config) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot run custom simulation: no brain reference");
        return;
    }
    
    // Store current state for restoration
    auto originalState = captureState();
    
    // Apply custom configuration
    // Note: This would need access to brain configuration system
    
    // Run simulation loop
    runSimulationLoop(config);
    
    // Restore original state
    restoreState(originalState);
}

BrainSimulator::SimulationState BrainSimulator::getSimulationState() const {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    return captureState();
}

void BrainSimulator::restoreSimulationState(const SimulationState& state) {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    restoreState(state);
}

void BrainSimulator::setStepCallback(std::function<void(BrainSimulator&, SimulationStep, double)> callback) {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    stepCallback_ = std::move(callback);
}

void BrainSimulator::setCompletionCallback(std::function<void(BrainSimulator&, const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    completionCallback_ = std::move(callback);
}

void BrainSimulator::setErrorCallback(std::function<void(BrainSimulator&, const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    errorCallback_ = std::move(callback);
}

void BrainSimulator::setMetricsCallback(std::function<void(BrainSimulator&, const SimulationState&)> callback) {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    metricsCallback_ = std::move(callback);
}

double BrainSimulator::getAverageStepTime() const {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    return avgStepTime_;
}

double BrainSimulator::getRealTimeFactor() const {
    return realTimeFactor_;
}

size_t BrainSimulator::getStepsPerSecond() const {
    return stepsPerSecond_;
}

double BrainSimulator::getSimulatedTimePerRealTimeSecond() const {
    return static_cast<double>(totalStepsExecuted_) / std::max<size_t>(1, stepsCompleted_);
}

size_t BrainSimulator::getCurrentStep() const {
    return currentStep_;
}

double BrainSimulator::getCurrentTime() const {
    return currentTime_;
}

BrainSimulator::SimulationStep BrainSimulator::getCurrentSimulationStep() const {
    return static_cast<SimulationStep>(currentStep_);
}

void BrainSimulator::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (brain_) {
        brain_->injectCurrent(neuron, current);
    }
}

void BrainSimulator::injectCurrentToType(NeuronType type, MembranePotential current) {
    if (brain_) {
        brain_->injectCurrentToNeurons(type, current);
    }
}

void BrainSimulator::injectCurrentToRegion(RegionId region, MembranePotential current) {
    if (brain_) {
        auto regionPtr = brain_->getRegion(region);
        if (regionPtr) {
            for (const auto& population : regionPtr->getPopulations()) {
                for (Neuron* neuron : population->getNeurons()) {
                    if (neuron) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    }
}

void BrainSimulator::injectSensoryInput(const class SensoryInput& input) {
    if (brain_) {
        brain_->receiveSensoryInput(input);
    }
}

void BrainSimulator::adjustWeight(SynapseId synapse, float weightDelta) {
    // Find and adjust synapse weight
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            if (syn->getId() == synapse) {
                float newWeight = syn->getWeight() + weightDelta;
                // Note: This would require synapse weight modification interface
                break;
            }
        }
    }
}

void BrainSimulator::setWeight(SynapseId synapse, float weight) {
    // Set synapse weight directly
}

void BrainSimulator::adjustThreshold(NeuronId neuron, float thresholdDelta) {
    // Find and adjust neuron threshold
}

void BrainSimulator::setThreshold(NeuronId neuron, float threshold) {
    // Set neuron threshold directly
}

void BrainSimulator::tunePlasticity(PlasticityId plasticityId, const PlasticityParameters& params) {
    // Access plasticity system through brain
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        // Configure STDP parameters
    }
}

void BrainSimulator::modifyNeuromodulator(NeuromodulatorId neuromodulator, float level) {
    // Access neuromodulation system
    brain_->applyNeuromodulation(class Neuromodulator{});
}

void BrainSimulator::setDevelopmentStage(DevelopmentalStage stage) {
    // Note: This would require access to development system
}

void BrainSimulator::snapshot(const std::string& name) {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    
    std::string snapshotName = name.empty() ? "auto_" + std::to_string(snapshotHistory_.size()) : name;
    
    snapshots_[snapshotName] = captureState();
    snapshotHistory_.push_back(snapshotName);
    
    // Limit history size
    if (snapshotHistory_.size() > 100) {
        std::string oldest = snapshotHistory_.front();
        snapshotHistory_.pop_front();
        snapshots_.erase(oldest);
    }
    
    NLM_LOG_INFO("Snapshot created: " + snapshotName);
}

bool BrainSimulator::restoreSnapshot(const std::string& name) {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    
    auto it = snapshots_.find(name);
    if (it == snapshots_.end()) {
        NLM_LOG_ERROR("Snapshot not found: " + name);
        return false;
    }
    
    restoreState(it->second);
    NLM_LOG_INFO("Snapshot restored: " + name);
    return true;
}

std::vector<std::string> BrainSimulator::listSnapshots() const {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    std::vector<std::string> names;
    names.reserve(snapshots_.size());
    
    for (const auto& pair : snapshots_) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool BrainSimulator::deleteSnapshot(const std::string& name) {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    
    auto it = snapshots_.find(name);
    if (it == snapshots_.end()) {
        return false;
    }
    
    // Remove from history
    snapshotHistory_.erase(std::remove(snapshotHistory_.begin(), snapshotHistory_.end(), name), snapshotHistory_.end());
    
    // Remove from map
    snapshots_.erase(it);
    
    NLM_LOG_INFO("Snapshot deleted: " + name);
    return true;
}

bool BrainSimulator::attachDebugger(std::shared_ptr<BrainDebugger> debugger) {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    
    if (debugger) {
        debugger_ = debugger;
        NLM_LOG_INFO("BrainDebugger attached to BrainSimulator");
        return true;
    }
    
    return false;
}

void BrainSimulator::lockSimulation() const {
    simulationMutex_.lock();
}

void BrainSimulator::unlockSimulation() const {
    simulationMutex_.unlock();
}

bool BrainSimulator::tryLockSimulation() const {
    return simulationMutex_.try_lock();
}

void BrainSimulator::runSimulationLoop(const CustomSimulationConfig& config) {
    isRunning_ = true;
    isPaused_ = false;
    
    size_t stepsToRun = config.maxSteps > 0 ? config.maxSteps : std::numeric_limits<size_t>::max();
    double timestep = config.timestep > 0.0 ? config.timestep : 0.01;
    
    // Initialize if callback provided
    if (config.initializationCallback) {
        config.initializationCallback(*this);
    }
    
    NLM_LOG_INFO("Custom simulation loop started");
    
    for (currentStep_ = 0; currentStep_ < stepsToRun && isRunning_; ++currentStep_) {
        // Check for pause
        if (isPaused_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        double stepStart = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        
        // Execute simulation step
        executeStep(timestep);
        
        currentTime_ += timestep;
        
        // Call step callback if provided
        if (config.stepCallback) {
            config.stepCallback(*this, currentStep_, currentTime_);
        }
        
        // Call metrics callback if provided
        if (metricsCallback_) {
            metricsCallback_(*this, captureState());
        }
        
        // Log step if enabled
        if (loggingEnabled_) {
            logStep(captureState());
        }
        
        // Calculate and update real time factor
        double stepEnd = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        double stepDuration = stepEnd - stepStart;
        
        std::lock_guard<std::mutex> lock(simulationMutex_);
        avgStepTime_ = 0.9 * avgStepTime_ + 0.1 * stepDuration;
        stepsPerSecond_ = static_cast<size_t>(1.0 / std::max<double>(stepDuration, 0.001));
        
        // Sleep to maintain real-time factor
        double targetStepTime = 1.0 / realTimeFactor_;
        if (stepDuration < targetStepTime) {
            double sleepTime = targetStepTime - stepDuration;
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
    }
    
    // Simulation completed
    isRunning_ = false;
    isPaused_ = false;
    
    // Call completion callback if provided
    if (config.completionCallback) {
        std::string reason = (currentStep_ >= stepsToRun) ? "completed max steps" : "stopped";
        config.completionCallback(*this, reason);
    }
    
    NLM_LOG_INFO("Custom simulation loop completed");
}

void BrainSimulator::executeStep(double timestep) {
    if (!brain_) return;
    
    // Record current state before step
    auto stateBefore = captureState();
    
    // Execute brain step
    brain_->step(currentStep_, currentTime_);
    
    // Update current time
    currentTime_ += timestep;
    currentStep_++;
    
    // Log performance metrics
    updatePerformanceMetrics();
    
    NLM_LOG_DEBUG("Simulation step " + std::to_string(currentStep_) + " completed, time=" + 
                 std::to_string(currentTime_) + "s");
}

BrainSimulator::SimulationState BrainSimulator::captureState() const {
    SimulationState state;
    
    if (!brain_) return state;
    
    state.step = currentStep_;
    state.time = currentTime_;
    state.wallClockTime = std::chrono::duration<double>::clock::now().time_since_epoch().count() - wallClockStart_;
    state.isRunning = isRunning_.load();
    state.isPaused = isPaused_.load();
    
    // Brain statistics
    state.activeNeuronCount = brain_->getActiveNeuronCount();
    state.firingNeuronCount = brain_->getFiringNeuronCount();
    state.averageFiringRate = brain_->getAverageFiringRate();
    
    // Calculate learning progress and stability
    state.learningProgress = 0.5f; // Would need learning system access
    state.stabilityIndex = 1.0f; // Would need stability analysis
    
    // Total reward from neuromodulation
    state.totalReward = 0.0f; // Would need reward tracking
    
    // Performance metrics
    state.performanceMetrics.push_back(avgStepTime_ * 1000.0); // Convert to ms
    state.performanceMetrics.push_back(getRealTimeFactor());
    state.performanceMetrics.push_back(static_cast<float>(getStepsPerSecond()));
    
    return state;
}

void BrainSimulator::restoreState(const SimulationState& state) {
    if (!brain_) return;
    
    // Restore simulation state
    currentStep_ = state.step;
    currentTime_ = state.time;
    
    // Note: This would need access to brain state restoration functionality
    // For now, just log the restoration
    
    NLM_LOG_INFO("Simulation state restored: step=" + std::to_string(state.step) + 
                 ", time=" + std::to_string(state.time) + "s");
}

void BrainSimulator::logStep(const SimulationState& state) {
    // Log step information to file
    // Note: This would need file logging infrastructure
    
    std::stringstream logLine;
    logLine << std::fixed << std::setprecision(3)
            << "Step: " << state.step 
            << " Time: " << state.time 
            << " Neurons: " << state.activeNeuronCount 
            << "/" << state.firingNeuronCount 
            << " Firing: " << state.averageFiringRate 
            << " Learning: " << state.learningProgress 
            << " Stability: " << state.stabilityIndex << std::endl;
    
    // Note: This would write to log file in real implementation
    NLM_LOG_DEBUG(std::string("Simulation step log: ") + logLine.str());
}

void BrainSimulator::updatePerformanceMetrics() {
    // Update performance metrics for real-time monitoring
    // Note: This would update performance dashboard if available
    
    // This is a placeholder for performance metric updates
    // In a real implementation, this would update performance monitoring systems
    NLM_LOG_DEBUG("Performance metrics updated");
}

void BrainSimulator::reset() {
    std::lock_guard<std::mutex> lock(simulationMutex_);
    
    currentStep_ = 0;
    currentTime_ = 0.0;
    wallClockStart_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    lastStepTime_ = 0.0;
    stepsCompleted_ = 0;
    totalStepsExecuted_ = 0;
    
    NLM_LOG_INFO("BrainSimulator state reset");
}
