// Development Tool Implementation
//
// This file provides the base class and common functionality for development CLI tools.

#pragma once

#include "tools/CLITool.hpp"
#include <sstream>

namespace nlm {

class DevelopmentTool : public CLITool {
public:
    DevelopmentTool() = default;
    ~DevelopmentTool() = default;
    
    // Tool information
    virtual std::string getName() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getUsage() const override = 0;
    virtual CommandType getType() const override {
        return CommandType::DEVELOPMENT;
    }
    virtual std::string getCategory() const override {
        return "Development";
    }
    
    // Development-specific helper methods
    bool debugSession(const std::string& options) const;
    bool traceSystem(const std::string& mode) const;
    bool profileSystem(const std::string& mode) const;
    bool monitorSystem(const std::string& mode) const;
    bool createSnapshot(const std::string& outputFile) const;
    bool restoreSnapshot(const std::string& inputFile) const;
    
protected:
    // Development-specific implementations
    virtual bool debugSessionImpl(const std::string& options) const;
    virtual bool traceSystemImpl(const std::string& mode) const;
    virtual bool profileSystemImpl(const std::string& mode) const;
    virtual bool monitorSystemImpl(const std::string& mode) const;
    virtual bool createSnapshotImpl(const std::string& outputFile) const;
    virtual bool restoreSnapshotImpl(const std::string& inputFile) const;
};

} // namespace nlm