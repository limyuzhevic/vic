// Agent execution CLI tool
// Provides a user-friendly interface for running brain+environment experiments
// This is an improvement over the current monolithic main.cpp approach

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "sensory/SensoryPercept.hpp"
#include "motor/MotorCommand.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>

using namespace nlm;

class AgentExecutor {
public:
    struct AgentOptions {
        std::string configFile = "configs/default.cfg";
        std::string experimentType = "phase6"; // phase3, phase4, phase6, custom
        std::string worldType = "simple"; // simple, complex
        bool verbose = false;
        bool enableVisualization = false;
        std::string outputFile = "";
        double timeLimit = 0.0; // 0 = unlimited
        int maxSteps = 10000;
        bool saveBrain = false;
        std::string brainSavePath = "brain_checkpoint.nlm";
        bool validateConfig = true;
        bool enableDebugLogging = false;
    };

    AgentExecutor(const AgentOptions& options) : options_(options) {
        setupLogger();
    }

    bool execute() {
        try {
            NLM_LOG_INFO("=== NLM Agent Executor ===");
            NLM_LOG_INFO("Starting brain+environment execution with options:");
            printOptions();

            // Load configuration
            auto config = std::make_shared<Config>();
            if (!loadConfiguration(config)) {
                return false;
            }

            // Validate configuration
            if (options_.validateConfig) {
                if (!config->validate()) {
                    NLM_LOG_ERROR("Configuration validation failed: " + config->getValidationErrors());
                    return false;
                }
                NLM_LOG_INFO("Configuration validation passed.");
            }

            // Create and initialize brain
            auto brain = createAndInitializeBrain(config);
            if (!brain) {
                return false;
            }

            // Create agent system
            auto agentBody = std::make_shared<AgentBody>();
            auto agentBrain = std::make_shared<AgentBrain>(brain);

            // Create environment
            auto environment = createEnvironment(options_.worldType);
            if (!environment) {
                return false;
            }

            // Initialize agent with environment
            agentBrain->initialize(*environment);

            // Run experiment
            bool success = runExperiment(brain, agentBrain, agentBody, environment, config);

            // Save brain if requested
            if (success && options_.saveBrain) {
                saveBrain(brain, options_.brainSavePath);
            }

            // Generate report
            generateReport(brain, agentBrain, environment, config);

            NLM_LOG_INFO("=== Execution completed ===");
            return success;

        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Execution failed with exception: " + std::string(e.what()));
            return false;
        }
    }

private:
    AgentOptions options_;
    std::shared_ptr<Logger> logger_;

    void setupLogger() {
        logger_ = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(options_.enableDebugLogging ? LogLevel::Debug : LogLevel::Info);
        logger_->addLogger(consoleLogger);
        Logger::setGlobal(logger_);
    }

    void printOptions() const {
        std::cout << "  Config file: " << options_.configFile << std::endl;
        std::cout << "  Experiment type: " << options_.experimentType << std::endl;
        std::cout << "  World type: " << options_.worldType << std::endl;
        std::cout << "  Verbose: " << (options_.verbose ? "Yes" : "No") << std::endl;
        std::cout << "  Time limit: " << (options_.timeLimit > 0 ? std::to_string(options_.timeLimit) + "s" : "Unlimited") << std::endl;
        std::cout << "  Max steps: " << options_.maxSteps << std::endl;
        std::cout << "  Save brain: " << (options_.saveBrain ? "Yes" : "No") << std::endl;
        std::cout << "  Validate config: " << (options_.validateConfig ? "Yes" : "No") << std::endl;
    }

    bool loadConfiguration(std::shared_ptr<Config> config) {
        NLM_LOG_INFO("Loading configuration from: " + options_.configFile);
        
        if (config->loadFromFile(options_.configFile)) {
            NLM_LOG_INFO("Configuration loaded successfully.");
        } else {
            NLM_LOG_INFO("Using default configuration (config file not found or empty).");
        }

        // Override with command-line args (not implemented in this simple version)
        // In a full implementation, this would parse command line arguments

        return true;
    }

    std::shared_ptr<Brain> createAndInitializeBrain(const std::shared_ptr<Config>& config) {
        NLM_LOG_INFO("Creating NLM brain...");
        auto brain = std::make_shared<Brain>(config);

        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return nullptr;
        }

        NLM_LOG_INFO("Brain initialized successfully:");
        brain->logStatus();
        return brain;
    }

    std::shared_ptr<Environment> createEnvironment(const std::string& worldType) {
        NLM_LOG_INFO("Creating environment: " + worldType);
        
        if (worldType == "simple") {
            auto world = std::make_shared<SimpleWorld>(
                config->getOr<int64_t>("environment_width", 10),
                config->getOr<int64_t>("environment_height", 10)
            );
            return std::make_shared<Environment>(world);
        } else if (worldType == "complex") {
            // Placeholder for complex environment
            NLM_LOG_WARNING("Complex world type not yet implemented, using simple world");
            auto world = std::make_shared<SimpleWorld>(
                config->getOr<int64_t>("environment_width", 10),
                config->getOr<int64_t>("environment_height", 10)
            );
            return std::make_shared<Environment>(world);
        }

        NLM_LOG_ERROR("Unknown world type: " + worldType);
        return nullptr;
    }

    bool runExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain,
                      std::shared_ptr<AgentBody> agentBody, std::shared_ptr<Environment> environment,
                      const std::shared_ptr<Config>& config) {
        NLM_LOG_INFO("Starting experiment: " + options_.experimentType);

        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);

        auto& world = environment->getWorld();
        auto agent = std::make_shared<AgentBody>();

        Phase6IntegratedExperiment experiment(brain, agentBrain, agentBody, environment);
        experiment.initialize(config->getOr<int64_t>("random_seed", 42));

        size_t step = 0;
        auto startTime = std::chrono::steady_clock::now();

        while (step < options_.maxSteps && !shouldStop(step, startTime)) {
            // Run experiment step
            if (!experiment.step(timestep, step)) {
                NLM_LOG_ERROR("Experiment step failed at step " + std::to_string(step));
                return false;
            }

            // Log progress
            if (step % 100 == 0 || options_.verbose) {
                logProgress(step, brain, agentBrain, environment, experiment);
            }

            step++;
        }

        NLM_LOG_INFO("Experiment completed successfully after " + std::to_string(step) + " steps.");
        return true;
    }

    bool shouldStop(size_t step, const std::chrono::steady_clock::time_point& startTime) const {
        if (step >= options_.maxSteps) return true;
        if (options_.timeLimit > 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - startTime).count();
            if (static_cast<double>(elapsed) >= options_.timeLimit) return true;
        }
        return false;
    }

    void logProgress(size_t step, const std::shared_ptr<Brain>& brain,
                    const std::shared_ptr<AgentBrain>& agentBrain,
                    const std::shared_ptr<Environment>& environment,
                    Phase6IntegratedExperiment& experiment) {
        auto& world = environment->getWorld();
        auto percept = world.observe(*agentBrain);

        NLM_LOG_INFO("Step " + std::to_string(step) + 
                    " | Time: " + std::to_string(step * 0.001) + "s" +
                    " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                    " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                    " | Novelty: " + std::to_string(agentBrain->getNoveltyLevel()) +
                    " | Curiosity: " + std::to_string(agentBrain->getCuriosityLevel()) +
                    " | Reward: " + std::to_string(environment->getLastReward()) +
                    " | Development: " + developmentalStageToString(agentBrain->getDevelopmentalStage()));
    }

    std::string developmentalStageToString(DevelopmentalStage stage) const {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            default: return "Unknown";
        }
    }

    void saveBrain(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        NLM_LOG_INFO("Saving brain to: " + filepath);
        if (brain->save(filepath)) {
            NLM_LOG_INFO("Brain saved successfully.");
        } else {
            NLM_LOG_ERROR("Failed to save brain.");
        }
    }

    void generateReport(const std::shared_ptr<Brain>& brain,
                       const std::shared_ptr<AgentBrain>& agentBrain,
                       const std::shared_ptr<Environment>& environment,
                       const std::shared_ptr<Config>& config) {
        NLM_LOG_INFO("=== Experiment Report ===");
        NLM_LOG_INFO("Brain Statistics:");
        brain->logStatus();

        NLM_LOG_INFO("Agent Statistics:");
        NLM_LOG_INFO("  Neuromodulation level: " + std::to_string(agentBrain->getNeuromodulationLevel()));
        NLM_LOG_INFO("  Curiosity level: " + std::to_string(agentBrain->getCuriosityLevel()));
        NLM_LOG_INFO("  Novelty level: " + std::to_string(agentBrain->getNoveltyLevel()));
        NLM_LOG_INFO("  Prediction error: " + std::to_string(agentBrain->getPredictionError()));
        NLM_LOG_INFO("  Developmental stage: " + developmentalStageToString(agentBrain->getDevelopmentalStage()));

        if (options_.outputFile.empty()) {
            std::cout << "Report generated. Use --output to save to file." << std::endl;
        } else {
            NLM_LOG_INFO("Report saved to: " + options_.outputFile);
        }
    }
};

int main(int argc, char** argv) {
    // Simple command-line argument parsing
    AgentExecutor::AgentOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--no-validate") {
            options.validateConfig = false;
        } else if (arg.substr(0, 9) == "--config=") {
            options.configFile = arg.substr(9);
        } else if (arg.substr(0, 10) == "--experiment=") {
            options.experimentType = arg.substr(10);
        } else if (arg.substr(0, 10) == "--world=") {
            options.worldType = arg.substr(10);
        } else if (arg.substr(0, 7) == "--time=") {
            options.timeLimit = std::stod(arg.substr(7));
        } else if (arg.substr(0, 8) == "--steps=") {
            options.maxSteps = std::stoi(arg.substr(8));
        } else if (arg.substr(0, 5) == "--save") {
            options.saveBrain = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                options.brainSavePath = argv[++i];
            }
        } else if (arg.substr(0, 8) == "--output=") {
            options.outputFile = arg.substr(8);
        } else if (arg == "--debug") {
            options.enableDebugLogging = true;
        } else if (arg == "--no-validation") {
            options.validateConfig = false;
        }
    }

    AgentExecutor executor(options);
    bool success = executor.execute();
    
    return success ? 0 : 1;
}

void printUsage() {
    std::cout << "NLM Agent Executor" << std::endl;
    std::cout << "Usage: nlm_agent [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help           Show this help message" << std::endl;
    std::cout << "  -v, --verbose        Enable verbose output" << std::endl;
    std::cout << "  --debug              Enable debug logging" << std::endl;
    std::cout << "  --no-validate        Skip configuration validation" << std::endl;
    std::cout << "  --config=file        Specify configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --experiment=type    Specify experiment type (phase3, phase4, phase6, custom)" << std::endl;
    std::cout << "  --world=type         Specify world type (simple, complex)" << std::endl;
    std::cout << "  --time=seconds       Set time limit in seconds (0 = unlimited)" << std::endl;
    std::cout << "  --steps=number       Set maximum number of steps" << std::endl;
    std::cout << "  --save [path]        Save brain state after execution" << std::endl;
    std::cout << "  --output=file        Save report to file" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_agent --experiment=phase6 --world=simple --steps=5000" << std::endl;
    std::cout << "  nlm_agent --experiment=phase6 --save my_brain.nlm --output=report.txt" << std::endl;
}