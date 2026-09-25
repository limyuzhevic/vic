#pragma once

#include "abstraction/AbstractComponents.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

class MemorySystemFactory {
public:
    // Factory type enum for configuration
    enum class MemorySystemType {
        WorkingMemory,
        EpisodicMemory,
        AssociativeMemory,
        NeuralWorkingMemory,
        NeuralEpisodicMemory,
        Custom
    };
    
    MemorySystemFactory();
    ~MemorySystemFactory();
    
    // Create memory system based on type
    std::unique_ptr<AbstractMemorySystem> createMemorySystem(
        MemorySystemType type,
        std::shared_ptr<Config> config = nullptr
    );
    
    std::unique_ptr<AbstractMemorySystem> createMemorySystem(
        const std::string& memorySystemName,
        std::shared_ptr<Config> config = nullptr
    );
    
    // Create specific memory system types
    std::unique_ptr<WorkingMemory> createWorkingMemory(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto workingMemory = std::make_unique<WorkingMemory>();
        if (config) {
            configureFromConfig(workingMemory.get(), config);
        }
        return workingMemory;
    }
    
    std::unique_ptr<EpisodicMemory> createEpisodicMemory(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto episodicMemory = std::make_unique<EpisodicMemory>();
        if (config) {
            configureFromConfig(episodicMemory.get(), config);
        }
        return episodicMemory;
    }
    
    std::unique_ptr<AssociativeMemory> createAssociativeMemory(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto associativeMemory = std::make_unique<AssociativeMemory>();
        if (config) {
            configureFromConfig(associativeMemory.get(), config);
        }
        return associativeMemory;
    }
    
    // Configuration-based factory creation
    std::unique_ptr<AbstractMemorySystem> createFromConfig(
        const Config& config
    );
    
    // Plugin-based factory creation
    std::unique_ptr<AbstractMemorySystem> createFromPlugin(
        const std::string& pluginName,
        std::shared_ptr<Config> config = nullptr
    );
    
    // List available memory system types
    static std::vector<MemorySystemType> getAvailableTypes() {
        return {
            MemorySystemType::WorkingMemory,
            MemorySystemType::EpisodicMemory,
            MemorySystemType::AssociativeMemory,
            MemorySystemType::NeuralWorkingMemory,
            MemorySystemType::NeuralEpisodicMemory
        };
    }
    
    // Get type name
    static const char* getTypeName(MemorySystemType type) {
        switch (type) {
            case MemorySystemType::WorkingMemory: return "WorkingMemory";
            case MemorySystemType::EpisodicMemory: return "EpisodicMemory";
            case MemorySystemType::AssociativeMemory: return "AssociativeMemory";
            case MemorySystemType::NeuralWorkingMemory: return "NeuralWorkingMemory";
            case MemorySystemType::NeuralEpisodicMemory: return "NeuralEpisodicMemory";
            default: return "Unknown";
        }
    }
    
    // Configuration helpers
    void configureFromConfig(AbstractMemorySystem* memorySystem, const Config& config);
    void configureFromConfig(WorkingMemory* workingMemory, const Config& config);
    void configureFromConfig(EpisodicMemory* episodicMemory, const Config& config);
    void configureFromConfig(AssociativeMemory* associativeMemory, const Config& config);
    
    // Plugin system
    bool registerPlugin(
        const std::string& name,
        std::function<std::unique_ptr<AbstractMemorySystem>(std::shared_ptr<Config>)> factory
    );
    std::unique_ptr<AbstractMemorySystem> createFromRegisteredPlugin(
        const std::string& name,
        std::shared_ptr<Config> config = nullptr
    );
    
    // Get registered plugin names
    std::vector<std::string> getRegisteredPlugins() const;
    
    // Statistics and monitoring
    size_t getCreationCount() const { return creationCount_; }
    size_t getPluginCount() const { return plugins_.size(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    mutable size_t creationCount_;
    std::unordered_map<std::string, std::function<std::unique_ptr<AbstractMemorySystem>(std::shared_ptr<Config>)>> plugins_;
};

} // namespace nlm
