# NLM Advanced Tutorials Guide

This document provides comprehensive step-by-step tutorials for advanced NLM usage, covering complex topics like adaptive robot control, cognitive development simulation, and neuromodulation-based curiosity.

## Table of Contents

### Tutorial 1: Building an Adaptive Robot Controller
1.1 Learning Objectives
1.2 Prerequisites
1.3 Step 1: Setup and Configuration
1.4 Step 2: Sensor Integration
1.5 Step 3: Motor Control Implementation
1.6 Step 4: Adaptive Learning
1.7 Step 5: Testing and Validation
1.8 Tutorial Summary

### Tutorial 2: Cognitive Development Simulation
2.1 Learning Objectives
2.2 Prerequisites
2.3 Step 1: Brain Configuration
2.4 Step 2: Developmental Stages
2.5 Step 3: Learning Mechanisms
2.6 Step 4: Environment Interaction
2.7 Step 5: Development Tracking
2.8 Tutorial Summary

### Tutorial 3: Memory-Enhanced Learning Systems
3.1 Learning Objectives
3.2 Prerequisites
3.3 Step 1: Memory System Setup
3.4 Step 2: Working Memory Implementation
3.5 Step 3: Episodic Memory Integration
3.6 Step 4: Associative Memory Building
3.7 Step 5: Memory-Enhanced Learning
3.8 Tutorial Summary

### Tutorial 4: Neuromodulation-Based Curiosity
4.1 Learning Objectives
4.2 Prerequisites
4.3 Step 1: Neuromodulation Setup
4.4 Step 2: Curiosity System Configuration
4.5 Step 3: Novelty Detection
4.6 Step 4: Exploration Behavior
4.7 Step 5: Curiosity-Driven Learning
4.8 Tutorial Summary

### Tutorial 5: Multi-Agent Coordination
5.1 Learning Objectives
5.2 Prerequisites
5.3 Step 1: Multi-Agent Setup
5.4 Step 2: Communication Protocols
5.5 Step 3: Coordination Mechanisms
5.6 Step 4: Shared Resource Management
5.7 Step 5: Collective Learning
5.8 Tutorial Summary

### Tutorial 6: Predictive Coding and Expectation Management
6.1 Learning Objectives
6.2 Prerequisites
6.3 Step 1: Prediction System Setup
6.4 Step 2: Expectation Formation
6.5 Step 3: Prediction Error Processing
6.6 Step 4: Expectation Updating
6.7 Step 5: Adaptive Predictions
6.8 Tutorial Summary

### Tutorial 7: Structural Plasticity and Brain Growth
7.1 Learning Objectives
7.2 Prerequisites
7.3 Step 1: Plasticity System Configuration
7.4 Step 2: Synaptogenesis Implementation
7.5 Step 3: Pruning Mechanisms
7.6 Step 4: Brain Growth Dynamics
7.7 Step 5: Activity-Dependent Plasticity
7.8 Tutorial Summary

### Tutorial 8: Social Learning and Imitation
8.1 Learning Objectives
8.2 Prerequisites
8.3 Step 1: Social System Setup
8.4 Step 2: Observation Mechanisms
8.5 Step 3: Imitation Learning
8.6 Step 4: Social Coordination
8.7 Step 5: Cultural Transmission
8.8 Tutorial Summary

---

## Tutorial 1: Building an Adaptive Robot Controller

### Learning Objectives
- Understand how to build a robot controller using NLM
- Learn sensor-motor integration techniques
- Implement adaptive learning for changing environments
- Debug and optimize robot control systems
- Apply NLM concepts to real-world robotics

### Prerequisites
- Basic understanding of NLM concepts (brain, world, agent)
- Familiarity with Python programming
- Understanding of sensor-motor loops
- Experience with neural simulation frameworks (recommended but not required)

### Step 1: Setup and Configuration

#### Install Required Dependencies
```python
# Install NLM
pip install -e .

# Install additional dependencies for robotics
pip install numpy scipy matplotlib opencv-python
```

#### Create Project Structure
```python
# Create project directory
mkdir adaptive_robot_controller
cd adaptive_robot_controller

# Create necessary files
mkdir -p src/config src/sensors src/actuators src/learning

# Create main controller script
nano main_controller.py
```

#### Basic Robot Controller Configuration
```python
# src/config/robot_config.py
import pynlm
import numpy as np

class RobotControllerConfig:
    """Configuration for adaptive robot controller."""
    
    def __init__(self):
        # Brain configuration
        self.brain_config = pynlm.createDefaultConfig()
        self.brain_config.set("brain.neuron_count", 3000)
        self.brain_config.set("brain.synapse_density", 0.06)
        self.brain_config.set("brain.v_thresh", -40.0)
        self.brain_config.set("brain.tau_mem", 10.0)
        
        # Sensor configuration
        self.sensor_config = {
            'camera': {
                'width': 64,
                'height': 64,
                'fps': 30,
                'fov': 90
            },
            'lidar': {
                'range': 10.0,
                'resolution': 0.1,
                'channels': 360
            },
            'proximity': {
                'range': 2.0,
                'resolution': 0.05
            }
        }
        
        # Actuator configuration
        self.actuator_config = {
            'motors': {
                'wheel_velocity': 2.0,  # m/s
                'turning_speed': 0.5,   # rad/s
                'max_acceleration': 1.0
            },
            'gripper': {
                'opening_speed': 1.0,
                'closing_speed': 1.0,
                'max_force': 50.0
            }
        }
        
        # Environment configuration
        self.environment_config = {
            'world_width': 20.0,
            'world_height': 20.0,
            'obstacle_density': 0.3,
            'target_objects': ['red_cube', 'blue_sphere', 'green_box']
        }
        
        # Learning configuration
        self.learning_config = {
            'learning_rate': 0.01,
            'exploration_rate': 0.1,
            'reward_discount': 0.95,
            'learning_episodes': 1000,
            'convergence_threshold': 0.01
        }
    
    def get_pynlm_config(self):
        """Get NLM brain configuration."""
        return self.brain_config
    
    def get_sensor_config(self, sensor_type):
        """Get configuration for specific sensor."""
        return self.sensor_config.get(sensor_type, {})
    
    def get_actuator_config(self, actuator_type):
        """Get configuration for specific actuator."""
        return self.actuator_config.get(actuator_type, {})

# Initialize configuration
config = RobotControllerConfig()
```

### Step 2: Sensor Integration

#### Camera Sensor Implementation
```python
# src/sensors/camera_sensor.py
import pynlm
import numpy as np
import cv2

class CameraSensor:
    """Camera sensor with NLM integration."""
    
    def __init__(self, width=64, height=64, fov=90):
        self.width = width
        self.height = height
        self.fov = fov
        self.frame_count = 0
        self.calibration_data = {}
        
    def capture_frame(self):
        """Capture image frame (simulated for NLM)."""
        # Simulate camera capture with noise and distortion
        frame = np.random.normal(0, 0.1, (self.height, self.width, 3))
        
        # Add realistic camera effects
        frame = self.apply_camera_effects(frame)
        
        # Convert to NLM vision format
        vision_data = self.convert_to_vision_format(frame)
        
        return vision_data
    
    def apply_camera_effects(self, frame):
        """Apply realistic camera effects."""
        # Apply noise
        noise = np.random.normal(0, 0.02, frame.shape)
        frame_noisy = frame + noise
        
        # Apply blur (simulating motion or focus)
        blur_amount = np.random.uniform(0, 0.5)
        if blur_amount > 0:
            frame_noisy = cv2.GaussianBlur(frame_noisy, (3, 3), blur_amount)
        
        # Apply exposure variation
        exposure = np.random.uniform(0.8, 1.2)
        frame_noisy = np.clip(frame_noisy * exposure, 0, 1)
        
        return frame_noisy
    
    def convert_to_vision_format(self, frame):
        """Convert camera frame to NLM vision format."""
        vision = pynlm.Vision(self.width, self.height, 3)
        vision.setData(frame.flatten())
        
        # Add intrinsic camera parameters
        vision.set_intrinsic_parameters(
            self.width, self.height, self.fov
        )
        
        return vision
    
    def calibrate(self, calibration_images):
        """Calibrate camera sensor."""
        # Calibrate for distortion, exposure, focus
        self.calibration_data = {
            'distortion_coefficients': np.random.random(5) * 0.1,
            'gain': np.random.uniform(0.8, 1.2),
            'offset': np.random.uniform(-0.1, 0.1)
        }
        
        return True

# Lidar Sensor Implementation
class LidarSensor:
    """Lidar sensor with NLM integration."""
    
    def __init__(self, range=10.0, resolution=0.1, channels=360):
        self.range = range
        self.resolution = resolution
        self.channels = channels
        self.points_per_scan = int(channels * range / resolution)
        
    def scan(self, robot_position):
        """Perform lidar scan."""
        # Simulate lidar scan
        scan_data = self.generate_scan_data(robot_position)
        
        # Convert to NLM touch format
        touch_data = self.convert_to_touch_format(scan_data)
        
        return touch_data
    
    def generate_scan_data(self, robot_position):
        """Generate simulated lidar scan data."""
        scan = []
        
        for angle in np.linspace(0, 2*np.pi, self.channels, endpoint=False):
            # Simulate distance measurement
            distance = self.measure_distance(angle, robot_position)
            
            # Add noise and outliers
            distance += np.random.normal(0, 0.05)
            distance = max(0.1, min(self.range, distance))
            
            scan.append(distance)
        
        return np.array(scan)
    
    def measure_distance(self, angle, robot_position):
        """Measure distance to nearest obstacle."""
        # Simulate distance to obstacles
        obstacle_distance = np.random.uniform(1.0, 5.0)
        
        # Add some objects with different distances
        if np.random.random() < 0.3:
            obstacle_distance = np.random.uniform(0.5, 3.0)
        
        return obstacle_distance
    
    def convert_to_touch_format(self, scan_data):
        """Convert lidar data to NLM touch format."""
        # Convert polar coordinates to Cartesian for touch sensor
        touch_data = []
        
        for i, distance in enumerate(scan_data):
            if distance < self.range:
                angle = i * self.resolution
                x = distance * np.cos(angle)
                y = distance * np.sin(angle)
                touch_data.extend([x, y, distance])
        
        return np.array(touch_data)

# Proximity Sensor Implementation
class ProximitySensor:
    """Proximity sensor with NLM integration."""
    
    def __init__(self, range=2.0, resolution=0.05):
        self.range = range
        self.resolution = resolution
        
    def detect_proximity(self, robot_position):
        """Detect nearby objects."""
        # Get nearby objects
        nearby_objects = self.find_nearby_objects(robot_position)
        
        # Convert to NLM internal signals
        internal_signals = self.convert_to_internal_format(nearby_objects)
        
        return internal_signals
    
    def find_nearby_objects(self, robot_position):
        """Find objects within proximity sensor range."""
        objects = []
        
        # Simulate nearby objects
        for i in range(5):
            obj_position = np.random.uniform(-self.range, self.range, 2)
            distance = np.linalg.norm(obj_position)
            
            if distance <= self.range and distance > 0.1:
                objects.append({
                    'position': obj_position,
                    'distance': distance,
                    'type': self.random_object_type(),
                    'confidence': self.calculate_detection_confidence(distance)
                })
        
        return objects
    
    def convert_to_internal_format(self, objects):
        """Convert proximity data to NLM internal signals."""
        # Convert to NLM internal signals format
        signals = []
        
        for obj in objects:
            # Encode object information
            signals.extend([
                obj['distance'],  # Distance
                obj['confidence'],  # Detection confidence
                0.0 if obj['type'] == 'static' else 1.0,  # Movable flag
                self.object_to_index(obj['type'])  # Object type
            ])
        
        return np.array(signals)
    
    def calculate_detection_confidence(self, distance):
        """Calculate detection confidence based on distance."""
        # Higher confidence for closer objects
        confidence = max(0.1, 1.0 - distance / self.range)
        
        # Add noise
        confidence += np.random.normal(0, 0.05)
        
        return np.clip(confidence, 0.0, 1.0)

# Integration with NLM
class SensorIntegration:
    """Integrate sensors with NLM brain."""
    
    def __init__(self, camera, lidar, proximity):
        self.camera = camera
        self.lidar = lidar
        self.proximity = proximity
        self.sensor_fusion = SensorFusion()
        
    def capture_all_sensors(self, robot_state):
        """Capture all sensor data."""
        # Capture individual sensor data
        camera_data = self.camera.capture_frame()
        lidar_data = self.lidar.scan(robot_state['position'])
        proximity_data = self.proximity.detect_proximity(robot_state['position'])
        
        # Fuse sensor data
        fused_percept = self.sensor_fusion.fuse(
            camera_data, lidar_data, proximity_data
        )
        
        return fused_percept
    
    def apply_sensor_processing(self, percept):
        """Apply sensor-specific processing."""
        # Extract features from each sensor modality
        vision_features = self.extract_vision_features(percept.get_vision())
        touch_features = self.extract_touch_features(percept.get_touch())
        internal_features = self.extract_internal_features(percept.get_internal())
        
        # Combine features
        processed_features = self.combine_features(
            vision_features, touch_features, internal_features
        )
        
        return processed_features
    
    def extract_vision_features(self, vision_data):
        """Extract features from vision data."""
        # Placeholder for vision feature extraction
        if vision_data is None:
            return {}
        
        # Extract basic features
        features = {
            'brightness': np.mean(vision_data),
            'contrast': np.std(vision_data),
            'edges': self.count_edges(vision_data),
            'colors': self.extract_colors(vision_data)
        }
        
        return features
    
    def extract_touch_features(self, touch_data):
        """Extract features from touch data."""
        # Placeholder for touch feature extraction
        if touch_data is None:
            return {}
        
        features = {
            'min_distance': np.min(touch_data) if len(touch_data) > 0 else float('inf'),
            'max_distance': np.max(touch_data) if len(touch_data) > 0 else 0,
            'avg_distance': np.mean(touch_data) if len(touch_data) > 0 else 0,
            'object_count': len(touch_data) // 3  # Assuming 3 values per object
        }
        
        return features
    
    def extract_internal_features(self, internal_data):
        """Extract features from internal signals."""
        # Placeholder for internal signal feature extraction
        if internal_data is None:
            return {}
        
        features = {
            'signal_strength': np.mean(internal_data),
            'signal_variance': np.var(internal_data),
            'num_signals': len(internal_data)
        }
        
        return features
    
    def combine_features(self, vision_features, touch_features, internal_features):
        """Combine features from all sensors."""
        combined = {}
        
        # Add features from all sensors
        combined.update(vision_features)
        combined.update(touch_features)
        combined.update(internal_features)
        
        # Add sensor reliability indicators
        combined['sensor_reliability'] = self.calculate_sensor_reliability(
            vision_features, touch_features, internal_features
        )
        
        return combined

class SensorFusion:
    """Sensor data fusion for robust perception."""
    
    def __init__(self):
        self.fusion_weights = {
            'camera': 0.4,
            'lidar': 0.4,
            'proximity': 0.2
        }
        
    def fuse(self, camera_data, lidar_data, proximity_data):
        """Fuse data from all sensors."""
        # Create NLM sensory percept
        percept = pynlm.SensoryPercept()
        
        # Set vision data
        if camera_data is not None:
            # Convert camera data to vision format
            vision = pynlm.Vision(64, 64, 3)
            vision.setData(np.random.random(64 * 64 * 3))  # Placeholder
            percept.setVision(vision)
        
        # Set touch data
        if lidar_data is not None or proximity_data is not None:
            # Combine touch data
            combined_touch = []
            if lidar_data is not None:
                combined_touch.extend(lidar_data)
            if proximity_data is not None:
                combined_touch.extend(proximity_data)
            
            touch = np.array(combined_touch)
            percept.setTouch(touch)
        
        # Set internal signals
        if proximity_data is not None:
            # Use proximity data as internal signals
            percept.setInternal(proximity_data)
        
        return percept
    
    def calculate_sensor_reliability(self, vision_features, touch_features, internal_features):
        """Calculate overall sensor reliability."""
        # Calculate reliability based on feature consistency
        reliability = 1.0
        
        # Vision reliability (based on contrast and edge detection)
        if 'contrast' in vision_features:
            vision_reliability = min(1.0, vision_features['contrast'] * 2)
            reliability *= vision_reliability
        
        # Touch reliability (based on distance measurements)
        if 'min_distance' in touch_features:
            touch_reliability = 1.0 - min(1.0, touch_features['min_distance'] / 10.0)
            reliability *= touch_reliability
        
        # Internal signal reliability (based on signal variance)
        if 'signal_variance' in internal_features:
            internal_reliability = min(1.0, internal_features['signal_variance'] * 10)
            reliability *= internal_reliability
        
        return reliability

# Object type utilities
def random_object_type():
    """Get random object type."""
    types = ['static', 'movable', 'hazard', 'target', 'resource']
    return np.random.choice(types)

def object_to_index(object_type):
    """Convert object type to index."""
    type_to_index = {
        'static': 0,
        'movable': 1,
        'hazard': 2,
        'target': 3,
        'resource': 4
    }
    return type_to_index.get(object_type, 0)

def count_edges(vision_data):
    """Count edges in vision data."""
    # Placeholder for edge detection
    return np.random.randint(100, 1000)

def extract_colors(vision_data):
    """Extract colors from vision data."""
    # Placeholder for color extraction
    return {'red': 0.3, 'green': 0.3, 'blue': 0.3, 'yellow': 0.1}
```

### Step 3: Motor Control Implementation

#### Motor Control System
```python
# src/actuators/motor_controller.py
import pynlm
import numpy as np
import math

class MotorController:
    """Motor controller for NLM robot."""
    
    def __init__(self, config):
        self.config = config
        self.motor_states = {}
        self.control_history = []
        self.adaptation_factor = 1.0
        
        # Initialize motor systems
        self.initialize_motors()
        
    def initialize_motors(self):
        """Initialize motor control systems."""
        # Wheel motors
        self.motor_states['wheel_left'] = {
            'position': 0.0,
            'velocity': 0.0,
            'acceleration': 0.0,
            'force': 0.0,
            'max_velocity': self.config.get_actuator_config('motors')['wheel_velocity'],
            'max_force': self.config.get_actuator_config('motors')['max_acceleration'] * 10
        }
        
        self.motor_states['wheel_right'] = {
            'position': 0.0,
            'velocity': 0.0,
            'acceleration': 0.0,
            'force': 0.0,
            'max_velocity': self.config.get_actuator_config('motors')['wheel_velocity'],
            'max_force': self.config.get_actuator_config('motors')['max_acceleration'] * 10
        }
        
        # Gripper motor
        self.motor_states['gripper'] = {
            'position': 0.0,
            'velocity': 0.0,
            'acceleration': 0.0,
            'force': 0.0,
            'max_velocity': self.config.get_actuator_config('gripper')['opening_speed'],
            'max_force': self.config.get_actuator_config('gripper')['max_force']
        }
        
    def process_action(self, action, timestep):
        """Process action and update motor states."""
        if action is None:
            return self.get_current_action()
        
        # Decode action into motor commands
        motor_commands = self.decode_action_to_motor(action)
        
        # Apply motor commands
        self.apply_motor_commands(motor_commands, timestep)
        
        # Update motor dynamics
        self.update_motor_dynamics(timestep)
        
        # Return current action
        return self.get_current_action()
    
    def decode_action_to_motor(self, action):
        """Decode action to motor commands."""
        # Create motor command based on action
        motor_commands = {}
        
        if action == 'MoveForward':
            motor_commands['wheel_left'] = {'velocity': self.config.get_actuator_config('motors')['wheel_velocity']}
            motor_commands['wheel_right'] = {'velocity': self.config.get_actuator_config('motors')['wheel_velocity']}
            
        elif action == 'MoveBackward':
            motor_commands['wheel_left'] = {'velocity': -self.config.get_actuator_config('motors')['wheel_velocity']}
            motor_commands['wheel_right'] = {'velocity': -self.config.get_actuator_config('motors')['wheel_velocity']}
            
        elif action == 'TurnLeft':
            motor_commands['wheel_left'] = {'velocity': -self.config.get_actuator_config('motors')['wheel_velocity'] * 0.5}
            motor_commands['wheel_right'] = {'velocity': self.config.get_actuator_config('motors')['wheel_velocity'] * 0.5}
            
        elif action == 'TurnRight':
            motor_commands['wheel_left'] = {'velocity': self.config.get_actuator_config('motors')['wheel_velocity'] * 0.5}
            motor_commands['wheel_right'] = {'velocity': -self.config.get_actuator_config('motors')['wheel_velocity'] * 0.5}
            
        elif action == 'Look':
            motor_commands['gripper'] = {'position': 1.0}  # Open gripper
            
        elif action == 'Interact':
            motor_commands['gripper'] = {'position': 0.5}  # Partially open
            
        elif action == 'Rest':
            motor_commands['wheel_left'] = {'velocity': 0.0}
            motor_commands['wheel_right'] = {'velocity': 0.0}
            motor_commands['gripper'] = {'position': 0.0}  # Close gripper
        
        else:
            # Custom action (e.g., from neural output)
            motor_commands = self.process_custom_action(action)
        
        return motor_commands
    
    def process_custom_action(self, action):
        """Process custom action from neural output."""
        # Process custom action parameters
        if hasattr(action, 'getType'):
            action_type = action.getType()
            parameters = action.getParameters() if hasattr(action, 'getParameters') else []
            
            # Map action type to motor commands
            if action_type == 'MoveForward':
                return {'wheel_left': {'velocity': parameters[0] if len(parameters) > 0 else 1.0},
                       'wheel_right': {'velocity': parameters[0] if len(parameters) > 0 else 1.0}}
            
            elif action_type == 'MoveBackward':
                return {'wheel_left': {'velocity': -(parameters[0] if len(parameters) > 0 else 1.0)},
                       'wheel_right': {'velocity': -(parameters[0] if len(parameters) > 0 else 1.0)}}
            
            elif action_type == 'TurnLeft':
                return {'wheel_left': {'velocity': -(parameters[0] if len(parameters) > 0 else 0.5)},
                       'wheel_right': {'velocity': parameters[0] if len(parameters) > 0 else 0.5}}
            
            elif action_type == 'TurnRight':
                return {'wheel_left': {'velocity': parameters[0] if len(parameters) > 0 else 0.5},
                       'wheel_right': {'velocity': -(parameters[0] if len(parameters) > 0 else 0.5)}}
        
        # Default empty motor commands
        return {}
    
    def apply_motor_commands(self, motor_commands, timestep):
        """Apply motor commands to update motor states."""
        for motor_name, command in motor_commands.items():
            if motor_name in self.motor_states:
                motor = self.motor_states[motor_name]
                
                # Update velocity based on command
                if 'velocity' in command:
                    target_velocity = command['velocity']
                    # Apply acceleration limits
                    velocity_change = min(
                        abs(target_velocity - motor['velocity']),
                        self.config.get_actuator_config('motors')['max_acceleration'] * timestep
                    )
                    
                    if target_velocity >= motor['velocity']:
                        motor['velocity'] += velocity_change
                    else:
                        motor['velocity'] -= velocity_change
                
                # Update position
                motor['position'] += motor['velocity'] * timestep
                
                # Update acceleration
                motor['acceleration'] = (motor['velocity'] - motor.get('previous_velocity', 0)) / timestep
                motor['previous_velocity'] = motor['velocity']
                
                # Calculate force
                motor['force'] = motor['acceleration'] * motor['max_force'] / self.config.get_actuator_config('motors')['max_acceleration']
    
    def update_motor_dynamics(self, timestep):
        """Update motor dynamics with physics simulation."""
        # Apply damping/friction
        for motor_name, motor in self.motor_states.items():
            # Velocity damping
            damping_coefficient = 0.95
            motor['velocity'] *= damping_coefficient
            
            # Force damping
            motor['force'] *= damping_coefficient
            
            # Position bounds
            motor['position'] = np.clip(motor['position'], 0, 1)
            
            # Velocity bounds
            max_vel = motor['max_velocity']
            motor['velocity'] = np.clip(motor['velocity'], -max_vel, max_vel)
    
    def get_current_action(self):
        """Get current motor action for NLM."""
        action = pynlm.Action(pynlm.ActionType.Custom)
        
        # Encode current motor state into action
        action_params = []
        
        # Add wheel positions/velocities
        action_params.extend([
            self.motor_states['wheel_left']['position'],
            self.motor_states['wheel_right']['position'],
            self.motor_states['wheel_left']['velocity'],
            self.motor_states['wheel_right']['velocity'],
            self.motor_states['gripper']['position'],
            self.motor_states['gripper']['velocity']
        ])
        
        action.setParameters(action_params)
        return action
    
    def adapt_to_environment(self, environment_state, timestep):
        """Adapt motor controller to environment changes."""
        # Adjust parameters based on environment
        if environment_state['obstacle_detected']:
            # Slow down when obstacles detected
            self.adaptation_factor *= 0.9
            
            # Reduce max velocity
            for motor_name, motor in self.motor_states.items():
                if 'wheel' in motor_name:
                    motor['max_velocity'] *= 0.8
        
        elif environment_state['clear_path']:
            # Speed up when path is clear
            self.adaptation_factor *= 1.1
            
            # Restore max velocity
            for motor_name, motor in self.motor_states.items():
                if 'wheel' in motor_name:
                    motor['max_velocity'] = self.config.get_actuator_config('motors')['wheel_velocity']
        
        # Record adaptation
        self.control_history.append({
            'step': timestep,
            'adaptation_factor': self.adaptation_factor,
            'obstacle_detected': environment_state['obstacle_detected'],
            'clear_path': environment_state['clear_path']
        })
    
    def get_control_performance(self):
        """Get motor control performance metrics."""
        if len(self.control_history) == 0:
            return {
                'position_accuracy': 0.0,
                'velocity_stability': 0.0,
                'adaptation_efficiency': 0.0,
                'response_time': 0.0
            }
        
        # Calculate performance metrics
        recent_history = self.control_history[-100:]  # Last 100 steps
        
        # Position accuracy (should be within bounds)
        position_accuracy = np.mean([1.0 - abs(m['position']) for m in self.motor_states.values()])
        
        # Velocity stability (low variance)
        velocity_variances = []
        for motor in self.motor_states.values():
            if 'velocities' in motor:
                velocity_variances.append(np.var(motor['velocities']))
        velocity_stability = 1.0 / (1.0 + np.mean(velocity_variances) if velocity_variances else 1.0)
        
        # Adaptation efficiency (change in adaptation factor)
        if len(recent_history) >= 2:
            adaptation_changes = [abs(h['adaptation_factor'] - recent_history[i]['adaptation_factor']) 
                                for i in range(1, len(recent_history))]
            adaptation_efficiency = 1.0 / (1.0 + np.mean(adaptation_changes))
        else:
            adaptation_efficiency = 1.0
        
        return {
            'position_accuracy': position_accuracy,
            'velocity_stability': velocity_stability,
            'adaptation_efficiency': adaptation_efficiency,
            'response_time': len(recent_history) / 10.0  # Arbitrary metric
        }

# Environment Simulator
class EnvironmentSimulator:
    """Environment simulator for robot testing."""
    
    def __init__(self, config):
        self.config = config
        self.robot_position = [10.0, 10.0]
        self.robot_orientation = 0.0
        self.obstacles = []
        self.targets = []
        self.world_objects = []
        
        # Initialize environment
        self.initialize_environment()
        
    def initialize_environment(self):
        """Initialize environment with obstacles and targets."""
        width = self.config.environment_config['world_width']
        height = self.config.environment_config['world_height']
        
        # Generate random obstacles
        num_obstacles = int(width * height * self.config.environment_config['obstacle_density'])
        for i in range(num_obstacles):
            x = np.random.uniform(1, width - 1)
            y = np.random.uniform(1, height - 1)
            
            # Ensure obstacle doesn't overlap with robot start position
            if np.linalg.norm([x - self.robot_position[0], y - self.robot_position[1]]) > 2.0:
                obstacle = {
                    'position': [x, y],
                    'size': np.random.uniform(0.5, 1.5),
                    'type': 'static',
                    'collision_detection': True
                }
                self.obstacles.append(obstacle)
        
        # Place target objects
        for target_type in self.config.environment_config['target_objects']:
            # Find suitable position for target
            for attempt in range(100):
                x = np.random.uniform(1, width - 1)
                y = np.random.uniform(1, height - 1)
                
                # Check if position is suitable
                if (np.linalg.norm([x - self.robot_position[0], y - self.robot_position[1]]) > 3.0 and
                    not self.is_collision([x, y], self.obstacles)):
                    
                    target = {
                        'position': [x, y],
                        'type': target_type,
                        'value': np.random.uniform(10, 100),
                        'available': True
                    }
                    self.targets.append(target)
                    break
    
    def is_collision(self, point, obstacles):
        """Check if point collides with any obstacle."""
        for obstacle in obstacles:
            distance = np.linalg.norm(
                [point[0] - obstacle['position'][0], 
                 point[1] - obstacle['position'][1]]
            )
            if distance < obstacle['size'] + 0.5:  # 0.5 is robot radius
                return True
        return False
    
    def update(self, timestep):
        """Update environment state."""
        # Update targets (e.g., move them)
        for target in self.targets:
            if target['available']:
                # Simple target movement
                target['position'][0] += np.random.uniform(-0.1, 0.1) * timestep
                target['position'][1] += np.random.uniform(-0.1, 0.1) * timestep
                
                # Keep target within bounds
                target['position'][0] = np.clip(target['position'][0], 1, 19)
                target['position'][1] = np.clip(target['position'][1], 1, 19)
                
                # Check if target reached its destination
                if np.random.random() < 0.01:
                    target['available'] = False
    
    def get_environment_state(self, robot_position):
        """Get environment state from robot perspective."""
        state = {
            'robot_position': robot_position,
            'robot_orientation': self.robot_orientation,
            'obstacle_detected': False,
            'clear_path': True,
            'nearby_targets': [],
            'target_values': []
        }
        
        # Check for nearby obstacles
        for obstacle in self.obstacles:
            distance = np.linalg.norm(
                [robot_position[0] - obstacle['position'][0],
                 robot_position[1] - obstacle['position'][1]]
            )
            
            if distance < 3.0:
                state['obstacle_detected'] = True
                state['clear_path'] = False
        
        # Find nearby targets
        for target in self.targets:
            if target['available']:
                distance = np.linalg.norm(
                    [robot_position[0] - target['position'][0],
                     robot_position[1] - target['position'][1]]
                )
                
                if distance < 5.0:
                    state['nearby_targets'].append(target['type'])
                    state['target_values'].append(target['value'])
        
        return state
    
    def apply_action(self, action, timestep):
        """Apply action to environment."""
        if action is None:
            return
        
        # Get action parameters
        action_type = action.getType() if hasattr(action, 'getType') else None
        parameters = action.getParameters() if hasattr(action, 'getParameters') else []
        
        # Update robot state based on action
        if action_type == 'MoveForward':
            speed = parameters[0] if len(parameters) > 0 else 1.0
            angle = self.robot_orientation
            self.robot_position[0] += speed * np.cos(angle) * timestep
            self.robot_position[1] += speed * np.sin(angle) * timestep
            
        elif action_type == 'MoveBackward':
            speed = parameters[0] if len(parameters) > 0 else 1.0
            angle = self.robot_orientation
            self.robot_position[0] -= speed * np.cos(angle) * timestep
            self.robot_position[1] -= speed * np.sin(angle) * timestep
            
        elif action_type == 'TurnLeft':
            turn_speed = parameters[0] if len(parameters) > 0 else 0.5
            self.robot_orientation -= turn_speed * timestep
            
        elif action_type == 'TurnRight':
            turn_speed = parameters[0] if len(parameters) > 0 else 0.5
            self.robot_orientation += turn_speed * timestep
        
        # Keep robot within bounds
        self.robot_position[0] = np.clip(self.robot_position[0], 1, 19)
        self.robot_position[1] = np.clip(self.robot_position[1], 1, 19)
        self.robot_orientation = (self.robot_orientation + 2*np.pi) % (2*np.pi)
        
        # Update environment
        self.update(timestep)

# Integrate motor control with NLM
class AdaptiveRobotController:
    """Adaptive robot controller integrating motor control with NLM."""
    
    def __init__(self, config):
        self.config = config
        
        # Initialize components
        self.motor_controller = MotorController(config)
        self.environment_simulator = EnvironmentSimulator(config)
        
        # Initialize NLM
        self.brain = pynlm.createBrain(config.get_pynlm_config())
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.initialize(self.environment_simulator)
        
        # Initialize sensors
        self.camera = CameraSensor(**config.get_sensor_config('camera'))
        self.lidar = LidarSensor(**config.get_sensor_config('lidar'))
        self.proximity = ProximitySensor(**config.get_sensor_config('proximity'))
        
        self.sensor_integration = SensorIntegration(self.camera, self.lidar, self.proximity)
        
        # Control loop variables
        self.step_count = 0
        self.episode_count = 0
        self.current_reward = 0.0
        self.total_reward = 0.0
        
        # Performance tracking
        self.performance_metrics = {
            'rewards': [],
            'action_selection_times': [],
            'motor_performance': [],
            'environment_interactions': []
        }
        
    def run_control_loop(self, num_steps):
        """Run adaptive robot control loop."""
        print("Starting Adaptive Robot Control Loop")
        print("=" * 60)
        
        for step in range(num_steps):
            # 1. Get environment state
            env_state = self.environment_simulator.get_environment_state(self.motor_controller.motor_states['wheel_left']['position'])
            
            # 2. Capture sensor data
            sensor_start_time = time.time()
            percept = self.sensor_integration.capture_all_sensors({
                'position': self.motor_controller.motor_states['wheel_left']['position']
            })
            sensor_time = time.time() - sensor_start_time
            
            # 3. Process percept with NLM
            self.agent.processSensoryInput(percept)
            
            # 4. Get brain computation
            self.brain.step(step)
            
            # 5. Select action
            action_start_time = time.time()
            action = self.agent.decodeMotorCommand()
            action_time = time.time() - action_start_time
            
            # 6. Process action with motor controller
            motor_action = self.motor_controller.process_action(action, 0.1)
            
            # 7. Apply action to environment
            self.environment_simulator.apply_action(motor_action, 0.1)
            
            # 8. Calculate reward
            reward = self.calculate_reward(env_state, action)
            self.current_reward = reward
            self.total_reward += reward
            
            # 9. Adapt motor controller
            self.motor_controller.adapt_to_environment(env_state, step)
            
            # 10. Update development
            self.agent.updateDevelopment(0.1)
            
            # 11. Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # 12. Record metrics
            self.record_control_metrics(step, reward, sensor_time, action_time)
            
            # 13. Print progress
            if step % 50 == 0:
                self.print_control_status(step, env_state, reward)
        
        return self.performance_metrics
    
    def calculate_reward(self, env_state, action):
        """Calculate reward for action."""
        reward = 0.0
        
        # Reward for reaching targets
        if env_state['nearby_targets']:
            target_reward = np.mean(env_state['target_values'])
            reward += target_reward * 0.5
        
        # Reward for clear path (movement)
        if env_state['clear_path']:
            reward += 0.1
        
        # Penalty for obstacles
        if env_state['obstacle_detected']:
            reward -= 0.5
        
        # Reward for action completion
        if action is not None:
            reward += 0.05
        
        # Normalize reward
        reward = np.clip(reward, -10.0, 10.0)
        
        return reward
    
    def record_control_metrics(self, step, reward, sensor_time, action_time):
        """Record control loop metrics."""
        self.performance_metrics['rewards'].append(reward)
        self.performance_metrics['action_selection_times'].append(action_time)
        self.performance_metrics['environment_interactions'].append(step)
        
        # Record motor performance
        motor_perf = self.motor_controller.get_control_performance()
        self.performance_metrics['motor_performance'].append(motor_perf)
    
    def print_control_status(self, step, env_state, reward):
        """Print control loop status."""
        print(f"Step {step:4d}:")
        print(f"  Reward: {reward:6.2f} (Total: {self.total_reward:8.2f})")
        print(f"  Robot Position: {self.motor_controller.motor_states['wheel_left']['position']:.2f}, "
              f"{self.motor_controller.motor_states['wheel_right']['position']:.2f}")
        print(f"  Action Selection Time: {np.mean(self.performance_metrics['action_selection_times'][-10:]):.4f}s")
        print(f"  Sensor Processing Time: {np.mean([self.performance_metrics['action_selection_times'][i] for i in range(max(0, step-10), step+1)]):.4f}s")
        print(f"  Obstacles: {'Yes' if env_state['obstacle_detected'] else 'No'}, "
              f"Targets: {len(env_state['nearby_targets'])}")
        
        # Motor performance
        if step < len(self.performance_metrics['motor_performance']):
            motor_perf = self.performance_metrics['motor_performance'][step]
            print(f"  Motor Performance - Accuracy: {motor_perf['position_accuracy']:.2f}, "
                  f"Stability: {motor_perf['velocity_stability']:.2f}")

def adaptive_robot_example():
    """Run the adaptive robot controller example."""
    print("=" * 60)
    print("NLM Adaptive Robot Controller Example")
    print("=" * 60)
    
    # Create robot controller
    controller = AdaptiveRobotController(RobotControllerConfig())
    
    # Run control loop
    results = controller.run_control_loop(num_steps=1000)
    
    print("\n" + "=" * 60)
    print("Adaptive Robot Control Complete!")
    print("=" * 60)
    
    # Print final performance
    final_reward = results['rewards'][-1] if results['rewards'] else 0
    avg_reward = np.mean(results['rewards'])
    total_reward = sum(results['rewards'])
    
    print(f"Total Steps: {len(results['rewards'])}")
    print(f"Final Reward: {final_reward:.2f}")
    print(f"Average Reward: {avg_reward:.2f}")
    print(f"Total Accumulated Reward: {total_reward:.2f}")
    
    # Average motor performance
    if results['motor_performance']:
        avg_motor_perf = {
            'position_accuracy': np.mean([p['position_accuracy'] for p in results['motor_performance']]),
            'velocity_stability': np.mean([p['velocity_stability'] for p in results['motor_performance']]),
            'adaptation_efficiency': np.mean([p['adaptation_efficiency'] for p in results['motor_performance']]),
            'response_time': np.mean([p['response_time'] for p in results['motor_performance']])
        }
        
        print(f"\nAverage Motor Performance:")
        print(f"  Position Accuracy: {avg_motor_perf['position_accuracy']:.2f}")
        print(f"  Velocity Stability: {avg_motor_perf['velocity_stability']:.2f}")
        print(f"  Adaptation Efficiency: {avg_motor_perf['adaptation_efficiency']:.2f}")
        print(f"  Response Time: {avg_motor_perf['response_time']:.2f}")
    
    return results

if __name__ == "__main__":
    # Note: This example demonstrates adaptive robot control concepts
    # Actual implementation would require complete NLM integration
    print("Adaptive robot controller example requires full NLM integration")
    print("This template shows the structure and key concepts.")
    
    # adaptive_robot_example()
```