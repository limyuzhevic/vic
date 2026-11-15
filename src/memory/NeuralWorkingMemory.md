# NeuralWorkingMemory - Memory System Documentation

## Purpose and Architecture

NeuralWorkingMemory implements working memory using neural dynamics, providing:
- Transient storage of sensory information and goals
- Persistent neural activity for maintaining information
- Recurrent connections for memory stability
- Neural competition for selective retention

## Key Mechanisms

### 1. Memory Storage (store())
Information is encoded as neural activation patterns:
- Pattern vector maps to active neurons
- Strength parameter modulates encoding
- Automatic recurrent connection formation

### 2. Memory Retrieval (retrieve())
- Returns current memory activation levels
- Preserves temporal order of stored patterns
- Used by higher cognitive systems for decision making

### 3. Memory Maintenance (update())
- Reinfires active memory neurons
- Implements trace aging for memory decay
- Runs competition to select memory winners
- Updates recurrent connections

### 4. Memory Management
- Capacity-based storage with automatic eviction
- Activity-dependent decay (forgetting)
- Snapshot/restore for checkpointing
- Integration with attention system

## Phase 6 Fixes Applied

### 1. Active Trace Management
**Issue**: activeTraces_ was never populated - only cleared
**Fix**: storeToNeuron() now registers traces, decayWeakTraces() rebuilds list

### 2. Memory Indices During Erasure
**Issue**: Erasing traces shifted memory indices
**Fix**: Rebuild activeTraces_ from remaining activations after each erasure

### 3. Memory Capacity Management
**Issue**: No validation of pattern size vs capacity
**Fix**: Added pattern size validation and warnings

## Integration with Brain Systems

### Sensory Input Integration
- storeToNeuron() called by Brain::receiveSensoryInput()
- Sensory neuron activation stored as working memory
- Enables sensory information to influence cognition

### Attention System Integration
- runCompetition() selects winning memory traces
- Winners broadcast to attentional selection system
- Implements selective attention mechanism

### Prediction System Integration
- Memory patterns used as prediction context
- Episodic predictions stored in working memory
- Enables predictive coding

## Usage Examples

### Basic Memory Operations
```cpp
brain->getWorkingMemory()->store(pattern, strength);
std::vector<float> retrieved = brain->getWorkingMemory()->retrieve();
brain->getWorkingMemory()->storeToNeuron(neuron, activation);
```

### Memory Capacity Management
```cpp
brain->getWorkingMemory()->setCapacity(100);
size_t usage = brain->getWorkingMemory()->getMemoryNeuronCount();
```

### Memory State Inspection
```cpp
size_t capacity = brain->getWorkingMemory()->getWorkingMemoryCapacity();
size_t usage = brain->getWorkingMemory()->getWorkingMemoryUsage();
float activity = brain->getWorkingMemory()->getWorkingMemoryActivity();
```

## Performance Considerations

### Memory Size Impact
- Large patterns increase memory footprint
- High capacity affects retrieval speed
- Recurrent connections increase computation

### Activity Management
- Too much activation causes runaway excitation
- Proper decay prevents memory explosion
- Competition maintains stability

### Integration Overhead
- Working memory operations add processing steps
- Recurrent connections increase synaptic computation
- Memory retrieval has linear time complexity

## Future Enhancements

### 1. Advanced Memory Types
- **Short-term buffer**: Fast-access temporary storage
- **Long-term consolidation**: Transfer to episodic memory
- **Associative memory**: Pattern relationships

### 2. Memory Quality Metrics
- **Stability**: How long memory persists
- **Distinctiveness**: How well memory stands out
- **Accessibility**: How quickly memory can be used

### 3. Dynamic Capacity Adjustment
- **Load-based resizing**: Adapt to information density
- **Task-specific allocation**: Allocate memory for specific purposes
- **Energy-aware operation**: Balance memory with power consumption

## Testing and Validation

### Unit Tests
- Memory storage and retrieval accuracy
- Capacity enforcement and eviction
- Activity decay and competition dynamics
- Integration with sensory input

### Integration Tests
- Memory-attention interaction
- Memory-prediction integration
- Memory development over time
- Checkpoint and restore functionality