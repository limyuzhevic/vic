#pragma once

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

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

} // namespace nlm