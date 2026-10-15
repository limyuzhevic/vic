// SelfModel checkpoint implementation

#include "SelfModelCheckpoint.hpp"
#include "../src/brain/Brain.hpp" // For SelfModel forward declaration
#include <fstream>
#include <algorithm>
#include <stdexcept>

namespace nlm {

namespace {

// SelfModelSaverLoader implementation
class SelfModelSaverLoaderImpl : public SelfModelSaverLoader {
public:
    bool save(const SelfModelCheckpoint& checkpoint) const override {
        try {
            std::ofstream file("self_model_checkpoint.tmp", std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Write magic
            const uint64_t magic = 0x5345454C4D4F4445ULL; // "SELFMODEL"
            file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
            
            // Write version
            const uint32_t version = 1;
            file.write(reinterpret_cast<const char*>(&version), sizeof(version));
            
            // Write action-effect mappings
            uint64_t mappingsSize = checkpoint.actionEffectMappings.size();
            file.write(reinterpret_cast<const char*>(&mappingsSize), sizeof(mappingsSize));
            
            if (mappingsSize > 0) {
                for (const auto& mapping : checkpoint.actionEffectMappings) {
                    writeMapping(mapping, file);
                }
            }
            
            // Write other fields
            file.write(reinterpret_cast<const char*>(&checkpoint.isActive), sizeof(checkpoint.isActive));
            file.write(reinterpret_cast<const char*>(&checkpoint.predictionAccuracy), sizeof(checkpoint.predictionAccuracy));
            file.write(reinterpret_cast<const char*>(&checkpoint.selfOtherDistinction), sizeof(checkpoint.selfOtherDistinction));
            
            file.close();
            
            std::rename("self_model_checkpoint.tmp", "self_model_checkpoint.bin");
            return true;
        } catch (const std::exception& e) {
            std::remove("self_model_checkpoint.tmp");
            return false;
        }
    }
    
    bool load(SelfModelCheckpoint& checkpoint) override {
        try {
            std::ifstream file("self_model_checkpoint.bin", std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Read magic
            uint64_t magic;
            file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
            if (magic != 0x5345454C4D4F4445ULL) {
                return false;
            }
            
            // Read version
            uint32_t version;
            file.read(reinterpret_cast<char*>(&version), sizeof(version));
            if (version != 1) {
                return false;
            }
            
            // Read action-effect mappings
            uint64_t mappingsSize;
            file.read(reinterpret_cast<char*>(&mappingsSize), sizeof(mappingsSize));
            checkpoint.actionEffectMappings.resize(mappingsSize);
            
            if (mappingsSize > 0) {
                for (auto& mapping : checkpoint.actionEffectMappings) {
                    readMapping(mapping, file);
                }
            }
            
            // Read other fields
            file.read(reinterpret_cast<char*>(&checkpoint.isActive), sizeof(checkpoint.isActive));
            file.read(reinterpret_cast<char*>(&checkpoint.predictionAccuracy), sizeof(checkpoint.predictionAccuracy));
            file.read(reinterpret_cast<char*>(&checkpoint.selfOtherDistinction), sizeof(checkpoint.selfOtherDistinction));
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    SelfModelCheckpoint createEmptyCheckpoint() const override {
        SelfModelCheckpoint checkpoint;
        checkpoint.isActive = false;
        checkpoint.predictionAccuracy = 0;
        checkpoint.selfOtherDistinction = 0;
        return checkpoint;
    }
    
    float getActionEffectPrediction(const ActionEffectMappingCheckpoint& mapping) const override {
        return mapping.reliability * mapping.observationCount;
    }
    
private:
    void writeMapping(const ActionEffectMappingCheckpoint& mapping, std::ofstream& file) const {
        // Write mapping data
        file.write(reinterpret_cast<const char*>(&mapping.action), sizeof(mapping.action));
        
        // Write resulting state
        uint64_t resultingSize = mapping.resultingSensoryState.size();
        file.write(reinterpret_cast<const char*>(&resultingSize), sizeof(resultingSize));
        if (resultingSize > 0) {
            file.write(reinterpret_cast<const char*>(mapping.resultingSensoryState.data()),
                     resultingSize * sizeof(float));
        }
        
        // Write other fields
        file.write(reinterpret_cast<const char*>(&mapping.reward), sizeof(mapping.reward));
        file.write(reinterpret_cast<const char*>(&mapping.energy), sizeof(mapping.energy));
        file.write(reinterpret_cast<const char*>(&mapping.novelty), sizeof(mapping.novelty));
        file.write(reinterpret_cast<const char*>(&mapping.reliability), sizeof(mapping.reliability));
        file.write(reinterpret_cast<const char*>(&mapping.observationCount), sizeof(mapping.observationCount));
        file.write(reinterpret_cast<const char*>(&mapping.predictionError), sizeof(mapping.predictionError));
    }
    
    void readMapping(ActionEffectMappingCheckpoint& mapping, std::ifstream& file) const {
        // Read mapping data
        file.read(reinterpret_cast<char*>(&mapping.action), sizeof(mapping.action));
        
        // Read resulting state
        uint64_t resultingSize;
        file.read(reinterpret_cast<char*>(&resultingSize), sizeof(resultingSize));
        mapping.resultingSensoryState.resize(resultingSize);
        if (resultingSize > 0) {
            file.read(reinterpret_cast<char*>(mapping.resultingSensoryState.data()),
                     resultingSize * sizeof(float));
        }
        
        // Read other fields
        file.read(reinterpret_cast<char*>(&mapping.reward), sizeof(mapping.reward));
        file.read(reinterpret_cast<char*>(&mapping.energy), sizeof(mapping.energy));
        file.read(reinterpret_cast<char*>(&mapping.novelty), sizeof(mapping.novelty));
        file.read(reinterpret_cast<char*>(&mapping.reliability), sizeof(mapping.reliability));
        file.read(reinterpret_cast<char*>(&mapping.observationCount), sizeof(mapping.observationCount));
        file.read(reinterpret_cast<char*>(&mapping.predictionError), sizeof(mapping.predictionError));
    }
};

SelfModelSaverLoader& getSelfModelSaverLoader() {
    static SelfModelSaverLoaderImpl instance;
    return instance;
}

} // namespace

// SelfModel save/load functions (stubs)
bool SelfModel::save(SelfModelCheckpoint& checkpoint) {
    return false; // Implementation in checkpoint file
}

bool SelfModel::load(const SelfModelCheckpoint& checkpoint) {
    return false; // Implementation in checkpoint file
}

float SelfModel::getActionEffectPrediction(const ActionEffectMappingCheckpoint& mapping) const {
    return getSelfModelSaverLoader().getActionEffectPrediction(mapping);
}

bool SelfModel::hasSelfModel() const {
    return false; // Simplified implementation
}

} // namespace nlm
