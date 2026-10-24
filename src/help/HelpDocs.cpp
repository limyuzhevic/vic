#include "HelpDocs.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace nlm {
namespace help {

// CLIHelp implementation
std::string CLIHelp::generateCommandHelp() const {
    std::ostringstream oss;
    oss << "=== NLM Command Line Interface ===" << std::endl;
    oss << std::endl;
    
    oss << "NLM (熙然) - Neural Learning Machine" << std::endl;
    oss << "=====================================" << std::endl;
    oss << std::endl;
    oss << "The NLM command line interface provides a unified interface for running neural" << std::endl;
    oss << "simulations, managing experiments, and analyzing results." << std::endl;
    oss << std::endl;
    oss << "Available commands:" << std::endl;
    oss << "  nlm run <EXPERIMENT> [OPTIONS]    Run a neural simulation experiment" << std::endl;
    oss << "  nlm config <COMMAND> [OPTIONS]   Configuration file management" << std::endl;
    oss << "  nlm experiment <COMMAND> [OPTIONS] Experiment management and execution" << std::endl;
    oss << "  nlm utils <UTILITY> [OPTIONS]    Utility functions and development tools" << std::endl;
    oss << std::endl;
    oss << "For more information about a specific command, use:" << std::endl;
    oss << "  nlm <COMMAND> --help" << std::endl;
    oss << "  nlm <SUBCOMMAND> --help" << std::endl;
    
    return oss.str();
}

std::string CLIHelp::generateSubcommandHelp(const std::string& commandName) const {
    if (commandName == "config") {
        return generateConfigHelp();
    } else if (commandName == "experiment") {
        return generateExperimentListHelp();
    } else if (commandName == "utils") {
        return generateUtilsListHelp();
    }
    
    return "Unknown subcommand: " + commandName;
}

std::string CLIHelp::generateExperimentHelp(const std::string& experimentName) const {
    if (experimentName == "phase6" || experimentName == "integration") {
        return generatePhase6Help();
    } else if (experimentName == "phase3" || experimentName == "neural") {
        return generatePhase3Help();
    } else if (experimentName == "phase4" || experimentName == "cognitive") {
        return generatePhase4Help();
    } else if (experimentName == "phase5" || experimentName == "performance") {
        return generatePhase5Help();
    }
    
    return "Unknown experiment: " + experimentName;
}

std::string CLIHelp::generateUtilsHelp(const std::string& utilityName) const {
    if (utilityName == "benchmark") {
        return generateBenchmarkHelp();
    } else if (utilityName == "validate") {
        return generateValidateHelp();
    } else if (utilityName == "stats") {
        return generateStatsHelp();
    }
    
    return "Unknown utility: " + utilityName;
}

std::string CLIHelp::generateConfigHelp() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Management ===" << std::endl;
    oss << std::endl;
    oss << "The nlm config command provides tools for managing configuration files." << std::endl;
    oss << std::endl;
    oss << "Available subcommands:" << std::endl;
    oss << "  show [FILE]      Display configuration contents" << std::endl;
    oss << "  load <INPUT> <OUTPUT>  Load configuration from one file to another" << std::endl;
    oss << "  save <OUTPUT>    Save configuration to file" << std::endl;
    oss << "  validate [FILE]   Validate configuration file" << std::endl;
    oss << "  create <OUTPUT> [PROFILE]  Create configuration from template" << std::endl;
    oss << "  profile [FILE]    Display configuration profile" << std::endl;
    oss << "  templates         List available configuration templates" << std::endl;
    oss << std::endl;
    oss << "For more information about a specific subcommand, use:" << std::endl;
    oss << "  nlm config <SUBCOMMAND> --help" << std::endl;
    
    return oss.str();
}

std::string CLIHelp::generateExperimentListHelp() const {
    std::ostringstream oss;
    oss << "=== NLM Experiment Management ===" << std::endl;
    oss << std::endl;
    oss << "The nlm experiment command provides tools for managing neural simulations." << std::endl;
    oss << std::endl;
    oss << "Available subcommands:" << std::endl;
    oss << "  run <NAME> [OPTIONS]    Run an experiment" << std::endl;
    oss << "  list [PATTERN]          List available experiments" << std::endl;
    oss << "  save <NAME> <OUTPUT>    Save experiment results" << std::endl;
    oss << "  load <INPUT> [OUTPUT]   Load experiment results" << std::endl;
    oss << "  compare <FILE1> <FILE2>  Compare experiment results" << std::endl;
    oss << "  analyze <FILE> [OPTIONS] Analyze experiment results" << std::endl;
    oss << std::endl;
    oss << "For more information about a specific subcommand, use:" << std::endl;
    oss << "  nlm experiment <SUBCOMMAND> --help" << std::endl;
    
    return oss.str();
}

std::string CLIHelp::generateUtilsListHelp() const {
    std::ostringstream oss;
    oss << "=== NLM Utilities ===" << std::endl;
    oss << std::endl;
    oss << "The nlm utils command provides various utility functions for development and analysis." << std::endl;
    oss << std::endl;
    oss << "Available utilities:" << std::endl;
    oss << "  benchmark [OPTIONS]      Run performance benchmarks" << std::endl;
    oss << "  validate [FILE]          Validate configuration files" << std::endl;
    oss << "  optimize [OPTIONS]       Optimize configuration parameters" << std::endl;
    oss << "  profile [OPTIONS]        Run performance profiling" << std::endl;
    oss << "  monitor [OPTIONS]        Start real-time monitoring" << std::endl;
    oss << "  convert <FILE> <FORMAT>  Convert configuration formats" << std::endl;
    oss << "  export <FILE> [OPTIONS]  Export results to various formats" << std::endl;
    oss << "  import <FILE> [OPTIONS]  Import results from various formats" << std::endl;
    oss << "  stats [FILE]             Collect and analyze statistics" << std::endl;
    oss << "  compare <FILE1> <FILE2>  Compare experiment results" << std::endl;
    oss << "  convertformat <FILE> <OUTPUT> [FORMAT]  Convert configuration format" << std::endl;
    oss << "  batch <ARGS>             Run batch processing" << std::endl;
    oss << std::endl;
    oss << "For more information about a specific utility, use:" << std::endl;
    oss << "  nlm utils <UTILITY> --help" << std::endl;
    
    return oss.str();
}

std::string CLIHelp::generatePhase1Help() const {
    std::ostringstream oss;
    oss << "=== Phase 1: Project Skeleton ===" << std::endl;
    oss << std::endl;
    oss << "Phase 1 establishes the foundation for the NLM project:" << std::endl;
    oss << std::endl;
    oss << "Key Features:" << std::endl;
    oss << "  • Project skeleton and basic structure" << std::endl;
    oss << "  • Core types and configuration system" << std::endl;
    oss << "  • Neural interfaces and communication" << std::endl;
    oss << "  • Basic logging and configuration loading" << std::endl;
    oss << std::endl;
    oss << "Typical Usage:" << std::endl;
    oss << "  nlm run phase1 --config=configs/phase1.cfg" << std::endl;
    oss << std::endl;
    oss << "Notes:" << std::endl;
    oss << "  This phase is mainly for setup and foundation. Real neural computation" << std::endl;
    oss << "  begins in Phase 2." << std::endl;
    
    return oss.str();
}

std::string CLIHelp::generatePhase2Help() const {
    std::ostringstream oss;
    oss << "=== Phase 2: Real Neural Computation ===" << std::endl;
    oss << std::endl;
    oss << "Phase 2 implements real spiking neural computation with:" << std::endl;
    oss << std::endl;
    oss << "Key Features:" << std::endl;
    oss << "  • Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
    oss << "  • Event-driven spike propagation with synaptic delays" << std::endl;
    oss << "  • STDP and Hebbian plasticity rules" << std::endl;
    oss << "  • Structural plasticity (synaptogenesis/pruning)" << std::endl;
    oss << std::endl;
    oss << "Typical Usage:" << std::endl;
    oss << "  nlm run phase2 --config=configs/phase2.cfg --steps=1000" << std::endl;
    oss << std::endl;
    oss << "Note:" << std::endl;
    oss << "  This phase provides the core neural computation engine for NLM." << std::endl;
    
    return oss.str();
}

std::string CLIHelp::generatePhase3Help() const {
    std::ostringstream oss;
    oss << "=== Phase 3: Cognitive Systems ===" << std::endl;
    oss << std::endl;
    oss << "Phase 3 implements advanced cognitive mechanisms:" << std::endl;
    oss << std::endl;
    oss << "Key Features:" << std::endl;
    oss << "  • Neural prediction system" << std::endl;
    oss << "  • Working memory" << std::endl;
    oss << "  • Episodic-like memory" << std::endl;
    oss << "  • Concept formation" << std::endl;
    oss << "  • Neural attention (NOT Transformer)" << std::endl;
    oss << "  • Predictive planning" << std::endl;
    oss << "  • Self-model" << std::endl;
    oss << "  • Social learning" << std::endl;
    oss << "  • Continual learning" << std::endl;
    oss << std::endl;
    oss << "Typical Usage:" << std::endl;
    oss << "  nlm run phase3 --config=configs/phase3.cfg --neurons=2000" << std::endl;
    oss << std::endl;
    oss << "Note:" << std::endl;
    oss << "  This phase demonstrates how cognitive systems can emerge from" << std::endl;
    oss << "  neural computation and interaction with the environment." << std::endl;
    
    return oss.str();
}

std::string CLIHelp::generatePhase4Help() const {
    std::ostringstream oss;
    oss << "=== Phase 4: Performance Optimizations ===" << std::endl;
    oss << std::endl;
    oss << "Phase 4 focuses on performance optimizations and scalability:" << std::endl;
    oss << std::endl;
    oss << "Key Features:" << std::endl;
    oss << "  • Performance optimizations" << std::endl;
    oss << "  • Memory pools and event queues" << std::endl;
    oss << "  • SIMD vectorization" << std::endl;
    oss << "  • Parallel processing" << std::endl;
    oss << "  • Checkpoint system" << std::endl;
    oss << std::endl;
    oss << "Typical Usage:" << std::endl;
    oss << "  nlm run phase4 --config=configs/phase4.cfg --steps=2000" << std::endl;
    oss << std::endl;
    oss << "Note:" << std::endl;
    oss << "  This phase prepares the system for large-scale simulations by" << std::endl;
    oss << "  optimizing performance and resource usage." << std::endl;
    
    return oss.str();
}

std::string CLIHelp::generatePhase5Help() const {
    std::ostringstream oss;
    oss << "=== Phase 5: Integration Test ===" << std::endl;
    oss << std::endl;
    oss << "Phase 5 serves as a final integration test before Phase 6:" << std::endl;
    oss << std::endl;
    oss << "Key Features:" << std::endl;
    oss << "  • Complete system integration" << std::endl;
    oss << "  • Cross-component communication" << std::endl;
    oss << "  • Full functionality demonstration" << std::endl;
    oss << "  • Performance benchmarking" << std::endl;
    oss << std::endl;
    oss << "Typical Usage:" << std::endl;
    oss << "  nlm run phase5 --config=configs/phase5.cfg --duration=300" << std::endl;
    oss << std::endl;
    oss << "Note:" << std::endl;
    oss << "  This phase verifies that all components work together correctly" << std::endl;
    oss << "  before proceeding to the final integration phase." << std::endl;
    
    return oss.str();
}

std::string CLIHelp::generatePhase6Help() const {
    std::ostringstream oss;
    oss << "=== Phase 6: Final Integration ===" << std::endl;
    oss << std::endl;
    oss << "Phase 6 represents the final integration of all NLM systems:" << std::endl;
    oss << std::endl;
    oss << "Key Features:" << std::endl;
    oss << "  • All memory systems integrated with neural processing" << std::endl;
    oss << "  • Neuromodulation affects plasticity and neural dynamics" << std::endl;
    oss << "  • Prediction integrated with learning" << std::endl;
    oss << "  • Development affects plasticity rates" << std::endl;
    oss << "  • Checkpoint save/load working" << std::endl;
    oss << "  • Replay and consolidation functional" << std::endl;
    oss << std::endl;
    oss << "Typical Usage:" << std::endl;
    oss << "  nlm run phase6 --config=configs/phase6.cfg --steps=5000" << std::endl;
    oss << std::endl;
    oss << "Note:" << std::endl;
    oss << "  Phase 6 creates a complete, functioning artificial neural substrate" << std::endl;
    oss << "  capable of changing its own synaptic connections through experience." << std::endl;
    
    return oss.str();
}

// Helper function implementations
std::vector<CLIHelp::CommandInfo> CLIHelp::getCommandInfos() {
    return {
        {"run", "Run simulations and experiments", "run <EXPERIMENT> [OPTIONS]", {}, {}},
        {"config", "Configuration file management", "config <COMMAND> [OPTIONS]", {}, {}},
        {"experiment", "Experiment management and execution", "experiment <COMMAND> [OPTIONS]", {}, {}},
        {"utils", "Utility functions and development tools", "utils <UTILITY> [OPTIONS]", {}, {}},
    };
}

std::vector<CLIHelp::SubcommandInfo> CLIHelp::getSubcommandInfos() {
    return {
        {"show", "Display configuration contents", "show [FILE]", {}, {"--help"}},
        {"load", "Load configuration from one file to another", "load <INPUT> <OUTPUT>", {}, {"--help"}},
        {"save", "Save configuration to file", "save <OUTPUT>", {}, {"--help"}},
        {"validate", "Validate configuration file", "validate [FILE]", {}, {"--help"}},
        {"create", "Create configuration from template", "create <OUTPUT> [PROFILE]", {}, {"--help"}},
        {"profile", "Display configuration profile", "profile [FILE]", {}, {"--help"}},
        {"templates", "List available configuration templates", "templates", {}, {}},
        {"run", "Run an experiment", "run <NAME> [OPTIONS]", {}, {"--help"}},
        {"list", "List available experiments", "list [PATTERN]", {}, {"--help"}},
        {"save", "Save experiment results", "save <NAME> <OUTPUT>", {}, {"--help"}},
        {"load", "Load experiment results", "load <INPUT> [OUTPUT]", {}, {"--help"}},
        {"compare", "Compare experiment results", "compare <FILE1> <FILE2>", {}, {"--help"}},
        {"analyze", "Analyze experiment results", "analyze <FILE> [OPTIONS]", {}, {"--help"}},
        {"benchmark", "Run performance benchmarks", "benchmark [OPTIONS]", {}, {"--help"}},
        {"validate", "Validate configuration files", "validate [FILE]", {}, {"--help"}},
        {"optimize", "Optimize configuration parameters", "optimize [OPTIONS]", {}, {"--help"}},
        {"profile", "Run performance profiling", "profile [OPTIONS]", {}, {"--help"}},
        {"monitor", "Start real-time monitoring", "monitor [OPTIONS]", {}, {"--help"}},
        {"convert", "Convert configuration formats", "convert <FILE> <FORMAT>", {}, {"--help"}},
        {"export", "Export results to various formats", "export <FILE> [OPTIONS]", {}, {"--help"}},
        {"import", "Import results from various formats", "import <FILE> [OPTIONS]", {}, {"--help"}},
        {"stats", "Collect and analyze statistics", "stats [FILE]", {}, {"--help"}},
        {"compare", "Compare experiment results", "compare <FILE1> <FILE2>", {}, {"--help"}},
        {"convertformat", "Convert configuration format", "convertformat <FILE> <OUTPUT> [FORMAT]", {}, {"--help"}},
        {"batch", "Run batch processing", "batch <ARGS>", {}, {"--help"}},
    };
}

// ConfigFileHelp implementation
std::string ConfigFileHelp::generateConfigFileHelp(const std::string& format) const {
    if (format == "json") {
        return generateJSONConfigHelp();
    } else if (format == "yaml") {
        return generateYAMLConfigHelp();
    }
    
    return generateCustomConfigHelp();
}

std::string ConfigFileHelp::generateJSONConfigHelp() const {
    std::ostringstream oss;
    oss << "=== JSON Configuration Format ===" << std::endl;
    oss << std::endl;
    oss << "NLM supports JSON (JavaScript Object Notation) configuration files." << std::endl;
    oss << std::endl;
    oss << "Example JSON configuration:" << std::endl;
    oss << "{\n";
    oss << "  \"neuron_count\": 1000,\n";
    oss << "  \"simulation_timestep\": 0.001,\n";
    oss << "  \"region_count\": 1,\n";
    oss << "  \"connection_probability\": 0.1,\n";
    oss << "  \"stdp_ltp_weight\": 0.01,\n";
    oss << "  \"stdp_ltd_weight\": 0.012,\n";
    oss << "  \"checkpoint_dir\": \"./checkpoints\"\n";
    oss << "}" << std::endl;
    oss << std::endl;
    oss << "Key JSON features:" << std::endl;
    oss << "  • Supports nested objects and arrays" << std::endl;
    oss << "  • Type-safe parsing with automatic conversion" << std::endl;
    oss << "  • Comments and formatting ignored" << std::endl;
    oss << "  • Cross-platform compatibility" << std::endl;
    
    return oss.str();
}

std::string ConfigFileHelp::generateYAMLConfigHelp() const {
    std::ostringstream oss;
    oss << "=== YAML Configuration Format ===" << std::endl;
    oss << std::endl;
    oss << "NLM supports YAML (YAML Ain't Markup Language) configuration files." << std::endl;
    oss << std::endl;
    oss << "Example YAML configuration:" << std::endl;
    oss << "neuron_count: 1000\n";
    oss << "simulation_timestep: 0.001\n";
    oss << "region_count: 1\n";
    oss << "connection_probability: 0.1\n";
    oss << "stdp_ltp_weight: 0.01\n";
    oss << "stdp_ltd_weight: 0.012\n";
    oss << "checkpoint_dir: ./checkpoints\n";
    oss << std::endl;
    oss << "Key YAML features:" << std::endl;
    oss << "  • Human-readable and readable syntax" << std::endl;
    oss << "  • Supports complex nested structures" << std::endl;
    oss << "  • Comments directly in file" << std::endl;
    oss << "  • YAML-specific data types (lists, mappings)" << std::endl;
    
    return oss.str();
}

std::string ConfigFileHelp::generateCustomConfigHelp() const {
    std::ostringstream oss;
    oss << "=== Custom Configuration Format ===" << std::endl;
    oss << std::endl;
    oss << "NLM also supports a simple custom key=value format for basic configurations." << std::endl;
    oss << std::endl;
    oss << "Example custom configuration:" << std::endl;
    oss << "neuron_count = 1000\n";
    oss << "simulation_timestep = 0.001\n";
    oss << "region_count = 1\n";
    oss << "connection_probability = 0.1\n";
    oss << "stdp_ltp_weight = 0.01\n";
    oss << "stdp_ltd_weight = 0.012\n";
    oss << "checkpoint_dir = ./checkpoints\n";
    oss << std::endl;
    oss << "Key custom format features:" << std::endl;
    oss << "  • Simple and easy to understand" << std::endl;
    oss << "  • No dependency on external libraries" << std::endl;
    oss << "  • Good for quick prototyping" << std::endl;
    oss << "  • Compatible with existing configuration system" << std::endl;
    
    return oss.str();
}

// QuickStartGuide implementation
std::string QuickStartGuide::generateQuickStart() const {
    std::ostringstream oss;
    oss << "=== NLM Quick Start Guide ===" << std::endl;
    oss << std::endl;
    oss << "This guide will help you get started with NLM (熙然) - the neural" << std::endl;
    oss << "simulation framework for experimental artificial intelligence." << std::endl;
    oss << std::endl;
    oss << "What is NLM?" << std::endl;
    oss << "NLM is a computational brain project that creates artificial neural" << std::endl;
    oss << "systems capable of learning and adapting through experience." << std::endl;
    oss << std::endl;
    oss << "Why Use NLM?" << std::endl;
    oss << "• Experimental and research framework for artificial brains" << std::endl;
    oss << "• Comprehensive neural simulation with multiple brain systems" << std::endl;
    oss << "• Focus on scientific investigation of neural mechanisms" << std::endl;
    oss << "• Modular architecture for easy extension and customization" << std::endl;
    oss << std::endl;
    
    return oss.str();
}

std::vector<QuickStartGuide::QuickStartStep> QuickStartGuide::getQuickStartSteps() {
    return {
        {1, "Install NLM", "Install and set up the NLM framework", "nlm install", "Installation complete"},
        {2, "Create Configuration", "Create a basic configuration file", "nlm config create myconfig.cfg default", "Configuration created"},
        {3, "Run Basic Experiment", "Run a simple neural simulation", "nlm run phase6 --config=myconfig.cfg --steps=100", "Experiment completed"},
        {4, "Analyze Results", "Examine experiment results and statistics", "nlm utils stats results.json", "Statistics analyzed"},
        {5, "Explore Advanced Features", "Explore advanced NLM features", "nlm help plugins", "Advanced features available"},
    };
}

// ErrorHelp implementation
std::string ErrorHelp::generateErrorHelp() const {
    std::ostringstream oss;
    oss << "=== NLM Error Handling Help ===" << std::endl;
    oss << std::endl;
    oss << "NLM provides comprehensive error handling and reporting." << std::endl;
    oss << std::endl;
    oss << "Error Handling Overview:" << std::endl;
    oss << "• All errors are caught and reported with detailed context" << std::endl;
    oss << "• Errors include helpful suggestions and recovery actions" << std::endl;
    oss << "• Errors are categorized by severity and type" << std::endl;
    oss << "• Error reporting includes stack traces and location information" << std::endl;
    oss << std::endl;
    oss << "Error Categories:" << std::endl;
    oss << "• Configuration errors - Invalid or missing configuration" << std::endl;
    oss << "• Neural system errors - Brain, neuron, or synapse issues" << std::endl;
    oss << "• Experiment errors - Experiment loading or execution problems" << std::endl;
    oss << "• Memory system errors - Working memory, episodic memory, etc." << std::endl;
    oss << "• File system errors - File I/O and access problems" << std::endl;
    oss << "• Network errors - Connection and communication issues" << std::endl;
    oss << "• Resource errors - Memory, CPU, or disk issues" << std::endl;
    oss << "• System errors - General system problems" << std::endl;
    oss << "• CLI errors - Command line interface issues" << std::endl;
    oss << std::endl;
    oss << "For more specific error information, use:" << std::endl;
    oss << "  nlm help error <ERROR_CODE>" << std::endl;
    
    return oss.str();
}

std::vector<ErrorHelp::ErrorInfo> ErrorHelp::getErrorInfos() {
    return {
        {"ConfigFileNotFound", "Configuration", "Warning", "Configuration file not found", "Check file path and permissions", "Retry with valid file path"},
        {"ConfigParseError", "Configuration", "Error", "Configuration file parsing error", "Check file format and syntax", "Fix file format or use different parser"},
        {"ConfigValidationError", "Configuration", "Error", "Configuration validation error", "Check field values and types", "Fix configuration values"},
        {"BrainInitializationError", "Neural System", "Critical", "Brain initialization failed", "Check configuration and system resources", "Reset configuration or check system resources"},
        {"ExperimentNotFound", "Experiment", "Warning", "Experiment not found", "Check experiment name and path", "Use existing experiment or create new one"},
        {"FileNotFound", "File System", "Warning", "File not found", "Check file path", "Use absolute path or create file"},
        {"FileReadError", "File System", "Error", "File read error", "Check file permissions and disk space", "Check file permissions"},
        {"FileWriteError", "File System", "Error", "File write error", "Check disk space and permissions", "Free disk space or check permissions"},
        {"MemoryError", "Resource", "Critical", "Memory allocation error", "Free system memory or increase memory limits", "Restart or use smaller simulation"},
        {"SystemError", "System", "Critical", "System error", "Check system logs", "Restart system or contact administrator"},
        {"CliArgumentError", "CLI", "Error", "Command line argument error", "Check command syntax", "Use --help for command syntax"},
        {"CliCommandNotFound", "CLI", "Warning", "Command not found", "Check command spelling", "Use --help to see available commands"},
    };
}

// HelpSystem implementation
std::string HelpSystem::getDefaultHelpContent(HelpType type) {
    switch (type) {
        case HelpType::CommandLine:
            return "NLM Command Line Interface Help";
        case HelpType::Configuration:
            return "NLM Configuration File Help";
        case HelpType::Experiment:
            return "NLM Experiment Help";
        case HelpType::Plugin:
            return "NLM Plugin Help";
        case HelpType::Statistics:
            return "NLM Statistics Help";
        case HelpType::ErrorHandling:
            return "NLM Error Handling Help";
        case HelpType::Complete:
            return "Complete NLM Documentation";
        default:
            return "Help not available";
    }
}

std::string HelpSystem::getDefaultQuickStart() {
    return "NLM Quick Start Guide - Get started with neural simulation in 5 steps.";
}

std::string HelpSystem::getDefaultTutorials() {
    return "NLM Tutorials - Step-by-step guides for using NLM effectively.";
}

} // namespace help
} // namespace nlm