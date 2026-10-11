// Update the original TODOs in Config.cpp to reflect new modular approach
// #include "Config.hpp"
// #include "ConfigValidation.hpp"
// #include "JSONYAMLParser.hpp"
// #include <fstream>
// #include <sstream>
// #include <algorithm>
// #include <filesystem>
// 
// namespace nlm {
// 
// struct Config::Impl {
//     std::vector<ConfigEntry> entries;
//     ConfigSchema schema;  // Added schema validation
// };
// 
// Config::Config() : pImpl(std::make_unique<Impl>()) {
//     // Initialize with default schema
//     initializeDefaultSchema();
// }
// 
// Config::~Config() = default;
// 
// Config::Config(Config&&) noexcept = default;
// 
// Config& Config::operator=(Config&&) noexcept = default;
// 
// bool Config::loadFromFile(const std::string& filepath) {
//     try {
//         // Enhanced config loading with proper JSON/YAML parser
//         // NEW MODULAR APPROACH: Use enhanced parser with validation
//         std::ifstream file(filepath);
//         if (!file.is_open()) {
//             return false;
//         }
//         
//         std::string content((std::istreambuf_iterator<char>(file)), 
//                           std::istreambuf_iterator<char>());
//         
//         // Parse using enhanced parser
//         auto parsedData = EnhancedConfigParser::parseContent(content, filepath);
//         
//         // Validate against schema
//         std::string validationErrors = pImpl->schema.validateAll(parsedData);
//         if (!validationErrors.empty()) {
//             throw ConfigParseError("Configuration validation failed: " + validationErrors, filepath);
//         }
//         
//         // Apply parsed data
//         for (const auto& [key, value] : parsedData) {
//             set(key, value, ConfigSource::File);
//         }
//         
//         return true;
//     } catch (const ConfigParseError& e) {
//         // Handle parsing errors gracefully
//         std::cerr << e.what() << std::endl;
//         return false;
//     }
// }
// 
// bool Config::loadFromArgs(int argc, char** argv) {
//     // Enhanced command line argument parsing with validation
//     for (int i = 1; i < argc; ++i) {
//         std::string arg(argv[i]);
//         
//         // Handle --key=value format
//         if (arg.substr(0, 2) == "--") {
//             size_t pos = arg.find('=');
//             if (pos != std::string::npos) {
//                 std::string key = arg.substr(2, pos - 2);
//                 std::string value = arg.substr(pos + 1);
//                 
//                 // Validate key before setting
//                 const auto* field = pImpl->schema.getField(key);
//                 if (field && !field->required) {
//                     // Skip unknown optional fields
//                     continue;
//                 }
//                 
//                 set(key, value, ConfigSource::CommandLine);
//             }
//         }
//         // Handle -key value format
//         else if (arg[0] == '-' && i + 1 < argc) {
//             std::string key = arg.substr(1);
//             std::string value = argv[++i];
//             
//             // Validate key before setting
//             const auto* field = pImpl->schema.getField(key);
//             if (field && !field->required) {
//                 continue;
//             }
//             
//             set(key, value, ConfigSource::CommandLine);
//         }
//     }
//     return true;
// }
// 
// void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
//     // Validate against schema before setting
//     const auto* field = pImpl->schema.getField(key);
//     if (field && !pImpl->schema.validate(key, value)) {
//         throw ConfigValidationError(key, field->type, "unknown");
//     }
//     
//     auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
//         [&key](const ConfigEntry& e) { return e.key == key; });
//     
//     if (it != pImpl->entries.end()) {
//         it->value = value;
//         it->source = source;
//     } else {
//         ConfigEntry entry(key, value, source, 
//                          field ? field->description : "");
//         pImpl->entries.push_back(entry);
//     }
// }
// 
// void Config::initializeDefaultSchema() {
//     // Initialize with basic schema for common configuration options
//     pImpl->schema.addField(ConfigSchemaBuilder::intField(
//         "max_neurons", "Maximum number of neurons in the network", 10000));
//     
//     pImpl->schema.addField(ConfigSchemaBuilder::doubleField(
//         "learning_rate", "Learning rate for plasticity rules", 0.01, true, 0.0, 1.0));
//     
//     pImpl->schema.addField(ConfigSchemaBuilder::stringField(
//         "plasticity_rule", "Plasticity rule type (STDP, Hebbian)", "STDP",
//         true, "^[a-zA-Z]+$", {"STDP", "Hebbian", "BCM"}));
//     
//     pImpl->schema.addField(ConfigSchemaBuilder::boolField(
//         "enable_debug_mode", "Enable debug logging and validation", false));
//     
//     pImpl->schema.addField(ConfigSchemaBuilder::arrayField(
//         "initial_weights", "Initial weight values for synapses", 
//         std::vector<int>{0, 0, 1}, 1, 1000));
// }
// 
// } // namespace nlm
