/*
 * NLM Dependency Injection Framework - Loose Coupling
 * Provides dependency injection and inversion of control for neural system
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <any>
#include <stdexcept>
#include <mutex>
#include <list>
#include <algorithm>

namespace nlm {

// Dependency Lifecycle
enum class DependencyState {
    Created,
    Resolving,
    Resolved,
    Initializing,
    Initialized,
    ShuttingDown,
    Destroyed,
    Error
};

// Dependency Registration Mode
enum class DependencyMode {
    Singleton,
    Transient,
    Factory,
    Lazy,
    Conditional
};

// Dependency Injection Configuration
struct DependencyConfig {
    std::string name;
    std::string description;
    std::type_index dependencyType;
    std::vector<std::string> dependencyNames;
    DependencyMode mode;
    bool optional = false;
    bool autoCreate = true;
    std::map<std::string, std::any> metadata;
    
    DependencyConfig() = default;
    DependencyConfig(const std::string& name, const std::string& desc,
                     const std::type_index& type)
        : name(name), description(desc), dependencyType(type), mode(DependencyMode::Singleton),
          optional(false), autoCreate(true) {}
};

// Dependency Interface
class IDependency {
public:
    virtual ~IDependency() = default;
    
    virtual const DependencyConfig& getConfig() const = 0;
    virtual DependencyState getState() const = 0;
    
    virtual bool resolve() = 0;
    virtual void shutdown() = 0;
    
    virtual bool isResolved() const { return getState() == DependencyState::Resolved; }
    virtual bool isInitialized() const { return getState() == DependencyState::Initialized; }
    
    virtual std::any getValue() = 0;
    virtual void setValue(const std::any& value) = 0;
};

// Typed Dependency Interface
class ITypedDependency {
public:
    virtual ~ITypedDependency() = default;
    
    virtual std::string getDependencyName() const = 0;
    virtual std::type_index getDependencyType() const = 0;
    
    // Generic get method for retrieving dependency instance
    template<typename T>
    std::shared_ptr<T> getDependency() {
        auto dependency = getDependencyImpl();
        if (!dependency) return nullptr;
        try {
            return std::static_pointer_cast<T>(dependency);
        } catch (const std::bad_cast&) {
            return nullptr;
        }
    }
    
    virtual std::shared_ptr<IDependency> getDependencyImpl() = 0;
    
    // Check if dependency is available
    virtual bool hasDependency() const = 0;
    
    // Get dependency metadata
    virtual std::map<std::string, std::any> getMetadata() const = 0;
    
    // Update dependency metadata
    virtual bool updateMetadata(const std::map<std::string, std::any>& metadata) = 0;
};

// Dependency Injection Container Interface
class IDependencyInjectionContainer {
public:
    virtual ~IDependencyInjectionContainer() = default;
    
    // Dependency registration
    virtual bool registerDependency(std::shared_ptr<IDependency> dependency) = 0;
    virtual bool registerDependency(std::shared_ptr<ITypedDependency> dependency) = 0;
    virtual bool unregisterDependency(const std::string& dependencyName) = 0;
    virtual bool unregisterDependency(std::type_index dependencyType) = 0;
    
    // Dependency resolution
    virtual std::shared_ptr<IDependency> getDependency(const std::string& dependencyName) = 0;
    virtual std::shared_ptr<IDependency> getDependency(std::type_index dependencyType) = 0;
    
    // Dependency configuration
    virtual bool configureDependency(const std::string& dependencyName,
                                    const std::map<std::string, std::any>& config) = 0;
    virtual std::map<std::string, std::any> getDependencyConfig(const std::string& dependencyName) const = 0;
    
    // Dependency lifecycle
    virtual bool resolveDependency(const std::string& dependencyName) = 0;
    virtual bool initializeDependency(const std::string& dependencyName) = 0;
    virtual bool shutdownDependency(const std::string& dependencyName) = 0;
    
    // Dependency query
    virtual std::vector<std::string> getDependencyNames() const = 0;
    virtual std::vector<std::type_index> getDependencyTypes() const = 0;
    virtual DependencyConfig getDependencyConfig(const std::string& dependencyName) const = 0;
    virtual DependencyState getDependencyState(const std::string& dependencyName) const = 0;
    
    // Dependency management
    virtual bool hasDependency(const std::string& dependencyName) const = 0;
    virtual bool hasDependency(std::type_index dependencyType) const = 0;
    virtual std::vector<std::string> getResolvedDependencies() const = 0;
    virtual std::string getContainerStatus() const = 0;
};

// Default Dependency Injection Container Implementation
class DependencyInjectionContainer : public IDependencyInjectionContainer {
public:
    DependencyInjectionContainer();
    ~DependencyInjectionContainer() override;
    
    // Dependency registration
    bool registerDependency(std::shared_ptr<IDependency> dependency) override;
    bool registerDependency(std::shared_ptr<ITypedDependency> dependency) override;
    bool unregisterDependency(const std::string& dependencyName) override;
    bool unregisterDependency(std::type_index dependencyType) override;
    
    // Dependency resolution
    std::shared_ptr<IDependency> getDependency(const std::string& dependencyName) override;
    std::shared_ptr<IDependency> getDependency(std::type_index dependencyType) override;
    
    // Dependency configuration
    bool configureDependency(const std::string& dependencyName,
                            const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getDependencyConfig(const std::string& dependencyName) const override;
    
    // Dependency lifecycle
    bool resolveDependency(const std::string& dependencyName) override;
    bool initializeDependency(const std::string& dependencyName) override;
    bool shutdownDependency(const std::string& dependencyName) override;
    
    // Dependency query
    std::vector<std::string> getDependencyNames() const override;
    std::vector<std::type_index> getDependencyTypes() const override;
    DependencyConfig getDependencyConfig(const std::string& dependencyName) const override;
    DependencyState getDependencyState(const std::string& dependencyName) const override;
    
    // Dependency management
    bool hasDependency(const std::string& dependencyName) const override;
    bool hasDependency(std::type_index dependencyType) const override;
    std::vector<std::string> getResolvedDependencies() const override;
    std::string getContainerStatus() const override;
    
    // Global instance access
    static DependencyInjectionContainer& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Dependency Registration Macros
#define REGISTER_DEPENDENCY(dependencyClass, ...) \
    namespace { \
        struct DependencyRegistration { \
            DependencyRegistration() { \
                IDependencyInjectionContainer& container = IDependencyInjectionContainer::getInstance(); \
                container.registerDependency(std::make_shared<dependencyClass>(__VA_ARGS__)); \
            } \
        }; \
        static DependencyRegistration dependencyReg; \
    }

// Dependency Factory
class DependencyFactory {
public:
    template<typename T, typename... Args>
    static std::function<std::shared_ptr<IDependency>(const std::map<std::string, std::any>&)> createFactory(Args... args) {
        return [args...](const std::map<std::string, std::any>& config) -> std::shared_ptr<IDependency> {
            auto dependency = std::make_shared<T>(std::forward<Args>(args)...);
            return dependency;
        };
    }
    
    template<typename T>
    static std::function<std::shared_ptr<IDependency>(const std::map<std::string, std::any>&)> createConfigurableFactory() {
        return [](const std::map<std::string, std::any>& config) -> std::shared_ptr<IDependency> {
            // For templated dependencies, configuration would be handled differently
            throw std::runtime_error("Configuration-based dependency creation not implemented for type " + 
                                   std::string(typeid(T).name());
        };
    }
};

// Dependency Injection Utilities
class DependencyInjectionUtils {
public:
    // Configuration template management
    static std::map<std::string, std::any> createDependencyConfig(const std::string& dependencyName,
                                                                const std::map<std::string, std::any>& defaults);
    
    // Dependency resolution
    static std::vector<std::string> resolveDependencies(const std::vector<std::string>& dependencyNames,
                                                      const std::map<std::string, DependencyState>& dependencyStates);
    
    // Dependency health checking
    static bool checkDependencyHealth(const std::shared_ptr<IDependency>& dependency);
    static std::string getDependencyHealthReport(const std::vector<std::shared_ptr<IDependency>>& dependencies);
    
    // Configuration validation
    static bool validateDependencyConfiguration(const std::map<std::string, std::any>& config);
    
    // Dependency lifecycle utilities
    static std::string getDependencyStateString(DependencyState state);
    static DependencyState stringToDependencyState(const std::string& stateStr);
    
    // Factory utilities
    static std::shared_ptr<IDependency> createDependencyFromFactory(
        const std::string& factoryName,
        const std::map<std::string, std::any>& config);
};

// Dependency Injection Framework
class DependencyInjectionFramework {
public:
    DependencyInjectionFramework();
    ~DependencyInjectionFramework();
    
    // Initialize the dependency injection framework
    bool initialize();
    
    // Shutdown the dependency injection framework
    void shutdown();
    
    // Get the global dependency injection container
    IDependencyInjectionContainer& getContainer() { return container_; }
    
    // Register core dependencies
    void registerCoreDependencies();
    
    // Dependency lifecycle management
    bool resolveAllDependencies();
    bool initializeAllDependencies();
    bool shutdownAllDependencies();
    
    // Dependency validation
    bool validateDependencyInjection();
    std::string getDependencyDiagnostics() const;
    
    // Get system status
    std::string getSystemStatus() const;
    std::map<std::string, std::any> getDependencyStatistics() const;
    
    // Global instance access
    static DependencyInjectionFramework& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    IDependencyInjectionContainer& container_;
};

} // namespace nlm