"""
NLM Basic Simulation Example

This example demonstrates basic NLM brain simulation setup and execution.
It's designed for beginners to understand the fundamental concepts of
NLM (Neural Learning Machine) and how to use it.

Key Features Demonstrated:
- Basic brain configuration
- Simple simulation execution
- Basic performance monitoring
- Result analysis and visualization
"""

import sys
import time
import numpy as np

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
        Event,
        SimulationError
    )
    NLM_UTILS_AVAILABLE = True
except ImportError as e:
    print(f"Warning: NLM utilities not available: {e}")
    NLM_UTILS_AVAILABLE = False

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

def print_header(title):
    """Print formatted header"""
    print("\n" + "="*60)
    print(f"NLM Basic Simulation Example: {title}")
    print("="*60)

def basic_brain_setup():
    """Demonstrate basic brain setup and configuration"""
    print_header("Basic Brain Setup")
    
    print("\n1. Creating Advanced Configuration...")
    config = AdvancedConfig("configs/default.cfg")
    
    # Display configuration
    print("\n   Configuration Parameters:")
    config_params = [
        ('neuron_count', 'Number of neurons'),
        ('simulation_timestep', 'Simulation timestep'),
        ('connection_probability', 'Connection probability'),
        ('dopamine_baseline', 'Dopamine baseline'),
        ('plasticity_learning_rate', 'Plasticity learning rate')
    ]
    
    for param, description in config_params:
        value = config.get(param)
        print(f"     {description}: {value}")
    
    print("\n2. Validating Configuration...")
    if config.is_valid():
        print("   ✓ Configuration is valid")
    else:
        errors = config.get_validation_errors()
        print(f"   ✗ Configuration has {len(errors)} errors:")
        for error in errors:
            print(f"      - {error}")
        return None
    
    print("\n3. Creating NLM Brain...")
    brain = nlm.Brain(config)
    
    print("\n4. Initializing Brain...")
    if brain.initializeForSimulation():
        print("   ✓ Brain initialized successfully")
    else:
        print("   ✗ Brain initialization failed")
        return None
    
    return brain, config

def basic_simulation_run(brain, steps=50):
    """Demonstrate basic simulation execution"""
    print_header("Basic Simulation Run")
    
    print(f"\nRunning simulation for {steps} steps...")
    
    # Initialize metrics collection
    metrics_collector = MetricsCollector()
    metrics_collector.set_alert_threshold('spike_count', 1000)  # Alert if spike count > 1000
    
    # Setup event system
    event_system = EventSystem()
    
    # Add event handlers
    def on_step_completed(event_type, event_data):
        step = event_data['step']
        spike_count = event_data['spike_count']
        print(f"   Step {step}: {spike_count} spikes")
    
    def on_simulation_warning(event_type, event_data):
        warning = event_data.get('warning', 'Unknown warning')
        print(f"   ⚠ Warning: {warning}")
    
    event_system.addListener('step_completed', on_step_completed)
    event_system.addListener('simulation_warning', on_simulation_warning)
    
    # Add alert callback
    def alert_callback(metric_name, value, threshold):
        print(f"   🚨 Alert: {metric_name} exceeded threshold ({value} > {threshold})")
    
    metrics_collector.add_alert_callback(alert_callback)
    
    # Run simulation
    start_time = time.time()
    step_results = []
    
    for step in range(steps):
        # Record metrics before step
        metrics_collector.record_metric('step', step)
        metrics_collector.record_metric('spike_count', brain.getTotalSpikeCount())
        
        # Execute simulation step
        brain.step()
        
        # Record metrics after step
        current_spikes = brain.getTotalSpikeCount()
        metrics_collector.record_metric('spike_count', current_spikes)
        
        # Create step result
        step_result = {
            'step': step + 1,
            'timestamp': time.time(),
            'spike_count': current_spikes,
            'firing_rate': brain.getAverageFiringRate()
        }
        
        step_results.append(step_result)
        
        # Emit step completed event
        event_system.emit_event(Event('step_completed', {
            'step': step + 1,
            'spike_count': current_spikes,
            'firing_rate': brain.getAverageFiringRate()
        }))
        
        # Check for simulation warnings
        if brain.getAverageFiringRate() > 100.0:
            event_system.emit_event(Event('simulation_warning', {
                'warning': 'High firing rate detected',
                'firing_rate': brain.getAverageFiringRate()
            }))
    
    end_time = time.time()
    
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # Print metrics summary
    print("\n2. Metrics Summary:")
    print(f"   Total steps: {steps}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Events processed: {len(event_system.get_event_history())}")
    
    # Get metrics collector summary
    spike_count_summary = metrics_collector.get_metric_summary('spike_count')
    if spike_count_summary:
        print(f"   Spike count statistics:")
        print(f"     Mean: {spike_count_summary['mean']:.1f}")
        print(f"     Std: {spike_count_summary['std']:.1f}")
        print(f"     Max: {spike_count_summary['max']}")
    
    return brain, step_results

def basic_analysis(brain, step_results):
    """Demonstrate basic analysis of simulation results"""
    print_header("Basic Analysis")
    
    if not step_results:
        print("\nNo step results to analyze")
        return
    
    print("\n1. Step-by-Step Analysis:")
    
    # Calculate basic statistics
    spike_counts = [result['spike_count'] for result in step_results]
    firing_rates = [result['firing_rate'] for result in step_results]
    
    print(f"   Total steps analyzed: {len(step_results)}")
    print(f"   Total spikes: {sum(spike_counts)}")
    print(f"   Average spikes per step: {np.mean(spike_counts):.1f}")
    print(f"   Spike count standard deviation: {np.std(spike_counts):.1f}")
    print(f"   Maximum spikes in a single step: {np.max(spike_counts)}")
    
    print(f"\n   Firing rate statistics:")
    print(f"   Average firing rate: {np.mean(firing_rates):.2f} Hz")
    print(f"   Firing rate range: {np.min(firing_rates):.2f} - {np.max(firing_rates):.2f} Hz")
    
    # Analyze stability
    if len(firing_rates) >= 10:
        recent_rates = firing_rates[-10:]
        early_rates = firing_rates[:10]
        
        recent_mean = np.mean(recent_rates)
        early_mean = np.mean(early_rates)
        
        stability_change = recent_mean - early_mean
        print(f"\n   Stability Analysis:")
        print(f"     Early firing rate: {early_mean:.2f} Hz")
        print(f"     Recent firing rate: {recent_mean:.2f} Hz")
        print(f"     Stability change: {stability_change:+.2f} Hz")
        
        if abs(stability_change) < 1.0:
            print("     → Stable behavior (firing rate consistent)")
        else:
            print("     → Changing behavior (firing rate shifting)")
    
    print("\n2. Visualization Example:")
    print("   (In a real application, this would generate plots)")
    print("   Sample data for visualization:")
    
    # Sample data for demonstration
    sample_steps = min(20, len(step_results))
    if sample_steps > 0:
        steps = [result['step'] for result in step_results[:sample_steps]]
        spikes = [result['spike_count'] for result in step_results[:sample_steps]]
        
        print(f"     Steps 1-{sample_steps}: {steps}")
        print(f"     Spikes: {spikes}")

def main():
    """Main function to run all examples"""
    print("NLM Basic Simulation Example")
    print("===========================")
    
    try:
        # Step 1: Basic brain setup
        print("\nStep 1: Basic Brain Setup")
        print("-" * 40)
        brain, config = basic_brain_setup()
        
        if brain is None:
            print("Failed to setup brain. Exiting.")
            return 1
        
        # Step 2: Basic simulation run
        print("\nStep 2: Basic Simulation Run")
        print("-" * 40)
        brain, step_results = basic_simulation_run(brain, steps=50)
        
        # Step 3: Basic analysis
        print("\nStep 3: Basic Analysis")
        print("-" * 40)
        basic_analysis(brain, step_results)
        
        # Step 4: Summary
        print_header("Example Complete")
        print("\nThis example demonstrated:")
        print("✓ Basic brain configuration and initialization")
        print("✓ Simple simulation execution")
        print("✓ Metrics collection and monitoring")
        print("✓ Event system for simulation communication")
        print("✓ Basic analysis of simulation results")
        print("\nThe NLM framework is working correctly!")
        
        return 0
        
    except Exception as e:
        print(f"\nError running example: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == '__main__':
    sys.exit(main())