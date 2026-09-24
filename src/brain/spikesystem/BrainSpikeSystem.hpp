#pragma once

#include "BrainSpikeSystem.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {
    class Brain;
    
    // Spike system for event-driven neural computation
    class SpikeSystem {
    public:
        SpikeSystem();
        ~SpikeSystem();
        
        // Register spike handler for post-synaptic events
        void registerHandler(std::function<void(const DetailedSpikeEvent&)>&& handler);
        
        // Register delayed spike handler for synaptic input delivery
        void registerDelayedHandler(std::function<void(const DelayedSpikeEvent&)>&& handler);
        
        // Process delayed spike events (deliver synaptic input)
        void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
        
        // Process immediate spikes
        void processSpikes(SimulationStep currentStep);
        
        // Queue a spike event for processing
        void queueSpike(const SpikeEvent& event);
        
        // Queue a delayed spike event
        void queueDelayedSpike(const DelayedSpikeEvent& event);
        
        // Get count of pending spike events
        size_t getPendingSpikeCount() const;
        size_t getPendingDelayedCount() const;
        
        // Reset spike system
        void reset();
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}
