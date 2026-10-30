// Advanced NLM Command Line Interface
// Provides comprehensive control over brain simulations via command line

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <stdexcept>

// Command line options structure
struct CLIOptions {
    string config_file = "configs/default.cfg";
    string output_file = "";
    string checkpoint_file = "";
    string log_file = "";
    string mode = "standard"; // standard, debug, benchmark, experiment
    int steps = 1000;
    double timestep = 0.001;
    int neurons = 0; // 0 = use config default
    int regions = 0;
    bool enable_plasticity = true;
    bool enable_development = false;
    bool enable_curiosity = false;
    bool enable_checkpointing = false;
    bool enable_replay = false;
    bool continuous = false;
    bool quiet = false;
    bool verbose = false;
    string experiment_type = "standard"; // standard, learning, stmep, plasticity
    map<string, string> custom_params;
    bool help = false;
    bool version = false;
    bool list_configs = false;
};

class NLMCLI {
private:
    CLIOptions options;
    shared_ptr<Brain> brain;
    shared_ptr<Config> config;
    shared_ptr<Logger> logger;
    chrono::steady_clock::time_point start_time;
    chrono::steady_clock::time_point step_start_time;
    size_t total_spikes_at_start = 0;
    size_t total_steps_completed = 0;
    bool running = false;
    
public:
    NLMCLI(int argc, char** argv) {
        parseArguments(argc, argv);
        setupLogging();
        initializeBrain();
    }
    
    void parseArguments(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--version") {
                options.version = true;
            } else if (arg == "--list-configs") {
                options.list_configs = true;
            } else if (arg == "--config" || arg == "-c") {
                if (i + 1 < argc) {
                    options.config_file = argv[++i];
                }
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) {
                    options.output_file = argv[++i];
                }
            } else if (arg == "--checkpoint" || arg == "-ck") {
                if (i + 1 < argc) {
                    options.checkpoint_file = argv[++i];
                }
            } else if (arg == "--log" || arg == "-l") {
                if (i + 1 < argc) {
                    options.log_file = argv[++i];
                }
            } else if (arg == "--mode" || arg == "-m") {
                if (i + 1 < argc) {
                    options.mode = argv[++i];
                }
            } else if (arg == "--steps" || arg == "-s") {
                if (i + 1 < argc) {
                    options.steps = stoi(argv[++i]);
                }
            } else if (arg == "--timestep") {
                if (i + 1 < argc) {
                    options.timestep = stod(argv[++i]);
                }
            } else if (arg == "--neurons" || arg == "-n") {
                if (i + 1 < argc) {
                    options.neurons = stoi(argv[++i]);
                }
            } else if (arg == "--regions") {
                if (i + 1 < argc) {
                    options.regions = stoi(argv[++i]);
                }
            } else if (arg == "--no-plasticity") {
                options.enable_plasticity = false;
            } else if (arg == "--development") {
                options.enable_development = true;
            } else if (arg == "--curiosity") {
                options.enable_curiosity = true;
            } else if (arg == "--checkpointing") {
                options.enable_checkpointing = true;
            } else if (arg == "--replay") {
                options.enable_replay = true;
            } else if (arg == "--continuous") {
                options.continuous = true;
            } else if (arg == "--quiet" || arg == "-q") {
                options.quiet = true;
            } else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            } else if (arg == "--experiment") {
                if (i + 1 < argc) {
                    options.experiment_type = argv[++i];
                }
            } else if (arg == "--param") {
                if (i + 2 < argc) {
                    options.custom_params[argv[++i]] = argv[++i];
                }
            } else {
                // Handle unrecognized arguments
                if (!options.quiet) {
                    cerr << "Warning: Unknown argument '" << arg << "'" << endl;
                }
            }
        }
    }
    
    void setupLogging() {
        logger = make_shared<Logger>();
        
        if (!options.log_file.empty()) {
            auto fileLogger = make_shared<FileLogger>(options.log_file, LogLevel::Info);
            logger->addLogger(fileLogger);
        }
        
        auto consoleLogger = make_shared<ConsoleLogger>(
            options.verbose ? LogLevel::Debug : LogLevel::Info
        );
        logger->addLogger(consoleLogger);
        
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("NLM CLI initialized with mode: " + options.mode);
    }
    
    void initializeBrain() {
        config = make_shared<Config>();
        
        // Load configuration
        if (config->loadFromFile(options.config_file)) {
            NLM_LOG_INFO("Loaded configuration from: " + options.config_file);
        } else {
            NLM_LOG_WARNING("Using default configuration (file not found)");
        }
        
        // Override with command line options
        if (options.neurons > 0) {
            config->set("brain.neuron_count", static_cast<int64_t>(options.neurons));
        }
        if (options.regions > 0) {
            config->set("brain.region_count", static_cast<int64_t>(options.regions));
        }
        config->set("simulation.timestep", options.timestep);
        
        // Enable requested features
        if (options.enable_plasticity) {
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
        }
        if (options.enable_development) {
            config->set("development.system.enabled", true);
        }
        if (options.enable_curiosity) {
            config->set("neuromodulation.curiosity.enable", true);
        }
        if (options.enable_checkpointing) {
            config->set("simulation.checkpoint.enable", true);
        }
        if (options.enable_replay) {
            config->set("simulation.replay.enable", true);
        }
        
        // Apply custom parameters
        for (const auto& param : options.custom_params) {
            config->set(param.first, param.second);
        }
        
        // Create brain
        brain = make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            throw runtime_error("Brain initialization failed");
        }
        
        // Record initial state
        total_spikes_at_start = brain->getTotalSpikeCount();
        start_time = chrono::steady_clock::now();
        
        NLM_LOG_INFO("Brain initialized successfully");
        NLM_LOG_INFO("Neuron count: " + to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Region count: " + to_string(brain->getRegionCount()));
    }
    
    void run() {
        if (options.help) {
            printHelp();
            return;
        }
        
        if (options.version) {
            printVersion();
            return;
        }
        
        if (options.list_configs) {
            listAvailableConfigs();
            return;
        }
        
        NLM_LOG_INFO("Starting NLM simulation in " + options.mode + " mode");
        NLM_LOG_INFO("Steps to run: " + to_string(options.steps));
        
        running = true;
        step_start_time = chrono::steady_clock::now();
        
        switch (options.mode) {
            case "debug":
                runDebugMode();
                break;
            case "benchmark":
                runBenchmarkMode();
                break;
            case "experiment":
                runExperimentMode();
                break;
            case "standard":
            default:
                runStandardMode();
                break;
        }
        
        running = false;
        
        // Final statistics
        auto end_time = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(end_time - start_time);
        
        if (!options.quiet) {
            printFinalStats(duration);
        }
        
        // Save checkpoint if requested
        if (options.enable_checkpointing && !options.checkpoint_file.empty()) {
            if (brain->save(options.checkpoint_file)) {
                NLM_LOG_INFO("Checkpoint saved to: " + options.checkpoint_file);
            } else {
                NLM_LOG_WARNING("Failed to save checkpoint");
            }
        }
        
        // Write results to output file if requested
        if (!options.output_file.empty()) {
            writeResultsToFile();
        }
        
        NLM_LOG_INFO("Simulation completed in " + to_string(duration.count()) + " seconds");
    }
    
    void runStandardMode() {
        NLM_LOG_INFO("Running in standard mode");
        
        for (size_t step = 0; step < options.steps && running; ++step) {
            // Record step start time
            auto step_start = chrono::steady_clock::now();
            
            // Execute simulation step
            brain->step(step, step * options.timestep);
            
            // Log progress periodically
            if (step % 100 == 0 && !options.quiet) {
                logStepProgress(step);
            }
            
            // Check for early termination
            if (checkTerminationCondition(step)) {
                break;
            }
            
            // Control step rate
            auto step_end = chrono::steady_clock::now();
            auto step_duration = chrono::duration_cast<chrono::milliseconds>(step_end - step_start);
            
            if (options.timestep > 0 && step_duration.count() < options.timestep * 1000) {
                this_thread::sleep_for(
                    chrono::milliseconds(static_cast<int>(options.timestep * 1000 - step_duration.count()))
                );
            }
            
            total_steps_completed++;
        }
    }
    
    void runDebugMode() {
        NLM_LOG_INFO("Running in debug mode - detailed logging enabled");
        
        for (size_t step = 0; step < options.steps && running; ++step) {
            NLM_LOG_DEBUG("=== STEP " + to_string(step) + " ===");
            
            // Log brain state before step
            if (step % 10 == 0) {
                logBrainState();
            }
            
            // Execute step with detailed monitoring
            auto step_start = chrono::steady_clock::now();
            brain->step(step, step * options.timestep);
            
            // Log performance metrics
            auto step_end = chrono::steady_clock::now();
            auto step_duration = chrono::duration_cast<chrono::microseconds>(step_end - step_start);
            
            NLM_LOG_DEBUG("Step execution time: " + to_string(step_duration.count()) + " microseconds");
            
            // Log region-specific activity
            logRegionActivity(step);
            
            // Check for learning events
            logLearningEvents(step);
            
            // Check for early termination
            if (checkTerminationCondition(step)) {
                break;
            }
            
            total_steps_completed++;
        }
    }
    
    void runBenchmarkMode() {
        NLM_LOG_INFO("Running in benchmark mode - performance measurement");
        
        // Warm-up phase
        NLM_LOG_INFO("Warm-up phase (100 steps)...");
        for (size_t step = 0; step < 100; ++step) {
            brain->step(step, step * options.timestep);
        }
        
        // Benchmark phase
        NLM_LOG_INFO("Benchmark phase...");
        auto bench_start = chrono::steady_clock::now();
        
        size_t spikes_at_start = brain->getTotalSpikeCount();
        size_t neurons_at_start = brain->getTotalNeuronCount();
        
        for (size_t step = 0; step < options.steps; ++step) {
            brain->step(step, step * options.timestep);
        }
        
        auto bench_end = chrono::steady_clock::now();
        auto bench_duration = chrono::duration_cast<chrono::seconds>(bench_end - bench_start);
        
        size_t spikes_at_end = brain->getTotalSpikeCount();
        size_t neurons_at_end = brain->getTotalNeuronCount();
        
        // Calculate performance metrics
        double steps_per_second = options.steps / bench_duration.count();
        double spikes_per_step = (spikes_at_end - spikes_at_start) / static_cast<double>(options.steps);
        
        NLM_LOG_INFO("=== BENCHMARK RESULTS ===");
        NLM_LOG_INFO("Duration: " + to_string(bench_duration.count()) + " seconds");
        NLM_LOG_INFO("Steps per second: " + to_string(steps_per_second));
        NLM_LOG_INFO("Spikes per step: " + to_string(spikes_per_step));
        NLM_LOG_INFO("Neurons stable: " + to_string(neurons_at_end == neurons_at_start));
        NLM_LOG_INFO("Spike stability: " + to_string((spikes_at_end - spikes_at_start) / static_cast<double>(spikes_at_start) < 0.01));
        
        // Write benchmark results to file
        if (!options.output_file.empty()) {
            writeBenchmarkResults(options.output_file, steps_per_second, spikes_per_step);
        }
    }
    
    void runExperimentMode() {
        NLM_LOG_INFO("Running in experiment mode");
        
        Phase6IntegratedExperiment experiment;
        
        // Create experiment configuration
        Phase6Config config;
        config.neuronCount = brain->getTotalNeuronCount();
        config.maxSteps = options.steps;
        config.enableCheckpointing = options.enable_checkpointing;
        config.enableReplay = options.enable_replay;
        config.enableDevelopment = options.enable_development;
        
        // Run experiment
        NLM_LOG_INFO("Running Phase 6 integration experiment...");
        auto experiment_start = chrono::steady_clock::now();
        
        auto result = experiment.run(config);
        
        auto experiment_end = chrono::steady_clock::now();
        auto experiment_duration = chrono::duration_cast<chrono::seconds>(experiment_end - experiment_start);
        
        NLM_LOG_INFO("=== EXPERIMENT RESULTS ===");
        NLM_LOG_INFO("Total reward: " + to_string(result.totalReward));
        NLM_LOG_INFO("Avg firing rate: " + to_string(result.avgFiringRate));
        NLM_LOG_INFO("Episodes stored: " + to_string(result.memoryEpisodesStored));
        NLM_LOG_INFO("Dopamine level: " + to_string(result.dopamineLevel));
        NLM_LOG_INFO("Experiment duration: " + to_string(experiment_duration.count()) + " seconds");
        
        // Write experiment results
        if (!options.output_file.empty()) {
            writeExperimentResults(options.output_file, result);
        }
    }
    
    void logStepProgress(size_t step) {
        size_t current_spikes = brain->getTotalSpikeCount() - total_spikes_at_start;
        size_t firing_neurons = brain->getFiringNeuronCount();
        float firing_rate = brain->getAverageFiringRate();
        float e_i_ratio = brain->getExcitationInhibitionRatio();
        
        auto current_time = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(current_time - start_time);
        
        NLM_LOG_INFO("Step " + to_string(step) + ": " +
                    "Spikes: " + to_string(current_spikes) + ", " +
                    "Firing: " + to_string(firing_neurons) + ", " +
                    "Rate: " + to_string(firing_rate) + ", " +
                    "E/I: " + to_string(e_i_ratio) + ", " +
                    "Elapsed: " + to_string(elapsed.count()) + "s");
    }
    
    void logBrainState() {
        NLM_LOG_DEBUG("Brain State:");
        NLM_LOG_DEBUG("  Total neurons: " + to_string(brain->getTotalNeuronCount()));
        NLM_LOG_DEBUG("  Total synapses: " + to_string(brain->getTotalSynapseCount()));
        NLM_LOG_DEBUG("  Total spikes: " + to_string(brain->getTotalSpikeCount()));
        NLM_LOG_DEBUG("  Firing neurons: " + to_string(brain->getFiringNeuronCount()));
        NLM_LOG_DEBUG("  Average firing rate: " + to_string(brain->getAverageFiringRate()));
        NLM_LOG_DEBUG("  E/I ratio: " + to_string(brain->getExcitationInhibitionRatio()));
        
        // Log region information
        size_t region_count = brain->getRegionCount();
        NLM_LOG_DEBUG("  Region count: " + to_string(region_count));
        
        for (size_t i = 0; i < region_count && i < 5; ++i) {
            auto region = brain->getRegion(static_cast<RegionId>(i + 1));
            if (region) {
                NLM_LOG_DEBUG("  Region " + to_string(i + 1) + ": " +
                            to_string(region->getPopulationCount()) + " populations,
                            " + to_string(region->getTotalNeuronCount()) + " neurons");
            }
        }
    }
    
    void logRegionActivity(size_t step) {
        if (step % 50 == 0) {
            size_t region_count = brain->getRegionCount();
            NLM_LOG_DEBUG("Region Activity (Step " + to_string(step) + "):");
            
            for (size_t i = 0; i < region_count; ++i) {
                auto region = brain->getRegion(static_cast<RegionId>(i + 1));
                if (region) {
                    size_t active_neurons = 0;
                    for (const auto& population : region->getPopulations()) {
                        for (const auto& neuron : population->getNeurons()) {
                            if (neuron->isActive()) {
                                active_neurons++;
                            }
                        }
                    }
                    
                    NLM_LOG_DEBUG("  Region " + to_string(i + 1) + ": " +
                                to_string(active_neurons) + " active neurons");
                }
            }
        }
    }
    
    void logLearningEvents(size_t step) {
        if (step % 100 == 0) {
            NLM_LOG_DEBUG("Learning Events (Step " + to_string(step) + "):");
            
            // Check for plasticity events
            auto stdp = brain->getSTDP();
            if (stdp) {
                NLM_LOG_DEBUG("  STDP: Learning active");
            }
            
            auto hebbian = brain->getHebbian();
            if (hebbian) {
                NLM_LOG_DEBUG("  Hebbian: Learning active");
            }
            
            auto structural = brain->getStructuralPlasticity();
            if (structural) {
                NLM_LOG_DEBUG("  Structural Plasticity: Synaptogenesis rate: " +
                            to_string(structural->getSynaptogenesisRate()));
            }
            
            // Check for development
            auto development = brain->getDevelopmentSystem();
            if (development) {
                NLM_LOG_DEBUG("  Development: Stage: " +
                            to_string(static_cast<int>(development->getDevelopmentalStage())));
            }
        }
    }
    
    bool checkTerminationCondition(size_t step) {
        // Check for manual interruption
        if (!running) {
            return true;
        }
        
        // Check for continuous mode interruption
        if (options.continuous && step >= options.steps) {
            return true;
        }
        
        // Check for high activity (potential instability)
        float firing_rate = brain->getAverageFiringRate();
        if (firing_rate > 100.0) { // Arbitrary high firing rate threshold
            NLM_LOG_WARNING("High firing rate detected: " + to_string(firing_rate) +
                           " - potential instability");
            // Don't terminate, just warn
        }
        
        // Check for learning saturation
        auto stdp = brain->getSTDP();
        if (stdp && stdp->getLearningRate() < 1e-6) {
            NLM_LOG_INFO("STDP learning rate very low - considering learning complete");
        }
        
        return false;
    }
    
    void printHelp() {
        cout << "NLM Command Line Interface" << endl;
        cout << "==========================" << endl;
        cout << "Usage: nlm_cli [options]" << endl;
        cout << endl;
        cout << "Basic Options:" << endl;
        cout << "  --help, -h              Show this help message" << endl;
        cout << "  --version               Show version information" << endl;
        cout << "  --list-configs          List available configuration files" << endl;
        cout << "  --config, -c <file>     Configuration file (default: configs/default.cfg)" << endl;
        cout << "  --output, -o <file>     Output file for results" << endl;
        cout << "  --checkpoint, -ck <file> Checkpoint file" << endl;
        cout << "  --log, -l <file>        Log file" << endl;
        cout << "  --mode <mode>           Execution mode (standard, debug, benchmark, experiment)" << endl;
        cout << "  --steps, -s <n>         Number of simulation steps" << endl;
        cout << "  --timestep <t>          Simulation timestep in seconds" << endl;
        cout << "  --neurons, -n <n>       Number of neurons (overrides config)" << endl;
        cout << "  --regions <n>           Number of regions (overrides config)" << endl;
        cout << endl;
        cout << "Feature Options:" << endl;
        cout << "  --no-plasticity         Disable plasticity (STDP and Hebbian)" << endl;
        cout << "  --development           Enable development system" << endl;
        cout << "  --curiosity             Enable curiosity-driven exploration" << endl;
        cout << "  --checkpointing         Enable checkpointing" << endl;
        cout << "  --replay                Enable replay capabilities" << endl;
        cout << "  --continuous            Run in continuous mode" << endl;
        cout << endl;
        cout << "Output Options:" << endl;
        cout << "  --quiet, -q             Quiet mode (minimal output)" << endl;
        cout << "  --verbose, -v           Verbose mode (debug output)" << endl;
        cout << endl;
        cout << "Experiment Options:" << endl;
        cout << "  --experiment <type>     Experiment type (standard, learning, stmep, plasticity)" << endl;
        cout << "  --param <key>=<value>   Custom parameter (can be repeated)" << endl;
        cout << endl;
        cout << "Modes:" << endl;
        cout << "  standard                Standard simulation (default)" << endl;
        cout << "  debug                   Debug mode with detailed logging" << endl;
        cout << "  benchmark               Performance benchmark" << endl;
        cout << "  experiment              Phase 6 integration experiment" << endl;
        cout << endl;
        cout << "Examples:" << endl;
        cout << "  nlm_cli --steps 1000 --neurons 500" << endl;
        cout << "  nlm_cli --mode debug --verbose --log simulation.log" << endl;
        cout << "  nlm_cli --mode benchmark --output benchmark_results.csv" << endl;
        cout << "  nlm_cli --mode experiment --steps 2000" << endl;
    }
    
    void printVersion() {
        cout << "NLM Phase 6 (熙然) - Advanced Neural Learning Machine" << endl;
        cout << "Version: 0.1.0" << endl;
        cout << "Build Type: Release" << endl;
        cout << "C++ Standard: C++20" << endl;
        cout << "Phase: FINAL INTEGRATION" << endl;
        cout << "All systems integrated and functional." << endl;
    }
    
    void listAvailableConfigs() {
        cout << "Available configuration files:" << endl;
        
        // Look for config files in common locations
        vector<string> config_paths = {
            "configs/default.cfg",
            "configs/learning.cfg",
            "configs/development.cfg",
            "configs/experiment.cfg",
            "configs/debug.cfg"
        };
        
        for (const auto& path : config_paths) {
            ifstream file(path);
            if (file.good()) {
                cout << "  ✓ " << path << endl;
                
                // Show some key config values
                ifstream config_file(path);
                string line;
                int line_count = 0;
                while (getline(config_file, line) && line_count < 10) {
                    if (line.find("#") == string::npos) { // Skip comments
                        cout << "    " << line << endl;
                        line_count++;
                    }
                }
            } else {
                cout << "  ✗ " << path << " (not found)" << endl;
            }
        }
    }
    
    void printFinalStats(chrono::seconds duration) {
        size_t final_spikes = brain->getTotalSpikeCount();
        size_t spikes_generated = final_spikes - total_spikes_at_start;
        
        cout << "\n=== FINAL STATISTICS ===" << endl;
        cout << "Total simulation time: " << duration.count() << " seconds" << endl;
        cout << "Steps completed: " << total_steps_completed << endl;
        cout << "Total spikes generated: " << spikes_generated << endl;
        cout << "Average spikes per second: " << 
            (spikes_generated / duration.count()) << endl;
        cout << "Final firing rate: " << brain->getAverageFiringRate() << endl;
        cout << "Final E/I ratio: " << brain->getExcitationInhibitionRatio() << endl;
        cout << "Final development stage: " << 
            static_cast<int>(brain->getDevelopmentalStage()) << endl;
        
        // Brain status summary
        brain->logStatus();
    }
    
    void writeResultsToFile() {
        ofstream file(options.output_file);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open output file: " + options.output_file);
            return;
        }
        
        file << "NLM Simulation Results" << endl;
        file << "=======================" << endl;
        file << "Configuration: " << options.config_file << endl;
        file << "Mode: " << options.mode << endl;
        file << "Steps: " << options.steps << endl;
        file << "Timestep: " << options.timestep << endl;
        file << "Neurons: " << brain->getTotalNeuronCount() << endl;
        file << "Regions: " << brain->getRegionCount() << endl;
        file << "Duration: " << chrono::duration_cast<chrono::seconds>(
            chrono::steady_clock::now() - start_time).count() << " seconds" << endl;
        file << endl;
        
        // Simulation progress
        file << "Simulation Progress:" << endl;
        file << "Step,Time,Spikes,FiringRate,EIRatio" << endl;
        
        // This would need to be modified to capture actual step-by-step data
        // For now, we'll write the final statistics
        file << options.steps << ","
             << chrono::duration_cast<chrono::seconds>(
                 chrono::steady_clock::now() - start_time).count() << ","
             << (brain->getTotalSpikeCount() - total_spikes_at_start) << ","
             << brain->getAverageFiringRate() << ","
             << brain->getExcitationInhibitionRatio() << endl;
        
        file.close();
        NLM_LOG_INFO("Results written to: " + options.output_file);
    }
    
    void writeBenchmarkResults(const string& filename, double steps_per_second, double spikes_per_step) {
        ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open benchmark file: " + filename);
            return;
        }
        
        file << "NLM Benchmark Results" << endl;
        file << "======================" << endl;
        file << "Steps per second: " << steps_per_second << endl;
        file << "Spikes per step: " << spikes_per_step << endl;
        file << "Total spikes: " << (brain->getTotalSpikeCount() - total_spikes_at_start) << endl;
        file << "Simulation time: " << 
            chrono::duration_cast<chrono::seconds>(
                chrono::steady_clock::now() - start_time).count() << " seconds" << endl;
        file << "Average firing rate: " << brain->getAverageFiringRate() << endl;
        
        file.close();
        NLM_LOG_INFO("Benchmark results written to: " + filename);
    }
    
    void writeExperimentResults(const string& filename, const Phase6IntegratedExperiment::Result& result) {
        ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open experiment results file: " + filename);
            return;
        }
        
        file << "NLM Phase 6 Experiment Results" << endl;
        file << "===============================" << endl;
        file << "Total reward: " << result.totalReward << endl;
        file << "Avg firing rate: " << result.avgFiringRate << endl;
        file << "Episodes stored: " << result.memoryEpisodesStored << endl;
        file << "Dopamine level: " << result.dopamineLevel << endl;
        file << "Working memory integrated: " << 
            (result.memoryWorkingMemoryIntegrated ? "true" : "false") << endl;
        file << "Episodic memory integrated: " << 
            (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << endl;
        file << "Neuromodulation integrated: " << 
            (result.neuromodulationIntegrated ? "true" : "false") << endl;
        file << "Prediction integrated: " << 
            (result.predictionIntegrated ? "true" : "false") << endl;
        file << "Development integrated: " << 
            (result.developmentIntegrated ? "true" : "false") << endl;
        file << "Checkpointing works: " << 
            (result.checkpointingWorks ? "true" : "false") << endl;
        file << "Replay works: " << 
            (result.replayWorks ? "true" : "false") << endl;
        file << "Total wall clock time: " << result.totalWallClockTime << " seconds" << endl;
        
        file.close();
        NLM_LOG_INFO("Experiment results written to: " + filename);
    }
    
    void stop() {
        running = false;
    }
};