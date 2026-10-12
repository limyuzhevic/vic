// MemorySystemExperiment - Tests working, episodic, and associative memory
#pragma once

#include "IExperiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>

namespace nlm {

class MemorySystemExperiment : public IExperiment {
public:
    MemorySystemExperiment();
    MemorySystemExperiment(const ExperimentFactory::ExperimentConfig& config);
    ~MemorySystemExperiment() override = default;
    
    // IExperiment interface implementation
    const std::string& getName() const override { return name; }
    const std::string& getDescription() const override { return description; }
    void setName(const std::string& name) override { this->name = name; }
    void setDescription(const std::string& description) override { this->description = description; }
    
    void addParameter(const std::string& name, double value) override {
        parametersDouble[name] = value;
    }
    void addParameter(const std::string& name, const std::string& value) override {
        parametersString[name] = value;
    }
    void addParameter(const std::string& name, bool value) override {
        parametersBool[name] = value;
    }
    double getParameterDouble(const std::string& name) const override {
        auto it = parametersDouble.find(name);
        return it != parametersDouble.end() ? it->second : 0.0;
    }
    std::string getParameterString(const std::string& name) const override {
        auto it = parametersString.find(name);
        return it != parametersString.end() ? it->second : "";
    }
    bool getParameterBool(const std::string& name) const override {
        auto it = parametersBool.find(name);
        return it != parametersBool.end() ? it->second : false;
    }
    const std::map<std::string, double>& getParametersDouble() const override { return parametersDouble; }
    const std::map<std::string, std::string>& getParametersString() const override { return parametersString; }
    const std::map<std::string, bool>& getParametersBool() const override { return parametersBool; }
    
    void setProgress(float progress) override { currentProgress = progress; }
    float getProgress() const override { return currentProgress; }
    void setStatus(const std::string& status) override { currentStatus = status; }
    const std::string& getStatus() const override { return currentStatus; }
    
    std::chrono::system_clock::time_point getStartTime() const override { return startTime; }
    std::chrono::system_clock::time_point getEndTime() const override { return endTime; }
    double getDuration() const override { 
        return std::chrono::duration<double>(endTime - startTime).total_seconds(); 
    }
    
    bool validateParameters() const override {
        return !parametersDouble.empty() || !parametersString.empty() || !parametersBool.empty();
    }
    
    void preRun() override {
        startTime = std::chrono::system_clock::now();
        currentProgress = 0.0f;
        currentStatus = "Initializing...";
        recordMemoryBaseline();
    }
    
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) override;
    
    void postRun() override {
        endTime = std::chrono::system_clock::now();
        currentStatus = "Completed";
        computeFinalMetrics();
        setProgress(1.0f);
    }
    
    void checkpoint() override;
    void restore() override;
    
    bool supportsParallelExecution() const override { return false; }
    bool supportsCheckpointing() const override { return true; }
    bool supportsProgressTracking() const override { return true; }
    void enableFeature(const std::string& feature) override { enabledFeatures.insert(feature); }
    bool isFeatureEnabled(const std::string& feature) const override { 
        return enabledFeatures.find(feature) != enabledFeatures.end(); 
    }
    
    void addTag(const std::string& tag) override { tags.push_back(tag); }
    bool hasTag(const std::string& tag) const override { 
        return std::find(tags.begin(), tags.end(), tag) != tags.end(); 
    }
    const std::vector<std::string>& getTags() const override { return tags; }
    
    // Get results
    const ExperimentResult& getResult() const { return result; }
    float getWorkingMemoryRetention() const { return workingMemoryRetention; }
    float getEpisodicRecallAccuracy() const { return episodicRecallAccuracy; }
    float getAssociationStrength() const { return associationStrength; }
    size_t getTotalMemoriesStored() const { return totalMemoriesStored; }
    
private:
    void recordMemoryBaseline();
    void computeFinalMetrics();
    void testWorkingMemory(std::shared_ptr<Brain> brain, size_t testIndex);
    void testEpisodicMemory(std::shared_ptr<Brain> brain, size_t testIndex);
    void testAssociativeMemory(std::shared_ptr<Brain> brain, size_t testIndex);
    
    std::string name = "MemorySystemExperiment";
    std::string description = "Tests working, episodic, and associative memory systems";
    std::map<std::string, double> parametersDouble;
    std::map<std::string, std::string> parametersString;
    std::map<std::string, bool> parametersBool;
    std::vector<std::string> tags;
    std::set<std::string> enabledFeatures;
    
    ExperimentResult result;
    float currentProgress;
    std::string currentStatus;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    // Memory metrics
    float workingMemoryRetention;
    float workingMemoryCapacity;
    float episodicRecallAccuracy;
    size_t episodesStored;
    float episodicInfluence;
    float associationStrength;
    float associationTransfer;
    size_t totalMemoriesStored;
    
    // Test data
    std::vector<float> workingMemoryTestResults;
    std::vector<float> episodicMemoryTestResults;
    std::vector<float> associativeMemoryTestResults;
};
