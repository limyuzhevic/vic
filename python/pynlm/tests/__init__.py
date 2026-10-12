"""
NLM Python Bindings Test Suite

Test suite for the NLM (Neural Learning Machine) Python bindings.

This package contains comprehensive tests covering all major functionality:

- Basic bindings functionality
- Configuration loading and management
- Brain operations and simulation
- Agent brain interface and control
- World simulation and spatial queries
- Sensory input handling
- Error handling and validation
- Performance characteristics and benchmarking
"""

from .test_bindings_basic import TestBasicBindings
from .test_bindings_config import TestConfigLoading, TestPythonicConfigFeatures, TestUtilityFunctions
from .test_bindings_brain import TestBrainInitialization, TestBrainSimulation, TestBrainRegions, TestBrainStatistics, TestPythonicBrainFeatures
from .test_bindings_agent import TestAgentBrainInitialization, TestSensoryProcessing, TestMotorOutput, TestNeuromodulation, TestDevelopmentalSystems, TestPythonicAgentFeatures
from .test_bindings_world import TestWorldCreation, TestWorldOperations, TestWorldObjectManagement, TestSpatialQueries, TestWorldConfiguration, TestUtilityFunctions as WorldUtilityFunctions
from .test_bindings_sensory import TestVisionSensory, TestAudioSensory, TestInternalSignalsSensory, TestSensoryInputBase, TestSensoryPercept
from .test_bindings_error_handling import TestConfigErrorHandling, TestTypeErrorHandling, TestResourceErrorHandling, TestDataErrorHandling, TestEdgeCaseErrorHandling, TestPythonicErrorHandling
from .test_bindings_performance import TestPerformanceCharacteristics, TestBenchmarkingPerformance, TestLargeScalePerformance, TestOperationLatency, TestScalingPerformance

__all__ = [
    "TestBasicBindings",
    "TestConfigLoading",
    "TestPythonicConfigFeatures",
    "TestUtilityFunctions",
    "TestBrainInitialization",
    "TestBrainSimulation",
    "TestBrainRegions",
    "TestBrainStatistics",
    "TestPythonicBrainFeatures",
    "TestAgentBrainInitialization",
    "TestSensoryProcessing",
    "TestMotorOutput",
    "TestNeuromodulation",
    "TestDevelopmentalSystems",
    "TestPythonicAgentFeatures",
    "TestWorldCreation",
    "TestWorldOperations",
    "TestWorldObjectManagement",
    "TestSpatialQueries",
    "TestWorldConfiguration",
    "WorldUtilityFunctions",
    "TestVisionSensory",
    "TestAudioSensory",
    "TestInternalSignalsSensory",
    "TestSensoryInputBase",
    "TestSensoryPercept",
    "TestConfigErrorHandling",
    "TestTypeErrorHandling",
    "TestResourceErrorHandling",
    "TestDataErrorHandling",
    "TestEdgeCaseErrorHandling",
    "TestPythonicErrorHandling",
    "TestPerformanceCharacteristics",
    "TestBenchmarkingPerformance",
    "TestLargeScalePerformance",
    "TestOperationLatency",
    "TestScalingPerformance",
]