    // Configuration implementation

// Add validation methods
bool Config::hasValidNeuronCount() const {
    int64_t count = getOr<int64_t>("neuron_count", 0);
    return count > 0 && count <= 100000;  // Reasonable limits
}

bool Config::hasValidConnectionProbability() const {
    double prob = getOr<double>("connection_probability", 0.0);
    return prob >= 0.0 && prob <= 1.0;
}

bool Config::hasValidTimestep() const {
    double timestep = getOr<double>("simulation_timestep", 0.0);
    return timestep > 0.0 && timestep <= 1.0;  // Reasonable timestep
}

// Add validation implementation
bool Config::validate() const {
    // Basic validation - can be extended with schema validation
    for (const auto& entry : pImpl->entries) {
        // Validate based on key
        if (entry.key == "neuron_count" && !hasValidNeuronCount()) {
            NLM_LOG_ERROR("Invalid neuron_count value: " + std::to_string(getOr<int64_t>(entry.key, 0)));
            return false;
        }
        
        if (entry.key == "connection_probability" && !hasValidConnectionProbability()) {
            NLM_LOG_ERROR("Invalid connection_probability value: " + std::to_string(getOr<double>(entry.key, 0.0)));
            return false;
        }
        
        if (entry.key == "simulation_timestep" && !hasValidTimestep()) {
            NLM_LOG_ERROR("Invalid simulation_timestep value: " + std::to_string(getOr<double>(entry.key, 0.0)));
            return false;
        }
    }
    return true;
}

// Add Python dict method implementation
py::dict Config::toPythonDict() const {
    py::dict result;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&result, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                result[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                result[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                result[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                                std::is_same_v<T, uint64_t>) {
                result[entry.key] = static_cast<double>(arg);  // Use float in Python
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                py::list list;
                for (const auto& item : arg) {
                    list.append(item);
                }
                result[entry.key] = list;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                py::list list;
                for (const auto& item : arg) {
                    list.append(item);
                }
                result[entry.key] = list;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                py::list list;
                for (const auto& item : arg) {
                    list.append(item);
                }
                result[entry.key] = list;
            }
        }, entry.value);
    }
    
    return result;
}

} // namespace nlm