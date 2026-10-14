#pragma once

#include "Command.hpp"
#include "Brain.hpp"
#include <string>
#include <chrono>

namespace nlm {

// Configuration Command - handles configuration operations
class ConfigurationCommand : public BrainCommand {
public:
    ConfigurationCommand(Brain* brain, const std::string& key, const std::string& value);
    ConfigurationCommand(Brain* brain, const std::string& key);
    
    void execute() override;
    std::string getName() const override { return "ConfigurationCommand"; }
    bool isUndoable() const override { return true; }
    void undo() override;
    
private:
    Brain* brain;
    std::string key;
    std::string value;
    std::string oldValue;
    bool wasGet;
};

// Simulation Command - handles simulation operations
class SimulationCommand : public BrainCommand {
public:
    SimulationCommand(Brain* brain, SimulationStep step, Timestamp time);
    SimulationCommand(Brain* brain);
    
    void execute() override;
    std::string getName() const override { return "SimulationCommand"; }
    bool isUndoable() const override { return true; }
    void undo() override;
    
private:
    Brain* brain;
    SimulationStep step;
    Timestamp time;
    SimulationStep oldStep;
    Timestamp oldTime;
    size_t oldSpikesThisStep;
    size_t oldTotalSpikes;
};

// Memory Command - handles memory operations
class MemoryCommand : public BrainCommand {
public:
    MemoryCommand(Brain* brain, const std::string& operation, size_t value = 0);
    
    void execute() override;
    std::string getName() const override { return "MemoryCommand"; }
    bool isUndoable() const override { return true; }
    void undo() override;
    
private:
    Brain* brain;
    std::string operation;
    size_t value;
    size_t oldValue;
};

// SensorMotor Command - handles sensory and motor operations
class SensorMotorCommand : public BrainCommand {
public:
    SensorMotorCommand(Brain* brain, const class SensoryInput& input);
    SensorMotorCommand(Brain* brain, NeuronId neuron, MembranePotential current);
    SensorMotorCommand(Brain* brain, NeuronType type, MembranePotential current);
    
    void execute() override;
    std::string getName() const override { return "SensorMotorCommand"; }
    bool isUndoable() const override { return false; }
    
private:
    Brain* brain;
    class SensoryInput input;
    NeuronId neuronId;
    MembranePotential current;
    NeuronType neuronType;
    bool isSensory;
    bool isNeuron;
};

// Plasticity Command - handles plasticity operations
class PlasticityCommand : public BrainCommand {
public:
    PlasticityCommand(Brain* brain);
    
    void execute() override;
    std::string getName() const override { return "PlasticityCommand"; }
    bool isUndoable() const override { return false; }
    
private:
    Brain* brain;
};

// Cognitive Command - handles cognitive operations
class CognitiveCommand : public BrainCommand {
public:
    CognitiveCommand(Brain* brain, const std::string& operation);
    
    void execute() override;
    std::string getName() const override { return "CognitiveCommand"; }
    bool isUndoable() const override { return false; }
    
private:
    Brain* brain;
    std::string operation;
};

// Development Command - handles development operations
class DevelopmentCommand : public BrainCommand {
public:
    DevelopmentCommand(Brain* brain, DevelopmentalStage stage);
    
    void execute() override;
    std::string getName() const override { return "DevelopmentCommand"; }
    bool isUndoable() const override { return true; }
    void undo() override;
    
private:
    Brain* brain;
    DevelopmentalStage stage;
    DevelopmentalStage oldStage;
};

// Neuromodulation Command - handles neuromodulation operations
class NeuromodulationCommand : public BrainCommand {
public:
    NeuromodulationCommand(Brain* brain, const class Neuromodulator& signal);
    
    void execute() override;
    std::string getName() const override { return "NeuromodulationCommand"; }
    bool isUndoable() const override { return false; }
    
private:
    Brain* brain;
    class Neuromodulator signal;
    float oldLTPWeight;
    float oldLTDWeight;
};

} // namespace nlm
