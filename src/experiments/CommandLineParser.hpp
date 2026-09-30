// Command-line argument parser for Phase 6 integration demo
class CommandLineArgs {
public:
    std::string configFile;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    bool enableNeuromodulation;
    size_t neuronCount;
    size_t maxSteps;
    float connectionProbability;
    size_t regionCount;
    std::string checkpointPath;
    bool help;
    
    CommandLineArgs() 
        : configFile("configs/default.cfg")
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , enableNeuromodulation(true)
        , neuronCount(500)
        , maxSteps(2000)
        , connectionProbability(0.1f)
        , regionCount(1)
        , checkpointPath("./nlm_phase6_demo_checkpoint.bin")
        , help(false) {}
    
    bool parse(int argc, char* argv[]);
    void printHelp() const;
    bool validate() const;
};

bool CommandLineArgs::parse(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            help = true;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "ERROR: --config requires a file path" << std::endl;
                return false;
            }
        } else if (arg == "--checkpoint" || arg == "-ckpt") {
            enableCheckpointing = true;
            if (i + 1 < argc) {
                checkpointPath = argv[++i];
            }
        } else if (arg == "--no-checkpoint") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        } else if (arg == "--no-neuromodulation") {
            enableNeuromodulation = false;
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) {
                try {
                    neuronCount = static_cast<size_t>(std::stoull(argv[++i]));
                } catch (...) {
                    std::cerr << "ERROR: Invalid neuron count" << std::endl;
                    return false;
                }
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                try {
                    maxSteps = static_cast<size_t>(std::stoull(argv[++i]));
                } catch (...) {
                    std::cerr << "ERROR: Invalid step count" << std::endl;
                    return false;
                }
            }
        } else if (arg == "--connection-prob" || arg == "-cp") {
            if (i + 1 < argc) {
                try {
                    connectionProbability = std::stof(argv[++i]);
                    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
                        std::cerr << "ERROR: Connection probability must be between 0.0 and 1.0" << std::endl;
                        return false;
                    }
                } catch (...) {
                    std::cerr << "ERROR: Invalid connection probability" << std::endl;
                    return false;
                }
            }
        } else if (arg == "--regions" || arg == "-r") {
            if (i + 1 < argc) {
                try {
                    regionCount = static_cast<size_t>(std::stoull(argv[++i]));
                } catch (...) {
                    std::cerr << "ERROR: Invalid region count" << std::endl;
                    return false;
                }
            }
        } else if (arg == "--help" || arg == "-h") {
            help = true;
        } else {
            std::cerr << "WARNING: Unknown argument: " << arg << std::endl;
        }
    }
    
    return true;
}

void CommandLineArgs::printHelp() const {
    std::cout << "=== NLM Phase 6 Demo - Usage ===" << std::endl << std::endl;
    std::cout << "Description: Run the Phase 6 integration test for the NLM artificial brain." << std::endl << std::endl;
    
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                    Show this help message and exit" << std::endl;
    std::cout << "  -c, --config FILE            Load configuration from FILE (default: configs/default.cfg)" << std::endl;
    std::cout << "  -n, --neurons N              Set neuron count (default: 500)" << std::endl;
    std::cout << "  -s, --steps S                Set maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  -cp, --connection-prob P     Set connection probability (0.0-1.0, default: 0.1)" << std::endl;
    std::cout << "  -r, --regions R              Set region count (default: 1)" << std::endl;
    std::cout << "  --checkpoint [PATH]           Enable checkpointing (default: ./nlm_phase6_demo_checkpoint.bin)" << std::endl;
    std::cout << "  --no-checkpoint              Disable checkpointing" << std::endl;
    std::cout << "  --no-replay                  Disable replay system" << std::endl;
    std::cout << "  --no-development              Disable development system" << std::endl;
    std::cout << "  --no-neuromodulation          Disable neuromodulation system" << std::endl;
    std::endl << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_phase6_demo                           # Run with default settings" << std::endl;
    std::cout << "  nlm_phase6_demo --config myconfig.cfg      # Use custom config file" << std::endl;
    std::cout << "  nlm_phase6_demo --neurons 1000 --steps 5000  # Run with larger brain" << std::endl;
    std::cout << "  nlm_phase6_demo --no-checkpoint --no-replay # Run without checkpointing/replay" << std::endl;
}

bool CommandLineArgs::validate() const {
    if (neuronCount == 0) {
        std::cerr << "ERROR: Neuron count must be positive" << std::endl;
        return false;
    }
    if (maxSteps == 0) {
        std::cerr << "ERROR: Step count must be positive" << std::endl;
        return false;
    }
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        std::cerr << "ERROR: Connection probability must be between 0.0 and 1.0" << std::endl;
        return false;
    }
    if (regionCount == 0) {
        std::cerr << "ERROR: Region count must be positive" << std::endl;
        return false;
    }
    return true;
}
