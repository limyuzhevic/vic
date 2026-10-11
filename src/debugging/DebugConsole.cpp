#include "DebugConsole.hpp"
#include "../brain/Brain.hpp"
#include "../visualization/VisualizationInterface.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>

namespace nlm {

struct DebugConsole::Impl {
    DebugConsoleConfig config;
    Brain* brain;
    VisualizationEngine* visualizationEngine;
    DataExporter* dataExporter;
    PerformanceProfiler* profiler;
    
    // Command registry
    std::map<std::string, struct CommandInfo> commands;
    
    // State
    bool paused;
    bool initialized;
    double lastUpdateTime;
    size_t stepsSinceStart;
    
    // History
    std::vector<std::string> commandHistory;
    std::vector<std::string> outputBuffer;
    
    // Breakpoints and watch variables
    std::vector<Breakpoint> breakpoints;
    std::vector<WatchVariable> watchVariables;
    
    // Timers
    std::chrono::steady_clock::time_point lastRefreshTime;
    
    // Log sink for console output
    std::shared_ptr<ILogSink> logSink;
    
    Impl(const DebugConsoleConfig& cfg) : config(cfg), brain(nullptr), visualizationEngine(nullptr),
                                         dataExporter(nullptr), profiler(nullptr), paused(false),
                                         initialized(false), stepsSinceStart(0), lastRefreshTime(std::chrono::steady_clock::now()) {
        // Initialize log sink for console output
        logSink = std::make_shared<ConsoleLogger>();
        if (config.useColors) {
            static_cast<ConsoleLogger*>(logSink.get())->setUseColors(true);
        }
    }
    
    ~Impl() {
        if (logSink) {
            logSink->flush();
        }
    }
};

DebugConsole::DebugConsole() : pImpl(std::make_unique<Impl>(DebugConsoleConfig())) {}

DebugConsole::DebugConsole(const DebugConsoleConfig& config) : pImpl(std::make_unique<Impl>(config)) {}

DebugConsole::~DebugConsole() = default;

bool DebugConsole::initialize(Brain* brain, VisualizationEngine* visualizationEngine,
                             DataExporter* dataExporter, PerformanceProfiler* profiler) {
    if (!brain) {
        NLM_LOG_ERROR("DebugConsole: Brain pointer is null");
        return false;
    }
    
    pImpl->brain = brain;
    pImpl->visualizationEngine = visualizationEngine;
    pImpl->dataExporter = dataExporter;
    pImpl->profiler = profiler;
    
    // Register built-in commands
    registerBuiltInCommands();
    
    pImpl->initialized = true;
    pImpl->lastUpdateTime = 0.0;
    
    NLM_LOG_INFO("DebugConsole: Initialized successfully");
    return true;
}

void DebugConsole::update(double deltaTime) {
    if (!pImpl->initialized || !pImpl->brain) return;
    
    pImpl->lastUpdateTime += deltaTime;
    
    // Auto-refresh if enabled
    if (pImpl->config.autoRefresh) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->lastRefreshTime).count();
        
        if (elapsed >= pImpl->config.refreshInterval) {
            refreshDisplay();
            pImpl->lastRefreshTime = now;
        }
    }
}

void DebugConsole::processInput(const std::string& input) {
    if (input.empty()) return;
    
    // Add to history
    pImpl->commandHistory.push_back(input);
    if (pImpl->commandHistory.size() > pImpl->config.commandHistorySize) {
        pImpl->commandHistory.erase(pImpl->commandHistory.begin());
    }
    
    // Execute command
    executeCommand(input);
}

DebugCommandResult DebugConsole::executeCommand(const std::string& command) {
    DebugCommandContext context;
    context.brain = pImpl->brain;
    context.visualizationEngine = pImpl->visualizationEngine;
    context.dataExporter = pImpl->dataExporter;
    context.profiler = pImpl->profiler;
    context.currentTime = pImpl->brain ? pImpl->brain->getCurrentTime() : 0.0;
    context.currentStep = pImpl->stepsSinceStart;
    
    // Parse command
    std::vector<std::string> args;
    std::string cmd;
    
    std::istringstream iss(command);
    iss >> cmd;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    // Check for built-in commands first
    auto it = pImpl->commands.find(cmd);
    if (it != pImpl->commands.end()) {
        auto& cmdInfo = it->second;
        return cmdInfo.handler(args, context);
    }
    
    // Check for breakpoints
    if (pImpl->config.enableBreakpoints && !pImpl->paused) {
        bool shouldBreakpoint = false;
        std::string breakpointMessages;
        
        for (const auto& bp : pImpl->breakpoints) {
            if (bp.active && context.currentStep == bp.step) {
                shouldBreakpoint = true;
                breakpointMessages += "Breakpoint at step " + std::to_string(bp.step) + ": " + bp.description + "\n";
            }
        }
        
        if (shouldBreakpoint) {
            printInfo("=== BREAKPOINT REACHED ===");
            print(breakpointMessages);
            pause();
            
            DebugCommandResult result(true, "Breakpoint hit");
            result.metadata["breakpoint_step"] = std::to_string(context.currentStep);
            return result;
        }
    }
    
    // Unknown command
    std::string errorMsg = "Unknown command: " + cmd + "\nAvailable commands: ";
    for (const auto& pair : pImpl->commands) {
        errorMsg += pair.first + " ";
    }
    
    return DebugCommandResult(false, errorMsg);
}

void DebugConsole::registerCommand(const std::string& name, const std::string& description,
                                  const std::string& usage, CommandHandler handler) {
    if (!pImpl) return;
    
    struct CommandInfo {
        std::string description;
        std::string usage;
        CommandHandler handler;
    };
    
    pImpl->commands[name] = {description, usage, handler};
}

void DebugConsole::addBreakpoint(size_t step, const std::string& description, bool active) {
    if (!pImpl) return;
    
    Breakpoint bp(step, description, active);
    pImpl->breakpoints.push_back(bp);
}

void DebugConsole::removeBreakpoint(size_t step) {
    if (!pImpl) return;
    
    auto it = std::remove_if(pImpl->breakpoints.begin(), pImpl->breakpoints.end(),
                            [step](const Breakpoint& bp) { return bp.step == step; });
    pImpl->breakpoints.erase(it, pImpl->breakpoints.end());
}

void DebugConsole::clearBreakpoints() {
    if (!pImpl) return;
    pImpl->breakpoints.clear();
}

const std::vector<Breakpoint>& DebugConsole::getBreakpoints() const {
    return pImpl->breakpoints;
}

void DebugConsole::addWatchVariable(const std::string& expression, const std::string& description) {
    if (!pImpl) return;
    
    WatchVariable wv(expression, description);
    pImpl->watchVariables.push_back(wv);
}

void DebugConsole::removeWatchVariable(const std::string& expression) {
    if (!pImpl) return;
    
    auto it = std::remove_if(pImpl->watchVariables.begin(), pImpl->watchVariables.end(),
                            [expression](const WatchVariable& wv) { return wv.expression == expression; });
    pImpl->watchVariables.erase(it, pImpl->watchVariables.end());
}

void DebugConsole::clearWatchVariables() {
    if (!pImpl) return;
    pImpl->watchVariables.clear();
}

const std::vector<WatchVariable>& DebugConsole::getWatchVariables() const {
    return pImpl->watchVariables;
}

MemoryDump DebugConsole::getMemoryDump() const {
    MemoryDump dump;
    
    if (!pImpl->brain) return dump;
    
    dump.totalNeurons = pImpl->brain->getTotalNeuronCount();
    dump.totalSynapses = pImpl->brain->getTotalSynapseCount();
    dump.activeNeurons = pImpl->brain->getActiveNeuronCount();
    dump.firingNeurons = pImpl->brain->getFiringNeuronCount();
    dump.averageFiringRate = pImpl->brain->getAverageFiringRate();
    dump.excitationInhibitionRatio = pImpl->brain->getExcitationInhibitionRatio();
    
    // Estimate memory usage (placeholder)
    dump.memoryUsage = dump.totalNeurons * 1024 + dump.totalSynapses * 512;
    
    return dump;
}

void DebugConsole::dumpMemory(const std::string& filepath) {
    MemoryDump dump = getMemoryDump();
    
    std::string filename = filepath.empty() ? "memory_dump_" + std::to_string(pImpl->stepsSinceStart) + ".txt" : filepath;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        printError("Failed to open file for memory dump: " + filename);
        return;
    }
    
    file << "=== NLM Memory Dump ===" << std::endl;
    file << "Timestamp: " << pImpl->brain ? pImpl->brain->getCurrentTime() : 0.0 << std::endl;
    file << "Step: " << pImpl->stepsSinceStart << std::endl;
    file << std::endl;
    
    file << "Neuron Statistics:" << std::endl;
    file << "  Total neurons: " << dump.totalNeurons << std::endl;
    file << "  Active neurons: " << dump.activeNeurons << std::endl;
    file << "  Firing neurons: " << dump.firingNeurons << std::endl;
    file << "  Average firing rate: " << dump.averageFiringRate << " Hz" << std::endl;
    file << std::endl;
    
    file << "Synapse Statistics:" << std::endl;
    file << "  Total synapses: " << dump.totalSynapses << std::endl;
    file << std::endl;
    
    file << "Network Properties:" << std::endl;
    file << "  Excitation/Inhibition ratio: " << dump.excitationInhibitionRatio << std::endl;
    file << "  Estimated memory usage: " << dump.memoryUsage << " bytes" << std::endl;
    
    file.close();
    
    printInfo("Memory dump saved to: " + filename);
}

void DebugConsole::pause() {
    if (!pImpl) return;
    pImpl->paused = true;
    NLM_LOG_INFO("DebugConsole: Simulation paused");
}

void DebugConsole::resume() {
    if (!pImpl) return;
    pImpl->paused = false;
    NLM_LOG_INFO("DebugConsole: Simulation resumed");
}

void DebugConsole::step() {
    if (!pImpl || !pImpl->brain) return;
    
    // Single step of the brain simulation
    pImpl->brain->step(pImpl->stepsSinceStart);
    pImpl->stepsSinceStart++;
}

bool DebugConsole::isPaused() const {
    return pImpl ? pImpl->paused : false;
}

void DebugConsole::updateConfig(const DebugConsoleConfig& config) {
    if (!pImpl) return;
    pImpl->config = config;
}

const DebugConsoleConfig& DebugConsole::getConfig() const {
    return pImpl->config;
}

void DebugConsole::print(const std::string& message) {
    if (!pImpl) return;
    
    // Add to output buffer
    pImpl->outputBuffer.push_back(message);
    
    // Log to global logger
    NLM_LOG_INFO(message);
}

void DebugConsole::printInfo(const std::string& message) {
    print("INFO: " + message);
}

void DebugConsole::printWarning(const std::string& message) {
    print("WARNING: " + message);
}

void DebugConsole::printError(const std::string& message) {
    print("ERROR: " + message);
}

const std::vector<std::string>& DebugConsole::getCommandHistory() const {
    return pImpl ? pImpl->commandHistory : std::vector<std::string>();
}

bool DebugConsole::shouldDisplay() const {
    return pImpl ? pImpl->initialized : false;
}

void DebugConsole::registerBuiltInCommands() {
    if (!pImpl) return;
    
    // Register all built-in commands
    registerCommand("help", "Show help information", "help [command]",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleHelpCommand(args, ctx);
                   });
    
    registerCommand("status", "Show brain status", "status",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleStatusCommand(args, ctx);
                   });
    
    registerCommand("step", "Step simulation forward", "step [count]",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleStepCommand(args, ctx);
                   });
    
    registerCommand("run", "Run simulation until breakpoint or command", "run",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleRunCommand(args, ctx);
                   });
    
    registerCommand("break", "Add a breakpoint", "break <step> [description]",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleBreakCommand(args, ctx);
                   });
    
    registerCommand("clear-breakpoints", "Clear all breakpoints", "clear-breakpoints",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleClearBreakpointsCommand(args, ctx);
                   });
    
    registerCommand("watch", "Add a watch variable", "watch <expression> [description]",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleWatchCommand(args, ctx);
                   });
    
    registerCommand("clear-watch", "Clear all watch variables", "clear-watch",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleClearWatchCommand(args, ctx);
                   });
    
    registerCommand("dump-memory", "Dump memory information", "dump-memory [filename]",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleDumpMemoryCommand(args, ctx);
                   });
    
    registerCommand("visualize", "Toggle visualization", "visualize",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleVisualizeCommand(args, ctx);
                   });
    
    registerCommand("export", "Export simulation data", "export <format> <filename>",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleExportCommand(args, ctx);
                   });
    
    registerCommand("profile", "Start/stop performance profiling", "profile",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleProfileCommand(args, ctx);
                   });
    
    registerCommand("reset", "Reset brain to initial state", "reset",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleResetCommand(args, ctx);
                   });
    
    registerCommand("quit", "Quit debug console", "quit",
                   [this](const std::vector<std::string>& args, DebugCommandContext& ctx) {
                       return handleQuitCommand(args, ctx);
                   });
}

DebugCommandResult DebugConsole::handleHelpCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    std::stringstream ss;
    
    if (args.empty()) {
        ss << "=== NLM Debug Console Help ===" << std::endl;
        ss << std::endl;
        ss << "Available commands:" << std::endl;
        
        for (const auto& pair : pImpl->commands) {
            ss << "  " << pair.first << " - " << pair.second.description << std::endl;
        }
        
        ss << std::endl;
        ss << "Type 'help <command>' for more information about a specific command.";
    } else {
        std::string cmd = args[0];
        auto it = pImpl->commands.find(cmd);
        if (it != pImpl->commands.end()) {
            auto& cmdInfo = it->second;
            ss << "=== Help: " << cmd << " ===" << std::endl;
            ss << std::endl;
            ss << cmdInfo.description << std::endl;
            ss << std::endl;
            ss << "Usage: " << cmdInfo.usage << std::endl;
        } else {
            return DebugCommandResult(false, "Unknown command: " + cmd);
        }
    }
    
    return DebugCommandResult(true, ss.str());
}

DebugCommandResult DebugConsole::handleStatusCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    std::stringstream ss;
    
    ss << "=== Brain Status ===" << std::endl;
    ss << "Step: " << ctx.currentStep << std::endl;
    ss << "Time: " << std::fixed << std::setprecision(3) << ctx.currentTime << " s" << std::endl;
    ss << "Paused: " << (pImpl->paused ? "Yes" : "No") << std::endl;
    ss << std::endl;
    
    if (pImpl->brain) {
        ss << "Neuron Statistics:" << std::endl;
        ss << "  Total: " << pImpl->brain->getTotalNeuronCount() << std::endl;
        ss << "  Active: " << pImpl->brain->getActiveNeuronCount() << std::endl;
        ss << "  Firing (this step): " << pImpl->brain->getFiringNeuronCount() << std::endl;
        ss << "  Average firing rate: " << std::fixed << std::setprecision(3) << pImpl->brain->getAverageFiringRate() << " Hz" << std::endl;
        ss << "  Total spikes: " << pImpl->brain->getTotalSpikeCount() << std::endl;
        ss << "  Pending events: " << pImpl->brain->getPendingSpikeEventCount() << std::endl;
        ss << "  Excitation/Inhibition ratio: " << std::fixed << std::setprecision(3) << pImpl->brain->getExcitationInhibitionRatio() << std::endl;
        ss << std::endl;
        
        ss << "Region Information:" << std::endl;
        for (const auto& regionId : pImpl->brain->getRegionIds()) {
            auto* region = pImpl->brain->getRegion(regionId);
            if (region) {
                ss << "  Region " << regionId.index() << ": " << region->getName() << std::endl;
                ss << "    Neurons: " << region->getTotalNeuronCount() << std::endl;
                ss << "    Synapses: " << region->getSynapseCount() << std::endl;
                ss << "    Populations: " << region->getPopulationCount() << std::endl;
            }
        }
    }
    
    ss << std::endl;
    ss << "Breakpoint Information:" << std::endl;
    ss << "  Total breakpoints: " << pImpl->breakpoints.size() << std::endl;
    for (const auto& bp : pImpl->breakpoints) {
        ss << "    Step " << bp.step << ": " << bp.description << " (" << (bp.active ? "active" : "inactive") << ")" << std::endl;
    }
    
    ss << std::endl;
    ss << "Watch Variables:" << std::endl;
    ss << "  Total watch variables: " << pImpl->watchVariables.size() << std::endl;
    for (const auto& wv : pImpl->watchVariables) {
        ss << "    " << wv.expression << " - " << wv.description << " (" << wv.updateCount << " updates)" << std::endl;
    }
    
    return DebugCommandResult(true, ss.str());
}

DebugCommandResult DebugConsole::handleStepCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    int steps = 1;
    if (!args.empty()) {
        try {
            steps = std::stoi(args[0]);
            if (steps < 1) steps = 1;
        } catch (...) {
            return DebugCommandResult(false, "Invalid step count: " + args[0]);
        }
    }
    
    if (!pImpl->brain) {
        return DebugCommandResult(false, "Brain not available");
    }
    
    std::stringstream ss;
    ss << "Stepping forward " << steps << " steps..." << std::endl;
    
    for (int i = 0; i < steps; ++i) {
        pImpl->brain->step(ctx.currentStep);
        pImpl->stepsSinceStart++;
        
        // Check for breakpoints
        bool breakpointTriggered = false;
        for (const auto& bp : pImpl->breakpoints) {
            if (bp.active && ctx.currentStep == bp.step) {
                ss << "Breakpoint at step " << bp.step << ": " << bp.description << std::endl;
                breakpointTriggered = true;
                break;
            }
        }
        
        if (breakpointTriggered) {
            break;
        }
    }
    
    return DebugCommandResult(true, ss.str());
}

DebugCommandResult DebugConsole::handleRunCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    if (!pImpl->brain) {
        return DebugCommandResult(false, "Brain not available");
    }
    
    std::stringstream ss;
    ss << "Running simulation until breakpoint..." << std::endl;
    
    while (!pImpl->paused) {
        pImpl->brain->step(ctx.currentStep);
        pImpl->stepsSinceStart++;
        ctx.currentStep++;
        
        // Check for breakpoints
        bool breakpointTriggered = false;
        for (const auto& bp : pImpl->breakpoints) {
            if (bp.active && ctx.currentStep == bp.step) {
                ss << "Breakpoint at step " << bp.step << ": " << bp.description << std::endl;
                breakpointTriggered = true;
                break;
            }
        }
        
        if (breakpointTriggered) {
            break;
        }
        
        // Safety check to prevent infinite loops
        if (ctx.currentStep > 1000000) {
            ss << "Warning: Reached maximum step count (1,000,000)" << std::endl;
            break;
        }
    }
    
    return DebugCommandResult(true, ss.str());
}

DebugCommandResult DebugConsole::handleBreakCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    if (args.size() < 1) {
        return DebugCommandResult(false, "Usage: break <step> [description]");
    }
    
    try {
        size_t step = std::stoull(args[0]);
        std::string description = args.size() > 1 ? args[1] : "Breakpoint at step " + std::to_string(step);
        
        addBreakpoint(step, description, true);
        
        std::stringstream ss;
        ss << "Breakpoint added at step " << step << ": " << description;
        return DebugCommandResult(true, ss.str());
    } catch (...) {
        return DebugCommandResult(false, "Invalid step number: " + args[0]);
    }
}

DebugCommandResult DebugConsole::handleClearBreakpointsCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    clearBreakpoints();
    return DebugCommandResult(true, "All breakpoints cleared");
}

DebugCommandResult DebugConsole::handleWatchCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    if (args.size() < 1) {
        return DebugCommandResult(false, "Usage: watch <expression> [description]");
    }
    
    std::string expression = args[0];
    std::string description = args.size() > 1 ? args[1] : "";
    
    addWatchVariable(expression, description);
    
    std::stringstream ss;
    ss << "Watch variable added: " << expression;
    if (!description.empty()) {
        ss << " - " << description;
    }
    ss << "; Total watch variables: " << pImpl->watchVariables.size();
    
    return DebugCommandResult(true, ss.str());
}

DebugCommandResult DebugConsole::handleClearWatchCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    clearWatchVariables();
    return DebugCommandResult(true, "All watch variables cleared");
}

DebugCommandResult DebugConsole::handleDumpMemoryCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    std::string filename = args.empty() ? "" : args[0];
    
    dumpMemory(filename);
    
    return DebugCommandResult(true, "Memory dump completed");
}

DebugCommandResult DebugConsole::handleVisualizeCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    if (!pImpl->visualizationEngine) {
        return DebugCommandResult(false, "Visualization engine not available");
    }
    
    std::stringstream ss;
    if (pImpl->visualizationEngine->isActive()) {
        pImpl->visualizationEngine->close();
        ss << "Visualization engine stopped";
    } else {
        if (pImpl->visualizationEngine->initialize()) {
            ss << "Visualization engine started";
        } else {
            ss << "Failed to start visualization engine";
        }
    }
    
    return DebugCommandResult(true, ss.str());
}

DebugCommandResult DebugConsole::handleExportCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    if (!pImpl->dataExporter) {
        return DebugCommandResult(false, "Data exporter not available");
    }
    
    if (args.size() < 2) {
        return DebugCommandResult(false, "Usage: export <format> <filename>");
    }
    
    std::string format = args[0];
    std::string filename = args[1];
    
    // Convert format string to export format
    DataExporter::ExportFormat exportFormat;
    if (format == "csv") exportFormat = DataExporter::ExportFormat::CSV;
    else if (format == "json") exportFormat = DataExporter::ExportFormat::JSON;
    else if (format == "binary") exportFormat = DataExporter::ExportFormat::Binary;
    else if (format == "protobuf") exportFormat = DataExporter::ExportFormat::Protobuf;
    else {
        return DebugCommandResult(false, "Unknown export format: " + format);
    }
    
    std::stringstream ss;
    if (pImpl->dataExporter->exportData(exportFormat, filename)) {
        ss << "Data exported successfully to " << filename;
    } else {
        ss << "Failed to export data to " << filename;
    }
    
    return DebugCommandResult(true, ss.str());
}

DebugCommandResult DebugConsole::handleProfileCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    if (!pImpl->profiler) {
        return DebugCommandResult(false, "Performance profiler not available");
    }
    
    std::stringstream ss;
    if (pImpl->profiler->isActive()) {
        pImpl->profiler->stop();
        ss << "Performance profiler stopped";
        
        // Print profiling results
        auto stats = pImpl->profiler->getStats();
        ss << "\n=== Performance Statistics ===" << std::endl;
        ss << "CPU time: " << stats.cpuTimeMs << " ms" << std::endl;
        ss << "Memory usage: " << stats.memoryUsageBytes << " bytes" << std::endl;
        ss << "Peak memory: " << stats.peakMemoryBytes << " bytes" << std::endl;
        ss << "Simulations per second: " << stats.simulationsPerSecond << std::endl;
    } else {
        pImpl->profiler->start();
        ss << "Performance profiler started";
    }
    
    return DebugCommandResult(true, ss.str());
}

DebugCommandResult DebugConsole::handleResetCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    if (!pImpl->brain) {
        return DebugCommandResult(false, "Brain not available");
    }
    
    std::stringstream ss;
    ss << "Resetting brain..." << std::endl;
    
    // Reset brain state
    pImpl->brain->reset();
    pImpl->stepsSinceStart = 0;
    
    // Clear breakpoints and watch variables
    clearBreakpoints();
    clearWatchVariables();
    
    ss << "Brain reset completed";
    
    return DebugCommandResult(true, ss.str());
}

DebugCommandResult DebugConsole::handleQuitCommand(const std::vector<std::string>& args, DebugCommandContext& ctx) {
    // Update configuration to disable console
    pImpl->config.enabled = false;
    
    std::stringstream ss;
    ss << "Debug console shutting down...";
    
    return DebugCommandResult(true, ss.str());
}

void DebugConsole::refreshDisplay() {
    if (!pImpl) return;
    
    // Refresh watch variables
    for (auto& wv : pImpl->watchVariables) {
        // TODO: Evaluate watch variable expression and update value
        wv.updateCount++;
    }
    
    // TODO: Refresh breakpoints
    // TODO: Refresh output buffer
}

} // namespace nlm
