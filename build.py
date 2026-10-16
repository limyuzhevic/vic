#!/usr/bin/env python3
"""
Development and Build Script for NLM Enhanced Python Bindings

This script provides comprehensive development and build capabilities
for the NLM Python bindings, including testing, documentation generation,
and deployment.
"""

import os
import sys
import subprocess
import argparse
import json
import time
import platform
from pathlib import Path
from typing import Dict, List, Any, Optional

# Global configuration
CONFIG = {
    'python_bindings': True,
    'advanced_features': True,
    'testing': True,
    'documentation': True,
    'build_type': 'Release',
    'generator': 'Ninja' if platform.system() != 'Windows' else 'Visual Studio 17 2022',
    'parallel_jobs': os.cpu_count() or 4,
}

def print_header(title: str):
    """Print formatted header."""
    print(f"\n{'=' * 60}")
    print(f"{title}")
    print(f"{'=' * 60}")

def print_step(step: int, description: str):
    """Print formatted step."""
    print(f"\n{step}. {description}")
    print("-" * 40)

def run_command(cmd: List[str], cwd: Optional[str] = None, env: Optional[Dict] = None) -> bool:
    """Run command and return success status."""
    try:
        print(f"Running: {' '.join(cmd)}")
        result = subprocess.run(cmd, cwd=cwd, env=env, check=True,
                              capture_output=True, text=True)
        if result.stdout:
            print("STDOUT:", result.stdout[:500] if len(result.stdout) > 500 else result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print(f"ERROR: Command failed with exit code {e.returncode}")
        print(f"STDERR: {e.stderr[:500] if e.stderr else 'No stderr'}")
        return False

def setup_python_environment():
    """Setup Python development environment."""
    print_step(1, "Setting up Python development environment")
    
    # Check Python version
    python_version = sys.version_info
    print(f"Python version: {python_version.major}.{python_version.minor}.{python_version.micro}")
    
    if python_version.major < 3 or (python_version.major == 3 and python_version.minor < 8):
        print("ERROR: Python 3.8 or higher is required")
        return False
    
    # Install package in development mode
    if not run_command([sys.executable, "-m", "pip", "install", "-e", ".[dev]"]):
        return False
    
    return True

def setup_cpp_environment():
    """Setup C++ build environment."""
    print_step(2, "Setting up C++ build environment")
    
    # Create build directory
    build_dir = Path("build")
    if not build_dir.exists():
        build_dir.mkdir(parents=True, exist_ok=True)
    
    # Configure CMake
    cmake_cmd = [
        "cmake", "..",
        f"-DCMAKE_BUILD_TYPE={CONFIG['build_type']}",
        f"-GNinja" if platform.system() != 'Windows' else f"-G{CONFIG['generator']}",
    ]
    
    if CONFIG['python_bindings']:
        cmake_cmd.append("-DNLM_ENABLE_PYTHON_BINDINGS=ON")
    
    if CONFIG['advanced_features']:
        cmake_cmd.append("-DNLM_ENABLE_ADVANCED_FEATURES=ON")
    
    if CONFIG['testing']:
        cmake_cmd.append("-DNLM_ENABLE_TESTING=ON")
    
    if not run_command(cmake_cmd, cwd=str(build_dir)):
        return False
    
    # Build
    build_cmd = ["make", "-j", str(CONFIG['parallel_jobs'])]
    if not run_command(build_cmd, cwd=str(build_dir)):
        return False
    
    return True

def run_python_tests():
    """Run Python tests."""
    print_step(3, "Running Python tests")
    
    # Install test dependencies
    if not run_command([sys.executable, "-m", "pip", "install", "pytest", "pytest-cov", "numpy"]):
        return False
    
    # Run tests with coverage
    test_cmd = [
        sys.executable, "-m", "pytest",
        "tests/", "-v",
        "--tb=short",
        f"--cov=pynlm",
        f"--cov-report=term-missing",
        f"--cov-report=html:htmlcov"
    ]
    
    if not run_command(test_cmd):
        return False
    
    return True

def run_cpp_tests():
    """Run C++ tests."""
    print_step(4, "Running C++ tests")
    
    if not run_command(["ctest", "--output-on-failure"], cwd="build"):
        return False
    
    return True

def build_examples():
    """Build example scripts."""
    print_step(5, "Building example scripts")
    
    # Create examples directory if it doesn't exist
    examples_dir = Path("examples")
    if not examples_dir.exists():
        examples_dir.mkdir(parents=True, exist_ok=True)
    
    # Create __init__.py for package support
    init_file = examples_dir / "__init__.py"
    with open(init_file, "w") as f:
        f.write('"""NLM Python examples and tutorials."""\n\n')
        f.write('__all__ = [\n')
        f.write('    "advanced_usage",\n')
        f.write('    "tutorial",\n')
        f.write('    "benchmarks"\n')
        f.write(']\n')
    
    # Create example files if they don't exist
    advanced_example = examples_dir / "advanced_usage.py"
    if not advanced_example.exists():
        create_advanced_example(advanced_example)
    
    tutorial_example = examples_dir / "tutorial.py"
    if not tutorial_example.exists():
        create_tutorial_example(tutorial_example)
    
    return True

def create_advanced_example(filepath: Path):
    """Create advanced usage example."""
    content = '''#!/usr/bin/env python3
"""
Advanced NLM Usage Examples

Demonstrates advanced usage patterns and best practices for the NLM Python API.
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
import json
import time
from typing import List, Dict, Optional, Callable, Any

def create_advanced_brain(config_overrides: Optional[Dict[str, Any]] = None):
    """
    Create an advanced brain with specific configuration.
    
    Args:
        config_overrides: Dictionary of configuration parameter overrides
        
    Returns:
        Configured and initialized brain instance
    """
    config = pynlm.createDefaultConfig()
    
    # Apply common advanced configurations
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value)
    
    # Set some sensible defaults for advanced usage
    if not config.has("brain.neuron_count"):
        config.set("brain.neuron_count", 5000)
    if not config.has("brain.synapse_density"):
        config.set("brain.synapse_density", 0.1)
    if not config.has("brain.region_count"):
        config.set("brain.region_count", 10)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

def create_training_agent(brain, world, enable_learning=True):
    """
    Create a complete training agent with all learning systems enabled.
    
    Args:
        brain: The neural brain
        world: The simulation world
        enable_learning: Whether to enable learning systems
        
    Returns:
        Configured agent brain interface
    """
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    if enable_learning:
        # Enable all learning systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    return agent

def run_episode(brain, world, agent, num_steps: int, 
               with_reward: bool = True, with_curiosity: bool = False):
    """
    Run a complete episode with the given agent.
    
    Args:
        brain: Neural brain
        world: Simulation world
        agent: Agent brain interface
        num_steps: Number of simulation steps
        with_reward: Whether to apply reward modulation
        with_curiosity: Whether to track curiosity
        
    Returns:
        Dictionary containing episode statistics
    """
    episode_data = {
        'steps': num_steps,
        'initial_neurons': brain.getTotalNeuronCount(),
        'initial_spikes': brain.getTotalSpikeCount(),
        'firing_rates': [],
        'rewards': [],
        'curiosity_levels': [],
        'actions_taken': 0,
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward if enabled
        if with_reward:
            # Simple reward based on action success
            reward = 0.1  # Small positive reward
            predicted = 0.05  # Predicted reward
            agent.applyRewardModulation(reward, predicted)
            episode_data['rewards'].append(reward)
        
        # Track curiosity if enabled
        if with_curiosity:
            curiosity = agent.getCuriosityLevel()
            episode_data['curiosity_levels'].append(curiosity)
        
        # Record firing rate
        firing_rate = brain.getAverageFiringRate()
        episode_data['firing_rates'].append(firing_rate)
        
        # Count actions
        if action != pynlm.MotorCommand.Wait:
            episode_data['actions_taken'] += 1
    
    episode_data['final_neurons'] = brain.getTotalNeuronCount()
    episode_data['final_spikes'] = brain.getTotalSpikeCount()
    
    return episode_data

def train_agent(training_episodes: int, brain_config: Optional[Dict] = None,
               render_interval: int = 10):
    """
    Train an agent using the NLM framework.
    
    Args:
        training_episodes: Number of training episodes
        brain_config: Brain configuration parameters
        render_interval: Render interval for visualization
        
    Returns:
        Trained brain and training statistics
    """
    # Create brain
    brain = create_advanced_brain(brain_config)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Create agent
    agent = create_training_agent(brain, world, enable_learning=True)
    
    # Training loop
    training_stats = {
        'episode': [],
        'total_spikes': [],
        'average_firing_rate': [],
        'total_actions': [],
        'final_development_stage': [],
    }
    
    for episode in range(training_episodes):
        episode_data = run_episode(
            brain, world, agent, 
            num_steps=100,
            with_reward=True,
            with_curiosity=True
        )
        
        # Record statistics
        training_stats['episode'].append(episode)
        training_stats['total_spikes'].append(episode_data['final_spikes'] - episode_data['initial_spikes'])
        training_stats['average_firing_rate'].append(np.mean(episode_data['firing_rates']))
        training_stats['total_actions'].append(episode_data['actions_taken'])
        training_stats['final_development_stage'].append(agent.getDevelopmentalStage())
        
        # Reset for next episode
        brain.reset()
        world.reset()
        agent.reset()
        
        # Print progress
        if episode % render_interval == 0:
            print(f"Episode {episode}: "
                  f"Spikes: {episode_data['final_spikes'] - episode_data['initial_spikes']}, "
                  f"Firing Rate: {np.mean(episode_data['firing_rates']):.3f}")
    
    return brain, training_stats

def save_training_results(brain, stats: Dict, filename: str):
    """
    Save training results to file.
    
    Args:
        brain: Trained brain
        stats: Training statistics
        filename: Output filename
    """
    results = {
        'brain_state': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'developmental_stage': brain.getDevelopmentalStage(),
        },
        'training_stats': stats
    }
    
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)

def plot_training_stats(stats: Dict):
    """
    Plot training statistics.
    
    Args:
        stats: Training statistics dictionary
    """
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot 1: Total spikes per episode
    axes[0, 0].plot(stats['episode'], stats['total_spikes'])
    axes[0, 0].set_title('Total Spikes per Episode')
    axes[0, 0].set_xlabel('Episode')
    axes[0, 0].set_ylabel('Spikes')
    axes[0, 0].grid(True)
    
    # Plot 2: Average firing rate per episode
    axes[0, 1].plot(stats['episode'], stats['average_firing_rate'])
    axes[0, 1].set_title('Average Firing Rate per Episode')
    axes[0, 1].set_xlabel('Episode')
    axes[0, 1].set_ylabel('Firing Rate')
    axes[0, 1].grid(True)
    
    # Plot 3: Total actions per episode
    axes[1, 0].plot(stats['episode'], stats['total_actions'])
    axes[1, 0].set_title('Total Actions per Episode')
    axes[1, 0].set_xlabel('Episode')
    axes[1, 0].set_ylabel('Actions')
    axes[1, 0].grid(True)
    
    # Plot 4: Development stage per episode
    stage_names = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
    stage_values = [pynlm.DevelopmentalStage.Initial, pynlm.DevelopmentalStage.CriticalPeriod,
                   pynlm.DevelopmentalStage.Maturation, pynlm.DevelopmentalStage.Adult,
                   pynlm.DevelopmentalStage.Aging]
    
    # Count development stages
    stage_counts = {stage: 0 for stage in stage_values}
    for stage in stats['final_development_stage']:
        if stage in stage_counts:
            stage_counts[stage] += 1
    
    axes[1, 1].bar(stage_names, [stage_counts[stage] for stage in stage_values])
    axes[1, 1].set_title('Development Stage Distribution')
    axes[1, 1].set_xlabel('Development Stage')
    axes[1, 1].set_ylabel('Count')
    axes[1, 1].tick_params(axis='x', rotation=45)
    
    plt.tight_layout()
    plt.show()

def create_neural_network_visualization(brain, output_file: str = "brain_network.png"):
    """
    Create a visualization of the neural network.
    
    Args:
        brain: Neural brain to visualize
        output_file: Output filename
    """
    try:
        import networkx as nx
        import matplotlib.pyplot as plt
        
        # Get all regions
        regions = brain.getRegions()
        region_ids = brain.getRegionIds()
        
        # Create a network graph
        G = nx.Graph()
        
        # Add nodes for regions
        for region_id in region_ids:
            region = brain.getRegion(region_id)
            if region:
                G.add_node(region_id, label=region.getName())
        
        # Get neuron connections (simplified for visualization)
        total_neurons = brain.getTotalNeuronCount()
        
        # Create a simple layout
        pos = nx.spring_layout(G, k=2, iterations=50)
        
        # Draw the graph
        plt.figure(figsize=(12, 8))
        
        # Draw nodes
        nx.draw_networkx_nodes(G, pos, node_color='lightblue', 
                              node_size=500, alpha=0.8)
        
        # Draw edges
        nx.draw_networkx_edges(G, pos, alpha=0.5, width=1)
        
        # Draw labels
        labels = nx.get_node_attributes(G, 'label')
        nx.draw_networkx_labels(G, pos, labels, font_size=8)
        
        # Add title and info
        plt.title(f'NLM Neural Network Visualization\n'
                 f'Total Neurons: {total_neurons}, '
                 f'Total Regions: {len(regions)}')
        
        plt.axis('off')
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"Neural network visualization saved to {output_file}")
        
    except ImportError:
        print("networkx or matplotlib not available for visualization")

if __name__ == "__main__":
    print("Advanced NLM Usage Examples")
    print("=" * 50)
    
    # Example 1: Basic brain creation
    print("\n1. Creating an advanced brain...")
    brain = create_advanced_brain({
        "brain.neuron_count": 2000,
        "brain.synapse_density": 0.05
    })
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Example 2: Training loop
    print("\n2. Training agent...")
    trained_brain, stats = train_agent(
        training_episodes=5,
        brain_config={"brain.neuron_count": 1000},
        render_interval=1
    )
    
    # Save results
    save_training_results(trained_brain, stats, "training_results.json")
    
    # Plot statistics
    plot_training_stats(stats)
    
    # Visualize network
    create_neural_network_visualization(trained_brain)
    
    print("\nAdvanced examples completed successfully!")
'''
    
    with open(filepath, "w") as f:
        f.write(content)

def create_tutorial_example(filepath: Path):
    """Create tutorial example."""
    content = '''#!/usr/bin/env python3
"""
NLM Tutorial: Getting Started with Neural Simulation

This tutorial provides a step-by-step introduction to using the NLM Python bindings
to create and simulate artificial neural systems.
"""

import pynlm
import numpy as np

def step_1_create_brain():
    """Step 1: Create a basic brain."""
    print("Step 1: Creating a basic brain...")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    print(f"Created default config with keys: {config.getKeys()}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    return brain

def step_2_simulation_loop(brain, steps=10):
    """Step 2: Run a simulation loop."""
    print(f"\nStep 2: Running {steps} simulation steps...")
    
    initial_spikes = brain.getTotalSpikeCount()
    initial_neurons = brain.getTotalNeuronCount()
    
    for step in range(steps):
        brain.step(step)
        
        if step % 5 == 0:
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            firing_neurons = brain.getFiringNeuronCount()
            
            print(f"  Step {step}: "
                  f"Firing={firing_rate:.3f}Hz, "
                  f"Active={active_neurons}/{initial_neurons}, "
                  f"Firing={firing_neurons}")
    
    total_spikes = brain.getTotalSpikeCount()
    final_spikes = total_spikes - initial_spikes
    
    print(f"Simulation complete! Total spikes generated: {final_spikes}")
    return brain

def step_3_create_world_and_agent():
    """Step 3: Create world and agent."""
    print("\nStep 3: Creating world and agent...")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    print(f"Created world: {world.getWidth()}x{world.getHeight()} units")
    print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()} pixels")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Created agent with reward modulation and curiosity enabled")
    
    return brain, world, agent

def step_4_complete_agent_simulation(brain, world, agent, steps=20):
    """Step 4: Run complete agent simulation."""
    print(f"\nStep 4: Running complete agent simulation for {steps} steps...")
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        if step % 10 == 0:
            agent.applyRewardModulation(1.0, 0.5)
        
        # Print status every 5 steps
        if step % 5 == 0:
            firing_rate = brain.getAverageFiringRate()
            neuromodulation = agent.getNeuromodulationLevel()
            curiosity = agent.getCuriosityLevel()
            
            print(f"  Step {step}: "
                  f"Firing={firing_rate:.3f}Hz, "
                  f"Neuro={neuromodulation:.3f}, "
                  f"Curiosity={curiosity:.3f}, "
                  f"Action={action}")
    
    print(f"Simulation complete!")
    return brain

def step_5_development_system(brain):
    """Step 5: Explore development system."""
    print("\nStep 5: Exploring development system...")
    
    stages = [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    
    stage_names = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
    
    print("Developmental stages progression:")
    for i, stage in enumerate(stages):
        brain.setDevelopmentalStage(stage)
        print(f"  {stage_names[i]}: {brain.getDevelopmentalStage()}")
        
        # Show some stage-specific properties
        if stage == pynlm.DevelopmentalStage.CriticalPeriod:
            print(f"    -> High plasticity, rapid learning")
        elif stage == pynlm.DevelopmentalStage.Maturation:
            print(f"    -> Maturation of neural circuits")
        elif stage == pynlm.DevelopmentalStage.Aging:
            print(f"    -> Reduced plasticity, maintenance")
    
    return brain

def step_6_memory_systems(brain):
    """Step 6: Explore memory systems."""
    print("\nStep 6: Exploring memory systems...")
    
    # Access memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    print(f"Working memory capacity: {working_memory.getCapacity() if working_memory else 'N/A'}")
    print(f"Episodic memory capacity: {episodic_memory.getCapacity() if episodic_memory else 'N/A'}")
    
    # Show memory statistics
    if working_memory:
        print(f"Working memory current size: {working_memory.getSize()}")
    
    return brain

def step_7_neuromodulation(agent):
    """Step 7: Explore neuromodulation."""
    print("\nStep 7: Exploring neuromodulation...")
    
    print("Neuromodulation levels:")
    print(f"  Reward modulation: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    # Enable all systems
    agent.enable_all_systems()
    print("Enabled all learning systems (reward, curiosity, development)")
    
    return agent

def main():
    """Main tutorial function."""
    print("NLM Python Bindings Tutorial")
    print("=" * 40)
    
    try:
        # Step 1: Create brain
        brain = step_1_create_brain()
        
        # Step 2: Run simulation
        brain = step_2_simulation_loop(brain, 10)
        
        # Step 3: Create world and agent
        brain, world, agent = step_3_create_world_and_agent()
        
        # Step 4: Complete simulation
        brain = step_4_complete_agent_simulation(brain, world, agent, 20)
        
        # Step 5: Development system
        brain = step_5_development_system(brain)
        
        # Step 6: Memory systems
        brain = step_6_memory_systems(brain)
        
        # Step 7: Neuromodulation
        agent = step_7_neuromodulation(agent)
        
        print("\n" + "=" * 40)
        print("Tutorial completed successfully!")
        print("\nNext steps:")
        print("1. Try the advanced usage examples")
        print("2. Run the test suite to verify functionality")
        print("3. Explore the documentation for advanced features")
        print("4. Create your own simulation scenarios")
        
    except Exception as e:
        print(f"\nERROR: Tutorial failed with error: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
'''
    
    with open(filepath, "w") as f:
        f.write(content)

def create_benchmarks():
    """Create benchmark scripts."""
    print_step(6, "Creating benchmark scripts")
    
    benchmarks_dir = Path("benchmarks")
    if not benchmarks_dir.exists():
        benchmarks_dir.mkdir(parents=True, exist_ok=True)
    
    # Create __init__.py
    init_file = benchmarks_dir / "__init__.py"
    with open(init_file, "w") as f:
        f.write('"""NLM performance benchmarks."""\n\n')
        f.write('__all__ = [\n')
        f.write('    "performance_test",\n')
        f.write('    "memory_usage",\n')
        f.write('    "comparative_benchmark"\n')
        f.write(']\n')
    
    # Create performance benchmark
    perf_benchmark = benchmarks_dir / "performance_test.py"
    if not perf_benchmark.exists():
        create_performance_benchmark(perf_benchmark)
    
    return True

def create_performance_benchmark(filepath: Path):
    """Create performance benchmark script."""
    content = '''#!/usr/bin/env python3
"""
Performance benchmark for NLM Python bindings.

This script benchmarks the performance of the NLM neural simulation framework,
testing different brain configurations and measuring computational requirements.
"""

import pynlm
import time
import statistics
import matplotlib.pyplot as plt
import numpy as np
from typing import Dict, List, Any

def benchmark_brain_performance(brain, num_trials: int = 5, steps_per_trial: int = 1000):
    """
    Benchmark brain performance across multiple trials.
    
    Args:
        brain: Brain to benchmark
        num_trials: Number of benchmark trials
        steps_per_trial: Number of simulation steps per trial
        
    Returns:
        Dictionary with performance metrics
    """
    print(f"Benchmarking brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Running {num_trials} trials of {steps_per_trial} steps each")
    
    trial_times = []
    spike_counts = []
    firing_rates = []
    
    for trial in range(num_trials):
        # Warm-up (important for consistent results)
        for i in range(100):
            brain.step(i)
        
        # Benchmark
        start_time = time.time()
        trial_spikes_start = brain.getTotalSpikeCount()
        trial_firing_start = brain.getAverageFiringRate()
        
        for i in range(steps_per_trial):
            brain.step(i)
        
        end_time = time.time()
        trial_spikes_end = brain.getTotalSpikeCount()
        trial_firing_end = brain.getAverageFiringRate()
        
        trial_time = end_time - start_time
        trial_spikes = trial_spikes_end - trial_spikes_start
        trial_firing = (trial_firing_end - trial_firing_start) / 2  # Average over trial
        
        trial_times.append(trial_time)
        spike_counts.append(trial_spikes)
        firing_rates.append(trial_firing)
        
        print(f"  Trial {trial + 1}: {trial_time:.3f}s "
              f"({steps_per_trial/trial_time:.0f} steps/sec), "
              f"Spikes={trial_spikes}, "
              f"AvgFiring={trial_firing:.3f}Hz")
        
        # Reset for next trial
        brain.reset()
    
    # Calculate statistics
    avg_time = statistics.mean(trial_times)
    std_time = statistics.stdev(trial_times) if len(trial_times) > 1 else 0.0
    avg_spikes = statistics.mean(spike_counts)
    std_spikes = statistics.stdev(spike_counts) if len(spike_counts) > 1 else 0.0
    avg_firing = statistics.mean(firing_rates)
    std_firing = statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0.0
    
    avg_perf = steps_per_trial / avg_time
    std_perf = (steps_per_trial / avg_time) * (std_time / avg_time) if avg_time > 0 else 0.0
    
    # Prepare results
    results = {
        'total_spikes': brain.getTotalSpikeCount(),
        'average_time': avg_time,
        'std_time': std_time,
        'average_spikes_per_trial': avg_spikes,
        'std_spikes': std_spikes,
        'average_firing_rate': avg_firing,
        'std_firing': std_firing,
        'average_performance': avg_perf,
        'std_performance': std_perf,
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'trials': num_trials,
        'steps_per_trial': steps_per_trial
    }
    
    print(f"\nPerformance Summary:")
    print(f"  Average time: {avg_time:.3f} ± {std_time:.3f}s")
    print(f"  Average performance: {avg_perf:.0f} ± {std_perf:.0f} steps/sec")
    print(f"  Average spikes per trial: {avg_spikes:.0f} ± {std_spikes:.0f}")
    print(f"  Average firing rate: {avg_firing:.3f} ± {std_firing:.3f}Hz")
    print(f"  Total spikes after all trials: {results['total_spikes']}")
    
    return results

def benchmark_memory_usage(brain, steps: int = 1000):
    """
    Benchmark memory usage.
    
    Args:
        brain: Brain to benchmark
        steps: Number of simulation steps
        
    Returns:
        Memory usage statistics
    """
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    
    print(f"Benchmarking memory usage over {steps} steps...")
    
    # Measure baseline memory
    baseline_mem = process.memory_info().rss / 1024 / 1024  # MB
    print(f"Baseline memory: {baseline_mem:.2f} MB")
    
    # Run simulation and monitor memory
    memory_samples = []
    time_samples = []
    
    for step in range(steps):
        if step % 100 == 0:
            current_mem = process.memory_info().rss / 1024 / 1024
            memory_samples.append(current_mem)
            time_samples.append(step)
            
            if step % 500 == 0:
                print(f"  Step {step}: {current_mem:.2f} MB")
        
        brain.step(step)
    
    final_mem = process.memory_info().rss / 1024 / 1024
    memory_increase = final_mem - baseline_mem
    
    print(f"Final memory: {final_mem:.2f} MB")
    print(f"Memory increase: {memory_increase:.2f} MB ({memory_increase/baseline_mem*100:.1f}%)")
    
    # Calculate statistics
    if memory_samples:
        avg_mem = statistics.mean(memory_samples)
        std_mem = statistics.stdev(memory_samples) if len(memory_samples) > 1 else 0.0
    else:
        avg_mem = final_mem
        std_mem = 0.0
    
    return {
        'baseline_memory': baseline_mem,
        'final_memory': final_mem,
        'memory_increase': memory_increase,
        'average_memory': avg_mem,
        'std_memory': std_mem,
        'memory_samples': len(memory_samples)
    }

def benchmark_configuration_space(configurations: List[Dict], steps: int = 100):
    """
    Benchmark different brain configurations.
    
    Args:
        configurations: List of brain configurations to test
        steps: Number of simulation steps for each configuration
        
    Returns:
        Comparison results
    """
    print("Benchmarking configuration space...")
    
    results = []
    
    for i, config in enumerate(configurations):
        print(f"\nConfiguration {i + 1}: {config}")
        
        # Create brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Apply configuration
        for key, value in config.items():
            brain.getConfig().set(key, value)
        
        brain.initialize()
        
        # Benchmark
        perf_results = benchmark_brain_performance(brain, num_trials=3, steps_per_trial=200)
        perf_results['config'] = config
        results.append(perf_results)
        
        print(f"  Neurons: {config.get('brain.neuron_count', 'default')}")
        print(f"  Performance: {perf_results['average_performance']:.0f} steps/sec")
    
    return results

def create_performance_plots(results: List[Dict], output_dir: str = "plots"):
    """
    Create performance comparison plots.
    
    Args:
        results: Benchmark results
        output_dir: Output directory for plots
    """
    import os
    
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # Extract data
    configs = [r['config'] for r in results]
    performances = [r['average_performance'] for r in results]
    firing_rates = [r['average_firing_rate'] for r in results]
    neurons = [r['neurons'] for r in results]
    
    # Create comparison plot
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot 1: Performance comparison
    axes[0, 0].bar(range(len(configs)), performances, alpha=0.7, edgecolor='black')
    axes[0, 0].set_title('Performance Comparison (steps/sec)')
    axes[0, 0].set_xlabel('Configuration')
    axes[0, 0].set_ylabel('Performance (steps/sec)')
    axes[0, 0].set_xticks(range(len(configs)))
    axes[0, 0].set_xticklabels([f'C{i+1}' for i in range(len(configs))])
    
    # Plot 2: Firing rate comparison
    axes[0, 1].bar(range(len(configs)), firing_rates, alpha=0.7, edgecolor='black', color='orange')
    axes[0, 1].set_title('Average Firing Rate Comparison')
    axes[0, 1].set_xlabel('Configuration')
    axes[0, 1].set_ylabel('Firing Rate (Hz)')
    axes[0, 1].set_xticks(range(len(configs)))
    axes[0, 1].set_xticklabels([f'C{i+1}' for i in range(len(configs))])
    
    # Plot 3: Neurons vs Performance
    axes[1, 0].scatter(neurons, performances, alpha=0.7, s=100)
    for i, (neuron, perf, config) in enumerate(zip(neurons, performances, configs)):
        axes[1, 0].annotate(f'C{i+1}', (neuron, perf), xytext=(5, 5), textcoords='offset points')
    axes[1, 0].set_title('Neurons vs Performance')
    axes[1, 0].set_xlabel('Number of Neurons')
    axes[1, 0].set_ylabel('Performance (steps/sec)')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Performance distribution
    axes[1, 1].boxplot(performances, labels=[f'C{i+1}' for i in range(len(configs))])
    axes[1, 1].set_title('Performance Distribution')
    axes[1, 1].set_ylabel('Performance (steps/sec)')
    
    plt.tight_layout()
    plot_path = os.path.join(output_dir, 'performance_comparison.png')
    plt.savefig(plot_path, dpi=150, bbox_inches='tight')
    print(f"Performance plot saved to {plot_path}")
    
    return plot_path

def main():
    """Main benchmark function."""
    print("NLM Performance Benchmark Suite")
    print("=" * 40)
    
    parser = argparse.ArgumentParser(description="Benchmark NLM Python bindings")
    parser.add_argument("--memory", action="store_true", help="Run memory benchmark")
    parser.add_argument("--config-space", action="store_true", help="Run configuration space benchmark")
    parser.add_argument("-o", "--output", default="benchmark_results.json", help="Output file for results")
    
    args = parser.parse_args()
    
    all_results = {}
    
    # Create brain for benchmarks
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    try:
        # Performance benchmark
        print("\n1. Performance Benchmark")
        print("-" * 30)
        perf_results = benchmark_brain_performance(brain, num_trials=5, steps_per_trial=500)
        all_results['performance'] = perf_results
        
        # Memory benchmark
        if args.memory:
            print("\n2. Memory Usage Benchmark")
            print("-" * 30)
            memory_results = benchmark_memory_usage(brain, steps=1000)
            all_results['memory'] = memory_results
        
        # Configuration space benchmark
        if args.config_space:
            print("\n3. Configuration Space Benchmark")
            print("-" * 30)
            
            configurations = [
                {"brain.neuron_count": 500},
                {"brain.neuron_count": 1000},
                {"brain.neuron_count": 2000},
                {"brain.neuron_count": 5000},
                {"brain.neuron_count": 10000}
            ]
            
            config_results = benchmark_configuration_space(configurations, steps=200)
            all_results['configuration_space'] = config_results
            
            # Create plots
            plot_path = create_performance_plots(config_results)
            all_results['performance_plot'] = plot_path
        
        # Save results
        with open(args.output, 'w') as f:
            json.dump(all_results, f, indent=2)
        
        print(f"\nBenchmark results saved to {args.output}")
        
        # Print summary
        print("\nBenchmark Summary:")
        print("=" * 40)
        for benchmark_name, results in all_results.items():
            if benchmark_name == 'performance':
                print(f"Performance: {results['average_performance']:.0f} steps/sec")
            elif benchmark_name == 'memory':
                print(f"Memory: {results['memory_increase']:.2f} MB increase")
            elif benchmark_name == 'configuration_space':
                print(f"Configuration space: {len(results)} configurations tested")
            elif benchmark_name == 'performance_plot':
                print(f"Plot generated: {results}")
        
        return 0
        
    except Exception as e:
        print(f"ERROR: Benchmark failed with error: {e}")
        return 1

if __name__ == "__main__":
    exit(main())
'''
    
    with open(filepath, "w") as f:
        f.write(content)

def main():
    """Main build script."""
    print_header("NLM Development and Build Script")
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Build and test NLM Python bindings")
    parser.add_argument("--skip-python", action="store_true", help="Skip Python setup")
    parser.add_argument("--skip-cpp", action="store_true", help="Skip C++ build")
    parser.add_argument("--skip-tests", action="store_true", help="Skip tests")
    parser.add_argument("--skip-examples", action="store_true", help="Skip example building")
    parser.add_argument("--skip-benchmarks", action="store_true", help="Skip benchmarks")
    parser.add_argument("--python-only", action="store_true", help="Only run Python tasks")
    parser.add_argument("--cpp-only", action="store_true", help="Only run C++ tasks")
    
    args = parser.parse_args()
    
    # Determine what to run
    run_python = not args.cpp_only and not args.skip_python
    run_cpp = not args.python_only and not args.skip_cpp
    run_tests = not args.skip_tests
    run_examples = not args.skip_examples
    run_benchmarks = not args.skip_benchmarks
    
    success = True
    
    # Run Python setup
    if run_python:
        if not setup_python_environment():
            success = False
    
    # Run C++ build
    if run_cpp:
        if not setup_cpp_environment():
            success = False
    
    # Run tests
    if run_tests:
        if run_python and not run_python_tests():
            success = False
        if run_cpp and not run_cpp_tests():
            success = False
    
    # Build examples
    if run_examples:
        if not build_examples():
            success = False
    
    # Create benchmarks
    if run_benchmarks:
        if not create_benchmarks():
            success = False
    
    # Print summary
    print_header("Build Summary")
    
    if success:
        print("✓ All tasks completed successfully!")
        print("\nNext steps:")
        print("1. Run tests: pytest tests/ -v")
        print("2. Run examples: python -m examples.advanced_usage")
        print("3. Build documentation: sphinx-build -b html docs/ docs/_build/html/")
        print("4. Deploy: pip install -e .[dev]")
    else:
        print("✗ Some tasks failed. Check output above for details.")
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
