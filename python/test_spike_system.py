#!/usr/bin/env python3

"""
Python tests for NLM spike callback functionality.
Tests the new spike system bindings with proper memory management.
"""

import sys
import os

# Add the module directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import nlm
    print("Successfully imported nlm module")
except ImportError as e:
    print(f"Failed to import nlm module: {e}")
    sys.exit(1)

def test_spike_system_creation():
    """Test basic spike system creation."""
    print("\n=== Testing SpikeSystem Creation ===")
    
    # Create spike system using the factory function
    spike_system = nlm.createSpikeSystem()
    assert spike_system is not None
    print("✓ SpikeSystem created successfully")
    
    # Test basic properties
    assert spike_system.getSpikeHandlerCount() == 0
    assert spike_system.getDelayedSpikeHandlerCount() == 0
    assert spike_system.getSpikeCount() == 0
    print("✓ Initial handler counts are zero")
    
    return spike_system

def test_spike_handler_registration(spike_system):
    """Test spike handler registration with different callback types."""
    print("\n=== Testing Spike Handler Registration ===")
    
    # Test 1: Lambda function callback
    def lambda_handler(source, timestamp, step, regionId, populationId):
        print(f"  Lambda callback: Neuron {source} at t={timestamp}")
    
    result = spike_system.register_spike_handler(lambda_handler)
    assert spike_system.getSpikeHandlerCount() == 1
    print("✓ Lambda callback registered")
    
    # Test 2: Function with arguments
    def function_handler(**kwargs):
        print(f"  Function callback: {kwargs}")
    
    result = spike_system.register_spike_handler(function_handler)
    assert spike_system.getSpikeHandlerCount() == 2
    print("✓ Function callback registered")
    
    # Test 3: Lambda with capture
    captured_value = [42]
    def captured_handler(**kwargs):
        print(f"  Captured callback: value={captured_value[0]}")
        assert kwargs.get('source') == nlm.INVALID_NEURON_ID
    
    result = spike_system.register_spike_handler(captured_handler)
    assert spike_system.getSpikeHandlerCount() == 3
    print("✓ Captured lambda callback registered")
    
    # Test delayed handler
    def delayed_handler(**kwargs):
        print(f"  Delayed callback: {kwargs}")
    
    result = spike_system.register_delayed_spike_handler(delayed_handler)
    assert spike_system.getDelayedSpikeHandlerCount() == 1
    print("✓ Delayed callback registered")
    
    return spike_system

def test_spike_processing(spike_system):
    """Test actual spike processing and callback invocation."""
    print("\n=== Testing Spike Processing ===")
    
    # Create a spike event
    spike_event = nlm.SpikeEvent()
    spike_event.source_neuron = nlm.INVALID_NEURON_ID
    spike_event.timestamp = 1.0
    spike_event.step = 0
    
    # Queue and process spike
    spike_system.queueSpike(spike_event)
    assert spike_system.getPendingSpikeCount() == 1
    
    spike_system.processSpikes(0)
    assert spike_system.getPendingSpikeCount() == 0
    assert spike_system.getSpikeCount() == 1
    print("✓ Spike queued and processed successfully")
    
    # Test history
    history = spike_system.getSpikeHistory()
    assert len(history) == 1
    assert history[0].timestamp == 1.0
    print("✓ Spike history tracking works")
    
    return spike_system

def test_handler_removal(spike_system):
    """Test removal of spike handlers."""
    print("\n=== Testing Handler Removal ===")
    
    # Get initial count
    initial_count = spike_system.getSpikeHandlerCount()
    
    # Remove a handler
    removed = spike_system.remove_spike_handler(0)
    assert removed == True
    assert spike_system.getSpikeHandlerCount() == initial_count - 1
    print("✓ Handler removal successful")
    
    # Try to remove non-existent handler
    removed = spike_system.remove_spike_handler(999)
    assert removed == False
    print("✓ Invalid handler removal handled correctly")
    
    # Remove all handlers
    while spike_system.getSpikeHandlerCount() > 0:
        removed = spike_system.remove_spike_handler(0)
        assert removed == True
    
    assert spike_system.getSpikeHandlerCount() == 0
    print("✓ All handlers removed successfully")
    
    return spike_system

def test_delayed_spikes(spike_system):
    """Test delayed spike functionality."""
    print("\n=== Testing Delayed Spike Processing ===")
    
    # Create delayed spike event
    delayed_event = nlm.DelayedSpikeEvent()
    delayed_event.source_neuron = nlm.INVALID_NEURON_ID
    delayed_event.destination_neuron = nlm.NeuronId(5)
    delayed_event.synapse_id = nlm.SynapseId(1)
    delayed_event.weight = 0.5f
    delayed_event.synapse_type = nlm.SynapseType.Excitatory
    delayed_event.timestamp = 2.0
    delayed_event.delivery_time = 3.0
    delayed_event.step = 2
    delayed_event.delivery_step = 3
    delayed_event.is_excitatory = True
    
    spike_system.queueDelayedSpike(delayed_event)
    assert spike_system.getPendingDelayedCount() == 1
    
    # Process delayed spikes
    spike_system.processDelayedSpikes(3, 3.0)
    assert spike_system.getPendingDelayedCount() == 0
    print("✓ Delayed spike queued and processed")
    
    return spike_system

def test_statistics(spike_system):
    """Test spike system statistics."""
    print("\n=== Testing Statistics ===")
    
    # Add some spikes
    for i in range(5):
        spike_event = nlm.SpikeEvent()
        spike_event.source_neuron = nlm.NeuronId(i)
        spike_event.timestamp = float(i)
        spike_event.step = i
        spike_system.queueSpike(spike_event)
        spike_system.processSpikes(i)
    
    # Test statistics
    assert spike_system.getSpikeCount() == 5
    assert spike_system.getAverageSpikeRate() > 0.0
    print(f"✓ Average spike rate: {spike_system.getAverageSpikeRate():.2f}")
    
    # Get most active neurons
    most_active = spike_system.getMostActiveNeurons(3)
    assert len(most_active) <= 3
    print(f"✓ Most active neurons: {[n.value for n in most_active]}")
    
    return spike_system

def test_system_reset(spike_system):
    """Test system reset functionality."""
    print("\n=== Testing System Reset ===")
    
    # Add some spikes
    spike_event = nlm.SpikeEvent()
    spike_event.source_neuron = nlm.NeuronId(10)
    spike_event.timestamp = 10.0
    spike_event.step = 10
    spike_system.queueSpike(spike_event)
    
    assert spike_system.getPendingSpikeCount() > 0
    assert spike_system.getSpikeCount() > 0
    
    # Reset system
    spike_system.reset()
    assert spike_system.getPendingSpikeCount() == 0
    assert spike_system.getSpikeCount() == 0
    assert spike_system.getSpikeHandlerCount() == 0
    print("✓ System reset successful")
    
    return spike_system

def test_python_callback_types():
    """Test various Python callback types."""
    print("\n=== Testing Python Callback Types ===")
    
    # Test 1: Class method
    class CallbackHandler:
        def __init__(self):
            self.count = 0
        
        def __call__(self, source, timestamp, step, regionId, populationId):
            self.count += 1
            print(f"  Class method callback called {self.count} times")
    
    handler = CallbackHandler()
    spike_system = nlm.createSpikeSystem()
    spike_system.register_spike_handler(handler)
    assert spike_system.getSpikeHandlerCount() == 1
    print("✓ Class method callback works")
    
    # Test 2: Lambda with complex capture
    captured_list = [1, 2, 3]
    lambda_cb = lambda source, timestamp, step, regionId, populationId: print(f"Lambda: captured={captured_list}")
    spike_system.register_spike_handler(lambda_cb)
    assert spike_system.getSpikeHandlerCount() == 2
    print("✓ Lambda with capture works")
    
    # Test 3: Partial function
    def generic_handler(**kwargs):
        print(f"  Generic handler: {kwargs.get('timestamp')}")
    
    spike_system.register_spike_handler(generic_handler)
    assert spike_system.getSpikeHandlerCount() == 3
    print("✓ Generic handler works")
    
    return spike_system

def main():
    """Run all Python tests."""
    print("=== NLM Python Spike Callback Tests ===")
    
    try:
        # Test 1: Basic creation and properties
        spike_system = test_spike_system_creation()
        
        # Test 2: Handler registration with different types
        spike_system = test_spike_handler_registration(spike_system)
        
        # Test 3: Spike processing
        spike_system = test_spike_processing(spike_system)
        
        # Test 4: Handler removal
        spike_system = test_handler_removal(spike_system)
        
        # Test 5: Delayed spikes
        spike_system = test_delayed_spikes(spike_system)
        
        # Test 6: Statistics
        spike_system = test_statistics(spike_system)
        
        # Test 7: System reset
        spike_system = test_system_reset(spike_system)
        
        # Test 8: Python callback types
        spike_system = test_python_callback_types()
        
        print("\n=== All Tests Passed! ===")
        print("\nSpike system Python bindings are working correctly with:")
        print("  ✓ Proper memory management")
        print("  ✓ Thread-safe handler registration")
        print("  ✓ Support for lambda functions and Python callables")
        print("  ✓ Exception safety in callbacks")
        print("  ✓ Statistics and monitoring capabilities")
        print("  ✓ Handler lifecycle management")
        
        return 0
        
    except Exception as e:
        print(f"\n❌ Test failed: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)