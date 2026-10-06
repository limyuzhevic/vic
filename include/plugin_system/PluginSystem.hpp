#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <any>
#include <functional>
#include <typeindex>

namespace nlm {

enum class PluginType {
    General,
    Service,
    BrainExtension,
    EventHandler,
    DataProcessor,
    Sensor,
    Motor,
    Memory,
    Cognitive,
    Neuromodulation
};

enum class PluginState {
    Unloaded,
    Loading,
    Loaded,
    Initializing,
    Initialized,
    Enabling,
    Enabled,
    Disabling,
    Disabled,
    Error
};

enum class PluginEventType {
    Load,
    Unload,
    Enable,
    Disable,
    Initialize,
    Shutdown,
    Error,
    ServiceRegistered,
    ServiceUnregistered,
    Custom
};

enum class ServiceType {
    Unknown,
    Data,
    Computation,
    Communication,
    Control,
    Perception,
    Action,
    Memory,
    Planning,
    Learning
};

enum class ExecutionStatus {
    Pending,
    Running,
    Completed,
    Failed,
    Cancelled,
    Timeout
};

struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string pluginPath;
    std::vector<std::string> dependencies;
    bool requiresBrain;
    bool autoEnable;
    PluginType type;
    std::map<std::string, std::string> metadata;
    
    PluginMetadata()
        : version("1.0.0")
        , author("Unknown")
        , requiresBrain(false)
        , autoEnable(false)
        , type(PluginType::General) {}
};

struct ServiceInfo {
    std::string name;
    std::string description;
    ServiceType type;
    std::type_index interfaceType;
    bool isSingleton;
    std::map<std::string, std::string> metadata;
    
    ServiceInfo()
        : isSingleton(false)
        , type(ServiceType::Unknown) {}
};

struct PluginEvent {
    PluginEventType type;
    std::string pluginName;
    std::map<std::string, std::any> data;
    Timestamp timestamp;
    
    PluginEvent()
        : timestamp(0.0) {}
};

struct ExecutionContext {
    std::string pluginName;
    std::string operation;
    std::map<std::string, std::string> arguments;
    std::map<std::string, std::any> typedArguments;
    std::string targetComponent;
    Timestamp timestamp;
    std::string source;
    std::map<std::string, std::any> result;
    ExecutionStatus status;
    
    ExecutionContext()
        : timestamp(0.0)
        , status(ExecutionStatus::Pending) {}
};

struct ServiceRequest {
    std::string serviceName;
    std::string operation;
    std::map<std::string, std::any> parameters;
    Timestamp timestamp;
    std::string requesterId;
    
    ServiceRequest()
        : timestamp(0.0) {}
};

struct ServiceResponse {
    bool success;
    std::map<std::string, std::any> data;
    std::string errorMessage;
    Timestamp timestamp;
    
    ServiceResponse()
        : success(false)
        , timestamp(0.0) {}
};

class Plugin;
class Service;

using PluginLoadListener = std::function<void(const PluginEvent&)>;
using PluginEventListener = std::function<void(const PluginEvent&)>;
using ServiceRequestHandler = std::function<bool(const ServiceRequest&, ServiceResponse&)>;

class Plugin {
public:
    virtual ~Plugin() {}
    
    virtual const PluginMetadata& getMetadata() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    virtual std::vector<ServiceInfo> getProvidedServices() const = 0;
    virtual bool onServiceRequest(const ServiceRequest& request, ServiceResponse& response) = 0;
    virtual bool onPluginEvent(const PluginEvent& event) = 0;
    virtual bool execute(const ExecutionContext& context) = 0;
    virtual void onBrainUpdate(const class BrainUpdateEvent& event) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    
protected:
    void registerService(const ServiceInfo& serviceInfo);
    void unregisterService(const std::string& serviceName);
    void emitEvent(const PluginEvent& event);
    
private:
    std::vector<ServiceInfo> providedServices_;
};

class Service {
public:
    virtual ~Service() {}
    
    virtual const ServiceInfo& getInfo() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    virtual bool handleRequest(const ServiceRequest& request, ServiceResponse& response) = 0;
    virtual std::string getName() const = 0;
    virtual ServiceType getType() const = 0;
};

class PluginLoadException : public std::runtime_error {
public:
    PluginLoadException(const std::string& message) : std::runtime_error(message) {}
};

class PluginInitializationException : public std::runtime_error {
public:
    PluginInitializationException(const std::string& message) : std::runtime_error(message) {}
};

class PluginExecutionException : public std::runtime_error {
public:
    PluginExecutionException(const std::string& message) : std::runtime_error(message) {}
};

class ServiceNotFoundException : public std::runtime_error {
public:
    ServiceNotFoundException(const std::string& serviceName) : std::runtime_error("Service not found: " + serviceName) {}
};

class ServiceInvocationException : public std::runtime_error {
public:
    ServiceInvocationException(const std::string& serviceName, const std::string& error) : std::runtime_error("Service invocation failed for " + serviceName + ": " + error) {}
};

class PluginSystemException : public std::runtime_error {
public:
    PluginSystemException(const std::string& message) : std::runtime_error(message) {}
};

} // namespace nlm
