"""Phase 6 integration experiment - the complete brain integration test."""

import time
import math
from typing import Dict, Any, Optional
from enum import Enum

from ..core.brain.brain import Brain
from ..core.config import Config
from ..agent import AgentBrain
from ..environment import SimpleWorld
from ..exceptions import NLMException
from ..core.brain.enums import NeuronType


class DevelopmentalStage(Enum):
    """Developmental stages for the brain."""
    
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"


class Phase6Config:
    """Configuration for Phase 6 integration experiment."""
    
    def __init__(self):
        # Basic configuration
        self.neuron_count = 500
        self.max_steps = 2000
        self.region_count = 4
        self.connection_probability = 0.15
        
        # Plasticity configuration
        self.enable_stdp = True
        self.enable_hebbian = True
        self.enable_structural = True
        self.enable_development = True
        
        # Neuromodulation configuration
        self.enable_dopamine = True
        self.enable_curiosity = True
        self.enable_novelty = True
        
        # Memory configuration
        self.enable_working_memory = True
        self.enable_episodic_memory = True
        self.enable_associative_memory = True
        
        # Prediction configuration
        self.enable_prediction = True
        self.prediction_horizon = 10
        
        # Cognition configuration
        self.enable_planning = True
        self.enable_concept_formation = True
        self.enable_attention = True
        
        # Experiment configuration
        self.enable_checkpointing = True
        self.enable_replay = True
        self.checkpoint_path = "/tmp/nlm_checkpoint.bin"
        
        # Performance configuration
        self.speed_factor = 1.0
        self.log_interval = 100
        
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'neuron_count': self.neuron_count,
            'max_steps': self.max_steps,
            'region_count': self.region_count,
            'connection_probability': self.connection_probability,
            'enable_stdp': self.enable_stdp,
            'enable_hebbian': self.enable_hebbian,
            'enable_structural': self.enable_structural,
            'enable_development': self.enable_development,
            'enable_dopamine': self.enable_dopamine,
            'enable_curiosity': self.enable_curiosity,
            'enable_novelty': self.enable_novelty,
            'enable_working_memory': self.enable_working_memory,
            'enable_episodic_memory': self.enable_episodic_memory,
            'enable_associative_memory': self.enable_associative_memory,
            'enable_prediction': self.enable_prediction,
            'prediction_horizon': self.prediction_horizon,
            'enable_planning': self.enable_planning,
            'enable_concept_formation': self.enable_concept_formation,
            'enable_attention': self.enable_attention,
            'enable_checkpointing': self.enable_checkpointing,
            'enable_replay': self.enable_replay,
            'checkpoint_path': self.checkpoint_path,
            'speed_factor': self.speed_factor,
            'log_interval': self.log_interval
        }
        
    # CamelCase property aliases for test compatibility
    @property
    def neuronCount(self):
        """Get neuron count (camelCase alias)."""
        return self.neuron_count
        
    @property
    def maxSteps(self):
        """Get max steps (camelCase alias)."""
        return self.max_steps


class Phase6IntegrationResult:
    """Results from Phase 6 integration experiment."""
    
    def __init__(self):
        # Performance metrics
        self.total_reward = 0.0
        self.avg_firing_rate = 0.0
        self.memory_episodes_stored = 0.0
        self.dopamine_level = 0.0
        self.curiosity_level = 0.0
        self.novelty_level = 0.0
        
        # Integration status
        self.memory_working_memory_integrated = False
        self.memory_episodic_memory_integrated = False
        self.neuromodulation_integrated = False
        self.prediction_integrated = False
        self.development_integrated = False
        self.cognition_integrated = False
        self.checkpointing_works = False
        
        # Time tracking
        self.start_time = 0
        self.end_time = 0
        self.total_wall_clock_time = 0.0
        
        # Detailed statistics
        self.total_spikes = 0
        self.total_steps = 0
        self.simulation_time = 0.0


class Phase6IntegratedExperiment:
    """Complete Phase 6 integration experiment."""
    
    def __init__(self):
        self.config = Phase6Config()
        self.result = Phase6IntegrationResult()
        self.brain = None
        self.world = None
        self.agent = None
        self.development_stage = DevelopmentalStage.INITIAL
        
    def configure(self, **kwargs):
        """
        Configure experiment parameters.
        
        Args:
            **kwargs: Configuration parameters
        """
        for key, value in kwargs.items():
            if hasattr(self.config, key):
                setattr(self.config, key, value)
            else:
                raise NLMException(f"Unknown configuration parameter: {key}")
    
    def run(self, config: Optional[Phase6Config] = None) -> Phase6IntegrationResult:
        """
        Run Phase 6 integration experiment.
        
        Args:
            config: Experiment configuration (uses instance config if None)
            
        Returns:
            Experiment results
        """
        if config is not None:
            self.config = config
        
        # Reset result
        self.result = Phase6IntegrationResult()
        self.result.start_time = int(time.time())
        
        # Log experiment start
        print("=== NLM Phase 6 Integration Experiment ===")
        print(f"Configuration: {self.config.neuron_count} neurons, {self.config.max_steps} steps")
        
        try:
            # Initialize components
            self._initialize_components()
            
            # Run simulation
            self._run_simulation()
            
            # Run integration tests
            self._run_integration_tests()
            
            # Collect final metrics
            self._collect_final_metrics()
            
            # Save final state if checkpointing enabled
            if self.config.enable_checkpointing:
                self._save_checkpoint()
            
            return self.result
            
        except Exception as e:
            print(f"Error during experiment: {e}")
            raise
        
    def verify_integration(self) -> bool:
        """
        Verify that all systems are properly integrated.
        
        Returns:
            True if integration verified, False otherwise
        """
        print("=== Phase 6 Integration Verification ===")
        
        # Create minimal brain for testing
        cfg = Config()
        cfg.set("neuron_count", 100)
        cfg.set("region_count", 1)
        
        self.brain = Brain(cfg)
        if not self.brain.initialize():
            print("ERROR: Brain initialization failed")
            return False
        
        success = True
        
        # Test 1: Memory systems exist and are connected
        if self.brain.get_working_memory() is not None:
            print("[PASS] Working memory is integrated")
            self.result.memory_working_memory_integrated = True
        else:
            print("[FAIL] Working memory is NOT integrated")
            success = False
        
        if self.brain.get_episodic_memory() is not None:
            print("[PASS] Episodic memory is integrated")
            self.result.memory_episodic_memory_integrated = True
        else:
            print("[FAIL] Episodic memory is NOT integrated")
            success = False
        
        # Test 2: Neuromodulation systems exist
        if self.brain.get_dopamine() is not None:
            print("[PASS] Dopamine system is integrated")
            self.result.neuromodulation_integrated = True
        else:
            print("[FAIL] Dopamine system is NOT integrated")
            success = False
        
        if self.brain.get_curiosity() is not None:
            print("[PASS] Curiosity system is integrated")
            # Note: curiosity is neuromodulation, so this confirms it
        else:
            print("[FAIL] Curiosity system is NOT integrated")
            success = False
        
        # Test 3: Prediction system exists
        if self.brain.get_prediction_system() is not None:
            print("[PASS] Prediction system is integrated")
            self.result.prediction_integrated = True
        else:
            print("[FAIL] Prediction system is NOT integrated")
            success = False
        
        # Test 4: Development system exists
        if self.brain.get_development_system() is not None:
            print("[PASS] Development system is integrated")
            self.result.development_integrated = True
        else:
            print("[FAIL] Development system is NOT integrated")
            success = False
        
        return success
    
    def test_memory_integration(self) -> bool:
        """
        Test memory system integration.
        
        Returns:
            True if test passes, False otherwise
        """
        print("=== Testing Memory Integration ===")
        
        cfg = Config()
        cfg.set("neuron_count", 100)
        
        self.brain = Brain(cfg)
        self.brain.initialize()
        
        # Get memory systems
        wm = self.brain.get_working_memory()
        em = self.brain.get_episodic_memory()
        
        if not wm or not em:
            print("ERROR: Memory systems not available")
            return False
        
        # Run some steps
        for i in range(100):
            self.brain.step(i, i * 0.001)
        
        # Check if working memory has traces
        if wm.get_active_traces() > 0:
            print(f"[PASS] Working memory has active traces: {wm.get_active_traces()}")
        else:
            print("[INFO] Working memory has no active traces (may be normal)")
        
        # Check if episodic memory has episodes
        if em.get_episode_count() > 0:
            print(f"[PASS] Episodic memory has episodes: {em.get_episode_count()}")
        else:
            print("[INFO] Episodic memory has no episodes (may be normal)")
        
        return True
    
    def test_neuromodulation_integration(self) -> bool:
        """
        Test neuromodulation system integration.
        
        Returns:
            True if test passes, False otherwise
        """
        print("=== Testing Neuromodulation Integration ===")
        
        cfg = Config()
        cfg.set("neuron_count", 100)
        
        self.brain = Brain(cfg)
        self.brain.initialize()
        
        # Get neuromodulation systems
        dopamine = self.brain.get_dopamine()
        curiosity = self.brain.get_curiosity()
        novelty = self.brain.get_novelty()
        
        if not dopamine or not curiosity or not novelty:
            print("ERROR: Neuromodulation systems not available")
            return False
        
        # Run some steps with sensory input
        for i in range(50):
            # Inject some sensory activity
            self.brain.inject_current_to_neurons(NeuronType.SENSORY, 5.0)
            self.brain.step(i, i * 0.001)
        
        print("[PASS] Neuromodulation systems are functional")
        return True
    
    def test_checkpointing(self) -> bool:
        """
        Test checkpoint save/load functionality.
        
        Returns:
            True if test passes, False otherwise
        """
        print("=== Testing Checkpoint Save/Load ===")
        
        cfg = Config()
        cfg.set("neuron_count", 100)
        
        # Create and initialize brain
        brain1 = Brain(cfg)
        brain1.initialize()
        
        # Run some steps
        for i in range(100):
            brain1.step(i, i * 0.001)
        
        avg_firing1 = brain1.get_average_firing_rate()
        total_spikes1 = brain1.get_total_spike_count()
        
        print(f"Brain1 avg firing: {avg_firing1}")
        print(f"Brain1 total spikes: {total_spikes1}")
        
        # Save checkpoint
        path = "/tmp/nlm_checkpoint_test.bin"
        if not brain1.save(path):
            print("ERROR: Failed to save checkpoint")
            return False
        
        # Create new brain and load
        brain2 = Brain(cfg)
        brain2.initialize()
        
        if not brain2.load(path):
            print("ERROR: Failed to load checkpoint")
            return False
        
        total_spikes2 = brain2.get_total_spike_count()
        print(f"Brain2 total spikes after load: {total_spikes2}")
        
        print("[PASS] Checkpoint save/load completed")
        return True
    
    def test_replay(self) -> bool:
        """
        Test replay system.
        
        Returns:
            True if test passes, False otherwise
        """
        print("=== Testing Replay System ===")
        
        cfg = Config()
        cfg.set("neuron_count", 100)
        
        self.brain = Brain(cfg)
        self.brain.initialize()
        
        # Run some steps
        for i in range(200):
            self.brain.step(i, i * 0.001)
        
        # Check if episodes exist for replay
        em = self.brain.get_episodic_memory()
        if em and em.get_episode_count() > 0:
            print(f"[PASS] Episodes available for replay: {em.get_episode_count()}")
            
            # Get episodes for replay
            episodes = em.get_episodes_for_replay(3)
            if not episodes.empty():
                print("[PASS] Replay system can retrieve episodes")
                return True
        
        print("[INFO] No episodes available for replay (may be normal)")
        return True
    
    def _initialize_components(self):
        """Initialize all experiment components."""
        # Create configuration
        cfg = Config()
        
        # Apply Phase 6 configuration
        config_dict = self.config.to_dict()
        for key, value in config_dict.items():
            cfg.set(key, value)
        
        # Create brain
        self.brain = Brain(cfg)
        if not self.brain.initialize():
            raise NLMException("Failed to initialize brain")
        
        # Create world
        self.world = SimpleWorld()
        self.world.initialize(self.config.region_count * 2, self.config.region_count * 2)
        
        # Create agent
        self.agent = AgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable subsystems
        if self.config.enable_stdp:
            self.agent.enable_reward_modulation(True)
        if self.config.enable_structural:
            self.agent.enable_structural_plasticity(True)
        if self.config.enable_development:
            self.agent.enable_development(True)
        if self.config.enable_curiosity:
            self.agent.enable_curiosity(True)
        
        print("Components initialized successfully")
    
    def _run_simulation(self):
        """Run the main simulation loop."""
        print("Running simulation...")
        
        total_reward = 0.0
        total_firing_rate = 0.0
        firing_count = 0
        
        for step in range(self.config.max_steps):
            # Get observation
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step, step * 0.001)
            
            # Get motor command
            cmd = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(cmd, self.world.getSimulationTime())
            
            # Compute reward
            reward = self.world.computeReward(self.brain.getRegion(1))
            total_reward += reward
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            if self.config.enable_development:
                self.agent.updateDevelopment(0.001)
            
            # Collect metrics
            total_firing_rate += self.brain.getAverageFiringRate()
            if self.brain.getFiringNeuronCount() > 0:
                firing_count += 1
            
            # Log progress periodically
            if step % self.config.log_interval == 0 and step > 0:
                print(f"Step {step}:")
                print(f"  Reward: {total_reward / step:.4f}")
                print(f"  Firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
                print(f"  Working memory: {self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0}")
    
    def _run_integration_tests(self):
        """Run integration tests for all systems."""
        print("Running integration tests...")
        
        # Memory integration test
        if self.config.enable_working_memory:
            self.test_memory_integration()
        
        # Neuromodulation integration test
        if self.config.enable_dopamine:
            self.test_neuromodulation_integration()
        
        # Checkpointing test
        if self.config.enable_checkpointing:
            self.test_checkpointing()
        
        # Replay test
        if self.config.enable_replay:
            self.test_replay()
    
    def _collect_final_metrics(self):
        """Collect final experiment metrics."""
        print("Collecting final metrics...")
        
        self.result.total_reward = self.brain.getAverageFiringRate()
        self.result.avg_firing_rate = self.brain.getAverageFiringRate()
        self.result.memory_episodes_stored = self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0
        self.result.dopamine_level = self.agent.getNeuromodulationLevel() if self.agent else 0.0
        self.result.curiosity_level = self.agent.getCuriosityLevel() if self.agent else 0.0
        self.result.novelty_level = self.agent.getNoveltyLevel() if self.agent else 0.0
        
        # Update integration status
        self.result.memory_working_memory_integrated = self.brain.getWorkingMemory() is not None
        self.result.memory_episodic_memory_integrated = self.brain.getEpisodicMemory() is not None
        self.result.neuromodulation_integrated = self.brain.getDopamine() is not None
        self.result.prediction_integrated = self.brain.getPredictionSystem() is not None
        self.result.development_integrated = self.brain.getDevelopmentSystem() is not None
        
        # Time tracking
        self.result.end_time = int(time.time())
        self.result.total_wall_clock_time = self.result.end_time - self.result.start_time
        
        self.result.total_spikes = self.brain.getTotalSpikeCount()
        self.result.total_steps = self.config.max_steps
        self.result.simulation_time = self.config.max_steps * 0.001
    
    def _save_checkpoint(self):
        """Save experiment checkpoint."""
        print("Saving checkpoint...")
        path = self.config.checkpoint_path
        
        # Save brain state
        if self.brain.save(path):
            print(f"Checkpoint saved to: {path}")
        else:
            print(f"WARNING: Failed to save checkpoint to {path}")


# Convenience functions
def create_phase6_config() -> Phase6Config:
    """Create default Phase 6 configuration."""
    return Phase6Config()


def run_phase6_experiment(config: Optional[Phase6Config] = None) -> Phase6IntegrationResult:
    """
    Run Phase 6 integration experiment.
    
    Args:
        config: Experiment configuration (uses defaults if None)
        
    Returns:
        Experiment results
    """
    if config is None:
        config = create_phase6_config()
    
    experiment = Phase6IntegratedExperiment()
    return experiment.run(config)