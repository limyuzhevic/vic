#!/usr/bin/env python3
"""
NLM Validation Tests
Comprehensive schema validation and consistency tests for NLM
"""

import pytest
import sys
import os
import tempfile
import json
import yaml
import re
from pathlib import Path
from typing import Dict, Any, List, Optional, Tuple

# Add project root to Python path
sys.path.insert(0, str(Path(__file__).parent))

class NLMValidationTester:
    """Comprehensive validation testing for NLM"""
    
    def __init__(self):
        self.test_dir = tempfile.mkdtemp(prefix='nlm_validation_test_')
        self.temp_files = []
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Cleanup
        import shutil
        shutil.rmtree(self.test_dir, ignore_errors=True)
        for filepath in self.temp_files:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def create_test_configuration(self, profile: str = 'default') -> Dict[str, Any]:
        """Create test configuration based on profile"""
        base_configs = {
            'default': {
                'brain': {
                    'neuron_count': 1000,
                    'synapse_density': 0.1,
                    'v_thresh': -50.0,
                    'v_rest': -70.0,
                    'v_reset': -75.0,
                    'tau_mem': 20.0,
                    'tau_ref': 2.0
                },
                'performance': {
                    'profile': 'balanced',
                    'optimization_level': 2,
                    'simd_enable': True,
                    'parallel_enable': True,
                    'memory_pool_enable': True,
                    'caching_enable': True
                },
                'neuromod': {
                    'curiosity': {
                        'enable': True,
                        'weight': 0.5,
                        'sensitivity': 1.0
                    },
                    'novelty': {
                        'enable': True,
                        'sensitivity': 1.0
                    },
                    'dopamine': {
                        'scale': 1.0
                    }
                },
                'plasticity': {
                    'stdp': {
                        'enable': True,
                        'learning_rate': 0.001,
                        'tau_plus': 20.0,
                        'tau_minus': 20.0
                    },
                    'hebbian': {
                        'enable': True
                    },
                    'structural': {
                        'enable': True
                    }
                }
            },
            'exploration': {
                'brain': {
                    'neuron_count': 1500,
                    'synapse_density': 0.12
                },
                'neuromod': {
                    'curiosity': {
                        'enable': True,
                        'weight': 0.8,
                        'sensitivity': 1.5
                    },
                    'novelty': {
                        'enable': True,
                        'sensitivity': 1.5
                    }
                },
                'plasticity': {
                    'structural': {'enable': True},
                    'hebbian': {'enable': True}
                }
            },
            'performance_optimized': {
                'brain': {
                    'neuron_count': 2000,
                    'synapse_density': 0.15
                },
                'performance': {
                    'profile': 'speed',
                    'optimization_level': 3,
                    'simd_enable': True,
                    'parallel_enable': True,
                    'memory_pool_enable': True,
                    'caching_enable': True
                },
                'learning': {
                    'plasticity': {
                        'adaptive': {
                            'enable': False
                        }
                    },
                    'curiosity': {
                        'weight': 0.3,
                        'sensitivity': 0.5
                    },
                    'novelty': {
                        'sensitivity': 0.7
                    }
                }
            },
            'memory_optimized': {
                'brain': {
                    'neuron_count': 2000,
                    'synapse_density': 0.08
                },
                'performance': {
                    'profile': 'memory',
                    'optimization_level': 3,
                    'simd_enable': False,
                    'parallel_enable': False,
                    'memory_pool_enable': True,
                    'caching_enable': True
                },
                'memory': {
                    'pools': {'enable': True},
                    'consolidation': {'enabled': True, 'policy': 'aggressive'}
                }
            },
            'research_attention': {
                'brain': {
                    'neuron_count': 500,
                    'synapse_density': 0.05
                },
                'performance': {
                    'profile': 'research',
                    'optimization_level': 1
                },
                'experiment': {
                    'research': {
                        'focus': 'attention',
                        'hypothesis': 'novelty_drives_attention'
                    },
                    'prediction': {
                        'horizon': 20,
                        'confidence_threshold': 0.9,
                        'error_scaling': {'dynamic': True}
                    },
                    'development': {
                        'critical_period': {
                            'enabled': True,
                            'duration': 600,
                            'plasticity_rate': 0.05
                        }
                    }
                }
            }
        }
        
        return base_configs.get(profile, base_configs['default'])
    
    def validate_json_schema(self, config: Dict[str, Any], schema: Dict[str, Any] = None) -> Tuple[bool, List[str]]:
        """Validate configuration against JSON schema rules"""
        errors = []
        
        # Define validation rules
        validation_rules = schema or {
            'brain.neuron_count': {
                'type': 'int',
                'min': 1,
                'max': 10000,
                'description': 'Number of neurons in the brain'
            },
            'brain.synapse_density': {
                'type': 'float',
                'min': 0.0,
                'max': 1.0,
                'description': 'Density of synaptic connections'
            },
            'brain.v_thresh': {
                'type': 'float',
                'min': -100.0,
                'max': 0.0,
                'description': 'Neuron threshold potential'
            },
            'performance.optimization_level': {
                'type': 'int',
                'min': 1,
                'max': 3,
                'description': 'Performance optimization level'
            },
            'performance.profile': {
                'type': 'string',
                'allowed_values': ['balanced', 'speed', 'memory', 'accuracy', 'adaptive'],
                'description': 'Performance profile type'
            },
            'neuromod.curiosity.weight': {
                'type': 'float',
                'min': 0.0,
                'max': 1.0,
                'description': 'Curiosity neuromodulation weight'
            },
            'neuromod.curiosity.sensitivity': {
                'type': 'float',
                'min': 0.0,
                'max': 2.0,
                'description': 'Curiosity neuromodulation sensitivity'
            },
            'plasticity.stdp.learning_rate': {
                'type': 'float',
                'min': 0.0,
                'max': 0.1,
                'description': 'STDP learning rate'
            }
        }
        
        # Validate each rule
        for key, rules in validation_rules.items():
            if self._has_nested_key(config, key):
                value = self._get_nested_value(config, key)
                
                # Type validation
                type_valid = self._validate_type(value, rules['type'])
                if not type_valid:
                    errors.append(f"Type mismatch for {key}: expected {rules['type']}, got {type(value).__name__}")
                
                # Range validation
                if 'min' in rules and value < rules['min']:
                    errors.append(f"Value {key}={value} < minimum {rules['min']}")
                
                if 'max' in rules and value > rules['max']:
                    errors.append(f"Value {key}={value} > maximum {rules['max']}")
                
                # Allowed values validation
                if 'allowed_values' in rules and value not in rules['allowed_values']:
                    errors.append(f"Value {key}={value} not in allowed values: {rules['allowed_values']}")
        
        return len(errors) == 0, errors
    
    def validate_yaml_format(self, config: Dict[str, Any]) -> Tuple[bool, List[str]]:
        """Validate configuration can be serialized to YAML"""
        errors = []
        
        try:
            # Try to convert to YAML
            import yaml
            yaml_string = yaml.dump(config, default_flow_style=False)
            
            # Try to parse back
            parsed_config = yaml.safe_load(yaml_string)
            
            # Basic structure validation
            if not isinstance(parsed_config, dict):
                errors.append("YAML serialization did not preserve dictionary structure")
            
            # Check key preservation
            original_keys = self._get_all_keys(config)
            parsed_keys = self._get_all_keys(parsed_config)
            
            if set(original_keys) != set(parsed_keys):
                errors.append("Key mismatch between original and parsed YAML configuration")
                
        except ImportError:
            errors.append("PyYAML not available for YAML validation")
        except Exception as e:
            errors.append(f"YAML validation failed: {e}")
        
        return len(errors) == 0, errors
    
    def validate_parameter_consistency(self, config: Dict[str, Any]) -> Tuple[bool, List[str]]:
        """Validate parameter consistency across configuration"""
        errors = []
        
        # Get all configuration values
        neuron_count = self._get_nested_value(config, 'brain.neuron_count', 1000)
        synapse_density = self._get_nested_value(config, 'brain.synapse_density', 0.1)
        optimization_level = self._get_nested_value(config, 'performance.optimization_level', 2)
        profile = self._get_nested_value(config, 'performance.profile', 'balanced')
        curiosity_weight = self._get_nested_value(config, 'neuromod.curiosity.weight', 0.5)
        curiosity_sensitivity = self._get_nested_value(config, 'neuromod.curiosity.sensitivity', 1.0)
        stdp_learning_rate = self._get_nested_value(config, 'plasticity.stdp.learning_rate', 0.001)
        
        # Cross-parameter consistency checks
        
        # 1. Neuron count vs synapse density consistency
        expected_synapses = neuron_count * synapse_density
        if expected_synapses < 0:
            errors.append(f"Invalid synapse calculation: {neuron_count} neurons * {synapse_density} density = {expected_synapses}")
        elif expected_synapses > neuron_count * 10:  # Reasonable upper bound
            errors.append(f"Excessive synapse count: {expected_synapses} exceeds reasonable limit")
        
        # 2. Profile vs optimization level consistency
        profile_optimization_map = {
            'balanced': 2,
            'speed': 3,
            'memory': 3,
            'accuracy': 3,
            'adaptive': 3
        }
        
        expected_optimization = profile_optimization_map.get(profile, 2)
        if optimization_level != expected_optimization:
            errors.append(f"Profile '{profile}' typically uses optimization level {expected_optimization}, "
                        f"but got {optimization_level}")
        
        # 3. Neuromodulation parameter consistency
        if profile == 'speed' and curiosity_weight > 0.6:
            errors.append(f"Speed profile should have lower curiosity weight (<0.6), got {curiosity_weight}")
        
        if profile == 'memory' and curiosity_sensitivity > 1.2:
            errors.append(f"Memory profile should have lower curiosity sensitivity (<1.2), got {curiosity_sensitivity}")
        
        # 4. Plasticity parameter consistency
        if optimization_level == 3 and stdp_learning_rate > 0.005:
            errors.append(f"High optimization level (3) should use lower STDP learning rate (<0.005), got {stdp_learning_rate}")
        
        # 5. Development parameter sanity checks
        critical_period_enabled = self._get_nested_value(config, 'experiment.development.critical_period.enabled', False)
        if critical_period_enabled:
            critical_period_duration = self._get_nested_value(config, 'experiment.development.critical_period.duration', 0)
            if critical_period_duration < 100 or critical_period_duration > 5000:
                errors.append(f"Critical period duration {critical_period_duration} outside reasonable range (100-5000)")
        
        return len(errors) == 0, errors
    
    def validate_performance_impact(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze and validate performance impact of configuration"""
        # Estimate performance metrics based on configuration
        
        neuron_count = self._get_nested_value(config, 'brain.neuron_count', 1000)
        optimization_level = self._get_nested_value(config, 'performance.optimization_level', 2)
        profile = self._get_nested_value(config, 'performance.profile', 'balanced')
        curiosity_weight = self._get_nested_value(config, 'neuromod.curiosity.weight', 0.5)
        memory_pool_enabled = self._get_nested_value(config, 'memory.pools.enable', False)
        
        # Performance impact calculations
        performance_impact = {
            'estimated_computational_load': self._estimate_computational_load(neuron_count, optimization_level),
            'memory_footprint_estimate': self._estimate_memory_footprint(neuron_count, profile),
            'performance_rating': self._calculate_performance_rating(neuron_count, optimization_level, profile),
            'stability_assessment': self._assess_stability(config),
            'optimization_efficiency': self._calculate_optimization_efficiency(config)
        }
        
        return performance_impact
    
    def _estimate_computational_load(self, neuron_count: int, optimization_level: int) -> Dict[str, Any]:
        """Estimate computational load based on neuron count and optimization"""
        # Base computational requirements
        base_operations_per_neuron = 50  # Operations per neuron per timestep
        optimization_factors = {
            1: 0.8,  # Basic optimization
            2: 1.0,  # Balanced optimization
            3: 1.3   # Aggressive optimization
        }
        
        estimated_ops_per_timestep = neuron_count * base_operations_per_neuron * optimization_factors.get(optimization_level, 1.0)
        estimated_timesteps_per_second = 1000 / estimated_ops_per_timestep
        
        return {
            'estimated_operations_per_timestep': int(estimated_ops_per_timestep),
            'estimated_timesteps_per_second': estimated_timesteps_per_second,
            'computational_intensity': 'low' if estimated_ops_per_timestep < 10000 else 'medium' if estimated_ops_per_timestep < 50000 else 'high',
            'cpu_requirements': 'low' if estimated_ops_per_timestep < 10000 else 'medium' if estimated_ops_per_timestep < 50000 else 'high'
        }
    
    def _estimate_memory_footprint(self, neuron_count: int, profile: str) -> Dict[str, Any]:
        """Estimate memory footprint based on neuron count and profile"""
        # Base memory requirements per neuron
        base_memory_per_neuron = 0.01  # MB per neuron
        
        # Profile multipliers
        profile_multipliers = {
            'balanced': 1.0,
            'speed': 1.1,
            'memory': 0.9,
            'accuracy': 1.05,
            'adaptive': 1.05
        }
        
        estimated_memory_mb = neuron_count * base_memory_per_neuron * profile_multipliers.get(profile, 1.0)
        
        return {
            'estimated_memory_mb': estimated_memory_mb,
            'neurons_per_mb': neuron_count / estimated_memory_mb if estimated_memory_mb > 0 else 0,
            'memory_efficiency': 'good' if neuron_count / estimated_memory_mb > 50 else 'moderate' if neuron_count / estimated_memory_mb > 20 else 'poor'
        }
    
    def _calculate_performance_rating(self, neuron_count: int, optimization_level: int, profile: str) -> float:
        """Calculate performance rating based on multiple factors"""
        # Performance factors
        neuron_factor = min(2.0, neuron_count / 1000.0)  # Rating decreases for very large networks
        optimization_factor = optimization_level / 3.0  # Higher optimization level = better performance
        profile_factor = {
            'speed': 1.2,
            'balanced': 1.0,
            'accuracy': 1.1,
            'memory': 0.9,
            'adaptive': 1.05
        }.get(profile, 1.0)
        
        # Combined performance rating
        performance_rating = neuron_factor * optimization_factor * profile_factor
        
        return max(0.1, min(5.0, performance_rating))  # Scale 0.1-5.0
    
    def _assess_stability(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Assess configuration stability based on parameter combinations"""
        stability_score = 1.0
        
        # Check for potentially unstable combinations
        
        # 1. High curiosity with low optimization
        optimization_level = self._get_nested_value(config, 'performance.optimization_level', 2)
        curiosity_weight = self._get_nested_value(config, 'neuromod.curiosity.weight', 0.5)
        
        if optimization_level == 1 and curiosity_weight > 0.7:
            stability_score -= 0.3
        
        # 2. High novelty sensitivity with low optimization
        novelty_sensitivity = self._get_nested_value(config, 'neuromod.novelty.sensitivity', 1.0)
        if optimization_level == 1 and novelty_sensitivity > 1.5:
            stability_score -= 0.3
        
        # 3. High learning rate with high optimization
        stdp_learning_rate = self._get_nested_value(config, 'plasticity.stdp.learning_rate', 0.001)
        if optimization_level == 3 and stdp_learning_rate > 0.005:
            stability_score -= 0.2
        
        # 4. Critical period too long
        critical_period_duration = self._get_nested_value(config, 'experiment.development.critical_period.duration', 0)
        if critical_period_duration > 2000:
            stability_score -= 0.2
        
        # Ensure stability score is within bounds
        stability_score = max(0.0, min(1.0, stability_score))
        
        stability_assessment = 'stable' if stability_score > 0.7 else 'moderate' if stability_score > 0.4 else 'unstable'
        
        return {
            'stability_score': stability_score,
            'stability_assessment': stability_assessment,
            'risk_factors': self._identify_risk_factors(config),
            'recommendations': self._generate_stability_recommendations(config)
        }
    
    def _identify_risk_factors(self, config: Dict[str, Any]) -> List[str]:
        """Identify potential risk factors in configuration"""
        risk_factors = []
        
        # Check for identified risk factors
        optimization_level = self._get_nested_value(config, 'performance.optimization_level', 2)
        curiosity_weight = self._get_nested_value(config, 'neuromod.curiosity.weight', 0.5)
        stdp_learning_rate = self._get_nested_value(config, 'plasticity.stdp.learning_rate', 0.001)
        
        if optimization_level == 1 and curiosity_weight > 0.7:
            risk_factors.append("High curiosity with low optimization may cause instability")
        
        if optimization_level == 3 and stdp_learning_rate > 0.005:
            risk_factors.append("High optimization with high learning rate may cause oscillations")
        
        if curiosity_weight > 0.8 and stdp_learning_rate > 0.003:
            risk_factors.append("Strong curiosity and plasticity combination may lead to runaway behavior")
        
        return risk_factors
    
    def _generate_stability_recommendations(self, config: Dict[str, Any]) -> List[str]:
        """Generate stability recommendations for configuration"""
        recommendations = []
        
        # Check for recommendations
        optimization_level = self._get_nested_value(config, 'performance.optimization_level', 2)
        
        if optimization_level == 1:
            recommendations.append("Consider increasing optimization level for better stability")
            recommendations.append("Monitor neural activity closely with low optimization")
        
        if optimization_level == 3:
            recommendations.append("High optimization level requires careful monitoring of neural dynamics")
            recommendations.append("Consider reducing STDP learning rate to prevent oscillations")
        
        curiosity_weight = self._get_nested_value(config, 'neuromod.curiosity.weight', 0.5)
        if curiosity_weight > 0.7:
            recommendations.append("High curiosity weight may cause excessive exploration - consider reducing")
        
        return recommendations
    
    def _calculate_optimization_efficiency(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate optimization efficiency metrics"""
        # Calculate efficiency based on various factors
        
        neuron_count = self._get_nested_value(config, 'brain.neuron_count', 1000)
        optimization_level = self._get_nested_value(config, 'performance.optimization_level', 2)
        profile = self._get_nested_value(config, 'performance.profile', 'balanced')
        
        # Calculate efficiency score
        efficiency_score = (
            (optimization_level / 3.0) * 0.4 +  # Optimization level contribution
            min(1.0, neuron_count / 5000.0) * 0.3 +  # Neuron count efficiency
            (1.0 if profile in ['speed', 'accuracy'] else 0.8) * 0.3  # Profile efficiency
        )
        
        efficiency_rating = 'excellent' if efficiency_score > 0.8 else 'good' if efficiency_score > 0.6 else 'moderate' if efficiency_score > 0.4 else 'poor'
        
        return {
            'efficiency_score': efficiency_score,
            'efficiency_rating': efficiency_rating,
            'optimization_potential': self._calculate_optimization_potential(config)
        }
    
    def _calculate_optimization_potential(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate potential for further optimization"""
        potential_factors = []
        
        optimization_level = self._get_nested_value(config, 'performance.optimization_level', 2)
        
        if optimization_level < 3:
            potential_factors.append("Increase optimization level for better performance")
        
        if self._get_nested_value(config, 'performance.simd_enable', False) is False:
            potential_factors.append("Enable SIMD vectorization for better performance")
        
        if self._get_nested_value(config, 'performance.parallel_enable', False) is False:
            potential_factors.append("Enable parallel processing for better performance")
        
        if self._get_nested_value(config, 'performance.memory_pool_enable', False) is False:
            potential_factors.append("Enable memory pools for better memory efficiency")
        
        return {
            'potential_improvement': len(potential_factors),
            'optimization_recommendations': potential_factors,
            'max_possible_improvement': min(0.5, len(potential_factors) * 0.1) if potential_factors else 0.0
        }
    
    def _has_nested_key(self, config: Dict[str, Any], key: str) -> bool:
        """Check if configuration has nested key"""
        parts = key.split('.')
        target = config
        for part in parts:
            if isinstance(target, dict) and part in target:
                target = target[part]
            else:
                return False
        return True
    
    def _get_nested_value(self, config: Dict[str, Any], key: str, default: Any = None) -> Any:
        """Get nested value from configuration"""
        parts = key.split('.')
        target = config
        for part in parts:
            if isinstance(target, dict) and part in target:
                target = target[part]
            else:
                return default
        return target
    
    def _validate_type(self, value: Any, expected_type: str) -> bool:
        """Validate value type"""
        type_mapping = {
            'int': int,
            'float': (float, int),
            'bool': bool,
            'string': str,
            'list': list,
            'dict': dict
        }
        
        expected_class = type_mapping.get(expected_type)
        return isinstance(value, expected_class)
    
    def _get_all_keys(self, config: Dict[str, Any]) -> List[str]:
        """Get all keys from configuration recursively"""
        keys = []
        
        def extract_keys(obj, prefix=''):
            if isinstance(obj, dict):
                for key, value in obj.items():
                    full_key = f"{prefix}.{key}" if prefix else key
                    keys.append(full_key)
                    extract_keys(value, full_key)
            elif isinstance(obj, list):
                for i, item in enumerate(obj):
                    full_key = f"{prefix}[{i}]"
                    extract_keys(item, full_key)
        
        extract_keys(config)
        return keys

if __name__ == "__main__":
    print("🧪 Running NLM Validation Tests")
    print("=" * 60)
    
    tester = NLMValidationTester()
    
    # Test different configuration profiles
    profiles = ['default', 'exploration', 'performance_optimized', 'memory_optimized', 'research_attention']
    
    all_results = []
    
    for profile in profiles:
        print(f"\n📋 Testing profile: {profile}")
        
        # Create test configuration
        config = tester.create_test_configuration(profile)
        
        # Test JSON schema validation
        print("   Running JSON schema validation...")
        schema_valid, schema_errors = tester.validate_json_schema(config)
        
        if schema_valid:
            print(f"     ✅ JSON schema validation passed")
        else:
            print(f"     ❌ JSON schema validation failed: {schema_errors}")
        
        # Test YAML format validation
        print("   Running YAML format validation...")
        yaml_valid, yaml_errors = tester.validate_yaml_format(config)
        
        if yaml_valid:
            print(f"     ✅ YAML format validation passed")
        else:
            print(f"     ❌ YAML format validation failed: {yaml_errors}")
        
        # Test parameter consistency
        print("   Running parameter consistency validation...")
        consistency_valid, consistency_errors = tester.validate_parameter_consistency(config)
        
        if consistency_valid:
            print(f"     ✅ Parameter consistency validation passed")
        else:
            print(f"     ❌ Parameter consistency validation failed: {consistency_errors}")
        
        # Test performance impact analysis
        print("   Running performance impact analysis...")
        performance_impact = tester.validate_performance_impact(config)
        
        print(f"     📊 Performance impact:")
        print(f"        Load intensity: {performance_impact['estimated_computational_load']['computational_intensity']}")
        print(f"        Memory efficiency: {performance_impact['memory_footprint_estimate']['memory_efficiency']}")
        print(f"        Performance rating: {performance_impact['performance_rating']:.2f}")
        print(f"        Stability: {performance_impact['stability_assessment']}")
        
        # Collect results
        test_result = {
            'profile': profile,
            'schema_valid': schema_valid,
            'schema_errors': schema_errors,
            'yaml_valid': yaml_valid,
            'yaml_errors': yaml_errors,
            'consistency_valid': consistency_valid,
            'consistency_errors': consistency_errors,
            'performance_impact': performance_impact,
            'overall_valid': schema_valid and yaml_valid and consistency_valid
        }
        
        all_results.append(test_result)
        
        print(f"     ✅ Profile {profile} testing completed")
    
    # Summary results
    print("\n" + "=" * 60)
    print("📊 Validation Test Results Summary:")
    
    passed = 0
    failed = 0
    
    for result in all_results:
        profile = result['profile']
        schema_ok = result['schema_valid']
        yaml_ok = result['yaml_valid']
        consistency_ok = result['consistency_valid']
        
        overall = result['overall_valid']
        status = "✅ PASSED" if overall else "❌ FAILED"
        
        print(f"   {status}: {profile}")
        print(f"     Schema: {'✅' if schema_ok else '❌'}, YAML: {'✅' if yaml_ok else '❌'}, "
              f"Consistency: {'✅' if consistency_ok else '❌'}")
        
        if overall:
            passed += 1
        else:
            failed += 1
            
            # Show specific errors
            if schema_errors:
                print(f"     Schema errors: {len(schema_errors)}")
            if yaml_errors:
                print(f"     YAML errors: {len(yaml_errors)}")
            if consistency_errors:
                print(f"     Consistency errors: {len(consistency_errors)}")
    
    print(f"\n🎯 Total: {passed + failed} profiles")
    print(f"   Passed: {passed}")
    print(f"   Failed: {failed}")
    
    # Calculate average performance impact
    if all_results:
        avg_performance_rating = np.mean([r['performance_impact']['performance_rating'] for r in all_results])
        avg_stability_score = np.mean([r['performance_impact']['stability_assessment'] for r in all_results])
        avg_efficiency_score = np.mean([r['performance_impact']['optimization_efficiency']['efficiency_score'] for r in all_results])
        
        print(f"\n📈 Average Validation Results:")
        print(f"   Performance rating: {avg_performance_rating:.2f}/5.0")
        print(f"   Stability assessment: {avg_stability_score:.2f}")
        print(f"   Efficiency score: {avg_efficiency_score:.2f}")
        
        # Save comprehensive validation results
        results_file = os.path.join(tester.test_dir, 'comprehensive_validation_results.json')
        with open(results_file, 'w') as f:
            json.dump({
                'test_type': 'comprehensive_validation',
                'profiles_tested': len(all_results),
                'results': all_results,
                'summary': {
                    'success_rate': passed / len(all_results) if all_results else 0,
                    'average_performance_rating': avg_performance_rating,
                    'average_efficiency_score': avg_efficiency_score,
                    'total_profiles': len(all_results)
                }
            }, f, indent=2)
        
        tester.temp_files.append(results_file)
    
    if failed > 0:
        print(f"\n⚠️  {failed} profile(s) failed validation. Please review the implementation.")
        sys.exit(1)
    else:
        print(f"\n✅ All {passed} profiles passed validation!")
        sys.exit(0)
