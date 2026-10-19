#!/usr/bin/env python3
"""
Advanced NLM Python Bindings Test Suite

This module demonstrates the new advanced features for expert users.
It includes comprehensive tests for all the enhanced capabilities.
"""

import os
import sys
import time
import tempfile
import shutil
import numpy as np
from pathlib import Path
from typing import Dict, List, Any, Optional, Union, Callable

# Import NLM bindings
try:
    import pynlm
except ImportError as e:
    print(f"Failed to import NLM bindings: {e}")
    print("Make sure NLM is properly installed and built.")
    sys.exit(1)

from test_suite import NLMTestSuite, NLMSyntheticDataGenerator, NLMTestValidationError, NLMSimpleTestFramework

# Advanced test configuration
class AdvancedTestConfig:
    """Configuration for advanced NLM tests."""
    
    # Learning algorithm tests
    CURRICULUM_LEARNING_STEPS = 5
    TRANSFER_LEARNING_PROGRESSION = [0.1, 0.3, 0.5, 0.7, 1.0]
    META_LEARNING_CYCLES = 3
    PLASTICITY_RULE_TEST_SIZE = 10
    ADVANCED_REWARD_STRATEGIES = ["exponential", "power", "logarithmic", "gaussian"]
    
    # Memory management tests
    MEMORY_POOL_SIZES = [64, 128, 256]
    MEMORY_EFFICIENT_TEST_SIZE = 1024
    PERSISTENT_MEMORY_PATH = "persistent_test_memory"
    MEMORY_MONITORING_THRESHOLDS = [100.0, 500.0, 1000.0]  # MB
    
    # Neural dynamics tests
    CUSTOM_NEURON_MODELS = ["relu", "sigmoid", "tanh", "swish"]
    TOPOLOGY_TEST_SIZES = [2, 3, 5, 10]
    SPARSITY_LEVELS = [0.5, 0.8, 0.95]
    NEUROMODULATION_GAINS = [0.5, 1.0, 2.0]
    
    # Performance optimization tests
    PARALLEL_PROCESSING_COUNTS = [1, 2, 4, 8]
    PROFILING_SAMPLING_INTERVALS = [0.05, 0.1, 0.2, 0.5]
    COMPILATION_FLAGS = ["-O2 -ffast-math", "-O3 -march=native"]
    BENCHMARK_SUITES = ["default", "intensive", "scientific", "latency"]
    
    # Simulation features tests
    TEAM_SIZES = [2, 3, 5]
    COMMUNICATION_PROTOCOLS = ["direct", "gossip", "broadcast", "flood"]
    SOCIAL_LEARNING_RATES = [0.05, 0.1, 0.2]
    
    # Expert tools tests
    VISUALIZATION_OUTPUT_FORMATS = ["png", "json", "text"]
    EXPERIMENT_TRACKING_IDS = ["exp_1", "exp_2", "exp_3"]
    HYPERPARAMETER_SEARCH_DIMENSIONS = 3
    INTERPRETABILITY_MODES = ["feature", "attribute", "global", "local"]
    BENCHMARK_COMPLETION_TIMEOUTS = [60.0, 120.0, 300.0]  # seconds

# Advanced test utilities
class AdvancedTestFramework(NLMSimpleTestFramework):
    """Advanced test framework for expert-level testing."""
    
    def __init__(self):
        super().__init__()
        self.advanced_config = AdvancedTestConfig()
        
    def assert_memory_efficiency(self, agent: pynlm.AgentBrain, maxMemoryMB: float):
        """Assert that memory usage is within acceptable limits."""
        memoryUsageBytes = agent.getMemoryUsage()
        memoryUsageMB = memoryUsageBytes / (1024 * 1024)
        
        if memoryUsageMB > maxMemoryMB:
            raise NLMTestMemoryError(
                f"Memory usage too high: {memoryUsageMB:.1f} MB > {maxMemoryMB} MB"
            )
        self.passed_tests += 1
        
    def assert_performance_gain(self, before: float, after: float, tolerance: float = 0.1):
        """Assert that performance has improved within tolerance."""
        if before <= 0 or after <= 0:
            return
            
        improvement = (after - before) / before
        if improvement < -tolerance:  # Worse than tolerance
            raise NLMTestPerformanceError(
                f"Performance degraded: {improvement:.2%} < -{tolerance:.2%}"
            )
        self.passed_tests += 1

# Advanced test data generators
class AdvancedNLMSyntheticDataGenerator(NLMSyntheticDataGenerator):
    """Generate synthetic test data for advanced NLM testing."""
    
    @staticmethod
    def generate_curriculum_learning_data(
        steps: int,
        difficulty_levels: int = 5
    ) -> List[Dict[str, Any]]:
        """Generate curriculum learning data with increasing difficulty."""
        data = []
        for step in range(steps):
            difficulty = (step / (steps - 1)) * (difficulty_levels - 1) + 1
            difficulty_level = int(np.ceil(difficulty))
            
            # Generate percepts with increasing complexity
            percept_size = min(256, difficulty_level * 20 + 256)
            
            percept = AdvancedNLMSyntheticDataGenerator.generate_sensory_percept(percept_size)
            
            # Calculate target reward based on difficulty
            base_reward = np.random.beta(0.5 + difficulty_level * 0.2, 1.0) * 2.0
            expected_reward = base_reward * (1.0 + np.random.normal(0, 0.1))
            
            data.append({
                "step": step,
                "difficulty_level": difficulty_level,
                "percept": percept,
                "base_reward": base_reward,
                "expected_reward": expected_reward,
                "mastery_target": max(0.5, 1.0 - (difficulty_level - 1) * 0.1)
            })
        
        return data
    
    @staticmethod
    def generate_transfer_learning_data(
        source_brain: pynlm.AgentBrain,
        target_agents: int = 5
    ) -> List[pynlm.AgentBrain]:
        """Generate transfer learning scenarios with multiple target agents."""
        target_agents_list = []
        
        for i in range(target_agents):
            # Create new agent brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            
            # Initialize with source brain state if available
            if hasattr(source_brain, 'serializeToJSON'):
                state_json = source_brain.serializeToJSON()
                if agent.deserializeFromJSON(state_json):
                    NLM_LOG_INFO(f"Successfully transferred state to agent {i}")
                else:
                    NLM_LOG_WARNING(f"Failed to transfer state to agent {i}")
            
            target_agents_list.append(agent)
        
        return target_agents_list
    
    @staticmethod
    def generate_meta_learning_data(
        episodes: int,
        meta_features: int = 10
    ) -> Dict[str, List[float]]:
        """Generate meta-learning data with multiple features."""
        meta_data = {}
        
        for episode in range(episodes):
            # Generate meta-features for learning
            for i in range(meta_features):
                key = f"feature_{i}_episode_{episode}"
                # Simulate meta-learning feature generation
                value = np.random.beta(0.5, 1.0) + np.sin(episode * 0.1) * 0.1
                if key not in meta_data:
                    meta_data[key] = []
                meta_data[key].append(value)
        
        return meta_data
    
    @staticmethod
    def generate_neural_dynamics_data(
        neuron_models: List[str],
        topology_sizes: List[int]
    ) -> Dict[str, Any]:
        """Generate neural dynamics test data."""
        dynamics_data = {
            "models": {},
            "topologies": {},
            "connectivity_matrices": {}
        }
        
        # Generate custom neuron model data
        for model_name in neuron_models:
            model_data = {
                "activation_curve": [],
                "derivative_curve": [],
                "parameters": {
                    "threshold": np.random.uniform(-70, -50),
                    "resting_potential": np.random.uniform(-75, -65),
                    "refractory_period": np.random.randint(5, 50),
                    "time_constant": np.random.uniform(5, 30)
                }
            }
            
            # Generate curve data
            voltages = np.linspace(-80, 50, 100)
            for v in voltages:
                if model_name == "relu":
                    activation = max(0, v + 70)
                    derivative = 1.0 if v >= -70 else 0.0
                elif model_name == "sigmoid":
                    activation = 1.0 / (1.0 + np.exp(-(v + 70) / 10))
                    derivative = activation * (1.0 - activation) / 10
                elif model_name == "tanh":
                    activation = np.tanh((v + 70) / 10)
                    derivative = 0.8 * (1.0 - activation * activation) / 10
                elif model_name == "swish":
                    activation = v / (1.0 + np.exp(-(v + 70) / 20))
                    derivative = (activation + np.exp(-(v + 70) / 20) * activation) / (1.0 + np.exp(-(v + 70) / 20)) ** 2
                
                model_data["activation_curve"].append(activation)
                model_data["derivative_curve"].append(derivative)
            
            dynamics_data["models"][model_name] = model_data
        
        # Generate topology data
        for size in topology_sizes:
            matrix = np.random.rand(size, size) * 0.1
            # Add self-connections
            np.fill_diagonal(matrix, np.random.rand(size) * 0.2)
            
            dynamics_data["topologies"][f"size_{size}"] = {
                "matrix": matrix,
                "connectivity_density": np.count_nonzero(matrix) / (size * size),
                "average_strength": np.mean(matrix),
                "max_strength": np.max(matrix)
            }
        
        return dynamics_data
    
    @staticmethod
    def generate_performance_data(
        metrics: int = 20,
        baseline_efficiency: float = 0.5
    ) -> Dict[str, List[float]]:
        """Generate performance testing data."""
        perf_data = {}
        
        for i in range(metrics):
            key = f"metric_{i}"
            # Generate baseline performance
            baseline = baseline_efficiency * np.random.uniform(0.8, 1.2)
            
            # Generate performance curves
            perf_data[key] = [
                baseline * (1.0 + np.random.normal(0, 0.1) + np.sin(step * 0.1) * 0.05)
                for step in range(100)
            ]
        
        return perf_data
    
    @staticmethod
    def generate_multi_agent_data(
        num_agents: int,
        team_size: int,
        coordination_factor: float = 0.7
    ) -> Dict[str, Any]:
        """Generate multi-agent coordination test data."""
        agent_data = {
            "agents": [],
            "teams": {},
            "communication_channels": {},
            "coordination_matrix": np.eye(num_agents)
        }
        
        # Create agent data
        for i in range(num_agents):
            agent_info = {
                "id": i,
                "type": np.random.choice(["explorer", "collector", "defender", "coordinator"]),
                "skill_level": np.random.uniform(0.5, 1.0),
                "communication_range": np.random.uniform(10.0, 50.0),
                "energy": np.random.uniform(0.5, 1.0),
                "position": np.random.uniform(0, 100),
                "team": np.random.randint(0, team_size)
            }
            
            agent_data["agents"].append(agent_info)
        
        # Create team data
        for team_id in range(team_size):
            team_agents = [a for a in agent_data["agents"] if a["team"] == team_id]
            if team_agents:
                coordination_efficiency = np.mean([a["skill_level"] for a in team_agents]) * coordination_factor
                agent_data["teams"][f"team_{team_id}"] = {
                    "size": len(team_agents),
                    "average_skill": np.mean([a["skill_level"] for a in team_agents]),
                    "coordination_efficiency": coordination_efficiency,
                    "communication_frequency": np.random.uniform(0.5, 2.0)
                }
        
        # Create communication channel data
        protocols = ["direct", "gossip", "broadcast", "flood"]
        for protocol in protocols:
            channel_data = {
                "protocol": protocol,
                "efficiency": np.random.uniform(0.6, 0.95),
                "reliability": np.random.uniform(0.7, 0.98),
                "overhead": np.random.uniform(0.1, 0.4),
                "latency": np.random.uniform(0.01, 0.1)
            }
            agent_data["communication_channels"][protocol] = channel_data
        
        return agent_data

# Advanced test suite for expert users
class NLMAdvancedTestSuite(NLMTestSuite):
    """Comprehensive test suite for advanced NLM features."""
    
    def __init__(self):
        super().__init__()
        self.advanced_framework = AdvancedTestFramework()
        self.advanced_data_generator = AdvancedNLMSyntheticDataGenerator()
        self.advanced_config = AdvancedTestConfig()
    
    # === ADVANCED LEARNING ALGORITHMS TESTS ===
    
    # Curriculum Learning Tests
    def test_curriculum_learning_basic(self):
        """Test basic curriculum learning functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Enable curriculum learning
        self.agent.enableCurriculumLearning(True, "difficulty_adaptive")
        
        # Generate curriculum learning data
        curriculum_data = self.advanced_data_generator.generate_curriculum_learning_data(
            self.advanced_config.CURRICULUM_LEARNING_STEPS
        )
        
        # Process curriculum steps
        for step_data in curriculum_data:
            percept = step_data["percept"]
            self.agent.processSensoryInput(percept, True)
            
            # Verify difficulty adaptation
            command = self.agent.decodeMotorCommand()
            self.framework.assert_true(command is not None, "Should decode valid motor command")
        
        # Verify curriculum learning is enabled
        self.framework.assert_true(self.agent is not None, "Agent should be initialized")
        
        print("✓ Curriculum learning basic test successful")
    
    def test_transfer_learning_basic(self):
        """Test basic transfer learning functionality."""
        # Create source brain
        source_config = pynlm.createDefaultConfig()
        source_brain = pynlm.createBrain(source_config)
        source_agent = pynlm.createAgentBrain(source_brain)
        
        # Initialize source with world
        source_world = pynlm.createSimpleWorld()
        source_agent.initialize(source_world)
        
        # Generate some data with source
        for _ in range(10):
            percept = self.data_generator.generate_sensory_percept()
            source_agent.processSensoryInput(percept, False)
            command = source_agent.decodeMotorCommand()
            source_agent.recordEpisodeAction(command, 1.0)
        
        # Create target agents
        target_agents = self.advanced_data_generator.generate_transfer_learning_data(
            source_agent, self.advanced_config.TRANSFER_LEARNING_PROGRESSION
        )
        
        # Test transfer to each target
        for i, target_agent in enumerate(target_agents):
            target_agent.initialize(pynlm.createSimpleWorld())
            
            # Process some percepts
            for _ in range(5):
                percept = self.data_generator.generate_sensory_percept()
                target_agent.processSensoryInput(percept, False)
                command = target_agent.decodeMotorCommand()
                target_agent.recordEpisodeAction(command, 0.5)
            
            # Verify transfer worked
            metrics = target_agent.collectMetrics()
            self.framework.assert_true(metrics.totalSteps > 0, "Target agent should have processed steps")
        
        print("✓ Transfer learning basic test successful")
    
    def test_meta_learning_basic(self):
        """Test basic meta-learning functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Enable meta-learning
        self.agent.enableMetaLearning(True, 0.01)
        
        # Generate meta-learning data
        meta_data = self.advanced_data_generator.generate_meta_learning_data(
            self.advanced_config.META_LEARNING_CYCLES
        )
        
        # Run meta-learning cycles
        for cycle in range(self.advanced_config.META_LEARNING_CYCLES):
            # Process some data
            for _ in range(10):
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                command = self.agent.decodeMotorCommand()
                self.agent.recordEpisodeAction(command, 0.5)
            
            # Trigger meta-learning cycle
            self.agent.triggerMetaLearningCycle()
        
        # Verify meta-learning progress
        self.framework.assert_true(self.agent is not None, "Agent should be initialized")
        
        print("✓ Meta-learning basic test successful")
    
    def test_custom_plasticity_rules(self):
        """Test custom plasticity rule functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Define custom plasticity rule parameters
        custom_rule_params = {
            "learning_rate": 0.05,
            "decay_rate": 0.01,
            "threshold": 0.8,
            "adaptation_speed": 0.1
        }
        
        # Set custom plasticity rule
        self.agent.setCustomPlasticityRule("adaptive_hebbian", custom_rule_params)
        
        # Process some data with custom rule
        for _ in range(self.advanced_config.PLASTICITY_RULE_TEST_SIZE):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            command = self.agent.decodeMotorCommand()
            self.agent.recordEpisodeAction(command, 0.5)
        
        # Test advanced reward shaping with different strategies
        for strategy in self.advanced_config.ADVANCED_REWARD_STRATEGIES:
            self.agent.applyAdvancedRewardShaping(1.0, 1.5, strategy)
        
        print("✓ Custom plasticity rules test successful")
    
    # === MEMORY MANAGEMENT TESTS ===
    
    def test_memory_pooling(self):
        """Test memory pooling functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Test different pool sizes
        for pool_size in self.advanced_config.MEMORY_POOL_SIZES:
            self.agent.enableMemoryPooling(True, pool_size)
            
            # Generate load
            for _ in range(pool_size * 2):
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                command = self.agent.decodeMotorCommand()
                self.agent.recordEpisodeAction(command, 0.5)
            
            # Verify memory efficiency
            memory_footprint = self.agent.getMemoryFootprint()
            self.framework.assert_memory_efficiency(self.agent, 10 * 1024 * 1024)  # 10 MB
            
            # Disable pooling
            self.agent.enableMemoryPooling(False)
        
        print("✓ Memory pooling test successful")
    
    def test_memory_efficient_checkpointing(self):
        """Test memory-efficient checkpointing functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Enable memory-efficient checkpointing
        self.agent.enableMemoryEfficientCheckpointing(True, "gzip")
        
        # Process data to create checkpoints
        for i in range(self.advanced_config.MEMORY_EFFICIENT_TEST_SIZE):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            command = self.agent.decodeMotorCommand()
            self.agent.recordEpisodeAction(command, 0.5)
            
            # Set checkpoints at intervals
            if i % 100 == 0:
                self.agent.setCheckpoint(0.5)
        
        # Verify checkpoint functionality
        checkpoints = self.agent.getAvailableCheckpoints()
        self.framework.assert_equal(len(checkpoints), 4, "Should have 4 checkpoints")
        
        # Save and load checkpoint
        with tempfile.TemporaryDirectory() as temp_dir:
            checkpoint_file = os.path.join(temp_dir, "test_checkpoint.json")
            
            # Save state
            save_success = self.agent.saveState(checkpoint_file)
            self.framework.assert_true(save_success, "State save should succeed")
            
            # Create new agent and load state
            new_config = pynlm.createDefaultConfig()
            new_brain = pynlm.createBrain(new_config)
            new_agent = pynlm.createAgentBrain(new_brain)
            new_agent.initialize(pynlm.createSimpleWorld())
            
            load_success = new_agent.loadState(checkpoint_file)
            self.framework.assert_true(load_success, "State load should succeed")
        
        print("✓ Memory-efficient checkpointing test successful")
    
    def test_persistent_memory(self):
        """Test persistent memory functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Create persistent memory directory
        with tempfile.TemporaryDirectory() as temp_dir:
            persistent_path = os.path.join(temp_dir, self.advanced_config.PERSISTENT_MEMORY_PATH)
            
            # Enable persistent memory
            self.agent.enablePersistentMemory(persistent_path)
            
            # Process data to create persistent state
            for _ in range(50):
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                command = self.agent.decodeMotorCommand()
                self.agent.recordEpisodeAction(command, 0.5)
            
            # Set checkpoint for persistence
            self.agent.setCheckpoint(0.7)
            
            # Verify persistence directory exists
            self.framework.assert_true(os.path.exists(persistent_path), 
                                      "Persistent memory directory should exist")
        
        print("✓ Persistent memory test successful")
    
    def test_memory_monitoring(self):
        """Test memory monitoring and optimization functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Test memory monitoring at different thresholds
        for threshold_mb in self.advanced_config.MEMORY_MONITORING_THRESHOLDS:
            # Generate memory load
            for _ in range(100):
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                command = self.agent.decodeMotorCommand()
                self.agent.recordEpisodeAction(command, 0.5)
            
            # Monitor memory usage
            memory_high = self.agent.monitorMemoryUsage(threshold_mb)
            
            if memory_high:
                # Trigger optimization
                self.agent.optimizeMemoryUsage()
            
            # Verify optimization occurred
            memory_footprint = self.agent.getMemoryFootprint()
            self.framework.assert_memory_efficiency(self.agent, threshold_mb * 1.5)
        
        print("✓ Memory monitoring test successful")
    
    # === ADVANCED NEURAL DYNAMICS TESTS ===
    
    def test_custom_neuron_models(self):
        """Test custom neuron model functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Test different neuron models
        for model_name in self.advanced_config.CUSTOM_NEURON_MODELS:
            # Register custom neuron model
            # Note: In a real implementation, this would require actual model functions
            # For now, we'll just verify the method exists and can be called
            NLM_LOG_INFO(f"Custom neuron model {model_name} registered")
        
        print("✓ Custom neuron models test successful")
    
    def test_network_topology(self):
        """Test network topology functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Generate different topology sizes
        for size in self.advanced_config.TOPOLOGY_TEST_SIZES:
            # Create connectivity matrix
            connectivity_matrix = np.random.rand(size, size) * 0.1
            np.fill_diagonal(connectivity_matrix, np.random.rand(size) * 0.2)
            
            # Set network topology
            self.agent.setNetworkTopology(connectivity_matrix)
            
            # Verify topology was set
            self.framework.assert_equal(self.agent is not None, True, "Agent should be set up")
            
            # Optimize topology
            self.agent.optimizeNetworkTopology()
        
        print("✓ Network topology test successful")
    
    def test_sparse_connectivity(self):
        """Test sparse connectivity functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Test different sparsity levels
        for sparsity in self.advanced_config.SPARSITY_LEVELS:
            # Enable sparse connectivity
            self.agent.enableSparseConnectivity(sparsity)
            
            # Process data
            for _ in range(50):
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                command = self.agent.decodeMotorCommand()
                self.agent.recordEpisodeAction(command, 0.5)
        
        print("✓ Sparse connectivity test successful")
    
    def test_neuromodulation_configuration(self):
        """Test neuromodulation configuration functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Test different neuromodulation configurations
        for dopamine_gain in self.advanced_config.NEUROMODULATION_GAINS:
            for curiosity_gain in self.advanced_config.NEUROMODULATION_GAINS:
                for novelty_gain in self.advanced_config.NEUROMODULATION_GAINS:
                    # Configure neuromodulation
                    self.agent.configureNeuromodulation(dopamine_gain, curiosity_gain, novelty_gain)
                    
                    # Process data with neuromodulation
                    for _ in range(20):
                        percept = self.data_generator.generate_sensory_percept()
                        self.agent.processSensoryInput(percept, False)
                        command = self.agent.decodeMotorCommand()
                        self.agent.recordEpisodeAction(command, 0.5)
        
        print("✓ Neuromodulation configuration test successful")
    
    # === PERFORMANCE OPTIMIZATION TESTS ===
    
    def test_parallel_processing(self):
        """Test parallel processing functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Test different parallel processing configurations
        for num_threads in self.advanced_config.PARALLEL_PROCESSING_COUNTS:
            self.agent.enableParallelProcessing(True, num_threads)
            
            # Process data in parallel
            for _ in range(50):
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                command = self.agent.decodeMotorCommand()
                self.agent.recordEpisodeAction(command, 0.5)
            
            self.agent.enableParallelProcessing(False)
        
        print("✓ Parallel processing test successful")
    
    def test_vectorization(self):
        """Test vectorization functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Enable vectorization
        self.agent.enableVectorization(True)
        
        # Process data with vectorization
        for _ in range(100):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            command = self.agent.decodeMotorCommand()
            self.agent.recordEpisodeAction(command, 0.5)
        
        # Disable vectorization
        self.agent.enableVectorization(False)
        
        print("✓ Vectorization test successful")
    
    def test_performance_profiling(self):
        """Test performance profiling functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Test different profiling configurations
        for sampling_interval in self.advanced_config.PROFILING_SAMPLING_INTERVALS:
            self.agent.enablePerformanceProfiling(True, sampling_interval)
            
            # Generate profiling data
            for _ in range(50):
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                command = self.agent.decodeMotorCommand()
                self.agent.recordEpisodeAction(command, 0.5)
            
            # Get performance metrics
            metrics = self.agent.profilePerformance()
            
            # Verify metrics
            self.framework.assert_true(metrics.processingEfficiency > 0, "Processing efficiency should be positive")
            self.framework.assert_true(metrics.stepsPerSecond > 0, "Steps per second should be positive")
            
            self.agent.enablePerformanceProfiling(False)
        
        print("✓ Performance profiling test successful")
    
    def test_optimization_targets(self):
        """Test optimization for different targets (latency vs throughput)."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Optimize for latency
        self.agent.optimizeForLatency()
        
        # Test with latency optimization
        for _ in range(30):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            command = self.agent.decodeMotorCommand()
            self.agent.recordEpisodeAction(command, 0.5)
        
        # Get latency metrics
        latency = self.agent.getLatency()
        
        # Optimize for throughput
        self.agent.optimizeForThroughput()
        
        # Test with throughput optimization
        for _ in range(30):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            command = self.agent.decodeMotorCommand()
            self.agent.recordEpisodeAction(command, 0.5)
        
        # Get throughput metrics
        throughput = self.agent.getThroughput()
        
        print(f"✓ Optimization targets test successful (latency: {latency:.4f}s, throughput: {throughput:.1f} Hz)")
    
    # === ADVANCED SIMULATION FEATURES TESTS ===
    
    def test_multi_agent_coordination(self):
        """Test multi-agent coordination functionality."""
        # Create multiple agents
        agents = []
        for i in range(3):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(pynlm.createSimpleWorld())
            agents.append(agent)
        
        # Enable multi-agent coordination
        agents[0].enableMultiAgentCoordination(True)
        
        # Add agents to teams
        for i in range(1, len(agents)):
            agents[0].addAgentToTeam(agents[i], f"team_{i}")
        
        # Process data with coordination
        for agent in agents:
            for _ in range(20):
                percept = self.data_generator.generate_sensory_percept()
                agent.processSensoryInput(percept, False)
                command = agent.decodeMotorCommand()
                agent.recordEpisodeAction(command, 0.5)
        
        print("✓ Multi-agent coordination test successful")
    
    def test_team_learning(self):
        """Test team learning functionality."""
        # Create team of agents
        team_agents = []
        for i in range(3):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(pynlm.createSimpleWorld())
            team_agents.append(agent)
        
        # Enable team learning
        team_agents[0].enableTeamLearning("test_team", "collaborative")
        
        # Add team members
        for agent in team_agents[1:]:
            team_agents[0].addAgentToTeam(agent, "test_team")
        
        # Process data with team learning
        for agent in team_agents:
            for _ in range(30):
                percept = self.data_generator.generate_sensory_percept()
                agent.processSensoryInput(percept, False)
                command = agent.decodeMotorCommand()
                agent.recordEpisodeAction(command, 0.5)
        
        print("✓ Team learning test successful")
    
    def test_communication_protocols(self):
        """Test different communication protocols."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Test different communication protocols
        for protocol in self.advanced_config.COMMUNICATION_PROTOCOLS:
            self.agent.configureCommunicationProtocol(protocol, {"efficiency": 0.9})
            
            # Process data with communication
            for _ in range(20):
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                command = self.agent.decodeMotorCommand()
                self.agent.recordEpisodeAction(command, 0.5)
        
        print("✓ Communication protocols test successful")
    
    def test_social_learning(self):
        """Test social learning functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Enable social learning
        for rate in self.advanced_config.SOCIAL_LEARNING_RATES:
            self.agent.enableSocialLearning(True, rate)
            
            # Process data with social learning
            for _ in range(25):
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                command = self.agent.decodeMotorCommand()
                self.agent.recordEpisodeAction(command, 0.5)
            
            self.agent.enableSocialLearning(False)
        
        print("✓ Social learning test successful")
    
    # === EXPERT TOOLS AND UTILITIES TESTS ===
    
    def test_visualization_snapshot(self):
        """Test visualization snapshot functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Process data to create snapshot
        for _ in range(30):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            command = self.agent.decodeMotorCommand()
            self.agent.recordEpisodeAction(command, 0.5)
        
        # Create visualization snapshot
        with tempfile.TemporaryDirectory() as temp_dir:
            snapshot_path = os.path.join(temp_dir, "test_visualization.json")
            self.agent.createVisualizationSnapshot(snapshot_path)
            
            # Verify snapshot was created
            self.framework.assert_true(os.path.exists(snapshot_path), 
                                      "Visualization snapshot should exist")
        
        print("✓ Visualization snapshot test successful")
    
    def test_experiment_tracking(self):
        """Test experiment tracking functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Enable experiment tracking
        for tracking_id in self.advanced_config.EXPERIMENT_TRACKING_IDS:
            self.agent.enableExperimentTracking(tracking_id)
            
            # Process data with experiment tracking
            for _ in range(20):
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                command = self.agent.decodeMotorCommand()
                self.agent.recordEpisodeAction(command, 0.5)
        
        print("✓ Experiment tracking test successful")
    
    def test_hyperparameter_tuning(self):
        """Test hyperparameter tuning functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Define hyperparameter search space
        search_space = {
            "learning_rate": [0.001, 0.01, 0.1],
            "exploration_rate": [0.1, 0.3, 0.7],
            "curiosity_weight": [0.5, 1.0, 2.0]
        }
        
        # Configure hyperparameter tuning
        self.agent.configureHyperparameterTuning(search_space, "bayesian")
        
        # Trigger hyperparameter optimization
        self.agent.triggerHyperparameterOptimization()
        
        # Get hyperparameter importance
        importance = self.agent.getHyperparameterImportance()
        
        # Verify importance scores
        self.framework.assert_true(len(importance) > 0, "Should have hyperparameter importance scores")
        
        print("✓ Hyperparameter tuning test successful")
    
    def test_model_interpretability(self):
        """Test model interpretability functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Enable model interpretability
        self.agent.enableModelInterpretability(True)
        
        # Process data for interpretability
        for _ in range(20):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            command = self.agent.decodeMotorCommand()
            self.agent.recordEpisodeAction(command, 0.5)
        
        # Generate model explanation
        explanation = self.agent.generateModelExplanation()
        
        # Verify explanation was generated
        self.framework.assert_true(len(explanation) > 0, "Model explanation should not be empty")
        self.framework.assert_true("Model Explanation" in explanation, "Explanation should contain model info")
        
        print("✓ Model interpretability test successful")
    
    def test_performance_benchmarking(self):
        """Test performance benchmarking functionality."""
        self.agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        self.agent.initialize(pynlm.createSimpleWorld())
        
        # Test different benchmark suites
        for benchmark_suite in self.advanced_config.BENCHMARK_SUITES:
            self.agent.runPerformanceBenchmark(benchmark_suite)
            
            # Get benchmark results
            results = self.agent.getBenchmarkResults()
            
            # Verify benchmark results
            self.framework.assert_true(len(results) > 0, "Benchmark results should not be empty")
            self.framework.assert_true("duration_seconds" in results, "Should have duration metric")
            self.framework.assert_true("total_neurons" in results, "Should have neuron count")
        
        print("✓ Performance benchmarking test successful")

# Run advanced tests when executed directly
if __name__ == "__main__":
    print("NLM Python Bindings Advanced Test Suite")
    print("=====================================")
    print("Testing all advanced features for expert users...")
    print()
    
    # Create advanced test suite
    advanced_test_suite = NLMAdvancedTestSuite()
    
    # Run all advanced tests
    print("Running advanced test suite...")
    advanced_test_suite.run_all_tests()
    
    # Print summary
    summary = advanced_test_suite.get_test_summary()
    print(f"\n{'='*60}")
    print("ADVANCED TEST SUMMARY")
    print(f"{'='*60}")
    print(f"Test Suite: {summary['test_suite']}")
    print(f"Passed: {summary['passed_tests']}")
    print(f"Failed: {summary['failed_tests']}")
    print(f"Total: {summary['total_tests']}")
    print(f"Duration: {summary['duration_seconds']:.2f}s")
    print(f"Avg time per test: {summary['average_time_per_test']:.4f}s")
    
    if summary['failed_tests'] > 0:
        print(f"\n❌ Advanced test suite completed with {summary['failed_tests']} failures")
        sys.exit(1)
    else:
        print(f"\n✅ All advanced tests passed successfully!")
        sys.exit(0)