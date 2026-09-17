# NLM Command Line Interface - Advanced Usage

## Quick Start Examples

### 1. Basic Simulation with Statistics
```bash
./nlm --neuron_count=2000 --config=configs/experiment.cfg --stats
```

### 2. Learning Experiment with Detailed Logging
```bash
./nlm --neuron_count=1000 --simulation_timestep=0.0005 \
     --enable_learning=true --log_level=Debug \
     --save_checkpoint=checkpoints/experiment_1.checkpoint
```

### 3. Development Phase Simulation
```bash
./nlm --developmental_stage=CriticalPeriod \
     --plasticity_modifier=0.8 --synaptogenesis_rate=0.0002f \
     --run_length=5000 --record_video=true
```

### 4. Agent World Simulation
```bash
./nlm --world_width=50 --world_height=50 --vision_range=12 \
     --agent_start=25,25 --max_energy=200 \
     --object_density=0.3 --run_steps=2000
```

## Command Line Options

### Core Options

#### `--help` or `-h`
Show help information and available options

#### `--version`
Display NLM version information

#### `--verbose` or `-v`
Enable verbose output for debugging

### Simulation Control

#### `--run_steps <n>` or `--steps <n>`
Number of simulation steps (default: 1000)

#### `--simulation_timestep <ms>` or `--dt <ms>`
Simulation timestep in milliseconds (default: 1.0)

#### `--run_length <ms>` or `--time <ms>`
Total simulation time in milliseconds

#### `--log_frequency <steps>`
Log statistics every N steps (default: 100)

### Brain Configuration

#### `--neuron_count <n>`
Total number of neurons (default: 1000)

#### `--region_count <n>`
Number of brain regions (default: 1)

#### `--connection_probability <p>`
Synaptic connection probability (0.0-1.0, default: 0.1)

#### `--enable_plasticity <rules>`
Enable plasticity systems (e.g., "STDP,Hebbian,Structural")

#### `--plasticity_learning_rate <rate>`
STDP learning rate (default: 0.01)

#### `--stdp_tau <ms>`
STDP time constant (default: 20.0ms)

### Neuromodulation

#### `--enable_neuromodulation <systems>`
Enable neuromodulation systems (e.g., "Dopamine,Curiosity,Novelty")

#### `--dopamine_level <level>`
Dopamine level (0.0-1.0, default: 0.0)

#### `--curiosity <level>`
Curiosity level (0.0-1.0, default: 0.0)

### Development

#### `--developmental_stage <stage>`
Developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging)

#### `--plasticity_modulation <factor>`
Global plasticity factor (0.1-2.0, default: 1.0)

### Agent World

#### `--world_width <w>, --world_height <h>`
World dimensions in meters

#### `--agent_start <x>,<y>`
Agent starting position

#### `--agent_orientation <angle>`
Agent orientation in degrees

#### `--object_density <density>`
Object density (0.0-1.0, default: 0.5)

### Recording and Output

#### `--log_level <level>`
Logging level (Error, Warning, Info, Debug, Trace)

#### `--save_checkpoint <file>`
Save brain state to checkpoint file

#### `--load_checkpoint <file>`
Load brain state from checkpoint file

#### `--enable_recording <types>`
Enable recording systems (spikes, weights, activity, neurons, synapses, regions)

### Performance

#### `--performance_mode <mode>`
Performance mode (Balanced, Optimized, Debug)

#### `--parallel_processing <threads>`
Enable parallel processing with N threads

### Experiment Management

#### `--trial_id <id>`
Experiment trial identifier

#### `--experiment_name <name>`
Experiment name for result organization

## Configuration File

### Format
Configuration files use `key=value` format, one per line

### Example
```
# Brain parameters
brain.neuron_count = 2000
brain.synapse_density = 0.1
brain.connection_probability = 0.15

# Plasticity
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.01
plasticity.stdp.tau = 20.0

# World
world.width = 50.0
world.height = 50.0
world.agent.start_x = 25.0
world.agent.start_y = 25.0
```

### Loading Configuration
```bash
./nlm --config=production.cfg
./nlm --config=experiment.json
```

## Script Examples

### Python Wrapper Script
```python
#!/usr/bin/env python3

import subprocess
import json
import time
import argparse
from pathlib import Path

class NLMRunner:
    def __init__(self, nlm_executable='./nlm'):
        self.nlm_executable = nlm_executable
        self.checkpoint_dir = Path('checkpoints')
        self.checkpoint_dir.mkdir(exist_ok=True)
    
    def run_simulation(self, config_file=None, options=None, name=None):
        """Run NLM simulation with configuration."""
        cmd = [self.nlm_executable]
        
        if config_file:
            cmd.extend(['--config', config_file])
        
        if options:
            for key, value in options.items():
                if isinstance(value, bool):
                    if value:
                        cmd.append(f'--{key}')
                elif isinstance(value, (list, tuple)):
                    cmd.extend([f'--{key}', ','.join(str(v) for v in value)])
                else:
                    cmd.extend([f'--{key}', str(value)])
        
        if name:
            timestamp = time.strftime('%Y%m%d_%H%M%S')
            cmd.extend(['--trial_id', f"{name}_{timestamp}"])
            cmd.extend(['--experiment_name', name])
        
        print(f"Running: {' '.join(cmd)}")
        
        try:
            result = subprocess.run(
                cmd, 
                capture_output=True, 
                text=True,
                timeout=3600  # 1 hour timeout
            )
            
            if result.returncode == 0:
                print(f"✓ Simulation completed successfully ({name or 'unnamed'})")
                if name:
                    # Save output to file
                    output_file = self.checkpoint_dir / f"{name}_output.txt"
                    with open(output_file, 'w') as f:
                        f.write(result.stdout)
                    print(f"Output saved to: {output_file}")
            else:
                print(f"✗ Simulation failed: {result.stderr}")
                
            return result
            
        except subprocess.TimeoutExpired:
            print(f"✗ Simulation timed out ({name or 'unnamed'})")
            return None
        except FileNotFoundError:
            print(f"✗ NLM executable not found: {self.nlm_executable}")
            return None

def main():
    parser = argparse.ArgumentParser(description='Run NLM simulations with configuration')
    parser.add_argument('--config', help='Configuration file')
    parser.add_argument('--name', help='Experiment name')
    parser.add_argument('--neurons', type=int, default=1000, help='Neuron count')
    parser.add_argument('--steps', type=int, default=1000, help='Number of steps')
    parser.add_argument('--time', type=float, default=1.0, help='Simulation time (ms)')
    parser.add_argument('--plasticity', type=float, default=0.01, help='Plasticity learning rate')
    parser.add_argument('--curiosity', type=float, default=0.0, help='Curiosity level')
    parser.add_argument('--development', choices=['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging'], 
                       default='Initial', help='Developmental stage')
    
    args = parser.parse_args()
    
    # Create options dictionary
    options = {
        'neuron_count': args.neurons,
        'run_steps': args.steps,
        'simulation_timestep': args.time / 1000.0,
        'plasticity_learning_rate': args.plasticity,
        'curiosity': args.curiosity,
        'developmental_stage': args.development,
        'enable_structural_plasticity': True,
        'enable_memory_learning': 'episodic,associative',
        'enable_replay': True,
        'enable_consolidation': True
    }
    
    # Initialize runner and run simulation
    runner = NLMRunner()
    result = runner.run_simulation(args.config, options, args.name)
    
    if result and result.returncode == 0:
        print("\n✓ Experiment completed successfully!")
        exit(0)
    else:
        print("\n✗ Experiment failed.")
        exit(1)

if __name__ == "__main__":
    main()
```

### Shell Script for Batch Experiments
```bash
#!/bin/bash

# NLM Batch Experiment Runner

EXPERIMENT_DIR="experiments/$(date +%Y%m%d_%H%M%S)"
mkdir -p "$EXPERIMENT_DIR"
mkdir -p "checkpoints/$EXPERIMENT_DIR"

# Experiment configurations
experiments=(
    "baseline,neurons=1000,time=1000,plasticity=0.01,curiosity=0.0"
    "high_plasticity,neurons=2000,time=2000,plasticity=0.05,curiosity=0.3"
    "development_focus,neurons=1500,time=3000,plasticity=0.02,curiosity=0.5,stage=CriticalPeriod"
    "agent_world,neurons=2000,time=1500,width=50,height=50,agent_start=25,25,object_density=0.3"
    "memory_heavy,neurons=3000,time=2000,plasticity=0.015,curiosity=0.4,memory_capacity=working=3000,episodic=1500"
)

# Run experiments
for exp_config in "${experiments[@]}"; do
    echo "=== Running experiment: $exp_config ==="
    
    # Parse configuration
    IFS=',' read -ra params <<< "$exp_config"
    base_name="${params[0]}"
    shift "${#params[@]}"
    
    # Build command
    cmd=("./nlm")
    for param in "${params[@]}"; do
        if [[ "$param" == *=* ]]; then
            key="${param%%=*}"
            value="${param#*=}"
            cmd+=("--${key} "${value}")
        fi
    done
    
    # Add output redirection
    cmd+=("--save_checkpoint" "checkpoints/$EXPERIMENT_DIR/${base_name}.checkpoint")
    cmd+=("--trial_id" "${base_name}_$(date +%Y%m%d_%H%M%S)")
    
    # Run experiment
    echo "Command: ${cmd[*]}"
    
    if "${cmd[@]}"; then
        echo "✓ Experiment $base_name completed successfully"
        
        # Copy results
        if [ -f "brain_stats.csv" ]; then
            cp brain_stats.csv "$EXPERIMENT_DIR/${base_name}_stats.csv"
        fi
        
        if [ -f "nlm_output.txt" ]; then
            cp nlm_output.txt "$EXPERIMENT_DIR/${base_name}_log.txt"
        fi
        
    else
        echo "✗ Experiment $base_name failed"
    fi
    
    echo ""
    echo "Results saved to: $EXPERIMENT_DIR"
    echo "==================================="
    echo ""
done

echo "All experiments completed! Results saved to: $EXPERIMENT_DIR"
```

### Interactive CLI Script
```python
#!/usr/bin/env python3

import cmd
import subprocess
import json
import time
from pathlib import Path

class NLMInteractiveCLI(cmd.Cmd):
    """Interactive command-line interface for NLM simulations."""
    
    intro = """
╔══════════════════════════════════════════════════════════════╗
║           NLM Advanced Command Line Interface              ║
╠══════════════════════════════════════════════════════════════╣
║ Type 'help' for available commands or 'help <command>' for details ║
╚══════════════════════════════════════════════════════════════╝

Welcome to the NLM Interactive CLI. This tool helps you:
  • Configure and run simulations with ease
  • Manage experiment parameters
  • Monitor simulation progress
  • Analyze results interactively
"""
    
    prompt = "nlm> "
    
    def __init__(self):
        super().__init__()
        self.current_config = {
            'neuron_count': 1000,
            'run_steps': 1000,
            'simulation_timestep': 0.001,
            'log_level': 'Info',
            'developmental_stage': 'Initial',
            'plasticity_modulation': 1.0,
            'curiosity': 0.0,
            'enable_structural_plasticity': True,
            'enable_memory_learning': 'episodic,associative',
            'enable_replay': True,
            'enable_consolidation': True
        }
        self.checkpoint_dir = Path('checkpoints')
        self.checkpoint_dir.mkdir(exist_ok=True)
    
    def do_run(self, args):
        """Run NLM simulation with current configuration."""
        print("\n" + "="*60)
        print("RUNNING SIMULATION")
        print("="*60)
        
        # Show configuration
        print("\nConfiguration:")
        for key, value in self.current_config.items():
            if isinstance(value, bool):
                print(f"  {key}: {'✓' if value else '✗'}")
            else:
                print(f"  {key}: {value}")
        
        # Build command
        cmd = ['./nlm']
        
        for key, value in self.current_config.items():
            if isinstance(value, bool):
                if value:
                    cmd.append(f'--{key}')
            elif isinstance(value, (list, tuple)):
                cmd.extend([f'--{key}', ','.join(str(v) for v in value)])
            else:
                cmd.extend([f'--{key}', str(value)])
        
        # Generate unique trial ID
        timestamp = time.strftime('%Y%m%d_%H%M%S')
        trial_id = f"cli_run_{timestamp}"
        cmd.extend(['--trial_id', trial_id])
        cmd.extend(['--experiment_name', f"cli_experiment_{timestamp}"])
        
        print(f"\nExecuting command:")
        print(" ", " ".join(cmd))
        
        # Check if executable exists
        if not Path('./nlm').exists():
            print("\n⚠ Warning: './nlm' not found in current directory.")
            print("Please ensure the NLM executable is built and available.")
            return
        
        # Run simulation with timeout
        print(f"\n🚀 Starting simulation (this may take several minutes)...")
        start_time = time.time()
        
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=7200  # 2 hour timeout
            )
            
            elapsed_time = time.time() - start_time
            
            if result.returncode == 0:
                print(f"\n✅ Simulation completed successfully!")
                print(f"⏱️  Duration: {elapsed_time/60:.1f} minutes")
                print(f"📁 Output saved to: checkpoints/{trial_id}_output.txt")
                
                # Show summary if available
                if "Total neurons:" in result.stdout:
                    print("\n📊 Simulation Summary:")
                    for line in result.stdout.split('\n'):
                        if any(keyword in line for keyword in ['neurons:', 'synapses:', 'spikes:', 'firing neurons:']):
                            print(f"   {line}")
            else:
                print(f"\n❌ Simulation failed with exit code {result.returncode}")
                print("\nError output:")
                print(result.stderr[:500] + "..." if len(result.stderr) > 500 else result.stderr)
                
        except subprocess.TimeoutExpired:
            print(f"\n⏰ Simulation timed out after 2 hours")
        except Exception as e:
            print(f"\n💥 Error running simulation: {e}")
    
    def do_set(self, args):
        """Set configuration parameter: set <key> <value>"""
        args = args.strip().split(maxsplit=1)
        if len(args) != 2:
            print("\n❌ Usage: set <key> <value>")
            print("Example: set neuron_count 5000")
            return
        
        key, value = args
        key = key.strip()
        value = value.strip()
        
        # Handle boolean values
        if value.lower() in ('true', 'false', 'yes', 'no', 'on', 'off'):
            bool_value = value.lower() in ('true', 'yes', 'on')
            self.current_config[key] = bool_value
            print(f"\n✅ Set {key} = {'✓' if bool_value else '✗'}")
            return
        
        # Handle numeric values
        if value.replace('.', '').isdigit():
            if '.' in value:
                self.current_config[key] = float(value)
            else:
                self.current_config[key] = int(value)
            print(f"\n✅ Set {key} = {self.current_config[key]}")
            return
        
        # Handle list values
        if value.startswith('[') and value.endswith(']'):
            try:
                import ast
                list_value = ast.literal_eval(value)
                if isinstance(list_value, list):
                    self.current_config[key] = list_value
                    print(f"\n✅ Set {key} = {self.current_config[key]}")
                    return
            except:
                print(f"\n❌ Could not parse list value: {value}")
                print("   Example format: [1, 2, 3] or ['a', 'b', 'c']")
                return
        
        # Handle string values
        self.current_config[key] = value
        print(f"\n✅ Set {key} = {self.current_config[key]}")
    
    def do_show(self, args):
        """Show current configuration."""
        print("\n" + "="*60)
        print("CURRENT CONFIGURATION")
        print("="*60)
        
        for key, value in self.current_config.items():
            if isinstance(value, bool):
                status = '✓' if value else '✗'
                print(f"  {key:<30} {status}")
            elif isinstance(value, (list, tuple)):
                print(f"  {key:<30} {', '.join(str(v) for v in value)}")
            else:
                print(f"  {key:<30} {value}")
        
        print("\n💡 Tip: Use 'set <key> <value>' to modify configuration")
        print("   Example: set neuron_count 5000")
    
    def do_reset(self, args):
        """Reset configuration to defaults."""
        print("\n🔄 Resetting configuration to defaults...")
        
        self.current_config = {
            'neuron_count': 1000,
            'run_steps': 1000,
            'simulation_timestep': 0.001,
            'log_level': 'Info',
            'developmental_stage': 'Initial',
            'plasticity_modulation': 1.0,
            'curiosity': 0.0,
            'enable_structural_plasticity': True,
            'enable_memory_learning': 'episodic,associative',
            'enable_replay': True,
            'enable_consolidation': True
        }
        
        print("✅ Configuration reset to defaults.")
    
    def do_config(self, args):
        """Load configuration from file."""
        if not args:
            print("\n❌ Usage: config <filename>")
            print("Example: config my_experiment.json")
            return
        
        config_file = args.strip()
        if not Path(config_file).exists():
            print(f"\n❌ Configuration file not found: {config_file}")
            return
        
        try:
            with open(config_file, 'r') as f:
                config = json.load(f)
            
            self.current_config.update(config)
            print(f"\n✅ Loaded configuration from: {config_file}")
            print(f"   Loaded {len(config)} parameters")
            
        except json.JSONDecodeError as e:
            print(f"\n❌ Invalid JSON in configuration file: {e}")
        except Exception as e:
            print(f"\n❌ Error loading configuration: {e}")
    
    def do_save(self, args):
        """Save current configuration to file."""
        if not args:
            filename = f"nlm_config_{int(time.time())}.json"
        else:
            filename = args.strip()
        
        try:
            with open(filename, 'w') as f:
                json.dump(self.current_config, f, indent=2)
            
            print(f"\n✅ Configuration saved to: {filename}")
            
        except Exception as e:
            print(f"\n❌ Error saving configuration: {e}")
    
    def do_help(self, args):
        """Get help on commands."""
        if args:
            # Get help for specific command
            super().do_help(args)
        else:
            print("\n" + "="*60)
            print("AVAILABLE COMMANDS")
            print("="*60)
            
            commands = [
                ("run", "Run simulation with current configuration"),
                ("set <k> <v>", "Set configuration parameter"),
                ("show", "Display current configuration"),
                ("reset", "Reset configuration to defaults"),
                ("config <f>", "Load configuration from JSON file"),
                ("save <f>", "Save configuration to JSON file"),
                ("history", "Show command history"),
                ("clear", "Clear screen"),
                ("help", "Show help information"),
                ("exit", "Exit the interpreter")
            ]
            
            for cmd, desc in commands:
                print(f"  {cmd:<20} {desc}")
            
            print("\n" + "="*60)
            print("EXAMPLES")
            print("="*60)
            print("  # Set neuron count to 5000")
            print("  set neuron_count 5000")
            print()
            print("  # Enable curiosity-driven exploration")
            print("  set curiosity 0.5")
            print()
            print("  # Set developmental stage")
            print("  set developmental_stage CriticalPeriod")
            print()
            print("  # Show current configuration")
            print("  show")
            print()
            print("  # Run simulation")
            print("  run")
            print()
            print("  # Save configuration to file")
            print("  save my_config.json")
            print()
            print("  # Load configuration from file")
            print("  config my_config.json")
    
    def do_history(self, args):
        """Show command history."""
        print("\n" + "="*60)
        print("COMMAND HISTORY")
        print("="*60)
        
        if not hasattr(self, 'cmd_queue'):
            print("  (No commands executed yet)")
            return
        
        for i, cmd in enumerate(self.cmd_queue[-10:], 1):
            print(f"  {i:2d}. {cmd}")
    
    def do_clear(self, args):
        """Clear the screen."""
        import os
        os.system('clear' if os.name == 'posix' else 'cls')
        print("\n" + "="*60)
        print("NLM Interactive CLI - Screen Cleared")
        print("="*60)
    
    def postcmd(self, line, stdin):
        """Add a blank line after each command and store in history."""
        if not hasattr(self, 'cmd_queue'):
            self.cmd_queue = []
        
        self.cmd_queue.append(line.strip())
        
        if len(self.cmd_queue) > 100:
            self.cmd_queue = self.cmd_queue[-100:]
        
        print()
        return line
    
    def do_exit(self, args):
        """Exit the interpreter."""
        print("\n" + "="*60)
        print("GOODBYE!")
        print("="*60)
        return True
    
    def precmd(self, line):
        """Process command before execution."""
        # Strip leading/trailing whitespace
        line = line.strip()
        
        # Convert simple shortcuts
        if line == "s":
            line = "show"
        elif line == "r":
            line = "run"
        elif line == "q":
            line = "exit"
        
        return line

if __name__ == "__main__":
    try:
        NLMInteractiveCLI().cmdloop()
    except KeyboardInterrupt:
        print("\n\n⚠ Simulation interrupted by user")
        print("Exiting gracefully...")
    except Exception as e:
        print(f"\n💥 Unexpected error: {e}")
        exit(1)
```

## Configuration File Examples

### Basic Configuration
```json
{
    "neuron_count": 2000,
    "run_steps": 5000,
    "simulation_timestep": 0.001,
    "log_level": "Debug",
    "developmental_stage": "CriticalPeriod",
    "plasticity_modulation": 0.8,
    "curiosity": 0.3,
    "enable_structural_plasticity": true,
    "enable_memory_learning": "episodic,associative",
    "enable_replay": true,
    "enable_consolidation": true
}
```

### World Configuration
```json
{
    "world": {
        "width": 50.0,
        "height": 50.0,
        "vision_range": 12,
        "agent_start": [25, 25],
        "agent_orientation": 45,
        "max_energy": 200,
        "object_density": 0.3
    },
    "world_objects": {
        "resource_count": 8,
        "hazard_count": 4,
        "resource_value": 15.0,
        "hazard_damage": 20.0
    }
}
```

### Learning Configuration
```json
{
    "learning": {
        "enable_curiosity": true,
        "curiosity_level": 0.6,
        "enable_novelty": true,
        "novelty_decay": 0.95,
        "enable_prediction": true,
        "prediction_confidence": 0.7,
        "enable_planning": true,
        "planning_depth": 4,
        "enable_concepts": true,
        "concept_formation_threshold": 0.7
    }
}
```

## Quick Reference

### Core Commands

| Command | Description | Example |
|---------|-------------|---------|
| `run` | Run simulation | `run` |
| `set <k> <v>` | Set parameter | `set neuron_count 5000` |
| `show` | Display config | `show` |
| `reset` | Reset defaults | `reset` |
| `config <f>` | Load config | `config my_exp.json` |
| `save <f>` | Save config | `save my_config.json` |

### Common Patterns

#### High Plasticity Learning
```bash
./nlm --enable_plasticity="STDP,Hebbian,Structural" \
     --plasticity_learning_rate=0.05 \
     --stdp_tau=30 \
     --enable_curiosity=true --curiosity=0.5
```

#### Development Study
```bash
./nlm --developmental_stage="CriticalPeriod,Maturation" \
     --plasticity_modulation=0.5 \
     --enable_replay=true \
     --enable_consolidation=true \
     --run_steps=3000
```

#### Agent World Experiment
```bash
./nlm --world_width=100 --world_height=80 \
     --agent_start=50,40 --agent_orientation=0 \
     --object_density=0.2 --resource_value=25 \
     --hazard_rate=0.05 --run_steps=2000
```

#### Memory-intensive Experiment
```bash
./nlm --memory_capacity="working=5000,episodic=2000,associative=1000" \
     --enable_memory_learning="episodic,associative,working" \
     --enable_consolidation=true --consolidation_threshold=0.4 \
     --run_steps=4000
```

### Tips and Best Practices

#### 1. Configuration Management
- Use JSON files for complex experiment configurations
- Document parameter choices with comments
- Keep backup copies of important checkpoints
- Use descriptive names for experiments

#### 2. Performance Optimization
- Use `performance_mode=Optimized` for production
- Enable parallel processing for large simulations
- Monitor memory usage with `log_level=Debug`
- Use appropriate timestep for your simulation scale

#### 3. Experiment Tracking
- Use consistent naming conventions
- Organize results by experiment type and date
- Implement parameter documentation
- Use unique trial IDs for reproducibility

#### 4. Troubleshooting

**Common Issues:**
- `Too much memory` → Reduce neuron_count, disable vectorization
- `Too slow` → Increase timestep, reduce log_frequency
- `Configuration errors` → Check JSON syntax, verify parameter names

**Debug Commands:**
```bash
# Debug run with minimal settings
./nlm --neuron_count=100 --run_steps=100 --log_level=Trace

# Test configuration
./nlm --config=test_config.json

# Check executable
./nlm --help
```

## Advanced Features

### Multi-Stage Experiments
```python
# Multi-stage experiment using Python
stages = [
    {"name": "baseline", "neurons": 1000, "time": 1000},
    {"name": "learning", "neurons": 2000, "time": 3000, "plasticity": 0.05},
    {"name": "development", "neurons": 3000, "time": 5000, "stage": "CriticalPeriod"}
]

runner = NLMRunner()
for stage in stages:
    runner.run_simulation(name=stage["name"], **stage)
```

### Parameter Sweeps
```bash
#!/bin/bash

# Systematic parameter sweep
for plasticity in 0.01 0.02 0.05 0.1; do
    for curiosity in 0.0 0.3 0.6 0.9; do
        echo "Running: plasticity=$plasticity, curiosity=$curiosity"
        ./nlm \
            --neuron_count=2000 \
            --plasticity_learning_rate=$plasticity \
            --curiosity=$curiosity \
            --run_steps=1000 \
            --trial_id="sweep_$(date +%Y%m%d_%H%M%S)"
        sleep 1
    done
done
```

### Real-time Monitoring
```python
import time
import subprocess
import threading

def monitor_simulation(process, interval=10):
    """Monitor simulation progress in real-time."""
    while process.poll() is None:
        try:
            output = process.stdout.readline()
            if output and "Total neurons:" in output:
                print(f"[{time.strftime('%H:%M:%S')}] {output.strip()}")
        except:
            break
        time.sleep(interval)

# Start monitored simulation
proc = subprocess.Popen(['./nlm', '--run_steps=10000', '--log_level=Info'])
monitor_thread = threading.Thread(target=monitor_simulation, args=(proc,))
monitor_thread.daemon = True
monitor_thread.start()

print("Simulation running with monitoring...")
print("Check terminal for real-time progress updates")

# Continue with other tasks
proc.wait()
print("Simulation completed!")
```

## Troubleshooting Guide

### Error Messages

**Configuration Errors:**
```
"Configuration error: unknown parameter 'plasticity.learning_rate'"
→ Use parameter 'plasticity_learning_rate' instead
```

**File Not Found:**
```
"Error: Configuration file 'experiment.json' not found"
→ Check file path and ensure file exists
```

**Memory Issues:**
```
"Error: Not enough memory for simulation"
→ Reduce neuron_count or region_count
```

**Executable Issues:**
```
"Error: './nlm' not found"
→ Build NLM or check executable permissions
```

### Debug Commands

```bash
# Minimal debug run
./nlm --neuron_count=100 --run_steps=100 --log_level=Debug

# Test configuration syntax
./nlm --config=debug_config.json

# Check available options
./nlm --help

# List all parameters
./nlm --list_parameters
```

## Best Practices

### 1. Experiment Management
- Use consistent parameter naming across runs
- Document initial conditions for reproducibility
- Implement parameter sweeps for systematic studies
- Keep track of configuration versions

### 2. Performance Optimization
- Profile your simulation to identify bottlenecks
- Use appropriate performance modes for your use case
- Monitor memory and CPU usage during long runs
- Consider parallel processing for large simulations

### 3. Result Organization
- Organize results by experiment type and date
- Use descriptive filenames for easy identification
- Implement backup strategies for important results
- Document experiment parameters for future reference

### 4. Reproducibility
- Save configuration files for each experiment
- Use version control for configuration management
- Document all command line options used
- Keep checkpoint files for partial results

This comprehensive guide provides everything needed to effectively use NLM for advanced neural network simulations, from basic usage to complex experimental setups and systematic parameter sweeps.
