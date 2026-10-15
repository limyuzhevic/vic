#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <algorithm>

namespace nlm {

/**
 * Event-Driven Neural Processing
 * 
 * Event-driven computation for large-scale neural networks:
 * - Only active neurons consume computation
 * - Efficient spike event queues
 * - Time-bucketed delayed spikes
 * - Event-driven plasticity updates
 * - Event-driven development system
 * 
 * Reduces computation for sparse networks
 */
class EventDrivenProcessing {
public:
    EventDrivenProcessing();
    ~EventDrivenProcessing();

    // ===== EVENT QUEUES =====

    // Spike event queue
    class SpikeEventQueue {
    public:
        struct SpikeEvent {
            NeuronId neuronId;           // Neuron that spiked
            Timestamp spikeTime;         // Time of spike
            RegionId regionId;           // Region ID
            PopulationId populationId;   // Population ID
            uint32_t eventOrder;         // Order of event
            float synapticStrength;      // Synaptic strength at time of spike
        };

        SpikeEventQueue();
        ~SpikeEventQueue();

        // Add spike event
        void addSpikeEvent(NeuronId neuronId, Timestamp spikeTime, RegionId regionId,
                          PopulationId populationId, float synapticStrength = 1.0f);

        // Process next spike event
        std::optional<SpikeEvent> processNextEvent();

        // Get number of pending events
        size_t size() const { return events_.size(); }

        // Check if queue is empty
        bool empty() const { return events_.empty(); }

        // Clear all events
        void clear();

        // Get events within time window
        std::vector<SpikeEvent> getEventsInTimeWindow(Timestamp startTime, 
                                                      Timestamp endTime) const;

        // Get events for specific neuron
        std::vector<SpikeEvent> getEventsForNeuron(NeuronId neuronId) const;

        // Get events for specific region
        std::vector<SpikeEvent> getEventsForRegion(RegionId regionId) const;

        // Get events for specific population
        std::vector<SpikeEvent> getEventsForPopulation(PopulationId populationId) const;

    private:
        std::vector<SpikeEvent> events_;
        size_t nextEventOrder_;
        mutable std::mutex mutex_;
    };

    // Delayed spike event queue
    class DelayedSpikeEventQueue {
    public:
        struct DelayedSpikeEvent {
            NeuronId neuronId;           // Neuron that will spike
            Timestamp scheduledTime;     // Scheduled spike time
            Delay delay;                 // Delay (in steps)
            Timestamp currentTime;       // Current simulation time
            Timestamp actualSpikeTime;   // Actual spike time (computed when due)
            RegionId regionId;           // Region ID
            PopulationId populationId;   // Population ID
            float synapticStrength;      // Synaptic strength at time of spike
        };

        DelayedSpikeEventQueue();
        ~DelayedSpikeEventQueue();

        // Add delayed spike event
        void addDelayedSpikeEvent(NeuronId neuronId, Timestamp scheduledTime,
                                 Delay delay, RegionId regionId,
                                 PopulationId populationId, float synapticStrength = 1.0f);

        // Process delayed spike events due at current time
        std::vector<SpikeEventQueue::SpikeEvent> processDueEvents(Timestamp currentTime);

        // Get number of pending delayed events
        size_t size() const { return events_.size(); }

        // Check if queue is empty
        bool empty() const { return events_.empty(); }

        // Clear all events
        void clear();

        // Get events scheduled within time window
        std::vector<DelayedSpikeEvent> getEventsInTimeWindow(Timestamp startTime,
                                                             Timestamp endTime) const;

        // Get events for specific neuron
        std::vector<DelayedSpikeEvent> getEventsForNeuron(NeuronId neuronId) const;

        // Get events for specific region
        std::vector<DelayedSpikeEvent> getEventsForRegion(RegionId regionId) const;

    private:
        std::vector<DelayedSpikeEvent> events_;
        mutable std::mutex mutex_;

        // Helper method to compute actual spike time
        Timestamp computeActualSpikeTime(const DelayedSpikeEvent& event) const;
    };

    // Plasticity event queue
    class PlasticityEventQueue {
    public:
        struct PlasticityEvent {
            SynapseId synapseId;          // Synapse to update
            PlasticityType plasticityType; // Type of plasticity
            Timestamp eventTime;         // Time of event
            float weightChange;          // Weight change
            float learningRate;          // Learning rate
            RegionId regionId;           // Region ID
            PopulationId populationId;   // Population ID
            float neuromodulator;         // Neuromodulator level
        };

        PlasticityEventQueue();
        ~PlasticityEventQueue();

        // Add plasticity event
        void addPlasticityEvent(SynapseId synapseId, PlasticityType plasticityType,
                               Timestamp eventTime, float weightChange, float learningRate,
                               RegionId regionId, PopulationId populationId, float neuromodulator);

        // Process plasticity events
        std::vector<PlasticityEvent> processEvents(Timestamp currentTime);

        // Get number of pending events
        size_t size() const { return events_.size(); }

        // Check if queue is empty
        bool empty() const { return events_.empty(); }

        // Clear all events
        void clear();

        // Get events within time window
        std::vector<PlasticityEvent> getEventsInTimeWindow(Timestamp startTime,
                                                           Timestamp endTime) const;

    private:
        std::vector<PlasticityEvent> events_;
        mutable std::mutex mutex_;
    };

    // Development event queue
    class DevelopmentEventQueue {
    public:
        struct DevelopmentEvent {
            DevelopmentalStage stage;     // Developmental stage
            Timestamp stageStartTime;     // Start time of stage
            Timestamp stageEndTime;       // End time of stage
            float stageProgress;          // Stage progress (0-1)
            float plasticityModifier;     // Plasticity modifier for stage
            float synaptogenesisRate;     // Synaptogenesis rate
            float pruningRate;            // Pruning rate
            RegionId regionId;           // Region ID
            PopulationId populationId;   // Population ID
        };

        DevelopmentEventQueue();
        ~DevelopmentEventQueue();

        // Add developmental stage
        void addDevelopmentStage(DevelopmentalStage stage, Timestamp startTime,
                                Timestamp endTime, RegionId regionId,
                                PopulationId populationId);

        // Process developmental events
        std::vector<DevelopmentEvent> processEvents(Timestamp currentTime);

        // Get number of pending events
        size_t size() const { return events_.size(); }

        // Check if queue is empty
        bool empty() const { return events_.empty(); }

        // Clear all events
        void clear();

        // Get events within time window
        std::vector<DevelopmentEvent> getEventsInTimeWindow(Timestamp startTime,
                                                            Timestamp endTime) const;

        // Get events for specific stage
        std::vector<DevelopmentEvent> getEventsForStage(DevelopmentalStage stage) const;

        // Get events for specific region
        std::vector<DevelopmentEvent> getEventsForRegion(RegionId regionId) const;

    private:
        std::vector<DevelopmentEvent> events_;
        mutable std::mutex mutex_;
    };

    // ===== EVENT-DRIVEN NEURAL PROCESSING =====

    // Event-driven neuron processing
    class EventDrivenNeuronProcessor {
    public:
        EventDrivenNeuronProcessor(std::shared_ptr<EventDrivenProcessing> eventDriven);
        ~EventDrivenNeuronProcessor();

        // Process neuron based on events
        void processNeuronEvents(NeuronId neuronId, Timestamp currentTime);

        // Process all pending spike events
        void processPendingSpikeEvents(Timestamp currentTime);

        // Process all pending plasticity events
        void processPendingPlasticityEvents(Timestamp currentTime);

        // Process all pending development events
        void processPendingDevelopmentEvents(Timestamp currentTime);

        // Update neuron based on event-driven dynamics
        void updateNeuronEventDriven(NeuronId neuronId, Timestamp currentTime, TimestepDuration dt);

        // Check if neuron should spike based on events
        bool checkNeuronSpikeEventDriven(NeuronId neuronId, Timestamp currentTime) const;

        // Get active neurons (neurons with pending events)
        std::vector<NeuronId> getActiveNeurons() const;

        // Get firing neurons (neurons that spiked this step)
        std::vector<NeuronId> getFiringNeurons() const;

    private:
        std::shared_ptr<EventDrivenProcessing> eventDriven_;

        // Neuron event states
        struct NeuronEventState {
            Timestamp lastSpikeTime;     // Last spike time
            Timestamp lastPlasticityUpdate; // Last plasticity update
            Timestamp lastDevelopmentUpdate; // Last development update
            bool hasPendingPlasticityEvents; // Has pending plasticity events
            bool hasPendingDevelopmentEvents; // Has pending development events
            float accumulatedPlasticity;  // Accumulated plasticity
            float accumulatedDevelopment; // Accumulated development
        };

        std::map<NeuronId, NeuronEventState> neuronEventStates_;
        mutable std::shared_mutex neuronEventMutex_;

        // Helper methods
        void updateNeuronEventState(NeuronId neuronId, Timestamp currentTime);
        bool hasPendingEventsForNeuron(NeuronId neuronId) const;
        float calculateEventDrivenInput(NeuronId neuronId, Timestamp currentTime) const;
    };

    // ===== TIME-BUCKETED PROCESSING =====

    // Time-bucketed event processing
    class TimeBucketedProcessor {
    public:
        TimeBucketedProcessor(std::shared_ptr<EventDrivenProcessing> eventDriven);
        ~TimeBucketedProcessor();

        // Set time bucket size
        void setTimeBucketSize(TimestepDuration bucketSize) { bucketSize_ = bucketSize; }

        // Get current time bucket
        size_t getCurrentTimeBucket(Timestamp currentTime) const;

        // Get events for specific time bucket
        std::vector<SpikeEventQueue::SpikeEvent> getEventsForBucket(size_t bucketIndex) const;

        // Get delayed events due in specific time bucket
        std::vector<DelayedSpikeEventQueue::DelayedSpikeEvent> getDelayedEventsForBucket(
            size_t bucketIndex) const;

        // Process time bucket
        void processTimeBucket(size_t bucketIndex, Timestamp currentTime);

        // Process all time buckets up to current time
        void processTimeBucketsUpTo(Timestamp currentTime);

        // Get number of time buckets
        size_t getTimeBucketCount() const { return timeBuckets_.size(); }

        // Clear old time buckets
        void clearOldTimeBuckets(Timestamp currentTime);

    private:
        std::shared_ptr<EventDrivenProcessing> eventDriven_;

        TimestepDuration bucketSize_;
        std::map<size_t, std::vector<SpikeEventQueue::SpikeEvent>> timeBuckets_;
        std::map<size_t, std::vector<DelayedSpikeEventQueue::DelayedSpikeEvent>> delayedTimeBuckets_;
        mutable std::shared_mutex timeBucketMutex_;

        // Helper methods
        size_t getTimeBucketIndex(Timestamp time) const;
        void addEventToTimeBucket(const SpikeEventQueue::SpikeEvent& event);
        void addDelayedEventToTimeBucket(const DelayedSpikeEventQueue::DelayedSpikeEvent& event);
    };

    // ===== EVENT-DRIVEN PLASTICITY =====

    // Event-driven plasticity processor
    class EventDrivenPlasticityProcessor {
    public:
        EventDrivenPlasticityProcessor(std::shared_ptr<EventDrivenProcessing> eventDriven);
        ~EventDrivenPlasticityProcessor();

        // Process plasticity events
        void processPlasticityEvents(Timestamp currentTime);

        // Apply plasticity event
        void applyPlasticityEvent(const PlasticityEventQueue::PlasticityEvent& event);

        // Get plasticity efficiency
        float getPlasticityEfficiency() const { return plasticityEfficiency_; }

        // Update plasticity efficiency
        void updatePlasticityEfficiency(float newEfficiency);

        // Get number of plasticity events processed
        size_t getPlasticityEventsProcessed() const { return plasticityEventsProcessed_; }

        // Get plasticity computation time
        double getPlasticityComputationTime() const { return plasticityComputationTime_; }

    private:
        std::shared_ptr<EventDrivenProcessing> eventDriven_;

        float plasticityEfficiency_;
        size_t plasticityEventsProcessed_;
        double plasticityComputationTime_;

        // Statistics
        struct PlasticityStats {
            size_t totalEvents;
            size_t successfulEvents;
            size_t failedEvents;
            double averageProcessingTime;
            float averageEfficiency;
        } plasticityStats_;

        // Helper methods
        void updateStats(const PlasticityEventQueue::PlasticityEvent& event,
                        bool success, double processingTime);
    };

    // ===== EVENT-DRIVEN DEVELOPMENT =====

    // Event-driven development processor
    class EventDrivenDevelopmentProcessor {
    public:
        EventDrivenDevelopmentProcessor(std::shared_ptr<EventDrivenProcessing> eventDriven);
        ~EventDrivenDevelopmentProcessor();

        // Process development events
        void processDevelopmentEvents(Timestamp currentTime);

        // Apply development event
        void applyDevelopmentEvent(const DevelopmentEventQueue::DevelopmentEvent& event);

        // Get developmental stage
        DevelopmentalStage getCurrentDevelopmentalStage() const { return currentDevelopmentalStage_; }

        // Set current developmental stage
        void setCurrentDevelopmentalStage(DevelopmentalStage stage) { currentDevelopmentalStage_ = stage; }

        // Get developmental progress
        float getDevelopmentalProgress() const { return developmentalProgress_; }

        // Update developmental progress
        void updateDevelopmentalProgress(float progress);

        // Get development efficiency
        float getDevelopmentEfficiency() const { return developmentEfficiency_; }

        // Update development efficiency
        void updateDevelopmentEfficiency(float newEfficiency);

        // Get number of development events processed
        size_t getDevelopmentEventsProcessed() const { return developmentEventsProcessed_; }

        // Get development computation time
        double getDevelopmentComputationTime() const { return developmentComputationTime_; }

    private:
        std::shared_ptr<EventDrivenProcessing> eventDriven_;

        DevelopmentalStage currentDevelopmentalStage_;
        float developmentalProgress_;
        float developmentEfficiency_;
        size_t developmentEventsProcessed_;
        double developmentComputationTime_;

        // Statistics
        struct DevelopmentStats {
            size_t totalEvents;
            size_t successfulEvents;
            size_t failedEvents;
            double averageProcessingTime;
            float averageEfficiency;
        } developmentStats_;

        // Helper methods
        void updateStats(const DevelopmentEventQueue::DevelopmentEvent& event,
                        bool success, double processingTime);
    };

    // ===== OVERRIDE =====

    // Override Brain for brain integration
    class IntegratedEventDrivenProcessing : public Brain {
    public:
        IntegratedEventDrivenProcessing(std::shared_ptr<Config> config,
                                       std::shared_ptr<EventDrivenProcessing> eventDriven);
        ~IntegratedEventDrivenProcessing() override;

        // Initialize event-driven processing
        bool initialize() override;

        // Enable/disable event-driven processing
        void enableEventDrivenProcessing(bool enable) { eventDrivenEnabled_ = enable; }
        bool isEventDrivenProcessingEnabled() const { return eventDrivenEnabled_; }

        // Set event-driven efficiency target
        void setEventDrivenEfficiencyTarget(float target) { efficiencyTarget_ = target; }
        float getEventDrivenEfficiencyTarget() const { return efficiencyTarget_; }

        // Get current efficiency
        float getCurrentEfficiency() const;

        // Update event-driven processing
        void updateEventDrivenProcessing(TimestepDuration dt);

        // Get performance statistics
        struct EventDrivenPerformanceStats {
            float computationEfficiency;      // How efficiently computation is performed
            float memoryEfficiency;           // Memory usage efficiency
            float eventProcessingEfficiency;   // Event processing efficiency
            float plasticityEfficiency;        // Plasticity computation efficiency
            float developmentEfficiency;       // Development computation efficiency
            float overallPerformance;          // Overall performance (0-1)
        };

        EventDrivenPerformanceStats getPerformanceStats() const;

    private:
        std::shared_ptr<EventDrivenProcessing> eventDriven_;
        bool eventDrivenEnabled_;
        float efficiencyTarget_;
        float currentEfficiency_;
        EventDrivenPerformanceStats currentStats_;

        // Helper methods
        void updateComputationEfficiency();
        void updateMemoryEfficiency();
        void updateEventProcessingEfficiency();
        void updatePlasticityEfficiency();
        void updateDevelopmentEfficiency();
    };

private:
    // Event queues
    SpikeEventQueue spikeEventQueue_;
    DelayedSpikeEventQueue delayedSpikeEventQueue_;
    PlasticityEventQueue plasticityEventQueue_;
    DevelopmentEventQueue developmentEventQueue_;

    // Event-driven processors
    EventDrivenNeuronProcessor neuronProcessor_;
    TimeBucketedProcessor timeBucketedProcessor_;
    EventDrivenPlasticityProcessor plasticityProcessor_;
    EventDrivenDevelopmentProcessor developmentProcessor_;

    // Configuration
    bool eventDrivenEnabled_;
    TimestepDuration timeBucketSize_;
    size_t maxEventQueueSize_;
    float eventProcessingThreshold_;

    // Statistics
    struct EventDrivenStats {
        size_t totalEventsProcessed;
        size_t totalEventsGenerated;
        double averageProcessingTime;
        float eventProcessingEfficiency;
        float memoryEfficiency;
    } stats_;

    // Helper methods
    void updateStatistics();
    bool checkEventProcessingThreshold() const;
    void cleanupOldEvents();
};

} // namespace nlm
