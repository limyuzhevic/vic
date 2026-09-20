#pragma once

#include "CommandLineParser.h"
#include "CommandRegistry.h"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../advanced/PowerUserAPI.hpp"
#include "../advanced/AdvancedSimulationController.hpp"
#include "../advanced/AdvancedDebugger.hpp"
#include <memory>
#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <thread>
#include <chrono>

namespace nlm {

// CLI option definitions for the new advanced interface
struct AdvancedCLIOptions {
    // Runtime control commands
    struct RuntimeControl {
        std::string pause;
        std::string resume;
        std::string step;
        std::string reset;
        std::string status;
        std::string help;
    } runtime;
    
    // State management
    struct StateManagement {
        std::string save;
        std::string load;
        std::string checkpoint;
        std::string replay;
    } state;
    
    // Configuration management
    struct Configuration {
        std::string set;
        std::string validate;
        std::string templateList;
        std::string templateApply;
    } config;
    
    // Performance monitoring
    struct Performance {
        std::string monitor;
        std::string profile;
        std::string metrics;
        std::string resources;
    } perf;
    
    // Visualization and output
    struct Output {
        std::string verbose;
        std::string format;
        std::string exportResults;
        std::string generateReport;
    } output;
    
    // Stimulus and control
    struct Control {
        std::string injectStimulus;
        std::string speed;
        std::string batch;
        std::string stepByStep;
    } control;
    
    AdvancedCLIOptions() {
        // Initialize with default command names
        runtime.pause = "pause";
        runtime.resume = "resume";
        runtime.step = "step";
        runtime.reset = "reset";
        runtime.status = "status";
        runtime.help = "help";
        
        state.save = "save";
        state.load = "load";
        state.checkpoint = "checkpoint";
        state.replay = "replay";
        
        config.set = "set";
        config.validate = "validate";
        config.templateList = "template-list";
        config.templateApply = "apply-template";
        
        perf.monitor = "monitor";
        perf.profile = "profile";
        perf.metrics = "metrics";
        perf.resources = "resources";
        
        output.verbose = "verbose";
        output.format = "format";
        output.exportResults = "export";
        output.generateReport = "report";
        
        control.injectStimulus = "inject";
        control.speed = "speed";
        control.batch = "batch";
        control.stepByStep = "step";
    }
};

// Enhanced CLI controller integrating all CLI features
class CLIController {
public:
    CLIController(std::shared_ptr<Brain> brain,
                  std::shared_ptr<Config> config,
                  std::shared_ptr<PowerUserAPI> powerUserAPI,
                  std::shared_ptr<AdvancedSimulationController> controller,
                  std::shared_ptr<AdvancedDebugger> debugger);
    ~CLIController();
    
    // Initialize CLI with brain and components
    bool initialize();
    
    // Process a command line input
    bool processCommandLine(int argc, char** argv);
    
    // Execute a runtime command
    bool executeCommand(const std::string& commandName,
                       const std::vector<std::string>& args);
    
    // Check if command exists
    bool hasCommand(const std::string& commandName) const;
    
    // Get command help
    std::string getCommandHelp(const std::string& commandName) const;
    
    // Get all available commands
    std::vector<std::string> getAvailableCommands() const;
    
    // Get current simulation status
    std::string getStatus() const;
    
    // Get performance metrics
    std::string getPerformanceReport() const;
    
    // Get configuration report
    std::string getConfigurationReport() const;
    
    // Get network connectivity report
    std::string getNetworkReport() const;
    
    // Update configuration
    bool updateConfiguration(const std::string& key, const ConfigValue& value);
    
    // Validate current configuration
    bool validateConfiguration() const;
    
    // Export results
    bool exportResults(const std::string& format, const std::string& filepath) const;
    
    // Generate report
    bool generateReport(const std::string& type, const std::string& filepath) const;
    
    // Check if simulation is paused
    bool isPaused() const;
    
    // Check if debugging is enabled
    bool isDebugging() const;
    
    // Get current timestep
    float getTimestep() const;
    
    // Get current step count
    size_t getStepCount() const;
    
    // Get current simulation time
    double getSimulationTime() const;
    
    // Get current performance stats
    PerformanceStats getPerformanceStats() const;
    
    // Cleanup resources
    void cleanup();
    
    // Set output stream
    void setOutputStream(std::ostream* outputStream);
    
    // Set error stream
    void setErrorStream(std::ostream* errorStream);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool setupCommandRegistry();
    bool setupArgumentParser();
    bool setupAdvancedOptions();
    bool registerRuntimeCommands();
    bool registerStateCommands();
    bool registerConfigCommands();
    bool registerPerformanceCommands();
    bool registerOutputCommands();
    bool registerControlCommands();
    
    // Command handlers
    CommandResult handlePauseCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleResumeCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleStepCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleResetCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleStatusCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleHelpCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    
    CommandResult handleSaveCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleLoadCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleCheckpointCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleReplayCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    
    CommandResult handleSetCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleValidateCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleTemplateListCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleTemplateApplyCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    
    CommandResult handleMonitorCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleProfileCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleMetricsCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleResourcesCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    
    CommandResult handleVerboseCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleFormatCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleExportCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleReportCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    
    CommandResult handleInjectCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleSpeedCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleBatchCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    CommandResult handleStepByStepCommand(const std::vector<std::string>& args, CommandExecutionContext& context);
    
    // Helper methods for parsing values
    bool parseFloat(const std::string& str, float& value) const;
    bool parseInt(const std::string& str, int& value) const;
    bool parseString(const std::string& str, std::string& value) const;
    bool parseBool(const std::string& str, bool& value) const;
    
    // Helper methods for validation
    bool validateNeuronId(const std::string& str, NeuronId& neuronId) const;
    bool validateSynapseId(const std::string& str, SynapseId& synapseId) const;
    bool validatePopulationId(const std::string& str, PopulationId& populationId) const;
    bool validateRegionId(const std::string& str, RegionId& regionId) const;
    bool validateStimulusPattern(const std::string& str, StimulusPattern& pattern) const;
    bool validateReplayMode(const std::string& str, ReplayMode& mode) const;
    bool validateNeuromodulationType(const std::string& str, NeuromodulationType& type) const;
    bool validatePlasticityType(const std::string& str, PlasticityType& type) const;
    bool validateEventFilterCategory(const std::string& str, EventFilterCategory& category) const;
    bool validateEventType(const std::string& str, EventType& type) const;
    bool validateDevelopmentalStage(const std::string& str, DevelopmentalStage& stage) const;
};