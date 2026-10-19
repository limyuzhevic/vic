// Procedural Memory Implementation
//===============================

class ProceduralMemory::Impl {
public:
    Brain* brain;
    size_t capacity;
    float decayRate;
    
    // Skills
    std::unordered_map<std::string, MemoryItem> skills;
    
    Impl(size_t cap, float decay) : capacity(cap), decayRate(decay) {}
};

ProceduralMemory::ProceduralMemory()
    : pImpl(new Impl(100, 0.005f))
{
    NLM_LOG_INFO("ProceduralMemory created");
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::store(const std::vector<float>& pattern, float relevance) {
    // Store as a procedural pattern
    std::string skillName = "skill_" + std::to_string(pImpl->skills.size());
    learnSkill(skillName, pattern);
    updateSkillProficiency(skillName, relevance);
}

std::vector<float> ProceduralMemory::retrieve(size_t index) const {
    // Find skill by index
    size_t count = 0;
    for (const auto& pair : pImpl->skills) {
        if (count == index) {
            return pair.second.pattern;
        }
        count++;
    }
    return {};
}

bool ProceduralMemory::contains(const std::vector<float>& pattern) const {
    for (const auto& pair : pImpl->skills) {
        const MemoryItem& skill = pair.second;
        if (skill.pattern.size() == pattern.size()) {
            bool match = true;
            for (size_t i = 0; i < pattern.size(); ++i) {
                if (std::abs(pattern[i] - skill.pattern[i]) > 0.1f) {
                    match = false;
                    break;
                }
            }
            if (match) return true;
        }
    }
    return false;
}

void ProceduralMemory::decay(float decayRate) {
    for (auto& pair : pImpl->skills) {
        pair.second.relevance *= (1.0f - decayRate);
        pair.second.strength *= (1.0f - decayRate * pImpl->decayRate);
    }
}

void ProceduralMemory::consolidate(float relevanceThreshold) {
    // Remove low-relevance skills
    pImpl->skills.erase(
        std::remove_if(pImpl->skills.begin(), pImpl->skills.end(),
                     [relevanceThreshold](const auto& pair) {
                         return pair.second.relevance < relevanceThreshold;
                     }),
        pImpl->skills.end()
    );
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

size_t ProceduralMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t ProceduralMemory::getCurrentSize() const {
    return pImpl->skills.size();
}

float ProceduralMemory::getLoad() const {
    if (pImpl->capacity == 0) return 0.0f;
    return static_cast<float>(pImpl->skills.size()) / pImpl->capacity;
}

void ProceduralMemory::updateFromBrain(Brain* brain) {
    pImpl->brain = brain;
}

void ProceduralMemory::applyToBrain(Brain* brain) {
    if (!brain || pImpl->brain != brain) return;
    
    // Apply high-proficiency skills to brain
    for (const auto& pair : pImpl->skills) {
        const MemoryItem& skill = pair.second;
        if (skill.strength > 0.5f) {
            // Inject skill pattern
            for (size_t i = 0; i < skill.pattern.size() && i < activeNeurons_.size(); ++i) {
                float activation = skill.pattern[i] * skill.strength * 2.0f;
                brain->injectCurrent(activeNeurons_[i], activation);
            }
        }
    }
}

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<float>& pattern) {
    MemoryItem skill(pattern.size(), pattern);
    skill.relevance = 1.0f;
    skill.strength = 1.0f;
    
    pImpl->skills[name] = skill;
}

std::vector<float> ProceduralMemory::executeSkill(const std::string& name) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        return it->second.pattern;
    }
    return {};
}

float ProceduralMemory::getSkillProficiency(const std::string& name) const {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        return it->second.strength;
    }
    return 0.0f;
}

void ProceduralMemory::updateSkillProficiency(const std::string& name, float delta) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        it->second.strength = std::clamp(it->second.strength + delta, 0.0f, 1.0f);
        it->second.relevance = std::clamp(it->second.relevance + delta, 0.0f, 1.0f);
    }
}

std::vector<std::string> ProceduralMemory::getAllSkills() const {
    std::vector<std::string> names;
    names.reserve(pImpl->skills.size());
    for (const auto& pair : pImpl->skills) {
        names.push_back(pair.first);
    }
    return names;
}

void ProceduralMemory::sequenceSkills(const std::vector<std::string>& skillSequence) {
    // Create compound skill from sequence
    if (skillSequence.size() < 2) return;
    
    std::string compoundName = "compound_" + skillSequence[0];
    for (size_t i = 1; i < skillSequence.size(); ++i) {
        compoundName += "_" + skillSequence[i];
    }
    
    std::vector<float> compoundPattern;
    for (const std::string& skillName : skillSequence) {
        auto it = pImpl->skills.find(skillName);
        if (it != pImpl->skills.end()) {
            // Combine patterns
            if (compoundPattern.empty()) {
                compoundPattern = it->second.pattern;
            } else {
                // Average patterns
                for (size_t i = 0; i < compoundPattern.size() && i < it->second.pattern.size(); ++i) {
                    compoundPattern[i] = (compoundPattern[i] + it->second.pattern[i]) * 0.5f;
                }
            }
        }
    }
    
    if (!compoundPattern.empty()) {
        learnSkill(compoundName, compoundPattern);
    }
}

void ProceduralMemory::interruptSkill(const std::string& name) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        it->second.relevance *= 0.1f;  // Mark as interrupted
    }
}

void ProceduralMemory::pauseSkill(const std::string& name) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        it->second.strength *= 0.5f;  // Reduce strength
    }
}

void ProceduralMemory::resumeSkill(const std::string& name) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        it->second.strength = std::min(1.0f, it->second.strength * 2.0f);
    }
}

} // namespace nlm