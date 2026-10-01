"""
Comprehensive integration test suite for NLM advanced features.
Tests end-to-end workflows, multi-agent systems, Python-C++ integration,
and complex integration scenarios.
"""

import unittest
import os
import sys
import tempfile
import json
import time
import threading
from typing import Dict, Any, List, Optional

# Add paths for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm
    import pynlm_advanced
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

from pynlm import createDefaultConfig
from pynlm_advanced import (
    AdvancedBrain, AdvancedPlasticity, AdvancedSimulation,
    AdvancedAnalysis, AdvancedConfig
)

# Constants
MAX_INTEGRATION_TIME = 60.0
EXPECTED_PERFORMANCE = 5.0  # minimum steps/second
MAX_MEMORY_USAGE = 512  # MB


@unittest.skipIf(not PYNLM_AVAILABLE, "pynlm_advanced module not available")
class TestPythonCppIntegration(unittest.TestCase):
    """Test Python-C++ integration with advanced features."""

    def setUp(self):
        """Set up integration test fixtures."""
        self.temp_files = []
        
    def tearDown(self):
        """Clean up test fixtures."""
        for filepath in self.temp_files:
            try:
                if os.path.exists(filepath):
                    os.remove(filepath)
            except:
                pass

    def test_full_advanced_workflow(self):
        """Test complete advanced workflow end-to-end."""
        # Step 1: Create advanced configuration
        config = createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("connection_probability", 0.15)
        config.set("development_rate", 0.5)
        
        # Step 2: Create advanced brain
        brain = AdvancedBrain(config)
        brain.initialize()
        
        # Verify brain creation
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        
        # Step 3: Configure advanced plasticity
        plasticity = AdvancedPlasticity()
        plasticity_config = {
            "stdpLTPWeight": 0.015,
            "stdpLTDWeight": 0.018,
            "hebbianLearningRate": 0.02,
            "bcmLearningRate": 0.01,
            "minWeight": -1.0,
            "maxWeight": 1.0
        }
        plasticity.configure(plasticity_config)
        
        # Step 4: Configure advanced simulation
        simulation = AdvancedSimulation()
        sim_config = {
            "populationSize": 10,
            "environmentTemplate": "open_field",
            "parallelConfig": {
                "enableParallel": True,
                "numThreads": 2
            }
        }
        simulation.configure(sim_config)
        
        # Step 5: Run simulation with brain
        simulation.start()
        
        for step in range(20):
            simulation.step(step)
            
            # Verify simulation is running
            self.assertTrue(simulation.isRunning())
        
        simulation.stop()
        
        # Step 6: Perform analysis
        analysis = AdvancedAnalysis()
        analysis_config = {
            "enableNeuralPatternAnalysis": True,
            "enableLearningCurveAnalysis": True,
            "enableStatisticalAnalysis": True
        }
        analysis.configure(analysis_config)
        
        # Generate some data for analysis
        brain.step(21)  # Continue brain steps
        
        # Test pattern analysis
        test_data = [0.1, 0.2, 0.3, 0.4, 0.5]
        pattern_result = analysis.analyzeNeuralPattern(test_data)
        self.assertIsNotNone(pattern_result)
        
        # Test learning curve analysis
        steps = list(range(10))
        learning_curve = analysis.analyzeLearningCurve(steps, test_data)
        self.assertIsNotNone(learning_curve)
        
        # Step 7: Configure advanced settings
        advanced_config = AdvancedConfig()
        
        # Create template
        template = {
            "name": "integration_test_template",
            "defaultValues": {"neuron_count": 2000},
            "templateType": "test"
        }
        
        advanced_config.createTemplate(template)
        
        # Export configuration
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
            self.temp_files.append(config_file)
        
        success = advanced_config.exportConfiguration(config, config_file)
        self.assertTrue(success)
        
        # Import configuration
        imported_config = createDefaultConfig()
        success = advanced_config.importConfiguration(imported_config, config_file)
        self.assertTrue(success)
        
        # Verify all components are working together
        final_neuron_count = brain.getTotalNeuronCount()
        self.assertGreater(final_neuron_count, 0)
        
    def test_multi_agent_system_integration(self):
        """Test multi-agent system integration with advanced features."""
        # Create advanced simulation
        simulation = AdvancedSimulation()
        
        # Configure multi-agent system
        sim_config = {
            "populationSize": 20,
            "environmentTemplate": "complex_environment",
            "parallelConfig": {
                "enableParallel": True,
                "numThreads": 4,
                "schedulingPolicy": "Dynamic"
            },
            "experimentControl": {
                "enableStepControl": True,
                "enableRewardControl": True,
                "simulationSpeed": 1.0
            }
        }
        
        simulation.configure(sim_config)
        
        # Create agents with different types
        agent_configs = [
            {"type": "coordinator", "behavior": "hierarchical"},
            {"type": "explorer", "behavior": "swarm"},
            {"type": "learner", "behavior": "individual"},
            {"type": "balancer", "behavior": "consensus"}
        ]
        
        agent_ids = []
        for agent_config in agent_configs:
            for i in range(5):  # 5 agents of each type
                agent_id = simulation.createAgent(agent_config["type"])
                agent_ids.append(agent_id)
                simulation.activateAgent(agent_id)
        
        # Start simulation
        simulation.start()
        
        # Run extended simulation
        start_time = time.time()
        for step in range(50):
            simulation.step(step)
            
            # Execute coordination cycle every 5 steps
            if step % 5 == 0:
                simulation.executeCoordinationCycle()
            
            # Check simulation performance
            elapsed = time.time() - start_time
            steps_per_second = step / elapsed if elapsed > 0 else 0
            self.assertGreater(steps_per_second, EXPECTED_PERFORMANCE)
        
        simulation.stop()
        
        # Get comprehensive statistics
        stats = simulation.getSimulationStats()
        performance = simulation.getSystemPerformance()
        
        # Verify performance metrics
        self.assertIn("total_steps", stats)
        self.assertIn("agent_count", stats)
        self.assertIn("performance_metrics", stats)
        
        self.assertIn("cpu_usage", performance)
        self.assertIn("memory_usage", performance)
        self.assertIn("simulation_speed", performance)
        
        # Get agent states
        agent_states = simulation.getAllAgentStates()
        self.assertEqual(len(agent_states), len(agent_ids))
        
        # Verify agent diversity
        agent_types = set()
        for state in agent_states:
            agent_types.add(state["type"])
        
        self.assertGreater(len(agent_types), 1)
        
    def test_plasticity_learning_integration(self):
        """Test plasticity and learning integration."""
        # Create brain and advanced simulation
        brain_config = createDefaultConfig()
        brain_config.set("neuron_count", 500)
        
        brain = AdvancedBrain(brain_config)
        brain.initialize()
        
        simulation = AdvancedSimulation()
        sim_config = {
            "populationSize": 5,
            "environmentTemplate": "learning_environment"
        }
        simulation.configure(sim_config)
        
        # Create agents
        for i in range(5):
            agent_id = simulation.createAgent("learner")
            simulation.activateAgent(agent_id)
            
            # Set learning rewards
            simulation.setReward(agent_id, 0.5 + (i * 0.1))
        
        # Configure advanced plasticity
        plasticity = AdvancedPlasticity()
        plasticity_config = {
            "stdpLTPWeight": 0.015,
            "stdpLTDWeight": 0.018,
            "hebbianLearningRate": 0.02,
            "rewardModulationFactor": 1.2,
            "predictionErrorModulation": 1.1
        }
        plasticity.configure(plasticity_config)
        
        # Run learning simulation
        simulation.start()
        
        for step in range(30):
            # Brain step
            brain.step(step)
            
            # Simulation step with plasticity
            simulation.step(step)
            
            # Trigger learning every 10 steps
            if step % 10 == 0:
                simulation.triggerLearning()
                
                # Apply plasticity
                synapse_data = {
                    "pre_synaptic": [0.0, 10.0, 20.0],
                    "post_synaptic": [5.0, 15.0],
                    "dt": 1.0,
                    "neuromodulation": 1.0
                }
                
                plasticity.applySTDP(synapse_data, synapse_data["pre_synaptic"], 
                                   synapse_data["post_synaptic"], synapse_data["dt"],
                                   synapse_data["neuromodulation"])
        
        simulation.stop()
        
        # Verify learning progress
        analysis = AdvancedAnalysis()
        analysis_config = {
            "enableLearningCurveAnalysis": True,
            "enableStatisticalAnalysis": True
        }
        analysis.configure(analysis_config)
        
        # Generate learning metrics
        steps = list(range(10))
        performance_metrics = [0.1, 0.3, 0.6, 0.8, 0.9, 0.95, 0.98, 1.0,
                             1.0, 1.0]
        
        learning_result = analysis.analyzeLearningCurve(steps, performance_metrics)
        self.assertIsNotNone(learning_result)
        self.assertIn("convergence_point", learning_result)

    def test_memory_efficient_simulation(self):
        """Test memory-efficient simulation with checkpointing."""
        # Create simulation with memory optimization
        simulation = AdvancedSimulation()
        
        sim_config = {
            "populationSize": 10,
            "environmentTemplate": "memory_efficient",
            "experimentControl": {
                "memoryMode": "Compressed",
                "enableCheckpointing": True,
                "enableLazyLoading": True,
                "enableMemoryPooling": True
            },
            "parallelConfig": {
                "enableParallel": False,  # Disable for memory efficiency test
                "numThreads": 1
            }
        }
        
        simulation.configure(sim_config)
        
        # Enable memory optimization features
        simulation.enableCheckpointing(True)
        simulation.enableLazyLoading(True)
        simulation.enableMemoryPooling(True)
        
        # Create and activate agents
        for i in range(5):
            agent_id = simulation.createAgent("memory_test")
            simulation.activateAgent(agent_id)
        
        # Run simulation with memory monitoring
        simulation.start()
        
        for step in range(40):
            simulation.step(step)
            
            # Test checkpointing at specific intervals
            if step % 10 == 0:
                # This would save state in a real implementation
                pass
            
            # Test lazy loading at specific intervals
            if step % 15 == 0:
                # This would load components on-demand in a real implementation
                pass
        
        simulation.stop()
        
        # Verify memory optimization was applied
        memory_mode = simulation.getMemoryMode()
        self.assertEqual(memory_mode, "Compressed")
        
        # Get system performance with memory metrics
        performance = simulation.getSystemPerformance()
        self.assertIn("memory_usage", performance)
        self.assertLess(performance["memory_usage"], MAX_MEMORY_USAGE)

    def test_configuration_advanced_features(self):
        """Test advanced configuration features integration."""
        # Create advanced configuration
        adv_config = AdvancedConfig()
        
        # Test 1: Configuration inheritance
        base_config = createDefaultConfig()
        base_config.set("neuron_count", 1000)
        base_config.set("connection_probability", 0.2)
        
        child_config = createDefaultConfig()
        child_config.set("neuron_count", 500)  # Override
        
        # Set up inheritance
        inheritance_config = {
            "baseConfig": "main_config",
            "inheritFrom": "parent_config",
            "inheritanceStrategy": "merge"
        }
        
        success = adv_config.inheritConfiguration(inheritance_config)
        self.assertTrue(success)
        
        # Test 2: Configuration templates
        template = {
            "name": "advanced_test_template",
            "description": "Template for advanced feature testing",
            "defaultValues": {
                "neuron_count": 2000,
                "connection_probability": 0.15,
                "learning_rate": 0.05,
                "development_rate": 0.3
            },
            "templateType": "advanced_testing",
            "validationRules": [
                {"key": "neuron_count", "type": "range", "min": 100, "max": 10000},
                {"key": "learning_rate", "type": "range", "min": 0.0, "max": 1.0}
            ]
        }
        
        success = adv_config.createTemplate(template)
        self.assertTrue(success)
        
        # Apply template
        template_config = createDefaultConfig()
        applied_config = adv_config.applyTemplate("advanced_test_template", template_config)
        
        # Verify template application
        self.assertEqual(applied_config.get("neuron_count"), 2000)
        self.assertEqual(applied_config.get("connection_probability"), 0.15)
        
        # Test 3: Schema validation
        schema = {
            "schema_name": "advanced_config_schema",
            "fields": [
                {"name": "neuron_count", "type": "integer", "min": 100, "max": 10000},
                {"name": "connection_probability", "type": "float", "min": 0.0, "max": 1.0},
                {"name": "learning_rate", "type": "float", "min": 0.0, "max": 1.0}
            ]
        }
        
        adv_config.createSchema(schema)
        
        # Validate template config
        is_valid = adv_config.validateConfiguration(template_config)
        self.assertTrue(is_valid)
        
        # Test 4: Parameter optimization
        optimization_config = {
            "method": "random_search",
            "objective": "maximize_stability",
            "constraints": [
                {"parameterName": "neuron_count", "minValue": 500, "maxValue": 2000},
                {"parameterName": "learning_rate", "minValue": 0.01, "maxValue": 0.1}
            ],
            "hyperparameters": {
                "population_size": 8,
                "generations": 3
            }
        }
        
        optimized_config = adv_config.optimizeParameters(optimization_config)
        self.assertIsNotNone(optimized_config)
        
        # Test 5: Configuration versioning
        version_info = adv_config.createVersion(template_config, "Template-based configuration")
        self.assertIsNotNone(version_info)
        
        # Get version history
        version_history = adv_config.getVersionHistory()
        self.assertGreater(len(version_history), 0)
        
        # Test 6: Configuration comparison
        config_a = createDefaultConfig()
        config_a.set("neuron_count", 1000)
        
        config_b = createDefaultConfig()
        config_b.set("neuron_count", 1500)
        
        comparison = adv_config.compareConfigurations(config_a, config_b)
        self.assertIn("differences", comparison)
        
        # Calculate configuration distance
        distance = adv_config.calculateConfigurationDistance(config_a, config_b)
        self.assertGreater(distance, 0.0)


@unittest.skipIf(not PYNLM_AVAILABLE, "pynlm_advanced module not available")
class TestEndToEndWorkflows(unittest.TestCase):
    """Test end-to-end workflows and real-world scenarios."""

    def setUp(self):
        """Set up end-to-end test fixtures."""
        self.temp_files = []
        
    def tearDown(self):
        """Clean up test fixtures."""
        for filepath in self.temp_files:
            try:
                if os.path.exists(filepath):
                    os.remove(filepath)
            except:
                pass

    def test_research_experiment_workflow(self):
        """Test research experiment workflow with advanced features."""
        project_name = "research_experiment_" + str(int(time.time()))
        
        try:
            # Phase 1: Configuration Setup
            print(f"Phase 1: Setting up configuration for {project_name}")
            
            base_config = createDefaultConfig()
            base_config.set("neuron_count", 2000)
            base_config.set("connection_probability", 0.15)
            base_config.set("development_rate", 0.3)
            base_config.set("learning_rate", 0.05)
            
            # Phase 2: Advanced Brain Configuration
            print("Phase 2: Configuring advanced brain")
            
            brain = AdvancedBrain(base_config)
            brain.initialize()
            
            # Enable advanced analysis features
            brain.enableAnalysisFeature("neural_pattern_analysis")
            brain.enableAnalysisFeature("learning_curve_analysis")
            brain.enableAnalysisFeature("statistical_analysis")
            
            # Phase 3: Plasticity Configuration
            print("Phase 3: Configuring advanced plasticity")
            
            plasticity = AdvancedPlasticity()
            plasticity_config = {
                "stdpLTPWeight": 0.015,
                "stdpLTDWeight": 0.018,
                "hebbianLearningRate": 0.02,
                "bcmLearningRate": 0.01,
                "rewardModulationFactor": 1.2,
                "predictionErrorModulation": 1.1
            }
            plasticity.configure(plasticity_config)
            
            # Phase 4: Multi-Agent Simulation
            print("Phase 4: Setting up multi-agent simulation")
            
            simulation = AdvancedSimulation()
            sim_config = {
                "populationSize": 15,
                "environmentTemplate": "research_environment",
                "parallelConfig": {
                    "enableParallel": True,
                    "numThreads": 4
                },
                "experimentControl": {
                    "enableStepControl": True,
                    "enableRewardControl": True,
                    "enableNoiseControl": True,
                    "simulationSpeed": 1.0
                }
            }
            simulation.configure(sim_config)
            
            # Create diverse agent population
            agent_types = ["researcher", "collaborator", "evaluator", "optimizer"]
            for agent_type in agent_types:
                for i in range(4):
                    agent_id = simulation.createAgent(agent_type)
                    simulation.activateAgent(agent_id)
                    
                    # Set type-specific rewards
                    if agent_type == "researcher":
                        simulation.setReward(agent_id, 1.0)
                    elif agent_type == "collaborator":
                        simulation.setReward(agent_id, 0.8)
                    elif agent_type == "evaluator":
                        simulation.setReward(agent_id, 0.6)
                    else:
                        simulation.setReward(agent_id, 0.9)
            
            # Phase 5: Advanced Analysis Setup
            print("Phase 5: Setting up advanced analysis")
            
            analysis = AdvancedAnalysis()
            analysis_config = {
                "enableNeuralPatternAnalysis": True,
                "enableLearningCurveAnalysis": True,
                "enableStatisticalAnalysis": True,
                "enableMachineLearning": True,
                "analysisInterval": 0.5
            }
            analysis.configure(analysis_config)
            
            # Phase 6: Simulation Execution
            print("Phase 6: Running simulation")
            
            simulation.start()
            
            experiment_data = {
                "step": 0,
                "agent_states": [],
                "learning_metrics": [],
                "performance_metrics": []
            }
            
            for step in range(40):
                # Update simulation
                simulation.step(step)
                
                # Record experiment data
                agent_states = simulation.getAllAgentStates()
                experiment_data["agent_states" = agent_states]
                
                # Get performance metrics
                performance = simulation.getSystemPerformance()
                experiment_data["performance_metrics"].append(performance)
                
                # Periodically analyze data
                if step % 10 == 0:
                    # Collect neural data from brain
                    brain_step_data = brain.getTotalNeuronCount()
                    experiment_data["learning_metrics"].append({
                        "step": step,
                        "neuron_count": brain_step_data,
                        "agent_count": len(agent_states)
                    })
                    
                    # Perform pattern analysis on sample data
                    sample_data = [0.1 * (i % 10) for i in range(20)]
                    pattern_result = analysis.analyzeNeuralPattern(sample_data)
                    experiment_data["pattern_analysis"].append(pattern_result)
            
            simulation.stop()
            
            # Phase 7: Advanced Configuration Analysis
            print("Phase 7: Advanced configuration analysis")
            
            adv_config = AdvancedConfig()
            
            # Create research-specific template
            research_template = {
                "name": "research_template",
                "description": "Template optimized for research experiments",
                "defaultValues": {
                    "neuron_count": 3000,
                    "connection_probability": 0.1,
                    "learning_rate": 0.03,
                    "development_rate": 0.2,
                    "plasticity_strength": 0.8
                },
                "validationRules": [
                    {"key": "neuron_count", "type": "range", "min": 1000, "max": 5000},
                    {"key": "learning_rate", "type": "range", "min": 0.01, "max": 0.1}
                ]
            }
            
            adv_config.createTemplate(research_template)
            
            # Validate experiment results
            is_valid = adv_config.validateConfiguration(base_config)
            self.assertTrue(is_valid, "Experiment configuration should be valid")
            
            # Phase 8: Results Export and Analysis
            print("Phase 8: Exporting and analyzing results")
            
            # Export experiment data
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
                experiment_file = f.name
                self.temp_files.append(experiment_file)
            
            # Create comprehensive results report
            results_report = {
                "project_name": project_name,
                "start_time": time.time(),
                "end_time": time.time(),
                "total_steps": 40,
                "final_agent_states": experiment_data["agent_states"],
                "performance_metrics": experiment_data["performance_metrics"],
                "learning_metrics": experiment_data["learning_metrics"],
                "pattern_analysis_results": experiment_data.get("pattern_analysis", []),
                "brain_statistics": {
                    "final_neuron_count": brain.getTotalNeuronCount(),
                    "final_synapse_count": brain.getTotalSynapseCount(),
                    "average_firing_rate": brain.getAverageFiringRate()
                }
            }
            
            with open(experiment_file, 'w') as f:
                json.dump(results_report, f, indent=2)
            
            # Verify results
            self.assertGreater(results_report["final_neuron_count"], 0)
            self.assertGreater(results_report["final_synapse_count"], 0)
            self.assertGreater(len(results_report["performance_metrics"]), 0)
            
            # Clean up
            if os.path.exists(experiment_file):
                os.remove(experiment_file)
                
        except Exception as e:
            self.fail(f"End-to-end workflow failed: {str(e)}")

    def test_real_world_scenario_simulation(self):
        """Test real-world scenario simulation with advanced plasticity."""
        # Scenario: Learning in dynamic environment
        scenario_config = {
            "environment_type": "dynamic_learning_environment",
            "agent_population": 8,
            "simulation_duration": 60,
            "advanced_features_enabled": True
        }
        
        # Create advanced components
        brain = AdvancedBrain(createDefaultConfig())
        brain.initialize()
        
        # Configure for realistic scenario
        brain.setSimulationMode("EventDriven")
        brain.enableParallelProcessing(True)
        brain.enableMemoryOptimization(True)
        
        # Enable advanced analysis features
        analysis_features = [
            "neural_pattern_analysis",
            "learning_curve_analysis",
            "statistical_analysis",
            "machine_learning_integration"
        ]
        
        for feature in analysis_features:
            brain.enableAnalysisFeature(feature)
        
        # Configure advanced plasticity for realistic learning
        plasticity = AdvancedPlasticity()
        plasticity_config = {
            "stdpLTPWeight": 0.01,
            "stdpLTDWeight": 0.02,
            "hebbianLearningRate": 0.015,
            "bcmLearningRate": 0.008,
            "rewardModulationFactor": 1.5,
            "predictionErrorModulation": 1.3,
            "noveltyModulation": 0.8,
            "minWeight": -0.5,
            "maxWeight": 0.5,
            "weightChangeLimit": 0.05
        }
        plasticity.configure(plasticity_config)
        
        # Create advanced simulation for the scenario
        simulation = AdvancedSimulation()
        sim_config = {
            "populationSize": scenario_config["agent_population"],
            "environmentTemplate": scenario_config["environment_type"],
            "parallelConfig": {
                "enableParallel": True,
                "numThreads": 4,
                "schedulingPolicy": "Workstealing"
            },
            "experimentControl": {
                "enableStepControl": True,
                "enableRewardControl": True,
                "enableNoiseControl": True,
                "simulationSpeed": 1.0,
                "stepDuration": 0.1
            }
        }
        
        simulation.configure(sim_config)
        
        # Create agents with realistic behaviors
        agent_templates = [
            {"type": "exploratory", "behavior": "random_walk", "learning_rate": 0.1},
            {"type": "exploiter", "behavior": "greedy", "learning_rate": 0.05},
            {"type": "cooperator", "behavior": "altruistic", "learning_rate": 0.08},
            {"type": "adaptable", "behavior": "adaptive", "learning_rate": 0.12}
        ]
        
        for template in agent_templates:
            for i in range(2):  # 2 agents per type
                agent_id = simulation.createAgent(template["type"])
                simulation.activateAgent(agent_id)
                
                # Set rewards based on agent type
                base_reward = 0.5
                if template["type"] == "exploratory":
                    simulation.setReward(agent_id, base_reward * 1.2)
                elif template["type"] == "exploiter":
                    simulation.setReward(agent_id, base_reward * 1.0)
                elif template["type"] == "cooperator":
                    simulation.setReward(agent_id, base_reward * 0.9)
                else:
                    simulation.setReward(agent_id, base_reward * 1.1)
        
        # Start realistic simulation
        simulation.start()
        
        # Run scenario with time tracking
        start_time = time.time()
        scenario_data = {
            "step": 0,
            "timestamp": start_time,
            "agent_metrics": [],
            "brain_metrics": [],
            "environmental_factors": [],
            "learning_progress": []
        }
        
        # Dynamic environment changes
        environmental_changes = [
            (10, {"temperature": 25.0, "humidity": 0.5}),
            (30, {"temperature": 30.0, "humidity": 0.7}),
            (50, {"temperature": 20.0, "humidity": 0.3})
        ]
        
        for step in range(scenario_config["simulation_duration"]):
            current_time = time.time()
            elapsed = current_time - start_time
            
            # Update environmental factors based on time
            for change_step, factors in environmental_changes:
                if step == change_step:
                    for factor_name, value in factors.items():
                        simulation.setEnvironmentParameter(factor_name, value)
            
            # Get current environmental state
            env_state = {}
            for factor_name in ["temperature", "humidity", "light_intensity"]:
                env_state[factor_name] = simulation.getEnvironmentParameter(factor_name)
            
            scenario_data["environmental_factors"].append({
                "step": step,
                "elapsed_seconds": elapsed,
                "environment_state": env_state
            })
            
            # Update brain and collect metrics
            brain.step(step)
            
            brain_metrics = {
                "step": step,
                "neuron_count": brain.getTotalNeuronCount(),
                "synapse_count": brain.getTotalSynapseCount(),
                "active_neurons": brain.getActiveNeuronCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "plasticity_indicators": []  # Would collect from plasticity system
            }
            
            scenario_data["brain_metrics"].append(brain_metrics)
            
            # Update simulation
            simulation.step(step)
            
            # Record agent metrics
            agent_states = simulation.getAllAgentStates()
            agent_metrics = []
            
            for agent_state in agent_states:
                agent_metric = {
                    "agent_id": agent_state["id"],
                    "type": agent_state["type"],
                    "health": agent_state["health"],
                    "energy": agent_state["energy"],
                    "experience": agent_state["experience"],
                    "fitness": agent_state["fitness"],
                    "attributes": agent_state["attributes"]
                }
                agent_metrics.append(agent_metric)
            
            scenario_data["agent_metrics"].append(agent_metrics)
            
            # Record learning progress
            learning_progress = {
                "step": step,
                "overall_performance": len([a for a in agent_metrics if a["fitness"] > 0.7]) / len(agent_metrics),
                "adaptation_rate": 0.1 * (step / scenario_config["simulation_duration"]),
                "cohesion": len(set(a["type"] for a in agent_metrics)) / len(agent_metrics)
            }
            
            scenario_data["learning_progress"].append(learning_progress)
            
            # Periodically analyze data
            if step % 15 == 0:
                # Analyze learning curves
                steps_data = list(range(10))
                performance_data = [lp["overall_performance"] for lp in scenario_data["learning_progress"][-10:]]
                
                analysis = AdvancedAnalysis()
                analysis_config = {
                    "enableLearningCurveAnalysis": True,
                    "enableStatisticalAnalysis": True
                }
                analysis.configure(analysis_config)
                
                learning_result = analysis.analyzeLearningCurve(steps_data, performance_data)
                scenario_data["learning_analysis"] = learning_result
                
                # Detect patterns
                pattern_result = analysis.analyzeNeuralPattern(performance_data)
                scenario_data["pattern_analysis"] = pattern_result
        
        simulation.stop()
        
        # Analyze final results
        final_results = {
            "total_steps": scenario_config["simulation_duration"],
            "total_agents": scenario_config["agent_population"],
            "final_brain_state": scenario_data["brain_metrics"][-1],
            "average_agent_performance": sum(lp["overall_performance"] for lp in scenario_data["learning_progress"]) / len(scenario_data["learning_progress"]),
            "environment_stability": len(set(f["environment_state"]["temperature"] for f in scenario_data["environmental_factors"])) / len(scenario_data["environmental_factors"]),
            "agent_diversity": len(set(am["type"] for am_list in scenario_data["agent_metrics"] for am in am_list)) / scenario_config["agent_population"]
        }
        
        # Verify scenario results
        self.assertGreater(final_results["average_agent_performance"], 0.5)
        self.assertGreater(final_results["environment_stability"], 0.5)
        self.assertGreater(final_results["agent_diversity"], 0.3)
        
        # Export results
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            results_file = f.name
            self.temp_files.append(results_file)
        
        with open(results_file, 'w') as f:
            json.dump(scenario_data, f, indent=2)
        
        # Verify results file
        self.assertTrue(os.path.exists(results_file))
        

if __name__ == '__main__':
    unittest.main()