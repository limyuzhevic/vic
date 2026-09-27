#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// TemporalRelation: Learns temporal relationships (before, after, sequence)
// NOT symbolic timestamps - encodes temporal order in neural activity

class TemporalRelation {
public:
    TemporalRelation();
    ~TemporalRelation();

    void initialize(Brain* brain);

    // Record that event A was followed by event B
    void recordSequence(const std::vector<float>& eventA,
                      const std::vector<float>& eventB,
                      float reward);

    // Given event A, predict what comes next
    std::vector<float> predictNext(const std::vector<float>& eventA) const;

    // Given event B, predict what came before
    std::vector<float> predictPrevious(const std::vector<float>& eventB) const;

    // Get confidence in prediction
    float getPredictionConfidence(const std::vector<float>& event) const;

    // Get sequence length
    size_t getSequenceLength() const { return sequences_.size(); }

    // Clear sequences
    void clear();

private:
    struct SequenceLink {
        std::vector<float> fromEvent;
        std::vector<float> toEvent;
        float strength;
        size_t observationCount;
    };

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<SequenceLink> sequences_;
};

} // namespace nlm