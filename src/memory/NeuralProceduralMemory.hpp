#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

class Brain;

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    
    Skill() : proficiency(0.0f) {}
    Skill(const std::string& name, const std::vector<NeuronId>& pattern)
        : name(name), neuralPattern(pattern), proficiency(0.0f) {}
};

class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();
    
    void initialize(Brain* brain);
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    Skill* getSkill(const std::string& name);
    std::vector<Skill*> getAllSkills();
    void updateProficiency(const std::string& name, float delta);
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
