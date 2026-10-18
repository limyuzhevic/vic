// ProfileTool.hpp - Base profiling tool
#pragma once

#include "ConfigTool.hpp"
#include <memory>
#include <string>

namespace nlm {

class ProfilingFramework;

class ProfileTool : public ConfigTool {
public:
    ProfileTool() = default;
    virtual ~ProfileTool() = default;
    
    CommandType getType() const override {
        return CommandType::PROFILING;
    }
    
    std::string getCategory() const override {
        return "Profiling Tools";
    }
    
    virtual std::string getProfileType() const = 0;
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override {
        // Parse common profiling arguments
        std::string program = findArg(args, {"--program", "-p"});
        std::string output = findArg(args, {"--output", "-o"});
        bool detectLeaks = hasArg(args, {"--detect-leaks"});
        int timeout = getIntArg(args, {"--timeout"}, 0);
        
        // Initialize profiling framework
        auto& framework = ProfilingFramework::getInstance();
        if (!framework.isProfiling("main_session")) {
            framework.startProfiling("main_session");
        }
        
        // Execute profiling
        bool success = performProfiling(program, output, detectLeaks, timeout);
        
        if (success && framework.isProfiling("main_session")) {
            framework.stopProfiling("main_session");
        }
        
        return success ? ExecutionResult::SUCCESS : ExecutionResult::FAILURE;
    }
    
protected:
    virtual bool performProfiling(const std::string& program, 
                                 const std::string& output,
                                 bool detectLeaks,
                                 int timeout) = 0;
};

} // namespace nlm
