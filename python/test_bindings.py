# Create a simple environment
env = pynlm.create_simple_env(100, 100)

# Create a random brain
brain = pynlm.create_random_brain()

# Test Pythonic properties on Config
config = pynlm.create_default_config()
print(f"Config keys: {config.keys}")
print(f"Config is empty: {config.is_empty}")
print(f"Config entry count: {config.entry_count}")

# Test high-level convenience method
brain_obj = pynlm.create_random_brain()
print(f"Brain region count: {brain_obj.get_region_count()}")
print(f"Brain region IDs: {brain_obj.get_region_ids()}")

# Test performance monitoring
perf_monitor = pynlm.create_performance_monitor()
perf_monitor.recordFrame(brain_obj)
summary = perf_monitor.getSummary()
print(f"Performance summary: {summary}")

# Test visualization helper
viz_helper = pynlm.create_visualization_helper()
brain_state = viz_helper.analyzeBrainState(brain_obj)
print(f"Brain state analysis: {brain_state}")

# Test context manager
with brain_obj:
    print("Context manager test successful")

print("All Pythonic features are working correctly!")
