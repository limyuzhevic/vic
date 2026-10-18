#!/usr/bin/env python3
"""
Comprehensive Demo Example
===========================

This example demonstrates full integration of all NLM Python API features.
It shows advanced workflow examples, complex parameter management,
and comprehensive error handling.

Key Features Demonstrated:
- Full integration of all new features
- Advanced workflow examples
- Complex parameter management
- Advanced error handling

File: examples/comprehensive_demo.py
"""

import time
import sys
import os
import json
from datetime import datetime
from typing import Dict, Any, List

# Add the python bindings directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm as nlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Error: pynlm module not available: {e}")
    sys.exit(1)

class ParameterValidator:
    """Advanced parameter validation for comprehensive demos."""
    
    def __init__(self):
        self.validation_rules = {
            "brain.neuron_count": {
                "type": "range",
                "min": 100,
                "max": 100000,
                "warning_if_greater_than": 10000
            },
            "plasticity.stdp.learning_rate": {
                "type": "range",
                "min": 0.0001,
                "max": 0.1
            },
            "neuromod.dopamine.scale": {
                "type": "range",
                "min": 0.1,
                "max": 10.0
            }
        }
    
    def validate_parameter(self, param: str, value: Any) -> List[str]:
        """
        Validate a single parameter.
        
        Args:
            param: Parameter name
            value: Parameter value
            
        Returns:
            List[str]: List of validation messages
        """
        messages = []
        
        if param not in self.validation_rules:
            return messages
        
        rule = self.validation_rules[param]
        
        # Type validation
        if rule["type"] == "range":
            if "min" in rule and value < rule["min"]:
                messages.append(
                    f"Parameter {param}: {value} below minimum {rule['min']}"
                )
            if "max" in rule and value > rule["max"]:
                messages.append(
                    f"Parameter {param}: {value} above maximum {rule['max']}"
                )
            
            # Warning for large values
            if "warning_if_greater_than" in rule and value > rule["warning_if_greater_than"]:
                messages.append(
                    f"⚠️ Warning: {param} is very large ({value}) - may impact performance"
                )
        
        return messages
    
    def validate_parameter(self, param: str, value: Any) -> List[str]:
        """
        Validate a single parameter.
        
        Args:
            param: Parameter name
            value: Parameter value
            
        Returns:
            List[str]: List of validation messages
        """
        messages = []
        
        if param not in self.validation_rules:
            return messages
        
        rule = self.validation_rules[param]
        
        # Type validation
        if rule["type"] == "range":
            if "min" in rule and value < rule["min"]:
                messages.append(
                    f"Parameter {param}: {value} below minimum {rule['min']}"
                )
            if "max" in rule and value > rule["max"]:
                messages.append(
                    f"Parameter {param}: {value} above maximum {rule['max']}"
                )
            
            # Warning for large values
            if "warning_if_greater_than" in rule and value > rule["warning_if_greater_than"]:
                messages.append(
                    f"⚠️ Warning: {param} is very large ({value}) - may impact performance"
                )
        
        return messages

class AdvancedWorkflowManager:
    """Manages advanced workflows with comprehensive parameter management."""
    
    def __init__(self):
        self.workflow_log = []
        self.parameter_history = []
        self.session_start_time = datetime.now()
    
    def log_workflow_step(self, step_name: str, details: Dict[str, Any]):
        """
        Log a workflow step.
        
        Args:
            step_name: Name of the step
            details: Step details and results
        """
        step_entry = {
            "timestamp": datetime.now().isoformat(),
            "step_name": step_name,
            "details": details
        }
        
        self.workflow_log.append(step_entry)
        
        # Print formatted step log
        print(f"📍 Step: {step_name}")
        print(f"   Time: {step_entry['timestamp']}")
        
        if "config_summary" in details:
            print(f"   Config: {details['config_summary']}")
        
        if "success" in details:
            status = "✅" if details["success"] else "❌"
            print(f"   Status: {status}")
        
        if "message" in details:
            print(f"   Message: {details['message']}")
        
        print()
    
    def record_parameter_change(self, param: str, old_value: Any, new_value: Any):
        """
        Record a parameter change.
        
        Args:
            param: Parameter name
            old_value: Previous value
            new_value: New value
        """
        change_entry = {
            "timestamp": datetime.now().isoformat(),
            "parameter": param,
            "old_value": old_value,
            "new_value": new_value
        }
        
        self.parameter_history.append(change_entry)
    
    def get_workflow_summary(self) -> Dict[str, Any]:
        """
        Get comprehensive workflow summary.
        
        Returns:
            Dict[str, Any]: Workflow summary
        """
        total_time = (datetime.now() - self.session_start_time).total_seconds()
        
        return {
            "session_id": f"demo_{datetime.now().strftime('%Y%m%d_%H%M%S')}",
            "total_steps": len(self.workflow_log),
            "total_parameter_changes": len(self.parameter_history),
            "total_duration_seconds": total_time,
            "steps_per_second": len(self.workflow_log) / total_time if total_time > 0 else 0,
            "parameter_changes_per_step": (
                len(self.parameter_history) / len(self.workflow_log) 
                if len(self.workflow_log) > 0 else 0
            ),
            "last_step_time": self.workflow_log[-1]["timestamp"] if self.workflow_log else None,
            "total_errors": sum(1 for step in self.workflow_log if not step.get("success", True))
        }

class ComprehensiveSimulation:
    """Comprehensive simulation with all advanced features."""
    
    def __init__(self, workflow_manager: AdvancedWorkflowManager):
        self.workflow_manager = workflow_manager
        self.world = None
        self.brain = None
        self.agent = None
    
    def initialize_advanced_simulation(self, config: Any) -> bool:
        """
        Initialize advanced simulation with comprehensive setup.
        
        Args:
            config: Configuration to use
            
        Returns:
            bool: True if initialization successful
        """
        print("🚀 Initializing Advanced Simulation")
        print("=" * 50)
        
        try:
            # Step 1: Create world
            self.workflow_manager.log_workflow_step("WORLD_CREATION", {
                "success": True,
                "message": "Creating advanced simulation world",
                "config_summary": f"Dimensions: 200x200, Vision: 32x32"
            })
            
            self.world = nlm.createSimpleWorld()
            self.world.configure(200.0, 200.0, 32, 32)
            self.world.setRandomSeed(42)
            
            # Step 2: Create brain with advanced configuration
            print("🧠 Creating advanced brain...")
            
            self.workflow_manager.log_workflow_step("BRAIN_CREATION", {
                "success": True,
                "message": "Creating neural network brain",
                "config_summary": f"Neurons: {config.get('brain.neuron_count', 'default')}, "
                                f"Regions: {config.get('brain.region_count', 'default')}, "
                                f"STDP: {config.get('plasticity.stdp.enable', False)}"
            })
            
            self.brain = nlm.createBrain(config)
            self.brain.initialize()
            
            # Step 3: Create advanced agent
            print("👤 Creating advanced agent...")
            
            self.workflow_manager.log_workflow_step("AGENT_CREATION", {
                "success": True,
                "message": "Creating intelligent agent interface",
                "config_summary": f"Exploration: enabled, "
                                f"Curiosity: enabled, "
                                f"Development: enabled"
            })
            
            self.agent = nlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            # Step 4: Configure advanced agent features
            print("⚙️  Configuring agent features...")
            
            # Enable all advanced features
            self.agent.enableRewardModulation(True)
            self.agent.enableStructuralPlasticity(True)
            self.agent.enableDevelopment(True)
            self.agent.enableCuriosity(True)
            
            # Set advanced exploration and curiosity strategies
            self.agent.setExplorationMode("epsilon_greedy")
            self.agent.setCuriosityStrategy("prediction_error")
            self.agent.enableLearningMode(True)
            
            self.workflow_manager.log_workflow_step("AGENT_FEATURE_CONFIGURATION", {
                "success": True,
                "message": "Configuring all advanced agent features",
                "config_summary": "Neuromodulation: ✓, "
                                "Structural Plasticity: ✓, "
                                "Development: ✓, "
                                "Curiosity: ✓, "
                                "Exploration: epsilon_greedy, "
                                "Curiosity Strategy: prediction_error"
            })
            
            # Step 5: Setup world environment
            print("🌍 Setting up world environment...")
            
            physics_params = {
                "gravity": 9.8,
                "friction": 0.3,
                "elasticity": 0.8,
                "damping": 0.95,
                "maxSpeed": 5.0,
                "acceleration": 2.0
            }
            self.world.setPhysicsParameters(physics_params)
            self.world.enableWeatherSimulation(True)
            self.world.setWeatherParameters(3.0, 0.1)
            
            # Add environmental objects
            for i in range(10):
                x, y = i * 20.0 + 5.0, 50.0 + i * 10.0
                obj = nlm.WorldObject(x, y, nlm.WorldObjectType.Resource, 1.0, 0.5)
                self.world.addObject(obj)
            
            self.workflow_manager.log_workflow_step("WORLD_ENVIRONMENT_SETUP", {
                "success": True,
                "message": "Setting up world physics and environment",
                "config_summary": "Physics: realistic, Weather: enabled, "
                                "Objects: 10 resources placed"
            })
            
            print("✅ Advanced simulation initialized successfully")
            return True
            
        except Exception as e:
            error_msg = f"Failed to initialize advanced simulation: {e}"
            print(f"❌ {error_msg}")
            
            self.workflow_manager.log_workflow_step("SIMULATION_INITIALIZATION_FAILURE", {
                "success": False,
                "message": error_msg,
                "error_type": type(e).__name__
            })
            
            return False
    
    def run_advanced_simulation_loop(self, num_steps: int) -> Dict[str, Any]:
        """
        Run comprehensive simulation loop.
        
        Args:
            num_steps: Number of simulation steps
            
        Returns:
            Dict[str, Any]: Simulation results
        """
        print(f"🎮 Running advanced simulation loop ({num_steps} steps)")
        print("=" * 50)
        
        simulation_results = {
            "steps_completed": 0,
            "successful_steps": 0,
            "failed_steps": 0,
            "final_metrics": {},
            "brain_metrics": {},
            "agent_metrics": {},
            "performance_metrics": {}
        }
        
        start_time = time.time()
        
        for step in range(num_steps):
            step_start = time.time()
            step_success = True
            step_errors = []
            
            try:
                # Advanced world update
                self.world.update(0.1)
                
                # Advanced sensory processing
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Advanced brain processing
                self.brain.step(step, self.world.getSimulationTime())
                
                # Advanced action selection
                action = self.agent.decodeMotorCommand()
                action_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Advanced neuromodulation
                reward = action_result.reward
                predicted = self.agent.getPredictionError()
                self.agent.applyRewardModulation(reward, predicted)
                
                # Advanced development update
                self.agent.updateDevelopment(0.1)
                
                # Collect metrics at checkpoints
                if step % 50 == 0 and step > 0:
                    self._collect_simulation_metrics(step, simulation_results)
                
                simulation_results["steps_completed"] += 1
                simulation_results["successful_steps"] += 1
                
            except Exception as e:
                step_success = False
                step_errors.append({
                    "error_type": type(e).__name__,
                    "error_message": str(e),
                    "step": step
                })
                
                simulation_results["failed_steps"] += 1
                
                print(f"⚠️  Step {step} failed: {e}")
        
        end_time = time.time()
        simulation_results["performance_metrics"]["total_duration"] = end_time - start_time
        simulation_results["performance_metrics"]["avg_step_time_ms"] = (
            (end_time - start_time) / num_steps * 1000 if num_steps > 0 else 0
        )
        simulation_results["performance_metrics"]["steps_per_second"] = (
            num_steps / (end_time - start_time) if (end_time - start_time) > 0 else 0
        )
        
        # Collect final metrics
        self._collect_simulation_metrics(num_steps, simulation_results)
        
        print(f"\n✅ Advanced simulation loop completed")
        print(f"   Total duration: {simulation_results['performance_metrics']['total_duration']:.2f}s")
        print(f"   Average step time: {simulation_results['performance_metrics']['avg_step_time_ms']:.2f}ms")
        print(f"   Steps per second: {simulation_results['performance_metrics']['steps_per_second']:.2f}")
        
        return simulation_results
    
    def _collect_simulation_metrics(self, step: int, results: Dict[str, Any]):
        """
        Collect simulation metrics at specified checkpoints.
        
        Args:
            step: Current step number
            results: Results dictionary to update
        """
        try:
            # Brain metrics
            brain_metrics = {
                "step": step,
                "neurons_firing": self.brain.getFiringNeuronCount(),
                "total_neurons": self.brain.getTotalNeuronCount(),
                "total_synapses": self.brain.getTotalSynapseCount(),
                "average_firing_rate": self.brain.getAverageFiringRate(),
                "excitation_inhibition_ratio": self.brain.getExcitationInhibitionRatio(),
                "total_spikes": self.brain.getTotalSpikeCount(),
                "developmental_stage": self.brain.getDevelopmentalStage().name
            }
            
            # Agent metrics
            agent_metrics = {
                "step": step,
                "neuromodulation_level": self.agent.getNeuromodulationLevel(),
                "curiosity_level": self.agent.getCuriosityLevel(),
                "novelty_level": self.agent.getNoveltyLevel(),
                "prediction_error": self.agent.getPredictionError(),
                "developmental_stage": self.agent.getDevelopmentalStage().name,
                "action_selection_stats": self.agent.getActionSelectionStats()
            }
            
            # World metrics
            world_metrics = {
                "step": step,
                "agent_energy": self.world.getAgentBody().energy,
                "agent_health": self.world.getAgentBody().health,
                "agent_x": self.world.getAgentBody().x,
                "agent_y": self.world.getAgentBody().y,
                "simulation_time": self.world.getSimulationTime()
            }
            
            # Store metrics
            if "brain_metrics" not in results:
                results["brain_metrics"] = []
            results["brain_metrics"].append(brain_metrics)
            
            if "agent_metrics" not in results:
                results["agent_metrics"] = []
            results["agent_metrics"].append(agent_metrics)
            
            if "world_metrics" not in results:
                results["world_metrics"] = []
            results["world_metrics"].append(world_metrics)
            
            print(f"   📊 Step {step}: "
                  f"Firing: {brain_metrics['neurons_firing']}/{brain_metrics['total_neurons']}, "
                  f"Neuromod: {agent_metrics['neuromodulation_level']:.3f}, "
                  f"Curiosity: {agent_metrics['curiosity_level']:.3f}, "
                  f"Energy: {world_metrics['agent_energy']:.1f}")
            
        except Exception as e:
            print(f"   ⚠️  Failed to collect metrics at step {step}: {e}")

class ComprehensiveDemoRunner:
    """Runs comprehensive demo with all features."""
    
    def __init__(self):
        self.workflow_manager = AdvancedWorkflowManager()
        self.simulation = ComprehensiveSimulation(self.workflow_manager)
        self.parameter_validator = ParameterValidator()
    
    def run_comprehensive_demo(self):
        """Run comprehensive demo showing all new features."""
        print("\n" + "="*80)
        print("🎯 NLM ADVANCED FEATURES COMPREHENSIVE DEMO")
        print("="*80)
        print(f"Demo started: {datetime.now()}")
        print()
        
        # Step 1: Configuration Management with Validation
        print("🔧 Step 1: Configuration Management with Validation")
        print("-" * 60)
        
        config = nlm.createDefaultConfig()
        
        # Add parameter definitions
        config.addParameterDefinition("brain.neuron_count", int, (100, 10000), True)
        config.addParameterDefinition("plasticity.stdp.learning_rate", float, (0.0001, 0.01), True)
        
        # Set advanced brain configuration
        config.set("brain.neuron_count", 5000)
        config.set("brain.region_count", 4)
        config.set("brain.connection_probability", 0.05)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.stdp.time_constant", 20.0)
        config.set("neuromod.dopamine.scale", 2.0)
        config.set("neuromod.serotonin.modulation", 0.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.prediction_error_weight", 0.8)
        config.set("development.enable", True)
        config.set("development.critical_period_start", 100.0)
        config.set("development.critical_period_end", 500.0)
        
        # Validate parameters
        print("Validating configuration parameters...")
        for param in config.getKeys():
            if param.startswith("brain.") or param.startswith("plasticity.") or \
               param.startswith("neuromod.") or param.startswith("development."):
                value = config.get(param)
                validation_messages = self.parameter_validator.validate_parameter(param, value)
                if validation_messages:
                    for msg in validation_messages:
                        print(f"   {msg}")
        
        self.workflow_manager.log_workflow_step("CONFIGURATION_SETUP", {
            "success": True,
            "message": "Advanced configuration created and validated",
            "config_summary": f"{len(config.getKeys())} parameters configured",
            "parameter_details": {
                "neuron_count": config.get("brain.neuron_count"),
                "region_count": config.get("brain.region_count"),
                "plasticity_enabled": config.get("plasticity.stdp.enable"),
                "curiosity_enabled": config.get("neuromod.curiosity.enable"),
                "development_enabled": config.get("development.enable")
            }
        })
        
        # Step 2: Advanced Brain Configuration
        print("\n🧠 Step 2: Advanced Brain Configuration")
        print("-" * 60)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        self.workflow_manager.log_workflow_step("BRAIN_INITIALIZATION", {
            "success": True,
            "message": "Advanced brain created with comprehensive configuration",
            "config_summary": f"Network size: {brain.getTotalNeuronCount()} neurons, "
                            f"{brain.getTotalSynapseCount()} synapses",
            "brain_metrics": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "regions": brain.getRegionCount()
            }
        })
        
        # Step 3: Advanced Agent with All Features
        print("\n👤 Step 3: Advanced Agent with All Features")
        print("-" * 60)
        
        agent = nlm.createAgentBrain(brain)
        
        # Initialize with world
        world = nlm.createSimpleWorld()
        world.configure(200.0, 200.0, 32, 32)
        world.reset()
        
        agent.initialize(world)
        
        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Set advanced exploration and curiosity strategies
        agent.setExplorationMode("epsilon_greedy")
        agent.setCuriosityStrategy("prediction_error")
        agent.enableLearningMode(True)
        
        self.workflow_manager.log_workflow_step("ADVANCED_AGENT_SETUP", {
            "success": True,
            "message": "Advanced agent initialized with all features enabled",
            "config_summary": "Neuromodulation: enabled, "
                            "Structural Plasticity: enabled, "
                            "Development: enabled, "
                            "Curiosity: enabled, "
                            "Exploration: epsilon_greedy, "
                            "Curiosity Strategy: prediction_error, "
                            "Learning Mode: enabled"
        })
        
        # Replace simulation's components with our initialized ones
        self.simulation.world = world
        self.simulation.brain = brain
        self.simulation.agent = agent
        
        # Step 4: Advanced Simulation with All Features
        print("\n🎮 Step 4: Advanced Simulation with All Features")
        print("-" * 60)
        
        simulation_results = self.simulation.run_advanced_simulation_loop(200)
        
        self.workflow_manager.log_workflow_step("ADVANCED_SIMULATION", {
            "success": True,
            "message": f"Advanced simulation completed successfully",
            "config_summary": f"Duration: {simulation_results['performance_metrics']['total_duration']:.2f}s, "
                            f"Steps: {simulation_results['steps_completed']}, "
                            f"Success Rate: {simulation_results['successful_steps']/max(1, simulation_results['steps_completed'])*100:.1f}%",
            "performance_metrics": simulation_results["performance_metrics"],
            "final_agent_metrics": simulation_results["agent_metrics"][-1] if simulation_results["agent_metrics"] else None,
            "final_brain_metrics": simulation_results["brain_metrics"][-1] if simulation_results["brain_metrics"] else None
        })
        
        # Step 5: Final Analysis and Reporting
        print("\n📊 Step 5: Final Analysis and Reporting")
        print("-" * 60)
        
        # Get workflow summary
        workflow_summary = self.workflow_manager.get_workflow_summary()
        
        print(f"Workflow Analysis:")
        print(f"  Total steps: {workflow_summary['total_steps']}")
        print(f"  Total parameter changes: {workflow_summary['total_parameter_changes']}")
        print(f"  Total duration: {workflow_summary['total_duration_seconds']:.2f}s")
        print(f"  Steps per second: {workflow_summary['steps_per_second']:.2f}")
        print(f"  Total errors: {workflow_summary['total_errors']}")
        
        print(f"\nSimulation Analysis:")
        perf = simulation_results["performance_metrics"]
        print(f"  Total simulation time: {perf['total_duration']:.2f}s")
        print(f"  Average step time: {perf['avg_step_time_ms']:.2f}ms")
        print(f"  Steps per second: {perf['steps_per_second']:.2f}")
        print(f"  Success rate: {simulation_results['successful_steps']/max(1, simulation_results['steps_completed'])*100:.1f}%")
        
        if simulation_results["agent_metrics"]:
            final_agent = simulation_results["agent_metrics"][-1]
            print(f"\nFinal Agent State:")
            print(f"  Neuromodulation level: {final_agent['neuromodulation_level']:.3f}")
            print(f"  Curiosity level: {final_agent['curiosity_level']:.3f}")
            print(f"  Novelty level: {final_agent['novelty_level']:.3f}")
            print(f"  Prediction error: {final_agent['prediction_error']:.3f}")
            print(f"  Developmental stage: {final_agent['developmental_stage']}")
        
        if simulation_results["brain_metrics"]:
            final_brain = simulation_results["brain_metrics"][-1]
            print(f"\nFinal Brain State:")
            print(f"  Neurons firing: {final_brain['neurons_firing']}/{final_brain['total_neurons']}")
            print(f"  Average firing rate: {final_brain['average_firing_rate']:.4f} Hz")
            print(f"  Excitation/inhibition ratio: {final_brain['excitation_inhibition_ratio']:.3f}")
            print(f"  Total spikes: {final_brain['total_spikes']:,}")
        
        print(f"\n" + "="*80)
        print("🎉 COMPREHENSIVE DEMO COMPLETE")
        print("="*80)
        print(f"Demo completed at: {datetime.now()}")
        print(f"Session ID: {workflow_summary['session_id']}")
        print(f"Total workflow steps: {workflow_summary['total_steps']}")
        print(f"Total simulation steps: {simulation_results['steps_completed']}")
        print(f"Success rate: {simulation_results['successful_steps']/max(1, simulation_results['steps_completed'])*100:.1f}%")

if __name__ == "__main__":
    # Run comprehensive demo
    demo = ComprehensiveDemoRunner()
    demo.run_comprehensive_demo()
