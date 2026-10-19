#!/usr/bin/env python3
"""
Test Advanced NLM Agent Capabilities

This script tests the advanced agent capabilities implemented for the NLM project.
It verifies that all the advanced features work correctly.
"""

import sys
import numpy as np
from typing import Dict, List, Any

# Add the nlm_advanced package to the path
sys.path.insert(0, '/workspace/app/python')

print("Testing Advanced NLM Agent Capabilities")
print("=" * 60)

# Import the advanced agent capabilities
try:
    from nlm_advanced.agent import (
        CustomArchitect,
        SocialLearner,
        HierarchicalController,
        AdaptiveBehaviorModifier,
        CollaborativeTrainer,
        TransferLearning,
        ContinualLearning,
        MetaLearning,
        LearningAlgorithm,
        ControlStrategy,
        AgentConfig
    )
    print("✓ Successfully imported all advanced agent capabilities")
except ImportError as e:
    print(f"✗ Import error: {e}")
    print("This is expected if the module structure is not properly set up.")
    sys.exit(1)

def test_advanced_architectures():
    """Test advanced agent architecture capabilities."""
    print("\n1. Testing Advanced Agent Architecture Capabilities")
    print("-" * 50)
    
    try:
        # Create a simple brain factory (mock)
        def brain_factory(config: AgentConfig):
            return {"agent_id": config.agent_id, "network_size": config.network_size}
        
        # Create custom architect
        architect = CustomArchitect(brain_factory)
        
        # Test neuro-dynamical architecture
        neuro_config = AgentConfig(
            agent_id="test_agent_1",
            algorithm=LearningAlgorithm.NEURO_DYNAMICAL,
            network_size=2000,
            hierarchy_levels=4
        )
        
        neuro_architecture = architect.create_custom_architecture("neuro_test", neuro_config)
        
        # Verify architecture properties
        assert 'name' in neuro_architecture
        assert neuro_architecture['name'] == 'neuro_test'
        assert 'topology' in neuro_architecture
        assert neuro_architecture['topology']['type'] == 'dynamical'
        assert 'connectivity' in neuro_architecture
        
        print("   ✓ Neuro-dynamical architecture created successfully")
        print(f"   ✓ Topology type: {neuro_architecture['topology']['type']}")
        print(f"   ✓ Number of layers: {neuro_architecture['topology']['layers']}")
        
        # Test social learning architecture
        social_config = AgentConfig(
            agent_id="test_agent_2",
            algorithm=LearningAlgorithm.SOCIAL_LEARNING,
            network_size=1500,
            social_connections=10
        )
        
        social_architecture = architect.create_custom_architecture("social_test", social_config)
        
        # Verify social architecture properties
        assert 'name' in social_architecture
        assert social_architecture['name'] == 'social_test'
        assert 'connectivity' in social_architecture
        assert social_architecture['connectivity']['social_connections'] == 10
        
        print("   ✓ Social learning architecture created successfully")
        print(f"   ✓ Social connections: {social_architecture['connectivity']['social_connections']}")
        print(f"   ✓ Clustering enabled: {social_architecture['topology']['clustering']}")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        return False
def test_social_learning():
    """Test advanced social learning capabilities."""
    print("\n2. Testing Advanced Social Learning Capabilities")
    print("-" * 50)
    
    try:
        # Create mock agents
        agents = [
            {"id": "agent_a", "skills": ["coordination"], "knowledge_capability": 0.8},
            {"id": "agent_b", "skills": ["problem_solving"], "knowledge_capability": 0.6},
            {"id": "agent_c", "skills": ["communication"], "knowledge_capability": 0.9}
        ]
        
        # Create social learner
        social_learner = SocialLearner(agents)
        
        # Test establishing social connections
        bond1 = social_learner.establish_social_connection("agent_a", "agent_b", "trust")
        bond2 = social_learner.establish_social_connection("agent_b", "agent_c", "cooperation")
        
        assert len(social_learner.social_bonds) == 2
        assert bond1.agent_a == "agent_a"
        assert bond1.agent_b == "agent_b"
        assert bond1.strength == 0.5
        
        print("   ✓ Social bonds established successfully")
        print(f"   ✓ Total bonds: {len(social_learner.social_bonds)}")
        
        # Test knowledge transfer
        agent_experience = {
            'actions': ['action1', 'action2'],
            'outcomes': [0.8, 0.9],
            'social_interactions': [('agent_b', 'shared_knowledge')]
        }
        
        transferred_knowledge = social_learner.social_learning_update("agent_a", agent_experience)
        
        assert 'social_context' in transferred_knowledge
        assert 'agent_id' in transferred_knowledge['social_context']
        
        print("   ✓ Knowledge transfer completed successfully")
        print(f"   ✓ Knowledge extracted: {list(transferred_knowledge.keys())}")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        return False
def test_hierarchical_control():
    """Test advanced hierarchical control capabilities."""
    print("\n3. Testing Advanced Hierarchical Control Capabilities")
    print("-" * 50)
    
    try:
        # Create mock agents
        agents = [
            {"id": "agent_1", "role": "controller"},
            {"id": "agent_2", "role": "executive"},
            {"id": "agent_3", "role": "sensorimotor"}
        ]
        
        # Create hierarchical controller
        hierarchical_controller = HierarchicalController(agents)
        
        # Test hierarchy creation
        hierarchy_structure = {
            'levels': 2,
            'authority_decay': 0.8,
            'parent_selection': 'random'
        }
        
        hierarchy = hierarchical_controller.create_hierarchy(hierarchy_structure)
        
        assert len(hierarchy) > 0
        assert all('agent_id' in node for node in hierarchy)
        
        print("   ✓ Hierarchy created successfully")
        print(f"   ✓ Number of hierarchy nodes: {len(hierarchy)}")
        
        # Test hierarchical control execution
        environment_state = {
            'environment': {'complexity': 0.7},
            'agents': {'agent_1': {'state': 'active'}},
            'resources': {'energy': 0.8},
            'goals': {'primary': 'reach_target'}
        }
        
        control_results = hierarchical_controller.execute_hierarchical_control(environment_state)
        
        assert 'final_action' in control_results
        assert 'individual_decisions' in control_results
        assert 'coordination_efficiency' in control_results
        
        print("   ✓ Hierarchical control executed successfully")
        print(f"   ✓ Coordination efficiency: {control_results['coordination_efficiency']:.2f}")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        return False
def test_adaptive_behavior():
    """Test advanced adaptive behavior modification."""
    print("\n4. Testing Advanced Adaptive Behavior Modification")
    print("-" * 50)
    
    try:
        # Create mock agents
        agents = [
            {"id": "agent_x", "skills": ["navigation"]},
            {"id": "agent_y", "skills": ["communication"]}
        ]
        
        # Create adaptive behavior modifier
        adaptive_modifier = AdaptiveBehaviorModifier(agents)
        
        # Add adaptation rules
        adaptation_rules = [
            {
                'name': 'test_rule',
                'conditions': {'performance_below': 0.5},
                'modifications': {'exploration_rate': {'type': 'additive', 'value': 0.1}}
            }
        ]
        
        for rule in adaptation_rules:
            adaptive_modifier.add_adaptation_rule(rule)
        
        assert len(adaptive_modifier.adaptation_rules) == 1
        
        print("   ✓ Adaptation rules added successfully")
        print(f"   ✓ Number of adaptation rules: {len(adaptive_modifier.adaptation_rules)}")
        
        # Test behavior adaptation
        environment_feedback = {
            'type': 'success',
            'reward': 0.8,
            'efficiency': 0.7,
            'novelty': 0.3
        }
        
        adapted_behavior = adaptive_modifier.adapt_agent_behavior("agent_x", environment_feedback, 0.1)
        
        assert 'agent_id' in adapted_behavior
        assert 'adapted_state' in adapted_behavior
        assert 'policy_optimization' in adapted_behavior
        
        print("   ✓ Behavior adaptation completed successfully")
        print(f"   ✓ Adaptation efficiency: {adapted_behavior['adaptation_efficiency']:.2f}")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        return False
def test_collaborative_training():
    """Test advanced collaborative training capabilities."""
    print("\n5. Testing Advanced Collaborative Training Capabilities")
    print("-" * 50)
    
    try:
        # Create collaborative trainer
        collaborative_trainer = CollaborativeTrainer()
        
        # Create training agents
        training_agents = [
            {"id": "agent_alpha", "role": "leader", "skills": ["coordination"]},
            {"id": "agent_beta", "role": "follower", "skills": ["execution"]}
        ]
        
        # Create training session
        training_session = collaborative_trainer.create_training_session(
            "test_session",
            training_agents,
            task_type="coordination",
            difficulty="easy"
        )
        
        assert 'session_id' in training_session
        assert 'name' in training_session
        assert 'shared_task' in training_session
        
        print("   ✓ Training session created successfully")
        print(f"   ✓ Session name: {training_session['name']}")
        print(f"   ✓ Task objectives: {len(training_session['shared_task']['objectives'])}")
        
        # Test training step
        step_actions = {
            "agent_alpha": {
                'action_type': 'move_forward',
                'effectiveness': 0.8,
                'collaboration_contribution': 0.5
            }
        }
        
        step_result = collaborative_trainer.conduct_training_step(
            training_session['session_id'], step_actions
        )
        
        assert 'step_number' in step_result
        assert 'agent_actions' in step_result
        assert 'performance_updates' in step_result
        
        print("   ✓ Training step completed successfully")
        print(f"   ✓ Step number: {step_result['step_number']}")
        print(f"   ✓ Performance updates: {len(step_result['performance_updates'])} agents")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        return False
def test_advanced_learning_modules():
    """Test advanced learning modules."""
    print("\n6. Testing Advanced Learning Modules")
    print("-" * 50)
    
    try:
        # Test TransferLearning
        transfer_learner = TransferLearning("test_agent")
        transfer_learner.knowledge_pool = {
            'task1': {
                'pattern1': {'parameters': {'weight': 0.5}},
                'pattern2': {'parameters': {'bias': 0.3}}
            },
            'task2': {
                'pattern1': {'parameters': {'weight': 0.6}},
                'pattern3': {'parameters': {'scale': 0.8}}
            }
        }
        
        transfer_result = transfer_learner.transfer_knowledge(
            'task1', 'task2', transfer_strength=0.8, task_similarity=0.7
        )
        
        assert 'transferred_patterns' in transfer_result
        assert 'final_effectiveness' in transfer_result
        
        print("   ✓ TransferLearning functionality verified")
        print(f"   ✓ Transfer effectiveness: {transfer_result['final_effectiveness']:.2f}")
        
        # Test ContinualLearning
        continual_learner = ContinualLearning("test_agent")
        continual_learner.task_memory = {
            'task1': {'access_count': 5, 'knowledge_strength': 0.9}
        }
        
        continual_result = continual_learner.learn_continually(
            {'data': 'new_task_data'}, 'task2', 0.01
        )
        
        assert 'task_id' in continual_result
        assert 'learning_result' in continual_result
        
        print("   ✓ ContinualLearning functionality verified")
        print(f"   ✓ Tasks learned: {continual_result['total_tasks_learned']}")
        
        # Test MetaLearning
        meta_learner = MetaLearning("test_agent")
        meta_result = meta_learner.adapt_learning_strategy(
            {'complexity': 0.7, 'dimensionality': 5}, 'test_task'
        )
        
        assert 'adapted_strategy' in meta_result
        assert 'task_analysis' in meta_result
        
        print("   ✓ MetaLearning functionality verified")
        print(f"   ✓ Task complexity: {meta_result['task_analysis']['complexity']:.2f}")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        return False
def test_integration():
    """Test integration of all advanced agent capabilities."""
    print("\n7. Testing Integration of Advanced Agent Capabilities")
    print("-" * 50)
    
    try:
        # Create a comprehensive agent system
        agents = [
            {"id": "agent_1", "type": "explorer", "skills": ["navigation", "exploration"]},
            {"id": "agent_2", "type": "social", "skills": ["coordination", "communication"]},
            {"id": "agent_3", "type": "learner", "skills": ["analysis", "adaptation"]}
        ]
        
        # Create all advanced modules
        architect = CustomArchitect(lambda config: config)
        social_learner = SocialLearner(agents)
        hierarchical_controller = HierarchicalController(agents)
        adaptive_modifier = AdaptiveBehaviorModifier(agents)
        collaborative_trainer = CollaborativeTrainer()
        
        # Test integration
        hierarchy = hierarchical_controller.create_hierarchy({'levels': 2})
        bond = social_learner.establish_social_connection("agent_1", "agent_2")
        
        training_session = collaborative_trainer.create_training_session(
            "integration_test", agents, task_type="coordination", difficulty="medium"
        )
        
        # Verify integration
        assert len(hierarchy) > 0
        assert len(social_learner.social_bonds) > 0
        assert 'session_id' in training_session
        
        print("   ✓ All advanced agent capabilities integrated successfully")
        print(f"   ✓ Hierarchy nodes: {len(hierarchy)}")
        print(f"   ✓ Social bonds: {len(social_learner.social_bonds)}")
        print(f"   ✓ Training sessions: {len(collaborative_trainer.training_sessions)}")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Integration test failed: {e}")
        return False
def main():
    """Main test function."""
    print("Running comprehensive tests for advanced NLM agent capabilities...")
    
    # Run all tests
    tests = [
        ("Advanced Architectures", test_advanced_architectures),
        ("Social Learning", test_social_learning),
        ("Hierarchical Control", test_hierarchical_control),
        ("Adaptive Behavior", test_adaptive_behavior),
        ("Collaborative Training", test_collaborative_training),
        ("Advanced Learning Modules", test_advanced_learning_modules),
        ("Integration", test_integration)
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_function in tests:
        try:
            if test_function():
                passed += 1
                print(f"   ✓ {test_name} PASSED")
            else:
                failed += 1
                print(f"   ✗ {test_name} FAILED")
        except Exception as e:
            failed += 1
            print(f"   ✗ {test_name} FAILED with exception: {e}")
    
    print("\n" + "=" * 60)
    print("TEST RESULTS SUMMARY")
    print("=" * 60)
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n✓ ALL TESTS PASSED!")
        print("The advanced NLM agent capabilities have been successfully implemented.")
        return 0
    else:
        print(f"\n✗ {failed} tests failed.")
        print("Please review the implementation.")
        return 1
if __name__ == "__main__":
    sys.exit(main())
