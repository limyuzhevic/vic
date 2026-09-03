# NLM Experiment Failures

This document records failed experiments, architectures, and approaches that didn't work as expected. Documenting failures is essential for scientific integrity and to prevent repeating mistakes.

## Purpose

1. **Scientific honesty**: Record what doesn't work
2. **Avoid repetition**: Don't try the same failed approaches
3. **Learn from mistakes**: Extract lessons from failures
4. **Context**: Show the development process including dead ends

## Format

```markdown
## Failure: [Brief Title]
**Date**: YYYY-MM-DD
**Phase**: Phase X
**Severity**: Minor / Major / Critical

### Hypothesis
What we expected to happen

### What Actually Happened
Detailed description of failure

### Root Cause Analysis
Why it failed

### Lessons Learned
What we learned from this failure

### Related Failures
Links to similar failures
```

---

## Failures Log

<!-- Add failures below -->

## Phase 5 Failures (In Progress)

<!-- To be populated during Phase 5 development -->

## Phase 4 Failures

### Failure: Transformer-like Attention in Phase 4
**Date**: 2024-XX-XX
**Phase**: Phase 4
**Severity**: Major

#### Hypothesis
We implemented an "attention-like" mechanism for neural selection that was intended to be competitive and biologically inspired.

#### What Actually Happened
Upon review, the mechanism bore strong resemblance to QKV attention used in Transformers:
- Computed query from one population
- Computed key/value from other populations
- Used softmax-like competition

#### Root Cause Analysis
The design emerged from reading Transformer literature rather than biological inspiration. We inadvertently borrowed from deep learning rather than designing from neural principles.

#### Lessons Learned
- Must explicitly verify designs don't resemble Transformers
- Need checklist for anti-Transformer review
- Biological inspiration must come first, not be retrofitted

#### Related Failures
- None (first major incident)

### Failure: Dense Synapse Storage Initial Design
**Date**: 2024-XX-XX
**Phase**: Phase 4
**Severity**: Major

#### Hypothesis
We could efficiently store synapses using dense vector<unique_ptr<Synapse>> per region.

#### What Actually Happened
At 100k neurons with 10% connectivity, this required 1 billion synapse objects, consuming excessive memory and causing cache thrashing during iteration.

#### Root Cause Analysis
Dense storage with pointer chasing has poor cache locality. Each synapse access requires dereferencing unique_ptr, causing random memory access patterns.

#### Lessons Learned
- Sparse connectivity requires actual sparse structures
- Pointer-heavy data structures don't scale
- SoA layouts critical for performance

#### Related Failures
- None (identified and fixed in Phase 5)

## Phase 3 Failures

### Failure: Fixed Reward Without Eligibility Traces
**Date**: 2024-XX-XX
**Phase**: Phase 3
**Severity**: Major

#### Hypothesis
Reward signals would immediately affect synaptic plasticity.

#### What Actually Happened
Plasticity changes occurred too fast - reward signals modified weights before temporal credit assignment could work correctly.

#### Root Cause Analysis
Biological synaptic plasticity has temporal requirements. Reward signals need eligibility traces to bridge the time gap between action and outcome.

#### Lessons Learned
- Eligibility traces are essential for TD learning
- Immediate reward effects cause instability
- Temporal credit assignment requires delayed plasticity

#### Related Failures
- None (fixed with eligibility trace implementation)

## Phase 2 Failures

### Failure: Pure Rate-Based neurons Initial Implementation
**Date**: 2024-XX-XX
**Phase**: Phase 2
**Severity**: Minor

#### Hypothesis
We could use simple rate-based neurons for faster simulation.

#### What Actually Happened
Rate-based neurons lost the temporal precision needed for STDP. Learning was slow and imprecise.

#### Root Cause Analysis
STDP fundamentally depends on spike timing. Rate codes don't preserve precise timing information.

#### Lessons Learned
- Spiking neurons necessary for STDP
- Timing information is essential for biological plausibility
- Rate codes can be useful for analysis but not core computation

#### Related Failures
- None

## Architecture Rejections

### Rejected: Transformer-based Planning
**Date**: 2024-XX-XX
**Severity**: Critical

#### Proposed Design
Use a Transformer to evaluate action sequences for planning.

#### Rejection Reason
This would replace neural computation with external search algorithm - exactly what NLM is designed NOT to do. The core principle is that planning should emerge from neural dynamics, not be provided by an external algorithm.

#### Related: Anti-Transformer rule enforcement

### Rejected: LSTM Memory Module
**Date**: 2024-XX-XX
**Severity**: Critical

#### Proposed Design
Add LSTM-based memory module for episodic memory.

#### Rejection Reason
LSTM is a conventional deep learning component. Memory should emerge from neural activity patterns, not be provided by an external memory mechanism.

## Performance Regressions

### Regression: Synapse Iteration Order
**Date**: 2024-XX-XX
**Severity**: Minor

#### Issue
Changing synapse iteration from population-local to random caused 30% slowdown.

#### Root Cause
Random iteration caused cache misses.

#### Fix
Maintain local-first iteration order, only use random order when necessary.

## Experiment Failures

### Failed: Single-Seed Learning Curve
**Date**: 2024-XX-XX
**Severity**: Minor

#### Experiment
Ran learning experiment with single random seed.

#### Result
High variance made results inconclusive. Lucky/unlucky seed made it appear that a mechanism worked/didn't work.

#### Lesson
Always run multiple seeds. Single runs are not scientifically valid.

## Notes for Researchers

1. **Don't delete this file**: Failures are valuable information
2. **Be specific**: Provide enough detail to understand the failure
3. **Include root cause**: Understanding why matters
4. **Extract lessons**: What did we learn?
5. **Update regularly**: Add failures as they occur

## How to Add a Failure

1. Create a section with the format above
2. Include all required fields
3. Be honest about severity
4. Focus on learning, not blame
5. Review for similar existing failures