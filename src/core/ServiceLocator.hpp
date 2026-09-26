/*
 * NLM Service Locator Pattern - System Access Framework
 * Provides service locator pattern for accessing systems and components in neural system
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
#include <type_traits>

namespace nlm {

// Service Lifecycle
enum class ServiceState {
    Created,
    Initializing,
    Ready,
    ShuttingDown,
    Destroyed
};

// Service Metadata
struct ServiceMetadata {
    std::string name;
    std::string description;
    std::type_index serviceType;
    std::vector<std::string> dependencies;
    bool singleton = false;
    bool autoCreate = true;
    bool autoDestroy = true;
    std::map<std::string, std::any> metadata;
    
    ServiceMetadata() = default;
    ServiceMetadata(const std::string& name, const std::string& desc, 
                   const std::type_index& type)
        : name(name), description(desc), serviceType(type), singleton(false), 
          autoCreate(true), autoDestroy(true) {}
};

// Service Interface
class IService {
public:
    virtual ~IService() = default;
    
    virtual const ServiceMetadata& getMetadata() const = 0;
    virtual ServiceState getState() const = 0;
    
    virtual bool initialize(const std::map<std::string, std::any>& config) = 0;
    virtual void shutdown() = 0;
    
    virtual bool isInitialized() const { return getState() == ServiceState::Ready; }
    virtual bool isSingleton() const { return getMetadata().singleton; }
};

// Typed Service Interface
class ITypedService {
public:
    virtual ~ITypedService() = default;
    
    virtual std::string getServiceName() const = 0;
    virtual std::type_index getServiceType() const = 0;
    
    // Generic get method for retrieving service instance
    template<typename T>
    std::shared_ptr<T> getService() {
        auto service = getServiceImpl();
        if (!service) return nullptr;
        try {
            return std::static_pointer_cast<T>(service);
        } catch (const std::bad_cast&) {
            return nullptr;
        }
    }
    
    virtual std::shared_ptr<IService> getServiceImpl() = 0;
    
    // Check if service is available
    virtual bool hasService() const = 0;
    
    // Get service configuration
    virtual std::map<std::string, std::any> getConfig() const = 0;
    
    // Update service configuration
    virtual bool updateConfig(const std::map<std::string, std::any>& config) = 0;
};

// Service Locator Interface
class IServiceLocator {
public:
    virtual ~IServiceLocator() = default;
    
    // Service registration
    virtual bool registerService(std::shared_ptr<IService> service) = 0;
    virtual bool registerService(std::shared_ptr<ITypedService> service) = 0;
    virtual bool unregisterService(const std::string& serviceName) = 0;
    virtual bool unregisterService(std::type_index serviceType) = 0;
    
    // Service lookup
    virtual std::shared_ptr<IService> getService(const std::string& serviceName) = 0;
    virtual std::shared_ptr<IService> getService(std::type_index serviceType) = 0;
    
    // Query services
    virtual std::vector<std::string> getServiceNames() const = 0;
    virtual std::vector<std::type_index> getServiceTypes() const = 0;
    virtual ServiceMetadata getServiceMetadata(const std::string& serviceName) const = 0;
    virtual ServiceState getServiceState(const std::string& serviceName) const = 0;
    
    // Service lifecycle
    virtual bool initializeService(const std::string& serviceName, 
                                  const std::map<std::string, std::any>& config = {}) = 0;
    virtual bool shutdownService(const std::string& serviceName) = 0;
    virtual bool resetService(const std::string& serviceName) = 0;
    
    // Configuration
    virtual bool configureService(const std::string& serviceName,
                                const std::map<std::string, std::any>& config) = 0;
    virtual std::map<std::string, std::any> getServiceConfig(const std::string& serviceName) const = 0;
    
    // Dependency injection
    virtual bool injectDependencies(const std::string& serviceName,
                                  const std::map<std::string, std::any>& dependencies) = 0;
    virtual std::map<std::string, std::any> getServiceDependencies(const std::string& serviceName) const = 0;
    
    // Service lifecycle management
    virtual std::vector<std::string> getReadyServices() const = 0;
    virtual std::vector<std::string> getServicesByTag(const std::string& tag) const = 0;
    virtual bool hasService(const std::string& serviceName) const = 0;
    virtual bool hasService(std::type_index serviceType) const = 0;
};

// Default Service Locator Implementation
class ServiceLocator : public IServiceLocator {
public:
    ServiceLocator();
    ~ServiceLocator() override;
    
    // Service registration
    bool registerService(std::shared_ptr<IService> service) override;
    bool registerService(std::shared_ptr<ITypedService> service) override;
    bool unregisterService(const std::string& serviceName) override;
    bool unregisterService(std::type_index serviceType) override;
    
    // Service lookup
    std::shared_ptr<IService> getService(const std::string& serviceName) override;
    std::shared_ptr<IService> getService(std::type_index serviceType) override;
    
    // Query services
    std::vector<std::string> getServiceNames() const override;
    std::vector<std::type_index> getServiceTypes() const override;
    ServiceMetadata getServiceMetadata(const std::string& serviceName) const override;
    ServiceState getServiceState(const std::string& serviceName) const override;
    
    // Service lifecycle
    bool initializeService(const std::string& serviceName,
                          const std::map<std::string, std::any>& config = {}) override;
    bool shutdownService(const std::string& serviceName) override;
    bool resetService(const std::string& serviceName) override;
    
    // Configuration
    bool configureService(const std::string& serviceName,
                        const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getServiceConfig(const std::string& serviceName) const override;
    
    // Dependency injection
    bool injectDependencies(const std::string& serviceName,
                          const std::map<std::string, std::any>& dependencies) override;
    std::map<std::string, std::any> getServiceDependencies(const std::string& serviceName) const override;
    
    // Service lifecycle management
    std::vector<std::string> getReadyServices() const override;
    std::vector<std::string> getServicesByTag(const std::string& tag) const override;
    bool hasService(const std::string& serviceName) const override;
    bool hasService(std::type_index serviceType) const override;
    
    // Global instance access
    static ServiceLocator& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Typed Service Implementation
class TypedService : public ITypedService {
public:
    template<typename T>
    TypedService(const std::string& name, std::shared_ptr<T> service)
        : serviceName_(name), serviceType_(typeid(T)), service_(service) {}
    
    std::string getServiceName() const override { return serviceName_; }
    std::type_index getServiceType() const override { return serviceType_; }
    
    std::shared_ptr<IService> getServiceImpl() override { return service_; }
    bool hasService() const override { return service_ != nullptr; }
    
    std::map<std::string, std::any> getConfig() const override {
        // Implementation would depend on T's configuration interface
        return std::map<std::string, std::any>();
    }
    
    bool updateConfig(const std::map<std::string, std::any>& config) override {
        // Implementation would depend on T's configuration interface
        return true;
    }
    
private:
    std::string serviceName_;
    std::type_index serviceType_;
    std::shared_ptr<IService> service_;
};

// Service Factory Interface
class IServiceFactory {
public:
    virtual ~IServiceFactory() = default;
    
    virtual std::shared_ptr<IService> createService(const std::map<std::string, std::any>& config = {}) = 0;
    virtual ServiceMetadata getServiceMetadata() const = 0;
    virtual bool canCreateService(const std::map<std::string, std::any>& config) const = 0;
};

// Generic Service Factory
class ServiceFactory {
public:
    template<typename T>
    class Factory : public IServiceFactory {
    public:
        Factory(std::function<std::shared_ptr<T>(const std::map<std::string, std::any>&)> creator,
               const std::string& name, const std::string& description)
            : creator_(creator) {
            metadata_.name = name;
            metadata_.description = description;
            metadata_.serviceType = typeid(T);
            metadata_.singleton = false;
        }
        
        std::shared_ptr<IService> createService(const std::map<std::string, std::any>& config) override {
            auto service = creator_(config);
            return service;
        }
        
        ServiceMetadata getServiceMetadata() const override {
            return metadata_;
        }
        
        bool canCreateService(const std::map<std::string, std::any>& config) const override {
            return true; // Simplified
        }
        
    private:
        std::function<std::shared_ptr<T>(const std::map<std::string, std::any>&)> creator_;
        ServiceMetadata metadata_;
    };
    
    template<typename T>
    static Factory<T> createFactory(const std::string& name, const std::string& description,
                                  std::function<std::shared_ptr<T>(const std::map<std::string, std::any>&)> creator) {
        return Factory<T>(creator, name, description);
    }
};

// Service System - Manages service lifecycle and dependency injection
class ServiceSystem {
public:
    ServiceSystem();
    ~ServiceSystem();
    
    // Initialize the service system
    bool initialize();
    
    // Shutdown the service system
    void shutdown();
    
    // Get the global service locator
    IServiceLocator& getLocator() { return locator_; }
    
    // Register system services
    void registerSystemServices();
    
    // Load services from configuration
    bool loadServicesFromConfig(const std::string& configFile);
    
    // Get system status
    std::string getSystemStatus() const;
    
    // Global instance access
    static ServiceSystem& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    IServiceLocator& locator_;
};

// Service Registration Macros
#define REGISTER_SERVICE(serviceClass) \
    namespace { \
        struct ServiceRegistration { \
            ServiceRegistration() { \
                ServiceLocator& locator = ServiceLocator::getInstance(); \
                locator.registerService(std::make_shared<serviceClass>()); \
            } \
        }; \
        static ServiceRegistration serviceReg; \
    }

#define REGISTER_TYPED_SERVICE(serviceClass, serviceName) \
    namespace { \
        struct TypedServiceRegistration { \
            TypedServiceRegistration() { \
                ServiceLocator& locator = ServiceLocator::getInstance(); \
                auto typedService = std::make_shared<TypedService<serviceClass>>(serviceName, std::make_shared<serviceClass>()); \
                locator.registerService(typedService); \
            } \
        }; \
        static TypedServiceRegistration typedServiceReg; \
    }

} // namespace nlm