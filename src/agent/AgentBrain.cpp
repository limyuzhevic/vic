#include "AgentBrain.hpp"
#include "../cognition/NeuralPlanner.hpp"
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
                } else if (type == NeuronType::Internal) {
                    for (Neuron* n : pop->getNeurons()) {
                        internalNeurons_.push_back(n);
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

// Phase 6 text encoder: a teacher word becomes a sparse sensory
// pattern, never a string the brain can match on. Lower-case, trim,
// FNV-1a hash, then splitmix to pick 4 distinct neurons out of 16
// dedicated text neurons. Different words get different (overlapping,
// as in real sensory codes) patterns; the same word always gets the
// same pattern, which is what makes paired learning possible at all.
// The pattern is injected on sensoryVision_ neurons 16..30 — the
// tail vision neurons that vision itself never uses (vision is 16
// rays, the vision group has 31 neurons), so text has its own
// channel and cannot masquerade as a visual scene.
std::vector<size_t> AgentBrain::encodeText(const std::string& text) {
    std::string norm;
    for (char c : text) {
        if (c == ' ' || c == '\t' || c == '\n') continue;
        norm += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (norm.empty()) return {};
    uint64_t h = 1469598103934665603ULL;
    for (char c : norm) {
        h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        h *= 1099511628211ULL;
    }
    std::vector<size_t> out;
    uint64_t state = h;
    while (out.size() < 4) {
        // splitmix64
        state += 0x9e3779b97f4a7c15ULL;
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        z = z ^ (z >> 31);
        size_t idx = static_cast<size_t>(z % 15);
        if (std::find(out.begin(), out.end(), idx) == out.end()) out.push_back(idx);
    }
    return out;
}

std::vector<float> AgentBrain::getInternalFiringRates() const {
    std::vector<float> rates;
    rates.reserve(internalNeurons_.size());
    for (const Neuron* n : internalNeurons_) {
        rates.push_back(n ? n->getFiringRate() : 0.0f);
    }
    return rates;
}

std::vector<float> AgentBrain::conceptPattern() const {
    std::vector<float> pat;
    for (float v : lastVision_) pat.push_back(1.0f - v);
    for (float t : lastTouch_) pat.push_back(t);
    return pat;
}

size_t AgentBrain::getConceptCount() const {
    return brain_ && brain_->getConceptFormation() ? brain_->getConceptFormation()->getConceptCount() : 0;
}

float AgentBrain::getSelfModelConfidence() const {
    if (!selfModelReady_) return 0.0f;
    float sum = 0.0f;
    int n = 0;
    for (int a = 0; a < 6; ++a) {
        float c = selfModel_.getSelfModelConfidence(static_cast<ActionType>(a));
        if (c > 0.0f) { sum += c; ++n; }
    }
    return n ? sum / n : 0.0f;
}

size_t AgentBrain::getSocialObservationCount() const {
    return socialReady_ ? socialLearning_.getObservationCount() : 0;
}

void AgentBrain::observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision,
                                   const std::vector<float>& afterVision) {
    if (!socialReady_ && brain_) { socialLearning_.initialize(brain_.get()); socialReady_ = true; }
    ActionType act = ActionType::Wait;
    switch (observed) {
        case MotorCommand::MoveForward: act = ActionType::MoveForward; break;
        case MotorCommand::MoveBackward: act = ActionType::MoveBackward; break;
        case MotorCommand::TurnLeft: act = ActionType::TurnLeft; break;
        case MotorCommand::TurnRight: act = ActionType::TurnRight; break;
        case MotorCommand::Interact: act = ActionType::Interact; break;
        default: break;
    }
    socialLearning_.observeAgentAction(act, beforeVision, afterVision);
}

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

    // Phase 6 self-model, wired into the loop: the percept arriving
    // now is the consequence of the action decoded last turn, so
    // record (previous vision -> current vision) for that action.
    // This is what lets the agent later predict what its own actions
    // do, instead of the SelfModel class sitting unused in the
    // library. It records from the loop's own percepts only.
    if (hasLastDecoded_ && !lastVision_.empty() && !percept.getVision().empty()) {
        if (!selfModelReady_) { selfModel_.initialize(brain_.get()); selfModelReady_ = true; }
        ActionType act = ActionType::Wait;
        switch (lastDecoded_) {
            case MotorCommand::MoveForward: act = ActionType::MoveForward; break;
            case MotorCommand::MoveBackward: act = ActionType::MoveBackward; break;
            case MotorCommand::TurnLeft: act = ActionType::TurnLeft; break;
            case MotorCommand::TurnRight: act = ActionType::TurnRight; break;
            case MotorCommand::Interact: act = ActionType::Interact; break;
            default: break;
        }
        selfModel_.recordSelfAction(act, lastVision_, percept.getVision());
    }
    lastText_ = percept.getText();

    // Phase 5 winner-take-all lateral inhibition: if the last decode
    // had a decisive winner (margin > 5 Hz) and the scene is stable
    // (low novelty), suppress the losing motor groups with an
    // inhibitory pulse on this presentation. The pulse is applied
    // before sensory injection so it shapes the coming 10 brain steps;
    // membrane leak (tau ~20 ms) means it decays across those steps
    // rather than silencing the losers outright. A close call (margin
    // <= 5 Hz) or a novel scene gets no inhibition, so competition
    // stays open when the brain is genuinely undecided or the world
    // has changed. Two lock-in guards, learned the hard way: the pulse
    // never applies to an Interact winner (interacting does not move
    // the agent, so the percept never changes and a pulse would freeze
    // the agent on Interact — the Phase 4 reflex-trap failure mode in
    // a new form), and it stops after the same group has won 3 times
    // in a row, so a movement winner cannot persist indefinitely.
    if (lastWinnerGroup_ >= 0 && lastWinnerGroup_ != 4 &&
        consecutiveWinnerCount_ < 3 &&
        lastWinnerMargin_ > 5.0f && noveltyLevel_ < 0.3f) {
        const std::vector<Neuron*>* groups[5] = {
            &motorForward_, &motorBackward_, &motorTurnLeft_,
            &motorTurnRight_, &motorInteract_};
        for (int g = 0; g < 5; ++g) {
            if (g == lastWinnerGroup_) continue;
            for (Neuron* n : *groups[g]) {
                if (n) n->injectCurrent(-4.0f);
            }
        }
        ++wtaPulseCount_;
    }

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

    // Phase 6 text-as-a-sense: inject the teacher word's sparse
    // pattern on the dedicated text neurons, in the same presentation
    // as the visual scene, so STDP can associate the two. The brain
    // sees only which text neurons fire — never the string — and no
    // decision below branches on text content.
    if (!percept.getText().empty()) {
        for (size_t idx : encodeText(percept.getText())) {
            size_t neuronIdx = 16 + idx;
            if (neuronIdx < sensoryVision_.size() && sensoryVision_[neuronIdx]) {
                sensoryVision_[neuronIdx]->injectCurrent(8.0f);
            }
        }
    }

    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    maxTouch_ = 0.0f;
    for (float t : touch) maxTouch_ = std::max(maxTouch_, t);
    {
        float visionMean = 0.0f;
        for (float v : vision) visionMean += v;
        if (!vision.empty()) visionMean /= static_cast<float>(vision.size());
        lastVision_ = vision;
        // Forward touch: touch sensors 0, 1, 7 are the forward cone
        // (-45..+45 degrees); the other sensors point sideways/back.
        forwardTouch_ = 0.0f;
        if (touch.size() >= 8) {
            forwardTouch_ = std::max({touch[0], touch[1], touch[7]});
        } else {
            forwardTouch_ = maxTouch_;
        }
        // Closest object in the direction of the strongest forward
        // touch. Phase 5 fix: the first version used centre rays
        // 5..10 only, and in a full Phase 3 run the reflex bonus and
        // suppression fired 0 times in 1000 steps — an object touched
        // at +-30-45 degrees is outside the centre cone, so the cue
        // never saw it. Touch sensors 0/1/7 point at 0/+45/-45 degrees
        // and vision rays span -45..+45 degrees (ray i at
        // -45 + 90*i/15), so the ray(s) nearest the touch direction
        // are: 0 degrees -> rays 7,8; +45 degrees -> ray 15;
        // -45 degrees -> ray 0. The smallest value below empty (1.0)
        // among those rays is the forward object, if any. See
        // AgentBrain.hpp for why >0.5 means resource-like and <0.35
        // hazard/wall-like at touch range.
        forwardObjectVision_ = 1.0f;
        if (touch.size() >= 8 && !vision.empty() && forwardTouch_ > 0.05f) {
            size_t touchDir = 0;
            float touchBest = touch[0];
            if (touch[1] > touchBest) { touchBest = touch[1]; touchDir = 1; }
            if (touch[7] > touchBest) { touchBest = touch[7]; touchDir = 7; }
            std::vector<size_t> rays;
            if (touchDir == 0) rays = {7, 8};
            else if (touchDir == 1) rays = {15};
            else rays = {0};
            for (size_t r : rays) {
                if (r < vision.size() && vision[r] < 0.99f) {
                    forwardObjectVision_ = std::min(forwardObjectVision_, vision[r]);
                }
            }
        } else if (!vision.empty()) {
            // Phase 6: without touch, the forward object is the
            // closest centre-ray object. The Phase 5 code left
            // forwardObjectVision_ at its previous value (or 1.0)
            // whenever nothing was touched, so bright-approach
            // phototaxis — which is explicitly a vision-only, no-
            // touch cue — could almost never fire from a fresh
            // percept. Vision alone now drives it.
            for (size_t r = 5; r <= 10 && r < vision.size(); ++r) {
                if (vision[r] < 0.99f) {
                    forwardObjectVision_ = std::min(forwardObjectVision_, vision[r]);
                }
            }
        }
        lastTouch_ = touch;
        thoughtPercept_ = "visionMean=" + std::to_string(visionMean) +
                          " fwdObjVision=" + std::to_string(forwardObjectVision_) +
                          " fwdTouch=" + std::to_string(forwardTouch_) +
                          " maxTouch=" + std::to_string(maxTouch_);
        hasThoughtOutcome_ = false;
    }
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
    // case while moving) never accumulated. Phase 5: the trace term
    // rises 0.3 -> 0.6, because with 0.3 a steady novelty of 0.05
    // settled curiosity at ~0.14 and late-episode exploration died
    // (late novelty 0.005-0.13 in Phase 4). With 0.6 the same steady
    // novelty settles near 0.25 — enough to keep exploration alive
    // across an episode — while the clamp at 1.0 and the proportional
    // (not gated) exploration rule keep it from swamping learned
    // behaviour. The trace still decays, so a fully familiar scene
    // still settles low; it just takes longer to get there.
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f
                        + 0.6f * curiosityLevel_;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;

    // Phase 5 thought: ask the brain's planner what it would do, from
    // learned action quality, before the motor decode decides. The
    // planner is advisory — decodeFromMotorNeurons below still makes
    // the decision — so the thought trace can report planned != acted
    // disagreements honestly. The planner's state (vision) is the
    // percept the brain already has; its action quality is updated
    // from real rewards in applyRewardModulation.
    if (brain_->getPlanner() && !lastVision_.empty()) {
        ActionType planned = brain_->getPlanner()->planAction(lastVision_);
        switch (planned) {
            case ActionType::MoveForward: thoughtPlanned_ = MotorCommand::MoveForward; break;
            case ActionType::MoveBackward: thoughtPlanned_ = MotorCommand::MoveBackward; break;
            case ActionType::TurnLeft: thoughtPlanned_ = MotorCommand::TurnLeft; break;
            case ActionType::TurnRight: thoughtPlanned_ = MotorCommand::TurnRight; break;
            case ActionType::Interact: thoughtPlanned_ = MotorCommand::Interact; break;
            default: thoughtPlanned_ = MotorCommand::Wait; break;
        }
        hasThoughtPlanned_ = true;
    }

    MotorCommand decoded = decodeFromMotorNeurons();

    // Phase 5 winner-take-all state for the next presentation: the
    // winning group (in decode order) and its margin over the
    // runner-up, from raw motor firing rates (before reflex/bias
    // adjustments, so the inhibition reflects the brain, not the bias).
    {
        auto acts = getMotorGroupActivities();
        float best = 0.0f, second = 0.0f;
        int bestIdx = -1;
        for (int i = 0; i < 5 && i < static_cast<int>(acts.size()); ++i) {
            float r = acts[i].firingRate;
            if (r > best) { second = best; best = r; bestIdx = i; }
            else if (r > second) { second = r; }
        }
        if (bestIdx == lastWinnerGroup_) {
            ++consecutiveWinnerCount_;
        } else {
            consecutiveWinnerCount_ = 1;
        }
        lastWinnerGroup_ = bestIdx;
        lastWinnerMargin_ = best - second;
    }

    // Apply curiosity-based exploration. Phase 4: the old code gated
    // exploration behind curiosity > 0.3 (decode) and > 0.5
    // (selectWithCuriosity), thresholds novelty could never reach after
    // step 0, so exploration was dead. Exploration is now proportional
    // to curiosity (see selectWithCuriosity) with no hard gate.
    if (curiosityEnabled_) {
        decoded = selectWithCuriosity(decoded);
    }

    // Phase 6 planner agreement: planned vs finally acted, after
    // curiosity exploration (which can still override both).
    if (hasThoughtPlanned_) {
        ++plannerDecisionCount_;
        if (thoughtPlanned_ == decoded) ++plannerAgreementCount_;
    }

    // Phase 5 diversity counters, from the final action actually taken.
    if (decoded == MotorCommand::TurnLeft || decoded == MotorCommand::TurnRight) {
        stepsSinceTurn_ = 0;
    } else {
        ++stepsSinceTurn_;
    }
    if (hasLastDecoded_ && decoded == lastDecoded_ &&
        (decoded == MotorCommand::MoveForward || decoded == MotorCommand::MoveBackward)) {
        ++repeatCount_;
    } else {
        repeatCount_ = 1;
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
    // Phase 6 silent-brain guard: if no motor neuron is actually
    // firing, no bias (planner, concept, reflex) may invent an
    // action from nothing. This was found by the reverted drive-
    // scaling experiment: with the network silenced, planner bias
    // alone produced 178 empty Interacts per episode from a brain
    // that was firing at 0 Hz. A silent brain waits.
    {
        float maxRaw = 0.0f;
        for (size_t i = 0; i < 5 && i < activities.size(); ++i) {
            maxRaw = std::max(maxRaw, activities[i].firingRate);
        }
        if (maxRaw <= 0.1f) return MotorCommand::Wait;
    }
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
    float turnLeftRate = activities[2].firingRate;
    float turnRightRate = activities[3].firingRate;
    float forwardRate = activities[0].firingRate;
    float backwardRate = activities[1].firingRate;
    // Phase 5 vision-gated interact reflex. Phase 4's reflex was
    // type-blind (touch cannot tell resource from hazard), so in the
    // Phase 5 pattern probe a dark object ahead (hazard encoding)
    // triggered Interact just like a bright one (margin 0.12 Hz, a
    // tie won only by the bonus). The gate below uses the forward
    // vision value from the same percept — see AgentBrain.hpp:
    //   resource cue: forward touch + forward object vision > 0.5
    //     -> the old +12 Hz bonus, with the Phase 4 guards kept
    //        (interact group actually firing, novelty > 0.02, not
    //        twice in a row, so the wall-trap failure mode stays fixed).
    //   hazard/wall cue: forward touch + vision < 0.35
    //     -> Interact is suppressed by 12 Hz instead, and both turn
    //        groups get +8 Hz: turn away from the thing ahead rather
    //        than interact with it or keep walking into it.
    // No world-object label is used: only vision/touch values the
    // brain already receives. A resource to the side (not in the
    // forward cone) gets no bonus — the agent must turn to face it
    // first, which the turn-diversity bias below helps it do.
    const bool resourceCue = forwardTouch_ > 0.05f &&
                             forwardObjectVision_ > 0.5f &&
                             forwardObjectVision_ < 0.99f;
    const bool hazardCue = forwardTouch_ > 0.05f &&
                           forwardObjectVision_ < 0.35f;
    // Phase 5 bright-approach (phototaxis), widened in Phase 6:
    // a bright object in the forward vision direction, before
    // touch range, biases Forward by +10 Hz (was +6). The old
    // >0.5 threshold only saw a resource within ~3 units
    // ((8-3)/8*0.8 = 0.5); at 5 units a resource reads 0.3, so
    // the agent walked past distant resources without ever
    // steering toward them — the main remaining conversion gap
    // once every action actually executes. The >0.3 floor still
    // excludes hazards (their encoding tops out at 0.3, at
    // touch range) while admitting resources out to ~5 units.
    // A far wall also reads bright (distance/8), so this can
    // still walk the agent toward a wall; as it approaches,
    // the wall's vision value falls below 0.35, the hazard cue
    // takes over, and turn diversity breaks the approach.
    if (maxTouch_ <= 0.05f && forwardObjectVision_ > 0.3f &&
        forwardObjectVision_ < 0.95f) {
        forwardRate += 10.0f;
    }
    // Phase 5 interact gating: interacting with nothing is free in
    // reward terms (0) but costs a step, and in Phase 4/5 measurement
    // almost every Interact was empty (174/175 in the baseline Phase 3
    // run) — not because of the reflex, which fired 0 times, but
    // because the Interact motor group wins by noise at ~80 Hz like
    // every other group. When nothing is within touch range
    // (maxTouch <= 0.05, and touch range 1.5 closely matches interact
    // range 1.0 + radius ~0.4), Interact is penalised by 12 Hz, so a
    // decisive Interact pattern can still win but noise cannot.
    if (maxTouch_ <= 0.05f) {
        interactRate = std::max(0.0f, interactRate - 12.0f);
    }
    // Bonus raised 12 -> 16 in Phase 5, then 16 -> 20 in Phase 6:
    // after the refractory change lowered all rates, the
    // Forward-vs-Interact deficit at a resource in the probe
    // reached 17.1 Hz and the 16 Hz bonus lost by 1.1 Hz. The
    // principle is unchanged (beat the typical deficit, not a
    // decisive pattern); the hazard turn boost rises 8 -> 12 for
    // the same reason (a 55.8 Hz backward pattern beat the
    // boosted 55.1 Hz turn in the dark probe). No-touch gating
    // and hazard suppression stay at 12 Hz.
    if (resourceCue && interactRate > 0.1f && noveltyLevel_ > 0.02f &&
        !(hasLastDecoded_ && lastDecoded_ == MotorCommand::Interact)) {
        interactRate += 20.0f;
        ++reflexBonusCount_;
    } else if (hazardCue) {
        interactRate = std::max(0.0f, interactRate - 12.0f);
        turnLeftRate += 12.0f;
        turnRightRate += 12.0f;
        ++reflexSuppressCount_;
    }
    // Phase 5 learned-preference bias: the scalar trait stored on the
    // Brain (updated only by sandbox reward, checkpointed) nudges
    // Interact by at most +-5 Hz. It cannot override a decisive motor
    // pattern, but it breaks ties toward what the agent has learned
    // to like — which is exactly how a reloaded brain should differ
    // from a fresh one.
    if (brain_) {
        interactRate += brain_->getLearnedPreference() * 5.0f;
    }
    // Phase 6 planner IN the loop (was advisory only). The planner
    // suggests from learned action quality; that suggestion now
    // biases the motor decode, but only once the planner has actually
    // learned something (some action quality beyond +-0.05 from
    // experienced rewards) and only when its suggestion is clearly
    // better in its own units (quality gap > 0.15, on a -1..1
    // reward/10 scale) than the action the motor pattern would pick.
    // A +15 Hz bias can overturn a near-tie, not a decisive motor
    // pattern, and the override is counted at winner selection below.
    MotorCommand preBiasBest = MotorCommand::Wait;
    {
        float rates[5] = {forwardRate, backwardRate, turnLeftRate, turnRightRate, interactRate};
        MotorCommand cmds[5] = {MotorCommand::MoveForward, MotorCommand::MoveBackward,
                                MotorCommand::TurnLeft, MotorCommand::TurnRight,
                                MotorCommand::Interact};
        float bestR = -1.0f;
        for (int i = 0; i < 5; ++i) {
            if (rates[i] > bestR) { bestR = rates[i]; preBiasBest = cmds[i]; }
        }
    }
    bool plannerBiasApplied = false;
    MotorCommand plannerCmd = MotorCommand::Wait;
    if (brain_ && brain_->getPlanner() && hasThoughtPlanned_ &&
        thoughtPlanned_ != MotorCommand::Wait && thoughtPlanned_ != preBiasBest) {
        auto toAction = [](MotorCommand c) {
            switch (c) {
                case MotorCommand::MoveForward: return ActionType::MoveForward;
                case MotorCommand::MoveBackward: return ActionType::MoveBackward;
                case MotorCommand::TurnLeft: return ActionType::TurnLeft;
                case MotorCommand::TurnRight: return ActionType::TurnRight;
                case MotorCommand::Interact: return ActionType::Interact;
                default: return ActionType::Wait;
            }
        };
        auto* planner = brain_->getPlanner();
        // Thresholds matched to the actual quality scale: quality
        // is an EMA toward reward/10, so a single +10 resource
        // moves Interact to ~+0.1 while everyday movement costs
        // sit near -0.001. A 0.15 gap would never fire; 0.05 does,
        // and only after a real resource-scale experience.
        bool learned = false;
        for (ActionType a : {ActionType::MoveForward, ActionType::MoveBackward,
                             ActionType::TurnLeft, ActionType::TurnRight,
                             ActionType::Interact, ActionType::Wait}) {
            if (std::abs(planner->getActionQuality(a)) > 0.02f) { learned = true; break; }
        }
        float qPlanned = planner->getActionQuality(toAction(thoughtPlanned_));
        float qBest = planner->getActionQuality(toAction(preBiasBest));
        // Phase 6 guard, learned from the reverted global-
        // inhibition experiment: a planner suggestion to Interact
        // when nothing is within touch range is exactly the
        // Phase 4 trap in planner form (115 empty interacts in
        // one episode). The planner may bias movement and turns
        // freely, but Interact requires something to interact with.
        bool interactWithoutTouch = thoughtPlanned_ == MotorCommand::Interact && maxTouch_ <= 0.05f;
        if (learned && !interactWithoutTouch && qPlanned - qBest > 0.05f) {
            plannerCmd = thoughtPlanned_;
            plannerBiasApplied = true;
            switch (thoughtPlanned_) {
                case MotorCommand::MoveForward: forwardRate += 15.0f; break;
                case MotorCommand::MoveBackward: backwardRate += 15.0f; break;
                case MotorCommand::TurnLeft: turnLeftRate += 15.0f; break;
                case MotorCommand::TurnRight: turnRightRate += 15.0f; break;
                case MotorCommand::Interact: interactRate += 15.0f; break;
                default: break;
            }
        }
    }
    // Phase 6 concept bias: if the current scene matches a concept
    // whose experienced rewards were strongly positive, interacting
    // is favoured; if strongly negative, interacting is penalised
    // and turning favoured. Concepts are formed from experience in
    // applyRewardModulation — no object label is used, only the
    // scene pattern and its reward history.
    if (brain_ && brain_->getConceptFormation()) {
        auto pat = conceptPattern();
        float norm = 0.0f;
        for (float v : pat) norm += v * v;
        if (norm > 0.01f) {
            size_t cid = brain_->getConceptFormation()->getMatchingConcept(pat);
            const auto* foundConcept = cid ? brain_->getConceptFormation()->getConcept(cid) : nullptr;
            if (foundConcept && !foundConcept->instances.empty()) {
                float avg = 0.0f;
                for (const auto& inst : foundConcept->instances) avg += inst.avgReward;
                avg /= static_cast<float>(foundConcept->instances.size());
                // Thresholds matched to per-step presentation: a
                // concept absorbs many -0.01 movement steps plus
                // occasional +-10/+ -15 events, so its average is
                // ~+0.2 for a resource scene and ~-0.3 for a hazard
                // scene, not +-10. A +-1 threshold would never fire.
                if (avg > 0.1f && maxTouch_ > 0.05f) { interactRate += 8.0f; ++conceptBiasCount_; }
                else if (avg < -0.1f) {
                    interactRate = std::max(0.0f, interactRate - 8.0f);
                    turnLeftRate += 6.0f; turnRightRate += 6.0f;
                    ++conceptBiasCount_;
                }
            }
        }
    }
    // Phase 6 self-model bias, discriminative only: vision patterns
    // are so similar frame to frame (most rays read empty) that
    // self-model confidence is ~0.97 for every action — biasing
    // every action by +3 Hz would cancel out and only inflate the
    // counter. So only the single most-confident action, and only
    // when it clearly leads the runner-up (> 0.05), gets +3 Hz.
    // In practice this fires rarely, which is the honest result:
    // the self-model records and predicts, but its confidence does
    // not yet discriminate actions well.
    if (selfModelReady_) {
        MotorCommand cmds[5] = {MotorCommand::MoveForward, MotorCommand::MoveBackward,
                                MotorCommand::TurnLeft, MotorCommand::TurnRight,
                                MotorCommand::Interact};
        ActionType acts[5] = {ActionType::MoveForward, ActionType::MoveBackward,
                              ActionType::TurnLeft, ActionType::TurnRight,
                              ActionType::Interact};
        float conf[5];
        for (int i = 0; i < 5; ++i) conf[i] = selfModel_.getSelfModelConfidence(acts[i]);
        int bestI = 0;
        float second = 0.0f;
        for (int i = 0; i < 5; ++i) {
            if (conf[i] > conf[bestI]) { second = conf[bestI]; bestI = i; }
            else if (conf[i] > second) { second = conf[i]; }
        }
        if (conf[bestI] > 0.8f && conf[bestI] - second > 0.05f) {
            switch (cmds[bestI]) {
                case MotorCommand::MoveForward: forwardRate += 3.0f; break;
                case MotorCommand::MoveBackward: backwardRate += 3.0f; break;
                case MotorCommand::TurnLeft: turnLeftRate += 3.0f; break;
                case MotorCommand::TurnRight: turnRightRate += 3.0f; break;
                case MotorCommand::Interact: interactRate += 3.0f; break;
                default: break;
            }
            ++selfBiasCount_;
        }
    }
    // Phase 6 social bias: if another agent was observed acting in a
    // closely matching state (similarity > 0.8), imitating that
    // action gets +5 Hz. With no observations this is inert, which
    // is correct for a single-agent world.
    if (socialReady_ && socialLearning_.getObservationCount() > 0 && !lastVision_.empty()) {
        auto [imitAct, sim] = socialLearning_.getImitationActionAndSimilarity(lastVision_);
        if (sim > 0.8f) {
            switch (imitAct) {
                case ActionType::MoveForward: forwardRate += 5.0f; ++socialBiasCount_; break;
                case ActionType::MoveBackward: backwardRate += 5.0f; ++socialBiasCount_; break;
                case ActionType::TurnLeft: turnLeftRate += 5.0f; ++socialBiasCount_; break;
                case ActionType::TurnRight: turnRightRate += 5.0f; ++socialBiasCount_; break;
                case ActionType::Interact: interactRate += 5.0f; ++socialBiasCount_; break;
                default: break;
            }
        }
    }
    // Phase 5 turn diversity / repetition fatigue. Phase 4 oscillated
    // forward/backward, turned only 0-2 times per episode and covered
    // ~2 cells in the Phase 5 demo's 100-step sandbox run. Two small
    // selection biases, both biologically flavoured (exploration drive
    // and motor fatigue), not a pathfinder:
    //   - no turn for >25 steps while moving -> +6 Hz to both turns;
    //   - same movement 3+ times in a row -> -8 Hz to that movement.
    if (stepsSinceTurn_ > 25 &&
        (forwardRate > backwardRate || backwardRate > forwardRate)) {
        turnLeftRate += 6.0f;
        turnRightRate += 6.0f;
    }
    if (repeatCount_ >= 3) {
        if (hasLastDecoded_ && lastDecoded_ == MotorCommand::MoveForward) {
            forwardRate = std::max(0.0f, forwardRate - 8.0f);
        } else if (hasLastDecoded_ && lastDecoded_ == MotorCommand::MoveBackward) {
            backwardRate = std::max(0.0f, backwardRate - 8.0f);
        }
    }
    // Phase 5 momentum bias: reversing direction on the very next
    // step is penalised by 8 Hz. Phase 4's dominant failure was
    // forward/backward alternation (80-98 oscillations per 200-step
    // episode in the Phase 5 baseline measurement), which keeps the
    // agent in 2-6 cells all episode. A reversal penalty is movement
    // inertia, not a pathfinder: the brain can still reverse when its
    // motor pattern is decisive, but a near-tie no longer flips it
    // back and forth every step.
    if (hasLastDecoded_ && lastDecoded_ == MotorCommand::MoveForward) {
        backwardRate = std::max(0.0f, backwardRate - 8.0f);
    } else if (hasLastDecoded_ && lastDecoded_ == MotorCommand::MoveBackward) {
        forwardRate = std::max(0.0f, forwardRate - 8.0f);
    }
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardRate},
        {MotorCommand::MoveBackward, backwardRate},
        {MotorCommand::TurnLeft, turnLeftRate},
        {MotorCommand::TurnRight, turnRightRate},
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
    // Phase 6: count a genuine planner override — the planner's
    // action won only because its bias overturned the motor winner.
    if (plannerBiasApplied && best == plannerCmd && best != preBiasBest) {
        ++plannerOverrideCount_;
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
    thoughtReward_ = reward;
    hasThoughtOutcome_ = true;
    // Phase 6 concept formation, wired into the loop: the pattern
    // that led to this action, paired with this action's reward.
    // Concepts therefore emerge from experienced pattern/reward
    // pairings — a bright scene repeatedly followed by +10 becomes
    // a different concept from a dark scene followed by -15 — and
    // the concept bias in decode can then use them. The pattern is
    // closeness (1 - vision) plus touch, so an empty scene is the
    // zero pattern and is skipped (it is not a concept).
    if (brain_->getConceptFormation() && !lastVision_.empty()) {
        auto pat = conceptPattern();
        float norm = 0.0f;
        for (float v : pat) norm += v * v;
        if (norm > 0.01f) {
            brain_->getConceptFormation()->presentExperience(pat, lastTouch_, reward, 0);
        }
    }
    // Phase 5 learning for the thought/preference systems, from the
    // same reward the synaptic modulation below uses:
    //  - Planner action quality: EMA toward reward/10 for the action
    //    actually taken, so the planner's next suggestion reflects
    //    experienced outcomes, not its initial zeros.
    //  - Learned preference trait: only Interact outcomes move it
    //    (a resource reward makes interacting more attractive, a
    //    hazard penalty less so), by 0.1 * reward/10 per event. This
    //    is the trait the checkpoint carries across a reload.
    if (hasLastDecoded_) {
        ActionType taken = ActionType::Wait;
        switch (lastDecoded_) {
            case MotorCommand::MoveForward: taken = ActionType::MoveForward; break;
            case MotorCommand::MoveBackward: taken = ActionType::MoveBackward; break;
            case MotorCommand::TurnLeft: taken = ActionType::TurnLeft; break;
            case MotorCommand::TurnRight: taken = ActionType::TurnRight; break;
            case MotorCommand::Interact: taken = ActionType::Interact; break;
            default: break;
        }
        if (brain_->getPlanner()) {
            float q = brain_->getPlanner()->getActionQuality(taken);
            float r = std::clamp(reward / 10.0f, -1.0f, 1.0f);
            brain_->getPlanner()->setActionQuality(taken, 0.9f * q + 0.1f * r);
        }
        if (lastDecoded_ == MotorCommand::Interact && std::abs(reward) > 0.01f) {
            float r = std::clamp(reward / 10.0f, -1.0f, 1.0f);
            brain_->setLearnedPreference(brain_->getLearnedPreference() + 0.1f * r);
        }
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Signal the brain's dopamine neuromodulator if available (Phase 6 integration)
    if (brain_ && brain_->getDopamine()) {
        brain_->getDopamine()->signalReward(reward);
        brain_->getDopamine()->signalRewardPredictionError(predictionError_);
        NLM_LOG_INFO("AgentBrain: Signaled brain dopamine system (reward=" + std::to_string(reward) + ", prediction_error=" + std::to_string(predictionError_) + ")");
    } else {
        NLM_LOG_INFO("AgentBrain: Reward modulation complete but no brain dopamine system available");
    }
    
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

namespace {
const char* phase5CmdName(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        default: return "Wait";
    }
}
}  // namespace

std::string AgentBrain::getThoughtTrace() const {
    std::string out = "perceived=[" + thoughtPercept_ + "] -> planned=[";
    out += hasThoughtPlanned_ ? phase5CmdName(thoughtPlanned_) : "none";
    out += "] -> acted=[" + std::string(phase5CmdName(lastDecoded_)) + "] -> outcome=[";
    out += hasThoughtOutcome_ ? ("reward=" + std::to_string(thoughtReward_)) : "none";
    out += "] novelty=" + std::to_string(noveltyLevel_) +
           " curiosity=" + std::to_string(curiosityLevel_);
    return out;
}

float AgentBrain::getLearnedPreference() const {
    return brain_ ? brain_->getLearnedPreference() : 0.0f;
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
    forwardTouch_ = 0.0f;
    forwardObjectVision_ = 1.0f;
    lastVision_.clear();
    lastWinnerGroup_ = -1;
    lastWinnerMargin_ = 0.0f;
    consecutiveWinnerCount_ = 0;
    reflexBonusCount_ = 0;
    reflexSuppressCount_ = 0;
    wtaPulseCount_ = 0;
    plannerOverrideCount_ = 0;
    plannerAgreementCount_ = 0;
    plannerDecisionCount_ = 0;
    conceptBiasCount_ = 0;
    selfBiasCount_ = 0;
    socialBiasCount_ = 0;
    lastTouch_.clear();
    lastText_.clear();
    stepsSinceTurn_ = 0;
    repeatCount_ = 0;
    hasLastDecoded_ = false;
    hasThoughtPlanned_ = false;
    hasThoughtOutcome_ = false;
}

} // namespace nlm
