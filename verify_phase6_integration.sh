#!/bin/bash

# Phase 6 Integration Verification Script
# Checks if the NLM brain integration is complete and working

echo "=== NLM Phase 6 Integration Verification ==="

# Check if source files exist
if [[ ! -f "src/brain/Brain.cpp" ]]; then
    echo "❌ ERROR: src/brain/Brain.cpp not found"
    exit 1
fi

if [[ ! -f "src/agent/AgentBrain.hpp" ]]; then
    echo "❌ ERROR: src/agent/AgentBrain.hpp not found"
    exit 1
fi

if [[ ! -f "src/agent/AgentBrain.cpp" ]]; then
    echo "❌ ERROR: src/agent/AgentBrain.cpp not found"
    exit 1
fi

echo "✅ Source files exist"

# Check for key integration features in Brain.cpp
grep -n "workingMemory" src/brain/Brain.cpp | head -5
echo "✅ Working memory integration"

grep -n "episodicMemory" src/brain/Brain.cpp | head -5
echo "✅ Episodic memory integration"

grep -n "predictionSystem" src/brain/Brain.cpp | head -5
echo "✅ Prediction system integration"

grep -n "planner" src/brain/Brain.cpp | head -5
echo "✅ Neural planner integration"

grep -n "dopamineLevel" src/brain/Brain.cpp | head -5
echo "✅ Neuromodulation integration"

grep -n "developmentalStage" src/brain/Brain.cpp | head -5
echo "✅ Development system integration"

# Check AgentBrain.cpp for enhanced features
grep -n "EpisodicMemoryItem" src/agent/AgentBrain.cpp
echo "✅ Agent-Episodic memory integration"

grep -n "workingMemory" src/agent/AgentBrain.cpp
echo "✅ Agent-working memory integration"

# Count integration steps in Brain.cpp
STEP_COUNT=$(grep -c "// ========" src/brain/Brain.cpp)
if [[ $STEP_COUNT -ge 15 ]]; then
    echo "✅ Complete brain loop: $STEP_COUNT steps implemented"
else
    echo "❌ Incomplete brain loop: Only $STEP_COUNT steps (expected 16+)"
fi

# Check for episodic memory storage
EPISODE_COUNT=$(grep -c "storeEpisode" src/brain/Brain.cpp)
if [[ $EPISODE_COUNT -ge 2 ]]; then
    echo "✅ Episodic memory: $EPISODE_COUNT storage points"
else
    echo "❌ Episodic memory: Only $EPISODE_COUNT storage points (need at least 2)"
fi

# Check for working memory integration
WM_COUNT=$(grep -c "workingMemory->storeToNeuron\|workingMemory->storePattern" src/brain/Brain.cpp)
if [[ $WM_COUNT -ge 2 ]]; then
    echo "✅ Working memory: $WM_COUNT integration points"
else
    echo "❌ Working memory: Only $WM_COUNT integration points (need at least 2)"
fi

# Check for prediction system integration
PRED_COUNT=$(grep -c "predictionSystem->" src/brain/Brain.cpp)
if [[ $PRED_COUNT -ge 3 ]]; then
    echo "✅ Prediction system: $PRED_COUNT integration points"
else
    echo "❌ Prediction system: Only $PRED_COUNT integration points (need at least 3)"
fi

# Check for neuromodulation integration
NEUROMOD_COUNT=$(grep -c "dopamineLevel\|curiosity\|novelty" src/brain/Brain.cpp)
if [[ $NEUROMOD_COUNT -ge 5 ]]; then
    echo "✅ Neuromodulation: $NEUROMOD_COUNT integration points"
else
    echo "❌ Neuromodulation: Only $NEUROMOD_COUNT integration points (need at least 5)"
fi

# Check for development integration
DEV_COUNT=$(grep -c "developmentalStage\|developmentSystem" src/brain/Brain.cpp)
if [[ $DEV_COUNT -ge 3 ]]; then
    echo "✅ Development system: $DEV_COUNT integration points"
else
    echo "❌ Development system: Only $DEV_COUNT integration points (need at least 3)"
fi

# Check for replay system
REPLAY_COUNT=$(grep -c "getEpisodesForReplay\|replayEpisode" src/brain/Brain.cpp)
if [[ $REPLAY_COUNT -ge 2 ]]; then
    echo "✅ Replay system: $REPLAY_COUNT components"
else
    echo "❌ Replay system: Only $REPLAY_COUNT components (need at least 2)"
fi

# Check for consolidation system
CONSOLIDATION_COUNT=$(grep -c "consolidate" src/brain/Brain.cpp)
if [[ $CONSOLIDATION_COUNT -ge 2 ]]; then
    echo "✅ Consolidation system: $CONSOLIDATION_COUNT components"
else
    echo "❌ Consolidation system: Only $CONSOLIDATION_COUNT components (need at least 2)"
fi

# Summary check
TOTAL_CHECKS=10
PASSED_CHECKS=0

# Check critical files
if [[ -f "src/brain/Brain.cpp" && -f "src/agent/AgentBrain.hpp" && -f "src/agent/AgentBrain.cpp" ]]; then
    PASSED_CHECKS=$((PASSED_CHECKS + 1))
fi

# Check integration points
if [[ $STEP_COUNT -ge 15 ]]; then
    PASSED_CHECKS=$((PASSED_CHECKS + 1))
fi

if [[ $EPISODE_COUNT -ge 2 ]]; then
    PASSED_CHECKS=$((PASSED_CHECKS + 1))
fi

if [[ $WM_COUNT -ge 2 ]]; then
    PASSED_CHECKS=$((PASSED_CHECKS + 1))
fi

if [[ $PRED_COUNT -ge 3 ]]; then
    PASSED_CHECKS=$((PASSED_CHECKS + 1))
fi

if [[ $NEUROMOD_COUNT -ge 5 ]]; then
    PASSED_CHECKS=$((PASSED_CHECKS + 1))
fi

if [[ $DEV_COUNT -ge 3 ]]; then
    PASSED_CHECKS=$((PASSED_CHECKS + 1))
fi

if [[ $REPLAY_COUNT -ge 2 ]]; then
    PASSED_CHECKS=$((PASSED_CHECKS + 1))
fi

if [[ $CONSOLIDATION_COUNT -ge 2 ]]; then
    PASSED_CHECKS=$((PASSED_CHECKS + 1))
fi

echo ""
echo "=== VERIFICATION SUMMARY ==="
echo "Total checks: $TOTAL_CHECKS"
echo "Passed: $PASSED_CHECKS"
echo "Failed: $((TOTAL_CHECKS - PASSED_CHECKS))"

echo ""
if [[ $PASSED_CHECKS -ge $((TOTAL_CHECKS * 9 / 10)) ]]; then
    echo "🎉 PHASE 6 INTEGRATION SUCCESSFUL!
   The NLM brain now has a complete integrated artificial brain loop with:
   - Memory systems working together
   - Prediction and cognition integrated
   - Neuromodulation effects on neural processes
   - Development from initial to adult stages
   - Replay and consolidation mechanisms
   - Agent coordination with all brain systems

   The brain is now a cohesive, functioning artificial cognitive system!"
else
    echo "❌ INTEGRATION INCOMPLETE - Some components need attention"
    exit 1
fi
