// SemanticMemory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    size_t maxFacts;
    
    Impl(size_t max) : maxFacts(max) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl(1000)) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Check if fact already exists
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    
    // Check capacity and insert
    if (pImpl->facts.size() < pImpl->maxFacts) {
        pImpl->facts.emplace_back(key, value);
    } else {
        // Simple FIFO: remove first fact
        pImpl->facts.erase(pImpl->facts.begin());
        pImpl->facts.emplace_back(key, value);
    }
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return pImpl->facts;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
}

// ProceduralMemory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    size_t maxSkills;
    
    Impl(size_t max) : maxSkills(max) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl(100)) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Check if skill already exists
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.proficiency = 0.1f; // Reset proficiency when relearned
            return;
        }
    }
    
    // Check capacity and insert
    if (pImpl->skills.size() < pImpl->maxSkills) {
        Skill skill;
        skill.name = name;
        skill.neuralPattern = pattern;
        skill.proficiency = 0.1f;
        pImpl->skills.push_back(skill);
    }
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

SemanticMemory::SemanticMemory()
    : pImpl(new Impl(1000))
{
    // Initialize semantic memory with basic knowledge
    storeFact("learning_rate", "0.01");
    storeFact("plasticity_enabled", "true");
    storeFact("development_stage", "initial");
    storeFact("curiosity_level", "0.5");
    storeFact("reward_function", "prediction_error");
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    
    if (pImpl->facts.size() < pImpl->maxFacts) {
        pImpl->facts.emplace_back(key, value);
    }
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return pImpl->facts;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
}

ProceduralMemory::ProceduralMemory()
    : pImpl(new Impl(100))
{
    // Initialize procedural memory with basic skills
    learnSkill("sensory_processing", {});
    learnSkill("motor_control", {});
    learnSkill("attention", {});
    learnSkill("memory", {});
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.proficiency = std::clamp(skill.proficiency + 0.1f, 0.0f, 1.0f);
            return;
        }
    }
    
    if (pImpl->skills.size() < pImpl->maxSkills) {
        Skill skill;
        skill.name = name;
        skill.neuralPattern = pattern;
        skill.proficiency = 0.1f;
        pImpl->skills.push_back(skill);
    }
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}
