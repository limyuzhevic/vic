#!/usr/bin/env python3
"""
Python Binding Tests for NLM

Tests for the enhanced Python bindings in pynlm.
Run with: python -m pytest test_bindings.py -v
"""

import pynlm
import pytest
import tempfile
import os

class TestBasicBindings:
    """Test basic Python bindings functionality."""
    
    def test_import_and_create_objects(self):
        """Test that we can import and create basic objects."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent_brain = pynlm.createAgentBrain(brain)
        
        assert config is not None
        assert brain is not None
        assert world is not None
        assert agent_brain is not None
        
        # Initialize objects
        brain.initialize()
        agent_brain.initialize(world)
    
    def test_config_operations(self):
        """Test configuration operations."""
        config = pynlm.createDefaultConfig()
        
        # Test setting and getting values
        config.set("test_int", 42)
        config.set("test_float", 3.14)
        config.set("test_string", "hello")
        config.set("test_bool", True)
        
        assert config.get("test_int") == 42
        assert config.get("test_float") == 3.14
        assert config.get("test_string") == "hello"
        assert config.get("test_bool") == True
        
        # Test keys
        config.set("another_key", "value")
        keys = config.getKeys()
        assert "test_int" in keys
        assert "another_key" in keys
        
        # Test summary
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
    
    def test_brain_basic_methods(self):
        """Test basic brain methods."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test region methods
        region_id = brain.addRegion("test_region")
        assert region_id is not None
        
        assert brain.getRegionCount() >= 1
        assert brain.getTotalNeuronCount() > 0
        
        # Test simulation methods
        brain.step(0, 0.0)
        
        # Test statistics
        spikes = brain.getTotalSpikeCount()
        active = brain.getActiveNeuronCount()
        firing = brain.getFiringNeuronCount()
        
        assert isinstance(spikes, (int, float))
        assert isinstance(active, (int, float))
        assert isinstance(firing, (int, float))
    
    def test_world_operations(self):
        """Test world operations."""
        world = pynlm.createSimpleWorld()
        world.configure(100, 100, 20, 20)
        
        assert world.getWidth() == 100
        assert world.getHeight() == 100
        assert world.getVisionWidth() == 20
        assert world.getVisionHeight() == 20
        
        world.setAgentStart(50, 50)
        
        world.update(0.001)
        
        percept = world.getSensoryPercept()
        assert percept is not None
        
        body = world.getAgentBody()
        assert body is not None
        assert body.x == 50
        assert body.y == 50

class TestMemoryBindings:
    """Test memory system bindings."""
    
    def setup_method(self):
        """Setup test environment."""
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.working_memory = self.brain.getWorkingMemory()
        self.episodic_memory = self.brain.getEpisodicMemory()
        self.semantic_memory = self.brain.getSemanticMemory()
        self.associative_memory = self.brain.getAssociativeMemory()
    
    def test_working_memory(self):
        """Test working memory bindings."""
        # Test basic operations
        pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
        self.working_memory.store(pattern, strength=1.0)
        
        retrieved = self.working_memory.retrieve()
        assert isinstance(retrieved, list)
        assert len(retrieved) == len(pattern)
        
        # Test capacity
        cap = self.working_memory.getCapacity()
        assert isinstance(cap, (int, float))
        assert cap > 0
        
        # Test decay rate
        rate = self.working_memory.getDecayRate()
        assert isinstance(rate, (int, float))
        
        # Test update
        self.working_memory.update(0.1)
        
        # Test clear
        self.working_memory.clear()
    
    def test_episodic_memory(self):
        """Test episodic memory bindings."""
        # Create an episode
        episode = pynlm.EpisodicMemoryItem()
        episode.timestamp = 1000
        episode.metadata = "test episode"
        
        self.episodic_memory.storeEpisode(episode)
        
        assert self.episodic_memory.getEpisodeCount() == 1
        
        retrieved = self.episodic_memory.retrieveEpisode(0)
        assert retrieved.timestamp == 1000
        assert retrieved.metadata == "test episode"
        
        # Test recent episodes
        recent = self.episodic_memory.getRecentEpisodes(1)
        assert len(recent) == 1
        
        # Test consolidate
        self.episodic_memory.consolidate(0.5)
        
        # Test clear
        self.episodic_memory.clear()
        assert self.episodic_memory.getEpisodeCount() == 0
    
    def test_semantic_memory(self):
        """Test semantic memory bindings."""
        # Store facts
        self.semantic_memory.storeFact("capital", "Paris")
        self.semantic_memory.storeFact("language", "French")
        
        # Retrieve facts
        assert self.semantic_memory.retrieveFact("capital") == "Paris"
        assert self.semantic_memory.retrieveFact("language") == "French"
        
        # Test hasFact
        assert self.semantic_memory.hasFact("capital") == True
        assert self.semantic_memory.hasFact("unknown") == False
        
        # Get all facts
        facts = self.semantic_memory.getAllFacts()
        assert len(facts) == 2
        
        # Clear
        self.semantic_memory.clear()
        assert len(self.semantic_memory.getAllFacts()) == 0
    
    def test_associative_memory(self):
        """Test associative memory bindings."""
        # Note: These would require actual neuron IDs in a real scenario
        # We're testing the binding structure here
        
        # Create neuron IDs (simplified for testing)
        neuron_a = pynlm.INVALID_NEURON_ID
        neuron_b = pynlm.INVALID_NEURON_ID
        
        # Test association creation
        self.associative_memory.associate(neuron_a, neuron_b, strength=0.7)
        
        # Test getting associations
        associations = self.associative_memory.getAssociations(neuron_a)
        assert isinstance(associations, list)
        
        # Test association strength
        strength = self.associative_memory.getAssociationStrength(neuron_a, neuron_b)
        assert isinstance(strength, (int, float))
        
        # Test update
        self.associative_memory.updateAssociation(neuron_a, neuron_b, 0.1)
        
        # Clear
        self.associative_memory.clear()

class TestNeuromodulationBindings:
    """Test neuromodulation system bindings."""
    
    def setup_method(self):
        """Setup test environment."""
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.dopamine = self.brain.getDopamine()
        self.curiosity = self.brain.getCuriosity()
        self.novelty = self.brain.getNovelty()
        self.prediction_error = self.brain.getPredictionErrorSignal()
    
    def test_dopamine(self):
        """Test dopamine neuromodulator bindings."""
        # Test basic properties
        assert self.dopamine.getName() == "Dopamine"
        
        # Test level operations
        self.dopamine.setLevel(0.5)
        assert self.dopamine.getLevel() == 0.5
        
        # Test plasticity factor
        factor = self.dopamine.getPlasticityFactor()
        assert isinstance(factor, (int, float))
        
        # Test reward signaling
        self.dopamine.signalReward(1.0)
        self.dopamine.signalRewardPredictionError(0.1)
        
        # Test update
        self.dopamine.update(0.1)
    
    def test_curiosity(self):
        """Test curiosity neuromodulator bindings."""
        # Test basic properties
        assert self.curiosity.getName() == "Curiosity"
        
        # Test exploration level
        self.curiosity.setExplorationLevel(0.8)
        assert self.curiosity.getExplorationLevel() == 0.8
        
        # Test level operations
        self.curiosity.setLevel(0.6)
        assert self.curiosity.getLevel() == 0.6
        
        # Test plasticity factor
        factor = self.curiosity.getPlasticityFactor()
        assert isinstance(factor, (int, float))
        
        # Test update
        self.curiosity.update(0.1)
    
    def test_novelty(self):
        """Test novelty neuromodulator bindings."""
        # Test basic properties
        assert self.novelty.getName() == "Novelty"
        
        # Test level operations
        self.novelty.setLevel(0.3)
        assert self.novelty.getLevel() == 0.3
        
        # Test novelty score
        input_data = [0.1, 0.5, 0.9]
        score = self.novelty.getNoveltyScore(input_data)
        assert isinstance(score, (int, float))
        
        # Test update
        self.novelty.update(0.1)
    
    def test_prediction_error(self):
        """Test prediction error neuromodulator bindings."""
        # Test basic properties
        assert self.prediction_error.getName() == "PredictionError"
        
        # Test level operations
        self.prediction_error.setLevel(0.2)
        assert self.prediction_error.getLevel() == 0.2
        
        # Test learning rate
        self.prediction_error.setLearningRate(0.01)
        
        # Test error computation
        prediction = [0.2, 0.4, 0.6]
        target = [0.1, 0.3, 0.5]
        error = self.prediction_error.getError(prediction, target)
        assert isinstance(error, (int, float))
        
        # Test update
        self.prediction_error.update(0.1)

class TestPredictionBindings:
    """Test prediction system bindings."""
    
    def setup_method(self):
        """Setup test environment."""
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.prediction_system = self.brain.getPredictionSystem()
    
    def test_prediction_system(self):
        """Test prediction system bindings."""
        # Note: This tests the binding structure since the actual prediction
        # implementation is still in development
        
        # Test basic operations
        assert hasattr(self.prediction_system, 'getPredictionError')
        assert hasattr(self.prediction_system, 'getConfidence')
        assert hasattr(self.prediction_system, 'train')
        assert hasattr(self.prediction_system, 'updatePredictions')
        
        # Test error history
        self.prediction_system.clearHistory()
        
        # Test prediction methods (would require actual data in full implementation)
        error = self.prediction_system.getPredictionError()
        confidence = self.prediction_system.getConfidence()
        
        assert isinstance(error, (int, float))
        assert isinstance(confidence, (int, float))

class TestCognitiveBindings:
    """Test cognitive system bindings."""
    
    def setup_method(self):
        """Setup test environment."""
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.planner = self.brain.getPlanner()
        self.concept_formation = self.brain.getConceptFormation()
        self.attention = self.brain.getAttention()
    
    def test_planner_bindings(self):
        """Test neural planner bindings."""
        # Test basic properties
        assert hasattr(self.planner, 'getPlanningDepth')
        assert hasattr(self.planner, 'getPlanningConfidence')
        assert hasattr(self.planner, 'setPlanningDepth')
        assert hasattr(self.planner, 'setCurrentGoal')
        
        # Test getter/setter operations
        self.planner.setPlanningDepth(5)
        assert self.planner.getPlanningDepth() == 5
        
        # Test goal management
        goal = [0.1, 0.2, 0.3]
        self.planner.setCurrentGoal(goal)
        assert self.planner.getCurrentGoal() == goal
    
    def test_concept_formation_bindings(self):
        """Test concept formation bindings."""
        # Note: Tests binding structure since concept formation is in development
        
        # Test basic operations
        assert hasattr(self.concept_formation, 'processPattern')
        assert hasattr(self.concept_formation, 'getConcepts')
        
        # In a real implementation, you would:
        # pattern = [0.1, 0.5, 0.9]
        # self.concept_formation.processPattern(pattern)
        # concepts = self.concept_formation.getConcepts()
    
    def test_attention_bindings(self):
        """Test attentional selection bindings."""
        # Test basic properties
        assert hasattr(self.attention, 'focusOnRegion')
        assert hasattr(self.attention, 'releaseAttention')
        assert hasattr(self.attention, 'getAttendedRegions')
        assert hasattr(self.attention, 'isAttended')
        
        # Test region focus
        region_id = self.brain.addRegion("focused_region")
        self.attention.focusOnRegion(region_id)
        
        # Test attention status
        neuron_id = pynlm.INVALID_NEURON_ID
        attended = self.attention.isAttended(neuron_id)
        assert isinstance(attended, bool)

class TestAgentBrainBindings:
    """Test agent brain bindings."""
    
    def setup_method(self):
        """Setup test environment."""
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.agent_brain = pynlm.createAgentBrain(self.brain)
        self.agent_brain.initialize(self.world)
    
    def test_agent_brain_operations(self):
        """Test agent brain operations."""
        # Test size methods
        sensory_size = self.agent_brain.getSensoryInputSize()
        motor_size = self.agent_brain.getMotorOutputSize()
        
        assert isinstance(sensory_size, (int, float))
        assert isinstance(motor_size, (int, float))
        
        # Test enable/disable methods
        self.agent_brain.enableRewardModulation(True)
        self.agent_brain.enableDevelopment(True)
        self.agent_brain.enableCuriosity(True)
        
        # Test status methods
        assert self.agent_brain.isRewardModulationEnabled() == True
        assert self.agent_brain.isDevelopmentEnabled() == True
        assert self.agent_brain.isCuriosityEnabled() == True
        
        # Test get methods
        stage = self.agent_brain.getDevelopmentalStage()
        neuromod_level = self.agent_brain.getNeuromodulationLevel()
        curiosity_level = self.agent_brain.getCuriosityLevel()
        
        assert isinstance(stage, (int, float))
        assert isinstance(neuromod_level, (int, float))
        assert isinstance(curiosity_level, (int, float))
        
        # Test reset
        self.agent_brain.reset()

class TestUtilityFunctions:
    """Test utility functions."""
    
    def test_create_functions(self):
        """Test factory creation functions."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent_brain = pynlm.createAgentBrain(brain)
        
        assert config is not None
        assert brain is not None
        assert world is not None
        assert agent_brain is not None
    
    def test_invalid_ids(self):
        """Test invalid ID constants."""
        assert pynlm.INVALID_NEURON_ID is not None
        assert pynlm.INVALID_SYNAPSE_ID is not None
        assert pynlm.INVALID_REGION_ID is not None
        assert pynlm.INVALID_POPULATION_ID is not None
        
        # Test they can be used as identifiers
        neuron_id = pynlm.INVALID_NEURON_ID
        assert neuron_id.value > 0  # Should have a valid value

class TestSerialization:
    """Test save/load functionality."""
    
    def test_brain_save_load(self):
        """Test brain save and load operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Perform some operations
        brain.step(0, 0.0)
        original_spikes = brain.getTotalSpikeCount()
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
            filepath = f.name
        
        try:
            # Save brain state
            brain.save(filepath)
            assert os.path.exists(filepath)
            
            # Create new brain and load
            new_config = pynlm.createDefaultConfig()
            new_brain = pynlm.createBrain(new_config)
            new_brain.load(filepath)
            
            # Check that loading worked (basic sanity check)
            assert new_brain is not None
            assert new_brain.getRegionCount() == brain.getRegionCount()
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)

if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])
