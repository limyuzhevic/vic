bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // TODO PHASE 2: Implement real Hebbian learning
    // PLACEHOLDER: Simple correlated firing increases weight
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count coincident spikes (simplified)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    // Apply weight change proportional to coincidences
    if (coincidences > 0) {
        applyWeightChange(synapse, pImpl->learningRate * static_cast<float>(coincidences));
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// Reward-modulated STDP (R-STDP)
struct RewardModulatedSTDP::Impl {
    float learningRate;
    float eligibilityTraceDecay;
    float baselineLTP;
    float baselineLTD;
    float timeConstant;
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    Impl() : learningRate(0.01f), eligibilityTraceDecay(0.95f), 
             baselineLTP(0.01f), baselineLTD(0.012f), timeConstant(20.0f),
             eligibilityTrace(0.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
    /*
     * Reward-modulated STDP (R-STDP) implementation
     * 
     * This rule combines spike timing information with reward signals
     * to modulate synaptic plasticity.
     * 
     * Mathematical formulation:
     * Δw = η × e(t) × Δw_STDP
     * 
     * Where:
     *   Δw = weight change
     *   η = learning rate
     *   e(t) = eligibility trace (integrates pre-post spike pairs)
     *   Δw_STDP = standard STDP weight change based on spike timing
     * 
     * Biological inspiration:
     *   - Combines dopamine signals with spike timing dependent plasticity
     *   - Implements temporal difference learning principles
     *   - Reflects reward prediction error signaling
     * 
     * Key mechanisms:
     *   - Eligibility traces decay over time
     *   - Reward signals can potentiate or depress synapses
     *   - Spike pairs contribute to eligibility trace
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= pImpl->eligibilityTraceDecay;
    
    // Update eligibility trace based on spike pairs
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: positive eligibility contribution
                float contribution = pImpl->baselineLTP * std::exp(-dt / tau);
                pImpl->eligibilityTrace += contribution;
            } else if (dt < 0) {
                // Post before pre: negative eligibility contribution
                float contribution = -pImpl->baselineLTD * std::exp(dt / tau);
                pImpl->eligibilityTrace += contribution;
            }
        }
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Weight change is modulated by eligibility trace and learning rate
    if (delta != 0.0f) {
        delta *= pImpl->learningRate * pImpl->eligibilityTrace;
        
        // Clamp weight to bounds
        float newWeight = synapse->getWeight() + delta;
        float minWeight = -1.0f;
        float maxWeight = 1.0f;
        newWeight = std::clamp(newWeight, minWeight, maxWeight);
        synapse->setWeight(newWeight);
        
        // Reset eligibility trace after weight change
        pImpl->eligibilityTrace = 0.0f;
    }
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setEligibilityTraceDecay(float decay) {
    pImpl->eligibilityTraceDecay = std::clamp(decay, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getEligibilityTraceDecay() const {
    return pImpl->eligibilityTraceDecay;
}

void RewardModulatedSTDP::signalReward(float reward, float predictionError) {
    // Apply reward modulation based on prediction error
    // Positive reward (prediction error > 0) enhances plasticity
    // Negative reward (prediction error < 0) suppresses plasticity
    
    if (predictionError > 0.0f) {
        // Positive prediction error: strengthen relevant synapses
        pImpl->eligibilityTrace += predictionError * pImpl->learningRate;
    } else if (predictionError < 0.0f) {
        // Negative prediction error: depress synapses
        pImpl->eligibilityTrace += predictionError * pImpl->learningRate * 0.5f;
    }
}

// Triplet STDP with synaptic facilitation/depression
struct TripletSTDP::Impl {
    float learningRate;
    float Aplus;           // LTP amplitude (for triplets)
    float Aminus;          // LTD amplitude (for triplets)
    float tauPlus;         // Time constant for LTP triplet
    float tauMinus;        // Time constant for LTD triplet
    float STPTimeWindow;   // Standard STDP time window
    
    // Synaptic facilitation and depression variables
    float facilitation;   // Short-term facilitation
    float depression;      // Short-term depression
    
    Impl() : learningRate(0.01f), Aplus(0.01f), Aminus(0.012f), 
             tauPlus(20.0f), tauMinus(20.0f), STPTimeWindow(20.0f),
             facilitation(1.0f), depression(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    /*
     * Triplet STDP implementation
     * 
     * This rule extends standard STDP to include triplet interactions,
     * capturing the effects of synaptic facilitation and depression.
     * 
     * Biological inspiration:
     *   - Models presynaptic facilitation (APV-dependent)
     *   - Models postsynaptic depression (calcium-dependent)
     *   - Captures short-term synaptic dynamics
     *   - More biologically realistic than pairwise STDP
     * 
     * Mathematical formulation:
     * Δw = η × (A+ × exp(-Δt/τ+) × F(t) - A- × exp(Δt/τ-) × D(t))
     * 
     * Where:
     *   F(t) = facilitation factor based on recent presynaptic activity
     *   D(t) = depression factor based on recent postsynaptic activity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update short-term synaptic dynamics
    float tau = pImpl->tauPlus;
    float deltaW = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: Triplet LTP contribution
                // Includes presynaptic facilitation factor
                float facilitation = std::min(1.0f, static_cast<float>(preSpikes.size()) / 10.0f);
                float tripletLTP = pImpl->Aplus * std::exp(-dt / tau) * facilitation;
                deltaW += tripletLTP;
            } else if (dt < 0) {
                // Post before pre: Triplet LTD contribution
                // Includes postsynaptic depression factor
                float depression = std::min(1.0f, static_cast<float>(postSpikes.size()) / 10.0f);
                float tripletLTD = -pImpl->Aminus * std::exp(dt / tau) * depression;
                deltaW += tripletLTD;
            }
        }
    }
    
    // Apply weight change with triplet interactions
    if (std::abs(deltaW) > 1e-6f) {
        deltaW *= pImpl->learningRate;
        synapse->addToWeight(deltaW);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float TripletSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void TripletSTDP::setAplus(float a) {
    pImpl->Aplus = std::clamp(a, 0.0f, 1.0f);
}

float TripletSTDP::getAplus() const {
    return pImpl->Aplus;
}

void TripletSTDP::setAminus(float a) {
    pImpl->Aminus = std::clamp(a, 0.0f, 1.0f);
}

float TripletSTDP::getAminus() const {
    return pImpl->Aminus;
}

void TripletSTDP::setTauPlus(float tau) {
    pImpl->tauPlus = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTauPlus() const {
    return pImpl->tauPlus;
}

void TripletSTDP::setTauMinus(float tau) {
    pImpl->tauMinus = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTauMinus() const {
    return pImpl->tauMinus;
}

void TripletSTDP::setSTDPTimeWindow(float window) {
    pImpl->STDPTimeWindow = std::clamp(window, 1.0f, 100.0f);
}

float TripletSTDP::getSTDPTimeWindow() const {
    return pImpl->STDPTimeWindow;
}

// Global (meta-)plasticity: changes in plasticity rules themselves
struct GlobalPlasticityRule::Impl {
    float plasticityLearningRate;
    float stabilityTarget;
    float plasticityChangeRate;
    
    // Current plasticity state
    float currentPlasticityLevel;
    float plasticityHistory;
    
    Impl() : plasticityLearningRate(0.001f), stabilityTarget(0.5f), 
             plasticityChangeRate(0.01f), currentPlasticityLevel(1.0f),
             plasticityHistory(0.0f) {}
};

GlobalPlasticityRule::GlobalPlasticityRule() : pImpl(new Impl) {}

GlobalPlasticityRule::~GlobalPlasticityRule() = default;

void GlobalPlasticityRule::update(Synapse* synapse,
                                   const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   TimestepDuration dt) {
    /*
     * Global (meta-)plasticity rule implementation
     * 
     * This rule implements changes in synaptic plasticity rules themselves,
     * allowing for metaplastic changes in response to learning history.
     * 
     * Biological inspiration:
     *   - Metaplasticity: plasticity of plasticity
     *   - Homeostatic regulation of synaptic changes
     *   - Prevents runaway excitation or depression
     *   - Stabilizes learning over long time scales
     * 
     * Mathematical formulation:
     *   dη/dt = -α × (η - η*) + β × ∑Δw
     * 
     * Where:
     *   η = learning rate (plasticity parameter)
     *   η* = target stability level
     *   α = decay rate
     *   β = plastic change contribution
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update global plasticity based on learning history
    float learningHistory = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (std::abs(dt) < 10.0f) {  // Within 10ms window
                // Accumulate recent weight changes
                learningHistory += 1.0f;
            }
        }
    }
    
    // Update plasticity level towards target
    float targetPlasticity = pImpl->stabilityTarget;
    float changeRate = pImpl->plasticityChangeRate;
    
    pImpl->currentPlasticityLevel += changeRate * (targetPlasticity - pImpl->currentPlasticityLevel);
    pImpl->currentPlasticityLevel += pImpl->plasticityLearningRate * learningHistory;
    
    // Apply global plasticity modulation to synapse
    // This could affect eligibility traces, learning rates, etc.
    float eligibilityTrace = synapse->getEligibilityTrace();
    eligibilityTrace *= pImpl->currentPlasticityLevel;
    synapse->setEligibilityTrace(eligibilityTrace);
}

void GlobalPlasticityRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Global plasticity modifies weight change
    if (delta != 0.0f) {
        delta *= pImpl->currentPlasticityLevel;
        
        // Apply modified weight change
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
        synapse->setWeight(newWeight);
    }
}

const char* GlobalPlasticityRule::getName() const {
    return "GlobalPlasticity";
}

void GlobalPlasticityRule::setPlasticityLearningRate(float rate) {
    pImpl->plasticityLearningRate = rate;
}

float GlobalPlasticityRule::getPlasticityLearningRate() const {
    return pImpl->plasticityLearningRate;
}

void GlobalPlasticityRule::setStabilityTarget(float target) {
    pImpl->stabilityTarget = std::clamp(target, 0.0f, 1.0f);
}

float GlobalPlasticityRule::getStabilityTarget() const {
    return pImpl->stabilityTarget;
}

void GlobalPlasticityRule::setPlasticityChangeRate(float rate) {
    pImpl->plasticityChangeRate = rate;
}

float GlobalPlasticityRule::getPlasticityChangeRate() const {
    return pImpl->plasticityChangeRate;
}

} // namespace nlm
