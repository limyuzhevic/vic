// CLI framework help text
const std::string CLI_HELP = 
    "\nNLM Command Line Interface\n\n"
    "Usage: nlm [options] [command] [args...]\n\n"
    "Commands:\n"
    "  demo      Run a demo mode\n"
    "  benchmark  Run performance benchmarks\n"
    "  script     Run batch processing script\n"
    "  test       Run system tests\n"
    "  validate   Validate configuration files\n\n"
    "General Options:\n"
    "  --help, -h           Show this help message\n"
    "  --version            Show version information\n"
    "  --config <file>      Load configuration from file\n"
    "  --log-level <level>  Set logging level (Trace, Debug, Info, Warning, Error, Critical, Off)\n"
    "  --log-file <file>    Log output to file\n"
    "  --quiet, -q          Quiet mode (suppress output)\n"
    "  --verbose, -v        Verbose mode (show detailed output)\n"
    "  --dry-run            Don't run commands, just show what would be executed\n\n"
    "Demo Options:\n"
    "  --demo-type <type>   Demo type: phase3, phase4, phase6\n"
    "  --episodes <n>       Number of episodes (default: 5)\n"
    "  --steps <n>          Steps per episode (default: 200)\n\n"
    "Benchmark Options:\n"
    "  --benchmark-config <neurons,steps> Specify benchmark configuration\n"
    "  --benchmark-output <file>    Save benchmark results to file\n"
    "  --detailed           Run detailed benchmark analysis\n\n"
    "Examples:\n"
    "  nlm                          Run Phase 6 demo\n"
    "  nlm demo --demo-type phase3  Run Phase 3 demo\n"
    "  nlm --config myconfig.cfg validate  Validate config\n"
    "  nlm --log-level debug demo      Run with debug logging\n";

// CLI error prefix
const std::string CLI_ERROR_PREFIX = "Error: ";

// CLI version
const std::string CLI_VERSION = "NLM Phase 6 Final Integration - v0.1.0";

// Check if string starts with prefix
inline bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

// Extract value from --key=value format
inline std::string extractValue(const std::string& arg) {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        return arg.substr(pos + 1);
    }
    return "";
}

// Convert string to lowercase
inline std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

// Parse benchmark configuration string
inline void parseBenchmarkConfig(const std::string& value, std::vector<std::pair<size_t, size_t>>& configs) {
    std::stringstream ss(value);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        std::stringstream itemStream(item);
        size_t neurons, steps;
        char comma;
        if (itemStream >> neurons >> comma >> steps) {
            configs.push_back({neurons, steps});
        }
    }
}
