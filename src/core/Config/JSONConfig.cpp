void JSONConfig::convertFromJSON(const json& j, Config& config) {
    config.clear();
    
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        
        if (key == "metadata") {
            // Handle metadata
            for (auto metaIt = it.value().begin(); metaIt != it.value().end(); ++metaIt) {
                std::string entryKey = metaIt.key();
                const auto& meta = metaIt.value();
                
                if (meta.contains("source") && meta.contains("value")) {
                    ConfigSource source = jsonSourceToConfigSource(meta["source"].get<int>());
                    std::string description = meta["description"].get<std::string>();
                    std::string valueStr = meta["value"].get<std::string>();
                    
                    config.set(entryKey, valueStr, source);
                    config.setEntryDescription(entryKey, description);
                }
            }
        } else {
            // Handle direct key-value pairs
            std::string valueStr = it.value().get<std::string>();
            config.set(key, valueStr, ConfigSource::File);
        }
    }
}

void JSONConfig::convertToJSON(const Config& config, json& j) {
    j = json::object();
    
    for (const auto& entry : config.getKeys()) {
        auto val = config.get<std::string>(entry);
        if (val.has_value()) {
            j[entry] = val.value();
        }
        
        // Add metadata about source and description
        auto entryInfo = json::object();
        entryInfo["source"] = configSourceToJSONString(config.getEntrySource(entry));
        entryInfo["description"] = config.getEntryDescription(entry);
        j["metadata"][entry] = entryInfo;
    }
}

void JSONConfig::convertFromJSON(const json& j, Config& config) {
    config.clear();
    
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        
        if (key == "metadata") {
            // Handle metadata
            for (auto metaIt = it.value().begin(); metaIt != it.value().end(); ++metaIt) {
                std::string entryKey = metaIt.key();
                const auto& meta = metaIt.value();
                
                if (meta.contains("source") && meta.contains("value")) {
                    ConfigSource source = jsonSourceToConfigSource(meta["source"].get<int>());
                    std::string description = meta["description"].get<std::string>();
                    std::string valueStr = meta["value"].get<std::string>();
                    
                    config.set(entryKey, valueStr, source);
                    config.setEntryDescription(entryKey, description);
                }
            }
        } else {
            // Handle direct key-value pairs
            std::string valueStr = it.value().get<std::string>();
            config.set(key, valueStr, ConfigSource::File);
        }
    }
}

void JSONConfig::convertToJSON(const Config& config, json& j) {
    j = json::object();
    
    for (const auto& entry : config.getKeys()) {
        auto val = config.get<std::string>(entry);
        if (val.has_value()) {
            j[entry] = val.value();
        }
        
        // Add metadata about source and description
        auto entryInfo = json::object();
        entryInfo["source"] = configSourceToJSONString(config.getEntrySource(entry));
        entryInfo["description"] = config.getEntryDescription(entry);
        j["metadata"][entry] = entryInfo;
    }
}

void JSONConfig::convertFromJSON(const json& j, Config& config) {
    config.clear();
    
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        
        if (key == "metadata") {
            // Handle metadata
            for (auto metaIt = it.value().begin(); metaIt != it.value().end(); ++metaIt) {
                std::string entryKey = metaIt.key();
                const auto& meta = metaIt.value();
                
                if (meta.contains("source") && meta.contains("value")) {
                    ConfigSource source = jsonSourceToConfigSource(meta["source"].get<int>());
                    std::string description = meta["description"].get<std::string>();
                    std::string valueStr = meta["value"].get<std::string>();
                    
                    config.set(entryKey, valueStr, source);
                    config.setEntryDescription(entryKey, description);
                }
            }
        } else {
            // Handle direct key-value pairs
            std::string valueStr = it.value().get<std::string>();
            config.set(key, valueStr, ConfigSource::File);
        }
    }
}

void JSONConfig::convertToJSON(const Config& config, json& j) {
    j = json::object();
    
    for (const auto& entry : config.getKeys()) {
        auto val = config.get<std::string>(entry);
        if (val.has_value()) {
            j[entry] = val.value();
        }
        
        // Add metadata about source and description
        auto entryInfo = json::object();
        entryInfo["source"] = configSourceToJSONString(config.getEntrySource(entry));
        entryInfo["description"] = config.getEntryDescription(entry);
        j["metadata"][entry] = entryInfo;
    }
}

void JSONConfig::convertFromJSON(const json& j, Config& config) {
    config.clear();
    
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        
        if (key == "metadata") {
            // Handle metadata
            for (auto metaIt = it.value().begin(); metaIt != it.value().end(); ++metaIt) {
                std::string entryKey = metaIt.key();
                const auto& meta = metaIt.value();
                
                if (meta.contains("source") && meta.contains("value")) {
                    ConfigSource source = jsonSourceToConfigSource(meta["source"].get<int>());
                    std::string description = meta["description"].get<std::string>();
                    std::string valueStr = meta["value"].get<std::string>();
                    
                    config.set(entryKey, valueStr, source);
                    config.setEntryDescription(entryKey, description);
                }
            }
        } else {
            // Handle direct key-value pairs
            std::string valueStr = it.value().get<std::string>();
            config.set(key, valueStr, ConfigSource::File);
        }
    }
}

void JSONConfig::convertToJSON(const Config& config, json& j) {
    j = json::object();
    
    for (const auto& entry : config.getKeys()) {
        auto val = config.get<std::string>(entry);
        if (val.has_value()) {
            j[entry] = val.value();
        }
        
        // Add metadata about source and description
        auto entryInfo = json::object();
        entryInfo["source"] = configSourceToJSONString(config.getEntrySource(entry));
        entryInfo["description"] = config.getEntryDescription(entry);
        j["metadata"][entry] = entryInfo;
    }
}

void JSONConfig::convertFromJSON(const json& j, Config& config) {
    config.clear();
    
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        
        if (key == "metadata") {
            // Handle metadata
            for (auto metaIt = it.value().begin(); metaIt != it.value().end(); ++metaIt) {
                std::string entryKey = metaIt.key();
                const auto& meta = metaIt.value();
                
                if (meta.contains("source") && meta.contains("value")) {
                    ConfigSource source = jsonSourceToConfigSource(meta["source"].get<int>());
                    std::string description = meta["description"].get<std::string>();
                    std::string valueStr = meta["value"].get<std::string>();
                    
                    config.set(entryKey, valueStr, source);
                    config.setEntryDescription(entryKey, description);
                }
            }
        } else {
            // Handle direct key-value pairs
            std::string valueStr = it.value().get<std::string>();
            config.set(key, valueStr, ConfigSource::File);
        }
    }
}
