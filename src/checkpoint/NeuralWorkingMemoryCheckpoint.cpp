// NeuralWorkingMemory save/load implementation

#include "NeuralWorkingMemoryCheckpoint.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include <fstream>
#include <algorithm>
#include <stdexcept>

namespace nlm {

namespace {

// NeuralWorkingMemorySaverLoader implementation
class NeuralWorkingMemorySaverLoaderImpl : public NeuralWorkingMemorySaverLoader {
public:
    bool save(const NeuralWorkingMemoryCheckpoint& checkpoint) const override {
        try {
            // Create a simple file-based save
            std::ofstream file("working_memory_checkpoint.tmp", std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Write magic number for validation
            const uint64_t magic = 0x4E574D43524B4D00ULL; // "NWMCKM"
            file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
            
            // Write checkpoint version
            const uint32_t version = 1;
            file.write(reinterpret_cast<const char*>(&version), sizeof(version));
            
            // Write checkpoint data (simplified serialization)
            size_t memoryNeuronsSize = checkpoint.memoryNeurons.size();
            file.write(reinterpret_cast<const char*>(&memoryNeuronsSize), sizeof(memoryNeuronsSize));
            
            if (memoryNeuronsSize > 0) {
                file.write(reinterpret_cast<const char*>(checkpoint.memoryNeurons.data()),
                         memoryNeuronsSize * sizeof(NeuronId));
            }
            
            // Write activations
            size_t activationsSize = checkpoint.memoryActivations.size();
            file.write(reinterpret_cast<const char*>(&activationsSize), sizeof(activationsSize));
            if (activationsSize > 0) {
                file.write(reinterpret_cast<const char*>(checkpoint.memoryActivations.data()),
                         activationsSize * sizeof(float));
            }
            
            // Write timestamps
            size_t timestampsSize = checkpoint.memoryTimestamps.size();
            file.write(reinterpret_cast<const char*>(&timestampsSize), sizeof(timestampsSize));
            if (timestampsSize > 0) {
                file.write(reinterpret_cast<const char*>(checkpoint.memoryTimestamps.data()),
                         timestampsSize * sizeof(SimulationStep));
            }
            
            // Write active traces
            size_t activeTracesSize = checkpoint.activeTraces.size();
            file.write(reinterpret_cast<const char*>(&activeTracesSize), sizeof(activeTracesSize));
            if (activeTracesSize > 0) {
                file.write(reinterpret_cast<const char*>(checkpoint.activeTraces.data()),
                         activeTracesSize * sizeof(size_t));
            }
            
            // Write parameters
            file.write(reinterpret_cast<const char*>(&checkpoint.capacity), sizeof(checkpoint.capacity));
            file.write(reinterpret_cast<const char*>(&checkpoint.decayRate), sizeof(checkpoint.decayRate));
            file.write(reinterpret_cast<const char*>(&checkpoint.neuromodulationFactor), sizeof(checkpoint.neuromodulationFactor));
            
            // Write recurrent connections
            size_t connectionsSize = checkpoint.recurrentConnections.size();
            file.write(reinterpret_cast<const char*>(&connectionsSize), sizeof(connectionsSize));
            if (connectionsSize > 0) {
                for (const auto& conn : checkpoint.recurrentConnections) {
                    file.write(reinterpret_cast<const char*>(&conn.first), sizeof(NeuronId));
                    file.write(reinterpret_cast<const char*>(&conn.second), sizeof(NeuronId));
                }
            }
            
            // Write winners
            size_t winnersSize = checkpoint.winners.size();
            file.write(reinterpret_cast<const char*>(&winnersSize), sizeof(winnersSize));
            if (winnersSize > 0) {
                file.write(reinterpret_cast<const char*>(checkpoint.winners.data()),
                         winnersSize * sizeof(NeuronId));
            }
            
            file.close();
            
            // Atomic save: move temp to final file
            std::rename("working_memory_checkpoint.tmp", "working_memory_checkpoint.bin");
            
            return true;
        } catch (const std::exception& e) {
            // Clean up temp file if it exists
            std::remove("working_memory_checkpoint.tmp");
            return false;
        }
    }
    
    bool load(NeuralWorkingMemoryCheckpoint& checkpoint) override {
        try {
            std::ifstream file("working_memory_checkpoint.bin", std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Read magic number for validation
            uint64_t magic;
            file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
            if (magic != 0x4E574D43524B4D00ULL) {
                return false;
            }
            
            // Read version
            uint32_t version;
            file.read(reinterpret_cast<char*>(&version), sizeof(version));
            if (version != 1) {
                return false;
            }
            
            // Read checkpoint data
            size_t memoryNeuronsSize;
            file.read(reinterpret_cast<char*>(&memoryNeuronsSize), sizeof(memoryNeuronsSize));
            checkpoint.memoryNeurons.resize(memoryNeuronsSize);
            if (memoryNeuronsSize > 0) {
                file.read(reinterpret_cast<char*>(checkpoint.memoryNeurons.data()),
                         memoryNeuronsSize * sizeof(NeuronId));
            }
            
            // Read activations
            size_t activationsSize;
            file.read(reinterpret_cast<char*>(&activationsSize), sizeof(activationsSize));
            checkpoint.memoryActivations.resize(activationsSize);
            if (activationsSize > 0) {
                file.read(reinterpret_cast<char*>(checkpoint.memoryActivations.data()),
                         activationsSize * sizeof(float));
            }
            
            // Read timestamps
            size_t timestampsSize;
            file.read(reinterpret_cast<char*>(&timestampsSize), sizeof(timestampsSize));
            checkpoint.memoryTimestamps.resize(timestampsSize);
            if (timestampsSize > 0) {
                file.read(reinterpret_cast<char*>(checkpoint.memoryTimestamps.data()),
                         timestampsSize * sizeof(SimulationStep));
            }
            
            // Read active traces
            size_t activeTracesSize;
            file.read(reinterpret_cast<char*>(&activeTracesSize), sizeof(activeTracesSize));
            checkpoint.activeTraces.resize(activeTracesSize);
            if (activeTracesSize > 0) {
                file.read(reinterpret_cast<char*>(checkpoint.activeTraces.data()),
                         activeTracesSize * sizeof(size_t));
            }
            
            // Read parameters
            file.read(reinterpret_cast<char*>(&checkpoint.capacity), sizeof(checkpoint.capacity));
            file.read(reinterpret_cast<char*>(&checkpoint.decayRate), sizeof(checkpoint.decayRate));
            file.read(reinterpret_cast<char*>(&checkpoint.neuromodulationFactor), sizeof(checkpoint.neuromodulationFactor));
            
            // Read recurrent connections
            size_t connectionsSize;
            file.read(reinterpret_cast<char*>(&connectionsSize), sizeof(connectionsSize));
            checkpoint.recurrentConnections.resize(connectionsSize);
            if (connectionsSize > 0) {
                for (auto& conn : checkpoint.recurrentConnections) {
                    file.read(reinterpret_cast<char*>(&conn.first), sizeof(NeuronId));
                    file.read(reinterpret_cast<char*>(&conn.second), sizeof(NeuronId));
                }
            }
            
            // Read winners
            size_t winnersSize;
            file.read(reinterpret_cast<char*>(&winnersSize), sizeof(winnersSize));
            checkpoint.winners.resize(winnersSize);
            if (winnersSize > 0) {
                file.read(reinterpret_cast<char*>(checkpoint.winners.data()),
                         winnersSize * sizeof(NeuronId));
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    NeuralWorkingMemoryCheckpoint createEmptyCheckpoint() const override {
        NeuralWorkingMemoryCheckpoint checkpoint;
        checkpoint.capacity = 0;
        checkpoint.decayRate = 0.1f;
        checkpoint.neuromodulationFactor = 1.0f;
        return checkpoint;
    }
};

NeuralWorkingMemorySaverLoader& getWorkingMemorySaverLoader() {
    static NeuralWorkingMemorySaverLoaderImpl instance;
    return instance;
}

} // namespace

// NeuralWorkingMemory save/load functions
bool NeuralWorkingMemory::save(NeuralWorkingMemoryCheckpoint& checkpoint) {
    try {
        // Create checkpoint from current state
        checkpoint.memoryNeurons.clear();
        checkpoint.memoryActivations.clear();
        checkpoint.memoryTimestamps.clear();
        checkpoint.activeTraces.clear();
        checkpoint.recurrentConnections.clear();
        checkpoint.winners.clear();
        
        // Copy memory neurons and activations
        checkpoint.memoryNeurons = memoryNeurons_;
        checkpoint.memoryActivations = memoryActivations_;
        
        // Get current timestep for timestamps
        SimulationStep currentStep = 0;
        if (brain_) {
            // Try to get current step from brain if available
            // In real implementation, this would be properly integrated
        }
        
        // Fill timestamps (simplified)
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            checkpoint.memoryTimestamps.push_back(currentStep + i);
        }
        
        // Copy active traces
        checkpoint.activeTraces = activeTraces_;
        
        // Copy recurrent connections (would need to access from pImpl)
        // This is simplified - real implementation would extract from pImpl
        
        // Copy winners
        checkpoint.winners = winners_;
        
        // Copy parameters
        checkpoint.capacity = capacity_;
        checkpoint.decayRate = decayRate_;
        checkpoint.neuromodulationFactor = 1.0f; // Would come from attention system
        
        return getWorkingMemorySaverLoader().save(checkpoint);
    } catch (const std::exception& e) {
        return false;
    }
}

bool NeuralWorkingMemory::load(const NeuralWorkingMemoryCheckpoint& checkpoint) {
    try {
        // Clear current state
        clear();
        
        // Restore memory neurons and activations
        memoryNeurons_.clear();
        memoryActivations_.clear();
        
        memoryNeurons_ = checkpoint.memoryNeurons;
        memoryActivations_ = checkpoint.memoryActivations;
        
        // Restore parameters
        capacity_ = checkpoint.capacity;
        decayRate_ = checkpoint.decayRate;
        
        // The actual restoration of working memory traces would need
        // to be implemented based on the brain's current state
        // This is a simplified implementation
        
        return getWorkingMemorySaverLoader().load(const_cast<NeuralWorkingMemoryCheckpoint&>(checkpoint));
    } catch (const std::exception& e) {
        return false;
    }
}

} // namespace nlm
