#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <unordered_map>

namespace nlm {

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store semantic fact (conceptual knowledge)
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve semantic fact by key
    std::string retrieveFact(const std::string& key) const;
    
    // Check if semantic fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all semantic facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all semantic knowledge
    void clear();
    
    // Get number of facts
    size_t getFactCount() const { return facts_.size(); }
    
    // Set activation level (for neural integration)
    void setActivation(const std::string& key, float activation);
    
    // Get activation level
    float getActivation(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal storage with neural activation
    std::unordered_map<std::string, std::string> facts_;
    std::unordered_map<std::string, float> activations_;
};

} // namespace nlm