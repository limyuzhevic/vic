# NLM Phase 6 - User Guide: Improved Reward and Learning Systems

## Overview

This guide documents the significant improvements made to the NLM Phase 6 implementation, focusing on the enhanced reward system and learning mechanisms that prevent rapid saturation and support selective pattern learning.

## Key Improvements

### 1. Enhanced Reward System

#### Before (Original)
- Resource collection: +10.0 per resource
- Hazard damage: -15.0 per hazard
- Movement costs: -0.01 to -0.02
- Very coarse reward granularity

#### After (Improved)
- Resource collection: +4.0 per resource (40% of original)
- Hazard damage: -6.0 per hazard (40% of original)
- Movement costs: -0.01 to -0.02 (more incremental)
- Added exploration rewards: +0.005 to +0.015 per resting action
- Added efficiency bonuses: +0.05 for optimal resource collection
- Added discovery bonuses: +0.1 for first exploration after 20s

#### Impact
The reward system now provides:
- More granular feedback for incremental learning
- Balanced positive/negative reinforcement
- Incentives for exploration and efficiency
- Prevention of rapid reward saturation

### 2. Improved Learning Mechanisms

#### A. Planner Learning
**Before**: Large, immediate reward impacts
```cpp
// Original: r = reward / 10.0f;  // Range: [-1.0, 1.0]
brain_->getPlanner()->setActionQuality(taken, 0.9f * q + 0.1f * r);
```

**After**: Controlled, bounded reward impacts
```cpp
// Improved: r = reward / 8.0f;  // Range: [-0.8, 0.8]
// Additional cap: max(0.05f, qDelta) to maintain selectivity
brain_->getPlanner()->setActionQuality(taken, targetQ);
```

#### B. Neuromodulation (Dopamine)
**Before**: Full prediction error
```cpp
dopamineLevel_ = predictionError_;  // Range: [-1.0, 1.0]
```

**After**: Scaled prediction error with stability
```cpp
dopamineLevel_ = predictionError_ * 0.8f;  // Range: [-0.8, 0.8]
```

#### C. Reward-Modulated Plasticity
**Before**: Aggressive weight updates
```cpp
float delta = eligibility * dopamineLevel_ * plasticityModifier_ * 0.05f;
delta = std::clamp(delta, -0.1f, 0.1f);
syn->addToWeight(delta);
```

**After**: Conservative weight updates
```cpp
float learningRate = 0.02f;  // Reduced from 0.05f
float maxWeightChange = 0.05f;  // Reduced from 0.1f
float delta = eligibility * dopamineLevel_ * plasticityModifier_ * learningRate;
delta = std::clamp(delta, -maxWeightChange, maxWeightChange);
// Only positive eligibility affects weights (selectivity preservation)
if (eligibility > 0.0f) {
    syn->addToWeight(delta);
}
```

#### D. Curiosity-Based Exploration
**Before**: Hard exploration gates
```cpp
float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30%
```

**After**: Adaptive exploration with familiarity damping
```cpp
float exploreChance = curiosityLevel_ * 0.15f;  // Reduced max
exploreChance *= std::max(0.5f, noveltyLevel_);  // Damped by familiarity
exploreChance = std::clamp(exploreChance, .0f, 0.2f);  // Cap at 20%
```

### 3. Concept Formation
**Before**: No concept updates (presentExperience calls with 0 learning rate)
**After**: Incremental concept learning
```cpp
brain_->getConceptFormation()->presentExperience(pat, lastTouch_, reward, 0.1f);
```

## Benefits of Improvements

### 1. Avoids Saturation
- Learning systems have caps and gradual updates
- Per-step changes bounded to maintain diversity
- No single event can dominate the learning process

### 2. Maintains Behavioral Diversity
- Selective pressure prevents runaway convergence
- Small updates preserve variety in action selection
- Better balance between exploitation and exploration

### 3. More Granular Learning
- Incremental reward signals provide finer feedback
- Gradual preference formation and adjustment
- Better credit assignment across sequences of actions

### 4. Adaptive Exploration
- Curiosity scales with environmental familiarity
- Exploration rewarded when novel patterns are present
- Avoids both premature convergence and excessive random behavior

### 5. Stable Plasticity
- Weight updates are controlled and bounded
- Eligibility trace management preserves selectivity
- Neuromodulation provides smoother learning dynamics

## Configuration Recommendations

### For Research and Experimentation
```bash
# Use the improved defaults (already applied)
# Test with different environment configurations
# Monitor learning curves over longer time periods

# Example: Run Phase 6 demo with enhanced systems
./build/nlm_phase6_demo --config configs/default.cfg
```

### Configuration File Examples
Refer to `configs/nlm_phase6_examples.cfg` for additional configuration options.

### Monitoring and Debugging
The improved systems include better diagnostic output through the existing thought traces and logging systems.

## Testing and Validation

### Performance Metrics
- **Learning curves**: Monitor gradual vs. saturated learning patterns
- **Behavioral diversity**: Track variety in action selection over time
- **Reward accumulation**: Observe incremental reward processing
- **Checkpoint reliability**: Verify save/load functionality

### Expected Outcomes
1. **Longer training windows**: Learning extends over more steps without saturation
2. **More diverse behaviors**: Agents maintain variety in action patterns
3. **Stable performance**: Performance improvements continue over time
4. **Better adaptation**: Agents respond more gracefully to environmental changes

## Migration Guide

### From Previous Phase 6 Versions
1. **Reward expectations**: Resource/hazard rewards are now 40% of previous values
2. **Learning speed**: Learning is more gradual - expect longer training periods
3. **Behavior variety**: Agents may exhibit more diverse behaviors initially
4. **Exploration patterns**: Exploration is more adaptive and less random

### No Breaking Changes
- All existing APIs remain unchanged
- Configuration file formats remain compatible
- Output formats and thought traces maintain existing structure
- Checkpoint format remains compatible

## Advanced Usage

### Custom Reward Scaling
For environments requiring different reward scales, modify the multipliers in the SimpleWorld implementation:

```cpp
// Example: For slower learning in complex environments
result.reward = obj.value * 0.2f;  // Further reduce resource rewards
```

### Learning Rate Tuning
The improved learning dynamics include several tunable parameters:

1. **Planner learning rate**: `0.05f` (reduce for very stable environments)
2. **Preference update rate**: `0.05f` (reduce for conservative learning)
3. **Plasticity learning rate**: `0.02f` (reduce for stable weight changes)
4. **Exploration cap**: `0.2f` (reduce for more focused learning)

## Troubleshooting

### If Learning Seems Too Slow
- Check reward multipliers in SimpleWorld.cpp
- Verify novelty levels are high enough to encourage exploration
- Review curiosity damping parameters in AgentBrain.cpp

### If Behaviors Are Too Diverse
- The improved systems are designed to maintain diversity
- This is often desirable for selective pattern learning
- Consider increasing learning rate caps if rapid convergence is desired

### If Exploration Is Insufficient
- Verify curiosity levels are being calculated correctly
- Check novelty detection is working properly
- Review the exploration damping logic

## Conclusion

The NLM Phase 6 improvements address the core issues of rapid learning saturation while maintaining the flexibility to support diverse, adaptive behavior. The system now provides:

- ✅ More granular reward feedback
- ✅ Stable learning dynamics with bounded updates
- ✅ Adaptive exploration based on environmental familiarity
- ✅ Selective pattern learning that preserves diversity
- ✅ Better balance between exploitation and exploration
- ✅ Maintained compatibility with existing systems and APIs

These improvements enable longer, more stable training periods and better quality learning outcomes, making the NLM system more suitable for complex, open-ended learning scenarios.
