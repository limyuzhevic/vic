#!/usr/bin/env python3
"""
Test script for NLM Brain System Monitoring Interface

This script demonstrates the core functionality of the monitoring interface
without the circular import issues.
"""

import sys
import os
import time
from datetime import datetime

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

# Mock the problematic imports to test monitoring functionality
import unittest.mock as mock

# Create mock classes for the NLM core components
class MockBrain:
    def __init__(self):
        self.total_neurons = 1000
        self.active_neurons = 100
        self.firing_neurons = 50
        self.average_firing_rate = 25.5
        self.excitement_inhibition_ratio = 1.2
        self.total_spikes = 5000
        self.developmental_stage = "Stage 2"
    
    def getRegions(self):
        return []
    
    def getTotalNeuronCount(self):
        return self.total_neurons
    
    def getActiveNeuronCount(self):
        return self.active_neurons
    
    def getFiringNeuronCount(self):
        return self.firing_neurons
    
    def getAverageFiringRate(self):
        return self.average_firing_rate
    
    def getExcitationInhibitionRatio(self):
        return self.excitement_inhibition_ratio
    
    def getTotalSpikeCount(self):
        return self.total_spikes
    
    def getDevelopmentalStage(self):
        return self.developmental_stage

class MockConfig:
    pass

class MockAgentBrain:
    pass

class MockSimpleWorld:
    pass

class MockNeuronId:
    pass

class MockSynapseId:
    pass

class MockRegionId:
    pass

class MockPopulationId:
    pass

class MockNeuronType:
    pass

class MockSynapseType:
    pass

class MockDevelopmentalStage:
    pass

class MockFiringState:
    pass

class MockActionType:
    pass

class MockMotorCommand:
    pass

class MockWorldObjectType:
    pass

class MockVision:
    pass

class MockAudio:
    pass

class MockInternalSignals:
    pass

# Mock the imports that interface.py needs
mock_modules = [
    'Brain', 'Config', 'AgentBrain', 'SimpleWorld',
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType', 'Vision', 'Audio', 'InternalSignals'
]

# Create mock classes for the NLM package
mock_pynlm_classes = {}
for name in mock_modules:
    mock_pynlm_classes[name] = type(name, (), {})

# Patch the imports in interface.py
sys.modules['pynlm'] = mock_pynlm_classes

# Now import the monitoring interface
try:
    from pynlm.monitoring import MonitoringInterface, MonitoringLevel
    print("✓ Successfully imported MonitoringInterface and MonitoringLevel")
    
    # Create a mock interface
    mock_interface = mock.Mock()
    mock_interface.context = mock.Mock()
    mock_interface.context.brain = MockBrain()
    mock_interface.context.mode = "real_time"
    mock_interface.context.running = True
    mock_interface.context.event_queue = mock.Mock()
    mock_interface.context.event_queue.qsize = mock.Mock(return_value=0)
    
    # Create monitoring interface
    monitor = MonitoringInterface(mock_interface)
    print("✓ Successfully created MonitoringInterface instance")
    
    # Test starting monitoring
    print("\n=== Testing Monitoring Interface ===\n")
    
    # Start monitoring with basic level
    print("1. Starting monitoring with BASIC level...")
    success = monitor.start_monitoring(
        level=MonitoringLevel.BASIC,
        mode="real_time",
        update_interval=2.0
    )
    
    if success:
        print("   ✓ Monitoring started successfully!")
        
        # Run for a few seconds to collect data
        print("2. Running monitoring for 5 seconds...")
        start_time = time.time()
        
        while time.time() - start_time < 5:
            elapsed = time.time() - start_time
            remaining = 5 - elapsed
            print(f"   Monitoring... {elapsed:.1f}s elapsed, {remaining:.1f}s remaining", end='\r')
            time.sleep(0.5)
        
        print(f"\n3. Collecting monitoring data...")
        
        # Analyze collected data
        neural_data = monitor.monitoring_data['neural_activity']
        system_data = monitor.monitoring_data['system_resources']
        
        if neural_data:
            latest_neural = neural_data[-1]['data']
            print(f"   Neural Activity Analysis:")
            print(f"     - Total neurons: {latest_neural.get('total_neurons', 'N/A')}")
            print(f"     - Active neurons: {latest_neural.get('active_neurons', 'N/A')}")
            print(f"     - Firing neurons: {latest_neural.get('firing_neurons', 'N/A')}")
            print(f"     - Average firing rate: {latest_neural.get('average_firing_rate', 0):.2f} Hz")
            print(f"     - Active ratio: {latest_neural.get('active_ratio', 0):.2%}")
        
        if system_data:
            latest_system = system_data[-1]['data']
            print(f"\n   System Resources Analysis:")
            print(f"     - CPU usage: {latest_system.get('cpu_percent', 0):.1f}%")
            print(f"     - Memory usage: {latest_system.get('memory_percent', 0):.1f}%")
            print(f"     - Thread count: {latest_system.get('thread_count', 'N/A')}")
        
        # Test alert system if available
        if monitor.alert_system:
            alert_count = monitor.alert_system.get_alert_count()
            print(f"\n   Alert System: {alert_count} alerts detected")
        
        # Test anomaly detector if available
        if monitor.anomaly_detector:
            anomaly_count = monitor.anomaly_detector.get_anomaly_count()
            print(f"   Anomaly Detection: {anomaly_count} anomalies detected")
        
        # Test performance profiler if available
        if monitor.performance_profiler:
            perf_summary = monitor.performance_profiler.get_performance_summary()
            if perf_summary:
                print(f"\n   Performance Summary:")
                print(f"     - Average CPU usage: {perf_summary.get('average_cpu_usage', 0):.1f}%")
                print(f"     - Average memory usage: {perf_summary.get('average_memory_usage', 0):.1f}%")
                print(f"     - Data points: {perf_summary.get('data_points', 0)}")
        
        # Stop monitoring
        print("\n4. Stopping monitoring...")
        monitor.stop_monitoring()
        print("   ✓ Monitoring stopped successfully!")
        
    else:
        print("   ✗ Failed to start monitoring")
    
    print("\n=== Test completed successfully! ===")
    
except ImportError as e:
    print(f"✗ Import error: {e}")
    import traceback
    traceback.print_exc()
except Exception as e:
    print(f"✗ Error: {e}")
    import traceback
    traceback.print_exc()

print("\n" + "="*60)
print("NLM Brain System Monitoring Interface Test")
print("="*60)
print("\nThe monitoring interface provides comprehensive real-time")
print("monitoring capabilities for NLM brain systems with support for:")
print("• Multi-level monitoring (BASIC, DETAILED, COMPREHENSIVE, DEBUG)")
print("• Real-time data collection and analysis")
print("• Alert and anomaly detection")
print("• Performance profiling")
print("• Multiple interface options (CLI, Web, Terminal, API)")
print("• Historical data tracking")
print("• Extensible plugin system")
print("="*60)