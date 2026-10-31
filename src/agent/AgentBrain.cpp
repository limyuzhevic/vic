#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Start with no prediction (all zeros), so the first scene is
    // novel — matching the old step-0 novelty spike — and the
    // prediction is then built from actual percepts.
    predictedVision_.clear();
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (16 rays in SimpleWorld -> sensoryVision_ neurons).
    // NOTE (Phase 4, tried and reverted): encoding closeness
    // (1 - vision) * 5 was tested first — it makes an empty scene
    // quiet and patterns sparse, but with random connectivity p=0.1 a
    // sparse pattern drives only ~4-8 sensory neurons, and that is not
    // enough coincident input to recruit internal/motor populations
    // (each needs ~6 coincident inputs to cross threshold), so the
    // whole network went silent and Phase 3 collapsed to ~200 Wait.
    // Selectivity is therefore pursued through plasticity and
    // homeostatic scaling (STDP.cpp, Brain.cpp), not by silencing the
    // sensory input. The original intensity encoding is kept.
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }

    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    maxTouch_ = 0.0f;
    for (float t : touch) maxTouch_ = std::max(maxTouch_, t);
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty as prediction error against a slowly adapting
    // expectation of the scene (habituation). Phase 4: the old code
    // differenced consecutive frames, which had two fatal problems.
    // (1) It divided by previousVision_.size() = 16*16 = 256 while the
    // world's vision is only 16 rays, dividing every real change by
    // 16. (2) Even correctly normalised, per-step vision change while
    // moving is only ~0-0.004 (the agent moves <=0.3 units per
    // executed action and most actions are throttled), so novelty was
    // ~0 on every step after step 0 and curiosity never fired. A slow
    // prediction (alpha 0.05, ~20-step lag) accumulates sustained
    // change: steady movement keeps prediction error elevated, while
    // a stationary agent's prediction catches up and novelty decays
    // to 0 — exactly the habituation behaviour curiosity needs.
    if (!vision.empty()) {
        if (predictedVision_.size() != vision.size()) {
            predictedVision_.assign(vision.size(), 0.0f);
        }
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size(); ++i) {
            totalDiff += std::abs(vision[i] - predictedVision_[i]);
        }
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ *= sensoryNoveltyDecay_;
        // Adapt the prediction toward the current scene.
        constexpr float kPredictionAlpha = 0.05f;
        for (size_t i = 0; i < vision.size(); ++i) {
            predictedVision_[i] += kPredictionAlpha * (vision[i] - predictedVision_[i]);
        }
    }

    // Update curiosity based on novelty. Phase 4: curiosity is a short
    // trace, not an instantaneous value — a single novel step used to
    // vanish immediately, and a small but sustained novelty (the normal
    // case while moving) never accumulated. The 0.3 trace term keeps
    // curiosity alive for a few steps after a novel event but decays
    // fast enough that steady, familiar input settles to a low level
    // (steady state for constant novelty n is ~2n/0.7, e.g. n=0.05
    // settles near 0.14), so exploration stays proportional and does
    // not swamp learned behaviour.
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f
                        + 0.3f * curiosityLevel_;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;

    MotorCommand decoded = decodeFromMotorNeurons();

    // Apply curiosity-based exploration. Phase 4: the old code gated
    // exploration behind curiosity > 0.3 (decode) and > 0.5
    // (selectWithCuriosity), thresholds novelty could never reach after
    // step 0, so exploration was dead. Exploration is now proportional
    // to curiosity (see selectWithCuriosity) with no hard gate.
    if (curiosityEnabled_) {
        decoded = selectWithCuriosity(decoded);
    }

    lastDecoded_ = decoded;
    hasLastDecoded_ = true;
    return decoded;
}

std::vector<AgentBrain::MotorGroupActivity> AgentBrain::getMotorGroupActivities() const {
    auto calc = [](const std::vector<Neuron*>& neurons, const char* name) {
        MotorGroupActivity out{name, 0.0f, 0, neurons.size()};
        if (neurons.empty()) return out;
        float sum = 0.0f;
        float rateSum = 0.0f;
        for (const Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            if (n->getState().firingState == FiringState::Refractory) ++out.refractoryCount;
            rateSum += n->getFiringRate();
        }
        out.activity = sum / static_cast<float>(neurons.size());
        out.firingRate = rateSum / static_cast<float>(neurons.size());
        return out;
    };
    return {
        calc(motorForward_, "forward"),
        calc(motorBackward_, "backward"),
        calc(motorTurnLeft_, "left"),
        calc(motorTurnRight_, "right"),
        calc(motorInteract_, "interact"),
        calc(motorWait_, "wait"),
    };
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    auto activities = getMotorGroupActivities();
    // Order matches getMotorGroupActivities(): fwd, bwd, left, right, interact, wait
    // Phase 4: innate proximity reflex for Interact. Interact never won
    // because its rate sat ~5 Hz below forward/backward in the saturated
    // attractor, and there was no link between being near something and
    // interacting with it — so the agent could walk past a resource and
    // never try. Touch range (1.5) closely matches interact range
    // (1.0 + object radius ~0.4), so maxTouch > 0.05 means something is
    // within interact range. When that holds, Interact gets a flat
    // +12 Hz bonus: enough to overcome the usual few-Hz deficit, not
    // enough to override a decisively winning movement pattern. This is
    // a reflex, not learned behaviour — the +10 reward that follows is
    // what reward-modulated plasticity can then learn from. It is
    // type-blind (touch cannot tell resource from hazard), so it can
    // also trigger a hazard interaction; that cost is real and is
    // reported with the results, not hidden.
    float interactRate = activities[4].firingRate;
    // The bonus only applies when the interact group is actually
    // firing: with a silent network (all rates ~0) the bonus alone
    // must not manufacture an Interact out of nothing — that produced
    // 11 spurious interacts in a silent Phase 3 episode.
    // It also does not apply twice in a row, and only while the scene
    // has changed recently (novelty > 0.02): without these, an agent
    // touching a wall (touch stays high, Interact does not move it)
    // decoded Interact on 108/200 steps and never repositioned — a
    // reflex trap, and STDP then reinforced the frozen-percept
    // pathways until Interact won on raw rate too. A stationary agent's
    // visual prediction catches up, novelty falls, and the bonus
    // switches off, so movement resumes; a moving agent near
    // something keeps novelty elevated and the bonus stays on.
    if (maxTouch_ > 0.05f && interactRate > 0.1f && noveltyLevel_ > 0.02f &&
        !(hasLastDecoded_ && lastDecoded_ == MotorCommand::Interact)) {
        interactRate += 12.0f;
    }
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, activities[0].firingRate},
        {MotorCommand::MoveBackward, activities[1].firingRate},
        {MotorCommand::TurnLeft, activities[2].firingRate},
        {MotorCommand::TurnRight, activities[3].firingRate},
        {MotorCommand::Interact, interactRate},
    };

    // Relative, spike-based decoding, not an absolute 0.5 mV membrane
    // threshold with Wait as the default winner. Two reasons: (1) the old
    // code required a motor group to beat both the Wait group (by strict
    // >) and 0.5, and motor membrane activities were 0 or far below 0.5,
    // so the agent could never move; (2) membrane deviation itself is a
    // bad proxy for firing, because a neuron that just spiked resets its
    // membrane to rest — at sampling time the neurons that fired have
    // activity 0, exactly erasing the signal. Mean firing rate (now
    // derived from counted spikes) survives the reset, so the group that
    // fired most in the recent past wins. Wait is not a motor pattern —
    // it is the absence of one — so it does not compete here. Only
    // return Wait when there is essentially no motor firing at all.
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    if (bestActivity <= 0.1f) {
        return MotorCommand::Wait;
    }
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action, with probability
    // proportional to curiosity. Phase 4: the old 0.5 hard gate meant
    // exploration never fired once novelty fell below 0.25 (which was
    // every step after step 0, given the novelty bug). Now a typical
    // moving curiosity of ~0.14 gives ~4% random actions, a novel scene
    // at curiosity ~0.85 gives ~25%, and a fully familiar, stationary
    // scene (curiosity ~0) gives none — exploration scales with novelty
    // instead of being on/off behind an unreachable threshold.
    {
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random

        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(7));
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();

            if (std::abs(eligibility) > 0.001f) {
                // Reward-modulated weight change. Phase 4: the old
                // rule used delta = eligibility * dopamine with no
                // learning rate, and eligibility traces accumulate
                // STDP deltas to ~4, so a single +10 reward (dopamine
                // clamped to 1) drove every eligible synapse straight
                // to its clamp — one event saturated the network. A
                // 0.05 learning rate and a +-0.1 per-event cap keep a
                // reward event meaningful without erasing all prior
                // selectivity in one step (standard R-STDP scaling).
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * 0.05f;
                delta = std::clamp(delta, -0.1f, 0.1f);
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

float AgentBrain::getExpectedReward() const {
    return expectedReward_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear the visual prediction (a new episode's first scene is
    // novel again) and the touch state.
    predictedVision_.clear();
    maxTouch_ = 0.0f;
    hasLastDecoded_ = false;
}

} // namespace nlm
