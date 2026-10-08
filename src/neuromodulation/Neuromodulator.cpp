// Acetylcholine implementation
Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

const char* Acetylcholine::getType() const {
    return "Acetylcholine";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float Acetylcholine::getPlasticityFactor() const {
    // Advanced acetylcholine-modulated plasticity factor
    float basePlasticity = 0.8f;
    float levelModulation = pImpl->level * 0.6f;
    float dopamineModulation = pImpl->dopamineInfluence * 0.3f;
    float curiosityModulation = pImpl->curiosityInfluence * 0.2f;
    float noveltyModulation = pImpl->noveltyInfluence * 0.2f;
    
    // Attention demand modulates plasticity
    float attentionModulation = pImpl->attentionDemand * 0.4f;
    
    float totalModulation = levelModulation + dopamineModulation + curiosityModulation + 
                           noveltyModulation + attentionModulation;
    
    float crossInfluences = 0.0f;
    for (const auto& pair : pImpl->crossModulatorInfluence) {
        crossInfluences += pair.second;
    }
    crossInfluences = std::min(1.0f, crossInfluences);
    
    float finalPlasticity = basePlasticity + totalModulation + crossInfluences * 0.15f;
    
    // Memory consolidation effect
    finalPlasticity *= (1.0f + pImpl->memoryConsolidation * 0.2f);
    
    return std::clamp(finalPlasticity, 0.2f, 3.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Adaptive decay with attentional focus
    float effectiveDecay = pImpl->decayRate;
    if (pImpl->attentionalFocus > 0.7f) {
        effectiveDecay *= 0.7f; // Slower decay when highly focused
    }
    
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - effectiveDecay * dtFloat);
    
    // Memory trace with consolidation
    pImpl->memory = pImpl->memory * 0.95f + pImpl->level * 0.05f;
    
    // Adaptive attentional focus
    pImpl->attentionalFocus = std::lerp(pImpl->attentionalFocus, pImpl->level, 0.1f);
}

float Acetylcholine::getStrength() const {
    return pImpl->strength;
}

void Acetylcholine::setStrength(float strength) {
    pImpl->strength = std::clamp(strength, 0.1f, 5.0f);
}

bool Acetylcholine::isActive() const {
    return pImpl->level > pImpl->baseline * 1.1f;
}

void Acetylcholine::activate() {
    pImpl->level = std::max(pImpl->level, 0.1f);
}

void Acetylcholine::deactivate() {
    pImpl->level = pImpl->baseline;
}

void Acetylcholine::adaptToSystemState(float learningProgress, float attentionDemand, float motivationLevel) {
    pImpl->attentionDemand = std::clamp(attentionDemand, 0.0f, 1.0f);
    
    // Adapt attentional focus based on learning progress
    if (learningProgress > 0.5f) {
        pImpl->attentionalFocus = std::lerp(pImpl->attentionalFocus, 0.8f, 0.2f);
    }
    
    // Adapt memory consolidation based on motivation
    pImpl->memoryConsolidation = std::lerp(pImpl->memoryConsolidation, motivationLevel * 0.7f, 0.15f);
}

void Acetylcholine::addInteraction(const std::string& targetModulator, 
                             NeuromodulatorInteraction interactionType, 
                             float influenceStrength) {
    float influence = influenceStrength;
    
    switch (interactionType) {
        case NeuromodulatorInteraction::EXCITATORY:
            influence *= 1.4f;
            break;
        case NeuromodulatorInteraction::INHIBITORY:
            influence *= 0.4f;
            break;
        case NeuromodulatorInteraction::FEEDBACK:
            influence *= 1.1f;
            break;
        case NeuromodulatorInteraction::FEEDFORWARD:
            influence *= 1.6f;
            break;
        case NeuromodulatorInteraction::MODULATORY:
            influence *= 1.0f;
            break;
    }
    
    pImpl->crossModulatorInfluence[targetModulator] = influence;
}

float Acetylcholine::getCrossModulatorInfluence(const std::string& modulatorName) const {
    auto it = pImpl->crossModulatorInfluence.find(modulatorName);
    return it != pImpl->crossModulatorInfluence.end() ? it->second : 0.0f;
}

void Acetylcholine::processSignal(float signal, const std::string& signalType) {
    if (signalType == "attention") {
        pImpl->attentionDemand = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "memory") {
        pImpl->memoryConsolidation = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "dopamine") {
        pImpl->dopamineInfluence = std::clamp(signal * 0.5f, 0.0f, 1.0f);
    } else if (signalType == "curiosity") {
        pImpl->curiosityInfluence = std::clamp(signal * 0.5f, 0.0f, 1.0f);
    }
}

const NeuromodulatorState& Acetylcholine::getState() const {
    static NeuromodulatorState state;
    state.level = pImpl->level;
    state.baseline = pImpl->baseline;
    state.peak = pImpl->peak;
    state.decayRate = pImpl->decayRate;
    state.releaseRate = pImpl->releaseRate;
    state.targetLevel = pImpl->attentionalFocus;
    state.adaptationRate = 0.1f;
    state.memory = pImpl->memory;
    state.learningRate = 0.6f + pImpl->level * 0.3f;
    state.attentionMod = pImpl->attentionGain;
    state.motivationMod = 1.0f;
    state.history = pImpl->history;
    state.crossModulatorInfluence = pImpl->crossModulatorInfluence;
    return state;
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.0f, 2.0f);
}

void Acetylcholine::setMemoryConsolidation(float consolidation) {
    pImpl->memoryConsolidation = std::clamp(consolidation, 0.0f, 1.0f);
}

void Acetylcholine::setAttentionalFocus(float focus) {
    pImpl->attentionalFocus = std::clamp(focus, 0.0f, 1.0f);
}

float Acetylcholine::getAttentionDemand() const {
    return pImpl->attentionDemand;
}

// Serotonin implementation
Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

const char* Serotonin::getType() const {
    return "Serotonin";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float Serotonin::getPlasticityFactor() const {
    // Advanced serotonin-modulated plasticity factor with learning rate
    float basePlasticity = 1.0f;
    float levelModulation = pImpl->level * 0.8f;
    float dopamineModulation = pImpl->dopamineInfluence * 0.3f;
    float noveltyModulation = pImpl->noveltyInfluence * 0.2f;
    float curiosityModulation = pImpl->curiosityInfluence * 0.1f;
    
    // Learning rate directly modulates plasticity
    float learningRateModulation = pImpl->learningRate * 0.5f;
    
    // Mood modulation affects plasticity
    float moodModulation = (1.0f - pImpl->moodModulation) * 0.2f; // Inverse relationship
    
    // Impulsivity control modulates plasticity
    float impulsivityModulation = pImpl->impulsivityControl * 0.1f;
    
    float totalModulation = levelModulation + dopamineModulation + noveltyModulation + 
                           curiosityModulation + learningRateModulation + moodModulation + 
                           impulsivityModulation;
    
    float crossInfluences = 0.0f;
    for (const auto& pair : pImpl->crossModulatorInfluence) {
        crossInfluences += pair.second;
    }
    crossInfluences = std::min(1.0f, crossInfluences);
    
    float finalPlasticity = basePlasticity + totalModulation + crossInfluences * 0.2f;
    
    // Learning rate adaptation
    pImpl->learningRate = std::lerp(pImpl->learningRate, pImpl->level * 0.5f, 0.1f);
    
    return std::clamp(finalPlasticity, 0.5f, 3.0f);
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Adaptive decay with learning rate
    float effectiveDecay = pImpl->decayRate;
    if (pImpl->learningRate > 0.7f) {
        effectiveDecay *= 0.6f; // Slower decay when learning fast
    }
    
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - effectiveDecay * dtFloat);
    
    // Memory trace
    pImpl->memory = pImpl->memory * 0.9f + pImpl->level * 0.1f;
    
    // Social behavior modulates level
    pImpl->level = std::lerp(pImpl->level, pImpl->socialBehavior * 0.7f, 0.05f);
}

float Serotonin::getStrength() const {
    return pImpl->strength;
}

void Serotonin::setStrength(float strength) {
    pImpl->strength = std::clamp(strength, 0.1f, 5.0f);
}

bool Serotonin::isActive() const {
    return pImpl->level > pImpl->baseline * 1.1f;
}

void Serotonin::activate() {
    pImpl->level = std::max(pImpl->level, 0.1f);
}

void Serotonin::deactivate() {
    pImpl->level = pImpl->baseline;
}

void Serotonin::adaptToSystemState(float learningProgress, float attentionDemand, float motivationLevel) {
    // Adapt learning rate based on learning progress
    if (learningProgress > 0.8f) {
        pImpl->learningRate = std::lerp(pImpl->learningRate, 0.9f, 0.2f);
    } else if (learningProgress < 0.3f) {
        pImpl->learningRate = std::lerp(pImpl->learningRate, 0.3f, 0.2f);
    }
    
    // Adapt mood based on motivation
    pImpl->moodModulation = std::lerp(pImpl->moodModulation, motivationLevel * 0.6f, 0.15f);
    
    // Adapt impulsivity control based on attention demand
    pImpl->impulsivityControl = std::lerp(pImpl->impulsivityControl, (1.0f - attentionDemand) * 0.7f, 0.1f);
}

void Serotonin::addInteraction(const std::string& targetModulator, 
                             NeuromodulatorInteraction interactionType, 
                             float influenceStrength) {
    float influence = influenceStrength;
    
    switch (interactionType) {
        case NeuromodulatorInteraction::EXCITATORY:
            influence *= 1.3f;
            break;
        case NeuromodulatorInteraction::INHIBITORY:
            influence *= 0.3f;
            break;
        case NeuromodulatorInteraction::FEEDBACK:
            influence *= 1.4f;
            break;
        case NeuromodulatorInteraction::FEEDFORWARD:
            influence *= 1.5f;
            break;
        case NeuromodulatorInteraction::MODULATORY:
            influence *= 1.0f;
            break;
    }
    
    pImpl->crossModulatorInfluence[targetModulator] = influence;
}

float Serotonin::getCrossModulatorInfluence(const std::string& modulatorName) const {
    auto it = pImpl->crossModulatorInfluence.find(modulatorName);
    return it != pImpl->crossModulatorInfluence.end() ? it->second : 0.0f;
}

void Serotonin::processSignal(float signal, const std::string& signalType) {
    if (signalType == "learning_rate") {
        pImpl->learningRate = std::clamp(signal, 0.1f, 1.0f);
    } else if (signalType == "mood") {
        pImpl->moodModulation = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "impulsivity") {
        pImpl->impulsivityControl = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "social") {
        pImpl->socialBehavior = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "dopamine") {
        pImpl->dopamineInfluence = std::clamp(signal * 0.5f, 0.0f, 1.0f);
    }
}

const NeuromodulatorState& Serotonin::getState() const {
    static NeuromodulatorState state;
    state.level = pImpl->level;
    state.baseline = pImpl->baseline;
    state.peak = pImpl->peak;
    state.decayRate = pImpl->decayRate;
    state.releaseRate = pImpl->releaseRate;
    state.targetLevel = pImpl->learningRate;
    state.adaptationRate = 0.1f;
    state.memory = pImpl->memory;
    state.learningRate = pImpl->learningRate;
    state.attentionMod = 1.0f;
    state.motivationMod = 1.0f;
    state.history = pImpl->history;
    state.crossModulatorInfluence = pImpl->crossModulatorInfluence;
    return state;
}

void Serotonin::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.1f, 1.0f);
}

void Serotonin::setMoodModulation(float mood) {
    pImpl->moodModulation = std::clamp(mood, 0.0f, 1.0f);
}

void Serotonin::setImpulsivityControl(float control) {
    pImpl->impulsivityControl = std::clamp(control, 0.0f, 1.0f);
}

void Serotonin::setSocialBehavior(float behavior) {
    pImpl->socialBehavior = std::clamp(behavior, 0.0f, 1.0f);
}

float Serotonin::getLearningRate() const {
    return pImpl->learningRate;
}

// Endorphins implementation
Endorphins::Endorphins() : pImpl(new Impl) {}

Endorphins::~Endorphins() = default;

const char* Endorphins::getName() const {
    return "Endo";
}

const char* Endorphins::getType() const {
    return "Endorphins";
}

float Endorphins::getLevel() const {
    return pImpl->level;
}

void Endorphins::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float Endorphins::getPlasticityFactor() const {
    // Advanced endorphins-modulated plasticity factor
    float basePlasticity = 0.7f;
    float levelModulation = pImpl->level * 0.7f;
    float dopamineModulation = pImpl->dopamineInfluence * 0.3f;
    float noveltyModulation = pImpl->noveltyInfluence * 0.2f;
    float curiosityModulation = pImpl->curiosityInfluence * 0.2f;
    
    // Motivation level directly modulates plasticity
    float motivationModulation = pImpl->motivationLevel * 0.6f;
    
    // Pain modulation inversely affects plasticity
    float painModulation = (1.0f - pImpl->painModulation) * 0.3f;
    
    // Reward feeling enhances plasticity
    float rewardModulation = pImpl->rewardFeeling * 0.4f;
    
    // Satisfaction effect
    float satisfactionModulation = pImpl->satisfaction * 0.2f;
    
    float totalModulation = levelModulation + dopamineModulation + noveltyModulation + 
                           curiosityModulation + motivationModulation + painModulation + 
                           rewardModulation + satisfactionModulation;
    
    float crossInfluences = 0.0f;
    for (const auto& pair : pImpl->crossModulatorInfluence) {
        crossInfluences += pair.second;
    }
    crossInfluences = std::min(1.0f, crossInfluences);
    
    float finalPlasticity = basePlasticity + totalModulation + crossInfluences * 0.25f;
    
    // Motivation adaptation
    pImpl->motivationLevel = std::lerp(pImpl->motivationLevel, pImpl->level * 0.8f, 0.1f);
    
    return std::clamp(finalPlasticity, 0.3f, 3.0f);
}

void Endorphins::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Adaptive decay with motivation
    float effectiveDecay = pImpl->decayRate;
    if (pImpl->motivationLevel > 0.7f) {
        effectiveDecay *= 0.5f; // Slower decay when highly motivated
    }
    
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - effectiveDecay * dtFloat);
    
    // Memory trace
    pImpl->memory = pImpl->memory * 0.95f + pImpl->level * 0.05f;
    
    // Satisfaction modulates level
    pImpl->level = std::lerp(pImpl->level, pImpl->satisfaction * 0.8f, 0.05f);
}

float Endorphins::getStrength() const {
    return pImpl->strength;
}

void Endorphins::setStrength(float strength) {
    pImpl->strength = std::clamp(strength, 0.1f, 5.0f);
}

bool Endorphins::isActive() const {
    return pImpl->level > pImpl->baseline * 1.1f;
}

void Endorphins::activate() {
    pImpl->level = std::max(pImpl->level, 0.1f);
}

void Endorphins::deactivate() {
    pImpl->level = pImpl->baseline;
}

void Endorphins::adaptToSystemState(float learningProgress, float attentionDemand, float motivationLevel) {
    pImpl->motivationLevel = std::clamp(motivationLevel, 0.0f, 1.0f);
    
    // Adapt reward feeling based on learning progress
    if (learningProgress > 0.7f) {
        pImpl->rewardFeeling = std::lerp(pImpl->rewardFeeling, 0.9f, 0.2f);
    }
    
    // Adapt pain modulation based on satisfaction
    pImpl->painModulation = std::lerp(pImpl->painModulation, (1.0f - pImpl->satisfaction) * 0.7f, 0.15f);
}

void Endorphins::addInteraction(const std::string& targetModulator, 
                             NeuromodulatorInteraction interactionType, 
                             float influenceStrength) {
    float influence = influenceStrength;
    
    switch (interactionType) {
        case NeuromodulatorInteraction::EXCITATORY:
            influence *= 1.5f;
            break;
        case NeuromodulatorInteraction::INHIBITORY:
            influence *= 0.3f;
            break;
        case NeuromodulatorInteraction::FEEDBACK:
            influence *= 1.3f;
            break;
        case NeuromodulatorInteraction::FEEDFORWARD:
            influence *= 1.6f;
            break;
        case NeuromodulatorInteraction::MODULATORY:
            influence *= 1.0f;
            break;
    }
    
    pImpl->crossModulatorInfluence[targetModulator] = influence;
}

float Endorphins::getCrossModulatorInfluence(const std::string& modulatorName) const {
    auto it = pImpl->crossModulatorInfluence.find(modulatorName);
    return it != pImpl->crossModulatorInfluence.end() ? it->second : 0.0f;
}

void Endorphins::processSignal(float signal, const std::string& signalType) {
    if (signalType == "motivation") {
        pImpl->motivationLevel = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "pain") {
        pImpl->painModulation = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "reward") {
        pImpl->rewardFeeling = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "satisfaction") {
        pImpl->satisfaction = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "dopamine") {
        pImpl->dopamineInfluence = std::clamp(signal * 0.5f, 0.0f, 1.0f);
    }
}

const NeuromodulatorState& Endorphins::getState() const {
    static NeuromodulatorState state;
    state.level = pImpl->level;
    state.baseline = pImpl->baseline;
    state.peak = pImpl->peak;
    state.decayRate = pImpl->decayRate;
    state.releaseRate = pImpl->releaseRate;
    state.targetLevel = pImpl->motivationLevel;
    state.adaptationRate = 0.1f;
    state.memory = pImpl->memory;
    state.learningRate = 0.5f;
    state.attentionMod = 1.0f;
    state.motivationMod = pImpl->motivationLevel;
    state.history = pImpl->history;
    state.crossModulatorInfluence = pImpl->crossModulatorInfluence;
    return state;
}

void Endorphins::setMotivationLevel(float motivation) {
    pImpl->motivationLevel = std::clamp(motivation, 0.0f, 1.0f);
}

void Endorphins::setPainModulation(float pain) {
    pImpl->painModulation = std::clamp(pain, 0.0f, 1.0f);
}

void Endorphins::setRewardFeeling(float feeling) {
    pImpl->rewardFeeling = std::clamp(feeling, 0.0f, 1.0f);
}

void Endorphins::setSatisfaction(float satisfaction) {
    pImpl->satisfaction = std::clamp(satisfaction, 0.0f, 1.0f);
}

float Endorphins::getMotivationLevel() const {
    return pImpl->motivationLevel;
}

// Norepinephrine implementation
Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

const char* Norepinephrine::getType() const {
    return "Norepinephrine";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float Norepinephrine::getPlasticityFactor() const {
    // Advanced norepinephrine-modulated plasticity factor
    float basePlasticity = 1.2f;
    float levelModulation = pImpl->level * 0.6f;
    float dopamineModulation = pImpl->dopamineInfluence * 0.2f;
    float noveltyModulation = pImpl->noveltyInfluence * 0.3f;
    float curiosityModulation = pImpl->curiosityInfluence * 0.2f;
    
    // Arousal level modulates plasticity
    float arousalModulation = pImpl->arousalLevel * 0.5f;
    
    // Vigilance effect
    float vigilanceModulation = pImpl->vigilance * 0.4f;
    
    // Stress response effect
    float stressModulation = pImpl->stressResponse * 0.3f;
    
    float totalModulation = levelModulation + dopamineModulation + noveltyModulation + 
                           curiosityModulation + arousalModulation + vigilanceModulation + 
                           stressModulation;
    
    float crossInfluences = 0.0f;
    for (const auto& pair : pImpl->crossModulatorInfluence) {
        crossInfluences += pair.second;
    }
    crossInfluences = std::min(1.0f, crossInfluences);
    
    float finalPlasticity = basePlasticity + totalModulation + crossInfluences * 0.2f;
    
    // Arousal adaptation
    pImpl->arousalLevel = std::lerp(pImpl->arousalLevel, pImpl->level * 0.7f, 0.1f);
    
    return std::clamp(finalPlasticity, 0.8f, 3.5f);
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Adaptive decay with arousal
    float effectiveDecay = pImpl->decayRate;
    if (pImpl->arousalLevel > 0.7f) {
        effectiveDecay *= 0.6f; // Slower decay when highly aroused
    }
    
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - effectiveDecay * dtFloat);
    
    // Memory trace
    pImpl->memory = pImpl->memory * 0.9f + pImpl->level * 0.1f;
    
    // Stress response modulates level
    pImpl->level = std::lerp(pImpl->level, pImpl->stressResponse * 0.6f, 0.05f);
}

float Norepinephrine::getStrength() const {
    return pImpl->strength;
}

void Norepinephrine::setStrength(float strength) {
    pImpl->strength = std::clamp(strength, 0.1f, 5.0f);
}

bool Norepinephrine::isActive() const {
    return pImpl->level > pImpl->baseline * 1.1f;
}

void Norepinephrine::activate() {
    pImpl->level = std::max(pImpl->level, 0.1f);
}

void Norepinephrine::deactivate() {
    pImpl->level = pImpl->baseline;
}

void Norepinephrine::adaptToSystemState(float learningProgress, float attentionDemand, float motivationLevel) {
    // Adapt arousal based on attention demand
    pImpl->arousalLevel = std::lerp(pImpl->arousalLevel, attentionDemand * 0.8f, 0.2f);
    
    // Adapt vigilance based on learning progress
    if (learningProgress > 0.6f) {
        pImpl->vigilance = std::lerp(pImpl->vigilance, 0.8f, 0.15f);
    } else {
        pImpl->vigilance = std::lerp(pImpl->vigilance, 0.3f, 0.15f);
    }
    
    // Adapt stress response based on motivation
    pImpl->stressResponse = std::lerp(pImpl->stressResponse, motivationLevel * 0.4f, 0.1f);
}

void Norepinephrine::addInteraction(const std::string& targetModulator, 
                             NeuromodulatorInteraction interactionType, 
                             float influenceStrength) {
    float influence = influenceStrength;
    
    switch (interactionType) {
        case NeuromodulatorInteraction::EXCITATORY:
            influence *= 1.4f;
            break;
        case NeuromodulatorInteraction::INHIBITORY:
            influence *= 0.3f;
            break;
        case NeuromodulatorInteraction::FEEDBACK:
            influence *= 1.2f;
            break;
        case NeuromodulatorInteraction::FEEDFORWARD:
            influence *= 1.5f;
            break;
        case NeuromodulatorInteraction::MODULATORY:
            influence *= 1.0f;
            break;
    }
    
    pImpl->crossModulatorInfluence[targetModulator] = influence;
}

float Norepinephrine::getCrossModulatorInfluence(const std::string& modulatorName) const {
    auto it = pImpl->crossModulatorInfluence.find(modulatorName);
    return it != pImpl->crossModulatorInfluence.end() ? it->second : 0.0f;
}

void Norepinephrine::processSignal(float signal, const std::string& signalType) {
    if (signalType == "arousal") {
        pImpl->arousalLevel = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "vigilance") {
        pImpl->vigilance = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "stress") {
        pImpl->stressResponse = std::clamp(signal, 0.0f, 1.0f);
    } else if (signalType == "dopamine") {
        pImpl->dopamineInfluence = std::clamp(signal * 0.5f, 0.0f, 1.0f);
    }
}

const NeuromodulatorState& Norepinephrine::getState() const {
    static NeuromodulatorState state;
    state.level = pImpl->level;
    state.baseline = pImpl->baseline;
    state.peak = pImpl->peak;
    state.decayRate = pImpl->decayRate;
    state.releaseRate = pImpl->releaseRate;
    state.targetLevel = pImpl->arousalLevel;
    state.adaptationRate = 0.1f;
    state.memory = pImpl->memory;
    state.learningRate = 0.6f;
    state.attentionMod = pImpl->vigilance;
    state.motivationMod = 1.0f;
    state.history = pImpl->history;
    state.crossModulatorInfluence = pImpl->crossModulatorInfluence;
    return state;
}

void Norepinephrine::setArousalLevel(float arousal) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 1.0f);
}

void Norepinephrine::setVigilance(float vigilance) {
    pImpl->vigilance = std::clamp(vigilance, 0.0f, 1.0f);
}

void Norepinephrine::setStressResponse(float stress) {
    pImpl->stressResponse = std::clamp(stress, 0.0f, 1.0f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

} // namespace nlm