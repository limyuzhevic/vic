// Brain Tool Implementation
//
// This file provides the base class and common functionality for brain management CLI tools.

#pragma once

#include "tools/CLITool.hpp"
#include <sstream>

namespace nlm {

class BrainTool : public CLITool {
public:
    BrainTool() = default;
    ~BrainTool() = default;
    
    // Tool information
    virtual std::string getName() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getUsage() const override = 0;
    virtual CommandType getType() const override {
        return CommandType::BRAIN;
    }
    virtual std::string getCategory() const override {
        return "Brain Management";
    }
    
    // Brain-specific helper methods
    bool saveBrainState(const std::string& outputFile, const Config& config) const;
    bool loadBrainState(const std::string& inputFile, Config& config) const;
    bool compareBrainStates(const std::string& file1, const std::string& file2) const;
    bool exportBrainState(const std::string& inputFile, const std::string& outputFile,
                         const std::string& format) const;
    bool importBrainState(const std::string& inputFile, Config& config) const;
    
    // Brain state analysis helpers
    std::string analyzeBrainStructure(const std::string& brainFile) const;
    double calculateBrainComplexity(const std::string& brainFile) const;
    std::vector<std::string> analyzeBrainConnectivity(const std::string& brainFile) const;
    
protected:
    // Brain-specific implementations
    virtual bool saveBrainStateImpl(const std::string& outputFile, const Config& config) const;
    virtual bool loadBrainStateImpl(const std::string& inputFile, Config& config) const;
    virtual bool compareBrainStatesImpl(const std::string& file1, const std::string& file2) const;
    virtual bool exportBrainStateImpl(const std::string& inputFile, const std::string& outputFile,
                                     const std::string& format) const;
    virtual bool importBrainStateImpl(const std::string& inputFile, Config& config) const;
    virtual std::string analyzeBrainStructureImpl(const std::string& brainFile) const;
    virtual double calculateBrainComplexityImpl(const std::string& brainFile) const;
    virtual std::vector<std::string> analyzeBrainConnectivityImpl(const std::string& brainFile) const;
};

} // namespace nlm