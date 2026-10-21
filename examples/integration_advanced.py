"""
Advanced Integration Examples for NLM

This module demonstrates advanced integration patterns for NLM with external systems.
Key features include:
- External system integration
- Data flow management
- Communication protocols
- System coordination
- Cross-platform compatibility

Author: NLM Project
License: MIT
Version: 1.0
"""

import pynlm
import time
import threading
import json
import numpy as np
from typing import Dict, List, Any, Optional, Callable
from abc import ABC, abstractmethod
import queue
class ExternalSystemInterface(ABC):
    """
    Abstract interface for external systems.
    
    This interface defines the contract for external systems to integrate
    with NLM. All external systems must implement this interface.
    """
    
    @abstractmethod
    def connect(self) -> bool:
        """
        Connect to the external system.
        
        Returns:
            True if connection successful, False otherwise
        """
        pass
        
    @abstractmethod
    def disconnect(self) -> bool:
        """
        Disconnect from the external system.
        
        Returns:
            True if disconnection successful, False otherwise
        """
        pass
        
    @abstractmethod
    def isConnected(self) -> bool:
        """
        Check if connected to external system.
        
        Returns:
            True if connected, False otherwise
        """
        pass
        
    @abstractmethod
    def sendData(self, data: Dict[str, Any]) -> bool:
        """
        Send data to external system.
        
        Args:
            data: Data to send
            
        Returns:
            True if data sent successfully, False otherwise
        """
        pass
        
    @abstractmethod
    def receiveData(self) -> Optional[Dict[str, Any]]:
        """
        Receive data from external system.
        
        Returns:
            Data received, or None if no data available
        """
        pass
        
    @abstractmethod
    def getSystemStatus(self) -> Dict[str, Any]:
        """
        Get system status.
        
        Returns:
            Dictionary with system status information
        """
        pass
class SensorSystemInterface(ExternalSystemInterface):
    """
    Interface for sensor systems.
    
    Sensor systems provide sensory data to NLM.
    This interface defines the contract for sensor systems.
    """
    
    @abstractmethod
    def startStreaming(self):
        """Start sensor data streaming."""
        pass
        
    @abstractmethod
    def stopStreaming(self):
        """Stop sensor data streaming."""
        pass
        
    @abstractmethod
    def getSensorData(self) -> pynlm.SensorPercept:
        """
        Get current sensor data.
        
        Returns:
            Current sensor percept
        """
        pass
class ActuatorSystemInterface(ExternalSystemInterface):
    """
    Interface for actuator systems.
    
    Actuator systems receive commands from NLM to control external devices.
    This interface defines the contract for actuator systems.
    """
    
    @abstractmethod
    def executeCommand(self, command: pynlm.Action, parameters: Dict[str, Any] = None):
        """
        Execute actuator command.
        
        Args:
            command: Action to execute
            parameters: Command parameters
        """
        pass
class CommunicationProtocol:
    """
    Communication protocol for external system integration.
    
    This class handles:
    - Message serialization
    - Message parsing
    - Error handling
    - Connection management
    - Data validation
    """
    
    def __init__(self, protocol_type: str = "json"):
        self.protocol_type = protocol_type
        self.message_queue = queue.Queue()
        self.connection_pool = {}
        self.retry_count = 3
        self.timeout = 5.0
        
    def serializeMessage(self, message: Dict[str, Any]) -> bytes:
        """
        Serialize message to bytes.
        
        Args:
            message: Message to serialize
            
        Returns:
            Serialized message
        """
        if self.protocol_type == "json":
            return json.dumps(message).encode('utf-8')
        elif self.protocol_type == "binary":
            # Implement binary serialization
            return message.tobytes() if hasattr(message, 'tobytes') else str(message).encode()
        else:
            raise ValueError(f"Unsupported protocol type: {self.protocol_type}")
        
    def deserializeMessage(self, data: bytes) -> Dict[str, Any]:
        """
        Deserialize message from bytes.
        
        Args:
            data: Serialized message data
            
        Returns:
            Deserialized message
        """
        if self.protocol_type == "json":
            return json.loads(data.decode('utf-8'))
        elif self.protocol_type == "binary":
            # Implement binary deserialization
            return {"data": data.decode()}
        else:
            raise ValueError(f"Unsupported protocol type: {self.protocol_type}")
        
    def validateMessage(self, message: Dict[str, Any]) -> bool:
        """
        Validate message structure.
        
        Args:
            message: Message to validate
            
        Returns:
            True if message is valid, False otherwise
        """
        required_fields = ['timestamp', 'message_type', 'data']
        return all(field in message for field in required_fields)
        
    def createMessage(self, message_type: str, data: Dict[str, Any]) -> Dict[str, Any]:
        """
        Create standardized message.
        
        Args:
            message_type: Type of message
            data: Message data
            
        Returns:
            Standardized message
        """
        return {
            'timestamp': time.time(),
            'message_type': message_type,
            'source': 'nlm',
            'data': data,
            'protocol': self.protocol_type
        }
class ExternalSensorSystem(SensorSystemInterface):
    """
    Example external sensor system.
    
    This example demonstrates how to integrate an external sensor system
    with NLM. The sensor system simulates real sensor data from an
    external device.
    """
    
    def __init__(self, sensor_id: str):
        self.sensor_id = sensor_id
        self.is_connected = False
        self.streaming = False
        self.last_data = None
        self.data_rate = 100  # Hz
        
    def connect(self) -> bool:
        """Connect to sensor system."""
        try:
            # Simulate connection process
            time.sleep(0.1)
            self.is_connected = True
            print(f"Connected to sensor system {self.sensor_id}")
            return True
        except Exception as e:
            print(f"Failed to connect to sensor system {self.sensor_id}: {e}")
            return False
            
    def disconnect(self) -> bool:
        """Disconnect from sensor system."""
        try:
            self.streaming = False
            self.is_connected = False
            print(f"Disconnected from sensor system {self.sensor_id}")
            return True
        except Exception as e:
            print(f"Failed to disconnect from sensor system {self.sensor_id}: {e}")
            return False
            
    def isConnected(self) -> bool:
        """Check connection status."""
        return self.is_connected
        
    def startStreaming(self):
        """Start sensor data streaming."""
        if not self.is_connected:
            raise RuntimeError("Sensor system not connected")
            
        self.streaming = True
        print(f"Started streaming from sensor system {self.sensor_id}")
        
    def stopStreaming(self):
        """Stop sensor data streaming."""
        self.streaming = False
        print(f"Stopped streaming from sensor system {self.sensor_id}")
        
    def getSensorData(self) -> pynlm.SensorPercept:
        """
        Get sensor data.
        
        Returns:
            Current sensor data
        """
        if not self.is_connected or not self.streaming:
            return None
            
        # Simulate sensor data
        if np.random.random() < 0.3:  # 30% chance of detecting something
            # Create vision data
            width, height = 8, 8
            data = np.random.uniform(0, 1, width * height).tolist()
            vision = pynlm.Vision(width, height)
            vision.setData(data)
            return vision
        else:
            return None
class ExternalActuatorSystem(ActuatorSystemInterface):
    """
    Example external actuator system.
    
    This example demonstrates how to integrate an external actuator system
    with NLM. The actuator system simulates real device control.
    """
    
    def __init__(self, actuator_id: str):
        self.actuator_id = actuator_id
        self.is_connected = False
        self.command_history = []
        
    def connect(self) -> bool:
        """Connect to actuator system."""
        try:
            time.sleep(0.1)
            self.is_connected = True
            print(f"Connected to actuator system {self.actuator_id}")
            return True
        except Exception as e:
            print(f"Failed to connect to actuator system {self.actuator_id}: {e}")
            return False
            
    def disconnect(self) -> bool:
        """Disconnect from actuator system."""
        try:
            self.is_connected = False
            print(f"Disconnected from actuator system {self.actuator_id}")
            return True
        except Exception as e:
            print(f"Failed to disconnect from actuator system {self.actuator_id}: {e}")
            return False
            
    def isConnected(self) -> bool:
        """Check connection status."""
        return self.is_connected
        
    def executeCommand(self, command: pynlm.Action, parameters: Dict[str, Any] = None):
        """
        Execute actuator command.
        
        Args:
            command: Action to execute
            parameters: Command parameters
        """
        if not self.is_connected:
            raise RuntimeError("Actuator system not connected")
            
        # Record command execution
        command_record = {
            'timestamp': time.time(),
            'command': str(command.getType()) if command else None,
            'parameters': parameters or {},
            'success': True
        }
        self.command_history.append(command_record)
        
        # Simulate actuator response
        print(f"Executed command {command.getType() if command else 'None'} "
              f"on actuator system {self.actuator_id}")
        
    def getSystemStatus(self) -> Dict[str, Any]:
        """Get system status."""
        return {
            'is_connected': self.is_connected,
            'actuator_id': self.actuator_id,
            'total_commands': len(self.command_history),
            'last_command': self.command_history[-1] if self.command_history else None,
            'system_health': 'healthy' if self.is_connected else 'disconnected'
        }
class ExternalSystemManager:
    """
    Manager for external system integration.
    
    This class manages:
    - External system registration
    - Connection management
    - Data flow coordination
    - Error handling
    - System monitoring
    """
    
    def __init__(self):
        self.external_systems = {}
        self.communication_protocol = CommunicationProtocol()
        self.data_queue = queue.Queue()
        self.system_monitor_thread = None
        self.is_running = False
        
    def registerExternalSystem(self, system_id: str, system: ExternalSystemInterface):
        """
        Register external system.
        
        Args:
            system_id: Unique identifier for external system
            system: External system interface
        """
        self.external_systems[system_id] = system
        print(f"Registered external system: {system_id}")
        
    def connectAllSystems(self):
        """Connect to all registered external systems."""
        for system_id, system in self.external_systems.items():
            try:
                if system.connect():
                    print(f"Successfully connected to {system_id}")
                else:
                    print(f"Failed to connect to {system_id}")
            except Exception as e:
                print(f"Error connecting to {system_id}: {e}")
                
    def disconnectAllSystems(self):
        """Disconnect from all external systems."""
        for system_id, system in self.external_systems.items():
            try:
                if system.disconnect():
                    print(f"Successfully disconnected from {system_id}")
            except Exception as e:
                print(f"Error disconnecting from {system_id}: {e}")
                
    def startMonitoring(self):
        """Start system monitoring."""
        self.is_running = True
        self.system_monitor_thread = threading.Thread(
            target=self.monitorSystems,
            daemon=True
        )
        self.system_monitor_thread.start()
        print("Started system monitoring")
        
    def stopMonitoring(self):
        """Stop system monitoring."""
        self.is_running = False
        if self.system_monitor_thread:
            self.system_monitor_thread.join(timeout=1.0)
        print("Stopped system monitoring")
        
    def monitorSystems(self):
        """Monitor external systems."""
        while self.is_running:
            self.checkSystemStatus()
            time.sleep(1.0)
            
    def checkSystemStatus(self):
        """Check status of all external systems."""
        for system_id, system in self.external_systems.items():
            if system.isConnected():
                try:
                    status = system.getSystemStatus()
                    # Log status
                    # In a real implementation, this would log to monitoring system
                except Exception as e:
                    print(f"Error checking status of {system_id}: {e}")
                    
    def sendDataToSystem(self, system_id: str, data: Dict[str, Any]):
        """
        Send data to external system.
        
        Args:
            system_id: Target system identifier
            data: Data to send
        """
        if system_id not in self.external_systems:
            raise ValueError(f"External system {system_id} not registered")
            
        system = self.external_systems[system_id]
        if not system.isConnected():
            raise RuntimeError(f"External system {system_id} not connected")
            
        # Serialize and send data
        message = self.communication_protocol.createMessage('data_transfer', data)
        serialized = self.communication_protocol.serializeMessage(message)
        
        # In a real implementation, this would send data over network
        print(f"Sent data to external system {system_id}")
        
    def receiveDataFromSystem(self, system_id: str) -> Optional[Dict[str, Any]]:
        """
        Receive data from external system.
        
        Args:
            system_id: Source system identifier
            
        Returns:
            Received data
        """
        if system_id not in self.external_systems:
            raise ValueError(f"External system {system_id} not registered")
            
        system = self.external_systems[system_id]
        if not system.isConnected():
            return None
            
        # In a real implementation, this would receive data from network
        # For now, return a simulated data response
        return {
            'timestamp': time.time(),
            'source': system_id,
            'data': {'sensor_reading': np.random.random()},
            'message_type': 'sensor_data'
        }
class IntegratedNEUSystem:
    """
    Advanced NLM system with external integrations.
    
    This system demonstrates:
    - Multiple external system integration
    - Data flow coordination
    - Real-time communication
    - Error handling and recovery
    - Performance monitoring
    """
    
    def __init__(self, brain: pynlm.Brain, world: pynlm.SimpleWorld):
        self.brain = brain
        self.world = world
        
        # Initialize external system manager
        self.system_manager = ExternalSystemManager()
        
        # Initialize sensor system
        self.sensor_system = ExternalSensorSystem("main_sensor")
        self.system_manager.registerExternalSystem("main_sensor", self.sensor_system)
        
        # Initialize actuator system
        self.actuator_system = ExternalActuatorSystem("main_actuator")
        self.system_manager.registerExternalSystem("main_actuator", self.actuator_system)
        
        # Control functions
        self.sensor_function = self.getIntegratedSensorData
        self.control_function = self.getIntegratedControl
        self.actuator_function = self.applyIntegratedActuator
        
    def connectAllSystems(self):
        """Connect all external systems."""
        self.system_manager.connectAllSystems()
        
    def startMonitoring(self):
        """Start system monitoring."""
        self.system_manager.startMonitoring()
        
    def executeIntegratedControl(self):
        """Execute integrated control loop."""
        print("=== Integrated Control Loop ===")
        
        # Get sensor data
        sensor_data = self.getIntegratedSensorData()
        if sensor_data is None:
            print("No sensor data available")
            return
            
        print(f"Sensor data received: {sensor_data}")
        
        # Apply control
        action = self.getIntegratedControl(sensor_data, time.time())
        print(f"Generated control action: {action.getType() if action else None}")
        
        # Apply actuator
        self.applyIntegratedActuator(action, time.time())
        
        print("Integrated control loop completed\n")
        
    def getIntegratedSensorData(self) -> Optional[pynlm.SensorPercept]:
        """Get integrated sensor data."""
        try:
            # Try to get data from external sensor system
            if self.sensor_system.isConnected():
                sensor_data = self.sensor_system.getSensorData()
                if sensor_data is not None:
                    return sensor_data
                    
            # Fallback to internal world data
            return self.world.getSensoryPercept()
            
        except Exception as e:
            print(f"Error getting sensor data: {e}")
            return self.world.getSensoryPercept()
        
    def getIntegratedControl(self, percept: pynlm.SensorPercept, current_time: float) -> pynlm.Action:
        """
        Get integrated control output.
        
        Args:
            percept: Sensory percept
            current_time: Current time
            
        Returns:
            Control action
        """
        # Apply NLM control
        self.brain.processSensoryInput(percept)
        self.brain.step(0)
        action = self.brain.produceAction()
        
        # Send action to external actuator system
        if self.actuator_system.isConnected():
            try:
                self.actuator_system.executeCommand(action)
            except Exception as e:
                print(f"Error executing actuator command: {e}")
                
        return action
        
    def applyIntegratedActuator(self, action: pynlm.Action, current_time: float):
        """
        Apply integrated actuator command.
        
        Args:
            action: Action to apply
            current_time: Current time
        """
        try:
            # Apply action to world
            self.world.applyMotorCommand(action, current_time)
            
            # Send data to external systems
            if self.sensor_system.isConnected():
                self.system_manager.sendDataToSystem(
                    "main_sensor",
                    {
                        'action_executed': str(action.getType()) if action else None,
                        'timestamp': current_time
                    }
                )
                
        except Exception as e:
            print(f"Error applying actuator command: {e}")
            
    def runIntegratedSimulation(self, duration: float, time_step: float = 0.01):
        """
        Run integrated simulation.
        
        Args:
            duration: Simulation duration
            time_step: Time step size
        """
        print(f"Running integrated simulation for {duration}s...")
        print(f"Time step: {time_step}s")
        
        # Connect and start systems
        self.connectAllSystems()
        self.startMonitoring()
        
        steps = int(duration / time_step)
        
        for step in range(steps):
            print(f"\n--- Step {step} ---")
            
            # Execute integrated control
            self.executeIntegratedControl()
            
            # Log progress periodically
            if step % 50 == 0:
                self.logProgress(step)
                
        # Stop monitoring
        self.system_manager.stopMonitoring()
        
        print("\n=== Integrated Simulation Complete ===")
        
    def logProgress(self, step: int):
        """Log simulation progress."""
        status = self.system_manager.getSystemStatus("main_sensor")
        if status:
            print(f"System status - Sensor connected: {status['is_connected']}")
            if status['last_command']:
                print(f"Last command: {status['last_command']['command']}")
                
        brain_status = {
            'neurons': self.brain.getTotalNeuronCount(),
            'spikes': self.brain.getTotalSpikeCount(),
            'firing': self.brain.getFiringNeuronCount()
        }
        print(f"Brain status: {brain_status}")
        
def runAdvancedIntegrationExample():
    """Run the advanced integration example."""
    print("=== Advanced Integration Example ===\n")
    
    # Create brain and world
    config = pynlm.createDemoConfig(1500, 0.1)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # Create integrated NLM system
    integrated_system = IntegratedNEUSystem(brain, world)
    
    print("Integrated NLM system initialized with:")
    print("- External sensor system integration")
    print("- External actuator system integration")
    print("- Real-time communication protocols")
    print("- System monitoring")
    print("- Error handling and recovery")
    print()
    
    # Run integrated simulation
    integrated_system.runIntegratedSimulation(duration=10.0, time_step=0.1)
    
    print("\n=== Advanced Integration Complete ===")
if __name__ == "__main__":
    runAdvancedIntegrationExample()