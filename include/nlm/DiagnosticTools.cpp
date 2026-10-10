#include "DiagnosticTools.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <pwd.h>
#include <cstring>

namespace nlm {

// SystemDiagnoser implementation
struct SystemDiagnoser::Impl {
    SystemInfo info;
    bool loggingEnabled = true;
    std::vector<std::string> diagnosticResults;
    mutable bool infoInitialized = false;
    std::mutex mutex;
};

SystemDiagnoser::SystemDiagnoser() : pImpl(std::make_unique<Impl>()) {
    // Initialize system info on demand
}

SystemDiagnoser::~SystemDiagnoser() = default;

SystemInfo SystemDiagnoser::getSystemInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->infoInitialized) {
        initializeSystemInfo();
    }
    
    return pImpl->info;
}

void SystemDiagnoser::Impl::initializeSystemInfo() {
    // Get OS information
    struct sysinfo sysInfo;
    sysinfo(&sysInfo);
    
    info.osName = "Linux"; // Simplified - in real implementation would detect OS
    info.cpuArchitecture = "x86_64"; // Simplified
    info.cpuCoreCount = std::thread::hardware_concurrency();
    info.cpuUsage = getCPUUsage();
    info.totalMemory = sysInfo.totalram * sysInfo.mem_unit;
    info.availableMemory = sysInfo.freeram * sysInfo.mem_unit;
    info.memoryUsage = (info.totalMemory - info.availableMemory) * 100.0 / info.totalMemory;
    
    // Calculate uptime
    info.uptime = std::to_string(sysInfo.uptime) + " seconds";
    
    info.timestamp = std::chrono::system_clock::now();
    infoInitialized = true;
}

double SystemDiagnoser::Impl::getCPUUsage() const {
    // Simplified CPU usage calculation
    // In a real implementation, this would parse /proc/stat
    static double lastTotal = 0.0;
    static double lastIdle = 0.0;
    static auto lastTime = std::chrono::steady_clock::now();
    
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(currentTime - lastTime).count();
    
    if (duration < 0.1) { // Don't update more frequently than once per 100ms
        return info.cpuUsage; // Return cached value
    }
    
    // In a real implementation, parse /proc/stat to get CPU times
    // For now, return a simulated value
    static double cpuUsage = 25.0; // 25% CPU usage
    
    lastTime = currentTime;
    return cpuUsage;
}

bool SystemDiagnoser::checkSystemHealth() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->infoInitialized) {
        initializeSystemInfo();
    }
    
    pImpl->diagnosticResults.clear();
    
    bool isHealthy = true;
    
    // Check CPU usage
    if (pImpl->info.cpuUsage > 90.0) {
        pImpl->diagnosticResults.push_back("High CPU usage: " + std::to_string(pImpl->info.cpuUsage) + "%");
        isHealthy = false;
    }
    
    // Check memory usage
    if (pImpl->info.memoryUsage > 90.0) {
        pImpl->diagnosticResults.push_back("High memory usage: " + std::to_string(pImpl->info.memoryUsage) + "%");
        isHealthy = false;
    }
    
    return isHealthy;
}

std::vector<std::string> SystemDiagnoser::getSystemHealthIssues() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->diagnosticResults;
}

double SystemDiagnoser::getCPUUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->infoInitialized) {
        initializeSystemInfo();
    }
    return pImpl->info.cpuUsage;
}

double SystemDiagnoser::getMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->infoInitialized) {
        initializeSystemInfo();
    }
    return pImpl->info.memoryUsage;
}

std::map<std::string, std::string> SystemDiagnoser::getSystemStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, std::string> stats;
    if (!pImpl->infoInitialized) {
        initializeSystemInfo();
    }
    
    stats["os"] = pImpl->info.osName;
    stats["cpu_cores"] = std::to_string(pImpl->info.cpuCoreCount);
    stats["cpu_usage"] = std::to_string(pImpl->info.cpuUsage) + "%";
    stats["total_memory"] = std::to_string(pImpl->info.totalMemory / (1024 * 1024)) + " MB";
    stats["available_memory"] = std::to_string(pImpl->info.availableMemory / (1024 * 1024)) + " MB";
    stats["memory_usage"] = std::to_string(pImpl->info.memoryUsage) + "%";
    stats["uptime"] = pImpl->info.uptime;
    
    return stats;
}

bool SystemDiagnoser::runDiagnostics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->diagnosticResults.clear();
    
    if (!pImpl->infoInitialized) {
        initializeSystemInfo();
    }
    
    // Run various diagnostics
    if (checkSystemHealth()) {
        pImpl->diagnosticResults.push_back("System health check passed");
    }
    
    if (pImpl->loggingEnabled) {
        pImpl->diagnosticResults.push_back("Diagnostics logging enabled");
    }
    
    return !pImpl->diagnosticResults.empty();
}

std::vector<std::string> SystemDiagnoser::getDiagnosticResults() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->diagnosticResults;
}

void SystemDiagnoser::enableLogging(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggingEnabled = enabled;
}

bool SystemDiagnoser::isLoggingEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->loggingEnabled;
}

SystemDiagnoser& SystemDiagnoser::getGlobal() {
    static SystemDiagnoser globalDiagnoser;
    return globalDiagnoser;
}

void SystemDiagnoser::setGlobal(std::shared_ptr<SystemDiagnoser> diagnoser) {
    // This is a placeholder for advanced global diagnoser management
}

// ComponentDiagnoser implementation
struct ComponentDiagnoser::Impl {
    std::map<std::string, ComponentInfo> components;
    bool loggingEnabled = true;
    std::vector<std::string> diagnosticResults;
    std::mutex mutex;
};

ComponentDiagnoser::ComponentDiagnoser() : pImpl(std::make_unique<Impl>()) {}

ComponentDiagnoser::~ComponentDiagnoser() = default;

void ComponentDiagnoser::registerComponent(const std::string& name,
                                          const std::string& version,
                                          const std::vector<std::string>& dependencies) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ComponentInfo info;
    info.name = name;
    info.version = version;
    info.status = "stopped";
    info.isHealthy = false;
    info.lastUpdate = std::chrono::system_clock::now();
    info.dependencies = dependencies;
    
    pImpl->components[name] = info;
}

void ComponentDiagnoser::unregisterComponent(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->components.erase(name);
}

std::optional<ComponentInfo> ComponentDiagnoser::getComponentInfo(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->components.find(name);
    if (it != pImpl->components.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::map<std::string, ComponentInfo> ComponentDiagnoser::getAllComponentInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->components;
}

bool ComponentDiagnoser::checkComponentHealth(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->components.find(name);
    if (it == pImpl->components.end()) {
        return false;
    }
    
    bool isHealthy = true;
    std::vector<std::string> issues;
    
    // Check component status
    if (it->second.status == "error") {
        isHealthy = false;
        issues.push_back("Component status: " + it->second.status);
    }
    
    // Check if healthy flag is set
    if (!it->second.isHealthy) {
        isHealthy = false;
        issues.push_back("Component health: not healthy");
    }
    
    // Store issues in diagnostic results
    if (!issues.empty()) {
        pImpl->diagnosticResults.push_back("Component " + name + ": " +
                                          (issues.size() > 1 ? "multiple issues" : issues[0]));
    }
    
    return isHealthy;
}

std::vector<std::string> ComponentDiagnoser::getComponentHealthIssues(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> issues;
    auto it = pImpl->components.find(name);
    if (it == pImpl->components.end()) {
        issues.push_back("Component not found");
        return issues;
    }
    
    // Basic health checks
    if (it->second.status == "error") {
        issues.push_back("Status: " + it->second.status);
    }
    
    if (!it->second.isHealthy) {
        issues.push_back("Health: unhealthy - " + it->second.healthMessage);
    }
    
    return issues;
}

bool ComponentDiagnoser::runComponentDiagnostics(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->components.find(name);
    if (it == pImpl->components.end()) {
        pImpl->diagnosticResults.push_back("Error: Component not found: " + name);
        return false;
    }
    
    // Update component state
    it->second.lastUpdate = std::chrono::system_clock::now();
    
    // Run basic diagnostics
    bool isHealthy = checkComponentHealth(name);
    
    if (isHealthy) {
        pImpl->diagnosticResults.push_back("Component " + name + ": health check passed");
    }
    
    return isHealthy;
}

std::vector<std::string> ComponentDiagnoser::getComponentDiagnosticResults(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> results;
    
    // Get component health issues
    auto healthIssues = getComponentHealthIssues(name);
    results.insert(results.end(), healthIssues.begin(), healthIssues.end());
    
    // Add any general diagnostic results
    results.insert(results.end(), pImpl->diagnosticResults.begin(), pImpl->diagnosticResults.end());
    
    return results;
}

void ComponentDiagnoser::updateComponentState(const std::string& name,
                                             const std::string& status,
                                             const std::map<std::string, std::string>& properties) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->components.find(name);
    if (it != pImpl->components.end()) {
        it->second.status = status;
        it->second.lastUpdate = std::chrono::system_clock::now();
        it->second.properties = properties;
        
        // Update health based on status
        if (status == "running") {
            it->second.isHealthy = true;
            it->second.healthMessage = "Running normally";
        } else if (status == "error") {
            it->second.isHealthy = false;
            it->second.healthMessage = "Component in error state";
        }
    }
}

ComponentDiagnoser& ComponentDiagnoser::getGlobal() {
    static ComponentDiagnoser globalDiagnoser;
    return globalDiagnoser;
}

void ComponentDiagnoser::setGlobal(std::shared_ptr<ComponentDiagnoser> diagnoser) {
    // This is a placeholder for advanced global diagnoser management
}

// ConfigurationDiagnoser implementation
struct ConfigurationDiagnoser::Impl {
    std::map<std::string, ConfigurationInfo> configurations;
    bool loggingEnabled = true;
    std::mutex mutex;
};

ConfigurationDiagnoser::ConfigurationDiagnoser() : pImpl(std::make_unique<Impl>()) {}

ConfigurationDiagnoser::~ConfigurationDiagnoser() = default;

void ConfigurationDiagnoser::addConfigurationSource(const std::string& source,
                                                  const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ConfigurationInfo info;
    info.source = source;
    info.values = config;
    info.timestamp = std::chrono::system_clock::now();
    
    pImpl->configurations[source] = info;
}

void ConfigurationDiagnoser::removeConfigurationSource(const std::string& source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->configurations.erase(source);
}

std::optional<ConfigurationInfo> ConfigurationDiagnoser::getConfigurationInfo(
    const std::string& source) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->configurations.find(source);
    if (it != pImpl->configurations.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::map<std::string, ConfigurationInfo> ConfigurationDiagnoser::getAllConfigurationInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->configurations;
}

bool ConfigurationDiagnoser::validateConfiguration(const std::string& source) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->configurations.find(source);
    if (it == pImpl->configurations.end()) {
        return false;
    }
    
    const auto& config = it->second;
    bool isValid = true;
    
    // Basic validation - in a real implementation would do more thorough checks
    for (const auto& pair : config.values) {
        if (pair.first.empty() || pair.second.empty()) {
            isValid = false;
            config.errors.push_back("Empty key or value: " + pair.first);
        }
    }
    
    return isValid;
}

std::vector<std::string> ConfigurationDiagnoser::getConfigurationErrors(
    const std::string& source) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> errors;
    
    auto it = pImpl->configurations.find(source);
    if (it == pImpl->configurations.end()) {
        errors.push_back("Source not found: " + source);
        return errors;
    }
    
    // Validate configuration
    if (!validateConfiguration(source)) {
        errors = it->second.errors;
    }
    
    return errors;
}

std::vector<std::string> ConfigurationDiagnoser::getConfigurationWarnings(
    const std::string& source) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> warnings;
    auto it = pImpl->configurations.find(source);
    if (it != pImpl->configurations.end()) {
        // Add warning logic here - for now return empty
    }
    return warnings;
}

void ConfigurationDiagnoser::mergeConfigurations(const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ConfigurationInfo info;
    info.source = "merged";
    info.values = config;
    info.timestamp = std::chrono::system_clock::now();
    
    pImpl->configurations["merged"] = info;
}

ConfigurationDiagnoser& ConfigurationDiagnoser::getGlobal() {
    static ConfigurationDiagnoser globalDiagnoser;
    return globalDiagnoser;
}

void ConfigurationDiagnoser::setGlobal(std::shared_ptr<ConfigurationDiagnoser> diagnoser) {
    // This is a placeholder for advanced global diagnoser management
}

// DependencyDiagnoser implementation
struct DependencyDiagnoser::Impl {
    std::map<std::string, DependencyInfo> dependencies;
    bool loggingEnabled = true;
    std::vector<std::string> scanResults;
    std::mutex mutex;
};

DependencyDiagnoser::DependencyDiagnoser() : pImpl(std::make_unique<Impl>()) {}

DependencyDiagnoser::~DependencyDiagnoser() = default;

void DependencyDiagnoser::addDependency(const std::string& name,
                                        const std::string& version,
                                        const std::string& path) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    DependencyInfo info;
    info.name = name;
    info.version = version;
    info.path = path;
    info.found = !path.empty();
    info.isLoaded = false;
    info.discoveryTime = std::chrono::system_clock::now();
    
    pImpl->dependencies[name] = info;
}

void DependencyDiagnoser::removeDependency(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->dependencies.erase(name);
}

std::optional<DependencyInfo> DependencyDiagnoser::getDependencyInfo(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependencies.find(name);
    if (it != pImpl->dependencies.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::map<std::string, DependencyInfo> DependencyDiagnoser::getAllDependencyInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->dependencies;
}

bool DependencyDiagnoser::checkDependencyHealth(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependencies.find(name);
    if (it == pImpl->dependencies.end()) {
        return false;
    }
    
    const auto& dep = it->second;
    bool isHealthy = dep.found && !dep.errors.empty() && dep.isLoaded;
    
    return isHealthy;
}

std::vector<std::string> DependencyDiagnoser::getDependencyHealthIssues(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> issues;
    
    auto it = pImpl->dependencies.find(name);
    if (it == pImpl->dependencies.end()) {
        issues.push_back("Dependency not found: " + name);
        return issues;
    }
    
    const auto& dep = it->second;
    
    if (!dep.found) {
        issues.push_back("Dependency not found: " + name);
    }
    
    if (!dep.isLoaded) {
        issues.push_back("Dependency not loaded: " + name);
    }
    
    issues.insert(issues.end(), dep.errors.begin(), dep.errors.end());
    
    return issues;
}

bool DependencyDiagnoser::runDependencyDiagnostics(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependencies.find(name);
    if (it == pImpl->dependencies.end()) {
        pImpl->scanResults.push_back("Error: Dependency not found: " + name);
        return false;
    }
    
    DependencyInfo& dep = it->second;
    
    // Simulate dependency check
    if (dep.found) {
        dep.isLoaded = true;
        pImpl->scanResults.push_back("Dependency " + name + ": loaded successfully");
        return true;
    } else {
        dep.errors.push_back("Dependency not found in system");
        pImpl->scanResults.push_back("Error: Dependency not found: " + name);
        return false;
    }
}

std::vector<std::string> DependencyDiagnoser::getDependencyDiagnosticResults(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->scanResults;
}

void DependencyDiagnoser::scanDependencies() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->scanResults.clear();
    
    for (auto& pair : pImpl->dependencies) {
        DependencyInfo& dep = pair.second;
        dep.discoveryTime = std::chrono::system_clock::now();
        
        // Simulate dependency discovery
        if (dep.found) {
            dep.isLoaded = true;
            pImpl->scanResults.push_back("Found: " + dep.name + " (v" + dep.version + ")");
        } else {
            pImpl->scanResults.push_back("Missing: " + dep.name);
        }
    }
}

std::vector<std::string> DependencyDiagnoser::getScanResults() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->scanResults;
}

DependencyDiagnoser& DependencyDiagnoser::getGlobal() {
    static DependencyDiagnoser globalDiagnoser;
    return globalDiagnoser;
}

void DependencyDiagnoser::setGlobal(std::shared_ptr<DependencyDiagnoser> diagnoser) {
    // This is a placeholder for advanced global diagnoser management
}

// ErrorDiagnoser implementation
struct ErrorDiagnoser::Impl {
    std::vector<ErrorInfo> errors;
    std::map<std::string, size_t> errorCounts;
    bool loggingEnabled = true;
    std::mutex mutex;
};

ErrorDiagnoser::ErrorDiagnoser() : pImpl(std::make_unique<Impl>()) {}

ErrorDiagnoser::~ErrorDiagnoser() = default;

void ErrorDiagnoser::logError(const std::string& type, const std::string& message,
                             const std::string& file, int line,
                             const std::string& function,
                             const std::map<std::string, std::string>& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ErrorInfo info;
    info.id = std::to_string(pImpl->errors.size() + 1);
    info.type = type;
    info.message = message;
    info.file = file;
    info.line = line;
    info.function = function;
    info.timestamp = std::chrono::system_clock::now();
    info.context = context;
    info.isResolved = false;
    info.resolutionMethod = "";
    
    pImpl->errors.push_back(info);
    pImpl->errorCounts[type]++;
}

std::optional<ErrorInfo> ErrorDiagnoser::getErrorInfo(const std::string& id) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& error : pImpl->errors) {
        if (error.id == id) {
            return error;
        }
    }
    return std::nullopt;
}

std::vector<ErrorInfo> ErrorDiagnoser::getAllErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errors;
}

std::vector<ErrorInfo> ErrorDiagnoser::getErrorsByType(const std::string& type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<ErrorInfo> results;
    for (const auto& error : pImpl->errors) {
        if (error.type == type) {
            results.push_back(error);
        }
    }
    return results;
}

bool ErrorDiagnoser::detectErrorPattern(const std::string& pattern) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& error : pImpl->errors) {
        if (error.message.find(pattern) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> ErrorDiagnoser::getDetectedPatterns() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> patterns;
    // Simple pattern detection based on error types
    std::set<std::string> uniqueTypes;
    for (const auto& error : pImpl->errors) {
        if (uniqueTypes.find(error.type) == uniqueTypes.end()) {
            uniqueTypes.insert(error.type);
            patterns.push_back("Pattern: " + error.type + " (count: " + 
                             std::to_string(pImpl->errorCounts.at(error.type)) + ")");
        }
    }
    return patterns;
}

std::map<std::string, int> ErrorDiagnoser::getErrorCountByType(
    size_t timeWindowSeconds) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, int> counts;
    auto currentTime = std::chrono::system_clock::now();
    auto timeWindow = std::chrono::seconds(timeWindowSeconds);
    
    for (const auto& error : pImpl->errors) {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - error.timestamp).count();
        
        if (age <= static_cast<double>(timeWindowSeconds)) {
            counts[error.type]++;
        }
    }
    
    return counts;
}

std::map<std::string, double> ErrorDiagnoser::getErrorRateByType(
    size_t timeWindowSeconds) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, double> rates;
    auto currentTime = std::chrono::system_clock::now();
    auto timeWindow = std::chrono::seconds(timeWindowSeconds);
    
    size_t totalErrors = 0;
    for (const auto& error : pImpl->errors) {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - error.timestamp).count();
        
        if (age <= static_cast<double>(timeWindowSeconds)) {
            totalErrors++;
        }
    }
    
    double errorRate = totalErrors / static_cast<double>(timeWindowSeconds);
    
    for (const auto& pair : pImpl->errorCounts) {
        rates[pair.first] = pair.second * errorRate;
    }
    
    return rates;
}

bool ErrorDiagnoser::resolveError(const std::string& id, const std::string& method) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& error : pImpl->errors) {
        if (error.id == id) {
            error.isResolved = true;
            error.resolutionMethod = method;
            return true;
        }
    }
    return false;
}

void ErrorDiagnoser::clearErrors() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errors.clear();
    pImpl->errorCounts.clear();
}

void ErrorDiagnoser::clearErrorsByType(const std::string& type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<ErrorInfo> remainingErrors;
    for (const auto& error : pImpl->errors) {
        if (error.type != type) {
            remainingErrors.push_back(error);
        }
    }
    
    pImpl->errors = remainingErrors;
    pImpl->errorCounts.erase(type);
}

ErrorDiagnoser& ErrorDiagnoser::getGlobal() {
    static ErrorDiagnoser globalDiagnoser;
    return globalDiagnoser;
}

void ErrorDiagnoser::setGlobal(std::shared_ptr<ErrorDiagnoser> diagnoser) {
    // This is a placeholder for advanced global diagnoser management
}

// PerformanceDiagnoser implementation
struct PerformanceDiagnoser::Impl {
    std::vector<PerformanceInfo> performanceData;
    std::map<std::string, double> thresholds;
    bool loggingEnabled = true;
    std::mutex mutex;
};

PerformanceDiagnoser::PerformanceDiagnoser() : pImpl(std::make_unique<Impl>()) {}

PerformanceDiagnoser::~PerformanceDiagnoser() = default;

void PerformanceDiagnoser::logPerformance(const PerformanceInfo& perfInfo) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->performanceData.push_back(perfInfo);
}

std::optional<PerformanceInfo> PerformanceDiagnoser::getPerformanceInfo(
    const std::string& operationName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& perf : pImpl->performanceData) {
        if (perf.operationName == operationName) {
            return perf;
        }
    }
    return std::nullopt;
}

std::vector<PerformanceInfo> PerformanceDiagnoser::getAllPerformanceInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->performanceData;
}

std::vector<PerformanceInfo> PerformanceDiagnoser::getRecentPerformanceInfo(
    size_t maxEntries) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<PerformanceInfo> recent;
    size_t count = std::min(maxEntries, pImpl->performanceData.size());
    
    if (count > 0) {
        recent.reserve(count);
        auto startIt = pImpl->performanceData.end() - count;
        recent.insert(recent.begin(), startIt, pImpl->performanceData.end());
    }
    
    return recent;
}

std::map<std::string, double> PerformanceDiagnoser::getAveragePerformanceByOperation(
    size_t timeWindowSeconds) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, double> averages;
    auto currentTime = std::chrono::system_clock::now();
    auto timeWindow = std::chrono::seconds(timeWindowSeconds);
    
    std::map<std::string, std::vector<double>> operationTimes;
    
    for (const auto& perf : pImpl->performanceData) {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - perf.timestamp).count();
        
        if (age <= static_cast<double>(timeWindowSeconds)) {
            operationTimes[perf.operationName].push_back(perf.duration);
        }
    }
    
    for (const auto& pair : operationTimes) {
        double sum = std::accumulate(pair.second.begin(), pair.second.end(), 0.0);
        averages[pair.first] = sum / pair.second.size();
    }
    
    return averages;
}

std::map<std::string, double> PerformanceDiagnoser::getPerformanceTrend(
    const std::string& operationName, size_t dataPoints) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, double> trend;
    
    std::vector<double> durations;
    for (const auto& perf : pImpl->performanceData) {
        if (perf.operationName == operationName) {
            durations.push_back(perf.duration);
        }
    }
    
    if (durations.size() > 1) {
        size_t count = std::min(dataPoints, durations.size());
        double sum = std::accumulate(durations.end() - count, durations.end(), 0.0);
        double average = sum / count;
        trend["average"] = average;
        
        // Calculate trend (simple linear regression)
        if (count > 1) {
            double n = static_cast<double>(count);
            double xSum = n * (n - 1) / 2.0;
            double ySum = sum;
            double xySum = 0.0;
            double y2Sum = 0.0;
            
            for (size_t i = 0; i < count; ++i) {
                double y = durations[i];
                xySum += i * y;
                y2Sum += y * y;
            }
            
            double slope = (n * xySum - xSum * ySum) / (n * xSum - xSum * xSum);
            trend["slope"] = slope;
            trend["r_squared"] = 1.0 - (y2Sum - ySum * ySum / n) / (y2Sum - ySum * ySum / n);
        }
    }
    
    return trend;
}

bool PerformanceDiagnoser::detectPerformanceBottleneck(
    const std::string& operationName, double threshold) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto avgPerf = getAveragePerformanceByOperation();
    
    auto it = avgPerf.find(operationName);
    if (it == avgPerf.end()) {
        return false;
    }
    
    double thresholdVal = threshold;
    auto threshIt = pImpl->thresholds.find(operationName);
    if (threshIt != pImpl->thresholds.end()) {
        thresholdVal = threshIt->second;
    }
    
    return it->second > thresholdVal;
}

std::vector<std::string> PerformanceDiagnoser::getPerformanceIssues() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> issues;
    
    auto avgPerf = getAveragePerformanceByOperation();
    for (const auto& pair : avgPerf) {
        if (detectPerformanceBottleneck(pair.first)) {
            issues.push_back("Performance bottleneck detected: " + pair.first +
                            " (average: " + std::to_string(pair.second) + "s)");
        }
    }
    
    return issues;
}

void PerformanceDiagnoser::setPerformanceThreshold(const std::string& operationName,
                                                   double threshold) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->thresholds[operationName] = threshold;
}


double PerformanceDiagnoser::getPerformanceThreshold(const std::string& operationName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->thresholds.find(operationName);
    if (it != pImpl->thresholds.end()) {
        return it->second;
    }
    
    // Return default threshold based on operation name
    if (operationName.find("step") != std::string::npos) {
        return 0.1; // 100ms threshold for steps
    } else if (operationName.find("memory") != std::string::npos) {
        return 100.0; // 100MB threshold for memory operations
    }
    
    return 1.0; // 1 second default threshold
}

void PerformanceDiagnoser::clearPerformanceData() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceData.clear();
}

PerformanceDiagnoser& PerformanceDiagnoser::getGlobal() {
    static PerformanceDiagnoser globalDiagnoser;
    return globalDiagnoser;
}

void PerformanceDiagnoser::setGlobal(std::shared_ptr<PerformanceDiagnoser> diagnoser) {
    // This is a placeholder for advanced global diagnoser management
}

} // namespace nlm
