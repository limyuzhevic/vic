# NLM CLI Reference

## Overview

This document provides comprehensive reference for all command-line interface (CLI) tools available in the NLM project. The CLI provides access to simulation tools, demos, and utilities for exploring and testing the NLM system.

## Available Commands

### Main Simulation Executable

**Executable:** `nlm` (built from `src/main.cpp`)

**Description:** Main simulation executable that runs a complete agent simulation with brain, world, and agent integration.

**Usage:**
```bash
nlm [options]
```

**Options:**
- `--help, -h` - Show help message and available options
- `--config <file>` - Load configuration from file
- `--neuron-count <n>` - Set number of neurons (default: 1000)
- `--world-size <width> <height>` - Set world dimensions
- `--steps <n>` - Number of simulation steps (default: 1000)
- `--seed <n>` - Random seed for reproducible simulations
- `--checkpoint <file>` - Save checkpoint to file
- `--load <file>` - Load brain state from checkpoint
- `--verbose, -v` - Enable verbose output

**Example:**
```bash
./nlm --neuron-count 500 --world-size 20 20 --steps 2000 --seed 42
```

### Phase 3 Demo

**Executable:** `nlm_phase3_demo`

**Description:** Phase 3 demonstration showing basic neural computation and world interaction.

**Usage:**
```bash
nlm_phase3_demo [options]
```

**Options:**
- `--help, -h` - Show help message
- `--neuron-count <n>` - Number of neurons (default: 1000)
- `--steps <n>` - Number of simulation steps (default: 100)
- `--seed <n>` - Random seed

**Example:**
```bash
./nlm_phase3_demo --neuron-count 500 --steps 200
```

### Phase 4 Demo

**Executable:** `nlm_phase4_demo`

**Description:** Phase 4 demonstration showing emerging cognitive capabilities including memory, prediction, and planning.

**Usage:**
```bash
nlm_phase4_demo [options]
```

**Options:**
- `--help, -h` - Show help message
- `--neuron-count <n>` - Number of neurons (default: 1000)
- `--steps <n>` - Number of simulation steps (default: 500)
- `--seed <n>` - Random seed
- `--enable-memory` - Enable memory systems test
- `--enable-prediction` - Enable prediction systems test

**Example:**
```bash
./nlm_phase4_demo --neuron-count 800 --steps 300 --enable-memory
```

### Phase 6 Integration Demo

**Executable:** `nlm_phase6_demo`

**Description:** Phase 6 integration test verifying all brain systems are connected and functional.

**Usage:**
```bash
nlm_phase6_demo [options]
```

**Options:**
- `--help, -h` - Show help message
- `--neuron-count <n>` - Number of neurons (default: 500)
- `--steps <n>` - Number of simulation steps (default: 2000)
- `--seed <n>` - Random seed
- `--enable-checkpoint` - Enable checkpoint save/load test
- `--enable-replay` - Enable memory replay test
- `--enable-development` - Enable development systems test

**Example:**
```bash
./nlm_phase6_demo --neuron-count 300 --steps 1000 --enable-checkpoint --enable-replay
```

**Demo Output Format:**
```
=== NLM Phase 6 Integration Demo ===
Testing the integrated artificial brain...

--- Integration Verification ---
[PASS/FAIL] Working memory integration
[PASS/FAIL] Episodic memory integration
[PASS/FAIL] Neuromodulation integration
[PASS/FAIL] Prediction integration
[PASS/FAIL] Development integration
[PASS/FAIL] Checkpointing

--- Individual System Tests ---
--- Memory Integration Test ---
--- Neuromodulation Integration Test ---
--- Checkpoint Test ---
--- Replay Test ---

--- Full Integration Experiment ---
=== FINAL RESULTS ===
Total reward: 0.123
Avg firing rate: 8.45
Episodes stored: 15
Dopamine level: 0.678

=== INTEGRATION STATUS ===
Working Memory: CONNECTED
Episodic Memory: DISCONNECTED
Neuromodulation: CONNECTED
Prediction: DISCONNECTED
Development: CONNECTED
Checkpointing: WORKING

Wall clock time: 12.34s
```

### Unit Tests

**Executable:** `nlm_test`

**Description:** Run unit tests for NLM core functionality.

**Usage:**
```bash
nlm_test [--verbose] [--filter <pattern>]
```

**Options:**
- `--verbose, -v` - Enable verbose test output
- `--filter <pattern>` - Run only tests matching pattern

**Example:**
```bash
./nlm_test --verbose
```

### Performance Benchmark

**Executable:** `benchmark`

**Description:** Run performance benchmarks for NLM simulation.

**Usage:**
```bash
benchmark [options]
```

**Options:**
- `--help, -h` - Show help message
- `--neuron-count <n>` - Number of neurons (default: 10000)
- `--steps <n>` - Number of simulation steps (default: 1000)
- `--iterations <n>` - Number of benchmark iterations (default: 5)

**Example:**
```bash
./benchmark --neuron-count 5000 --steps 500 --iterations 3
```

## Common CLI Patterns

### Running Different Demo Modes

```bash
# Quick neural computation demo
./nlm_phase3_demo

# Cognitive emergence demo
./nlm_phase4_demo --enable-memory --enable-prediction

# Full integration test
./nlm_phase6_demo --enable-checkpoint --enable-replay
```

### Running with Custom Configuration

```bash
# Use configuration file
cmake .. -DCONFIG_FILE=/path/to/config.json
make -j4
./nlm --config my_config.json
```

### Running Reproducible Experiments

```bash
# Same random seed for reproducibility
./nlm --seed 12345 --steps 1000

# Different world configurations
./nlm_phase4_demo --world-size 10 10 --seed 999
```

## Script Examples

### Shell Script: Run Phase 6 Demo with Monitoring

```bash
#!/bin/bash

# Phase 6 Integration Demo with Resource Monitoring

echo "Starting NLM Phase 6 Integration Demo..."
echo "====================================="

# Record start time
START_TIME=$(date +%s)

# Run the demo with resource monitoring
/usr/bin/time -v ./nlm_phase6_demo \
    --neuron-count 300 \
    --steps 1000 \
    --enable-checkpoint \
    --enable-replay \
    --enable-development 2>&1 | tee demo_output.log

# Record end time
END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

echo "====================================="
echo "Demo completed in ${DURATION} seconds"
echo "Output saved to demo_output.log"

# Extract key results
grep "=== FINAL RESULTS ===" demo_output.log
```

### Python Script: Automate Experiment Suite

```python
#!/usr/bin/env python3

import subprocess
import time
import json
import os

class NLMDemoRunner:
    def __init__(self, build_dir="/workspace/app/build"):
        self.build_dir = build_dir
        self.phase6_demo = f"{build_dir}/nlm_phase6_demo"
    
    def run_phase6_demo(self, config):
        """Run Phase 6 integration demo with given configuration."""
        cmd = [self.phase6_demo]
        
        # Add configuration options
        if "neuron_count" in config:
            cmd.extend(["--neuron-count", str(config["neuron_count"])])
        if "steps" in config:
            cmd.extend(["--steps", str(config["steps"])])
        if "enable_checkpoint" in config and config["enable_checkpoint"]:
            cmd.append("--enable-checkpoint")
        if "enable_replay" in config and config["enable_replay"]:
            cmd.append("--enable-replay")
        if "seed" in config:
            cmd.extend(["--seed", str(config["seed"])])
        
        print(f"Running: {' '.join(cmd)}")
        
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=config.get("timeout", 300)
            )
            
            if result.returncode == 0:
                print("Demo completed successfully!")
                return self.parse_demo_output(result.stdout)
            else:
                print(f"Demo failed with error: {result.stderr}")
                return None
                
        except subprocess.TimeoutExpired:
            print(f"Demo timed out after {config.get('timeout', 300)} seconds")
            return None
        except FileNotFoundError:
            print(f"Demo executable not found: {self.phase6_demo}")
            return None
    
    def parse_demo_output(self, output):
        """Parse demo output and extract key metrics."""
        metrics = {}
        
        for line in output.split('\n'):
            if "Total reward:" in line:
                metrics["total_reward"] = float(line.split(':')[1].strip())
            elif "Avg firing rate:" in line:
                metrics["avg_firing_rate"] = float(line.split(':')[1].strip())
            elif "Episodes stored:" in line:
                metrics["episodes_stored"] = float(line.split(':')[1].strip())
            elif "Dopamine level:" in line:
                metrics["dopamine_level"] = float(line.split(':')[1].strip())
            elif "Wall clock time:" in line:
                metrics["wall_clock_time"] = float(line.split(':')[1].strip().replace('s', ''))
            elif "Working Memory:" in line:
                metrics["working_memory"] = "CONNECTED" in line
            elif "Episodic Memory:" in line:
                metrics["episodic_memory"] = "CONNECTED" in line
            elif "Neuromodulation:" in line:
                metrics["neuromodulation"] = "CONNECTED" in line
            elif "Prediction:" in line:
                metrics["prediction"] = "CONNECTED" in line
            elif "Development:" in line:
                metrics["development"] = "CONNECTED" in line
        
        return metrics

def run_experiment_suite():
    """Run a suite of experiments with different configurations."""
    
    runner = NLMDemoRunner()
    
    # Define experiment configurations
    experiments = [
        {
            "name": "Small Scale Integration",
            "neuron_count": 200,
            "steps": 500,
            "enable_checkpoint": True,
            "enable_replay": True,
            "seed": 42
        },
        {
            "name": "Medium Scale Integration",
            "neuron_count": 500,
            "steps": 1000,
            "enable_checkpoint": True,
            "enable_replay": True,
            "seed": 123
        },
        {
            "name": "Large Scale Integration",
            "neuron_count": 800,
            "steps": 2000,
            "enable_checkpoint": True,
            "enable_replay": False,  # Skip replay for speed
            "seed": 456
        }
    ]
    
    results = {}
    
    for exp in experiments:
        print(f"\n{'='*60}")
        print(f"Running: {exp['name']}")
        print(f"{'='*60}")
        
        metrics = runner.run_phase6_demo(exp)
        
        if metrics:
            results[exp["name"]] = metrics
            
            # Print summary
            print(f"\nResults:")
            print(f"  Total Reward: {metrics.get('total_reward', 'N/A')}")
            print(f"  Avg Firing Rate: {metrics.get('avg_firing_rate', 'N/A')}")
            print(f"  Episodes Stored: {metrics.get('episodes_stored', 'N/A')}")
            print(f"  Wall Clock Time: {metrics.get('wall_clock_time', 'N/A')}s")
            print(f"  Integration Status: WM={metrics.get('working_memory')}, "
                  f"EM={metrics.get('episodic_memory')}, "
                  f"NM={metrics.get('neuromodulation')}, "
                  f"P={metrics.get('prediction')}, "
                  f"D={metrics.get('development')}")
    
    # Save results to JSON file
    with open("experiment_results.json", "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"\n{'='*60}")
    print("All experiments completed!")
    print("Results saved to experiment_results.json")
    print(f"{'='*60}")

if __name__ == "__main__":
    run_experiment_suite()
```

### Run the Python Script:

```bash
python3 nlm_demo_runner.py
```

## Summary

The NLM CLI provides a comprehensive set of tools for running simulations, demos, and tests of the NLM system. The most important commands are:

1. **`./nlm`** - Main simulation with brain, world, and agent integration
2. **`./nlm_phase6_demo`** - Phase 6 integration test (most comprehensive)
3. **`./nlm_phase4_demo`** - Phase 4 demo showing cognitive emergence
4. **`./nlm_phase3_demo`** - Phase 3 demo showing basic neural computation

The CLI supports extensive customization through configuration options and enables reproducible experiments with seed control.

For more detailed information about specific system components, refer to the corresponding documentation files (`docs/ARCHITECTURE.md`, `docs/CONFIGURATION.md`, etc.).
