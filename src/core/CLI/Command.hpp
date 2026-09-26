#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <stdexcept>

namespace nlm {

class Config;
class Brain;
class Command;

using CommandCallback = std::function<bool(const std::vector<std::string>&)>;

class Command {
public:
    virtual ~Command() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual bool execute(const std::vector<std::string>& args) = 0;
};

class CommandRegistry {
public:
    static CommandRegistry& getInstance() {
        static CommandRegistry instance;
        return instance;
    }
    
    void registerCommand(std::unique_ptr<Command> cmd) {
        commands_[cmd->getName()] = std::move(cmd);
    }
    
    Command* getCommand(const std::string& name) {
        auto it = commands_.find(name);
        return it != commands_.end() ? it->second.get() : nullptr;
    }
    
    std::vector<std::string> getCommandNames() const {
        std::vector<std::string> names;
        names.reserve(commands_.size());
        for (const auto& pair : commands_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void printHelp() const {
        std::cout << "Available commands:" << std::endl;
        for (const auto& pair : commands_) {
            std::cout << "  " << pair.first << " - " << pair.second->getDescription() << std::endl;
        }
    }

private:
    CommandRegistry() = default;
    std::unordered_map<std::string, std::unique_ptr<Command>> commands_;
};

class CommandBase : public Command {
protected:
    std::string name_;
    std::string description_;
    std::string usage_;
    
public:
    CommandBase(const std::string& name, const std::string& description, const std::string& usage)
        : name_(name), description_(description), usage_(usage) {}
    
    std::string getName() const override { return name_; }
    std::string getDescription() const override { return description_; }
    std::string getUsage() const override { return usage_; }
    
    bool execute(const std::vector<std::string>& args) override {
        return executeImpl(args);
    }
    
    virtual bool executeImpl(const std::vector<std::string>& args) = 0;
};

class CLIApp {
public:
    CLIApp(const std::string& name, const std::string& version, const std::string& description)
        : name_(name), version_(version), description_(description) {}
    
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
    }
    
    bool run(int argc, char** argv) {
        if (argc < 2) {
            printHelp();
            return false;
        }
        
        std::string commandName = argv[1];
        Command* cmd = CommandRegistry::getInstance().getCommand(commandName);
        
        if (!cmd) {
            std::cout << "Unknown command: " << commandName << std::endl;
            std::cout << "Use 'help' to see available commands." << std::endl;
            printHelp();
            return false;
        }
        
        std::vector<std::string> args;
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        std::cout << "Running command: " << commandName << std::endl;
        bool result = cmd->execute(args);
        
        if (result) {
            std::cout << "Command completed successfully." << std::endl;
        } else {
            std::cout << "Command failed." << std::endl;
        }
        
        return result;
    }
    
    void printHelp() const {
        std::cout << name_ << " - " << description_ << std::endl;
        std::cout << "Version: " << version_ << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: " << name_ << " <command> [options]" << std::endl;
        std::cout << std::endl;
        
        CommandRegistry::getInstance().printHelp();
    }
    
    bool executeCommand(const std::string& commandName, const std::vector<std::string>& args) {
        Command* cmd = CommandRegistry::getInstance().getCommand(commandName);
        if (!cmd) {
            std::cout << "Unknown command: " << commandName << std::endl;
            return false;
        }
        
        return cmd->execute(args);
    }

private:
    std::string name_;
    std::string version_;
    std::string description_;
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm
