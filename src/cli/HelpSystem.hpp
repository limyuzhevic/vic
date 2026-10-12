// NLM (熙然) - Neural Learning Machine CLI Help System
//
// This file implements the help system for the CLI, providing comprehensive
// documentation, examples, and command-specific help information.

#include "cli/HelpSystem.hpp"
#include <iomanip>

namespace nlm {
namespace cli {

HelpSystem::HelpSystem() {
    // Initialize help content
    commandDescriptions = {
        {CommandType::Run, "Run simulation with specified configuration and parameters"},
        {CommandType::Configure, "Manage configuration files and settings"},
        {CommandType::Experiment, "Run specialized neural network experiments"},
        {CommandType::Analyze, "Analyze simulation results and statistics"},
        {CommandType::Benchmark, "Perform performance benchmarking"},
        {CommandType::Help, "Show help information"},
        {CommandType::Version, "Show version information"},
        {CommandType::ListPresets, "List available configuration presets"},
        {CommandType::ShowConfig, "Display current configuration"},
        {CommandType::SaveConfig, "Save configuration to file"}
    };
    
    optionDescriptions = {
        {"verbose", "Verbosity level (quiet, error, warning, info, debug, trace)"},
        {"config", "Configuration file path"},
        {"output", "Output file path"},
        {"log", "Log file path"},
        {"type", "Simulation/experiment type"},
        {"steps", "Number of simulation steps"},
        {"neurons", "Number of neurons"},
        {"regions", "Number of brain regions"},
        {"preset", "Configuration preset name"},
        {"file", "File path for input/output"},
        {"format", "Output format (text, json, csv)"},
        {"level", "Verbosity or log level"},
        {"filter", "Filter output by category"}
    };
    
    presetDescriptions = {
        {"basic", "Simple neuron dynamics with basic connectivity (500 neurons, 1 region)"},
        {"plasticity", "Learning and adaptation with STDP and Hebbian plasticity (1000 neurons, 2 regions)"},
        {"development", "Developmental stages with synaptogenesis and pruning (800 neurons, 2 regions)"},
        {"cognitive", "Full cognitive capabilities including memory and attention (2000 neurons, 4 regions)"},
        {"social", "Social interaction simulation with reward-based learning (1500 neurons, 3 regions)"},
        {"memory", "Memory-intensive scenarios with high synaptic stability (3000 neurons, 2 regions)"},
        {"attention", "Attention and focus simulation with competitive dynamics (1200 neurons, 3 regions)"}
    };
}

void HelpSystem::displayFullHelp() const {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                    NLM Command-Line Interface                  ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";
    
    std::cout << "USAGE: nlm [options] <command> [arguments]\n\n";
    
    displayAvailableCommands();
    std::cout << "\n";
    
    displayAvailableOptions();
    std::cout << "\n";
    
    displayExamples();
    std::cout << "\n";
    
    std::cout << "For detailed help on a specific command, use: nlm help <command>\n";
    std::cout << "To see all available presets: nlm --list-presets\n";
}

void HelpSystem::displayCommandHelp(const std::string& command) const {
    CommandType cmdType = CommandHandler::parseCommandType(command);
    
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                   Command: " << std::left << std::setw(36) << command << "║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";
    
    std::cout << "Description: " << commandDescriptions.at(cmdType) << "\n\n";
    
    // Display command-specific options
    std::cout << "Command-specific options:\n";
    
    if (command == "run") {
        std::cout << "  --type <type>          Simulation type: basic, plasticity, stdp, development, cognitive, social, memory, attention\n";
        std::cout << "  --steps <n>            Number of simulation steps\n";
        std::cout << "  --neurons <n>          Number of neurons\n";
        std::cout << "  --regions <n>          Number of brain regions\n";
    }
    else if (command == "experiment") {
        std::cout << "  --type <type>          Experiment type: plasticity, development, cognitive, social, memory, attention\n";
        std::cout << "  --steps <n>            Number of experiment steps\n";
        std::cout << "  --neurons <n>          Number of neurons\n";
    }
    else if (command == "analyze") {
        std::cout << "  --format <format>      Output format: text, json, csv\n";
        std::cout << "  --filter <category>    Filter results by category\n";
        std::cout << "  --output <file>        Output file\n";
    }
    else if (command == "benchmark") {
        std::cout << "  --duration <sec>       Benchmark duration in seconds\n";
        std::cout << "  --steps <n>            Number of steps to benchmark\n";
        std::cout << "  --neurons <n>          Number of neurons\n";
    }
    else if (command == "configure") {
        std::cout << "  --show                Show current configuration\n";
        std::cout << "  --save <file>         Save configuration to file\n";
        std::cout << "  --reset               Reset to default configuration\n";
    }
    
    std::cout << "\n";
}

void HelpSystem::displayPresetInfo(const std::string& presetName) const {
    if (presetDescriptions.find(presetName) == presetDescriptions.end()) {
        std::cout << "Unknown preset: " << presetName << "\n";
        return;
    }
    
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                 Preset: " << std::left << std::setw(39) << presetName << "║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";
    
    std::cout << "Description: " << presetDescriptions.at(presetName) << "\n\n";
}

void HelpSystem::displayExamples() const {
    std::cout << "EXAMPLES:\n\n";
    
    std::cout << "1. Basic simulation:\n";
    std::cout << "   nlm run --type basic --steps 100\n\n";
    
    std::cout << "2. Plasticity experiment:\n";
    std::cout << "   nlm experiment --type plasticity --steps 1000 --neurons 1000\n\n";
    
    std::cout << "3. Development simulation:\n";
    std::cout << "   nlm run --type development --preset development\n\n";
    
    std::cout << "4. Cognitive experiment:\n";
    std::cout << "   nlm experiment --type cognitive --neurons 2000\n\n";
    
    std::cout << "5. Social interaction simulation:\n";
    std::cout << "   nlm run --type social --regions 3\n\n";
    
    std::cout << "6. Memory consolidation experiment:\n";
    std::cout << "   nlm experiment --type memory --steps 5000\n\n";
    
    std::cout << "7. Attention focus simulation:\n";
    std::cout << "   nlm run --type attention --steps 2000\n\n";
    
    std::cout << "8. Configuration management:\n";
    std::cout << "   nlm configure --show\n";
    std::cout << "   nlm configure --save my_config.json\n\n";
    
    std::cout << "9. Analysis of results:\n";
    std::cout << "   nlm analyze --format json --output results.json\n\n";
    
    std::cout << "10. Performance benchmarking:\n";
    std::cout << "    nlm benchmark --duration 60 --neurons 500\n";
}

void HelpSystem::displayAvailableCommands() const {
    std::cout << "AVAILABLE COMMANDS:\n\n";
    
    std::cout << "run           Run simulation with specified configuration\n";
    std::cout << "experiment    Run specialized neural network experiments\n";
    std::cout << "analyze       Analyze simulation results and statistics\n";
    std::cout << "benchmark     Performance benchmarking\n";
    std::cout << "configure     Manage configuration files and settings\n";
    std::cout << "help          Show help information\n";
    std::cout << "version       Show version information\n";
    std::cout << "list-presets  List available configuration presets\n";
    std::cout << "show-config   Display current configuration\n";
    std::cout << "save-config   Save configuration to file\n\n";
}

void HelpSystem::displayAvailableOptions() const {
    std::cout << "GLOBAL OPTIONS:\n\n";
    
    for (const auto& pair : optionDescriptions) {
        std::cout << "  --" << std::left << std::setw(12) << pair.first << "  " << pair.second << "\n";
    }
    
    std::cout << "\n";
    
    std::cout << "PRESETS:\n\n";
    
    for (const auto& pair : presetDescriptions) {
        std::cout << "  " << std::left << std::setw(15) << pair.first << "  " << pair.second << "\n";
    }
    
    std::cout << "\n";
}

} // namespace cli
} // namespace nlm
