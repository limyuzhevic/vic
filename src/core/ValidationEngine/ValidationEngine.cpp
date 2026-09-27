#include "ValidationEngine.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <thread>
#include <chrono>
#include <future>

namespace nlm {

void ValidationStatistics::recordValidation(bool success, const std::string& context) {
    totalValidations++;
    lastValidationTime = std::chrono::system_clock::now();
    
    if (success) {
        successfulValidations++;
    } else {
        failedValidations++;
    }
    
    if (!context.empty()) {
        validationCounts[context]++;
    }
}

void ValidationStatistics::recordRecovery(bool success, double recoveryTime) {
    recoveryAttempts++;
    if (success) {
        successfulRecoveries++;
    }
    
    if (recoveryAttempts > 0) {
        averageRecoveryTime = ((averageRecoveryTime * (successfulRecoveries - (success ? 1 : 0))) + recoveryTime) / recoveryAttempts;
    }
}

double ValidationStatistics::getSuccessRate() const {
    if (totalValidations == 0) return 0.0;
    return static_cast<double>(successfulValidations) / totalValidations;
}

size_t EnhancedValidationResult::getErrorCount(ErrorSeverity severity) const {
    return std::count_if(errors.begin(), errors.end(),
        [severity](const std::shared_ptr<Error>& error) {
            return error->getSeverity() == severity;
        });
}

size_t EnhancedValidationResult::getWarningCount(ErrorSeverity severity) const {
    return std::count_if(warnings.begin(), warnings.end(),
        [severity](const std::shared_ptr<Error>& warning) {
            return warning->getSeverity() == severity;
        });
}

struct ValidationEngine::Impl {
    std::vector<ValidationRule> rules;
    std::map<std::string, std::map<std::string, std::string>> config;
    ValidationConfig engineConfig;
    ValidationStatistics stats;
    std::map<std::string, std::shared_ptr<IRecoveryStrategy>> recoveryStrategies;
    std::shared_ptr<ErrorReporter> errorReporter;
    std::mutex mutex;
    bool threadSafe;
    bool batchValidationEnabled;
    std::map<std::string, std::chrono::steady_clock::time_point> lastValidationTimes;
    std::map<std::string, std::thread> validationThreads;
    std::map<std::string, std::promise<EnhancedValidationResult>> validationPromises;

    Impl(const ValidationConfig& config) : engineConfig(config), threadSafe(config.threadSafe), batchValidationEnabled(true) {
        recoveryStrategies["Default"] = std::make_shared<DefaultRecoveryStrategy>();
        recoveryStrategies["Memory"] = std::make_shared<MemoryRecoveryStrategy>();
        recoveryStrategies["File"] = std::make_shared<FileRecoveryStrategy>();
        recoveryStrategies["Configuration"] = std::make_shared<ConfigurationRecoveryStrategy>();
    }

    bool validateAgainstRules(const ValidationContext& context) {
        for (const auto& rule : rules) {
            try {
                if (!rule.validator(context.data)) {
                    return false;
                }
            } catch (const std::exception& e) {
                // Rule execution failed
                return false;
            }
        }
        return true;
    }

    EnhancedValidationResult applyRecovery(const EnhancedValidationResult& result) {
        EnhancedValidationResult recoveryResult = result;
        
        for (const auto& error : result.errors) {
            bool recovered = false;
            for (const auto& strategy : recoveryStrategies) {
                if (strategy.second->canRecover(error)) {
                    recovered = strategy.second->recover(const_cast<std::shared_ptr<Error>&>(error));
                    if (recovered) {
                        recoveryResult.recoveryApplied = true;
                        break;
                    }
                }
            }
        }
        
        return recoveryResult;
    }

    void recordValidationResult(const EnhancedValidationResult& result, const std::string& context) {
        std::lock_guard<std::mutex> lock(mutex);
        stats.recordValidation(result.valid, context);
        
        for (const auto& error : result.errors) {
            stats.errorCounts[error->getCategory()]++;
        }
    }

    std::string generateStatisticsReport() const {
        std::ostringstream oss;
        oss << "Validation Engine Statistics:" << std::endl;
        oss << "  Total validations: " << stats.totalValidations << std::endl;
        oss << "  Successful validations: " << stats.successfulValidations << std::endl;
        oss << "  Failed validations: " << stats.failedValidations << std::endl;
        oss << "  Success rate: " << stats.getSuccessRate() * 100 << "%" << std::endl;
        oss << "  Recovery attempts: " << stats.recoveryAttempts << std::endl;
        oss << "  Successful recoveries: " << stats.successfulRecoveries << std::endl;
        oss << "  Average recovery time: " << stats.averageRecoveryTime << "s" << std::endl;
        oss << "  Error type distribution:" << std::endl;
        
        for (const auto& pair : stats.errorCounts) {
            oss << "    " << pair.first << ": " << pair.second << std::endl;
        }
        
        return oss.str();
    }

    void loadConfigFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                config[key] = value;
            }
        }
    }

    void saveConfigToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return;
        }
        
        for (const auto& section : config) {
            for (const auto& pair : section.second) {
                file << section.first << "::" << pair.first << " = " << pair.second << std::endl;
            }
        }
    }

    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
};

ValidationEngine::ValidationEngine() : pImpl(std::make_unique<Impl>(ValidationConfig())), batchValidationEnabled(true) {}

ValidationEngine::ValidationEngine(const ValidationConfig& config) : pImpl(std::make_unique<Impl>(config)), batchValidationEnabled(true) {}

ValidationEngine::~ValidationEngine() = default;

void ValidationEngine::addRule(const ValidationRule& rule) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rules.push_back(rule);
}

void ValidationEngine::addRule(const std::string& name, const std::string& description,
                               std::function<bool(const std::vector<std::string>&)> validator,
                               ErrorType errorType,
                               ErrorSeverity severity,
                               bool required) {
    ValidationRule rule(name, description, validator, errorType, severity, required);
    addRule(rule);
}

void ValidationEngine::removeRule(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rules.erase(std::remove_if(pImpl->rules.begin(), pImpl->rules.end(),
        [&name](const ValidationRule& rule) {
            return rule.name == name;
        }), pImpl->rules.end());
}

void ValidationEngine::clearRules() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rules.clear();
}

bool ValidationEngine::hasRule(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->rules.begin(), pImpl->rules.end(),
        [&name](const ValidationRule& rule) {
            return rule.name == name;
        });
}

ValidationRule* ValidationEngine::getRule(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find_if(pImpl->rules.begin(), pImpl->rules.end(),
        [&name](const ValidationRule& rule) {
            return rule.name == name;
        });n    return (it != pImpl->rules.end()) ? &(*it) : nullptr;
}

EnhancedValidationResult ValidationEngine::validate(const std::vector<std::string>& data, const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ValidationContext valContext(pImpl->engineConfig.defaultContext.empty() ? context : pImpl->engineConfig.defaultContext, "", data);
    return validate(valContext);
}

EnhancedValidationResult ValidationEngine::validate(const ValidationContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedValidationResult result;
    result.context = context.name;
    
    // Validate against rules
    bool rulesValid = pImpl->validateAgainstRules(context);
    if (!rulesValid) {
        result.valid = false;
        // Add errors from validation rules
        for (const auto& rule : pImpl->rules) {
            try {
                if (!rule.validator(context.data)) {
                    auto error = std::make_shared<ValidationError>(
                        "Validation rule failed: " + rule.name,
                        rule.severity,
                        context.name, 0, "Validation"
                    );
                    result.addError(error);
                }
            } catch (const std::exception& e) {
                auto error = std::make_shared<ValidationError>(
                    "Validation rule execution failed: " + rule.name + ": " + std::string(e.what()),
                    ErrorSeverity::Error,
                    context.name, 0, "Validation"
                );
                result.addError(error);
            }
        }
    }
    
    // Apply recovery if enabled
    if (pImpl->engineConfig.enableAutoRecovery && !result.valid && !result.recoveryApplied) {
        result = pImpl->applyRecovery(result);
    }
    
    // Record statistics
    pImpl->recordValidationResult(result, context.name);
    
    // Report errors if error reporter is set
    if (pImpl->errorReporter) {
        for (const auto& error : result.errors) {
            if (error->getSeverity() >= pImpl->engineConfig.minimumSeverity) {
                pImpl->errorReporter->report(error);
            }
        }
    }
    
    return result;
}

EnhancedValidationResult ValidationEngine::validateConfiguration(const std::string& key, const std::string& value) {
    EnhancedValidationResult result;
    
    // Basic configuration validation rules
    if (key == "random_seed") {
        try {
            size_t pos;
            long seed = std::stol(value, &pos);
            if (pos != value.size() || seed < 0) {
                result.valid = false;
                auto error = std::make_shared<ConfigurationError>(
                    "random_seed must be non-negative integer",
                    ErrorSeverity::Error,
                    "config", 0, "Validation"
                );
                result.addError(error);
            }
        } catch (const std::exception&) {
            result.valid = false;
            auto error = std::make_shared<ConfigurationError>(
                "random_seed must be an integer",
                ErrorSeverity::Error,
                "config", 0, "Validation"
            );
            result.addError(error);
        }
    } else if (key == "simulation_timestep") {
        try {
            size_t pos;
            double timestep = std::stod(value, &pos);
            if (pos != value.size() || timestep <= 0.0) {
                result.valid = false;
                auto error = std::make_shared<ConfigurationError>(
                    "simulation_timestep must be positive",
                    ErrorSeverity::Error,
                    "config", 0, "Validation"
                );
                result.addError(error);
            }
        } catch (const std::exception&) {
            result.valid = false;
            auto error = std::make_shared<ConfigurationError>(
                "simulation_timestep must be a number",
                ErrorSeverity::Error,
                "config", 0, "Validation"
            );
            result.addError(error);
        }
    }
    
    if (!result.valid) {
        pImpl->recordValidationResult(result, "configuration");
    }
    
    return result;
}

EnhancedValidationResult ValidationEngine::validateSystemState() {
    EnhancedValidationResult result;
    // System state validation would check memory, CPU, file handles, etc.
    return result;
}

EnhancedValidationResult ValidationEngine::validateNeuralState(const std::string& neuronId) {
    EnhancedValidationResult result;
    // Neural state validation would check neuron health, connections, etc.
    return result;
}

EnhancedValidationResult ValidationEngine::validateMemoryState() {
    EnhancedValidationResult result;
    // Memory state validation would check memory pools, allocations, etc.
    return result;
}

EnhancedValidationResult ValidationEngine::validatePlasticityState() {
    EnhancedValidationResult result;
    // Plasticity state validation would check weight updates, learning rates, etc.
    return result;
}

bool ValidationEngine::attemptRecovery(const std::shared_ptr<Error>& error) {
    return attemptRecovery(error, "Default");
}

bool ValidationEngine::attemptRecovery(const std::shared_ptr<Error>& error, const std::string& strategyName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->recoveryStrategies.find(strategyName);
    if (it == pImpl->recoveryStrategies.end()) {
        return false;
    }
    
    return it->second->canRecover(error) && it->second->recover(const_cast<std::shared_ptr<Error>&>(error));
}

bool ValidationEngine::autoRecover(const EnhancedValidationResult& result) {
    EnhancedValidationResult recoveryResult = pImpl->applyRecovery(result);
    return recoveryResult.recoveryApplied;
}

void ValidationEngine::addRecoveryStrategy(std::shared_ptr<IRecoveryStrategy> strategy) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->recoveryStrategies[strategy->getName()] = strategy;
}

void ValidationEngine::removeRecoveryStrategy(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->recoveryStrategies.erase(name);
}

void ValidationEngine::clearRecoveryStrategies() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->recoveryStrategies.clear();
}

std::shared_ptr<IRecoveryStrategy> ValidationEngine::getRecoveryStrategy(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->recoveryStrategies.find(name);
    return (it != pImpl->recoveryStrategies.end()) ? it->second : nullptr;
}

void ValidationEngine::setConfig(const ValidationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->engineConfig = config;
}

void ValidationEngine::configure(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config[key] = value;
}

std::string ValidationEngine::getConfigValue(const std::string& key, const std::string& defaultValue) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& section : pImpl->config) {
        for (const auto& pair : section.second) {
            if (pair.first == key) {
                return pair.second;
            }
        }
    }
    return defaultValue;
}

void ValidationEngine::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.totalValidations = 0;
    pImpl->stats.successfulValidations = 0;
    pImpl->stats.failedValidations = 0;
    pImpl->stats.recoveryAttempts = 0;
    pImpl->stats.successfulRecoveries = 0;
    pImpl->stats.averageRecoveryTime = 0.0;
    pImpl->stats.validationCounts.clear();
    pImpl->stats.errorCounts.clear();
}

std::string ValidationEngine::getStatisticsReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->generateStatisticsReport();
}

void ValidationEngine::setErrorReporter(std::shared_ptr<ErrorReporter> reporter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorReporter = reporter;
}

void ValidationEngine::setErrorLevel(ErrorSeverity level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->engineConfig.minimumSeverity = level;
}

std::vector<std::shared_ptr<Error>> ValidationEngine::getRecentErrors(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::shared_ptr<Error>> allErrors;
    // In a real implementation, would maintain a rolling buffer of recent errors
    return allErrors;
}

std::map<std::string, size_t> ValidationEngine::getErrorTypeDistribution() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats.errorCounts;
}

std::map<std::string, size_t> ValidationEngine::getErrorCategoryDistribution() const {
    std::map<std::string, size_t> categoryCounts;
    for (const auto& pair : pImpl->stats.errorCounts) {
        // Extract category from error type
        std::string category = pair.first;
        if (category.find("Error") != std::string::npos) {
            category = category.substr(0, category.find("Error")) + "Error";
        }
        categoryCounts[category] += pair.second;
    }
    return categoryCounts;
}

std::map<std::string, double> ValidationEngine::getErrorFrequency() const {
    std::map<std::string, double> frequencies;
    for (const auto& pair : pImpl->stats.errorCounts) {
        frequencies[pair.first] = static_cast<double>(pair.second) / pImpl->stats.totalValidations;
    }
    return frequencies;
}

void ValidationEngine::optimizeForSpeed() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->batchValidationEnabled = true;
}

void ValidationEngine::optimizeForAccuracy() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->batchValidationEnabled = false;
}

void ValidationEngine::scheduleValidation(std::function<EnhancedValidationResult()> validationFunc, double intervalSeconds) {
    // In a real implementation, would use threading and scheduling
    // For now, just call it once
    validationFunc();
}

void ValidationEngine::stopAllScheduledValidations() {
    // Stop all scheduled validations
}

bool ValidationEngine::exportRules(const std::string& filepath) const {
    pImpl->saveConfigToFile(filepath);
    return true;
}

bool ValidationEngine::importRules(const std::string& filepath) {
    pImpl->loadConfigFromFile(filepath);
    return true;
}

// ErrorQueue implementation
struct ErrorQueue::Impl {
    std::queue<std::shared_ptr<Error>> queue;
    size_t maxSize;
    std::mutex mutex;
    std::condition_variable condVar;
    bool closed;

    Impl(size_t maxSize) : maxSize(maxSize), closed(false) {}

    void push(std::shared_ptr<Error> error) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(error);
        condVar.notify_one();
    }

    bool tryPush(std::shared_ptr<Error> error) {
        std::lock_guard<std::mutex> lock(mutex);
        if (queue.size() >= maxSize) {
            return false;
        }
        queue.push(error);
        return true;
    }

    std::shared_ptr<Error> pop() {
        std::unique_lock<std::mutex> lock(mutex);
        condVar.wait(lock, [this]() { return !queue.empty() || closed; });
        if (queue.empty() && closed) {
            return nullptr;
        }
        std::shared_ptr<Error> error = queue.front();
        queue.pop();
        return error;
    }

    bool tryPop(std::shared_ptr<Error>& error) {
        std::lock_guard<std::mutex> lock(mutex);
        if (queue.empty()) {
            return false;
        }
        error = queue.front();
        queue.pop();
        return true;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        while (!queue.empty()) {
            queue.pop();
        }
    }
};

ErrorQueue::ErrorQueue(size_t maxQueueSize) : pImpl(std::make_unique<Impl>(maxQueueSize)) {}

ErrorQueue::~ErrorQueue() = default;

void ErrorQueue::push(std::shared_ptr<Error> error) {
    pImpl->push(error);
}

bool ErrorQueue::tryPush(std::shared_ptr<Error> error) {
    return pImpl->tryPush(error);
}

std::shared_ptr<Error> ErrorQueue::pop() {
    return pImpl->pop();
}

bool ErrorQueue::tryPop(std::shared_ptr<Error>& error) {
    return pImpl->tryPop(error);
}

size_t ErrorQueue::size() const {
    return pImpl->size();
}

bool ErrorQueue::empty() const {
    return pImpl->empty();
}

void ErrorQueue::clear() {
    pImpl->clear();
}

// DefaultRecoveryStrategy implementation
bool DefaultRecoveryStrategy::canRecover(const std::shared_ptr<Error>& error) const {
    return error->isRecoverable() && !error->isSystemError();
}

bool DefaultRecoveryStrategy::recover(std::shared_ptr<Error>& error) {
    // Default recovery implementation
    return false;
}

// MemoryRecoveryStrategy implementation
bool MemoryRecoveryStrategy::canRecover(const std::shared_ptr<Error>& error) const {
    return error->getType() == ErrorType::MemoryError && error->isRecoverable();
}

bool MemoryRecoveryStrategy::recover(std::shared_ptr<Error>& error) {
    // Memory recovery implementation
    return false;
}

// FileRecoveryStrategy implementation
bool FileRecoveryStrategy::canRecover(const std::shared_ptr<Error>& error) const {
    return error->getType() == ErrorType::FileError && error->isRecoverable();
}

bool FileRecoveryStrategy::recover(std::shared_ptr<Error>& error) {
    // File recovery implementation
    return false;
}

// ConfigurationRecoveryStrategy implementation
bool ConfigurationRecoveryStrategy::canRecover(const std::shared_ptr<Error>& error) const {
    return error->getType() == ErrorType::ConfigurationError && error->isRecoverable();
}

bool ConfigurationRecoveryStrategy::recover(std::shared_ptr<Error>& error) {
    // Configuration recovery implementation
    return false;
}

// ValidationUtils implementation
bool ValidationUtils::isNumber(const std::string& str, double* value) {
    try {
        size_t pos;
        double result = std::stod(str, &pos);
        if (value) *value = result;
        return pos == str.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool ValidationUtils::isInteger(const std::string& str, long* value) {
    try {
        size_t pos;
        long result = std::stol(str, &pos);
        if (value) *value = result;
        return pos == str.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool ValidationUtils::isBoolean(const std::string& str, bool* value) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "false") {
        if (value) *value = (lower == "true");
        return true;
    }
    return false;
}

bool ValidationUtils::isValidName(const std::string& name) {
    return !name.empty() && std::all_of(name.begin(), name.end(),
        [](char c) { return std::isalnum(c) || c == '_'; });
}

bool ValidationUtils::isValidPath(const std::string& path, bool writeAccess) {
    // Path validation implementation
    return true;
}

bool ValidationUtils::isInRange(double value, double min, double max, bool inclusive) {
    if (inclusive) {
        return value >= min && value <= max;
    } else {
        return value > min && value < max;
    }
}

bool ValidationUtils::isPositive(double value) {
    return value > 0.0;
}

bool ValidationUtils::isNonNegative(double value) {
    return value >= 0.0;
}

bool ValidationUtils::isLessThan(double value, double max) {
    return value < max;
}

bool ValidationUtils::isGreaterThan(double value, double min) {
    return value > min;
}

bool ValidationUtils::hasMinimumLength(const std::string& str, size_t minLength) {
    return str.size() >= minLength;
}

bool ValidationUtils::hasMaximumLength(const std::string& str, size_t maxLength) {
    return str.size() <= maxLength;
}

bool ValidationUtils::containsOnly(const std::string& str, const std::string& allowedChars) {
    return std::all_of(str.begin(), str.end(),
        [&allowedChars](char c) {
            return allowedChars.find(c) != std::string::npos;
        });
}

bool ValidationUtils::isValidIdentifier(const std::string& identifier) {
    return !identifier.empty() && std::isalpha(identifier[0]);
}

bool ValidationUtils::isValidConfigKey(const std::string& key) {
    return !key.empty() && !containsOnly(key, " .:-_");
}

bool ValidationUtils::isValidConfigValue(const std::string& key, const std::string& value) {
    // Configuration value validation
    return !value.empty();
}

bool ValidationUtils::checkMemoryAvailability(size_t minBytes) {
    // Memory availability check
    return true;
}

bool ValidationUtils::checkFileDescriptorAvailability(size_t minDescriptors) {
    // File descriptor availability check
    return true;
}

bool ValidationUtils::checkNetworkAvailability() {
    // Network availability check
    return true;
}

bool ValidationUtils::isValidNeuronId(const std::string& id) {
    return !id.empty() && std::isdigit(id[0]);
}

bool ValidationUtils::isValidSynapseId(const std::string& id) {
    return !id.empty() && std::isdigit(id[0]);
}

bool ValidationUtils::isValidRegionId(const std::string& id) {
    return !id.empty() && std::isdigit(id[0]);
}

bool ValidationUtils::checkPerformanceConstraints(const std::string& operation, double timeLimit) {
    // Performance constraint check
    return true;
}

bool ValidationUtils::checkMemoryConstraints(size_t memoryLimit) {
    // Memory constraint check
    return true;
}

std::vector<std::string> ValidationUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream tokenStream(str);
    std::string token;
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

std::string ValidationUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ValidationUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool ValidationUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string ValidationUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string ValidationUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Global validation engine instance
namespace {
    ValidationEngine* globalValidationEngine = nullptr;
    std::mutex globalValidationEngineMutex;
}

ValidationEngine& getGlobalValidationEngine() {
    std::lock_guard<std::mutex> lock(globalValidationEngineMutex);
    if (!globalValidationEngine) {
        globalValidationEngine = new ValidationEngine();
    }
    return *globalValidationEngine;
}

void setGlobalValidationEngine(ValidationEngine& engine) {
    std::lock_guard<std::mutex> lock(globalValidationEngineMutex);
    globalValidationEngine = &engine;
}

} // namespace nlm
