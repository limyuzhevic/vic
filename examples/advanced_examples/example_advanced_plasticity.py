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


class Example2AdvancedPlasticityFeatures:
    """Example 2: Advanced Plasticity Mechanisms - STDP, Hebbian, BCM, Oja, etc."""

    def __init__(self):
        self.title = "Advanced Plasticity Mechanisms Example"
        self.description = "Demonstrates multiple learning rules and plasticity mechanisms"
        self.code_file = "example_advanced_plasticity.py"

    def run_example(self):
        """Run the advanced plasticity example."""
        if not PYNLM_AVAILABLE:
            print("Skipping example - pynlm_advanced not available")
            return

        print("\n" + "="*60)
        print(f"EXAMPLE 2: {self.title}")
        print("="*60)
        print(f"Description: {self.description}")
        print()

        # Step 1: Create brain with advanced plasticity
        print("Step 1: Creating brain for plasticity testing...")
        config = createDefaultConfig()
        config.set("neuron_count", 800)
        
        brain = AdvancedBrain(config)
        brain.initialize()
        
        print(f"  - Brain initialized with {brain.getTotalNeuronCount()} neurons")

        # Step 2: Configure advanced plasticity system
        print("\nStep 2: Configuring advanced plasticity system...")
        
        plasticity = AdvancedPlasticity()
        
        # Comprehensive plasticity configuration
        plasticity_config = {
            "stdpLTPWeight": 0.015,
            "stdpLTDWeight": 0.018,
            "stdpTimeConstant": 25.0,
            "stdpTraceDecay": 10.0,
            "hebbianLearningRate": 0.02,
            "hebbianThreshold": 0.5,
            "bcmLearningRate": 0.01,
            "bcmThreshold": 0.6,
            "bcmSlidingThreshold": 0.1,
            "ojaLearningRate": 0.015,
            "ojaRegularization": 0.5,
            "httPreWeight": 0.8,
            "httPostWeight": 0.8,
            "homeostaticTarget": 1.0,
            "homeostaticLearningRate": 0.01,
            "homeostaticTimeConstant": 50.0,
            "rewardModulationFactor": 1.2,
            "predictionErrorModulation": 1.1,
            "noveltyModulation": 0.9,
            "minWeight": -1.0,
            "maxWeight": 1.0,
            "weightChangeLimit": 0.1
        }
        
        plasticity.configure(plasticity_config)
        
        print(f"  - Plasticity configured with {len(plasticity_config)} parameters")
        print(f"  - STDP parameters: LTP={plasticity_config['stdpLTPWeight']}, "
              f"LTD={plasticity_config['stdpLTDWeight']}, "
              f"tau={plasticity_config['stdpTimeConstant']}")
        print(f"  - Hebbian parameters: learning_rate={plasticity_config['hebbianLearningRate']}, "
              f"threshold={plasticity_config['hebbianThreshold']}")

        # Step 3: Add all plasticity rules
        print("\nStep 3: Adding plasticity rules...")
        
        plasticity_rules = [
            ("STDP", "STDP"),
            ("Hebbian", "Hebbian"),
            ("BCM", "BCM"),
            ("Oja", "Oja"),
            ("HTT", "HTT"),
            ("RewardModulatedSTDP", "RewardModulatedSTDP"),
            ("TripletSTDP", "TripletSTDP"),
            ("EligibilityTrace", "EligibilityTrace"),
            ("SynapticTagging", "SynapticTagging"),
            ("Homeostatic", "Homeostatic")
        ]
        
        for rule_name, rule_type in plasticity_rules:
            success = plasticity.addPlasticityRule(rule_name, rule_type)
            print(f"  - Added {rule_type} rule: {rule_name} - {'success' if success else 'failed'}")

        # Step 4: Set active rule and test learning
        print("\nStep 4: Testing different learning mechanisms...")
        
        test_scenarios = [
            ("STDP learning", "STDP", lambda: plasticity.applySTDP({}, [], [], 1.0, 1.0)),
            ("Hebbian learning", "Hebbian", lambda: plasticity.applyConstrainedHebbian({}, [], [], 1.0)),
            ("BCM learning", "BCM", lambda: plasticity.applyBCM({}, [], [], 1.0)),
            ("Oja learning", "Oja", lambda: plasticity.applyOja({}, [], [], 1.0)),
            ("Homeostatic adjustment", "Homeostatic", lambda: plasticity.adjustHomeostasis(1.5))
        ]
        
        for scenario_name, rule_name, learning_function in test_scenarios:
            print(f"\n  Testing {scenario_name}...")
            
            # Set active rule
            plasticity.setActiveRule(rule_name)
            current_rule = plasticity.getActiveRule()
            print(f"    Active rule: {current_rule}")
            
            # Perform learning
            start_time = time.time()
            for i in range(10):
                learning_function()
            end_time = time.time()
            
            learning_time = end_time - start_time
            print(f"    Learning completed in {learning_time*1000:.2f}ms")
            
            # Get statistics
            stats = plasticity.calculateStats()
            print(f"    Statistics - Average weight: {stats.get('average_weight', 0):.3f}, "
                  f"Variance: {stats.get('weight_variance', 0):.3f}")

        # Step 5: Test advanced plasticity features
        print("\nStep 5: Testing advanced plasticity features...")
        
        # Create test synapse for advanced features
        class TestSynapse:
            def __init__(self):
                self.id = 12345
                self.weight = 0.5
                self.pre_synaptic_activity = []
                self.post_synaptic_activity = []
        
        test_synapse = TestSynapse()
        
        # Test trace updates
        print("  Testing trace updates...")
        for i in range(5):
            pre_spikes = [float(i * 10)]
            post_spikes = [float(i * 10 + 5)]
            plasticity.updateSynapse(test_synapse, pre_spikes, post_spikes, 1.0, 1.0)
        
        trace_value = plasticity.getTraceValue(test_synapse)
        print(f"    Trace value: {trace_value:.3f}")
        
        # Test synaptic tagging
        print("  Testing synaptic tagging...")
        plasticity.tagSynapse(test_synapse.id, 0.8)
        has_tag = plasticity.hasTag(test_synapse.id)
        tag_strength = plasticity.getTagStrength(test_synapse.id)
        print(f"    Has tag: {has_tag}, Tag strength: {tag_strength:.3f}")
        
        # Test homeostasis
        print("  Testing homeostasis...")
        plasticity.adjustHomeostasis(1.5)  # High activity
        homeostasis_stats = plasticity.calculateStats()
        print(f"    Homeostasis stats: {homeostasis_stats}")
        
        # Test pattern learning
        print("  Testing pattern learning...")
        pattern_data = [0.1, 0.2, 0.3, 0.4, 0.5]
        plasticity.applyPatternLearning(pattern_data)
        print(f"    Pattern learning applied to {len(pattern_data)}-element sequence")
        
        # Test reward modulation
        print("  Testing reward modulation...")
        plasticity.applyRewardModulation(0.7)
        print(f"    Reward modulation applied with factor: 0.7")
        
        # Test prediction error
        print("  Testing prediction error...")
        plasticity.applyPredictionError(0.3)
        print(f"    Prediction error applied with value: 0.3")
        
        # Test custom learning rules
        print("  Testing custom learning rules...")
        custom_rule = {
            "name": "custom_biological_rule",
            "type": "custom",
            "parameters": {
                "base_strength": 0.5,
                "modulation_factor": 1.2,
                "decay_rate": 0.05
            }
        }
        
        plasticity.setCustomLearningRule("custom_biological_rule", custom_rule)
        print(f"    Custom rule added: {custom_rule['name']}")

        # Step 6: Test trace and tag management
        print("\nStep 6: Testing trace and tag management...")
        
        # Reset traces
        plasticity.resetTraces()
        print("  - Traces reset")
        
        # Create multiple synapses for testing
        test_synapses = []
        for i in range(5):
            synapse = TestSynapse()
            synapse.id = 10000 + i
            test_synapses.append(synapse)
        
        # Update traces for all synapses
        for i, synapse in enumerate(test_synapses):
            pre_spikes = [float(i * 10)]
            post_spikes = [float(i * 10 + 5)]
            plasticity.updateSynapse(synapse, pre_spikes, post_spikes, 1.0, 1.0)
        
        # Get trace values
        trace_values = [plasticity.getTraceValue(synapse) for synapse in test_synapses]
        print(f"  - Trace values: {[f'{v:.3f}' for v in trace_values]}")
        
        # Test weight limit enforcement
        print("  Testing weight limit enforcement...")
        # Create synapse with extreme weight
        extreme_synapse = TestSynapse()
        extreme_synapse.weight = 2.0  # Exceeds max_weight
        
        plasticity.updateSynapse(extreme_synapse, [], [], 1.0, 1.0)
        
        # Check if weight limits were enforced
        plasticity.enforceWeightLimits()
        print(f"  - Weight limits enforced (weight should be capped at 1.0)")

        # Step 7: Test configuration export/import
        print("\nStep 7: Testing configuration export/import...")
        
        # Export current configuration
        exported_config = plasticity.exportConfiguration()
        print(f"  - Exported configuration with {len(exported_config)} parameters")
        
        # Modify configuration
        exported_config["test_parameter"] = "test_value"
        
        # Import modified configuration
        plasticity.importConfiguration(exported_config)
        print(f"  - Imported modified configuration")
        
        # Verify import
        imported_value = plasticity.getConfigValue("test_parameter")
        print(f"  - Imported parameter value: {imported_value}")

        # Step 8: Log plasticity status
        print("\nStep 8: Logging plasticity status...")
        plasticity.logStatus()

        print("\n" + "="*60)
        print("EXAMPLE 2 COMPLETED SUCCESSFULLY")
        print("="*60)
        print(f"This example tested {len(plasticity_rules)} plasticity rules,")
        print(f"{len(test_scenarios)} learning scenarios.")
        print("="*60)


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