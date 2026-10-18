// CommandLineParser implementation

#include "CommandLineParser.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

CommandLineParser::CommandLineParser()
    : helpRequested(false), versionRequested(false), simulationSpeed(1.0f) {
    setupDefaultCommands();
}

CommandLineParser::~CommandLineParser() = default;

void CommandLineParser::setupDefaultCommands() {
    // Register basic commands
    registerCommand("help", "Show help information", "help", {
        "-h", "--help"
    }, [this]() { printHelp(); });

    registerCommand("version", "Show version information", "version", {
        "-v", "--version"
    }, [this]() { printVersion(); });

    registerCommand("create", "Create a new brain instance", 
                   "create [options]", {
        "-c", "--config", "-n", "--neurons", "-r", "--regions",
        "--stdp", "--hebbian", "--structural"
    }, [this]() {
        std::map<std::string, std::string> config;
        for (const auto& [key, value] : options) {
            config[key] = value;
        }
        createBrain(config);
    });

    registerCommand("run", "Run the brain simulation", "run [steps] [duration]", {
        "-s", "--steps", "-d", "--duration", "-c", "--config",
        "--test1", "--test2", "--test3", "--plasticity",
        "--continuous", "--auto-save", "--save-interval"
    }, [this]() {
        std::vector<std::string> commands;
        // Extract run-specific options
        if (options.count("auto-save")) {
            commands.push_back("auto-save:" + options.at("auto-save"));
        }
        if (options.count("save-interval")) {
            commands.push_back("save-interval:" + options.at("save-interval"));
        }
        runBrain(options, commands);
    });

    registerCommand("save", "Save brain state to file", "save <filename>", {
        "-f", "--filename", "--binary", "--compressed"
    }, [this]() {
        std::string filename = options.count("filename") ? options.at("filename") : "brain_state.nbl";
        saveBrain(filename);
    });

    registerCommand("load", "Load brain state from file", "load <filename>", {
        "-f", "--filename", "--validate"
    }, [this]() {
        std::string filename = options.count("filename") ? options.at("filename") : "brain_state.nbl";
        loadBrain(filename);
    });

    registerCommand("reset", "Reset brain to initial state", "reset", {
        "--hard", "--clear-memory"
    }, [this]() {
        resetBrain();
    });

    registerCommand("stats", "Display brain statistics", "stats", {
        "-v", "--verbose", "--json", "--html"
    }, [this]() {
        if (currentBrain) {
            currentBrain->logStatus();
        }
    });

    registerCommand("benchmark", "Run performance benchmark", "benchmark [steps]", {
        "-s", "--steps", "-w", "--warmup", "--detailed"
    }, [this]() {
        // Execute benchmark command
        int steps = options.count("steps") ? std::stoi(options.at("steps")) : 1000;
        // TODO: Implement benchmark execution
    });

    registerCommand("export", "Export brain data", "export <format> <filename>", {
        "--format", "--neurons", "--synapses", "--weights", "--topology"
    }, [this]() {
        // Execute export command
        std::string format = options.count("format") ? options.at("format") : "txt";
        std::string filename = options.count("filename") ? options.at("filename") : "export.dat";
        // TODO: Implement export functionality
    });

    registerCommand("test", "Run neural network tests", "test <test_name>", {
        "--all", "--comprehensive", "--quick", "--memory", "--performance"
    }, [this]() {
        // Execute test command
        // TODO: Implement test execution
    });
}

bool CommandLineParser::parse(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
            printHelp();
            return true;
        }

        if (arg == "--version" || arg == "-v") {
            versionRequested = true;
            printVersion();
            return true;
        }

        if (arg.substr(0, 2) == "--") {
            // Long option
            std::string option = arg.substr(2);
            size_t equals = option.find('=');
            if (equals != std::string::npos) {
                std::string key = option.substr(0, equals);
                std::string value = option.substr(equals + 1);
                options[key] = value;
            } else {
                options[arg] = "true";
            }
        } else if (arg.substr(0, 1) == "-") {
            // Short option(s)
            for (size_t j = 1; j < arg.length(); ++j) {
                std::string option(1, arg[j]);
                // Check if next character is also a short option
                if (j + 1 < arg.length() && arg[j + 1] != '-' && !std::isalpha(arg[j + 1])) {
                    continue; // Already processed this combination
                }
                options[option] = "true";
            }
        } else {
            // Positional argument
            positionalArgs.push_back(arg);
        }
    }

    return true;
}

void CommandLineParser::registerCommand(const std::string& name, const std::string& description,
                                       const std::string& usage, const std::vector<std::string>& flags,
                                       std::function<void()> action, bool requiresBrain) {
    Command cmd;
    cmd.name = name;
    cmd.description = description;
    cmd.usage = usage;
    cmd.flags = flags;
    cmd.action = action;
    cmd.requiresBrain = requiresBrain;
    commands.push_back(cmd);
}

void CommandLineParser::printHelp() const {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm [options] [command] [args]" << std::endl;
    std::cout << std::endl;
    std::cout << "COMMANDS:" << std::endl;

    for (const auto& cmd : commands) {
        std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
        std::cout << "    Usage: " << cmd.usage << std::endl;
    }

    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --version           Show version information" << std::endl;
    std::cout << "  -c, --config <file>     Load configuration from file" << std::endl;
    std::cout << std::endl;
    std::cout << "For more information about a specific command, use:" << std::endl;
    std::cout << "  nlm help <command_name>" << std::endl;
}

void CommandLineParser::printVersion() const {
    std::cout << "NLM Phase 2 - Real Neural Computation" << std::endl;
    std::cout << "Version: 2.0.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Event-driven Spike Propagation" << std::endl;
    std::cout << "  - STDP and Hebbian Plasticity" << std::endl;
    std::cout << "  - Structural Plasticity" << std::endl;
}

bool CommandLineParser::createBrain(const std::map<std::string, std::string>& config) {
    try {
        auto brainConfig = std::make_shared<Config>();
        
        // Apply configuration
        for (const auto& [key, value] : config) {
            if (key == "neurons" || key == "neuron_count") {
                brainConfig->set("neuron_count", std::stoll(value), ConfigSource::CommandLine);
            } else if (key == "regions" || key == "region_count") {
                brainConfig->set("region_count", std::stoll(value), ConfigSource::CommandLine);
            } else if (key == "connection_probability" || key == "connectivity") {
                brainConfig->set("connection_probability", std::stof(value), ConfigSource::CommandLine);
            } else if (key == "stdp" || key == "enable_stdp") {
                brainConfig->set("stdp_enabled", std::stoi(value), ConfigSource::CommandLine);
            } else if (key == "hebbian" || key == "enable_hebbian") {
                brainConfig->set("hebbian_enabled", std::stoi(value), ConfigSource::CommandLine);
            } else if (key == "structural" || key == "enable_structural") {
                brainConfig->set("structural_plasticity_enabled", std::stoi(value), ConfigSource::CommandLine);
            }
        }

        // Set defaults for Phase 2
        brainConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        brainConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Create brain
        currentBrain = std::make_shared<Brain>(brainConfig);
        
        if (currentBrain->initialize()) {
            std::cout << "Brain created successfully!" << std::endl;
            return true;
        } else {
            std::cout << "Failed to create brain!" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "Error creating brain: " << e.what() << std::endl;
        return false;
    }
}

bool CommandLineParser::runBrain(const std::map<std::string, std::string>& config,
                                const std::vector<std::string>& commands) {
    if (!currentBrain) {
        std::cout << "No brain created yet. Use 'create' command first." << std::endl;
        return false;
    }

    try {
        // Parse run options
        int steps = 100;
        float duration = 1.0f;
        bool test1 = false;
        bool test2 = false;
        bool test3 = false;
        bool plasticity = false;
        bool continuous = false;
        std::string saveInterval;

        for (const auto& cmd : commands) {
            size_t colon = cmd.find(':');
            if (colon != std::string::npos) {
                std::string key = cmd.substr(0, colon);
                std::string value = cmd.substr(colon + 1);
                if (key == "auto-save") {
                    saveInterval = value;
                }
            }
        }

        if (config.count("steps")) {
            steps = std::stoi(config.at("steps"));
        }
        if (config.count("duration")) {
            duration = std::stof(config.at("duration"));
        }
        if (config.count("test1")) {
            test1 = true;
        }
        if (config.count("test2")) {
            test2 = true;
        }
        if (config.count("test3")) {
            test3 = true;
        }
        if (config.count("plastic")) {
            plasticity = true;
        }
        if (config.count("continuous")) {
            continuous = true;
        }

        std::cout << "Starting brain simulation..." << std::endl;
        std::cout << "Steps: " << steps << ", Duration: " << duration << "s" << std::endl;

        if (test1) {
            std::cout << "Running Test 1: Basic Connectivity Test" << std::endl;
            // This would call the test function from main.cpp
        }

        if (test2) {
            std::cout << "Running Test 2: Plasticity Experiment" << std::endl;
            // This would call the plasticity test function
        }

        if (test3) {
            std::cout << "Running Test 3: STDP Verification" << std::endl;
            // This would call the STDP test function
        }

        if (!plasticity) {
            // Run standard brain simulation
            for (int i = 0; i < steps; ++i) {
                currentBrain->step(i);
                
                if (continuous && i % 10 == 0) {
                    std::cout << "Step " << i << "/" << steps 
                              << " | Total spikes: " << currentBrain->getTotalSpikeCount()
                              << " | Active neurons: " << currentBrain->getActiveNeuronCount()
                              << " | Average firing rate: " << currentBrain->getAverageFiringRate()
                              << std::endl;
                }

                // Auto-save if interval specified
                if (!saveInterval.empty() && i > 0 && i % std::stoi(saveInterval) == 0) {
                    std::string filename = "brain_checkpoint_" + std::to_string(i) + ".nbl";
                    saveBrain(filename);
                }
            }
        }

        std::cout << "Simulation completed!" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error running brain: " << e.what() << std::endl;
        return false;
    }
}

bool CommandLineParser::saveBrain(const std::string& filename) {
    if (!currentBrain) {
        std::cout << "No brain to save!" << std::endl;
        return false;
    }

    try {
        if (currentBrain->save(filename)) {
            std::cout << "Brain saved to: " << filename << std::endl;
            return true;
        } else {
            std::cout << "Failed to save brain to: " << filename << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "Error saving brain: " << e.what() << std::endl;
        return false;
    }
}

bool CommandLineParser::loadBrain(const std::string& filename) {
    if (!currentBrain) {
        std::cout << "No brain to load into!" << std::endl;
        return false;
    }

    try {
        if (currentBrain->load(filename)) {
            std::cout << "Brain loaded from: " << filename << std::endl;
            return true;
        } else {
            std::cout << "Failed to load brain from: " << filename << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "Error loading brain: " << e.what() << std::endl;
        return false;
    }
}

bool CommandLineParser::resetBrain() {
    if (!currentBrain) {
        std::cout << "No brain to reset!" << std::endl;
        return false;
    }

    try {
        currentBrain->reset();
        currentBrain->initialize();
        std::cout << "Brain reset successfully!" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error resetting brain: " << e.what() << std::endl;
        return false;
    }
}

} // namespace nlm
