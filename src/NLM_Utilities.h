// NLM Phase 6 - Utility Functions and Base Classes
// This file provides common functionality to reduce code duplication
// across demo files and improve code organization.

#ifndef NLM_UTILITIES_H
#define NLM_UTILITIES_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <map>
#include <functional>
#include <algorithm>
#include <cmath>

using namespace std;

namespace nlm {
namespace utils {

// Forward declarations
class DemoBase;
class ConfigManager;
class ErrorHandler;
class MetricsCollector;
class ProgressReporter;

// Configuration structure for consistent handling
struct DemoOptions {
    // Core options
    string configFile = "configs/default.cfg";
    uint64_t randomSeed = 42;
    string outputPath = ".";
    
    // Brain configuration
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.15f;
    double simulationTimestep = 0.001;
    
    // Experiment configuration
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    
    // Output configuration
    bool verbose = true;
    bool quiet = false;
    string reportFormat = "text";
    
    // Analysis options
    bool compareBeforeAfter = true;
    size_t baselineSteps = 100;
    bool analyzeConvergence = true;
    
    // Experimental controls
    bool runFullDemo = true;
    bool runQuickVerification = true;
    bool runDeepAnalysis = false;
};

// Enhanced error handling with context and severity levels
class ErrorHandler {
public:
    enum class Severity { INFO, WARNING, ERROR, FATAL };
    
    struct ErrorContext {
        string message;
        string sourceFile;
        int lineNumber;
        Severity severity;
        chrono::system_clock::time_point timestamp;
        string details;
        
        ErrorContext(const string& msg, const string& file, int line, 
                    Severity sev = Severity::ERROR, const string& det = "")
            : message(msg), sourceFile(file), lineNumber(line), 
              severity(sev), timestamp(chrono::system_clock::now()),
              details(det) {}
    };
    
private:
    vector<ErrorContext> errors_;
    bool exitOnError_ = true;
    
public:
    void addError(const string& message, const string& sourceFile = "", 
                  int lineNumber = 0, Severity severity = Severity::ERROR,
                  const string& details = "") {
        errors_.push_back(ErrorContext(message, sourceFile, lineNumber, 
                                     severity, details));
        
        // Output based on severity
        switch (severity) {
            case Severity::INFO:
                if (message.find("[INFO]") == string::npos) {
                    cout << "[INFO] " << message << endl;
                }
                break;
            case Severity::WARNING:
                cout << "[WARNING] " << message << endl;
                break;
            case Severity::ERROR:
                cout << "[ERROR] " << message << endl;
                if (exitOnError_) {
                    printErrors();
                    exit(1);
                }
                break;
            case Severity::FATAL:
                cout << "[FATAL] " << message << endl;
                printErrors();
                exit(1);
                break;
        }
    }
    
    void printErrors() {
        if (errors_.empty()) return;
        
        cout << "\n=== ERROR SUMMARY ===" << endl;
        cout << "Total errors: " << errors_.size() << endl;
        
        for (size_t i = 0; i < errors_.size(); ++i) {
            const auto& err = errors_[i];
            cout << "  " << (i + 1) << ". [" << severityToString(err.severity) << "] "
                 << err.message << endl;
            
            if (!err.details.empty()) {
                cout << "     Details: " << err.details << endl;
            }
            
            if (!err.sourceFile.empty()) {
                cout << "     Source: " << err.sourceFile << ":" 
                     << err.lineNumber << endl;
            }
        }
    }
    
    bool hasErrors() const { return !errors_.empty(); }
    
    static string severityToString(Severity sev) {
        switch (sev) {
            case Severity::INFO: return "INFO";
            case Severity::WARNING: return "WARNING";
            case Severity::ERROR: return "ERROR";
            case Severity::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    void setExitOnError(bool exit) { exitOnError_ = exit; }
};

// Progress reporting with visual feedback
class ProgressReporter {
private:
    bool enabled_ = true;
    int barWidth_ = 40;
    string prefix_ = "[PROGRESS]";
    
public:
    void report(const string& message, double progress) {
        if (!enabled_) return;
        
        cout << prefix_ << " [";
        int pos = static_cast<int>(barWidth_ * progress);
        for (int i = 0; i < barWidth_; ++i) {
            if (i < pos) cout << "=";
            else if (i == pos) cout << ">";
            else cout << " ";
        }
        cout << "] " << static_cast<int>(progress * 100.0) << "% " 
             << message << endl;
    }
    
    void setEnabled(bool enabled) { enabled_ = enabled; }
    void setBarWidth(int width) { barWidth_ = width; }
    void setPrefix(const string& prefix) { prefix_ = prefix; }
};

// Metrics collection for performance analysis
class MetricsCollector {
private:
    struct Metric {
        string name;
        double value;
        string unit;
        chrono::system_clock::time_point timestamp;
        map<string, string> tags;
        
        Metric(const string& name, double value, const string& unit = "")
            : name(name), value(value), unit(unit), 
              timestamp(chrono::system_clock::now()) {}
    };
    
    vector<Metric> metrics_;
    double startTime_;
    
public:
    MetricsCollector() {
        startTime_ = chrono::duration<double>::clock::now().time_since_epoch().count();
    }
    
    void addMetric(const string& name, double value, const string& unit = "",
                   const map<string, string>& tags = {}) {
        metrics_.push_back(Metric(name, value, unit));
    }
    
    void recordTime(const string& name, double seconds, 
                   const map<string, string>& tags = {}) {
        addMetric(name, seconds, "s", tags);
    }
    
    void recordRate(const string& name, double rate, const string& unit = "Hz",
                   const map<string, string>& tags = {}) {
        addMetric(name, rate, unit, tags);
    }
    
    void recordCount(const string& name, size_t count,
                    const map<string, string>& tags = {}) {
        addMetric(name, static_cast<double>(count), "count", tags);
    }
    
    void printSummary() {
        if (metrics_.empty()) return;
        
        cout << "\n=== METRICS SUMMARY ===" << endl;
        
        // Group metrics by name
        map<string, vector<Metric>> grouped;
        for (const auto& metric : metrics_) {
            grouped[metric.name].push_back(metric);
        }
        
        for (const auto& pair : grouped) {
            const string& name = pair.first;
            const vector<Metric>& metricGroup = pair.second;
            
            if (metricGroup.size() == 1) {
                const auto& m = metricGroup[0];
                cout << name << ": " << fixed << setprecision(4) 
                     << m.value << " " << m.unit << endl;
            } else {
                cout << name << ": [";
                for (size_t i = 0; i < metricGroup.size(); ++i) {
                    if (i > 0) cout << ", ";
                    cout << fixed << setprecision(4) << metricGroup[i].value
                         << " " << metricGroup[i].unit;
                }
                cout << "]" << endl;
            }
        }
    }
    
    double getElapsedTime() const {
        return chrono::duration<double>::clock::now().time_since_epoch().count() - startTime_;
    }
    
    void clear() { metrics_.clear(); }
};

// Configuration management utilities
class ConfigManager {
public:
    static DemoOptions parseCommandLine(int argc, char* argv[]) {
        DemoOptions options;
        
        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            
            if (arg == "--help") {
                printUsage(argv[0]);
                exit(0);
            }
            else if (arg == "--version") {
                cout << "NLM Phase 6 Demo - Version 1.0.0" << endl;
                exit(0);
            }
            else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            }
            else if (arg == "--seed" && i + 1 < argc) {
                options.randomSeed = stoull(argv[++i]);
            }
            else if (arg == "--neurons" && i + 1 < argc) {
                options.neuronCount = stoull(argv[++i]);
            }
            else if (arg == "--regions" && i + 1 < argc) {
                options.regionCount = stoull(argv[++i]);
            }
            else if (arg == "--connection-prob" && i + 1 < argc) {
                options.connectionProbability = stof(argv[++i]);
            }
            else if (arg == "--timestep" && i + 1 < argc) {
                options.simulationTimestep = stod(argv[++i]);
            }
            else if (arg == "--max-steps" && i + 1 < argc) {
                options.maxSteps = stoull(argv[++i]);
            }
            else if (arg == "--no-checkpointing") {
                options.enableCheckpointing = false;
            }
            else if (arg == "--no-replay") {
                options.enableReplay = false;
            }
            else if (arg == "--no-development") {
                options.enableDevelopment = false;
            }
            else if (arg == "--quiet") {
                options.quiet = true;
                options.verbose = false;
            }
            else if (arg == "--no-report") {
                options.reportFormat = "none";
            }
            else if (arg == "--compare-before-after") {
                options.compareBeforeAfter = true;
            }
            else if (arg == "--baseline-steps" && i + 1 < argc) {
                options.baselineSteps = stoull(argv[++i]);
            }
            else if (arg == "--run-deep-analysis") {
                options.runDeepAnalysis = true;
            }
            else if (arg == "--no-full-demo") {
                options.runFullDemo = false;
            }
            else if (arg == "--run-quick-test") {
                options.runQuickVerification = true;
            }
            else if (arg == "--no-quick-test") {
                options.runQuickVerification = false;
            }
            else if (arg == "--no-deep-analysis") {
                options.runDeepAnalysis = false;
            }
            else if (arg == "--run-phase3-demo") {
                options.demoType = 3;
            }
            else if (arg == "--run-phase4-demo") {
                options.demoType = 4;
            }
            else if (arg == "--run-phase6-demo") {
                options.demoType = 6;
            }
            else {
                cout << "Unknown argument: " << arg << endl;
                printUsage(argv[0]);
                exit(1);
            }
        }
        
        return options;
    }
    
private:
    static void printUsage(const char* programName) {
        cout << "Usage: " << programName << " [options]" << endl;
        cout << "\nCore Options:" << endl;
        cout << "  --help                    Show this help message" << endl;
        cout << "  --version                 Show version information" << endl;
        cout << "  --config <file>           Configuration file" << endl;
        cout << "  --seed <seed>             Random seed" << endl;
        cout << "  --neurons <count>         Number of neurons" << endl;
        cout << "  --regions <count>         Number of brain regions" << endl;
        cout << "  --connection-prob <p>     Connection probability" << endl;
        cout << "  --timestep <dt>           Simulation timestep" << endl;
        cout << "  --max-steps <steps>       Maximum simulation steps" << endl;
        cout << "\nDemo Types:" << endl;
        cout << "  --run-phase3-demo         Run Phase 3 demo" << endl;
        cout << "  --run-phase4-demo         Run Phase 4 demo" << endl;
        cout << "  --run-phase6-demo         Run Phase 6 demo (default)" << endl;
        cout << "\nFeatures:" << endl;
        cout << "  --no-checkpointing        Disable checkpointing" << endl;
        cout << "  --no-replay               Disable replay" << endl;
        cout << "  --no-development          Disable developmental features" << endl;
        cout << "  --quiet                   Quiet mode" << endl;
        cout << "  --no-report               Disable report generation" << endl;
        cout << "  --compare-before-after    Compare performance before/after" << endl;
        cout << "  --baseline-steps <steps>  Number of baseline steps" << endl;
        cout << "  --run-deep-analysis       Run deep analysis mode" << endl;
    }
};

// Time formatting utilities
class TimeUtils {
public:
    static string formatDuration(double seconds) {
        if (seconds < 60.0) {
            return to_string(static_cast<int>(seconds)) + "s";
        }
        else if (seconds < 3600.0) {
            int minutes = static_cast<int>(seconds / 60.0);
            int remainingSeconds = static_cast<int>(seconds) % 60;
            return to_string(minutes) + "m " + to_string(remainingSeconds) + "s";
        }
        else {
            int hours = static_cast<int>(seconds / 3600.0);
            int remainingMinutes = static_cast<int>((seconds - hours * 3600.0) / 60.0);
            return to_string(hours) + "h " + to_string(remainingMinutes) + "m";
        }
    }
    
    static string getCurrentTimestamp() {
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        auto ms = chrono::duration_cast<chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        stringstream ss;
        ss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << setw(3) << setfill('0') << ms.count();
        return ss.str();
    }
};

// File utilities
class FileUtils {
public:
    static bool ensureDirectoryExists(const string& path) {
        // Create directories recursively if needed
        // Implementation would use mkdir -p equivalent
        return true;
    }
    
    static string getFileExtension(const string& filename) {
        size_t pos = filename.find_last_of(".");
        if (pos == string::npos) return "";
        return filename.substr(pos + 1);
    }
    
    static string removeFileExtension(const string& filename) {
        size_t pos = filename.find_last_of(".");
        if (pos == string::npos) return filename;
        return filename.substr(0, pos);
    }
};

// Math utilities
class MathUtils {
public:
    static double clamp(double value, double min, double max) {
        return std::max(min, std::min(max, value));
    }
    
    static double normalize(double value, double min, double max) {
        if (max <= min) return 0.0;
        return (value - min) / (max - min);
    }
    
    static double lerp(double a, double b, double t) {
        return a + (b - a) * t;
    }
    
    static double smoothStep(double edge0, double edge1, double x) {
        double t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
        return t * t * (3.0 - 2.0 * t);
    }
    
    static double sigmoid(double x) {
        return 1.0 / (1.0 + exp(-x));
    }
    
    static double gaussian(double x, double mean, double stddev) {
        double exponent = -0.5 * pow((x - mean) / stddev, 2.0);
        return (1.0 / (stddev * sqrt(2.0 * M_PI))) * exp(exponent);
    }
};

} // namespace utils
} // namespace nlm

#endif // NLM_UTILITIES_H

// Version with implementations for common demo patterns
namespace nlm {
namespace demo_utils {

class DemoBase {
protected:
    utils::DemoOptions options_;
    utils::ErrorHandler errorHandler_;
    utils::ProgressReporter progressReporter_;
    utils::MetricsCollector metricsCollector_;
    
    string currentDemoName_;
    double demoStartTime_;
    
public:
    DemoBase() : demoStartTime_(chrono::duration<double>::clock::now().time_since_epoch().count()) {}
    
    virtual ~DemoBase() = default;
    
    virtual bool initialize() = 0;
    virtual bool run() = 0;
    virtual bool cleanup() = 0;
    
    void setDemoName(const string& name) {
        currentDemoName_ = name;
        progressReporter_.setPrefix("[" + name + "]");
    }
    
    bool runDemo(utils::DemoOptions& options) {
        options_ = options;
        
        // Initialize based on demo type
        if (options.demoType == 3) {
            setDemoName("Phase 3");
        }
        else if (options.demoType == 4) {
            setDemoName("Phase 4");
        }
        else {
            setDemoName("Phase 6");
        }
        
        // Run initialization
        if (!initialize()) {
            errorHandler_.addError("Demo initialization failed", "",
                                0, utils::ErrorHandler::Severity::FATAL);
            return false;
        }
        
        // Record start time
        metricsCollector_.recordTime("demo_init", 
            chrono::duration<double>::clock::now().time_since_epoch().count() - demoStartTime_,
            {{ "demo", currentDemoName_ }});
        
        // Run main demo logic
        if (!run()) {
            errorHandler_.addError("Demo execution failed", "",
                                0, utils::ErrorHandler::Severity::ERROR);
            return false;
        }
        
        // Record completion time
        double executionTime = chrono::duration<double>::clock::now().time_since_epoch().count() - demoStartTime_;
        metricsCollector_.recordTime("demo_total", executionTime,
            {{ "demo", currentDemoName_ }});
        
        // Cleanup
        cleanup();
        
        // Print metrics
        metricsCollector_.printSummary();
        
        return !errorHandler_.hasErrors();
    }
    
    // Common configuration setup
    std::shared_ptr<Config> createConfig() {
        auto config = std::make_shared<Config>();
        
        // Basic brain configuration
        config->set("neuron_count", options_.neuronCount);
        config->set("region_count", options_.regionCount);
        config->set("connection_probability", options_.connectionProbability);
        config->set("random_seed", options_.randomSeed);
        config->set("simulation_timestep", options_.simulationTimestep);
        
        // Standard experiment parameters
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        
        return config;
    }
    
    // Common brain setup with error handling
    std::shared_ptr<Brain> createAndInitializeBrain() {
        auto config = createConfig();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            errorHandler_.addError("Failed to initialize brain", "",
                                0, utils::ErrorHandler::Severity::FATAL,
                                "This typically occurs due to invalid configuration");
            return nullptr;
        }
        
        return brain;
    }
    
    // Standardized brain logging
    void logBrainStatus(const std::shared_ptr<Brain>& brain) {
        if (!brain) return;
        
        errorHandler_.addError("=== Brain Status ===", "",
                            0, utils::ErrorHandler::Severity::INFO);
        errorHandler_.addError(string("Neurons: ") + to_string(brain->getTotalNeuronCount()), "",
                            0, utils::ErrorHandler::Severity::INFO);
        errorHandler_.addError(string("Synapses: ") + to_string(brain->getTotalSynapseCount()), "",
                            0, utils::ErrorHandler::Severity::INFO);
        errorHandler_.addError(string("Regions: ") + to_string(brain->getRegionCount()), "",
                            0, utils::ErrorHandler::Severity::INFO);
        errorHandler_.addError(string("Firing rate: ") + to_string(brain->getAverageFiringRate()), "",
                            0, utils::ErrorHandler::Severity::INFO);
    }
};

} // namespace demo_utils
} // namespace nlm

// Convenience function for printing formatted headers
inline void printDemoHeader(const string& title) {
    cout << "\n" << string(60, '=') << endl;
    cout << "  " << title << endl;
    cout << string(60, '=') << endl;
}

// Convenience function for printing formatted results
inline void printDemoResult(const string& name, double value, double max = 1.0, 
                           const string& unit = "") {
    cout << "  " << left << setw(30) << name << ": ";
    cout << fixed << setprecision(4) << value;
    if (max > 0) {
        cout << " (" << setprecision(1) << (value/max*100) << "%)";
    }
    if (!unit.empty()) {
        cout << " " << unit;
    }
    cout << endl;
}

#endif // NLM_DEMO_UTILITIES_H

// Implementation file for additional utilities
namespace nlm {
namespace demo_impl {

// Progress tracking with ETA calculation
class ProgressTracker {
private:
    chrono::system_clock::time_point startTime_;
    double progress_ = 0.0;
    string currentStep_;
    
public:
    ProgressTracker() : startTime_(chrono::system_clock::now()) {}
    
    void startStep(const string& step) {
        currentStep_ = step;
        progress_ = 0.0;
    }
    
    void updateProgress(double newProgress) {
        progress_ = newProgress;
        printProgress();
    }
    
    void endStep() {
        currentStep_ = "";
        progress_ = 0.0;
    }
    
    double getEstimatedTimeRemaining() const {
        if (progress_ <= 0.0 || progress_ >= 1.0) return -1.0;
        
        auto now = chrono::system_clock::now();
        auto elapsed = chrono::duration<double>(now - startTime_).count();
        double totalEstimated = elapsed / progress_;
        return totalEstimated - elapsed;
    }
    
private:
    void printProgress() {
        if (currentStep_.empty()) return;
        
        double eta = getEstimatedTimeRemaining();
        cout << "[" << currentStep_ << "] ";
        cout << fixed << setprecision(1) << (progress_ * 100.0) << "%";
        
        if (eta > 0.0) {
            cout << " ETA: " << utils::TimeUtils::formatDuration(eta);
        }
        
        cout << "\r" << flush;
    }
};

// Standardized experiment result structure
struct ExperimentResult {
    string experimentName;
    double reward;
    double fitness;
    double complexity;
    double novelty;
    size_t neuronsUsed;
    size_t synapsesCreated;
    double executionTime;
    chrono::system_clock::time_point timestamp;
    map<string, string> metadata;
    
    ExperimentResult(const string& name) 
        : experimentName(name), reward(0.0), fitness(0.0), complexity(0.0),
          novelty(0.0), neuronsUsed(0), synapsesCreated(0), 
          executionTime(0.0), timestamp(chrono::system_clock::now()) {}
    
    string toCSV() const {
        stringstream ss;
        ss << experimentName << ","
           << reward << ","
           << fitness << ","
           << complexity << ","
           << novelty << ","
           << neuronsUsed << ","
           << synapsesCreated << ","
           << executionTime << ","
           << utils::TimeUtils::getCurrentTimestamp();
        
        for (const auto& pair : metadata) {
            ss << "," << pair.first << "=" << pair.second;
        }
        
        return ss.str();
    }
    
    string toJSON() const {
        stringstream ss;
        ss << "{\n";
        ss << "  \"name\": \"" << experimentName << "\",\n";
        ss << "  \"reward\": " << reward << ",\n";
        ss << "  \"fitness\": " << fitness << ",\n";
        ss << "  \"complexity\": " << complexity << ",\n";
        ss << "  \"novelty\": " << novelty << ",\n";
        ss << "  \"neuronsUsed\": " << neuronsUsed << ",\n";
        ss << "  \"synapsesCreated\": " << synapsesCreated << ",\n";
        ss << "  \"executionTime\": " << executionTime << ",\n";
        ss << "  \"timestamp\": \"" << utils::TimeUtils::getCurrentTimestamp() << "\"\n";
        
        if (!metadata.empty()) {
            ss << ",\n  \"metadata\": {\n";
            bool first = true;
            for (const auto& pair : metadata) {
                if (!first) ss << ",\n";
                ss << "    \"" << pair.first << "\": \"" << pair.second << "\"";
                first = false;
            }
            ss << "\n  }\n";
        }
        
        ss << "}\n";
        return ss.str();
    }
};

// Standardized experiment runner with common patterns
class StandardExperimentRunner {
public:
    static ExperimentResult runStandardExperiment(
        const string& experimentName,
        std::function<void(double)> progressCallback,
        std::function<double()> getReward,
        std::function<void()> setupCallback,
        std::function<void()> cleanupCallback,
        size_t maxSteps = 1000,
        double timestep = 0.001) {
        
        ExperimentResult result(experimentName);
        auto startTime = chrono::high_resolution_clock::now();
        
        try {
            // Setup
            setupCallback();
            
            // Run experiment
            for (size_t step = 0; step < maxSteps; ++step) {
                // Update progress
                double progress = static_cast<double>(step) / maxSteps;
                progressCallback(progress);
                
                // Perform experiment step
                // This would be implemented by the specific experiment
                
                // Record reward at certain intervals
                if (step % 100 == 0) {
                    result.reward += getReward();
                }
                
                // Break if experiment indicates completion
                // (implementation-specific)
            }
            
            // Calculate final metrics
            result.fitness = result.reward / maxSteps;
            result.neuronsUsed = 0; // Would be set by experiment
            result.synapsesCreated = 0; // Would be set by experiment
            
            auto endTime = chrono::high_resolution_clock::now();
            result.executionTime = 
                chrono::duration<double>(endTime - startTime).count();
                
        } catch (const exception& e) {
            result.metadata["error"] = string("Exception: ") + e.what();
        }
        
        // Cleanup
        cleanupCallback();
        
        return result;
    }
};

} // namespace demo_impl
} // namespace nlm

// Final convenience functions for common demo operations
namespace nlm {
namespace demo_examples {

// Example of standardized error handling
inline bool handleError(const string& operation, const string& details = "") {
    utils::ErrorHandler::Severity severity = utils::ErrorHandler::Severity::ERROR;
    
    if (details.find("not implemented") != string::npos) {
        severity = utils::ErrorHandler::Severity::WARNING;
    }
    
    utils::ErrorHandler::getInstance().addError(
        operation, "", 0, severity, details);
    
    return severity != utils::ErrorHandler::Severity::FATAL;
}

// Example of standardized progress reporting
inline void demoProgress(const string& demoName, double progress, 
                        const string& status = "") {
    utils::ProgressReporter reporter;
    reporter.setPrefix("[" + demoName + "]");
    
    string message = status.empty() ? "" : "(" + status + ")";
    reporter.report("Progress " + to_string(static_cast<int>(progress * 100.0)) + "% " + message, progress);
}

// Example of standardized result printing
inline void printDemoResults(const vector<demo_impl::ExperimentResult>& results) {
    cout << "\n" << string(60, '=') << endl;
    cout << "  EXPERIMENT RESULTS SUMMARY" << endl;
    cout << string(60, '=') << endl;
    
    for (const auto& result : results) {
        cout << "\nExperiment: " << result.experimentName << endl;
        cout << "  Reward: " << fixed << setprecision(4) << result.reward << endl;
        cout << "  Fitness: " << result.fitness << endl;
        cout << "  Execution Time: " << utils::TimeUtils::formatDuration(result.executionTime) << endl;
        cout << "  Neurons Used: " << result.neuronsUsed << endl;
        cout << "  Synapses Created: " << result.synapsesCreated << endl;
        
        if (!result.metadata.empty()) {
            cout << "  Metadata:" << endl;
            for (const auto& pair : result.metadata) {
                cout << "    " << pair.first << ": " << pair.second << endl;
            }
        }
    }
}

} // namespace demo_examples
} // namespace nlm

// Export commonly used names
namespace nlm {
    using utils::DemoOptions;
    using utils::ErrorHandler;
    using utils::ProgressReporter;
    using utils::MetricsCollector;
    using utils::ConfigManager;
    using utils::TimeUtils;
    using utils::FileUtils;
    using utils::MathUtils;
    using demo_utils::DemoBase;
    using demo_impl::ProgressTracker;
    using demo_impl::ExperimentResult;
    using demo_impl::StandardExperimentRunner;
}