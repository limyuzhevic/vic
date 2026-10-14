#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    void initialize(Brain* brain);
    void associate(const std::vector<float>& patternA,
                  const std::vector<float>& patternB,
                  float strength);
    void associateFromExperience(const struct EpisodicMemoryItem& episode);
    std::vector<std::vector<float>> retrieve(const std::vector<float>& pattern) const;
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& pattern) const;
    void clear();
    NeuronId findPatternNeuron(const std::vector<float>& pattern) const;
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    Brain* brain_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

}