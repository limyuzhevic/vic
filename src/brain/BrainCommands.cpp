#include "BrainCommands.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>

namespace nlm {

// ConfigurationCommand implementation
ConfigurationCommand::ConfigurationCommand(Brain* brain, const std::string& key, const std::string& value)
    : brain(brain), key(key), value(value), wasGet(false) {}

ConfigurationCommand::ConfigurationCommand(Brain* brain, const std::string& key)
    : brain(brain), key(key), wasGet(true) {}

void ConfigurationCommand::execute() {
    if (wasGet) {
        // Get operation - we need to store the old value for undo
        // This is simplified - in a real implementation, you'd access config directly
        NLM_LOG_INFO("ConfigurationCommand: Getting key " + key);
        // In a real implementation, we'd get the value from brain->getConfig()
    } else {
        NLM_LOG_INFO("ConfigurationCommand: Setting key " + key + " to " + value);
        // In a real implementation, we'd set the value on brain->getConfig()
    }
}

void ConfigurationCommand::undo() {
    if (wasGet) {
        NLM_LOG_INFO("ConfigurationCommand: Undo get operation for key " + key);
    } else {
        NLM_LOG_INFO("ConfigurationCommand: Undo set operation for key " + key);
    }
}

// SimulationCommand implementation
SimulationCommand::SimulationCommand(Brain* brain, SimulationStep step, Timestamp time)
    : brain(brain), step(step), time(time), oldSpikesThisStep(0), oldTotalSpikes(0) {}

SimulationCommand::SimulationCommand(Brain* brain)
    : brain(brain), step(0), time(0.0), oldSpikesThisStep(0), oldTotalSpikes(0) {}

void SimulationCommand::execute() {
    if (step == 0 && time == 0.0) {
        // Reset operation
        oldSpikesThisStep = brain->getPendingSpikeEventCount();
        oldTotalSpikes = brain->getTotalSpikeCount();
        NLM_LOG_INFO("SimulationCommand: Reset brain state");
        brain->reset();
    } else {
        NLM_LOG_INFO("SimulationCommand: Stepping to step " + std::to_string(step) + ", time " + std::to_string(time));
        brain->step(step, time);
    }
}

void SimulationCommand::undo() {
    NLM_LOG_INFO("SimulationCommand: Undo simulation step");
}

// MemoryCommand implementation
MemoryCommand::MemoryCommand(Brain* brain, const std::string& operation, size_t value)
    : brain(brain), operation(operation), value(value), oldValue(0) {}

void MemoryCommand::execute() {
    if (operation == "clear") {
        oldValue = brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0;
        NLM_LOG_INFO("MemoryCommand: Clearing working memory");
        if (brain->getWorkingMemory()) brain->getWorkingMemory()->clear();
    } else if (operation == "consolidate") {
        NLM_LOG_INFO("MemoryCommand: Consolidating memory with threshold " + std::to_string(value));
        if (brain->getEpisodicMemory()) brain->getEpisodicMemory()->consolidate(value);
    }
}

void MemoryCommand::undo() {
    NLM_LOG_INFO("MemoryCommand: Undo memory operation " + operation);
}

// SensorMotorCommand implementation
SensorMotorCommand::SensorMotorCommand(Brain* brain, const class SensoryInput& input)
    : brain(brain), input(input), isSensory(true), isNeuron(false) {}

SensorMotorCommand::SensorMotorCommand(Brain* brain, NeuronId neuron, MembranePotential current)
    : brain(brain), neuronId(neuron), current(current), isSensory(false), isNeuron(true) {}

SensorMotorCommand::SensorMotorCommand(Brain* brain, NeuronType type, MembranePotential current)
    : brain(brain), neuronType(type), current(current), isSensory(false), isNeuron(false) {}

void SensorMotorCommand::execute() {
    if (isSensory) {
        NLM_LOG_INFO("SensorMotorCommand: Processing sensory input");
        brain->receiveSensoryInput(input);
    } else if (isNeuron) {
        NLM_LOG_INFO("SensorMotorCommand: Injecting current " + std::to_string(current) + " into neuron " + std::to_string(neuronId));
        brain->injectCurrent(neuronId, current);
    } else {
        NLM_LOG_INFO("SensorMotorCommand: Injecting current " + std::to_string(current) + " to neurons of type " + std::to_string(static_cast<int>(neuronType)));
        brain->injectCurrentToNeurons(neuronType, current);
    }
}

// PlasticityCommand implementation
PlasticityCommand::PlasticityCommand(Brain* brain) : brain(brain) {}

void PlasticityCommand::execute() {
    NLM_LOG_INFO("PlasticityCommand: Updating plasticity rules");
    brain->updatePlasticity();
}

// CognitiveCommand implementation
CognitiveCommand::CognitiveCommand(Brain* brain, const std::string& operation) : brain(brain), operation(operation) {}

void CognitiveCommand::execute() {
    NLM_LOG_INFO("CognitiveCommand: Executing cognitive operation " + operation);
    if (brain->getPlanner() && operation == "plan") {
        // In a real implementation, this would be more complex
    }
}

// DevelopmentCommand implementation
DevelopmentCommand::DevelopmentCommand(Brain* brain, DevelopmentalStage stage) : brain(brain), stage(stage), oldStage(stage) {}

void DevelopmentCommand::execute() {
    NLM_LOG_INFO("DevelopmentCommand: Setting developmental stage to " + std::to_string(static_cast<int>(stage)));
    brain->setDevelopmentalStage(stage);
    // In a real implementation, this would also trigger development updates
}
}

void DevelopmentCommand::undo() {
    NLM_LOG_INFO("DevelopmentCommand: Undo developmental stage change");
    brain->setDevelopmentalStage(oldStage);
}

// NeuromodulationCommand implementation
NeuromodulationCommand::NeuromodulationCommand(Brain* brain, const class Neuromodulator& signal) : brain(brain), signal(signal) {}

void NeuromodulationCommand::execute() {
    NLM_LOG_INFO("NeuromodulationCommand: Applying neuromodulation signal with level " + std::to_string(signal.getLevel()));
    brain->applyNeuromodulation(signal);
}

} // namespace nlm
