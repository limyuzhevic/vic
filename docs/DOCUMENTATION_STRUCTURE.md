# NLM - Documentation Structure

## Overview

This document provides the structure for NLM documentation, ensuring a clear learning pathway for users of all experience levels.

## Documentation Hierarchy

```
NLM/
├── README.md                    # Project overview and quick start
├── docs/
│   ├── STRUCTURE.md            # System architecture
│   ├── SCIENCE.md              # Scientific background
│   ├── INSTALLATION.md          # Build and installation guide
│   ├── USAGE/
│   │   ├── BASIC.md            # Beginner tutorial
│   │   ├── INTERMEDIATE.md      # Intermediate usage
│   │   └── ADVANCED.md          # Advanced features
│   ├── API_REFERENCE/
│   │   ├── BRAIN.md             # Brain class API
│   │   ├── CONFIG.md            # Configuration API
│   │   ├── AGENT.md             # AgentBrain API
│   │   └── WORLD.md             # World API
│   ├── TUTORIALS/
│   │   ├── NEURON_MODELS.md     # Understanding neuron types
│   │   ├── PLASTICITY.md        # Learning mechanisms
│   │   ├── MEMORY.md            # Memory systems
│   │   └── BEHAVIOR.md          # Behavior patterns
│   └── EXAMPLES/
│       ├── SIMPLE.md            # Simple examples
│       ├── COMPLETE.md          # Complete simulations
│       └── ADVANCED.md          # Advanced examples
├── examples/
│   ├── basic_brain.py          # Basic usage example
│   ├── agent_world.py          # Agent-environment interaction
│   ├── learning_loop.py        # Complete learning example
│   └── debug_analysis.py       # Analysis example
└── scripts/
    ├── build.py               # Build automation
    ├── install.py              # Installation helpers
    ├── test.py                 # Test runner
    ├── benchmark.py            # Performance benchmarking
    └── visualize.py            # Visualization tools
```

## Document Purpose

### README.md
**Purpose**: High-level overview and quick entry point
**Content**:
- Project description and vision
- Quick installation instructions
- Basic usage example (3 lines max)
- Key features summary
- Links to detailed documentation

### docs/USAGE/BASIC.md
**Purpose**: Complete beginner tutorial
**Structure**:
1. What is NLM? (1 paragraph)
2. Install and setup (copy-paste commands)
3. Three essential examples:
   - Silent brain (just thinking)
   - Brain with world (sensing and acting)
   - Complete agent (learning and development)
4. Common patterns and pitfalls
5. Next steps for learning

### docs/USAGE/INTERMEDIATE.md
**Purpose**: Moving from beginner to intermediate user
**Content**:
- Configuration customization
- Multi-step simulations
- Visualization basics
- Performance tuning
- Experiment design

### docs/USAGE/ADVANCED.md
**Purpose**: Expert-level usage
**Content**:
- Neural architecture design
- Advanced plasticity mechanisms
- Memory system integration
- Complex agent behaviors
- Performance optimization
- Custom implementations

## API Reference Structure

### docs/API_REFERENCE/BRAIN.md
**Structure**:
- Brain class overview
- Core methods (grouped by function)
- Configuration integration
- Neural manipulation
- State management
- Event handling

### docs/API_REFERENCE/CONFIG.md
**Structure**:
- Configuration system overview
- Key-value access patterns
- File I/O operations
- Section management
- Validation and constraints

### docs/API_REFERENCE/AGENT.md
**Structure**:
- Agent architecture
- Sensory processing
- Motor control
- Learning mechanisms
- Development stages

### docs/API_REFERENCE/WORLD.md
**Structure**:
- World simulation basics
- Environment interaction
- Object management
- Physics simulation
- Visualization

## Tutorial Structure

### docs/TUTORIALS/NEURON_MODELS.md
**Focus**: Understanding different neuron types and their roles
- LIF neuron mechanics
- Excitatory/inhibitory balance
- Sensory vs motor neurons
- Modulatory pathways

### docs/TUTORIALS/PLASTICITY.md
**Focus**: Learning and adaptation mechanisms
- STDP basics
- Hebbian learning
- Structural plasticity
- Neuromodulation effects

### docs/TUTORIALS/MEMORY.md
**Focus**: Memory systems and their integration
- Working memory
- Episodic memory
- Semantic memory
- Consolidation processes

### docs/TUTORIALS/BEHAVIOR.md
**Focus**: From neural activity to behavior
- Action selection
- Motor control
- Decision making
- Goal-directed behavior

## Examples Structure

### examples/basic_brain.py
**Goal**: Demonstrate core neural simulation
```python
# Simple brain that thinks
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)

print(f"Brain simulated {brain.getTotalSpikeCount()} spikes")
```

### examples/agent_world.py
**Goal**: Show agent-environment interaction
```python
# Complete agent simulation
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run episode
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### examples/learning_loop.py
**Goal**: Complete learning scenario
```python
# Learning with reward and development
import pynlm

# Create and enable learning
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning episode
for step in range(1000):
    # Environment interaction
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learning from experience
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, agent.getPredictionError())
    agent.updateDevelopment(0.1)
    
    # Monitor progress
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Scripts Purpose

### scripts/build.py
**Purpose**: Simplified build process
- Automated CMake configuration
- Dependency management
- Multiple build options (debug, release, etc.)
- Cross-platform compatibility

### scripts/install.py
**Purpose**: Installation and setup
- Python package installation
- Library building
- Environment configuration
- Dependency validation

### scripts/test.py
**Purpose**: Testing framework
- Unit test runner
- Integration test execution
- Performance benchmarking
- Test result analysis

### scripts/benchmark.py
**Purpose**: Performance measurement
- Simulation timing
- Memory usage tracking
- Bottleneck identification
- Comparison between configurations

### scripts/visualize.py
**Purpose**: Visualization tools
- Activity plotting
- Neural connectivity visualization
- Simulation recording
- Output formatting

## Cross-Reference Strategy

### Usage References
- **README.md** → Links to `docs/USAGE/BASIC.md`
- **BASIC.md** → Links to `docs/API_REFERENCE/` sections
- **INTERMEDIATE.md** → Links to `docs/TUTORIALS/` topics
- **ADVANCED.md** → Links to `docs/EXAMPLES/` complex examples

### API References
- **All API docs** → Link to relevant examples in `examples/`
- **Configuration docs** → Link to `docs/TUTORIALS/` sections
- **Tutorial docs** → Link to `docs/API_REFERENCE/` for implementation details

### Technical References
- **SCIENCE.md** → Referenced by all tutorial and advanced docs
- **STRUCTURE.md** → Background for architecture discussions
- **ARCHITECTURE.md** (in docs/) → Referenced by API documentation

## Maintenance Guidelines

### Adding New Content
1. **New tutorials**: Add to `docs/TUTORIALS/`
2. **New examples**: Add to `examples/` with clear comments
3. **API changes**: Update corresponding API reference
4. **Bug fixes**: Update relevant documentation

### Updating Documentation
- **Version changes**: Update relevant sections
- **API changes**: Update API reference documentation
- **Examples**: Test and update examples
- **Tutorials**: Refresh if content becomes outdated

### Validation
- Cross-reference checker: Ensure all links work
- Example validation: Run examples to verify they work
- API consistency: Ensure API documentation matches actual API
- Tutorial testing: Verify tutorial examples execute correctly

## Implementation Priority

**Phase 1** (Week 1-2):
1. Create `docs/STRUCTURE.md`
2. Create `docs/USAGE/BASIC.md`
3. Update `README.md` with simplified overview
4. Create `examples/basic_brain.py`

**Phase 2** (Week 3-4):
1. Create `docs/USAGE/INTERMEDIATE.md`
2. Create `docs/API_REFERENCE/` structure
3. Create `docs/TUTORIALS/` structure
4. Create `scripts/` directory with basic tools

**Phase 3** (Month 2):
1. Create `docs/USAGE/ADVANCED.md`
2. Complete `docs/TUTORIALS/` content
3. Create `docs/EXAMPLES/` detailed examples
4. Implement `scripts/` tools

This documentation structure ensures:
- Clear learning pathway for all user levels
- Consistent formatting and information organization
- Easy maintenance and updates
- Comprehensive coverage of all features
- Strong connection between theory and practice
