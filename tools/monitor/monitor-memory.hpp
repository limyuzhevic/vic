// Memory monitoring tool
//
// This file provides the implementation of memory monitoring CLI tool

#pragma once

#include "tools/monitor/MonitoringTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class MonitorMemoryTool : public MonitoringTool {
public:
    MonitorMemoryTool() = default;
    ~MonitorMemoryTool() = default;
    
    std::string getName() const override {
        return "monitor-memory";
    }
    
    std::string getDescription() const override {
        return "Monitor memory system performance";
    }
    
    std::string getUsage() const override {
        return "monitor-memory --brain <file> --output <file> [--interval <float>] [--memory-type working|episodic|semantic] [--duration <float>]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool monitorMemoryImpl(const std::string& brainFile, const std::string& outputFile,
                          double interval, const std::string& memoryType, double duration) const;
};

} // namespace nlm
