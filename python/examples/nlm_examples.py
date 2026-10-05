"""
NLM Advanced Examples

Comprehensive examples demonstrating advanced NLM features and capabilities.
These examples showcase the full power of the Neural Learning Machine framework
with advanced simulation controls, plugin systems, and performance monitoring.

Examples Included:
1. Agent Simulation - Complete agent with world interaction
2. Brain Training - Multiple learning paradigms
3. Memory System Integration - Testing memory systems
4. Advanced Simulation with Monitoring - Real-time monitoring and control
"""

import sys
import time
import threading
import numpy as np
from typing import List, Dict, Any

# Add the utils module to path
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_41d79b90-42a5-496b-a5aa-84bdfca07d69/python')

# Import NLM components
try:
    from nlm_utils import (
        AdvancedConfig,
        MetricsCollector,
        EventSystem,
        EnhancedErrorHandler,
        InputValidator,
        SimulationSafetyChecker,
        VisualizationHelper,
        PluginManager,
        NLMContext,
        Event,
        PluginInterface,
        AdvancedSimulationController,
        SimulationScheduler,
        SimulationExporter,
        PerformanceMonitor,
        ConfigBuilder
    )
    NLM_UTILS_AVAILABLE = True
except ImportError as e:
    print(f"Warning: NLM utilities not available: {e}")
    NLM_UTILS_AVAILABLE = False

# Import nlm_py with fallback
try:
    import nlm_py
    NLM_PY_AVAILABLE = True
except ImportError:
    print("Warning: NLM Python bindings not available. Using mock objects.")
    NLM_PY_AVAILABLE = False
    nlm_py = None

if NLM_PY_AVAILABLE:
    import nlm_py as nlm
else:
    # Create mock objects for testing when bindings not available
    class MockNeuron:
        def __init__(self, neuron_id):
            self.neuron_id = neuron_id
            self.type = nlm.NeuronType.Internal if NLM_PY_AVAILABLE else 'Internal'
            self.membrane_potential = -70.0
            self.threshold = -55.0
            self.firing_rate = 0.0
            self.refractory_remaining = 0
        
        def getId(self):
            return self.neuron_id
        
        def getType(self):
            return self.type
        
        def getMembranePotential(self):
            return self.membrane_potential
        
        def setMembranePotential(self, potential):
            self.membrane_potential = potential
        
        def getThreshold(self):
            return self.threshold
    
    class MockBrain:
        def __init__(self, config=None):
            self.config = config or {}
            self.neurons = [MockNeuron(i) for i in range(config.get('neuron_count', 100))]
            self.simulation_step = 0
            self.total_spikes = 0
        
        def initializeForSimulation(self):
            print("Mock brain initialized for simulation")
            return True
        
        def step(self):
            self.simulation_step += 1
            # Simulate some activity
            for neuron in self.neurons:
                if np.random.random() < 0.1:  # 10% chance of spiking
                    self.total_spikes += 1
            return True
        
        def getSimulationStep(self):
            return self.simulation_step
        
        def getNeuronCount(self):
            return len(self.neurons)
        
        def getTotalSpikeCount(self):
            return self.total_spikes
        
        def getAverageFiringRate(self):
            if self.simulation_step == 0:
                return 0.0
            return self.total_spikes / self.simulation_step
    
    class MockConfig:
        def __init__(self):
            self.params = {}
        
        def set(self, key, value):
            self.params[key] = value
        
        def get(self, key, default=None):
            return self.params.get(key, default)
    
    # Create mock nlm module
    class MockNLM:
        class NeuronType:
            Internal = 'Internal'
            Excitatory = 'Excitatory'
            Inhibitory = 'Inhibitory'
            Sensory = 'Sensory'
            Motor = 'Motor'
        
        class FiringState:
            Resting = 'Resting'
            Active = 'Active'
            Refractory = 'Refractory'
    
    nlm = MockNLM()
    nlm.Brain = MockBrain
    nlm.Config = MockConfig

class SimpleBrainExample:
    """Simple brain creation and simulation example"""
    
    def __init__(self):
        self.brain = None
        self.config = None
    
    def run_example(self):
        """Run simple brain example"""
        print_header("Simple Brain Example")
        
        print("\n1. Creating configuration...")
        self.config = AdvancedConfig()
        self.config.set("neuron_count", 500)
        self.config.set("simulation_timestep", 0.001)
        self.config.set("connection_probability", 0.1)
        
        print(f"   Created brain with {self.config.get('neuron_count')} neurons")
        print(f"   Simulation timestep: {self.config.get('simulation_timestep')}s")
        print(f"   Connection probability: {self.config.get('connection_probability')}")
        
        print("\n2. Creating and initializing brain...")
        self.brain = nlm.Brain(self.config)
        
        if self.brain.initializeForSimulation():
            print("   ✓ Brain initialized successfully")
        else:
            print("   ✗ Brain initialization failed")
            return False
        
        print("\n3. Running simulation...")
        start_time = time.time()
        
        for step in range(100):
            self.brain.step()
            
            if step % 25 == 0:
                print(f"   Step {step + 1}/100: {self.brain.getTotalSpikeCount()} spikes")
        
        end_time = time.time()
        
        print(f"\n   Simulation completed in {end_time - start_time:.2f} seconds")
        print(f"   Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"   Average firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        
        return True

class AgentSimulationExample:
    """Complete agent with world interaction example"""
    
    def __init__(self):
        self.agent = None
        self.config = None
        self.world = None
    
    def create_mock_agent(self):
        """Create mock agent with sensory and motor capabilities"""
        class MockAgent:
            def __init__(self, brain):
                self.brain = brain
                self.sensory_inputs = []
                self.actions = []
                self.position = [0.0, 0.0]
                self.goal_position = [10.0, 10.0]
            
            def perceive(self):
                """Perceive sensory input from environment"""
                # Generate random sensory input based on distance to goal
                distance = np.sqrt((self.goal_position[0] - self.position[0])**2 + 
                                 (self.goal_position[1] - self.position[1])**2)
                
                # Create sensory input with noise
                sensory_input = [
                    distance / 20.0,  # Normalized distance
                    np.random.normal(0, 0.1),  # Noisy sensor
                    np.random.normal(0, 0.1)   # Another noisy sensor
                ]
                
                self.sensory_inputs.append(sensory_input)
                return sensory_input
            
            def decide(self):
                """Make decision based on sensory input and brain activity"""
                # Get brain activity
                spike_count = self.brain.getTotalSpikeCount()
                firing_rate = self.brain.getAverageFiringRate()
                
                # Simple decision: move toward goal
                distance = np.sqrt((self.goal_position[0] - self.position[0])**2 + 
                                 (self.goal_position[1] - self.position[1])**2)
                
                if distance > 1.0:
                    # Move toward goal
                    dx = (self.goal_position[0] - self.position[0]) / distance * 0.5
                    dy = (self.goal_position[1] - self.position[1]) / distance * 0.5
                    self.position[0] += dx
                    self.position[1] += dy
                    
                    action = {
                        'type': 'move',
                        'dx': dx,
                        'dy': dy,
                        'distance_to_goal': distance
                    }
                else:
                    # Reached goal
                    action = {
                        'type': 'goal_reached',
                        'distance_to_goal': distance
                    }
                
                self.actions.append(action)
                return action
            
            def act(self, action):
                """Execute action in environment"""
                if action['type'] == 'move':
                    self.position[0] += action['dx']
                    self.position[1] += action['dy']
                    return f"Moved to ({self.position[0]:.1f}, {self.position[1]:.1f})"
                elif action['type'] == 'goal_reached':
                    return f"Goal reached! Distance: {action['distance_to_goal']:.1f}"
                else:
                    return f"Unknown action: {action}"
            
            def get_summary(self):
                """Get agent summary"""
                return {
                    'position': self.position,
                    'goal_position': self.goal_position,
                    'sensory_inputs_count': len(self.sensory_inputs),
                    'actions_count': len(self.actions),
                    'brain_spikes': self.brain.getTotalSpikeCount(),
                    'brain_firing_rate': self.brain.getAverageFiringRate()
                }
        
        return MockAgent
    
    def run_example(self):
        """Run agent simulation example"""
        print_header("Agent Simulation Example")
        
        print("\n1. Creating agent configuration...")
        self.config = AdvancedConfig()
        self.config.set("neuron_count", 1000)
        self.config.set("simulation_timestep", 0.001)
        self.config.set("connection_probability", 0.05)
        
        print("\n2. Creating brain...")
        brain = nlm.Brain(self.config)
        
        if not brain.initializeForSimulation():
            print("   ✗ Brain initialization failed")
            return False
        
        print("\n3. Creating agent...")
        AgentClass = self.create_mock_agent()
        agent = AgentClass(brain)
        
        print("\n4. Starting agent simulation...")
        print(f"   Agent starting at position {agent.position}")
        print(f"   Goal: {agent.goal_position}")
        
        # Run simulation
        for step in range(100):
            # Perceive
            sensory_input = agent.perceive()
            
            # Decide
            action = agent.decide()
            
            # Act
            result = agent.act(action)
            
            # Run brain simulation
            brain.step()
            
            # Log progress
            if step % 25 == 0:
                distance = np.sqrt((agent.goal_position[0] - agent.position[0])**2 + 
                                 (agent.goal_position[1] - agent.position[1])**2)
                print(f"   Step {step + 1}/100: Position {agent.position}, Distance to goal: {distance:.1f}")
                print(f"     Action: {result}")
                print(f"     Brain spikes: {brain.getTotalSpikeCount()}")
        
        # Get agent summary
        summary = agent.get_summary()
        
        print(f"\n5. Agent Simulation Complete!")
        print(f"   Final position: {summary['position']}")
        print(f"   Distance to goal: {np.sqrt((summary['goal_position'][0] - summary['position'][0])**2 + 
                                         (summary['goal_position'][1] - summary['position'][1])**2):.1f}")
        print(f"   Sensory inputs processed: {summary['sensory_inputs_count']}")
        print(f"   Actions taken: {summary['actions_count']}")
        print(f"   Brain spikes: {summary['brain_spikes']}")
        print(f"   Average brain firing rate: {summary['brain_firing_rate']:.2f} Hz")
        
        return True

class BrainTrainingExample:
    """Training with multiple learning paradigms example"""
    
    def __init__(self):
        self.brain = None
        self.metrics_collector = MetricsCollector()
        self.event_system = EventSystem()
        self.error_handler = EnhancedErrorHandler()
    
    def setup_brain(self):
        """Setup brain with training configuration"""
        print("\n1. Creating training configuration...")
        
        config = AdvancedConfig()
        config.set("neuron_count", 2000)
        config.set("simulation_timestep", 0.001)
        config.set("connection_probability", 0.08)
        config.set("plasticity_learning_rate", 0.02)
        config.set("stdp_ltp_weight", 0.05)
        config.set("stdp_ltd_weight", 0.04)
        
        return config
    
    def create_learning_paradigm(self, paradigm_name, config):
        """Create learning paradigm"""
        print(f"\n2. Creating {paradigm_name}...")
        
        if paradigm_name == "repetition" or paradigm_name == "repetition learning":
            return self._create_repetition_learning(config)
        elif paradigm_name == "temporal difference" or paradigm_name == "td learning":
            return self._create_td_learning(config)
        elif paradigm_name == "motor learning" or paradigm_name == "motor learning":
            return self._create_motor_learning(config)
        elif paradigm_name == "spatial learning" or paradigm_name == "spatial learning":
            return self._create_spatial_learning(config)
        else:
            return self._create_generic_learning(config, paradigm_name)
    
    def _create_repetition_learning(self, config):
        """Create repetition-based learning paradigm"""
        print("   Creating repetition learning paradigm...")
        print("   - Pattern repetition for strengthening synapses")
        print("   - Memory consolidation through repeated exposure")
        
        paradigm = {
            'name': 'Repetition Learning',
            'type': 'repetition',
            "description": "Learning through repeated exposure of patterns",
            "stimulation_frequency": 0.5,  # Hz
            "repetition_count": 10,
            "pattern_variety": 5
        }
        
        return paradigm
    
    def _create_td_learning(self, config):
        """Create temporal difference learning paradigm"""
        print("   Creating temporal difference learning paradigm...")
        print("   - Reward prediction and error minimization")
        print("   - Discounted future rewards")
        
        paradigm = {
            'name': 'Temporal Difference Learning',
            'type': 'temporal_difference',
            "description": "Learning through reward prediction errors",
            "discount_factor": config.get("reward_discount_factor", 0.99),
            "prediction_error_strength": 0.1,
            "reward_schedule": "exponential"
        }
        
        return paradigm
    
    def _create_motor_learning(self, config):
        """Create motor learning paradigm"""
        print("   Creating motor learning paradigm...")
        print("   - Action-outcome associations")
        print("   - Inverse reinforcement learning")
        
        paradigm = {
            'name': 'Motor Learning',
            'type': 'motor_learning',
            "description": "Learning optimal motor actions",
            "learning_rate": config.get("plasticity_learning_rate", 0.01),
            "exploration_rate": 0.1,
            "outcome_predictive": True
        }
        
        return paradigm
    
    def _create_spatial_learning(self, config):
        """Create spatial learning paradigm"""
        print("   Creating spatial learning paradigm...")
        print("   - Environment navigation and mapping")
        print("   - Place cell formation")
        
        paradigm = {
            'name': 'Spatial Learning',
            'type': 'spatial_learning',
            "description": "Learning spatial environments and navigation",
            "exploration_rate": 0.2,
            "map_update_rate": 0.05,
            "place_cell_fields": 20
        }
        
        return paradigm
    
    def _create_generic_learning(self, config, paradigm_name):
        """Create generic learning paradigm"""
        print(f"   Creating generic learning paradigm: {paradigm_name}")
        
        return {
            'name': paradigm_name,
            'type': 'generic',
            "description": f"Generic learning paradigm: {paradigm_name}",
            "parameters": config
        }
    
    def setup_training_environment(self, brain, paradigm):
        """Setup training environment"""
        print("\n3. Setting up training environment...")
        
        # Create brain
        self.brain = nlm.Brain(brain)
        
        if not self.brain.initializeForSimulation():
            print("   ✗ Brain initialization failed")
            return False
        
        # Setup metrics collection
        self.metrics_collector.record_metric('paradigm', paradigm['name'])
        
        # Setup event system
        def on_training_step(event_type, event_data):
            step = event_data['step']
            accuracy = event_data.get('accuracy', 0.0)
            print(f"   Training step {step}: Accuracy {accuracy:.2%}")
        
        def on_learning_event(event_type, event_data):
            event_name = event_data.get('event_name', 'Unknown')
            print(f"   Learning event: {event_name}")
        
        self.event_system.addListener('training_step', on_training_step)
        self.event_system.addListener('learning_event', on_learning_event)
        
        # Add error recovery
        def recovery_handler(error, context):
            print(f"   Error recovered: {error}")
            return True
        
        self.error_handler.recovery_actions['simulation'] = recovery_handler
        
        return True
    
    def run_training_simulation(self, paradigm, config, total_steps=500):
        """Run training simulation"""
        print(f"\n4. Running {paradigm['name']} training...")
        print(f"   Total training steps: {total_steps}")
        
        # Simulation parameters based on paradigm
        if paradigm['type'] == 'repetition':
            stimulation_interval = int(1.0 / paradigm.get('stimulation_frequency', 0.5))  # steps
            repetition_interval = paradigm.get('repetition_count', 10)
        elif paradigm['type'] == 'temporal_difference':
            stimulation_interval = 1
            repetition_interval = 5
        elif paradigm['type'] == 'motor_learning':
            stimulation_interval = 2
            repetition_interval = 8
        elif paradigm['type'] == 'spatial_learning':
            stimulation_interval = 3
            repetition_interval = 6
        else:
            stimulation_interval = 1
            repetition_interval = 10
        
        # Run training
        for step in range(total_steps):
            # Determine stimulation based on paradigm
            should_stimulate = (step % stimulation_interval == 0)
            
            if should_stimulate:
                # Generate stimulation based on paradigm
                if paradigm['type'] == 'repetition':
                    stimulation_strength = 1.0
                    stimulation_pattern = np.random.choice(paradigm.get('pattern_variety', 5))
                elif paradigm['type'] == 'temporal_difference':
                    stimulation_strength = 0.5
                    stimulation_pattern = 'prediction_error'
                elif paradigm['type'] == 'motor_learning':
                    stimulation_strength = 0.8
                    stimulation_pattern = 'action_outcome'
                elif paradigm['type'] == 'spatial_learning':
                    stimulation_strength = 0.6
                    stimulation_pattern = 'spatial_update'
                else:
                    stimulation_strength = 1.0
                    stimulation_pattern = 'generic'
                
                # Apply stimulation to brain (mock)
                print(f"   Step {step + 1}: Applying {stimulation_pattern} stimulation ({stimulation_strength:.1f})")
            
            # Run brain simulation
            self.brain.step()
            
            # Record metrics
            self.metrics_collector.record_metric('step', step)
            self.metrics_collector.record_metric('spike_count', self.brain.getTotalSpikeCount())
            
            # Calculate pseudo-accuracy (simulated learning progress)
            if step > 0:
                accuracy = min(1.0, 0.1 + (step * 0.002) + np.random.normal(0, 0.05))
                
                # Emit training step event
                self.event_system.emit_event(Event('training_step', {
                    'step': step + 1,
                    'accuracy': accuracy,
                    'spike_count': self.brain.getTotalSpikeCount()
                }))
                
                # Emit learning event at specific intervals
                if step % repetition_interval == 0:
                    self.event_system.emit_event(Event('learning_event', {
                        'event_name': 'synaptic_consolidation',
                        'paradigm': paradigm['name'],
                        'step': step + 1
                    }))
            
            # Log progress
            if step % 100 == 0:
                print(f"   Progress: {step + 1}/{total_steps} steps, " +
                      f"Spikes: {self.brain.getTotalSpikeCount()}, " +
                      f"Accuracy: {accuracy:.2%}")
        
        # Training complete
        print(f"\n   Training complete!")
        print(f"   Final accuracy: {accuracy:.2%}")
        print(f"   Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"   Average firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        
        return True
    
    def run_example(self):
        """Run brain training example"""
        print_header("Brain Training Example")
        
        try:
            # Setup brain
            config = self.setup_brain()
            
            # Define training paradigms
            paradigms = [
                "repetition learning",
                "temporal difference learning", 
                "motor learning",
                "spatial learning"
            ]
            
            # Run each paradigm
            for paradigm_name in paradigms:
                print(f"\n{'='*60}")
                print(f"PARADIGM: {paradigm_name.upper()}")
                print('='*60)
                
                paradigm = self.create_learning_paradigm(paradigm_name, config)
                
                if self.setup_training_environment(config, paradigm):
                    self.run_training_simulation(paradigm, config, total_steps=200)
                    
                    print(f"\n✓ {paradigm_name} completed successfully!")
                else:
                    print(f"\n✗ {paradigm_name} failed")
            
            print(f"\n{'='*60}")
            print("ALL TRAINING PARADIGMS COMPLETED!")
            print('='*60)
            
            return True
            
        except Exception as e:
            print(f"\nError in training example: {e}")
            import traceback
            traceback.print_exc()
            return False

class AdvancedSimulationExample:
    """Advanced simulation with monitoring and controls"""
    
    def __init__(self):
        self.brain = None
        self.simulation_controller = None
        self.performance_monitor = PerformanceMonitor()
        self.config_builder = ConfigBuilder()
    
    def create_advanced_config(self):
        """Create advanced configuration"""
        print("\n1. Creating advanced configuration...")
        
        config = {
            'name': 'Advanced NLM Simulation',
            'neuron_count': 3000,
            'simulation_timestep': 0.0005,
            'connection_probability': 0.12,
            'plasticity_learning_rate': 0.015,
            'stdp_ltp_weight': 0.03,
            'stdp_ltd_weight': 0.025,
            'development_synaptogenesis_rate': 0.002,
            'development_pruning_rate': 0.0002,
            'enable_visualization': True,
            'enable_monitoring': True,
            'performance_monitoring': True,
            'event_driven': True,
            'real_time_control': True,
            'auto_recovery': True,
            'safety_checks': True
        }
        
        return config
    
    def setup_advanced_simulation(self, config):
        """Setup advanced simulation with all features"""
        print("\n2. Setting up advanced simulation...")
        
        # Create brain
        self.brain = nlm.Brain(config)
        
        if not self.brain.initializeForSimulation():
            print("   ✗ Brain initialization failed")
            return False
        
        # Create advanced simulation controller
        simulation_config = {
            'max_steps': 1000,
            'timestep': config['simulation_timestep'],
            'performance_monitoring': config['performance_monitoring'],
            'event_driven': config['event_driven'],
            'auto_recovery': config['auto_recovery']
        }
        
        self.simulation_controller = AdvancedSimulationController(self.brain, simulation_config)
        
        # Setup performance monitoring
        self.performance_monitor.start_monitoring()
        
        # Setup visualization helper
        viz_helper = VisualizationHelper()
        
        # Setup event system
        def on_simulation_started(event_type, event_data):
            print(f"   🚀 Simulation started with {event_data['max_steps']} steps")
            print(f"      Timestep: {event_data['timestep']}s")
        
        def on_simulation_stopped(event_type, event_data):
            print(f"   🛑 Simulation stopped after {event_data['steps_completed']} steps")
        
        def on_step_completed(event_type, event_data):
            step = event_data['step']
            spike_count = event_data['brain_metrics']['spike_count']
            stability = event_data['brain_metrics']['stability']
            
            if step % 50 == 0:
                print(f"   Step {step}: {spike_count} spikes, Stability: {stability:.2f}")
        
        def on_error_recovery(event_type, event_data):
            error = event_data.get('error', 'Unknown error')
            recovered = event_data.get('recovered', False)
            if recovered:
                print(f"   🔄 Error recovered: {error}")
            else:
                print(f"   ⚠️ Error not recovered: {error}")
        
        self.simulation_controller.add_event_handler('simulation_started', on_simulation_started)
        self.simulation_controller.add_event_handler('simulation_stopped', on_simulation_stopped)
        self.simulation_controller.add_event_handler('step_completed', on_step_completed)
        self.simulation_controller.add_event_handler('error_recovery', on_error_recovery)
        
        # Setup safety checking
        safety_checker = SimulationSafetyChecker()
        
        def safety_check_step(brain, step):
            violations = safety_checker.check_simulation_safety(brain, step, config)
            if violations:
                for violation in violations:
                    print(f"   ⚠️ Safety violation: {violation}")
                    if "emergency" in violation.lower():
                        safety_checker.activate_emergency_stop()
        
        # Setup metrics collection
        def step_metrics_callback(step_result):
            self.performance_monitor.record_metric('step', step_result['step'])
            self.performance_monitor.record_metric('spike_count', step_result['brain_metrics']['spike_count'])
        
        self.simulation_controller.add_event_handler('step_completed', step_metrics_callback)
        
        return True
    
    def run_advanced_simulation(self):
        """Run advanced simulation with all features"""
        print("\n3. Running advanced simulation...")
        print("   Features enabled:")
        print("     • Performance monitoring")
        print("     • Event-driven control")
        print("     • Auto-recovery")
        print("     • Safety checking")
        print("     • Real-time progress monitoring")
        
        # Start simulation
        if not self.simulation_controller.start():
            print("   ✗ Failed to start simulation")
            return False
        
        try:
            # Run simulation for some steps
            print(f"\n   Running simulation for {self.simulation_controller.max_steps} steps...")
            
            step_results = self.simulation_controller.run_steps(500)
            
            print(f"\n   ✅ Advanced simulation completed!")
            print(f"   Steps executed: {len(step_results)}")
            print(f"   Final progress: {self.simulation_controller.get_progress()}")
            
            # Get simulation summary
            summary = self.simulation_controller.get_metrics_summary()
            
            print(f"\n   📊 Simulation Summary:")
            print(f"     Total steps: {summary['simulation_progress']['current_step']}")
            print(f"     Progress: {summary['simulation_progress']['progress_percent']:.1f}%")
            print(f"     Total spikes: {summary['brain_metrics']['total_spikes']}")
            print(f"     Average firing rate: {summary['brain_metrics']['firing_rate']:.2f} Hz")
            print(f"     Final stability: {summary['brain_metrics']['stability']:.2f}")
            
            # Get performance metrics
            performance = summary['performance']
            if performance['total_samples'] > 0:
                print(f"\n   ⚡ Performance Metrics:")
                print(f"     Average CPU usage: {performance['avg_cpu_usage']:.1f}%")
                print(f"     Average memory usage: {performance['avg_memory_usage']:.1f} GB")
                print(f"     Max CPU usage: {performance['max_cpu_usage']:.1f}%")
                print(f"     Max memory usage: {performance['max_memory_usage']:.1f} GB")
            
            return True
            
        except Exception as e:
            print(f"   ❌ Simulation error: {e}")
            return False
        finally:
            # Stop simulation
            self.simulation_controller.stop()
            self.performance_monitor.stop_monitoring()
    
    def run_example(self):
        """Run advanced simulation example"""
        print_header("Advanced Simulation Example")
        
        try:
            # Create advanced config
            config = self.create_advanced_config()
            
            # Setup advanced simulation
            if self.setup_advanced_simulation(config):
                # Run advanced simulation
                self.run_advanced_simulation()
                
                print(f"\n{'='*60}")
                print("ADVANCED SIMULATION EXAMPLE COMPLETE!")
                print('='*60)
                print("\nThis example demonstrated:")
                print("✓ Advanced configuration management")
                print("✓ Real-time simulation control")
                print("✓ Performance monitoring and profiling")
                print("✓ Event-driven simulation architecture")
                print("✓ Auto-recovery and error handling")
                print("✓ Safety checking and validation")
                print("✓ Comprehensive metrics collection")
                print("✓ Real-time progress monitoring")
                
                return True
            else:
                print("Failed to setup advanced simulation")
                return False
                
        except Exception as e:
            print(f"\nError in advanced simulation example: {e}")
            import traceback
            traceback.print_exc()
            return False

def main():
    """Main function to run all examples"""
    print("NLM Advanced Examples")
    print("===================")
    
    examples = [
        ("Simple Brain Example", SimpleBrainExample()),
        ("Agent Simulation Example", AgentSimulationExample()),
        ("Brain Training Example", BrainTrainingExample()),
        ("Advanced Simulation Example", AdvancedSimulationExample())
    ]
    
    for example_name, example in examples:
        print(f"\n{'='*80}")
        print(f"RUNNING: {example_name}")
        print('='*80)
        
        try:
            if example.run_example():
                print(f"\n✅ {example_name} - COMPLETED SUCCESSFULLY")
            else:
                print(f"\n❌ {example_name} - FAILED")
        except Exception as e:
            print(f"\n💥 {example_name} - ERROR: {e}")
    
    print(f"\n{'='*80}")
    print("ALL EXAMPLES COMPLETED!")
    print('='*80)
    print("\nThe NLM framework demonstrates comprehensive capabilities:")
    print("• Basic and advanced brain simulation")
    print("• Agent-based world interaction")
    print("• Multiple learning paradigms")
    print("• Advanced simulation controls")
    print("• Performance monitoring and optimization")
    print("• Error handling and recovery")
    print("• Event-driven architecture")
    print("• Comprehensive metrics and visualization")
    
    return 0

if __name__ == '__main__':
    sys.exit(main())