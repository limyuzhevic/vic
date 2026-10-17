#include "SystemInitializer.hpp"
#include "LoggerHelper.hpp"
#include <algorithm>
#include <fstream>

namespace nlm {

// SystemInitializer implementation

struct SystemInitializer::Impl {
    std::map<std::string, std::pair<SystemInitFunc, Phase>> systemInitializers;
    InitStepCallback initCallback;
    bool initialized;
    std::vector<std::string> executionLog;
    
    struct SystemInitInfo {
        std::string name;
        SystemInitFunc initFunc;
        Phase phase;
        bool completed;
        std::string errorMessage;
    };
    
    std::vector<SystemInitInfo> systemInitInfos;
};

SystemInitializer::SystemInitializer() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
}

SystemInitializer::~SystemInitializer() = default;

void SystemInitializer::addSystemInitializer(const std::string& name, 
                                            SystemInitFunc initFunc,
                                            Phase phase) {
    pImpl->systemInitializers[name] = {initFunc, phase};
    pImpl->systemInitInfos.push_back({
        name, initFunc, phase, false, ""
    });
}

void SystemInitializer::setInitCallback(InitStepCallback callback) {
    pImpl->initCallback = callback;
}

bool SystemInitializer::initialize(std::shared_ptr<Config> config) {
    if (pImpl->initialized) {
        LoggerHelper::warning("SystemInitializer already initialized");
        return true;
    }
    
    // Sort initializers by phase
    std::vector<SystemInitInfo> sortedInfos = pImpl->systemInitInfos;
    std::sort(sortedInfos.begin(), sortedInfos.end(),
        [](const SystemInitInfo& a, const SystemInitInfo& b) {
            return static_cast<int>(a.phase) < static_cast<int>(b.phase);
        });
    
    bool allSuccessful = true;
    
    for (const auto& info : sortedInfos) {
        std::string stepName = "Initializing " + info.name;
        
        if (pImpl->initCallback) {
            pImpl->initCallback(stepName, info.phase);
        }
        
        LoggerHelper::info("Starting: " + stepName);
        
        bool success = info.initFunc(config);
        
        if (success) {
            pImpl->executionLog.push_back("✓ " + stepName + " - SUCCESS");
            info.completed = true;
            LoggerHelper::info("✓ " + stepName + " - SUCCESS");
        } else {
            allSuccessful = false;
            pImpl->executionLog.push_back("✗ " + stepName + " - FAILED");
            info.completed = false;
            info.errorMessage = "System initialization failed";
            LoggerHelper::error("✗ " + stepName + " - FAILED: " + info.errorMessage);
        }
    }
    
    pImpl->initialized = allSuccessful;
    
    if (allSuccessful) {
        LoggerHelper::info("SystemInitializer - All systems initialized successfully");
    } else {
        LoggerHelper::error("SystemInitializer - Some systems failed to initialize");
    }
    
    return allSuccessful;
}

bool SystemInitializer::isInitialized() const {
    return pImpl->initialized;
}

void SystemInitializer::reset() {
    pImpl->initialized = false;
    pImpl->executionLog.clear();
    for (auto& info : pImpl->systemInitInfos) {
        info.completed = false;
        info.errorMessage.clear();
    }
}

std::string SystemInitializer::getStatusString() const {
    if (pImpl->initialized) {
        return "INITIALIZED";
    }
    
    size_t completedCount = 0;
    for (const auto& info : pImpl->systemInitInfos) {
        if (info.completed) {
            completedCount++;
        }
    }
    
    return "PARTIALLY_INITIALIZED (" + std::to_string(completedCount) +
           "/" + std::to_string(pImpl->systemInitInfos.size()) + ")";
}

// ConfigLoader implementation

struct ConfigLoader::Impl {
    LoadingStrategy strategy;
    ValidationCallback validationCallback;
    std::vector<ValidationError> validationErrors;
    std::map<std::string, std::string> defaultValues;
};

ConfigLoader::ConfigLoader() : pImpl(std::make_unique<Impl>()) {
    pImpl->strategy = LoadingStrategy::MERGED;
}

ConfigLoader::~ConfigLoader() = default;

void ConfigLoader::setStrategy(LoadingStrategy strategy) {
    pImpl->strategy = strategy;
}

void ConfigLoader::setValidationCallback(ValidationCallback callback) {
    pImpl->validationCallback = callback;
}

bool ConfigLoader::loadConfig(std::shared_ptr<Config> config, const std::string& filepath,
                             int argc, char** argv) {
    clearValidationErrors();
    
    bool success = true;
    
    switch (pImpl->strategy) {
        case LoadingStrategy::DEFAULT_ONLY:
            // Config comes from defaults only
            break;
        case LoadingStrategy::FILE_ONLY:
            success = loadFromFile(config, filepath);
            break;
        case LoadingStrategy::COMMAND_LINE_ONLY:
            success = loadFromArgs(config, argc, argv);
            break;
        case LoadingStrategy::MERGED:
            success = loadFromFile(config, filepath);
            if (success) {
                success = loadFromArgs(config, argc, argv);
            }
            break;
        case LoadingStrategy::OVERRIDE:
            success = loadFromArgs(config, argc, argv);
            if (success) {
                success = loadFromFile(config, filepath);
            }
            break;
        case LoadingStrategy::DEEP_MERGE:
            // Merge configs (command line overrides file)
            success = loadFromFile(config, filepath);
            if (success) {
                std::shared_ptr<Config> tempConfig = std::make_shared<Config>();
                success = loadFromArgs(tempConfig, argc, argv);
                if (success) {
                    success = mergeConfig(config, tempConfig, true);
                }
            }
            break;
    }
    
    if (success) {
        // Validate the loaded configuration
        success = validateConfig(config);
    }
    
    return success;
}

bool ConfigLoader::loadFromFile(std::shared_ptr<Config> config, const std::string& filepath) {
    if (filepath.empty()) {
        LoggerHelper::warning("No filepath provided for file loading");
        return false;
    }
    
    LoggerHelper::info("Loading configuration from file: " + filepath);
    
    // In a real implementation, this would load from JSON/YAML file
    // For now, just log that we would load from file
    return true;
}

bool ConfigLoader::loadFromArgs(std::shared_ptr<Config> config, int argc, char** argv) {
    LoggerHelper::info("Loading configuration from command line arguments");
    
    // In a real implementation, this would parse command line arguments
    // For now, just log that we would load from args
    return true;
}

bool ConfigLoader::mergeConfig(std::shared_ptr<Config> target,
                              const std::shared_ptr<Config>& source,
                              bool override) {
    // Merge source config into target config
    // In a real implementation, this would iterate through all keys in source
    // and add them to target (or override if specified)
    
    LoggerHelper::info("Merging configuration");
    return true;
}

bool ConfigLoader::validateConfig(const std::shared_ptr<Config>& config) const {
    // Validate configuration
    // In a real implementation, this would validate based on schema/rules
    
    LoggerHelper::info("Validating configuration");
    return true;
}

const std::vector<ConfigLoader::ValidationError>& ConfigLoader::getValidationErrors() const {
    return pImpl->validationErrors;
}

void ConfigLoader::clearValidationErrors() {
    pImpl->validationErrors.clear();
}

// Template method implementations

template<typename T>
bool InitHelper::initializeSystem(std::shared_ptr<Config> config, 
                                  std::shared_ptr<T>& system,
                                  const std::string& systemName) {
    LoggerHelper::info("Initializing system: " + systemName);
    
    try {
        system = std::make_shared<T>();
        
        if (!system->initialize(config)) {
            LoggerHelper::error("Failed to initialize system: " + systemName);
            return false;
        }
        
        LoggerHelper::info("Successfully initialized system: " + systemName);
        return true;
    } catch (const std::exception& e) {
        LoggerHelper::error("Exception initializing system " + systemName + ": " + std::string(e.what()));
        return false;
    }
}

template<typename T>
void InitHelper::configureFromConfig(T* system, std::shared_ptr<Config> config,
                                    const std::string& prefix) {
    if (!system) {
        LoggerHelper::warning("Cannot configure null system");
        return;
    }
    
    LoggerHelper::info("Configuring system from config: " + prefix);
    
    // In a real implementation, this would configure the system based on config values
}

template<typename T>
std::shared_ptr<T> InitHelper::createAndInitialize(std::shared_ptr<Config> config,
                                                 const std::string& systemName) {
    std::shared_ptr<T> system = std::make_shared<T>();
    
    if (!initializeSystem(config, system, systemName)) {
        LoggerHelper::error("Failed to create and initialize system: " + systemName);
        return nullptr;
    }
    
    return system;
}

template<typename T>
T InitHelper::getConfigValue(std::shared_ptr<Config> config, 
                           const std::string& key,
                           const T& defaultValue,
                           bool logWarning) {
    if (config->has(key)) {
        return config->get<T>(key).value_or(defaultValue);
    } else {
        if (logWarning) {
            LoggerHelper::warning("Using default value for config key: " + key);
        }
        return defaultValue;
    }
}

// Factory implementation

std::map<std::string, SystemFactory::SystemFactoryFunc> SystemFactory::factoryRegistry_;

std::shared_ptr<INeuralSystem> SystemFactory::createSystem(const std::string& type,
                                                          std::shared_ptr<Config> config) {
    auto it = factoryRegistry_.find(type);
    if (it != factoryRegistry_.end()) {
        return it->second(config);
    }
    
    LoggerHelper::error("Unknown system type: " + type);
    return nullptr;
}

template<typename T>
void SystemFactory::registerSystemFactory(const std::string& type) {
    factoryRegistry_[type] = [](std::shared_ptr<Config> config) {
        return std::make_shared<T>(config);
    };
}

std::vector<std::string> SystemFactory::getRegisteredSystemTypes() {
    std::vector<std::string> types;
    types.reserve(factoryRegistry_.size());
    
    for (const auto& pair : factoryRegistry_) {
        types.push_back(pair.first);
    }
    
    return types;
}

// Other classes implementation

InitializationTemplate::InitializationTemplate() : pImpl(std::make_unique<Impl>()) {}
InitializationTemplate::~InitializationTemplate() = default;

void InitializationTemplate::addStep(const InitializationStep& step) {
    pImpl->steps.push_back(step);
}

bool InitializationTemplate::executeSequence() {
    // Sort steps by priority
    std::vector<InitializationStep> sortedSteps = pImpl->steps;
    std::sort(sortedSteps.begin(), sortedSteps.end(),
        [](const InitializationStep& a, const InitializationStep& b) {
            return a.priority > b.priority; // Higher priority first
        });
    
    bool allSuccessful = true;
    
    for (const auto& step : sortedSteps) {
        LoggerHelper::info("Executing step: " + step.name);
        
        bool stepSuccess = step.execute();
        if (!stepSuccess) {
            LoggerHelper::error("Step failed: " + step.name);
            allSuccessful = false;
        }
        
        if (step.validate && !step.validate()) {
            LoggerHelper::error("Step validation failed: " + step.name);
            allSuccessful = false;
        }
        
        pImpl->executionLog.push_back(
            step.name + " - " + (stepSuccess ? "SUCCESS" : "FAILED")
        );
    }
    
    return allSuccessful;
}

const std::vector<std::string>& InitializationTemplate::getExecutionLog() const {
    return pImpl->executionLog;
}

// CodeOrganizer implementation

CodeOrganizer::CodeOrganizer() : pImpl(std::make_unique<Impl>()) {}
CodeOrganizer::~CodeOrganizer() = default;

std::vector<CodeOrganizer::RefactoringOpportunity> CodeOrganizer::analyzeCode(const std::string& directory) {
    // In a real implementation, this would parse C++ code and identify patterns
    // For now, return empty vector
    return std::vector<RefactoringOpportunity>();
}

std::string CodeOrganizer::generateReport(const std::vector<RefactoringOpportunity>& opportunities) {
    // Generate report from refactoring opportunities
    // In a real implementation, this would generate a detailed report
    
    std::ostringstream oss;
    oss << "Code Analysis Report\n";
    oss << "===================\n\n";
    oss << "Total opportunities found: " << opportunities.size() << "\n\n";
    
    for (size_t i = 0; i < opportunities.size(); ++i) {
        oss << "Opportunity " << (i + 1) << ":\n";
        oss << "  Description: " << opportunities[i].description << "\n";
        oss << "  Location: " << opportunities[i].location << "\n";
        oss << "  Suggested Fix: " << opportunities[i].suggestedFix << "\n";
        oss << "  Estimated Impact: " << (opportunities[i].estimatedImpact * 100) << "%\n";
        oss << "  High Priority: " << (opportunities[i].isHighPriority ? "Yes" : "No") << "\n\n";
    }
    
    return oss.str();
}

bool CodeOrganizer::applyRefactoring(const RefactoringOpportunity& opportunity) {
    // Apply the suggested refactoring
    // In a real implementation, this would modify the code
    
    LoggerHelper::info("Applying refactoring: " + opportunity.description);
    return true;
}

bool CodeOrganizer::isAvailable() {
    return true;
}

// ArchitectureDoc implementation

ArchitectureDoc::ArchitectureDoc() : pImpl(std::make_unique<Impl>()) {}
ArchitectureDoc::~ArchitectureDoc() = default;

void ArchitectureDoc::addComponent(const Component& component) {
    pImpl->components.push_back(component);
}

void ArchitectureDoc::addRelationship(const Relationship& relationship) {
    pImpl->relationships.push_back(relationship);
}

std::string ArchitectureDoc::generateDocumentation() const {
    // Generate architecture documentation
    // In a real implementation, this would generate a formatted document
    
    std::ostringstream oss;
    oss << "NLM Architecture Documentation\n";
    oss << "==============================\n\n";
    
    oss << "Components:\n";
    for (const auto& component : pImpl->components) {
        oss << "  - " << component.name << " (" << component.type << ")\n";
        oss << "    Purpose: " << component.purpose << "\n";
        if (!component.dependencies.empty()) {
            oss << "    Dependencies: ";
            for (size_t i = 0; i < component.dependencies.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << component.dependencies[i];
            }
            oss << "\n";
        }
    }
    
    oss << "\nRelationships:\n";
    for (const auto& relationship : pImpl->relationships) {
        oss << "  - " << relationship.source << " " << relationship.type << " " << relationship.target << "\n";
        oss << "    " << relationship.description << "\n";
    }
    
    return oss.str();
}

bool ArchitectureDoc::save(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        LoggerHelper::error("Failed to open file for writing: " + filepath);
        return false;
    }
    
    file << generateDocumentation();
    file.close();
    
    LoggerHelper::info("Architecture documentation saved to: " + filepath);
    return true;
}

bool ArchitectureDoc::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        LoggerHelper::error("Failed to open file for reading: " + filepath);
        return false;
    }
    
    // In a real implementation, this would parse the file
    // For now, just read and log
    std::string line;
    while (std::getline(file, line)) {
        // Parse and build architecture
    }
    
    file.close();
    
    LoggerHelper::info("Architecture documentation loaded from: " + filepath);
    return true;
}

bool ArchitectureDoc::isAvailable(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

} // namespace nlm
