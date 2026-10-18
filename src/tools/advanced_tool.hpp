// Advanced Tool Implementation
//
// This file provides the base class and common functionality for advanced CLI tools.

#pragma once

#include "tools/CLITool.hpp"
#include <sstream>

namespace nlm {

class AdvancedTool : public CLITool {
public:
    AdvancedTool() = default;
    ~AdvancedTool() = default;
    
    // Tool information
    virtual std::string getName() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getUsage() const override = 0;
    virtual CommandType getType() const override {
        return CommandType::ADVANCED;
    }
    virtual std::string getCategory() const override {
        return "Advanced";
    }
    
    // Advanced-specific helper methods
    bool executeBatch(const std::string& commandFile) const;
    bool executeParallel(const std::string& command) const;
    bool executeDistributed(const std::string& host) const;
    bool scaleOut(const std::string& nodeCount) const;
    bool loadBalance(const std::string& algorithm) const;
    
protected:
    // Advanced-specific implementations
    virtual bool executeBatchImpl(const std::string& commandFile) const;
    virtual bool executeParallelImpl(const std::string& command) const;
    virtual bool executeDistributedImpl(const std::string& host) const;
    virtual bool scaleOutImpl(const std::string& nodeCount) const;
    virtual bool loadBalanceImpl(const std::string& algorithm) const;
};

} // namespace nlm