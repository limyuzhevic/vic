import pynlm

print("Testing config creation with simple approach...")

# Create a simple config without using the factory functions
try:
    # Import the Config class directly
    from pynlm.config import Config
    
    # Create config with minimal parameters
    config = Config()
    print("Config created successfully")
    
    # Check if config has basic functionality
    print(f"Config keys: {config.get_keys()}")
    
    # Set a simple parameter
    config.set("test_param", 42)
    print(f"Set test_param: {config.get('test_param')}")
    
    # Test validation
    validation_result = config.validate()
    print(f"Validation successful: {validation_result.is_valid}")
    
    print("All basic config tests passed!")
    
except Exception as e:
    print(f"Error: {e}")
    import traceback
    traceback.print_exc()
