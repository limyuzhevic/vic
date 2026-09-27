# Fix the config.py file
import re

# Read the config.py file
with open('pynlm/config.py', 'r') as f:
    content = f.read()

# Fix the _extract_from_properties function
old_function = '''def _extract_from_properties(props):
            for key, value in props.items():
                if "default" in value:
                    defaults[key] = value["default"]
                elif "properties" in value:
                    defaults.update(_extract_from_properties(value["properties"]))'''

new_function = '''def _extract_from_properties(props):
            for key, value in props.items():
                if "default" in value:
                    defaults[key] = value["default"]
                elif "properties" in value and value["properties"] is not None:
                    defaults.update(_extract_from_properties(value["properties"]))'''

# Replace the function
fixed_content = content.replace(old_function, new_function)

# Write the fixed content back
with open('pynlm/config.py', 'w') as f:
    f.write(fixed_content)

print("Fixed _extract_from_properties function")

# Test the fix
from pynlm.config import Config

config = Config()
print("Config created successfully")
print(f"Config keys: {config.get_keys()}")

config.set("test_param", 42)
print(f"Set test_param: {config.get('test_param')}")

validation_result = config.validate()
print(f"Validation successful: {validation_result.is_valid}")

print("All tests passed!")
