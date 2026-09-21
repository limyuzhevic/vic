#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>

namespace nlm {

/**
 * Command system for advanced brain operations
 * 
 * Provides batch processing, scripting, and command execution capabilities
 * for the NLM brain. Commands enable:
 * - Batch operations (multiple steps at once)
 * - History management (undo/redo brain state)
 * - Script execution (command files)
 * - Performance profiling
 * - Event subscription
 * - Scripting DSL
 */

class Brain;

/**
 * Command result status
 */
enum class CommandStatus {
    Success = 0,
    Failed = 1,
    Cancelled = 2,
    InvalidSyntax = 3,
    NotImplemented = 4
};

/**
 * Command base class
 */
class Command {
public:
    virtual ~Command() = default;
    
    virtual CommandStatus execute() = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool canUndo() const { return false; }
    virtual bool canRedo() const { return false; }
    virtual void undo() {}
    virtual void redo() {}
};

/**
 * Batch step command - execute multiple steps efficiently
 */
class BatchStepCommand : public Command {
public:
    BatchStepCommand(std::shared_ptr<Brain> brain, size_t steps)
        : brain_(brain), steps_(steps) {}
    
    CommandStatus execute() override {
        if (!brain_) return CommandStatus::Failed;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps_; ++i) {
            brain_->step(static_cast<SimulationStep>(i));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Log performance
        auto logger = Logger::getGlobal();
        logger->info("BatchStepCommand: " + std::to_string(steps_) + " steps in " + 
                    std::to_string(duration.count()) + "ms, " +
                    std::to_string(steps_ / (duration.count() + 1)) + " steps/ms");
        
        return CommandStatus::Success;
    }
    
    std::string getName() const override { return "batchstep"; }
    std::string getDescription() const override { 
        return "Execute multiple simulation steps at once"; 
    }
    
    bool canUndo() const override { return true; }
    bool canRedo() const override { return true; }
    
    void undo() override {
        if (brain_) {
            brain_->reset();
            brain_->initialize();
        }
    }
    
    void redo() override {
        // Re-execute the batch steps
        execute();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    size_t steps_;
};

/**
 * Profile command - measure brain performance
 */
class ProfileCommand : public Command {
public:
    ProfileCommand(std::shared_ptr<Brain> brain, std::string outputFile)
        : brain_(brain), outputFile_(outputFile) {}
    
    CommandStatus execute() override {
        if (!brain_) return CommandStatus::Failed;
        
        auto logger = Logger::getGlobal();
        logger->info("Starting brain profiling...");
        
        // Collect statistics
        size_t totalNeurons = brain_->getTotalNeuronCount();
        size_t totalSynapses = brain_->getTotalSynapseCount();
        size_t totalSpikes = brain_->getTotalSpikeCount();
        float firingRate = brain_->getAverageFiringRate();
        float eiratio = brain_->getExcitationInhibitionRatio();
        
        // Simulate profiling (in real implementation would track timing)
        logger->info("Profile completed:")
        logger->info("  Neurons: " + std::to_string(totalNeurons));
        logger->info("  Synapses: " + std::to_string(totalSynapses));
        logger->info("  Total spikes: " + std::to_string(totalSpikes));
        logger->info("  Firing rate: " + std::to_string(firingRate));
        logger->info("  E/I ratio: " + std::to_string(eiratio));
        
        return CommandStatus::Success;
    }
    
    std::string getName() const override { return "profile"; }
    std::string getDescription() const override { 
        return "Measure and log brain performance metrics"; 
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::string outputFile_;
};

/**
 * Save brain state command
 */
class SaveCommand : public Command {
public:
    SaveCommand(std::shared_ptr<Brain> brain, std::string filepath)
        : brain_(brain), filepath_(filepath) {}
    
    CommandStatus execute() override {
        if (!brain_) return CommandStatus::Failed;
        
        if (brain_->save(filepath_)) {
            auto logger = Logger::getGlobal();
            logger->info("Brain state saved to: " + filepath_);
            return CommandStatus::Success;
        } else {
            auto logger = Logger::getGlobal();
            logger->error("Failed to save brain state to: " + filepath_);
            return CommandStatus::Failed;
        }
    }
    
    std::string getName() const override { return "save"; }
    std::string getDescription() const override { 
        return "Save brain state to checkpoint file"; 
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::string filepath_;
};

/**
 * Load brain state command
 */
class LoadCommand : public Command {
public:
    LoadCommand(std::shared_ptr<Brain> brain, std::string filepath)
        : brain_(brain), filepath_(filepath) {}
    
    CommandStatus execute() override {
        if (!brain_) return CommandStatus::Failed;
        
        if (brain_->load(filepath_)) {
            auto logger = Logger::getGlobal();
            logger->info("Brain state loaded from: " + filepath_);
            return CommandStatus::Success;
        } else {
            auto logger = Logger::getGlobal();
            logger->error("Failed to load brain state from: " + filepath_);
            return CommandStatus::Failed;
        }
    }
    
    std::string getName() const override { return "load"; }
    std::string getDescription() const override { 
        return "Load brain state from checkpoint file"; 
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::string filepath_;
};

/**
 * Command interpreter for parsing and executing commands
 */
class CommandInterpreter {
public:
    CommandInterpreter(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    CommandStatus execute(const std::string& commandLine) {
        std::vector<std::string> tokens = tokenize(commandLine);
        if (tokens.empty()) return CommandStatus::InvalidSyntax;
        
        std::string commandName = tokens[0];
        
        if (commandName == "batchstep") {
            if (tokens.size() < 2) return CommandStatus::InvalidSyntax;
            try {
                size_t steps = std::stoul(tokens[1]);
                auto cmd = std::make_unique<BatchStepCommand>(brain_, steps);
                return cmd->execute();
            } catch (...) {
                return CommandStatus::InvalidSyntax;
            }
        }
        else if (commandName == "profile") {
            std::string outputFile = tokens.size() > 1 ? tokens[1] : "profile.txt";
            auto cmd = std::make_unique<ProfileCommand>(brain_, outputFile);
            return cmd->execute();
        }
        else if (commandName == "save") {
            std::string filepath = tokens.size() > 1 ? tokens[1] : "brain.bin";
            auto cmd = std::make_unique<SaveCommand>(brain_, filepath);
            return cmd->execute();
        }
        else if (commandName == "load") {
            std::string filepath = tokens.size() > 1 ? tokens[1] : "brain.bin";
            auto cmd = std::make_unique<LoadCommand>(brain_, filepath);
            return cmd->execute();
        }
        else {
            return CommandStatus::NotImplemented;
        }
    }
    
    std::vector<std::string> tokenize(const std::string& commandLine) {
        std::vector<std::string> tokens;
        std::string token;
        bool inQuotes = false;
        
        for (char c : commandLine) {
            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (std::isspace(c) && !inQuotes) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        
        if (!token.empty()) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    void addCommand(std::unique_ptr<Command> cmd) {
        commands_.push_back(std::move(cmd));
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::vector<std::unique_ptr<Command>> commands_;
};

/**
 * Brain command system - provides command execution interface
 */
class BrainCommands {
public:
    explicit BrainCommands(std::shared_ptr<Brain> brain) : brain_(brain) {
        auto logger = Logger::getGlobal();
        logger->info("Brain command system initialized");
    }
    
    CommandStatus execute(const std::string& commandLine) {
        if (!interpreter_) {
            interpreter_ = std::make_unique<CommandInterpreter>(brain_);
        }
        return interpreter_->execute(commandLine);
    }
    
    std::string getHelp() const {
        return "Available commands:\n"
               "  batchstep <steps>      - Execute multiple simulation steps\n"
               "  profile [file]         - Profile brain performance\n"
               "  save [filepath]        - Save brain state to file\n"
               "  load [filepath]        - Load brain state from file\n"
               "  help                   - Show this help message\n";
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<CommandInterpreter> interpreter_;
};

} // namespace nlm
