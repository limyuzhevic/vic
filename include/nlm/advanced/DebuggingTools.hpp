#pragma once

#include <string>
#include <vector>
#include "../core/Config/Config.hpp"

namespace nlm {

class AdvancedDebuggingSystem {
public:
    AdvancedDebuggingSystem(const std::string& id, const Config& config);
    ~AdvancedDebuggingSystem();
    
    void startProfiling(const std::string& functionName);
    
    void endProfiling(const std::string& functionName);
    
    void trackAllocation(size_t bytes);
    
    void trackDeallocation(size_t bytes);
    
    void recordMemoryUsage();
    
    bool validate(const Config& config);
    
    void log(const std::string& level, const std::string& module,
             const std::string& message, const std::string& function = "",
             int line = 0);
    
    void takeSystemSnapshot();
    
    std::string getSystemStatus() const;
    
    void saveDebugState(const Config& config) const;
    
    void loadDebugState(const Config& config);
    
    void stop();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
