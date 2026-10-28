#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <argparse/argparse.hpp>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

using namespace nlm;
using namespace std;

class NLMCommandLineInterface {
public:
    NLMCommandLineInterface() {
        // Initialize logger
        auto logger = make_shared<Logger>();
        auto consoleLogger = make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    void run(int argc, char** argv) {
        // Create argument parser
        argparse::ArgumentParser parser(
            "NLM Command Line Interface",
            "Version 0.1.0 - Advanced operations for NLM Phase 6",
            "MIT License"
        );
        
        // Global options
        parser.add_argument("--verbose", "-v")
            .default_value(false)
            .help("Enable verbose output");
            
        parser.add_argument("--quiet", "-q")
            .default_value(false)
            .help("Suppress non-error output");
            
        parser.add_argument("--config", "-c")
            .default_value("configs/default.cfg")
            .help("Configuration file path");
            
        parser.add_argument("--output", "-o")
            .default_value("")
            .help("Output file for results");
            
        parser.add_argument("--log-level")
            .choices({"debug", "info", "warning", "error", "critical"})
            .default_value("info")
            .help("Log level");
        
        // Create subcommands
        auto& run_cmd = parser.add_subcommand("run", "Run simulation")
            .positional("<steps>", "Number of simulation steps")
            .named("--neurons", "Number of neurons", 1000)
            .named("--regions", "Number of regions", 1)
            .named("--output-file", "Output file", "")
            .named("--enable-plasticity", "Enable plasticity", true)
            .named("--enable-neuromodulation", "Enable neuromodulation", true)
            .named("--enable-development", "Enable development", true)
            .named("--enable-curiosity", "Enable curiosity", true)
            .named("--reward-modulation", "Enable reward modulation", true);
        
        auto& batch_cmd = parser.add_subcommand("batch", "Run multiple simulations")
            .positional("<experiments>", "Number of experiments in batch")
            .positional("<steps>", "Steps per experiment")
            .named("--config-dir", "Configuration directory", "configs")
            .named("--results-dir", "Results directory", "results")
            .named("--parallel", "Run experiments in parallel", false)
            .named("--max-workers", "Maximum parallel workers", 4);
        
        auto& save_cmd = parser.add_subcommand("save", "Save brain state")
            .positional("<filepath>", "File path to save brain state")
            .named("--brain", "Brain instance", 0);
        
        auto& load_cmd = parser.add_subcommand("load", "Load brain state")
            .positional("<filepath>", "File path to load brain state")
            .named("--brain", "Brain instance", 0);
        
        auto& benchmark_cmd = parser.add_subcommand("benchmark", "Run performance benchmarks")
            .named("--large", "Run large brain benchmark", false)
            .named("--simulation", "Run simulation benchmark", false)
            .named("--memory", "Run memory benchmark", false)
            .named("--duration", "Benchmark duration in seconds", 60);
        
        auto& explore_cmd = parser.add_subcommand("explore", "Explore brain configurations")
            .named("--search-space", "Hyperparameter search space", "default")
            .named("--iterations", "Number of iterations", 10)
            .named("--objective", "Optimization objective", "performance")
            .named("--constraints", "Optimization constraints", "");
        
        auto& monitor_cmd = parser.add_subcommand("monitor", "Monitor running brain")
            .named("--interval", "Monitoring interval in seconds", 1.0)
            .named("--duration", "Monitoring duration in seconds", 0.0); // 0 = infinite
            
        auto& help_cmd = parser.add_subcommand("help", "Show help information")
            .named("--topic", "Help topic", "");
        
        // Parse arguments
        try {
            parser.parse_args(argc, argv);
        } catch (const std::exception& e) {
            cerr << "Error: " << e.what() << endl;
            return;
        }
        
        // Execute commands
        if (parser["run"].as<bool>()) {
            runSimulation(parser["run"].as<int>());
        } else if (parser["batch"].as<bool>()) {
            runBatch(parser["batch"].as<int>(), parser["batch"].as<int>());
        } else if (parser["save"].as<bool>()) {
            saveBrainState(parser["save"].as<std::string>());
        } else if (parser["load"].as<bool>()) {
            loadBrainState(parser["load"].as<std::string>());
        } else if (parser["benchmark"].as<bool>()) {
            runBenchmarks(parser["benchmark"].as<bool>(), parser["benchmark"].as<bool>(), parser["benchmark"].as<bool>(), parser["benchmark"].as<int>());
        } else if (parser["explore"].as<bool>()) {
            exploreConfigurations(parser["explore"].as<std::string>(), parser["explore"].as<int>(), parser["explore"].as<std::string>(), parser["explore"].as<std::string>());
        } else if (parser["monitor"].as<bool>()) {
            monitorBrain(parser["monitor"].as<double>(), parser["monitor"].as<double>());
        } else if (parser["help"].as<bool>()) {
            showHelp(parser["help"].as<std::string>());
        } else {
            parser.print_help();
        }
    }
    
private:
    void runSimulation(int steps) {
        cout << "=== Running NLM Simulation ===" << endl;
        
        // Create configuration
        Config config;
        config.set("random_seed", 42, ConfigSource::CommandLine);
        config.set("simulation_timestep", 0.001, ConfigSource::CommandLine);
        config.set("neuron_count", 1000, ConfigSource::CommandLine);
        config.set("region_count", 1, ConfigSource::CommandLine);
        config.set("connection_probability", 0.1, ConfigSource::CommandLine);
        config.set("stdp_ltp_weight", 0.02, ConfigSource::CommandLine);
        config.set("stdp_ltd_weight", 0.015, ConfigSource::CommandLine);
        
        // Create and initialize brain
        auto brain = make_shared<Brain>(config);
        brain->initialize();
        
        // Enable subsystems based on command line
        auto agentBrain = make_shared<AgentBrain>(brain);
        auto world = make_shared<SimpleWorld>();
        
        // Run simulation
        auto startTime = chrono::steady_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
            
            if (step % 100 == 0 && verbose_) {
                cout << "Step " << step << ": " 
                     << brain->getFiringNeuronCount() << " firing neurons, "
                     << brain->getTotalSpikeCount() << " total spikes" << endl;
            }
        }
        
        auto endTime = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
        
        // Collect results
        auto results = collectResults(brain);
        
        // Output results
        outputResults(results, "simulation");
        
        cout << "=== Simulation Complete ===" << endl;
        cout << "Steps: " << steps << endl;
        cout << "Duration: " << duration << " seconds" << endl;
        cout << "Average time per step: " << (duration * 1000.0 / steps) << " ms" << endl;
    }
    
    void runBatch(int experiments, int stepsPerExperiment) {
        cout << "=== Running NLM Batch Simulation ===" << endl;
        
        vector<map<string, double>> results;
        
        for (int exp = 0; exp < experiments; ++exp) {
            cout << "Running experiment " << (exp + 1) << "/" << experiments << "..." << endl;
            
            // Create configuration for this experiment
            Config config;
            config.set("random_seed", 42 + exp, ConfigSource::CommandLine);
            config.set("simulation_timestep", 0.001, ConfigSource::CommandLine);
            config.set("neuron_count", 1000, ConfigSource::CommandLine);
            config.set("region_count", 1, ConfigSource::CommandLine);
            config.set("connection_probability", 0.1, ConfigSource::CommandLine);
            
            // Create and initialize brain
            auto brain = make_shared<Brain>(config);
            brain->initialize();
            
            // Run experiment
            for (int step = 0; step < stepsPerExperiment; ++step) {
                brain->step(step);
            }
            
            // Collect results
            auto expResults = collectResults(brain);
            expResults["experiment"] = exp + 1;
            results.push_back(expResults);
        }
        
        // Output batch results
        outputBatchResults(results);
        
        cout << "=== Batch Simulation Complete ===" << endl;
        cout << "Experiments: " << experiments << endl;
        cout << "Steps per experiment: " << stepsPerExperiment << endl;
        cout << "Total steps: " << experiments * stepsPerExperiment << endl;
    }
    
    void saveBrainState(const std::string& filepath) {
        cout << "=== Saving Brain State ===" << endl;
        
        // For now, create a simple configuration
        Config config;
        config.set("random_seed", 42, ConfigSource::CommandLine);
        config.set("neuron_count", 100, ConfigSource::CommandLine);
        config.set("region_count", 1, ConfigSource::CommandLine);
        
        // Create and initialize brain
        auto brain = make_shared<Brain>(config);
        brain->initialize();
        
        // Save brain state
        if (brain->save(filepath)) {
            cout << "✓ Brain state saved to: " << filepath << endl;
        } else {
            cerr << "✗ Failed to save brain state to: " << filepath << endl;
        }
    }
    
    void loadBrainState(const std::string& filepath) {
        cout << "=== Loading Brain State ===" << endl;
        
        // Create configuration
        Config config;
        config.set("random_seed", 42, ConfigSource::CommandLine);
        config.set("neuron_count", 100, ConfigSource::CommandLine);
        config.set("region_count", 1, ConfigSource::CommandLine);
        
        // Create and initialize brain
        auto brain = make_shared<Brain>(config);
        brain->initialize();
        
        // Load brain state
        if (brain->load(filepath)) {
            cout << "✓ Brain state loaded from: " << filepath << endl;
        } else {
            cerr << "✗ Failed to load brain state from: " << filepath << endl;
        }
    }
    
    void runBenchmarks(bool large, bool simulation, bool memory, int duration) {
        cout << "=== Running NLM Benchmarks ===" << endl;
        
        auto startTime = chrono::steady_clock::now();
        
        if (large) {
            cout << "Running large brain benchmark..." << endl;
            runLargeBrainBenchmark();
        }
        
        if (simulation) {
            cout << "Running simulation benchmark..." << endl;
            runSimulationBenchmark();
        }
        
        if (memory) {
            cout << "Running memory benchmark..." << endl;
            runMemoryBenchmark();
        }
        
        auto endTime = chrono::steady_clock::now();
        auto totalDuration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
        
        cout << "=== Benchmarks Complete ===" << endl;
        cout << "Total duration: " << totalDuration << " seconds" << endl;
    }
    
    void runLargeBrainBenchmark() {
        Config config;
        config.set("neuron_count", 5000);
        config.set("region_count", 5);
        config.set("connection_probability", 0.05);
        
        auto brain = make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = chrono::steady_clock::now();
        
        for (int step = 0; step < 1000; ++step) {
            brain->step(step);
        }
        
        auto endTime = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
        
        cout << "   Large brain (5000 neurons, 5 regions): " << duration << "s" << endl;
        cout << "   Performance: " << (1000.0 / duration) << " steps/sec" << endl;
    }
    
    void runSimulationBenchmark() {
        Config config;
        config.set("neuron_count", 1000);
        config.set("region_count", 2);
        config.set("connection_probability", 0.1);
        
        auto brain = make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = chrono::steady_clock::now();
        
        for (int step = 0; step < 5000; ++step) {
            brain->step(step);
        }
        
        auto endTime = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
        
        cout << "   Simulation benchmark (1000 neurons, 5000 steps): " << duration << "s" << endl;
        cout << "   Performance: " << (5000.0 / duration) << " steps/sec" << endl;
    }
    
    void runMemoryBenchmark() {
        vector<vector<float>> largeData;
        
        auto startTime = chrono::steady_clock::now();
        
        // Allocate memory
        largeData.resize(1000, vector<float>(1000, 0.0f));
        
        // Initialize data
        for (int i = 0; i < 1000; ++i) {
            for (int j = 0; j < 1000; ++j) {
                largeData[i][j] = static_cast<float>(i * j) / 1000000.0f;
            }
        }
        
        // Access data
        float sum = 0.0f;
        for (int i = 0; i < 1000; ++i) {
            for (int j = 0; j < 1000; ++j) {
                sum += largeData[i][j];
            }
        }
        
        auto endTime = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
        
        cout << "   Memory benchmark (1M floats): " << duration << "s" << endl;
        cout << "   Data sum: " << sum << endl;
    }
    
    void exploreConfigurations(const std::string& searchSpace, int iterations, const std::string& objective, const std::string& constraints) {
        cout << "=== Exploring NLM Configurations ===" << endl;
        cout << "Search space: " << searchSpace << endl;
        cout << "Iterations: " << iterations << endl;
        cout << "Objective: " << objective << endl;
        cout << "Constraints: " << constraints << endl;
        
        // For now, implement basic hyperparameter search
        vector<Config> configs;
        for (int i = 0; i < iterations; ++i) {
            Config config;
            config.set("random_seed", 42 + i, ConfigSource::CommandLine);
            config.set("neuron_count", 500 + i * 50, ConfigSource::CommandLine);
            config.set("region_count", 1 + (i % 3), ConfigSource::CommandLine);
            config.set("connection_probability", 0.1 + (i % 5) * 0.02, ConfigSource::CommandLine);
            configs.push_back(config);
        }
        
        cout << "Generated " << configs.size() << " configurations" << endl;
        
        // Run experiments
        vector<map<string, double>> results;
        for (size_t i = 0; i < configs.size(); ++i) {
            cout << "Running experiment " << (i + 1) << "/" << configs.size() << "..." << endl;
            
            auto brain = make_shared<Brain>(configs[i]);
            brain->initialize();
            
            // Run simulation
            for (int step = 0; step < 100; ++step) {
                brain->step(step);
            }
            
            // Collect results
            auto result = collectResults(brain);
            result["iteration"] = i + 1;
            results.push_back(result);
        }
        
        // Output exploration results
        outputExplorationResults(results);
        
        cout << "=== Configuration Exploration Complete ===" << endl;
    }
    
    void monitorBrain(double interval, double duration) {
        cout << "=== Monitoring NLM Brain ===" << endl;
        cout << "Monitoring interval: " << interval << " seconds" << endl;
        cout << "Monitoring duration: " << duration << " seconds" << endl;
        
        if (duration <= 0.0) {
            cout << "Monitoring until interrupted (Ctrl+C)" << endl;
        }
        
        // Create brain
        Config config;
        config.set("random_seed", 42, ConfigSource::CommandLine);
        config.set("neuron_count", 100, ConfigSource::CommandLine);
        config.set("region_count", 1, ConfigSource::CommandLine);
        
        auto brain = make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = chrono::steady_clock::now();
        
        while (true) {
            auto currentTime = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
            
            if (duration > 0.0 && elapsed >= duration) {
                break;
            }
            
            // Collect and display statistics
            displayBrainStats(brain, elapsed);
            
            // Sleep for interval
            this_thread::sleep_for(chrono::seconds(static_cast<int>(interval)));
        }
        
        cout << "=== Brain Monitoring Complete ===" << endl;
    }
    
    void showHelp(const std::string& topic) {
        if (topic.empty()) {
            cout << "=== NLM Command Line Interface Help ===" << endl;
            cout << "Available commands:" << endl;
            cout << "  run <steps>          Run a single simulation" << endl;
            cout << "  batch <experiments> <steps>  Run multiple experiments" << endl;
            cout << "  save <filepath>      Save brain state" << endl;
            cout << "  load <filepath>      Load brain state" << endl;
            cout << "  benchmark            Run performance benchmarks" << endl;
            cout << "  explore              Explore hyperparameters" << endl;
            cout << "  monitor              Monitor running brain" << endl;
            cout << "  help [topic]         Show help information" << endl;
            cout << "" << endl;
            cout << "Use 'nlm --help <command>' for detailed help on a specific command." << endl;
        } else {
            cout << "Help for topic: " << topic << endl;
            // TODO: Implement topic-specific help
            cout << "Help for topic '" << topic << "' not yet implemented." << endl;
        }
    }
    
    map<string, double> collectResults(shared_ptr<Brain> brain) {
        map<string, double> results;
        
        results["total_neurons"] = brain->getTotalNeuronCount();
        results["total_synapses"] = brain->getTotalSynapseCount();
        results["active_neurons"] = brain->getActiveNeuronCount();
        results["firing_neurons"] = brain->getFiringNeuronCount();
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["average_firing_rate"] = brain->getAverageFiringRate();
        results["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        if (auto* dopamine = brain->getDopamine()) {
            results["dopamine_level"] = dopamine->getLevel();
        }
        
        if (auto* curiosity = brain->getCuriosity()) {
            results["curiosity_level"] = curiosity->getLevel();
        }
        
        return results;
    }
    
    void outputResults(const map<string, double>& results, const string& type) {
        if (verbose_) {
            cout << "Results:" << endl;
            for (const auto& result : results) {
                cout << "  " << result.first << ": " << result.second << endl;
            }
        }
        
        if (!outputFile_.empty()) {
            ofstream outfile(outputFile_, ios::app);
            if (outfile.is_open()) {
                outfile << type << " results at " 
                        << chrono::system_clock::now() << ":" << endl;
                for (const auto& result : results) {
                    outfile << result.first << ": " << result.second << endl;
                }
                outfile << endl;
            }
        }
    }
    
    void outputBatchResults(const vector<map<string, double>>& results) {
        cout << "Batch results:" << endl;
        for (const auto& result : results) {
            cout << "  Experiment " << result.at("experiment") << ": " 
                 << result.at("total_spikes") << " spikes, " 
                 << result.at("average_firing_rate") << " Hz average firing rate" << endl;
        }
        
        if (!outputFile_.empty()) {
            ofstream outfile(outputFile_, ios::app);
            if (outfile.is_open()) {
                outfile << "Batch results:" << endl;
                for (const auto& result : results) {
                    outfile << "  Experiment " << result.at("experiment") << ": " 
                           << result.at("total_spikes") << " spikes, " 
                           << result.at("average_firing_rate") << " Hz average firing rate" << endl;
                }
            }
        }
    }
    
    void outputExplorationResults(const vector<map<string, double>>& results) {
        cout << "Exploration results:" << endl;
        for (const auto& result : results) {
            cout << "  Iteration " << result.at("iteration") << ": " 
                 << result.at("total_spikes") << " spikes, " 
                 << result.at("average_firing_rate") << " Hz average firing rate" << endl;
        }
        
        if (!outputFile_.empty()) {
            ofstream outfile(outputFile_, ios::app);
            if (outfile.is_open()) {
                outfile << "Exploration results:" << endl;
                for (const auto& result : results) {
                    outfile << "  Iteration " << result.at("iteration") << ": " 
                           << result.at("total_spikes") << " spikes, " 
                           << result.at("average_firing_rate") << " Hz average firing rate" << endl;
                }
            }
        }
    }
    
    void displayBrainStats(shared_ptr<Brain> brain, double elapsedTime) {
        cout << "Time: " << elapsedTime << "s, " 
             << "Neurons: " << brain->getTotalNeuronCount() << ", " 
             << "Firing: " << brain->getFiringNeuronCount() << ", " 
             << "Spikes: " << brain->getTotalSpikeCount() << ", " 
             << "E/I ratio: " << brain->getExcitationInhibitionRatio() << endl;
    }
    
    bool verbose_ = false;
    std::string outputFile_;
};

int main(int argc, char** argv) {
    NLMCommandLineInterface cli;
    cli.run(argc, argv);
    return 0;
}