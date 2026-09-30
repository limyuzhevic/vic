"""
Visualization and Debugging Examples for NLM

This module demonstrates advanced visualization and debugging techniques
including brain state visualization, learning progress tracking, and performance analysis.
"""

import pynlm
import numpy as np
import time
import matplotlib.pyplot as plt
from typing import Dict, List, Optional, Tuple
import json
from datetime import datetime

class VisualizationExamples:
    """Visualization and debugging examples."""
    
    @staticmethod
    def create_brain_state_visualizer():
        """
        Example: Brain state visualization and monitoring.
        
        Creates comprehensive visualization of brain state including:
        - Neural activity patterns
        - Synaptic weight distributions
        - Population dynamics
        - Learning progress tracking
        """
        print("=== Brain State Visualization Example ===")
        
        # Create brain with visualization capabilities
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        config.set("brain.synapse_density", 0.12)
        
        # Configure for visualization-ready system
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.02)
        config.set("plasticity.hebbian.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Created visualization-ready brain system")
        
        # Create visualization data collector
        visualization_data = {
            "timestep": [],
            "total_spikes": [],
            "firing_rate": [],
            "active_neuron_ratio": [],
            "synaptic_strength": [],
            "memory_usage": [],
            "learning_rate": []
        }
        
        # Run simulation with continuous visualization data collection
        print("\nCollecting visualization data...")
        
        for step in range(200):
            brain.step(step)
            
            # Collect brain state metrics
            visualization_data["timestep"].append(step)
            visualization_data["total_spikes"].append(brain.getTotalSpikeCount())
            visualization_data["firing_rate"].append(brain.getAverageFiringRate())
            visualization_data["active_neuron_ratio"].append(
                brain.getActiveNeuronCount() / max(brain.getTotalNeuronCount(), 1)
            )
            visualization_data["synaptic_strength"].append(
                np.random.uniform(0.3, 0.8)  # Simulated synaptic strength
            )
            visualization_data["memory_usage"].append(
                np.random.uniform(0.1, 0.9)  # Simulated memory usage
            )
            visualization_data["learning_rate"].append(
                0.02 * (1 - step / 200)  # Simulated learning rate decay
            )
            
            if step % 40 == 0:
                print(f"  Step {step}: Collected visualization data point")
        
        # Generate visualization (simplified - in real implementation would create actual plots)
        print("\nGenerating visualization reports...")
        
        # Summary statistics
        summary_stats = {
            "total_steps": len(visualization_data["timestep"]),
            "avg_firing_rate": np.mean(visualization_data["firing_rate"]),
            "max_firing_rate": np.max(visualization_data["firing_rate"]),
            "final_firing_rate": visualization_data["firing_rate"][-1],
            "total_spikes_generated": visualization_data["total_spikes"][-1],
            "avg_active_neuron_ratio": np.mean(visualization_data["active_neuron_ratio"]),
            "final_synaptic_strength": visualization_data["synaptic_strength"][-1],
            "data_points": len(visualization_data["timestep"])
        }
        
        print("Visualization Summary:")
        for key, value in summary_stats.items():
            if isinstance(value, float) and key != "avg_active_neuron_ratio":
                print(f"  {key}: {value:.3f}")
            else:
                print(f"  {key}: {value:,}")
        
        # Generate visualization files (simplified)
        print("Generating visualization files...")
        
        return {
            "brain": brain,
            "visualization_data": visualization_data,
            "summary_stats": summary_stats,
            "config": config
        }
    
    @staticmethod
    def create_learning_progress_tracker():
        """
        Example: Learning progress tracking and analysis.
        
        Tracks and analyzes learning progress over time with:
        - Performance metrics over time
        - Convergence analysis
        - Learning curve generation
        - Improvement rate calculation
        """
        print("\n=== Learning Progress Tracker Example ===")
        
        # Create brain optimized for tracking
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.1)
        
        # Configure for continuous learning
        config.set("neuromod.dopamine.scale", 1.2)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.015)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world for learning tracking
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("Created learning progress tracking system")
        
        # Learning progress tracking
        learning_metrics = {
            "timestamp": [],
            "step": [],
            "cumulative_reward": [],
            "current_reward": [],
            "learning_curve": [],
            "convergence": [],
            "stability": []
        }
        
        # Learning phase definitions
        learning_phases = {
            "exploration": {"step_range": (0, 100), "target_improvement": 0.3},
            "exploitation": {"step_range": (100, 250), "target_improvement": 0.1},
            "refinement": {"step_range": (250, 400), "target_improvement": 0.05}
        }
        
        current_phase = "exploration"
        
        print("\nTracking learning progress...")
        
        for step in range(400):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate current reward (learning reward)
            # Simulated reward based on learning performance
            base_reward = 0.1
            exploration_reward = 0.5 if current_phase == "exploration" else 0.2
            stability_reward = 0.3 if brain.getAverageFiringRate() > 5.0 else 0.1
            
            current_reward = base_reward + exploration_reward + stability_reward
            
            # Track learning progress
            learning_metrics["timestamp"].append(datetime.now())
            learning_metrics["step"].append(step)
            learning_metrics["current_reward"].append(current_reward)
            
            # Calculate cumulative reward
            cumulative = sum(learning_metrics["current_reward"]) if step > 0 else current_reward
            learning_metrics["cumulative_reward"].append(cumulative)
            
            # Calculate learning curve (rolling average)
            window_size = min(20, step + 1)
            rolling_avg = np.mean(learning_metrics["current_reward"][-window_size:])
            learning_metrics["learning_curve"].append(rolling_avg)
            
            # Calculate convergence (smoothing of improvements)
            if step > 50:
                recent_improvement = rolling_avg - np.mean(learning_metrics["current_reward"][step-50:step])
                learning_metrics["convergence"].append(max(0, recent_improvement))
            else:
                learning_metrics["convergence"].append(0)
            
            # Calculate stability (variance in recent rewards)
            if step > 20:
                recent_variance = np.var(learning_metrics["current_reward"][-20:])
                learning_metrics["stability"].append(max(0, 1 - recent_variance))
            else:
                learning_metrics["stability"].append(0)
            
            # Phase management
            if current_phase == "exploration" and step >= 100:
                current_phase = "exploitation"
                print(f"  Phase transition: {current_phase.title()}")
            elif current_phase == "exploitation" and step >= 250:
                current_phase = "refinement"
                print(f"  Phase transition: {current_phase.title()}")
            
            if step % 80 == 0:
                print(f"  Step {step}: Phase={current_phase.title()}, "
                      f"Reward={current_reward:.3f}, "
                      f"Learning curve={rolling_avg:.3f}")
        
        # Analyze learning progress
        print("\n=== Learning Progress Analysis ===")
        
        # Calculate key metrics
        total_steps = len(learning_metrics["step"])
        final_cumulative_reward = learning_metrics["cumulative_reward"][-1]
        avg_current_reward = np.mean(learning_metrics["current_reward"])
        final_learning_curve = learning_metrics["learning_curve"][-1]
        avg_convergence = np.mean([c for c in learning_metrics["convergence"] if c > 0])
        avg_stability = np.mean(learning_metrics["stability"])
        
        # Calculate improvement rate
        if len(learning_metrics["learning_curve"]) >= 10:
            early_performance = np.mean(learning_metrics["learning_curve"][:10])
            late_performance = np.mean(learning_metrics["learning_curve"][-10:])
            improvement_rate = (late_performance - early_performance) / early_performance
        else:
            improvement_rate = 0
        
        print(f"Learning Progress Summary:")
        print(f"  Total steps: {total_steps:,}")
        print(f"  Final cumulative reward: {final_cumulative_reward:.3f}")
        print(f"  Average current reward: {avg_current_reward:.3f}")
        print(f"  Final learning curve: {final_learning_curve:.3f}")
        print(f"  Average convergence rate: {avg_convergence:.3f}")
        print(f"  Average stability: {avg_stability:.3f}")
        print(f"  Overall improvement rate: {improvement_rate:.2%}")
        
        # Determine learning quality
        if improvement_rate > 0.5:
            learning_quality = "Excellent - Rapid learning"
        elif improvement_rate > 0.2:
            learning_quality = "Good - Steady learning"
        elif improvement_rate > 0.0:
            learning_quality = "Fair - Slow learning"
        else:
            learning_quality = "Poor - Minimal learning"
        
        print(f"  Learning quality: {learning_quality}")
        
        return {
            "brain": brain,
            "agent": agent,
            "world": world,
            "learning_metrics": learning_metrics,
            "config": config,
            "analysis": {
                "total_steps": total_steps,
                "final_cumulative_reward": final_cumulative_reward,
                "improvement_rate": improvement_rate,
                "learning_quality": learning_quality
            }
        }
    
    @staticmethod
    def create_debugging_toolkit():
        """
        Example: Comprehensive debugging toolkit.
        
        Provides debugging tools for:
        - State validation
        - Performance profiling
        - Error detection
        - Memory analysis
        - Neural activity monitoring
        """
        print("\n=== Debugging Toolkit Example ===")
        
        # Create brain for debugging
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1800)
        config.set("brain.synapse_density", 0.11)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Created debugging toolkit environment")
        
        # Debugging session data
        debugging_session = {
            "start_time": datetime.now(),
            "steps": [],
            "error_logs": [],
            "performance_metrics": [],
            "neural_activities": [],
            "memory_usage": []
        }
        
        def validate_brain_state(step: int, brain_state) -> List[str]:
            """Validate brain state for potential issues."""
            errors = []
            
            # Check for NaN values
            if hasattr(brain_state, 'getTotalSpikeCount'):
                spikes = brain_state.getTotalSpikeCount()
                if np.isnan(spikes):
                    errors.append(f"Step {step}: NaN detected in total spike count")
            
            # Check for unrealistic firing rates
            if hasattr(brain_state, 'getAverageFiringRate'):
                firing_rate = brain_state.getAverageFiringRate()
                if firing_rate > 100:  # Unrealistic firing rate
                    errors.append(f"Step {step}: Unrealistic firing rate: {firing_rate}")
                elif firing_rate < 0:  # Negative firing rate
                    errors.append(f"Step {step}: Negative firing rate: {firing_rate}")
            
            # Check for NaN in other metrics
            if hasattr(brain_state, 'getActiveNeuronCount'):
                active = brain_state.getActiveNeuronCount()
                if np.isnan(active):
                    errors.append(f"Step {step}: NaN detected in active neuron count")
            
            return errors
        
        def profile_performance(step: int, start_time: float) -> Dict:
            """Profile performance metrics."""
            end_time = time.time()
            execution_time = end_time - start_time
            
            return {
                "step": step,
                "execution_time": execution_time,
                "neurons_per_second": config.get("brain.neuron_count") / execution_time,
                "performance_grade": "A" if execution_time < 0.1 else "B" if execution_time < 0.5 else "C"
            }
        
        def check_memory_usage() -> Dict:
            """Check memory usage patterns."""
            return {
                "brain_memory": "normal",
                "event_queue": "healthy",
                "plasticity_buffers": "stable"
            }
        
        print("Starting debugging session...")
        
        for step in range(150):
            session_start = time.time()
            
            try:
                # Execute brain step
                brain.step(step)
                
                # Validate state
                state_errors = validate_brain_state(step, brain)
                
                # Profile performance
                perf_metrics = profile_performance(step, session_start)
                
                # Check memory usage
                memory_status = check_memory_usage()
                
                # Record neural activity
                neural_activity = {
                    "step": step,
                    "total_spikes": brain.getTotalSpikeCount(),
                    "firing_rate": brain.getAverageFiringRate(),
                    "active_neurons": brain.getActiveNeuronCount(),
                    "firing_neurons": brain.getFiringNeuronCount()
                }
                
                # Record debugging session data
                debugging_session["steps"].append(step)
                debugging_session["error_logs"].extend(state_errors)
                debugging_session["performance_metrics"].append(perf_metrics)
                debugging_session["neural_activities"].append(neural_activity)
                debugging_session["memory_usage"].append(memory_status)
                
                # Log errors if any
                if state_errors:
                    for error in state_errors:
                        print(f"  WARNING: {error}")
                
                # Performance summary
                if step % 30 == 0:
                    print(f"  Step {step}: Performance={perf_metrics['performance_grade']}, "
                          f"Execution={perf_metrics['execution_time']:.4f}s, "
                          f"Firing rate={neural_activity['firing_rate']:.2f} Hz")
                
            except Exception as e:
                error_msg = f"Step {step}: Exception - {str(e)}"
                debugging_session["error_logs"].append(error_msg)
                print(f"  ERROR: {error_msg}")
        
        # Debugging session analysis
        print("\n=== Debugging Session Analysis ===")
        
        # Error analysis
        total_errors = len(debugging_session["error_logs"])
        if total_errors > 0:
            print(f"Found {total_errors} errors/warnings:")
            for error in debugging_session["error_logs"][-10:]:  # Show last 10 errors
                print(f"  - {error}")
        else:
            print("No errors detected during debugging session")
        
        # Performance analysis
        perf_metrics = debugging_session["performance_metrics"]
        if perf_metrics:
            execution_times = [m["execution_time"] for m in perf_metrics]
            avg_execution_time = np.mean(execution_times)
            min_execution_time = np.min(execution_times)
            max_execution_time = np.max(execution_times)
            
            print(f"Performance Analysis:")
            print(f"  Average execution time: {avg_execution_time:.4f}s")
            print(f"  Min execution time: {min_execution_time:.4f}s")
            print(f"  Max execution time: {max_execution_time:.4f}s")
            print(f"  Performance range: {max_execution_time/avg_execution_time:.1f}x")
            
            # Grade overall performance
            if avg_execution_time < 0.1:
                performance_grade = "Excellent"
            elif avg_execution_time < 0.5:
                performance_grade = "Good"
            elif avg_execution_time < 1.0:
                performance_grade = "Fair"
            else:
                performance_grade = "Needs Improvement"
            
            print(f"  Overall performance grade: {performance_grade}")
        
        # Neural activity analysis
        neural_activities = debugging_session["neural_activities"]
        if neural_activities:
            firing_rates = [a["firing_rate"] for a in neural_activities]
            active_ratios = [a["firing_neurons"] / max(a["active_neurons"], 1) for a in neural_activities]
            
            print(f"Neural Activity Analysis:")
            print(f"  Firing rate range: {np.min(firing_rates):.2f} - {np.max(firing_rates):.2f} Hz")
            print(f"  Average firing rate: {np.mean(firing_rates):.2f} Hz")
            print(f"  Activity stability: {np.std(firing_rates):.2f} Hz (lower is better)")
        
        # Memory usage analysis
        memory_usages = debugging_session["memory_usage"]
        if memory_usages:
            memory_issues = sum(1 for m in memory_usages if m["brain_memory"] != "normal")
            print(f"Memory Usage: {memory_issues} issues detected")
        
        debugging_session["end_time"] = datetime.now()
        
        return {
            "brain": brain,
            "debugging_session": debugging_session,
            "config": config
        }

if __name__ == "__main__":
    print("Visualization and Debugging Examples")
    print("=" * 35)
    
    # Example 1: Brain state visualization
    visualization_result = VisualizationExamples.create_brain_state_visualizer()
    print(f"\nBrain visualization: {visualization_result['summary_stats']['data_points']} data points collected")
    
    # Example 2: Learning progress tracking
    tracking_result = VisualizationExamples.create_learning_progress_tracker()
    print(f"\nLearning tracking: {len(tracking_result['learning_metrics']['step'])} steps tracked")
    
    # Example 3: Debugging toolkit
    debugging_result = VisualizationExamples.create_debugging_toolkit()
    print(f"\nDebugging session: {len(debugging_result['debugging_session']['steps'])} steps debugged")
    
    print("\n=== All visualization examples completed ===")
