#!/usr/bin/env python3
"""
Test script for the new training loop utilities.

This script tests the basic functionality of all the new training loop classes
and functions to ensure they work correctly.
"""

import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def test_imports():
    """Test that all new classes can be imported"""
    print("Testing imports...")
    
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Check if new classes are available
        new_classes = [
            'TrainingLoop',
            'ReinforcementLearner', 
            'TabularQLearning',
            'DeepQLearning',
            'PolicyGradientLearner',
            'CurriculumLearner',
            'SimulationScheduler',
            'ExperienceReplay',
            'QTable',
            'TrainingExperience',
            'LearningProgress',
            'ResourceConfig',
            'ExperimentConfig',
            'TrainingJob'
        ]
        
        for class_name in new_classes:
            try:
                class_obj = getattr(pynlm, class_name)
                print(f"✓ Found class: {class_name}")
            except AttributeError:
                print(f"✗ Missing class: {class_name}")
                return False
        
        # Check enums
        enums = [
            'RLAlgorithm',
            'ExplorationStrategy',
            'DifficultyLevel',
            'NeuralVisualization',
            'ActionType',
            'WorldObjectType'
        ]
        
        for enum_name in enums:
            try:
                enum_obj = getattr(pynlm, enum_name)
                print(f"✓ Found enum: {enum_name}")
            except AttributeError:
                print(f"✗ Missing enum: {enum_name}")
                return False
        
        # Check factory functions
        factory_functions = [
            'createLearner',
            'createStandardCurriculum',
            'createLocalScheduler',
            'createTrainingLoop',
            'runReinforcementTraining',
            'createCurriculumLearner',
            'scheduleTraining',
            'getSupportedAlgorithms',
            'stringToAlgorithm',
            'algorithmToString'
        ]
        
        for func_name in factory_functions:
            try:
                func_obj = getattr(pynlm, func_name)
                print(f"✓ Found factory function: {func_name}")
            except AttributeError:
                print(f"✗ Missing factory function: {func_name}")
                return False
        
        return True
        
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False

def test_class_creation():
    """Test that classes can be instantiated"""
    print("\nTesting class creation...")
    
    try:
        import pynlm
        
        # Create a test brain
        brain = pynlm.createQuickTestBrain()
        print(f"✓ Created test brain with {brain.getTotalNeuronCount()} neurons")
        
        # Test TrainingLoop creation
        config = pynlm.ExperimentConfig.createTrainingConfig()
        environment = pynlm.SimpleWorld()
        agent = pynlm.AgentBrain(brain)
        learner = pynlm.createLearner(pynlm.RLAlgorithm.TABULAR_Q_LEARNING, brain, agent, environment, config)
        
        trainingLoop = pynlm.createTrainingLoop(agent, environment, config, brain, learner)
        print("✓ Created TrainingLoop")
        
        # Test ReinforcementLearner
        print("✓ Created ReinforcementLearner")
        
        # Test CurriculumLearner
        curriculumLearner = pynlm.createCurriculumLearner(0)
        print("✓ Created CurriculumLearner")
        
        # Test SimulationScheduler
        scheduler = pynlm.createLocalScheduler(pynlm.ResourceConfig())
        print("✓ Created SimulationScheduler")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed during class creation: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_factory_functions():
    """Test factory functions"""
    print("\nTesting factory functions...")
    
    try:
        import pynlm
        
        # Test ReinforcementLearnerFactory
        algorithms = pynlm.getSupportedAlgorithms()
        print(f"✓ Got {len(algorithms)} supported algorithms")
        
        for algorithm_name in algorithms:
            algorithm = pynlm.stringToAlgorithm(algorithm_name)
            converted_name = pynlm.algorithmToString(algorithm)
            print(f"  - {algorithm_name} -> {algorithm} -> {converted_name}")
        
        # Test CurriculumLearnerFactory
        curriculumTypes = pynlm.CurriculumLearnerFactory.getAvailableCurriculumTypes()
        print(f"✓ Got {len(curriculumTypes)} curriculum types")
        
        # Test SimulationSchedulerFactory
        scheduler = pynlm.createLocalScheduler(pynlm.ResourceConfig())
        print("✓ Created local scheduler")
        
        # Test experiment scheduling
        experiments = [
            pynlm.ExperimentConfig(
                experimentName="test1",
                algorithm=pynlm.RLAlgorithm.TABULAR_Q_LEARNING,
                episodeCount=50
            ),
            pynlm.ExperimentConfig(
                experimentName="test2",
                algorithm=pynlm.RLAlgorithm.DEEP_Q_NETWORK,
                episodeCount=100
            )
        ]
        
        resources = pynlm.ResourceConfig()
        resources.cpuCores = 2
        resources.gpuMemory = 4
        resources.ramMemory = 16
        
        scheduled = pynlm.optimizeExperimentSchedule(experiments, resources)
        print(f"✓ Scheduled {len(scheduled)} experiments")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed during factory function testing: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_data_structures():
    """Test data structures"""
    print("\nTesting data structures...")
    
    try:
        import pynlm
        
        # Test TrainingExperience
        experience = pynlm.TrainingExperience()
        print("✓ Created TrainingExperience")
        
        # Test LearningProgress
        progress = pynlm.LearningProgress()
        print("✓ Created LearningProgress")
        
        # Test ExperienceReplay
        replay = pynlm.ExperienceReplay(100)
        print("✓ Created ExperienceReplay")
        
        # Test QTable
        qtable = pynlm.QTable(100, 16)
        print("✓ Created QTable")
        
        # Test EnvironmentAdaptation
        adaptation = pynlm.EnvironmentAdaptation()
        print("✓ Created EnvironmentAdaptation")
        
        # Test ResourceConfig
        resources = pynlm.ResourceConfig()
        print("✓ Created ResourceConfig")
        
        # Test ExperimentConfig
        experiment = pynlm.ExperimentConfig()
        print("✓ Created ExperimentConfig")
        
        # Test TrainingJob
        job = pynlm.TrainingJob()
        print("✓ Created TrainingJob")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed during data structure testing: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_enums():
    """Test enumerations"""
    print("\nTesting enumerations...")
    
    try:
        import pynlm
        
        # Test RLAlgorithm enum
        algorithms = [
            pynlm.RLAlgorithm.TABULAR_Q_LEARNING,
            pynlm.RLAlgorithm.DEEP_Q_NETWORK,
            pynlm.RLAlgorithm.POLICY_GRADIENT,
            pynlm.RLAlgorithm.Q_LEARNING,
            pynlm.RLAlgorithm.SARSA,
            pynlm.RLAlgorithm.EXPECTATION_MAXIMIZATION,
            pynlm.RLAlgorithm.PPO,
            pynlm.RLAlgorithm.A2C,
            pynlm.RLAlgorithm.DDPG,
            pynlm.RLAlgorithm.SAC
        ]
        
        print(f"✓ RLAlgorithm enum has {len(algorithms)} values")
        
        # Test ExplorationStrategy enum
        strategies = [
            pynlm.ExplorationStrategy.EPSILON_GREEDY,
            pynlm.ExplorationStrategy.UCB1,
            pynlm.ExplorationStrategy.THERMAL,
            pynlm.ExplorationStrategy.NOISE,
            pynlm.ExplorationStrategy.COUNT,
            pynlm.ExplorationStrategy.ADAPTIVE
        ]
        
        print(f"✓ ExplorationStrategy enum has {len(strategies)} values")
        
        # Test DifficultyLevel enum
        difficulties = [
            pynlm.DifficultyLevel.EASY,
            pynlm.DifficultyLevel.MEDIUM,
            pynlm.DifficultyLevel.HARD,
            pynlm.DifficultyLevel.EXPERT,
            pynlm.DifficultyLevel.NIGHTMARE
        ]
        
        print(f"✓ DifficultyLevel enum has {len(difficulties)} values")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed during enumeration testing: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    print("=" * 60)
    print("Testing Neural Learning Machine - Training Loop Utilities")
    print("=" * 60)
    
    tests = [
        ("Import Testing", test_imports),
        ("Class Creation Testing", test_class_creation),
        ("Factory Function Testing", test_factory_functions),
        ("Data Structure Testing", test_data_structures),
        ("Enumeration Testing", test_enums)
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n{'=' * 60}")
        print(f"Running: {test_name}")
        print(f"{'=' * 60}")
        
        try:
            if test_func():
                print(f"✓ {test_name} PASSED")
                passed += 1
            else:
                print(f"✗ {test_name} FAILED")
        except Exception as e:
            print(f"✗ {test_name} ERROR: {e}")
            import traceback
            traceback.print_exc()
    
    print(f"\n{'=' * 60}")
    print(f"Test Summary: {passed}/{total} tests passed")
    print(f"{'=' * 60}")
    
    if passed == total:
        print("✓ All tests passed!")
        print("The training loop utilities are ready for use.")
    else:
        print("✗ Some tests failed.")
        print("Please check the implementation for issues.")
    
    return passed == total

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)