#include "AgentBrain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

// Constants for neuron group distribution
namespace {
    constexpr size_t NUM_MOTOR_GROUPS = 6;  // forward, backward, turn_left, turn_right, interact, wait
    constexpr size_t NUM_SENSORY_GROUPS = 4;  // vision, touch, internal, proprioception
    constexpr size_t MAX_MOTOR_GROUP_SIZE = 50;  // Maximum neurons per motor group
    constexpr size_t MAX_SENSORY_GROUP_SIZE = 100;  // Maximum neurons per sensory group
}

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
    // Initialize motor and sensory neuron groups from brain populations
    // This creates the mapping between neural populations and action categories
    // for motor neurons (forward, backward, turn_left, turn_right, interact, wait)
    // and sensory modalities (vision, touch, internal, proprioception)
    if (brain_) {
        assignNeuronGroupsToActionCategories();
    } else {
        NLM_LOG_WARN("AgentBrain initialized with null brain pointer");
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::assignNeuronGroupsToActionCategories() {
    // Assign neurons from brain populations to action categories in a structured way
    // This method iterates through all regions and populations in the brain,
    // distributing motor neurons across the 6 action categories (forward, backward,
    // turn_left, turn_right, interact, wait) and sensory neurons across the 4
    // sensory modalities (vision, touch, internal, proprioception).
    //
    // The distribution uses modulo arithmetic to ensure even coverage across
    // all categories, preventing any single action from having disproportionately
    // many neurons.
    //
    // Error handling: If brain_ is null, log a warning and return early.
    if (!brain_) {
        NLM_LOG_WARN("AgentBrain::assignNeuronGroupsToActionCategories: null brain pointer");
        return;
    }
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                // Distribute motor neurons to action groups based on their order
                // This ensures each action type gets a balanced distribution
                distributeMotorNeurons(pop->getNeurons());
            } else if (type == NeuronType::Sensory) {
                // Distribute sensory neurons to sensory modality groups
                distributeSensoryNeurons(pop->getNeurons());
            }
        }
    }
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    // Distribute motor neurons across the 6 action categories using modulo arithmetic
    // This creates a balanced distribution where each action type (forward, backward,
    // turn_left, turn_right, interact, wait) receives approximately the same number
    // of neurons, ensuring fair representation in the motor output system.
    //
    // The distribution algorithm uses neuron index modulo NUM_MOTOR_GROUPS to
    // determine which action category each neuron belongs to, providing a
    // deterministic and reproducible assignment.
    for (size_t i = 0; i < neurons.size(); ++i) {
        Neuron* neuron = neurons[i];
        
        // Use modulo arithmetic to distribute neurons evenly across motor groups
        // This ensures all 6 action types get some neural representation
        switch (i % NUM_MOTOR_GROUPS) {
            case 0: motorForward_.push_back(neuron); break;
            case 1: motorBackward_.push_back(neuron); break;
            case 2: motorTurnLeft_.push_back(neuron); break;
            case 3: motorTurnRight_.push_back(neuron); break;
            case 4: motorInteract_.push_back(neuron); break;
            case 5: motorWait_.push_back(neuron); break;
        }
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    // Distribute sensory neurons across the 4 sensory modalities using modulo arithmetic
    // This creates a balanced distribution where each sensory modality (vision, touch,
    // internal, proprioception) receives approximately the same number of neurons,
    // ensuring comprehensive sensory processing capabilities.
    //
    // The distribution algorithm uses neuron index modulo NUM_SENSORY_GROUPS to
    // determine which sensory modality each neuron belongs to, providing a
    // deterministic and reproducible assignment.
    for (size_t i = 0; i < neurons.size(); ++i) {
        Neuron* neuron = neurons[i];
        
        // Distribute sensory neurons across different modalities
        // This provides redundancy and distributed processing
        switch (i % NUM_SENSORY_GROUPS) {
            case 0: sensoryVision_.push_back(neuron); break;
            case 1: sensoryTouch_.push_back(neuron); break;
            case 2: sensoryInternal_.push_back(neuron); break;
            case 3: sensoryProprioception_.push_back(neuron); break;
        }
    }
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
    // Process sensory input from the world and inject current into sensory neurons
    // This method handles multiple sensory modalities: vision, touch, internal signals,
    // and proprioception. It also computes novelty based on prediction error for
    // curiosity-driven exploration.
    //
    // Error handling: If brain_ is null, log a warning and return early.
    if (!brain_) {
        NLM_LOG_WARN("AgentBrain::processSensoryInput: null brain pointer");
        return;
    }

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
        if (touch.size() >= 8 && !vision.empty()) {
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
        }
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
    if (!brain_) {
        NLM_LOG_WARN("AgentBrain::decodeMotorCommand called with null brain pointer");
        return MotorCommand::Wait;
    }

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
    //
    // This records which motor group won the last action selection,
    // by how much it led its nearest competitor, and tracks how many
    // consecutive times the same group has won. This information is used
    // to apply lateral inhibition to prevent runaway dominance and
    // maintain behavioral diversity.
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
    //
    // When enabled, curiosity adds random action selection to maintain
    // behavioral exploration and prevent the agent from getting stuck
    // in local optima.
    if (curiosityEnabled_) {
        decoded = selectWithCuriosity(decoded);
    }

    // Phase 5 diversity counters, from the final action actually taken.
    //
    // These counters track behavioral patterns to encourage diversity:
    // - stepsSinceTurn_: steps since last directional change (turn)
    // - repeatCount_: consecutive repetitions of same movement
    //
    // Used for turn-diversity bias (no turn for >25 steps) and
    // repetition-fatigue penalty (same movement 3+ times), not a pathfinder.
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
    // Phase 5 bright-approach (phototaxis): a bright object in the
    // forward vision direction, before touch range, biases Forward by
    // +6 Hz. This is a sensory-motor mapping from vision brightness
    // alone — no object label, no coordinates, no pathfinder — and it
    // is what can actually bring the agent to a resource, because
    // touch-range reflexes almost never fire in the sparse 20x20
    // world (reflex bonus fired 0 times in the Phase 5 world-loop
    // measurement). A far wall also reads bright (wall vision is
    // distance/8), so this can also walk the agent toward a wall; as
    // it approaches, the wall's vision value falls below 0.35, the
    // hazard cue above takes over, and turn diversity breaks the
    // approach. The bias is small enough that a decisive motor
    // pattern still wins.
    if (maxTouch_ <= 0.05f && forwardObjectVision_ > 0.5f &&
        forwardObjectVision_ < 0.95f) {
        forwardRate += 6.0f;
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
    // Bonus raised 12 -> 16 in Phase 5: after the working-memory fix,
    // motor rates rose enough that the typical Forward-vs-Interact
    // deficit at a resource reached 12.6 Hz and the old bonus lost by
    // 0.6 Hz in the pattern probe. The principle is unchanged (beat
    // the typical deficit, not a decisive pattern); the hazard
    // suppression and no-touch gating stay at 12 Hz.
    if (resourceCue && interactRate > 0.1f && noveltyLevel_ > 0.02f &&
        !(hasLastDecoded_ && lastDecoded_ == MotorCommand::Interact)) {
        interactRate += 16.0f;
        ++reflexBonusCount_;
    } else if (hazardCue) {
        interactRate = std::max(0.0f, interactRate - 12.0f);
        turnLeftRate += 8.0f;
        turnRightRate += 8.0f;
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
    //
    // Error handling: If brain_ is null or random generator is null,
    // log warnings and return the default command.
    //
    // The exploration function allows the agent to try random actions,
    // maintaining behavioral diversity and preventing convergence to
    // local optima.
    //
    // Available actions include all movement types, turning actions,
    // looking actions, interact, and wait. The random selection is
    // uniformly distributed across all 8 available actions.
    if (!brain_) {
        NLM_LOG_WARN("AgentBrain::selectWithCuriosity called with null brain pointer");
        return defaultCmd;
    }
    
    // Check for null random generator
    auto* rng = brain_->getRandomGenerator();
    if (!rng) {
        NLM_LOG_WARN("AgentBrain::selectWithCuriosity: null random generator");
        return defaultCmd;
    }

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

        float r = rng->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = rng->uniformInt(uint32_t(0), uint32_t(7));
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
    // Apply reward-based neuromodulation to the brain for learning
    // This method implements reward-modulated plasticity, updating:
    // - Planner action quality through exponential moving average
    // - Learned preference trait based on Interact outcomes
    // - Prediction error and dopamine-like signals
    // - Synaptic weights through eligibility traces
    // - Plasticity factors based on neuromodulatory signals
    //
    // Error handling: If brain_ is null, log a warning and return early.
    // If rewardModulation is disabled, return without processing.
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_WARN("AgentBrain::applyRewardModulation called with null brain pointer");
        }
        return;
    }
    
    thoughtReward_ = reward;
    hasThoughtOutcome_ = true;
    
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
    
    // Compute prediction error (difference between expected and actual reward)
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    // This signal modulates plasticity throughout the network
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range to prevent extreme values
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply reward-modulated plasticity to all synapses with eligibility traces
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
                
                // Decay eligibility trace for next reward event
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    // This allows reward signals to control learning rate throughout development
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
    stepsSinceTurn_ = 0;
    repeatCount_ = 0;
    hasLastDecoded_ = false;
    hasThoughtPlanned_ = false;
    hasThoughtOutcome_ = false;
}

} // namespace nlm
