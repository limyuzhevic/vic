# NLM User Guide

## Tutorial 1: Simple Brain Simulation

### Learning Objectives

- Understand NLM's neural architecture
- Run a basic simulation and observe neural activity
- Learn about different brain regions and neuron types
- Explore spike generation and propagation

### Prerequisites

- NLM installed and built (see Getting Started Guide)
- Basic understanding of neural concepts (optional)

### Overview

This tutorial demonstrates NLM's core neural computation capabilities. You'll run a simple simulation that shows:
- How neurons integrate inputs and generate spikes
- How spikes propagate through neural networks
- The role of different neuron types (sensory, internal, motor)
- Basic learning mechanisms (STDP, Hebbian learning)

### Step 1: Running the Simulation

Open a terminal and run:

```bash
./nlm
```

### Expected Output Analysis

During execution, you'll see several sections of output:

#### 1. Brain Initialization
```
=== NLM Phase 2: Real Neural Computation ===
Initializing NLM Phase 2 Real Neural Computation...
```

**What this means:**
- Creating a simulated brain with 500 neurons
- Setting up 1 brain region
- Establishing random connectivity between neurons

#### 2. Test 1: Basic Neural Connectivity
```
=== Test 1: Basic Neural Connectivity ===
Inject current into 10 neurons...
Spikes generated: X
✓ Spikes propagate through network
```

**What you'll observe:**
- Current injected into sensory neurons
- Membrane potential rises above threshold
- Neurons generate action potentials (spikes)
- Spikes propagate through synaptic connections

#### 3. Test 2: Plasticity Learning Experiment
```
=== Test 2: Plasticity Learning Experiment ===
Applying repeated input patterns (1000 steps)...
Step 0 | Spikes: X | Firing: Y
Step 100 | Spikes: X | Firing: Y
...
=== Learning Experiment Results ===
Weight Statistics:
  Initial mean weight: 0.123
  Final mean weight: 0.156
  Change: 0.033
```

**What you'll observe:**
- Synaptic weights changing over time (learning)
- Hebbian learning strengthening connections
- STDP refining spike timing relationships
- Overall network excitability changing

#### 4. Test 3: STDP Verification
```
=== Test 3: STDP Verification ===
Testing STDP on 5 synapses:
  Synapse 0 before: 0.123
  Synapse 1 before: 0.145
  ...
After correlated activity:
  Synapse 0 after: 0.134 (+0.011)
  Synapse 1 after: 0.132 (+0.003)
  ...
✓ STDP WORKING: Pre-before-post produced potentiation
```

**What you'll observe:**
- Pre-synaptic spike followed by post-synaptic spike
- Synaptic weight increase (potentiation)
- Demonstrating spike-timing-dependent plasticity

### Step 2: Understanding the Neural Network

#### Brain Structure

Your simulation creates:

```
Brain (1 region)
├── Population 1 (Sensory neurons, 125 neurons)
│   ├── Neuron 1-125: Receive external inputs
│   └── Record spike activity
├── Population 2 (Internal neurons, 250 neurons)
│   ├── Neuron 126-375: Process and integrate
│   └── Generate network activity
├── Population 3 (Motor neurons, 125 neurons)
│   ├── Neuron 376-500: Produce outputs
│   └── Control actions
```

#### Neuron Types

1. **Sensory Neurons (Type 0)**:
   - Receive external current inputs
   - Convert environmental signals to neural activity
   - Have lower firing thresholds

2. **Internal Neurons (Type 1)**:
   - Process synaptic inputs from other neurons
   - Integrate multiple inputs over time
   - Core computational units

3. **Motor Neurons (Type 2)**:
   - Produce outputs based on internal state
   - Control actions in the simulated environment
   - Have higher firing thresholds

#### Neural Dynamics

Each neuron follows the **Leaky Integrate-and-Fire (LIF)** model:

```
dV/dt = (V_rest - V)/τ + I/C
```

Where:
- `V` = Membrane potential (mV)
- `V_rest` = Resting potential (-70 mV)
- `τ` = Time constant (20 ms)
- `I` = Input current
- `C` = Membrane capacitance (1 nF)

**Key Events:**
1. Inputs summate over time
2. Potential reaches threshold (-55 mV)
3. Spike is generated
4. Potential resets and enters refractory period
5. Cycle repeats

### Step 3: Analyzing the Results

#### Understanding the Output

1. **Spike Count**: Total number of spikes generated
2. **Mean Weight**: Average synaptic strength in the network
3. **STDP Results**: Whether pre-before-post pairing produced potentiation
4. **Learning Detection**: Whether significant learning occurred

#### Key Metrics to Observe

1. **Connectivity Test**: Do spikes propagate through the network?
2. **Learning Effectiveness**: Did synaptic weights change significantly?
3. **STDP Correctness**: Did pre-before-post produce potentiation?
4. **Network Stability**: Did the network settle into a stable state?

### Step 4: Exploring Configuration Options

Try different configurations to understand parameter effects:

#### Smaller Network
```bash
./nlm --neurons 100 --timestep 0.001
```

#### Larger Network
```bash
./nlm --neurons 2000 --stdp-ltp 0.01 --stdp-ltd 0.012
```

#### Enhanced Plasticity
```bash
./nlm --neurons 1000 --hebbian-learning 0.02 --plasticity-mod 0.8
```

### Step 5: Extending the Tutorial

#### Custom Configuration File

Create `custom_config.cfg`:

```json
{
  "simulation": {
    "neuron-count": 800,
    "timestep": 0.001,
    "connection-probability": 0.15
  },
  "plasticity": {
    "stdp": {
      "ltp-weight": 0.025,
      "ltd-weight": 0.01
    }
  }
}
```

Run with custom config:
```bash
./nlm --config custom_config.cfg
```

#### Save and Load Brain States

Save your trained brain:
```bash
./nlm --save-brain trained_brain.nlm
```

Load later:
```bash
./nlm --load-brain trained_brain.nlm
```

## Tutorial 2: Learning Experiment

### Learning Objectives

- Understand how NLM learns through experience
- Explore plasticity mechanisms (STDP, Hebbian learning)
- Observe development effects on learning
- Create and run custom learning experiments

### Prerequisites

- Basic understanding from Tutorial 1
- Familiarity with command-line options

### Overview

This tutorial focuses on NLM's learning capabilities. You'll explore:
- How synaptic weights change through experience
- The role of neuromodulation in learning
- Developmental stages and their effect on plasticity
- Creating effective learning environments

### Step 1: Running a Learning Experiment

Run with enhanced plasticity:

```bash
./nlm --neurons 1500 --stdp-ltp 0.02 --stdp-ltd 0.015 \
     --hebbian-learning 0.01 --development-stage critical \
     --plasticity-mod 0.8
```

### Expected Output Analysis

#### Initial State
```
Initial state recorded:
  Synapses: 562
  Mean weight: 0.125
```

#### Learning Progress (every 100 steps)
```
Step 0 | Spikes: 15 | Firing: 8
Step 100 | Spikes: 45 | Firing: 23
Step 200 | Spikes: 78 | Firing: 41
Step 300 | Spikes: 92 | Firing: 48
```

#### Final Results
```
Learning Experiment Results:
Weight Statistics:
  Initial mean weight: 0.125
  Final mean weight: 0.187
  Change: 0.062

Synaptic Changes:
  Strengthened: 234
  Weakened: 89
  Unchanged: 239

✓ LEARNING DETECTED: Synaptic weights changed through experience
```

### Step 2: Understanding Learning Mechanisms

#### Two Main Learning Mechanisms

1. **STDP (Spike-Timing Dependent Plasticity)**:
   - Pairs spike timing to adjust synaptic strength
   - Pre-before-post → Potentiation (increase weight)
   - Post-before-pre → Depression (decrease weight)
   - Effective range: ~20ms

2. **Hebbian Learning**:
   - Co-activity strengthens connections
   - "Neurons that fire together, wire together"
   - Fired simultaneously neurons increase weight

#### Neuromodulation Effects

Dopamine modulates learning:
- **Positive prediction error** → Increases plasticity
- **Negative prediction error** → Decreases plasticity
- **Homeostatic regulation** → Maintains stability

#### Developmental Modulation

Plasticity modifier changes with developmental stage:

| Stage | Plasticity Modifier | Learning Characteristics |
|-------|-------------------|-------------------------|
| Initial | 1.0 | High plasticity, rapid learning |
| Critical Period | 0.8 | Moderate plasticity, refined learning |
| Maturation | 0.5 | Low plasticity, stable learning |
| Adult | 0.2 | Very low plasticity, maintenance |

### Step 3: Customizing Learning Experiments

#### Experiment 1: High Plasticity

```bash
./nlm --neurons 1000 --plasticity-mod 1.5 \
     --development-stage initial --curiosity-factor 0.5
```

#### Experiment 2: Stable Learning

```bash
./nlm --neurons 1000 --plasticity-mod 0.3 \
     --development-stage adult --curiosity-factor 0.1
```

#### Experiment 3: Exploration vs Exploitation

```bash
./nlm --neurons 1000 --plasticity-mod 0.7 \
     --development-stage critical --curiosity-factor 0.4 \
     --novelty-threshold 0.15
```

### Step 4: Analyzing Learning Curves

#### Key Metrics

1. **Learning Rate**: Speed of synaptic weight changes
2. **Convergence**: How quickly learning stabilizes
3. **Exploration**: Curiosity-driven behavior
4. **Memory Consolidation**: Long-term retention of important experiences

#### Visualizing Learning

Watch how the network evolves:

- **Early Phase**: Rapid weight changes, high variability
- **Middle Phase**: Gradual refinement, stable patterns
- **Late Phase**: Fine-tuning, oscillation around equilibrium

### Step 5: Extending Learning Experiments

#### Multi-Session Learning

```bash
# Session 1: Initial learning
./nlm --save-brain session1_brain.nlm

# Session 2: Continued learning from saved state
./nlm --load-brain session1_brain.nlm --neurons 1000

# Session 3: Advanced learning with new inputs
./nlm --load-brain session2_brain.nlm --neurons 1500
```

#### Transfer Learning

Train on one task, test on another:

```bash
# Task 1: Visual pattern recognition
./nlm --neurons 1000 --stdp-ltp 0.02 --stdp-ltd 0.015

# Save learned patterns
save_task1_patterns.bat

# Task 2: Different patterns, same neural substrate
./nlm --load-brain trained_brain.nlm --neurons 1000 --stdp-ltp 0.01
```

## Tutorial 3: STDP Verification

### Learning Objectives

- Understand spike-timing dependent plasticity (STDP)
- Verify STDP mechanisms work correctly
- Explore the relationship between spike timing and synaptic change
- Test different STDP parameters

### Prerequisites

- Understanding of neural spikes and timing
- Basic familiarity with synaptic plasticity

### Overview

STDP is a fundamental learning mechanism where the precise timing of pre- and post-synaptic spikes determines whether a synapse strengthens (potentiates) or weakens (depresses). This tutorial verifies that NLM's STDP implementation works correctly.

### Step 1: Running STDP Verification

Run the built-in STDP verification:

```bash
./nlm --neurons 500 --stdp-tau 20
```

### Expected Output

#### Pre-Activity
```
Testing STDP on 5 synapses:
  Synapse 0 before: 0.123
  Synapse 1 before: 0.145
  Synapse 2 before: 0.118
  Synapse 3 before: 0.132
  Synapse 4 before: 0.126
```

#### Correlated Activity
```
Creating correlated pre->post activity (potentiation)...
Step 0: Pre fires, Post waits
Step 1: Post fires
Step 2: Pre fires, Post waits
Step 3: Post fires
...
```

#### Post-Activity
```
After correlated activity:
  Synapse 0 after: 0.134 (Δ=+0.011)
  Synapse 1 after: 0.132 (+0.003)
  Synapse 2 after: 0.119 (+0.001)
  Synapse 3 after: 0.135 (+0.003)
  Synapse 4 after: 0.128 (+0.002)
```

#### Verification Results
```
✓ STDP WORKING: Pre-before-post produced potentiation
Total weight change: +0.020
```

### Step 2: Understanding STDP Mechanics

#### STDP Rule Implementation

NLM implements the classic STDP rule:

```
if pre fires BEFORE post:
    Δweight = +A_plus * exp(-Δt/τ_plus)
elif post fires BEFORE pre:
    Δweight = -A_minus * exp(-Δt/τ_minus)
else:
    Δweight = 0
```

Where:
- `Δt` = Time difference between spikes (positive = pre before post)
- `A_plus` = LTP magnitude (0.02 in default config)
- `A_minus` = LTD magnitude (0.015 in default config)
- `τ_plus` = LTP time constant (20ms)
- `τ_minus` = LTD time constant (20ms)

#### Biological Interpretation

- **Pre-before-post**: Hebbian learning, strengthen connection
- **Post-before-pre**: Anti-Hebbian learning, weaken connection
- **Simultaneous**: No change

### Step 3: Testing Different STDP Scenarios

#### Scenario 1: Strong Potentiation

```bash
./nlm --neurons 500 --stdp-ltp 0.05 --stdp-ltd 0.01 --stdp-tau 30
```

**Expected:** Larger weight changes, longer time window

#### Scenario 2: Balanced Learning

```bash
./nlm --neurons 500 --stdp-ltp 0.015 --stdp-ltd 0.015 --stdp-tau 15
```

**Expected:** Moderate weight changes, shorter time window

#### Scenario 3: Depression Dominated

```bash
./nlm --neurons 500 --stdp-ltp 0.01 --stdp-ltd 0.03 --stdp-tau 10
```

**Expected:** Net weight decrease, rapid depression

### Step 4: Analyzing STDP Results

#### Key Measurements

1. **Δt Range**: Effective time window for STDP
2. **Weight Change**: Magnitude of synaptic modification
3. **Plasticity Direction**: Potentiation vs. depression
4. **Stability**: Whether changes converge or oscillate

#### Expected Behavior

For pre-before-post pairing:
- Weight should increase (potentiation)
- Larger Δt → smaller effect (exponential decay)
- Effect should saturate at maximum weight

### Step 5: Advanced STDP Analysis

#### Spike Timing Precision

Test with precise timing:
```bash
# Create controlled spike timing
# (Would need custom implementation for precise control)
```

#### Multiple Spike Trains

Test with different firing patterns:
- **Regular firing**: Consistent potentiation/depression
- **Burst firing**: Stronger effects from bursts
- **Irregular firing**: Variable effects based on pattern

#### Integration with Other Plasticity Rules

Test STDP with Hebbian learning:
```bash
# Both STDP and Hebbian active
./nlm --neurons 1000 --stdp-ltp 0.02 --hebbian-learning 0.01
```

## Tutorial 4: Agent Integration

### Learning Objectives

- Understand the AgentBrain system
- Explore sensory processing and motor control
- Learn about agent-environment interaction
- Create agents with behaviors

### Prerequisites

- Basic NLM understanding
- Familiarity with brain simulation concepts

### Overview

This tutorial explores NLM's Agent system, which connects the neural brain to a simulated world. You'll learn how perception, action, and learning work together in an embodied agent.

### Step 1: Running Agent Demo

Run the agent-based learning demonstration:

```bash
./nlm_phase3_demo
```

### Expected Output

#### Agent Initialization
```
AgentBrain initialized with 144 vision sensory neurons, 36 touch sensory neurons, 12 internal sensory neurons
```

#### Environment Interaction
```
Step 0 | Vision: [0.1, 0.2, ...] | Action: MoveForward | Reward: 0.0
Step 1 | Vision: [0.15, 0.25, ...] | Action: TurnRight | Reward: 0.1
Step 2 | Vision: [0.2, 0.3, ...] | Action: Interact | Reward: 1.0
```

#### Learning Progress
```
Step 0: Curiosity=0.1, Dopamine=0.0, Novelty=0.05
Step 100: Curiosity=0.3, Dopamine=0.2, Novelty=0.15
Step 500: Curiosity=0.8, Dopamine=0.6, Novelty=0.4
Step 1000: Curiosity=0.9, Dopamine=0.9, Novelty=0.8
```

### Step 2: Understanding Agent Architecture

#### Agent Components

```
AgentBrain
├── Sensory Systems
│   ├── Vision (16×16 grid = 256 neurons)
│   ├── Touch (8 sensors)
│   ├── Internal (homeostatic signals)
│   └── Proprioception (body state)
├── Motor Systems
│   ├── MoveForward
│   ├── MoveBackward
│   ├── TurnLeft/TurnRight
│   ├── LookLeft/Right
│   ├── Interact
│   └── Wait
└── Neuromodulation
    ├── Dopamine (reward prediction error)
    ├── Curiosity (exploration drive)
    ├── Novelty (unpredictability)
    └── Prediction Error
```

#### Perception Pipeline

```
WORLD STATE
    ↓
SENSOR DATA
    ↓
AGENT BRAIN
    ↓
INTERNAL REPRESENTATION
    ↓
DECISION MAKING
    ↓
ACTION SELECTION
    ↓
WORLD CONSEQUENCE
    ↓
REWARD SIGNAL
    ↓
NEUROMODULATION
```

### Step 3: Sensory Processing

#### Vision System

256 vision neurons process a 16×16 grayscale image:
- Each neuron responds to specific visual features
- Distributed representation of visual scenes
- Sensitive to contrast and edges

#### Touch System

8 touch sensors detect collisions:
- Binary collision signals
- Learned patterns of contact
- Object boundary detection

#### Internal System

4 internal signals represent homeostatic needs:
- Energy level (hunger/thirst)
- Health status
- Developmental needs

### Step 4: Motor Control

#### Action Selection

Motor commands are selected based on neural activity:

1. **Activity Calculation**: Measure firing rates of motor neuron groups
2. **Max Selection**: Choose highest activity group
3. **Curiosity Modulation**: Add exploration when curiosity is high
4. **Action Execution**: Send motor commands to world

#### Motor Groups

- **MoveForward**: Actively pushing forward
- **MoveBackward**: Retreating from threat
- **TurnLeft/Right**: Steering behavior
- **LookLeft/Right**: Scanning environment
- **Interact**: Engaging with objects
- **Wait**: Pausing and observing

### Step 5: Learning and Adaptation

#### Curiosity-Driven Exploration

When novelty is high, agents explore more:
- Random actions increase with curiosity level
- Reduces sensory predictability
- Discovers new environmental features

#### Reward-Based Learning

Dopamine signals reward prediction error:
- **Unexpected reward**: High dopamine, increased learning
- **Expected reward**: Baseline dopamine, maintenance
- **Omitted reward**: Negative dopamine, decreased learning

#### Development Effects

Agent development follows neural development:
- **Initial**: High plasticity, rapid learning
- **Critical**: Fine-tuned behaviors, specialization
- **Adult**: Stable, efficient behavior patterns

### Step 6: Customizing Agent Behavior

#### Exploration Parameters

```bash
# High exploration
./nlm_phase3_demo --curiosity-factor 0.8 --novelty-threshold 0.2

# Low exploration  
./nlm_phase3_demo --curiosity-factor 0.1 --novelty-threshold 0.05

# Adaptive exploration
./nlm_phase3_demo --curiosity-factor 0.5 --novelty-threshold 0.1
```

#### Motor Skill Development

```bash
# Different motor capabilities
./nlm_phase3_demo --motor-gain 0.5    # Precise movements
./nlm_phase3_demo --motor-gain 1.0    # Normal movements
./nlm_phase3_demo --motor-gain 2.0    # Aggressive movements
```

## Tutorial 5: Development Stages

### Learning Objectives

- Understand NLM's developmental system
- Explore how development affects learning and behavior
- Compare different developmental trajectories
- Create developmental experiments

### Prerequisites

- Basic understanding of NLM brain components
- Familiarity with plasticity mechanisms

### Overview

This tutorial explores NLM's developmental system, which models how biological organisms change over time. You'll learn how developmental stages affect plasticity, learning, and behavior.

### Step 1: Understanding Developmental Stages

NLM implements 4 developmental stages:

#### Stage 1: Initial (0-60 steps)
- **Plasticity Modifier**: 1.0 (maximum plasticity)
- **Characteristics**: Rapid learning, exploratory behavior
- **Synaptogenesis**: High rate
- **Pruning**: Low rate

#### Stage 2: Critical Period (60-300 steps)
- **Plasticity Modifier**: 0.8 (high plasticity)
- **Characteristics**: Refined behaviors, specialization
- **Synaptogenesis**: Moderate rate
- **Pruning**: Moderate rate

#### Stage 3: Maturation (300-900 steps)
- **Plasticity Modifier**: 0.5 (low plasticity)
- **Characteristics**: Stable performance, fine-tuning
- **Synaptogenesis**: Low rate
- **Pruning**: High rate

#### Stage 4: Adult (900+ steps)
- **Plasticity Modifier**: 0.2 (minimal plasticity)
- **Characteristics**: Stable, efficient behavior
- **Synaptogenesis**: Very low rate
- **Pruning**: Maximum rate

### Step 2: Running Development Experiment

#### Basic Development Study

```bash
./nlm_phase4_demo
```

**Expected Output:**
- Development progression monitoring
- Plasticity rate changes over time
- Behavioral refinement demonstration
- Long-term memory consolidation

#### Detailed Development Analysis

```bash
./nlm_phase4_demo --development-stage detailed --timing --performance-metrics
```

### Step 3: Analyzing Developmental Progression

#### Key Metrics to Track

1. **Plasticity Rate**: How quickly synapses change over time
2. **Behavior Refinement**: How actions become more efficient
3. **Memory Consolidation**: How important experiences become permanent
4. **Specialization**: How different neuron groups become specialized

#### Expected Development Patterns

**Learning Trajectory:**
```
Stage 1 (Initial):  
  High learning rate (0.8-1.0)
  Many synaptic changes
  Rapid behavior acquisition

Stage 2 (Critical):
  Moderate learning rate (0.5-0.8)
  Selective synaptic changes  
  Behavior refinement

Stage 3 (Maturation):
  Low learning rate (0.2-0.5)
  Fine-tuning of connections
  Performance optimization

Stage 4 (Adult):
  Minimal learning rate (0.0-0.2)
  Maintenance of established behaviors
  Stability and robustness
```

### Step 4: Developmental Experiments

#### Experiment 1: Accelerated Development

```bash
./nlm_phase4_demo --plasticity-mod 1.5 --development-stage fast \
                --synaptogenesis-rate 0.0002 --pruning-rate 0.00005
```

**Expected:** Faster development, more stable adult behavior

#### Experiment 2: Delayed Development

```bash
./nlm_phase4_demo --plasticity-mod 0.5 --development-stage slow \
                --synaptogenesis-rate 0.00005 --pruning-rate 0.00001
```

**Expected:** Slower development, extended learning period

#### Experiment 3: Abnormal Development

```bash
./nlm_phase4_demo --plasticity-mod 0.3 --development-stage abnormal \
                --synaptogenesis-rate 0.00015 --pruning-rate 0.00015
```

**Expected:** Possible learning difficulties, unstable behavior

### Step 5: Developmental Mechanisms

#### Synaptogenesis (New Synapse Formation)

```
Formula: rate = base_rate × plasticity_modifier

Stage 1: 0.0001 × 1.0 = 0.0001 per step
Stage 4: 0.0001 × 0.2 = 0.00002 per step
```

#### Pruning (Synapse Elimination)

```
Formula: rate = base_rate × (2.0 - plasticity_modifier)

Stage 1: 0.00001 × 1.8 = 0.000018 per step
Stage 4: 0.00001 × 1.8 = 0.000018 per step
```

#### Plasticity Modulation

```
Effect on learning: weight_change × plasticity_modifier

Stage 1: weight_change × 1.0 = full learning effect
Stage 4: weight_change × 0.2 = 20% learning effect
```

### Step 6: Applications of Developmental Studies

#### Education Research
```bash
# Studying optimal learning periods
./nlm_phase4_demo --development-stage educational --critical-period 100
```

#### Therapy Research
```bash
# Studying developmental disorders
./nlm_phase4_demo --development-stage abnormal --plasticity-mod 0.4
```

#### AI Development
```bash
# Creating developmental AI systems
./nlm_phase4_demo --development-stage progressive --lifelong-learning
```

## Additional Resources

### Code Examples

See the `examples/` directory for complete working examples:

1. **Basic Simulation**: `examples/basic_simulation.cpp`
2. **Learning Demo**: `examples/learning_demo.cpp`
3. **Development Demo**: `examples/development_demo.cpp`
4. **Agent Behavior**: `examples/agent_behavior.cpp`
5. **Experiment Framework**: `examples/experiment_framework.cpp`
6. **Performance Benchmark**: `examples/performance_benchmark.cpp`

### Documentation

- **Architecture**: `docs/ARCHITECTURE.md`
- **Science**: `docs/SCIENCE.md`
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md`
- **Roadmap**: `docs/ROADMAP.md`

### Community Resources

- **GitHub**: Code repository and issues
- **Documentation**: Detailed API and usage guides
- **Forums**: User discussions and support
- **Examples**: Additional working examples and demos

### Next Steps

1. **Complete all 5 tutorials**: Build comprehensive understanding
2. **Try custom configurations**: Experiment with different parameters
3. **Create your own experiments**: Apply NLM to research questions
4. **Extend the framework**: Add new components and capabilities
5. **Contribute back**: Share improvements with the community

---

**Tutorial Complete!** 🎉

You now have hands-on experience with NLM's core capabilities:
- ✅ Basic neural simulation and spike dynamics
- ✅ Learning through plasticity (STDP, Hebbian)
- ✅ Agent perception and action
- ✅ Developmental behavior progression
- ✅ Neuromodulation effects (curiosity, reward)

**Ready for more advanced topics?**

Check out the [API Reference](./api_reference/) for detailed class documentation and the [Developer Guide](./developer_guide/) for extending NLM.