#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <iostream>
#include "core/Config/Config.hpp"

namespace nlm {

/**
 * CLI command types
 */
enum class CLICommand {
    Help,
    Version,
    Simulate,
    Benchmark,
    Test,
    Analyze,
    Export,
    Config,
    Check,
    Interactive,
    Custom
};

/**
 * Base command handler interface
 */
class CLICommandHandler {
public:
    virtual ~CLICommandHandler() = default;
    virtual bool execute(const std::vector<std::string>& args) = 0;
    virtual void printHelp() const = 0;
    virtual CLICommand getCommandType() const = 0;
};

/**
 * Command factory for creating command handlers
 */
class CLICommandFactory {
public:
    virtual ~CLICommandFactory() = default;
    virtual std::unique_ptr<CLICommandHandler> createHandler(CLICommand command) = 0;
};

/**
 * Default CLI command factory implementation
 */
class DefaultCLICommandFactory : public CLICommandFactory {
private:
    std::unordered_map<CLICommand, std::function<std::unique_ptr<CLICommandHandler>()>> handlers_;
    
public:
    DefaultCLICommandFactory() {
        handlers_[CLICommand::Help] = []() { return std::make_unique<HelpCommandHandler>(); };
        handlers_[CLICommand::Version] = []() { return std::make_unique<VersionCommandHandler>(); };
        handlers_[CLICommand::Simulate] = []() { return std::make_unique<SimulateCommandHandler>(); };
        handlers_[CLICommand::Benchmark] = []() { return std::make_unique<BenchmarkCommandHandler>(); };
        handlers_[CLICommand::Test] = []() { return std::make_unique<TestCommandHandler>(); };
        handlers_[CLICommand::Analyze] = []() { return std::make_unique<AnalyzeCommandHandler>(); };
        handlers_[CLICommand::Export] = []() { return std::make_unique<ExportCommandHandler>(); };
    }
    
    std::unique_ptr<CLICommandHandler> createHandler(CLICommand command) override {
        auto it = handlers_.find(command);
        if (it != handlers_.end()) {
            return it->second();
        }
        return nullptr;
    }
};

/**
 * Command handler implementations - declarations only
 */
class HelpCommandHandler;
class VersionCommandHandler;
class SimulateCommandHandler;
class BenchmarkCommandHandler;
class TestCommandHandler;
class AnalyzeCommandHandler;
class ExportCommandHandler;

/**
 * Enhanced CLI Handler with factory pattern
 */
class CLIHandler {
private:
    std::vector<std::string> args_;
    std::unique_ptr<CLICommandFactory> factory_;
    
public:
    CLIHandler(int argc, char** argv);
    ~CLIHandler() = default;
    
    bool parseCommandLine();
    static void printBanner();
    
    // Set custom factory for testing or extensions
    void setFactory(std::unique_ptr<CLICommandFactory> factory) {
        factory_ = std::move(factory);
    }
};

// Command handler forward declarations for implementation in .cpp file
class HelpCommandHandler : public CLICommandHandler {
public:
    void printHelp() const override;
    CLICommand getCommandType() const override { return CLICommand::Help; }
    bool execute(const std::vector<std::string>& args) override;
};

class VersionCommandHandler : public CLICommandHandler {
public:
    void printHelp() const override;
    CLICommand getCommandType() const override { return CLICommand::Version; }
    bool execute(const std::vector<std::string>& args) override;
};

class SimulateCommandHandler : public CLICommandHandler {
public:
    void printHelp() const override;
    CLICommand getCommandType() const override { return CLICommand::Simulate; }
    bool execute(const std::vector<std::string>& args) override;
};

class BenchmarkCommandHandler : public CLICommandHandler {
public:
    void printHelp() const override;
    CLICommand getCommandType() const override { return CLICommand::Benchmark; }
    bool execute(const std::vector<std::string>& args) override;
};

class TestCommandHandler : public CLICommandHandler {
public:
    void printHelp() const override;
    CLICommand getCommandType() const override { return CLICommand::Test; }
    bool execute(const std::vector<std::string>& args) override;
};

class AnalyzeCommandHandler : public CLICommandHandler {
public:
    void printHelp() const override;
    CLICommand getCommandType() const override { return CLICommand::Analyze; }
    bool execute(const std::vector<std::string>& args) override;
};

class ExportCommandHandler : public CLICommandHandler {
public:
    void printHelp() const override;
    CLICommand getCommandType() const override { return CLICommand::Export; }
    bool execute(const std::vector<std::string>& args) override;
};

} // namespace nlm
