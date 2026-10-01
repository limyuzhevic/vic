# NLM Migration Guide
# ====================

This document provides comprehensive guidance for migrating from older versions of NLM (Neural Learning Machine) to the current version. It covers migration paths, compatibility considerations, breaking changes, and migration strategies.

## 1. Migration Overview

NLM has evolved significantly through its development phases, with each version introducing new features, improved performance, and architectural changes. This guide provides migration paths for users coming from:

- **Legacy versions** (pre-Phase 6)
- **Phase 5 and earlier** versions
- **Configuration-based systems**
- **Plugin-based extensions**

### Supported Migration Paths

| From Version | To Version | Complexity | Notes |
|--------------|------------|------------|-------|
| v0.9 - v1.0 | v1.0 | Low | Minor API improvements |
| v1.0 - v1.5 | v1.5 | Medium | New features, configuration changes |
| v1.5 - v2.0 | v2.0 | High | Major architecture changes |
| v2.0 - v2.5 | v2.5 | Medium | New capabilities, enhanced performance |
| v2.5 - v3.0 | v3.0 | High | Complete rewrite, new API |

## 2. Breaking Changes and Migration Strategies

### Major API Changes

#### Configuration System

**Before (Legacy)**:
```python
# Old configuration format
config = {
    'neuron_count': 1000,
    'plasticity_learning_rate': 0.01,
    'environment_width': 10
}
```

**After (Current)**:
```python
# New configuration format
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("environment.width", 10)
```

**Migration Strategy**:
1. Update configuration loading to use `pynlm.createDefaultConfig()`
2. Use dot notation for nested parameters (`brain.neuron_count` instead of `neuron_count`)
3. Add migration layer for old configuration formats
4. Validate and normalize configuration values

#### Brain Initialization

**Before (Legacy)**:
```python
# Old brain initialization
brain = Brain(config)
brain.initialize()
```

**After (Current)**:
```python
# New brain initialization
brain = pynlm.createBrain(config)
brain.initialize()
```

**Migration Strategy**:
1. Replace `Brain()` with `pynlm.createBrain()`
2. Ensure configuration is properly validated
3. Update initialization parameters if needed
4. Test initialization with small configurations first

#### Agent Interface

**Before (Legacy)**:
```python
# Old agent interface
agent = AgentBrain(brain)
agent.initialize(world)
```

**After (Current)**:
```python
# New agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

**Migration Strategy**:
1. Replace `AgentBrain()` with `plnlm.createAgentBrain()`
2. Update agent initialization method calls
3. Ensure world configuration compatibility
4. Test agent-world interaction

#### World Simulation

**Before (Legacy)**:
```python
# Old world simulation
world = World()
world.configure(width=10, height=10)
world.reset()
```

**After (Current)**:
```python
# New world simulation
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
```

**Migration Strategy**:
1. Replace `World()` with `pynlm.createSimpleWorld()`
2. Add vision parameters for sensory processing
3. Update world configuration methods
4. Test world simulation with agent integration

### Configuration File Format Changes

#### Old Configuration Format
```ini
# Legacy configuration format
neuron_count = 1000
plasticity_learning_rate = 0.01
environment_width = 10
environment_height = 10
random_seed = 42
simulation_timestep = 0.001
```

#### New Configuration Format
```ini
# New configuration format
brain.neuron_count = 1000
brain.region_count = 2
brain.synapse_density = 0.1
plasticity.stdp.learning_rate = 0.01
plasticity.stdp.ltp_weight = 0.01
plasticity.stdp.ltd_weight = 0.012
environment.name = GridWorld
environment.width = 10
environment.height = 10
environment.visionWidth = 8
environment.visionHeight = 8
random_seed = 42
simulation_timestep = 0.001
```

**Migration Strategy**:
1. Convert flat configuration to nested format
2. Add missing configuration parameters with defaults
3. Validate configuration compatibility
4. Test migrated configuration thoroughly

### Command Line Interface Changes

#### Old CLI Commands
```bash
# Legacy CLI commands
nlm create-brain 1000
nlm run-sim my_config.cfg --steps 1000
nlm benchmark my_config.cfg
```

#### New CLI Commands
```bash
# New CLI commands
nlmc create-brain 1000
nlmc run-sim my_config.cfg --steps 1000
nlmc benchmark my_config.cfg
```

**Migration Strategy**:
1. Update command names (`nlm` -> `nlmc`)
2. Ensure command arguments are compatible
3. Update help documentation
4. Test all CLI commands

## 3. Migration Tools and Scripts

### Automated Migration Script

```python
import pynlm
import json
import os
import shutil
from pathlib import Path
from typing import Dict, Any

class NLMMigrationTool:
    """Automated migration tool for NLM configurations and code."""
    
    def __init__(self, source_dir: str, target_dir: str):
        self.source_dir = Path(source_dir)
        self.target_dir = Path(target_dir)
        self.backup_dir = target_dir / "backup"
        self.migration_log = []
        
        # Create backup directory
        self.backup_dir.mkdir(exist_ok=True)
    
    def migrate_configuration(self, source_config_path: str, backup: bool = True):
        """Migrate a single configuration file."""
        source_file = Path(source_config_path)
        
        if not source_file.exists():
            raise FileNotFoundError(f"Configuration file not found: {source_file}")
        
        # Log migration start
        self.migration_log.append({
            'timestamp': time.time(),
            'action': 'migrate_config',
            'source': str(source_file),
            'status': 'started'
        })
        
        try:
            # Backup original if requested
            if backup:
                backup_file = self.backup_dir / f"backup_{source_file.name}"
                shutil.copy2(source_file, backup_file)
                self.migration_log[-1]['backup_created'] = str(backup_file)
            
            # Parse configuration based on file extension
            if source_file.suffix == '.cfg':
                config = self.migrate_cfg_file(source_file)
            elif source_file.suffix == '.json':
                config = self.migrate_json_file(source_file)
            else:
                raise ValueError(f"Unsupported configuration file format: {source_file.suffix}")
            
            # Save migrated configuration
            target_file = self.target_dir / source_file.name
            config.save(target_file)
            
            # Log migration success
            self.migration_log[-1].update({
                'status': 'success',
                'target': str(target_file)
            })
            
            return True
            
        except Exception as e:
            # Log migration failure
            self.migration_log[-1].update({
                'status': 'failed',
                'error': str(e)
            })
            
            raise
    
    def migrate_cfg_file(self, cfg_file: Path) -> any:
        """Migrate .cfg file to new configuration format."""
        config = pynlm.createDefaultConfig()
        
        # Read and parse .cfg file
        with open(cfg_file, 'r') as f:
            content = f.read()
        
        # Parse key-value pairs
        lines = content.split('\n')
        for line in lines:
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                # Convert to appropriate type
                converted_value = self.convert_config_value(value)
                
                # Apply migration rules for legacy keys
                legacy_key = self.migrate_legacy_key(key)
                if legacy_key:
                    config.set(legacy_key, converted_value)
        
        return config
    
    def migrate_json_file(self, json_file: Path) -> any:
        """Migrate .json file to new configuration format."""
        with open(json_file, 'r') as f:
            legacy_config = json.load(f)
        
        # Create new configuration
        config = pynlm.createDefaultConfig()
        
        # Migrate legacy structure to new format
        if 'brain' not in legacy_config:
            # Migrate legacy keys to new format
            legacy_config = self.migrate_legacy_structure(legacy_config)
        
        # Apply configuration values
        self.apply_config_to_brain(config, legacy_config)
        
        return config
    
    def migrate_legacy_structure(self, legacy_config: Dict[str, Any]) -> Dict[str, Any]:
        """Convert legacy configuration structure to new format."""
        new_config = {}
        
        # Map legacy keys to new keys
        key_mappings = {
            'neuron_count': 'brain.neuron_count',
            'plasticity_learning_rate': 'plasticity.stdp.learning_rate',
            'environment_width': 'environment.width',
            'environment_height': 'environment.height',
            'random_seed': 'random_seed',
            'simulation_timestep': 'simulation_timestep'
        }
        
        for legacy_key, value in legacy_config.items():
            if legacy_key in key_mappings:
                new_key = key_mappings[legacy_key]
                new_config[new_key] = value
            else:
                new_config[legacy_key] = value
        
        return new_config
    
    def migrate_legacy_key(self, legacy_key: str) -> str:
        """Convert legacy configuration key to new format."""
        # Migration rules for legacy keys
        if legacy_key == 'neuron_count':
            return 'brain.neuron_count'
        elif legacy_key == 'plasticity_learning_rate':
            return 'plasticity.stdp.learning_rate'
        elif legacy_key == 'environment_width':
            return 'environment.width'
        elif legacy_key == 'environment_height':
            return 'environment.height'
        elif legacy_key == 'random_seed':
            return 'random_seed'
        elif legacy_key == 'simulation_timestep':
            return 'simulation_timestep'
        elif legacy_key == 'synapse_density':
            return 'brain.synapse_density'
        elif legacy_key == 'region_count':
            return 'brain.region_count'
        elif legacy_key == 'dopamine_baseline':
            return 'neuromodulation.dopamine_baseline'
        
        return None  # Keep original key if not in migration list
    
    def apply_config_to_brain(self, config, legacy_config: Dict[str, Any]):
        """Apply legacy configuration values to brain."""
        for key, value in legacy_config.items():
            config.set(key, value)
    
    def convert_config_value(self, value: str) -> Any:
        """Convert configuration value to appropriate type."""
        # Try to convert to boolean
        if value.lower() in ('true', 'false', 'yes', 'no'):
            return value.lower() in ('true', 'yes')
        
        # Try to convert to integer
        try:
            return int(value)
        except ValueError:
            pass
        
        # Try to convert to float
        try:
            return float(value)
        except ValueError:
            pass
        
        # Return as string
        return value
    
    def migrate_python_code(self, source_file: Path, target_file: Path):
        """Migrate Python code from legacy format."""
        with open(source_file, 'r') as f:
            content = f.read()
        
        # Apply migration patterns
        migrated_content = self.migrate_python_patterns(content)
        
        # Save migrated code
        with open(target_file, 'w') as f:
            f.write(migrated_content)
        
        return target_file
    
    def migrate_python_patterns(self, content: str) -> str:
        """Apply migration patterns to Python code."""
        # Pattern 1: Replace Brain() with pynlm.createBrain()
        content = content.replace('Brain(', 'pynlm.createBrain(')
        
        # Pattern 2: Replace AgentBrain() with pynlm.createAgentBrain()
        content = content.replace('AgentBrain(', 'pynlm.createAgentBrain(')
        
        # Pattern 3: Replace World() with pynlm.createSimpleWorld()
        content = content.replace('World(', 'pynlm.createSimpleWorld(')
        
        # Pattern 4: Update import statements
        if 'import nlm' in content:
            content = content.replace('import nlm', 'import pynlm')
        
        # Pattern 5: Update configuration access
        content = content.replace('.neuron_count', '.brain.neuron_count')
        content = content.replace('.plasticity_learning_rate', '.plasticity.stdp.learning_rate')
        content = content.replace('.environment_width', '.environment.width')
        
        return content
    
    def generate_migration_report(self):
        """Generate migration report."""
        report = f"# NLM Migration Report\n\n"
        
        report += f"## Migration Summary\n"
        report += f"- Total migrations attempted: {len(self.migration_log)}\n"
        report += f"- Successful migrations: {len([m for m in self.migration_log if m['status'] == 'success'])}\n"
        report += f"- Failed migrations: {len([m for m in self.migration_log if m['status'] == 'failed'])}\n"
        
        report += f"\n## Migration Log\n"
        report += f"| Timestamp | Action | Source | Status | Target |\n"
        report += f"|-----------|--------|--------|--------|--------|\n"
        
        for migration in self.migration_log:
            timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(migration['timestamp']))
            action = migration['action']
            source = migration['source']
            status = migration['status']
            target = migration.get('target', 'N/A')
            
            report += f"| {timestamp} | {action} | {source} | {status} | {target} |\n"
        
        report += f"\n## Migration Recommendations\n"
        
        # Generate recommendations based on migration results
        failed_migrations = [m for m in self.migration_log if m['status'] == 'failed']
        
        if failed_migrations:
            report += f"### Issues Requiring Attention\n"
            for migration in failed_migrations:
                report += f"1. {migration['source']}: {migration.get('error', 'Unknown error')}\n"
        
        report += f"\n### Post-Migration Verification Steps\n"
        report += f"1. Test migrated configurations with small simulations\n"
        report += f"2. Verify compatibility with existing plugins\n"
        report += f"3. Check performance of migrated configurations\n"
        report += f"4. Update documentation and examples\n"
        report += f"5. Set up automated tests for migrated functionality\n"
        
        return report
```

### Configuration Migration Script

```python
#!/usr/bin/env python3
"""
Configuration migration script for NLM.
Converts legacy configuration files to new format.
"""

import argparse
import json
import os
import sys
from pathlib import Path

import pynlm

def main():
    parser = argparse.ArgumentParser(description='Migrate NLM configuration files')
    parser.add_argument('source', help='Source configuration file or directory')
    parser.add_argument('target', help='Target directory for migrated files')
    parser.add_argument('--backup', action='store_true', help='Create backup of original files')
    parser.add_argument('--format', choices=['cfg', 'json'], default='cfg', help='Target configuration format')
    
    args = parser.parse_args()
    
    # Create target directory if it doesn't exist
    Path(args.target).mkdir(parents=True, exist_ok=True)
    
    # Process source
    source_path = Path(args.source)
    
    if source_path.is_file():
        # Migrate single file
        migrate_file(source_path, Path(args.target), args.backup, args.format)
    elif source_path.is_dir():
        # Migrate all configuration files in directory
        migrate_directory(source_path, Path(args.target), args.backup, args.format)
    else:
        print(f"Error: Source path does not exist: {args.source}")
        sys.exit(1)
    
    print("Migration completed successfully!")

def migrate_file(source_file: Path, target_dir: Path, backup: bool, target_format: str):
    """Migrate a single configuration file."""
    print(f"Migrating: {source_file.name}")
    
    try:
        # Load configuration based on file extension
        if source_file.suffix == '.cfg':
            config = load_cfg_config(source_file)
        elif source_file.suffix == '.json':
            config = load_json_config(source_file)
        else:
            print(f"  Warning: Unsupported file format {source_file.suffix}, skipping")
            return
        
        # Create target file path
        target_file = target_dir / source_file.stem
        if target_format == 'cfg':
            target_file = target_file.with_suffix('.cfg')
        else:
            target_file = target_file.with_suffix('.json')
        
        # Save configuration
        if target_format == 'cfg':
            config.save(str(target_file))
            print(f"  Saved: {target_file.name}")
        else:
            # Convert to dictionary and save as JSON
            config_dict = config.to_dict() if hasattr(config, 'to_dict') else {}
            with open(target_file, 'w') as f:
                json.dump(config_dict, f, indent=2)
            print(f"  Saved: {target_file.name}")
        
        # Backup if requested
        if backup:
            backup_file = target_dir / f"backup_{source_file.name}"
            if source_file != backup_file:
                shutil.copy2(source_file, backup_file)
                print(f"  Backup created: {backup_file.name}")
        
    except Exception as e:
        print(f"  Error: {e}")
        return False
    
    return True

def migrate_directory(source_dir: Path, target_dir: Path, backup: bool, target_format: str):
    """Migrate all configuration files in a directory."""
    print(f"Migrating all configuration files from: {source_dir}")
    
    # Find configuration files
    config_files = []
    for ext in ['.cfg', '.json', '.yaml', '.yml']:
        config_files.extend(source_dir.glob(f'*{ext}'))
    
    if not config_files:
        print("  No configuration files found")
        return
    
    success_count = 0
    for config_file in config_files:
        if migrate_file(config_file, target_dir, backup, target_format):
            success_count += 1
    
    print(f"\nMigration complete: {success_count}/{len(config_files)} files migrated")

def load_cfg_config(cfg_file: Path) -> any:
    """Load configuration from .cfg file."""
    config = pynlm.createDefaultConfig()
    
    with open(cfg_file, 'r') as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                # Convert value
                converted_value = convert_value(value)
                
                # Apply migration
                migrated_key = migrate_legacy_key(key)
                if migrated_key:
                    config.set(migrated_key, converted_value)
    
    return config

def load_json_config(json_file: Path) -> any:
    """Load configuration from .json file."""
    with open(json_file, 'r') as f:
        config_dict = json.load(f)
    
    # Convert to new format
    config = pynlm.createDefaultConfig()
    
    # Apply migration for legacy keys
    for key, value in config_dict.items():
        migrated_key = migrate_legacy_key(key)
        if migrated_key:
            config.set(migrated_key, value)
        else:
            config.set(key, value)
    
    return config

def convert_value(value: str) -> any:
    """Convert string value to appropriate type."""
    # Try boolean
    if value.lower() in ('true', 'false', 'yes', 'no'):
        return value.lower() in ('true', 'yes')
    
    # Try integer
    try:
        return int(value)
    except ValueError:
        pass
    
    # Try float
    try:
        return float(value)
    except ValueError:
        pass
    
    # Return string
    return value

def migrate_legacy_key(key: str) -> str:
    """Convert legacy configuration key to new format."""
    key_mappings = {
        'neuron_count': 'brain.neuron_count',
        'plasticity_learning_rate': 'plasticity.stdp.learning_rate',
        'environment_width': 'environment.width',
        'environment_height': 'environment.height',
        'random_seed': 'random_seed',
        'simulation_timestep': 'simulation_timestep',
        'synapse_density': 'brain.synapse_density',
        'region_count': 'brain.region_count',
        'dopamine_baseline': 'neuromodulation.dopamine_baseline'
    }
    
    return key_mappings.get(key, key)

if __name__ == "__main__":
    main()
```

## 4. Migration Testing and Validation

### Post-Migration Testing

```python
import pynlm
import time
import json
from typing import Dict, List

class MigrationValidator:
    """Validate NLM migrations and ensure compatibility."""
    
    def __init__(self):
        self.validation_results = []
    
    def validate_migration(self, original_config, migrated_config, test_scenarios):
        """Validate migration between configurations."""
        print("Validating migration...")
        
        validation_result = {
            'original_config': original_config,
            'migrated_config': migrated_config,
            'test_scenarios': test_scenarios,
            'results': [],
            'passed': True
        }
        
        for scenario_name, scenario_config in test_scenarios.items():
            print(f"  Testing scenario: {scenario_name}")
            
            scenario_result = self.run_migration_test(
                original_config, migrated_config, scenario_config
            )
            
            validation_result['results'].append(scenario_result)
            
            if not scenario_result['passed']:
                validation_result['passed'] = False
        
        self.validation_results.append(validation_result)
        return validation_result
    
    def run_migration_test(self, original_config, migrated_config, scenario_config):
        """Run specific migration test scenario."""
        print(f"    Running test: {scenario_config.get('description', 'Untitled test')}")
        
        test_result = {
            'name': scenario_config.get('name', 'Unnamed test'),
            'description': scenario_config.get('description', ''),
            'steps': scenario_config.get('steps', 100),
            'iterations': scenario_config.get('iterations', 3),
            'comparisons': [],
            'passed': True
        }
        
        # Test original configuration
        original_results = self.test_configuration(original_config, scenario_config)
        
        # Test migrated configuration
        migrated_results = self.test_configuration(migrated_config, scenario_config)
        
        # Compare results
        comparison = self.compare_test_results(original_results, migrated_results)
        
        test_result['comparisons'].append(comparison)
        
        if not comparison['compatible']:
            test_result['passed'] = False
            print(f"    FAILED: {comparison['reason']}")
        else:
            print(f"    PASSED: Compatible with original configuration")
        
        return test_result
    
    def test_configuration(self, config, scenario_config):
        """Test a configuration with given scenario."""
        results = {
            'steps_completed': 0,
            'final_firing_rate': 0,
            'final_neuron_count': 0,
            'success': True,
            'error': None
        }
        
        try:
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            steps = scenario_config.get('steps', 100)
            for step in range(steps):
                brain.step(step)
            
            # Record results
            results['steps_completed'] = steps
            results['final_firing_rate'] = brain.getAverageFiringRate()
            results['final_neuron_count'] = brain.getTotalNeuronCount()
            
        except Exception as e:
            results['success'] = False
            results['error'] = str(e)
        
        return results
    
    def compare_test_results(self, original_results, migrated_results):
        """Compare results from original and migrated configurations."""
        comparison = {
            'steps_match': original_results['steps_completed'] == migrated_results['steps_completed'],
            'firing_rate_similar': self.is_firing_rate_similar(
                original_results['final_firing_rate'], migrated_results['final_firing_rate']
            ),
            'neuron_count_match': original_results['final_neuron_count'] == migrated_results['final_neuron_count'],
            'both_successful': original_results['success'] and migrated_results['success'],
            'compatible': True,
            'reason': ''
        }
        
        # Check compatibility
        if not comparison['steps_match']:
            comparison['compatible'] = False
            comparison['reason'] = f"Step count mismatch: {original_results['steps_completed']} vs {migrated_results['steps_completed']}"
        
        elif not comparison['both_successful']:
            comparison['compatible'] = False
            comparison['reason'] = f"One configuration failed: {original_results.get('error', '')} vs {migrated_results.get('error', '')}"
        
        elif not comparison['firing_rate_similar']:
            comparison['compatible'] = False
            comparison['reason'] = f"Firing rate too different: {original_results['final_firing_rate']:.2f} vs {migrated_results['final_firing_rate']:.2f}"
        
        elif not comparison['neuron_count_match']:
            comparison['compatible'] = False
            comparison['reason'] = f"Neuron count mismatch: {original_results['final_neuron_count']} vs {migrated_results['final_neuron_count']}"
        
        return comparison
    
    def is_firing_rate_similar(self, rate1: float, rate2: float, threshold: float = 0.2):
        """Check if two firing rates are similar within threshold."""
        if rate1 == 0 and rate2 == 0:
            return True
        
        if rate1 == 0 or rate2 == 0:
            return False
        
        ratio = abs(rate1 - rate2) / max(rate1, rate2)
        return ratio <= threshold
    
    def generate_validation_report(self):
        """Generate validation report."""
        report = f"# NLM Migration Validation Report\n\n"
        
        report += f"## Migration Validation Summary\n"
        report += f"- Total validations performed: {len(self.validation_results)}\n"
        report += f"- Successful migrations: {len([v for v in self.validation_results if v['passed']])}\n"
        report += f"- Failed migrations: {len([v for v in self.validation_results if not v['passed']])}\n"
        
        if self.validation_results:
            report += f"\n## Detailed Validation Results\n"
            
            for i, validation in enumerate(self.validation_results, 1):
                report += f"### Validation {i}: {validation['original_config']} -> {validation['migrated_config']}\n"
                
                all_passed = all(result['passed'] for result in validation['results'])
                report += f"**Overall Result**: {'PASSED' if all_passed else 'FAILED'}\n"
                
                for j, result in enumerate(validation['results'], 1):
                    report += f"- **Test {j}** ({result['name']}): {'PASSED' if result['passed'] else 'FAILED'}\n"
                    
                    if result['comparisons']:
                        for k, comparison in enumerate(result['comparisons'], 1):
                            report += f"  - Comparison {k}: {'PASS' if comparison['compatible'] else 'FAIL'} - {comparison['reason'] if not comparison['compatible'] else 'Compatible'}\n"
                
                report += f"\n"
        
        # Generate recommendations
        report += self.generate_migration_recommendations()
        
        return report
    
    def generate_migration_recommendations(self):
        """Generate recommendations based on validation results."""
        report = f"## Migration Recommendations\n"
        
        if not self.validation_results:
            report += f"No validation results available. Please run validation tests before deployment.\n"
            return report
        
        # Check for common issues
        issues = []
        for validation in self.validation_results:
            if not validation['passed']:
                for result in validation['results']:
                    for comparison in result['comparisons']:
                        if not comparison['compatible']:
                            issues.append({
                                'issue': comparison['reason'],
                                'severity': 'high',
                                'validation': f"{validation['original_config']} -> {validation['migrated_config']}"
                            })
        
        if issues:
            report += f"### Issues Requiring Attention\n"
            for issue in issues:
                report += f"1. **{issue['severity'].upper()}**: {issue['issue']}\n"
                report += f"   Context: Migration {issue['validation']}\n"
            
            report += f"\n### Recommended Actions\n"
            report += f"1. Review failing tests and identify root causes\n"
            report += f"2. Apply necessary configuration fixes\n"
            report += f"3. Update migration scripts to handle edge cases\n"
            report += f"4. Re-run validation tests after fixes\n"
        else:
            report += f"### All Migrations Passed\n"
            report += f"Migration validation successful. Configurations are compatible.\n"
            report += f"\n### Next Steps\n"
            report += f"1. Deploy migrated configurations\n"
            report += f"2. Update documentation and examples\n"
            report += f"3. Set up automated migration tests\n"
            report += f"4. Monitor performance after deployment\n"
        
        return report
```

### Migration Test Suite

```python
#!/usr/bin/env python3
"""
Migration test suite for NLM.
Comprehensive tests to validate configuration migrations.
"""

import pynlm
import json
import time
import os
import sys
from pathlib import Path

# Import validation functions
from migration_validator import MigrationValidator

class MigrationTestSuite:
    """Comprehensive test suite for NLM migrations."""
    
    def __init__(self):
        self.validator = MigrationValidator()
        self.test_results = []
    
    def run_all_tests(self):
        """Run all migration tests."""
        print("Running NLM migration test suite...")
        
        # Define test scenarios
        test_scenarios = self.get_test_scenarios()
        
        # Test each scenario
        for scenario_name, scenario_config in test_scenarios.items():
            print(f"\nTesting scenario: {scenario_name}")
            print(f"Description: {scenario_config.get('description', '')}")
            
            # Get configuration pairs to test
            config_pairs = self.get_config_pairs(scenario_config)
            
            for original_name, migrated_name in config_pairs:
                # Load configurations
                original_config = self.load_config(original_name)
                migrated_config = self.load_config(migrated_name)
                
                # Run validation
                validation_result = self.validator.validate_migration(
                    original_config, migrated_config, scenario_config
                )
                
                self.test_results.append({
                    'scenario': scenario_name,
                    'original_config': original_name,
                    'migrated_config': migrated_name,
                    'result': validation_result
                })
        
        # Generate report
        self.generate_test_report()
        
        return self.test_results
    
    def get_test_scenarios(self) -> dict:
        """Get test scenarios for migration validation."""
        return {
            'basic_functionality': {
                'name': 'Basic Functionality Test',
                'description': 'Test basic brain initialization and simulation',
                'steps': 100,
                'iterations': 3,
                'config_pairs': [
                    ('legacy_basic.cfg', 'modern_basic.cfg'),
                    ('legacy_development.cfg', 'modern_development.cfg')
                ]
            },
            'plasticity_behavior': {
                'name': 'Plasticity Behavior Test',
                'description': 'Test plasticity mechanisms are preserved',
                'steps': 200,
                'iterations': 3,
                'config_pairs': [
                    ('legacy_plasticity.cfg', 'modern_plasticity.cfg')
                ]
            },
            'performance_stability': {
                'name': 'Performance Stability Test',
                'description': 'Test performance consistency and stability',
                'steps': 150,
                'iterations': 5,
                'config_pairs': [
                    ('legacy_performance.cfg', 'modern_performance.cfg')
                ]
            },
            'memory_efficiency': {
                'name': 'Memory Efficiency Test',
                'description': 'Test memory usage and efficiency',
                'steps': 50,
                'iterations': 4,
                'config_pairs': [
                    ('legacy_memory.cfg', 'modern_memory.cfg')
                ]
            }
        }
    
    def get_config_pairs(self, scenario_config: dict) -> list:
        """Get configuration pairs to test for a scenario."""
        return scenario_config.get('config_pairs', [])
    
    def load_config(self, config_name: str) -> any:
        """Load configuration by name."""
        # Try different locations
        possible_paths = [
            Path(f"configs/{config_name}"),
            Path(f"legacy_configs/{config_name}"),
            Path(f"test_configs/{config_name}"),
            Path(config_name)
        ]
        
        for path in possible_paths:
            if path.exists():
                # Load based on file extension
                if path.suffix == '.cfg':
                    return load_cfg_config(path)
                elif path.suffix == '.json':
                    return load_json_config(path)
                else:
                    # Try to determine file type
                    content = path.read_text()
                    if 'neuron_count' in content or 'brain.neuron_count' in content:
                        # Assume .cfg format
                        return load_cfg_config(path)
                    else:
                        # Try JSON
                        try:
                            return load_json_config(path)
                        except:
                            continue
        
        # If config not found, create a default one for testing
        print(f"Warning: Configuration {config_name} not found, using default")
        return pynlm.createDefaultConfig()
    
    def generate_test_report(self):
        """Generate comprehensive test report."""
        report = "# NLM Migration Test Suite Report\n\n"
        
        report += f"## Test Suite Summary\n"
        report += f"- Total test scenarios: {len(self.get_test_scenarios())}\n"
        report += f"- Total validation runs: {len(self.test_results)}\n"
        
        passed_tests = sum(1 for test in self.test_results if test['result']['passed'])
        failed_tests = len(self.test_results) - passed_tests
        
        report += f"- Passed tests: {passed_tests}\n"
        report += f"- Failed tests: {failed_tests}\n"
        
        report += f"\n## Detailed Test Results\n"
        
        for i, test in enumerate(self.test_results, 1):
            scenario = test['scenario']
            original = test['original_config']
            migrated = test['migrated_config']
            result = test['result']
            
            report += f"### Test {i}: {scenario}\n"
            report += f"- **Configuration Pair**: {original} -> {migrated}\n"
            report += f"- **Result**: {'PASSED' if result['passed'] else 'FAILED'}\n"
            
            for j, scenario_result in enumerate(result['results'], 1):
                report += f"  - **Subtest {j}** ({scenario_result['name']}): {'PASSED' if scenario_result['passed'] else 'FAILED'}\n"
            
            report += f"\n"
        
        # Generate recommendations
        report += self.generate_test_recommendations()
        
        return report
    
    def generate_test_recommendations(self):
        """Generate recommendations based on test results."""
        report = f"## Test Suite Recommendations\n"
        
        failed_tests = [test for test in self.test_results if not test['result']['passed']]
        
        if not failed_tests:
            report += f"### All Tests Passed ✅\n"
            report += f"All migration tests passed successfully. The migration is ready for deployment.\n"
            
            report += f"\n### Next Steps\n"
            report += f"1. Deploy migrated configurations\n"
            report += f"2. Update documentation and examples\n"
            report += f"3. Set up automated migration tests\n"
            report += f"4. Monitor performance after deployment\n"
        else:
            report += f"### Migration Issues Found ❌\n"
            report += f"{len(failed_tests)} test scenarios failed. Review and fix issues before deployment.\n"
            
            report += f"\n### Recommended Actions\n"
            report += f"1. Review failing test results in detail\n"
            report += f"2. Identify root causes of failures\n"
            report += f"3. Apply necessary fixes to configurations or migration scripts\n"
            report += f"4. Re-run failed tests after fixes\n"
            report += f"5. Update migration tools and documentation\n"
        
        return report

# Main test execution
if __name__ == "__main__":
    print("NLM Migration Test Suite")
    print("=" * 50)
    
    test_suite = MigrationTestSuite()
    test_suite.run_all_tests()
    
    print("\nTest suite completed!")
```

## 5. Post-Migration Support

### Troubleshooting Common Issues

1. **Configuration Validation Errors**
   - **Issue**: Configuration validation fails after migration
   - **Solution**: Check configuration compatibility, use validation tools
   - **Command**: `./nlmc validate-config <config_file>`

2. **Performance Degradation**
   - **Issue**: Migrated configurations perform worse than original
   - **Solution**: Optimize migrated configuration, benchmark against original
   - **Command**: `./nlmc benchmark <config_file> --iterations 10`

3. **Missing Features**
   - **Issue**: Features from original configuration are missing
   - **Solution**: Update migrated configuration, add missing parameters
   - **Command**: Check configuration template differences

4. **Plugin Compatibility**
   - **Issue**: Plugins from original system no longer work
   - **Solution**: Update plugin interfaces, test plugin compatibility
   - **Command**: Test each plugin with migrated configuration

### Support Resources

1. **Documentation**
   - Migration guide and configuration reference
   - API documentation with examples
   - Command-line tool documentation

2. **Tools**
   - Automated migration scripts
   - Configuration validation tools
   - Performance benchmarking tools

3. **Community**
   - NLM user forums
   - GitHub repository for issues
   - Developer documentation and guides

## 6. Migration Checklist

### Pre-Migration Checklist

- [ ] Review breaking changes in migration guide
- [ ] Backup all original configurations
- [ ] Test migration scripts on non-production data
- [ ] Verify compatibility with existing plugins
- [ ] Set up validation testing environment
- [ ] Document migration procedures

### Migration Checklist

- [ ] Convert legacy configuration files
- [ ] Update Python code to use new API
- [ ] Test migrated configurations with basic scenarios
- [ ] Validate performance and stability
- [ ] Run comprehensive migration tests
- [ ] Update documentation and examples
- [ ] Set up automated migration tests
- [ ] Deploy to staging environment
- [ ] Perform user acceptance testing
- [ ] Deploy to production

### Post-Migration Checklist

- [ ] Monitor performance after deployment
- [ ] Validate user workflows and processes
- [ ] Collect feedback and address issues
- [ ] Update backup and recovery procedures
- [ ] Archive legacy configurations
- [ ] Train users on new procedures
- [ ] Document lessons learned

## 7. Conclusion

Migration from older NLM versions requires careful planning and execution. By following this migration guide, using the provided tools, and implementing the recommended strategies, users can successfully migrate to the current NLM version while minimizing disruption and ensuring compatibility.

Key success factors:

1. **Comprehensive Planning**: Understand all breaking changes and migration requirements
2. **Automated Tools**: Use migration scripts and validation tools
3. **Testing**: Implement thorough testing at all stages
4. **Documentation**: Maintain clear documentation of changes and procedures
5. **Support**: Provide resources and support for users during migration

By following this guide, NLM users can achieve a smooth migration path that preserves functionality while taking advantage of new features and improvements in the current version.
