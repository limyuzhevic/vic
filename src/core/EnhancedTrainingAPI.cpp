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
class EventSystem;
class DecoratorSupport;

// Enhanced Brain implementation with modern features
class EnhancedBrain {
public:
    EnhancedBrain(const std::shared_ptr<ConfigObject>& config = nullptr) : config_(config) {
        initialize();
    }
    
    // Context manager support
    EnhancedBrain& initialize() {
        if (!brain_) {
            brain_ = std::make_shared<Brain>(config_ ? config_->getSharedPtr() : nullptr);
            if (!brain_->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
        }
        return *this;
    }
    
    void step(size_t step, double time = 0.0) {
        brain_->step(static_cast<SimulationStep>(step), time);
    }
    
    std::unique_ptr<EnhancedAction> produceAction() {
        auto action = brain_->produceAction();
        auto result = std::make_unique<EnhancedAction>();
        result->type = action->getType();
        result->parameters = action->getParameters();
        result->timestamp = action->getTimestamp();
        result->reward = 0.0f; // Will be set by world
        result->success = true;
        result->message = "Action produced";
        return result;
    }
    
    void reset() {
        if (brain_) brain_->reset();
    }
    
    void save(const std::string& filepath) const {
        if (brain_) brain_->save(filepath);
    }
    
    void load(const std::string& filepath) {
        if (brain_) brain_->load(filepath);
    }
    
    // Modern property system
    std::string getStatus() const {
        std::stringstream ss;
        ss << "Brain Status:" << std::endl;
        ss << "  Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        ss << "  Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        ss << "  Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
        ss << "  Firing rate: " << brain_->getAverageFiringRate() << std::endl;
        ss << "  Developmental stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
        return ss.str();
    }
    
    size_t getNeuronCount() const { return brain_->getTotalNeuronCount(); }
    size_t getSynapseCount() const { return brain_->getTotalSynapseCount(); }
    size_t getActiveNeuronCount() const { return brain_->getActiveNeuronCount(); }
    float getAverageFiringRate() const { return brain_->getAverageFiringRate(); }
    float getExcitationInhibitionRatio() const { return brain_->getExcitationInhibitionRatio(); }
    size_t getTotalSpikeCount() const { return brain_->getTotalSpikeCount(); }
    
    // NumPy integration
    py::array_t<float> getNeuronActivity() const {
        // Return neuron activity as NumPy array
        std::vector<float> activity;
        // Implementation would extract activity from brain
        return py::array_t<float>(activity.size(), activity.data());
    }
    
    py::array_t<float> getFiringRates() const {
        // Return firing rates as NumPy array
        std::vector<float> rates;
        // Implementation would extract firing rates from brain
        return py::array_t<float>(rates.size(), rates.data());
    }
    
    py::array_t<float> getWeights() const {
        // Return synaptic weights as NumPy array
        std::vector<float> weights;
        // Implementation would extract weights from brain
        return py::array_t<float>(weights.size(), weights.data());
    }
    
    // Memory systems access
    std::shared_ptr<WorkingMemory> getWorkingMemory() const {
        return brain_ ? brain_->getWorkingMemory() : nullptr;
    }
    
    std::shared_ptr<EpisodicMemory> getEpisodicMemory() const {
        return brain_ ? brain_->getEpisodicMemory() : nullptr;
    }
    
    std::shared_ptr<AssociativeMemory> getAssociativeMemory() const {
        return brain_ ? brain_->getAssociativeMemory() : nullptr;
    }
    
    // Neuromodulation systems
    float getDopamineLevel() const {
        auto* dopamine = brain_ ? brain_->getDopamine() : nullptr;
        return dopamine ? dopamine->getLevel() : 0.0f;
    }
    
    float getCuriosityLevel() const {
        auto* curiosity = brain_ ? brain_->getCuriosity() : nullptr;
        return curiosity ? curiosity->getLevel() : 0.0f;
    }
    
    float getNoveltyLevel() const {
        auto* novelty = brain_ ? brain_->getNovelty() : nullptr;
        return novelty ? novelty->getLevel() : 0.0f;
    }
    
    float getPredictionError() const {
        auto* error = brain_ ? brain_->getPredictionErrorSignal() : nullptr;
        return error ? error->getError() : 0.0f;
    }
    
    // Development system
    DevelopmentalStage getDevelopmentalStage() const {
        return brain_ ? brain_->getDevelopmentalStage() : DevelopmentalStage::Adult;
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        if (brain_) brain_->setDevelopmentalStage(stage);
    }
    
    // Cognitive systems
    std::string getPlannerOutput() const {
        auto* planner = brain_ ? brain_->getPlanner() : nullptr;
        return planner ? planner->getPlan() : "No planner available";
    }
    
    std::string getConceptSummary() const {
        auto* conceptFormation = brain_ ? brain_->getConceptFormation() : nullptr;
        return conceptFormation ? conceptFormation->getConceptSummary() : "No concepts formed";
    }
    
    std::string getAttentionFocus() const {
        auto* attention = brain_ ? brain_->getAttention() : nullptr;
        return attention ? attention->getFocus() : "No attention focus";
    }
    
    // Configuration access
    std::shared_ptr<const ConfigObject> getConfig() const { return config_; }
    
    // Async support
    std::future<void> stepAsync(size_t step, double time = 0.0) {
        return std::async(std::launch::async, [this, step, time]() {
            this->step(step, time);
        });
    }
    
    std::future<std::unique_ptr<EnhancedAction>> produceActionAsync() {
        return std::async(std::launch::async, [this]() {
            return this->produceAction();
        });
    }
    
    // Python iteration support
    EnhancedBrainIterator begin() const {
        return EnhancedBrainIterator(brain_->getRegions());
    }
    
    EnhancedBrainIterator end() const {
        return EnhancedBrainIterator(brain_->getRegions(), brain_->getRegions().size());
    }
    
    // Property access for regions
    std::vector<RegionId> getRegionIds() const { return brain_->getRegionIds(); }
    std::shared_ptr<NeuralRegion> getRegion(RegionId id) const { return brain_->getRegion(id); }
    
    // Event system integration
    void connectEventCallback(const std::string& eventName, const std::function<void(const std::any&)>& callback) {
        if (!eventSystem_) eventSystem_ = std::make_unique<TypedEventSystem>();
        // Implementation would connect callback to specific event channel
    }
    
    // Modern C++ features
    EnhancedBrain& operator=(const EnhancedBrain&) = delete;
    EnhancedBrain(const EnhancedBrain&) = delete;
    EnhancedBrain(EnhancedBrain&&) noexcept = default;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<ConfigObject> config_;
    std::unique_ptr<TypedEventSystem> eventSystem_;
};

// Modern World class with improved API
class EnhancedWorld {
public:
    EnhancedWorld() {
        width_ = 16.0f;
        height_ = 16.0f;
        maxEnergy_ = 100.0f;
        energyDecayRate_ = 0.01f;
        simTime_ = 0.0;
        rngSeed_ = 123456;
        rngState_ = rngSeed_;
        lastStepTime_ = 0.0;
        stepCount_ = 0;
    }
    
    // Context manager support
    void configure(float width, float height, size_t visionWidth = 5, size_t visionHeight = 5) {
        width_ = width;
        height_ = height;
        visionWidth_ = visionWidth;
        visionHeight_ = visionHeight;
    }
    
    void reset() {
        simTime_ = 0.0;
        stepCount_ = 0;
        // Reset agent and world state
    }
    
    void setAgentStart(float x, float y) {
        agent_.x = x;
        agent_.y = y;
    }
    
    // Simulation control
    void update(double timestep) {
        auto start = std::chrono::high_resolution_clock::now();
        
        simTime_ += timestep;
        stepCount_++;
        
        // Update world state
        generateVision();
        checkCollisions();
        
        auto end = std::chrono::high_resolution_clock::now();
        lastStepTime_ = std::chrono::duration<double>(end - start).count();
    }
    
    std::unique_ptr<EnhancedAction> applyAction(const EnhancedAction& action, double currentTime) {
        // Apply action to world
        // Update agent state
        // Return action result
        auto result = std::make_unique<EnhancedAction>();
        result->type = ActionType::Wait;
        result->timestamp = currentTime;
        result->reward = 0.0f;
        result->success = true;
        result->message = "Action applied";
        return result;
    }
    
    // Sensory input
    std::unique_ptr<EnhancedPercept> getSensoryPercept() const {
        auto percept = std::make_unique<EnhancedPercept>();
        percept->timestamp = simTime_;
        // Generate sensory input
        return percept;
    }
    
    // Agent state
    EnhancedAgentBody getAgentBody() const { return agent_; }
    
    // World objects
    void addObject(const EnhancedWorldObject& obj) {
        WorldObject worldObj;
        worldObj.x = obj.x;
        worldObj.y = obj.y;
        worldObj.radius = obj.radius;
        worldObj.type = obj.type;
        worldObj.value = obj.value;
        worldObj.active = obj.active;
        objects_.push_back(worldObj);
    }
    
    void removeObject(float x, float y) {
        objects_.erase(std::remove_if(objects_.begin(), objects_.end(),
            [x, y](const WorldObject& obj) { return std::abs(obj.x - x) < 0.1f && std::abs(obj.y - y) < 0.1f; }),
            objects_.end());
    }
    
    bool isValidPosition(float x, float y) const {
        return x >= 0 && x < width_ && y >= 0 && y < height_;
    }
    
    EnhancedWorldObject getObjectAt(float x, float y) const {
        for (const auto& obj : objects_) {
            float dx = std::abs(obj.x - x);
            float dy = std::abs(obj.y - y);
            if (dx < obj.radius && dy < obj.radius) {
                EnhancedWorldObject result;
                result.x = obj.x;
                result.y = obj.y;
                result.radius = obj.radius;
                result.type = obj.type;
                result.value = obj.value;
                result.active = obj.active;
                return result;
            }
        }
        return EnhancedWorldObject();
    }
    
    // World properties
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    float getMaxEnergy() const { return maxEnergy_; }
    void setMaxEnergy(float e) { maxEnergy_ = e; }
    float getEnergyDecayRate() const { return energyDecayRate_; }
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
    double getSimulationTime() const { return simTime_; }
    uint64_t getRandomSeed() const { return rngSeed_; }
    void setRandomSeed(uint64_t seed) { rngSeed_ = seed; rngState_ = seed; }
    
    // NumPy integration for visualization
    py::array_t<float> getWorldGrid() const {
        // Create 2D array representing world
        std::vector<float> grid;
        grid.resize(width_ * height_, 0.0f);
        // Fill with world data
        return py::array_t<float>(grid.size(), grid.data());
    }
    
    py::array_t<float> getAgentPosition() const {
        std::vector<float> pos{agent_.x, agent_.y};
        return py::array_t<float>(pos.size(), pos.data());
    }
    
    // Performance monitoring
    double getLastStepTime() const { return lastStepTime_; }
    size_t getStepCount() const { return stepCount_; }
    
    // Iterator support for world objects
    EnhancedWorldObjectIterator begin() const {
        return EnhancedWorldObjectIterator(objects_);
    }
    
    EnhancedWorldObjectIterator end() const {
        return EnhancedWorldObjectIterator(objects_, objects_.size());
    }
    
private:
    void generateVision() {}
    void checkCollisions() {}
    
    float width_;
    float height_;
    float maxEnergy_;
    float energyDecayRate_;
    double simTime_;
    uint64_t rngSeed_;
    uint64_t rngState_;
    double lastStepTime_;
    size_t stepCount_;
    
    EnhancedAgentBody agent_;
    std::vector<WorldObject> objects_;
    EnhancedPercept sensory_;
    size_t visionWidth_;
    size_t visionHeight_;
};

// Enhanced Agent implementation
class EnhancedAgent {
public:
    EnhancedAgent(std::shared_ptr<EnhancedBrain> brain) : brain_(brain) {}
    
    // Agent lifecycle
    void initialize(std::shared_ptr<EnhancedWorld> world) {
        world_ = world;
        // Initialize agent with world
    }
    
    void reset() {
        // Reset agent state
    }
    
    // Processing pipeline
    std::unique_ptr<EnhancedAction> process(const EnhancedPercept& percept) {
        // Process sensory input and produce action
        return std::make_unique<EnhancedAction>();
    }
    
    EnhancedAction decodeMotorCommand() {
        // Decode motor command from brain activity
        return EnhancedAction();
    }
    
    // Neuromodulation
    void applyReward(float reward, float predictedReward = 0.0f) {
        // Apply reward to brain
    }
    
    void updateDevelopment(double timestep) {
        // Update development
    }
    
    // State access
    DevelopmentalStage getDevelopmentalStage() const {
        return brain_ ? brain_->getDevelopmentalStage() : DevelopmentalStage::Adult;
    }
    
    float getNeuromodulationLevel() const { return brain_ ? brain_->getDopamineLevel() : 0.0f; }
    float getCuriosityLevel() const { return brain_ ? brain_->getCuriosityLevel() : 0.0f; }
    float getNoveltyLevel() const { return brain_ ? brain_->getNoveltyLevel() : 0.0f; }
    float getPredictionError() const { return brain_ ? brain_->getPredictionError() : 0.0f; }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Properties
    std::shared_ptr<EnhancedBrain> getBrain() const { return brain_; }
    std::shared_ptr<EnhancedWorld> getWorld() const { return world_; }
    
    // Event callbacks
    void setActionCallback(const std::function<std::unique_ptr<EnhancedAction>()>& callback) {
        actionCallback_ = callback;
    }
    
    void setPerceptCallback(const std::function<void(const EnhancedPercept&)>& callback) {
        perceptCallback_ = callback;
    }
    
    void setRewardCallback(const std::function<void(float)>& callback) {
        rewardCallback_ = callback;
    }
    
private:
    std::shared_ptr<EnhancedBrain> brain_;
    std::shared_ptr<EnhancedWorld> world_;
    
    // Configuration flags
    bool rewardModulationEnabled_ = false;
    bool structuralPlasticityEnabled_ = false;
    bool developmentEnabled_ = false;
    bool curiosityEnabled_ = false;
    
    // Callbacks
    std::function<std::unique_ptr<EnhancedAction>()> actionCallback_;
    std::function<void(const EnhancedPercept&)> perceptCallback_;
    std::function<void(float)> rewardCallback_;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

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

} // namespace nlm