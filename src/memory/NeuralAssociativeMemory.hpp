#pragma once

#include "../memory/Memory.hpp"

namespace nlm {

class NeuralAssociativeMemory : public AssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory() override = default;
    
    void initialize(Brain* brain);
    
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f) override;
    
    void associateFromExperience(const EpisodicMemoryItem& episode) override;
    
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                             size_t maxResults = 5) const override;
    
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const override;
    
    void updateAssociation(const std::vector<float>& patternA,
                           const std::vector<float>& patternB,
                           float delta) override;
    
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 2) const override;
    
    void clear() override;
    
    size_t getAssociationCount() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    
    NeuronId findPatternNeuron(const std::vector<float>& pattern);
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;
};

} // namespace nlm
