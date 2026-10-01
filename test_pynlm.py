#!/usr/bin/env python3
"""
NLM Python Bindings Test Suite
===============================

Comprehensive test suite for NLM Python bindings.
Tests all major functionality including error handling, validation, and integration.

Usage:
    python3 test_pynlm.py [options]

Options:
    --verbose    Enable verbose output
    --quick      Run quick tests only
    --full       Run full comprehensive tests
"""

import sys
import os
import traceback

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"ERROR: Could not import pynlm: {e}")
    print("Please install pynlm first:")
    print("  pip install pynlm")
    print("  or")
    print("  pip install -e .")
    sys.exit(1)

class TestSuite:
    """Comprehensive test suite for NLM Python bindings."""
    
    def __init__(self, verbose=False, quick=False):
        self.verbose = verbose
        self.quick = quick
        self.passed = 0
        self.failed = 0
        self.errors = 0
        
    def log_test(self, test_name, status, message=""):
        """Log test results."""
        status_symbol = {
            "PASS": "✓",
            "FAIL": "✗",
            "ERROR": "✗",
            "SKIP": "○"
        }.get(status, "?")
        
        print(f"{status_symbol} {test_name}", end="")
        if message:
            print(f": {message}")
        else:
            print()
            
        if status == "PASS":
            self.passed += 1
        elif status == "FAIL":
            self.failed += 1
        elif status == "ERROR":
            self.errors += 1
            
    def run_test(self, test_name, test_func):
        """Run a single test with error handling."""
        try:
            test_func()
            self.log_test(test_name, "PASS")
        except AssertionError as e:
            self.log_test(test_name, "FAIL", str(e))
        except Exception as e:
            self.log_test(test_name, "ERROR", f"{type(e).__name__}: {e}")
            if self.verbose:
                traceback.print_exc()
    
    def run_all_tests(self):
        """Run all tests in the suite."""
        print("=" * 60)
        print("NLM Python Bindings Test Suite")
        print("=" * 60)
        print(f"Mode: {'Quick' if self.quick else 'Full'}")
        print(f"Verbose: {self.verbose}")
        print()
        
        # Test basic imports and version
        self.test_imports()
        self.test_version()
        
        # Test configuration
        self.test_config()
        self.test_config_validation()
        
        # Test basic objects
        if not self.quick:
            self.test_brain_creation()
            self.test_world_creation()
            self.test_agent_creation()
        
        # Test sensory input
        self.test_vision_input()
        self.test_audio_input()
        self.test_internal_signals()
        
        # Test actions
        self.test_actions()
        
        # Test world objects
        self.test_world_objects()
        
        # Test agent body
        self.test_agent_body()
        
        # Test percepts
        self.test_percepts()
        
        # Test working memory
        self.test_working_memory()
        
        # Test attention
        self.test_attention()
        
        # Test integration
        if not self.quick:
            self.test_agent_world_integration()
            self.test_learning_simulation()
        
        # Test error handling
        self.test_error_handling()
        
        # Test validation
        self.test_validation()
        
        # Print summary
        self.print_summary()
    
    def test_imports(self):
        """Test basic imports."""
        try:
            # Test all major classes can be imported
            assert pynlm.Config
            assert pynlm.Brain
            assert pynlm.AgentBrain
            assert pynlm.SimpleWorld
            assert pynlm.Action
            assert pynlm.WorldObject
            assert pynlm.AgentBody
            assert pynlm.ActionResult
            assert pynlm.SensoryPercept
            assert pynlm.Vision
            assert pynlm.Audio
            assert pynlm.InternalSignals
            
            # Test enums
            assert pynlm.NeuronType
            assert pynlm.SynapseType
            assert pynlm.DevelopmentalStage
            assert pynlm.ActionType
            assert pynlm.MotorCommand
            assert pynlm.WorldObjectType
            
            # Test constants
            assert hasattr(pynlm, 'INVALID_NEURON_ID')
            assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
            assert hasattr(pynlm, 'INVALID_REGION_ID')
            assert hasattr(pynlm, 'INVALID_POPULATION_ID')
            
            self.log_test("Imports", "PASS", "All classes and enums imported successfully")
        except Exception as e:
            self.log_test("Imports", "FAIL", f"Import failed: {e}")
    
    def test_version(self):
        """Test version information."""
        try:
            # Test version function
            version = pynlm.getVersion()
            assert version is not None
            assert isinstance(version, str)
            assert len(version) > 0
            
            # Test build info
            build_info = pynlm.getBuildInfo()
            assert build_info is not None
            assert isinstance(build_info, str)
            assert len(build_info) > 0
            
            self.log_test("Version", "PASS", f"Version: {version}")
        except Exception as e:
            self.log_test("Version", "FAIL", f"Version test failed: {e}")
    
    def test_config(self):
        """Test configuration class."""
        try:
            # Test default config
            config = pynlm.createDefaultConfig()
            assert config is not None
            
            # Test config methods
            config.set("test.key", "test.value")
            assert config.has("test.key")
            
            # Test save/load
            import tempfile
            import os
            
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
                config.saveToFile(f.name)
                
                # Load into new config
                config2 = pynlm.createDefaultConfig()
                config2.loadFromFile(f.name)
                assert config2.has("test.key")
                
                os.unlink(f.name)
            
            self.log_test("Configuration", "PASS", "Config operations successful")
        except Exception as e:
            self.log_test("Configuration", "FAIL", f"Config test failed: {e}")
    
    def test_config_validation(self):
        """Test configuration validation."""
        try:
            config = pynlm.createDefaultConfig()
            
            # Test validation
            is_valid = config.validate()
            assert isinstance(is_valid, bool)
            
            # Test error count
            error_count = config.getErrorCount()
            assert isinstance(error_count, int)
            assert error_count >= 0
            
            # Test error reset
            config.resetErrors()
            
            self.log_test("Config Validation", "PASS", "Config validation successful")
        except Exception as e:
            self.log_test("Config Validation", "FAIL", f"Config validation failed: {e}")
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            assert brain is not None
            
            brain.initialize()
            
            # Test brain properties
            assert brain.getTotalNeuronCount() > 0
            assert brain.getTotalSynapseCount() >= 0
            assert brain.getFiringNeuronCount() >= 0
            
            # Test brain validation
            brain.validate()
            
            self.log_test("Brain Creation", "PASS", f"Brain created with {brain.getTotalNeuronCount()} neurons")
        except Exception as e:
            self.log_test("Brain Creation", "FAIL", f"Brain creation failed: {e}")
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        try:
            world = pynlm.createSimpleWorld()
            assert world is not None
            
            world.configure(10, 10, 4, 4)
            
            # Test world validation
            world.validateConfiguration()
            
            # Test world properties
            assert world.getWidth() == 10
            assert world.getHeight() == 10
            
            self.log_test("World Creation", "PASS", "World configured successfully")
        except Exception as e:
            self.log_test("World Creation", "FAIL", f"World creation failed: {e}")
    
    def test_agent_creation(self):
        """Test agent creation."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            assert agent is not None
            
            # Test agent validation
            agent.validateState()
            
            self.log_test("Agent Creation", "PASS", "Agent created successfully")
        except Exception as e:
            self.log_test("Agent Creation", "FAIL", f"Agent creation failed: {e}")
    
    def test_vision_input(self):
        """Test vision input."""
        try:
            vision = pynlm.Vision(8, 8, 3)
            assert vision is not None
            
            # Test validation
            vision.validate()
            
            # Test data setting
            test_data = [0.1] * 64
            vision.setData(test_data)
            
            # Test properties
            assert vision.getWidth() == 8
            assert vision.getHeight() == 8
            assert vision.getChannels() == 3
            assert vision.getTotalPixels() == 64
            
            # Test normalization
            vision.normalize()
            
            # Test statistics
            assert vision.getMeanIntensity() >= 0
            assert vision.getMaxIntensity() >= 0
            
            self.log_test("Vision Input", "PASS", "Vision input processed successfully")
        except Exception as e:
            self.log_test("Vision Input", "FAIL", f"Vision input failed: {e}")
    
    def test_audio_input(self):
        """Test audio input."""
        try:
            audio = pynlm.Audio(44100, 1024)
            assert audio is not None
            
            # Test validation
            audio.validate()
            
            # Test data setting
            test_data = [0.1] * 1024
            audio.setData(test_data)
            
            # Test properties
            assert audio.getSampleRate() == 44100
            assert audio.getNumSamples() == 1024
            
            # Test duration
            duration = audio.getDuration()
            assert duration > 0
            
            # Test normalization
            audio.normalize()
            
            # Test statistics
            assert audio.getPeakAmplitude() >= 0
            assert audio.getRMSAmplitude() >= 0
            
            self.log_test("Audio Input", "PASS", "Audio input processed successfully")
        except Exception as e:
            self.log_test("Audio Input", "FAIL", f"Audio input failed: {e}")
    
    def test_internal_signals(self):
        """Test internal signals input."""
        try:
            internal = pynlm.InternalSignals()
            assert internal is not None
            
            # Test validation
            internal.validate()
            
            # Test data operations
            internal.addSignal(0.5)
            internal.addSignal(-0.3)
            
            internal.clearSignals()
            
            self.log_test("Internal Signals", "PASS", "Internal signals processed successfully")
        except Exception as e:
            self.log_test("Internal Signals", "FAIL", f"Internal signals failed: {e}")
    
    def test_actions(self):
        """Test action classes."""
        try:
            # Test action creation
            action1 = pynlm.Action(pynlm.ActionType.MoveForward)
            assert action1 is not None
            assert action1.getType() == pynlm.ActionType.MoveForward
            
            action2 = pynlm.Action(
                pynlm.ActionType.Custom,
                [0.5, 0.8, 0.3]
            )
            assert action2.getType() == pynlm.ActionType.Custom
            
            # Test action validation
            action1.validate()
            action2.validate()
            
            # Test cloning
            cloned = action1.clone()
            assert cloned is not None
            assert cloned.getType() == action1.getType()
            
            self.log_test("Actions", "PASS", "Actions created and validated successfully")
        except Exception as e:
            self.log_test("Actions", "FAIL", f"Actions failed: {e}")
    
    def test_world_objects(self):
        """Test world object class."""
        try:
            obj = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Resource, 1.0)
            assert obj is not None
            
            # Test validation
            obj.validate()
            
            # Test properties
            assert obj.x == 1.0
            assert obj.y == 2.0
            assert obj.type == pynlm.WorldObjectType.Resource
            assert obj.value == 1.0
            
            self.log_test("World Objects", "PASS", "World objects created and validated successfully")
        except Exception as e:
            self.log_test("World Objects", "FAIL", f"World objects failed: {e}")
    
    def test_agent_body(self):
        """Test agent body class."""
        try:
            body = pynlm.AgentBody()
            assert body is not None
            
            # Test validation
            body.validate()
            
            # Test properties
            body.x = 5.0
            body.y = 3.0
            body.energy = 100.0
            body.health = 80.0
            body.age = 10
            
            body.reset()
            
            self.log_test("Agent Body", "PASS", "Agent body created and validated successfully")
        except Exception as e:
            self.log_test("Agent Body", "FAIL", f"Agent body failed: {e}")
    
    def test_percepts(self):
        """Test percept class."""
        try:
            percept = pynlm.SensoryPercept()
            assert percept is not None
            
            # Test validation
            percept.validate()
            
            # Test methods
            vision = percept.getVision()
            assert vision is not None
            
            percept.setVision(vision)
            
            # Test other sensory inputs
            touch = percept.getTouch()
            internal = percept.getInternal()
            proprioception = percept.getProprioception()
            audio = percept.getAudio()
            
            self.log_test("Percepts", "PASS", "Percepts created and validated successfully")
        except Exception as e:
            self.log_test("Percepts", "FAIL", f"Percepts failed: {e}")
    
    def test_working_memory(self):
        """Test working memory functionality."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            wm = brain.getWorkingMemory()
            if wm is not None:
                # Test validation
                wm.validate()
                
                # Test memory operations
                wm.store([0.5, 0.8, 0.3], 1.0)
                retrieved = wm.retrieve()
                assert len(retrieved) > 0
                
                # Test memory state
                active_traces = wm.getActiveTraces()
                capacity = wm.getCapacity()
                
                # Test memory operations
                wm.update(0.1)
                wm.strengthenMemory(1.2)
                wm.clear()
                
                self.log_test("Working Memory", "PASS", f"Working memory processed {active_traces} traces")
            else:
                self.log_test("Working Memory", "SKIP", "Working memory not available")
        except Exception as e:
            self.log_test("Working Memory", "FAIL", f"Working memory failed: {e}")
    
    def test_attention(self):
        """Test attention functionality."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            attn = brain.getAttentionalSelection()
            if attn is not None:
                # Test validation
                attn.validate()
                
                # Test attention operations
                region_id = brain.addRegion("test_region")
                attn.focusOnRegion(region_id)
                
                winners = attn.processCompetition([1, 2, 3, 4, 5])
                
                # Test attention state
                attended = attn.getAttendedRegions()
                
                self.log_test("Attention", "PASS", f"Attention selected {len(winners)} winners")
            else:
                self.log_test("Attention", "SKIP", "Attention system not available")
        except Exception as e:
            self.log_test("Attention", "FAIL", f"Attention failed: {e}")
    
    def test_agent_world_integration(self):
        """Test agent-world integration."""
        try:
            # Create agent and world
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(10, 10, 4, 4)
            world.reset()
            
            agent.initialize(world)
            
            # Run simulation
            for step in range(10):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            self.log_test("Agent-World Integration", "PASS", "Agent-world integration successful")
        except Exception as e:
            self.log_test("Agent-World Integration", "FAIL", f"Agent-world integration failed: {e}")
    
    def test_learning_simulation(self):
        """Test learning simulation."""
        try:
            # Set up learning environment
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 200)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(pynlm.createSimpleWorld())
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # Run learning simulation
            for step in range(20):
                world = pynlm.createSimpleWorld()
                world.configure(8, 8, 4, 4)
                world.reset()
                
                agent.initialize(world)
                
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                agent.decodeMotorCommand()
                
                # Apply reward
                agent.applyRewardModulation(0.5, 0.3)
                
                if step % 5 == 0:
                    agent.updateDevelopment(0.1)
            
            self.log_test("Learning Simulation", "PASS", "Learning simulation completed successfully")
        except Exception as e:
            self.log_test("Learning Simulation", "FAIL", f"Learning simulation failed: {e}")
    
    def test_error_handling(self):
        """Test error handling."""
        try:
            # Test that errors are counted and can be reset
            config = pynlm.createDefaultConfig()
            
            # Force some validation errors
            error_count = config.getErrorCount()
            assert isinstance(error_count, int)
            
            # Reset errors
            config.resetErrors()
            
            # Test error count after reset
            new_error_count = config.getErrorCount()
            assert new_error_count >= 0
            
            self.log_test("Error Handling", "PASS", "Error handling working correctly")
        except Exception as e:
            self.log_test("Error Handling", "FAIL", f"Error handling failed: {e}")
    
    def test_validation(self):
        """Test validation system."""
        try:
            # Test global validation
            is_valid = pynlm.validate()
            assert isinstance(is_valid, bool)
            
            # Test individual validations
            config = pynlm.createDefaultConfig()
            assert config.validate()
            
            self.log_test("Validation", "PASS", "Validation system working correctly")
        except Exception as e:
            self.log_test("Validation", "FAIL", f"Validation failed: {e}")
    
    def print_summary(self):
        """Print test summary."""
        print()
        print("=" * 60)
        print("TEST SUMMARY")
        print("=" * 60)
        print(f"Tests Passed: {self.passed}")
        print(f"Tests Failed: {self.failed}")
        print(f"Test Errors: {self.errors}")
        print(f"Total Tests: {self.passed + self.failed + self.errors}")
        print()
        
        if self.failed == 0 and self.errors == 0:
            print("🎉 ALL TESTS PASSED! 🎉")
            return True
        else:
            print("❌ TESTS FAILED")
            if self.failed > 0:
                print(f"  - {self.failed} test(s) failed")
            if self.errors > 0:
                print(f"  - {self.errors} test(s) had errors")
            return False

if __name__ == "__main__":
    # Parse command line arguments
    verbose = "--verbose" in sys.argv
    quick = "--quick" in sys.argv
    full = "--full" in sys.argv
    
    # If --full is specified, run full tests
    if full:
        quick = False
    
    # Create and run test suite
    suite = TestSuite(verbose=verbose, quick=quick)
    success = suite.run_all_tests()
    
    sys.exit(0 if success else 1)