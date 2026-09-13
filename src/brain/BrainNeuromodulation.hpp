#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

class BrainNeuromodulation {
public:
    // Forward declaration for friendship
    friend class Brain;
    
    BrainNeuromodulation() = default;
    ~BrainNeuromodulation() = default;
    
    // Dopamine system operations
    void initialize(class Brain* brain);
    void update(float timestep);
    void applyDopamineEffects(class Brain* brain);
    float getDopamineLevel() const;
    float getPlasticityFactor() const;
    
    // Curiosity system operations
    void initializeCuriosity(class Brain* brain);
    void update(float timestep);
    void processNovelty(const std::vector<float>& pattern);
    float getCuriosityLevel() const;
    
    // Novelty detection operations
    void initializeNovelty(class Brain* brain);
    void update(float timestep);
    float getNoveltyLevel() const;
    
    // Prediction error operations
    void initializePredictionError(class Brain* brain);
    void update(float timestep);
    float getPredictionError() const;
    
private:
    // Neuromodulation systems
    class Dopamine* dopamine;
    class Curiosity* curiosity;
    class Novelty* novelty;
    class PredictionError* predictionError;
    
    // Internal state
    class Brain* brainRef;
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionErrorLevel;
};

} // namespace nlm