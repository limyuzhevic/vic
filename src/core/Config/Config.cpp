// Enhanced set method implementations for Config

template<typename T>
void Config::setT(const std::string& key, T value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Enhanced has method for Config
bool Config::has(const std::string& key) const {
    if (!pImpl) {
        return false;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

// Enhanced remove method for Config
void Config::remove(const std::string& key) {
    if (!pImpl) {
        return;
    }
    
    if (has(key)) {
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
        NLM_LOG_DEBUG("Removed configuration key: " + key);
    }
}

// Enhanced getKeys method for Config
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    if (!pImpl) {
        return keys;
    }
    
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

// Enhanced clear method for Config
void Config::clear() {
    if (pImpl) {
        size_t oldCount = pImpl->entries.size();
        pImpl->entries.clear();
        if (oldCount > 0) {
            NLM_LOG_INFO("Cleared " + std::to_string(oldCount) + " configuration entries");
        }
    }
}

// Enhanced summary method for Config
std::string Config::summary() const {
    if (!pImpl) {
        return "Configuration: Not initialized";
    }
    
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    // Count entries by source
    std::map<int, int> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[static_cast<int>(entry.source)]++;
    }
    
    oss << "  Sources:";
    for (const auto& [source, count] : sourceCounts) {
        oss << " " << count << "x (" << source << ")";
    }
    oss << "\n";
    
    // Show some example entries
    oss << "  Examples:";
    size_t exampleCount = std::min<size_t>(5, pImpl->entries.size());
    for (size_t i = 0; i < exampleCount; ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "\n    " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "]";
    }
    
    if (pImpl->entries.size() > exampleCount) {
        oss << "\n    ... and " << (pImpl->entries.size() - exampleCount) << " more entries";
    }
    
    if (!pImpl->lastError.empty()) {
        oss << "\n\nLast Error: " << pImpl->lastError;
    }
    
    return oss.str();
}

// Enhanced trim method for Config
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Enhanced toLower method for Config
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Enhanced Config template instantiations
namespace {
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}

} // namespace nlm