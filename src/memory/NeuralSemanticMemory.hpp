#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

class Brain;

class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();
    
    void initialize(Brain* brain);
    void storeFact(const std::string& key, const std::string& value);
    std::string retrieveFact(const std::string& key) const;
    bool hasFact(const std::string& key) const;
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
