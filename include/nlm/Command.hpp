#pragma once

#include "CommonBaseClasses.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * Command pattern implementation for actions
 * Represents an action that can be executed, undone, or redone
 */
class ActionCommand : public Command {
public:
    virtual ~ActionCommand() = default;
    
    // Execute the command
    void execute() override = 0;
    
    // Undo the command
    void undo() override = 0;
    
    // Redo the command
    void redo() override = 0;
    
    // Check if the command can be undone
    bool canUndo() const override { return canUndoImpl(); }
    
    // Check if the command can be redone
    bool canRedo() const override { return canRedoImpl(); }
    
protected:
    virtual bool canUndoImpl() const = 0;
    virtual bool canRedoImpl() const = 0;
};

/**
 * Simulation command for controlling simulation state
 */
class SimulationCommand : public ActionCommand {
public:
    virtual ~SimulationCommand() = default;
    
    // Start the simulation
    virtual void start() = 0;
    
    // Stop the simulation
    virtual void stop() = 0;
    
    // Step the simulation forward
    virtual void step() = 0;
    
    // Pause the simulation
    virtual void pause() = 0;
};

/**
 * Brain state command for modifying brain state
 */
class BrainStateCommand : public ActionCommand {
public:
    virtual ~BrainStateCommand() = default;
    
    // Set the brain state to the specified value
    virtual void setState(const std::string& state) = 0;
    
    // Get the current brain state
    virtual std::string getState() const = 0;
};

/**
 * Real implementation of simulation command
 */
class SimulationCommandImpl : public SimulationCommand {
public:
    SimulationCommandImpl() : executed(false), undone(false) {}
    
    void execute() override;
    void undo() override;
    void redo() override;
    bool canUndoImpl() const override { return executed && !undone; }
    bool canRedoImpl() const override { return executed && undone; }
    
    void start() override;
    void stop() override;
    void step() override;
    void pause() override;
    
private:
    bool executed;
    bool undone;
};
}