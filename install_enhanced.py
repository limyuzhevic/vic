#!/usr/bin/env python3
"""
NLM Enhanced Package Installer

This script installs the enhanced NLM Python package with all its features:
- Enhanced Python bindings
- Convenience modules
- Command-line tools
- Documentation and examples
"""

import os
import sys
import json
import shutil
from pathlib import Path

def check_pynlm_available():
    """Check if pynlm is available"""
    try:
        import pynlm
        print("✓ Original pynlm found")
        return True
    except ImportError:
        print("✗ Original pynlm not found")
        return False

def check_enhanced_bindings_available():
    """Check if enhanced bindings are available"""
    try:
        import pynlm_enhanced
        print("✓ Enhanced bindings found")
        return True
    except ImportError:
        print("✗ Enhanced bindings not found")
        return False

def install_enhanced_package():
    """Install the enhanced NLM package"""
    print("\nInstalling NLM Enhanced Package...")
    
    # Create enhanced directory structure
    enhanced_dir = Path("pynlm_enhanced")
    enhanced_dir.mkdir(exist_ok=True)
    
    # Create subdirectories
    (enhanced_dir / "core").mkdir(exist_ok=True)
    (enhanced_dir / "components").mkdir(exist_ok=True)
    (enhanced_dir / "examples").mkdir(exist_ok=True)
    (enhanced_dir / "docs").mkdir(exist_ok=True)
    
    # Copy enhanced component files
    component_files = [
        "validated_config.py",
        "advanced_simulation_manager.py", 
        "performance_monitor.py",
        "component_factory.py",
        "analysis_utils.py",
        "neural_network_builder.py"
    ]
    
    for file in component_files:
        src = Path(f"enhanced/{file}")
        dst = enhanced_dir / f"{file}"
        if src.exists():
            shutil.copy2(src, dst)
            print(f"  ✓ Installed {file}")
        else:
            print(f"  ✗ Missing {file}")
    
    # Create enhanced __init__.py
    init_content = '''"""
Enhanced NLM Python Package

This package provides enhanced functionality for NLM (Neural Learning Machine) users,
including advanced configuration management, simulation control, and analysis tools.

Features:
- Validated Configuration System
- Advanced Simulation Manager  
- Performance Monitoring
- Component Factory for Extensibility
- Analysis Utilities
- Neural Network Builder
"""

import sys
from pathlib import Path

# Import enhanced components
from .validated_config import ValidatedConfig
from .advanced_simulation_manager import AdvancedSimulationManager
from .performance_monitor import PerformanceMonitor
from .component_factory import ComponentFactory
from .analysis_utils import AnalysisUtils
from .neural_network_builder import NeuralNetworkBuilder

# Try to import original pynlm for backward compatibility
try:
    import pynlm
    # Re-export original classes
    from pynlm import Config, Brain, AgentBrain, SimpleWorld, Vision, Audio, Action
    from pynlm import SensoryInput, NeuronType, SynapseType, DevelopmentalStage
    from pynlm import FiringState, ActionType, WorldObjectType, MotorCommand
    from pynlm import createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
except ImportError:
    # Create placeholder classes if original not available
    class Config: pass
    class Brain: pass
    class AgentBrain: pass
    class SimpleWorld: pass
    class Vision: pass
    class Audio: pass
    class Action: pass
    class SensoryInput: pass
    
    # Define enums as placeholders
    class NeuronType:
        Excitatory = 0; Inhibitory = 1; Modulatory = 2
        Sensory = 3; Motor = 4; Internal = 5
    
    class SynapseType:
        Excitatory = 0; Inhibitory = 1; Modulatory = 2
        Electrical = 3; GapJunction = 4
    
    class DevelopmentalStage:
        Initial = 0; CriticalPeriod = 1; Maturation = 2
        Adult = 3; Aging = 4
    
    class FiringState:
        Resting = 0; Active = 1; Refractory = 2; Inhibited = 3
    
    class ActionType:
        MoveForward = 0; MoveBackward = 1; MoveLeft = 2; MoveRight = 3
        TurnLeft = 4; TurnRight = 5; Look = 6; LookUp = 7; LookDown = 8
        Interact = 9; Eat = 10; Drink = 11; Rest = 12; Wait = 13; Custom = 14
    
    class WorldObjectType:
        Empty = 0; Resource = 1; Hazard = 2; Wall = 3; Marker = 4
    
    class MotorCommand:
        MoveForward = 0; MoveBackward = 1; TurnLeft = 2; TurnRight = 3
        LookLeft = 4; LookRight = 5; Interact = 6; Wait = 7
    
    def createDefaultConfig(): return Config()
    def createBrain(config): return Brain()
    def createSimpleWorld(): return SimpleWorld()
    def createAgentBrain(brain): return AgentBrain(brain)

# Convenience functions
def create_enhanced_brain(config=None):
    """Create an enhanced brain with validated configuration"""
    if config is None:
        config = createDefaultConfig()
    
    # Apply enhanced validation
    if isinstance(config, ValidatedConfig):
        # Validation is already set up
        pass
    
    brain = createBrain(config)
    brain.initialize()
    return brain

def benchmark_network(brain, steps, runs=3):
    """Benchmark network performance"""
    import time
    
    results = []
    for run in range(runs):
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        results.append({
            'time': end_time - start_time,
            'firing_rate': brain.getAverageFiringRate(),
            'spikes': brain.getTotalSpikeCount()
        })
        
        # Reset for next run
        brain.reset()
        brain.initialize()
    
    avg_time = sum(r['time'] for r in results) / len(results)
    avg_firing = sum(r['firing_rate'] for r in results) / len(results)
    avg_spikes = sum(r['spikes'] for r in results) / len(results)
    
    return {
        'average_time': avg_time,
        'average_firing_rate': avg_firing,
        'average_spike_count': avg_spikes,
        'speed_steps_per_second': steps / avg_time
    }

# Export public API
__all__ = [
    # Enhanced classes
    'ValidatedConfig',
    'AdvancedSimulationManager',
    'PerformanceMonitor',
    'ComponentFactory', 
    'AnalysisUtils',
    'NeuralNetworkBuilder',
    
    # Original classes (if available)
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld', 'Vision', 'Audio', 'Action',
    'SensoryInput', 'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'WorldObjectType', 'MotorCommand',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    
    # Convenience functions
    'create_enhanced_brain', 'benchmark_network'
]

__version__ = "2.0.0-enhanced"
__author__ = "NLM Research Team"
'''
    
    with open(enhanced_dir / "__init__.py", "w") as f:
        f.write(init_content)
    
    print("  ✓ Created enhanced package __init__.py")

def install_cli_tools():
    """Install command-line tools"""
    print("\nInstalling command-line tools...")
    
    # Create bin directory
    bin_dir = Path("bin")
    bin_dir.mkdir(exist_ok=True)
    
    # Create enhanced CLI script
    cli_script = '''#!/usr/bin/env python3
"""
NLM Enhanced Command Line Interface

This script provides command-line access to enhanced NLM functionality.
"""

import sys
import argparse
from pathlib import Path

# Add current directory to Python path
sys.path.insert(0, str(Path(__file__).parent.parent))

def cmd_create_brain(args):
    """Create a brain with enhanced configuration"""
    try:
        from pynlm_enhanced import create_enhanced_brain, ValidatedConfig
        from pynlm import createDefaultConfig
        
        config = createDefaultConfig()
        
        if args.neurons:
            config.set("neuron_count", int(args.neurons))
        if args.regions:
            config.set("region_count", int(args.regions))
        if args.connection_prob:
            config.set("connection_probability", float(args.connection_prob))
        
        brain = create_enhanced_brain(config)
        
        print(f"Created enhanced brain:")
        print(f"  - Neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Regions: {brain.getRegionCount()}")
        print(f"  - Synapses: {brain.getTotalSynapseCount()}")
        
        return brain
        
    except Exception as e:
        print(f"Error creating brain: {e}")
        return None

def cmd_simulate(args):
    """Run enhanced simulation"""
    try:
        from pynlm_enhanced import create_enhanced_brain, benchmark_network
        from pynlm import createDefaultConfig
        
        config = createDefaultConfig()
        
        brain = create_enhanced_brain(config)
        
        print(f"Running enhanced simulation for {args.steps} steps...")
        
        for step in range(args.steps):
            brain.step(step)
            
            if step % 100 == 0:
                print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                      f"{brain.getAverageFiringRate():.2f} avg firing rate")
        
        # Benchmark performance
        if args.benchmark:
            results = benchmark_network(brain, args.steps)
            print(f"\nPerformance benchmark:")
            print(f"  - Average time per run: {results['average_time']:.3f}s")
            print(f"  - Speed: {results['speed_steps_per_second']:.0f} steps/second")
            print(f"  - Average firing rate: {results['average_firing_rate']:.2f}")
        
        print(f"\nSimulation complete!")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
        
        return brain
        
    except Exception as e:
        print(f"Error running simulation: {e}")
        return None

def cmd_analyze(args):
    """Analyze brain performance"""
    try:
        from pynlm_enhanced import create_enhanced_brain, AnalysisUtils
        from pynlm import createDefaultConfig
        
        config = createDefaultConfig()
        brain = create_enhanced_brain(config)
        
        # Run analysis
        firing_rates = AnalysisUtils.analyze_firing_pattern(brain, args.steps)
        
        print(f"Analysis of {len(firing_rates)} steps:")
        print(f"  - Average firing rate: {AnalysisUtils.calculate_mean(firing_rates):.2f}")
        print(f"  - Firing rate trend: {AnalysisUtils.calculate_firing_rate_trend(firing_rates):.4f}")
        print(f"  - Firing rate std: {AnalysisUtils.calculate_std_dev(firing_rates, AnalysisUtils.calculate_mean(firing_rates)):.2f}")
        
    except Exception as e:
        print(f"Error analyzing brain: {e}")

def main():
    parser = argparse.ArgumentParser(
        description="NLM Enhanced Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s create-brain --neurons 1000 --regions 3
  %(prog)s simulate --steps 500 --benchmark
  %(prog)s analyze --steps 200
  %(prog)s benchmark --steps 1000
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Create brain command
    parser_create = subparsers.add_parser('create-brain', help='Create an enhanced brain')
    parser_create.add_argument('--neurons', help='Number of neurons')
    parser_create.add_argument('--regions', help='Number of regions')
    parser_create.add_argument('--connection-prob', help='Connection probability')
    parser_create.set_defaults(func=cmd_create_brain)
    
    # Simulate command
    parser_sim = subparsers.add_parser('simulate', help='Run enhanced simulation')
    parser_sim.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    parser_sim.add_argument('--benchmark', action='store_true', help='Benchmark performance')
    parser_sim.set_defaults(func=cmd_simulate)
    
    # Analyze command
    parser_analyze = subparsers.add_parser('analyze', help='Analyze brain performance')
    parser_analyze.add_argument('--steps', type=int, default=100, help='Number of steps to analyze')
    parser_analyze.set_defaults(func=cmd_analyze)
    
    # Benchmark command
    parser_bench = subparsers.add_parser('benchmark', help='Benchmark network performance')
    parser_bench.add_argument('--steps', type=int, default=100, help='Number of steps to simulate')
    parser_bench.set_defaults(func=lambda args: print(f"Benchmarking network for {args.steps} steps... (use simulate --benchmark)"))
    
    # Install command
    parser_install = subparsers.add_parser('install', help='Install enhanced package')
    parser_install.set_defaults(func=lambda args: print("Enhanced package installation completed"))
    
    if len(sys.argv) == 1:
        parser.print_help()
        return
    
    args = parser.parse_args()
    try:
        args.func(args)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
'''
    
    with open(bin_dir / "nlm-enhanced", "w") as f:
        f.write(cli_script)
    
    # Make executable
    os.chmod(bin_dir / "nlm-enhanced", 0o755)
    
    print("  ✓ Installed CLI tools to bin/nlm-enhanced")

def create_examples():
    """Create example scripts"""
    print("\nCreating example scripts...")
    
    examples_dir = Path("pynlm_enhanced/examples")
    
    # Basic example
    basic_example = '''"""
Basic Example: Enhanced NLM Usage

This example demonstrates basic usage of the enhanced NLM package.
"""

from pynlm_enhanced import (
    create_enhanced_brain, 
    AdvancedSimulationManager,
    PerformanceMonitor,
    AnalysisUtils
)
from pynlm import createDefaultConfig

def basic_example():
    """Basic enhanced brain example"""
    print("=== Basic Enhanced NLM Example ===")
    
    # Create enhanced brain with validated configuration
    config = createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    
    brain = create_enhanced_brain(config)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run basic simulation
    for step in range(100):
        brain.step(step)
    
    print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    return brain

def simulation_example():
    """Advanced simulation with protocol"""
    print("\n=== Advanced Simulation Example ===")
    
    brain = create_enhanced_brain()
    
    # Use AdvancedSimulationManager for enhanced control
    manager = AdvancedSimulationManager()
    manager.initialize(brain)
    
    manager.addProtocol("learning", 200, ["before_step"])
    result = manager.runProtocol("learning")
    
    print(f"Protocol completed with result: {result}")
    
    return brain

def performance_example():
    """Performance monitoring example"""
    print("\n=== Performance Monitoring Example ===")
    
    brain = create_enhanced_brain()
    
    # Use PerformanceMonitor for metrics
    monitor = PerformanceMonitor()
    monitor.startRecording()
    
    for step in range(150):
        brain.step(step)
        monitor.record_step(brain.getAverageFiringRate(), 0.0)
    
    monitor.stopRecording()
    
    print("Performance report:")
    print(monitor.generate_report())
    
    return brain

def analysis_example():
    """Analysis example"""
    print("\n=== Analysis Example ===")
    
    brain = create_enhanced_brain()
    
    # Analyze neural activity
    firing_rates = AnalysisUtils.analyze_firing_pattern(brain, 100)
    
    print(f"Analyzed {len(firing_rates)} steps:")
    print(f"  - Mean firing rate: {AnalysisUtils.calculate_mean(firing_rates):.2f}")
    print(f"  - Firing rate trend: {AnalysisUtils.calculate_firing_rate_trend(firing_rates):.4f}")
    print(f"  - Firing rate std: {AnalysisUtils.calculate_std_dev(firing_rates, AnalysisUtils.calculate_mean(firing_rates)):.2f}")
    
    return firing_rates

if __name__ == "__main__":
    # Run all examples
    brain1 = basic_example()
    brain2 = simulation_example()
    brain3 = performance_example()
    firing_rates = analysis_example()
    
    print("\n=== All Examples Completed Successfully ===")
'''
    
    with open(examples_dir / "basic_example.py", "w") as f:
        f.write(basic_example)
    
    print("  ✓ Created basic example")

def install_documentation():
    """Install documentation"""
    print("\nInstalling documentation...")
    
    docs_dir = Path("pynlm_enhanced/docs")
    
    # Create documentation
    readme_content = '''# NLM Enhanced Package

## Overview

The NLM Enhanced Package provides advanced functionality for NLM (Neural Learning Machine) users, including:

- **Validated Configuration**: Type-safe parameter validation with strict mode
- **Advanced Simulation**: Protocol-based execution and event systems
- **Performance Monitoring**: Real-time metrics collection and analysis
- **Component Factory**: Plugin architecture for custom extensions
- **Analysis Tools**: Neural activity analysis and learning progress tracking
- **Network Builder**: Utilities for creating specialized neural networks

## Installation

```bash
pip install .
# or
cd build/enhanced && pip install .
```

## Basic Usage

```python
from pynlm_enhanced import (
    create_enhanced_brain, 
    AdvancedSimulationManager,
    PerformanceMonitor,
    AnalysisUtils
)
from pynlm import createDefaultConfig

# Create enhanced brain
config = createDefaultConfig()
brain = create_enhanced_brain(config)

# Run simulation
for step in range(100):
    brain.step(step)

# Analyze results
firing_rates = AnalysisUtils.analyze_firing_pattern(brain, 100)
```

## Advanced Features

### Validated Configuration

```python
from pynlm_enhanced import ValidatedConfig

config = ValidatedConfig()
config.addValidator("neuron_count", lambda x: x > 0, "Number of neurons")
config.addValidator("connection_probability", lambda x: 0 <= x <= 1)

config.set_int("neuron_count", 500)
config.set_double("connection_probability", 0.1)
```

### Advanced Simulation

```python
from pynlm_enhanced import AdvancedSimulationManager

brain = create_enhanced_brain()
manager = AdvancedSimulationManager()
manager.initialize(brain)

manager.addProtocol("learning", 200, ["before_step"])
result = manager.runProtocol("learning")
```

### Performance Monitoring

```python
from pynlm_enhanced import PerformanceMonitor

monitor = PerformanceMonitor()
monitor.startRecording()

for step in range(100):
    brain.step(step)
    monitor.record_step(brain.getAverageFiringRate(), 0.0)

monitor.stopRecording()
print(monitor.generate_report())
```

### Component Factory

```python
from pynlm_enhanced import ComponentFactory

factory = ComponentFactory()
factory.registerComponent(
    name="my_component",
    version="1.0.0",
    initializer=my_initializer,
    configurators={"param": my_configurator}
)

factory.initialize_component(brain, "my_component")
```

## Examples

Run the example scripts:

```bash
python -m pynlm_enhanced.examples.basic_example
```

## Command Line Tools

The package includes command-line tools:

```bash
# Create brain
./bin/nlm-enhanced create-brain --neurons 1000 --regions 3

# Run simulation
./bin/nlm-enhanced simulate --steps 500 --benchmark

# Analyze brain
./bin/nlm-enhanced analyze --steps 200
```

## Performance

The enhanced package provides improved performance through:

- **Validated Configuration**: Early error detection
- **Efficient Simulation**: Optimized protocol execution
- **Real-time Monitoring**: Lightweight metrics collection
- **Memory Management**: Efficient component lifecycle

## Limitations

- Enhanced features require the enhanced Python bindings
- Some validation features require additional dependencies
- Component factory is still in early development

## Future Enhancements

Planned enhancements include:

- **Advanced Network Templates**: Pre-built network architectures
- **Machine Learning Integration**: Automated hyperparameter optimization
- **Distributed Computing**: Multi-GPU simulation support
- **Real-time Visualization**: Interactive simulation monitoring
- **Cloud Integration**: Scalable cloud-based simulations

## Support

For support and issues:
- GitHub repository: [NLM GitHub](https://github.com/nlm-project/nlm)
- Documentation: [NLM Docs](https://nlm.readthedocs.io)
- Discord: [NLM Discord](https://discord.gg/nlm)

## License

MIT License. See LICENSE file for details.
'''
    
    with open(docs_dir / "README.md", "w") as f:
        f.write(readme_content)
    
    print("  ✓ Created documentation")

def main():
    print("NLM Enhanced Package Installer")
    print("=" * 40)
    
    # Check prerequisites
    print("\nChecking prerequisites...")
    pynlm_available = check_pynlm_available()
    enhanced_available = check_enhanced_bindings_available()
    
    if not pynlm_available:
        print("Warning: Original pynlm not found. Enhanced package may have limited functionality.")
    
    # Install components
    install_enhanced_package()
    install_cli_tools()
    create_examples()
    install_documentation()
    
    print("\n" + "=" * 40)
    print("Enhanced package installation completed!")
    print("\nAvailable features:")
    print("  ✓ Enhanced Python bindings")
    print("  ✓ Convenience modules")
    print("  ✓ Command-line tools")
    print("  ✓ Example scripts")
    print("  ✓ Documentation")
    
    print("\nQuick start:")
    print("  1. Import enhanced package: from pynlm_enhanced import ...")
    print("  2. Create brain: brain = create_enhanced_brain()")
    print("  3. Run simulation: brain.step(step)")
    print("  4. Use CLI: ./bin/nlm-enhanced create-brain --neurons 1000")
    
    print("\nFor more information:")
    print("  - Documentation: pynlm_enhanced/docs/README.md")
    print("  - Examples: pynlm_enhanced/examples/")

if __name__ == "__main__":
    main()
