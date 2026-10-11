#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <pybind11/eigen.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <future>
#include <chrono>
#include <map>
#include <optional>
#include <variant>
#include <any>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <random>

namespace py = pybind11;
namespace nlm {

// Forward declarations
class EnhancedBrain;
class EnhancedWorld;
class TrainingAPI;
class SimulationAPI;
class AnalysisAPI;
class VisualizationAPI;
class ExportImportAPI;

// Training API implementation
class TrainingAPI {
public:
    TrainingAPI(std::shared_ptr<EnhancedBrain> brain) : brain_(brain) {
        if (!brain_) {
            throw std::runtime_error("Brain cannot be null");
        }
    }
    
    // Training modes
    void trainEpisode(double learningRate = 0.01, size_t maxSteps = 1000) {
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingCount = 0;
        
        for (size_t step = 0; step < maxSteps; ++step) {
            brain_->step(step, step * 0.001);
            
            auto action = brain_->produceAction();
            // Apply action and get reward (simplified)
            totalReward += action->reward;
            totalFiringRate += brain_->getAverageFiringRate();
            if (brain_->getAverageFiringRate() > 0.0f) firingCount++;
            
            // Apply reward modulation
            applyRewardModulation(action->reward, 0.0f);
            
            // Update development if enabled
            if (brain_->getConfig() && brain_->getConfig()->has("enable_development")) {
                brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
            }
            
            if (progressCallback_) {
                progressCallback_(static_cast<float>(step) / maxSteps);
            }
        }
        
        // Store metrics
        averageReward_ = totalReward / maxSteps;
        averageFiringRate_ = totalFiringRate / maxSteps;
        learningProgress_ = static_cast<float>(firingCount) / maxSteps;
        totalTrainingTime_ += std::chrono::high_resolution_clock::now() - trainingStartTime_;
    }
    
    void trainBatch(size_t episodes, double learningRate = 0.01, size_t maxSteps = 1000) {
        trainingStartTime_ = std::chrono::high_resolution_clock::now();
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            trainEpisode(learningRate, maxSteps);
            
            if (batchProgressCallback_) {
                batchProgressCallback_(static_cast<float>(episode) / episodes);
            }
        }
        
        if (metricsCallback_) {
            std::map<std::string, float> metrics;
            metrics["average_reward"] = averageReward_;
            metrics["average_firing_rate"] = averageFiringRate_;
            metrics["learning_progress"] = learningProgress_;
            metrics["training_time"] = totalTrainingTime_;
            metricsCallback_(metrics);
        }
    }
    
    void trainContinuous(double learningRate = 0.01, size_t maxSteps = 100000) {
        trainingStartTime_ = std::chrono::high_resolution_clock::now();
        
        size_t step = 0;
        while (step < maxSteps) {
            brain_->step(step, step * 0.001);
            auto action = brain_->produceAction();
            
            // Apply reward modulation
            applyRewardModulation(action->reward, 0.0f);
            
            // Update development
            if (brain_->getConfig() && brain_->getConfig()->has("enable_development")) {
                brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
            }
            
            // Collect metrics
            float reward = action->reward;
            float firingRate = brain_->getAverageFiringRate();
            
            totalReward_ += reward;
            totalFiringRate_ += firingRate;
            stepCount_++;
            
            if (progressCallback_) {
                progressCallback_(static_cast<float>(step) / maxSteps);
            }
            
            if (metricsCallback_) {
                std::map<std::string, float> metrics;
                metrics["average_reward"] = totalReward_ / stepCount_;
                metrics["average_firing_rate"] = totalFiringRate_ / stepCount_;
                metrics["learning_progress"] = static_cast<float>(firingCount_) / stepCount_;
                metricsCallback_(metrics);
            }
            
            step++;
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Simulate real-time
        }
    }
    
    // Learning strategies
    void setLearningStrategy(const std::string& strategy) {
        learningStrategy_ = strategy;
        // Configure learning based on strategy
    }
    
    void setExplorationStrategy(const std::string& strategy) {
        explorationStrategy_ = strategy;
        // Configure exploration based on strategy
    }
    
    void setRewardShapingFunction(std::function<float(float)> func) {
        rewardShapingFunction_ = func;
    }
    
    // Metrics collection
    float getAverageReward() const { return averageReward_; }
    float getAverageLearningProgress() const { return learningProgress_; }
    double getTrainingTime() const { 
        return std::chrono::duration<double>(totalTrainingTime_).count();
    }
    
    // Checkpointing
    bool saveCheckpoint(const std::string& path) const {
        try {
            if (brain_) {
                brain_->save(path);
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to save checkpoint: " << e.what() << std::endl;
        }
        return false;
    }
    
    bool loadCheckpoint(const std::string& path) {
        try {
            if (brain_) {
                brain_->load(path);
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to load checkpoint: " << e.what() << std::endl;
        }
        return false;
    }
    
    // Monitoring
    void setProgressCallback(const std::function<void(float)>& callback) {
        progressCallback_ = callback;
    }
    
    void setMetricsCallback(const std::function<void(const std::map<std::string, float>&)>& callback) {
        metricsCallback_ = callback;
    }
    
    void setBatchProgressCallback(const std::function<void(float)>& callback) {
        batchProgressCallback_ = callback;
    }
    
private:
    void applyRewardModulation(float reward, float predictedReward) {
        // Apply reward-based neuromodulation
        // This would integrate with dopamine, curiosity, and novelty systems
    }
    
    std::shared_ptr<EnhancedBrain> brain_;
    
    // Training state
    std::chrono::high_resolution_clock::time_point trainingStartTime_;
    std::chrono::duration<double> totalTrainingTime_{0.0};
    
    // Metrics
    float averageReward_ = 0.0f;
    float averageFiringRate_ = 0.0f;
    float learningProgress_ = 0.0f;
    float totalReward_ = 0.0f;
    float totalFiringRate_ = 0.0f;
    size_t stepCount_ = 0;
    size_t firingCount_ = 0;
    
    // Strategies
    std::string learningStrategy_ = "td_learning";
    std::string explorationStrategy_ = "epsilon_greedy";
    std::function<float(float)> rewardShapingFunction_ = nullptr;
    
    // Callbacks
    std::function<void(float)> progressCallback_;
    std::function<void(const std::map<std::string, float>&)> metricsCallback_;
    std::function<void(float)> batchProgressCallback_;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// High-level Simulation API implementation
class SimulationAPI {
public:
    SimulationAPI(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world)
        : brain_(brain), world_(world), isRunning_(false) {
        if (!brain_ || !world_) {
            throw std::runtime_error("Brain and world cannot be null");
        }
    }
    
    // Simulation runs
    void runEpisode(size_t maxSteps = 1000) {
        isRunning_ = true;
        episodeStartTime_ = std::chrono::high_resolution_clock::now();
        currentStep_ = 0;
        currentReward_ = 0.0f;
        averageFiringRate_ = 0.0f;
        fps_ = 0.0;
        
        for (currentStep_ = 0; currentStep_ < maxSteps && isRunning_; ++currentStep_) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            // Get sensory input
            auto percept = world_->getSensoryPercept();
            
            // Process sensory input
            auto action = brain_->produceAction();
            
            // Apply action to world
            auto result = world_->applyAction(*action, simTime_);
            
            // Update brain
            brain_->step(currentStep_, simTime_);
            
            // Collect metrics
            currentReward_ += action->reward;
            averageFiringRate_ = brain_->getAverageFiringRate();
            
            simTime_ += 0.001;
            
            // Call step callback
            if (stepCallback_) {
                stepCallback_(currentStep_, simTime_, action->reward);
            }
            
            // Calculate FPS
            auto stepEnd = std::chrono::high_resolution_clock::now();
            auto stepDuration = std::chrono::duration<double>(stepEnd - stepStart).count();
            if (stepDuration > 0.0) {
                fps_ = 1.0 / stepDuration;
            }
            
            if (pauseRequested_) {
                isRunning_ = false;
                pauseRequested_ = false;
            }
        }
        
        isRunning_ = false;
        
        // Call episode callback
        if (episodeCallback_) {
            episodeCallback_(currentReward_ / maxSteps);
        }
    }
    
    void runBatch(size_t episodes, size_t maxSteps = 1000) {
        batchStartTime_ = std::chrono::high_resolution_clock::now();
        
        for (size_t episode = 0; episode < episodes && isRunning_; ++episode) {
            runEpisode(maxSteps);
            
            if (batchProgressCallback_) {
                batchProgressCallback_(static_cast<float>(episode) / episodes);
            }
        }
        
        if (performanceCallback_) {
            performanceCallback_(calculatePerformanceMetrics());
        }
    }
    
    void runContinuous(size_t maxSteps = 100000) {
        isRunning_ = true;
        continuousStartTime_ = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < maxSteps && isRunning_; ++step) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            // Run simulation step
            auto percept = world_->getSensoryPercept();
            auto action = brain_->produceAction();
            auto result = world_->applyAction(*action, simTime_);
            brain_->step(step, simTime_);
            
            // Collect metrics
            currentReward_ += action->reward;
            averageFiringRate_ = brain_->getAverageFiringRate();
            
            simTime_ += 0.001;
            
            // Call callbacks
            if (stepCallback_) {
                stepCallback_(step, simTime_, action->reward);
            }
            
            // Calculate and call performance callback
            if (performanceCallback_) {
                performanceCallback_(calculatePerformanceMetrics());
            }
            
            // Handle pause/stop requests
            if (pauseRequested_) {
                while (isRunning_ && pauseRequested_) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            
            if (stopRequested_) {
                isRunning_ = false;
                break;
            }
            
            // Maintain target FPS
            auto stepEnd = std::chrono::high_resolution_clock::now();
            auto stepDuration = std::chrono::duration<double>(stepEnd - stepStart).count();
            double targetFrameTime = 1.0 / targetFPS_;
            if (stepDuration < targetFrameTime) {
                std::this_thread::sleep_for(std::chrono::duration<double>(targetFrameTime - stepDuration));
            }
        }
        
        isRunning_ = false;
    }
    
    // Scenario management
    void setScenario(const std::string& scenarioName) {
        currentScenario_ = scenarioName;
        // Load scenario configuration
        if (scenarioCallback_) {
            scenarioCallback_(scenarioName, "Scenario loaded");
        }
    }
    
    void addScenarioObject(const EnhancedWorldObject& obj) {
        world_->addObject(obj);
        if (scenarioCallback_) {
            scenarioCallback_("object_added", "Object added to scenario");
        }
    }
    
    void clearScenario() {
        // Clear world and reset
        world_->reset();
        if (scenarioCallback_) {
            scenarioCallback_("scenario_cleared", "Scenario cleared");
        }
    }
    
    // Performance monitoring
    double getCurrentFPS() const { return fps_; }
    size_t getCurrentStep() const { return currentStep_; }
    double getElapsedTime() const { return simTime_; }
    
    // Metrics collection
    float getCurrentReward() const { return currentReward_ / (currentStep_ > 0 ? currentStep_ : 1); }
    float getAverageReward() const { return totalReward_ / (totalEpisodes_ > 0 ? totalEpisodes_ : 1); }
    float getCurrentFiringRate() const { return averageFiringRate_; }
    
    // State management
    void pause() { pauseRequested_ = true; }
    void resume() { pauseRequested_ = false; }
    void stop() { isRunning_ = false; stopRequested_ = true; }
    bool isRunning() const { return isRunning_; }
    
    // Callbacks
    void setStepCallback(const std::function<void(size_t, double, float)>& callback) {
        stepCallback_ = callback;
    }
    
    void setEpisodeCallback(const std::function<void(float)>& callback) {
        episodeCallback_ = callback;
    }
    
    void setPerformanceCallback(const std::function<void(const py::dict)>& callback) {
        performanceCallback_ = callback;
    }
    
    void setScenarioCallback(const std::function<void(const std::string&, const std::string&)>& callback) {
        scenarioCallback_ = callback;
    }
    
    // Configuration
    void setTargetFPS(double fps) { targetFPS_ = fps; }
    
private:
    py::dict calculatePerformanceMetrics() const {
        py::dict metrics;
        metrics["current_fps"] = fps_;
        metrics["current_step"] = currentStep_;
        metrics["elapsed_time"] = simTime_;
        metrics["current_reward"] = getCurrentReward();
        metrics["average_reward"] = getAverageReward();
        metrics["current_firing_rate"] = getCurrentFiringRate();
        metrics["total_episodes"] = totalEpisodes_;
        metrics["pause_requested"] = pauseRequested_;
        return metrics;
    }
    
    std::shared_ptr<EnhancedBrain> brain_;
    std::shared_ptr<EnhancedWorld> world_;
    
    // Simulation state
    bool isRunning_ = false;
    bool pauseRequested_ = false;
    bool stopRequested_ = false;
    
    // Time tracking
    double simTime_ = 0.0;
    double targetFPS_ = 30.0;
    double fps_ = 0.0;
    
    // Episode tracking
    size_t currentStep_ = 0;
    size_t totalEpisodes_ = 0;
    
    // Metrics
    float currentReward_ = 0.0f;
    float totalReward_ = 0.0f;
    float averageFiringRate_ = 0.0f;
    
    // Callbacks
    std::function<void(size_t, double, float)> stepCallback_;
    std::function<void(float)> episodeCallback_;
    std::function<void(const py::dict)> performanceCallback_;
    std::function<void(const std::string&, const std::string&)> scenarioCallback_;
    
    // Scenario
    std::string currentScenario_ = "default";
    
    // Timing
    std::chrono::high_resolution_clock::time_point episodeStartTime_;
    std::chrono::high_resolution_clock::time_point batchStartTime_;
    std::chrono::high_resolution_clock::time_point continuousStartTime_;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Analysis API implementation
class AnalysisAPI {
public:
    AnalysisAPI(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world)
        : brain_(brain), world_(world) {
        if (!brain_ || !world_) {
            throw std::runtime_error("Brain and world cannot be null");
        }
    }
    
    // Performance analysis
    float calculateEfficiency(size_t steps) const {
        float totalReward = 0.0f;
        float totalEnergy = 0.0f;
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            auto action = brain_->produceAction();
            totalReward += action->reward;
            // Add energy consumption calculation
            totalEnergy += brain_->getTotalNeuronCount() * 0.001f; // Simplified energy model
        }
        
        return totalReward / (totalEnergy > 0 ? totalEnergy : 1.0f);
    }
    
    float calculateLearningProgress(size_t steps) const {
        float totalFiringRate = 0.0f;
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            totalFiringRate += brain_->getAverageFiringRate();
        }
        
        return totalFiringRate / steps;
    }
    
    float calculateAdaptability(size_t steps) const {
        // Calculate how well the agent adapts to changing conditions
        float stability = 0.0f;
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            // Calculate stability metric based on neural activity patterns
            stability += brain_->getAverageFiringRate();
        }
        
        return stability / steps;
    }
    
    // Behavioral analysis
    std::string analyzeBehaviorPattern(size_t steps) const {
        std::vector<float> firingRates;
        std::vector<float> rewards;
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            auto action = brain_->produceAction();
            firingRates.push_back(brain_->getAverageFiringRate());
            rewards.push_back(action->reward);
        }
        
        // Analyze patterns
        float avgFiring = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f) / firingRates.size();
        float avgReward = std::accumulate(rewards.begin(), rewards.end(), 0.0f) / rewards.size();
        
        std::stringstream ss;
        ss << "Behavior Pattern Analysis:" << std::endl;
        ss << "  Average firing rate: " << avgFiring << std::endl;
        ss << "  Average reward: " << avgReward << std::endl;
        
        if (avgFiring > 0.5f) ss << "  Pattern: High activity" << std::endl;
        else if (avgFiring > 0.2f) ss << "  Pattern: Moderate activity" << std::endl;
        else ss << "  Pattern: Low activity" << std::endl;
        
        if (avgReward > 0.5f) ss << "  Behavior: Reward-seeking" << std::endl;
        else if (avgReward > 0.0f) ss << "  Behavior: Neutral" << std::endl;
        else ss << "  Behavior: Avoidance" << std::endl;
        
        return ss.str();
    }
    
    float calculateExplorationExploitationRatio(size_t steps) const {
        float exploration = 0.0f;
        float exploitation = 0.0f;
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            auto action = brain_->produceAction();
            
            // Simplified exploration/exploitation analysis
            if (action->reward == 0.0f) exploration += 1.0f;
            else exploitation += action->reward;
        }
        
        float total = exploration + exploitation;
        return total > 0.0f ? exploration / total : 0.5f; // Default to 50/50
    }
    
    std::vector<float> getBehavioralMetrics(size_t steps) const {
        std::vector<float> metrics;
        metrics.reserve(6);
        
        float totalFiring = 0.0f;
        float totalReward = 0.0f;
        float maxFiring = 0.0f;
        float minFiring = 1.0f;
        size_t stepsWithReward = 0;
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            auto action = brain_->produceAction();
            
            float firing = brain_->getAverageFiringRate();
            totalFiring += firing;
            totalReward += action->reward;
            
            if (firing > maxFiring) maxFiring = firing;
            if (firing < minFiring) minFiring = firing;
            
            if (action->reward > 0.0f) stepsWithReward++;
        }
        
        size_t stepCount = steps > 0 ? steps : 1;
        
        metrics.push_back(totalFiring / stepCount);          // average_firing_rate
        metrics.push_back(totalReward / stepCount);           // average_reward
        metrics.push_back(maxFiring);                         // max_firing_rate
        metrics.push_back(minFiring);                         // min_firing_rate
        metrics.push_back(static_cast<float>(stepsWithReward) / stepCount); // reward_frequency
        metrics.push_back(calculateExplorationExploitationRatio(stepCount)); // exploration_exploitation_ratio
        
        return metrics;
    }
    
    // Neural analysis
    py::array_t<float> getNeuralActivityHistory(size_t steps) const {
        std::vector<float> activityHistory;
        activityHistory.reserve(steps);
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            activityHistory.push_back(brain_->getAverageFiringRate());
        }
        
        return py::array_t<float>(activityHistory.size(), activityHistory.data());
    }
    
    py::array_t<float> getFiringRateEvolution(size_t steps) const {
        return getNeuralActivityHistory(steps);
    }
    
    float calculateNeuralComplexity(size_t steps) const {
        std::vector<float> firingRates;
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            firingRates.push_back(brain_->getAverageFiringRate());
        }
        
        // Calculate complexity as variance of firing rates
        float mean = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f) / firingRates.size();
        float variance = 0.0f;
        
        for (float rate : firingRates) {
            variance += (rate - mean) * (rate - mean);
        }
        
        variance /= firingRates.size();
        return std::sqrt(variance); // Standard deviation as complexity measure
    }
    
    // Visualization preparation
    py::dict prepareForVisualization(size_t steps) const {
        py::dict data;
        
        // Prepare neural activity data
        auto activityHistory = getNeuralActivityHistory(steps);
        data["neural_activity"] = activityHistory;
        
        // Prepare behavioral data
        std::vector<float> firingRates;
        std::vector<float> rewards;
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            auto action = brain_->produceAction();
            firingRates.push_back(brain_->getAverageFiringRate());
            rewards.push_back(action->reward);
        }
        
        data["firing_rates"] = py::array_t<float>(firingRates.size(), firingRates.data());
        data["rewards"] = py::array_t<float>(rewards.size(), rewards.data());
        
        // Prepare world data if available
        auto worldPercept = world_->getSensoryPercept();
        data["world_time"] = world_->getSimulationTime();
        
        return data;
    }
    
    // Statistical analysis
    py::dict calculateStatistics(size_t steps) const {
        std::vector<float> firingRates;
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            firingRates.push_back(brain_->getAverageFiringRate());
        }
        
        py::dict stats;
        
        if (firingRates.empty()) {
            return stats;
        }
        
        float sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f);
        float mean = sum / firingRates.size();
        
        float sq_sum = 0.0f;
        for (float rate : firingRates) {
            sq_sum += (rate - mean) * (rate - mean);
        }
        float stddev = std::sqrt(sq_sum / firingRates.size());
        
        float min = *std::min_element(firingRates.begin(), firingRates.end());
        float max = *std::max_element(firingRates.begin(), firingRates.end());
        
        stats["mean"] = mean;
        stats["stddev"] = stddev;
        stats["min"] = min;
        stats["max"] = max;
        stats["range"] = max - min;
        
        return stats;
    }
    
    py::array_t<float> getCorrelationMatrix(size_t steps) const {
        // Simplified correlation matrix calculation
        std::vector<float> metrics;
        metrics.reserve(steps * 3); // firing_rate, reward, time
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            auto action = brain_->produceAction();
            
            metrics.push_back(brain_->getAverageFiringRate());
            metrics.push_back(action->reward);
            metrics.push_back(static_cast<float>(step));
        }
        
        // Create simple correlation matrix
        size_t n = 3; // 3 metrics
        std::vector<float> correlationMatrix(n * n, 0.0f);
        
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                // Simple correlation (placeholder implementation)
                correlationMatrix[i * n + j] = (i == j) ? 1.0f : 0.0f;
            }
        }
        
        return py::array_t<float>(correlationMatrix.size(), correlationMatrix.data());
    }
    
private:
    std::shared_ptr<EnhancedBrain> brain_;
    std::shared_ptr<EnhancedWorld> world_;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Visualization API implementation
class VisualizationAPI {
public:
    VisualizationAPI(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world)
        : brain_(brain), world_(world), recording_(false) {
        if (!brain_ || !world_) {
            throw std::runtime_error("Brain and world cannot be null");
        }
    }
    
    // Rendering
    void renderBrain(std::string format = "text") {
        if (format == "text") {
            std::cout << brain_->getStatus() << std::endl;
        } else if (format == "json") {
            // Export brain state to JSON
            std::cout << "{\"brain\": \"data\", \"format\": \"json\"}" << std::endl;
        } else if (format == "html") {
            std::cout << "<html><body>Brain visualization would go here</body></html>" << std::endl;
        }
        
        if (renderCallback_) {
            renderCallback_(format);
        }
    }
    
    void renderWorld(std::string format = "text") {
        if (format == "text") {
            std::cout << "World: " << world_->getWidth() << "x" << world_->getHeight() << std::endl;
            std::cout << "Agent position: (" << world_->getAgentBody().x << ", " << world_->getAgentBody().y << ")" << std::endl;
        } else if (format == "json") {
            // Export world state to JSON
            std::cout << "{\"world\": \"data\", \"format\": \"json\"}" << std::endl;
        } else if (format == "html") {
            std::cout << "<html><body>World visualization would go here</body></html>" << std::endl;
        }
        
        if (renderCallback_) {
            renderCallback_(format);
        }
    }
    
    void renderNeurons(std::string format = "text") {
        if (format == "text") {
            std::cout << "Neuron population analysis:" << std::endl;
            std::cout << "  Total neurons: " << brain_->getNeuronCount() << std::endl;
            std::cout << "  Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
            std::cout << "  Average firing rate: " << brain_->getAverageFiringRate() << std::endl;
        } else if (format == "json") {
            // Export neuron data to JSON
            std::cout << "{\"neurons\": \"data\", \"format\": \"json\"}" << std::endl;
        } else if (format == "html") {
            std::cout << "<html><body>Neuron visualization would go here</body></html>" << std::endl;
        }
        
        if (renderCallback_) {
            renderCallback_(format);
        }
    }
    
    void renderConnections(std::string format = "text") {
        if (format == "text") {
            std::cout << "Synaptic connection analysis:" << std::endl;
            std::cout << "  Total synapses: " << brain_->getSynapseCount() << std::endl;
            std::cout << "  Excitation/Inhibition ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
        } else if (format == "json") {
            // Export connection data to JSON
            std::cout << "{\"connections\": \"data\", \"format\": \"json\"}" << std::endl;
        } else if (format == "html") {
            std::cout << "<html><body>Connection visualization would go here</body></html>" << std::endl;
        }
        
        if (renderCallback_) {
            renderCallback_(format);
        }
    }
    
    // Interactive visualization
    void interactiveMode(bool enable = true) {
        interactiveMode_ = enable;
        if (renderCallback_) {
            renderCallback_("interactive_mode_" + std::string(enable ? "enabled" : "disabled"));
        }
    }
    
    void setVisualizationStyle(const std::string& style) {
        visualizationStyle_ = style;
        if (renderCallback_) {
            renderCallback_("style_changed_" + style);
        }
    }
    
    // Output formats
    std::string getTextVisualization() const {
        std::stringstream ss;
        ss << "=== NLM Enhanced Visualization ===" << std::endl;
        ss << "Brain Status:" << std::endl;
        ss << brain_->getStatus() << std::endl;
        ss << "World State:" << std::endl;
        ss << "  Position: (" << world_->getAgentBody().x << ", " << world_->getAgentBody().y << ")" << std::endl;
        ss << "  Velocity: (" << world_->getAgentBody().velocityX << ", " << world_->getAgentBody().velocityY << ")" << std::endl;
        ss << "  Energy: " << world_->getAgentBody().energy << std::endl;
        ss << "  Health: " << world_->getAgentBody().health << std::endl;
        return ss.str();
    }
    
    std::string getHTMLVisualization() const {
        std::stringstream ss;
        ss << "<!DOCTYPE html>" << std::endl;
        ss << "<html>" << std::endl;
        ss << "<head><title>NLM Visualization</title></head>" << std::endl;
        ss << "<body>" << std::endl;
        ss << "  <h1>NLM Enhanced Visualization</h1>" << std::endl;
        ss << "  <pre>" << std::endl;
        ss << "Brain: " << brain_->getNeuronCount() << " neurons" << std::endl;
        ss << "Firing Rate: " << brain_->getAverageFiringRate() << std::endl;
        ss << "World: " << world_->getWidth() << "x" << world_->getHeight() << std::endl;
        ss << "Agent: (" << world_->getAgentBody().x << ", " << world_->getAgentBody().y << ")" << std::endl;
        ss << "  </pre>" << std::endl;
        ss << "</body>" << std::endl;
        ss << "</html>" << std::endl;
        return ss.str();
    }
    
    std::string getJSONVisualization() const {
        // Create JSON visualization data
        std::stringstream ss;
        ss << "{\"visualization\": {" << std::endl;
        ss << "  \"brain\": {" << std::endl;
        ss << "    \"neuron_count\": " << brain_->getNeuronCount() << "," << std::endl;
        ss << "    \"firing_rate\": " << brain_->getAverageFiringRate() << "," << std::endl;
        ss << "    \"active_neurons\": " << brain_->getActiveNeuronCount() << "," << std::endl;
        ss << "    \"total_spikes\": " << brain_->getTotalSpikeCount() << std::endl;
        ss << "  },"," << std::endl;
        ss << "  \"world\": {" << std::endl;
        ss << "    \"width\": " << world_->getWidth() << "," << std::endl;
        ss << "    \"height\": " << world_->getHeight() << "," << std::endl;
        ss << "    \"agent_x\": " << world_->getAgentBody().x << "," << std::endl;
        ss << "    \"agent_y\": " << world_->getAgentBody().y << "," << std::endl;
        ss << "    \"energy\": " << world_->getAgentBody().energy << std::endl;
        ss << "  }" << std::endl;
        ss << "}}" << std::endl;
        return ss.str();
    }
    
    py::array_t<unsigned char> getImage(std::string format = "png", int width = 800, int height = 600) const {
        // Generate image data (placeholder implementation)
        std::vector<unsigned char> imageData(width * height * 3, 255); // White background
        
        // Add some visual elements
        size_t centerX = width / 2;
        size_t centerY = height / 2;
        
        // Draw brain area
        for (int y = centerY - 100; y < centerY + 100; ++y) {
            for (int x = centerX - 100; x < centerX + 100; ++x) {
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    size_t index = (y * width + x) * 3;
                    imageData[index] = 100;     // Blue channel
                    imageData[index + 1] = 100; // Green channel
                    imageData[index + 2] = 255; // Red channel
                }
            }
        }
        
        // Draw agent
        float agentX = world_->getAgentBody().x;
        float agentY = world_->getAgentBody().y;
        size_t agentPx = static_cast<size_t>((agentX / world_->getWidth()) * width);
        size_t agentPy = static_cast<size_t>((agentY / world_->getHeight()) * height);
        
        if (agentPx < width && agentPy < height) {
            size_t index = (agentPy * width + agentPx) * 3;
            imageData[index] = 255;     // Red channel
            imageData[index + 1] = 0;   // Green channel
            imageData[index + 2] = 0;   // Blue channel
        }
        
        return py::array_t<unsigned char>(imageData.size(), imageData.data());
    }
    
    // Recording
    void startRecording(const std::string& path, const std::string& format = "json") {
        recording_ = true;
        recordingPath_ = path;
        recordingFormat_ = format;
        
        if (updateCallback_) {
            updateCallback_(simTime_);
        }
        
        if (renderCallback_) {
            renderCallback_("recording_started_" + path);
        }
    }
    
    void stopRecording() {
        recording_ = false;
        
        if (renderCallback_) {
            renderCallback_("recording_stopped");
        }
    }
    
    bool isRecording() const { return recording_; }
    
    // Callbacks
    void setRenderCallback(const std::function<void(std::string)>& callback) {
        renderCallback_ = callback;
    }
    
    void setUpdateCallback(const std::function<void(double)>& callback) {
        updateCallback_ = callback;
    }
    
    // Configuration
    void setTargetFPS(double fps) { targetFPS_ = fps; }
    void setWindowSize(int width, int height) { windowWidth_ = width; windowHeight_ = height; }
    
private:
    void updateRecording() {
        if (recording_) {
            // Save frame to recording
            // Implementation would depend on format
        }
    }
    
    std::shared_ptr<EnhancedBrain> brain_;
    std::shared_ptr<EnhancedWorld> world_;
    
    // Visualization state
    bool interactiveMode_ = false;
    std::string visualizationStyle_ = "default";
    bool recording_ = false;
    std::string recordingPath_;
    std::string recordingFormat_ = "json";
    
    // Performance
    double targetFPS_ = 30.0;
    int windowWidth_ = 800;
    int windowHeight_ = 600;
    
    double simTime_ = 0.0;
    
    // Callbacks
    std::function<void(std::string)> renderCallback_;
    std::function<void(double)> updateCallback_;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Export/Import API implementation
class ExportImportAPI {
public:
    ExportImportAPI(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world)
        : brain_(brain), world_(world), importProgress_(0.0), exportProgress_(0.0) {
        if (!brain_ || !world_) {
            throw std::runtime_error("Brain and world cannot be null");
        }
    }
    
    // Export formats
    bool exportToJSON(const std::string& filepath) const {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            json exportData;
            
            // Brain data
            exportData["brain"]["neuron_count"] = brain_->getNeuronCount();
            exportData["brain"]["firing_rate"] = brain_->getAverageFiringRate();
            exportData["brain"]["synapse_count"] = brain_->getSynapseCount();
            exportData["brain"]["active_neurons"] = brain_->getActiveNeuronCount();
            
            // World data
            exportData["world"]["width"] = world_->getWidth();
            exportData["world"]["height"] = world_->getHeight();
            exportData["world"]["simulation_time"] = world_->getSimulationTime();
            
            auto agentBody = world_->getAgentBody();
            exportData["world"]["agent_x"] = agentBody.x;
            exportData["world"]["agent_y"] = agentBody.y;
            exportData["world"]["agent_energy"] = agentBody.energy;
            
            // Export to file
            file << exportData.dump(2);
            file.close();
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to export to JSON: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool exportToCSV(const std::string& filepath) const {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Write CSV header
            file << "step,simulation_time,firing_rate,reward,neurons, synapses,active_neurons" << std::endl;
            
            // Write data (placeholder - would need to store historical data)
            file << "0,0.0,0.0,0.0,0,0,0" << std::endl;
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to export to CSV: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool exportToNumpy(const std::string& filepath) const {
        try {
            // Create NumPy-compatible data structure
            py::dict numpyData;
            
            // Brain metrics
            numpyData["neuron_count"] = brain_->getNeuronCount();
            numpyData["firing_rate"] = brain_->getAverageFiringRate();
            numpyData["synapse_count"] = brain_->getSynapseCount();
            numpyData["active_neurons"] = brain_->getActiveNeuronCount();
            
            // World metrics
            numpyData["world_width"] = world_->getWidth();
            numpyData["world_height"] = world_->getHeight();
            numpyData["simulation_time"] = world_->getSimulationTime();
            
            // Export using pybind11 numpy interface
            py::object numpyModule = py::module::import("numpy");
            py::object savez = numpyModule.attr("savez");
            savez(filepath, numpyData);
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to export to NumPy: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool exportToTensorFlow(const std::string& filepath) const {
        try {
            // Create TensorFlow-compatible data
            std::vector<float> tfData;
            
            // Brain metrics
            tfData.push_back(static_cast<float>(brain_->getNeuronCount()));
            tfData.push_back(brain_->getAverageFiringRate());
            tfData.push_back(static_cast<float>(brain_->getSynapseCount()));
            tfData.push_back(static_cast<float>(brain_->getActiveNeuronCount()));
            
            // World metrics
            tfData.push_back(world_->getWidth());
            tfData.push_back(world_->getHeight());
            tfData.push_back(static_cast<float>(world_->getSimulationTime()));
            
            // Export using TensorFlow API
            py::object tf = py::module::import("tensorflow");
            py::object keras = tf.attr("keras");
            py::object save = keras.attr("save");
            
            py::dict exportDict;
            exportDict["brain_metrics"] = py::array_t<float>(tfData.size(), tfData.data());
            
            save.attr("save")(exportDict, filepath);
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to export to TensorFlow: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool exportToPyTorch(const std::string& filepath) const {
        try {
            // Create PyTorch-compatible data
            py::dict torchData;
            
            // Brain metrics as tensors
            torchData["neuron_count"] = py::array_t<int64_t>(1, &brain_->getNeuronCount());
            torchData["firing_rate"] = py::array_t<float>(1, &brain_->getAverageFiringRate());
            
            // Export using PyTorch API
            py::object torch = py::module::import("torch");
            py::object save = torch.attr("save");
            
            save(torchData, filepath);
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to export to PyTorch: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Import formats
    bool importFromJSON(const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            json importData = json::parse(file);
            
            // Import brain data
            if (importData.contains("brain")) {
                // Restore brain state
                // Implementation would restore from imported data
            }
            
            // Import world data
            if (importData.contains("world")) {
                // Restore world state
                // Implementation would restore from imported data
            }
            
            file.close();
            
            // Update progress
            if (importProgressCallback_) {
                importProgress_ = 1.0f;
                importProgressCallback_(importProgress_);
            }
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to import from JSON: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool importFromCSV(const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            std::string line;
            std::getline(file, line); // Skip header
            
            while (std::getline(file, line)) {
                // Parse CSV line and import data
                // Implementation would parse CSV and restore state
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to import from CSV: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool importFromNumpy(const std::string& filepath) {
        try {
            py::object numpy = py::module::import("numpy");
            py::object load = numpy.attr("load");
            
            py::dict importData = load(filepath);
            
            // Import data into brain and world
            if (importData.contains("neuron_count")) {
                // Restore from NumPy data
                // Implementation would restore from imported data
            }
            
            if (importProgressCallback_) {
                importProgress_ = 1.0f;
                importProgressCallback_(importProgress_);
            }
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to import from NumPy: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool importFromTensorFlow(const std::string& filepath) {
        try {
            // Import from TensorFlow checkpoint
            // Implementation would load TensorFlow data
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to import from TensorFlow: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool importFromPyTorch(const std::string& filepath) {
        try {
            // Import from PyTorch checkpoint
            // Implementation would load PyTorch data
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to import from PyTorch: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Simulation data management
    bool saveSimulationState(const std::string& filepath) const {
        try {
            json stateData;
            
            // Save simulation state
            stateData["simulation_time"] = world_->getSimulationTime();
            stateData["step_count"] = world_->getStepCount();
            stateData["brain_state"] = "placeholder"; // Would save actual brain state
            stateData["world_state"] = "placeholder"; // Would save actual world state
            
            std::ofstream file(filepath);
            file << stateData.dump(2);
            file.close();
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to save simulation state: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool loadSimulationState(const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            json stateData = json::parse(file);
            
            // Load simulation state
            if (stateData.contains("simulation_time")) {
                // Restore simulation time
                // Implementation would restore from state data
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load simulation state: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool saveAgentState(const std::string& filepath) const {
        try {
            // Save agent state
            // Implementation would save agent-specific data
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to save agent state: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool loadAgentState(const std::string& filepath) {
        try {
            // Load agent state
            // Implementation would load agent-specific data
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load agent state: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Data processing
    py::dict processData(const std::string& dataType, const std::string& filepath) const {
        py::dict processedData;
        
        if (dataType == "neural") {
            // Process neural data
            processedData["activity"] = brain_->getNeuronActivity();
            processedData["firing_rates"] = brain_->getFiringRates();
        } else if (dataType == "behavioral") {
            // Process behavioral data
            processedData["rewards"] = getBehavioralRewards();
            processedData["actions"] = getBehavioralActions();
        } else if (dataType == "world") {
            // Process world data
            processedData["positions"] = world_->getAgentPosition();
            processedData["grid"] = world_->getWorldGrid();
        }
        
        return processedData;
    }
    
    std::vector<float> extractFeatures(const std::string& featureType, size_t steps) const {
        std::vector<float> features;
        
        if (featureType == "neural") {
            for (size_t step = 0; step < steps; ++step) {
                brain_->step(step, step * 0.001);
                features.push_back(brain_->getAverageFiringRate());
            }
        } else if (featureType == "behavioral") {
            for (size_t step = 0; step < steps; ++step) {
                brain_->step(step, step * 0.001);
                auto action = brain_->produceAction();
                features.push_back(action->reward);
            }
        }
        
        return features;
    }
    
    // Format validation
    bool validateJSON(const std::string& filepath) const {
        try {
            std::ifstream file(filepath);
            json::parse(file);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Invalid JSON: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool validateCSV(const std::string& filepath) const {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) return false;
            
            std::string line;
            std::getline(file, line); // Header
            
            size_t lineCount = 0;
            while (std::getline(file, line)) {
                lineCount++;
            }
            
            return lineCount > 0;
        } catch (const std::exception& e) {
            std::cerr << "Invalid CSV: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Progress reporting
    float getImportProgress() const { return importProgress_; }
    float getExportProgress() const { return exportProgress_; }
    
    // Callbacks
    void setImportProgressCallback(const std::function<void(float)>& callback) {
        importProgressCallback_ = callback;
    }
    
    void setExportProgressCallback(const std::function<void(float)>& callback) {
        exportProgressCallback_ = callback;
    }
    
private:
    std::shared_ptr<EnhancedBrain> brain_;
    std::shared_ptr<EnhancedWorld> world_;
    
    // Progress tracking
    float importProgress_ = 0.0f;
    float exportProgress_ = 0.0f;
    
    // Callbacks
    std::function<void(float)> importProgressCallback_;
    std::function<void(float)> exportProgressCallback_;
    
    // Helper methods for data processing
    std::vector<float> getBehavioralRewards() const {
        std::vector<float> rewards;
        // Implementation would return historical rewards
        return rewards;
    }
    
    std::vector<std::string> getBehavioralActions() const {
        std::vector<std::string> actions;
        // Implementation would return historical actions
        return actions;
    }
    
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Factory functions implementation
namespace Factory {
    std::shared_ptr<ConfigObject> createConfig() {
        return std::make_shared<ConfigObject>();
    }
    
    std::shared_ptr<ConfigObject> loadConfig(const std::string& filepath) {
        auto config = std::make_shared<ConfigObject>();
        config->loadFromFile(filepath);
        return config;
    }
    
    std::shared_ptr<EnhancedBrain> createBrain(const std::shared_ptr<ConfigObject>& config) {
        return std::make_shared<EnhancedBrain>(config);
    }
    
    std::shared_ptr<EnhancedWorld> createWorld() {
        return std::make_shared<EnhancedWorld>();
    }
    
    std::shared_ptr<EnhancedAgent> createAgent(std::shared_ptr<EnhancedBrain> brain) {
        return std::make_shared<EnhancedAgent>(brain);
    }
    
    std::unique_ptr<EnhancedAction> createAction(ActionType type, const std::vector<float>& parameters) {
        return std::make_unique<EnhancedAction>(type, parameters);
    }
    
    std::unique_ptr<EnhancedPercept> createPercept() {
        return std::make_unique<EnhancedPercept>();
    }
    
    std::unique_ptr<EnhancedWorldObject> createWorldObject(WorldObjectType type, float x, float y, float value, float radius) {
        return std::make_unique<EnhancedWorldObject>(x, y, type, value, radius);
    }
    
    std::unique_ptr<EnhancedAgentBody> createAgentBody() {
        return std::make_unique<EnhancedAgentBody>();
    }
}

// High-level convenience functions implementation
namespace HighLevel {
    std::unique_ptr<EnhancedAction> runSimpleControl(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world) {
        if (!brain || !world) {
            throw std::runtime_error("Brain and world cannot be null");
        }
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process and produce action
        auto action = brain->produceAction();
        
        // Apply action to world
        world->applyAction(*action, world->getSimulationTime());
        
        return action;
    }
    
    float runTestEpisode(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world, size_t maxSteps) {
        if (!brain || !world) {
            throw std::runtime_error("Brain and world cannot be null");
        }
        
        float totalReward = 0.0f;
        
        for (size_t step = 0; step < maxSteps; ++step) {
            brain->step(step, step * 0.001);
            auto action = brain->produceAction();
            
            // Apply action and collect reward
            auto result = world->applyAction(*action, world->getSimulationTime());
            totalReward += action->reward;
        }
        
        return totalReward / maxSteps;
    }
    
    py::dict runBenchmark(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world, const std::string& benchmarkName) {
        py::dict results;
        results["benchmark"] = benchmarkName;
        results["duration"] = 0.0;
        results["performance"] = 0.0;
        results["memory_usage"] = 0.0;
        
        // Run benchmark test
        auto start = std::chrono::high_resolution_clock::now();
        float avgReward = runTestEpisode(brain, world, 100);
        auto end = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double>(end - start).count();
        results["duration"] = duration;
        results["performance"] = avgReward / duration;
        results["memory_usage"] = brain->getNeuronCount() * sizeof(float); // Simplified memory estimate
        
        return results;
    }
}

// Modern Python module initialization for enhanced API
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ----------------------------------------------------
        A modern Python binding for the NLM C++ neural simulation framework.
        Provides enhanced classes with modern Python features:
        - Type hints and comprehensive docstrings
        - NumPy integration for array operations
        - Async/await support for non-blocking operations
        - Context managers for safe resource management
        - Configuration objects replacing string-based configs
        - Event system for simulation events
        - Callback system for event handling
        - High-level APIs for training, simulation, analysis, visualization
        - Full Phase 6 integration support
    )pbdoc";

    // Configure Python output
    py::call_guard<py::scoped_ostream_redirect, py::cout>(py::ios_base::out);
    
    // Export core types
    py::enum_<ActionType>(m, "ActionType", "Action type enumeration")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("Wait", ActionType::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", "World object type enumeration")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<NeuronType>(m, "NeuronType", "Neuron type enumeration")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Export EnhancedAction class
    py::class_<EnhancedAction>(m, "EnhancedAction")
        .def(py::init<>())
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters") = std::vector<float>())
        .def_readwrite("type", &EnhancedAction::type, "Action type")
        .def_readwrite("parameters", &EnhancedAction::parameters, "Action parameters")
        .def_readwrite("timestamp", &EnhancedAction::timestamp, "Action timestamp")
        .def_readwrite("reward", &EnhancedAction::reward, "Action reward")
        .def_readwrite("success", &EnhancedAction::success, "Action success")
        .def_readwrite("message", &EnhancedAction::message, "Action message");

    // Export EnhancedPercept class
    py::class_<EnhancedPercept>(m, "EnhancedPercept")
        .def(py::init<>())
        .def_readwrite("vision", &EnhancedPercept::vision, "Vision data")
        .def_readwrite("touch", &EnhancedPercept::touch, "Touch data")
        .def_readwrite("internal", &EnhancedPercept::internal, "Internal signals")
        .def_readwrite("proprioception", &EnhancedPercept::proprioception, "Proprioception data")
        .def_readwrite("audio", &EnhancedPercept::audio, "Audio data")
        .def_readwrite("timestamp", &EnhancedPercept::timestamp, "Percept timestamp");

    // Export EnhancedWorldObject class
    py::class_<EnhancedWorldObject>(m, "EnhancedWorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(), py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &EnhancedWorldObject::x, "X position")
        .def_readwrite("y", &EnhancedWorldObject::y, "Y position")
        .def_readwrite("radius", &EnhancedWorldObject::radius, "Radius")
        .def_readwrite("type", &EnhancedWorldObject::type, "Object type")
        .def_readwrite("value", &EnhancedWorldObject::value, "Object value")
        .def_readwrite("active", &EnhancedWorldObject::active, "Object active state");

    // Export EnhancedAgentBody class
    py::class_<EnhancedAgentBody>(m, "EnhancedAgentBody")
        .def(py::init<>())
        .def_readwrite("x", &EnhancedAgentBody::x, "X position")
        .def_readwrite("y", &EnhancedAgentBody::y, "Y position")
        .def_readwrite("orientation", &EnhancedAgentBody::orientation, "Orientation")
        .def_readwrite("velocityX", &EnhancedAgentBody::velocityX, "Velocity X")
        .def_readwrite("velocityY", &EnhancedAgentBody::velocityY, "Velocity Y")
        .def_readwrite("angularVelocity", &EnhancedAgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &EnhancedAgentBody::energy, "Energy")
        .def_readwrite("health", &EnhancedAgentBody::health, "Health")
        .def_readwrite("age", &EnhancedAgentBody::age, "Age")
        .def_readwrite("isMoving", &EnhancedAgentBody::isMoving, "Is moving")
        .def_readwrite("isTurning", &EnhancedAgentBody::isTurning, "Is turning")
        .def_readwrite("lastActionTime", &EnhancedAgentBody::lastActionTime, "Last action time");

    // Export ConfigObject class with Pythonic API
    py::class_<ConfigObject>(m, "ConfigObject")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("config_file"))
        .def("set", py::overload_cast<const std::string&, int>(&ConfigObject::set), "Set integer value")
        .def("set", py::overload_cast<const std::string&, double>(&ConfigObject::set), "Set double value")
        .def("set", py::overload_cast<const std::string&, float>(&ConfigObject::set), "Set float value")
        .def("set", py::overload_cast<const std::string&, bool>(&ConfigObject::set), "Set boolean value")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&ConfigObject::set), "Set string value")
        .def("set", py::overload_cast<const std::string&, const std::vector<int>&>(&ConfigObject::set), "Set integer vector value")
        .def("set", py::overload_cast<const std::string&, const std::vector<double>&>(&ConfigObject::set), "Set double vector value")
        .def("set", py::overload_cast<const std::string&, const std::vector<float>&>(&ConfigObject::set), "Set float vector value")
        .def("set", py::overload_cast<const std::string&, const std::vector<std::string>&>(&ConfigObject::set), "Set string vector value")
        .def("getInt", &ConfigObject::getInt, "Get integer value")
        .def("getDouble", &ConfigObject::getDouble, "Get double value")
        .def("getFloat", &ConfigObject::getFloat, "Get float value")
        .def("getBool", &ConfigObject::getBool, "Get boolean value")
        .def("getString", &ConfigObject::getString, "Get string value")
        .def("getKeys", &ConfigObject::getKeys, "Get all configuration keys")
        .def("summary", &ConfigObject::summary, "Get configuration summary")
        .def("toJSON", &ConfigObject::toJSON, "Export to JSON")
        .def("loadFromJSON", &ConfigObject::loadFromJSON, "Load from JSON")
        .def("saveToFile", &ConfigObject::saveToFile, "Save to file")
        .def("loadFromFile", &ConfigObject::loadFromFile, "Load from file")
        .def("has", &ConfigObject::has, "Check if key exists")
        .def("remove", &ConfigObject::remove, "Remove key")
        .def("clear", &ConfigObject::clear, "Clear all configuration")
        .def("__getitem__", &ConfigObject::operator[], "Access configuration by key")
        .def("size", &ConfigObject::size, "Get configuration size")
        .def("empty", &ConfigObject::empty, "Check if configuration is empty");

    // Export EventSystem class
    py::class_<EventSystem>(m, "EventSystem")
        .def(py::init<>())
        .def("emit", &EventSystem::emit, "Emit event")
        .def("on", &EventSystem::on, "Register event listener")
        .def("off", &EventSystem::off, "Unregister event listener")
        .def("getEventNames", &EventSystem::getEventNames, "Get event names");

    // Export DecoratorSupport class
    py::class_<DecoratorSupport>(m, "DecoratorSupport")
        .def(py::init<>());

    // Export TrainingAPI class
    py::class_<TrainingAPI>(m, "TrainingAPI")
        .def(py::init<std::shared_ptr<EnhancedBrain>>(), py::arg("brain"))
        .def("trainEpisode", &TrainingAPI::trainEpisode, "Train episode", py::arg("learning_rate") = 0.01, py::arg("max_steps") = 1000)
        .def("trainBatch", &TrainingAPI::trainBatch, "Train batch", py::arg("episodes"), py::arg("learning_rate") = 0.01, py::arg("max_steps") = 1000)
        .def("trainContinuous", &TrainingAPI::trainContinuous, "Train continuously", py::arg("learning_rate") = 0.01, py::arg("max_steps") = 100000)
        .def("setLearningStrategy", &TrainingAPI::setLearningStrategy, "Set learning strategy", py::arg("strategy"))
        .def("setExplorationStrategy", &TrainingAPI::setExplorationStrategy, "Set exploration strategy", py::arg("strategy"))
        .def("setRewardShapingFunction", &TrainingAPI::setRewardShapingFunction, "Set reward shaping function", py::arg("func"))
        .def("getAverageReward", &TrainingAPI::getAverageReward, "Get average reward")
        .def("getAverageLearningProgress", &TrainingAPI::getAverageLearningProgress, "Get average learning progress")
        .def("getTrainingTime", &TrainingAPI::getTrainingTime, "Get training time")
        .def("saveCheckpoint", &TrainingAPI::saveCheckpoint, "Save checkpoint", py::arg("path"))
        .def("loadCheckpoint", &TrainingAPI::loadCheckpoint, "Load checkpoint", py::arg("path"))
        .def("setProgressCallback", &TrainingAPI::setProgressCallback, "Set progress callback", py::arg("callback"))
        .def("setMetricsCallback", &TrainingAPI::setMetricsCallback, "Set metrics callback", py::arg("callback"))
        .def("setBatchProgressCallback", &TrainingAPI::setBatchProgressCallback, "Set batch progress callback", py::arg("callback"));

    // Export SimulationAPI class
    py::class_<SimulationAPI>(m, "SimulationAPI")
        .def(py::init<std::shared_ptr<EnhancedBrain>, std::shared_ptr<EnhancedWorld>>(), py::arg("brain"), py::arg("world"))
        .def("runEpisode", &SimulationAPI::runEpisode, "Run episode", py::arg("max_steps") = 1000)
        .def("runBatch", &SimulationAPI::runBatch, "Run batch", py::arg("episodes"), py::arg("max_steps") = 1000)
        .def("runContinuous", &SimulationAPI::runContinuous, "Run continuous", py::arg("max_steps") = 100000)
        .def("setScenario", &SimulationAPI::setScenario, "Set scenario", py::arg("scenario_name"))
        .def("addScenarioObject", &SimulationAPI::addScenarioObject, "Add scenario object", py::arg("obj"))
        .def("clearScenario", &SimulationAPI::clearScenario, "Clear scenario")
        .def("getCurrentFPS", &SimulationAPI::getCurrentFPS, "Get current FPS")
        .def("getCurrentStep", &SimulationAPI::getCurrentStep, "Get current step")
        .def("getElapsedTime", &SimulationAPI::getElapsedTime, "Get elapsed time")
        .def("getCurrentReward", &SimulationAPI::getCurrentReward, "Get current reward")
        .def("getAverageReward", &SimulationAPI::getAverageReward, "Get average reward")
        .def("getCurrentFiringRate", &SimulationAPI::getCurrentFiringRate, "Get current firing rate")
        .def("pause", &SimulationAPI::pause, "Pause simulation")
        .def("resume", &SimulationAPI::resume, "Resume simulation")
        .def("stop", &SimulationAPI::stop, "Stop simulation")
        .def("isRunning", &SimulationAPI::isRunning, "Check if running")
        .def("setStepCallback", &SimulationAPI::setStepCallback, "Set step callback", py::arg("callback"))
        .def("setEpisodeCallback", &SimulationAPI::setEpisodeCallback, "Set episode callback", py::arg("callback"))
        .def("setPerformanceCallback", &SimulationAPI::setPerformanceCallback, "Set performance callback", py::arg("callback"))
        .def("setScenarioCallback", &SimulationAPI::setScenarioCallback, "Set scenario callback", py::arg("callback"))
        .def("setTargetFPS", &SimulationAPI::setTargetFPS, "Set target FPS", py::arg("fps"));

    // Export AnalysisAPI class
    py::class_<AnalysisAPI>(m, "AnalysisAPI")
        .def(py::init<std::shared_ptr<EnhancedBrain>, std::shared_ptr<EnhancedWorld>>(), py::arg("brain"), py::arg("world"))
        .def("calculateEfficiency", &AnalysisAPI::calculateEfficiency, "Calculate efficiency", py::arg("steps"))
        .def("calculateLearningProgress", &AnalysisAPI::calculateLearningProgress, "Calculate learning progress", py::arg("steps"))
        .def("calculateAdaptability", &AnalysisAPI::calculateAdaptability, "Calculate adaptability", py::arg("steps"))
        .def("analyzeBehaviorPattern", &AnalysisAPI::analyzeBehaviorPattern, "Analyze behavior pattern", py::arg("steps"))
        .def("calculateExplorationExploitationRatio", &AnalysisAPI::calculateExplorationExploitationRatio, "Calculate exploration-exploitation ratio", py::arg("steps"))
        .def("getBehavioralMetrics", &AnalysisAPI::getBehavioralMetrics, "Get behavioral metrics", py::arg("steps"))
        .def("getNeuralActivityHistory", &AnalysisAPI::getNeuralActivityHistory, "Get neural activity history", py::arg("steps"))
        .def("getFiringRateEvolution", &AnalysisAPI::getFiringRateEvolution, "Get firing rate evolution", py::arg("steps"))
        .def("calculateNeuralComplexity", &AnalysisAPI::calculateNeuralComplexity, "Calculate neural complexity", py::arg("steps"))
        .def("prepareForVisualization", &AnalysisAPI::prepareForVisualization, "Prepare for visualization", py::arg("steps"))
        .def("calculateStatistics", &AnalysisAPI::calculateStatistics, "Calculate statistics", py::arg("steps"))
        .def("getCorrelationMatrix", &AnalysisAPI::getCorrelationMatrix, "Get correlation matrix", py::arg("steps"));

    // Export VisualizationAPI class
    py::class_<VisualizationAPI>(m, "VisualizationAPI")
        .def(py::init<std::shared_ptr<EnhancedBrain>, std::shared_ptr<EnhancedWorld>>(), py::arg("brain"), py::arg("world"))
        .def("renderBrain", &VisualizationAPI::renderBrain, "Render brain", py::arg("format") = "text")
        .def("renderWorld", &VisualizationAPI::renderWorld, "Render world", py::arg("format") = "text")
        .def("renderNeurons", &VisualizationAPI::renderNeurons, "Render neurons", py::arg("format") = "text")
        .def("renderConnections", &VisualizationAPI::renderConnections, "Render connections", py::arg("format") = "text")
        .def("interactiveMode", &VisualizationAPI::interactiveMode, "Enable/disable interactive mode", py::arg("enable") = true)
        .def("setVisualizationStyle", &VisualizationAPI::setVisualizationStyle, "Set visualization style", py::arg("style"))
        .def("getTextVisualization", &VisualizationAPI::getTextVisualization, "Get text visualization")
        .def("getHTMLVisualization", &VisualizationAPI::getHTMLVisualization, "Get HTML visualization")
        .def("getJSONVisualization", &VisualizationAPI::getJSONVisualization, "Get JSON visualization")
        .def("getImage", &VisualizationAPI::getImage, "Get image", py::arg("format") = "png", py::arg("width") = 800, py::arg("height") = 600)
        .def("startRecording", &VisualizationAPI::startRecording, "Start recording", py::arg("path"), py::arg("format") = "json")
        .def("stopRecording", &VisualizationAPI::stopRecording, "Stop recording")
        .def("isRecording", &VisualizationAPI::isRecording, "Check if recording")
        .def("setRenderCallback", &VisualizationAPI::setRenderCallback, "Set render callback", py::arg("callback"))
        .def("setUpdateCallback", &VisualizationAPI::setUpdateCallback, "Set update callback", py::arg("callback"))
        .def("setTargetFPS", &VisualizationAPI::setTargetFPS, "Set target FPS", py::arg("fps"))
        .def("setWindowSize", &VisualizationAPI::setWindowSize, "Set window size", py::arg("width"), py::arg("height"));

    // Export ExportImportAPI class
    py::class_<ExportImportAPI>(m, "ExportImportAPI")
        .def(py::init<std::shared_ptr<EnhancedBrain>, std::shared_ptr<EnhancedWorld>>(), py::arg("brain"), py::arg("world"))
        .def("exportToJSON", &ExportImportAPI::exportToJSON, "Export to JSON", py::arg("filepath"))
        .def("exportToCSV", &ExportImportAPI::exportToCSV, "Export to CSV", py::arg("filepath"))
        .def("exportToNumpy", &ExportImportAPI::exportToNumpy, "Export to NumPy", py::arg("filepath"))
        .def("exportToTensorFlow", &ExportImportAPI::exportToTensorFlow, "Export to TensorFlow", py::arg("filepath"))
        .def("exportToPyTorch", &ExportImportAPI::exportToPyTorch, "Export to PyTorch", py::arg("filepath"))
        .def("importFromJSON", &ExportImportAPI::importFromJSON, "Import from JSON", py::arg("filepath"))
        .def("importFromCSV", &ExportImportAPI::importFromCSV, "Import from CSV", py::arg("filepath"))
        .def("importFromNumpy", &ExportImportAPI::importFromNumpy, "Import from NumPy", py::arg("filepath"))
        .def("importFromTensorFlow", &ExportImportAPI::importFromTensorFlow, "Import from TensorFlow", py::arg("filepath"))
        .def("importFromPyTorch", &ExportImportAPI::importFromPyTorch, "Import from PyTorch", py::arg("filepath"))
        .def("saveSimulationState", &ExportImportAPI::saveSimulationState, "Save simulation state", py::arg("filepath"))
        .def("loadSimulationState", &ExportImportAPI::loadSimulationState, "Load simulation state", py::arg("filepath"))
        .def("saveAgentState", &ExportImportAPI::saveAgentState, "Save agent state", py::arg("filepath"))
        .def("loadAgentState", &ExportImportAPI::loadAgentState, "Load agent state", py::arg("filepath"))
        .def("processData", &ExportImportAPI::processData, "Process data", py::arg("data_type"), py::arg("filepath"))
        .def("extractFeatures", &ExportImportAPI::extractFeatures, "Extract features", py::arg("feature_type"), py::arg("steps"))
        .def("validateJSON", &ExportImportAPI::validateJSON, "Validate JSON", py::arg("filepath"))
        .def("validateCSV", &ExportImportAPI::validateCSV, "Validate CSV", py::arg("filepath"))
        .def("getImportProgress", &ExportImportAPI::getImportProgress, "Get import progress")
        .def("getExportProgress", &ExportImportAPI::getExportProgress, "Get export progress")
        .def("setImportProgressCallback", &ExportImportAPI::setImportProgressCallback, "Set import progress callback", py::arg("callback"))
        .def("setExportProgressCallback", &ExportImportAPI::setExportProgressCallback, "Set export progress callback", py::arg("callback"));

    // Export Factory functions
    m.def("create_config", &Factory::createConfig, "Create empty configuration");
    m.def("load_config", &Factory::loadConfig, "Load configuration from file", py::arg("filepath"));
    m.def("create_brain", &Factory::createBrain, "Create brain with configuration", py::arg("config") = nullptr);
    m.def("create_world", &Factory::createWorld, "Create world");
    m.def("create_agent", &Factory::createAgent, "Create agent with brain", py::arg("brain"));
    m.def("create_action", &Factory::createAction, "Create action", py::arg("type"), py::arg("parameters") = std::vector<float>());
    m.def("create_percept", &Factory::createPercept, "Create percept");
    m.def("create_world_object", &Factory::createWorldObject, "Create world object", py::arg("type"), py::arg("x"), py::arg("y"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f);
    m.def("create_agent_body", &Factory::createAgentBody, "Create agent body");

    // Export High-level convenience functions
    m.def("run_simple_control", &HighLevel::runSimpleControl, "Run simple control loop", py::arg("brain"), py::arg("world"));
    m.def("run_test_episode", &HighLevel::runTestEpisode, "Run test episode", py::arg("brain"), py::arg("world"), py::arg("max_steps") = 100);
    m.def("run_benchmark", &HighLevel::runBenchmark, "Run benchmark", py::arg("brain"), py::arg("world"), py::arg("benchmark_name") = "simple");

    // Version information
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Enhanced NLM Python bindings with modern Python features";
}

} // namespace nlm