# NLM - Easy Usage Guide for Beginners

**Note:** This guide focuses on the C++ NLM Phase 6 command-line interface. For Python bindings, see the NLM Python documentation.

## What is NLM?

NLM (熙然, "serene flow") is an experimental artificial developmental brain project. It simulates:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells  
- **Learning** = When connections get stronger or weaker based on experience
- **Memory** = The brain remembering patterns and episodes

This is a **research project** investigating whether computational systems can develop cognition through interaction with environments.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks, learns, and remembers
2. **World** - The environment the brain lives in (provides stimuli and rewards)
3. **Agent** - The bridge connecting brain to world (handles sensory input and motor output)

---

## Quick Start (Copy & Paste)

### Method 1: Using the Enhanced CLI (Recommended)

```bash
# Build the project first
mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4

# Run the Phase 6 integration demo
nlm-cli demo --quick

# Run full integration with custom settings
nlm-cli demo --full

# Run custom simulation
nlm-cli run --neurons=1000 --steps=5000 --checkpoint=./my_checkpoint
```

### Method 2: Direct Binary Execution

```bash
# Build the project
mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4

# Run Phase 6 integration demo (as documented in README)
./nlm_phase6_demo

# Run with custom configuration
./nlm_phase6_demo --neurons=2000 --steps=10000 --config=./myconfig.cfg
```

### Method 3: Python Interface

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
for i in range(100):
    brain.step(i)

print(f"Your brain created {brain.getTotalSpikeCount()} spikes!")
```

---

## Quick Analogy (C++ Interface)

| Real World | NLM Command Line |
|------------|------------------|
| See something | `./nlm_phase6_demo` |
| Your brain processes it | `./nlm_phase6_demo --integration-only` |
| You decide to act | `./nlm_phase6_demo --demo-mode` |
| You do the action | `./nlm_phase6_demo --checkpoint=./test` |
| Your brain learns | `./nlm_phase6_demo --load=./checkpoint` |

---

## The Simplest Possible Example

### Using the Enhanced CLI:

```bash
# Build first
mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4

# Run quick demo
nlm-cli demo --quick
```

### Command Line Help:

```bash
# Show help for main CLI
nlm-cli help

# Show help for run command
nlm-cli run --help

# Show help for demo command
nlm-cli demo --help

# Show system information
nlm-cli info
```

---

## Common Patterns

### Pattern 1: Run a Quick Demo

```bash
# Quick demo (50 neurons, 100 steps) - perfect for learning
nlm-cli demo --quick

# Full demo (500 neurons, 2000 steps) - comprehensive test
nlm-cli demo --full
```

### Pattern 2: Run a Full Simulation

```bash
# Custom simulation with checkpointing
nlm-cli run --neurons=1000 --steps=5000 --checkpoint=./simulation_checkpoint

# Load and continue simulation
nlm-cli run --load=./simulation_checkpoint --demo-mode
```

### Pattern 3: Performance Testing

```bash
# Benchmark performance
nlm-cli run --performance-test --neurons=2000

# Integration testing only
nlm-cli run --integration-only
```

### Pattern 4: Configuration Management

```bash
# List available configurations
nlm-cli config list

# Show default configuration
nlm-cli config show default.cfg

# Create custom configuration
nlm-cli config create myconfig.cfg

# Run with custom configuration
./nlm_phase6_demo --config=./myconfig.cfg
```

---

## What Each Command Does

### `nlm-cli` (Main Command)

Manages all NLM operations with subcommands:
- **`run`** - Run NLM simulation
- **`demo`** - Run Phase 6 integration demo
- **`info`** - Show system information
- **`config`** - Manage configuration files
- **`help`** - Show help

### `./nlm_phase6_demo` (Binary)

Phase 6 integration demo with the following options:
- `--quick` - Quick demo (50 neurons, 100 steps)
- `--full` - Full demo (500 neurons, 2000 steps)
- `--help` - Show help
- `--config=<file>` - Load configuration from file
- `--neurons=<count>` - Number of neurons
- `--steps=<count>` - Number of simulation steps
- `--checkpoint=<path>` - Save/load checkpoint
- `--load=<path>` - Load brain state from checkpoint
- `--no-checkpoint` - Disable checkpointing
- `--no-replay` - Disable replay system
- `--no-development` - Disable development plasticity
- `--verbose` - Enable verbose logging

### `./nlm` (Legacy Binary)

Phase 2 neural computation (legacy - for backward compatibility):
- `--help` - Show help
- `--config=<file>` - Load configuration
- `--neurons=<count>` - Number of neurons
- `--steps=<count>` - Number of steps

### Configuration Files

Files in `configs/` directory:
- **`default.cfg`** - Optimized for Phase 6 integration
- **`expert.cfg`** - Optimized for performance
- **`beginner.cfg`** - Optimized for learning
- **`performance.cfg`** - Optimized for benchmarking

---

## Mini Projects

### Mini Project 1: Quick Demo

```bash
# Run quick demo
nlm-cli demo --quick

# This will:
# - Create a brain with 50 neurons
# - Run 100 simulation steps
# - Test all brain systems integration
# - Show results in an easy-to-understand format
```

### Mini Project 2: Full Integration Test

```bash
# Run full demo
nlm-cli demo --full

# This will:
# - Create a brain with 500 neurons
# - Run 2000 simulation steps
# - Test all advanced features:
#   * Memory systems (working + episodic)
#   * Neuromodulation (dopamine, curiosity)
#   * Prediction system
#   * Development system
#   * Replay and consolidation
#   * Checkpoint save/load
# - Provide detailed performance metrics
```

### Mini Project 3: Custom Simulation

```bash
# Create custom simulation
nlm-cli run --neurons=1000 --steps=5000 --checkpoint=./my_checkpoint

# Features:
# - Custom neuron count
# - Custom simulation steps
# - Automatic checkpoint saving
# - Progress reporting
```

### Mini Project 4: Continue from Checkpoint

```bash
# Load and continue simulation
nlm-cli run --load=./my_checkpoint --demo-mode

# Continue from where you left off
# with full integration testing
```

---

## Quick Reference

| What you want | Command |
|--------------|--------|
| Run quick demo | `nlm-cli demo --quick` |
| Run full demo | `nlm-cli demo --full` |
| Custom simulation | `nlm-cli run --neurons=1000 --steps=5000` |
| Load checkpoint | `nlm-cli run --load=./checkpoint` |
| Save checkpoint | `nlm-cli run --checkpoint=./checkpoint` |
| System info | `nlm-cli info` |
| Configuration help | `nlm-cli config help` |
| Show configs | `nlm-cli config list` |

---

## Troubleshooting

**"The command is not found: nlm-cli"**

**Solution:**
```bash
# Make the CLI utility executable (one-time setup)
chmod +x nlm-cli

# Or run from the build directory
./build/nlm-cli demo --quick
```

**"Phase 6 demo not found!"**

**Solution:**
```bash
# Build the project
mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4

# Check if the binary exists
ls -la build/
```

**"Configuration file not found"**

**Solution:**
```bash
# Create new configuration
nlm-cli config create myconfig.cfg

# Or use default configuration
nlm-cli demo --quick
```

**"Help not showing up"**

**Solution:**
```bash
# Check CLI is executable
ls -la nlm-cli

# Make executable if needed
chmod +x nlm-cli

# Try again
nlm-cli help
```

---

## Getting Started Checklist

### One-Time Setup:
- [ ] Check if project is already built (look for `build/` directory)
- [ ] If not built: `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4`
- [ ] Make CLI executable: `chmod +x nlm-cli`

### First Run:
- [ ] Run quick demo: `nlm-cli demo --quick`
- [ ] Explore system information: `nlm-cli info`
- [ ] List configurations: `nlm-cli config list`
- [ ] Try full demo: `nlm-cli demo --full`

### Next Steps:
- [ ] Read `HOW_TO_USE.md` for detailed information
- [ ] Read `PHASE6_IMPROVEMENTS.md` for enhancement details
- [ ] Experiment with different configurations
- [ ] Create custom configurations using `nlm-cli config create`

That's it! You're now ready to use the NLM Phase 6 system.

**Remember:** This is a research project for studying artificial developmental brains. The results represent computational experiments, not claims about biological accuracy or consciousness.

---

*Last Updated: $(date +%Y-%m-%d)*
*NLM Project Team - Phase 6 Easy Usage Guide*
