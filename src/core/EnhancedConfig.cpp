std::unordered_map<std::string, std::string> config;

// Parse JSON file content
if (loadFromFile(filepath)) {
    NLM_LOG_INFO("Loaded configuration from: " + filepath);
} else {
    NLM_LOG_WARNING("Configuration file not found: " + filepath);
    // Try to load from default location
    if (loadFromFile("configs/default.json")) {
        NLM_LOG_INFO("Loaded default configuration from: configs/default.json");
    } else {
        NLM_LOG_INFO("Using empty configuration.");
    }
}

// Override with command line args
loadFromArgs(argc, argv);