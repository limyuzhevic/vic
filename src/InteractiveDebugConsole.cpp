#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <chrono>
#include <thread>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include "core/Events/NeuralEventSystem.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

class InteractiveDebugConsole {
public:
    InteractiveDebugConsole(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<Logger> logger,
                           std::shared_ptr<Config> config)
        : brain_(brain), logger_(logger), config_(config) {
        initializeCommands();
        initializeEventFilters();
        startConsoleThread();
    }
    
    ~InteractiveDebugConsole() {
        stopConsoleThread();
    }
    
    void start() {
        std::cout << "=== NLM Interactive Debug Console ===" << std::endl;
        std::cout << "Type 'help' for available commands or 'exit' to quit." << std::endl;
        
        std::string input;
        while (running_) {
            std::cout << ">>> ";
            std::getline(std::cin, input);
            
            if (!input.empty()) {
                processInput(input);
            }
        }
    }
    
    void stop() {
        running_ = false;
    }
    
    void setAutoRefresh(bool enabled, double interval_seconds = 1.0) {
        auto_refresh_enabled_ = enabled;
        auto_refresh_interval_ = interval_seconds;
    }
    
    void setScreenClear(bool enabled) {
        screen_clear_enabled_ = enabled;
    }
    
private:
    void initializeCommands() {
        // Command registrations
        registerCommand("help", "Show help information",
            [this](const std::vector<std::string>& args) {
                showHelp(args);
            });
        
        registerCommand("help", "Get detailed help for specific command",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    showCommandHelp(args[0]);
                } else {
                    std::cout << "Error: Command name required for help." << std::endl;
                }
            });
        
        registerCommand("commands", "List all available commands",
            [this](const std::vector<std::string>& args) {
                listCommands();
            });
        
        registerCommand("brain reset", "Reset brain to initial state",
            [this](const std::vector<std::string>& args) {
                brainReset();
            });
        
        registerCommand("brain status", "Show current brain status",
            [this](const std::vector<std::string>& args) {
                brainStatus();
            });
        
        registerCommand("brain initialize", "Re-initialize brain with current config",
            [this](const std::vector<std::string>& args) {
                brainInitialize();
            });
        
        registerCommand("brain save", "Save brain state to file",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    brainSave(args[0]);
                } else {
                    std::cout << "Error: File path required for brain save." << std::endl;
                }
            });
        
        registerCommand("brain load", "Load brain state from file",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    brainLoad(args[0]);
                } else {
                    std::cout << "Error: File path required for brain load." << std::endl;
                }
            });
        
        registerCommand("simulate", "Run simulation for specified steps",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    try {
                        size_t steps = std::stoul(args[0]);
                        simulate(steps);
                    } catch (const std::exception& e) {
                        std::cout << "Error: Invalid step count. " << e.what() << std::endl;
                    }
                } else {
                    std::cout << "Error: Step count required for simulate." << std::endl;
                }
            });
        
        registerCommand("simulate step", "Run one simulation step",
            [this](const std::vector<std::string>& args) {
                simulateStep();
            });
        
        registerCommand("pause", "Pause simulation",
            [this](const std::vector<std::string>& args) {
                pauseSimulation();
            });
        
        registerCommand("resume", "Resume simulation",
            [this](const std::vector<std::string>& args) {
                resumeSimulation();
            });
        
        registerCommand("speed", "Set simulation speed (0.1x to 10x)",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    try {
                        double speed = std::stod(args[0]);
                        if (speed >= 0.1 && speed <= 10.0) {
                            setSimulationSpeed(speed);
                        } else {
                            std::cout << "Error: Speed must be between 0.1 and 10.0." << std::endl;
                        }
                    } catch (const std::exception& e) {
                        std::cout << "Error: Invalid speed value. " << e.what() << std::endl;
                    }
                } else {
                    std::cout << "Error: Speed factor required for speed command." << std::endl;
                }
            });
        
        registerCommand("duration", "Set simulation duration in seconds",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    try {
                        double duration = std::stod(args[0]);
                        setSimulationDuration(duration);
                    } catch (const std::exception& e) {
                        std::cout << "Error: Invalid duration. " << e.what() << std::endl;
                    }
                } else {
                    std::cout << "Error: Duration required for duration command." << std::endl;
                }
            });
        
        registerCommand("time", "Set current simulation time",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    try {
                        double time = std::stod(args[0]);
                        setCurrentTime(time);
                    } catch (const std::exception& e) {
                        std::cout << "Error: Invalid time. " << e.what() << std::endl;
                    }
                } else {
                    std::cout << "Error: Time required for time command." << std::endl;
                }
            });
        
        // Neuron commands
        registerCommand("inject", "Inject current into neuron(s)",
            [this](const std::vector<std::string>& args) {
                if (args.size() >= 2) {
                    try {
                        int target = std::stoi(args[0]);
                        double current = std::stod(args[1]);
                        injectCurrent(target, current);
                    } catch (const std::exception& e) {
                        std::cout << "Error: Invalid injection parameters. " << e.what() << std::endl;
                    }
                } else {
                    std::cout << "Error: Target neuron and current required for inject." << std::endl;
                }
            });
        
        registerCommand("neurons", "List or show information about neurons",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    try {
                        int neuron_id = std::stoi(args[0]);
                        showNeuronDetails(neuron_id);
                    } catch (const std::exception& e) {
                        std::cout << "Error: Invalid neuron ID. " << e.what() << std::endl;
                    }
                } else {
                    listNeurons();
                }
            });
        
        registerCommand("spikes", "Get recent spikes",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    try {
                        size_t count = std::stoul(args[0]);
                        showRecentSpikes(count);
                    } catch (const std::exception& e) {
                        std::cout << "Error: Invalid spike count. " << e.what() << std::endl;
                    }
                } else {
                    std::cout << "Error: Spike count required for spikes command." << std::endl;
                }
            });
        
        registerCommand("spike_history", "Show spike history for neuron",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    try {
                        int neuron_id = std::stoi(args[0]);
                        showSpikeHistory(neuron_id);
                    } catch (const std::exception& e) {
                        std::cout << "Error: Invalid neuron ID. " << e.what() << std::endl;
                    }
                } else {
                    std::cout << "Error: Neuron ID required for spike_history." << std::endl;
                }
            });
        
        // Event system commands
        registerCommand("events", "Show recent events", [this](const std::vector<std::string>& args) {
            if (args.size() > 0) {
                eventFilter(args[0]);
            } else {
                showEvents();
            }
        });
        
        registerCommand("event_system status", "Show event system status",
            [this](const std::vector<std::string>& args) {
                eventSystemStatus();
            });
        
        registerCommand("event_system enable", "Enable event processing",
            [this](const std::vector<std::string>& args) {
                eventSystemEnable(true);
            });
        
        registerCommand("event_system disable", "Disable event processing",
            [this](const std::vector<std::string>& args) {
                eventSystemEnable(false);
            });
        
        registerCommand("subscribe", "Subscribe to events matching filter",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    subscribeToEvents(args[0]);
                } else {
                    std::cout << "Error: Filter required for subscribe." << std::endl;
                }
            });
        
        registerCommand("unsubscribe", "Remove event subscription",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    unsubscribeFromEvents(args[0]);
                } else {
                    std::cout << "Error: Callback identifier required for unsubscribe." << std::endl;
                }
            });
        
        // Configuration commands
        registerCommand("config", "Show current configuration",
            [this](const std::vector<std::string>& args) {
                showConfig();
            });
        
        registerCommand("config set", "Set configuration value",
            [this](const std::vector<std::string>& args) {
                if (args.size() >= 2) {
                    configSet(args[0], args[1]);
                } else {
                    std::cout << "Error: Key and value required for config set." << std::endl;
                }
            });
        
        registerCommand("config get", "Get configuration value",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    configGet(args[0]);
                } else {
                    std::cout << "Error: Key required for config get." << std::endl;
                }
            });
        
        registerCommand("config save", "Save configuration to file",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    configSave(args[0]);
                } else {
                    std::cout << "Error: File path required for config save." << std::endl;
                }
            });
        
        registerCommand("config load", "Load configuration from file",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    configLoad(args[0]);
                } else {
                    std::cout << "Error: File path required for config load." << std::endl;
                }
            });
        
        registerCommand("config keys", "List all configuration keys",
            [this](const std::vector<std::string>& args) {
                configKeys();
            });
        
        // Statistics commands
        registerCommand("stats", "Show comprehensive statistics",
            [this](const std::vector<std::string>& args) {
                showStatistics();
            });
        
        registerCommand("stats neurons", "Show neuron statistics",
            [this](const std::vector<std::string>& args) {
                showNeuronStatistics();
            });
        
        registerCommand("stats synapses", "Show synapse statistics",
            [this](const std::vector<std::string>& args) {
                showSynapseStatistics();
            });
        
        registerCommand("stats memory", "Show memory system statistics",
            [this](const std::vector<std::string>& args) {
                showMemoryStatistics();
            });
        
        registerCommand("stats performance", "Show performance metrics",
            [this](const std::vector<std::string>& args) {
                showPerformanceStatistics();
            });
        
        // Analysis commands
        registerCommand("analysis spike_patterns", "Detect spike patterns",
            [this](const std::vector<std::string>& args) {
                analyzeSpikePatterns();
            });
        
        registerCommand("analysis connectivity", "Analyze neural connectivity",
            [this](const std::vector<std::string>& args) {
                analyzeConnectivity();
            });
        
        registerCommand("analysis plasticity", "Analyze plasticity activity",
            [this](const std::vector<std::string>& args) {
                analyzePlasticity();
            });
        
        registerCommand("analysis activity", "Analyze overall neural activity",
            [this](const std::vector<std::string>& args) {
                analyzeActivity();
            });
        
        registerCommand("analysis anomalies", "Detect anomalies in simulation",
            [this](const std::vector<std::string>& args) {
                detectAnomalies();
            });
        
        // File operations
        registerCommand("log save", "Save log to file",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    logSave(args[0]);
                } else {
                    std::cout << "Error: File path required for log save." << std::endl;
                }
            });
        
        registerCommand("log append", "Append to log file",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    logAppend(args[0]);
                } else {
                    std::cout << "Error: File path required for log append." << std::endl;
                }
            });
        
        registerCommand("log clear", "Clear current log",
            [this](const std::vector<std::string>& args) {
                logClear();
            });
        
        registerCommand("file list", "List files in directory",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    fileList(args[0]);
                } else {
                    std::cout << "Error: Path required for file list." << std::endl;
                }
            });
        
        registerCommand("file read", "Read file contents",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    fileRead(args[0]);
                } else {
                    std::cout << "Error: File path required for file read." << std::endl;
                }
            });
        
        registerCommand("file write", "Write file contents",
            [this](const std::vector<std::string>& args) {
                if (args.size() >= 2) {
                    fileWrite(args[0], args[1]);
                } else {
                    std::cout << "Error: File path and content required for file write." << std::endl;
                }
            });
        
        // System commands
        registerCommand("exit", "Exit interactive console",
            [this](const std::vector<std::string>& args) {
                exitConsole();
            });
        
        registerCommand("shutdown", "Gracefully shutdown simulation",
            [this](const std::vector<std::string>& args) {
                shutdownSimulation();
            });
        
        registerCommand("help", "Show help menu",
            [this](const std::vector<std::string>& args) {
                showHelp(args);
            });
        
        registerCommand("screen", "Toggle screen clearing",
            [this](const std::vector<std::string>& args) {
                toggleScreenClear();
            });
        
        registerCommand("refresh", "Set auto-refresh interval",
            [this](const std::vector<std::string>& args) {
                if (args.size() > 0) {
                    try {
                        double interval = std::stod(args[0]);
                        setAutoRefreshInterval(interval);
                    } catch (const std::exception& e) {
                        std::cout << "Error: Invalid refresh interval. " << e.what() << std::endl;
                    }
                } else {
                    std::cout << "Error: Refresh interval required." << std::endl;
                }
            });
    }
    
    void initializeEventFilters() {
        // Initialize default event filters
        eventFilters_[
            "default"] = EventFilter();
        
        eventFilters_[
            "spike"] = EventFilter({
                NeuralEventType::NeuronFired,
                NeuralEventType::SpikePatternDetected,
                NeuralEventType::BurstEvent
            });
        
        eventFilters_[
            "plasticity"] = EventFilter({
                NeuralEventType::SynapticWeightChanged,
                NeuralEventType::STDPTriggered,
                NeuralEventType::HebbianLearning
            });
        
        eventFilters_[
            "memory"] = EventFilter({
                NeuralEventType::MemoryEncoding,
                NeuralEventType::MemoryRetrieval,
                NeuralEventType::MemoryConsolidation
            });
        
        eventFilters_[
            "neuromodulation"] = EventFilter({
                NeuralEventType::DopamineRelease,
                NeuralEventType::SerotoninRelease,
                NeuralEventType::AcetylcholineRelease
            });
    }
    
    void registerCommand(const std::string& name, const std::string& description,
                        const std::function<void(const std::vector<std::string>&)>& handler) {
        commands_[name] = {
            description,
            handler
        };
    }
    
    void processInput(const std::string& input) {
        // Parse command
        std::istringstream iss(input);
        std::string token;
        std::vector<std::string> args;
        
        while (iss >> token) {
            args.push_back(token);
        }
        
        if (args.empty()) {
            return;
        }
        
        // Find command
        std::string command = args[0];
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        // Remove command from args
        args.erase(args.begin());
        
        // Execute command
        auto it = commands_.find(command);
        if (it != commands_.end()) {
            try {
                it->second.handler(args);
            } catch (const std::exception& e) {
                std::cout << "Error executing command: " << e.what() << std::endl;
            }
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
    
    void showHelp(const std::vector<std::string>& args) {
        if (args.size() > 0) {
            // Show specific command help
            showCommandHelp(args[0]);
        } else {
            // Show general help
            std::cout << "=== NLM Interactive Debug Console - Help ===" << std::endl;
            std::cout << std::endl;
            std::cout << "Available commands:" << std::endl;
            
            for (const auto& [name, info] : commands_) {
                std::cout << "  " << name << " - " << info.description << std::endl;
            }
            
            std::cout << std::endl;
            std::cout << "Type 'help <command>' for detailed information about a specific command." << std::endl;
        }
    }
    
    void showCommandHelp(const std::string& command_name) {
        std::string command = command_name;
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        auto it = commands_.find(command);
        if (it != commands_.end()) {
            std::cout << "=== " << command << " ===" << std::endl;
            std::cout << it->second.description << std::endl;
            std::cout << std::endl;
            
            // Show usage examples based on command type
            showCommandExamples(command);
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }
    
    void listCommands() {
        std::cout << "=== Available Commands ===" << std::endl;
        
        int max_length = 0;
        for (const auto& [name, info] : commands_) {
            max_length = std::max(max_length, static_cast<int>(name.length()));
        }
        
        for (const auto& [name, info] : commands_) {
            std::cout << "  " << name << std::string(max_length - name.length(), ' ') 
                      << " - " << info.description << std::endl;
        }
    }
    
    void showCommandExamples(const std::string& command) {
        // Provide usage examples for common commands
        if (command == "help") {
            std::cout << "Usage: help [command]" << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  help                    Show all available commands" << std::endl;
            std::cout << "  help brain reset        Show help for brain reset command" << std::endl;
        } else if (command == "brain reset") {
            std::cout << "Usage: brain reset" << std::endl;
            std::cout << "Description: Reset brain to initial state" << std::endl;
            std::cout << "Example: brain reset" << std::endl;
        } else if (command == "brain save") {
            std::cout << "Usage: brain save <file>" << std::endl;
            std::cout << "Description: Save brain state to file" << std::endl;
            std::cout << "Example: brain save brain_state.json" << std::endl;
        } else if (command == "simulate") {
            std::cout << "Usage: simulate <steps>" << std::endl;
            std::cout << "Description: Run simulation for specified steps" << std::endl;
            std::cout << "Example: simulate 1000" << std::endl;
        } else if (command == "inject") {
            std::cout << "Usage: inject <neuron_id> <current>" << std::endl;
            std::cout << "Description: Inject current into specific neuron" << std::endl;
            std::cout << "Example: inject 42 50.0" << std::endl;
        } else if (command == "neurons") {
            std::cout << "Usage: neurons [neuron_id]" << std::endl;
            std::cout << "Description: List all neurons or show specific neuron" << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  neurons                    List all neurons" << std::endl;
            std::cout << "  neurons 42                Show details for neuron 42" << std::endl;
        } else if (command == "events") {
            std::cout << "Usage: events [filter]" << std::endl;
            std::cout << "Description: Show events or filter events" << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  events                    Show all recent events" << std::endl;
            std::cout << "  events spike              Show spike-related events" << std::endl;
            std::cout << "  events --type NeuronFired Show only neuron fired events" << std::endl;
        }
    }
    
    void brainReset() {
        if (brain_) {
            brain_->reset();
            std::cout << "Brain reset completed." << std::endl;
            updateDisplay();
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void brainStatus() {
        if (brain_) {
            brain_->logStatus();
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void brainInitialize() {
        if (brain_) {
            if (brain_->initialize()) {
                std::cout << "Brain initialized successfully." << std::endl;
            } else {
                std::cout << "Error: Failed to initialize brain." << std::endl;
            }
            updateDisplay();
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void brainSave(const std::string& filepath) {
        if (brain_) {
            if (brain_->save(filepath)) {
                std::cout << "Brain state saved to: " << filepath << std::endl;
            } else {
                std::cout << "Error: Failed to save brain state to " << filepath << std::endl;
            }
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void brainLoad(const std::string& filepath) {
        if (brain_) {
            if (brain_->load(filepath)) {
                std::cout << "Brain state loaded from: " << filepath << std::endl;
            } else {
                std::cout << "Error: Failed to load brain state from " << filepath << std::endl;
            }
            updateDisplay();
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void simulate(size_t steps) {
        if (!simulation_paused_) {
            std::cout << "Simulation already running. Use 'pause' to pause." << std::endl;
            return;
        }
        
        std::cout << "Running simulation for " << steps << " steps..." << std::endl;
        
        for (size_t i = 0; i < steps && running_ && !simulation_paused_; ++i) {
            brain_->step(i);
            
            if (auto_refresh_enabled_) {
                updateDisplay();
            }
            
            // Check if we should pause
            std::this_thread::sleep_for(std::chrono::milliseconds(
                static_cast<int>(1000.0 / (simulation_speed_ * 10.0))));
        }
        
        std::cout << "Simulation completed." << std::endl;
        updateDisplay();
    }
    
    void simulateStep() {
        if (brain_) {
            brain_->step(current_step_);
            current_step_++;
            updateDisplay();
            std::cout << "Simulation step completed: " << (current_step_ - 1) << std::endl;
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void pauseSimulation() {
        simulation_paused_ = true;
        std::cout << "Simulation paused." << std::endl;
    }
    
    void resumeSimulation() {
        simulation_paused_ = false;
        std::cout << "Simulation resumed." << std::endl;
    }
    
    void setSimulationSpeed(double speed) {
        simulation_speed_ = speed;
        std::cout << "Simulation speed set to: " << speed << "x" << std::endl;
    }
    
    void setSimulationDuration(double duration) {
        simulation_duration_ = duration;
        std::cout << "Simulation duration set to: " << duration << " seconds" << std::endl;
    }
    
    void setCurrentTime(double time) {
        current_time_ = time;
        std::cout << "Current simulation time set to: " << time << " seconds" << std::endl;
    }
    
    void injectCurrent(int neuron_id, double current) {
        if (brain_) {
            brain_->injectCurrent(neuron_id, current);
            std::cout << "Current injected into neuron " << neuron_id << ": " << current << std::endl;
            updateDisplay();
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void listNeurons() {
        if (brain_) {
            std::cout << "=== Neurons ===" << std::endl;
            size_t total_neurons = brain_->getTotalNeuronCount();
            std::cout << "Total neurons: " << total_neurons << std::endl;
            
            for (size_t i = 0; i < std::min<size_t>(10, total_neurons); ++i) {
                std::cout << "  Neuron " << i << ": Type=" << static_cast<int>(brain_->getNeuronType(i)) 
                          << ", Active=" << (brain_->getNeuronState(i) ? "Yes" : "No") << std::endl;
            }
            
            if (total_neurons > 10) {
                std::cout << "  ... and " << (total_neurons - 10) << " more neurons" << std::endl;
            }
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void showNeuronDetails(int neuron_id) {
        if (brain_) {
            std::cout << "=== Neuron Details ===" << std::endl;
            std::cout << "Neuron ID: " << neuron_id << std::endl;
            std::cout << "Type: " << static_cast<int>(brain_->getNeuronType(neuron_id)) << std::endl;
            std::cout << "Region: " << brain_->getNeuronRegion(neuron_id) << std::endl;
            std::cout << "Population: " << brain_->getNeuronPopulation(neuron_id) << std::endl;
            std::cout << "State: " << (brain_->getNeuronState(neuron_id) ? "Active" : "Inactive") << std::endl;
            std::cout << "Firing rate: " << brain_->getNeuronFiringRate(neuron_id) << std::endl;
            std::cout << "Membrane potential: " << brain_->getNeuronMembranePotential(neuron_id) << std::endl;
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void showRecentSpikes(size_t count) {
        if (brain_) {
            std::cout << "=== Recent Spikes ===" << std::endl;
            
            // Get spike events from event system
            auto spike_filter = eventFilters_["spike"];
            auto events = event_system_->getEvents(spike_filter);
            
            size_t display_count = std::min(count, events.size());
            for (size_t i = 0; i < display_count; ++i) {
                const auto& event = events[events.size() - 1 - i];
                std::cout << "  " << event.timestamp << ": Neuron " 
                          << event.sourceNeuronId << " (Data: " << event.eventData << ")" 
                          << std::endl;
            }
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void showSpikeHistory(int neuron_id) {
        if (brain_) {
            std::cout << "=== Spike History for Neuron " << neuron_id << " ===" << std::endl;
            
            // Get spike events for this neuron
            auto spike_filter = EventFilter();
            spike_filter.sourceNeurons.insert(neuron_id);
            auto events = event_system_->getEvents(spike_filter);
            
            for (const auto& event : events) {
                std::cout << "  " << event.timestamp << ": Data=" << event.eventData 
                          << ", Processed=" << (event.isProcessed ? "Yes" : "No") << std::endl;
            }
            
            if (events.empty()) {
                std::cout << "  No spike history available." << std::endl;
            }
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void showEvents() {
        if (event_system_) {
            std::cout << "=== Recent Events ===" << std::endl;
            
            auto events = event_system_->getUnprocessedEvents();
            size_t display_count = std::min<size_t>(10, events.size());
            
            for (size_t i = 0; i < display_count; ++i) {
                const auto& event = events[i];
                std::cout << "  " << event.timestamp << ": " << static_cast<int>(event.type)
                          << " (Source: " << event.sourceNeuronId 
                          << ", Target: " << event.targetNeuronId << ")"
                          << std::endl;
            }
            
            if (events.empty()) {
                std::cout << "  No events available." << std::endl;
            }
        } else {
            std::cout << "Error: Event system not initialized." << std::endl;
        }
    }
    
    void eventFilter(const std::string& filter_name) {
        if (event_system_) {
            auto it = eventFilters_.find(filter_name);
            if (it != eventFilters_.end()) {
                auto events = event_system_->getEvents(it->second);
                std::cout << "=== Events matching filter: " << filter_name << " ===" << std::endl;
                
                for (const auto& event : events) {
                    std::cout << "  " << event.timestamp << ": " << static_cast<int>(event.type)
                              << " (Source: " << event.sourceNeuronId 
                              << ", Target: " << event.targetNeuronId << ")"
                              << std::endl;
                }
                
                if (events.empty()) {
                    std::cout << "  No events match this filter." << std::endl;
                }
            } else {
                std::cout << "Error: Unknown filter: " << filter_name << std::endl;
                std::cout << "Available filters: ";
                bool first = true;
                for (const auto& [name, filter] : eventFilters_) {
                    if (!first) std::cout << ", ";
                    std::cout << name;
                    first = false;
                }
                std::cout << std::endl;
            }
        } else {
            std::cout << "Error: Event system not initialized." << std::endl;
        }
    }
    
    void eventSystemStatus() {
        if (event_system_) {
            std::cout << event_system_->getEventSystemStatus() << std::endl;
        } else {
            std::cout << "Error: Event system not initialized." << std::endl;
        }
    }
    
    void eventSystemEnable(bool enabled) {
        if (event_system_) {
            event_system_->enableEventProcessing(enabled);
            std::cout << "Event system " << (enabled ? "enabled" : "disabled") << std::endl;
        } else {
            std::cout << "Error: Event system not initialized." << std::endl;
        }
    }
    
    void subscribeToEvents(const std::string& filter_name) {
        if (event_system_) {
            auto it = eventFilters_.find(filter_name);
            if (it != eventFilters_.end()) {
                // Create a simple callback for demonstration
                auto callback = [this](const NeuralEvent& event, bool& processed) {
                    // Process event (for demo, just print)
                    std::cout << "Event processed: " << static_cast<int>(event.type) << std::endl;
                    processed = true;
                };
                
                event_system_->subscribe(it->second, callback);
                std::cout << "Subscribed to events with filter: " << filter_name << std::endl;
            } else {
                std::cout << "Error: Unknown filter: " << filter_name << std::endl;
            }
        } else {
            std::cout << "Error: Event system not initialized." << std::endl;
        }
    }
    
    void unsubscribeFromEvents(const std::string& callback_id) {
        if (event_system_) {
            // For demo, we don't track individual callbacks
            std::cout << "Unsubscribe functionality requires callback tracking." << std::endl;
        } else {
            std::cout << "Error: Event system not initialized." << std::endl;
        }
    }
    
    void showConfig() {
        if (config_) {
            std::cout << config_->summary() << std::endl;
        } else {
            std::cout << "Error: Configuration not initialized." << std::endl;
        }
    }
    
    void configSet(const std::string& key, const std::string& value) {
        if (config_) {
            config_->set(key, value, ConfigSource::Runtime);
            std::cout << "Configuration updated: " << key << " = " << value << std::endl;
        } else {
            std::cout << "Error: Configuration not initialized." << std::endl;
        }
    }
    
    void configGet(const std::string& key) {
        if (config_) {
            auto value = config_->get<std::string>(key);
            if (value.has_value()) {
                std::cout << key << " = " << value.value() << std::endl;
            } else {
                std::cout << "Key not found: " << key << std::endl;
            }
        } else {
            std::cout << "Error: Configuration not initialized." << std::endl;
        }
    }
    
    void configSave(const std::string& filepath) {
        if (config_) {
            if (config_->saveToFile(filepath)) {
                std::cout << "Configuration saved to: " << filepath << std::endl;
            } else {
                std::cout << "Error: Failed to save configuration to " << filepath << std::endl;
            }
        } else {
            std::cout << "Error: Configuration not initialized." << std::endl;
        }
    }
    
    void configLoad(const std::string& filepath) {
        if (config_) {
            if (config_->loadFromFile(filepath)) {
                std::cout << "Configuration loaded from: " << filepath << std::endl;
            } else {
                std::cout << "Error: Failed to load configuration from " << filepath << std::endl;
            }
        } else {
            std::cout << "Error: Configuration not initialized." << std::endl;
        }
    }
    
    void configKeys() {
        if (config_) {
            std::cout << "=== Configuration Keys ===" << std::endl;
            auto keys = config_->getKeys();
            for (const auto& key : keys) {
                std::cout << "  " << key << std::endl;
            }
        } else {
            std::cout << "Error: Configuration not initialized." << std::endl;
        }
    }
    
    void showStatistics() {
        if (brain_) {
            std::cout << "=== Brain Statistics ===" << std::endl;
            std::cout << "Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
            std::cout << "Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
            std::cout << "Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
            std::cout << "Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
            std::cout << "Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
            std::cout << "Average firing rate: " << brain_->getAverageFiringRate() << std::endl;
            std::cout << "Excitatory/Inhibitory ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
            
            if (event_system_) {
                std::cout << std::endl << "=== Event System Statistics ===" << std::endl;
                std::cout << event_system_->getEventSystemStatus() << std::endl;
            }
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void showNeuronStatistics() {
        if (brain_) {
            std::cout << "=== Neuron Statistics ===" << std::endl;
            size_t total_neurons = brain_->getTotalNeuronCount();
            
            // Basic statistics
            std::cout << "Total neurons: " << total_neurons << std::endl;
            std::cout << "Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
            std::cout << "Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
            
            if (total_neurons > 0) {
                double firing_rate = static_cast<double>(brain_->getFiringNeuronCount()) / 
                                   static_cast<double>(total_neurons) * 100.0;
                std::cout << "Firing rate percentage: " << std::fixed << std::setprecision(2) << firing_rate << "%" << std::endl;
            }
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void showSynapseStatistics() {
        if (brain_) {
            std::cout << "=== Synapse Statistics ===" << std::endl;
            std::cout << "Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
            
            // Calculate synapse statistics based on brain state
            double connection_density = 0.0;
            if (brain_->getTotalNeuronCount() > 1) {
                connection_density = static_cast<double>(brain_->getTotalSynapseCount()) / 
                                  static_cast<double>(brain_->getTotalNeuronCount() * 
                                                    (brain_->getTotalNeuronCount() - 1));
            }
            std::cout << "Connection density: " << std::fixed << std::setprecision(4) << connection_density << std::endl;
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void showMemoryStatistics() {
        if (brain_) {
            std::cout << "=== Memory System Statistics ===" << std::endl;
            
            // Access memory systems through brain interface
            auto working_memory = brain_->getWorkingMemory();
            auto episodic_memory = brain_->getEpisodicMemory();
            auto associative_memory = brain_->getAssociativeMemory();
            
            if (working_memory) {
                std::cout << "Working memory: Active" << std::endl;
            } else {
                std::cout << "Working memory: Not available" << std::endl;
            }
            
            if (episodic_memory) {
                std::cout << "Episodic memory: Active" << std::endl;
            } else {
                std::cout << "Episodic memory: Not available" << std::endl;
            }
            
            if (associative_memory) {
                std::cout << "Associative memory: Active" << std::endl;
            } else {
                std::cout << "Associative memory: Not available" << std::endl;
            }
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void showPerformanceStatistics() {
        if (brain_) {
            std::cout << "=== Performance Statistics ===" << std::endl;
            std::cout << "Simulation speed: " << simulation_speed_ << "x" << std::endl;
            std::cout << "Current step: " << current_step_ << std::endl;
            std::cout << "Current time: " << current_time_ << " seconds" << std::endl;
            std::cout << "Simulation duration: " << simulation_duration_ << " seconds" << std::endl;
            
            // Calculate performance metrics
            double steps_per_second = simulation_speed_ * 1000.0 / auto_refresh_interval_;
            std::cout << "Estimated steps per second: " << steps_per_second << std::endl;
            
            if (event_system_) {
                std::cout << std::endl << "=== Event System Performance ===" << std::endl;
                size_t total_events = event_system_->getEventCount();
                std::cout << "Total events processed: " << total_events << std::endl;
                
                if (total_events > 0) {
                    double processing_rate = total_events / simulation_duration_;
                    std::cout << "Event processing rate: " << processing_rate << " events/second" << std::endl;
                }
            }
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void analyzeSpikePatterns() {
        if (brain_) {
            std::cout << "=== Spike Pattern Analysis ===" << std::endl;
            
            // Get spike events
            auto spike_filter = eventFilters_["spike"];
            auto events = event_system_->getEvents(spike_filter);
            
            if (events.empty()) {
                std::cout << "No spike events available for analysis." << std::endl;
                return;
            }
            
            // Analyze spike patterns
            std::cout << "Total spike events: " << events.size() << std::endl;
            
            // Group spikes by source neuron
            std::map<int, size_t> neuron_spike_counts;
            for (const auto& event : events) {
                neuron_spike_counts[event.sourceNeuronId]++;
            }
            
            std::cout << "Spike activity by neuron:" << std::endl;
            for (const auto& [neuron_id, count] : neuron_spike_counts) {
                std::cout << "  Neuron " << neuron_id << ": " << count << " spikes" << std::endl;
            }
            
            // Detect burst patterns (multiple spikes close in time)
            std::cout << "Burst detection:" << std::endl;
            size_t burst_count = 0;
            for (size_t i = 1; i < events.size(); ++i) {
                double time_diff = events[i].timestamp - events[i-1].timestamp;
                if (time_diff < 0.01) {  // Within 10ms
                    burst_count++;
                }
            }
            
            std::cout << "  Bursts detected: " << burst_count << std::endl;
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void analyzeConnectivity() {
        if (brain_) {
            std::cout << "=== Connectivity Analysis ===" << std::endl;
            
            size_t total_neurons = brain_->getTotalNeuronCount();
            size_t total_synapses = brain_->getTotalSynapseCount();
            
            if (total_neurons == 0) {
                std::cout << "No neurons available for analysis." << std::endl;
                return;
            }
            
            // Calculate connectivity metrics
            double connection_probability = static_cast<double>(total_synapses) / 
                                          static_cast<double>(total_neurons * (total_neurons - 1));
            
            std::cout << "Network size: " << total_neurons << " neurons" << std::endl;
            std::cout << "Total connections: " << total_synapses << std::endl;
            std::cout << "Connection probability: " << std::fixed << std::setprecision(6) << connection_probability << std::endl;
            
            // Calculate clustering coefficient (simplified)
            double average_degree = total_synapses * 2.0 / total_neurons;  // Assuming directed graph
            std::cout << "Average degree: " << std::fixed << std::setprecision(2) << average_degree << std::endl;
            
            // Determine network topology
            if (connection_probability < 0.01) {
                std::cout << "Network type: Sparse (small-world)" << std::endl;
            } else if (connection_probability > 0.1) {
                std::cout << "Network type: Dense (random)" << std::endl;
            } else {
                std::cout << "Network type: Medium connectivity" << std::endl;
            }
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void analyzePlasticity() {
        if (brain_) {
            std::cout << "=== Plasticity Analysis ===" << std::endl;
            
            // Get plasticity events
            auto plasticity_filter = eventFilters_["plasticity"];
            auto events = event_system_->getEvents(plasticity_filter);
            
            std::cout << "Total plasticity events: " << events.size() << std::endl;
            
            if (events.empty()) {
                std::cout << "No plasticity events available." << std::endl;
                return;
            }
            
            // Analyze plasticity by type
            size_t stdp_count = 0;
            size_t hebbian_count = 0;
            size_t reward_count = 0;
            
            for (const auto& event : events) {
                switch (event.type) {
                    case NeuralEventType::STDPTriggered:
                        stdp_count++;
                        break;
                    case NeuralEventType::HebbianLearning:
                        hebbian_count++;
                        break;
                    case NeuralEventType::SynapticWeightChanged:
                        if (event.eventData > 0) {
                            reward_count++;
                        }
                        break;
                    default:
                        break;
                }
            }
            
            std::cout << "STDP events: " << stdp_count << std::endl;
            std::cout << "Hebbian learning events: " << hebbian_count << std::endl;
            std::cout << "Reward-modulated plasticity: " << reward_count << std::endl;
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void analyzeActivity() {
        if (brain_) {
            std::cout << "=== Overall Activity Analysis ===" << std::endl;
            
            size_t total_neurons = brain_->getTotalNeuronCount();
            size_t active_neurons = brain_->getActiveNeuronCount();
            size_t firing_neurons = brain_->getFiringNeuronCount();
            size_t total_spikes = brain_->getTotalSpikeCount();
            
            std::cout << "Activity overview:" << std::endl;
            std::cout << "  Active neurons: " << active_neurons << " (" 
                      << std::fixed << std::setprecision(2) << 
                      (total_neurons > 0 ? static_cast<double>(active_neurons) / total_neurons * 100.0 : 0.0) << "%)" << std::endl;
            std::cout << "  Firing neurons: " << firing_neurons << " (" 
                      << std::fixed << std::setprecision(2) << 
                      (total_neurons > 0 ? static_cast<double>(firing_neurons) / total_neurons * 100.0 : 0.0) << "%)" << std::endl;
            std::cout << "  Total spikes: " << total_spikes << std::endl;
            
            // Calculate firing rate
            double firing_rate = (total_neurons > 0 && simulation_duration_ > 0) ? 
                               static_cast<double>(total_spikes) / simulation_duration_ : 0.0;
            std::cout << "  Average firing rate: " << std::fixed << std::setprecision(2) << firing_rate << " spikes/second" << std::endl;
            
            // Get event statistics if available
            if (event_system_) {
                std::cout << std::endl << "Event system activity:" << std::endl;
                for (int i = 0; i < static_cast<int>(NeuralEventType::CustomEvent) + 1; ++i) {
                    NeuralEventType type = static_cast<NeuralEventType>(i);
                    size_t processed = event_system_->getProcessedEventCount(type);
                    size_t unprocessed = event_system_->getUnprocessedEventCount(type);
                    
                    if (processed > 0 || unprocessed > 0) {
                        std::cout << "  " << static_cast<int>(type) << ": " 
                                  << "Processed=" << processed << ", Unprocessed=" << unprocessed << std::endl;
                    }
                }
            }
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void detectAnomalies() {
        if (brain_) {
            std::cout << "=== Anomaly Detection ===" << std::endl;
            
            // Check for common anomalies
            size_t total_neurons = brain_->getTotalNeuronCount();
            size_t total_spikes = brain_->getTotalSpikeCount();
            
            std::vector<std::string> anomalies;
            
            // Check for excessive firing
            if (total_neurons > 0 && total_spikes > 1000000) {
                anomalies.push_back("Very high spike count: " + std::to_string(total_spikes));
            }
            
            // Check for zero activity
            if (total_spikes == 0) {
                anomalies.push_back("No spike activity detected");
            }
            
            // Check for very low firing
            double firing_rate = (total_neurons > 0 && simulation_duration_ > 0) ? 
                               static_cast<double>(total_spikes) / simulation_duration_ : 0.0;
            if (firing_rate < 0.01) {
                anomalies.push_back("Extremely low firing rate: " + std::to_string(firing_rate) + " spikes/second");
            }
            
            // Check event system health
            if (event_system_) {
                size_t total_events = event_system_->getEventCount();
                size_t unprocessed = 0;
                for (int i = 0; i < static_cast<int>(NeuralEventType::CustomEvent) + 1; ++i) {
                    NeuralEventType type = static_cast<NeuralEventType>(i);
                    unprocessed += event_system_->getUnprocessedEventCount(type);
                }
                
                if (unprocessed > 1000) {
                    anomalies.push_back("High number of unprocessed events: " + std::to_string(unprocessed));
                }
            }
            
            if (anomalies.empty()) {
                std::cout << "No anomalies detected." << std::endl;
            } else {
                std::cout << "Anomalies detected:" << std::endl;
                for (const auto& anomaly : anomalies) {
                    std::cout << "  - " << anomaly << std::endl;
                }
            }
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
        }
    }
    
    void logSave(const std::string& filepath) {
        std::ofstream log_file(filepath);
        if (log_file.is_open()) {
            log_file << "=== NLM Interactive Debug Console Log ===" << std::endl;
            log_file << "Log saved at: " << std::chrono::system_clock::now() << std::endl;
            log_file << "Brain state: " << (brain_ ? "Initialized" : "Not initialized") << std::endl;
            log_file << "Event system: " << (event_system_ ? "Initialized" : "Not initialized") << std::endl;
            
            if (brain_) {
                log_file << "Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
                log_file << "Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
                log_file << "Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
            }
            
            log_file.close();
            std::cout << "Log saved to: " << filepath << std::endl;
        } else {
            std::cout << "Error: Failed to save log to " << filepath << std::endl;
        }
    }
    
    void logAppend(const std::string& filepath) {
        std::ofstream log_file(filepath, std::ios::app);
        if (log_file.is_open()) {
            log_file << std::chrono::system_clock::now() << ": Log appended" << std::endl;
            log_file.close();
            std::cout << "Log appended to: " << filepath << std::endl;
        } else {
            std::cout << "Error: Failed to append log to " << filepath << std::endl;
        }
    }
    
    void logClear() {
        // In a real implementation, this would clear the log buffer
        std::cout << "Log cleared." << std::endl;
    }
    
    void fileList(const std::string& path) {
        std::cout << "Listing files in: " << path << std::endl;
        std::cout << "(Note: Directory listing not implemented in this demo)" << std::endl;
    }
    
    void fileRead(const std::string& path) {
        std::cout << "Reading file: " << path << std::endl;
        std::cout << "(Note: File reading not implemented in this demo)" << std::endl;
    }
    
    void fileWrite(const std::string& path, const std::string& content) {
        std::cout << "Writing file: " << path << std::endl;
        std::cout << "(Note: File writing not implemented in this demo)" << std::endl;
    }
    
    void exitConsole() {
        std::cout << "Exiting interactive console..." << std::endl;
        running_ = false;
    }
    
    void shutdownSimulation() {
        std::cout << "Shutting down simulation..." << std::endl;
        running_ = false;
        if (event_system_) {
            event_system_->clearEvents();
        }
    }
    
    void toggleScreenClear() {
        screen_clear_enabled_ = !screen_clear_enabled_;
        std::cout << "Screen clearing " << (screen_clear_enabled_ ? "enabled" : "disabled") << std::endl;
    }
    
    void setAutoRefreshInterval(double interval) {
        auto_refresh_interval_ = interval;
        if (interval > 0) {
            auto_refresh_enabled_ = true;
            std::cout << "Auto-refresh enabled with interval: " << interval << " seconds" << std::endl;
        } else {
            auto_refresh_enabled_ = false;
            std::cout << "Auto-refresh disabled" << std::endl;
        }
    }
    
    void startConsoleThread() {
        console_thread_ = std::thread(&InteractiveDebugConsole::consoleUpdateLoop, this);
    }
    
    void stopConsoleThread() {
        running_ = false;
        if (console_thread_.joinable()) {
            console_thread_.join();
        }
    }
    
    void consoleUpdateLoop() {
        while (running_) {
            if (auto_refresh_enabled_) {
                updateDisplay();
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(
                static_cast<int>(auto_refresh_interval_ * 1000.0)));
        }
    }
    
    void updateDisplay() {
        if (screen_clear_enabled_) {
            system("clear");
        }
        
        // Update display with current brain state
        if (brain_) {
            std::cout << "=== NLM Interactive Debug Console ===" << std::endl;
            std::cout << "Simulation: " << (simulation_paused_ ? "PAUSED" : "RUNNING") 
                      << " (Speed: " << simulation_speed_ << "x)" << std::endl;
            std::cout << "Step: " << current_step_ << ", Time: " << current_time_ << "s" << std::endl;
            std::cout << "Brain: " << brain_->getTotalNeuronCount() << " neurons, " 
                      << brain_->getTotalSynapseCount() << " synapses" << std::endl;
            
            if (event_system_) {
                std::cout << "Events: " << event_system_->getEventCount() 
                          << " (Unprocessed: " << event_system_->getEventCount() << ")" << std::endl;
            }
            
            std::cout << std::endl;
        }
    }
    
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
        if (brain_) {
            // Initialize event system
            event_system_ = std::make_shared<NeuralEventSystem>();
            
            // Register event generators
            size_t neuron_count = brain_->getTotalNeuronCount();
            for (size_t i = 0; i < neuron_count; ++i) {
                event_system_->addEventGenerator(i);
            }
        }
    }
    
    void setLogger(std::shared_ptr<Logger> logger) {
        logger_ = logger;
    }
    
    void setConfig(std::shared_ptr<Config> config) {
        config_ = config;
    }

private:
    struct CommandInfo {
        std::string description;
        std::function<void(const std::vector<std::string>&)> handler;
    };
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<NeuralEventSystem> event_system_;
    
    std::map<std::string, CommandInfo> commands_;
    std::map<std::string, EventFilter> eventFilters_;
    
    // Console state
    bool running_ = true;
    bool simulation_paused_ = false;
    bool auto_refresh_enabled_ = false;
    bool screen_clear_enabled_ = true;
    double auto_refresh_interval_ = 1.0;
    double simulation_speed_ = 1.0;
    double simulation_duration_ = 0.0;
    uint64_t current_step_ = 0;
    double current_time_ = 0.0;
    
    std::thread console_thread_;
};

// Global instance for easy access
static InteractiveDebugConsole* g_console_instance = nullptr;

// Function to start interactive console
void startInteractiveConsole(std::shared_ptr<Brain> brain, 
                            std::shared_ptr<Logger> logger,
                            std::shared_ptr<Config> config) {
    // Create and start console
    g_console_instance = new InteractiveDebugConsole(brain, logger, config);
    
    // Configure console
    g_console_instance->setAutoRefresh(true, 1.0);
    g_console_instance->setScreenClear(true);
    
    // Start console
    g_console_instance->start();
}

// Function to stop interactive console
void stopInteractiveConsole() {
    if (g_console_instance) {
        g_console_instance->stop();
        delete g_console_instance;
        g_console_instance = nullptr;
    }
}

// Main function for interactive console mode
int main(int argc, char** argv) {
    // Parse command line arguments
    bool help = false;
    std::string config_file = "configs/default.cfg";
    bool debug = false;
    bool benchmark = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            help = true;
        } else if (arg == "--config" && i + 1 < argc) {
            config_file = argv[++i];
        } else if (arg == "--debug") {
            debug = true;
        } else if (arg == "--benchmark") {
            benchmark = true;
        }
    }
    
    if (help) {
        std::cout << "=== NLM Interactive Debug Console ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: ./nlm_interactive [--help] [--config=file] [--debug] [--benchmark]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show help information" << std::endl;
        std::cout << "  --config=file            Load configuration from file" << std::endl;
        std::cout << "  --debug                  Enable debug mode" << std::endl;
        std::cout << "  --benchmark              Enable benchmark mode" << std::endl;
        std::cout << std::endl;
        std::cout << "The interactive console provides real-time interaction with the NLM brain." << std::endl;
        std::cout << "Type 'help' for available commands." << std::endl;
        return 0;
    }
    
    // Initialize NLM
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(config_file)) {
        std::cout << "Warning: Failed to load configuration from " << config_file 
                  << ". Using default configuration." << std::endl;
    }
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cout << "Error: Failed to initialize brain." << std::endl;
        return 1;
    }
    
    // Create logger
    auto logger = std::make_shared<Logger>();
    auto console_logger = std::make_shared<ConsoleLogger>(debug ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(console_logger);
    Logger::setGlobal(logger);
    
    // Display brain information
    brain->logStatus();
    
    // Start interactive console
    std::cout << "Starting interactive console..." << std::endl;
    startInteractiveConsole(brain, logger, config);
    
    // Clean up
    stopInteractiveConsole();
    
    return 0;
}

// Function to check if console is running
bool isConsoleRunning() {
    return g_console_instance != nullptr && g_console_instance->isRunning();
}

// Function to check console status
std::string getConsoleStatus() {
    if (g_console_instance) {
        return g_console_instance->getStatus();
    }
    return "Console not running";
}