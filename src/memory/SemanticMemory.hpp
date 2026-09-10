#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// Semantic Memory: Gradually acquired knowledge and facts
// NOT a simple lookup - facts are connected and influence neural processing
//
// Key mechanisms:
// - Facts are stored as associative networks
// - Retrieval triggers spreading activation
// - Knowledge integration through pattern completion
// - Truth value and confidence weighting

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store a new fact
    // The fact is integrated into the knowledge network
    void storeFact(const std::string& key, const std::string& value, float confidence = 1.0f);

    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;

    // Check if fact exists
    bool hasFact(const std::string& key) const;

    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;

    // Get fact confidence
    float getFactConfidence(const std::string& key) const;

    // Update fact value
    void updateFact(const std::string& key, const std::string& value);

    // Clear all facts
    void clear();

    // Get number of facts
    size_t getFactCount() const { return facts_.size(); }

private:
    // Find or create fact entry
    void ensureFactEntry(const std::string& key);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<std::pair<std::string, std::string>> facts_;
    std::vector<float> confidences_;
    std::vector<std::vector<std::string>> relatedFacts_;
};

} // namespace nlm