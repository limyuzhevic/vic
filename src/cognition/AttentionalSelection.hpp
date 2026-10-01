#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

namespace nlm {

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();

    void initialize(Brain* brain);

    // Update attention system
    void update(double timestep);

    // Process competition between working memory traces
    void processCompetition(const std::vector<NeuronId>& competitors);

    // Set attentional focus
    void setFocus(const std::vector<float>& focusPattern);

    // Get attentional weight for a neuron
    float getAttentionalWeight(NeuronId neuronId) const;

    // Clear attention
    void reset();

    // Check if attention is engaged
    bool isEngaged() const { return engaged_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    bool engaged_;
    std::vector<float> currentFocus_;
    std::vector<float> attentionalWeights_;
    float totalAttentionalResource_;

    // Internal methods
    void updateAttentionalWeights();
    std::vector<float> softmax(const std::vector<float>& logits);
};

} // namespace nlm