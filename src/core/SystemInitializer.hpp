#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <type_traits>
#include "LoggerHelper.hpp"

namespace nlm {

class Config;

// SystemInitializer - Common initialization patterns
class SystemInitializer {
public:
    // Initialization phase enumeration
    enum class Phase {
        PRE_INIT,
        CONFIG_LOAD,
        SYSTEM_SETUP,
        NEURAL_SETUP,
        PLANNING_SETUP,
        NEUROMODULATION_SETUP,
        MEMORY_SETUP,
        ENHANCED_MEMORY_SETUP,
        CONNECTIVITY_SETUP,
        POST_INIT,
        COMPLETE
    };

    // Initialization step callback
    using InitStepCallback = std::function<bool(const std::string& stepName, Phase phase)>;

    // System initialization function
    using SystemInitFunc = std::function<bool(std::shared_ptr<Config> config)>;

    SystemInitializer();
    ~SystemInitializer();
    
    // Add initialization step
    void addSystemInitializer(const std::string& name, 
                             SystemInitFunc initFunc,
                             Phase phase = Phase::SYSTEM_SETUP);
    
    // Set initialization callback
    void setInitCallback(InitStepCallback callback);
    
    // Run initialization sequence
    bool initialize(std::shared_ptr<Config> config);
    
    // Check if initialization completed
    bool isInitialized() const;
    
    // Reset initialization state
    void reset();
    
    // Get initialization status
    std::string getStatusString() const;
    
    // Disable copying
    SystemInitializer(const SystemInitializer&) = delete;
    SystemInitializer& operator=(const SystemInitializer&) = delete;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ConfigLoader - Common configuration loading patterns
class ConfigLoader {
public:
    // Configuration loading strategy
    enum class LoadingStrategy {
        DEFAULT_ONLY,
        FILE_ONLY,
        COMMAND_LINE_ONLY,
        MERGED,  // File + Command Line
        OVERRIDE, // Command Line overrides file
        DEEP_MERGE
    };

    // Configuration validation error
    struct ValidationError {
        std::string key;
        std::string message;
        bool isFatal;
        
        ValidationError(const std::string& k, const std::string& msg, bool fatal = true)
            : key(k), message(msg), isFatal(fatal) {}
    };

    // Validation callback
    using ValidationCallback = std::function<bool(const ValidationError& error)>;

    ConfigLoader();
    ~ConfigLoader();
    
    // Set loading strategy
    void setStrategy(LoadingStrategy strategy);
    
    // Set validation callback
    void setValidationCallback(ValidationCallback callback);
    
    // Load configuration with strategy
    bool loadConfig(std::shared_ptr<Config> config, const std::string& filepath = "",
                   int argc = 0, char** argv = nullptr);
    
    // Load from file
    bool loadFromFile(std::shared_ptr<Config> config, const std::string& filepath);
    
    // Load from command line
    bool loadFromArgs(std::shared_ptr<Config> config, int argc, char** argv);
    
    // Merge configurations
    static bool mergeConfig(std::shared_ptr<Config> target,
                           const std::shared_ptr<Config>& source,
                           bool override = false);
    
    // Validate configuration
    bool validateConfig(const std::shared_ptr<Config>& config) const;
    
    // Get validation errors
    const std::vector<ValidationError>& getValidationErrors() const;
    
    // Clear validation errors
    void clearValidationErrors();
    
    // Disable copying
    ConfigLoader(const ConfigLoader&) = delete;
    ConfigLoader& operator=(const ConfigLoader&) = delete;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Helper functions for initialization and configuration
namespace InitHelper {
    // Template function to initialize systems with error handling
    template<typename T>
    static bool initializeSystem(std::shared_ptr<Config> config, 
                                std::shared_ptr<T>& system,
                                const std::string& systemName);
    
    // Template function to configure systems from config
    template<typename T>
    static void configureFromConfig(T* system, std::shared_ptr<Config> config,
                                   const std::string& prefix = "");
    
    // Helper to create and initialize a system
    template<typename T>
    static std::shared_ptr<T> createAndInitialize(std::shared_ptr<Config> config,
                                                 const std::string& systemName);
    
    // Safe value getter with defaults
    template<typename T>
    static T getConfigValue(std::shared_ptr<Config> config, 
                           const std::string& key,
                           const T& defaultValue,
                           bool logWarning = false);
}

// Common base classes for neural systems

// INeuralSystem - Common interface for all neural systems
class INeuralSystem {
public:
    virtual ~INeuralSystem() = default;
    
    // Initialize the neural system
    virtual bool initialize(std::shared_ptr<Config> config) = 0;
    
    // Update the neural system
    virtual void update(double dt) = 0;
    
    // Get system name
    virtual std::string getName() const = 0;
    
    // Check if system is initialized
    virtual bool isInitialized() const = 0;
    
    // Save system state
    virtual bool save(const std::string& filepath) const = 0;
    
    // Load system state
    virtual bool load(const std::string& filepath) = 0;
};

// IUpdatable - Common update interface for systems that need regular updates
class IUpdatable {
public:
    virtual ~IUpdatable() = default;
    
    // Update with time delta
    virtual void update(double dt) = 0;
    
    // Check if update is needed
    virtual bool needsUpdate() const = 0;
    
    // Mark for update
    virtual void markForUpdate() = 0;
    
    // Clear update flag
    virtual void clearUpdateFlag() = 0;
};

// ISaveable - Common save/load interface
class ISaveable {
public:
    virtual ~ISaveable() = default;
    
    // Save state to file
    virtual bool save(const std::string& filepath) const = 0;
    
    // Load state from file
    virtual bool load(const std::string& filepath) = 0;
    
    // Get version string for compatibility checking
    virtual std::string getVersion() const = 0;
};

// Template Method pattern for initialization sequences
class InitializationTemplate {
public:
    struct InitializationStep {
        std::string name;
        std::function<bool()> execute;
        std::function<bool()> validate;
        int priority;
        
        InitializationStep(const std::string& n, 
                           std::function<bool()> exec,
                           std::function<bool()> valid,
                           int prio = 0)
            : name(n), execute(exec), validate(valid), priority(prio) {}
    };

    InitializationTemplate();
    ~InitializationTemplate();
    
    // Add initialization step
    void addStep(const InitializationStep& step);
    
    // Execute initialization sequence
    bool executeSequence();
    
    // Get execution log
    const std::vector<std::string>& getExecutionLog() const;
    
    // Disable copying
    InitializationTemplate(const InitializationTemplate&) = delete;
    InitializationTemplate& operator=(const InitializationTemplate&) = delete;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Builder pattern for complex objects
template<typename T>
class ObjectBuilder {
public:
    virtual ~ObjectBuilder() = default;
    
    // Build the object
    virtual std::shared_ptr<T> build() = 0;
    
    // Reset builder state
    virtual void reset() = 0;
};

// Factory pattern for system creation
class SystemFactory {
public:
    static std::shared_ptr<INeuralSystem> createSystem(const std::string& type,
                                                       std::shared_ptr<Config> config);
    
    // Register system factory
    template<typename T>
    static void registerSystemFactory(const std::string& type);
    
    // Get list of registered system types
    static std::vector<std::string> getRegisteredSystemTypes();

private:
    using SystemFactoryFunc = std::function<std::shared_ptr<INeuralSystem>(std::shared_ptr<Config>)>;
    
    static std::map<std::string, SystemFactoryFunc> factoryRegistry_;
};

// Observer pattern for event handling
class EventObserver {
public:
    virtual ~EventObserver() = default;
    
    virtual void onEvent(const std::string& eventType, 
                       const std::map<std::string, std::string>& eventData) = 0;
};

class EventSubject {
public:
    virtual ~EventSubject() = default;
    
    virtual void addObserver(EventObserver* observer) = 0;
    virtual void removeObserver(EventObserver* observer) = 0;
    virtual void notifyObservers(const std::string& eventType,
                                const std::map<std::string, std::string>& eventData) = 0;
};

// CodeOrganizer - Tool to analyze and suggest refactoring
class CodeOrganizer {
public:
    struct RefactoringOpportunity {
        std::string description;
        std::string location;
        std::string suggestedFix;
        double estimatedImpact;  // 0.0 to 1.0
        bool isHighPriority;
        
        RefactoringOpportunity(const std::string& desc,
                              const std::string& loc,
                              const std::string& fix,
                              double impact,
                              bool priority)
            : description(desc), location(loc), suggestedFix(fix),
              estimatedImpact(impact), isHighPriority(priority) {}
    };

    CodeOrganizer();
    ~CodeOrganizer();
    
    // Analyze code for refactoring opportunities
    std::vector<RefactoringOpportunity> analyzeCode(const std::string& directory);
    
    // Generate refactoring report
    std::string generateReport(const std::vector<RefactoringOpportunity>& opportunities);
    
    // Apply suggested refactoring
    bool applyRefactoring(const RefactoringOpportunity& opportunity);
    
    // Check if tool is available
    static bool isAvailable();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ArchitectureDoc - High-level architecture documentation
class ArchitectureDoc {
public:
    struct Component {
        std::string name;
        std::string type;
        std::string purpose;
        std::vector<std::string> dependencies;
        std::vector<std::string> responsibilities;
        
        Component(const std::string& n, const std::string& t,
                 const std::string& p)
            : name(n), type(t), purpose(p) {}
    };

    struct Relationship {
        std::string source;
        std::string target;
        std::string type;  // "depends", "composes", "inherits"
        std::string description;
        
        Relationship(const std::string& src, const std::string& tgt,
                    const std::string& type, const std::string& desc)
            : source(src), target(tgt), type(type), description(desc) {}
    };

    ArchitectureDoc();
    ~ArchitectureDoc();
    
    // Add component to architecture
    void addComponent(const Component& component);
    
    // Add relationship between components
    void addRelationship(const Relationship& relationship);
    
    // Generate architecture documentation
    std::string generateDocumentation() const;
    
    // Save architecture documentation
    bool save(const std::string& filepath) const;
    
    // Load architecture documentation
    bool load(const std::string& filepath);
    
    // Check if file is available
    static bool isAvailable(const std::string& filepath = "ARCHITECTURE.md");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
