// Development monitoring tool
//
// This file provides the implementation of development monitoring CLI tool

#pragma once

#include "tools/monitor/MonitoringTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class MonitorDevelopmentTool : public MonitoringTool {
public:
    MonitorDevelopmentTool() = default;
    ~MonitorDevelopmentTool() = default;
    
    std::string getName() const override {
        return "monitor-development";
    }
    
    std::string getDescription() const override {
        return "Monitor development processes";
    }
    
    std::string getUsage() const override {
        return "monitor-development --brain <file> --output <file> [--interval <float>] [--dev-type maturation|pruning|synaptogenesis] [--duration <float>]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool monitorDevelopmentImpl(const std::string& brainFile, const std::string& outputFile,
                                double interval, const std::string& devType, double duration) const;
};

} // namespace nlm
