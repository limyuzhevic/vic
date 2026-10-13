#include "MemoryPoolManager.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <memory>

namespace nlm {

// Global memory pool manager instance
namespace {
    class MemoryPoolManagerImpl {
    public:
        MemoryPoolManagerImpl() {
            // Initialize with default pools
            neuronPool = std::make_unique<NeuronMemoryPool<Neuron>>("default_neuron_pool");
            synapsePool = std::make_unique<SynapseMemoryPool<Synapse>>("default_synapse_pool");
            
            // Pre-allocate some memory for initial use
            neuronPool->resize(1024);
            synapsePool->resize(2048);
        }
        
        ~MemoryPoolManagerImpl() = default;
        
        // Global default pools
        std::unique_ptr<NeuronMemoryPool<Neuron>> neuronPool;
        std::unique_ptr<SynapseMemoryPool<Synapse>> synapsePool;
    };
    
    std::unique_ptr<MemoryPoolManagerImpl> gMemoryPoolManager;
    
    // Thread-safe accessor for the global instance
    MemoryPoolManagerImpl& getMemoryPoolManagerImpl() {
        static MemoryPoolManagerImpl instance;
        return instance;
    }
}

MemoryPoolManager& getMemoryPoolManager() {
    static MemoryPoolManager manager;
    return manager;
}

} // namespace nlm
