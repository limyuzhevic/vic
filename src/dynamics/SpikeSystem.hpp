#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>

namespace nlm {

// SpikeEvent with additional metadata
struct DetailedSpikeEvent {
    NeuronId source;
    Timestamp timestamp;
    SimulationStep step;
    RegionId regionId;
    PopulationId populationId;
    
    DetailedSpikeEvent() : source(), timestamp(0.0), step(0), regionId(), populationId() {}
    DetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, RegionId rid, PopulationId pid)
        : source(nid), timestamp(ts), step(s), regionId(rid), populationId(pid) {}
};

// Event-driven neural computation system
// Handles spike event processing and delayed synaptic transmission
class SpikeSystem {
public:
    SpikeSystem();
    ~SpikeSystem();
    
    // Disable copying, enable moving
    SpikeSystem(const SpikeSystem&) = delete;
    SpikeSystem& operator=(const SpikeSystem&) = delete;
    SpikeSystem(SpikeSystem&&) noexcept;
    SpikeSystem& operator=(SpikeSystem&&) noexcept;
    
    // Queue a spike event
    void queueSpike(const SpikeEvent& event);
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    
    // Queue a delayed spike event (for synaptic transmission)
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    
    // Process all queued spikes for current step
    void processSpikes(SimulationStep currentStep);
    
    // Process pending delayed spikes that should be delivered now
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Register spike handler callback (called when spikes are generated)
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    void registerHandler(SpikeHandler handler);
    
    // Register delayed spike handler (called when delayed spikes are delivered)
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    // Get spike history
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    void clearHistory();
    
    // Register neuromodulation handlers
    using DopamineHandler = std::function<void(float)>;
    void registerDopamineHandler(DopamineHandler handler);
    
    using CuriosityHandler = std::function<void(float)>;
    void registerCuriosityHandler(CuriosityHandler handler);
    
    using NoveltyHandler = std::function<void(float)>;
    void registerNoveltyHandler(NoveltyHandler handler);
    
    using PredictionErrorHandler = std::function<void(float)>;
    void registerPredictionErrorHandler(PredictionErrorHandler handler);
    
    using IntegratedNeuromodulationHandler = std::function<void(float, float, float, float)>;
    void registerIntegratedNeuromodulationHandler(IntegratedNeuromodulationHandler handler);
    
    // Configure neuromodulation integration
    void configureDopamineModulation(bool enabled, float scale = 1.0f);
    void configureCuriosityModulation(bool enabled, float scale = 1.0f);
    void configureNoveltyModulation(bool enabled, float scale = 1.0f);
    void configurePredictionErrorModulation(bool enabled, float scale = 1.0f);
    
    // Set neuromodulation levels directly
    void setDopamineLevel(float level);
    void setCuriosityLevel(float level);
    void setNoveltyLevel(float level);
    void setPredictionErrorLevel(float level);
    
    // Get current neuromodulation levels
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorLevel() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
