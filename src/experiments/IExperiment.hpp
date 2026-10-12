// IExperiment - Abstract base class for all experiments
#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>

namespace nlm {

class IExperiment {
public:
    virtual ~IExperiment() = default;
    
    // Experiment metadata
    virtual const std::string& getName() const = 0;
    virtual const std::string& getDescription() const = 0;
    virtual void setName(const std::string& name) = 0;
    virtual void setDescription(const std::string& description) = 0;
    
    // Configuration
    virtual void addParameter(const std::string& name, double value) = 0;
    virtual void addParameter(const std::string& name, const std::string& value) = 0;
    virtual void addParameter(const std::string& name, bool value) = 0;
    virtual double getParameterDouble(const std::string& name) const = 0;
    virtual std::string getParameterString(const std::string& name) const = 0;
    virtual bool getParameterBool(const std::string& name) const = 0;
    virtual const std::map<std::string, double>& getParametersDouble() const = 0;
    virtual const std::map<std::string, std::string>& getParametersString() const = 0;
    virtual const std::map<std::string, bool>& getParametersBool() const = 0;
    
    // Progress tracking
    virtual void setProgress(float progress) = 0;
    virtual float getProgress() const = 0;
    virtual void setStatus(const std::string& status) = 0;
    virtual const std::string& getStatus() const = 0;
    
    // Timing
    virtual std::chrono::system_clock::time_point getStartTime() const = 0;
    virtual std::chrono::system_clock::time_point getEndTime() const = 0;
    virtual double getDuration() const = 0;
    
    // Execution
    virtual bool validateParameters() const = 0;
    virtual void preRun() = 0;
    virtual bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) = 0;
    virtual void postRun() = 0;
    virtual void checkpoint() = 0;
    virtual void restore() = 0;
    
    // Features
    virtual bool supportsParallelExecution() const = 0;
    virtual bool supportsCheckpointing() const = 0;
    virtual bool supportsProgressTracking() const = 0;
    virtual void enableFeature(const std::string& feature) = 0;
    virtual bool isFeatureEnabled(const std::string& feature) const = 0;
    
    // Tags for categorization
    virtual void addTag(const std::string& tag) = 0;
    virtual bool hasTag(const std::string& tag) const = 0;
    virtual const std::vector<std::string>& getTags() const = 0;
};
