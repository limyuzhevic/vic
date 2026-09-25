#include "advanced/AdvancedCommands.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>

namespace nlm {

AdvancedCommandArgs AdvancedCommands::parseAdvancedArgs(int argc, char** argv) {
    AdvancedCommandArgs args;
    args.debugLevel = 0;
    args.profileDuration = 0.0;
    args.monitor = false;
    args.help = false;
    args.version = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--batch-run" && i + 1 < argc) {
            args.batchFile = argv[++i];
        }
        else if (arg == "--export-model" && i + 1 < argc) {
            args.exportModel = argv[++i];
        }
        else if (arg == "--import-model" && i + 1 < argc) {
            args.importModel = argv[++i];
        }
        else if (arg == "--benchmark" && i + 1 < argc) {
            args.benchmarkMode = argv[++i];
        }
        else if (arg == "--compare" && i + 1 < argc) {
            if (args.compareConfigs.empty()) {
                args.compareConfigs = argv[++i];
            } else {
                args.compareConfigs += "," + std::string(argv[++i]);
            }
        }
        else if (arg == "--profile" && i + 1 < argc) {
            args.profileDuration = std::stod(argv[++i]);
        }
        else if (arg == "--debug-level" && i + 1 < argc) {
            args.debugLevel = std::stoi(argv[++i]);
        }
        else if (arg == "--monitor") {
            args.monitor = true;
        }
        else if (arg == "--help" || arg == "-h") {
            args.help = true;
        }
        else if (arg == "--version" || arg == "-v") {
            args.version = true;
        }
        else if (arg.substr(0, 2) == "--") {
            args.customArgs.push_back(arg);
        }
    }
    
    return args;
}

void AdvancedCommands::printAdvancedHelp() {
    std::cout << R"(
════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
║                               ADVANCED NLM COMMAND LINE INTERFACE                              ║
╠════════════════════════════════════════════════════════════════════════════════════════════════════════╣
║
║  CORE FUNCTIONALITY:
║    NLM (熙然) Neural Learning Machine - Phase 2: Real Neural Computation
║    Advanced interface for spiking neural networks with comprehensive testing and analysis tools
║
║  BASIC COMMANDS:
║    --help, -h                       Show this comprehensive help information
║    --version, -v                     Display version and build information
║
║  EXECUTION MODES:
║    Standard Execution (default)     Run complete test suite with basic connectivity, plasticity,
║                                    and STDP verification tests
║    --monitor                        Enable real-time monitoring interface for live network observation
║    --batch-run <file>               Process multiple simulation configurations from a batch file
║    --profile <duration>             Run performance profiling for specified duration (seconds)
║    --benchmark <mode>               Execute performance benchmarks (neurons, memory, timing)
║    --compare <config1> <config2>    Compare two configurations side-by-side
║
║  MODEL MANAGEMENT:
║    --export-model <filename>        Export complete brain state to JSON file
║    --import-model <filename>        Import brain state from JSON file
║
║  VISUALIZATION COMMANDS:
║    --visualize <type>               Generate visual outputs:
║                                    connectivity, spiking, plasticity, network, heatmap
║
║  ADVANCED CONFIGURATION:
║    Network Architecture:
║      --neuron_count <n>            Set number of neurons (default: 500)
║      --region_count <n>            Set number of brain regions (default: 1)
║      --connection_probability <p>  Set connection probability (default: 0.15)
║
║    Neural Parameters:
║      --lif_tau <tau>               Membrane time constant (default: 10.0)
║      --lif_v_rest <v>              Resting potential (default: -65.0)
║      --lif_v_thresh <v>            Threshold potential (default: -50.0)
║      --lif_v_reset <v>             Reset potential (default: -65.0)
║      --lif_c_m <c>                 Membrane capacitance (default: 1.0)
║
║    Synaptic Properties:
║      --synapse_max_weight <w>       Maximum synaptic weight (default: 1.0)
║      --synapse_delay <d>            Synaptic delay in ms (default: 5.0)
║      --synapse_gaba_enabled <b>     Enable GABA inhibition (default: true)
║
║    Plasticity Rules:
║      --stdp_ltp_weight <w>          LTP weight factor (default: 0.02)
║      --stdp_ltd_weight <w>          LTD weight factor (default: 0.015)
║      --stdp_tau <tau>               STDP time constant (default: 20.0)
║      --hebbian_rate <r>             Hebbian learning rate (default: 0.01)
║
║    Development Dynamics:
║      --synaptogenesis_rate <r>      Rate of new synapse formation (default: 0.0001)
║      --pruning_rate <r>             Rate of synapse elimination (default: 0.00001)
║
║    Performance Optimization:
║      --num_threads <n>              Number of threads for parallel processing
║      --memory_pool_size <size>      Memory pool size in MB
║      --checkpoint_interval <i>      Checkpoint save interval (steps)
║
║  OUTPUT OPTIONS:
║      --log_level <level>            Set log level: DEBUG, INFO, WARNING, ERROR
║      --log_to_file <file>            Write log output to specified file
║      --output_stats <file>           Save simulation statistics to file
║      --verbose                       Enable verbose output for detailed monitoring
║
╠════════════════════════════════════════════════════════════════════════════════════════════════════════╣
║
║  USAGE EXAMPLES:
║
║  Example 1: Basic simulation with advanced monitoring:
║    ./nlm --neuron_count 1000 --region_count 2 --monitor
║
║  Example 2: Batch processing multiple configurations:
║    ./nlm --batch-run batch_configs.txt
║
║    batch_configs.txt format:
║      configs/config_a.cfg
║      configs/config_b.cfg
║      configs/config_c.cfg
║
║  Example 3: Performance profiling:
║    ./nlm --profile 60 --neuron_count 2000
║
║  Example 4: Model export/import:
║    ./nlm --export-model brain_state.json
║    ./nlm --import-model brain_state.json
║
║  Example 5: Simulation comparison:
║    ./nlm --compare configs/normal.cfg configs/stimulated.cfg
║
║  Example 6: Visualization output:
║    ./nlm --visualize connectivity --output_stats connectivity_stats.txt
║
║  Example 7: Advanced configuration with custom parameters:
║    ./nlm --neuron_count 800 --connection_probability 0.2 \
║           --stdp_ltp_weight 0.03 --stdp_ltd_weight 0.02 \
║           --synaptogenesis_rate 0.001 --pruning_rate 0.0001
║
╠════════════════════════════════════════════════════════════════════════════════════════════════════════╣
║
║  VISUALIZATION TYPES:
║    connectivity                   Create neuron connectivity matrix visualization
║    spiking                         Generate spiking activity over time plots
║    plasticity                      Show synaptic weight evolution during learning
║    network                        Display brain region topology and connectivity
║    heatmap                         Create activity heatmap for neural regions
║
║  PERFORMANCE BENCHMARKS:
║    neurons                         Benchmark neuron creation and initialization speed
║    memory                         Test memory usage patterns and efficiency
║    timing                          Measure overall simulation performance
║
║  FILE FORMATS:
║    Configuration files: YAML format with parameters in standard NLM format
║    Model export files: JSON format containing brain state, neurons, and synapses
║    Batch files: Plain text with one configuration path per line
║
║  ERROR HANDLING:
║    All errors are logged with detailed diagnostic information
║    Invalid arguments display usage and exit with error code 1
║    Missing configuration files are reported and execution continues
║
╠════════════════════════════════════════════════════════════════════════════════════════════════════════╣
║
║  VERSION AND SUPPORT:
║    NLM (熙然) Neural Learning Machine v2.0
║    Phase 2: Real Neural Computation with Advanced Features
║    For support and documentation, visit the NLM project documentation
║
╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝


  Core Commands:
    --batch-run <config_list>      Run multiple simulations from config file list
    --export-model <filename>       Export current brain state to file
    --import-model <filename>       Import brain state from file
    --benchmark <mode>              Run performance benchmark (neurons, memory, timing)
    --compare <config1> <config2>    Compare two configurations side-by-side
    --profile <duration>             Profile simulation for given duration (seconds)
    --debug-level <level>            Set debug level (0-3)
    --monitor                        Enable real-time monitoring interface

  Advanced Configuration Options:
    --neuron_count <n>               Number of neurons (default: 1000)
    --region_count <n>               Number of brain regions (default: 2)
    --connection_probability <p>     Connection probability (default: 0.1)
    --simulation_timestep <dt>       Simulation timestep (default: 0.001)

    Neural Parameters:
      --lif_tau <tau>                 Membrane time constant
      --lif_v_rest <v>                Resting potential
      --lif_v_thresh <v>              Threshold potential
      --lif_v_reset <v>               Reset potential
      --lif_c_m <c>                   Membrane capacitance
      
    Synaptic Properties:
      --synapse_max_weight <w>         Maximum synaptic weight
      --synapse_delay <d>              Synaptic delay (ms)
      --synapse_gaba_enabled <b>       Enable GABA inhibition
      
    Plasticity Settings:
      --stdp_ltp_weight <w>            LTP weight (default: 0.01)
      --stdp_ltd_weight <w>            LTD weight (default: 0.012)
      --stdp_tau <tau>                 STDP time constant (default: 20.0)
      --hebbian_rate <r>               Hebbian learning rate (default: 0.01)
      
    Development Settings:
      --synaptogenesis_rate <r>        Rate of new synapse formation
      --pruning_rate <r>               Rate of synapse elimination
      
    Performance Options:
      --num_threads <n>                Number of threads for parallel processing
      --memory_pool_size <size>        Memory pool size in MB
      --checkpoint_interval <i>        Checkpoint save interval
    
    Output Options:
      --log_level <level>              Log level (DEBUG, INFO, WARNING, ERROR)
      --log_to_file <file>              Log to file
      --output_stats <file>             Save statistics to file
      --verbose                         Enable verbose output
    )" << std::endl;
}

void AdvancedCommands::printVersionInfo() {
    std::cout << "NLM (熙然) Neural Learning Machine v2.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation with Advanced Features" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Advanced Features: Batch processing, Performance monitoring, Model export/import" << std::endl;
    std::cout << "              Real-time monitoring, Simulation comparison, Visualization capabilities" << std::endl;
    std::cout << "              Comprehensive debugging and analysis tools" << std::endl;
}

bool AdvancedCommands::handleAdvancedCommands(const AdvancedCommandArgs& args) {
    if (args.help) {
        printAdvancedHelp();
        return true;
    }
    
    if (args.version) {
        printVersionInfo();
        return true;
    }
    
    // Handle other advanced commands
    if (!args.batchFile.empty()) {
        // Batch processing would be handled by the main application
        std::cout << "Batch processing: " << args.batchFile << std::endl;
    }
    
    if (!args.exportModel.empty()) {
        std::cout << "Export model: " << args.exportModel << std::endl;
    }
    
    if (!args.importModel.empty()) {
        std::cout << "Import model: " << args.importModel << std::endl;
    }
    
    if (!args.benchmarkMode.empty()) {
        std::cout << "Benchmark mode: " << args.benchmarkMode << std::endl;
    }
    
    if (!args.compareConfigs.empty()) {
        std::cout << "Compare configs: " << args.compareConfigs << std::endl;
    }
    
    if (args.profileDuration > 0) {
        std::cout << "Profile duration: " << args.profileDuration << " seconds" << std::endl;
    }
    
    if (args.debugLevel > 0) {
        std::cout << "Debug level: " << args.debugLevel << std::endl;
    }
    
    if (args.monitor) {
        std::cout << "Monitoring enabled" << std::endl;
    }
    
    return false; // Continue with normal execution
}

std::string AdvancedCommands::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string AdvancedCommands::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace nlm