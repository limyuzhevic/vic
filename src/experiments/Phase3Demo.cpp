# NLM Phase 3 Integration Demo

## Phase 3 Integration Demo

**Phase 3: World Interaction Loop**

This demo showcases the Phase 3 integration between the NLM brain and the simulated world. It demonstrates how the brain learns through interaction with an environment.

### Features Demonstrated:

- **World-Environment Interaction**: Brain perceives world state and produces actions
- **Sensory Processing**: Vision and motor neuron integration
- **Action Selection**: Motor commands based on neural activity
- **Reward-Based Learning**: Dopaminergic modulation of synaptic weights
- **Structural Plasticity**: Growing and pruning synaptic connections
- **Development**: Brain maturation over time

### Demo Output:

The demo will show:
- Total spikes and neural activity
- Reward accumulation and learning
- Synaptic weight changes over time
- Developmental progression
- Memory formation and consolidation

### Running the Demo:

```bash
# From the build directory
./nlm_phase3_demo

# Or run directly with CMake
mkdir build
cd build
cmake ..
make -j4
./nlm_phase3_demo
```

### Technical Details:

This demo integrates the following systems:

1. **SimpleWorld**: 2D grid-based environment with objects and resources
2. **AgentBrain**: Interface connecting brain to world perception and action
3. **NLM Brain**: Real spiking neural network with LIF neurons
4. **Neuromodulation**: Dopamine, curiosity, and novelty systems
5. **Plasticity**: STDP and Hebbian learning rules
6. **Development**: Synaptogenesis and pruning over time

### What You'll See:

- **Initial State**: Sparse connectivity and low activity
- **Early Learning**: Rapid synaptic strengthening through reward
- **Middle Phase**: Stable behavior with memory formation
- **Late Development**: Structural reorganization and optimization

### Learning Outcomes:

This demo illustrates how:
- Neural systems can learn from environmental interaction
- Reward signals guide synaptic modification
- Structural changes support adaptive behavior
- Memory systems consolidate experience
- Development enables lifelong learning

### Comparison with Other Phases:

- **Phase 2**: Basic neural computation (no environment)
- **Phase 3**: Integration with environment and learning
- **Phase 4**: Complex cognition and planning
- **Phase 6**: Full integration with all systems

This Phase 3 demo provides the foundation for understanding how biological brains interact with their environment to learn and adapt.
