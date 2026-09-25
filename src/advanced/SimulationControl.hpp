#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "<string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <chrono>

namespace nlm {

// Simulation control types
enum class SimulationPhase {
    Initialization,
    Running,
    Paused,
    Terminated,
    Checkpointing,
    Recovery
};

enum class SimulationMode {
    EventDriven,
    TimeStepDriven,
    Hybrid,
    AdaptiveTimeStep,
    VariableTimeStep
};

enum class TimeSteppingAlgorithm {
    Fixed,
    Adaptive,
    RungeKutta4,
    RungeKutta45,
    AdamsBashforth,
    BackwardEuler,
    ExponentialTimeDifferencing,
    NeuralSpecific
};

// Event-driven simulation types
enum class EventType {
    Spike,
    SynapseUpdate,
    PlasticityChange,
    NeuromodulationChange,
    DevelopmentStageChange,
    Custom
};

// Custom observation model
class ObservationModel {
public:
    virtual ~ObservationModel() = default;
    
    virtual float observe(const Neuron* neuron) const = 0;
    virtual float observe(const Synapse* synapse) const = 0;
    virtual float observe(const NeuralRegion* region) const = 0;
    virtual void update(float dt) = 0;
    
    virtual std::string getModelType() const = 0;
    virtual void setParameters(const std::map<std::string, float>& params) = 0;
    virtual std::map<std::string, float> getParameters() const = 0;
};

// Custom event for event-driven simulation
class SimulationEvent {
public:
    SimulationEvent(EventType type, Timestamp timestamp, SimulationStep step);
    
    EventType getType() const { return type; }
    Timestamp getTimestamp() const { return timestamp; }
    SimulationStep getStep() const { return step; }
    
    virtual void process(std::vector<NeuralRegion*>& regions);
    virtual std::string getEventDescription() const;
    
protected:
    EventType type;
    Timestamp timestamp;
    SimulationStep step;
};

// Event handler interface
class EventHandler {
public:
    virtual ~EventHandler() = default;
    virtual bool handleEvent(const SimulationEvent& event, std::vector<NeuralRegion*>& regions) = 0;
    virtual std::string getHandlerName() const = 0;
    virtual EventType getHandledEventType() const = 0;
};

// Custom simulation loop control
class SimulationLoopController {
public:
    SimulationLoopController();
    ~SimulationLoopController();
    
    // Main loop control
    bool initialize(std::vector<NeuralRegion*>& regions);
    bool step(std::vector<NeuralRegion*>& regions, Timestamp currentTime, SimulationStep step);
    bool terminate(std::vector<NeuralRegion*>& regions);
    bool pause(std::vector<NeuralRegion*>& regions);
    bool resume(std::vector<NeuralRegion*>& regions);
    
    // Configuration
    void setSimulationMode(SimulationMode mode);
    void setTimeSteppingAlgorithm(TimeSteppingAlgorithm algorithm);
    void setTimeStepSize(TimestepDuration dt);
    void setMaxTime(Timestamp maxTime);
    void setMinTime(Timestamp minTime);
    void enableEventDriven(bool enable);
    void setMaxEventsPerStep(size_t maxEvents);
    
    // Accessors
    SimulationPhase getPhase() const { return phase; }
    Timestamp getCurrentTime() const { return currentTime; }
    SimulationStep getCurrentStep() const { return currentStep; }
    TimestepDuration getTimeStepSize() const { return timeStepSize; }
    SimulationMode getSimulationMode() const { return simulationMode; }
    TimeSteppingAlgorithm getTimeSteppingAlgorithm() const { return timeSteppingAlgorithm; }
    
    // Event management
    void addEventHandler(std::unique_ptr<EventHandler> handler);
    void removeEventHandler(const std::string& handlerName);
    void addCustomEvent(std::unique_ptr<SimulationEvent> event);
    void clearEvents();
    size_t getEventCount() const { return events.size(); }
    
    // Performance and timing
    float getAverageTimePerStep() const { return avgTimePerStep; }
    float getSimulationSpeed() const { return simulationSpeed; }
    void resetTimingStats();
    
    // Callbacks
    void setPreStepCallback(std::function<void(std::vector<NeuralRegion*>&)> callback);
    void setPostStepCallback(std::function<void(std::vector<NeuralRegion*>&)> callback);
    void setPhaseChangeCallback(std::function<void(SimulationPhase, SimulationPhase)> callback);
    
    // Checkpointing
    bool saveCheckpoint(const std::string& filepath, std::vector<NeuralRegion*>& regions);
    bool loadCheckpoint(const std::string& filepath, std::vector<NeuralRegion*>& regions);
    bool createAutoCheckpoint(const std::string& directory, std::vector<NeuralRegion*>& regions);
    
    // Advanced control
    void enableAdaptiveTimeStep(bool enable);
    void setStabilityThreshold(float threshold);
    void setMaxTimeStepChange(float maxChange);
    bool shouldChangeTimeStep(const std::vector<NeuralRegion*>& regions);
    float calculateOptimalTimeStep(const std::vector<NeuralRegion*>& regions);
    
    // Event-driven simulation
    void triggerEvent(EventType type, Timestamp timestamp);
    void triggerSpikeEvent(NeuronId neuronId, Timestamp timestamp);
    void triggerPlasticityEvent(SynapseId synapseId, Timestamp timestamp);
    void triggerNeuromodulationEvent(Timestamp timestamp);
    
    // Custom event handlers
    template<typename T, typename... Args>
    void registerEventHandler(Args&&... args) {
        eventHandlers.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    }
    
    template<typename Predicate>
    void filterEvents(Predicate predicate) {
        auto it = std::remove_if(events.begin(), events.end(), predicate);
        events.erase(it, events.end());
    }
    
    // Statistics and analysis
    std::map<std::string, float> getPerformanceStats() const;
    std::map<std::string, size_t> getEventStats() const;
    std::string getSimulationReport() const;
    
protected:
    void processEvents(std::vector<NeuralRegion*>& regions);
    void sortEventsByTime();
    void updateTimingStats(float stepTime);
    SimulationPhase phase;
    Timestamp currentTime;
    SimulationStep currentStep;
    TimestepDuration timeStepSize;
    Timestamp maxTime;
    Timestamp minTime;
    SimulationMode simulationMode;
    TimeSteppingAlgorithm timeSteppingAlgorithm;
    bool enableAdaptiveTimeStepFlag;
    float stabilityThreshold;
    float maxTimeStepChange;
    bool eventDrivenEnabled;
    size_t maxEventsPerStep;
    
    std::vector<std::unique_ptr<SimulationEvent>> events;
    std::vector<std::unique_ptr<EventHandler>> eventHandlers;
    
    float avgTimePerStep;
    float simulationSpeed;
    size_t stepCount;
    
    std::function<void(std::vector<NeuralRegion*>&)> preStepCallback;
    std::function<void(std::vector<NeuralRegion*>&)> postStepCallback;
    std::function<void(SimulationPhase, SimulationPhase)> phaseChangeCallback;
};

// Advanced time-stepping algorithms
class AdvancedTimeStepper {
public:
    AdvancedTimeStepper();
    ~AdvancedTimeStepper();
    
    // Time-stepping algorithms
    virtual Timestamp advanceTime(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                  TimeSteppingAlgorithm algorithm, TimestepDuration dt);
    
    // Specific algorithms
    static Timestamp advanceFixed(Timestamp currentTime, TimestepDuration dt);
    static Timestamp advanceAdaptive(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                     TimestepDuration dt, float stabilityThreshold);
    static Timestamp advanceRungeKutta4(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                         TimestepDuration dt);
    static Timestamp advanceRungeKutta45(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                          TimestepDuration dt);
    static Timestamp advanceAdamsBashforth(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                           TimestepDuration dt);
    static Timestamp advanceBackwardEuler(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                          TimestepDuration dt);
    static Timestamp advanceExponentialTimeDifferencing(Timestamp currentTime,
                                                         const std::vector<NeuralRegion*>& regions,
                                                         TimestepDuration dt);
    static Timestamp advanceNeuralSpecific(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                           TimestepDuration dt);
    
    // Algorithm configuration
    void setAlgorithmParameters(TimeSteppingAlgorithm algorithm,
                               const std::map<std::string, float>& parameters);
    std::map<std::string, float> getAlgorithmParameters(TimeSteppingAlgorithm algorithm) const;
    
    // Stability and error control
    bool checkStability(const std::vector<NeuralRegion*>& regions, TimeSteppingAlgorithm algorithm);
    float estimateError(const std::vector<NeuralRegion*>& regions, TimeSteppingAlgorithm algorithm,
                       TimestepDuration dt);
    float adjustTimeStepBasedOnError(TimestepDuration currentDt, float error, float tolerance);
    
    // Neural-specific time stepping
    static float calculateNeuronStability(const Neuron* neuron, TimestepDuration dt);
    static float calculateSynapseStability(const Synapse* synapse, TimestepDuration dt);
    static float calculateNetworkStability(const std::vector<NeuralRegion*>& regions, TimestepDuration dt);
    
protected:
    std::map<TimeSteppingAlgorithm, std::map<std::string, float>> algorithmParameters;
    float stabilityThreshold;
    float errorTolerance;
    float maxTimeStepChange;
};

// Event-driven simulation manager
class EventManager {
public:
    EventManager();
    ~EventManager();
    
    // Event management
    void addEvent(std::unique_ptr<SimulationEvent> event);
    void addScheduledEvent(std::unique_ptr<SimulationEvent> event, Timestamp delay);
    void addRecurringEvent(std::unique_ptr<SimulationEvent> event, Timestamp interval, 
                          size_t repetitions = -1); // -1 means infinite
    
    // Event processing
    std::vector<std::unique_ptr<SimulationEvent>> getEventsToProcess(Timestamp currentTime,
                                                                      SimulationStep currentStep);
    void processEvent(std::unique_ptr<SimulationEvent> event, std::vector<NeuralRegion*>& regions);
    void processAllPendingEvents(std::vector<NeuralRegion*>& regions, Timestamp currentTime,
                                SimulationStep currentStep);
    
    // Event types
    void scheduleSpikeEvent(NeuronId neuronId, Timestamp delay);
    void scheduleSynapseEvent(SynapseId synapseId, Timestamp delay);
    void schedulePlasticityEvent(SynapseId synapseId, Timestamp delay);
    void scheduleNeuromodulationEvent(Timestamp delay);
    void scheduleDevelopmentEvent(DevelopmentalStage newStage, Timestamp delay);
    
    // Event handlers
    void addEventHandler(std::unique_ptr<EventHandler> handler);
    void removeEventHandler(const std::string& handlerName);
    void addEventFilter(std::function<bool(const SimulationEvent&)> filter);
    void clearEventFilters();
    
    // Configuration
    void setEventProcessingMode(bool realTime);
    void setMaxEventsPerSecond(size_t maxEvents);
    void setEventQueueSize(size_t maxQueueSize);
    void enablePriorityQueue(bool enable);
    
    // Statistics
    size_t getTotalEventsProcessed() const { return totalEventsProcessed; }
    size_t getEventsInQueue() const { return eventsQueue.size(); }
    float getAverageEventProcessingTime() const { return avgEventProcessingTime; }
    
    // Event analysis
    std::map<std::string, size_t> getEventTypeStats() const;
    std::map<std::string, float> getEventTimeStats() const;
    std::string getEventReport() const;
    
    // Export/import
    bool saveEventLog(const std::string& filepath);
    bool loadEventLog(const std::string& filepath);
    std::string exportEventDataAsText() const;
    std::string exportEventDataAsJSON() const;
    
    // Advanced features
    void enableCustomEventProcessing(std::function<void(std::unique_ptr<SimulationEvent>)> processor);
    void setEventCompression(bool enable, size_t compressionLevel = 5);
    void enableEventPrioritization(const std::vector<EventType>& priorityOrder);
    bool reorderEventQueue(const std::vector<size_t>& newOrder);
    
    // Event analysis tools
    float calculateEventInterarrivalTime(EventType eventType) const;
    float calculateEventProcessingDuration(EventType eventType) const;
    size_t countEventOccurrences(EventType eventType, Timestamp startTime, Timestamp endTime) const;
    std::vector<Timestamp> getEventTimestamps(EventType eventType) const;
    
protected:
    void processScheduledEvents(Timestamp currentTime, SimulationStep currentStep);
    void processRecurringEvents(Timestamp currentTime, SimulationStep currentStep);
    void cleanupCompletedRecurringEvents(Timestamp currentTime, SimulationStep currentStep);
    void updateEventStats(const SimulationEvent& event, float processingTime);
    
    bool realTimeProcessing;
    size_t maxEventsPerSecond;
    size_t eventQueueSize;
    bool priorityQueueEnabled;
    bool enableCompression;
    size_t compressionLevel;
    
    std::vector<std::unique_ptr<SimulationEvent>> eventsQueue;
    std::vector<std::unique_ptr<EventHandler>> eventHandlers;
    std::vector<std::function<bool(const SimulationEvent&)>> eventFilters;
    std::vector<EventType> priorityOrder;
    
    size_t totalEventsProcessed;
    float avgEventProcessingTime;
    
    std::map<EventType, std::vector<Timestamp>> eventTimestamps;
    std::map<EventType, size_t> eventTypeCounts;
    std::map<EventType, float> eventProcessingTimes;
    
    std::function<void(std::unique_ptr<SimulationEvent>)> customEventProcessor;
};

// Advanced observation model implementations
class FIRObservationModel : public ObservationModel {
public:
    FIRObservationModel();
    ~FIRObservationModel() override = default;
    
    float observe(const Neuron* neuron) const override;
    float observe(const Synapse* synapse) const override;
    float observe(const NeuralRegion* region) const override;
    void update(float dt) override;
    
    std::string getModelType() const override { return "FIR"; }
    void setParameters(const std::map<std::string, float>& params) override;
    std::map<std::string, float> getParameters() const override;
    
    // FIR-specific parameters
    void setFilterCoefficients(const std::vector<float>& coeffs);
    std::vector<float> getFilterCoefficients() const;
    void setFilterOrder(size_t order) { filterOrder = order; }
    size_t getFilterOrder() const { return filterOrder; }
    
private:
    std::vector<float> filterCoefficients;
    size_t filterOrder;
    std::vector<float> stateVector;
};

class IFObservationModel : public ObservationModel {
public:
    IFObservationModel();
    ~IFObservationModel() override = default;
    
    float observe(const Neuron* neuron) const override;
    float observe(const Synapse* synapse) const override;
    float observe(const NeuralRegion* region) const override;
    void update(float dt) override;
    
    std::string getModelType() const override { return "IF"; }
    void setParameters(const std::map<std::string, float>& params) override;
    std::map<std::string, float> getParameters() const override;
    
    // IF-specific parameters
    void setCutoffFrequency(float freq) { cutoffFrequency = freq; }
    float getCutoffFrequency() const { return cutoffFrequency; }
    void setGain(float gain) { gainFactor = gain; }
    float getGain() const { return gainFactor; }
    
private:
    float cutoffFrequency;
    float gainFactor;
    float stateVariable;
};

class KalmanFilterObservationModel : public ObservationModel {
public:
    KalmanFilterObservationModel();
    ~KalmanFilterObservationModel() override = default;
    
    float observe(const Neuron* neuron) const override;
    float observe(const Synapse* synapse) const override;
    float observe(const NeuralRegion* region) const override;
    void update(float dt) override;
    
    std::string getModelType() const override { return "KalmanFilter"; }
    void setParameters(const std::map<std::string, float>& params) override;
    std::map<std::string, float> getParameters() const override;
    
    // Kalman filter parameters
    void setProcessNoise(float noise) { processNoise = noise; }
    float getProcessNoise() const { return processNoise; }
    void setMeasurementNoise(float noise) { measurementNoise = noise; }
    float getMeasurementNoise() const { return measurementNoise; }
    void setInitialCovariance(float cov) { initialCovariance = cov; }
    float getInitialCovariance() const { return initialCovariance; }
    
private:
    float processNoise;
    float measurementNoise;
    float initialCovariance;
    float stateEstimate;
    float stateCovariance;
};

class CustomNeuralObservationModel : public ObservationModel {
public:
    CustomNeuralObservationModel(std::function<float(const Neuron*)> neuronObs,
                                std::function<float(const Synapse*)> synapseObs,
                                std::function<float(const NeuralRegion*)> regionObs);
    ~CustomNeuralObservationModel() override = default;
    
    float observe(const Neuron* neuron) const override;
    float observe(const Synapse* synapse) const override;
    float observe(const NeuralRegion* region) const override;
    void update(float dt) override;
    
    std::string getModelType() const override { return "CustomNeural"; }
    void setParameters(const std::map<std::string, float>& params) override;
    std::map<std::string, float> getParameters() const override;
    
    // Custom function setters
    void setNeuronObservation(std::function<float(const Neuron*)> obs) { neuronObs = obs; }
    void setSynapseObservation(std::function<float(const Synapse*)> obs) { synapseObs = obs; }
    void setRegionObservation(std::function<float(const NeuralRegion*)> obs) { regionObs = obs; }
    
private:
    std::function<float(const Neuron*)> neuronObs;
    std::function<float(const Synapse*)> synapseObs;
    std::function<float(const NeuralRegion*)> regionObs;
    std::map<std::string, float> parameters;
};

// Advanced reward and punishment systems
class AdvancedRewardSystem {
public:
    AdvancedRewardSystem();
    ~AdvancedRewardSystem();
    
    // Reward types
    enum class RewardType {
        Extrinsic,
        Intrinsic,
        Predictive,
        Episodic,
        Social,
        Shaping,
        CuriosityDriven,
        NoveltyBased
    };
    
    // Reward structures
    struct RewardPacket {
        float value;
        RewardType type;
        Timestamp timestamp;
        SimulationStep step;
        std::string source;
        float confidence;
        std::map<std::string, float> attributes;
        
        RewardPacket(float v, RewardType t, const std::string& s = "") 
            : value(v), type(t), timestamp(0.0), step(0), source(s), confidence(1.0f) {}
    };
    
    // Reward administration
    void deliverReward(const RewardPacket& reward, std::vector<NeuralRegion*>& regions);
    void scheduleReward(const RewardPacket& reward, Timestamp delay);
    void cancelReward(const std::string& source);
    
    // Reward modulation
    void setRewardModulation(float dopamine, float serotonin, float acetylcholine);
    void applyRewardModulation(std::vector<NeuralRegion*>& regions);
    void setRewardSensitivity(float sensitivity) { rewardSensitivity = sensitivity; }
    float getRewardSensitivity() const { return rewardSensitivity; }
    
    // Reward prediction and error
    float calculatePredictionError(float predictedReward, float actualReward);
    void updateRewardPredictions(const std::vector<RewardPacket>& rewards);
    float getExpectedReward(const std::string& context) const;
    
    // Reward shaping and scheduling
    void addRewardShapingRule(const std::string& ruleName,
                             std::function<float(const std::vector<RewardPacket>&)> rule);
    void removeRewardShapingRule(const std::string& ruleName);
    void scheduleShapedReward(const std::string& ruleName, float baseReward,
                             const std::map<std::string, float>& context);
    
    // Reward statistics
    float getTotalRewardsDelivered() const { return totalRewardsDelivered; }
    float getAverageRewardValue() const { return totalRewardsDelivered > 0 ? 
                                        accumulatedRewardValue / totalRewardsDelivered : 0.0f; }
    size_t getRewardCount() const { return rewardCount; }
    
    // Reward analysis
    std::map<std::string, float> analyzeRewardPatterns() const;
    std::vector<RewardPacket> getRewardsByType(RewardType type) const;
    std::vector<RewardPacket> getRecentRewards(size_t count = 10) const;
    
    // Reward export/import
    bool saveRewardLog(const std::string& filepath);
    bool loadRewardLog(const std::string& filepath);
    std::string exportRewardsAsJSON() const;
    
    // Advanced reward features
    void enableTemporalDifferenceLearning(float discountFactor = 0.99f, float learningRate = 0.01f);
    void enableEpisodicRewardProcessing(bool enable);
    void enableSocialLearning(bool enable, const std::string& socialGroup = "default");
    void enableIntrinsicMotivation(bool enable, float noveltyThreshold = 0.5f);
    
    // Reward learning algorithms
    void temporalDifferenceLearning(const RewardPacket& reward, float valueEstimate);
    void reinforcementLearning(const std::vector<RewardPacket>& rewards,
                              const std::vector<std::vector<float>>& stateFeatures);
    void policyGradientLearning(const std::vector<RewardPacket>& rewards,
                               const std::vector<std::vector<float>>& actionProbabilities);
    
    // Reward optimization
    float optimizeRewardSchedule(const std::vector<RewardPacket>& rewards);
    bool predictRewardOutcome(const std::vector<float>& stateFeatures, float& predictedReward);
    void adaptRewardStrategy(const std::vector<RewardPacket>& rewards);
    
protected:
    void processScheduledRewards(Timestamp currentTime, SimulationStep currentStep);
    void applyShapedRewards(std::vector<NeuralRegion*>& regions);
    void updateLearningAlgorithms(const std::vector<RewardPacket>& rewards);
    
    float rewardSensitivity;
    float dopamineLevel;
    float serotoninLevel;
    float acetylcholineLevel;
    
    float totalRewardsDelivered;
    float accumulatedRewardValue;
    size_t rewardCount;
    
    std::vector<RewardPacket> scheduledRewards;
    std::map<std::string, std::function<float(const std::vector<RewardPacket>&)>> rewardShapingRules;
    
    float discountFactor;
    float learningRate;
    bool enableTemporalDifference;
    bool enableEpisodicProcessing;
    bool enableSocialLearning;
    std::string socialGroup;
    bool enableIntrinsicMotivation;
    float noveltyThreshold;
    
    std::map<std::string, float> rewardPredictions;
    std::map<std::string, float> valueEstimates;
};

} // namespace nlm
