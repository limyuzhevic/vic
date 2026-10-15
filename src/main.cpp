} // namespace nlm

// Command line argument parsing namespace
namespace nlm::cli {

    struct CommandLineOptions {
        std::string configFile = "configs/default.cfg";
        bool verbose = false;
        bool quiet = false;
        bool help = false;
        std::string mode = "demo";
        std::string experiment = "phase2";
        size_t neuronCount = 500;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        size_t maxSteps = 1000;
        std::string checkpointPath;
        bool enableCheckpointing = false;
        bool enableReplay = false;
        bool enableDevelopment = false;
        bool enableNeuromodulation = false;
        bool enableLearning = false;
        uint64_t randomSeed = 42;
        std::string logLevel = "INFO";
        bool exportResults = false;
        std::string outputFile;
        bool enableSIMD = false;
        bool enableParallel = false;
        bool enableMemoryPools = false;
        bool enableCaching = false;
        bool enableCuriosity = false;
        bool enableNovelty = false;
        bool enableConceptFormation = false;
        bool enablePlanning = false;
        bool enableAttention = false;
        bool enableSocialLearning = false;
        bool enableContinualLearning = false;
    };

    class ArgumentParser {
    public:
        static CommandLineOptions parse(int argc, char** argv);
        static void printHelp(const std::string& programName);
        static void printVersion();
        static bool validateOptions(const CommandLineOptions& options);
        
    private:
        static std::string trim(const std::string& str);
        static std::string toLower(const std::string& str);
        static bool isFlag(const std::string& arg);
        static std::string getFlagValue(const std::string& arg, const std::string& flag);
    };
}

// CLI Implementation
namespace nlm::cli {

    CommandLineOptions ArgumentParser::parse(int argc, char** argv) {
        CommandLineOptions options;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--version") {
                options.help = true; // Will trigger version printing
            } else if (arg.substr(0, 2) == "--") {
                std::string key = arg.substr(2);
                size_t pos = key.find('=');
                if (pos != std::string::npos) {
                    std::string value = key.substr(pos + 1);
                    key = key.substr(0, pos);
                    
                    if (key == "config") {
                        options.configFile = value;
                    } else if (key == "mode") {
                        options.mode = value;
                    } else if (key == "experiment") {
                        options.experiment = value;
                    } else if (key == "neuron-count") {
                        options.neuronCount = std::stoul(value);
                    } else if (key == "region-count") {
                        options.regionCount = std::stoul(value);
                    } else if (key == "connection-prob") {
                        options.connectionProbability = std::stof(value);
                    } else if (key == "max-steps") {
                        options.maxSteps = std::stoul(value);
                    } else if (key == "checkpoint-path") {
                        options.checkpointPath = value;
                    } else if (key == "log-level") {
                        options.logLevel = value;
                    } else if (key == "random-seed") {
                        options.randomSeed = std::stoull(value);
                    } else if (key == "output-file") {
                        options.outputFile = value;
                    }
                } else {
                    // Boolean flags
                    if (key == "verbose") options.verbose = true;
                    else if (key == "quiet") options.quiet = true;
                    else if (key == "enable-checkpointing") options.enableCheckpointing = true;
                    else if (key == "enable-replay") options.enableReplay = true;
                    else if (key == "enable-development") options.enableDevelopment = true;
                    else if (key == "enable-neuromodulation") options.enableNeuromodulation = true;
                    else if (key == "enable-learning") options.enableLearning = true;
                    else if (key == "export-results") options.exportResults = true;
                    else if (key == "enable-simd") options.enableSIMD = true;
                    else if (key == "enable-parallel") options.enableParallel = true;
                    else if (key == "enable-memory-pools") options.enableMemoryPools = true;
                    else if (key == "enable-caching") options.enableCaching = true;
                    else if (key == "enable-curiosity") options.enableCuriosity = true;
                    else if (key == "enable-novelty") options.enableNovelty = true;
                    else if (key == "enable-concept-formation") options.enableConceptFormation = true;
                    else if (key == "enable-planning") options.enablePlanning = true;
                    else if (key == "enable-attention") options.enableAttention = true;
                    else if (key == "enable-social-learning") options.enableSocialLearning = true;
                    else if (key == "enable-continual-learning") options.enableContinualLearning = true;
                }
            } else if (arg[0] == '-') {
                std::string key = arg.substr(1);
                if (i + 1 < argc) {
                    std::string value = argv[++i];
                    
                    if (key == "c") options.configFile = value;
                    else if (key == "m") options.mode = value;
                    else if (key == "e") options.experiment = value;
                    else if (key == "n") options.neuronCount = std::stoul(value);
                    else if (key == "r") options.regionCount = std::stoul(value);
                    else if (key == "p") options.connectionProbability = std::stof(value);
                    else if (key == "s") options.maxSteps = std::stoul(value);
                    else if (key == "l") options.logLevel = value;
                    else if (key == "seed") options.randomSeed = std::stoull(value);
                    else if (key == "o") options.outputFile = value;
                }
            }
        }
        
        return options;
    }

    std::string ArgumentParser::trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }

    std::string ArgumentParser::toLower(const std::string& str) {
        std::string result = str;
        for (char& c : result) {
            c = std::tolower(c);
        }
        return result;
    }

    bool ArgumentParser::isFlag(const std::string& arg) {
        return arg.find('=') != std::string::npos;
    }

    std::string ArgumentParser::getFlagValue(const std::string& arg, const std::string& flag) {
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string key = arg.substr(0, pos);
            if (toLower(key) == toLower(flag)) {
                return arg.substr(pos + 1);
            }
        }
        return "";
    }

    void ArgumentParser::printHelp(const std::string& programName) {
        std::cout << "NLM (Neural Learning Machine) - Command Line Interface" << std::endl;
        std::cout << "=======================================================" << std::endl << std::endl;
        
        std::cout << "Usage: " << programName << " [options]" << std::endl << std::endl;
        
        std::cout << "Basic Options:" << std::endl;
        std::cout << "  -h, --help                  Show this help message" << std::endl;
        std::cout << "  --version                   Show version information" << std::endl;
        std::cout << "  -c, --config FILE           Configuration file path (default: configs/default.cfg)" << std::endl;
        std::cout << "  -m, --mode MODE             Operating mode: 'demo', 'experiment', 'debug' (default: demo)" << std::endl;
        std::cout << "  -e, --experiment NAME       Experiment phase: 'phase2', 'phase3', 'phase4', 'phase5', 'phase6' (default: phase2)" << std::endl << std::endl;
        
        std::cout << "Simulation Parameters:" << std::endl;
        std::cout << "  -n, --neuron-count COUNT    Number of neurons (default: 500)" << std::endl;
        std::cout << "  -r, --region-count COUNT    Number of regions (default: 1)" << std::endl;
        std::cout << "  -p, --connection-prob PROB  Connection probability (default: 0.1)" << std::endl;
        std::cout << "  -s, --max-steps STEPS       Maximum simulation steps (default: 1000)" << std::endl << std::endl;
        
        std::cout << "Advanced Options:" << std::endl;
        std::cout << "  --enable-checkpointing      Enable checkpoint save/load functionality" << std::endl;
        std::cout << "  --enable-replay             Enable episodic memory replay" << std::endl;
        std::cout << "  --enable-development        Enable developmental processes" << std::endl;
        std::cout << "  --enable-neuromodulation     Enable neuromodulation systems" << std::endl;
        std::cout << "  --enable-learning           Enable learning experiments" << std::endl;
        std::cout << "  --export-results            Export results to file" << std::endl;
        std::cout << "  --checkpoint-path PATH      Checkpoint file path (default: ./checkpoint.bin)" << std::endl;
        std::cout << "  --output-file FILE          Output file for results (default: nlm_results.txt)" << std::endl;
        std::cout << "  --random-seed SEED          Random seed for reproducible results" << std::endl;
        std::cout << "  --log-level LEVEL           Logging level: 'DEBUG', 'INFO', 'WARNING', 'ERROR' (default: INFO)" << std::endl << std::endl;
        
        std::cout << "Phase-specific Options:" << std::endl;
        std::cout << "  For Phase 3 experiments:" << std::endl;
        std::cout << "    --enable-curiosity        Enable curiosity-driven exploration" << std::endl;
        std::cout << "    --enable-novelty          Enable novelty detection" << std::endl;
        std::cout << "  For Phase 4 experiments:" << std::endl;
        std::cout << "    --enable-concept-formation Enable concept formation" << std::endl;
        std::cout << "    --enable-planning          Enable action planning" << std::endl;
        std::cout << "  For Phase 6 experiments:" << std::endl;
        std::cout << "    --enable-social-learning   Enable social learning capabilities" << std::endl;
        std::cout << "    --enable-continual-learning Enable continual learning" << std::endl << std::endl;
        
        std::cout << "Performance Options:" << std::endl;
        std::cout << "  --enable-simd               Enable SIMD vectorization" << std::endl;
        std::cout << "  --enable-parallel           Enable parallel processing" << std::endl;
        std::cout << "  --enable-memory-pools       Enable memory pools for optimization" << std::endl;
        std::cout << "  --enable-caching            Enable performance caching" << std::endl << std::endl;
        
        std::cout << "Output and Export Options:" << std::endl;
        std::cout << "  --export-results            Export detailed results" << std::endl;
        std::cout << "  --export-json              Export results as JSON" << std::endl;
        std::cout << "  --export-csv               Export results as CSV" << std::endl;
        std::cout << "  --output-file FILE          Output file for results" << std::endl;
        std::cout << "  --results-dir DIR           Directory for result files" << std::endl << std::endl;
        
        std::cout << "Debug and Development Options:" << std::endl;
        std::cout << "  --debug                     Enable debug mode" << std::endl;
        std::cout << "  --dry-run                   Validate configuration without running" << std::endl;
        std::cout << "  --verbose                   Enable verbose logging" << std::endl;
        std::cout << "  --quiet                    Suppress non-error output" << std::endl << std::endl;
        
        std::cout << "Expert Options:" << std::endl;
        std::cout << "  --random-seed SEED          Set random seed for reproducible experiments" << std::endl;
        std::cout << "  --timestep DT               Set simulation timestep" << std::endl;
        std::cout << "  --neuron-distribution TYPE  Neuron initialization distribution" << std::endl;
        std::cout << "  --connection-model MODEL    Synaptic connection model" << std::endl;
        std::cout << "  --plasticity-rules RULES    Plasticity rules to enable/disable" << std::endl;
        std::cout << "  --neuromodulation-scale SCALE Scale neuromodulation effects" << std::endl << std::endl;
        
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << programName << " --mode demo --experiment phase2" << std::endl;
        std::cout << "  " << programName << " --experiment phase6 --neuron-count 1000 --max-steps 5000 --enable-checkpointing --enable-replay" << std::endl;
        std::cout << "  " << programName << " --mode experiment --config my_config.cfg --random-seed 12345 --export-results --export-json" << std::endl;
        std::cout << "  " << programName << " --experiment phase3 --enable-curiosity --enable-novelty --enable-social-learning" << std::endl;
        std::cout << "  " << programName << " --experiment phase4 --enable-concept-formation --enable-planning --enable-attention" << std::endl;
        std::cout << "  " << programName << " --experiment phase5 --enable-simd --enable-parallel --enable-memory-pools" << std::endl;
    }

    void ArgumentParser::printVersion() {
        std::cout << "NLM Phase 2 - Version 0.1.0" << std::endl;
        std::cout << "Neural Learning Machine - Real Neural Computation Phase" << std::endl;
        std::cout << "Build: Oct 15, 2026" << std::endl;
    }

    bool ArgumentParser::validateOptions(const CommandLineOptions& options) {
        if (options.neuronCount == 0) {
            std::cerr << "Error: Neuron count must be greater than 0" << std::endl;
            return false;
        }
        
        if (options.maxSteps == 0) {
            std::cerr << "Error: Max steps must be greater than 0" << std::endl;
            return false;
        }
        
        if (options.connectionProbability < 0.0f || options.connectionProbability > 1.0f) {
            std::cerr << "Error: Connection probability must be between 0.0 and 1.0" << std::endl;
            return false;
        }
        
        return true;
    }
}