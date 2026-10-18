// World Tool Implementation
//
// This file provides the base class and common functionality for world management CLI tools.

#pragma once

#include "tools/CLITool.hpp"
#include <sstream>

namespace nlm {

class WorldTool : public CLITool {
public:
    WorldTool() = default;
    ~WorldTool() = default;
    
    // Tool information
    virtual std::string getName() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getUsage() const override = 0;
    virtual CommandType getType() const override {
        return CommandType::WORLD;
    }
    virtual std::string getCategory() const override {
        return "World/Environment";
    }
    
    // World-specific helper methods
    bool saveWorldState(const std::string& outputFile, const Config& config) const;
    bool loadWorldState(const std::string& inputFile, Config& config) const;
    bool testWorldConfiguration(const std::string& configFile) const;
    bool exploreWorldState(const std::string& worldFile) const;
    
protected:
    // World-specific implementations
    virtual bool saveWorldStateImpl(const std::string& outputFile, const Config& config) const;
    virtual bool loadWorldStateImpl(const std::string& inputFile, Config& config) const;
    virtual bool testWorldConfigurationImpl(const std::string& configFile) const;
    virtual bool exploreWorldStateImpl(const std::string& worldFile) const;
};

} // namespace nlm