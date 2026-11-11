#pragma once

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

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
    // Phase 6 wired selection: the winning motor command among the
    // five actions (fwd/bwd/left/right/interact) from the final rates.
    float rates[5] = {forwardRate, backwardRate, turnLeftRate, turnRightRate, interactRate};
    MotorCommand cmds[5] = {MotorCommand::MoveForward, MotorCommand::MoveBackward,
                            MotorCommand::TurnLeft, MotorCommand::TurnRight,
                            MotorCommand::Interact};
    float bestR = -1.0f;
    MotorCommand bestCmd = MotorCommand::Wait;
    for (int i = 0; i < 5; ++i) {
        if (rates[i] > bestR) { bestR = rates[i]; bestCmd = cmds[i]; }
    }
    return bestCmd;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Phase 4: the old code gated exploration behind curiosity > 0.3
    // (decode) and > 0.5 (selectWithCuriosity), thresholds novelty could
    // never reach after step 0, so exploration was dead. Exploration
    // is now proportional to curiosity (see decodeMotorCommand) with
    // no hard gate. Curiosity rises with novelty and prediction error,
    // so exploration responds to the actual environment state.
    float exploreFactor = curiosityLevel_;
    if (exploreFactor <= 0.0f) return defaultCmd;
    // Turn curiosity into an exploration bias: replace the default
    // command with a random action from the remaining 5 — exactly
    // what the old code did, but proportional to curiosity instead of
    // gated. The 0.15 threshold prevents exploration when curiosity is
    // too low to overcome the motor group's intrinsic firing rates.
    if (exploreFactor < 0.15f) return defaultCmd;
    
    std::vector<MotorCommand> candidates;
    for (int i = 0; i < 5; ++i) {
        if (i != static_cast<int>(defaultCmd)) {
            candidates.push_back(static_cast<MotorCommand>(i));
        }
    }
    if (candidates.empty()) return defaultCmd;
    
    return candidates[rand() % candidates.size()];
}

} // namespace nlm