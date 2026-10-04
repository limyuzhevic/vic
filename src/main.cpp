// Progress bar implementation
class ProgressBar {
private:
    size_t total;
    size_t current;
    std::string label;
    bool enabled;
    
public:
    ProgressBar(size_t total, const std::string& label = "Progress", bool enabled = true)
        : total(total), current(0), label(label), enabled(enabled) {}
    
    void update(size_t value) {
        if (!enabled) return;
        current = value;
        float progress = (float)current / total;
        int barWidth = 50;
        
        std::cout << "\r" << Color::CYAN << label << ": [" << Color::RESET;
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << Color::GREEN << "=";
            else if (i == pos) std::cout << Color::YELLOW << ">";
            else std::cout << Color::DIM << " ";
        }
        std::cout << Color::CYAN << "] " << Color::RESET;
        std::cout << std::fixed << std::setprecision(1) << (progress * 100.0) << "%";
        std::cout.flush();
        
        if (current >= total) {
            std::cout << "\r" << Color::GREEN << label << ": [" << Color::RESET;
            for (int i = 0; i < barWidth; ++i) std::cout << Color::GREEN << "=";
            std::cout << Color::CYAN << "] 100.0%" << Color::RESET << std::endl;
        }
    }
    
    void finish() {
        if (!enabled) return;
        update(total);
    }
};

// Signal handler for graceful error handling
void signalHandler(int signal) {
    std::cout << "\n" << Color::YELLOW << "[!] Received signal " << signal << ". Shutting down gracefully..." << Color::RESET << std::endl;
    g_errorExit = true;
}

// Comprehensive error handling utility
class ErrorHandler {
public:
    static void logError(const std::string& context, const std::string& message, bool fatal = false) {
        std::cerr << Color::RED << "[ERROR]" << Color::RESET << " " << context << ": " << message << std::endl;
        if (fatal) {
            std::cerr << Color::YELLOW << "[!] Fatal error. Shutting down..." << Color::RESET << std::endl;
            std::exit(1);
        }
    }
    
    static void logWarning(const std::string& context, const std::string& message) {
        std::cerr << Color::YELLOW << "[WARNING]" << Color::RESET << " " << context << ": " << message << std::endl;
    }
    
    static void logInfo(const std::string& context, const std::string& message) {
        std::cout << Color::BLUE << "[INFO]" << Color::RESET << " " << context << ": " << message << std::endl;
    }
    
    static void logSuccess(const std::string& context, const std::string& message) {
        std::cout << Color::GREEN << "[SUCCESS]" << Color::RESET << " " << context << ": " << message << std::endl;
    }
    
    static void logProgress(const std::string& context, const std::string& message) {
        std::cout << Color::CYAN << "[PROGRESS]" << Color::RESET << " " << context << ": " << message << std::endl;
    }
};

// Enhanced Help System with command categorization and comprehensive documentation
class HelpSystem {
public:
    struct CommandInfo {
        std::string name;
        std::string shortDescription;
        std::string description;
        std::vector<std::string> aliases;
        std::string category;
        std::string examples;
        std::map<std::string, std::string> options;
        std::string notes;
    };

private:
    std::vector<CommandInfo> commands;
    bool interactiveMode;
    std::string searchTerm;
    
    void initializeCommands() {
        // Basic Operations Group
        commands.push_back({
            "help", 
            "Show help and usage information",
            "Display comprehensive help documentation with command categorization, usage examples, and configuration details.",
            {"-h", "--help"},
            "Basic Operations",
            "nlm --help\nnlm -h\nnlm --help --category basic",
            {},
            "Use '--category <name>' to filter commands by category."
        });
        
        commands.push_back({
            "version", 
            "Display version information",
            "Show NLM version and build information.",
            {"-v", "--version"},
            "Basic Operations",
            "nlm --version",
            {},
            "No flags required."
        });
        
        commands.push_back({
            "quiet", 
            "Suppress non-essential output",
            "Enable quiet mode to suppress progress indicators and informational messages.",
            {"-q", "--quiet"},
            "Basic Operations",
            "nlm --quiet --demo-mode",
            {},
            "Can be combined with any command to reduce output."
        });
        
        // Phase Selection Group
        commands.push_back({
            "demo-mode", 
            "Run Phase 6 integration demo",
            "Run a comprehensive demonstration of all Phase 6 integration features.",
            {"--demo-mode"},
            "Demo Mode",
            "nlm --demo-mode\nnlm --demo-mode --iterations 3000",
            {{"--iterations", "Number of simulation steps (default: 1000)"}},
            "This runs all integration tests and shows the complete brain functionality."
        });
        
        commands.push_back({
            "phase2", 
            "Run original Phase 2 tests (backward compatibility)",
            "Run the original Phase 2 test suite for backward compatibility.",
            {"--phase2"},
            "Demo Mode",
            "nlm --phase2\nnlm --phase2 --config configs/phase2.cfg",
            {{"--config", "Configuration file path (default: configs/phase2.cfg)"}},
            "Phase 2 maintains the original neural learning machine interface."
        });
        
        commands.push_back({
            "phase6", 
            "Run Phase 6 integration tests",
            "Run the comprehensive Phase 6 integration test suite with all memory systems.",
            {"--phase6"},
            "Demo Mode",
            "nlm --phase6\nnlm --phase6 --save brain_state.bin",
            {{"--save", "Save brain state to file after completion"}},
            "Phase 6 tests integration between all brain systems (memory, neuromodulation, prediction)."
        });
        
        // Configuration Group
        commands.push_back({
            "config", 
            "Configuration file loading",
            "Load configuration from JSON or YAML file with precedence: CLI > Config file > Defaults.",
            {"--config"},
            "Configuration",
            "nlm --config config.json\nnlm --config=custom_config.yaml",
            {{"file", "Configuration file path (JSON/YAML format)"}},
            "File format: JSON uses {.json}, YAML uses {--- or key:}."
        });
        
        commands.push_back({
            "neuron-count", 
            "Override neuron count",
            "Set the number of neurons in the brain (range: 10-10000000).",
            {"--neuron-count"},
            "Configuration",
            "nlm --phase6 --neuron-count 2000",
            {{"N", "Number of neurons (recommended: 500-5000)"}},
            "Use environment variable NLM_NEURON_COUNT as default."
        });
        
        commands.push_back({
            "region-count", 
            "Override region count",
            "Set the number of brain regions (range: 1-100).",
            {"--region-count"},
            "Configuration",
            "nlm --phase6 --neuron-count 2000 --region-count 2",
            {{"N", "Number of regions (recommended: 1-10)"}},
            "More regions enable better modularization but increase memory usage."
        });
        
        // File Operations Group
        commands.push_back({
            "save", 
            "Save brain state to file",
            "Save the current brain state to a binary file for checkpointing and resuming.",
            {"--save"},
            "File Operations",
            "nlm --phase6 --save my_brain.bin\nnlm --phase6 --save brain_state.bin",
            {{"file", "Output filename (recommended: .bin extension)"}},
            "Can be combined with --load to load and save different states."
        });
        
        commands.push_back({
            "load", 
            "Load brain state from file",
            "Load a saved brain state from file to resume previous sessions.",
            {"--load"},
            "File Operations",
            "nlm --phase6 --load saved_brain.bin",
            {{"file", "Input filename"}},
            "File format: Binary .bin files (architecture-specific)."
        });
        
        commands.push_back({
            "checkpoint", 
            "Perform checkpoint operations",
            "Create periodic checkpoints during long-running simulations for recovery.",
            {"--checkpoint"},
            "File Operations",
            "nlm --phase6 --checkpoint --save brain_checkpoint.bin",
            {{}},
            "Automatically saves every 100 iterations when combined with --save."
        });
        
        // Advanced Options Group
        commands.push_back({
            "iterations", 
            "Number of simulation steps",
            "Set total number of simulation steps to run (default: 1000).",
            {"--iterations"},
            "Advanced Options",
            "nlm --demo-mode --iterations 3000",
            {{"N", "Number of simulation steps (default: 1000)"}},
            "Progress bar will show completion status. Use with --quiet to suppress progress."
        });
        
        commands.push_back({
            "verbose", 
            "Enable verbose logging",
            "Enable detailed logging for debugging and monitoring.",
            {"--verbose"},
            "Advanced Options",
            "nlm --phase6 --verbose",
            {},
            "Shows detailed information about neural activity and learning."
        });
        
        commands.push_back({
            "interactive", 
            "Interactive help mode",
            "Enter interactive help mode with searchable commands and real-time guidance.",
            {"--interactive"},
            "Advanced Options",
            "nlm --interactive",
            {},
            "Type 'help <command>' for detailed command information or 'search <term>' to find related commands."
        });
    }
    
public:
    HelpSystem() : interactiveMode(false), searchTerm("") {
        initializeCommands();
    }
    
    void setInteractive(bool enabled) {
        interactiveMode = enabled;
        if (enabled) {
            showInteractiveHelp();
        }
    }
    
    void setSearchTerm(const std::string& term) {
        searchTerm = term;
    }
    
    void showHelp(const std::string& category = "", bool interactive = false) {
        if (interactive) {
            showInteractiveHelp();
            return;
        }
        
        std::cout << Color::BOLD << Color::CYAN << "\n=== Enhanced NLM Command-Line Interface ===\n" << Color::RESET;
        std::cout << Color::BOLD << Color::GREEN << "NLM - Neural Learning Machine (熙然)\n" << Color::RESET;
        std::cout << Color::DIM << "Phase 2 & 6: Real Neural Computation with Integration Tests\n" << Color::RESET;
        
        if (!category.empty()) {
            std::cout << Color::BOLD << Color::YELLOW << "Category: " << category << Color::RESET << std::endl;
            showCategoryHelp(category);
        } else {
            showAllCategories();
        }
        
        showConfigurationInfo();
        showEnvironmentVariables();
        showErrorGuide();
        showUsageExamples();
        
        std::cout << Color::BOLD << "\n=== Quick Reference ===\n" << Color::RESET;
        std::cout << Color::GREEN << "For interactive help: nlm --interactive\n" << Color::RESET;
        std::cout << Color::GREEN << "For contextual help: help <command> in interactive mode\n" << Color::RESET;
        std::cout << Color::GREEN << "For error recovery: check nlm docs/troubleshooting.md\n" << Color::RESET;
        
        std::cout << std::endl;
    }
    
    void showCategoryHelp(const std::string& category) {
        std::cout << Color::BOLD << "=== " << category << " Commands ===\n" << Color::RESET;
        
        for (const auto& cmd : commands) {
            if (cmd.category == category) {
                std::cout << Color::GREEN << "  " << formatCommandName(cmd.name, cmd.aliases) << Color::RESET;
                std::cout << std::string(30 - cmd.name.length(), ' ');
                std::cout << cmd.shortDescription << std::endl;
            }
        }
        
        std::cout << std::endl;
    }
    
    void showAllCategories() {
        std::cout << Color::BOLD << "=== Command Categories ===\n" << Color::RESET;
        
        std::map<std::string, std::vector<CommandInfo>> categorized;
        for (const auto& cmd : commands) {
            categorized[cmd.category].push_back(cmd);
        }
        
        for (const auto& category : {"Basic Operations", "Demo Mode", "Configuration", "File Operations", "Advanced Options"}) {
            if (categorized.find(category) != categorized.end()) {
                std::cout << Color::CYAN << "  " << category << ":" << Color::RESET << std::endl;
                int count = 0;
                for (const auto& cmd : categorized[category]) {
                    count++;
                    std::cout << "    " << count << ". " << Color::GREEN << formatCommandName(cmd.name, cmd.aliases) << Color::RESET;
                    std::cout << std::string(35 - cmd.name.length(), ' ');
                    std::cout << cmd.shortDescription << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }
    
    void showInteractiveHelp() {
        std::cout << Color::BOLD << Color::CYAN << "\n=== Interactive Help Mode ===\n" << Color::RESET;
        std::cout << Color::GREEN << "Commands:\n" << Color::RESET;
        std::cout << "  help <command>           - Get detailed help for a specific command\n";
        std::cout << "  search <term>            - Search for commands or topics\n";
        std::cout << "  category <name>          - Show commands in a specific category\n";
        std::cout << "  list                      - List all commands\n";
        std::cout << "  exit                      - Exit interactive mode\n";
        std::cout << std::endl;
        
        if (!searchTerm.empty()) {
            std::cout << Color::YELLOW << "Search results for: " << searchTerm << Color::RESET << std::endl;
            searchCommands();
        }
    }
    
    void showConfigurationInfo() {
        std::cout << Color::BOLD << "\n=== Configuration File Format ===\n" << Color::RESET;
        std::cout << Color::GREEN << "JSON Example:" << Color::RESET << std::endl;
        std::cout << "{" << std::endl;
        std::cout << "  \"neuron_count\": 1000," << std::endl;
        std::cout << "  \"region_count\": 1," << std::endl;
        std::cout << "  \"simulation_timestep\": 0.001," << std::endl;
        std::cout << "  \"connection_probability\": 0.15" << std::endl;
        std::cout << "}" << std::endl;
        std::cout << std::endl;
        
        std::cout << Color::GREEN << "YAML Example:" << Color::RESET << std::endl;
        std::cout << "---" << std::endl;
        std::cout << "neuron_count: 1000" << std::endl;
        std::cout << "region_count: 1" << std::endl;
        std::cout << "simulation_timestep: 0.001" << std::endl;
        std::cout << "connection_probability: 0.15" << std::endl;
        std::cout << "..." << std::endl;
        std::cout << std::endl;
        
        std::cout << Color::GREEN << "Configuration Precedence:" << Color::RESET << std::endl;
        std::cout << "  1. Command Line Arguments (--key value)" << std::endl;
        std::cout << "  2. Configuration File (configs/*.cfg)" << std::endl;
        std::cout << "  3. Environment Variables (NLM_*)" << std::endl;
        std::cout << "  4. Default Values" << std::endl;
        std::cout << std::endl;
    }
    
    void showEnvironmentVariables() {
        std::cout << Color::BOLD << "=== Environment Variables ===\n" << Color::RESET;
        
        std::vector<std::pair<std::string, std::string>> envVars = {
            {"NLM_NEURON_COUNT", "Default neuron count"},
            {"NLM_REGION_COUNT", "Default region count"},
            {"NLM_SIMULATION_TIMESTEP", "Default simulation timestep"},
            {"NLM_RANDOM_SEED", "Random seed for reproducible results"},
            {"NLM_CONNECTION_PROBABILITY", "Default connection probability"},
            {"NLM_STDP_LTP_WEIGHT", "STDP LTP weight"},
            {"NLM_STDP_LTD_WEIGHT", "STDP LTD weight"},
            {"NLM_SYNAPTISSGENESIS_RATE", "Synaptogenesis rate"},
            {"NLM_PRUNING_RATE", "Pruning rate"}
        };
        
        for (const auto& var : envVars) {
            std::cout << "  " << Color::CYAN << var.first << Color::RESET << " - " << var.second << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    void showErrorGuide() {
        std::cout << Color::BOLD << "=== Error Messages Guide ===\n" << Color::RESET;
        
        std::vector<std::pair<std::string, std::string>> errorTypes = {
            {"Configuration validation failed!", "Check configuration file format and parameter ranges"},
            {"Failed to load configuration", "Verify file path and format (JSON/YAML)"},
            {"Invalid argument format", "Use --key value or --key=value format"},
            {"Command not found", "Use --help for available commands"},
            {"Phase selection conflict", "Use only one of --demo-mode, --phase2, --phase6"}
        };
        
        for (const auto& error : errorTypes) {
            std::cout << Color::RED << "  \"" << error.first << "\"" << Color::RESET << std::endl;
            std::cout << "    " << Color::DIM << error.second << Color::RESET << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    void showUsageExamples() {
        std::cout << Color::BOLD << "=== Usage Examples ===\n" << Color::RESET;
        
        std::vector<std::pair<std::string, std::string>> examples = {
            {"nlm --demo-mode", "Run comprehensive integration demo"},
            {"nlm --phase2", "Run original Phase 2 tests"},
            {"nlm --phase6 --save brain.bin", "Run Phase 6 and save brain state"},
            {"nlm --phase6 --neuron-count 2000 --region-count 2", "Run with custom brain size"},
            {"nlm --phase6 --config my_config.cfg", "Load custom configuration"},
            {"nlm --phase6 --iterations 5000", "Run with custom iteration count"},
            {"nlm --help --category configuration", "Show only configuration commands"},
            {"nlm --interactive", "Enter interactive help mode"}
        };
        
        for (const auto& example : examples) {
            std::cout << "  " << Color::YELLOW << example.first << Color::RESET << " - " << example.second << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    void searchCommands() {
        if (searchTerm.empty()) return;
        
        std::transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), ::tolower);
        
        for (const auto& cmd : commands) {
            if (cmd.name.find(searchTerm) != std::string::npos || 
                cmd.shortDescription.find(searchTerm) != std::string::npos) {
                std::cout << "  " << Color::GREEN << cmd.name << Color::RESET << ": " << cmd.shortDescription << std::endl;
            }
        }
    }
    
private:
    std::string formatCommandName(const std::string& name, const std::vector<std::string>& aliases) {
        std::string result = "--" + name;
        if (!aliases.empty()) {
            result += ", " + aliases[0];
        }
        return result;
    }
};
// Enhanced CLI argument parser with comprehensive validation and help integration
class EnhancedCLI {
public:
    EnhancedCLI(int argc, char** argv, HelpSystem& helpSystem);

    // Core parsing and validation methods
    bool parseArgs(int argc, char** argv);
    void printHelp(const std::string& category = );
    void printContextualHelp(const std::string& command);
    bool validateCommandCombos() const;
    bool validateArguments() const;
    void showProgress(size_t current, size_t total, const std::string& label) const;

    // Argument access methods
    bool hasFlag(const std::string& flag) const { return flags.find(flag) != flags.end(); }
    bool hasArg(const std::string& arg) const { return args.find(arg) != args.end(); }
    std::string getArg(const std::string& arg, const std::string& defaultValue = ) const {
        auto it = args.find(arg);
        return (it != args.end()) ? it->second : defaultValue;
    }
    int getArgInt(const std::string& arg, int defaultValue = 0) const {
        return parseInt(getArg(arg), defaultValue);
    }
    double getArgDouble(const std::string& arg, double defaultValue = 0.0) const {
        return parseDouble(getArg(arg), defaultValue);
    }
    float getArgFloat(const std::string& arg, float defaultValue = 0.0f) const {
        return parseFloat(getArg(arg), defaultValue);
    }
    bool getArgBool(const std::string& arg, bool defaultValue = false) const {
        return parseBool(getArg(arg), defaultValue);
    }
    const std::map<std::string, std::string>& getArgs() const { return args; }
    const std::map<std::string, std::string>& getFlags() const { return flags; }

private:
    std::map<std::string, std::string> args;
    std::map<std::string, std::string> flags;
    HelpSystem& helpSystemRef;
    std::vector<std::string> invalidCommands;
    std::map<std::string, std::string> parsedArgs;

    // Helper parsing functions
    std::string trim(const std::string& str) const {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
    int parseInt(const std::string& str, int defaultValue) const {
        if (str.empty()) return defaultValue;
        try { return std::stoi(str); } catch(...) { return defaultValue; }
    }
    double parseDouble(const std::string& str, double defaultValue) const {
        if (str.empty()) return defaultValue;
        try { return std::stod(str); } catch(...) { return defaultValue; }
    }
    float parseFloat(const std::string& str, float defaultValue) const {
        if (str.empty()) return defaultValue;
        try { return std::stof(str); } catch(...) { return defaultValue; }
    }
    bool parseBool(const std::string& str, bool defaultValue) const {
        if (str.empty()) return defaultValue;
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
    }

    // Enhanced argument parsing with comprehensive validation
    bool parseArgs(int argc, char** argv) {
        std::set<std::string> processedArgs;
        bool hasHelpFlag = false;
        bool hasInteractiveFlag = false;
        bool hasInvalidArgs = false;
        std::vector<std::string> demoModeFlags;
        std::vector<std::string> phaseFlags;

        // Phase detection and validation
        if (argv[1] && (std::string(argv[1]) == --phase2 || std::string(argv[1]) == --phase6 || std::string(argv[1]) == --demo-mode)) {
            if (phaseFlags.empty()) phaseFlags.push_back(std::string(argv[1]));
            else {
                ErrorHandler::logError(CLI, Cannot specify multiple phase modes. Use only one of --phase2, --phase6, --demo-mode., true);
                return false;
            }
        }

    // Continue with EnhancedCLI implementation...

