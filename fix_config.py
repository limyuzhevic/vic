import re

# Read the Config.cpp file
with open('src/core/Config/Config.cpp', 'r') as f:
    content = f.read()

# Find all occurrences of configValueToString function
pattern = r'std::string Config::configValueToString\(const ConfigValue& value\) \{[^}]+\}'
matches = re.findall(pattern, content, re.DOTALL)

print(f"Found {len(matches)} occurrences of configValueToString function")

if len(matches) > 1:
    print("Removing duplicate function definitions...")
    
    # Find the first occurrence and keep it
    first_match = matches[0]
    
    # Create new content with only the first function
    new_content = content.replace(matches[0], first_match)
    for duplicate in matches[1:]:
        new_content = new_content.replace(duplicate, '')
    
    # Write the fixed content back to the file
    with open('src/core/Config/Config.cpp', 'w') as f:
        f.write(new_content)
    
    print("Config.cpp file has been fixed")
else:
    print("Only one occurrence found, no need to fix")
