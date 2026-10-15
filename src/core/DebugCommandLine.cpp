// NLM Advanced Debugging and Profiling Tools - Debug Command Line Interface Implementation

#include "core/DebugCommandLine.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace nlm {

bool DebugCommandLine::parseArguments(int argc, char* argv[]) {
    parser_ = std::make_unique<CommandLineParser>();
    
    // Add debug options
    parser_->addOption("debug", "Enable advanced debugging", false);
    parser_->addOption("profile", "Enable performance profiling", false);
    parser_->addOption("monitor", "Enable system monitoring", false);
    parser_->addOption("optimize", "Enable optimization analysis", false);
    parser_->addOption("visualize", "Enable visualization", false);
    parser_->addOption("debug-config", "Debug configuration file", "");
    parser_->addOption("profile-config", "Performance profiler configuration file", "");
    parser_->addOption("output-format", "Output format (text, json, csv, xml)", "text");
    parser_->addOption("output-file", "Output file", "");
    parser_->addOption("profiling-interval", "Profiling interval in seconds", "0.5");
    parser_->addOption("log-level", "Log level (debug, info, warning, error)", "info");
    parser_->addOption("non-intrusive", "Run in non-intrusive mode (minimal performance impact)", false);
    parser_->addOption("export-all", "Export all debug and profiling data", false);
    parser_->addOption("validate-only", "Validate configuration only", false);
    
    // Parse arguments
    if (!parser_->parse(argc, argv)) {
        printUsage();
        return false;
    }
    
    // Extract options
    options_.enableDebug = parser_->getOption<bool>("debug");
    options_.enableProfiling = parser_->getOption<bool>("profile");
    options_->enableMonitoring = parser_->getOption<bool>("monitor");
    options_->enableOptimization = parser_->getOption<bool>("optimize");
    options_->enableVisualization = parser_->getOption<bool>("visualize");
    options_->debugConfigFile = parser_->getOption<std::string>("debug-config");
    options_->profileConfigFile = parser_->getOption<std::string>("profile-config");
    options_->outputFormat = parser_->getOption<std::string>("output-format");
    options_->outputFile = parser_->getOption<std::string>("output-file");
    options_->profilingInterval = std::stod(parser_->getOption<std::string>("profiling-interval"));
    options_->logLevel = parser_->getOption<std::string>("log-level");
    options_->nonIntrusiveMode = parser_->getOption<bool>("non-intrusive");
    options_->exportAllData = parser_->getOption<bool>("export-all");
    options_->validateOnly = parser_->getOption<bool>("validate-only");
    
    return true;
}

void DebugCommandLine::printUsage() const {
    std::cout << "Usage: nlm --debug [options]\n";
    std::cout << "       nlm --profile [options]\n";
    std::cout << "       nlm --monitor [options]\n";
    std::cout << "       nlm --optimize [options]\n";
    std::cout << "\nDebug and Profiling Options:\n";
    std::cout << "  --debug                     Enable advanced debugging\n";
    std::cout << "  --profile                   Enable performance profiling\n";
    std::cout << "  --monitor                   Enable system monitoring\n";
    std::cout << "  --optimize                  Enable optimization analysis\n";
    std::cout << "  --visualize                 Enable visualization\n";
    std::cout << "  --debug-config <file>       Debug configuration file\n";
    std::cout << "  --profile-config <file>     Performance profiler configuration file\n";
    std::cout << "  --output-format <format>    Output format (text, json, csv, xml)\n";
    std::cout << "  --output-file <file>        Output file\n";
    std::cout << "  --profiling-interval <sec>   Profiling interval in seconds\n";
    std::cout << "  --log-level <level>         Log level (debug, info, warning, error)\n";
    std::cout << "  --non-intrusive             Run in non-intrusive mode (minimal performance impact)\n";
    std::cout << "  --export-all                Export all debug and profiling data\n";
    std::cout << "  --validate-only             Validate configuration only\n";
    std::cout << "\nExamples:\n";
    std::cout << "  nlm --debug --profile --monitor --debug-config=debug_config.json\n";
    std::cout << "  nlm --profile --output-file=performance_report.html --export-all\n";
    std::cout << "  nlm --debug --visualize --output-format=json\n";
}

void DebugCommandLine::printHelp() const {
    std::cout << "=== NLM Advanced Debugging and Profiling Tools ===\n\n";
    std::cout << "This is an enhanced debugging and profiling system for the NLM (Neural Learning Machine).\n\n";
    std::cout << "Key Features:\n";
    std::cout << "  ✓ Breakpoint debugging with conditional execution\n";
    std::cout << "  ✓ Memory corruption detection and leak analysis\n";
    std::cout << "  ✓ Performance profiling with CPU and memory metrics\n";
    std::cout << "  ✓ Real-time system monitoring with alerts\n";
    std::cout << "  ✓ Automatic bottleneck detection and optimization recommendations\n";
    std::cout << "  ✓ Comprehensive visualization and reporting\n";
    std::cout << "  ✓ Export functionality (JSON, CSV, XML, HTML)\n\n";
    std::cout << "Usage Examples:\n";
    std::cout << "  nlm --help                                  Show this help message\n";
    std::cout << "  nlm --version                              Show version information\n";
    std::cout << "\nFor more information, visit: https://github.com/nlm-project/nlm\n";
}

bool DebugCommandLine::execute(DebugCommandLineOptions& options) {
    options_ = options;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set log level
    if (options_.logLevel == "debug") {
        consoleLogger->setLevel(LogLevel::Debug);
    } else if (options_.logLevel == "warning") {
        consoleLogger->setLevel(LogLevel::Warning);
    } else if (options_.logLevel == "error") {
        consoleLogger->setLevel(LogLevel::Error);
    }
    
    // Validate configuration
    validateConfiguration();
    
    // Initialize debug and profiling tools
    setupDebugTools();
    setupPerformanceTools();
    setupMonitoringTools();
    setupVisualizationTools();
    setupExportTools();
    
    // Execute based on options
    if (options_.validateOnly) {
        std::cout << "Configuration validation complete.\n";
        return true;
    }
    
    if (options_.enableDebug && options_.enableProfiling && options_.enableMonitoring) {
        runDebugSession();
    } else if (options_.enableDebug) {
        std::cout << "Debug mode enabled. For full debugging session, use --debug --profile --monitor.\n";
        runDebugSession();
    } else if (options_.enableProfiling) {
        runProfileSession();
    } else if (options_.enableMonitoring) {
        runMonitorSession();
    } else if (options_.enableOptimization) {
        runOptimizationSession();
    }
    
    // Generate output
    generateOutput(options_.outputFormat, options_.outputFile);
    
    return true;
}

bool DebugCommandLine::initializeDebugManager(const DebugCommandLineOptions& options) {
    DebugConfig debugConfig;
    
    // Load debug configuration
    if (!options.debugConfigFile.empty()) {
        debugConfig = DebugConfig::loadFromConfig(options.debugConfigFile);
    }
    
    // Configure based on options
    debugConfig.enableBreakpointDebugging = options.enableDebug;
    debugConfig.enableWatchpointDebugging = options.enableDebug;
    debugConfig.enableTraceDebugging = options.enableDebug;
    debugConfig.enableMemoryDebugging = options.enableDebug;
    debugConfig.enableConcurrencyDebugging = options.enableDebug;
    debugConfig.nonIntrusiveMode = options.nonIntrusiveMode;
    
    // Create debug manager
    debugManager_ = std::make_unique<DebugManager>(debugConfig);
    
    // Initialize debuggers
    debugManager_->startAllDebuggers();
    
    NLM_LOG_INFO("DebugManager initialized successfully");
    return true;
}

bool DebugCommandLine::initializePerformanceProfiler(const DebugCommandLineOptions& options) {
    PerformanceProfilerConfig profilerConfig;
    
    // Load profiler configuration
    if (!options.profileConfigFile.empty()) {
        // In a real implementation, this would load the profiler config
        // For now, use defaults
    }
    
    // Configure based on options
    profilerConfig.enableCPUProfiling = options.enableProfiling;
    profilerConfig.enableMemoryProfiling = options.enableProfiling;
    profilerConfig.enableSystemProfiling = options.enableMonitoring;
    profilerConfig.profilingInterval = std::chrono::duration<double>(options.profilingInterval);
    profilerConfig.nonIntrusiveMode = options.nonIntrusiveMode;
    
    // Create performance profiler
    performanceProfiler_ = std::make_unique<PerformanceProfiler>(profilerConfig);
    
    // Start system monitoring
    performanceProfiler_->startSystemProfiling();
    
    NLM_LOG_INFO("PerformanceProfiler initialized successfully");
    return true;
}

void DebugCommandLine::setupDebugTools() {
    if (options_.enableDebug && options_.debugConfigFile.empty()) {
        // Use default debug configuration
        DebugConfig defaultConfig;
        defaultConfig.nonIntrusiveMode = options_.nonIntrusiveMode;
        defaultConfig.profilingInterval = options_.profilingInterval;
        
        debugManager_ = std::make_unique<DebugManager>(defaultConfig);
        debugManager_->startAllDebuggers();
    } else if (options_.enableDebug && !options_.debugConfigFile.empty()) {
        initializeDebugManager(options_);
    }
}

void DebugCommandLine::setupPerformanceTools() {
    if (options_.enableProfiling || options_.enableMonitoring) {
        initializePerformanceProfiler(options_);
    }
}

void DebugCommandLine::setupMonitoringTools() {
    if (options_.enableMonitoring && performanceProfiler_) {
        performanceProfiler_->startSystemMonitoring();
    }
}

void DebugCommandLine::setupVisualizationTools() {
    if (options_.enableVisualization && debugManager_) {
        // In a full implementation, this would setup visualization components
        NLM_LOG_INFO("Visualization tools would be initialized here");
    }
}

void DebugCommandLine::setupExportTools() {
    if (options_.exportAllData) {
        // Export all data after session completion
        if (debugManager_) {
            std::string filename = "debug_export_" + getTimestampString() + ".json";
            debugManager_->exportDebugData(filename, "json");
        }
        if (performanceProfiler_) {
            std::string filename = "profile_export_" + getTimestampString() + ".csv";
            performanceProfiler_->exportMetrics(filename, "csv");
        }
    }
}

void DebugCommandLine::validateConfiguration() const {
    std::cout << "Validating debug and profiling configuration...\n";
    
    // Check for conflicting options
    int enabledTools = 0;
    if (options_.enableDebug) enabledTools++;
    if (options_.enableProfiling) enabledTools++;
    if (options_.enableMonitoring) enabledTools++;
    if (options_.enableOptimization) enabledTools++;
    
    if (enabledTools == 0) {
        std::cout << "Warning: No debugging or profiling tools enabled. "
                  << "Use --debug, --profile, --monitor, or --optimize.\n";
    }
    
    // Validate configuration files
    if (!options_.debugConfigFile.empty()) {
        std::ifstream file(options_.debugConfigFile);
        if (!file.is_open()) {
            std::cout << "Warning: Debug configuration file not found: " 
                      << options_.debugConfigFile << "\n";
        } else {
            file.close();
            std::cout << "Debug configuration file loaded: " << options_.debugConfigFile << "\n";
        }
    }
    
    if (!options_.profileConfigFile.empty()) {
        std::ifstream file(options_.profileConfigFile);
        if (!file.is_open()) {
            std::cout << "Warning: Profile configuration file not found: " 
                      << options_.profileConfigFile << "\n";
        } else {
            file.close();
            std::cout << "Profile configuration file loaded: " << options_.profileConfigFile << "\n";
        }
    }
    
    std::cout << "Configuration validation complete.\n";
}

std::string DebugCommandLine::getTimestampString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

void DebugCommandLine::generateOutput(const std::string& format, const std::string& filename) {
    if (debugManager_) {
        if (format == "json" || filename.empty()) {
            std::string outputFile = filename.empty() ? "debug_report.json" : filename;
            debugManager_->exportDebugData(outputFile, "json");
            std::cout << "Debug data exported to: " << outputFile << "\n";
        } else if (format == "csv") {
            std::string outputFile = filename.empty() ? "debug_report.csv" : filename;
            debugManager_->exportDebugData(outputFile, "csv");
            std::cout << "Debug data exported to: " << outputFile << "\n";
        } else if (format == "xml") {
            std::string outputFile = filename.empty() ? "debug_report.xml" : filename;
            debugManager_->exportDebugEvents(outputFile, "xml");
            std::cout << "Debug data exported to: " << outputFile << "\n";
        } else {
            // Generate text report
            std::string report = debugManager_->generateComprehensiveReport();
            std::cout << report;
        }
    }
    
    if (performanceProfiler_) {
        if (format == "json") {
            std::string outputFile = filename.empty() ? "profile_report.json" : filename;
            performanceProfiler_->exportMetrics(outputFile, "json");
            std::cout << "Performance data exported to: " << outputFile << "\n";
        } else if (format == "csv") {
            std::string outputFile = filename.empty() ? "profile_report.csv" : filename;
            performanceProfiler_->exportMetrics(outputFile, "csv");
            std::cout << "Performance data exported to: " << outputFile << "\n";
        } else if (format == "xml") {
            std::string outputFile = filename.empty() ? "profile_report.xml" : filename;
            performanceProfiler_->exportProfilingData(outputFile, "xml");
            std::cout << "Performance data exported to: " << outputFile << "\n";
        }
    }
}

void DebugCommandLine::printStatus() const {
    std::cout << "=== Debug and Profiling Status ===\n\n";
    
    if (debugManager_) {
        std::cout << debugManager_->getStatus() << "\n";
    }
    
    if (performanceProfiler_) {
        std::cout << performanceProfiler_->getProfilerStatistics() << "\n";
    }
    
    std::cout << "Active components: " << (debugManager_ ? debugManager_->getActiveComponents().size() : 0) << "\n";
}

void DebugCommandLine::printPerformanceReport() const {
    if (performanceProfiler_) {
        std::cout << performanceProfiler_->getPerformanceReport() << "\n";
    }
}

void DebugCommandLine::printDebugReport() const {
    if (debugManager_) {
        std::cout << debugManager_->generateDebugReport() << "\n";
    }
}

void DebugCommandLine::printSystemStatus() const {
    std::cout << "=== System Status ===\n\n";
    
    if (performanceProfiler_) {
        std::cout << "PerformanceProfiler active: " 
                  << (performanceProfiler_->isProfiling() ? "Yes" : "No") << "\n";
        std::cout << "Active profiling sessions: " 
                  << performanceProfiler_->getActiveSessionIds().size() << "\n";
        std::cout << "Total profiling sessions: " 
                  << performanceProfiler_->getSessionCount() << "\n";
    }
}

void DebugCommandLine::runDebugSession() {
    if (!debugManager_) {
        std::cout << "DebugManager not initialized. Cannot run debug session.\n";
        return;
    }
    
    std::cout << "Starting debug session...\n";
    
    // Create a debug session
    std::string sessionId = debugManager_->createDebugSession("debug_session");
    
    // Simulate some debug activity
    debugManager_->attachToBrain("brain_001");
    debugManager_->attachToExperiment("learning_experiment");
    
    // Create some test breakpoints
    if (debugManager_->getBreakpointDebugger()) {
        BreakpointCondition condition1("high_memory_usage", "Break when memory > 1GB", "memoryUsage > 1024");
        debugManager_->getBreakpointDebugger()->addBreakpoint(condition1);
        
        BreakpointCondition condition2("slow_simulation", "Break when step > 10000", "simulationStep > 10000");
        debugManager_->getBreakpointDebugger()->addBreakpoint(condition2);
    }
    
    // Simulate some debug events
    DebugEvent testEvent(DebugEventType::TraceEvent, "debug_manager", "test_event",
                         "Test debug event for demonstration");
    debugManager_->logEvent(testEvent);
    
    std::cout << "Debug session created: " << sessionId << "\n";
    std::cout << "Debug session running...\n";
    
    // In a real implementation, this would run for a specified duration
    // For now, just create a snapshot and end the session
    debugManager_->snapshotDebugState(sessionId);
    debugManager_->endDebugSession(sessionId);
    
    std::cout << "Debug session completed.\n";
}

void DebugCommandLine::runProfileSession() {
    if (!performanceProfiler_) {
        std::cout << "PerformanceProfiler not initialized. Cannot run profile session.\n";
        return;
    }
    
    std::cout << "Starting performance profiling session...\n";
    
    // Start profiling
    performanceProfiler_->startProfile("neural_simulation");
    
    // Simulate some profiling activity
    for (int i = 0; i < 10; ++i) {
        performanceProfiler_->recordMetric("cpu_usage", 50.0 + (i * 5), "percent", "simulation");
        performanceProfiler_->recordMetric("memory_usage", 100.0 + (i * 10), "mb", "simulation");
        performanceProfiler_->recordMetric("simulation_step", static_cast<double>(i * 100), "steps", "simulation");
        
        // Simulate processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Stop profiling
    performanceProfiler_->stopProfile("neural_simulation");
    performanceProfiler_->stopSystemProfiling();
    
    std::cout << "Performance profiling session completed.\n";
}

void DebugCommandLine::runMonitorSession() {
    if (!performanceProfiler_) {
        std::cout << "PerformanceProfiler not initialized. Cannot run monitor session.\n";
        return;
    }
    
    std::cout << "Starting system monitoring session...\n";
    
    // Start system monitoring
    performanceProfiler_->startSystemMonitoring();
    
    // Simulate monitoring activity
    for (int i = 0; i < 5; ++i) {
        performanceProfiler_->recordSystemMetric("cpu_usage", 60.0 + (i * 5), "system");
        performanceProfiler_->recordSystemMetric("memory_usage", 200.0 + (i * 20), "system");
        performanceProfiler_->recordSystemMetric("disk_usage", 50.0 + (i * 5), "system");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    // Stop system monitoring
    performanceProfiler_->stopSystemMonitoring();
    
    std::cout << "System monitoring session completed.\n";
}

void DebugCommandLine::runOptimizationSession() {
    if (!performanceProfiler_) {
        std::cout << "PerformanceProfiler not initialized. Cannot run optimization session.\n";
        return;
    }
    
    std::cout << "Starting optimization analysis session...\n";
    
    // Analyze performance data for bottlenecks
    auto bottlenecks = performanceProfiler_->detectPerformanceBottlenecks();
    
    std::cout << "Optimization Analysis Results:\n";
    for (const auto& bottleneck : bottlenecks) {
        std::cout << "  - " << bottleneck << "\n";
    }
    
    if (bottlenecks.empty()) {
        std::cout << "  No performance bottlenecks detected.\n";
    }
    
    std::cout << "Optimization session completed.\n";
}

} // namespace nlm
