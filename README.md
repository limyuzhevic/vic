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

## CLI Commands

NLM provides a comprehensive command-line interface for controlling the neural simulation and exploring advanced features.

### Core Commands

#### `nlm_cli.py`
The main command-line interface for NLM operations:

**Basic Brain Operations:**
- `nlm_cli.py --help` - Show all available commands
- `nlm_cli.py --create --name mybrain --neurons 1000` - Create a new brain configuration
- `nlm_cli.py --brain mybrain --initialize` - Initialize a brain with saved configuration
- `nlm_cli.py --brain mybrain --reset` - Reset brain state
- `nlm_cli.py --brain mybrain --step 100` - Run simulation steps

**Memory Operations:**
- `nlm_cli.py --brain mybrain --memory status` - Check memory system status
- `nlm_cli.py --brain mybrain --memory episodes list` - List stored episodes
- `nlm_cli.py --brain mybrain --memory working` - Show working memory contents
- `nlm_cli.py --brain mybrain --memory consolidate 0.5` - Consolidate memories
- `nlm_cli.py --brain mybrain --memory replay 10` - Replay specific episode

**Neuromodulation Control:**
- `nlm_cli.py --brain mybrain --neuromod dopamine 0.5` - Set dopamine level
- `nlm_cli.py --brain mybrain --neuromod curiosity 0.8` - Set curiosity level
- `nlm_cli.py --brain mybrain --neuromod novelty true` - Enable novelty detection
- `nlm_cli.py --brain mybrain --neuromod status` - Show neuromodulation status

**Development Control:**
- `nlm_cli.py --brain mybrain --dev accelerate` - Speed up development
- `nlm_cli.py --brain mybrain --dev stage <stage>` - Set developmental stage
- `nlm_cli.py --brain mybrain --dev status` - Show developmental status

**Prediction and Planning:**
- `nlm_cli.py --brain mybrain --predict enable` - Enable prediction system
- `nlm_cli.py --brain mybrain --predict error` - Show prediction error
- `nlm_cli.py --brain mybrain --plan create --sequence "move,interact,wait"` - Create action plan
- `nlm_cli.py --brain mybrain --plan evaluate --action move` - Evaluate action
- `nlm_cli.py --brain mybrain --plan status` - Show planning status

**Concept and Cognition:**
- `nlm_cli.py --brain mybrain --concept learn "resource=15"` - Learn concept
- `nlm_cli.py --brain mybrain --concept list` - List learned concepts
- `nlm_cli.py --brain mybrain --cognition analyze --pattern "resource"` - Analyze patterns
- `nlm_cli.py --brain mybrain --cognition status` - Show cognition status

**Checkpoint Operations:**
- `nlm_cli.py --brain mybrain --checkpoint save "save1.bin"` - Save brain state
- `nlm_cli.py --brain mybrain --checkpoint load "save1.bin"` - Load brain state
- `nlm_cli.py --brain mybrain --checkpoint status` - Show checkpoint status

**Experiment Operations:**
- `nlm_cli.py --brain mybrain --experiment phase6` - Run Phase 6 integration experiment
- `nlm_cli.py --brain mybrain --experiment status` - Show experiment status
- `nlm_cli.py --brain mybrain --experiment metrics` - Show experiment metrics

### Advanced Commands

#### Action Planning
- `nlm_cli.py --brain mybrain --action plan` - Generate action plan based on current state
- `nlm_cli.py --brain mybrain --action select --type explore` - Select action based on type
- `nlm_cli.py --brain mybrain --action evaluate --candidate "move"` - Evaluate action candidate

#### Memory Management
- `nlm_cli.py --brain mybrain --memory cleanup --threshold 0.1` - Clean up weak memories
- `nlm_cli.py --brain mybrain --memory import "file.bin"` - Import memories from file
- `nlm_cli.py --brain mybrain --memory export "file.bin"` - Export memories to file

#### Development Management
- `nlm_cli.py --brain mybrain --dev reset` - Reset to initial development stage
- `nlm_cli.py --brain mybrain --dev accelerate` - Speed up development rate
- `nlm_cli.py --brain mybrain --dev query --aspect excitability` - Query development aspect

#### World Interaction
- `nlm_cli.py --world configure --width 50 --height 50` - Configure world dimensions
- `nlm_cli.py --world set-agent --x 25 --y 25` - Set agent starting position
- `nlm_cli.py --world objects add --type resource --value 50` - Add world object

#### Simulation Control
- `nlm_cli.py --brain mybrain --simulation speed 2.0` - Set simulation speed
- `nlm_cli.py --brain mybrain --simulation pause` - Pause simulation
- `nlm_cli.py --brain mybrain --simulation resume` - Resume simulation
- `nlm_cli.py --brain mybrain --simulation status` - Show simulation status

### Scripting and Automation

NLM CLI supports scripting for automated experiments:

**Example Script (`experiment.sh`):**
```bash
#!/bin/bash
# Run NLM exploration experiment

# Initialize brain
nlm_cli.py --create --name explorer --neurons 500 --regions 2 --connections 0.2

# Configure neuromodulation
nlm_cli.py --brain explorer --neuromod dopamine 0.3 --neuromod curiosity 0.8

# Run exploration
for i in {1..100}; do
    nlm_cli.py --brain explorer --step 10
    if [ $i -eq 50 ]; then
        nlm_cli.py --brain explorer --memory consolidate 0.3
    fi
    if [ $i -eq 100 ]; then
        nlm_cli.py --brain explorer --checkpoint save "explorer_step_$i.bin"
    fi
done

# Show results
nlm_cli.py --brain explorer --memory status
nlm_cli.py --brain explorer --experiment metrics
```

**Interactive Mode:**
```bash
nlm_cli.py --brain mybrain --interactive
```

In interactive mode, you can:
- Type commands directly
- Get context-aware help
- Explore brain state
- Run experiments interactively

### Configuration and Customization

#### Configuration Files
- Default configuration: `configs/default.cfg`
- Custom configurations: Any `.cfg` file in `configs/` directory
- Configuration can be overridden via command line

#### Command Line Options
All commands support these common options:
- `--help` - Show command help
- `--verbose` - Enable verbose output
- `--quiet` - Suppress output
- `--config <file>` - Specify configuration file
- `--output <file>` - Specify output file
- `--format <format>` - Output format (text, json, csv)

#### Configuration Examples

**Basic Exploration:**
```bash
nlm_cli.py --create --name explorer --neurons 1000 --regions 2 \
    --connections 0.1 --stdp_ltp_weight 0.01 \
    --stdp_ltd_weight 0.012 --synaptogenesis_rate 0.0001
```

**Learning with Memory:**
```bash
nlm_cli.py --brain explorer --neuromod dopamine 0.5 --neuromod curiosity 0.9 \
    --world configure --width 30 --height 30 --world set-agent --x 15 --y 15
```

**Phase 6 Integration Test:**
```bash
nlm_cli.py --brain mybrain --experiment phase6 --steps 2000 \
    --enable-checkpoint --checkpoint-path "./checkpoints/experiment.bin"
```

### Python API Integration

The CLI can be used from Python for programmatic control:

```python
import subprocess
import json

# Run experiment via CLI
result = subprocess.run([
    'nlm_cli.py', '--brain', 'mybrain', '--experiment', 'phase6',
    '--steps', '1000', '--output', 'results.json'
], capture_output=True, text=True)

if result.returncode == 0:
    data = json.loads(result.stdout)
    print(f"Experiment completed: {data['total_reward']}")
```

## Testing and Validation

### Command Testing
```bash
# Test all commands
nlm_cli.py --test --all

# Test specific command categories
nlm_cli.py --test --memory
nlm_cli.py --test --neuromod
nlm_cli.py --test --cognition

# Test brain integration
nlm_cli.py --brain mybrain --test --integration
```

### Performance Testing
```bash
# Stress test brain with many neurons
nlm_cli.py --create --name stress --neurons 5000 --regions 5 \
    --connections 0.05 --test --performance

# Test memory systems
nlm_cli.py --brain stress --memory --stress --episodes 1000 --working 5000
```

## Scripting and Automation Examples

The following files provide examples of using NLM CLI for various tasks:

- `examples/explore.sh` - Basic exploration script
- `examples/learning.sh` - Learning and memory script
- `examples/integration.sh` - Phase 6 integration test
- `examples/benchmark.sh` - Performance benchmark
- `examples/visualization.py` - Visualization script

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
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
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
