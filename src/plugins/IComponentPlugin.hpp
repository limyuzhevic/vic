#pragma once

#include "IPlugin.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {
namespace plugins {

// Component system types
enum class ComponentType {
    NEURAL_SYSTEM,
    PLASTICITY_RULE,
    NEUROMODULATOR,
    MEMORY_SYSTEM,
    DEVELOPMENTAL,
    PREDICTION,
    COGNITION,
    MOTOR,
    SENSORY,
    ENVIRONMENTAL,
    UTILITY,
    CUSTOM
};

// Component state
enum class ComponentState {
    UNINITIALIZED,
    INITIALIZING,
    READY,
    RUNNING,
    PAUSED,
    TERMINATING,
    TERMINATED,
    ERROR
};

// Component interface for system-level components
class IComponentPlugin : public virtual IPlugin {
public:
    virtual ~IComponentPlugin() = default;
    
    // Component identification
    virtual std::string getComponentType() const = 0;
    virtual std::string getComponentName() const = 0;
    virtual ComponentType getCategory() const = 0;
    
    // Component lifecycle
    virtual bool initializeComponent(std::unordered_map<std::string, std::string> config) = 0;
    virtual void updateComponent(TimestepDuration dt) = 0;
    virtual void shutdownComponent() = 0;
    
    // Component state
    virtual ComponentState getComponentState() const = 0;
    virtual std::vector<std::string> getStateTransitions() const = 0;
    
    // System integration
    virtual bool connectToSystem(const std::string& systemName, void* systemInterface) = 0;
    virtual bool disconnectFromSystem(const std::string& systemName) = 0;
    virtual std::vector<std::string> getConnectedSystems() const = 0;
    
    // Data exchange
    virtual bool sendData(const std::string& target, const std::vector<float>& data) = 0;
    virtual bool receiveData(const std::string& source, std::vector<float>& data) = 0;
    virtual std::unordered_map<std::string, std::vector<float>> getAvailableData() const = 0;
    
    // Configuration schema
    virtual std::string getConfigurationSchema() const = 0;
    virtual std::vector<std::string> getRequiredParameters() const = 0;
    virtual std::vector<std::string> getOptionalParameters() const = 0;
    
    // Validation and diagnostics
    virtual bool validateConfiguration() const = 0;
    virtual std::vector<std::string> getValidationErrors() const = 0;
    virtual std::vector<std::string> getWarnings() const = 0;
    
    // Performance metrics
    virtual double getComponentUptime() const = 0;
    virtual uint64_t getTotalOperations() const = 0;
    virtual double getAverageOperationTime() const = 0;
    
    // Resource management
    virtual size_t getMemoryUsage() const = 0;
    virtual size_t getResourceUsage() const = 0;
    virtual void reportResourceUsage() = 0;
    
    // Serialization support
    virtual bool serialize(std::ostream& output) const = 0;
    virtual bool deserialize(std::istream& input) = 0;
    
    // Component metadata
    virtual std::string getAuthor() const = 0;
    virtual std::string getCopyright() const = 0;
    virtual std::string getLicense() const = 0;
    virtual std::string getWebsite() const = 0;
    
    // Update frequency
    virtual TimestepDuration getUpdateInterval() const = 0;
    virtual void setUpdateInterval(TimestepDuration interval) = 0;
    
    // Priority system
    virtual int getPriority() const = 0;
    virtual void setPriority(int priority) = 0;
    
    // Health monitoring
    virtual bool isHealthy() const = 0;
    virtual std::vector<std::string> getHealthIssues() const = 0;
    virtual void performHealthCheck() = 0;
};

} // namespace plugins
} // namespace nlm
