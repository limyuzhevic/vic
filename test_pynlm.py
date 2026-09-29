#!/usr/bin/env python3
"""
Comprehensive Python binding tests for NLM (Neural Learning Machine)

This script tests the Python bindings to ensure they work correctly
and provide the expected functionality.
"""

import sys
import os
import tempfile
import json
from pathlib import Path

# Add the Python bindings directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYTHON_AVAILABLE = True
except ImportError as e:
    print(f"Python bindings not available: {e}")
    PYTHON_AVAILABLE = False
    pynlm = None

class NLMTester:
    """Test suite for NLM Python bindings"""
    
    def __init__(self):
        self.passed_tests = 0
        self.failed_tests = 0
        self.errors = []
        self.temp_files = []
    
    def run_all_tests(self):
        """Run all tests"""
        if not PYTHON_AVAILABLE:
            self.add_error("Python bindings not available - skipping tests")
            return
        
        print("=== NLM Python Bindings Test Suite ===\n")
        
        # Test configuration management
        self.test_config()
        
        # Test brain creation and initialization
        self.test_brain_creation()
        
        # Test world operations
        self.test_world_operations()
        
        # Test sensory processing
        self.test_sensory_processing()
        
        # Test action handling
        self.test_action_handling()
        
        # Test agent brain interface
        self.test_agent_brain_interface()
        
        # Test serialization
        self.test_serialization()
        
        # Test advanced features
        self.test_advanced_features()
        
        # Test error handling
        self.test_error_handling()
        
        self.print_summary()
    
    def add_error(self, message, test_name=None):
        """Add an error to the test results"""
        self.errors.append({
            'message': message,
            'test': test_name or "Unknown",
            'type': 'error'
        })
        self.failed_tests += 1
        print(f"❌ ERROR: {message}")
    
    def add_success(self, message, test_name=None):
        """Add a successful test"""
        self.passed_tests += 1
        print(f"✅ PASS: {message}")
    
    def test_config(self):
        """Test configuration management"""
        print("\n--- Testing Configuration Management ---")
        
        try:
            # Test default config creation
            config = pynlm.createDefaultConfig()
            self.add_success("Default config created")
            
            # Test config properties
            config.set("neuron_count", 500)
            config.set("random_seed", 123)
            config.set("simulation_timestep", 0.001)
            
            if config.has("neuron_count"):
                self.add_success("Config has key")
            else:
                self.add_error("Config doesn't have expected key")
            
            # Test getting values
            neuron_count = config.getOr("neuron_count", 100)
            if neuron_count == 500:
                self.add_success("Config getOr works correctly")
            else:
                self.add_error(f"Config getOr returned {neuron_count}, expected 500")
            
            # Test config summary
            summary = config.summary()
            if isinstance(summary, str) and len(summary) > 0:
                self.add_success(f"Config summary: {summary[:50]}...")
            else:
                self.add_error("Config summary not working")
            
            # Test config to string
            config_str = str(config)
            if isinstance(config_str, str) and "Config" in config_str:
                self.add_success("Config string representation works")
            else:
                self.add_error("Config string representation failed")
            
            # Test config file operations
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
                temp_file = f.name
                self.temp_files.append(temp_file)
                config.saveToFile(temp_file)
            
            config2 = pynlm.createDefaultConfig()
            if config2.loadFromFile(temp_file):
                self.add_success("Config file loading works")
            else:
                self.add_error("Config file loading failed")
            
            # Test config clearing
            config2.clear()
            if not config2.has("neuron_count"):
                self.add_success("Config clearing works")
            else:
                self.add_error("Config clearing failed")
            
        except Exception as e:
            self.add_error(f"Configuration test failed: {str(e)}")
    
    def test_brain_creation(self):
        """Test brain creation and basic functionality"""
        print("\n--- Testing Brain Creation ---")
        
        try:
            # Create brain with config
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            self.add_success("Brain created with config")
            
            # Initialize brain
            brain.initialize()
            self.add_success("Brain initialized")
            
            # Test brain properties
            neuron_count = brain.getTotalNeuronCount()
            if neuron_count > 0:
                self.add_success(f"Brain has {neuron_count} neurons")
            else:
                self.add_error("Brain has zero neurons")
            
            # Test step function
            brain.step(0, 0.001)
            self.add_success("Brain step executed")
            
            # Test developmental stage
            stage = brain.getDevelopmentalStage()
            if hasattr(pynlm, 'DevelopmentalStage') and stage in [pynlm.DevelopmentalStage.Initial, 
                                                                pynlm.DevelopmentalStage.CriticalPeriod,
                                                                pynlm.DevelopmentalStage.Maturation,
                                                                pynlm.DevelopmentalStage.Adult,
                                                                pynlm.DevelopmentalStage.Aging]:
                self.add_success(f"Brain has valid developmental stage: {stage}")
            else:
                self.add_error(f"Invalid developmental stage: {stage}")
            
            # Test performance metrics
            metrics = brain.getPerformanceMetrics()
            if isinstance(metrics, dict):
                self.add_success("Brain performance metrics available")
            else:
                self.add_error("Brain performance metrics not working")
            
        except Exception as e:
            self.add_error(f"Brain creation test failed: {str(e)}")
    
    def test_world_operations(self):
        """Test world operations and interactions"""
        print("\n--- Testing World Operations ---")
        
        try:
            # Create world
            world = pynlm.createSimpleWorld()
            self.add_success("World created")
            
            # Configure world
            world.configure(100, 100, 20, 20)
            self.add_success("World configured")
            
            # Set agent start position
            world.setAgentStart(50, 50)
            self.add_success("Agent start position set")
            
            # Get world properties
            width = world.getWidth()
            height = world.getHeight()
            if width == 100 and height == 100:
                self.add_success(f"World dimensions correct: {width}x{height}")
            else:
                self.add_error(f"World dimensions incorrect: {width}x{height}")
            
            # Test reset
            world.reset()
            self.add_success("World reset")
            
            # Test object operations
            obj = pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, 5.0)
            world.addObject(obj)
            self.add_success("World object added")
            
            # Test position validation
            if world.isValidPosition(30, 30):
                self.add_success("Position validation works")
            else:
                self.add_error("Position validation failed")
            
            # Test agent body
            body = world.getAgentBody()
            if hasattr(body, 'x') and hasattr(body, 'y') and hasattr(body, 'energy'):
                self.add_success("Agent body accessible")
            else:
                self.add_error("Agent body not accessible")
            
            # Test simulation time
            time = world.getSimulationTime()
            if isinstance(time, (int, float)) and time >= 0:
                self.add_success(f"Simulation time: {time}")
            else:
                self.add_error("Invalid simulation time")
            
        except Exception as e:
            self.add_error(f"World operations test failed: {str(e)}")
    
    def test_sensory_processing(self):
        """Test sensory input processing"""
        print("\n--- Testing Sensory Processing ---")
        
        try:
            # Create sensory inputs
            vision = pynlm.Vision(10, 10, 3)
            vision.setData([0.1, 0.2, 0.3] * 100)  # 10x10x3 = 300 values
            
            audio = pynlm.Audio(44100, 11025)  # 0.25 seconds of audio
            audio.setData([0.0, 0.5, 0.8, 0.3] * 2756)
            
            internal = pynlm.InternalSignals()
            internal.addSignal(0.5)
            internal.addSignal(0.8)
            internal.addSignal(0.3)
            
            # Test sensory properties
            if vision.getWidth() == 10 and vision.getHeight() == 10:
                self.add_success("Vision dimensions correct")
            else:
                self.add_error("Vision dimensions incorrect")
            
            if audio.getSampleRate() == 44100:
                self.add_success("Audio sample rate correct")
            else:
                self.add_error("Audio sample rate incorrect")
            
            if internal.getSignalCount() == 3:
                self.add_success("Internal signals count correct")
            else:
                self.add_error("Internal signals count incorrect")
            
            # Create sensory percept
            percept = pynlm.SensoryPercept()
            percept.setVision(vision)
            percept.setAudio(audio)
            percept.setInternal(internal)
            percept.setTimestamp(1.0)
            
            # Test percept properties
            if percept.getVisionWidth() == 10:
                self.add_success("Percept vision width correct")
            else:
                self.add_error("Percept vision width incorrect")
            
            if percept.getVisionHeight() == 10:
                self.add_success("Percept vision height correct")
            else:
                self.add_error("Percept vision height incorrect")
            
            # Test all signals
            all_signals = percept.getAllSignals()
            if isinstance(all_signals, dict):
                self.add_success("Percept all signals works")
            else:
                self.add_error("Percept all signals not working")
            
            # Test percept string representation
            percept_str = str(percept)
            if isinstance(percept_str, str) and "SensoryPercept" in percept_str:
                self.add_success("Percept string representation works")
            else:
                self.add_error("Percept string representation failed")
            
        except Exception as e:
            self.add_error(f"Sensory processing test failed: {str(e)}")
    
    def test_action_handling(self):
        """Test action creation and handling"""
        print("\n--- Testing Action Handling ---")
        
        try:
            # Test basic action creation
            action = pynlm.Action(pynlm.ActionType.MoveForward)
            self.add_success("Basic action created")
            
            if action.getName() == "MoveForward":
                self.add_success("Action name correct")
            else:
                self.add_error(f"Action name incorrect: {action.getName()}")
            
            # Test action with parameters
            action2 = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.8, 1.2])
            self.add_success("Action with parameters created")
            
            if len(action2.getParameters()) == 3:
                self.add_success("Action parameters count correct")
            else:
                self.add_error(f"Action parameters count incorrect: {len(action2.getParameters())}")
            
            # Test action cloning
            action3 = action2.clone()
            if action3.getName() == action2.getName() and action3.getParameters() == action2.getParameters():
                self.add_success("Action cloning works")
            else:
                self.add_error("Action cloning failed")
            
            # Test action string representation
            action_str = str(action)
            if isinstance(action_str, str) and "MoveForward" in action_str:
                self.add_success("Action string representation works")
            else:
                self.add_error("Action string representation failed")
            
            # Test action repr
            action_repr = repr(action)
            if isinstance(action_repr, str) and "Action" in action_repr:
                self.add_success("Action repr works")
            else:
                self.add_error("Action repr failed")
            
            # Test world object actions
            world_obj = pynlm.WorldObject(20, 30, pynlm.WorldObjectType.Resource, 10.0)
            self.add_success("World object created")
            
            if world_obj.getTypeName() == "Resource":
                self.add_success("World object type name correct")
            else:
                self.add_error(f"World object type name incorrect: {world_obj.getTypeName()}")
            
            # Test distance calculation
            distance = world_obj.distanceTo(10, 10)
            if isinstance(distance, (int, float)) and distance > 0:
                self.add_success(f"Distance calculation works: {distance:.2f}")
            else:
                self.add_error("Distance calculation failed")
            
        except Exception as e:
            self.add_error(f"Action handling test failed: {str(e)}")
    
    def test_agent_brain_interface(self):
        """Test agent brain interface"""
        print("\n--- Testing Agent Brain Interface ---")
        
        try:
            # Setup components
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(50, 50, 10, 10)
            world.setAgentStart(25, 25)
            
            # Create agent brain
            agent_brain = pynlm.createAgentBrain(brain)
            self.add_success("Agent brain created")
            
            # Initialize agent brain
            agent_brain.initialize(world)
            self.add_success("Agent brain initialized")
            
            # Test get sizes
            input_size = agent_brain.getSensoryInputSize()
            output_size = agent_brain.getMotorOutputSize()
            
            if isinstance(input_size, (int, float)) and input_size > 0:
                self.add_success(f"Agent brain input size: {input_size}")
            else:
                self.add_error("Agent brain input size incorrect")
            
            if isinstance(output_size, (int, float)) and output_size > 0:
                self.add_success(f"Agent brain output size: {output_size}")
            else:
                self.add_error("Agent brain output size incorrect")
            
            # Test get status
            status = agent_brain.getStatus()
            if isinstance(status, dict):
                self.add_success("Agent brain status available")
            else:
                self.add_error("Agent brain status not working")
            
            # Test enable/disable features
            agent_brain.enableRewardModulation(True)
            if agent_brain.isRewardModulationEnabled():
                self.add_success("Reward modulation enable/disable works")
            else:
                self.add_error("Reward modulation enable/disable failed")
            
            # Test development
            agent_brain.updateDevelopment(0.01)
            stage = agent_brain.getDevelopmentalStage()
            self.add_success(f"Development stage: {stage}")
            
            # Test neuromodulation levels
            curiosity = agent_brain.getCuriosityLevel()
            novelty = agent_brain.getNoveltyLevel()
            
            if isinstance(curiosity, (int, float)) and isinstance(novelty, (int, float)):
                self.add_success(f"Neuromodulation levels: curiosity={curiosity:.3f}, novelty={novelty:.3f}")
            else:
                self.add_error("Neuromodulation levels not working")
            
        except Exception as e:
            self.add_error(f"Agent brain interface test failed: {str(e)}")
    
    def test_serialization(self):
        """Test serialization and deserialization"""
        print("\n--- Testing Serialization ---")
        
        try:
            # Create and configure brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Perform some operations to change state
            brain.step(0, 0.001)
            
            # Save brain state
            with tempfile.NamedTemporaryFile(suffix='.nnl', delete=False) as f:
                temp_file = f.name
                self.temp_files.append(temp_file)
            
            brain.save(temp_file)
            self.add_success("Brain state saved")
            
            # Create new brain and load state
            brain2 = pynlm.createBrain(config)
            brain2.load(temp_file)
            self.add_success("Brain state loaded")
            
            # Reset brain for more tests
            brain2.reset()
            brain2.initialize()
            
            # Test config serialization
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
                config_file = f.name
                self.temp_files.append(config_file)
            
            config.saveToFile(config_file)
            self.add_success("Config file saved")
            
            # Load config from file
            config2 = pynlm.createDefaultConfig()
            if config2.loadFromFile(config_file):
                self.add_success("Config file loaded")
            else:
                self.add_error("Config file loading failed")
            
            # Test config string representation
            config_str = str(config2)
            if isinstance(config_str, str) and len(config_str) > 0:
                self.add_success("Config string representation works")
            else:
                self.add_error("Config string representation failed")
            
        except Exception as e:
            self.add_error(f"Serialization test failed: {str(e)}")
    
    def test_advanced_features(self):
        """Test advanced Python features"""
        print("\n--- Testing Advanced Features ---")
        
        try:
            # Test convenience functions
            config = pynlm.createRandomConfig()
            self.add_success("createRandomConfig works")
            
            # Test version
            version = pynlm.version()
            if isinstance(version, str) and "NLM" in version:
                self.add_success(f"Version: {version}")
            else:
                self.add_error("Version not working")
            
            # Test simulation episode (simplified)
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            world = pynlm.createSimpleWorld()
            
            # Minimal setup for simulation
            brain.initialize()
            world.configure(20, 20, 5, 5)
            
            agent_brain = pynlm.createAgentBrain(brain)
            agent_brain.initialize(world)
            
            # Run a simple simulation
            total_reward = 0.0
            steps = 10
            
            for step in range(steps):
                percept = world.getSensoryPercept()
                agent_brain.processSensoryInput(percept)
                action = agent_brain.decodeMotorCommand()
                result = world.applyMotorCommand(action, world.getSimulationTime())
                total_reward += result.reward
                agent_brain.applyRewardModulation(result.reward, 0.0)
                world.update(0.1)
            
            self.add_success(f"Simulation completed: {steps} steps, total reward: {total_reward:.3f}")
            
            # Test brain benchmarking
            brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
            benchmark_steps = pynlm.benchmarkBrain(brain2, durationMs=100)
            
            if isinstance(benchmark_steps, int) and benchmark_steps > 0:
                self.add_success(f"Benchmark completed: {benchmark_steps} steps in 100ms")
            else:
                self.add_error("Benchmark not working")
            
        except Exception as e:
            self.add_error(f"Advanced features test failed: {str(e)}")
    
    def test_error_handling(self):
        """Test error handling and edge cases"""
        print("\n--- Testing Error Handling ---")
        
        try:
            # Test invalid operations (should not crash)
            world = pynlm.createSimpleWorld()
            
            # Test invalid position
            valid_pos = world.isValidPosition(0, 0)
            self.add_success(f"Position validation: {valid_pos}")
            
            # Test agent body access
            body = world.getAgentBody()
            pos = body.getPosition()
            self.add_success(f"Agent position: {pos}")
            
            # Test with invalid config (should handle gracefully)
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test with various object types
            for obj_type in [pynlm.WorldObjectType.Empty, 
                           pynlm.WorldObjectType.Resource,
                           pynlm.WorldObjectType.Hazard,
                           pynlm.WorldObjectType.Wall,
                           pynlm.WorldObjectType.Marker]:
                obj = pynlm.WorldObject(0, 0, obj_type, 1.0)
                if obj.getTypeName() in ["Empty", "Resource", "Hazard", "Wall", "Marker"]:
                    self.add_success(f"World object type {obj_type} created")
                else:
                    self.add_error(f"World object type {obj_type} failed")
            
            # Test action types
            for action_type in [pynlm.ActionType.MoveForward,
                              pynlm.ActionType.TurnLeft,
                              pynlm.ActionType.Interact,
                              pynlm.ActionType.Wait]:
                action = pynlm.Action(action_type)
                if action.getName() in ["MoveForward", "TurnLeft", "Interact", "Wait"]:
                    self.add_success(f"Action type {action_type} created")
                else:
                    self.add_error(f"Action type {action_type} failed")
            
            # Test enum values
            if hasattr(pynlm, 'NeuronType') and hasattr(pynlm, 'SynapseType'):
                self.add_success("All enumerations available")
            else:
                self.add_error("Missing enumerations")
            
        except Exception as e:
            self.add_error(f"Error handling test failed: {str(e)}")
    
    def print_summary(self):
        """Print test summary"""
        print("\n" + "="*50)
        print("=== TEST SUMMARY ===")
        print("="*50)
        print(f"Tests passed: {self.passed_tests}")
        print(f"Tests failed: {self.failed_tests}")
        print(f"Total errors: {len(self.errors)}")
        
        if self.errors:
            print("\n=== ERRORS ===")
            for error in self.errors:
                print(f"- {error['test']}: {error['message']}")
        
        # Clean up temporary files
        for temp_file in self.temp_files:
            try:
                if os.path.exists(temp_file):
                    os.unlink(temp_file)
            except:
                pass
        
        if self.failed_tests == 0 and len(self.errors) == 0:
            print("\n🎉 ALL TESTS PASSED!")
            return True
        else:
            print(f"\n❌ {self.failed_tests} TESTS FAILED")
            return False
if __name__ == "__main__":
    print("Starting NLM Python Bindings Test Suite...")
    
    tester = NLMTester()
    success = tester.run_all_tests()
    
    if success:
        print("\n✅ Test suite completed successfully!")
        sys.exit(0)
    else:
        print("\n❌ Test suite completed with failures!")
        sys.exit(1)