// NLM Advanced Debugging and Profiling Tools - Breakpoint Debugger Implementation

#include "debug/BreakpointDebugger.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

BreakpointDebugger::BreakpointDebugger() : running_(false) {
    // Initialize with basic variables
    variables_["neuronCount"] = 0.0;
    variables_["synapseCount"] = 0.0;
    variables_["firingRate"] = 0.0;
    variables_["simulationStep"] = 0.0;
    variables_["cpuUsage"] = 0.0;
    variables_["memoryUsage"] = 0.0;
}

BreakpointDebugger::~BreakpointDebugger() {
    stop();
}

void BreakpointDebugger::addBreakpoint(const BreakpointCondition& condition) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (conditions_.find(condition.id) != conditions_.end()) {
        // Update existing condition
        conditions_[condition.id] = condition;
        return;
    }
    
    conditions_[condition.id] = condition;
    
    // Create breakpoint instance
    Breakpoint bp(condition);
    breakpoints_[condition.id] = bp;
    
    NLM_LOG_INFO("Breakpoint added: " + condition.id + " - " + condition.description);
}

void BreakpointDebugger::removeBreakpoint(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto condIt = conditions_.find(id);
    if (condIt != conditions_.end()) {
        conditions_.erase(condIt);
    }
    
    auto bpIt = breakpoints_.find(id);
    if (bpIt != breakpoints_.end()) {
        breakpoints_.erase(bpIt);
    }
    
    hitCounts_.erase(id);
    
    NLM_LOG_INFO("Breakpoint removed: " + id);
}

void BreakpointDebugger::enableBreakpoint(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = conditions_.find(id);
    if (it != conditions_.end()) {
        it->second.enabled = true;
    }
}

void BreakpointDebugger::disableBreakpoint(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = conditions_.find(id);
    if (it != conditions_.end()) {
        it->second.enabled = false;
    }
}

bool BreakpointDebugger::hasBreakpoint(const std::string& id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return conditions_.find(id) != conditions_.end();
}

bool BreakpointDebugger::shouldBreak(const std::map<std::string, double>& variables) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : conditions_) {
        const auto& condition = pair.second;
        if (!condition.enabled) continue;
        
        if (evaluateCondition(condition, variables)) {
            return true;
        }
    }
    
    return false;
}

void BreakpointDebugger::handleBreakpoint(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto bpIt = breakpoints_.find(id);
    if (bpIt == breakpoints_.end()) return;
    
    Breakpoint& breakpoint = bpIt->second;
    breakpoint.triggerCount++;
    hitCounts_[id]++;
    breakpoint.triggeredTime = std::chrono::system_clock::now();
    breakpoint.isActive = breakpoint.condition.persistent || !breakpoint.condition.persistent;
    
    // Log breakpoint event
    logBreakpoint(breakpoint);
    
    // Call callback if registered
    if (breakpointTriggeredCallback_) {
        breakpointTriggeredCallback_(breakpoint);
    }
}

std::vector<Breakpoint> BreakpointDebugger::getActiveBreakpoints() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Breakpoint> activeBreakpoints;
    
    for (const auto& pair : breakpoints_) {
        if (pair.second.isActive && pair.second.condition.enabled) {
            activeBreakpoints.push_back(pair.second);
        }
    }
    
    return activeBreakpoints;
}

bool BreakpointDebugger::evaluateCondition(const BreakpointCondition& condition, 
                                          const std::map<std::string, double>& variables) const {
    if (!condition.conditionEvaluator) {
        // Simple expression evaluation for basic conditions
        return parseExpression(condition.condition, variables);
    }
    
    return condition.conditionEvaluator(variables);
}

void BreakpointDebugger::start() {
    if (!running_) {
        running_ = true;
        NLM_LOG_INFO("BreakpointDebugger started");
    }
}

void BreakpointDebugger::stop() {
    if (running_) {
        running_ = false;
        NLM_LOG_INFO("BreakpointDebugger stopped");
    }
}

void BreakpointDebugger::pause() {
    // Implementation for pausing breakpoint checking
    NLM_LOG_INFO("BreakpointDebugger paused");
}

void BreakpointDebugger::resume() {
    // Implementation for resuming breakpoint checking
    NLM_LOG_INFO("BreakpointDebugger resumed");
}

size_t BreakpointDebugger::getBreakpointHits(const std::string& id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = hitCounts_.find(id);
    return it != hitCounts_.end() ? it->second : 0;
}

size_t BreakpointDebugger::getTotalBreakpointHits() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t total = 0;
    for (const auto& pair : hitCounts_) {
        total += pair.second;
    }
    return total;
}

std::string BreakpointDebugger::getBreakpointStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "Breakpoint Statistics:\n";
    oss << "  Total breakpoints: " << conditions_.size() << "\n";
    oss << "  Total hits: " << getTotalBreakpointHits() << "\n";
    
    for (const auto& pair : conditions_) {
        oss << "  " << pair.first << ": " << pair.second.description << " (enabled: " 
            << (pair.second.enabled ? "yes" : "no") << ")\n";
    }
    
    return oss.str();
}

void BreakpointDebugger::setBreakpointTriggeredCallback(std::function<void(const Breakpoint&)> callback) {
    breakpointTriggeredCallback_ = callback;
}

void BreakpointDebugger::attachToSimulation(const std::string& simulationId) {
    std::lock_guard<std::mutex> lock(mutex_);
    attachedComponents_.push_back(simulationId);
    NLM_LOG_INFO("BreakpointDebugger attached to simulation: " + simulationId);
}

void BreakpointDebugger::attachToComponent(const std::string& componentId) {
    std::lock_guard<std::mutex> lock(mutex_);
    attachedComponents_.push_back(componentId);
    NLM_LOG_INFO("BreakpointDebugger attached to component: " + componentId);
}

void BreakpointDebugger::setVariable(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(mutex_);
    variables_[name] = value;
}

double BreakpointDebugger::getVariable(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = variables_.find(name);
    return it != variables_.end() ? it->second : 0.0;
}

std::map<std::string, double> BreakpointDebugger::getAllVariables() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return variables_;
}

BreakpointCondition BreakpointDebugger::createConditionFromString(const std::string& condition) {
    BreakpointCondition cond("default", "Condition from string", condition);
    
    // Parse simple conditions
    if (condition.find(">") != std::string::npos) {
        // Create evaluator for simple comparison
        // This is a simplified version - real implementation would be more sophisticated
    }
    
    return cond;
}

bool BreakpointDebugger::parseExpression(const std::string& expression, 
                                         const std::map<std::string, double>& variables) const {
    // Simplified expression parser - in practice, use a proper expression evaluator
    if (expression == "neuronCount > 1000") {
        auto it = variables.find("neuronCount");
        return it != variables.end() && it->second > 1000.0;
    } else if (expression == "firingRate < 0.01") {
        auto it = variables.find("firingRate");
        return it != variables.end() && it->second < 0.01;
    } else if (expression == "simulationStep % 100 == 0") {
        auto it = variables.find("simulationStep");
        return it != variables.end() && std::fmod(it->second, 100.0) < 0.001;
    } else if (expression == "cpuUsage > 80") {
        auto it = variables.find("cpuUsage");
        return it != variables.end() && it->second > 80.0;
    }
    
    return false;
}

void BreakpointDebugger::triggerBreakpoint(const BreakpointCondition& condition) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find or create breakpoint
    auto bpIt = breakpoints_.find(condition.id);
    if (bpIt == breakpoints_.end()) {
        Breakpoint bp(condition);
        bpIt = breakpoints_.insert(std::make_pair(condition.id, bp)).first;
    }
    
    Breakpoint& breakpoint = bpIt->second;
    breakpoint.triggerCount++;
    hitCounts_[condition.id]++;
    breakpoint.isActive = condition.persistent;
    breakpoint.triggeredTime = std::chrono::system_clock::now();
}

void BreakpointDebugger::logBreakpoint(const Breakpoint& breakpoint) {
    std::ostringstream oss;
    oss << "[DEBUG] Breakpoint triggered: " << breakpoint.condition.id << " - " << breakpoint.condition.description;
    oss << " (hit count: " << breakpoint.triggerCount << ")";
    
    NLM_LOG_INFO(oss.str());
}

} // namespace nlm
