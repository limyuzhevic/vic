#pragma once

#include "Plugin.hpp"
#include "../memory/WorkingMemory.hpp"
#include <string>
#include <memory>
#include <unordered_map>

namespace nlm {

// Abstract base class for memory system plugins
class MemoryPlugin : public virtual IPlugin, public virtual IConfigurable, public virtual IState {
public:
    virtual ~MemoryPlugin() = default;
    
    // Plugin category identification
    const char* getCategory() const override { return "Memory"; }
    
    // Memory plugin specific operations
    virtual std::unique_ptr<NeuralWorkingMemory> createWorkingMemory(const std::string& memoryName = "Default") = 0;
    virtual bool configureMemory(NeuralWorkingMemory* memory, const std::unordered_map<std::string, std::string>& config) = 0;
    virtual void updateMemory(NeuralWorkingMemory* memory, TimestepDuration dt) = 0;
    
    // Memory-specific configuration parameters
    virtual std::vector<std::string> getMemoryConfigParameters() const {
        return {"capacity", "decayRate", "learningRate", "traceLength", "retrievalStrength"};
    }
    
    virtual std::string getMemoryParameterType(const std::string& paramName) const {
        if (paramName == "capacity") return "int";
        if (paramName == "decayRate") return "float";
        if (paramName == "learningRate") return "float";
        if (paramName == "traceLength") return "int";
        if (paramName == "retrievalStrength") return "float";
        return "string";
    }
    
    virtual std::string getMemoryParameterDescription(const std::string& paramName) const {
        if (paramName == "capacity") return "Maximum number of items the memory can hold";
        if (paramName == "decayRate") return "Natural decay rate of memory items";
        if (paramName == "learningRate") return "Learning rate for new item encoding";
        if (paramName == "traceLength") return "Duration of memory traces (in steps)";
        if (paramName == "retrievalStrength") return "Strength of retrieved memory items";
        return "";
    }
};

// Example memory plugin: WorkingMemoryPlugin
class WorkingMemoryPlugin : public MemoryPlugin {
public:
    WorkingMemoryPlugin() = default;
    ~WorkingMemoryPlugin() override = default;
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            "WorkingMemoryPlugin", "1.0.0", "Working memory implementation for active information",
            "NLM Team", {}, 70, "Memory", {
                {"capacity", "50"}, {"decayRate", "0.01"}, {"learningRate", "0.1"},
                {"traceLength", "100"}, {"retrievalStrength", "0.8"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        setState(PluginState::Initialized);
        return true;
    }
    
    void update(TimestepDuration dt) override {
        // Apply decay to all memory items
        if (workingMemory_) {
            applyMemoryDecay(workingMemory_.get(), dt);
        }
    }
    
    void shutdown() override {
        if (workingMemory_) {
            workingMemory_.reset();
        }
        setState(PluginState::Unloaded);
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "WorkingMemoryPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == PluginState::Initialized || state_ == PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        capacity_ = getIntFromConfig(config, "capacity", 50);
        decayRate_ = getFloatFromConfig(config, "decayRate", 0.01f);
        learningRate_ = getFloatFromConfig(config, "learningRate", 0.1f);
        traceLength_ = getIntFromConfig(config, "traceLength", 100);
        retrievalStrength_ = getFloatFromConfig(config, "retrievalStrength", 0.8f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return getMemoryConfigParameters();
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return getMemoryParameterType(paramName);
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        return getMemoryParameterDescription(paramName);
    }
    
    std::unique_ptr<NeuralWorkingMemory> createWorkingMemory(const std::string& memoryName = "Default") override {
        auto memory = std::make_unique<NeuralWorkingMemory>(capacity_);
        memory->setDecayRate(decayRate_);
        memory->setLearningRate(learningRate_);
        memory->setTraceLength(traceLength_);
        memory->setRetrievalStrength(retrievalStrength_);
        memory->setName(memoryName);
        return memory;
    }
    
    bool configureMemory(NeuralWorkingMemory* memory, const std::unordered_map<std::string, std::string>& config) override {
        if (!memory) return false;
        
        memory->setCapacity(capacity_);
        memory->setDecayRate(decayRate_);
        memory->setLearningRate(learningRate_);
        memory->setTraceLength(traceLength_);
        memory->setRetrievalStrength(retrievalStrength_);
        
        if (config.find("memoryName") != config.end()) {
            memory->setName(config.at("memoryName"));
        }
        
        return true;
    }
    
    void updateMemory(NeuralWorkingMemory* memory, TimestepDuration dt) override {
        // Update memory dynamics
        if (memory) {
            applyMemoryDecay(memory, dt);
            applyMemoryConsolidation(memory, dt);
        }
    }
    
    bool saveState(const std::string& filepath) const override {
        if (!workingMemory_) return false;
        // Serialize memory state to file
        // For now, return false as implementation would require file I/O
        return false;
    }
    
    bool loadState(const std::string& filepath) override {
        if (!workingMemory_) return false;
        // Deserialize memory state from file
        // For now, return false as implementation would require file I/O
        return false;
    }
    
    bool hasState() const override {
        return workingMemory_ != nullptr && workingMemory_->getItemCount() > 0;
    }
    
    void clearState() override {
        if (workingMemory_) {
            workingMemory_->clear();
        }
    }
    
    std::string getStateSignature() const override {
        return "WorkingMemoryPlugin_v1_" + std::to_string(workingMemory_ ? workingMemory_->getItemCount() : 0);
    }
    
    void setWorkingMemory(std::unique_ptr<NeuralWorkingMemory> memory) {
        workingMemory_ = std::move(memory);
    }
    
    NeuralWorkingMemory* getWorkingMemory() {
        return workingMemory_.get();
    }
    
    const NeuralWorkingMemory* getWorkingMemory() const {
        return workingMemory_.get();
    }
    
    float getCpuUsage() const override { return decayRate_ * 0.5f; }
    size_t getMemoryUsage() const override { return sizeof(WorkingMemoryPlugin) + (workingMemory_ ? workingMemory_->getMemoryUsage() : 0); }
    size_t getActiveObjects() const override { return workingMemory_ ? workingMemory_->getItemCount() : 0; }
    float getPerformanceScore() const override { return workingMemory_ ? 1.0f / (1.0f + workingMemory_->getAverageDecay()) : 1.0f; }
    void resetPerformanceMetrics() override {}
    
private:
    PluginState state_ = PluginState::Unloaded;
    int capacity_ = 50;
    float decayRate_ = 0.01f;
    float learningRate_ = 0.1f;
    int traceLength_ = 100;
    float retrievalStrength_ = 0.8f;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unordered_map<std::string, std::string> config_;
    
    void applyMemoryDecay(NeuralWorkingMemory* memory, TimestepDuration dt) {
        if (!memory) return;
        
        // Apply exponential decay to all items
        // This would be implemented based on the actual memory system
    }
    
    void applyMemoryConsolidation(NeuralWorkingMemory* memory, TimestepDuration dt) {
        if (!memory) return;
        
        // Apply consolidation dynamics
        // This would be implemented based on the actual memory system
    }
    
    int getIntFromConfig(const std::unordered_map<std::string, std::string>& config, 
                        const std::string& key, int defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stoi(it->second) : defaultValue;
    }
    
    float getFloatFromConfig(const std::unordered_map<std::string, std::string>& config,
                            const std::string& key, float defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stof(it->second) : defaultValue;
    }
    
    void setState(PluginState state) {
        state_ = state;
    }
};

// Note: NeuralWorkingMemory class would need to be included from memory module
// This is a placeholder for demonstration
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory(int capacity) : capacity_(capacity) {}
    
    void setDecayRate(float rate) { decayRate_ = rate; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setTraceLength(int length) { traceLength_ = length; }
    void setRetrievalStrength(float strength) { retrievalStrength_ = strength; }
    void setName(const std::string& name) { name_ = name; }
    
    int getCapacity() const { return capacity_; }
    float getDecayRate() const { return decayRate_; }
    float getLearningRate() const { return learningRate_; }
    int getTraceLength() const { return traceLength_; }
    float getRetrievalStrength() const { return retrievalStrength_; }
    const std::string& getName() const { return name_; }
    
    size_t getItemCount() const { return 0; }
    size_t getMemoryUsage() const { return sizeof(NeuralWorkingMemory); }
    float getAverageDecay() const { return decayRate_; }
    void clear() {}
    
private:
    int capacity_;
    float decayRate_;
    float learningRate_;
    int traceLength_;
    float retrievalStrength_;
    std::string name_;
};

} // namespace nlm