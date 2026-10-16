nlohmann_json.hpp
# Install JSON library
# #include <nlohmann/json.hpp>

# Then use nlohmann::json for all JSON operations in Config.cpp

# Key improvements for JSON support:
# 1. Parse full JSON objects with nested structures
# 2. Support JSON arrays and complex types
# 3. Better error handling with descriptive messages
# 4. Merge configuration layers (default < file < args)
# 5. Save configuration in pretty-printed JSON format

# Example JSON parsing logic:
# auto json = nlohmann::json::parse(fileStream);
# for (auto it = json.begin(); it != json.end(); ++it) {
#     std::string key = it.key();
#     ConfigValue value = json_to_config_value(*it);
#     set(key, value, ConfigSource::File);
# }

# Benefits:
# - Standard format that's widely supported
# - Human-readable configuration files
# - Easy to edit with text editors
# - Better version control
# - Supports nested configuration sections
# - Better data validation and type checking