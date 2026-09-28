import pytest
import pynlm

class TestPlanning:
    """Test planning and prediction systems"""

    def test_planning_system(self, medium_brain):
        """Test neural planning system"""
        planner = medium_brain.getPlanner()
        assert planner is not None
        
        # Test planner properties
        assert planner.getPlanningDepth() > 0
        assert planner.getActionSelectionTimeout() > 0
        
        # Test action planning
        action_options = [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Interact
        ]
        
        planned_sequence = planner.planActionSequence(
            action_options,
            medium_brain.getSensoryInput(),  # Simplified for testing
            5
        )
        
        assert planned_sequence is not None
        assert len(planned_sequence) > 0
        assert all(action in action_options for action in planned_sequence)

    def test_prediction_system(self, medium_brain):
        """Test prediction system"""
        prediction_system = medium_brain.getPredictionSystem()
        assert prediction_system is not None
        
        # Test prediction system properties
        assert prediction_system.getPredictionHorizon() > 0
        assert prediction_system.getPredictionAccuracy() >= 0.0
        
        # Test prediction training and inference
        training_data = []
        for step in range(50):
            medium_brain.step(step)
            # Create simple sequence data for training
            sequence = [float(step % 10), float(step % 5), float(step % 3)]
            training_data.append(sequence)
        
        if training_data:
            prediction_system.train(training_data)
            
            # Make predictions
            future_predictions = prediction_system.predictNextSteps(3)
            assert future_predictions is not None
            assert len(future_predictions) == 3

    def test_attention_system(self, medium_brain):
        """Test attentional selection system"""
        attention = medium_brain.getAttention()
        assert attention is not None
        
        # Test attention properties
        assert attention.getInhibitionStrength() > 0.0
        assert attention.getExcitationStrength() > 0.0
        assert attention.getFocusDuration() > 0.0
        
        # Test attentional selection
        for step in range(50):
            medium_brain.step(step)
            # Attention system should update based on neural activity
            # (Specific implementation details depend on NLM architecture)

    def test_planning_prediction_integration(self, medium_brain, test_world, test_agent):
        """Test integration between planning and prediction systems"""
        # Initialize all systems
        planner = medium_brain.getPlanner()
        prediction_system = medium_brain.getPredictionSystem()
        attention = medium_brain.getAttention()
        
        assert planner is not None
        assert prediction_system is not None
        assert attention is not None
        
        # Run integrated planning-prediction cycle
        for step in range(100):
            test_world.update(0.1)
            
            # Get sensory input
            percept = test_world.getSensoryPercept()
            test_agent.processSensoryInput(percept)
            
            # Brain processing
            medium_brain.step(step)
            
            # Planning and prediction updates
            if planner:
                # Plan actions based on current state
                action_options = [
                    pynlm.ActionType.MoveForward,
                    pynlm.ActionType.TurnLeft,
                    pynlm.ActionType.TurnRight
                ]
                planner.planActionSequence(
                    action_options,
                    percept.getAllSignals() if percept else [],
                    3
                )
            
            if prediction_system:
                # Update predictions
                prediction_system.updatePredictions(percept)
            
            if attention:
                # Update attentional focus
                attention.updateFocus(percept)