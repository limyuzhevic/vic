#include "AdvancedActions.hpp"
#include <iostream>
#include <thread>
#include <algorithm>
#include <chrono>

namespace nlm {

// Simple callback implementation for demonstration
class LoggingCallback : public ActionCallback {
public:
    LoggingCallback(const std::string& name) : name_(name) {}
    
    void onActionStart(const ActionResult& result) override {
        std::cout << "[" << name_ << "] Action started: " 
                  << static_cast<int>(result.getAction()->getType()) << std::endl;
    }
    
    void onActionComplete(const ActionResult& result) override {
        std::cout << "[" << name_ << "] Action completed successfully" << std::endl;
    }
    
    void onActionFailed(const ActionResult& result) override {
        std::cout << "[" << name_ << "] Action failed: " << result.getMessage() << std::endl;
    }
    
    void onActionProgress(const ActionResult& result, float progress) override {
        if (progress > 0.0f && progress < 1.0f) {
            std::cout << "[" << name_ << "] Action progress: " << (progress * 100) << "%" << std::endl;
        }
    }
    
    std::string getId() const override {
        return name_;
    }
    
private:
    std::string name_;
};

// Additional callback example
class StatsCallback : public ActionCallback {
public:
    struct Stats {
        size_t totalActions = 0;
        size_t successfulActions = 0;
        size_t failedActions = 0;
        double totalDuration = 0.0;
    };
    
    StatsCallback() : stats_() {}
    
    void onActionStart(const ActionResult& result) override {
        stats_.totalActions++;
    }
    
    void onActionComplete(const ActionResult& result) override {
        stats_.successfulActions++;
        stats_.totalDuration += result.getDuration();
    }
    
    void onActionFailed(const ActionResult& result) override {
        stats_.failedActions++;
        stats_.totalDuration += result.getDuration();
    }
    
    void onActionProgress(const ActionResult& result, float progress) override {
        // Not used in this implementation
    }
    
    std::string getId() const override {
        return "stats_callback";
    }
    
    Stats getStats() const {
        return stats_;
    }
    
private:
    Stats stats_;
};

// ActionSequence implementation details
void ActionSequence::notifyCallbacks(const ActionResult& result, const std::string& event) {
    for (auto& callback : callbacks_) {
        if (event == "start") {
            callback->onActionStart(result);
        } else if (event == "complete") {
            callback->onActionComplete(result);
        } else if (event == "failed") {
            callback->onActionFailed(result);
        }
    }
}

// Main AdvancedActions implementation details
void AdvancedActions::Impl::addDefaultTemplate() {
    ActionTemplate template_("MoveWithParams", "Movement action with parameters");
    template_.addParameter(ActionParameter("Type", ActionParameter::Type::Int, 0));
    template_.addParameter(ActionParameter("Distance", ActionParameter::Type::Float, 1.0f));
    template_.addParameter(ActionParameter("Speed", ActionParameter::Type::Float, 1.0f));
    templates["MoveWithParams"] = std::move(template_);
}

void AdvancedActions::Impl::addDefaultRecoveryStrategy() {
    ActionRecovery::RecoveryStrategy strategy;
    strategy.name = "DefaultRetry";
    strategy.maxRetries = 3;
    strategy.retryDelay = 0.5f;
    strategy.shouldRetry = [](const ActionResult& result) {
        return result.isFailed() && result.getStatus() != ActionResult::Status::TimedOut;
    };
    strategy.getFallbackAction = [](const Action& action) {
        return std::make_unique<Action>(ActionType::Wait);
    };
    recovery.addRecoveryStrategy(strategy);
}

std::unique_ptr<Action> AdvancedActions::Impl::createDefaultAction() {
    std::uniform_int_distribution<int> dist(0, 8);
    ActionType type = static_cast<ActionType>(dist(rng));
    return std::make_unique<Action>(type);
}

} // namespace nlm
