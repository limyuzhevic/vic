# NLM — 熙然

**An Experimental Artificial Developmental Brain**

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Current Status (2026-10-31)

**Phase 2: Real Neural Computation** - This is the production version with real spiking neural computation including:
- LIF neuron dynamics with realistic membrane behavior
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Developmental stages affecting learning rates

**Phase 6: FINAL INTEGRATION** - ALL SYSTEMS NOW WORKING:
- ✅ Memory Systems: Working memory, episodic memory, associative memory
- ✅ Neuromodulation: Dopamine, curiosity, novelty detection
- ✅ Prediction System: Integrated with prediction error signals
- ✅ Cognition Systems: Neural planner, concept formation, attention
- ✅ Development Integration: Age-dependent plasticity
- ✅ Persistence: Checkpoint save/load working
- ✅ Replay and Consolidation: Memory replay functionality

**Key Achievements:**
- ~37k synapses created for 500-neuron brain (Phase 2)
- Spikes propagate: ~8k spikes in Test 1, ~199k spikes over 1000 steps in Test 2
- STDP potentiates synapses (0.35 -> 0.99)
- Inhibitory neurons (20% of population) with stable plasticity
- Checkpoint save/load now functional (Phase 6)

**Remaining Challenges:**
- Phase 3/6 demos report 0 reward in behavioral tests
- Activity remains high (~200 spikes/step)
- Learning mostly saturation, not selective pattern learning

## Quick Start

### Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Run Demos
```bash
./nlm              # Phase 2: Basic neural computation
./nlm_phase3_demo  # Phase 3: World interaction
./nlm_phase4_demo  # Phase 4: Cognitive integration
./nlm_phase5_demo  # Phase 5: Performance optimizations
./nlm_phase6_demo  # Phase 6: Final integration
./nlm_benchmark    # Performance benchmark
```

### Python Bindings
```bash
# Build with Python bindings
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4

# Install Python module
cd python
pip install -e .
```

### Testing
```bash
mkdir build_test
cd build_test
cmake .. -DCMAKE_BUILD_TYPE=Debug
make nlm_test
./nlm_test
```

## Configuration

Edit `configs/default.cfg` for simulation parameters:

```toml
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1  # (FIXED: duplicate line removed)
environment_name = GridWorld
log_level = INFO
```

Override via command line:
```bash
./nlm --neuron_count=500 --region_count=1 --connection_probability=0.15
```

## Project Structure

```
src/
├── core/           # Core utilities (Config, Random, Logger, SimulationClock)
├── brain/          # Neural components (Neuron, Synapse, NeuralPopulation, NeuralRegion, Brain)
├── dynamics/       # Neural dynamics (NeuralDynamics, SpikeSystem)
├── plasticity/     # Plasticity rules (PlasticityRule, Hebbian, STDP, StructuralPlasticity)
├── development/    # Developmental system (DevelopmentSystem, Synaptogenesis, Pruning, Maturation)
├── neuromodulation/ # Neuromodulators (Dopamine, Reward, PredictionError, Novelty, Curiosity)
├── memory/        # Memory systems (WorkingMemory, EpisodicMemory, NeuralAssociativeMemory)
├── prediction/     # Prediction systems (NeuralPrediction, ActionConsequencePredictor)
├── cognition/     # Cognitive mechanisms (AttentionalSelection, NeuralPlanner, ConceptFormation)
├── sensory/        # Sensory processing (SensoryInput, Vision, Audio, InternalSignals)
├── motor/         # Motor system (Action, MotorSystem)
├── environment/   # Environment interface (Environment, Observation)
├── agent/         # Agent-brain interface (SensoryPercept, AgentBrain)
├── world/         # Simple world simulation
├── experiments/   # Experiment framework (Phase 3-6)
└── visualization/ # Visualization
    └── performance/ # Performance optimizations (Checkpoints, pools, queues)
tests/             # Unit tests
configs/           # Configuration files
python/            # Python bindings
docs/              # Documentation
```

## Documentation

- `docs/ARCHITECTURE.md` - System architecture
- `docs/SCIENCE.md` - Scientific background
- `docs/ROADMAP.md` - Development roadmap
- `docs/EXPERIMENTS.md` - Experimental protocols
- `docs/LIMITATIONS.md` - Known limitations
- `docs/PHASE6_FINAL_AUDIT.md` - Final integration audit
- `easy_usage.md` - Quick start guide
- `HOW_TO_USE.md` - Comprehensive manual

## Scientific Innovation

**Phase 2: Real Neural Computation**
- LIF neuron dynamics with realistic membrane properties
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

**Phase 6: Final Integration**
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

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

## Scientific Limitations

**NLM is a research project**, not a complete brain model:

- Simplified LIF neuron model (not biologically detailed)
- No realistic ion channel dynamics or dendritic morphology
- No detailed cortical architecture
- No claims of consciousness, intelligence, or human-like cognition
- Limited by available computing resources

## Getting Help

- **For beginners**: Read `easy_usage.md`
- **For researchers**: Read `docs/SCIENCE.md`
- **For developers**: Read `docs/ARCHITECTURE.md`
- **For users**: Read `HOW_TO_USE.md`
- **Issues**: GitHub repository

## Contributing

Focus on:
- Scientific validation of neural models
- Improving efficiency and scalability
- Adding new neural mechanisms
- Better visualization and analysis tools
- Integration with experimental data

## License

MIT

## Authors

Research project — See docs for scientific background.
