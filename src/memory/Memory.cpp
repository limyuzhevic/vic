// Neural Semantic Memory Implementation
struct NeuralSemanticMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for concept recall
    std::vector<NeuronId> conceptNeurons;
    std::unordered_map<std::string, NeuronId> conceptToNeuron;
    std::unordered_set<std::string> conceptNeuronsSet;
    
    Impl() : brain(nullptr) {}
};

NeuralSemanticMemory::NeuralSemanticMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

void NeuralSemanticMemory::storeConcept(const std::string& concept,
                                      const std::vector<float>& activationPattern,
                                      const std::string& definition) {
    if (concept.empty() || activationPattern.empty() || !brain_) return;
    
    // Find or create neuron for this concept
    NeuronId conceptNeuron = findConceptNeuron(concept);
    
    // Store concept in neural pattern
    storeConceptInNeuron(conceptNeuron, activationPattern);
    
    // Store definition in semantic store
    storeDefinition(concept, definition);
    
    // Create associative links to related concepts
    std::vector<std::string> relatedConcepts = getRelatedConcepts(concept);
    for (const auto& related : relatedConcepts) {
        NeuronId relatedNeuron = findConceptNeuron(related);
        if (relatedNeuron != INVALID_NEURON_ID) {
            // Create association between concepts
            associateConcepts(conceptNeuron, relatedNeuron, 0.7f);
        }
    }
}

void NeuralSemanticMemory::storeConceptInNeuron(NeuronId neuron, const std::vector<float>& pattern) {
    if (!brain_ || neuron == INVALID_NEURON_ID) return;
    
    // Inject pattern into neuron to establish neural representation
    // Find neuron and set its activation to match the pattern
    for (auto* region : brain_->getAllRegions()) {
        for (auto* neuronPtr : region->getAllNeurons()) {
            if (neuronPtr->getId() == neuron) {
                // Use first activation value as neuron input
                float activation = pattern.size() > 0 ? pattern[0] : 0.5f;
                neuronPtr->injectCurrent(activation * 4.0f);
                break;
            }
        }
    }
}

std::vector<float> NeuralSemanticMemory::retrieveConcept(const std::string& concept) const {
    if (!brain_) return {};
    
    NeuronId conceptNeuron = findConceptNeuron(concept);
    if (conceptNeuron == INVALID_NEURON_ID) return {};
    
    // Retrieve the neural pattern stored in this neuron
    std::vector<float> pattern;
    for (auto* region : brain_->getAllRegions()) {
        for (auto* neuronPtr : region->getAllNeurons()) {
            if (neuronPtr->getId() == conceptNeuron) {
                // Estimate pattern from neuron activation
                pattern.push_back(neuronPtr->getMembranePotential() / 10.0f);
                break;
            }
        }
    }
    
    return pattern;
}

std::string NeuralSemanticMemory::retrieveDefinition(const std::string& concept) const {
    auto it = definitions_.find(concept);
    if (it != definitions_.end()) {
        return it->second;
    }
    return "";
}

bool NeuralSemanticMemory::hasConcept(const std::string& concept) const {
    return pImpl->conceptNeuronsSet.find(concept) != pImpl->conceptNeuronsSet.end();
}

std::vector<std::string> NeuralSemanticMemory::getAllConcepts() const {
    std::vector<std::string> concepts;
    concepts.reserve(pImpl->conceptNeuronsSet.size());
    for (const auto& concept : pImpl->conceptNeuronsSet) {
        concepts.push_back(concept);
    }
    return concepts;
}

void NeuralSemanticMemory::clearConcept(const std::string& concept) {
    pImpl->conceptNeuronsSet.erase(concept);
    definitions_.erase(concept);
    // Remove associations
    for (auto& assoc : conceptAssociations_) {
        if (std::get<0>(assoc) == concept || std::get<1>(assoc) == concept) {
            assoc = std::make_tuple("", "", 0.0f);
        }
    }
}

void NeuralSemanticMemory::clear() {
    pImpl->conceptNeuronsSet.clear();
    pImpl->conceptNeurons.clear();
    definitions_.clear();
    conceptAssociations_.clear();
}

void NeuralSemanticMemory::updateConceptRelevance(const std::string& concept, float relevanceDelta) {
    // Update neural strength based on concept usage
    auto it = conceptRelevance_.find(concept);
    if (it != conceptRelevance_.end()) {
        it->second = std::clamp(it->second + relevanceDelta, 0.0f, 1.0f);
    }
}

std::vector<std::string> NeuralSemanticMemory::getRelatedConcepts(const std::string& concept) const {
    std::vector<std::string> related;
    
    for (const auto& assoc : conceptAssociations_) {
        if (std::get<0>(assoc) == concept) {
            // Find related concept
            for (const auto& other : pImpl->conceptNeuronsSet) {
                if (other == std::get<1>(assoc)) {
                    related.push_back(other);
                    break;
                }
            }
        }
    }
    
    return related;
}

void NeuralSemanticMemory::associateConcepts(const std::string& conceptA, const std::string& conceptB, float strength) {
    if (conceptA.empty() || conceptB.empty() || conceptA == conceptB) return;
    
    conceptAssociations_.emplace_back(conceptA, conceptB, strength);
}

NeuronId NeuralSemanticMemory::findConceptNeuron(const std::string& concept) {
    // Check if we already have this concept
    auto it = conceptToNeuron_.find(concept);
    if (it != conceptToNeuron_.end()) {
        return it->second;
    }
    
    // Create new concept neuron
    NeuronId newNeuronId(conceptNeurons_.size() + 50000);
    conceptNeurons_.push_back(newNeuronId);
    conceptToNeuron_[concept] = newNeuronId;
    pImpl->conceptNeuronsSet.insert(concept);
    
    return newNeuronId;
}

void NeuralSemanticMemory::storeDefinition(const std::string& concept, const std::string& definition) {
    definitions_[concept] = definition;
}

// Neural Procedural Memory Implementation
struct NeuralProceduralMemory::Impl {
    Brain* brain;
    
    // Motor pattern neurons for skill execution
    std::vector<NeuronId> skillNeurons;
    std::unordered_map<std::string, NeuronId> skillToNeuron;
    std::unordered_set<std::string> skillNeuronsSet;
    
    Impl() : brain(nullptr) {}
};

NeuralProceduralMemory::NeuralProceduralMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralProceduralMemory initialized");
}

void NeuralProceduralMemory::learnSkill(const std::string& skillName,
                                       const std::vector<NeuronId>& motorPattern,
                                       float proficiency) {
    if (skillName.empty() || motorPattern.empty() || !brain_) return;
    
    // Create skill neuron for this motor pattern
    NeuronId skillNeuron = findSkillNeuron(skillName);
    
    // Encode motor pattern in skill neuron
    encodeMotorPattern(skillNeuron, motorPattern);
    
    // Store skill information
    storeSkillInfo(skillName, motorPattern, proficiency);
    
    // Create synergies with related skills
    std::vector<std::string> relatedSkills = getRelatedSkills(skillName);
    for (const auto& related : relatedSkills) {
        NeuronId relatedNeuron = findSkillNeuron(related);
        if (relatedNeuron != INVALID_NEURON_ID) {
            // Create skill association
            associateSkills(skillNeuron, relatedNeuron, 0.6f);
        }
    }
}

void NeuralProceduralMemory::encodeMotorPattern(NeuronId skillNeuron, const std::vector<NeuronId>& pattern) {
    if (!brain_ || skillNeuron == INVALID_NEURON_ID) return;
    
    // Inject pattern into motor regions to establish neural representation
    for (auto* region : brain_->getAllRegions()) {
        for (auto* neuronPtr : region->getAllNeurons()) {
            if (std::find(pattern.begin(), pattern.end(), neuronPtr->getId()) != pattern.end()) {
                // This neuron is part of the motor pattern
                // Enhance its connection to the skill neuron
                neuronPtr->injectCurrent(2.0f);
            }
        }
    }
}

SkillInfo* NeuralProceduralMemory::getSkill(const std::string& skillName) {
    auto it = skills_.find(skillName);
    if (it != skills_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<SkillInfo*> NeuralProceduralMemory::getAllSkills() {
    std::vector<SkillInfo*> result;
    for (auto& pair : skills_) {
        result.push_back(&pair.second);
    }
    return result;
}

void NeuralProceduralMemory::updateProficiency(const std::string& skillName, float delta) {
    auto it = skills_.find(skillName);
    if (it != skills_.end()) {
        it->second.proficiency = std::clamp(it->second.proficiency + delta, 0.0f, 1.0f);
        
        // Update neural strength based on proficiency changes
        updateSkillNeuronStrength(skillName, delta);
    }
}

void NeuralProceduralMemory::clearSkill(const std::string& skillName) {
    skills_.erase(skillName);
    // Remove from skill set
    for (auto it = pImpl->skillNeuronsSet.begin(); it != pImpl->skillNeuronsSet.end(); ) {
        if (*it == skillName) {
            it = pImpl->skillNeuronsSet.erase(it);
        } else {
            ++it;
        }
    }
}

void NeuralProceduralMemory::clear() {
    skills_.clear();
    pImpl->skillNeuronsSet.clear();
    pImpl->skillNeurons.clear();
    skillAssociations_.clear();
}

std::vector<std::string> NeuralProceduralMemory::getRelatedSkills(const std::string& skillName) const {
    std::vector<std::string> related;
    
    for (const auto& assoc : skillAssociations_) {
        if (std::get<0>(assoc) == skillName) {
            // Find related skill
            for (const auto& other : pImpl->skillNeuronsSet) {
                if (other == std::get<1>(assoc)) {
                    related.push_back(other);
                    break;
                }
            }
        }
    }
    
    return related;
}

void NeuralProceduralMemory::associateSkills(const std::string& skillA, const std::string& skillB, float strength) {
    if (skillA.empty() || skillB.empty() || skillA == skillB) return;
    
    skillAssociations_.emplace_back(skillA, skillB, strength);
}

NeuronId NeuralProceduralMemory::findSkillNeuron(const std::string& skillName) {
    // Check if we already have this skill
    auto it = skillToNeuron_.find(skillName);
    if (it != skillToNeuron_.end()) {
        return it->second;
    }
    
    // Create new skill neuron
    NeuronId newNeuronId(skillNeurons_.size() + 60000);
    skillNeurons_.push_back(newNeuronId);
    skillToNeuron_[skillName] = newNeuronId;
    pImpl->skillNeuronsSet.insert(skillName);
    
    return newNeuronId;
}

void NeuralProceduralMemory::storeSkillInfo(const std::string& skillName, const std::vector<NeuronId>& pattern, float proficiency) {
    SkillInfo info;
    info.name = skillName;
    info.neuralPattern = pattern;
    info.proficiency = proficiency;
    skills_[skillName] = info;
}

void NeuralProceduralMemory::updateSkillNeuronStrength(const std::string& skillName, float delta) {
    NeuronId skillNeuron = skillToNeuron_[skillName];
    if (skillNeuron == INVALID_NEURON_ID) return;
    
    // Update neuron strength based on proficiency changes
    // This would involve updating synaptic connections
    // For now, we just log the change
    NLM_LOG_DEBUG("Skill proficiency updated: " + skillName + " -> " + std::to_string(delta));
}