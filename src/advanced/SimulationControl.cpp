#include "SimulationControl.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <fstream>
#include <chrono>

namespace nlm {

// SimulationEvent implementation
SimulationEvent::SimulationEvent(EventType type, Timestamp timestamp, SimulationStep step)
    : type(type), timestamp(timestamp), step(step) {
}

void SimulationEvent::process(std::vector<NeuralRegion*>& regions) {
    // Default event processing - override in derived classes
}

std::string SimulationEvent::getEventDescription() const {
    return "Generic Event";
}

// SpikeEvent implementation (would need to be in separate header if extended)
// For now, we'll use the base SimulationEvent

// SimulationLoopController implementation
SimulationLoopController::SimulationLoopController()
    : phase(SimulationPhase::Initialization),
      currentTime(0.0), currentStep(0),
      timeStepSize(0.001), maxTime(std::numeric_limits<Timestamp>::max()),
      minTime(0.0), simulationMode(SimulationMode::TimeStepDriven),
      timeSteppingAlgorithm(TimeSteppingAlgorithm::Fixed),
      enableAdaptiveTimeStepFlag(false), stabilityThreshold(0.01f),
      maxTimeStepChange(0.5f), eventDrivenEnabled(false),
      maxEventsPerStep(100), avgTimePerStep(0.0f), simulationSpeed(1.0f),
      stepCount(0) {
}

SimulationLoopController::~SimulationLoopController() {
    // Cleanup
}

bool SimulationLoopController::initialize(std::vector<NeuralRegion*>& regions) {
    // Initialize simulation with regions
    phase = SimulationPhase::Running;
    currentStep = 0;
    currentTime = minTime;
    stepCount = 0;
    
    // Initialize regions
    for (auto* region : regions) {
        if (region) {
            region->reset();
        }
    }
    
    return true;
}

bool SimulationLoopController::step(std::vector<NeuralRegion*>& regions, Timestamp currentTime, SimulationStep step) {
    if (phase != SimulationPhase::Running) {
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Pre-step callback
    if (preStepCallback) {
        preStepCallback(regions);
    }
    
    // Update time
    this->currentTime = currentTime;
    currentStep = step;
    
    // Process events
    processEvents(regions);
    
    // Step regions
    for (auto* region : regions) {
        if (region) {
            region->step(currentTime);
        }
    }
    
    // Post-step callback
    if (postStepCallback) {
        postStepCallback(regions);
    }
    
    // Update timing stats
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    updateTimingStats(duration / 1000000.0f);
    
    stepCount++;
    
    return true;
}

bool SimulationLoopController::terminate(std::vector<NeuralRegion*>& regions) {
    phase = SimulationPhase::Terminated;
    
    // Cleanup regions
    for (auto* region : regions) {
        if (region) {
            region->reset();
        }
    }
    
    return true;
}

bool SimulationLoopController::pause(std::vector<NeuralRegion*>& regions) {
    phase = SimulationPhase::Paused;
    return true;
}

bool SimulationLoopController::resume(std::vector<NeuralRegion*>& regions) {
    phase = SimulationPhase::Running;
    return true;
}

void SimulationLoopController::setSimulationMode(SimulationMode mode) {
    simulationMode = mode;
    eventDrivenEnabled = (mode == SimulationMode::EventDriven || mode == SimulationMode::Hybrid);
}

void SimulationLoopController::setTimeSteppingAlgorithm(TimeSteppingAlgorithm algorithm) {
    timeSteppingAlgorithm = algorithm;
}

void SimulationLoopController::setTimeStepSize(TimestepDuration dt) {
    timeStepSize = dt;
}

void SimulationLoopController::setMaxTime(Timestamp max) {
    maxTime = max;
}

void SimulationLoopController::setMinTime(Timestamp min) {
    minTime = min;
}

void SimulationLoopController::enableEventDriven(bool enable) {
    eventDrivenEnabled = enable;
    if (enable && simulationMode != SimulationMode::EventDriven) {
        simulationMode = SimulationMode::EventDriven;
    }
}

void SimulationLoopController::setMaxEventsPerStep(size_t maxEvents) {
    maxEventsPerStep = maxEvents;
}

void SimulationLoopController::addEventHandler(std::unique_ptr<EventHandler> handler) {
    if (handler) {
        eventHandlers.push_back(std::move(handler));
    }
}

void SimulationLoopController::removeEventHandler(const std::string& handlerName) {
    auto it = std::remove_if(eventHandlers.begin(), eventHandlers.end(),
                           [&handlerName](const std::unique_ptr<EventHandler>& handler) {
                               return handler && handler->getHandlerName() == handlerName;
                           });
    eventHandlers.erase(it, eventHandlers.end());
}

void SimulationLoopController::addCustomEvent(std::unique_ptr<SimulationEvent> event) {
    if (event && events.size() < maxEventsPerStep) {
        events.push_back(std::move(event));
        sortEventsByTime();
    }
}

void SimulationLoopController::clearEvents() {
    events.clear();
}

void SimulationLoopController::processEvents(std::vector<NeuralRegion*>& regions) {
    if (!eventDrivenEnabled || events.empty()) {
        return;
    }
    
    // Process events for current time step
    auto it = events.begin();
    while (it != events.end()) {
        if ((*it)->getStep() <= currentStep) {
            // Process event
            (*it)->process(regions);
            
            // Notify event handlers
            for (auto& handler : eventHandlers) {
                if (handler && handler->getHandledEventType() == (*it)->getType()) {
                    handler->handleEvent(*(*it), regions);
                }
            }
            
            it = events.erase(it);
        } else {
            ++it;
        }
    }
}

void SimulationLoopController::sortEventsByTime() {
    std::sort(events.begin(), events.end(),
              [](const std::unique_ptr<SimulationEvent>& a, const std::unique_ptr<SimulationEvent>& b) {
                  return a->getStep() < b->getStep();
              });
}

void SimulationLoopController::updateTimingStats(float stepTime) {
    const float alpha = 0.1f; // Smoothing factor
    avgTimePerStep = (1.0f - alpha) * avgTimePerStep + alpha * stepTime;
    
    if (stepCount > 0) {
        simulationSpeed = timeStepSize / avgTimePerStep;
    }
}

void SimulationLoopController::resetTimingStats() {
    avgTimePerStep = 0.0f;
    simulationSpeed = 1.0f;
    stepCount = 0;
}

void SimulationLoopController::setPreStepCallback(std::function<void(std::vector<NeuralRegion*>&)> callback) {
    preStepCallback = callback;
}

void SimulationLoopController::setPostStepCallback(std::function<void(std::vector<NeuralRegion*>&)> callback) {
    postStepCallback = callback;
}

void SimulationLoopController::setPhaseChangeCallback(std::function<void(SimulationPhase, SimulationPhase)> callback) {
    phaseChangeCallback = callback;
}

bool SimulationLoopController::saveCheckpoint(const std::string& filepath, std::vector<NeuralRegion*>& regions) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Save checkpoint data (simplified)
    file << "Checkpoint not fully implemented";
    file.close();
    return true;
}

bool SimulationLoopController::loadCheckpoint(const std::string& filepath, std::vector<NeuralRegion*>& regions) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Load checkpoint data (simplified)
    file.close();
    return true;
}

bool SimulationLoopController::createAutoCheckpoint(const std::string& directory, std::vector<NeuralRegion*>& regions) {
    // Create automatic checkpoint
    return true;
}

void SimulationLoopController::enableAdaptiveTimeStep(bool enable) {
    enableAdaptiveTimeStepFlag = enable;
}

void SimulationLoopController::setStabilityThreshold(float threshold) {
    stabilityThreshold = threshold;
}

void SimulationLoopController::setMaxTimeStepChange(float maxChange) {
    maxTimeStepChange = maxChange;
}

bool SimulationLoopController::shouldChangeTimeStep(const std::vector<NeuralRegion*>& regions) {
    if (!enableAdaptiveTimeStepFlag) return false;
    
    // Check if adaptive time stepping should be used
    return calculateOptimalTimeStep(regions) != timeStepSize;
}

float SimulationLoopController::calculateOptimalTimeStep(const std::vector<NeuralRegion*>& regions) {
    // Calculate optimal time step based on neural dynamics
    float optimalDt = timeStepSize;
    
    for (const auto* region : regions) {
        if (region) {
            auto neurons = region->getAllNeurons();
            for (const auto* neuron : neurons) {
                if (neuron) {
                    // Calculate stability condition for this neuron
                    float neuronStability = AdvancedTimeStepper::calculateNeuronStability(neuron, timeStepSize);
                    if (neuronStability < optimalDt) {
                        optimalDt = neuronStability;
                    }
                }
            }
        }
    }
    
    // Limit change rate
    float maxChange = maxTimeStepChange * timeStepSize;
    float change = std::abs(optimalDt - timeStepSize);
    if (change > maxChange) {
        optimalDt = timeStepSize + (optimalDt > timeStepSize ? maxChange : -maxChange);
    }
    
    return optimalDt;
}

void SimulationLoopController::triggerEvent(EventType type, Timestamp timestamp) {
    // Create and add event
    // Implementation would need access to event types
}

void SimulationLoopController::triggerSpikeEvent(NeuronId neuronId, Timestamp timestamp) {
    // Trigger spike event
}

void SimulationLoopController::triggerPlasticityEvent(SynapseId synapseId, Timestamp timestamp) {
    // Trigger plasticity event
}

void SimulationLoopController::triggerNeuromodulationEvent(Timestamp timestamp) {
    // Trigger neuromodulation event
}

void SimulationLoopController::triggerDevelopmentEvent(DevelopmentalStage newStage, Timestamp timestamp) {
    // Trigger development event
}

std::map<std::string, float> SimulationLoopController::getPerformanceStats() const {
    std::map<std::string, float> stats;
    stats["avg_time_per_step"] = avgTimePerStep;
    stats["simulation_speed"] = simulationSpeed;
    stats["step_count"] = static_cast<float>(stepCount);
    return stats;
}

std::map<std::string, size_t> SimulationLoopController::getEventStats() const {
    std::map<std::string, size_t> stats;
    stats["events_in_queue"] = events.size();
    stats["event_handlers"] = eventHandlers.size();
    return stats;
}

std::string SimulationLoopController::getSimulationReport() const {
    std::string report = "Simulation Report\n";
    report += "==================\n";
    report += "Phase: " + std::to_string(static_cast<int>(phase)) + "\n";
    report += "Current Time: " + std::to_string(currentTime) + "s\n";
    report += "Current Step: " + std::to_string(currentStep) + "\n";
    report += "Time Step: " + std::to_string(timeStepSize) + "s\n";
    report += "Simulation Mode: " + std::to_string(static_cast<int>(simulationMode)) + "\n";
    report += "Events in Queue: " + std::to_string(events.size()) + "\n";
    report += "Average Time per Step: " + std::to_string(avgTimePerStep) + "s\n";
    report += "Simulation Speed: " + std::to_string(simulationSpeed) + "x\n";
    return report;
}

// AdvancedTimeStepper implementation
AdvancedTimeStepper::AdvancedTimeStepper()
    : stabilityThreshold(0.01f), errorTolerance(0.001f), maxTimeStepChange(0.1f) {
}

AdvancedTimeStepper::~AdvancedTimeStepper() {
    // Cleanup
}

Timestamp AdvancedTimeStepper::advanceTime(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                           TimeSteppingAlgorithm algorithm, TimestepDuration dt) {
    switch (algorithm) {
        case TimeSteppingAlgorithm::Fixed:
            return advanceFixed(currentTime, dt);
        case TimeSteppingAlgorithm::Adaptive:
            return advanceAdaptive(currentTime, regions, dt, stabilityThreshold);
        case TimeSteppingAlgorithm::RungeKutta4:
            return advanceRungeKutta4(currentTime, regions, dt);
        case TimeSteppingAlgorithm::RungeKutta45:
            return advanceRungeKutta45(currentTime, regions, dt);
        case TimeSteppingAlgorithm::AdamsBashforth:
            return advanceAdamsBashforth(currentTime, regions, dt);
        case TimeSteppingAlgorithm::BackwardEuler:
            return advanceBackwardEuler(currentTime, regions, dt);
        case TimeSteppingAlgorithm::ExponentialTimeDifferencing:
            return advanceExponentialTimeDifferencing(currentTime, regions, dt);
        case TimeSteppingAlgorithm::NeuralSpecific:
            return advanceNeuralSpecific(currentTime, regions, dt);
        default:
            return advanceFixed(currentTime, dt);
    }
}

Timestamp AdvancedTimeStepper::advanceFixed(Timestamp currentTime, TimestepDuration dt) {
    return currentTime + dt;
}

Timestamp AdvancedTimeStepper::advanceAdaptive(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                               TimestepDuration dt, float stabilityThreshold) {
    // Adaptive time stepping based on stability
    float stability = calculateNetworkStability(regions, dt);
    
    if (stability < stabilityThreshold) {
        // Reduce time step for stability
        dt *= 0.5f;
    } else if (stability > stabilityThreshold * 1.5f) {
        // Can increase time step
        dt *= 1.1f;
    }
    
    // Limit change rate
    float maxChange = maxTimeStepChange * dt;
    // Simple change limitation (would need more sophisticated logic)
    
    return currentTime + dt;
}

Timestamp AdvancedTimeStepper::advanceRungeKutta4(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                                  TimestepDuration dt) {
    // Runge-Kutta 4th order integration
    // This is a simplified placeholder
    return currentTime + dt;
}

Timestamp AdvancedTimeStepper::advanceRungeKutta45(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                                   TimestepDuration dt) {
    // Runge-Kutta-Fehlberg 4(5) integration
    // This is a simplified placeholder
    return currentTime + dt;
}

Timestamp AdvancedTimeStepper::advanceAdamsBashforth(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                                     TimestepDuration dt) {
    // Adams-Bashforth multi-step method
    // This is a simplified placeholder
    return currentTime + dt;
}

Timestamp AdvancedTimeStepper::advanceBackwardEuler(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                                   TimestepDuration dt) {
    // Backward Euler implicit method
    // This is a simplified placeholder
    return currentTime + dt;
}

Timestamp AdvancedTimeStepper::advanceExponentialTimeDifferencing(Timestamp currentTime,
                                                                   const std::vector<NeuralRegion*>& regions,
                                                                   TimestepDuration dt) {
    // Exponential time differencing
    // This is a simplified placeholder
    return currentTime + dt;
}

Timestamp AdvancedTimeStepper::advanceNeuralSpecific(Timestamp currentTime, const std::vector<NeuralRegion*>& regions,
                                                    TimestepDuration dt) {
    // Neural-specific time stepping
    // This is a simplified placeholder
    return currentTime + dt;
}

void AdvancedTimeStepper::setAlgorithmParameters(TimeSteppingAlgorithm algorithm,
                                                 const std::map<std::string, float>& parameters) {
    algorithmParameters[algorithm] = parameters;
}

std::map<std::string, float> AdvancedTimeStepper::getAlgorithmParameters(TimeSteppingAlgorithm algorithm) const {
    auto it = algorithmParameters.find(algorithm);
    if (it != algorithmParameters.end()) {
        return it->second;
    }
    return {};
}

bool AdvancedTimeStepper::checkStability(const std::vector<NeuralRegion*>& regions, TimeSteppingAlgorithm algorithm) {
    // Check stability based on algorithm
    switch (algorithm) {
        case TimeSteppingAlgorithm::Fixed:
            return true;
        case TimeSteppingAlgorithm::Adaptive:
            return calculateNetworkStability(regions, 0.001f) > stabilityThreshold;
        default:
            return true;
    }
}

float AdvancedTimeStepper::estimateError(const std::vector<NeuralRegion*>& regions, TimeSteppingAlgorithm algorithm,
                                        TimestepDuration dt) {
    // Estimate integration error
    return 0.01f; // Placeholder
}

float AdvancedTimeStepper::adjustTimeStepBasedOnError(TimestepDuration currentDt, float error, float tolerance) {
    // Adjust time step based on error
    if (error > tolerance) {
        return currentDt * 0.5f;
    } else if (error < tolerance * 0.1f) {
        return currentDt * 1.2f;
    }
    return currentDt;
}

float AdvancedTimeStepper::calculateNeuronStability(const Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return dt;
    
    // Calculate stability based on neuron parameters
    float tau = 10.0f; // Time constant
    float stability = tau * std::log(1.0f + dt / tau);
    return stability;
}

float AdvancedTimeStepper::calculateSynapseStability(const Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return dt;
    
    // Calculate stability based on synapse parameters
    float weight = synapse->getWeight();
    float stability = std::abs(weight) * dt;
    return std::max(0.001f, stability);
}

float AdvancedTimeStepper::calculateNetworkStability(const std::vector<NeuralRegion*>& regions, TimestepDuration dt) {
    float totalStability = 0.0f;
    int neuronCount = 0;
    
    for (const auto* region : regions) {
        if (region) {
            auto neurons = region->getAllNeurons();
            for (const auto* neuron : neurons) {
                if (neuron) {
                    totalStability += calculateNeuronStability(neuron, dt);
                    neuronCount++;
                }
            }
        }
    }
    
    return neuronCount > 0 ? totalStability / neuronCount : dt;
}

// EventManager implementation
EventManager::EventManager()
    : realTimeProcessing(false), maxEventsPerSecond(1000), eventQueueSize(10000),
      priorityQueueEnabled(false), enableCompression(false), compressionLevel(5),
      totalEventsProcessed(0), avgEventProcessingTime(0.0f) {
}

EventManager::~EventManager() {
    // Cleanup
}

void EventManager::addEvent(std::unique_ptr<SimulationEvent> event) {
    if (event && eventsQueue.size() < eventQueueSize) {
        eventsQueue.push_back(std::move(event));
        if (priorityQueueEnabled) {
            // Re-sort if priority queue
            std::sort(eventsQueue.begin(), eventsQueue.end(),
                     [](const std::unique_ptr<SimulationEvent>& a, const std::unique_ptr<SimulationEvent>& b) {
                         return a->getStep() < b->getStep();
                     });
        }
    }
}

void EventManager::addScheduledEvent(std::unique_ptr<SimulationEvent> event, Timestamp delay) {
    if (event) {
        // Schedule event for future time
        event->step += static_cast<SimulationStep>(delay / 0.001f); // Assuming 1ms timestep
        addEvent(std::move(event));
    }
}

void EventManager::addRecurringEvent(std::unique_ptr<SimulationEvent> event, Timestamp interval,
                                     size_t repetitions) {
    if (event && repetitions > 0) {
        // Make event recurring (simplified implementation)
        event->step = 0; // Reset step for recurring events
        addEvent(std::move(event));
    }
}

std::vector<std::unique_ptr<SimulationEvent>> EventManager::getEventsToProcess(Timestamp currentTime,
                                                                               SimulationStep currentStep) {
    std::vector<std::unique_ptr<SimulationEvent>> eventsToProcess;
    
    for (auto& event : eventsQueue) {
        if (event && event->getStep() <= currentStep) {
            eventsToProcess.push_back(std::move(event));
        }
    }
    
    // Remove processed events from queue
    eventsQueue.erase(std::remove_if(eventsQueue.begin(), eventsQueue.end(),
                                   [&currentStep](const std::unique_ptr<SimulationEvent>& event) {
                                       return event && event->getStep() <= currentStep;
                                   }),
                    eventsQueue.end());
    
    return eventsToProcess;
}

void EventManager::processEvent(std::unique_ptr<SimulationEvent> event, std::vector<NeuralRegion*>& regions) {
    if (!event) return;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Process event
    event->process(regions);
    
    // Update statistics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    updateEventStats(*event, duration / 1000000.0f);
    
    totalEventsProcessed++;
}

void EventManager::processAllPendingEvents(std::vector<NeuralRegion*>& regions, Timestamp currentTime,
                                          SimulationStep currentStep) {
    auto eventsToProcess = getEventsToProcess(currentTime, currentStep);
    
    for (auto& event : eventsToProcess) {
        processEvent(std::move(event), regions);
    }
}

void EventManager::scheduleSpikeEvent(NeuronId neuronId, Timestamp delay) {
    // Schedule spike event
    // Implementation would need SpikeEvent class
}

void EventManager::scheduleSynapseEvent(SynapseId synapseId, Timestamp delay) {
    // Schedule synapse event
}

void EventManager::schedulePlasticityEvent(SynapseId synapseId, Timestamp delay) {
    // Schedule plasticity event
}

void EventManager::scheduleNeuromodulationEvent(Timestamp delay) {
    // Schedule neuromodulation event
}

void EventManager::scheduleDevelopmentEvent(DevelopmentalStage newStage, Timestamp delay) {
    // Schedule development event
}

void EventManager::addEventHandler(std::unique_ptr<EventHandler> handler) {
    if (handler) {
        eventHandlers.push_back(std::move(handler));
    }
}

void EventManager::removeEventHandler(const std::string& handlerName) {
    auto it = std::remove_if(eventHandlers.begin(), eventHandlers.end(),
                           [&handlerName](const std::unique_ptr<EventHandler>& handler) {
                               return handler && handler->getHandlerName() == handlerName;
                           });
    eventHandlers.erase(it, eventHandlers.end());
}

void EventManager::addEventFilter(std::function<bool(const SimulationEvent&)> filter) {
    if (filter) {
        eventFilters.push_back(filter);
    }
}

void EventManager::clearEventFilters() {
    eventFilters.clear();
}

void EventManager::setEventProcessingMode(bool realTime) {
    realTimeProcessing = realTime;
}

void EventManager::setMaxEventsPerSecond(size_t maxEvents) {
    maxEventsPerSecond = maxEvents;
}

void EventManager::setEventQueueSize(size_t maxQueueSize) {
    eventQueueSize = maxQueueSize;
}

void EventManager::enablePriorityQueue(bool enable) {
    priorityQueueEnabled = enable;
}

std::map<std::string, size_t> EventManager::getEventTypeStats() const {
    std::map<std::string, size_t> stats;
    for (const auto& pair : eventTypeCounts) {
        stats[std::to_string(static_cast<int>(pair.first))] = pair.second;
    }
    return stats;
}

std::map<std::string, float> EventManager::getEventTimeStats() const {
    std::map<std::string, float> stats;
    for (const auto& pair : eventProcessingTimes) {
        stats[std::to_string(static_cast<int>(pair.first))] = pair.second;
    }
    return stats;
}

std::string EventManager::getEventReport() const {
    std::string report = "Event Manager Report\n";
    report += "====================\n";
    report += "Events in Queue: " + std::to_string(eventsQueue.size()) + "\n";
    report += "Total Events Processed: " + std::to_string(totalEventsProcessed) + "\n";
    report += "Average Processing Time: " + std::to_string(avgEventProcessingTime) + "s\n";
    report += "Max Events per Second: " + std::to_string(maxEventsPerSecond) + "\n";
    report += "Event Queue Size: " + std::to_string(eventQueueSize) + "\n";
    report += "Priority Queue Enabled: " + std::to_string(priorityQueueEnabled) + "\n";
    return report;
}

bool EventManager::saveEventLog(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << getEventReport();
    file.close();
    return true;
}

bool EventManager::loadEventLog(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Load event log (simplified)
    file.close();
    return true;
}

std::string EventManager::exportEventDataAsText() const {
    std::string data = "Event Data\n";
    data += "==========\n";
    for (const auto& pair : eventTypeCounts) {
        data += "Event Type " + std::to_string(static_cast<int>(pair.first)) + 
                ": " + std::to_string(pair.second) + " times\n";
    }
    return data;
}

std::string EventManager::exportEventDataAsJSON() const {
    std::string json = "{\"events\": [";
    bool first = true;
    for (const auto& pair : eventTypeCounts) {
        if (!first) json += ", ";
        json += "{\"type\": " + std::to_string(static_cast<int>(pair.first)) +
                ", \"count\": " + std::to_string(pair.second) + "}";
        first = false;
    }
    json += "]}";
    return json;
}

void EventManager::enableCustomEventProcessing(std::function<void(std::unique_ptr<SimulationEvent>)> processor) {
    customEventProcessor = processor;
}

void EventManager::setEventCompression(bool enable, size_t compressionLevel) {
    enableCompression = enable;
    this->compressionLevel = compressionLevel;
}

void EventManager::enableEventPrioritization(const std::vector<EventType>& priorityOrder) {
    this->priorityOrder = priorityOrder;
}

bool EventManager::reorderEventQueue(const std::vector<size_t>& newOrder) {
    if (newOrder.size() != eventsQueue.size()) {
        return false;
    }
    
    std::vector<std::unique_ptr<SimulationEvent>> reordered;
    reordered.reserve(eventsQueue.size());
    
    for (size_t index : newOrder) {
        if (index < eventsQueue.size()) {
            reordered.push_back(std::move(eventsQueue[index]));
        }
    }
    
    eventsQueue = std::move(reordered);
    return true;
}

float EventManager::calculateEventInterarrivalTime(EventType eventType) const {
    auto it = eventTimestamps.find(eventType);
    if (it == eventTimestamps.end() || it->second.size() < 2) {
        return 0.0f;
    }
    
    std::vector<float> interarrivals;
    for (size_t i = 1; i < it->second.size(); ++i) {
        interarrivals.push_back(it->second[i] - it->second[i-1]);
    }
    
    float sum = std::accumulate(interarrivals.begin(), interarrivals.end(), 0.0f);
    return interarrivals.empty() ? 0.0f : sum / interarrivals.size();
}

float EventManager::calculateEventProcessingDuration(EventType eventType) const {
    auto it = eventProcessingTimes.find(eventType);
    if (it != eventProcessingTimes.end()) {
        return it->second;
    }
    return 0.0f;
}

size_t EventManager::countEventOccurrences(EventType eventType, Timestamp startTime, Timestamp endTime) const {
    auto it = eventTimestamps.find(eventType);
    if (it == eventTimestamps.end()) return 0;
    
    size_t count = 0;
    for (float timestamp : it->second) {
        if (timestamp >= startTime && timestamp <= endTime) {
            count++;
        }
    }
    return count;
}

std::vector<Timestamp> EventManager::getEventTimestamps(EventType eventType) const {
    auto it = eventTimestamps.find(eventType);
    if (it != eventTimestamps.end()) {
        return it->second;
    }
    return {};
}

void EventManager::processScheduledEvents(Timestamp currentTime, SimulationStep currentStep) {
    // Process scheduled events
}

void EventManager::processRecurringEvents(Timestamp currentTime, SimulationStep currentStep) {
    // Process recurring events
}

void EventManager::cleanupCompletedRecurringEvents(Timestamp currentTime, SimulationStep currentStep) {
    // Cleanup recurring events
}

void EventManager::updateEventStats(const SimulationEvent& event, float processingTime) {
    eventTypeCounts[event.getType()]++;
    eventProcessingTimes[event.getType()] += processingTime;
    
    // Update average
    float alpha = 0.1f;
    avgEventProcessingTime = (1.0f - alpha) * avgEventProcessingTime + alpha * processingTime;
    
    eventTimestamps[event.getType()].push_back(event.getTimestamp());
}

// ObservationModel implementations
FIRObservationModel::FIRObservationModel()
    : filterCoefficients({0.1f, 0.2f, 0.3f, 0.2f, 0.1f}), 
      filterOrder(5), stateVector(5, 0.0f) {
}

float FIRObservationModel::observe(const Neuron* neuron) const {
    if (!neuron) return 0.0f;
    
    // Simple FIR filter implementation
    float observation = neuron->getFiringRate(); // Assuming neuron has this method
    // Apply FIR filter (simplified)
    return observation;
}

float FIRObservationModel::observe(const Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    return synapse->getWeight();
}

float FIRObservationModel::observe(const NeuralRegion* region) const {
    if (!region) return 0.0f;
    
    return region->getAverageFiringRate();
}

void FIRObservationModel::update(float dt) {
    // Update filter state
    // Simplified implementation
}

void FIRObservationModel::setParameters(const std::map<std::string, float>& params) {
    for (const auto& param : params) {
        if (param.first == "filter_order") {
            filterOrder = static_cast<size_t>(param.second);
            filterCoefficients.resize(filterOrder);
            stateVector.resize(filterOrder);
        } else if (param.first == "coeff_0") {
            filterCoefficients[0] = param.second;
        } // etc...
    }
}

std::map<std::string, float> FIRObservationModel::getParameters() const {
    std::map<std::string, float> params;
    params["filter_order"] = static_cast<float>(filterOrder);
    params["coeff_0"] = filterCoefficients[0];
    return params;
}

// IFObservationModel implementation
IFObservationModel::IFObservationModel()
    : cutoffFrequency(10.0f), gainFactor(1.0f), stateVariable(0.0f) {
}

float IFObservationModel::observe(const Neuron* neuron) const {
    if (!neuron) return 0.0f;
    
    // Simple IF model implementation
    float membranePotential = neuron->getMembranePotential(); // Assuming neuron has this method
    float threshold = neuron->getThreshold(); // Assuming neuron has this method
    
    return gainFactor * (1.0f - std::exp(-cutoffFrequency * membranePotential / threshold));
}

float IFObservationModel::observe(const Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    return synapse->getWeight() * gainFactor;
}

float IFObservationModel::observe(const NeuralRegion* region) const {
    if (!region) return 0.0f;
    
    return region->getAverageFiringRate() * gainFactor;
}

void IFObservationModel::update(float dt) {
    // Update IF model state
    stateVariable += cutoffFrequency * dt;
}

void IFObservationModel::setParameters(const std::map<std::string, float>& params) {
    for (const auto& param : params) {
        if (param.first == "cutoff_frequency") {
            cutoffFrequency = param.second;
        } else if (param.first == "gain") {
            gainFactor = param.second;
        }
    }
}

std::map<std::string, float> IFObservationModel::getParameters() const {
    std::map<std::string, float> params;
    params["cutoff_frequency"] = cutoffFrequency;
    params["gain"] = gainFactor;
    return params;
}

// KalmanFilterObservationModel implementation
KalmanFilterObservationModel::KalmanFilterObservationModel()
    : processNoise(0.01f), measurementNoise(0.1f), initialCovariance(1.0f),
      stateEstimate(0.0f), stateCovariance(1.0f) {
}

float KalmanFilterObservationModel::observe(const Neuron* neuron) const {
    if (!neuron) return 0.0f;
    
    // Kalman filter observation
    float measurement = neuron->getFiringRate(); // Assuming neuron has this method
    
    // Simple Kalman filter update
    float innovation = measurement - stateEstimate;
    float kalmanGain = stateCovariance / (stateCovariance + measurementNoise);
    
    return stateEstimate + kalmanGain * innovation;
}

float KalmanFilterObservationModel::observe(const Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    return synapse->getWeight();
}

float KalmanFilterObservationModel::observe(const NeuralRegion* region) const {
    if (!region) return 0.0f;
    
    return region->getAverageFiringRate();
}

void KalmanFilterObservationModel::update(float dt) {
    // Kalman filter prediction and update
    stateCovariance += processNoise * dt;
}

void KalmanFilterObservationModel::setParameters(const std::map<std::string, float>& params) {
    for (const auto& param : params) {
        if (param.first == "process_noise") {
            processNoise = param.second;
        } else if (param.first == "measurement_noise") {
            measurementNoise = param.second;
        } else if (param.first == "initial_covariance") {
            initialCovariance = param.second;
        }
    }
}

std::map<std::string, float> KalmanFilterObservationModel::getParameters() const {
    std::map<std::string, float> params;
    params["process_noise"] = processNoise;
    params["measurement_noise"] = measurementNoise;
    params["initial_covariance"] = initialCovariance;
    return params;
}

// CustomNeuralObservationModel implementation
CustomNeuralObservationModel::CustomNeuralObservationModel(
    std::function<float(const Neuron*)> neuronObs,
    std::function<float(const Synapse*)> synapseObs,
    std::function<float(const NeuralRegion*)> regionObs)
    : neuronObs(neuronObs), synapseObs(synapseObs), regionObs(regionObs) {
}

float CustomNeuralObservationModel::observe(const Neuron* neuron) const {
    return neuronObs ? neuronObs(neuron) : 0.0f;
}

float CustomNeuralObservationModel::observe(const Synapse* synapse) const {
    return synapseObs ? synapseObs(synapse) : 0.0f;
}

float CustomNeuralObservationModel::observe(const NeuralRegion* region) const {
    return regionObs ? regionObs(region) : 0.0f;
}

void CustomNeuralObservationModel::update(float dt) {
    // Custom update logic
}

void CustomNeuralObservationModel::setParameters(const std::map<std::string, float>& params) {
    parameters = params;
}

std::map<std::string, float> CustomNeuralObservationModel::getParameters() const {
    return parameters;
}

} // namespace nlm
