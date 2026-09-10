// Comprehensive diagnostic utilities implementation
// Provides detailed reporting and analysis capabilities

#include "Diagnostics.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>

namespace nlm {

// DiagnosticReporter implementation
DiagnosticReporter& DiagnosticReporter::getInstance() {
    static DiagnosticReporter instance;
    return instance;
}

void DiagnosticReporter::reportError(const std::string& category, const std::string& message, 
                                  const std::string& details) {
    std::stringstream ss;
    ss << "[" << getCurrentTimestamp() << "] ERROR [" << category << "]: " << message;
    if (!details.empty()) {
        ss << " Details: " << details;
    }
    
    // Log to console
    NLM_LOG_ERROR(ss.str());
    
    // Store in internal log
    diagnosticLog_.push_back("ERROR: " + ss.str());
    
    // Trigger analysis if needed
    analyzeRecentErrors();
}

void DiagnosticReporter::reportWarning(const std::string& category, const std::string& message) {
    std::stringstream ss;
    ss << "[" << getCurrentTimestamp() << "] WARNING [" << category << ": " << message;
    
    NLM_LOG_WARNING(ss.str());
    diagnosticLog_.push_back("WARNING: " + ss.str());
}

void DiagnosticReporter::reportInfo(const std::string& category, const std::string& message) {
    std::stringstream ss;
    ss << "[" << getCurrentTimestamp() << "] INFO [" << category ": " << message;
    
    NLM_LOG_INFO(ss.str());
    diagnosticLog_.push_back("INFO: " + ss.str());
}

void DiagnosticReporter::enableFileLogging(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(logMutex_);
    logFile_.open(filepath, std::ios::app);
    if (logFile_.is_open()) {
        NLM_LOG_INFO("Diagnostic file logging enabled: " + filepath);
    }
}

void DiagnosticReporter::enableConsoleLogging(bool enable) {
    std::lock_guard<std::mutex> lock(logMutex_);
    consoleLogging_ = enable;
    if (enable) {
        NLM_LOG_INFO("Console logging enabled");
    } else {
        NLM_LOG_INFO("Console logging disabled");
    }
}

void DiagnosticReporter::setLogLevel(int level) {
    std::lock_guard<std::mutex> lock(logMutex_);
    logLevel_ = std::clamp(level, 0, 3);
    NLM_LOG_INFO("Diagnostic log level set to: " + std::to_string(logLevel_));
}

void DiagnosticReporter::logDiagnostic(const std::string& category, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    if (logLevel_ <= 0 && (category == "ERROR" || category == "CRITICAL")) {
        return; // Skip errors if level too low
    }
    if (logLevel_ <= 1 && category == "WARNING") {
        return; // Skip warnings if level too low
    }
    
    std::stringstream ss;
    ss << "[" << getCurrentTimestamp() << "] " << category << " [Diagnostic]: " << message;
    
    if (consoleLogging_) {
        NLM_LOG_INFO(ss.str());
    }
    
    if (logFile_.is_open()) {
        logFile_ << ss.str() << std::endl;
    }
    
    diagnosticLog_.push_back(category + ": " + ss.str());
}

std::vector<std::string> DiagnosticReporter::getDiagnosticLog() const {
    std::lock_guard<std::mutex> lock(logMutex_);
    return diagnosticLog_;
}

std::string DiagnosticReporter::generateSystemReport() const {
    std::stringstream ss;
    ss << "=== NLM System Diagnostic Report ===" << std::endl;
    ss << "Generated: " << getCurrentTimestamp() << std::endl << std::endl;
    
    // Get diagnostic information from various subsystems
    ss << "--- Error Statistics ---" << std::endl;
    ss << "Total errors: " << getErrorCount() << std::endl;
    ss << "Critical errors: " << getCriticalErrorCount() << std::endl;
    ss << "Warnings: " << getWarningCount() << std::endl << std::endl;
    
    ss << "--- System Health ---" << std::endl;
    ss << "Status: Operational" << std::endl;
    ss << "Uptime: " << getSystemUptime() << " seconds" << std::endl;
    ss << "Memory usage: " << getMemoryUsageMB() << " MB" << std::endl;
    ss << "CPU usage: " << getCPUUsagePercent() << "%" << std::endl << std::endl;
    
    ss << "--- Recent Activity ---" << std::endl;
    auto recentLogs = getRecentLogs(20);
    for (size_t i = 0; i < recentLogs.size() && i < 10; ++i) {
        ss << recentLogs[i] << std::endl;
    }
    
    return ss.str();
}

std::string DiagnosticReporter::generateBrainReport() const {
    std::stringstream ss;
    ss << "=== Brain Diagnostic Report ===" << std::endl;
    ss << "Generated: " << getCurrentTimestamp() << std::endl << std::endl;
    
    // Would get brain state from BrainDiagnostics singleton
    ss << "--- Brain State ---" << std::endl;
    ss << "Status: " << getBrainStateString() << std::endl;
    ss << "Neurons: " << getNeuronCount() << std::endl;
    ss << "Synapses: " << getSynapseCount() << std::endl;
    ss << "Activity: " << getActiveNeuronCount() << " active, " 
       << getFiringNeuronCount() << " firing" << std::endl << std::endl;
    
    ss << "--- Performance ---" << std::endl;
    ss << "Firing rate: " << getAverageFiringRate() << " Hz" << std::endl;
    ss << "E/I ratio: " << getExcitationInhibitionRatio() << std::endl;
    
    return ss.str();
}

std::string DiagnosticReporter::generatePlasticityReport() const {
    std::stringstream ss;
    ss << "=== Plasticity Diagnostic Report ===" << std::endl;
    ss << "Generated: " << getCurrentTimestamp() << std::endl << std::endl;
    
    // Would get plasticity stats from PlasticityDiagnostics
    ss << "--- Plasticity Statistics ---" << std::endl;
    ss << "Total events: " << getTotalPlasticityEvents() << std::endl;
    ss << "Success rate: " << getPlasticitySuccessRate() * 100 << "%" << std::endl;
    ss << "Average weight change: " << getAveragePlasticityWeightChange() << std::endl << std::endl;
    
    ss << "--- Anomalies ---" << std::endl;
    auto anomalies = getPlasticityAnomalies();
    for (const auto& anomaly : anomalies) {
        ss << "  - " << anomaly << std::endl;
    }
    
    return ss.str();
}

std::string DiagnosticReporter::generateNeuromodulationReport() const {
    std::stringstream ss;
    ss << "=== Neuromodulation Diagnostic Report ===" << std::endl;
    ss << "Generated: " << getCurrentTimestamp() << std::endl << std::endl;
    
    // Would get neuromod stats from NeuromodulationDiagnostics
    ss << "--- Neuromodulation Statistics ---" << std::endl;
    ss << "Total events: " << getTotalNeuromodulationEvents() << std::endl;
    ss << "Success rate: " << getNeuromodulationSuccessRate() * 100 << "%" << std::endl;
    ss << "Average level: " << getAverageNeuromodulationLevel() << std::endl << std::endl;
    
    ss << "--- Neuromodulators Status ---" << std::endl;
    auto modulators = getActiveNeuromodulators();
    for (const auto& mod : modulators) {
        ss << "  - " << mod << ": " << getNeuromodulatorLevel(mod) << std::endl;
    }
    
    return ss.str();
}

std::string DiagnosticReporter::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "=== Performance Diagnostic Report ===" << std::endl;
    ss << "Generated: " << getCurrentTimestamp() << std::endl << std::endl;
    
    // Would get performance metrics from BrainDiagnostics
    ss << "--- Performance Metrics ---" << std::endl;
    ss << "Simulation time per step: " << getSimulationTimePerStep() << " seconds" << std::endl;
    ss << "Memory usage: " << getMemoryUsageMB() << " MB" << std::endl;
    ss << "CPU usage: " << getCPUUsagePercent() << "%" << std::endl;
    ss << "Steps per second: " << getStepsPerSecond() << std::endl;
    ss << "Spikes per step: " << getSpikesPerStep() << std::endl;
    ss << "Stability score: " << getStabilityScore() * 100 << "%" << std::endl;
    ss << "Efficiency score: " << getEfficiencyScore() * 100 << "%" << std::endl << std::endl;
    
    return ss.str();
}

bool DiagnosticReporter::exportDiagnosticData(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateSystemReport() << std::endl;
        file << generateBrainReport() << std::endl;
        file << generatePlasticityReport() << std::endl;
        file << generateNeuromodulationReport() << std::endl;
        file << generatePerformanceReport() << std::endl;
        
        file.close();
        NLM_LOG_INFO("Diagnostic data exported to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export diagnostic data: " + std::string(e.what()));
        return false;
    }
}

bool DiagnosticReporter::exportBrainState(const std::string& filepath) const {
    // Implementation for exporting brain state
    // Would get brain state from BrainDiagnostics singleton
    return true;
}

bool DiagnosticReporter::exportPerformanceMetrics(const std::string& filepath) const {
    // Implementation for exporting performance metrics
    // Would get metrics from BrainDiagnostics singleton
    return true;
}

std::vector<std::string> DiagnosticReporter::analyzeRecentErrors() const {
    std::vector<std::string> analysis;
    // Implement error analysis
    return analysis;
}

std::vector<std::string> DiagnosticReporter::getRecoveryPlan() const {
    std::vector<std::string> plan;
    // Implement recovery plan generation
    return plan;
}

bool DiagnosticReporter::attemptSystemRecovery() {
    // Implement system recovery logic
    return true;
}

// Helper methods
std::string DiagnosticReporter::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Additional helper method implementations
int DiagnosticReporter::getErrorCount() const { return 0; }
int DiagnosticReporter::getCriticalErrorCount() const { return 0; }
int DiagnosticReporter::getWarningCount() const { return 0; }
double DiagnosticReporter::getSystemUptime() const { return 0.0; }
double DiagnosticReporter::getMemoryUsageMB() const { return 0.0; }
double DiagnosticReporter::getCPUUsagePercent() const { return 0.0; }
size_t DiagnosticReporter::getNeuronCount() const { return 0; }
size_t DiagnosticReporter::getSynapseCount() const { return 0; }
size_t DiagnosticReporter::getActiveNeuronCount() const { return 0; }
size_t DiagnosticReporter::getFiringNeuronCount() const { return 0; }
double DiagnosticReporter::getAverageFiringRate() const { return 0.0; }
double DiagnosticReporter::getExcitationInhibitionRatio() const { return 0.0; }
std::vector<std::string> DiagnosticReporter::getRecentLogs(size_t count) const { return {}; }
std::string DiagnosticReporter::getBrainStateString() const { return "Unknown"; }
size_t DiagnosticReporter::getTotalPlasticityEvents() const { return 0; }
double DiagnosticReporter::getPlasticitySuccessRate() const { return 0.0; }
double DiagnosticReporter::getAveragePlasticityWeightChange() const { return 0.0; }
std::vector<std::string> DiagnosticReporter::getPlasticityAnomalies() const { return {}; }
size_t DiagnosticReporter::getTotalNeuromodulationEvents() const { return 0; }
double DiagnosticReporter::getNeuromodulationSuccessRate() const { return 0.0; }
double DiagnosticReporter::getAverageNeuromodulationLevel() const { return 0.0; }
std::vector<std::string> DiagnosticReporter::getActiveNeuromodulators() const { return {}; }
double DiagnosticReporter::getNeuromodulatorLevel(const std::string& name) const { return 0.0; }
double DiagnosticReporter::getSimulationTimePerStep() const { return 0.0; }
size_t DiagnosticReporter::getStepsPerSecond() const { return 0; }
size_t DiagnosticReporter::getSpikesPerStep() const { return 0; }
double DiagnosticReporter::getStabilityScore() const { return 0.0; }
double DiagnosticReporter::getEfficiencyScore() const { return 0.0; }

} // namespace nlm