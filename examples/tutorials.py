#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Practical Tutorials

This file contains practical tutorials for getting started with NLM,
including step-by-step guides and real-world examples.
"""

import nlm
import os
import json
from pathlib import Path

def tutorial_1_getting_started():
    """
    Tutorial 1: Getting Started with NLM
    
    This tutorial covers the basics of setting up and running your first
    NLM simulation.
    """
    print("=== Tutorial 1: Getting Started with NLM ===")
    print()
    
    print("Step 1: Install NLM")
    print("   pip install nlm")
    print()
    
    print("Step 2: Import the library")
    print("   import nlm")
    print()
    
    print("Step 3: Create your first brain")
    print("   brain = nlm.Brain()")
    print()
    
    print("Step 4: Run a simple simulation")
    brain = nlm.Brain()
    result = brain.run_steps(100)
    print(f"   Result: {result}")
    print()
    
    print("Step 5: Check the results")
    print(f"   Average reward: {result['total_reward']:.4f}")
    print(f"   Average firing rate: {result['average_firing_rate']:.2f} Hz")
    print(f"   Total spikes: {result['total_spikes']}")
    print()
    
    return True

def tutorial_2_configuration_management():
    """
    Tutorial 2: Configuration Management
    
    Learn how to create, modify, and save NLM configurations.
    """
    print("=== Tutorial 2: Configuration Management ===")
    print()
    
    print("Step 1: Create a configuration")
    print("   config = nlm.Config()")
    config = nlm.Config()
    print()
    
    print("Step 2: Set configuration parameters")
    print("   config.set('neuron_count', 1000)")
    print("   config.set('simulation_timestep', 0.001)")
    print("   config.set('random_seed', 12345)")
    config.set("neuron_count", 1000)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 12345)
    print()
    
    print("Step 3: Create brain with custom configuration")
    print("   brain = nlm.Brain(config)")
    brain = nlm.Brain(config)
    print()
    
    print("Step 4: Modify configuration")
    print("   config.set('connection_probability', 0.2)")
    config.set("connection_probability", 0.2)
    print()
    
    print("Step 5: Save configuration to file")
    filename = "my_config.json"
    print(f"   config.export_to_json('{filename}')")
    success = config.export_to_json(filename)
    print(f"   Success: {success}")
    print()
    
    print("Step 6: Load configuration from file")
    print(f"   new_config = nlm.Config()")
    print(f"   new_config.import_from_json('{filename}')")
    new_config = nlm.Config()
    new_config.import_from_json(filename)
    print(f"   Loaded keys: {new_config.get_keys()}")
    print()
    
    # Clean up
    if os.path.exists(filename):
        os.remove(filename)
    
    return True

def tutorial_3_memory_systems():
    """
    Tutorial 3: Working with Memory Systems
    
    Learn how to use NLM's memory systems for storing and retrieving experiences.
    """
    print("=== Tutorial 3: Working with Memory Systems ===")
    print()
    
    print("Step 1: Create brain and run simulation")
    brain = nlm.Brain()
    result = brain.run_steps(200)
    print(f"   Simulation completed with reward: {result['total_reward']:.4f}")
    print()
    
    print("Step 2: Check working memory status")
    working_mem = brain.get_working_memory()
    print(f"   Working memory: {working_mem}")
    print()
    
    print("Step 3: Check episodic memory status")
    episodic_mem = brain.get_episodic_memory()
    print(f"   Episodic memory: {episodic_mem}")
    print()
    
    print("Step 4: Analyze memory efficiency")
    working_traces = working_mem.get("active_traces", 0)
    episodic_episodes = episodic_mem.get("episode_count", 0)
    
    print(f"   Working memory traces: {working_traces}")
    print(f"   Episodic memory episodes: {episodic_episodes}")
    
    if working_traces > 0 or episodic_episodes > 0:
        print("   ✅ Memory systems are active and storing information")
    else:
        print("   ⚠️  Memory systems may need more activity to store data")
    print()
    
    return True

def tutorial_4_neuromodulation():
    """
    Tutorial 4: Understanding Neuromodulation
    
    Learn how neuromodulation systems affect learning and behavior.
    """
    print("=== Tutorial 4: Understanding Neuromodulation ===")
    print()
    
    print("Step 1: Create brain and run simulation")
    brain = nlm.Brain()
    result = brain.run_steps(100)
    print(f"   Simulation reward: {result['total_reward']:.4f}")
    print()
    
    print("Step 2: Check neuromodulation levels")
    neuromod = brain.get_neuromodulation()
    print(f"   Neuromodulation levels: {neuromod}")
    print()
    
    print("Step 3: Analyze neuromodulation impact")
    dopamine = neuromod.get("dopamine", 0)
    curiosity = neuromod.get("curiosity", 0)
    novelty = neuromod.get("novelty", 0)
    
    print(f"   Dopamine (reward signal): {dopamine}")
    print(f"   Curiosity (exploration): {curiosity}")
    print(f"   Novelty (unexpected events): {novelty}")
    print()
    
    print("Step 4: Interpret neuromodulation")
    if dopamine > 0.5:
        print("   🟢 High dopamine: Strong reward learning")
    else:
        print("   🟡 Low dopamine: Weak reward learning")
    
    if curiosity > 0.5:
        print("   🟢 High curiosity: Active exploration")
    else:
        print("   🟡 Low curiosity: Limited exploration")
    
    if novelty > 0.5:
        print("   🟢 High novelty: Sensitive to new experiences")
    else:
        print("   🟡 Low novelty: Less sensitive to novelty")
    print()
    
    return True

def tutorial_5_checkpointing():
    """
    Tutorial 5: Checkpoint Management
    
    Learn how to save and load brain states for interrupted simulations.
    """
    print("=== Tutorial 5: Checkpoint Management ===")
    print()
    
    print("Step 1: Create and train a brain")
    brain = nlm.Brain()
    print("   Training brain for 100 steps...")
    result = brain.run_steps(100)
    print(f"   Initial reward: {result['total_reward']:.4f}")
    print()
    
    print("Step 2: Save checkpoint")
    checkpoint_file = "brain_checkpoint.bin"
    print(f"   Saving to: {checkpoint_file}")
    success = brain.save_checkpoint(checkpoint_file)
    print(f"   Success: {success}")
    print()
    
    print("Step 3: Get brain statistics")
    initial_stats = brain.get_stats()
    print(f"   Initial total spikes: {initial_stats['total_spikes']}")
    print(f"   Initial neurons: {initial_stats['total_neurons']}")
    print()
    
    print("Step 4: Create new brain and load checkpoint")
    new_brain = nlm.Brain()
    print("   Loading checkpoint...")
    load_success = new_brain.load_checkpoint(checkpoint_file)
    print(f"   Load success: {load_success}")
    print()
    
    print("Step 5: Compare states")
    if load_success:
        loaded_stats = new_brain.get_stats()
        print(f"   Loaded total spikes: {loaded_stats['total_spikes']}")
        print(f"   Loaded neurons: {loaded_stats['total_neurons']}")
        
        if initial_stats['total_spikes'] == loaded_stats['total_spikes']:
            print("   ✅ Checkpoint/load successful - state preserved!")
        else:
            print("   ⚠️  Checkpoint/load may have issues - state not fully preserved")
    else:
        print("   ❌ Failed to load checkpoint")
    print()
    
    # Clean up
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
    
    return True

def tutorial_6_performance_monitoring():
    """
    Tutorial 6: Performance Monitoring
    
    Learn how to monitor and optimize NLM simulation performance.
    """
    print("=== Tutorial 6: Performance Monitoring ===")
    print()
    
    print("Step 1: Import performance monitor")
    print("   from monitor import PerformanceMonitor")
    from monitor import PerformanceMonitor
    print()
    
    print("Step 2: Check system health")
    health_checker = PerformanceMonitor.SystemHealthChecker()
    health_report = health_checker.check_system_health()
    print(f"   System status: {health_report['status']}")
    if health_report['issues']:
        print("   Issues found:")
        for issue in health_report['issues']:
            print(f"     ⚠️  {issue}")
    print()
    
    print("Step 3: Create and benchmark brain")
    brain = nlm.Brain()
    print("   Benchmarking 100 steps...")
    
    import time
    start_time = time.time()
    result = brain.run_steps(100)
    duration = time.time() - start_time
    
    speed = 100 / duration
    print(f"   Duration: {duration:.3f}s")
    print(f"   Speed: {speed:.1f} steps/second")
    print(f"   Reward: {result['total_reward']:.4f}")
    print()
    
    print("Step 4: Performance summary")
    print(f"   Steps per second: {speed:.1f}")
    print(f"   Average step time: {duration/100*1000:.1f}ms")
    print(f"   Efficiency: {result['total_reward']/duration:.3f} reward/second")
    print()
    
    return True

def tutorial_7_experiment_framework():
    """
    Tutorial 7: Using the Experiment Framework
    
    Learn how to run structured integration experiments.
    """
    print("=== Tutorial 7: Using the Experiment Framework ===")
    print()
    
    print("Step 1: Create Phase 6 experiment")
    print("   experiment = nlm.Phase6Experiment()")
    experiment = nlm.Phase6Experiment()
    print()
    
    print("Step 2: Verify system integration")
    print("   integration_ok = experiment.verify_integration()")
    integration_ok = experiment.verify_integration()
    print(f"   Integration status: {'✅ Pass' if integration_ok else '❌ Fail'}")
    print()
    
    if not integration_ok:
        print("   ⚠️  System integration failed - check configuration")
        return False
    
    print("Step 3: Configure experiment parameters")
    config = {
        "neuronCount": 500,
        "maxSteps": 1000,
        "enableCheckpointing": True,
        "enableReplay": True,
        "enableDevelopment": True
    }
    print("   config = {")
    for key, value in config.items():
        print(f"       '{key}': {value},")
    print("   }")
    print()
    
    print("Step 4: Run the experiment")
    print("   result = experiment.run_experiment(config)")
    result = experiment.run_experiment(config)
    print()
    
    print("Step 5: Analyze results")
    print(f"   Total reward: {result['totalReward']:.4f}")
    print(f"   Average firing rate: {result['avgFiringRate']:.2f} Hz")
    print(f"   Episodes stored: {result['memoryEpisodesStored']}")
    print(f"   Wall time: {result['wallClockTime']:.2f}s")
    print()
    
    print("Step 6: Check integration status")
    integration = result['integration']
    for system, status in integration.items():
        status_str = "✅ CONNECTED" if status else "❌ DISCONNECTED"
        print(f"   {system}: {status_str}")
    print()
    
    print("Step 7: Individual system tests")
    print("   Testing memory integration...")
    memory_ok = experiment.test_memory_integration()
    print(f"   Memory: {'✅ Pass' if memory_ok else '❌ Fail'}")
    
    print("   Testing neuromodulation integration...")
    neuromod_ok = experiment.test_neuromodulation_integration()
    print(f"   Neuromodulation: {'✅ Pass' if neuromod_ok else '❌ Fail'}")
    
    print("   Testing checkpointing...")
    checkpoint_ok = experiment.test_checkpointing()
    print(f"   Checkpointing: {'✅ Pass' if checkpoint_ok else '❌ Fail'}")
    
    print("   Testing replay...")
    replay_ok = experiment.test_replay()
    print(f"   Replay: {'✅ Pass' if replay_ok else '❌ Fail'}")
    print()
    
    print("🎉 Experiment completed successfully!")
    return True

def create_tutorial_project():
    """
    Create a sample NLM project structure
    """
    print("=== Creating Sample NLM Project ===")
    print()
    
    # Create project directory
    project_dir = Path("nlm_project")
    project_dir.mkdir(exist_ok=True)
    
    # Create subdirectories
    (project_dir / "configs").mkdir(exist_ok=True)
    (project_dir / "checkpoints").mkdir(exist_ok=True)
    (project_dir / "logs").mkdir(exist_ok=True)
    
    # Create configuration file
    config_content = """# NLM Configuration
neuron_count = 500
simulation_timestep = 0.001
random_seed = 42
enable_checkpointing = true
checkpoint_path = "./checkpoints/brain.bin"
"""
    
    config_file = project_dir / "configs" / "default.cfg"
    with open(config_file, 'w') as f:
        f.write(config_content)
    
    # Create main script
    main_script = project_dir / "run_experiment.py"
    with open(main_script, 'w') as f:
        f.write("""
import nlm
import sys

def main():
    print("NLM Project Experiment")
    print("=" * 40)
    
    # Load configuration
    config = nlm.Config("configs/default.cfg")
    
    # Create brain
    brain = nlm.Brain(config)
    
    # Run experiment
    print("Running experiment...")
    result = brain.run_steps(500)
    
    # Save results
    results = {
        "reward": result['total_reward'],
        "firing_rate": result['average_firing_rate'],
        "spikes": result['total_spikes'],
        "timestamp": str(result.get('timestamp', ''))
    }
    
    # Save to file
    import json
    with open("results.json", 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"Experiment completed!")
    print(f"Reward: {result['total_reward']:.4f}")
    print(f"Results saved to results.json")
    
    # Save brain checkpoint
    brain.save_checkpoint("checkpoints/final_brain.bin")
    print("Brain checkpoint saved")
    
    return True

if __name__ == "__main__":
    main()
""")
    
    # Create README
    readme_content = """# NLM Project

This project demonstrates the use of NLM (Neural Learning Machine) for neural simulation experiments.

## Configuration

Edit `configs/default.cfg` to adjust simulation parameters:

- `neuron_count`: Number of neurons (default: 500)
- `simulation_timestep`: Time step in seconds (default: 0.001)
- `random_seed`: Random seed for reproducibility (default: 42)
- `enable_checkpointing`: Enable checkpoint saving (default: true)
- `checkpoint_path`: Path to checkpoint file (default: ./checkpoints/brain.bin)

## Running the Experiment

```bash
python3 run_experiment.py
```

## Results

Results are saved to `results.json` with the following information:
- reward: Total average reward
- firing_rate: Average firing rate in Hz
- spikes: Total number of spikes
- timestamp: Experiment completion time

## Checkpoints

Brain checkpoints are saved to `checkpoints/brain.bin` for resuming interrupted experiments.

## Next Steps

1. Modify the configuration to experiment with different parameters
2. Add additional metrics and analysis
3. Implement your own experiment design
4. Use the monitoring tools to track performance
"""
    
    readme_file = project_dir / "README.md"
    with open(readme_file, 'w') as f:
        f.write(readme_content)
    
    print(f"✅ Created NLM project directory: {project_dir}")
    print(f"   Configuration: {config_file}")
    print(f"   Main script: {main_script}")
    print(f"   README: {readme_file}")
    print()
    
    return True

def main():
    """Run all tutorials"""
    print("NLM (Neural Learning Machine) - Practical Tutorials")
    print("=" * 60)
    print()
    
    tutorials = [
        ("Getting Started", tutorial_1_getting_started),
        ("Configuration Management", tutorial_2_configuration_management),
        ("Memory Systems", tutorial_3_memory_systems),
        ("Neuromodulation", tutorial_4_neuromodulation),
        ("Checkpointing", tutorial_5_checkpointing),
        ("Performance Monitoring", tutorial_6_performance_monitoring),
        ("Experiment Framework", tutorial_7_experiment_framework),
    ]
    
    for name, tutorial_func in tutorials:
        try:
            print(f"{'='*60}")
            print(f"Tutorial: {name}")
            print(f"{'='*60}")
            
            if tutorial_func():
                print(f"✅ {name} completed successfully")
            else:
                print(f"❌ {name} completed with errors")
                
        except Exception as e:
            print(f"❌ {name} failed: {e}")
            import traceback
            traceback.print_exc()
        
        print()
    
    # Create sample project
    print(f"{'='*60}")
    print("Sample Project")
    print(f"{'='*60}")
    try:
        create_tutorial_project()
        print("✅ Sample project created successfully")
    except Exception as e:
        print(f"❌ Sample project creation failed: {e}")
    
    print(f"{'='*60}")
    print("ALL TUTORIALS COMPLETED")
    print(f"{'='*60}")
    
    print("\nNext Steps:")
    print("1. Try the advanced examples in examples/advanced_examples.py")
    print("2. Use the monitoring tools in monitor.py")
    print("3. Create your own NLM projects using the sample structure")
    print("4. Explore the Python API documentation")

if __name__ == "__main__":
    main()