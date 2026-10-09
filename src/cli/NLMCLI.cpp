// NLM CLI - Enhanced Interface
// Provides powerful command-line interface for NLM experiments

#include "cli/NLMCLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <map>
#include <algorithm>
#include <cctype>

namespace NLMCLI {

    namespace {
        std::shared_ptr<Logger> globalLogger;
        std::ofstream logStream;
        bool loggingInitialized = false;
    }

    void initialize() {
        globalLogger = std::make_shared<Logger>();
        logStream.open("nlm_cli.log", std::ios::out | std::ios::trunc);
        if (!logStream.is_open()) {
            std::cerr << "WARNING: Could not open log file" << std::endl;
        }
        loggingInitialized = true;
    }

    void cleanup() {
        if (logStream.is_open()) {
            logStream.close();
        }
        loggingInitialized = false;
    }

    CLIArgs parseArguments(int argc, char* argv[]) {
        CLIArgs args;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "--demo" || arg == "-d") {
                args.mode = Mode::DEMO;
            } else if (arg == "--experiment" || arg == "-e") {
                args.mode = Mode::EXPERIMENT;
                if (i + 1 < argc) args.experimentName = argv[++i];
            } else if (arg == "--batch" || arg == "-b") {
                args.mode = Mode::BATCH;
                if (i + 1 < argc) args.batchFile = argv[++i];
            } else if (arg == "--checkpoint" || arg == "-c") {
                args.mode = Mode::CHECKPOINT;
                if (i + 1 < argc) {
                    args.checkpointCommand = argv[++i];
                    if (i + 1 < argc) args.checkpointPath = argv[++i];
                }
            } else if (arg == "--visualize" || arg == "-v") {
                args.mode = Mode::VISUALIZE;
                if (i + 1 < argc) args.vizType = argv[++i];
            } else if (arg == "--test" || arg == "-t") {
                args.mode = Mode::TEST;
                if (i + 1 < argc) args.testSuite = argv[++i];
            } else if (arg == "--config" || arg == "-f") {
                if (i + 1 < argc) args.configFile = argv[++i];
            } else if (arg == "--help" || arg == "-h") {
                args.mode = Mode::HELP;
            } else if (arg == "--version") {
                std::cout << "NLM v0.1.0" << std::endl;
                exit(0);
            } else if (arg == "--verbose" || arg == "-V") {
                args.verbosity++;
            } else if (arg == "--quiet" || arg == "-q") {
                args.quiet = true;
            } else if (arg == "--log") {
                if (i + 1 < argc) args.logFile = argv[++i];
            } else if (arg == "--auto-save") {
                args.autoSaveCheckpoint = true;
            } else if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    args.extraArgs[arg.substr(2, pos-2)] = arg.substr(pos + 1);
                } else {
                    args.extraArgs[arg.substr(2)] = "";
                }
            } else {
                args.experimentName = arg;
            }
        }

        if (args.mode == Mode::NONE) {
            args.mode = Mode::DEMO;
        }

        return args;
    }

    std::shared_ptr<Config> loadConfig(const std::string& configFile) {
        auto config = std::make_shared<Config>();

        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_WARNING("Using default configuration (file not found: " + configFile + ")");
        }

        return config;
    }

    void applyOverrides(std::shared_ptr<Config>& config, const CLIArgs& args) {
        for (const auto& override : args.extraArgs) {
            std::string key = override.first;
            std::string value = override.second;

            try {
                if (value.find('.') != std::string::npos) {
                    float floatValue = std::stof(value);
                    config->set(key, floatValue, ConfigSource::CommandLine);
                } else {
                    int intValue = std::stoi(value);
                    config->set(key, static_cast<int64_t>(intValue), ConfigSource::CommandLine);
                }
            } catch (...) {
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }

    void setupLogging(int verbosity, const std::string& logFile, bool quiet) {
        LogLevel level = LogLevel::Info;
        if (verbosity >= 3) level = LogLevel::Debug;
        else if (verbosity == 1) level = LogLevel::Info;
        else if (verbosity == 2) level = LogLevel::Warning;

        auto consoleLogger = std::make_shared<ConsoleLogger>(level);
        globalLogger->addLogger(consoleLogger);

        if (!logFile.empty()) {
            auto fileLogger = std::make_shared<FileLogger>(logFile);
            globalLogger->addLogger(fileLogger);
        }

        Logger::setGlobal(globalLogger);

        NLM_LOG_INFO(std::string("Logging level: ") + std::to_string(static_cast<int>(level)));
        if (!logFile.empty()) {
            NLM_LOG_INFO("Log file: " + logFile);
        }
    }

    void runBuiltInDemo(const std::shared_ptr<Config>& config) {
        NLM_LOG_INFO("Running built-in demo...");

        Phase6IntegratedExperiment experiment;

        Phase6Config demoConfig;
        demoConfig.neuronCount = config->getOr<size_t>("neuron_count", 500);
        demoConfig.maxSteps = config->getOr<size_t>("max_steps", 2000);
        demoConfig.enableCheckpointing = config->getOr<bool>("enable_checkpointing", true);
        demoConfig.enableReplay = config->getOr<bool>("enable_replay", true);
        demoConfig.enableDevelopment = config->getOr<bool>("enable_development", true);

        auto result = experiment.run(demoConfig);

        NLM_LOG_INFO("Demo completed successfully");
        NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
        NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
    }

    void runNamedExperiment(const std::shared_ptr<Config>& config, const std::string& experimentName) {
        NLM_LOG_INFO("Running experiment: " + experimentName);

        std::map<std::string, Phase6Config> experiments;

        Phase6Config cognitiveExp;
        cognitiveExp.neuronCount = 1000;
        cognitiveExp.maxSteps = 5000;
        cognitiveExp.enableCheckpointing = true;
        cognitiveExp.enableReplay = false;
        cognitiveExp.enableDevelopment = true;
        experiments["cognitive"] = cognitiveExp;

        Phase6Config socialExp;
        socialExp.neuronCount = 800;
        socialExp.maxSteps = 3000;
        socialExp.enableCheckpointing = true;
        socialExp.enableReplay = true;
        socialExp.enableDevelopment = true;
        experiments["social"] = socialExp;

        Phase6Config developmentalExp;
        developmentalExp.neuronCount = 600;
        developmentalExp.maxSteps = 4000;
        developmentalExp.enableCheckpointing = true;
        developmentalExp.enableReplay = true;
        developmentalExp.enableDevelopment = true;
        experiments["developmental"] = developmentalExp;

        if (experiments.find(experimentName) != experiments.end()) {
            Phase6IntegratedExperiment experiment;
            auto result = experiment.run(experiments[experimentName]);

            NLM_LOG_INFO(std::string("Experiment ") + experimentName + " completed");
            NLM_LOG_INFO("Result: Reward=" + std::to_string(result.totalReward) +
                         ", Firing=" + std::to_string(result.avgFiringRate) +
                         ", Episodes=" + std::to_string(result.memoryEpisodesStored));
        } else {
            NLM_LOG_ERROR(std::string("Unknown experiment: ") + experimentName);
            NLM_LOG_INFO("Available experiments: cognitive, social, developmental");
            throw std::runtime_error("Unknown experiment requested");
        }
    }

    void printHelp() {
        std::cout << "NLM — 熙然 (Phase 6: Integrated Artificial Brain)" << std::endl;
        std::cout << std::endl;
        std::cout << "USAGE:" << std::endl;
        std::cout << "  nlm [OPTIONS] [EXPERIMENT]" << std::endl;
        std::cout << std::endl;
        std::cout << "MODES:" << std::endl;
        std::cout << "  --demo, -d          Run built-in demo (default)" << std::endl;
        std::cout << "  --experiment, -e NAME  Run named experiment (cognitive, social, developmental)" << std::endl;
        std::cout << "  --batch, -b FILE     Run batch of experiments from file" << std::endl;
        std::cout << "  --checkpoint, -c CMD [PATH]  Checkpoint management" << std::endl;
        std::cout << "  --visualize, -v TYPE  Run visualization mode" << std::endl;
        std::cout << "  --test, -t SUITE     Run system tests" << std::endl;
        std::cout << "  --config, -f FILE     Configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --help, -h            Show this help" << std::endl;
        std::cout << "  --version              Show version info" << std::endl;
        std::cout << std::endl;
        std::cout << "ADVANCED OPTIONS:" << std::endl;
        std::cout << "  --verbose, -V          Increase verbosity" << std::endl;
        std::cout << "  --quiet, -q            Quiet mode (minimal output)" << std::endl;
        std::cout << "  --log FILE             Log to file" << std::endl;
        std::cout << "  --auto-save            Automatically save final checkpoint" << std::endl;
        std::cout << "  --param KEY=VALUE      Set configuration parameter" << std::endl;
        std::cout << std::endl;
        std::cout << "EXPERIMENT NAMES:" << std::endl;
        std::cout << "  cognitive     - Complex cognitive tasks and learning" << std::endl;
        std::cout << "  social        - Social learning and interaction" << std::endl;
        std::cout << "  developmental  - Long-term development and maturation" << std::endl;
        std::cout << std::endl;
        std::cout << "CHECKPOINT COMMANDS:" << std::endl;
        std::cout << "  save PATH      Save current brain state" << std::endl;
        std::cout << "  load PATH      Load brain state" << std::endl;
        std::cout << "  list           List available checkpoints" << std::endl;
        std::cout << std::endl;
        std::cout << "VISUALIZATION TYPES:" << std::endl;
        std::cout << "  neural         - Neural activity visualization" << std::endl;
        std::cout << "  memory         - Memory system visualization" << std::endl;
        std::cout << "  development    - Developmental stages visualization" << std::endl;
        std::cout << "  complete       - All systems visualization" << std::endl;
        std::cout << std::endl;
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  nlm --demo                              Run quick demo" << std::endl;
        std::cout << "  nlm --experiment cognitive --verbose     Run cognitive experiment with debug output" << std::endl;
        std::cout << "  nlm --checkpoint save ./mybrain.bin        Save brain state" << std::endl;
        std::cout << "  nlm --visualize neural --auto-save       Run visualization and auto-save" << std::endl;
        std::cout << std::endl;
        std::cout << "For more information, visit: https://github.com/yourrepo/nlm" << std::endl;
    }

    void saveCheckpoint(const std::shared_ptr<Config>& config, const std::string& path) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for checkpointing");
        }

        for (int i = 0; i < 100; ++i) {
            brain->step(i, i * 0.001);
        }

        if (brain->save(path)) {
            NLM_LOG_INFO("Checkpoint saved successfully to: " + path);
        } else {
            throw std::runtime_error("Failed to save checkpoint");
        }
    }

    std::string getVersionInfo() {
        return "NLM v0.1.0 (Phase 6)";
    }

    std::vector<std::string> listAvailableExperiments() {
        return {"cognitive", "social", "developmental", "all"};
    }

} // namespace NLMCLI
