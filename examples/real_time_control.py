"""
Real-Time Control Examples for NLM

This module demonstrates real-time control applications using NLM.
Key features include:
- Real-time control loops
- Timing-critical operations
- Sensor integration
- Actuator control
- Performance monitoring

Author: NLM Project
License: MIT
Version: 1.0
"""

import pynlm
import time
import threading
import numpy as np
from typing import List, Dict, Any, Callable
import queue
import signal
import sys
class ControlSystem:
    """
    Real-time control system for NLM.
    
    This system implements:
    - Real-time control loops
    - Sensor data processing
    - Actuator command generation
    - Timing-critical operations
    - Error handling and recovery
    """
    
    def __init__(self, brain: pynlm.Brain, world: pynlm.SimpleWorld):
        self.brain = brain
        self.world = world
        
        # Control system state
        self.control_loop_active = False
        self.control_thread = None
        self.simulation_rate = 1000  # Hz
        self.control_latency = 0.001  # seconds
        self.computation_budget = 0.0002  # seconds per control cycle
        
        # Data queues
        self.sensor_queue = queue.Queue(maxsize=10)
        self.actuator_queue = queue.Queue(maxsize=10)
        self.control_queue = queue.Queue(maxsize=10)
        
        # Performance monitoring
        self.cycle_times = []
        self.control_cycles_completed = 0
        self.errors = []
        
        # Control functions
        self.sensor_function = None
        self.control_function = None
        self.actuator_function = None
        
    def setSensorFunction(self, func: Callable[[], pynlm.SensorPercept]):
        """Set sensor data acquisition function."""
        self.sensor_function = func
        
    def setControlFunction(self, func: Callable[[pynlm.SensorPercept, float], pynlm.Action]):
        """Set control computation function."""
        self.control_function = func
        
    def setActuatorFunction(self, func: Callable[[pynlm.Action, float], None]):
        """Set actuator application function."""
        self.actuator_function = func
        
    def startControlLoop(self):
        """Start real-time control loop."""
        if self.control_thread is not None:
            return
            
        self.control_loop_active = True
        self.control_thread = threading.Thread(target=self.controlLoop, daemon=True)
        self.control_thread.start()
        
        print(f"Control loop started at {self.simulation_rate} Hz")
        
    def stopControlLoop(self):
        """Stop real-time control loop."""
        self.control_loop_active = False
        
        if self.control_thread:
            self.control_thread.join(timeout=1.0)
            self.control_thread = None
            
        print("Control loop stopped")
        
    def controlLoop(self):
        """Main control loop."""
        last_time = time.time()
        target_period = 1.0 / self.simulation_rate
        
        while self.control_loop_active:
            loop_start = time.time()
            
            try:
                # Execute control cycle
                self.executeControlCycle(loop_start, target_period)
                
            except Exception as e:
                self.errors.append({
                    'timestamp': loop_start,
                    'error': str(e),
                    'type': type(e).__name__
                })
                print(f"Control loop error: {e}")
            
            # Sleep to maintain timing
            elapsed = time.time() - loop_start
            sleep_time = max(0, target_period - elapsed)
            
            if sleep_time > 0:
                time.sleep(sleep_time)
                
    def executeControlCycle(self, current_time: float, target_period: float):
        """
        Execute one control cycle.
        
        Args:
            current_time: Current time
            target_period: Target control cycle period
        """
        cycle_start = time.time()
        
        # Measure computation time
        comp_start = time.time()
        
        # Get sensor data
        if self.sensor_function:
            percept = self.sensor_function()
        else:
            percept = self.getDefaultSensorData()
        
        # Process control computation
        if self.control_function:
            action = self.control_function(percept, current_time)
        else:
            action = self.defaultControl(percept, current_time)
        
        # Apply actuator commands
        if self.actuator_function:
            self.actuator_function(action, current_time)
        else:
            self.defaultActuator(action, current_time)
        
        comp_time = time.time() - comp_start
        
        # Check if computation time exceeds budget
        if comp_time > self.computation_budget:
            print(f"Warning: Control computation exceeded budget ({comp_time:.6f}s > {self.computation_budget:.6f}s)")
        
        # Record metrics
        cycle_time = time.time() - cycle_start
        self.cycle_times.append(cycle_time)
        self.control_cycles_completed += 1
        
        # Log performance
        if len(self.cycle_times) % 100 == 0:
            self.logPerformance()
            
    def getDefaultSensorData(self) -> pynlm.SensorPercept:
        """Get default sensor data from world."""
        return self.world.getSensoryPercept()
        
    def defaultControl(self, percept: pynlm.SensorPercept, current_time: float) -> pynlm.Action:
        """Default control policy."""
        # Simple policy based on percept
        if percept:
            # Get vision features
            vision = percept.getVision()
            if vision:
                width, height = vision.getWidth(), vision.getHeight()
                # Simple policy: move toward center
                if width > 10 and height > 10:
                    return pynlm.Action(pynlm.ActionType.MoveForward)
        
        return pynlm.Action(pynlm.ActionType.Wait)
        
    def defaultActuator(self, action: pynlm.Action, current_time: float):
        """Default actuator application."""
        # Apply action to world
        self.world.applyMotorCommand(action, current_time)
        
    def getPerformanceReport(self) -> Dict[str, Any]:
        """Get control system performance report."""
        if not self.cycle_times:
            return {
                'status': 'no_cycles_completed',
                'control_loop_active': self.control_loop_active
            }
        
        cycle_times_array = np.array(self.cycle_times)
        
        return {
            'status': 'running' if self.control_loop_active else 'stopped',
            'total_cycles': len(self.cycle_times),
            'avg_cycle_time_ms': np.mean(cycle_times_array) * 1000,
            'min_cycle_time_ms': np.min(cycle_times_array) * 1000,
            'max_cycle_time_ms': np.max(cycle_times_array) * 1000,
            'std_cycle_time_ms': np.std(cycle_times_array) * 1000,
            'target_frequency_hz': self.simulation_rate,
            'achieved_frequency_hz': 1.0 / np.mean(cycle_times_array) if np.mean(cycle_times_array) > 0 else 0,
            'computation_budget_ms': self.computation_budget * 1000,
            'error_count': len(self.errors),
            'error_rate': len(self.errors) / max(1, len(self.cycle_times))
        }
        
    def logPerformance(self):
        """Log performance metrics."""
        report = self.getPerformanceReport()
        print(f"Control Performance - "
              f"Cycles: {report['total_cycles']}, "
              f"Avg Time: {report['avg_cycle_time_ms']:.3f}ms, "
              f"Hz: {report['achieved_frequency_hz']:.1f}, "
              f"Errors: {report['error_count']}")
class MultiRateControlSystem:
    """
    Multi-rate control system with different control loops.
    
    This system implements:
    - High-frequency control loops (1kHz)
    - Medium-frequency control loops (100Hz)
    - Low-frequency control loops (10Hz)
    - Coordinated control execution
    - Resource management
    """
    
    def __init__(self, brain: pynlm.Brain, world: pynlm.SimpleWorld):
        self.brain = brain
        self.world = world
        
        # Control systems
        self.high_rate_system = None
        self.medium_rate_system = None
        self.low_rate_system = None
        
        # Timing configuration
        self.simulation_time_step = 0.001  # 1kHz
        self.simulation_start_time = time.time()
        
    def setupControlSystems(self):
        """Setup control systems with different rates."""
        # High-rate control (1kHz)
        self.high_rate_system = ControlSystem(self.brain, self.world)
        self.high_rate_system.setSensorFunction(self.getHighRateSensorData)
        self.high_rate_system.setControlFunction(self.getHighRateControl)
        self.high_rate_system.setActuatorFunction(self.applyHighRateActuator)
        self.high_rate_system.simulation_rate = 1000
        self.high_rate_system.computation_budget = 0.0001
        
        # Medium-rate control (100Hz)
        self.medium_rate_system = ControlSystem(self.brain, self.world)
        self.medium_rate_system.setSensorFunction(self.getMediumRateSensorData)
        self.medium_rate_system.setControlFunction(self.getMediumRateControl)
        self.medium_rate_system.setActuatorFunction(self.applyMediumRateActuator)
        self.medium_rate_system.simulation_rate = 100
        self.medium_rate_system.computation_budget = 0.0002
        
        # Low-rate control (10Hz)
        self.low_rate_system = ControlSystem(self.brain, self.world)
        self.low_rate_system.setSensorFunction(self.getLowRateSensorData)
        self.low_rate_system.setControlFunction(self.getLowRateControl)
        self.low_rate_system.setActuatorFunction(self.applyLowRateActuator)
        self.low_rate_system.simulation_rate = 10
        self.low_rate_system.computation_budget = 0.001
        
    def startAllControlSystems(self):
        """Start all control systems."""
        if self.high_rate_system:
            self.high_rate_system.startControlLoop()
        if self.medium_rate_system:
            self.medium_rate_system.startControlLoop()
        if self.low_rate_system:
            self.low_rate_system.startControlLoop()
            
        print("All control systems started")
        
    def stopAllControlSystems(self):
        """Stop all control systems."""
        if self.high_rate_system:
            self.high_rate_system.stopControlLoop()
        if self.medium_rate_system:
            self.medium_rate_system.stopControlLoop()
        if self.low_rate_system:
            self.low_rate_system.stopControlLoop()
            
        print("All control systems stopped")
        
    def getHighRateSensorData(self) -> pynlm.SensorPercept:
        """High-rate sensor data (1kHz)."""
        # High-frequency sensor updates
        return self.world.getSensoryPercept()
        
    def getMediumRateSensorData(self) -> pynlm.SensorPercept:
        """Medium-rate sensor data (100Hz)."""
        # Medium-frequency sensor updates with averaging
        return self.world.getSensoryPercept()
        
    def getLowRateSensorData(self) -> pynlm.SensorPercept:
        """Low-rate sensor data (10Hz)."""
        # Low-frequency sensor updates
        return self.world.getSensoryPercept()
        
    def getHighRateControl(self, percept: pynlm.SensorPercept, current_time: float) -> pynlm.Action:
        """High-rate control logic (1kHz)."""
        # Fast, reactive control for high-frequency dynamics
        return pynlm.Action(pynlm.ActionType.MoveForward)
        
    def getMediumRateControl(self, percept: pynlm.SensorPercept, current_time: float) -> pynlm.Action:
        """Medium-rate control logic (100Hz)."""
        # Medium-speed control with state estimation
        return pynlm.Action(pynlm.ActionType.TurnLeft)
        
    def getLowRateControl(self, percept: pynlm.SensorPercept, current_time: float) -> pynlm.Action:
        """Low-rate control logic (10Hz)."""
        # Slow, deliberative control for long-term planning
        return pynlm.Action(pynlm.ActionType.Rest)
        
    def applyHighRateActuator(self, action: pynlm.Action, current_time: float):
        """Apply high-rate actuator commands."""
        # Apply action quickly
        self.world.applyMotorCommand(action, current_time)
        
    def applyMediumRateActuator(self, action: pynlm.Action, current_time: float):
        """Apply medium-rate actuator commands."""
        # Apply action with medium speed
        time.sleep(0.001)  # Simulate actuator delay
        self.world.applyMotorCommand(action, current_time)
        
    def applyLowRateActuator(self, action: pynlm.Action, current_time: float):
        """Apply low-rate actuator commands."""
        # Apply action slowly
        time.sleep(0.01)  # Simulate actuator delay
        self.world.applyMotorCommand(action, current_time)
        
    def getIntegratedPerformanceReport(self) -> Dict[str, Any]:
        """Get integrated performance report from all control systems."""
        report = {
            'high_rate_system': {},
            'medium_rate_system': {},
            'low_rate_system': {},
            'overall': {}
        }
        
        # Get individual reports
        if self.high_rate_system:
            report['high_rate_system'] = self.high_rate_system.getPerformanceReport()
        if self.medium_rate_system:
            report['medium_rate_system'] = self.medium_rate_system.getPerformanceReport()
        if self.low_rate_system:
            report['low_rate_system'] = self.low_rate_system.getPerformanceReport()
        
        # Calculate overall metrics
        total_cycles = (report['high_rate_system'].get('total_cycles', 0) +
                       report['medium_rate_system'].get('total_cycles', 0) +
                       report['low_rate_system'].get('total_cycles', 0))
        
        total_errors = (report['high_rate_system'].get('error_count', 0) +
                       report['medium_rate_system'].get('error_count', 0) +
                       report['low_rate_system'].get('error_count', 0))
        
        report['overall'] = {
            'total_control_cycles': total_cycles,
            'total_errors': total_errors,
            'error_rate': total_errors / max(1, total_cycles),
            'all_systems_active': (
                report['high_rate_system'].get('status') == 'running' and
                report['medium_rate_system'].get('status') == 'running' and
                report['low_rate_system'].get('status') == 'running'
            )
        }
        
        return report
class RealTimeController:
    """
    Real-time controller with comprehensive error handling and recovery.
    
    This controller implements:
    - Real-time error detection and recovery
    - Adaptive control parameters
    - Resource monitoring and management
    - Graceful degradation
    - Performance optimization
    """
    
    def __init__(self, brain: pynlm.Brain, world: pynlm.SimpleWorld):
        self.brain = brain
        self.world = world
        
        # Control system
        self.control_system = ControlSystem(brain, world)
        
        # State
        self.is_running = False
        self.error_count = 0
        self.recovery_actions = []
        
        # Configuration
        self.max_consecutive_errors = 5
        self.recovery_timeout = 1.0
        self.performance_threshold = 0.9
        
    def start(self):
        """Start the real-time controller."""
        if self.is_running:
            return
            
        self.is_running = True
        self.error_count = 0
        self.recovery_actions.clear()
        
        # Setup control system
        self.control_system.startControlLoop()
        
        print("Real-time controller started")
        
    def stop(self):
        """Stop the real-time controller."""
        if not self.is_running:
            return
            
        self.is_running = False
        
        # Stop control system
        self.control_system.stopControlLoop()
        
        print("Real-time controller stopped")
        
    def getStatus(self) -> Dict[str, Any]:
        """Get controller status."""
        return {
            'is_running': self.is_running,
            'error_count': self.error_count,
            'recovery_actions': self.recovery_actions,
            'control_system_status': self.control_system.getPerformanceReport()
        }
        
    def handleError(self, error: Exception):
        """
        Handle control error.
        
        Args:
            error: Error that occurred
        """
        self.error_count += 1
        self.recovery_actions.append({
            'timestamp': time.time(),
            'error': str(error),
            'type': type(error).__name__
        })
        
        # Check if we need recovery
        if self.error_count >= self.max_consecutive_errors:
            self.performRecovery()
            
    def performRecovery(self):
        """Perform recovery actions."""
        print("Performing recovery actions...")
        
        # Stop control system
        self.control_system.stopControlLoop()
        
        # Wait for recovery timeout
        time.sleep(self.recovery_timeout)
        
        # Reset error count
        self.error_count = 0
        
        # Restart control system
        self.control_system.startControlLoop()
        
        print("Recovery completed")
def runRealTimeControlExample():
    """Run the real-time control example."""
    print("=== Real-Time Control Example ===\n")
    
    # Create brain and world
    config = pynlm.createDemoConfig(2000, 0.1)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # Create control system
    controller = RealTimeController(brain, world)
    
    print("Real-time controller initialized with:")
    print("- High-frequency control loop (1kHz)")
    print("- Sensor integration")
    print("- Actuator control")
    print("- Error handling and recovery")
    print("- Performance monitoring")
    print()
    
    # Start controller
    controller.start()
    
    # Get initial status
    status = controller.getStatus()
    print(f"Controller started - Status: {status['status']}")
    
    # Simulate some control cycles
    for i in range(100):
        # Get current status
        status = controller.getStatus()
        
        if i % 50 == 0:
            print(f"Cycle {i}: Errors={status['error_count']}, "
                  f"Cycles={status['control_system_status'].get('total_cycles', 0)}")
        
        # Simulate some work
        time.sleep(0.01)
    
    # Stop controller
    controller.stop()
    
    print("\n=== Real-Time Control Complete ===")
if __name__ == "__main__":
    runRealTimeControlExample()