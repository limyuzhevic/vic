/*
 * NLM Extension System - Plugin Development Framework
 * Provides extension points, hooks, and extensibility mechanisms for neural system
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

namespace nlm {

// Extension Type
enum class ExtensionType {
    Hook,
    Interface,
    Implementation,
    Bridge,
    Adapter
};

// Extension Point Definition
struct ExtensionPoint {
    std::string name;
    std::string description;
    std::type_index extensionType;
    bool optional = false;
    std::map<std::string, std::any> metadata;
    
    ExtensionPoint() = default;
    ExtensionPoint(const std::string& name, const std::string& desc, 
                   const std::type_index& type, bool optional = false)
        : name(name), description(desc), extensionType(type), optional(optional) {}
};

// Hook Interface
class IHook {
public:
    virtual ~IHook() = default;
    
    virtual std::string getHookName() const = 0;
    virtual std::type_index getArgumentType() const = 0;
    virtual std::type_index getReturnType() const = 0;
    
    virtual void execute(const std::any& argument) = 0;
    virtual bool isAsync() const { return false; }
};

// Hook Implementation
class Hook : public IHook {
public:
    Hook(const std::string& name, const std::type_index& argType, 
         const std::type_index& retType)
        : hookName_(name), argumentType_(argType), returnType_(retType) {}
    
    std::string getHookName() const override { return hookName_; }
    std::type_index getArgumentType() const override { return argumentType_; }
    std::type_index getReturnType() const override { return returnType_; }
    
    void execute(const std::any& argument) override {
        if (argument.type() != argumentType_) {
            throw std::runtime_error("Hook argument type mismatch");
        }
        onExecute(argument);
    }
    
    void setExecuteCallback(std::function<void(const std::any&)> callback) {
        executeCallback_ = callback;
    }
    
    void setAsync(bool async) {
        async_ = async;
    }
    
    bool isAsync() const override { return async_; }
    
    void setArgumentExtractor(std::function<std::any(const std::any&)> extractor) {
        argumentExtractor_ = extractor;
    }
    
    std::any extractArgument(const std::any& data) {
        if (argumentExtractor_) {
            return argumentExtractor_(data);
        }
        return data;
    }
    
protected:
    virtual void onExecute(const std::any& argument) {
        if (executeCallback_) {
            executeCallback_(argument);
        }
    }
    
private:
    std::string hookName_;
    std::type_index argumentType_;
    std::type_index returnType_;
    bool async_ = false;
    std::function<void(const std::any&)> executeCallback_;
    std::function<std::any(const std::any&)> argumentExtractor_;
};

// Interface Extension
class IInterfaceExtension {
public:
    virtual ~IInterfaceExtension() = default;
    virtual std::string getInterfaceName() const = 0;
    virtual std::any getInterface() = 0;
};

// Interface Extension Implementation
class InterfaceExtension : public IInterfaceExtension {
public:
    InterfaceExtension(const std::string& interfaceName, std::any interface)
        : interfaceName_(interfaceName), interface_(interface) {}
    
    std::string getInterfaceName() const override { return interfaceName_; }
    std::any getInterface() override { return interface_; }
    
private:
    std::string interfaceName_;
    std::any interface_;
};

// Implementation Extension
class IImplementationExtension {
public:
    virtual ~IImplementationExtension() = default;
    virtual std::string getExtensionPoint() const = 0;
    virtual std::any getImplementation() = 0;
    virtual std::map<std::string, std::any> getParameters() const = 0;
};

// Implementation Extension Implementation
class ImplementationExtension : public IImplementationExtension {
public:
    ImplementationExtension(const std::string& extensionPoint, std::any implementation,
                           const std::map<std::string, std::any>& parameters = {})
        : extensionPoint_(extensionPoint), implementation_(implementation), parameters_(parameters) {}
    
    std::string getExtensionPoint() const override { return extensionPoint_; }
    std::any getImplementation() override { return implementation_; }
    std::map<std::string, std::any> getParameters() const override { return parameters_; }
    
private:
    std::string extensionPoint_;
    std::any implementation_;
    std::map<std::string, std::any> parameters_;
};

// Extension Manager - Manages all extensions
class IExtensionManager {
public:
    virtual ~IExtensionManager() = default;
    
    // Extension point management
    virtual bool registerExtensionPoint(const ExtensionPoint& extensionPoint) = 0;
    virtual bool unregisterExtensionPoint(const std::string& name) = 0;
    virtual ExtensionPoint getExtensionPoint(const std::string& name) const = 0;
    virtual std::vector<ExtensionPoint> getAllExtensionPoints() const = 0;
    
    // Hook management
    virtual bool registerHook(const std::string& extensionPointName, std::shared_ptr<IHook> hook) = 0;
    virtual bool unregisterHook(const std::string& extensionPointName, const std::string& hookName) = 0;
    virtual std::vector<std::shared_ptr<IHook>> getHooks(const std::string& extensionPointName) const = 0;
    
    // Interface extension management
    virtual bool registerInterfaceExtension(const std::string& extensionPointName, 
                                           std::shared_ptr<IInterfaceExtension> extension) = 0;
    virtual std::vector<std::shared_ptr<IInterfaceExtension>> getInterfaceExtensions(
        const std::string& extensionPointName) const = 0;
    
    // Implementation extension management
    virtual bool registerImplementationExtension(const std::string& extensionPointName,
                                                std::shared_ptr<IImplementationExtension> extension) = 0;
    virtual std::vector<std::shared_ptr<IImplementationExtension>> getImplementationExtensions(
        const std::string& extensionPointName) const = 0;
    
    // Extension execution
    virtual std::any executeHooks(const std::string& extensionPointName, const std::any& argument) = 0;
    virtual std::vector<std::any> executeAllHooks(const std::string& extensionPointName, const std::any& argument) = 0;
    
    // Extension discovery
    virtual std::vector<std::string> getExtensionPointsForType(ExtensionType type) const = 0;
    virtual bool hasExtensionPoint(const std::string& name) const = 0;
};

// Default extension manager implementation
class ExtensionManager : public IExtensionManager {
public:
    ExtensionManager();
    ~ExtensionManager() override;
    
    // Extension point management
    bool registerExtensionPoint(const ExtensionPoint& extensionPoint) override;
    bool unregisterExtensionPoint(const std::string& name) override;
    ExtensionPoint getExtensionPoint(const std::string& name) const override;
    std::vector<ExtensionPoint> getAllExtensionPoints() const override;
    
    // Hook management
    bool registerHook(const std::string& extensionPointName, std::shared_ptr<IHook> hook) override;
    bool unregisterHook(const std::string& extensionPointName, const std::string& hookName) override;
    std::vector<std::shared_ptr<IHook>> getHooks(const std::string& extensionPointName) const override;
    
    // Interface extension management
    bool registerInterfaceExtension(const std::string& extensionPointName,
                                   std::shared_ptr<IInterfaceExtension> extension) override;
    std::vector<std::shared_ptr<IInterfaceExtension>> getInterfaceExtensions(
        const std::string& extensionPointName) const override;
    
    // Implementation extension management
    bool registerImplementationExtension(const std::string& extensionPointName,
                                        std::shared_ptr<IImplementationExtension> extension) override;
    std::vector<std::shared_ptr<IImplementationExtension>> getImplementationExtensions(
        const std::string& extensionPointName) const override;
    
    // Extension execution
    std::any executeHooks(const std::string& extensionPointName, const std::any& argument) override;
    std::vector<std::any> executeAllHooks(const std::string& extensionPointName, const std::any& argument) override;
    
    // Extension discovery
    std::vector<std::string> getExtensionPointsForType(ExtensionType type) const override;
    bool hasExtensionPoint(const std::string& name) const override;
    
    // Global instance access
    static ExtensionManager& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Hook Builder - Helper for creating hooks
class HookBuilder {
public:
    template<typename ArgumentType, typename ReturnType>
    static std::shared_ptr<Hook> createHook(const std::string& name,
                                          std::function<ReturnType(ArgumentType)> callback) {
        auto hook = std::make_shared<Hook>(name, typeid(ArgumentType), typeid(ReturnType));
        
        // Create wrapper that extracts argument and calls callback
        auto wrapper = [callback](const std::any& argument) {
            auto arg = std::any_cast<ArgumentType>(argument);
            auto result = callback(arg);
            // Return result would need to be handled differently
            // This is simplified for demonstration
        };
        
        hook->setExecuteCallback(wrapper);
        return hook;
    }
    
    template<typename ArgumentType>
    static std::shared_ptr<Hook> createHook(const std::string& name,
                                          std::function<void(ArgumentType)> callback,
                                          bool async = false) {
        auto hook = std::make_shared<Hook>(name, typeid(ArgumentType), typeid(void));
        hook->setExecuteCallback([callback](const std::any& argument) {
            auto arg = std::any_cast<ArgumentType>(argument);
            callback(arg);
        });
        hook->setAsync(async);
        return hook;
    }
    
    // Hook with argument extractor
    template<typename ArgumentType, typename DataType>
    static std::shared_ptr<Hook> createHook(const std::string& name,
                                          std::function<void(ArgumentType)> callback,
                                          std::function<ArgumentType(DataType)> extractor,
                                          bool async = false) {
        auto hook = std::make_shared<Hook>(name, typeid(ArgumentType), typeid(void));
        hook->setExecuteCallback([callback, extractor](const std::any& data) {
            auto arg = extractor(std::any_cast<DataType>(data));
            callback(arg);
        });
        hook->setAsync(async);
        return hook;
    }
};

// Extension System - Main extension management
class ExtensionSystem {
public:
    ExtensionSystem();
    ~ExtensionSystem();
    
    // Initialize the extension system
    bool initialize();
    
    // Shutdown the extension system
    void shutdown();
    
    // Get the global extension manager
    IExtensionManager& getManager() { return manager_; }
    
    // Register common extension points
    void registerCommonExtensionPoints();
    
    // Execute system-wide hooks
    template<typename ArgumentType>
    void executeSystemHooks(const std::string& extensionPointName, const ArgumentType& argument) {
        auto hooks = manager_.getHooks(extensionPointName);
        for (auto& hook : hooks) {
            try {
                std::any arg = argument;
                hook->execute(arg);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error executing hook " + hook->getHookName() + ": " + std::string(e.what()));
            }
        }
    }
    
    // Global instance access
    static ExtensionSystem& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    IExtensionManager& manager_;
};

} // namespace nlm