// Utility Tool Implementation
//
// This file provides the base class and common functionality for utility CLI tools.

#pragma once

#include "tools/CLITool.hpp"
#include <sstream>

namespace nlm {

class UtilityTool : public CLITool {
public:
    UtilityTool() = default;
    ~UtilityTool() = default;
    
    // Tool information
    virtual std::string getName() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getUsage() const override = 0;
    virtual CommandType getType() const override {
        return CommandType::UTILITY;
    }
    virtual std::string getCategory() const override {
        return "Utility";
    }
    
    // Utility-specific helper methods
    std::string getVersionInfo() const;
    bool performHealthCheck() const;
    std::string getSystemStats() const;
    std::string getLogs(const std::string& options) const;
    bool runBenchmark(const std::string& config) const;
    bool configureBenchmark(const std::string& preset) const;
    
protected:
    // Utility-specific implementations
    virtual std::string getVersionInfoImpl() const;
    virtual bool performHealthCheckImpl() const;
    virtual std::string getSystemStatsImpl() const;
    virtual std::string getLogsImpl(const std::string& options) const;
    virtual bool runBenchmarkImpl(const std::string& config) const;
    virtual bool configureBenchmarkImpl(const std::string& preset) const;
};

} // namespace nlm