#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace nlm {

class CLICommandHandler {
public:
    CLICommandHandler(Config& config) : config(config) {
        initializeCommands();
    }
    
    void executeCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            printHelp();
            return;
        }
        
        std::string command = args[0];
        
        if (commandHandlers.find(command) != commandHandlers.end()) {
            commandHandlers[command](args);
        } else if (subcommandHandlers.find(command) != subcommandHandlers.end()) {
            subcommandHandlers[command](args);
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            printHelp();
        }
    }
    
    void printHelp() const {
        std::cout << "NLM - Neural Learning Machine Enhanced CLI" << std::endl;
        std::cout << "===========================================" << std::endl << std::endl;
        
        std::cout << "Available commands:" << std::endl;
        std::cout << "  config      - Configuration management commands" << std::endl;
        std::cout << "  brain       - Brain state management commands" << std::endl;
        std::cout << "  experiment  - Experiment management commands" << std::endl;
        std::cout << "  system      - System monitoring commands" << std::endl;
        std::cout << "  integration - Integration management commands" << std::endl;
        std::cout << "  visualize   - Visualization commands" << std::endl;
        std::cout << "  advanced    - Advanced features and automation" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm config show" << std::endl;
        std::cout << "  nlm brain dump brain_state.json" << std::endl;
        std::cout << "  nlm experiment run --name learning_exp --steps 100" << std::endl;
        std::cout << "  nlm system status" << std::endl;
        std::cout << "  nlm visualize start" << std::endl;
        std::cout << std::endl;
    }
    
private:
    Config& config;
    
    struct CommandHandler {
        std::string description;
        std::function<void(const std::vector<std::string>&)> handler;
    };
    
    std::map<std::string, CommandHandler> commandHandlers;
    std::map<std::string, CommandHandler> subcommandHandlers;
    
    void initializeCommands() {
        // Config commands
        commandHandlers["help"] = {"Show help information", [this](const std::vector<std::string>& args) { printHelp(); }};
        commandHandlers["version"] = {"Show version information", [this](const std::vector<std::string>& args) { printVersion(); }};
        
        // Brain commands
        commandHandlers["brain"] = {"Brain management commands", [this](const std::vector<std::string>& args) { handleBrainCommand(args); }};
        
        // Experiment commands  
        commandHandlers["experiment"] = {"Experiment management commands", [this](const std::vector<std::string>& args) { handleExperimentCommand(args); }};
        
        // System commands
        commandHandlers["system"] = {"System monitoring commands", [this](const std::vector<std::string>& args) { handleSystemCommand(args); }};
        
        // Integration commands
        commandHandlers["integration"] = {"Integration management commands", [this](const std::vector<std::string>& args) { handleIntegrationCommand(args); }};
        
        // Visualization commands
        commandHandlers["visualize"] = {"Visualization commands", [this](const std::vector<std::string>& args) { handleVisualizeCommand(args); }};
        
        // Advanced features
        commandHandlers["advanced"] = {"Advanced features commands", [this](const std::vector<std::string>& args) { handleAdvancedCommand(args); }};
    }
    
    // Config subcommand handlers
    void handleConfigCommand(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: nlm config <command> [options]" << std::endl;
            std::cout << "Commands:" << std::endl;
            std::cout << "  show      - Display current configuration" << std::endl;
            std::cout << "  get <key> - Get specific configuration value" << std::endl;
            std::cout << "  set <key> <value> - Set configuration value" << std::endl;
            std::cout << "  validate  - Validate configuration" << std::endl;
            std::cout << "  migrate   - Migrate from legacy format" << std::endl;
            std::cout << "  export <format> - Export configuration" << std::endl;
            std::cout << "  import <file> <format> - Import configuration" << std::endl;
            return;
        }
        
        std::string subcommand = args[1];
        
        if (subcommand == "show") {
            configShow();
        } else if (subcommand == "get" && args.size() >= 3) {
            configGet(args[2]);
        } else if (subcommand == "set" && args.size() >= 4) {
            configSet(args[2], args[3]);
        } else if (subcommand == "validate") {
            configValidate();
        } else if (subcommand == "migrate") {
            configMigrate();
        } else if (subcommand == "export" && args.size() >= 3) {
            configExport(args[2]);
        } else if (subcommand == "import" && args.size() >= 4) {
            configImport(args[2], args[3]);
        } else {
            std::cout << "Invalid command: nlm config " << subcommand << std::endl;
            printConfigHelp();
        }
    }
    
    // Config command implementations
    void configShow() {
        std::cout << config.summary() << std::endl;
    }
    
    void configGet(const std::string& key) {
        std::string value;
        if (config.get<std::string>(key, value).isSuccess()) {
            std::cout << key << " = " << value << std::endl;
        } else {
            std::cout << "Key not found: " << key << std::endl;
        }
    }
    
    void configSet(const std::string& key, const std::string& value) {
        ConfigResult result = config.set(key, value, ConfigSource::Runtime);
        if (result.isSuccess()) {
            std::cout << "Set " << key << " = " << value << std::endl;
        } else {
            std::cout << "Error setting " << key << ": " << result.getMessage() << std::endl;
        }
    }
    
    void configValidate() {
        ConfigResult result = config.validate();
        if (result.isSuccess()) {
            std::cout << "Configuration is valid" << std::endl;
        } else {
            std::cout << "Configuration validation failed: " << result.getMessage() << std::endl;
        }
    }
    
    void configMigrate() {
        std::cout << "Migrating from legacy format..." << std::endl;
        // Implementation would go here
        std::cout << "Migration completed (stub)" << std::endl;
    }
    
    void configExport(const std::string& format) {
        std::cout << "Exporting configuration in " << format << " format..." << std::endl;
        // Implementation would go here
        std::cout << "Export completed (stub)" << std::endl;
    }
    
    void configImport(const std::string& file, const std::string& format) {
        std::cout << "Importing configuration from " << file << " in " << format << " format..." << std::endl;
        // Implementation would go here
        std::cout << "Import completed (stub)" << std::endl;
    }
    
    void printConfigHelp() const {
        std::cout << "Config commands:" << std::endl;
        std::cout << "  nlm config show                    - Display current configuration" << std::endl;
        std::cout << "  nlm config get <key>              - Get specific configuration value" << std::endl;
        std::cout << "  nlm config set <key> <value>      - Set configuration value" << std::endl;
        std::cout << "  nlm config validate               - Validate configuration" << std::endl;
        std::cout << "  nlm config migrate                - Migrate from legacy format" << std::endl;
        std::cout << "  nlm config export <format>        - Export configuration" << std::endl;
        std::cout << "  nlm config import <file> <format> - Import configuration" << std::endl;
    }
    
    // Brain command handlers
    void handleBrainCommand(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: nlm brain <command> [options]" << std::endl;
            std::cout << "Commands:" << std::endl;
            std::cout << "  dump <file>         - Save brain state to file" << std::endl;
            std::cout << "  load <file>         - Load brain state from file" << std::endl;
            std::cout << "  save <checkpoint>    - Save brain checkpoint" << std::endl;
            std::cout << "  snapshot            - Take snapshot of current state" << std::endl;
            std::cout << "  compare <file1> <file2> - Compare brain states" << std::endl;
            std::cout << "  analyze             - Analyze brain structure and state" << std::endl;
            std::cout << "  stats               - Show brain statistics" << std::endl;
            return;
        }
        
        std::string subcommand = args[1];
        
        if (subcommand == "dump" && args.size() >= 3) {
            brainDump(args[2]);
        } else if (subcommand == "load" && args.size() >= 3) {
            brainLoad(args[2]);
        } else if (subcommand == "save" && args.size() >= 3) {
            brainSave(args[2]);
        } else if (subcommand == "snapshot") {
            brainSnapshot();
        } else if (subcommand == "compare" && args.size() >= 4) {
            brainCompare(args[2], args[3]);
        } else if (subcommand == "analyze") {
            brainAnalyze();
        } else if (subcommand == "stats") {
            brainStats();
        } else {
            std::cout << "Invalid command: nlm brain " << subcommand << std::endl;
            printBrainHelp();
        }
    }
    
    // Brain command implementations
    void brainDump(const std::string& file) {
        std::cout << "Dumping brain state to " << file << "..." << std::endl;
        // Implementation would go here
        std::cout << "Brain dump completed (stub)" << std::endl;
    }
    
    void brainLoad(const std::string& file) {
        std::cout << "Loading brain state from " << file << "..." << std::endl;
        // Implementation would go here
        std::cout << "Brain load completed (stub)" << std::endl;
    }
    
    void brainSave(const std::string& checkpoint) {
        std::cout << "Saving brain checkpoint " << checkpoint << "..." << std::endl;
        // Implementation would go here
        std::cout << "Brain save completed (stub)" << std::endl;
    }
    
    void brainSnapshot() {
        std::cout << "Taking brain snapshot..." << std::endl;
        // Implementation would go here
        std::cout << "Brain snapshot completed (stub)" << std::endl;
    }
    
    void brainCompare(const std::string& file1, const std::string& file2) {
        std::cout << "Comparing brain states " << file1 << " and " << file2 << "..." << std::endl;
        // Implementation would go here
        std::cout << "Brain compare completed (stub)" << std::endl;
    }
    
    void brainAnalyze() {
        std::cout << "Analyzing brain structure and state..." << std::endl;
        // Implementation would go here
        std::cout << "Brain analysis completed (stub)" << std::endl;
    }
    
    void brainStats() {
        std::cout << "Brain statistics:" << std::endl;
        // Implementation would go here
        std::cout << "  (stub)" << std::endl;
    }
    
    void printBrainHelp() const {
        std::cout << "Brain commands:" << std::endl;
        std::cout << "  nlm brain dump <file>                     - Save brain state to file" << std::endl;
        std::cout << "  nlm brain load <file>                     - Load brain state from file" << std::endl;
        std::cout << "  nlm brain save <checkpoint>                - Save brain checkpoint" << std::endl;
        std::cout << "  nlm brain snapshot                        - Take snapshot of current state" << std::endl;
        std::cout << "  nlm brain compare <file1> <file2>          - Compare brain states" << std::endl;
        std::cout << "  nlm brain analyze                         - Analyze brain structure and state" << std::endl;
        std::cout << "  nlm brain stats                           - Show brain statistics" << std::endl;
    }
    
    // Experiment command handlers
    void handleExperimentCommand(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: nlm experiment <command> [options]" << std::endl;
            std::cout << "Commands:" << std::endl;
            std::cout << "  run <name> [args...]                - Run specific experiment" << std::endl;
            std::cout << "  list                                 - List available experiments" << std::endl;
            std::cout << "  info <name>                         - Show experiment information" << std::endl;
            std::cout << "  configure <name>                    - Configure experiment parameters" << std::endl;
            std::cout << "  results                              - Show experiment results" << std::endl;
            std::cout << "  logs <name>                         - Show experiment logs" << std::endl;
            return;
        }
        
        std::string subcommand = args[1];
        
        if (subcommand == "run" && args.size() >= 3) {
            experimentRun(args[2], std::vector<std::string>(args.begin() + 3, args.end()));
        } else if (subcommand == "list") {
            experimentList();
        } else if (subcommand == "info" && args.size() >= 3) {
            experimentInfo(args[2]);
        } else if (subcommand == "configure" && args.size() >= 3) {
            experimentConfigure(args[2]);
        } else if (subcommand == "results") {
            experimentResults();
        } else if (subcommand == "logs" && args.size() >= 3) {
            experimentLogs(args[2]);
        } else {
            std::cout << "Invalid command: nlm experiment " << subcommand << std::endl;
            printExperimentHelp();
        }
    }
    
    // Experiment command implementations
    void experimentRun(const std::string& name, const std::vector<std::string>& args) {
        std::cout << "Running experiment: " << name << std::endl;
        for (const auto& arg : args) {
            std::cout << "  " << arg << std::endl;
        }
        std::cout << "Experiment execution completed (stub)" << std::endl;
    }
    
    void experimentList() {
        std::cout << "Available experiments:" << std::endl;
        // Implementation would go here
        std::cout << "  (stub)" << std::endl;
    }
    
    void experimentInfo(const std::string& name) {
        std::cout << "Experiment info for: " << name << std::endl;
        // Implementation would go here
        std::cout << "Experiment info completed (stub)" << std::endl;
    }
    
    void experimentConfigure(const std::string& name) {
        std::cout << "Configuring experiment: " << name << std::endl;
        // Implementation would go here
        std::cout << "Experiment configuration completed (stub)" << std::endl;
    }
    
    void experimentResults() {
        std::cout << "Experiment results:" << std::endl;
        // Implementation would go here
        std::cout << "  (stub)" << std::endl;
    }
    
    void experimentLogs(const std::string& name) {
        std::cout << "Experiment logs for: " << name << std::endl;
        // Implementation would go here
        std::cout << "Experiment logs completed (stub)" << std::endl;
    }
    
    void printExperimentHelp() const {
        std::cout << "Experiment commands:" << std::endl;
        std::cout << "  nlm experiment run <name> [args...]     - Run specific experiment" << std::endl;
        std::cout << "  nlm experiment list                     - List available experiments" << std::endl;
        std::cout << "  nlm experiment info <name>              - Show experiment information" << std::endl;
        std::cout << "  nlm experiment configure <name>          - Configure experiment parameters" << std::endl;
        std::cout << "  nlm experiment results                  - Show experiment results" << std::endl;
        std::cout << "  nlm experiment logs <name>              - Show experiment logs" << std::endl;
    }
    
    // System command handlers
    void handleSystemCommand(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: nlm system <command> [options]" << std::endl;
            std::cout << "Commands:" << std::endl;
            std::cout << "  info        - Show system information" << std::endl;
            std::cout << "  benchmark   - Run system benchmarks" << std::endl;
            std::cout << "  status      - Show system status" << std::endl;
            std::cout << "  logs        - Show system logs" << std::endl;
            std::cout << "  memory      - Show memory usage" << std::endl;
            std::cout << "  performance - Show performance metrics" << std::endl;
            return;
        }
        
        std::string subcommand = args[1];
        
        if (subcommand == "info") {
            systemInfo();
        } else if (subcommand == "benchmark") {
            systemBenchmark();
        } else if (subcommand == "status") {
            systemStatus();
        } else if (subcommand == "logs") {
            systemLogs();
        } else if (subcommand == "memory") {
            systemMemory();
        } else if (subcommand == "performance") {
            systemPerformance();
        } else {
            std::cout << "Invalid command: nlm system " << subcommand << std::endl;
            printSystemHelp();
        }
    }
    
    // System command implementations
    void systemInfo() {
        std::cout << "System information:" << std::endl;
        // Implementation would go here
        std::cout << "  (stub)" << std::endl;
    }
    
    void systemBenchmark() {
        std::cout << "Running system benchmarks..." << std::endl;
        // Implementation would go here
        std::cout << "Benchmark completed (stub)" << std::endl;
    }
    
    void systemStatus() {
        std::cout << "System status:" << std::endl;
        // Implementation would go here
        std::cout << "  (stub)" << std::endl;
    }
    
    void systemLogs() {
        std::cout << "System logs:" << std::endl;
        // Implementation would go here
        std::cout << "  (stub)" << std::endl;
    }
    
    void systemMemory() {
        std::cout << "Memory usage:" << std::endl;
        // Implementation would go here
        std::cout << "  (stub)" << std::endl;
    }
    
    void systemPerformance() {
        std::cout << "Performance metrics:" << std::endl;
        // Implementation would go here
        std::cout << "  (stub)" << std::endl;
    }
    
    void printSystemHelp() const {
        std::cout << "System commands:" << std::endl;
        std::cout << "  nlm system info           - Show system information" << std::endl;
        std::cout << "  nlm system benchmark      - Run system benchmarks" << std::endl;
        std::cout << "  nlm system status         - Show system status" << std::endl;
        std::cout << "  nlm system logs           - Show system logs" << std::endl;
        std::cout << "  nlm system memory         - Show memory usage" << std::endl;
        std::cout << "  nlm system performance    - Show performance metrics" << std::endl;
    }
    
    // Integration command handlers
    void handleIntegrationCommand(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: nlm integration <command> [options]" << std::endl;
            std::cout << "Commands:" << std::endl;
            std::cout << "  verify     - Verify system integration" << std::endl;
            std::cout << "  test       - Run integration tests" << std::endl;
            std::cout << "  status     - Show integration status" << std::endl;
            std::cout << "  complete   - Complete integration testing" << std::endl;
            return;
        }
        
        std::string subcommand = args[1];
        
        if (subcommand == "verify") {
            integrationVerify();
        } else if (subcommand == "test") {
            integrationTest();
        } else if (subcommand == "status") {
            integrationStatus();
        } else if (subcommand == "complete") {
            integrationComplete();
        } else {
            std::cout << "Invalid command: nlm integration " << subcommand << std::endl;
            printIntegrationHelp();
        }
    }
    
    // Integration command implementations
    void integrationVerify() {
        std::cout << "Verifying system integration..." << std::endl;
        // Implementation would go here
        std::cout << "Integration verification completed (stub)" << std::endl;
    }
    
    void integrationTest() {
        std::cout << "Running integration tests..." << std::endl;
        // Implementation would go here
        std::cout << "Integration tests completed (stub)" << std::endl;
    }
    
    void integrationStatus() {
        std::cout << "Integration status:" << std::endl;
        // Implementation would go here
        std::cout << "  (stub)" << std::endl;
    }
    
    void integrationComplete() {
        std::cout << "Completing integration testing..." << std::endl;
        // Implementation would go here
        std::cout << "Integration testing completed (stub)" << std::endl;
    }
    
    void printIntegrationHelp() const {
        std::cout << "Integration commands:" << std::endl;
        std::cout << "  nlm integration verify     - Verify system integration" << std::endl;
        std::cout << "  nlm integration test       - Run integration tests" << std::endl;
        std::cout << "  nlm integration status     - Show integration status" << std::endl;
        std::cout << "  nlm integration complete   - Complete integration testing" << std::endl;
    }
    
    // Visualization command handlers
    void handleVisualizeCommand(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: nlm visualize <command> [options]" << std::endl;
            std::cout << "Commands:" << std::endl;
            std::cout << "  start      - Start visualization" << std::endl;
            std::cout << "  stop       - Stop visualization" << std::endl;
            std::cout << "  capture <file> - Capture visualization" << std::endl;
            std::cout << "  render <file>  - Render visualization" << std::endl;
            std::cout << "  export <file> <format> - Export visualization" << std::endl;
            return;
        }
        
        std::string subcommand = args[1];
        
        if (subcommand == "start") {
            visualizeStart();
        } else if (subcommand == "stop") {
            visualizeStop();
        } else if (subcommand == "capture" && args.size() >= 3) {
            visualizeCapture(args[2]);
        } else if (subcommand == "render" && args.size() >= 3) {
            visualizeRender(args[2]);
        } else if (subcommand == "export" && args.size() >= 4) {
            visualizeExport(args[2], args[3]);
        } else {
            std::cout << "Invalid command: nlm visualize " << subcommand << std::endl;
            printVisualizeHelp();
        }
    }
    
    // Visualization command implementations
    void visualizeStart() {
        std::cout << "Starting visualization..." << std::endl;
        // Implementation would go here
        std::cout << "Visualization started (stub)" << std::endl;
    }
    
    void visualizeStop() {
        std::cout << "Stopping visualization..." << std::endl;
        // Implementation would go here
        std::cout << "Visualization stopped (stub)" << std::endl;
    }
    
    void visualizeCapture(const std::string& file) {
        std::cout << "Capturing visualization to " << file << "..." << std::endl;
        // Implementation would go here
        std::cout << "Visualization captured (stub)" << std::endl;
    }
    
    void visualizeRender(const std::string& file) {
        std::cout << "Rendering visualization from " << file << "..." << std::endl;
        // Implementation would go here
        std::cout << "Visualization rendered (stub)" << std::endl;
    }
    
    void visualizeExport(const std::string& file, const std::string& format) {
        std::cout << "Exporting visualization to " << file << " in " << format << " format..." << std::endl;
        // Implementation would go here
        std::cout << "Visualization exported (stub)" << std::endl;
    }
    
    void printVisualizeHelp() const {
        std::cout << "Visualization commands:" << std::endl;
        std::cout << "  nlm visualize start                     - Start visualization" << std::endl;
        std::cout << "  nlm visualize stop                      - Stop visualization" << std::endl;
        std::cout << "  nlm visualize capture <file>             - Capture visualization" << std::endl;
        std::cout << "  nml visualize render <file>              - Render visualization" << std::endl;
        std::cout << "  nml visualize export <file> <format>    - Export visualization" << std::endl;
    }
    
    // Advanced command handlers
    void handleAdvancedCommand(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: nlm advanced <command> [options]" << std::endl;
            std::cout << "Commands:" << std::endl;
            std::cout << "  batch run <script>           - Run batch script" << std::endl;
            std::cout << "  script execute <file>        - Execute script file" << std::endl;
            std::cout << "  parallel run <command> <count> - Run command in parallel" << std::endl;
            std::cout << "  monitor <duration>           - Monitor system for duration" << std::endl;
            std::cout << "  watch <file>                 - Watch file for changes" << std::endl;
            return;
        }
        
        std::string subcommand = args[1];
        
        if (subcommand == "run" && args.size() >= 3) {
            advancedBatchRun(args[2]);
        } else if (subcommand == "execute" && args.size() >= 3) {
            advancedScriptExecute(args[2]);
        } else if (subcommand == "run" && args.size() >= 4) {
            advancedParallelRun(args[2], args[3]);
        } else if (subcommand == "monitor" && args.size() >= 3) {
            advancedMonitor(args[2]);
        } else if (subcommand == "watch" && args.size() >= 3) {
            advancedWatch(args[2]);
        } else {
            std::cout << "Invalid command: nlm advanced " << subcommand << std::endl;
            printAdvancedHelp();
        }
    }
    
    // Advanced command implementations
    void advancedBatchRun(const std::string& script) {
        std::cout << "Running batch script: " << script << std::endl;
        // Implementation would go here
        std::cout << "Batch run completed (stub)" << std::endl;
    }
    
    void advancedScriptExecute(const std::string& file) {
        std::cout << "Executing script file: " << file << std::endl;
        // Implementation would go here
        std::cout << "Script execution completed (stub)" << std::endl;
    }
    
    void advancedParallelRun(const std::string& command, const std::string& count) {
        std::cout << "Running command in parallel: " << command << " with " << count << " instances" << std::endl;
        // Implementation would go here
        std::cout << "Parallel run completed (stub)" << std::endl;
    }
    
    void advancedMonitor(const std::string& duration) {
        std::cout << "Monitoring system for duration: " << duration << std::endl;
        // Implementation would go here
        std::cout << "Monitoring completed (stub)" << std::endl;
    }
    
    void advancedWatch(const std::string& file) {
        std::cout << "Watching file for changes: " << file << std::endl;
        // Implementation would go here
        std::cout << "Watch completed (stub)" << std::endl;
    }
    
    void printAdvancedHelp() const {
        std::cout << "Advanced commands:" << std::endl;
        std::cout << "  nlm advanced batch run <script>          - Run batch script" << std::endl;
        std::cout << "  nlm advanced script execute <file>       - Execute script file" << std::endl;
        std::cout << "  nlm advanced parallel run <command> <count> - Run command in parallel" << std::endl;
        std::cout << "  nlm advanced monitor <duration>           - Monitor system for duration" << std::endl;
        std::cout << "  nml advanced watch <file>                 - Watch file for changes" << std::endl;
    }
    
    void printVersion() const {
        std::cout << "NLM - Neural Learning Machine v2.0" << std::endl;
        std::cout << "Enhanced Configuration System" << std::endl;
        std::cout << "All CLI commands implemented (stubs)" << std::endl;
    }
};

int main(int argc, char** argv) {
    // Initialize NLM configuration system
    nlm::Config config;
    
    // Load configuration from file if specified
    std::string configFile = "config.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config.legacyLoadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Override with command line args
    config.legacyLoadFromArgs(argc, argv);
    
    // Create and run CLI command handler
    CLICommandHandler cliHandler(config);
    
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    cliHandler.executeCommand(args);
    
    return 0;
}
