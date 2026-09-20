#pragma once

#include "IMemorySystem.hpp"
#include "BrainMemoryAdapter.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

// MemorySystemFactory - Creates and manages memory systems
class MemorySystemFactory : public IMemorySystemFactory {
public:
    MemorySystemFactory() : defaultAdapter_(std::make_shared<DefaultBrainMemoryAdapter>()) {}
    
    std::unique_ptr<IWorkingMemory> createWorkingMemory() override {
        auto memory = std::make_unique<WorkingMemoryAdapter>();
        memory->initialize(nullptr, defaultAdapter_);
        return memory;
    }
    
    std::unique_ptr<IEpisodicMemory> createEpisodicMemory() override {
        auto memory = std::make_unique<EpisodicMemoryAdapter>();
        memory->initialize(nullptr, defaultAdapter_);
        return memory;
    }
    
    std::unique_ptr<IAssociativeMemory> createAssociativeMemory() override {
        auto memory = std::make_unique<AssociativeMemoryAdapter>();
        memory->initialize(nullptr, defaultAdapter_);
        return memory;
    }
    
    const std::unordered_map<std::string, std::string>& getDefaultConfig() const override {
        // Return default configuration
        static std::unordered_map<std::string, std::string> defaultConfig;
        return defaultConfig;
    }
    
    // Set default adapter
    void setDefaultAdapter(std::shared_ptr<BrainMemoryAdapter> adapter) {
        defaultAdapter_ = adapter;
    }
    
    // Get default adapter
    std::shared_ptr<BrainMemoryAdapter> getDefaultAdapter() const {
        return defaultAdapter_;
    }

private:
    std::shared_ptr<BrainMemoryAdapter> defaultAdapter_;
};

} // namespace nlm
