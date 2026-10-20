#pragma once

#include "../brain/Brain.hpp"

namespace nlm {

class BrainPredictionSystem {
public:
    BrainPredictionSystem(Brain::Impl* pImpl) : pImpl(pImpl) {}
    
    // Update prediction system with current state
    void updatePredictionSystem();

private:
    Brain::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Brain.hpp"

namespace nlm {

class BrainCognitionSystems {
public:
    BrainCognitionSystems(Brain::Impl* pImpl) : pImpl(pImpl) {}
    
    // Update attention system
    void updateAttentionSystem(Timestamp currentTime);
    
    // Update concept formation
    void updateConceptFormation();

private:
    Brain::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Brain.hpp"

namespace nlm {

class BrainDevelopment {
public:
    BrainDevelopment(Brain::Impl* pImpl) : pImpl(pImpl) {}
    
    // Apply developmental effects
    void applyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime);

private:
    Brain::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Brain.hpp"

namespace nlm {

class BrainStatistics {
public:
    BrainStatistics(Brain::Impl* pImpl) : pImpl(pImpl) {}
    
    // Collect and log statistics
    void collectAndLogStatistics() const;

private:
    Brain::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Brain.hpp"

namespace nlm {

class BrainSerialization {
public:
    BrainSerialization(Brain::Impl* pImpl) : pImpl(pImpl) {}
    
    // Save brain state to file
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);

private:
    Brain::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Brain.hpp"

namespace nlm {

class BrainPlasticity {
public:
    BrainPlasticity(Brain::Impl* pImpl) : pImpl(pImpl) {}
    
    // Apply all plasticity rules
    void applyPlasticityRules(Timestamp currentTime);
    
    // Apply structural plasticity
    void applyStructuralPlasticity(SimulationStep currentStep);

private:
    Brain::Impl* pImpl;
};

} // namespace nlm