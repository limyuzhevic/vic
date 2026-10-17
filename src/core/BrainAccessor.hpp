#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

// Base interface for neural system access
class INeuralSystemAccessor {
public:
    virtual ~INeuralSystemAccessor() = default;
    
    // Template-based access to neural systems
    template<typename T>
    T* getNeuralSystem(const std::string& systemName);
    
    template<typename T>
    const T* getNeuralSystem(const std::string& systemName) const;
    
    // Check if system exists
    virtual bool hasSystem(const std::string& systemName) const = 0;
    
    // Get all system names
    virtual std::vector<std::string> getSystemNames() const = 0;
};

// Template-based neural system access implementation
class NeuralSystemAccessor : public INeuralSystemAccessor {
public:
    NeuralSystemAccessor() = default;
    ~NeuralSystemAccessor() override = default;
    
    // Register a neural system
    template<typename T>
    void registerNeuralSystem(const std::string& systemName,
                             std::shared_ptr<T> systemPtr);
    
    // Template-based access to neural systems
    template<typename T>
    T* getNeuralSystem(const std::string& systemName) {
        auto it = systems_.find(systemName);
        if (it != systems_.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }
    
    template<typename T>
    const T* getNeuralSystem(const std::string& systemName) const {
        auto it = systems_.find(systemName);
        if (it != systems_.end()) {
            return dynamic_cast<const T*>(it->second.get());
        }
        return nullptr;
    }
    
    // Check if system exists
    bool hasSystem(const std::string& systemName) const override {
        return systems_.find(systemName) != systems_.end();
    }
    
    // Get all system names
    std::vector<std::string> getSystemNames() const override {
        std::vector<std::string> names;
        names.reserve(systems_.size());
        for (const auto& pair : systems_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Disable copying
    NeuralSystemAccessor(const NeuralSystemAccessor&) = delete;
    NeuralSystemAccessor& operator=(const NeuralSystemAccessor&) = delete;

private:
    std::unordered_map<std::string, std::shared_ptr<void>> systems_;
};

// Enhanced Brain accessor for common operations
class BrainAccessor {
public:
    BrainAccessor() = default;
    ~BrainAccessor() = default;
    
    // Template-based property access
    template<typename T>
    static T getProperty(const std::string& key, const T& defaultValue);
    
    // Template-based neural system access (shortcut for common brain systems)
    static NeuralWorkingMemory* getWorkingMemory(const Brain* brain);
    static NeuralEpisodicMemory* getEpisodicMemory(const Brain* brain);
    static NeuralAssociativeMemory* getAssociativeMemory(const Brain* brain);
    
    static const NeuralWorkingMemory* getWorkingMemory(const Brain* brain);
    static const NeuralEpisodicMemory* getEpisodicMemory(const Brain* brain);
    static const NeuralAssociativeMemory* getAssociativeMemory(const Brain* brain);
    
    // Statistical access
    static float getEIEratio(const Brain* brain);
    static size_t getTotalSpikeCount(const Brain* brain);
    static size_t getTotalNeuronCount(const Brain* brain);
    static float getAverageFiringRate(const Brain* brain);
    
    // Memory system status
    static bool isWorkingMemoryEnabled(const Brain* brain);
    static bool isEpisodicMemoryEnabled(const Brain* brain);
    static bool isAssociativeMemoryEnabled(const Brain* brain);
    
    // Neuromodulation status
    static float getDopamineLevel(const Brain* brain);
    static float getAcetylcholineLevel(const Brain* brain);
    static float getNorepinephrineLevel(const Brain* brain);
    static float getSerotoninLevel(const Brain* brain);
    
    // Development status
    static int getDevelopmentalStage(const Brain* brain);
    
    // Disable copying
    BrainAccessor(const BrainAccessor&) = delete;
    BrainAccessor& operator=(const BrainAccessor&) = delete;
};

} // namespace nlm
