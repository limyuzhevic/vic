// NLM (熙然) - Neural Learning Machine CLI Parser Implementation
//
// This file implements the CLI parser using a simple but effective approach
// for argument parsing without external dependencies.

#include "cli/CLIParser.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace nlm {
namespace cli {

CLIParser::CLIParser() : 
    currentCommand(CommandType::Run),
    verbosity(VerbosityLevel::Info),
    helpRequested(false),
    versionRequested(false),
    listPresetsRequested(false) {
}

bool CLIParser::parse(int argc, char** argv) {
    if (argc < 2) {
        // No arguments provided, show basic help
        helpRequested = true;
        return true;
    }
    
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    auto it = args.begin();
    
    // Parse common options first
    while (it != args.end()) {
        const std::string& arg = *it;
        
        // Check for common options
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
            ++it;
            continue;
        }
        else if (arg == "--version" || arg == "-v") {
            versionRequested = true;
            ++it;
            continue;
        }
        else if (arg == "--list-presets") {
            listPresetsRequested = true;
            ++it;
            continue;
        }
        else if (arg.find("--verbose=") == 0) {
            verbosity = parseVerbosity(arg.substr(11));
            ++it;
            continue;
        }
        else if (arg == "--verbose" && it + 1 != args.end()) {
            verbosity = parseVerbosity(*(++it));
            ++it;
            continue;
        }
        else if (arg.find("--config=") == 0) {
            configFile = arg.substr(9);
            ++it;
            continue;
        }
        else if (arg == "--config" && it + 1 != args.end()) {
            configFile = *++it;
            ++it;
            continue;
        }
        else if (arg.find("--output=") == 0) {
            outputFile = arg.substr(9);
            ++it;
            continue;
        }
        else if (arg == "--output" && it + 1 != args.end()) {
            outputFile = *++it;
            ++it;
            continue;
        }
        else if (arg.find("--log=") == 0) {
            logFile = arg.substr(6);
            ++it;
            continue;
        }
        else if (arg == "--log" && it + 1 != args.end()) {
            logFile = *++it;
            ++it;
            continue;
        }
        
        // Check for commands
        if (arg == "run") {
            currentCommand = CommandType::Run;
            ++it;
            break;
        }
        else if (arg == "configure") {
            currentCommand = CommandType::Configure;
            ++it;
            break;
        }
        else if (arg == "experiment") {
            currentCommand = CommandType::Experiment;
            ++it;
            break;
        }
        else if (arg == "analyze") {
            currentCommand = CommandType::Analyze;
            ++it;
            break;
        }
        else if (arg == "benchmark") {
            currentCommand = CommandType::Benchmark;
            ++it;
            break;
        }
        else if (arg == "help") {
            currentCommand = CommandType::Help;
            ++it;
            break;
        }
        else if (arg == "version") {
            currentCommand = CommandType::Version;
            ++it;
            break;
        }
        
        // Store as positional argument for now
        positionalArgs.push_back(arg);
        ++it;
    }
    
    // Parse remaining arguments based on command
    if (currentCommand != CommandType::Help && 
        currentCommand != CommandType::Version &&
        currentCommand != CommandType::ListPresets) {
        
        while (it != args.end()) {
            const std::string& arg = *it;
            
            // Check for --key=value format
            if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
                size_t equalsPos = arg.find("=");
                std::string key = arg.substr(2, equalsPos - 2);
                std::string value = arg.substr(equalsPos + 1);
                parsedOptions[key] = value;
                ++it;
            }
            // Check for --key value format
            else if (arg.find("--") == 0 && it + 1 != args.end() &&
                     (*(*(it + 1).base())).find("--") != 0) {
                std::string key = arg.substr(2);
                std::string value = *(++it);
                parsedOptions[key] = value;
                ++it;
            }
            else {
                positionalArgs.push_back(arg);
                ++it;
            }
        }
    }
    
    return true;
}

VerbosityLevel CLIParser::parseVerbosity(const std::string& levelStr) const {
    std::string level = levelStr;
    std::transform(level.begin(), level.end(), level.begin(), ::tolower);
    
    if (level == "quiet" || level == "q") return VerbosityLevel::Quiet;
    if (level == "error" || level == "e") return VerbosityLevel::Error;
    if (level == "warning" || level == "w") return VerbosityLevel::Warning;
    if (level == "info" || level == "i") return VerbosityLevel::Info;
    if (level == "debug" || level == "d") return VerbosityLevel::Debug;
    if (level == "trace" || level == "t") return VerbosityLevel::Trace;
    
    return VerbosityLevel::Info; // Default
}

void CLIParser::displayHelp() const {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                    NLM Command-Line Interface                  ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";
    
    std::cout << "USAGE: nlm [options] <command> [arguments]\n\n";
    
    std::cout << "COMMANDS:\n";
    std::cout << "  run           Run simulation\n";
    std::cout << "  configure     Manage configuration\n";
    std::cout << "  experiment    Run experiment\n";
    std::cout << "  analyze       Analyze results\n";
    std::cout << "  benchmark     Performance benchmarking\n\n";
    
    std::cout << "GLOBAL OPTIONS:\n";
    std::cout << "  --help, -h            Show help\n";
    std::cout << "  --version, -v         Show version\n";
    std::cout << "  --list-presets        List available presets\n";
    std::cout << "  --verbose=<level>     Verbosity level (quiet, error, warning, info, debug, trace)\n";
    std::cout << "  --config=<file>       Configuration file\n";
    std::cout << "  --output=<file>       Output file\n";
    std::cout << "  --log=<file>          Log file\n\n";
    
    std::cout << "PRESETS:\n";
    std::cout << "  basic          Simple neuron dynamics\n";
    std::cout << "  plasticity     Learning and adaptation\n";
    std::cout << "  development    Developmental stages\n";
    std::cout << "  cognitive      Full cognitive capabilities\n";
    std::cout << "  social         Social interaction simulation\n";
    std::cout << "  memory         Memory-intensive scenarios\n";
    std::cout << "  attention      Attention and focus simulation\n\n";
    
    std::cout << "Use 'nlm help <command>' for detailed help on a specific command.\n";
}

void CLIParser::displayVersion() const {
    std::cout << "NLM Version 2.0.0 (Phase 2: Real Neural Computation)\n";
    std::cout << "Build Date: " << __DATE__ << " " << __TIME__ << "\n";
    std::cout << "Architecture: C++20 with modern CLI interface\n";
}

void CLIParser::displayPresets() const {
    std::cout << "AVAILABLE PRESETS:\n\n";
    
    std::cout << "basic:\n";
    std::cout << "  Description: Simple neuron dynamics with basic connectivity\n";
    std::cout << "  Parameters: 500 neurons, single region, moderate plasticity\n\n";
    
    std::cout << "plasticity:\n";
    std::cout << "  Description: Learning and adaptation simulation\n";
    std::cout << "  Parameters: Enhanced STDP and Hebbian learning, high plasticity rates\n\n";
    
    std::cout << "development:\n";
    std::cout << "  Description: Developmental stages with synaptogenesis and pruning\n";
    std::cout << "  Parameters: Initial sparse connectivity, developmental plasticity\n\n";
    
    std::cout << "cognitive:\n";
    std::cout << "  Description: Full cognitive capabilities including memory and attention\n";
    std::cout << "  Parameters: Multiple brain regions, complex interactions\n\n";
    
    std::cout << "social:\n";
    std::cout << "  Description: Social interaction simulation with reward-based learning\n";
    std::cout << "  Parameters: Social reward modulation, behavioral learning\n\n";
    
    std::cout << "memory:\n";
    std::cout << "  Description: Memory-intensive scenarios with high synaptic stability\n";
    std::cout << "  Parameters: Strong memory consolidation, low forgetting rates\n\n";
    
    std::cout << "attention:\n";
    std::cout << "  Description: Attention and focus simulation with competitive dynamics\n";
    std::cout << "  Parameters: Attention competition, gain modulation\n";
}

} // namespace cli
} // namespace nlm
