#include "PluginSystem.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include <memory>

namespace nlm {
    class SimpleMemoryPlugin : public MemoryPlugin {
    private:
        std::unique_ptr<NeuralWorkingMemory> workingMemory_;
        std::shared_ptr<Brain> brain_;
        bool initialized_ = false;
        
    public:
        std::string getName() const override {
            return "SimpleMemoryPlugin";
        }
        
        std::string getVersion() const override {
            return "1.0.0";
        }
        
        PluginType getType() const override {
            return PluginType::MEMORY;
        }
        
        std::string getDescription() const override {
            return "Simple memory management plugin for working memory";
        }
        
        bool initialize(const std::unordered_map<std::string, std::string>& config) override {
            if (initialized_) return true;
            
            // Check for required configuration
            auto it = config.find("capacity");
            if (it == config.end()) {
                NLM_LOG_ERROR("MemoryPlugin requires 'capacity' configuration");
                return false;
            }
            
            int capacity = std::stoi(it->second);
            
            // Create working memory instance (this is a simplified example)
            // In a real implementation, you would integrate with the brain's memory system
            workingMemory_ = std::make_unique<NeuralWorkingMemory>();
            workingMemory_->setCapacity(capacity);
            
            initialized_ = true;
            NLM_LOG_INFO("SimpleMemoryPlugin initialized with capacity: " + std::to_string(capacity));
            return true;
        }
        
        void update(double timestep) override {
            if (!initialized_) return;
            
            // Update working memory state
            workingMemory_->update(timestep);
        }
        
        void shutdown() override {
            if (!initialized_) return;
            
            workingMemory_.reset();
            initialized_ = false;
            NLM_LOG_INFO("SimpleMemoryPlugin shut down");
        }
        
        bool isInitialized() const override {
            return initialized_;
        }
        
        // MemoryPlugin specific methods
        size_t getWorkingMemoryCapacity() const override {
            return workingMemory_ ? workingMemory_->getCapacity() : 0;
        }
        
        void setWorkingMemoryCapacity(size_t capacity) override {
            if (workingMemory_) {
                workingMemory_->setCapacity(capacity);
            }
        }
        
        size_t getWorkingMemoryUsage() const override {
            return workingMemory_ ? workingMemory_->getActiveTraces() : 0;
        }
        
        size_t getEpisodicMemoryCapacity() const override {
            // This would be implemented based on the specific memory system
            return 1000;
        }
        
        void setEpisodicMemoryCapacity(size_t capacity) override {
            // This would be implemented based on the specific memory system
        }
        
        size_t getEpisodicMemoryUsage() const override {
            // This would be implemented based on the specific memory system
            return 0;
        }
    };
} // namespace nlm

// Plugin factory function
extern "C" {
    NLM_API std::unique_ptr<nlm::Plugin> createPlugin() {
        return std::make_unique<nlm::SimpleMemoryPlugin>();
    }
    
    NLM_API const char* getPluginName() {
        return "SimpleMemoryPlugin";
    }
    
    NLM_API const char* getPluginVersion() {
        return "1.0.0";
    }
    
    NLM_API const char* getPluginDescription() {
        return "Simple memory management plugin for working memory";
    }
    
    NLM_API nlm::PluginType getPluginType() {
        return nlm::PluginType::MEMORY;
    }
}