#pragma once

#include "Action.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <stdexcept>
#include <unordered_set>
#include <chrono>
#include <algorithm>
#include <optional>
#include "../core/Config/Config.hpp"

// Forward declarations
using SimulationStep = int;

namespace nlm {

// Action event structure for event-driven architecture
struct ActionEvent {
    std::unique_ptr<Action> action;
    ActionResult result;
    SimulationStep step;
    auto timestamp;
    std::string eventType;
    std::unordered_map<std::string, float> context;
};

// Memory pool for efficient action allocation
class ActionMemoryPool {
public:
    static std::unique_ptr<Action> allocate(ActionType type);
    static void deallocate(std::unique_ptr<Action>& action);
    static void clear();
    
private:
    static std::vector<std::unique_ptr<Action>> actionPool_;
};

// Action factory for creating actions with validation
class ActionFactory {
public:
    static std::unique_ptr<Action> createAction(ActionType type,
                                                const std::vector<float>& parameters = {});
    static std::unique_ptr<Action> createActionFromTemplate(
        const std::string& templateName,
        const std::vector<float>& parameters);
    
    static void registerType(ActionType type, const std::string& name);
    static std::vector<std::string> getRegisteredTypes() const;
    
private:
    static std::map<ActionType, std::string> typeNames_;
};

// Action statistics tracker
class ActionStatsTracker {
public:
    static void recordAction(const ActionResult& result);
    static struct Stats {
        size_t totalActions = 0;
        size_t successfulActions = 0;
        size_t failedActions = 0;
        double averageExecutionTime = 0.0;
        std::unordered_map<ActionType, size_t> actionsByType;
        std::chrono::steady_clock::time_point firstActionTime;
        std::chrono::steady_clock::time_point lastActionTime;
    } getStats();
    
    static void reset();
    static void exportStats(const std::string& filename);
    
private:
    static Stats stats_;
};

// Enhanced action parameter system
class ActionParameter {
public:
    enum class Type {
        Float,
        Int,
        Bool,
        String,
        Action,
        Vector,
        Custom
    };
    
    ActionParameter(const std::string& name, Type type, float defaultValue = 0.0f);
    ActionParameter(const std::string& name, Type type, int defaultValue = 0);
    ActionParameter(const std::string& name, Type type, bool defaultValue = false);
    ActionParameter(const std::string& name, Type type, const std::string& defaultValue);
    ActionParameter(const std::string& name, Type type, const std::vector<float>& defaultValue);
    
    std::string getName() const;
    Type getType() const;
    
    // Float conversions (for backward compatibility)
    float asFloat() const;
    int asInt() const;
    bool asBool() const;
    std::string asString() const;
    const std::vector<float>& asVector() const;
    
    // Setters
    void setFloat(float value);
    void setInt(int value);
    void setBool(bool value);
    void setString(const std::string& value);
    void setVector(const std::vector<float>& value);
    
    // Validation
    void validate(float min = -std::numeric_limits<float>::max(), 
                  float max = std::numeric_limits<float>::max()) const;

private:
    std::string name_;
    Type type_;
    union Value {
        float floatValue;
        int intValue;
        bool boolValue;
        std::string* stringValue;
        std::vector<float>* vectorValue;
        
        Value() : floatValue(0.0f) {}
        ~Value() {
            if (type_ == Type::String) delete stringValue;
            if (type_ == Type::Vector) delete vectorValue;
        }
    } value_;
    Type type_;
};

// Action execution result
class ActionResult {
public:
    enum class Status {
        Success,
        Failed,
        PartialSuccess,
        Pending,
        Cancelled,
        TimedOut
    };
    
    ActionResult(std::unique_ptr<Action> action, Status status = Status::Success);
    ~ActionResult();
    
    std::unique_ptr<Action> getAction() const;
    Status getStatus() const;
    void setStatus(Status status);
    
    const std::string& getMessage() const;
    void setMessage(const std::string& message);
    
    const std::vector<float>& getResults() const;
    void setResults(const std::vector<float>& results);
    
    // Timing information
    auto getStartTime() const;
    auto getEndTime() const;
    double getDuration() const;
    
    bool isSuccess() const;
    bool isFailed() const;

private:
    std::unique_ptr<Action> action_;
    Status status_;
    std::string message_;
    std::vector<float> results_;
    auto startTime_;
    auto endTime_;
};

// Action execution callback
class ActionCallback {
public:
    virtual ~ActionCallback() = default;
    
    virtual void onActionStart(const ActionResult& result) = 0;
    virtual void onActionComplete(const ActionResult& result) = 0;
    virtual void onActionFailed(const ActionResult& result) = 0;
    virtual void onActionProgress(const ActionResult& result, float progress) = 0;
    
    // Get callback ID for registration
    virtual std::string getId() const = 0;
};

// Action template for predefined patterns
class ActionTemplate {
public:
    ActionTemplate(const std::string& name, const std::string& description);
    ~ActionTemplate();
    
    std::string getName() const;
    std::string getDescription() const;
    
    const std::vector<ActionParameter>& getParameters() const;
    void addParameter(const ActionParameter& param);
    
    std::unique_ptr<Action> createAction(const std::vector<float>& paramValues) const;
    
    // Template validation
    bool validateParameters(const std::vector<float>& paramValues) const;
    std::string getValidationError(const std::vector<float>& paramValues) const;

private:
    std::string name_;
    std::string description_;
    std::vector<ActionParameter> parameters_;
    
    mutable std::vector<float> tempParams_;
};

// Multi-action coordinator
class ActionSequence {
public:
    ActionSequence(const std::string& name);
    ~ActionSequence();
    
    std::string getName() const;
    void addAction(std::unique_ptr<Action> action);
    void addAction(std::unique_ptr<Action> action, bool optional);
    
    const std::vector<std::unique_ptr<Action>>& getActions() const;
    const std::vector<bool>& getActionOptions() const;
    
    // Execute the sequence
    std::vector<ActionResult> execute(const std::unordered_set<std::string>& tags = {});
    
    // Execution control
    void stop();
    bool isExecuting() const;
    
    // Result management
    const std::vector<ActionResult>& getResults() const;
    void clearResults();
    
    // Callbacks
    void addCallback(std::unique_ptr<ActionCallback> callback);
    void removeCallback(const std::string& callbackId);

private:
    std::string name_;
    std::vector<std::unique_ptr<Action>> actions_;
    std::vector<bool> actionOptions_;
    bool executing_;
    std::vector<ActionResult> results_;
    std::vector<std::unique_ptr<ActionCallback>> callbacks_;
    
    void notifyCallbacks(const ActionResult& result, const std::string& event);
};

// Action validation system
class ActionValidator {
public:
    struct ValidationRule {
        std::string name;
        std::function<bool(const Action&)> preCondition;
        std::function<bool(const ActionResult&)> postCondition;
        std::string errorMessage;
    };
    
    void addPreValidationRule(const std::string& name, 
                             std::function<bool(const Action&)> rule,
                             const std::string& errorMessage);
    void addPostValidationRule(const std::string& name,
                              std::function<bool(const ActionResult&)> rule,
                              const std::string& errorMessage);
    
    std::vector<std::string> validatePre(const Action& action) const;
    std::vector<std::string> validatePost(const ActionResult& result) const;
    
    bool validateEnvironment() const;

private:
    std::vector<ValidationRule> preRules_;
    std::vector<ValidationRule> postRules_;
};

// Action failure recovery system
class ActionRecovery {
public:
    struct RecoveryStrategy {
        std::string name;
        float maxRetries;
        float retryDelay;
        std::function<bool(const ActionResult&)> shouldRetry;
        std::function<std::unique_ptr<Action>(const Action&)> getFallbackAction;
    };
    
    void addRecoveryStrategy(const RecoveryStrategy& strategy);
    std::vector<ActionResult> executeWithRecovery(std::unique_ptr<Action> action,
                                                  const ActionValidator& validator = ActionValidator());
    
    void setGlobalRetryPolicy(float maxRetries, float retryDelay);

private:
    std::vector<RecoveryStrategy> recoveryStrategies_;
    float globalMaxRetries_;
    float globalRetryDelay_;
};

// Main enhanced action system
class AdvancedActions {
public:
    AdvancedActions();
    ~AdvancedActions();
    
    // Action templates management
    void registerTemplate(const ActionTemplate& template_);
    std::vector<ActionTemplate> getTemplates() const;
    std::unique_ptr<Action> createFromTemplate(const std::string& templateName,
                                              const std::vector<float>& parameters);
    
    // Action sequences management
    void registerSequence(const ActionSequence& sequence);
    std::unique_ptr<ActionSequence> createSequence(const std::string& name);
    
    // Action execution with validation and recovery
    ActionResult executeAction(std::unique_ptr<Action> action,
                              const ActionValidator& validator = ActionValidator());
    ActionResult executeActionWithRecovery(std::unique_ptr<Action> action,
                                           const ActionRecovery& recovery = ActionRecovery());
    
    // Multi-action coordination
    std::vector<ActionResult> executeSequence(const std::string& sequenceName);
    
    // Callbacks
    void registerCallback(const std::string& id, std::unique_ptr<ActionCallback> callback);
    void unregisterCallback(const std::string& id);
    
    // Configuration
    void loadConfiguration(const Config& config);
    Config exportConfiguration() const;
    
    // Performance optimization
    void enableBatchProcessing(size_t batchSize);
    void disableBatchProcessing();
    bool isBatchProcessingEnabled() const;
    
    // Statistics
    struct Stats {
        size_t totalActionsExecuted = 0;
        size_t successfulActions = 0;
        size_t failedActions = 0;
        double averageExecutionTime = 0.0;
        std::unordered_map<std::string, size_t> actionsByType;
    };
    
    Stats getStats() const;
    void resetStats();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
