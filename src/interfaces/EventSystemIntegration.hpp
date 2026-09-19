// EventSystem integration with ComponentFactory and existing SpikeSystem
// This file provides the necessary integration to make EventSystem work with the existing codebase

#pragma once

#include "EventSystem.hpp"
#include "../core/ComponentFactory.hpp"
#include "../brain/Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Integration: EventSystem automatically integrates with SpikeSystem
// when EventSystem is initialized with a Brain that has a SpikeSystem

// Function to create an EventSystem and automatically register it with Brain's SpikeSystem
inline std::unique_ptr<EventSystem> createEventSystemForBrain(Brain* brain, const std::shared_ptr<Config>& config) {
    if (!brain) return nullptr;
    
    auto eventSystem = std::make_unique<EventSystem>();
    
    // Initialize the EventSystem with the brain
    if (eventSystem->initialize(brain)) {
        // EventSystem automatically registers handlers that process:
        // 1. Spike events (converted from nlm::SpikeEvent)
        // 2. Delayed spike events (converted from nlm::DelayedSpikeEvent)
        // 3. Detailed spike events (converted from DetailedSpikeEvent)
        
        // Configure event system based on config
        if (config) {
            auto maxEvents = config->getOr<uint64_t>("max_events_in_memory", 10000);
            eventSystem->setMaxEventsInMemory(maxEvents);
            
            auto timeout = config->getOr<float>("event_timeout", 1000.0f);
            eventSystem->setEventTimeout(timeout);
        }
        
        return eventSystem;
    }
    
    return nullptr;
}

// Function to integrate existing SpikeSystem with EventSystem
// This can be called to enhance the existing queue-based approach
inline void integrateSpikeSystemWithEventSystem(
    SpikeSystem* spikeSystem, 
    EventSystem& eventSystem
) {
    if (!spikeSystem) return;
    
    // Register event handlers that forward events to SpikeSystem
    eventSystem.registerEventHandler(1, [spikeSystem](Event& event) {
        auto* spikeEvent = dynamic_cast<SpikeEvent*>(&event);
        if (spikeEvent) {
            nlm::SpikeEvent convertedEvent(
                spikeEvent->source_neuron,
                spikeEvent->timestamp,
                spikeEvent->step,
                spikeEvent->synapticDrive,
                spikeEvent->isEmergent
            );
            spikeSystem->queueSpike(convertedEvent);
        }
    });
    
    eventSystem.registerEventHandler(2, [spikeSystem](Event& event) {
        auto* delayedEvent = dynamic_cast<DelayedSpikeEvent*>(&event);
        if (delayedEvent) {
            nlm::DelayedSpikeEvent convertedEvent(
                delayedEvent->source_neuron,
                delayedEvent->destination_neuron,
                delayedEvent->synapse_id,
                delayedEvent->weight,
                delayedEvent->synapse_type,
                delayedEvent->timestamp,
                delayedEvent->delivery_time,
                delayedEvent->step,
                delayedEvent->delivery_step
            );
            spikeSystem->queueDelayedSpike(convertedEvent);
        }
    });
}

// Function to convert SpikeSystem's DetailedSpikeEvent to EventSystem Event
inline std::unique_ptr<Event> convertDetailedSpikeEventToEvent(
    const DetailedSpikeEvent& detailedEvent,
    EventSystem& eventSystem
) {
    return eventSystem.createDetailedSpikeEvent(detailedEvent);
}

// Main integration function: Replace queue-based approach with EventSystem
// This function demonstrates how to migrate from the existing queue-based SpikeSystem
to the new event-driven architecture
inline bool migrateToEventDrivenArchitecture(
    Brain* brain,
    const std::shared_ptr<Config>& config
) {
    if (!brain) return false;
    
    // Create EventSystem
    auto eventSystem = createEventSystemForBrain(brain, config);
    if (!eventSystem) return false;
    
    // Get existing SpikeSystem
    auto* spikeSystem = brain->getSpikeSystem();
    if (!spikeSystem) return false;
    
    // Integrate them
    integrateSpikeSystemWithEventSystem(spikeSystem, *eventSystem);
    
    // Store EventSystem in Brain if Brain supports it
    // In a complete implementation, Brain would have an EventSystem member
    // For now, we'll just return the event system for use by the main loop
    
    return true;
}

// Function to demonstrate the new event-driven computation approach
inline void demonstrateEventDrivenComputation(EventSystem& eventSystem) {
    if (eventSystem.hasError()) return;
    
    // Create and schedule spike events
    auto spikeEvent1 = eventSystem.createSpikeEvent(
        NeuronId(1), 0.1, 1, 1.5, false
    );
    eventSystem.scheduleEvent(std::move(spikeEvent1));
    
    // Create and schedule delayed spike events
    auto delayedEvent1 = eventSystem.createDelayedSpikeEvent(
        NeuronId(1), NeuronId(2), SynapseId(1),
        0.8f, SynapseType::Excitatory,
        0.1, 0.2, 1, 2 // source, dest, synapse, weight, type, ts, delivery, step, delivery_step
    );
    eventSystem.scheduleEventDelayed(std::move(delayedEvent1), 0.1);
    
    // Create and schedule detailed spike events
    DetailedSpikeEvent detailedEvent;
    detailedEvent.source = NeuronId(3);
    detailedEvent.timestamp = 0.3;
    detailedEvent.step = 2;
    detailedEvent.regionId = RegionId(1);
    detailedEvent.populationId = PopulationId(1);
    
    auto detailedEvent1 = eventSystem.createDetailedSpikeEvent(detailedEvent);
    eventSystem.scheduleEvent(std::move(detailedEvent1));
    
    // Process events for current step
    eventSystem.processEventsForStep(1);
    
    // Process events for next step
    eventSystem.processEventsForStep(2);
}

// Utility function to get EventSystem statistics
inline std::string getEventSystemStats(EventSystem& eventSystem) {
    return eventSystem.getEventSystemStats();
}

// Function to update event system (advance time)
inline void updateEventSystem(EventSystem& eventSystem, TimestepDuration dt) {
    eventSystem.update(dt);
}

// Function to check if event system is healthy
inline bool isEventSystemHealthy(EventSystem& eventSystem) {
    return checkEventSystemHealth(eventSystem);
}

} // namespace nlm
