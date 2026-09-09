#ifndef NLM_NEURALASSOCIATEMEMORY_HPP
#define NLM_NEURALASSOCIATEMEMORY_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include "Neuron.hpp"
#include "Synapse.hpp"

namespace nlm {

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    void initialize(std::shared_ptr<Brain> brain);
    void reset();
    
    void storePattern(const std::vector<NeuronId>& pattern, float strength = 1.0f);
    void recallPattern(const std::vector<NeuronId>& pattern, std::vector<float>& activations);
    void updatePattern(const std::vector<NeuronId>& pattern, float learningRate = 0.01f);
    
    size_t getPatternCount() const;
    size_t getPatternCapacity() const;
    void setPatternCapacity(size_t capacity);
    
    float getPatternStrength(const std::vector<NeuronId>& pattern) const;
    void increaseStrength(const std::vector<NeuronId>& pattern, float amount);
    void decreaseStrength(const std::vector<NeuronId>& pattern, float amount);
    
    // Hebbian learning integration
    void integrateWithHebbian(bool enable);
    void processHebbianSpikes(const std::vector<NeuronId>& preNeurons, const std::vector<NeuronId>& postNeurons);
    
    // Statistics
    size_t getTotalActivations() const;
    float getAverageActivationStrength() const;
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Pattern storage
    struct PatternEntry {
        std::vector<NeuronId> pattern;
        float strength;
        float lastAccessed;
        size_t accessCount;
        std::vector<float> activationHistory;
    };
    
    std::vector<PatternEntry> patterns_;
    size_t capacity_;
    bool integrateHebbian_;
    
    // Temporary storage for current computation
    std::vector<float> currentActivations_;
    
    // Helper methods
    void pruneWeakPatterns();
    size_t findPatternIndex(const std::vector<NeuronId>& pattern) const;
    float computePatternSimilarity(const PatternEntry& a, const PatternEntry& b) const;
};

} // namespace nlm

#endif // NLM_NEURALASSOCIATEMEMORY_HPP