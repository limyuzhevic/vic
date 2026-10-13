#include "ExperimentBase.hpp"
#include "Experiment.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace nlm {

struct ExperimentBase::Impl {
    std::string name;
    uint64_t seed;
    std::unordered_map<std::string, std::string> parameters;
    std::unordered_map<std::string, double> results;
    std::unordered_map<std::string, double> performanceMetrics;
    std::unordered_map<std::string, std::string> config;
    
    // Lifecycle state
    bool initialized;
    bool running;
    bool finalized;
    bool setupCompleted;
    bool teardownCompleted;
    
    // Timing and progress
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double elapsedTime;
    double progress;
    
    // Error handling
    std::vector<std::string> errorMessages;
    bool threadSafe;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // For serialization
    time_t timestamp;
    
    Impl() : 
        name("Experiment"),
        seed(0),
        initialized(false),
        running(false),
        finalized(false),
        setupCompleted(false),
        teardownCompleted(false),
        elapsedTime(0.0),
        progress(0.0),
        threadSafe(true),
        timestamp(0)
    {}
};

ExperimentBase::ExperimentBase(const std::string& name) : pImpl(std::make_unique<Impl>()) {
    pImpl->name = name;
    pImpl->initialized = false;
    pImpl->running = false;
    pImpl->finalized = false;
}

ExperimentBase::~ExperimentBase() = default;

bool ExperimentBase::executeLifecycle() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        // Validate experiment first
        if (!validate()) {
            return false;
        }
        
        // Initialize
        pImpl->initialized = initialize();
        if (!pImpl->initialized) {
            return false;
        }
        onInitialize();
        
        // Setup
        pImpl->setupCompleted = setup();
        if (!pImpl->setupCompleted) {
            return false;
        }
        onSetup();
        
        // Run experiment
        pImpl->running = true;
        bool runSuccess = run();
        pImpl->running = false;
        
        if (!runSuccess) {
            return false;
        }
        onRun();
        
        // Teardown
        pImpl->teardownCompleted = teardown();
        if (!pImpl->teardownCompleted) {
            return false;
        }
        onTeardown();
        
        // Finalization
        pImpl->finalized = finalize();
        if (!pImpl->finalized) {
            return false;
        }
        onFinalize();
        
        // Generate report
        report();
        
        return true;
    } catch (const NLMException& e) {
        handleError(e);
        return false;
    } catch (const std::exception& e) {
        handleError(NLMException(NLMException::ErrorCode::Unknown, e.what()));
        return false;
    }
}

bool ExperimentBase::validate() {
    validateExperiment();
    return true;
}

bool ExperimentBase::setup() {
    return true;
}

bool ExperimentBase::teardown() {
    return true;
}

bool ExperimentBase::report() {
    return true;
}

const std::string& ExperimentBase::getName() const {
    return pImpl->name;
}

void ExperimentBase::setName(const std::string& name) {
    pImpl->name = name;
}

uint64_t ExperimentBase::getSeed() const {
    return pImpl->seed;
}

void ExperimentBase::setSeed(uint64_t seed) {
    pImpl->seed = seed;
}

void ExperimentBase::addParameter(const std::string& key, const std::string& value) {
    pImpl->parameters[key] = value;
}

std::string ExperimentBase::getParameter(const std::string& key) const {
    auto it = pImpl->parameters.find(key);
    return it != pImpl->parameters.end() ? it->second : "";
}

bool ExperimentBase::hasParameter(const std::string& key) const {
    return pImpl->parameters.find(key) != pImpl->parameters.end();
}

void ExperimentBase::clearParameters() {
    pImpl->parameters.clear();
}

void ExperimentBase::addResult(const std::string& key, double value) {
    pImpl->results[key] = value;
}

double ExperimentBase::getResult(const std::string& key) const {
    auto it = pImpl->results.find(key);
    return it != pImpl->results.end() ? it->second : 0.0;
}

bool ExperimentBase::hasResult(const std::string& key) const {
    return pImpl->results.find(key) != pImpl->results.end();
}

const std::unordered_map<std::string, double>& ExperimentBase::getResults() const {
    return pImpl->results;
}

std::string ExperimentBase::generateReport() const {
    std::ostringstream oss;
    oss << "Experiment Report: " << pImpl->name << "\n";
    oss << "=======================================\n";
    oss << "Seed: " << pImpl->seed << "\n";
    oss << "Initialized: " << (pImpl->initialized ? "Yes" : "No") << "\n";
    oss << "Running: " << (pImpl->running ? "Yes" : "No") << "\n";
    oss << "Finalized: " << (pImpl->finalized ? "Yes" : "No") << "\n";
    oss << "Setup Completed: " << (pImpl->setupCompleted ? "Yes" : "No") << "\n";
    oss << "Teardown Completed: " << (pImpl->teardownCompleted ? "Yes" : "No") << "\n";
    oss << "Elapsed Time: " << std::fixed << std::setprecision(3) << pImpl->elapsedTime << "s\n";
    oss << "Progress: " << std::fixed << std::setprecision(2) << pImpl->progress * 100 << "%\n\n";
    
    oss << "Parameters:\n";
    for (const auto& pair : pImpl->parameters) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    oss << "\nResults:\n";
    for (const auto& pair : pImpl->results) {
        oss << "  " << pair.first << ": " << std::fixed << std::setprecision(6) << pair.second << "\n";
    }
    
    oss << "\nPerformance Metrics:\n";
    for (const auto& pair : pImpl->performanceMetrics) {
        oss << "  " << pair.first << ": " << std::fixed << std::setprecision(6) << pair.second << "\n";
    }
    
    oss << "\nErrors:\n";
    for (const auto& error : pImpl->errorMessages) {
        oss << "  " << error << "\n";
    }
    
    return oss.str();
}

void ExperimentBase::onInitialize() {}

void ExperimentBase::onSetup() {}

void ExperimentBase::onRun() {}

void ExperimentBase::onTeardown() {}

void ExperimentBase::onFinalize() {}

void ExperimentBase::startTiming() {
    pImpl->startTime = std::chrono::system_clock::now();
}

void ExperimentBase::stopTiming() {
    auto end = std::chrono::system_clock::now();
    pImpl->elapsedTime = std::chrono::duration<double>(end - pImpl->startTime).count();
}

double ExperimentBase::getElapsedTime() const {
    return pImpl->elapsedTime;
}

void ExperimentBase::recordPerformanceMetric(const std::string& name, double value) {
    pImpl->performanceMetrics[name] = value;
}

const std::unordered_map<std::string, double>& ExperimentBase::getPerformanceMetrics() const {
    return pImpl->performanceMetrics;
}

std::string ExperimentBase::serialize() const {
    std::ostringstream oss;
    oss << "ExperimentBase\n";
    oss << "Name:" << pImpl->name << "\n";
    oss << "Seed:" << pImpl->seed << "\n";
    oss << "Initialized:" << (pImpl->initialized ? "true" : "false") << "\n";
    oss << "Running:" << (pImpl->running ? "true" : "false") << "\n";
    oss << "Finalized:" << (pImpl->finalized ? "true" : "false") << "\n";
    oss << "ElapsedTime:" << pImpl->elapsedTime << "\n";
    oss << "Progress:" << pImpl->progress << "\n";
    oss << "ThreadSafe:" << (pImpl->threadSafe ? "true" : "false") << "\n";
    
    oss << "Parameters:";
    for (const auto& pair : pImpl->parameters) {
        oss << "[" << pair.first << "," << pair.second << "]";
    }
    oss << "\n";
    
    oss << "Results:";
    for (const auto& pair : pImpl->results) {
        oss << "[" << pair.first << "," << pair.second << "]";
    }
    oss << "\n";
    
    oss << "Config:";
    for (const auto& pair : pImpl->config) {
        oss << "[" << pair.first << "," << pair.second << "]";
    }
    oss << "\n";
    
    oss << "PerformanceMetrics:";
    for (const auto& pair : pImpl->performanceMetrics) {
        oss << "[" << pair.first << "," << pair.second << "]";
    }
    oss << "\n";
    
    oss << "Errors:";
    for (const auto& error : pImpl->errorMessages) {
        // Escape special characters in error message
        std::string escaped = error;
        size_t pos = 0;
        while ((pos = escaped.find(']', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "]]");
            pos += 2;
        }
        oss << "[" << escaped << "]";
    }
    oss << "\n";
    
    return oss.str();
}

bool ExperimentBase::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string line;
    
    try {
        while (std::getline(iss, line)) {
            if (line.empty()) continue;
            
            if (line.find("Name:") == 0) {
                pImpl->name = line.substr(5);
            } else if (line.find("Seed:") == 0) {
                pImpl->seed = std::stoull(line.substr(5));
            } else if (line.find("Initialized:") == 0) {
                pImpl->initialized = (line.substr(11) == "true");
            } else if (line.find("Running:") == 0) {
                pImpl->running = (line.substr(7) == "true");
            } else if (line.find("Finalized:") == 0) {
                pImpl->finalized = (line.substr(9) == "true");
            } else if (line.find("ElapsedTime:") == 0) {
                pImpl->elapsedTime = std::stod(line.substr(11));
            } else if (line.find("Progress:") == 0) {
                pImpl->progress = std::stod(line.substr(8));
            } else if (line.find("ThreadSafe:") == 0) {
                pImpl->threadSafe = (line.substr(10) == "true");
            } else if (line.find("Parameters:") == 0) {
                pImpl->parameters.clear();
                // Parse parameters
                size_t pos = 11; // Skip "Parameters:"
                while (pos < line.length()) {
                    size_t bracketStart = line.find('[', pos);
                    if (bracketStart == std::string::npos) break;
                    size_t bracketEnd = line.find(']', bracketStart);
                    if (bracketEnd == std::string::npos) break;
                    
                    std::string param = line.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
                    size_t commaPos = param.find(',');
                    if (commaPos != std::string::npos) {
                        std::string key = param.substr(0, commaPos);
                        std::string value = param.substr(commaPos + 1);
                        pImpl->parameters[key] = value;
                    }
                    
                    pos = bracketEnd + 1;
                }
            } else if (line.find("Results:") == 0) {
                pImpl->results.clear();
                size_t pos = 8; // Skip "Results:"
                while (pos < line.length()) {
                    size_t bracketStart = line.find('[', pos);
                    if (bracketStart == std::string::npos) break;
                    size_t bracketEnd = line.find(']', bracketStart);
                    if (bracketEnd == std::string::npos) break;
                    
                    std::string result = line.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
                    size_t commaPos = result.find(',');
                    if (commaPos != std::string::npos) {
                        std::string key = result.substr(0, commaPos);
                        double value = std::stod(result.substr(commaPos + 1));
                        pImpl->results[key] = value;
                    }
                    
                    pos = bracketEnd + 1;
                }
            } else if (line.find("Config:") == 0) {
                pImpl->config.clear();
                size_t pos = 7; // Skip "Config:"
                while (pos < line.length()) {
                    size_t bracketStart = line.find('[', pos);
                    if (bracketStart == std::string::npos) break;
                    size_t bracketEnd = line.find(']', bracketStart);
                    if (bracketEnd == std::string::npos) break;
                    
                    std::string configItem = line.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
                    size_t commaPos = configItem.find(',');
                    if (commaPos != std::string::npos) {
                        std::string key = configItem.substr(0, commaPos);
                        std::string value = configItem.substr(commaPos + 1);
                        pImpl->config[key] = value;
                    }
                    
                    pos = bracketEnd + 1;
                }
            } else if (line.find("PerformanceMetrics:") == 0) {
                pImpl->performanceMetrics.clear();
                size_t pos = 18; // Skip "PerformanceMetrics:"
                while (pos < line.length()) {
                    size_t bracketStart = line.find('[', pos);
                    if (bracketStart == std::string::npos) break;
                    size_t bracketEnd = line.find(']', bracketStart);
                    if (bracketEnd == std::string::npos) break;
                    
                    std::string metric = line.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
                    size_t commaPos = metric.find(',');
                    if (commaPos != std::string::npos) {
                        std::string key = metric.substr(0, commaPos);
                        double value = std::stod(metric.substr(commaPos + 1));
                        pImpl->performanceMetrics[key] = value;
                    }
                    
                    pos = bracketEnd + 1;
                }
            } else if (line.find("Errors:") == 0) {
                pImpl->errorMessages.clear();
                size_t pos = 7; // Skip "Errors:"
                while (pos < line.length()) {
                    size_t bracketStart = line.find('[', pos);
                    if (bracketStart == std::string::npos) break;
                    size_t bracketEnd = line.find(']', bracketStart);
                    if (bracketEnd == std::string::npos) break;
                    
                    std::string error = line.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
                    // Unescape special characters
                    size_t posEsc = 0;
                    while ((posEsc = error.find("]]", posEsc)) != std::string::npos) {
                        error.replace(posEsc, 2, "]");
                        posEsc++;
                    }
                    
                    pImpl->errorMessages.push_back(error);
                    pos = bracketEnd + 1;
                }
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

void ExperimentBase::validateExperiment() const {
    // Default validation - can be overridden by derived classes
    if (pImpl->name.empty()) {
        throw NLMException(NLMException::ErrorCode::InvalidArgument, "Experiment name cannot be empty");
    }
    
    if (pImpl->seed == 0) {
        // Not necessarily an error, but could be logged
    }
}

void ExperimentBase::handleError(const NLMException& e) {
    pImpl->errorMessages.push_back("Error (Code " + std::to_string(static_cast<int>(e.getErrorCode())) + "): " + e.getMessage());
}

bool ExperimentBase::isThreadSafe() const {
    return pImpl->threadSafe;
}

void ExperimentBase::setConfig(const std::unordered_map<std::string, std::string>& config) {
    pImpl->config = config;
}

const std::unordered_map<std::string, std::string>& ExperimentBase::getConfig() const {
    return pImpl->config;
}

void ExperimentBase::clearConfig() {
    pImpl->config.clear();
}

bool ExperimentBase::isInitialized() const {
    return pImpl->initialized;
}

bool ExperimentBase::isRunning() const {
    return pImpl->running;
}

double ExperimentBase::getProgress() const {
    return pImpl->progress;
}

void ExperimentBase::addMetric(const std::string& name, double value) {
    // Create a temporary Experiment instance for backward compatibility
    Experiment tempExp("Temp");
    tempExp.recordMetric(name, value);
    
    // This would normally be integrated with the existing Experiment class
    // For now, we'll store it in results as well
    addResult(name, value);
}

double ExperimentBase::getMetric(const std::string& name) const {
    // This would normally retrieve from the existing Experiment class
    // For now, we'll use the result system
    return getResult(name);
}

void ExperimentBase::addStepData(SimulationStep step, size_t neuronCount, size_t synapseCount,
                                size_t firingCount, float avgFiringRate) {
    // Create a temporary Experiment instance for backward compatibility
    Experiment tempExp("Temp");
    tempExp.recordStep(step, neuronCount, synapseCount, firingCount, avgFiringRate);
    
    // The step data is stored in the Experiment class for backward compatibility
    // This method maintains the interface while storing data in the new result system
    addResult("step_" + std::to_string(step), static_cast<double>(neuronCount));
}

} // namespace nlm
