#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    void initialize(Brain* brain);
    
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);
                   
    void associateFromExperience(const EpisodicMemoryItem& episode);
    
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                             size_t maxResults = 5) const;
    
    float getAssociationStrength(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;
    
    void updateAssociation(const std::vector<float>& patternA,
                           const std::vector<float>& patternB,
                           float delta);
    
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 2) const;
    
    void clear();
    
    size_t getAssociationCount() const { return associations_.size(); }
    
    Brain* getBrain() { return brain_; }
    
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