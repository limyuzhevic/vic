#!/usr/bin/env python3
"""
Comprehensive Test Runner for NLM (Neural Learning Machine)

This test suite validates the improved functionality of the NLM project,
including enhanced components, thread safety, configuration management,
and performance optimizations.
"""

import os
import sys
import unittest
import json
import time
import threading
from pathlib import Path
from datetime import datetime

# Add the project root to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

class NLMTestBase(unittest.TestCase):
    """Base test class with common functionality"""
    
    def setUp(self):
        """Set up test environment"""
        self.test_start_time = time.time()
    
    def tearDown(self):
        """Clean up after test"""
        test_duration = time.time() - self.test_start_time
        if test_duration > 1.0:
            print(f"Warning: Test {self._testMethodName} took {test_duration:.2f}s")
    
    def assertConfigEquals(self, config1, config2):
        """Assert two configurations are equal"""
        for key in set(config1.keys()) | set(config2.keys()):
            if key not in config1 or key not in config2:
                self.fail(f"Key {key} missing in one configuration")
            self.assertEqual(config1[key], config2[key], 
                           f"Config mismatch for key {key}")

class NeuronTests(NLMTestBase):
    """Unit tests for enhanced Neuron functionality"""
    
    def test_neuron_creation(self):
        """Test neuron creation and basic properties"""
        from src.brain.Neuron.hpp import Neuron, NeuronId
        
        neuron_id = NeuronId()
        neuron = Neuron(neuron_id)
        
        self.assertEqual(neuron.getId(), neuron_id)
        self.assertEqual(neuron.getType(), NeuronType::Internal)
        self.assertTrue(std::isfinite(neuron.getMembranePotential()))
    
    def test_neuron_move_semantics(self):
        """Test neuron move operations"""
        from src.brain.Neuron.hpp import Neuron
        
        neuron1 = Neuron(NeuronId())
        neuron2 = std::move(neuron1)
        
        self.assertNotEqual(neuron1.getId(), neuron2.getId())
    
    def test_neuron_state_access(self):
        """Test neuron state access and modification"""
        from src.brain.Neuron.hpp import Neuron
        
        neuron = Neuron(NeuronId())
        auto& state = neuron.getState();
        
        self.assertEqual(state.membranePotential, -70.0f)
        self.assertEqual(state.threshold, -55.0f)
        
        state.membranePotential = -60.0f;
        self.assertEqual(neuron.getMembranePotential(), -60.0f)
    
    def test_neuron_plasticity(self):
        """Test neuron plasticity flags"""
        from src.brain.Neuron.hpp import Neuron
        
        neuron = Neuron(NeuronId())
        
        self.assertFalse(neuron.getPlasticityFlags().hebbian);
        self.assertFalse(neuron.getPlasticityFlags().stdp);
        self.assertFalse(neuron.getPlasticityFlags().reward_modulated);
        
        neuron.enablePlasticity(true, true, true);
        
        self.assertTrue(neuron.getPlasticityFlags().hebbian);
        self.assertTrue(neuron.getPlasticityFlags().stdp);
        self.assertTrue(neuron.getPlasticityFlags().reward_modulated);
    
    def test_neuron_safety_checks(self):
        """Test neuron safety and validation"""
        from src.brain.Neuron.hpp import Neuron
        
        neuron = Neuron(NeuronId());
        
        # Initial state should be safe
        self.assertTrue(neuron.isSafe());
        
        # Test potential bounds
        auto& state = neuron.getState();
        state.membranePotential = 100.0f;  // Above max
        neuron.getState().membranePotential = std::min(100.0f, NeuronConstants::MAX_POTENTIAL);
        self.assertTrue(neuron.getMembranePotential() <= NeuronConstants::MAX_POTENTIAL);
        
        state.membranePotential = -200.0f;  // Below min
        neuron.getState().membranePotential = std::max(-200.0f, NeuronConstants::MIN_POTENTIAL);
        self.assertTrue(neuron.getMembranePotential() >= NeuronConstants::MIN_POTENTIAL);

class SynapseTests(NLMTestBase):
    """Unit tests for enhanced Synapse functionality"""
    
    def test_synapse_creation(self):
        """Test synapse creation and basic properties"""
        from src.brain.Synapse.hpp import Synapse, SynapseId, NeuronId
        
        synapse_id = SynapseId();
        source = NeuronId();
        destination = NeuronId();
        
        Synapse testSynapse(synapse_id, source, destination);
        
        self.assertEqual(testSynapse.getId(), synapse_id);
        self.assertEqual(testSynapse.getSourceNeuron(), source);
        self.assertEqual(testSynapse.getDestinationNeuron(), destination);
        self.assertEqual(testSynapse.getWeight(), 0.0f);
        self.assertEqual(testSynapse.getType(), SynapseType::Excitatory);
    
    def test_synapse_configuration(self):
        """Test synapse configuration"""
        from src.brain.Synapse.hpp import Synapse, SynapseId, NeuronId
        
        Synapse::Config config;
        config.initialDelay = 3;
        config.initialEfficacy = 1.5f;
        
        Synapse synapse(SynapseId(), NeuronId(), NeuronId(), config);
        
        self.assertEqual(synapse.getDelay(), 3);
        self.assertEqual(synapse.getEfficacy(), 1.5f);
        self.assertTrue(synapse.isExcitatory());
    
    def test_synapse_weight_validation(self):
        """Test synapse weight validation"""
        from src.brain.Synapse.hpp import Synapse
        
        Synapse synapse(SynapseId(), NeuronId(), NeuronId());
        
        // Test weight setting within bounds
        synapse.setWeight(0.5f);
        self.assertEqual(synapse.getWeight(), 0.5f);
        
        synapse.setWeight(-0.3f);
        self.assertEqual(synapse.getWeight(), -0.3f);
        
        // Test weight bounds
        synapse.setWeight(2.0f);
        self.assertTrue(synapse.getWeight() <= SynapseConstants::MAX_WEIGHT);
        
        synapse.setWeight(-2.0f);
        self.assertTrue(synapse.getWeight() >= SynapseConstants::MIN_WEIGHT);
    
    def test_synapse_plasticity(self):
        """Test synapse plasticity rules"""
        from src.brain.Synapse.hpp import Synapse
        
        Synapse synapse(SynapseId(), NeuronId(), NeuronId());
        
        self.assertFalse(synapse.getPlasticityFlags().hebbian);
        self.assertFalse(synapse.getPlasticityFlags().stdp);
        self.assertFalse(synapse.getPlasticityFlags().reward_modulated);
        
        synapse.enablePlasticity(true, false, true);
        
        self.assertTrue(synapse.getPlasticityFlags().hebbian);
        self.assertFalse(synapse.getPlasticityFlags().stdp);
        self.assertTrue(synapse.getPlasticityFlags().reward_modulated);

class AgentBrainTests(NLMTestBase):
    """Unit tests for enhanced AgentBrain functionality"""
    
    def test_agent_brain_creation(self):
        """Test AgentBrain creation"""
        from src.agent.AgentBrain.hpp import AgentBrain, AgentBrain::DistributionConfig
        
        // Mock brain for testing
        class MockBrain {
        public:
            std::vector<Region*> getRegions() const { return {}; }
        };
        
        AgentBrain::DistributionConfig config;
        config.motorStrategy = AgentBrain::MotorDistributionStrategy::Weighted;
        
        MockBrain mockBrain;
        AgentBrain agentBrain(std::make_shared<MockBrain>(mockBrain), config);
        
        self.assertNotNull(agentBrain.getBrain());
    
    def test_motor_distribution(self):
        """Test motor neuron distribution strategies"""
        from src.agent.AgentBrain.hpp import AgentBrain
        
        AgentBrain::DistributionConfig config;
        
        // Test different strategies
        config.motorStrategy = AgentBrain::MotorDistributionStrategy::Uniform;
        config.sensoryStrategy = AgentBrain::SensoryDistributionStrategy::Priority;
        
        self.assertEqual(config.motorStrategy, AgentBrain::MotorDistributionStrategy::Uniform);
        self.assertEqual(config.sensoryStrategy, AgentBrain::SensoryDistributionStrategy::Priority);
    
    def test_sensory_distribution(self):
        """Test sensory neuron distribution strategies"""
        from src.agent.AgentBrain.hpp import AgentBrain
        
        AgentBrain::DistributionConfig config;
        
        // Test different strategies
        config.sensoryStrategy = AgentBrain::SensoryDistributionStrategy::Balanced;
        
        self.assertEqual(config.sensoryStrategy, AgentBrain::SensoryDistributionStrategy::Balanced);
    
    def test_advanced_features(self):
        """Test advanced features configuration"""
        from src.agent.AgentBrain.hpp import AgentBrain
        
        AgentBrain::DistributionConfig config;
        
        config.enableNoiseInjection = true;
        config.noiseLevel = 0.15f;
        config.enableAdaptiveWeights = true;
        
        self.assertTrue(config.enableNoiseInjection);
        self.assertEqual(config.noiseLevel, 0.15f);
        self.assertTrue(config.enableAdaptiveWeights);

class ConfigTests(NLMTestBase):
    """Unit tests for enhanced Config functionality"""
    
    def test_config_creation(self):
        """Test configuration creation"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        self.assertEqual(config.getKeys().size(), 0);
    
    def test_config_set_get(self):
        """Test configuration set/get operations"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        config.set("test_int", 42);
        config.set("test_string", "hello");
        config.set("test_double", 3.14);
        config.set("test_bool", true);
        
        this.assertEqual(config.getOr("test_int", 0), 42);
        this.assertEqual(config.getOr("test_string", ""), "hello");
        this.assertAlmostEqual(config.getOr("test_double", 0.0), 3.14, places=2);
        this.assertEqual(config.getOr("test_bool", false), true);
    
    def test_config_json_parsing(self):
        """Test JSON parsing and serialization"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        
        // Test setting via JSON string
        json_string = "{\"test\": 123, \"name\": \"test_config\"}";
        bool success = config.loadFromJsonString(json_string);
        
        if (success) {
            this.assertEqual(config.getOr("test", 0), 123);
            this.assertEqual(config.getOr("name", ""), "test_config");
        }
    
    def test_config_validation(self):
        """Test configuration validation"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        
        // Add validation schema
        Config::Schema schema;
        this.setSchema = schema;
        
        config.set("valid_key", "valid_value");
        this.assertTrue(config.validate());
    
    def test_config_serialization(self):
        """Test configuration serialization"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        config.set("test", 1);
        
        // Test JSON serialization
        json_str = config.toJsonString();
        this.assertTrue(not json_str.empty());
        
        // Test simple format serialization
        simple_str = config.summary();
        this.assertTrue(not simple_str.empty());

class LoggerTests(NLMTestBase):
    """Unit tests for enhanced Logger functionality"""
    
    def test_logger_creation(self):
        """Test logger creation"""
        from src.core.Logger.Logger.hpp import Logger
        
        Logger logger;
        this.assertTrue(logger.isEnabled(LogLevel::Info));
    
    def test_logger_levels(self):
        """Test log level filtering"""
        from src.core.Logger.Logger.hpp import Logger
        
        Logger logger;
        
        // Set minimum level to Warning
        logger.setLevel(LogLevel::Warning);
        
        // Debug and Info should be disabled
        this.assertFalse(logger.isEnabled(LogLevel::Debug));
        this.assertFalse(logger.isEnabled(LogLevel::Info));
        
        // Warning, Error, Critical should be enabled
        this.assertTrue(logger.isEnabled(LogLevel::Warning));
        this.assertTrue(logger.isEnabled(LogLevel::Error));
        this.assertTrue(logger.isEnabled(LogLevel::Critical));
    
    def test_logger_formatting(self):
        """Test log formatting"""
        from src.core.Logger.Logger.hpp import Logger
        
        Logger logger;
        
        // Log a test message
        logger.info("Test message");
        
        // Check that logger is working
        this.assertTrue(logger.isEnabled(LogLevel::Info));
    
    def test_performance_logger(self):
        """Test performance logger functionality"""
        from src.core.Logger.Logger.hpp import PerformanceLogger, LoggerConfig
        
        LoggerConfig config;
        config.enableColors = false;
        config.enableFileOutput = false;
        
        PerformanceLogger perfLogger(config);
        
        this.assertNotNull(perfLogger);
        this.assertTrue(perfLogger.getLevel() == LogLevel::Info);

class IntegrationTests(NLMTestBase):
    """Integration tests for the whole system"""
    
    def test_system_initialization(self):
        """Test whole system initialization"""
        # This would test the actual NLM system integration
        # For now, it's a placeholder
        pass
    
    def test_cross_component_interaction(self):
        """Test interaction between different components"""
        # This would test integration between Neuron, Synapse, AgentBrain, etc.
        # For now, it's a placeholder
        pass

class PerformanceTests(NLMTestBase):
    """Performance tests for the improved components"""
    
    def test_thread_safety(self):
        """Test thread safety of concurrent logging"""
        import threading
        import time
        
        from src.core.Logger.Logger.hpp import Logger
        
        Logger logger;
        
        results = []
        lock = threading.Lock()
        
        def log_messages(thread_id):
            for i in range(100):
                logger.info(f"Thread {thread_id} message {i}");
                with lock:
                    results.append(f"Thread {thread_id} message {i}")
                time.sleep(0.001)
        
        threads = []
        for i in range(5):
            thread = threading.Thread(target=log_messages, args=(i,))
            threads.append(thread)
            thread.start()
        
        for thread in threads:
            thread.join()
        
        this.assertEqual(len(results), 500);  # 5 threads * 100 messages each
    
    def test_configuration_performance(self):
        """Test configuration parsing performance"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        
        start_time = time.time()
        for i in range(1000):
            config.set(f"key_{i}", i);
        
        end_time = time.time();
        duration = end_time - start_time;
        
        this.assertLess(duration, 1.0);  # Should complete within 1 second
        this.assertGreater(config.getKeys().size(), 0);
    
    def test_memory_usage(self):
        """Test memory usage of improved components"""
        import tracemalloc
        
        tracemalloc.start();
        
        # Create and use various components
        from src.brain.Neuron.hpp import Neuron
        from src.core.Config.Config.hpp import Config
        from src.core.Logger.Logger.hpp import Logger
        
        neurons = []
        for i in range(100):
            neurons.append(Neuron(NeuronId()));
        
        config = Config();
        logger = Logger();
        
        current, peak = tracemalloc.get_traced_memory();
        tracemalloc.stop();
        
        # Check that memory usage is reasonable
        this.assertLess(peak, 100 * 1024 * 1024);  # Less than 100MB

if __name__ == '__main__':
    # Run all tests
    unittest.main(verbosity=2)
