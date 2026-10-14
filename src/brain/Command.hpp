#pragma once

#include <string>
#include <memory>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;

// BrainCommand base class - implementing the Command Pattern
class BrainCommand {
public:
    virtual ~BrainCommand() = default;
    
    // Execute the command
    virtual void execute() = 0;
    
    // Get command name for identification
    virtual std::string getName() const = 0;
    
    // Check if command is undoable
    virtual bool isUndoable() const { return false; }
    
    // Undo the command
    virtual void undo() {}
};

// BrainCommandBuilder - Builder pattern for commands
class BrainCommandBuilder {
public:
    template<typename CommandType, typename... Args>
    static std::unique_ptr<BrainCommand> build(Args&&... args) {
        return std::make_unique<CommandType>(std::forward<Args>(args)...);
    }
};

// Command categories for organization
enum class CommandCategory {
    Configuration,
    Simulation,
    Memory,
    SensorMotor,
    Plasticity,
    Cognitive,
    Development,
    Neuromodulation
};

} // namespace nlm
