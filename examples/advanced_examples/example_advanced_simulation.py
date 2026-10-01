"""
Advanced NLM examples demonstrating the new functionality.
These examples showcase the advanced features added to the NLM framework.
"""

import sys
import os
import tempfile
import json
import time
import statistics
from typing import Dict, Any, List, Optional

# Add the Python module path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm
    import pynlm_advanced
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm_advanced not available, some examples will be skipped")

from pynlm import createDefaultConfig, loadConfig
from pynlm_advanced import AdvancedBrain, AdvancedPlasticity, AdvancedSimulation
from pynlm_advanced import AdvancedAnalysis, AdvancedConfig
from pynlm import Types

# Import RegionId from Types
RegionId = Types.RegionId


class Example3AdvancedSimulationFeatures:
    """Example 3: Advanced Simulation Features - Multi-Agent Systems"""

    def __init__(self):
        self.title = "Advanced Simulation Features Example"
        self.description = "Demonstrates multi-agent systems and advanced simulation capabilities"
        self.code_file = "example_advanced_simulation.py"

    def run_example(self):
        """Run the advanced simulation example."""
        if not PYNLM_AVAILABLE:
            print("Skipping example - pynlm_advanced not available")
            return

        print("\n" + "="*60)
        print(f"EXAMPLE 3: {self.title}")
        print("="*60)
        print(f"Description: {self.description}")
        print()

        # Step 1: Create advanced simulation
        print("Step 1: Creating advanced simulation...")
        
        simulation = AdvancedSimulation()
        
        # Configure comprehensive simulation settings
        sim_config = {
            "populationSize": 20,
            "environmentTemplate": "expert_environment",
            "parallelConfig": {
                "enableParallel": True,
                "numThreads": 4,
                "schedulingPolicy": "Dynamic",
                "loadBalancingThreshold": 0.8,
                "memoryAllocationStrategy": "adaptive"
            },
            "experimentControl": {
                "enableStepControl": True,
                "enableConditionControl": True,
                "enableRewardControl": True,
                "enableNoiseControl": True,
                "enableSpeedControl": True,
                "noiseLevel": 0.1,
                "stepDuration": 0.5,
                "simulationSpeed": 1.0
            }
        }
        
        simulation.configure(sim_config)
        
        print(f"  - Simulation configured with {sim_config['populationSize']} agents")
        print(f"  - Parallel processing enabled: {sim_config['parallelConfig']['enableParallel']}")
        print(f"  - Threads available: {sim_config['parallelConfig']['numThreads']}")

        # Step 2: Create diverse agent population
        print("\nStep 2: Creating diverse agent population...")
        
        # Define agent templates with specialized roles
        agent_templates = [
            {
                "name": "coordinator",
                "type": "hierarchical_leader",
                "capabilities": ["coordination", "decision_making", "strategy_planning"],
                "behavior": "hierarchical",
                "health": 1.0,
                "energy": 1.0,
                "experience": 0.8,
                "fitness": 1.0
            },
            {
                "name": "specialist",
                "type": "expert_specialist",
                "capabilities": ["expertise", "problem_solving", "technical_skills"],
                "behavior": "individual",
                "health": 0.9,
                "energy": 0.8,
                "experience": 0.9,
                "fitness": 0.95
            },
            {
                "name": "collaborator",
                "type": "social_cooperator",
                "capabilities": ["cooperation", "resource_sharing", "teamwork"],
                "behavior": "altruistic",
                "health": 0.8,
                "energy": 0.9,
                "experience": 0.7,
                "fitness": 0.9
            },
            {
                "name": "explorer",
                "type": "adaptive_explorer",
                "capabilities": ["exploration", "adaptation", "innovation"],
                "behavior": "exploratory",
                "health": 0.7,
                "energy": 0.8,
                "experience": 0.6,
                "fitness": 0.8
            },
            {
                "name": "balancer",
                "type": "consensus_builder",
                "capabilities": ["negotiation", "compromise", "mediation"],
                "behavior": "consensus",
                "health": 0.6,
                "energy": 0.7,
                "experience": 0.5,
                "fitness": 0.85
            }
        ]
        
        created_agents = []
        for template in agent_templates:
            for i in range(4):  # 4 agents per template type
                agent_id = simulation.createAgent(template["type"])
                created_agents.append(agent_id)
                simulation.activateAgent(agent_id)
                
                # Set agent attributes
                simulation.setAgentAttribute(agent_id, "name", f"{template['name']}_{i+1}")
                simulation.setAgentAttribute(agent_id, "capabilities", template["capabilities"])
                simulation.setAgentAttribute(agent_id, "behavior", template["behavior"])
                simulation.setAgentAttribute(agent_id, "health", template["health"])
                simulation.setAgentAttribute(agent_id, "energy", template["energy"])
                simulation.setAgentAttribute(agent_id, "experience", template["experience"])
                simulation.setAgentAttribute(agent_id, "fitness", template["fitness"])
                
                # Set rewards based on agent role
                reward = 1.0
                if template["name"] == "coordinator":
                    reward = 1.2
                elif template["name"] == "specialist":
                    reward = 1.1
                elif template["name"] == "collaborator":
                    reward = 0.9
                elif template["name"] == "explorer":
                    reward = 0.8
                else:
                    reward = 0.7
                
                simulation.setReward(agent_id, reward)
                
                print(f"  - Created {template['type']} agent: {agent_id}")

        print(f"\n  - Total agents created: {len(created_agents)}")
        print(f"  - Agent types: {len(agent_templates)}")

        # Step 3: Configure environment
        print("\nStep 3: Configuring simulation environment...")
        
        # Set environment parameters
        environment_parameters = {
            "temperature": 25.0,
            "humidity": 0.6,
            "light_intensity": 500.0,
            "air_quality": 0.9,
            "noise_level": 0.2,
            "resource_abundance": 0.8,
            "threat_level": 0.3,
            "social_cohesion": 0.7
        }
        
        for param_name, value in environment_parameters.items():
            simulation.setEnvironmentParameter(param_name, value)
            print(f"    Set {param_name}: {value}")
        
        # Get current environment state
        current_environment = {}
        for param_name in environment_parameters.keys():
            current_environment[param_name] = simulation.getEnvironmentParameter(param_name)
        
        print(f"  - Current environment state recorded")

        # Step 4: Start advanced simulation
        print("\nStep 4: Starting advanced simulation...")
        
        simulation.start()
        
        # Print initial status
        initial_stats = simulation.getSimulationStats()
        print(f"  - Simulation started")
        print(f"  - Initial stats: {initial_stats}")
        
        # Enable memory optimization
        simulation.enableMemoryOptimization()
        print(f"  - Memory optimization enabled")

        # Step 5: Run extended simulation
        print("\nStep 5: Running extended simulation...")
        
        simulation_data = {
            "step": 0,
            "agent_metrics": [],
            "environmental_factors": [],
            "coordination_events": [],
            "learning_progress": []
        }
        
        for step in range(40):
            # Update simulation
            simulation.step(step)
            
            # Collect agent metrics
            agent_states = simulation.getAllAgentStates()
            simulation_data["agent_metrics"].append({
                "step": step,
                "active_agents": len([s for s in agent_states if s["health"] > 0.5]),
                "average_fitness": statistics.mean([s["fitness"] for s in agent_states]),
                "average_energy": statistics.mean([s["energy"] for s in agent_states]),
                "coordination_score": sum([s.get("coordination_score", 0.5) for s in agent_states]) / len(agent_states)
            })
            
            # Collect environmental factors
            env_factors = {}
            for param_name in environment_parameters.keys():
                env_factors[param_name] = simulation.getEnvironmentParameter(param_name)
            
            simulation_data["environmental_factors"].append({
                "step": step,
                "timestamp": time.time(),
                "environmental_state": env_factors
            })
            
            # Execute coordination cycle every 5 steps
            if step % 5 == 0:
                simulation.executeCoordinationCycle()
                
                # Record coordination events
                coordination_events = simulation.getRecentEvents()
                simulation_data["coordination_events"].append({
                    "step": step,
                    "events": coordination_events,
                    "event_count": len(coordination_events)
                })
            
            # Record learning progress
            learning_progress = {
                "step": step,
                "collective_learning": statistics.mean([s.get("learning_progress", 0.5) for s in agent_states]),
                "adaptation_rate": 0.1 * (step / 40.0),
                "social_cohesion": simulation.getEnvironmentParameter("social_cohesion")
            }
            
            simulation_data["learning_progress"].append(learning_progress)
            
            # Print progress every 10 steps
            if step % 10 == 0:
                print(f"  Step {step:2d}: Agents={simulation_data['agent_metrics'][-1]['active_agents']}/{len(created_agents)}, "
                      f"Avg Fitness={simulation_data['agent_metrics'][-1]['average_fitness']:.2f}, "
                      f"Coordination={simulation_data['agent_metrics'][-1]['coordination_score']:.2f}")

        # Step 6: Stop simulation
        print("\nStep 6: Stopping simulation...")
        
        simulation.stop()
        
        # Get final statistics
        final_stats = simulation.getSimulationStats()
        performance = simulation.getSystemPerformance()
        
        print(f"  - Simulation stopped")
        print(f"  - Final statistics: {final_stats}")
        print(f"  - System performance: {performance}")

        # Step 7: Analyze results
        print("\nStep 7: Analyzing simulation results...")
        
        # Analyze agent performance
        agent_metrics = simulation_data["agent_metrics"]
        final_agent_metrics = agent_metrics[-1]
        
        print(f"  - Final active agents: {final_agent_metrics['active_agents']}")
        print(f"  - Final average fitness: {final_agent_metrics['average_fitness']:.3f}")
        print(f"  - Final average energy: {final_agent_metrics['average_energy']:.3f}")
        print(f"  - Final coordination score: {final_agent_metrics['coordination_score']:.3f}")
        
        # Analyze environmental changes
        env_changes = simulation_data["environmental_factors"]
        temperature_changes = [e["environmental_state"]["temperature"] for e in env_changes]
        
        print(f"  - Temperature variation: {max(temperature_changes) - min(temperature_changes):.1f}°C")
        print(f"  - Average temperature: {statistics.mean(temperature_changes):.1f}°C")
        
        # Analyze learning progress
        learning_progress = simulation_data["learning_progress"]
        learning_efficiency = [l["collective_learning"] for l in learning_progress]
        
        print(f"  - Learning efficiency trend: {learning_efficiency[-1]:.3f} (final)")
        print(f"  - Adaptation rate: {learning_progress[-1]['adaptation_rate']:.3f}")
        
        # Analyze coordination events
        coordination_events = simulation_data["coordination_events"]
        total_events = sum([e["event_count"] for e in coordination_events])
        avg_events_per_cycle = statistics.mean([e["event_count"] for e in coordination_events])
        
        print(f"  - Total coordination events: {total_events}")
        print(f"  - Average events per coordination cycle: {avg_events_per_cycle:.1f}")

        # Step 8: Test advanced simulation features
        print("\nStep 8: Testing advanced simulation features...")
        
        # Test memory modes
        print("  Testing memory modes...")
        for mode in ["Standard", "Compressed", "Sparse"]:
            simulation.setMemoryMode(mode)
            print(f"    Set memory mode: {mode}")
        
        # Test checkpointing
        print("  Testing checkpointing...")
        simulation.enableCheckpointing(True)
        print("    Checkpointing enabled")
        
        # Test lazy loading
        print("  Testing lazy loading...")
        simulation.enableLazyLoading(True)
        print("    Lazy loading enabled")
        
        # Test memory pooling
        print("  Testing memory pooling...")
        simulation.enableMemoryPooling(True)
        print("    Memory pooling enabled")
        
        # Test parallel processing with thread control
        print("  Testing parallel processing...")
        simulation.enableParallelProcessing(True)
        simulation.setThreadCount(2)
        active_threads = simulation.getActiveThreadCount()
        print(f"    Set {active_threads} active threads")

        # Step 9: Export simulation state
        print("\nStep 9: Exporting simulation state...")
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            simulation_file = f.name
        
        success = simulation.exportState(simulation_file)
        print(f"  - Simulation state exported: {success}")
        print(f"  - File saved to: {simulation_file}")

        # Step 10: Export configuration
        print("\nStep 10: Exporting configuration...")
        
        sim_config_file = simulation_file.replace('.json', '_config.json')
        success = simulation.exportConfiguration(sim_config_file)
        print(f"  - Configuration exported: {success}")
        print(f"  - File saved to: {sim_config_file}")

        # Step 11: Log simulation status
        print("\nStep 11: Logging simulation status...")
        simulation.logSimulationStatus()

        print("\n" + "="*60)
        print("EXAMPLE 3 COMPLETED SUCCESSFULLY")
        print("="*60)
        print(f"This example demonstrated {len(created_agents)} agents with {len(agent_templates)} roles,")
        print(f"simulating for 40 steps with coordination cycles and environmental changes.")
        print(f"Final coordination score: {final_agent_metrics['coordination_score']:.3f}")
        print("="*60)


class ExamplesPackage:
    """Package to manage all advanced examples."""

    def __init__(self):
        self.examples = [
            Example1AdvancedBrainFeatures(),
            Example2AdvancedPlasticityFeatures(),
            Example3AdvancedSimulationFeatures()
        ]

    def run_all_examples(self):
        """Run all examples in sequence."""
        print("\n" + "="*80)
        print("RUNNING ALL ADVANCED NLM EXAMPLES")
        print("="*80)
        print()
        
        for i, example in enumerate(self.examples, 1):
            try:
                example.run_example()
            except Exception as e:
                print(f"\nERROR in {example.title}:")
                print(f"  {str(e)}")
                print(f"  Skipping to next example...")
        
        print("\n" + "="*80)
        print("ALL EXAMPLES COMPLETED")
        print("="*80)
        
        return True


if __name__ == '__main__':
    if PYNLM_AVAILABLE:
        examples = ExamplesPackage()
        examples.run_all_examples()
    else:
        print("pynlm_advanced not available. Please install the NLM advanced features")
        print("to run these examples.")
        
        # Print available examples even if dependencies are missing
        print("\nAvailable advanced examples:")
        for example in ExamplesPackage().examples:
            print(f"  - {example.title}: {example.description}")
            print(f"    File: {example.code_file}")
        
        sys.exit(1)