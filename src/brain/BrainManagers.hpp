#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

class MemoryManager;
class NeuromodulationManager;
class DevelopmentManager;

// Brain managers - handles memory, neuromodulation, and development
struct BrainManagers {
    std::unique_ptr<MemoryManager> memoryManager;
    std::unique_ptr<NeuromodulationManager> neuromodulationManager;
    std::unique_ptr<DevelopmentManager> developmentManager;
    
    bool initialize(Brain* brain);
    void update(float timestep);
    void applyNeuromodulation(std::vector<std::unique_ptr<NeuralRegion>>& regions);
    void applyDevelopmentEffects(Brain* brain, float plasticityModifier);
    bool validateConfiguration(std::shared_ptr<Config> config);
    void logStatus() const;
    
    // Access methods
    MemoryManager* getMemoryManager() { return memoryManager.get(); }
    NeuromodulationManager* getNeuromodulationManager() { return neuromodulationManager.get(); }
    DevelopmentManager* getDevelopmentManager() { return developmentManager.get(); }
};

} // namespace nlm
