# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 4 Demo (Basic Cognition)

```bash
# Basic usage - run with default settings
./nlm_phase4_demo

# Custom configuration with command line options
./nlm_phase4_demo \
    --trials 200 \
    --neurons 1500 \
    --regions 3 \
    --seed 123 \
    --verbose

# Load settings from configuration file
./nlm_phase4_demo --config my_settings.cfg
```

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Usage (pynlm)

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 100 simulation steps
for i in range(100):
    brain.step(i)

print(f"Brain completed {brain.getTotalSpikeCount()} spikes")
```

### Advanced Python Usage

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("random_seed", 123)
config.set("simulation_timestep", 0.0005)

# Create brain with custom settings
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world for complete simulation
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
├── tests/
└── configs/
```

## Usage Examples

### 1. Command Line Interface

```bash
# Show help information
./nlm_phase4_demo --help

# Show version information  
./nlm_phase4_demo --version

# Run with custom settings
./nlm_phase4_demo --trials 100 --neurons 1500 --regions 3 --seed 42 --verbose

# Load configuration from file
./nlm_phase4_demo --config custom_settings.cfg
```

### 2. Configuration File Example

Create `custom_settings.cfg`:

```
# NLM Configuration File
neuron_count = 1500
region_count = 3
random_seed = 42
simulation_timestep = 0.001
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Enable all features
enable_reward_modulation = true
enable_structural_plasticity = true
enable_development = true
enable_curiosity = true
```

### 3. Python Programmatic Configuration

```python
import pynlm

# Create brain with specific settings
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("random_seed", 12345)
config.set("connection_probability", 0.15)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(200):
    brain.step(step)
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### 4. Complete Agent Simulation

```python
import pynlm

# Setup with custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1500)
config.set("region_count", 3)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run complete simulation loop
print("Starting complete agent simulation...")
for step in range(300):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"reward: {agent.getNeuromodulationLevel():.3f}")

print("Simulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### 5. Experiment Runner Script

Save this as `run_experiments.py`:

```python
#!/usr/bin/env python3
import sys
import pynlm
import argparse

def parse_arguments():
    parser = argparse.ArgumentParser(
        description="Run NLM experiments with command-line configuration"
    )
    parser.add_argument("--trials", type=int, default=100,
                       help="Number of simulation steps (default: 100)")
    parser.add_argument("--neurons", type=int, default=1000,
                       help="Number of neurons in brain (default: 1000)")
    parser.add_argument("--regions", type=int, default=2,
                       help="Number of brain regions (default: 2)")
    parser.add_argument("--seed", type=int, default=42,
                       help="Random seed for reproducibility (default: 42)")
    parser.add_argument("--verbose", action="store_true",
                       help="Enable verbose logging output")
    parser.add_argument("--output", type=str, default="",
                       help="Output file for results (optional)")
    return parser.parse_args()

def main():
    args = parse_arguments()
    
    # Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", args.neurons)
    config.set("region_count", args.regions)
    config.set("random_seed", args.seed)
    
    print("=" * 60)
    print("NLM Experiment Runner")
    print("=" * 60)
    print(f"Configuration: {args.neurons} neurons, {args.regions} regions")
    print(f"Seed: {args.seed}, Trials: {args.trials}")
    print(f"Verbose: {args.verbose}")
    if args.output:
        print(f"Output: {args.output}")
    print("=" * 60)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    print(f"Starting simulation for {args.trials} steps...")
    for step in range(args.trials):
        brain.step(step)
        
        if args.verbose and step % 20 == 0:
            print(f"  Step {step:3d}: {brain.getFiringNeuronCount():3d} "
                  f"neurons firing, {brain.getTotalSpikeCount():4d} total spikes")
    
    # Print final statistics
    print("\n" + "=" * 60)
    print("Simulation Complete!")
    print("=" * 60)
    print(f"Total simulation steps: {args.trials}")
    print(f"Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes accumulated: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    
    if args.output:
        # Save results to file
        with open(args.output, 'w') as f:
            f.write(f"NLM Experiment Results\n")
            f.write(f"======================\n")
            f.write(f"Total steps: {args.trials}\n")
            f.write(f"Neurons: {args.neurons}\n")
            f.write(f"Regions: {args.regions}\n")
            f.write(f"Seed: {args.seed}\n")
            f.write(f"Final firing neurons: {brain.getFiringNeuronCount()}\n")
            f.write(f"Total spikes: {brain.getTotalSpikeCount()}\n")
            f.write(f"Average firing rate: {brain.getAverageFiringRate():.3f}\n")
        print(f"Results saved to: {args.output}")

if __name__ == "__main__":
    main()
```

Then run it with:

```bash
python run_experiments.py --trials 200 --neurons 1500 --regions 3 --seed 123 --verbose --output results.txt
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.

(End of file - total 847 lines)