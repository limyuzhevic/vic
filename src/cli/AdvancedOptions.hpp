// Performance command options
struct ProfileCommandOptions : public CommandOptions {
    // Profiling options
    bool profile = false;                      // Enable profiling
    bool benchmark = false;                    // Enable benchmarking
    bool analyze = false;                      // Enable analysis
    std::string outputFile;                    // Output file path
    std::string outputFormat = "text";         // Output format: text, json, csv, html
    bool realTime = false;                     // Real-time profiling
    double samplingInterval = 0.1;             // Sampling interval in seconds
    
    // Profiler selection
    bool cpuProfiler = false;                  // Enable CPU profiler
    bool memoryProfiler = false;               // Enable memory profiler
    bool threadProfiler = false;               // Enable thread profiler
    bool simulationProfiler = false;           // Enable simulation profiler
    
    // Benchmarking options
    bool enableBenchmarks = false;             // Enable all benchmarks
    std::string benchmarkSuite;                // Specific benchmark suite name
    bool runScalability = false;               // Run scalability tests
    bool runStressTest = false;                // Run stress tests
    bool runLoadTest = false;                  // Run load tests
    size_t maxThreads = 1;                     // Maximum threads for benchmarking
    
    // Performance analysis
    bool detectRegressions = false;            // Detect performance regressions
    double regressionThreshold = 0.05;         // Regression threshold (5%)
    bool generateReports = false;              // Generate performance reports
    
    // Output and reporting
    bool showDashboard = false;                 // Show performance dashboard
    bool exportMetrics = false;                 // Export metrics to file
    std::string dashboardFormat = "html";      // Dashboard format
    
    // Configuration
    std::string configFile = "configs/default.cfg";
    bool validateConfig = false;                // Validate configuration before profiling
    
    // Advanced profiling
    bool enableHardwareProfiling = false;       // Enable hardware-specific profiling
    bool enableIoprofiling = false;             // Enable I/O profiling
    double timeLimit = 0.0;                    // Time limit in seconds (0 = unlimited)
    size_t memoryLimit = 0;                    // Memory limit in MB (0 = unlimited)
    
    void parse(const std::vector<std::string>& args) override;
    bool validate() const override;
    std::string getProfileTypes() const;
};