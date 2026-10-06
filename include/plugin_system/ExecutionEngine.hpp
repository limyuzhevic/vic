#pragma once

#include "ExecutionEngine.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>

namespace nlm {

class ExecutionEngine {
public:
    ExecutionEngine();
    ~ExecutionEngine();
    
    bool execute(const ExecutionContext& context);
    bool executeAsync(const ExecutionContext& context);
    ExecutionStatus getExecutionStatus(const std::string& executionId) const;
    bool cancelExecution(const std::string& executionId);
    std::vector<ExecutionContext> getExecutionHistory(const std::string& pluginName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
