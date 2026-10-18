#!/usr/bin/env python3
"""
Example Validation Script
========================

This script validates that all NLM example files can be imported and have basic functionality.
It checks for syntax errors, import issues, and basic functionality without running full simulations.

File: examples/example_validator.py
"""

import sys
import os
import importlib.util
from datetime import datetime

# Add the examples directory to the path
examples_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, examples_dir)

class ExampleValidator:
    """Validates NLM example files."""
    
    def __init__(self):
        self.examples_dir = examples_dir
        self.results = []
    
    def validate_example(self, example_name: str) -> dict:
        """
        Validate a single example file.
        
        Args:
            example_name: Name of the example file (without .py)
            
        Returns:
            dict: Validation results
        """
        print(f"\n🔍 Validating {example_name}...")
        
        result = {
            "name": example_name,
            "file_path": os.path.join(self.examples_dir, f"{example_name}.py"),
            "status": "unknown",
            "errors": [],
            "warnings": [],
            "imports": [],
            "functions": [],
            "lines": 0
        }
        
        try:
            # Check if file exists
            if not os.path.exists(result["file_path"]):
                result["errors"].append(f"File not found: {result['file_path']}")
                result["status"] = "error"
                return result
            
            # Check file size
            file_size = os.path.getsize(result["file_path"])
            if file_size == 0:
                result["errors"].append("File is empty")
                result["status"] = "error"
                return result
            
            # Try to import the module
            spec = importlib.util.spec_from_file_location(example_name, result["file_path"])
            if spec is None:
                result["errors"].append("Cannot load module specification")
                result["status"] = "error"
                return result
            
            module = importlib.util.module_from_spec(spec)
            
            # Mock pynlm for import testing if not available
            try:
                import pynlm as nlm
                result["imports"].append("pynlm - SUCCESS")
            except ImportError:
                result["warnings"].append("pynlm module not available - using mocks")
                # Create mock pynlm module
                mock_nlm = type(sys)('pynlm')
                
                # Mock classes and methods
                class MockConfig:
                    def __init__(self):
                        self._data = {}
                    
                    def set(self, key, value):
                        self._data[key] = value
                    
                    def get(self, key, default=None):
                        return self._data.get(key, default)
                    
                    def has(self, key):
                        return key in self._data
                    
                    def getKeys(self):
                        return list(self._data.keys())
                    
                    def clear(self):
                        self._data.clear()
                    
                    def summary(self):
                        return "Mock config"
                    
                    def validateConsistency(self):
                        return True
                    
                    def validateCompleteness(self):
                        return True
                    
                    def validateFilePath(self, path):
                        return True
                    
                    def addParameterDefinition(self, *args):
                        pass
                    
                    def loadFromFile(self, path):
                        pass
                
                class MockBrain:
                    def __init__(self, config):
                        self.config = config
                    
                    def initialize(self):
                        pass
                    
                    def step(self, step, time=None):
                        pass
                    
                    def getTotalNeuronCount(self):
                        return 1000
                    
                    def getTotalSynapseCount(self):
                        return 5000
                    
                    def getFiringNeuronCount(self):
                        return 100
                    
                    def getAverageFiringRate(self):
                        return 10.0
                    
                    def getExcitationInhibitionRatio(self):
                        return 1.5
                    
                    def getTotalSpikeCount(self):
                        return 5000
                    
                    def getDevelopmentalStage(self):
                        class MockStage:
                            name = "Adult"
                        return MockStage()
                    
                    def getAllNeuronIds(self):
                        return []
                    
                    def getConnectionMatrix(self):
                        return {}
                    
                    def setExternalInput(self, *args):
                        pass
                    
                    def getNeuralTrace(self):
                        return []
                
                class MockAgentBrain:
                    def __init__(self, brain):
                        self.brain = brain
                    
                    def initialize(self, world):
                        pass
                    
                    def getSensoryInputSize(self):
                        return 100
                    
                    def getMotorOutputSize(self):
                        return 10
                    
                    def processSensoryInput(self, percept):
                        pass
                    
                    def decodeMotorCommand(self):
                        return MockMotorCommand()
                    
                    def applyRewardModulation(self, reward, predicted):
                        pass
                    
                    def updateDevelopment(self, timestep):
                        pass
                    
                    def getDevelopmentalStage(self):
                        class MockStage:
                            name = "Adult"
                        return MockStage()
                    
                    def getNeuromodulationLevel(self):
                        return 0.5
                    
                    def getCuriosityLevel(self):
                        return 0.3
                    
                    def getNoveltyLevel(self):
                        return 0.4
                    
                    def getPredictionError(self):
                        return 0.1
                    
                    def reset(self):
                        pass
                    
                    def enableRewardModulation(self, enable):
                        pass
                    
                    def enableStructuralPlasticity(self, enable):
                        pass
                    
                    def enableDevelopment(self, enable):
                        pass
                    
                    def enableCuriosity(self, enable):
                        pass
                    
                    def isRewardModulationEnabled(self):
                        return True
                    
                    def isStructuralPlasticityEnabled(self):
                        return True
                    
                    def isDevelopmentEnabled(self):
                        return True
                    
                    def isCuriosityEnabled(self):
                        return True
                    
                    def setExplorationMode(self, mode):
                        pass
                    
                    def getActionSelectionStats(self):
                        return []
                    
                    def setCuriosityStrategy(self, strategy):
                        pass
                    
                    def getBehaviorHistory(self):
                        return []
                    
                    def enableLearningMode(self, enable):
                        pass
                
                class MockMotorCommand:
                    pass
                
                class MockWorld:
                    def __init__(self):
                        self._time = 0.0
                    
                    def configure(self, *args):
                        pass
                    
                    def reset(self):
                        pass
                    
                    def update(self, timestep):
                        self._time += timestep
                    
                    def applyMotorCommand(self, action, current_time):
                        class MockActionResult:
                            reward = 1.0
                            success = True
                            message = "Success"
                        return MockActionResult()
                    
                    def getSensoryPercept(self):
                        class MockPercept:
                            pass
                        return MockPercept()
                    
                    def getAgentBody(self):
                        class MockBody:
                            x = 0.0
                            y = 0.0
                            orientation = 0.0
                            velocityX = 0.0
                            velocityY = 0.0
                            angularVelocity = 0.0
                            energy = 100.0
                            health = 100.0
                            age = 0.0
                            isMoving = False
                            isTurning = False
                            lastActionTime = 0.0
                        return MockBody()
                    
                    def getObjectsByType(self, obj_type):
                        return []
                    
                    def getSimulationTime(self):
                        return self._time
                
                class MockSimpleWorld:
                    def __init__(self):
                        self._world = MockWorld()
                    
                    def configure(self, *args):
                        self._world.configure(*args)
                    
                    def reset(self):
                        self._world.reset()
                    
                    def setAgentStart(self, *args):
                        pass
                    
                    def update(self, timestep):
                        self._world.update(timestep)
                    
                    def applyMotorCommand(self, *args):
                        return self._world.applyMotorCommand(*args)
                    
                    def getSensoryPercept(self):
                        return self._world.getSensoryPercept()
                    
                    def getAgentBody(self):
                        return self._world.getAgentBody()
                    
                    def addObject(self, obj):
                        pass
                    
                    def removeObject(self, *args):
                        pass
                    
                    def isValidPosition(self, *args):
                        return True
                    
                    def getWidth(self):
                        return 200.0
                    
                    def getHeight(self):
                        return 200.0
                    
                    def getMaxEnergy(self):
                        return 1000.0
                    
                    def setMaxEnergy(self, e):
                        pass
                    
                    def getEnergyDecayRate(self):
                        return 0.01
                    
                    def setEnergyDecayRate(self, r):
                        pass
                    
                    def getSimulationTime(self):
                        return self._world.getSimulationTime()
                    
                    def setRandomSeed(self, seed):
                        pass
                    
                    def getRandomSeed(self):
                        return 42
                
                # Add mocks to the module
                mock_nlm.Config = MockConfig
                mock_nlm.createBrain = lambda config: MockBrain(config)
                mock_nlm.createAgentBrain = lambda brain: MockAgentBrain(brain)
                mock_nlm.createSimpleWorld = lambda: MockSimpleWorld()
                mock_nlm.WorldObjectType = type('WorldObjectType', (), {
                    'Empty': 0, 'Resource': 1, 'Hazard': 2, 'Wall': 3, 'Marker': 4
                })
                mock_nlm.WorldObject = lambda *args, **kwargs: type('WorldObject', (), {
                    'x': kwargs.get('x', 0),
                    'y': kwargs.get('y', 0),
                    'type': kwargs.get('type', 0),
                    'value': kwargs.get('value', 0),
                    'radius': kwargs.get('radius', 0.5),
                    'active': True
                })()
                
                sys.modules['pynlm'] = mock_nlm
                result["imports"].append("pynlm - MOCKED")
            
            # Execute the module to check for syntax errors
            spec.loader.exec_module(module)
            result["status"] = "success"
            result["lines"] = len(open(result["file_path"], 'r').readlines())
            
            # Try to find functions in the module
            for attr_name in dir(module):
                if not attr_name.startswith('_'):
                    attr = getattr(module, attr_name)
                    if callable(attr) and not attr_name.startswith('test_'):
                        result["functions"].append(attr_name)
            
        except SyntaxError as e:
            result["errors"].append(f"Syntax error: {e}")
            result["status"] = "error"
        except Exception as e:
            result["errors"].append(f"Import error: {type(e).__name__}: {e}")
            result["status"] = "error"
        
        return result
    
    def validate_all_examples(self) -> dict:
        """
        Validate all example files.
        
        Returns:
            dict: Complete validation results
        """
        print("\n" + "="*70)
        print("🔍 NLM EXAMPLE VALIDATION")
        print("="*70)
        print(f"Validated at: {datetime.now()}")
        print()
        
        # List of example files to validate
        example_files = [
            "advanced_agent_simulation",
            "research_simulation", 
            "performance_benchmark",
            "robust_simulation",
            "comprehensive_demo"
        ]
        
        # Validate each example
        for example_name in example_files:
            result = self.validate_example(example_name)
            self.results.append(result)
            
            # Print result summary
            status_icon = {"success": "✅", "error": "❌"}.get(result["status"], "⚠️")
            print(f"{status_icon} {example_name}: {result['status']}")
            
            if result["warnings"]:
                print(f"   ⚠️  Warnings: {len(result['warnings'])}")
            
            if result["errors"]:
                print(f"   ❌ Errors: {len(result['errors'])}")
                for error in result["errors"][:2]:  # Show first 2 errors
                    print(f"      - {error}")
        
        # Generate summary report
        summary = self._generate_summary_report()
        
        return summary
    
    def _generate_summary_report(self) -> dict:
        """
        Generate comprehensive summary report.
        
        Returns:
            dict: Summary report
        """
        total_examples = len(self.results)
        successful_examples = len([r for r in self.results if r["status"] == "success"])
        error_examples = len([r for r in self.results if r["status"] == "error"])
        
        # Calculate total lines
        total_lines = sum(r["lines"] for r in self.results)
        
        # Calculate average file size
        total_size = sum(os.path.getsize(r["file_path"]) for r in self.results if os.path.exists(r["file_path"]))
        avg_size_kb = total_size / 1024 / successful_examples if successful_examples > 0 else 0
        
        summary = {
            "validation_time": datetime.now().isoformat(),
            "total_examples": total_examples,
            "successful_examples": successful_examples,
            "error_examples": error_examples,
            "success_rate": (successful_examples / total_examples * 100) if total_examples > 0 else 0,
            "total_lines_of_code": total_lines,
            "average_file_size_kb": avg_size_kb,
            "detailed_results": self.results
        }
        
        print(f"\n" + "="*70)
        print("📊 VALIDATION SUMMARY")
        print("="*70)
        print(f"Total examples validated: {summary['total_examples']}")
        print(f"Successful examples: {summary['successful_examples']}")
        print(f"Examples with errors: {summary['error_examples']}")
        print(f"Success rate: {summary['success_rate']:.1f}%")
        print(f"Total lines of code: {summary['total_lines_of_code']:,}")
        print(f"Average file size: {summary['average_file_size_kb']:.1f} KB")
        
        # Print individual results
        print(f"\n📋 DETAILED RESULTS:")
        for result in self.results:
            status = result["status"]
            icon = {"success": "✅", "error": "❌"}.get(status, "⚠️")
            print(f"  {icon} {result['name']} ({result['lines']} lines)")
            
            if result["warnings"]:
                print(f"     ⚠️  {len(result['warnings'])} warnings")
            
            if result["errors"]:
                print(f"     ❌ {len(result['errors'])} errors")
        
        return summary

if __name__ == "__main__":
    # Run validation
    validator = ExampleValidator()
    summary = validator.validate_all_examples()
    
    # Exit with appropriate code
    if summary["error_examples"] > 0:
        print(f"\n❌ Validation completed with {summary['error_examples']} errors")
        sys.exit(1)
    else:
        print(f"\n✅ All examples validated successfully")
        sys.exit(0)
