// ProceduralMemory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    std::unordered_map<std::string, size_t> skillEffectiveness;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern, float initialProficiency) {
    auto it = pImpl->skillEffectiveness.find(name);
    if (it != pImpl->skillEffectiveness.end()) {
        it->second = initialProficiency;
    } else {
        Skill skill;
        skill.name = name;
        skill.neuralPattern = pattern;
        skill.proficiency = initialProficiency;
        skill.lastUsed = 0.0f;
        skill.successCount = 0;
        pImpl->skills.push_back(skill);
        pImpl->skillEffectiveness[name] = initialProficiency;
    }
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->skillEffectiveness.find(name);
    if (it != pImpl->skillEffectiveness.end()) {
        size_t index = 0;
        for (size_t i = 0; i < pImpl->skills.size(); ++i) {
            if (pImpl->skills[i].name == name) {
                index = i;
                break;
            }
        }
        return &pImpl->skills[index];
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    result.reserve(pImpl->skills.size());
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta, bool success) {
    auto it = pImpl->skillEffectiveness.find(name);
    if (it != pImpl->skillEffectiveness.end()) {
        float proficiency = it->second;
        if (success) {
            proficiency += delta * 0.5f;
        } else {
            proficiency += delta * 0.1f;
        }
        it->second = std::clamp(proficiency, 0.0f, 1.0f);
        
        // Find the skill to update
        for (auto& skill : pImpl->skills) {
            if (skill.name == name) {
                skill.proficiency = it->second;
                if (success) {
                    skill.successCount++;
                    skill.lastUsed = 0.0f;
                }
                break;
            }
        }
    }
}

float ProceduralMemory::getSkillEffectiveness(const std::string& name) const {
    auto it = pImpl->skillEffectiveness.find(name);
    if (it != pImpl->skillEffectiveness.end()) {
        return it->second;
    }
    return 0.0f;
}

std::string ProceduralMemory::selectBestSkill(const std::vector<float>& neuralPattern) const {
    std::string bestSkill = "";
    float bestMatch = 0.0f;
    
    for (const auto& skill : pImpl->skills) {
        float matchScore = computeNeuralPatternSimilarity(neuralPattern, skill.neuralPattern);
        if (matchScore > bestMatch) {
            bestMatch = matchScore;
            bestSkill = skill.name;
        }
    }
    
    return bestSkill;
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillEffectiveness.clear();
}

float ProceduralMemory::computeNeuralPatternSimilarity(const std::vector<float>& pattern1, const std::vector<NeuronId>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i];
        norm1 += pattern1[i] * pattern1[i];
    }
    
    for (const auto& neuronId : pattern2) {
        float val = neuronId.value * 0.1f;
        dotProduct += val;
        norm2 += val * val;
    }
    
    if (norm1 > 0.0f && norm2 > 0.0f) {
        return dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
    }
    return 0.0f;
}