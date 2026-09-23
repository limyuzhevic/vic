"""
NLM Configuration and Customization Examples

This example demonstrates advanced configuration techniques and customization
options for the enhanced NLM Python bindings.
"""

from pynlm import AdvancedBrain, Config, NLMTypes
import json
from pathlib import Path


def basic_configuration():
    """Basic configuration examples."""
    print("=== Basic Configuration Example ===\n")
    
    # Method 1: Direct configuration
    config = Config()
    config.configure(
        max_steps=1000,
        simulation_speed=1.0,
        reward_threshold=0.0
    )
    
    print("1. Direct Configuration:")
    print(f"   Parameters: {config.get_keys()}")
    print(f"   Summary:\n{config.summary()}")
    
    # Method 2: Using brain constructor
    brain1 = AdvancedBrain(verbose=False)
    print("\n2. Brain with default config:")
    print(f"   Default parameters: {brain1._config.get_keys()}")
    
    # Method 3: Custom configuration
    custom_config = Config().configure(
        max_steps=500,
        simulation_speed=2.0,
        enable_visualization=True,
        enable_monitoring=False,
        enable_reward_modulation=True,
        enable_development=False
    )
    
    brain2 = AdvancedBrain(config=custom_config, verbose=False)
    print("\n3. Custom Configuration:")
    print(f"   Custom parameters: {brain2._config.get_keys()}")
    print(f"   Can configure simulation: {brain2._config.has('max_steps')}")
    print(f"   Can get value: {brain2._config.get('simulation_speed')}")


def configuration_file_management():
    """Configuration file saving and loading."""
    print("\n=== Configuration File Management Example ===\n")
    
    # Create configuration
    config = Config()
    config.configure(
        max_steps=1000,
        simulation_speed=1.0,
        reward_threshold=0.5,
        enable_visualization=True,
        visualization_backend='matplotlib',
        enable_monitoring=True,
        monitoring_interval=0.1,
        enable_reward_modulation=True,
        enable_structural_plasticity=False,
        enable_development=True,
        enable_curiosity=False
    )
    
    print("1. Created Configuration:")
    print(config.summary())
    
    # Save to file
    config_file = Path("config.json")
    try:
        with open(config_file, 'w') as f:
            json.dump(dict(config._parameters), f, indent=2)
        print(f"\n2. Configuration saved to {config_file}")
    except Exception as e:
        print(f"\n2. Failed to save configuration: {e}")
    
    # Load from file
    loaded_config = Config()
    try:
        with open(config_file, 'r') as f:
            params = json.load(f)
        
        loaded_config.configure(**params)
        print(f"\n3. Configuration loaded from {config_file}")
        print(f"   Loaded parameters: {loaded_config.get_keys()}")
        print(f"   Max steps: {loaded_config.get('max_steps')}")
        print(f"   Simulation speed: {loaded_config.get('simulation_speed')}")
    except Exception as e:
        print(f"\n3. Failed to load configuration: {e}")
    
    # Clean up
    try:
        config_file.unlink()
        print(f"\n4. Configuration file cleaned up")
    except:
        pass


def advanced_configuration_validation():
    """Advanced configuration validation and error handling."""
    print("\n=== Advanced Configuration Validation Example ===\n")
    
    config = Config()
    
    # Add custom validation rules
    print("1. Adding custom validation rules...")
    
    def validate_simulation_speed(value):
        errors = []
        if isinstance(value, (int, float)):
            if value <= 0:
                errors.append("Simulation speed must be positive")
            elif value > 10:
                errors.append("Simulation speed too high (max 10)")
        return errors
    
    def validate_positive_int(value):
        errors = []
        if not isinstance(value, int):
            errors.append("Must be an integer")
        elif value <= 0:
            errors.append("Must be positive")
        elif value > 10000:
            errors.append("Too large (max 10000)")
        return errors
    
    def validate_percentage(value):
        errors = []
        if isinstance(value, (int, float)):
            if value < 0 or value > 1:
                errors.append("Must be between 0 and 1")
        return errors
    
    config.add_validation_rule('simulation_speed', validate_simulation_speed)
    config.add_validation_rule('max_steps', validate_positive_int)
    config.add_validation_rule('reward_threshold', validate_percentage)
    
    config.add_description('simulation_speed', 'Controls simulation time scaling')
    config.add_description('max_steps', 'Maximum number of simulation steps')
    config.add_description('reward_threshold', 'Minimum reward for success')
    
    print("   Added validation rules for: simulation_speed, max_steps, reward_threshold")
    
    # Test validation
    print("\n2. Testing validation...")
    
    test_cases = [
        ('valid_params', {'max_steps': 100, 'simulation_speed': 1.0, 'reward_threshold': 0.5}, True),
        ('negative_steps', {'max_steps': -100, 'simulation_speed': 1.0, 'reward_threshold': 0.5}, False),
        ('slow_speed', {'max_steps': 100, 'simulation_speed': -1.0, 'reward_threshold': 0.5}, False),
        ('high_speed', {'max_steps': 100, 'simulation_speed': 15.0, 'reward_threshold': 0.5}, False),
        ('invalid_threshold', {'max_steps': 100, 'simulation_speed': 1.0, 'reward_threshold': 1.5}, False),
        ('high_steps', {'max_steps': 20000, 'simulation_speed': 1.0, 'reward_threshold': 0.5}, False)
    ]
    
    for test_name, params, should_pass in test_cases:
        try:
            test_config = Config().configure(**params)
            if should_pass:
                print(f"   ✓ {test_name}: Correctly accepted")
            else:
                print(f"   ✗ {test_name}: Should have been rejected")
        except Exception as e:
            if not should_pass:
                print(f"   ✓ {test_name}: Correctly rejected - {type(e).__name__}")
            else:
                print(f"   ✗ {test_name}: Should have been accepted - {e}")
    
    # Show validation errors
    print("\n3. Detailed validation errors:")
    invalid_params = {'max_steps': -50, 'simulation_speed': -2.0, 'reward_threshold': 1.2}
    errors = config.get_validation_errors(invalid_params)
    for error in errors:
        print(f"   - {error}")


def configuration_merging():
    """Advanced configuration merging techniques."""
    print("\n=== Configuration Merging Example ===\n")
    
    # Base configuration
    base_config = Config().configure(
        max_steps=1000,
        simulation_speed=1.0,
        enable_monitoring=False,
        enable_visualization=False
    )
    
    print("1. Base Configuration:")
    print(f"   {base_config.summary()}")
    
    # Environment-specific configurations
    development_config = Config().configure(
        max_steps=500,
        simulation_speed=0.5,
        enable_monitoring=True,
        monitoring_interval=0.05,
        enable_development=True,
        enable_curiosity=True
    )
    
    production_config = Config().configure(
        max_steps=2000,
        simulation_speed=1.5,
        enable_monitoring=True,
        monitoring_interval=0.1,
        enable_visualization=True,
        visualization_backend='matplotlib',
        enable_reward_modulation=True
    )
    
    test_config = Config().configure(
        max_steps=100,
        simulation_speed=2.0
        # Other defaults will apply
    )
    
    print("\n2. Environment Configurations:")
    
    # Merge configurations (in practice, you'd implement this logic)
    def merge_config(base: Config, override: Config) -> Config:
        """Merge two configurations, with override taking precedence."""
        merged = Config()
        
        # Start with base parameters
        merged.configure(**base._parameters)
        
        # Override with override parameters
        merged.configure(**override._parameters)
        
        # Copy validation rules from base
        merged._validation_rules = base._validation_rules.copy()
        
        return merged
    
    # Merge test configuration with production
    merged = merge_config(production_config, test_config)
    print(f"\n3. Merged Configuration (Production + Test):")
    print(f"   Max steps: {merged.get('max_steps')} (from test)")
    print(f"   Simulation speed: {merged.get('simulation_speed')} (from test)")
    print(f"   Enable monitoring: {merged.get('enable_monitoring')} (from production)")
    print(f"   Enable visualization: {merged.get('enable_visualization')} (from production)")
    
    # Create brain with merged configuration
    brain = AdvancedBrain(config=merged, verbose=False)
    print(f"\n4. Brain with merged config:")
    print(f"   Brain configured with {len(merged._parameters)} parameters")


def configuration_profiles():
    """Configuration profiles for different use cases."""
    print("\n=== Configuration Profiles Example ===\n")
    
    # Define configuration profiles
    profiles = {
        'quick_test': {
            'max_steps': 50,
            'simulation_speed': 2.0,
            'enable_monitoring': False,
            'enable_visualization': False,
            'description': 'Fast testing with minimal overhead'
        },
        'research': {
            'max_steps': 1000,
            'simulation_speed': 1.0,
            'enable_monitoring': True,
            'monitoring_interval': 0.1,
            'enable_development': True,
            'enable_curiosity': True,
            'enable_structural_plasticity': True,
            'enable_reward_modulation': True,
            'description': 'Comprehensive research simulation'
        },
        'production': {
            'max_steps': 2000,
            'simulation_speed': 1.2,
            'enable_monitoring': True,
            'monitoring_interval': 0.05,
            'enable_visualization': True,
            'visualization_backend': 'matplotlib',
            'enable_reward_modulation': True,
            'description': 'Production-ready simulation'
        },
        'real_time': {
            'max_steps': 100,
            'simulation_speed': 0.5,
            'enable_monitoring': True,
            'monitoring_interval': 0.02,
            'enable_visualization': True,
            'visualization_backend': 'plotly',
            'enable_reward_modulation': True,
            'description': 'Real-time interactive simulation'
        }
    }
    
    print("1. Available Configuration Profiles:")
    for profile_name, profile_config in profiles.items():
        print(f"\n   {profile_name.upper()}:")
        print(f"     Description: {profile_config['description']}")
        print(f"     Key settings: max_steps={profile_config['max_steps']}, "
              f"speed={profile_config['simulation_speed']}, "
              f"monitoring={profile_config['enable_monitoring']}, "
              f"visualization={profile_config['enable_visualization']}")
    
    # Create configuration factory function
    def get_config(profile_name: str) -> Config:
        """Get configuration for a specific profile."""
        if profile_name not in profiles:
            raise ValueError(f"Unknown profile: {profile_name}")
        
        return Config().configure(**profiles[profile_name])
    
    print("\n2. Using Configuration Factory:")
    for profile in ['quick_test', 'research', 'production']:
        config = get_config(profile)
        print(f"\n   {profile} profile:")
        print(f"     Configured with {len(config._parameters)} parameters")
        
        # Show a few key parameters
        for key in ['max_steps', 'simulation_speed', 'enable_monitoring']:
            if config.has(key):
                print(f"       {key}: {config.get(key)}")


def main():
    """Run all configuration examples."""
    print("NLM Configuration and Customization Examples")
    print("=" * 60)
    print()
    
    basic_configuration()
    configuration_file_management()
    advanced_configuration_validation()
    configuration_merging()
    configuration_profiles()
    
    print("\n" + "=" * 60)
    print("All configuration examples completed successfully!")


if __name__ == "__main__":
    main()