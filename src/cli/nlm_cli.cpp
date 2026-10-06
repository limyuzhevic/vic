// Command line interface for NLM (Neural Learning Machine)
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>

// Include NLM headers
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/Vision.hpp"
#include "motor/Action.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "memory/WorkingMemory.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "cognition/NeuralPlanner.hpp"

using namespace nlm;

class NLMCommandLineInterface {
public:
    NLMCommandLineInterface() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                 NLM Command Line Interface                 ║" << std::endl;
        std::cout << "║                 Neural Learning Machine                     ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    void print_help() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      NLM CLI Help                          ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ Usage: nlm [command] [options]                            ║" << std::endl;
        std::cout << "║                                                             ║" << std::endl;
        std::cout << "║ Commands:                                                   ║" << std::endl;
        std::cout << "║   run      Run a simulation with configuration file      ║" << std::endl;
        std::cout << "║   create   Create and save a new configuration             ║" << std::endl;
        std::cout << "║   analyze  Analyze existing configuration                ║" << std::endl;
        std::cout << "║   benchmark Run performance benchmark                    ║" << std::endl;
        std::cout << "║   validate Validate configuration file                   ║" << std::endl;
        std::cout << "║   help     Show this help message                        ║" << std::endl;
        std::cout << "║                                                             ║" << std::endl;
        std::cout << "║ Examples:                                                  ║" << std::endl;
        std::cout << "║   nlm run config.txt --steps 1000 --brain-size 1000     ║" << std::endl;
        std::cout << "║   nlm create my_config.cfg --neurons 500              ║" << std::endl;
        std::cout << "║   nlm analyze config.txt                                 ║" << std::endl;
        std::cout << "║                                                             ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    }
    
    bool run_simulation(const std::string& config_file, int steps, int brain_size) {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                   Running NLM Simulation                    ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        
        try {
            // Load configuration
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(config_file)) {
                std::cout << "║ ERROR: Failed to load configuration file: " << config_file << "               ║" << std::endl;
                return false;
            }
            
            // Override configuration with command line options
            config->set("neuron_count", static_cast<int64_t>(brain_size), ConfigSource::CommandLine);
            config->set("steps", static_cast<int64_t>(steps), ConfigSource::CommandLine);
            
            std::cout << "║ Configuration loaded successfully" << std::endl;
            std::cout << "║ Neurons: " << config->getOr<int64_t>("neuron_count", 1000) << "                    ║" << std::endl;
            std::cout << "║ Steps: " << config->getOr<int64_t>("steps", 1000) << "                     ║" << std::endl;
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            std::cout << "║ Brain initialized" << std::endl;
            std::cout << "║ Total neurons: " << brain->getTotalNeuronCount() << "                   ║" << std::endl;
            std::cout << "║ Total synapses: " << brain->getTotalSynapseCount() << "                  ║" << std::endl;
            
            // Run simulation
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (int step = 0; step < steps; ++step) {
                // Simple simulation step with random input
                if (step % 100 == 0) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                
                brain->step(step, step * 0.001f);
                
                if (step % 100 == 0) {
                    std::cout << "║ Step: " << step << "/" << steps << " ("
                              << (static_cast<double>(step) / steps * 100.0) << "%)       ║" << std::endl;
                }
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
            
            std::cout << "║ Simulation completed successfully" << std::endl;
            std::cout << "║ Total time: " << duration.count() << " seconds" << std::endl;
            std::cout << "║ Final spikes: " << brain->getTotalSpikeCount() << "              ║" << std::endl;
            std::cout << "║ Average firing rate: " << brain->getAverageFiringRate() << " Hz     ║" << std::endl;
            
            // Save brain state
            std::string output_file = "simulation_output_" + std::to_string(steps) + "steps_" + 
                                     std::to_string(brain->getTotalSpikeCount()) + "spikes.bin";
            brain->save(output_file);
            
            std::cout << "║ Brain state saved to: " << output_file << "             ║" << std::endl;
            
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << std::endl;
            std::cout << "Simulation completed successfully!" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "║ ERROR: " << e.what() << std::endl;
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            return false;
        }
    }
    
    bool create_config(const std::string& filename, int neurons, int regions, double connection_prob) {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                 Creating New Configuration                    ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        
        try {
            auto config = std::make_shared<Config>();
            
            // Set basic configuration
            config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::User);
            config->set("region_count", static_cast<int64_t>(regions), ConfigSource::User);
            config->set("connection_probability", connection_prob, ConfigSource::User);
            
            // Set advanced configurations
            config->set("plasticity.stdp.enable", true, ConfigSource::User);
            config->set("plasticity.hebbian.enable", true, ConfigSource::User);
            config->set("plasticity.structural.enable", true, ConfigSource::User);
            
            config->set("neuromod.dopamine.scale", 1.0, ConfigSource::User);
            config->set("neuromod.curiosity.enable", true, ConfigSource::User);
            config->set("neuromod.novelty.enable", true, ConfigSource::User);
            
            // Set development parameters
            config->set("development.critical_period", 0.2, ConfigSource::User);
            config->set("development.maturation_age", 10.0, ConfigSource::User);
            
            // Set memory parameters
            config->set("memory.working.capacity", 1000, ConfigSource::User);
            config->set("memory.episodic.capacity", 100, ConfigSource::User);
            
            // Set prediction parameters
            config->set("prediction.horizon", 10, ConfigSource::User);
            config->set("prediction.error_threshold", 0.1, ConfigSource::User);
            
            // Save configuration
            if (!config->saveToFile(filename)) {
                std::cout << "║ ERROR: Failed to save configuration to " << filename << "          ║" << std::endl;
                return false;
            }
            
            std::cout << "║ Configuration created successfully" << std::endl;
            std::cout << "║ File: " << filename << std::endl;
            std::cout << "║ Neurons: " << neurons << "                       ║" << std::endl;
            std::cout << "║ Regions: " << regions << "                         ║" << std::endl;
            std::cout << "║ Connection probability: " << connection_prob << "           ║" << std::endl;
            
            // Show analysis
            std::string analysis = config->analyze_config();
            std::cout << "║ Configuration quality: " << config->get_quality_score() << "/100" << std::endl;
            
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << std::endl;
            std::cout << "Configuration created successfully!" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "║ ERROR: " << e.what() << std::endl;
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            return false;
        }
    }
    
    bool analyze_config(const std::string& filename) {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                Analyzing Configuration File                    ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        
        try {
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(filename)) {
                std::cout << "║ ERROR: Failed to load configuration file: " << filename << "               ║" << std::endl;
                return false;
            }
            
            // Perform analysis
            std::string analysis = config->analyze_config();
            
            std::cout << "║ ANALYSIS RESULTS:" << std::endl;
            std::cout << "║ " << analysis << "                                        ║" << std::endl;
            
            // Generate recommendations
            std::string recommendations = "RECOMMENDATIONS:\n";
            int quality = config->get_quality_score();
            
            if (quality < 50) {
                recommendations += "  • Configuration quality is below optimal (" + std::to_string(quality) + "/100)\n";
                recommendations += "  • Consider increasing neuron count or connection probability\n";
                recommendations += "  • Add more structured plasticity rules\n";
            } else if (quality < 80) {
                recommendations += "  • Configuration quality is moderate (" + std::to_string(quality) + "/100)\n";
                recommendations += "  • Consider tuning neuromodulation parameters\n";
            } else {
                recommendations += "  • Excellent configuration quality!\n";
                recommendations += "  • Consider increasing complexity for more advanced tasks\n";
            }
            
            // Check for missing required keys
            auto missing_keys = config->validate_required_keys();
            if (!missing_keys.empty()) {
                recommendations += "  • Missing recommended keys: ";
                for (size_t i = 0; i < missing_keys.size(); ++i) {
                    if (i > 0) recommendations += ", ";
                    recommendations += missing_keys[i];
                }
                recommendations += "\n";
            }
            
            std::cout << "║ RECOMMENDATIONS:" << std::endl;
            std::cout << "║ " << recommendations << "                                       ║" << std::endl;
            
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << std::endl;
            std::cout << "Analysis completed!" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "║ ERROR: " << e.what() << std::endl;
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            return false;
        }
    }
    
    bool run_benchmark(int neurons, int steps, int iterations) {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                Running Performance Benchmark                  ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        
        try {
            double total_time = 0.0;
            int total_spikes = 0;
            int total_neurons = 0;
            
            for (int i = 0; i < iterations; ++i) {
                auto config = std::make_shared<Config>();
                config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::User);
                config->set("region_count", static_cast<int64_t>(1), ConfigSource::User);
                config->set("connection_probability", 0.1f, ConfigSource::User);
                
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                auto start_time = std::chrono::high_resolution_clock::now();
                
                for (int step = 0; step < steps; ++step) {
                    if (step % 50 == 0) {
                        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
                    }
                    brain->step(step, step * 0.001f);
                }
                
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
                
                total_time += duration.count() / 1000000.0; // Convert to seconds
                total_spikes += brain->getTotalSpikeCount();
                total_neurons += brain->getTotalNeuronCount();
                
                if ((i + 1) % 5 == 0) {
                    std::cout << "║ Iteration " << (i + 1) << "/" << iterations << ": "
                              << (total_time / (i + 1)) << " seconds per run     ║" << std::endl;
                }
            }
            
            double avg_time = total_time / iterations;
            double avg_spikes = static_cast<double>(total_spikes) / iterations;
            double avg_neurons = static_cast<double>(total_neurons) / iterations;
            
            std::cout << "║ Benchmark Results:" << std::endl;
            std::cout << "║  Average time per run: " << avg_time << " seconds" << std::endl;
            std::cout << "║  Average spikes per run: " << avg_spikes << "             " << std::endl;
            std::cout << "║  Average neurons per run: " << avg_neurons << "             " << std::endl;
            std::cout << "║  Steps per second: " << (steps * iterations) / total_time << "       " << std::endl;
            std::cout << "║  Spikes per second: " << avg_spikes / avg_time << "            " << std::endl;
            
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << std::endl;
            std::cout << "Benchmark completed!" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "║ ERROR: " << e.what() << std::endl;
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            return false;
        }
    }
    
    bool validate_config(const std::string& filename) {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                Validating Configuration File                    ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        
        try {
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(filename)) {
                std::cout << "║ ERROR: Failed to load configuration file" << std::endl;
                std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
                return false;
            }
            
            int quality = config->get_quality_score();
            auto missing_keys = config->validate_required_keys();
            
            std::cout << "║ Validation Results:" << std::endl;
            std::cout << "║  Configuration quality: " << quality << "/100" << std::endl;
            std::cout << "║  Missing required keys: " << missing_keys.size() << std::endl;
            std::cout << "║  Total configuration entries: " << config->getKeys().size() << std::endl;
            
            if (quality >= 80) {
                std::cout << "║  STATUS: VALID (High quality)" << std::endl;
            } else if (quality >= 60) {
                std::cout << "║  STATUS: VALID (Acceptable quality)" << std::endl;
            } else {
                std::cout << "║  STATUS: REQUIRES IMPROVEMENT" << std::endl;
            }
            
            if (!missing_keys.empty()) {
                std::cout << "║  Missing keys:" << std::endl;
                for (const auto& key : missing_keys) {
                    std::cout << "║    - " << key << std::endl;
                }
            }
            
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << std::endl;
            std::cout << "Configuration " << (quality >= 80 ? "VALID" : "NEEDS IMPROVEMENT") 
                      << "!" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "║ ERROR: " << e.what() << std::endl;
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            return false;
        }
    }
    
    void print_version() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                 NLM Command Line Interface                    ║" << std::endl;
        std::cout << "║                      Version 1.0.0                          ║" << std::endl;
        std::cout << "║                                                             ║" << std::endl;
        std::cout << "║  Built with C++20, using NLM neural simulation framework   ║" << std::endl;
        std::cout << "║  Features: Configuration management, simulation, analysis   ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    }
};

// Command line argument parsing
struct CommandLineArgs {
    std::string command;
    std::string config_file;
    std::string output_file;
    int neurons = 1000;
    int steps = 1000;
    int regions = 1;
    int iterations = 1;
    double connection_prob = 0.1;
    bool help = false;
    bool version = false;
    bool validate_only = false;
    bool benchmark_mode = false;
};

CommandLineArgs parse_arguments(int argc, char** argv) {
    CommandLineArgs args;
    
    if (argc < 2) {
        args.help = true;
        return args;
    }
    
    args.command = argv[1];
    
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg == "--validate") {
            args.validate_only = true;
        } else if (arg == "--benchmark") {
            args.benchmark_mode = true;
        } else if (arg == "--steps" && i + 1 < argc) {
            args.steps = std::stoi(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            args.neurons = std::stoi(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            args.regions = std::stoi(argv[++i]);
        } else if (arg == "--iterations" && i + 1 < argc) {
            args.iterations = std::stoi(argv[++i]);
        } else if (arg == "--connection-prob" && i + 1 < argc) {
            args.connection_prob = std::stod(argv[++i]);
        } else if (arg == "--config" && i + 1 < argc) {
            args.config_file = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            args.output_file = argv[++i];
        } else if (arg.find('--') == 0) {
            // Handle --key value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                // Could store in a map for config overrides
            }
        } else {
            // Positional argument
            if (args.config_file.empty()) {
                args.config_file = arg;
            }
        }
    }
    
    return args;
}

int main(int argc, char** argv) {
    try {
        NLMCommandLineInterface cli;
        
        CommandLineArgs args = parse_arguments(argc, argv);
        
        if (args.help) {
            cli.print_help();
            return 0;
        }
        
        if (args.version) {
            cli.print_version();
            return 0;
        }
        
        bool success = false;
        
        if (args.command == "run") {
            if (args.config_file.empty()) {
                std::cout << "ERROR: Configuration file required for 'run' command" << std::endl;
                cli.print_help();
                return 1;
            }
            success = cli.run_simulation(args.config_file, args.steps, args.neurons);
        } else if (args.command == "create") {
            std::string filename = args.config_file.empty() ? "config.cfg" : args.config_file;
            success = cli.create_config(filename, args.neurons, args.regions, args.connection_prob);
        } else if (args.command == "analyze") {
            if (args.config_file.empty()) {
                std::cout << "ERROR: Configuration file required for 'analyze' command" << std::endl;
                cli.print_help();
                return 1;
            }
            success = cli.analyze_config(args.config_file);
        } else if (args.command == "validate") {
            if (args.config_file.empty()) {
                std::cout << "ERROR: Configuration file required for 'validate' command" << std::endl;
                cli.print_help();
                return 1;
            }
            success = cli.validate_config(args.config_file);
        } else if (args.command == "benchmark") {
            success = cli.run_benchmark(args.neurons, args.steps, args.iterations);
        } else {
            std::cout << "ERROR: Unknown command: " << args.command << std::endl;
            cli.print_help();
            return 1;
        }
        
        return success ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
