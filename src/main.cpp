// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Command-line interface and main program logic
namespace nlm {

    class CommandLineParser {
    public:
        struct Options {
            std::string configFile = "configs/default.cfg";
            std::string outputFile;
            bool verbose = false;
            bool help = false;
            bool benchmark = false;
            std::string benchmarkMode = "all"; // "connectivity", "plasticity", "stdp", "all"
            int simulationSteps = 1000;
            double timestep = 0.001;
            bool runPhase2 = true;
            bool runPhase3 = false;
            bool runPhase4 = false;
            bool runPhase5 = false;
            bool runPhase6 = false;
        };

        static Options parseArgs(int argc, char** argv) {
            Options options;
            
            for (int i = 1; i < argc; ++i) {
                std::string arg(argv[i]);
                
                if (arg == "--help" || arg == "-h") {
                    options.help = true;
                } else if (arg == "--verbose" || arg == "-v") {
                    options.verbose = true;
                } else if (arg == "--benchmark") {
                    options.benchmark = true;
                } else if (arg.find("--benchmark=") == 0) {
                    options.benchmarkMode = arg.substr(12);
                } else if (arg.find("--config") == 0) {
                    if (arg.find('=') != std::string::npos) {
                        options.configFile = arg.substr(arg.find('=') + 1);
                    } else if (i + 1 < argc) {
                        options.configFile = argv[++i];
                    }
                } else if (arg.find("--output") == 0) {
                    if (arg.find('=') != std::string::npos) {
                        options.outputFile = arg.substr(arg.find('=') + 1);
                    } else if (i + 1 < argc) {
                        options.outputFile = argv[++i];
                    }
                } else if (arg.find("--steps=") == 0) {
                    options.simulationSteps = std::stoi(arg.substr(7));
                } else if (arg.find("--timestep=") == 0) {
                    options.timestep = std::stod(arg.substr(10));
                } else if (arg == "--phase3") {
                    options.runPhase3 = true;
                } else if (arg == "--phase4") {
                    options.runPhase4 = true;
                } else if (arg == "--phase5") {
                    options.runPhase5 = true;
                } else if (arg == "--phase6") {
                    options.runPhase6 = true;
                }
            }
            
            return options;
        }
        
        static void printHelp() {
            std::cout << "NLM (Neural Learning Machine) - Neural Simulation Framework" << std::endl;
            std::cout << "\nUsage:" << std::endl;
            std::cout << "  ./nlm [options]" << std::endl;
            std::cout << "\nOptions:" << std::endl;
            std::cout << "  --help, -h                    Show this help message" << std::endl;
            std::cout << "  --verbose, -v                 Enable verbose output" << std::endl;
            std::cout << "  --benchmark                   Run benchmark tests" << std::endl;
            std::cout << "  --benchmark=<mode>            Benchmark mode (connectivity, plasticity, stdp, all)" << std::endl;
            std::cout << "  --config=<file>               Configuration file (default: configs/default.cfg)" << std::endl;
            std::cout << "  --output=<file>               Output file for results" << std::endl;
            std::cout << "  --steps=<n>                   Number of simulation steps" << std::endl;
            std::cout << "  --timestep=<t>                Simulation timestep" << std::endl;
            std::cout << "  --phase3                      Run Phase 3 experiments" << std::endl;
            std::cout << "  --phase4                      Run Phase 4 experiments" << std::endl;
            std::cout << "  --phase5                      Run Phase 5 experiments" << std::endl;
            std::cout << "  --phase6                      Run Phase 6 experiments" << std::endl;
            std::cout << "\nExamples:" << std::endl;
            std::cout << "  ./nlm --benchmark --steps=1000" << std::endl;
            std::cout << "  ./nlm --config=myconfig.cfg --phase6" << std::endl;
            std::cout << "  ./nlm --verbose --benchmark=plasticity" << std::endl;
        }
    };

    // Forward declarations
    class BenchmarkRunner;
    class LearningExperiment;
    class Phase2Tests;

    // Main application class
    class NLMApplication {
    public:
        NLMApplication(const CommandLineParser::Options& options) : options_(options) {}
        
        int run() {
            printBanner();
            
            // Initialize logger
            auto logger = std::make_shared<Logger>();
            auto consoleLogger = std::make_shared<ConsoleLogger>(options_.verbose ? LogLevel::Debug : LogLevel::Info);
            logger->addLogger(consoleLogger);
            Logger::setGlobal(logger);
            
            NLM_LOG_INFO("NLM Phase 2 Application Starting");
            NLM_LOG_INFO("Configuration file: " + options_.configFile);
            
            try {
                // Load configuration
                auto config = loadConfig();
                if (!config) {
                    NLM_LOG_ERROR("Failed to load configuration");
                    return 1;
                }
                
                // Initialize simulation clock
                double timestep = config->getOr<double>("simulation_timestep", options_.timestep);
                SimulationClock clock(timestep);
                NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
                
                // Initialize brain
                NLM_LOG_INFO("Initializing NLM Brain...");
                auto brain = std::make_shared<Brain>(config);
                
                if (!brain->initialize()) {
                    NLM_LOG_ERROR("Failed to initialize brain!");
                    return 1;
                }
                
                brain->logStatus();
                
                // Run experiments based on options
                if (options_.benchmark) {
                    runBenchmarkTests(brain);
                } else {
                    if (options_.runPhase2) {
                        NLM_LOG_INFO("Running Phase 2: Basic Neural Computation Tests");
                        runPhase2Tests(brain);
                    }
                    
                    if (options_.runPhase3) {
                        NLM_LOG_INFO("Running Phase 3: World Interaction Tests");
                        // runPhase3Tests(brain); // TODO: Implement
                    }
                    
                    if (options_.runPhase4) {
                        NLM_LOG_INFO("Running Phase 4: Advanced Learning Tests");
                        // runPhase4Tests(brain); // TODO: Implement
                    }
                    
                    if (options_.runPhase5) {
                        NLM_LOG_INFO("Running Phase 5: Performance Optimization Tests");
                        // runPhase5Tests(brain); // TODO: Implement
                    }
                    
                    if (options_.runPhase6) {
                        NLM_LOG_INFO("Running Phase 6: Integration Tests");
                        // runPhase6Tests(brain); // TODO: Implement
                    }
                }
                
                // Save results if requested
                if (!options_.outputFile.empty()) {
                    saveResults(brain, options_.outputFile);
                }
                
                NLM_LOG_INFO("Application completed successfully");
                return 0;
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Application error: " + std::string(e.what()));
                return 1;
            }
        }
        
    private:
        CommandLineParser::Options options_;
        
        std::shared_ptr<Config> loadConfig() {
            auto config = std::make_shared<Config>();
            
            // Try to load from file if provided
            if (config->loadFromFile(options_.configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " + options_.configFile);
            } else {
                NLM_LOG_INFO("Using default configuration.");
            }
            
            // Override with defaults
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", options_.timestep, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
            
            // STDP parameters
            config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
            config->set("stdp_tau", 20.0f, ConfigSource::Default);
            
            // Structural plasticity parameters
            config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
            config->set("pruning_rate", 0.00001f, ConfigSource::Default);
            
            return config;
        }
        
        void runBenchmarkTests(std::shared_ptr<Brain> brain) {
            NLM_LOG_INFO("=== Running Benchmark Tests ===");
            
            BenchmarkRunner runner(brain);
            
            if (options_.benchmarkMode == "connectivity" || options_.benchmarkMode == "all") {
                NLM_LOG_INFO("Running connectivity benchmark...");
                runner.runConnectivityBenchmark();
            }
            
            if (options_.benchmarkMode == "plasticity" || options_.benchmarkMode == "all") {
                NLM_LOG_INFO("Running plasticity benchmark...");
                runner.runPlasticityBenchmark();
            }
            
            if (options_.benchmarkMode == "stdp" || options_.benchmarkMode == "all") {
                NLM_LOG_INFO("Running STDP benchmark...");
                runner.runStdpBenchmark();
            }
            
            NLM_LOG_INFO("=== Benchmark Tests Complete ===");
        }
        
        void runPhase2Tests(std::shared_ptr<Brain> brain) {
            NLM_LOG_INFO("=== Phase 2: Basic Neural Computation Tests ===");
            
            Phase2Tests tester(brain);
            tester.runAllTests();
            
            NLM_LOG_INFO("=== Phase 2 Tests Complete ===");
        }
        
        void saveResults(std::shared_ptr<Brain> brain, const std::string& filepath) {
            NLM_LOG_INFO("Saving results to: " + filepath);
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open output file: " + filepath);
                return;
            }
            
            file << "NLM Simulation Results" << std::endl;
            file << "=====================" << std::endl;
            file << "Timestamp: " << std::time(nullptr) << std::endl;
            file << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            file << "Neuron count: " << brain->getTotalNeuronCount() << std::endl;
            file << "Synapse count: " << brain->getTotalSynapseCount() << std::endl;
            file << "Firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
            
            file.close();
            NLM_LOG_INFO("Results saved successfully");
        }
    };

} // namespace nlm

int main(int argc, char** argv) {
    auto options = nlm::CommandLineParser::parseArgs(argc, argv);
    
    if (options.help) {
        nlm::CommandLineParser::printHelp();
        return 0;
    }
    
    nlm::NLMApplication app(options);
    return app.run();
}