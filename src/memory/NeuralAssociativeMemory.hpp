#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <string>
#include <limits>

namespace nlm {

// Enhanced associative memory with pattern completion and network analysis
class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association with strength and context
    void associate(NeuronId a, NeuronId b, float strength, const std::string& context = "");
    
    // Enhanced retrieval capabilities
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    float getAssociationStrengthWithContext(NeuronId a, NeuronId b, const std::string& context) const;
    float getMeanAssociationStrength(NeuronId neuron) const;
    
    // Pattern completion with context
    std::vector<NeuronId> completePattern(const std::vector<NeuronId>& partialPattern) const;
    std::vector<NeuronId> completePatternWithContext(const std::vector<NeuronId>& partialPattern,
                                                     const std::string& context) const;
    
    // Network-level operations
    std::vector<std::pair<NeuronId, NeuronId>> getTopKAssociations(float minStrength, size_t k) const;
    std::vector<std::vector<NeuronId>> findAssociationChains(size_t maxChainLength) const;
    
    // Update association with learning rules
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    void updateAssociationWithContext(NeuronId a, NeuronId b, float delta, const std::string& context);
    void applySTDPToAssociations(const std::vector<NeuronId>& preSpikes, const std::vector<NeuronId>& postSpikes);
    
    // Interference management
    void applyLateralInhibition(float inhibitionStrength = 0.1f);
    void resolveAssociativeInterference();
    void normalizeAssociationNetwork();
    
    // Network analysis
    float getNetworkDensity() const;
    float getAverageAssociationStrength() const;
    float getClusteringCoefficient() const;
    float getCharacteristicPathLength() const;
    float getNetworkIntegrity() const;
    
    // Statistical operations
    std::vector<float> getAssociationDistribution() const;
    std::vector<NeuronId> getHighDegreeNodes(float threshold) const;
    float getNodeInfluence(NeuronId neuron) const;
    
    // Clear all
    void clear();
    
    // Memory statistics
    size_t getAssociationCount() const;
    size_t getNeuronCount() const;
    float getNetworkIntegrity() const;
    
    // Configuration
    void setMaxAssociations(size_t max);
    void setAssociationDecayRate(float rate);
    void setInhibitionRadius(float radius);
    void setLearningRate(float rate);
    void setContextMatchingThreshold(float threshold);
    
private:
    struct Association {
        NeuronId a;
        NeuronId b;
        float strength;
        std::string context;
        SimulationStep lastUpdated;
        float eligibility;
        
        Association(NeuronId a, NeuronId b, float strength, const std::string& context = "")
            : a(a), b(b), strength(strength), context(context), 
              lastUpdated(0), eligibility(0.0f) {}
    };
    
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm