// Advanced CLI implementation
#include "interfaces/AdvancedInterface.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Experiment.hpp"
#include "performance/PerformanceMonitor.hpp"
#include "debugging/DebugTools.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

AdvancedCLI::AdvancedCLI() : pImpl(std::make_unique<Impl>()) {}
AdvancedCLI::~AdvancedCLI() = default;

AdvancedCLI::CommandArgs AdvancedCLI::parse(int argc, char** argv) {
    CommandArgs args;
    args.verbose = false;
    args.jsonOutput = false;
    
    if (argc < 2) {
        args.command = Command::HELP;
        return args;
    }

    std::string arg(argv[1]);
    
    // Map string commands to Command enum
    if (arg == "help" || arg == "--help" || arg == "-h") {
        args.command = Command::HELP;
    } else if (arg == "version" || arg == "--version") {
        args.command = Command::VERSION;
    } else if (arg == "config") {
        args.command = Command::CONFIG;
    } else if (arg == "config-validate") {
        args.command = Command::CONFIG_VALIDATE;
    } else if (arg == "config-show") {
        args.command = Command::CONFIG_SHOW;
    } else if (arg == "config-set") {
        args.command = Command::CONFIG_SET;
    } else if (arg == "config-export") {
        args.command = Command::CONFIG_EXPORT;
    } else if (arg == "config-import") {
        args.command = Command::CONFIG_IMPORT;
    } else if (arg == "config-template") {
        args.command = Command::CONFIG_TEMPLATE;
    } else if (arg == "experiment") {
        args.command = Command::EXPERIMENT;
    } else if (arg == "experiment-run") {
        args.command = Command::EXPERIMENT_RUN;
    } else if (arg == "experiment-list") {
        args.command = Command::EXPERIMENT_LIST;
    } else if (arg == "experiment-status") {
        args.command = Command::EXPERIMENT_STATUS;
    } else if (arg == "experiment-stop") {
        args.command = Command::EXPERIMENT_STOP;
    } else if (arg == "monitor") {
        args.command = Command::MONITOR;
    } else if (arg == "monitor-start") {
        args.command = Command::MONITOR_START;
    } else if (arg == "monitor-stop") {
        args.command = Command::MONITOR_STOP;
    } else if (arg == "monitor-metrics") {
        args.command = Command::MONITOR_METRICS;
    } else if (arg == "debug") {
        args.command = Command::DEBUG;
    } else if (arg == "debug-neurons") {
        args.command = Command::DEBUG_NEURONS;
    } else if (arg == "debug-synapses") {
        args.command = Command::DEBUG_SYNAPSES;
    } else if (arg == "debug-memory") {
        args.command = Command::DEBUG_MEMORY;
    } else if (arg == "debug-prediction") {
        args.command = Command::DEBUG_PREDICTION;
    } else if (arg == "debug-neuromodulation") {
        args.command = Command::DEBUG_NEUROMODULATION;
    } else if (arg == "debug-plasticity") {
        args.command = Command::DEBUG_PLASTICITY;
    } else if (arg == "visualization") {
        args.command = Command::VISUALIZATION;
    } else if (arg == "visualization-toggle") {
        args.command = Command::VISUALIZATION_TOGGLE;
    } else if (arg == "visualization-update") {
        args.command = Command::VISUALIZATION_UPDATE;
    } else if (arg == "visualization-export") {
        args.command = Command::VISUALIZATION_EXPORT;
    } else if (arg == "performance") {
        args.command = Command::PERFORMANCE;
    } else if (arg == "performance-profile") {
        args.command = Command::PERFORMANCE_PROFILE;
    } else if (arg == "performance-stats") {
        args.command = Command::PERFORMANCE_STATS;
    } else if (arg == "performance-network") {
        args.command = Command::PERFORMANCE_NETWORK;
    } else if (arg == "batch") {
        args.command = Command::BATCH;
    } else if (arg == "batch-run") {
        args.command = Command::BATCH_RUN;
    } else if (arg == "batch-status") {
        args.command = Command::BATCH_STATUS;
    } else if (arg == "batch-cancel") {
        args.command = Command::BATCH_CANCEL;
    } else if (arg == "config-schema") {
        args.command = Command::CONFIG_SCHEMA;
    } else if (arg == "config-validate-all") {
        args.command = Command::CONFIG_VALIDATE_ALL;
    } else if (arg == "config-optimize") {
        args.command = Command::CONFIG_OPTIMIZE;
    } else if (arg == "config-backup") {
        args.command = Command::CONFIG_BACKUP;
    } else if (arg == "config-restore") {
        args.command = Command::CONFIG_RESTORE;
    } else if (arg == "system-info") {
        args.command = Command::SYSTEM_INFO;
    } else if (arg == "system-logs") {
        args.command = Command::SYSTEM_LOGS;
    } else if (arg == "system-resources") {
        args.command = Command::SYSTEM_RESOURCES;
    } else if (arg == "system-network") {
        args.command = Command::SYSTEM_NETWORK;
    } else {
        args.command = Command::UNKNOWN;
    }

    // Parse remaining arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle options
        if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--json" || arg == "-j") {
            args.jsonOutput = true;
        } else if (arg.find("--output=") == 0) {
            args.outputFile = arg.substr(9);
        } else if (arg.find("--file=") == 0) {
            args.options["file"] = arg.substr(7);
        } else if (arg.find("--config=") == 0) {
            args.options["config"] = arg.substr(8);
        } else if (arg.find("--experiment=") == 0) {
            args.options["experiment"] = arg.substr(12);
        } else if (arg.find("--batch=") == 0) {
            args.options["batch"] = arg.substr(7);
        } else if (arg.find("--interval=") == 0) {
            args.options["interval"] = arg.substr(11);
        } else if (arg.find("--format=") == 0) {
            args.options["format"] = arg.substr(8);
        } else {
            args.args.push_back(arg);
        }
    }

    args.timestamp = std::chrono::system_clock::now();
    return args;
}

bool AdvancedCLI::execute(const CommandArgs& args, std::ostream& output) {
    output << "NLM Advanced CLI Command: " << static_cast<int>(args.command) << std::endl;
    
    switch (args.command) {
        case Command::HELP:
            output << "Available commands:" << std::endl;
            for (int i = static_cast<int>(Command::UNKNOWN) + 1; i < static_cast<int>(Command::HELP_COMMAND); ++i) {
                Command cmd = static_cast<Command>(i);
                output << "  " << getCommandString(cmd) << std::endl;
            }
            break;

        case Command::VERSION:
            output << "NLM Advanced Interface v2.0.0" << std::endl;
            output << "Build: Phase 6 Integration" << std::endl;
            break;

        case Command::CONFIG:
            output << "Configuration management commands available:" << std::endl;
            output << "  config-show    - Show current configuration" << std::endl;
            output << "  config-validate - Validate configuration" << std::endl;
            output << "  config-set     - Set configuration value" << std::endl;
            output << "  config-export  - Export configuration to file" << std::endl;
            output << "  config-import  - Import configuration from file" << std::endl;
            break;

        case Command::EXPERIMENT:
            output << "Experiment management commands available:" << std::endl;
            output << "  experiment-run    - Run experiment" << std::endl;
            output << "  experiment-list   - List experiments" << std::endl;
            output << "  experiment-status - Get experiment status" << std::endl;
            output << "  experiment-stop   - Stop experiment" << std::endl;
            break;

        case Command::MONITOR:
            output << "Monitoring commands available:" << std::endl;
            output << "  monitor-start    - Start monitoring" << std::endl;
            output << "  monitor-stop     - Stop monitoring" << std::endl;
            output << "  monitor-metrics  - Get metrics" << std::endl;
            break;

        case Command::DEBUG:
            output << "Debug commands available:" << std::endl;
            output << "  debug-neurons    - Debug neurons" << std::endl;
            output << "  debug-synapses   - Debug synapses" << std::endl;
            output << "  debug-memory     - Debug memory" << std::endl;
            output << "  debug-prediction - Debug prediction" << std::endl;
            output << "  debug-neuromodulation - Debug neuromodulation" << std::endl;
            output << "  debug-plasticity - Debug plasticity" << std::endl;
            break;

        case Command::VISUALIZATION:
            output << "Visualization commands available:" << std::endl;
            output << "  visualization-toggle - Toggle visualization" << std::endl;
            output << "  visualization-update - Update visualization" << std::endl;
            output << "  visualization-export - Export visualization" << std::endl;
            break;

        case Command::PERFORMANCE:
            output << "Performance commands available:" << std::endl;
            output << "  performance-profile - Profile performance" << std::endl;
            output << "  performance-stats   - Get performance stats" << std::endl;
            output << "  performance-network - Get network stats" << std::endl;
            break;

        case Command::BATCH:
            output << "Batch processing commands available:" << std::endl;
            output << "  batch-run     - Run batch" << std::endl;
            output << "  batch-status   - Get batch status" << std::endl;
            output << "  batch-cancel   - Cancel batch" << std::endl;
            break;

        case Command::SYSTEM_INFO:
            output << "System information:" << std::endl;
            output << "  CPU: Multi-core supported" << std::endl;
            output << "  Memory: System dependent" << std::endl;
            output << "  Network: Network interfaces available" << std::endl;
            output << "  Storage: Available storage detected" << std::endl;
            break;

        default:
            output << "Unknown command. Use help for available commands." << std::endl;
            return false;
    }

    return true;
}

std::string AdvancedCLI::getHelpText(Command command) const {
    return getHelp(command);
}

AdvancedCLI::Command AdvancedCLI::getCommand(const std::string& cmd) const {
    // Map string to Command enum
    static const std::unordered_map<std::string, Command> commandMap = {
        {"help", Command::HELP},
        {"version", Command::VERSION},
        {"config", Command::CONFIG},
        {"config-validate", Command::CONFIG_VALIDATE},
        {"config-show", Command::CONFIG_SHOW},
        {"config-set", Command::CONFIG_SET},
        {"config-export", Command::CONFIG_EXPORT},
        {"config-import", Command::CONFIG_IMPORT},
        {"config-template", Command::CONFIG_TEMPLATE},
        {"experiment", Command::EXPERIMENT},
        {"experiment-run", Command::EXPERIMENT_RUN},
        {"experiment-list", Command::EXPERIMENT_LIST},
        {"experiment-status", Command::EXPERIMENT_STATUS},
        {"experiment-stop", Command::EXPERIMENT_STOP},
        {"monitor", Command::MONITOR},
        {"monitor-start", Command::MONITOR_START},
        {"monitor-stop", Command::MONITOR_STOP},
        {"monitor-metrics", Command::MONITOR_METRICS},
        {"debug", Command::DEBUG},
        {"debug-neurons", Command::DEBUG_NEURONS},
        {"debug-synapses", Command::DEBUG_SYNAPSES},
        {"debug-memory", Command::DEBUG_MEMORY},
        {"debug-prediction", Command::DEBUG_PREDICTION},
        {"debug-neuromodulation", Command::DEBUG_NEUROMODULATION},
        {"debug-plasticity", Command::DEBUG_PLASTICITY},
        {"visualization", Command::VISUALIZATION},
        {"visualization-toggle", Command::VISUALIZATION_TOGGLE},
        {"visualization-update", Command::VISUALIZATION_UPDATE},
        {"visualization-export", Command::VISUALIZATION_EXPORT},
        {"performance", Command::PERFORMANCE},
        {"performance-profile", Command::PERFORMANCE_PROFILE},
        {"performance-stats", Command::PERFORMANCE_STATS},
        {"performance-network", Command::PERFORMANCE_NETWORK},
        {"batch", Command::BATCH},
        {"batch-run", Command::BATCH_RUN},
        {"batch-status", Command::BATCH_STATUS},
        {"batch-cancel", Command::BATCH_CANCEL},
        {"config-schema", Command::CONFIG_SCHEMA},
        {"config-validate-all", Command::CONFIG_VALIDATE_ALL},
        {"config-optimize", Command::CONFIG_OPTIMIZE},
        {"config-backup", Command::CONFIG_BACKUP},
        {"config-restore", Command::CONFIG_RESTORE},
        {"system-info", Command::SYSTEM_INFO},
        {"system-logs", Command::SYSTEM_LOGS},
        {"system-resources", Command::SYSTEM_RESOURCES},
        {"system-network", Command::SYSTEM_NETWORK},
    };

    auto it = commandMap.find(cmd);
    return it != commandMap.end() ? it->second : Command::UNKNOWN;
}

bool AdvancedCLI::requiresExperiment(Command command) const {
    switch (command) {
        case Command::EXPERIMENT_RUN:
        case Command::EXPERIMENT_STATUS:
        case Command::EXPERIMENT_STOP:
        case Command::DEBUG_NEURONS:
        case Command::DEBUG_SYNAPSES:
        case Command::DEBUG_MEMORY:
        case Command::DEBUG_PREDICTION:
        case Command::DEBUG_NEUROMODULATION:
        case Command::DEBUG_PLASTICITY:
        case Command::VISUALIZATION:
        case Command::VISUALIZATION_TOGGLE:
        case Command::VISUALIZATION_UPDATE:
        case Command::VISUALIZATION_EXPORT:
        case Command::PERFORMANCE:
        case Command::PERFORMANCE_PROFILE:
        case Command::PERFORMANCE_STATS:
        case Command::PERFORMANCE_NETWORK:
        case Command::BATCH:
        case Command::BATCH_RUN:
        case Command::BATCH_STATUS:
        case Command::BATCH_CANCEL:
            return true;

        default:
            return false;
    }
}

bool AdvancedCLI::isExperimentCommand(Command command) const {
    return requiresExperiment(command);
}

bool AdvancedCLI::isVisualizationCommand(Command command) const {
    return command == Command::VISUALIZATION || 
           command == Command::VISUALIZATION_TOGGLE ||
           command == Command::VISUALIZATION_UPDATE ||
           command == Command::VISUALIZATION_EXPORT;
}

bool AdvancedCLI::isDebugCommand(Command command) const {
    return command == Command::DEBUG ||
           command == Command::DEBUG_NEURONS ||
           command == Command::DEBUG_SYNAPSES ||
           command == Command::DEBUG_MEMORY ||
           command == Command::DEBUG_PREDICTION ||
           command == Command::DEBUG_NEUROMODULATION ||
           command == Command::DEBUG_PLASTICITY;
}

bool AdvancedCLI::isMonitoringCommand(Command command) const {
    return command == Command::MONITOR ||
           command == Command::MONITOR_START ||
           command == Command::MONITOR_STOP ||
           command == Command::MONITOR_METRICS;
}

bool AdvancedCLI::isPerformanceCommand(Command command) const {
    return command == Command::PERFORMANCE ||
           command == Command::PERFORMANCE_PROFILE ||
           command == Command::PERFORMANCE_STATS ||
           command == Command::PERFORMANCE_NETWORK;
}

bool AdvancedCLI::isConfigCommand(Command command) const {
    return command == Command::CONFIG ||
           command == Command::CONFIG_VALIDATE ||
           command == Command::CONFIG_SHOW ||
           command == Command::CONFIG_SET ||
           command == Command::CONFIG_EXPORT ||
           command == Command::CONFIG_IMPORT ||
           command == Command::CONFIG_TEMPLATE ||
           command == Command::CONFIG_SCHEMA ||
           command == Command::CONFIG_VALIDATE_ALL ||
           command == Command::CONFIG_OPTIMIZE ||
           command == Command::CONFIG_BACKUP ||
           command == Command::CONFIG_RESTORE;
}

std::string AdvancedCLI::getCommandString(Command command) const {
    static const std::unordered_map<Command, std::string> commandStrings = {
        {Command::UNKNOWN, "unknown"},
        {Command::HELP, "help"},
        {Command::VERSION, "version"},
        {Command::CONFIG, "config"},
        {Command::CONFIG_VALIDATE, "config-validate"},
        {Command::CONFIG_SHOW, "config-show"},
        {Command::CONFIG_SET, "config-set"},
        {Command::CONFIG_EXPORT, "config-export"},
        {Command::CONFIG_IMPORT, "config-import"},
        {Command::CONFIG_TEMPLATE, "config-template"},
        {Command::EXPERIMENT, "experiment"},
        {Command::EXPERIMENT_RUN, "experiment-run"},
        {Command::EXPERIMENT_LIST, "experiment-list"},
        {Command::EXPERIMENT_STATUS, "experiment-status"},
        {Command::EXPERIMENT_STOP, "experiment-stop"},
        {Command::MONITOR, "monitor"},
        {Command::MONITOR_START, "monitor-start"},
        {Command::MONITOR_STOP, "monitor-stop"},
        {Command::MONITOR_METRICS, "monitor-metrics"},
        {Command::DEBUG, "debug"},
        {Command::DEBUG_NEURONS, "debug-neurons"},
        {Command::DEBUG_SYNAPSES, "debug-synapses"},
        {Command::DEBUG_MEMORY, "debug-memory"},
        {Command::DEBUG_PREDICTION, "debug-prediction"},
        {Command::DEBUG_NEUROMODULATION, "debug-neuromodulation"},
        {Command::DEBUG_PLASTICITY, "debug-plasticity"},
        {Command::VISUALIZATION, "visualization"},
        {Command::VISUALIZATION_TOGGLE, "visualization-toggle"},
        {Command::VISUALIZATION_UPDATE, "visualization-update"},
        {Command::VISUALIZATION_EXPORT, "visualization-export"},
        {Command::PERFORMANCE, "performance"},
        {Command::PERFORMANCE_PROFILE, "performance-profile"},
        {Command::PERFORMANCE_STATS, "performance-stats"},
        {Command::PERFORMANCE_NETWORK, "performance-network"},
        {Command::BATCH, "batch"},
        {Command::BATCH_RUN, "batch-run"},
        {Command::BATCH_STATUS, "batch-status"},
        {Command::BATCH_CANCEL, "batch-cancel"},
        {Command::CONFIG_SCHEMA, "config-schema"},
        {Command::CONFIG_VALIDATE_ALL, "config-validate-all"},
        {Command::CONFIG_OPTIMIZE, "config-optimize"},
        {Command::CONFIG_BACKUP, "config-backup"},
        {Command::CONFIG_RESTORE, "config-restore"},
        {Command::SYSTEM_INFO, "system-info"},
        {Command::SYSTEM_LOGS, "system-logs"},
        {Command::SYSTEM_RESOURCES, "system-resources"},
        {Command::SYSTEM_NETWORK, "system-network"},
        {Command::HELP_COMMAND, "help-command"},
    };

    auto it = commandStrings.find(command);
    return it != commandStrings.end() ? it->second : "unknown";
}

} // namespace nlm