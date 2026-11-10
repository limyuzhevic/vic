class CLIParser {
public:
    struct ParsedArgs {
        std::string experimentMode;  // Phase 3,4,5,6
        std::string configPreset;   // quick-test, full-experiment, research
        std::string configFile;     // config file path
        bool verbose;               // verbose output
        bool benchmark;             // benchmark mode
        bool saveResults;           // save results to file
        std::string resultsFile;    // results file path
        bool progress;              // progress reporting
        std::string logLevel;       // DEBUG, INFO, WARNING, ERROR
        std::vector<std::string> extraArgs; // additional arguments
        bool showHelp;              // help flag
        bool showVersion;           // version flag
        int benchmarkRuns;          // number of benchmark runs
        double benchmarkTimeout;    // benchmark timeout in seconds
    };

    static ParsedArgs parseArgs(int argc, char** argv);
    static void printHelp();
    static void printVersion();
    static std::string getConfigPreset(const std::string& preset);
};

// Configuration preset loader
class ConfigPresets {
public:
    static void applyPreset(const std::string& preset, std::shared_ptr<Config> config);
    static bool hasPreset(const std::string& preset);
};

// Result serialization
class ResultSerializer {
public:
    static bool saveResults(const std::string& filepath, const std::string& experimentName, const std::string& results);
    static bool loadResults(const std::string& filepath, std::string& experimentName, std::string& results);
};