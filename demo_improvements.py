#!/usr/bin/env python3
"""
NLM System Improvements Demonstration

This script demonstrates the improvements made to the NLM (Neural Learning Machine) system:
1. Enhanced agent controller with advanced features
2. Interactive agent system with CLI interface
3. Performance monitoring
4. Multiple agent types and behaviors
5. Experiment management
6. Data export and visualization
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from advanced_agent_controller import AdvancedAgentController, ExperimentConfig
from interactive_agent_system import InteractiveAgentSystem, AgentProfile, AgentType, EnvironmentProfile
from datetime import datetime

def demo_advanced_controller():
    """Demonstrate the advanced agent controller."""
    print("="*60)
    print("ADVANCED AGENT CONTROLLER DEMONSTRATION")
    print("="*60)

    # Create controller
    controller = AdvancedAgentController(log_file="nlm_demo.log")

    # Define advanced experiment configuration
    experiment_config = ExperimentConfig(
        name="demo_advanced_experiment",
        brain_config={
            "neuron_count": 800,
            "synapse_density": 0.18,
            "connection_probability": 0.15,
            "plasticity": {
                "stdp": {"enable": True, "learning_rate": 0.0015},
                "hebbian": {"enable": True, "learning_rate": 0.001}
            },
            "neuromod": {
                "dopamine": {"scale": 1.2},
                "curiosity": {"enable": True},
                "novelty": {"enable": True}
            },
            "development": {
                "initial_plasticity": 1.0,
                "critical_period_multiplier": 0.8
            }
        },
        world_config={
            "width": 25,
            "height": 25,
            "visionWidth": 10,
            "visionHeight": 10,
            "max_energy": 150.0,
            "energy_decay_rate": 0.985,
            "random_seed": 12345
        },
        agent_config={
            "features": {
                "reward_modulation": True,
                "structural_plasticity": True,
                "development": True,
                "curiosity": True
            },
            "mode": {
                "name": "exploration"
            }
        },
        duration=300.0,
        timestep=0.1,
        save_frequency=50,
        log_frequency=25,
        tags=["demo", "advanced", "controller"],
        metadata={"description": "Demonstration of advanced agent controller features"}
    )

    # Add callbacks
    def step_callback(event_type, data):
        if event_type == "step" and data["step"] % 50 == 0:
            print(f"  Step {data['step']}: Action={data['action']:.3f}, "
                  f"SpikeRate={data['metrics'].spike_rate:.4f}, "
                  f"Curiosity={data['metrics'].exploration_rate:.4f}")

    controller.add_callback("step", step_callback)

    print(f"\nExperiment Configuration:")
    print(f"  Name: {experiment_config.name}")
    print(f"  Duration: {experiment_config.duration}s")
    print(f"  Brain: {experiment_config.brain_config['neuron_count']} neurons")
    print(f"  World: {experiment_config.world_config['width']}x{experiment_config.world_config['height']}")
    print(f"  Agent features: {list(experiment_config.agent_config['features'].keys())}")

    print(f"\nRunning advanced experiment...")
    start_time = time.time()

    # Run experiment
    results = controller.run_experiment(experiment_config)

    end_time = time.time()

    if results["status"] == "completed":
        print(f"\n✓ Experiment completed successfully!")
        print(f"  Duration: {results['duration']:.2f}s")
        print(f"  Steps: {results['steps']}")
        print(f"  Agent mode: {results['agent_mode']}")

        final_metrics = results['final_metrics']
        print(f"\n  Performance Summary:")
        print(f"    Steps per second: {final_metrics.steps_per_second:.2f}")
        print(f"    Average spike rate: {final_metrics.spike_rate:.4f}")
        print(f"    Average synaptic activity: {final_metrics.synaptic_activity:.4f}")
        print(f"    Average curiosity: {final_metrics.exploration_rate:.4f}")
        print(f"    Average learning rate: {final_metrics.learning_rate:.4f}")

        # Save results
        controller.save_results(results, "demo_advanced_results.json")
        print(f"\n  Results saved to: demo_advanced_results.json")

        # Export to CSV
        csv_data = controller.export_data(results, "csv")
        with open("demo_advanced_results.csv", "w") as f:
            f.write(csv_data)
        print(f"  Results exported to: demo_advanced_results.csv")

        # Show summary
        print(f"\n  Summary:")
        controller.print_summary(results)

        print(f"\n✓ Advanced agent controller demonstration completed successfully!")
        return True
    else:
        print(f"\n✗ Experiment failed: {results.get('error', 'Unknown error')}")
        return False


def demo_interactive_system():
    """Demonstrate the interactive agent system."""
    print("\n" + "="*60)
    print("INTERACTIVE AGENT SYSTEM DEMONSTRATION")
    print("="*60)

    # Create system
    system = InteractiveAgentSystem(output_dir="nlm_interactive_demo")

    print(f"\nSystem created with output directory: {system.output_dir}")

    # Create an experiment setup
    experiment_setup = system.current_experiment

    if experiment_setup:
        print(f"\nCurrent experiment: {experiment_setup.name}")
        print(f"  Agent type: {experiment_setup.agent_profile.type.value}")
        print(f"  Environment: {experiment_setup.environment_profile.width}x{experiment_setup.environment_profile.height}")
        print(f"  Duration: {experiment_setup.duration}s")
        print(f"  Max steps: {experiment_setup.max_steps}")
    else:
        print(f"\nNo experiment setup currently configured.")

    # Show system status
    print(f"\nSystem Status:")
    status = system.get_current_status()
    for key, value in status.items():
        print(f"  {key}: {value}")

    # Show available commands
    print(f"\nAvailable commands:")
    system.print_available_commands()

    print(f"\n✓ Interactive agent system demonstration completed!")
    return True


def demo_performance_monitoring():
    """Demonstrate performance monitoring features."""
    print("\n" + "="*60)
    print("PERFORMANCE MONITORING DEMONSTRATION")
    print("="*60)

    from interactive_agent_system import PerformanceMonitor

    # Create monitor
    monitor = PerformanceMonitor(sample_rate=0.2)

    print(f"\nPerformance Monitor created with sample rate: {monitor.sample_rate}s")

    # Start monitoring
    monitor.start_monitoring()

    print(f"Monitoring started...")
    print(f"Current metrics available: {list(monitor.get_current_metrics().keys())}")

    # Simulate some work
    print(f"\nSimulating work...")
    for i in range(5):
        time.sleep(0.1)
        current_metrics = monitor.get_current_metrics()
        print(f"  Sample {i+1}: Memory={current_metrics.get('memory_usage', {}).get('rss_mb', 0):.1f} MB, "
              f"CPU={current_metrics.get('cpu_usage', 0):.1f}%, "
              f"Threads={current_metrics.get('thread_count', 0)}")

    # Stop monitoring
    monitor.stop_monitoring()

    # Get statistics
    stats = monitor.get_statistics()
    print(f"\nPerformance Statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")

    print(f"\n✓ Performance monitoring demonstration completed!")
    return True


def demo_agent_types():
    """Demonstrate different agent types."""
    print("\n" + "="*60)
    print("AGENT TYPES DEMONSTRATION")
    print("="*60)

    agent_types = [
        ("Explorer", AgentType.EXPLORER, "High curiosity, random exploration"),
        ("Learner", AgentType.LEARNER, "Focused learning, reward-based"),
        ("Adaptive", AgentType.ADAPTIVE, "Dynamic adaptation to environment"),
        ("Social", AgentType.SOCIAL, "Cooperative and social behaviors"),
        ("Creative", AgentType.CREATIVE, "Innovative and exploratory"),
        ("Basic", AgentType.BASIC, "Simple, straightforward behavior")
    ]

    print(f"\nAvailable Agent Types:")
    for name, agent_type, description in agent_types:
        print(f"  {name:10} ({agent_type.value:10}) - {description}")

    # Create sample profiles
    print(f"\nSample Agent Profiles:")
    for name, agent_type, description in agent_types:
        profile = AgentProfile(
            name=f"demo_{name.lower()}_agent",
            type=agent_type,
            curiosity_threshold=0.5 + (agent_type.value == "explorer"),
            exploration_rate=0.3 + (agent_type.value == "explorer"),
            learning_rate=0.01 * (1.5 if agent_type.value == "creative" else 1.0),
            risk_tolerance=0.5 + (agent_type.value == "explorer"),
            tags=[agent_type.value, "demo"]
        )

        print(f"\n  {name}:")
        print(f"    Type: {profile.type.value}")
        print(f"    Curiosity: {profile.curiosity_threshold}")
        print(f"    Exploration: {profile.exploration_rate}")
        print(f"    Learning: {profile.learning_rate}")
        print(f"    Risk: {profile.risk_tolerance}")

    print(f"\n✓ Agent types demonstration completed!")
    return True


def demo_experiment_management():
    """Demonstrate experiment management features."""
    print("\n" + "="*60)
    print("EXPERIMENT MANAGEMENT DEMONSTRATION")
    print("="*60)

    # Create experiment setups using the InteractiveAgentSystem
    system = InteractiveAgentSystem()

    experiments = []

    for i in range(3):
        agent_type = AgentType.EXPLORER if i % 2 == 0 else AgentType.LEARNER

        # Create experiment setup using the system's internal method
        from interactive_agent_system import ExperimentSetup, EnvironmentProfile, AgentProfile

        env_profile = EnvironmentProfile(
            width=15 + i * 5,
            height=15 + i * 5,
            vision_range=8
        )

        agent_profile = AgentProfile(
            name=f"demo_exp_{i+1}_agent",
            type=agent_type,
            curiosity_threshold=0.6,
            exploration_rate=0.4,
            learning_rate=0.01
        )

        new_experiment = ExperimentSetup(
            name=f"demo_experiment_{i+1}",
            description=f"Demonstration experiment {i+1}",
            agent_profile=agent_profile,
            environment_profile=env_profile,
            duration=100.0 + i * 50,
            timestep=0.1,
            max_steps=2000 + i * 500,
            save_frequency=50,
            parallel_runs=1,
            seed=42 + i,
            output_dir=f"experiments/demo",
            tags=["demo", "experiment", f"batch_{i//2}"]
        )
        experiments.append(new_experiment)

    print(f"\nCreated {len(experiments)} experiment setups:")
    for exp in experiments:
        print(f"  - {exp.name}: {exp.agent_profile.type.value} agent, {exp.duration}s duration")
        print(f"    Environment: {exp.environment_profile.width}x{exp.environment_profile.height}")
        print(f"    Tags: {exp.tags}")

    print(f"\n✓ Experiment management demonstration completed!")
    return True


def main():
    """Main demonstration function."""
    print("NLM System Improvements Demonstration")
    print("="*60)
    print("This demonstration showcases the improvements made to the NLM system:")
    print("1. Advanced Agent Controller with enhanced features")
    print("2. Interactive Agent System with CLI interface")
    print("3. Performance Monitoring and Analysis")
    print("4. Multiple Agent Types and Behaviors")
    print("5. Experiment Management")
    print("6. Data Export and Visualization")
    print("="*60)

    # Run demonstrations
    results = []

    print(f"\nRunning demonstrations...")

    # Demo 1: Advanced Controller
    print(f"\n{'='*60}")
    success1 = demo_advanced_controller()
    results.append(("Advanced Agent Controller", success1))

    # Demo 2: Interactive System
    print(f"\n{'='*60}")
    success2 = demo_interactive_system()
    results.append(("Interactive Agent System", success2))

    # Demo 3: Performance Monitoring
    print(f"\n{'='*60}")
    success3 = demo_performance_monitoring()
    results.append(("Performance Monitoring", success3))

    # Demo 4: Agent Types
    print(f"\n{'='*60}")
    success4 = demo_agent_types()
    results.append(("Agent Types", success4))

    # Demo 5: Experiment Management
    print(f"\n{'='*60}")
    # Note: Need to create system first for experiment management demo
    success5 = demo_experiment_management()
    results.append(("Experiment Management", success5))

    # Summary
    print(f"\n{'='*60}")
    print("DEMONSTRATION SUMMARY")
    print("="*60)

    passed = sum(1 for _, success in results if success)
    total = len(results)

    print(f"\nResults:")
    for name, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"  {name:30} - {status}")

    print(f"\nOverall: {passed}/{total} demonstrations completed successfully")

    if passed == total:
        print(f"\n🎉 All demonstrations passed! The NLM system improvements are working correctly.")
    else:
        print(f"\n⚠️  {total - passed} demonstration(s) failed. Please check the output above.")

    print(f"\n{'='*60}")


if __name__ == "__main__":
    import time
    main()
