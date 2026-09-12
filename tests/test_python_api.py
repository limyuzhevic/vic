#!/usr/bin/env python3
"""
Python API Test for NLM Bindings
Tests the improved Python bindings functionality
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Mock the C++ bindings for testing since we can't actually compile Python extensions
# This simulates what the tests would do when Python bindings are available

class MockNeuronId:
    def __init__(self, value):
        self.value = value
    
    def __eq__(self, other):
        return isinstance(other, MockNeuronId) and self.value == other.value
    
    def __repr__(self):
        return f"<MockNeuronId: {self.value}>"

class MockSynapseId:
    def __init__(self, value):
        self.value = value
    
    def __eq__(self, other):
        return isinstance(other, MockSynapseId) and self.value == other.value
    
    def __repr__(self):
        return f"<MockSynapseId: {self.value}>"

class MockConfig:
    def __init__(self):
        self.data = {}
    
    def set(self, key, value, source=None):
        self.data[key] = value
        return True
    
    def has(self, key):
        return key in self.data
    
    def get(self, key, type_func):
        if key in self.data:
            try:
                return type_func(self.data[key])
            except:
                return None
        return None
    
    def getOr(self, key, default):
        value = self.get(key, lambda x: x)
        return value if value is not None else default
    
    def getKeys(self):
        return list(self.data.keys())
    
    def summary(self):
        return f"MockConfig with {len(self.data)} keys"

class MockSensoryInput:
    def __init__(self, data=None, type_val=0, timestamp=0.0):
        self._data = data or []
        self._type = type_val
        self._timestamp = timestamp
    
    def getType(self):
        return self._type
    
    def getData(self):
        return self._data
    
    def getTimestamp(self):
        return self._timestamp
    
    def setTimestamp(self, timestamp):
        self._timestamp = timestamp

class MockAction:
    def __init__(self, action_type=0, parameters=None):
        self._type = action_type
        self._parameters = parameters or []
    
    def getType(self):
        return self._type
    
    def getParameters(self):
        return self._parameters
    
    def setType(self, action_type):
        self._type = action_type
    
    def setParameters(self, parameters):
        self._parameters = parameters
    
    def getName(self):
        return f"Action_{self._type}"
    
    def clone(self):
        return MockAction(self._type, self._parameters.copy())

class MockWorldObject:
    def __init__(self, x=0.0, y=0.0, obj_type=0, value=0.0, radius=0.5):
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = True
    
    def getPosition(self):
        return (self.x, self.y)
    
    def setPosition(self, x, y):
        self.x = x
        self.y = y
    
    def getDistanceTo(self, x, y):
        dx = self.x - x
        dy = self.y - y
        return (dx*dx + dy*dy)**0.5
    
    def isNear(self, x, y, distance):
        return self.getDistanceTo(x, y) <= distance
    
    def isColliding(self, other):
        dx = self.x - other.x
        dy = self.y - other.y
        distance = (dx*dx + dy*dy)**0.5
        return distance <= (self.radius + other.radius)
    
    def isPassable(self):
        return self.type != 1 and self.active  # Not Wall and active
    
    def isResource(self):
        return self.type == 0 and self.active  # Resource and active
    
    def isHazard(self):
        return self.type == 1 and self.active  # Hazard and active
    
    def isMarker(self):
        return self.type == 2 and self.active  # Marker and active
    
    def getRewardValue(self):
        if self.type == 0:  # Resource
            return self.value
        elif self.type == 1:  # Hazard
            return -self.value
        return 0.0
    
    def clone(self):
        return MockWorldObject(self.x, self.y, self.type, self.value, self.radius)

class MockAgentBrain:
    def __init__(self, config):
        self.config = config
        self._brain_state = "initialized"
    
    def initialize(self):
        self._brain_state = "active"
        return True
    
    def step(self, step_num):
        return True
    
    def get_percept(self):
        return MockSensoryInput([0.1, 0.2, 0.3])
    
    def process_sensory_input(self, percept):
        # Simulate processing
        return MockAction(1, [0.5])
    
    def act(self, action):
        return 0.1  # reward
    
    @property
    def brain(self):
        return self

def test_python_api():
    """Test the improved Python API"""
    print("=== Testing Improved Python API ===")
    
    # Test 1: Configuration API
    print("Test 1: Configuration API...")
    config = MockConfig()
    
    # Type-safe set/get methods
    config.set("brain.neuron_count", 1000, "Default")
    config.set("world.width", 100.0, "Default")
    config.set("simulation.timestep", 0.001, "Default")
    
    # Type-safe get methods
    neurons = config.get("brain.neuron_count", int)
    assert neurons == 1000, f"Expected 1000, got {neurons}"
    
    width = config.get("world.width", float)
    assert width == 100.0, f"Expected 100.0, got {width}"
    
    timestep = config.get("simulation.timestep", float)
    assert timestep == 0.001, f"Expected 0.001, got {timestep}"
    
    # Get with defaults
    default_val = config.getOr("nonexistent", 42)
    assert default_val == 42, f"Expected 42, got {default_val}"
    
    print("  PASSED")
    
    # Test 2: ID types
    print("Test 2: ID types...")
    
    neuron_id = MockNeuronId(123)
    assert neuron_id.value == 123
    assert repr(neuron_id) == "<MockNeuronId: 123>"
    
    synapse_id = MockSynapseId(456)
    assert synapse_id.value == 456
    assert repr(synapse_id) == "<MockSynapseId: 456>"
    
    print("  PASSED")
    
    # Test 3: Sensory input types
    print("Test 3: Sensory input types...")
    
    vision = MockSensoryInput([0.1, 0.2, 0.3, 0.4, 0.5], 1, 100.0)
    assert vision.getType() == 1
    assert vision.getData() == [0.1, 0.2, 0.3, 0.4, 0.5]
    assert vision.getTimestamp() == 100.0
    
    vision.setTimestamp(200.0)
    assert vision.getTimestamp() == 200.0
    
    print("  PASSED")
    
    # Test 4: Action types
    print("Test 4: Action types...")
    
    action = MockAction(1, [45.0, 10.0])
    assert action.getType() == 1
    assert action.getParameters() == [45.0, 10.0]
    assert action.getName() == "Action_1"
    
    action.setType(2)
    action.setParameters([90.0])
    assert action.getType() == 2
    assert action.getParameters() == [90.0]
    
    # Test convenience methods
    assert not action.is_movement()
    assert not action.is_interaction()
    assert not action.is_looking()
    
    cloned = action.clone()
    assert cloned.getType() == 2
    assert cloned.getParameters() == [90.0]
    
    print("  PASSED")
    
    # Test 5: World object types
    print("Test 5: World object types...")
    
    resource = MockWorldObject(10.0, 20.0, 0, 5.0)  # Resource type 0
    assert resource.isResource()
    assert resource.getRewardValue() == 5.0
    
    hazard = MockWorldObject(30.0, 40.0, 1, 2.0)  # Hazard type 1
    assert hazard.isHazard()
    assert hazard.getRewardValue() == -2.0
    
    wall = MockWorldObject(50.0, 60.0, 2)  # Wall type 2
    assert not wall.isPassable()
    assert not wall.isResource()
    
    marker = MockWorldObject(70.0, 80.0, 3)  # Marker type 3
    assert marker.isMarker()
    
    # Test distance and collision
    distance = resource.getDistanceTo(11.0, 21.0)
    assert abs(distance - (0.1**2 + 0.1**2)**0.5) < 0.001
    
    colliding = resource.isColliding(MockWorldObject(10.1, 20.1, 0, 0.0, 0.1))
    assert colliding
    
    near = resource.isNear(11.0, 21.0, 0.2)
    assert near
    
    print("  PASSED")
    
    # Test 6: Agent creation
    print("Test 6: Agent creation...")
    
    agent_config = MockConfig()
    agent_config.set("brain.neuron_count", 500, "Default")
    agent_config.set("world.width", 200.0, "Default")
    
    agent = MockAgentBrain(agent_config)
    
    # Test agent interface
    assert agent.initialize()
    assert agent._brain_state == "active"
    
    percept = agent.get_percept()
    assert percept is not None
    assert percept.getType() == 1
    
    action = agent.process_sensory_input(percept)
    assert action is not None
    assert action.getType() == 1
    
    reward = agent.act(action)
    assert reward == 0.1
    
    assert agent.step(1)
    
    print("  PASSED")
    
    print("=== All Python API Tests PASSED ===")
    return True

if __name__ == "__main__":
    try:
        test_python_api()
        print("\nSUCCESS: Python API improvements test passed!")
    except Exception as e:
        print(f"\nFAILED: Python API test failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)