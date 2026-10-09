#!/usr/bin/env python3
"""
Example usage demonstrating NLM Python bindings enhancements

This script shows how to use the new Pythonic API, dynamic binding generation,
event system, plugin system, and custom type definitions in NLM.
"""

import sys
import tempfile
import json
from typing import Dict, Any

# Add python directory to path
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_206b7df8-50af-4a9d-949d-21e5ab9e36cf/python')

# Import NLM modules
try:
    import pynlm
    import nlm_ext
    import nlm_plugins
    import nlm_events
    import nlm_types
except ImportError as e:
    print(f"Import error: {e}")
    print("Please ensure NLM Python bindings are properly installed")
    sys.exit(1)

def example_basic_usage():
    """Example: Basic usage of enhanced NLM API"""
    print("=" * 60)
    print("Example 1: Basic Enhanced Usage")
    print("=" * 60)
    
    # Create a brain with configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize the brain
    brain.initialize()
    
    # Use context manager support
    with brain as b:
        print(f"Brain initialized successfully")
        print(f"Total neuron count: {b.getTotalNeuronCount()}")
        
        # Perform simulation steps
        for step in range(5):
            b.step(step)
            print(f"Completed step {step}")
        
        # Save brain state
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            b.save(f.name)
            print(f"Brain saved to: {f.name}")
    
    print("Basic usage example completed!\n")

def example_property_access():
    """Example: Property-style access"""
    print("=" * 60)
    print("Example 2: Property-Style Access")
    print("=" * 60)
    
    # Create a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Use enhanced API for property access
    print("Brain properties via nlm_ext:")
    
    # Get class information
    class_info = nlm_ext.get_class_info('Brain')
    if class_info:
        print(f"  - Has context manager: {class_info.get('has_context_manager', False)}")
        print(f"  - Has properties: {class_info.get('has_properties', False)}")
        print(f"  - Methods count: {class_info.get('methods_count', 0)}")
    
    # List all available classes
    classes = nlm_ext.list_classes()
    print(f"  Available classes: {', '.join(classes)}")
    
    print("Property access example completed!\n")

def example_dynamic_binding():
    """Example: Dynamic binding generation"""
    print("=" * 60)
    print("Example 3: Dynamic Binding Generation")
    print("=" * 60)
    
    # Use dynamic API for runtime class discovery
    print("Dynamic class discovery:")
    
    # Get all available classes
    available_classes = nlm_ext.list_classes()
    print(f"  Classes: {available_classes}")
    
    # Get methods for specific classes
    brain_methods = nlm_ext.get_methods('Brain')
    print(f"  Brain methods: {len(brain_methods)} methods")
    
    # Create instances using dynamic API
    try:
        config = nlm_ext.create_instance('Config')
        print(f"  Created Config instance: {config is not None}")
    except Exception as e:
        print(f"  Config creation failed: {e}")
    
    print("Dynamic binding example completed!\n")

def example_plugin_system():
    """Example: Plugin system"""
    print("=" * 60)
    print("Example 4: Plugin System")
    print("=" * 60)
    
    # Create a sample plugin
    @nlm_plugins.register_plugin
    class MonitoringPlugin(nlm_plugins.NLMPlugin):
        def __init__(self):
            super().__init__("MonitoringPlugin")
            self.metrics = []
        
        def setup(self, api):
            print(f"  Plugin '{self.name}' setup with API")
            self.api = api
        
        def teardown(self):
            print(f"  Plugin '{self.name}' teardown")
        
        def handle_event(self, event_name: str, data: Dict[str, Any]):
            print(f"  Plugin '{self.name}' handling event: {event_name}")
            self.metrics.append({
                'event': event_name,
                'data': data,
                'timestamp': time.time()
            })
    
    # Create plugin instance
    plugin = nlm_plugins.create_plugin('MonitoringPlugin')
    
    print(f"Plugin created: {plugin.name}")
    print(f"Plugin enabled: {plugin.is_enabled()}")
    
    # Add to global plugin manager
    nlm_plugins.add_plugin(plugin)
    
    print("Plugin system example completed!\n")

def example_event_system():
    """Example: Event system"""
    print("=" * 60)
    print("Example 5: Event System")
    print("=" * 60)
    
    # Create event system
    event_system = nlm_events.NLMEventSystem()
    
    # Define event handlers
    def step_handler(event):
        print(f"  Step completed: step={event.data.step_id}, neurons={event.data.neuron_count}")
    
    def spike_handler(event):
        print(f"  Spike detected: neuron={event.data.neuron_id}, rate={event.data.firing_rate}")
    
    def learning_handler(event):
        print(f"  Learning occurred: {event.data.event_type}, strength={event.data.strength}")
    
    # Register event handlers
    event_system.emitter.on(nlm_events.EventType.STEP_COMPLETED, step_handler)
    event_system.emitter.on(nlm_events.EventType.SPIKE_DETECTED, spike_handler)
    event_system.emitter.on(nlm_events.EventType.LEARNING_OCCURRED, learning_handler)
    
    # Create sample event data
    from datetime import datetime
    step_event = nlm_events.SimulationStepEvent(
        step_id=1,
        current_time=0.1,
        neuron_count=100,
        active_neuron_count=25,
        spike_count=15,
        learning_events=3
    )
    
    # Emit events
    print("  Emitting simulation step event...")
    event_system.emit_step_completed(step_event)
    
    spike_event = nlm_events.SpikeEvent(
        neuron_id=42,
        neuron_type="Excitatory",
        region_id=1,
        firing_rate=50.0,
        timestamp=time.time()
    )
    
    print("  Emitting spike event...")
    event_system.emit_spike_detected(spike_event)
    
    learning_event = nlm_events.LearningEvent(
        event_type="STDP",
        strength=0.5,
        learning_rate=0.01,
        affected_neurons=[1, 2, 3],
        affected_synapses=[10, 11, 12],
        timestamp=time.time()
    )
    
    print("  Emitting learning event...")
    event_system.emit_learning_occurred(learning_event)
    
    print("Event system example completed!\n")

def example_custom_types():
    """Example: Custom type definitions"""
    print("=" * 60)
    print("Example 6: Custom Type Definitions")
    print("=" * 60)
    
    # Define custom validator
    class PositiveValidator(nlm_types.NLMTypeValidator):
        def validate(self, instance):
            return hasattr(instance, 'value') and instance.value > 0
        
        def get_error_message(self, instance):
            return "Value must be positive"
    
    # Register validator
    nlm_types.add_validator(PositiveValidator())
    
    # Create custom type instances
    print("Custom type registration:")
    
    # Create Neuritoid
    try:
        neuritoid = nlm_types.Neuritoid(
            id=1,
            type=nlm_types.NeuronType.Excitatory,
            position=(0.0, 0.0),
            velocity=(0.1, 0.1),
            membrane_potential=-65.0,
            refractory_period=5.0,
            age=1.0,
            weight=1.0,
            plasticity_params={},
            metadata={}
        )
        print(f"  Neuritoid created: id={neuritoid.id}, type={neuritoid.type}")
    except Exception as e:
        print(f"  Neuritoid creation failed: {e}")
    
    # Create Synapseoid
    try:
        synapseoid = nlm_types.Synapseoid(
            id=1,
            pre_neuron_id=1,
            post_neuron_id=2,
            type=nlm_types.SynapseType.Excitatory,
            strength=1.0,
            delay=0.1,
            plasticity_rules={},
            metadata={}
        )
        print(f"  Synapseoid created: id={synapseoid.id}, type={synapseoid.type}")
    except Exception as e:
        print(f"  Synapseoid creation failed: {e}")
    
    # Create Regionoid
    try:
        regionoid = nlm_types.Regionoid(
            id=1,
            name="TestRegion",
            neurons=[],
            synapses=[],
            properties={},
            metadata={}
        )
        print(f"  Regionoid created: id={regionoid.id}, name={regionoid.name}")
    except Exception as e:
        print(f"  Regionoid creation failed: {e}")
    
    # Test serialization
    print("\nTesting serialization:")
    try:
        neuritoid_data = nlm_types.serialize(neuritoid)
        print(f"  Neuritoid serialized: {neuritoid_data is not None}")
        
        deserialized = nlm_types.deserialize(neuritoid_data, nlm_types.Neuritoid)
        print(f"  Neuritoid deserialized: {deserialized is not None}")
    except Exception as e:
        print(f"  Serialization test failed: {e}")
    
    print("Custom type example completed!\n")

def example_callback_system():
    """Example: Callback system"""
    print("=" * 60)
    print("Example 7: Callback System")
    print("=" * 60)
    
    # Track callback executions
    step_calls = []
    spike_calls = []
    learning_calls = []
    
    # Define callbacks
    def step_callback(step_data):
        step_calls.append(step_data)
        print(f"  Step callback: step {step_data.get('step', 'unknown')}")
    
    def spike_callback(spike_data):
        spike_calls.append(spike_data)
        print(f"  Spike callback: neuron {spike_data.get('neuron_id', 'unknown')}")
    
    def learning_callback(learning_data):
        learning_calls.append(learning_data)
        print(f"  Learning callback: {learning_data.get('event_type', 'unknown')}")
    
    # Register callbacks
    nlm_ext.add_step_callback(step_callback)
    nlm_ext.add_spike_callback(spike_callback)
    nlm_ext.add_learning_callback(learning_callback)
    
    # Execute callbacks
    print("Executing callbacks...")
    nlm_ext.execute_step_callbacks({"step": 1, "neuron_count": 100})
    nlm_ext.execute_spike_callbacks({"neuron_id": 42, "firing_rate": 50.0})
    nlm_ext.execute_learning_callbacks({"event_type": "STDP", "strength": 0.5})
    
    print(f"Callbacks executed: {len(step_calls)} step, {len(spike_calls)} spike, {len(learning_calls)} learning")
    
    print("Callback system example completed!\n")

def example_hook_system():
    """Example: Hook system"""
    print("=" * 60)
    print("Example 8: Hook System")
    print("=" * 60)
    
    # Track hook executions
    pre_step_calls = []
    post_step_calls = []
    reset_calls = []
    
    # Define hooks
    def pre_step_hook(step_id, current_time):
        pre_step_calls.append((step_id, current_time))
        print(f"  Pre-step hook: step={step_id}, time={current_time}")
    
    def post_step_hook(step_id, current_time):
        post_step_calls.append((step_id, current_time))
        print(f"  Post-step hook: step={step_id}, time={current_time}")
    
    def reset_hook():
        reset_calls.append(True)
        print("  Reset hook called")
    
    # Register hooks
    nlm_ext.add_hook('pre_step', pre_step_hook)
    nlm_ext.add_hook('post_step', post_step_hook)
    nlm_ext.add_hook('on_reset', reset_hook)
    
    # Execute hooks
    print("Executing hooks...")
    nlm_ext.call_hooks('pre_step', 1, 0.1)
    nlm_ext.call_hooks('post_step', 1, 0.1)
    nlm_ext.call_hooks('on_reset')
    
    print(f"Hooks executed: {len(pre_step_calls)} pre-step, {len(post_step_calls)} post-step, {len(reset_calls)} reset")
    
    print("Hook system example completed!\n")

def example_advanced_integration():
    """Example: Advanced integration of multiple features"""
    print("=" * 60)
    print("Example 9: Advanced Integration")
    print("=" * 60)
    
    # Create a brain with enhanced API
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup event system
    event_system = nlm_events.NLMEventSystem()
    
    def custom_step_handler(event):
        print(f"  Custom handler: Processed step {event.data.step_id}")
    
    event_system.emitter.on(nlm_events.EventType.STEP_COMPLETED, custom_step_handler)
    
    # Setup plugin system
    @nlm_plugins.register_plugin
    class AdvancedPlugin(nlm_plugins.NLMPlugin):
        def setup(self, api):
            self.api = api
            print(f"  Advanced plugin ready")
        
        def teardown(self):
            print(f"  Advanced plugin cleaned up")
    
    # Create and enable plugin
    plugin = nlm_plugins.create_plugin('AdvancedPlugin')
    nlm_plugins.add_plugin(plugin)
    nlm_plugins.enable_plugin('AdvancedPlugin')
    
    # Use callback system
    def monitoring_callback(data):
        print(f"  Monitoring: {data}")
    
    nlm_ext.add_step_callback(monitoring_callback)
    
    # Use hook system
    def integration_hook(step_id, current_time):
        print(f"  Integration hook: Custom logic for step {step_id}")
    
    nlm_ext.add_hook('pre_step', integration_hook)
    
    # Run simulation
    print("Running simulation with integrated features...")
    for step in range(3):
        # Execute hooks
        nlm_ext.call_hooks('pre_step', step, step * 0.1)
        
        # Emit event
        step_event = nlm_events.SimulationStepEvent(
            step_id=step,
            current_time=step * 0.1,
            neuron_count=100,
            active_neuron_count=25 + step * 10,
            spike_count=15 + step * 5,
            learning_events=3 + step
        )
        event_system.emit_step_completed(step_event)
        
        # Execute step in brain (simplified)
        brain.step(step)
        
        # Execute post-step hook
        nlm_ext.call_hooks('post_step', step, step * 0.1)
    
    print("Advanced integration example completed!\n")

def main():
    """Run all examples"""
    print("NLM Python Bindings Enhancement Examples")
    print("=" * 60)
    print()
    
    # Import time for examples
    global time
    import time
    
    # Run examples
    example_basic_usage()
    example_property_access()
    example_dynamic_binding()
    example_plugin_system()
    example_event_system()
    example_custom_types()
    example_callback_system()
    example_hook_system()
    example_advanced_integration()
    
    print("=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)
    print()
    print("Summary of features demonstrated:")
    print("1. Enhanced Pythonic API (context managers, property access)")
    print("2. Dynamic binding generation (class discovery, introspection)")
    print("3. Plugin system (plugin registration, management)")
    print("4. Event system (event handling, emission, filtering)")
    print("5. Custom type definitions (type registry, validation)")
    print("6. Callback system (step, spike, learning callbacks)")
    print("7. Hook system (pre/post-step, reset hooks)")
    print("8. Advanced integration (combining multiple features)")
    
    return 0
if __name__ == "__main__":
    import time
    sys.exit(main())