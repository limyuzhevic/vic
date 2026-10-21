import sys

print("NLM Improved Functionality Demo")
print("=" * 50)

# Example 1: Configuration Management
print("\n1. Configuration Management")
print("-" * 30)

config = nlm.get_default_config()
print(f"Default config neuron_count: {config.get('neuron_count')}")

# Modify configuration
config.set("neuron_count", 2000)
config.set("simulation_timestep", 0.0005)
config.set("log_level", "DEBUG")

print(f"Modified neuron_count: {config.get('neuron_count')}")

# Save configuration
config.save_to_file("demo_config.json")
print("Saved configuration to demo_config.json")

# Load configuration
new_config = nlm.Config()
new_config.load_from_file("demo_config.json")
print(f"Loaded neuron_count: {new_config.get('neuron_count')}")

# Example 2: Performance Monitoring
print("\n2. Performance Monitoring")
print("-" * 30)

stats = nlm.get_performance_stats()
print(f"Performance stats: {stats}")

# Example 3: Configuration Validation
print("\n3. Configuration Validation")
print("-" * 30)

# Create a configuration
config = nlm.Config()
config.set("random_seed", 42)
config.set("neuron_count", 100)
config.set("log_level", "INFO")

# Show configuration summary
print("Configuration summary:")
print(config.summary())

# Example 4: Error Handling
print("\n4. Error Handling")
print("-" * 30)

try:
    # Create invalid configuration (string where int expected)
    config = nlm.Config()
    config.set("neuron_count", "not_a_number")  # This should fail or convert
    print(f"Set neuron_count to: {config.get('neuron_count')}")
except Exception as e:
    print(f"Error (expected): {type(e).__name__}: {e}")

# Example 5: Configuration Export/Import
print("\n5. Configuration Export/Import")
print("-" * 30)

# Create test configuration
test_config = nlm.get_research_config()

# Export to JSON
test_config.save_to_file("test_config.json")
print("Exported to test_config.json")

# Read and show content
with open("test_config.json", "r") as f:
    content = json.load(f)
    print(f"Config keys: {list(content.keys())}")
    print(f"neuron_count: {content.get('neuron_count')}")

# Example 6: Utility Functions
print("\n6. Utility Functions")
print("-" * 30)

print(f"formatNumber(1234.567): {nlm.Utils.formatNumber(1234.567)}")
print(f"formatTime(3661.5): {nlm.Utils.formatTime(3661.5)}")
print(f"generateTimestamp(): {nlm.Utils.generateTimestamp()}")
print(f"safeFilename('test file.txt'): {nlm.Utils.sanitizeFilename('test file.txt')}")

# Example 7: Feature Detection
print("\n7. Feature Detection")
print("-" * 30)

features = nlm.check_features()
print("Available features:")
for feature, available in features.items():
    status = "✓" if available else "✗"
    print(f"  {status} {feature}")

# Example 8: Environment Detection
print("\n8. Environment Detection")
print("-" * 30)

import platform
print(f"Platform: {platform.system()}")
print(f"Python version: {sys.version_info.major}.{sys.version_info.minor}")
print(f"Is Python 3.8+: {sys.version_info >= (3, 8)}")

# Example 9: Advanced Configuration Types
print("\n9. Advanced Configuration Types")
print("-" * 30)

research_config = nlm.get_research_config()
print(f"Research config neuron_count: {research_config.get('neuron_count')}")

realtime_config = nlm.get_realtime_config()
print(f"Realtime config neuron_count: {realtime_config.get('neuron_count')}")

development_config = nlm.get_development_config()
print(f"Development config neuron_count: {development_config.get('neuron_count')}")

# Example 10: Safe Directory Creation
print("\n10. Safe Directory Operations")
print("-" * 30)

# Create test directory
test_dir = "test_nlm_dir"
created = nlm.Utils.create_directory(test_dir)
print(f"Created directory: {created} (exists: {nlm.Utils.directoryExists(test_dir)})")

# Create timestamped filename
timestamped = nlm.Utils.timestamped_filename("nlm_results", "csv")
print(f"Timestamped filename: {timestamped}")

# Example 11: Configuration Summary Analysis
print("\n11. Configuration Analysis")
print("-" * 30)

config = nlm.Config()
config.set("random_seed", 123)
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.001)
config.set("region_count", 2)
config.set("log_level", "INFO")

summary = config.summary()
lines = summary.split('\n')
print(f"Configuration has {len(lines)} lines in summary")
print("Contains 'Configuration': {' in summary:", 'Configuration' in summary)
print("Contains 'neuron_count': {' in summary:", 'neuron_count' in summary)

# Example 12: String Operations
print("\n12. String Utility Functions")
print("-" * 30)

sample_text = "Hello World! This is a test."
print(f"Original: {sample_text}")
print(f"Split by space: {nlm.Utils.splitString(sample_text, ' ')}")
print(f"Contains 'test': {nlm.Utils.stringContains(sample_text, 'test')}")

print("\n" + "=" * 50)
print("NLM Demo Complete!")
print("All improved functionality demonstrated successfully.")
print("=" * 50)
