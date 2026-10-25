// Validation and error handling implementation

// Validation implementation
template<typename T>
bool Config::validate(const std::vector<std::string>& requiredKeys) const {
    validationErrors_.clear();
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            validationErrors_.push_back("Missing required key: " + key);
        }
    }
    
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            validationErrors_.push_back("Invalid key: " + entry.key);
        }
        if (!validateValue(entry.key, entry.value)) {
            validationErrors_.push_back("Invalid value for key: " + entry.key);
        }
    }
    
    return validationErrors_.empty();
}

bool Config::validateSchema(const std::vector<ConfigEntry>& schema) const {
    validationErrors_.clear();
    
    for (const auto& schemaEntry : schema) {
        if (!has(schemaEntry.key)) {
            if (schemaEntry.source != ConfigSource::Runtime) {
                continue;
            }
            validationErrors_.push_back("Missing required schema key: " + schemaEntry.key);
            continue;
        }
        
        auto value = getFromEntry(schemaEntry.key);
        if (value && !isValueTypeCompatible(value.value(), schemaEntry.value)) {
            validationErrors_.push_back("Value type mismatch for key: " + schemaEntry.key);
        }
    }
    
    return validationErrors_.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return validationErrors_;
}

std::string Config::getDiagnosticInfo() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Diagnostics ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Validation errors: " << validationErrors_.size() << "\n";
    
    if (!validationErrors_.empty()) {
        oss << "\nValidation errors:\n";
        for (const auto& error : validationErrors_) {
            oss << "  - " << error << "\n";
        }
    }
    
    oss << "\nEntry sources:\n";
    std::unordered_map<ConfigSource, size_t> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    for (const auto& [source, count] : sourceCount) {
        oss << "  " << static_cast<int>(source) << ": " << count << " entries\n";
    }
    
    return oss.str();
}

void Config::merge(const Config& other, bool overwrite) {
    for (const auto& entry : other.pImpl->entries) {
        if (overwrite || !has(entry.key)) {
            pImpl->entries.push_back(entry);
        }
    }
}

bool Config::isValid() const {
    return validationErrors_.empty();
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

std::vector<std::pair<std::string, ConfigSource>> Config::getSourceReport() const {
    std::vector<std::pair<std::string, ConfigSource>> report;
    report.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        report.emplace_back(entry.key, entry.source);
    }
    
    return report;
}

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) return false;
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.') {
            return false;
        }
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    std::visit([this, &key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            if (arg < -1000000 || arg > 1000000) {
                validationErrors_.push_back("Integer value out of range for key: " + key);
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (arg < -1000000000000LL || arg > 1000000000000LL) {
                validationErrors_.push_back("Int64 value out of range for key: " + key);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(arg) || std::isinf(arg)) {
                validationErrors_.push_back("Invalid float value for key: " + key);
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // Boolean validation always passes
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                validationErrors_.push_back("Empty string value for key: " + key);
            }
        }
    }, value);
    
    return validationErrors_.empty();
}

void Config::logValidationError(const std::string& error) {
    validationErrors_.push_back(error);
}

bool Config::isValueTypeCompatible(const ConfigValue& actual, const ConfigValue& expected) const {
    return actual.index() == expected.index();
}

std::optional<ConfigValue> Config::getFromEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
}

// Validation template instantiations
template bool Config::validate<int>(const std::vector<std::string>&) const;
template bool Config::validate<double>(const std::vector<std::string>&) const;
template bool Config::validate<bool>(const std::vector<std::string>&) const;
template bool Config::validate<std::string>(const std::vector<std::string>&) const;

// Additional vector template instantiations
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

// Additional getOr vector template instantiations
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm