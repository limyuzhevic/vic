// Validate configuration against expected schema and ranges
bool Config::validate() const {
    // This method validates the current configuration
    // For now, it just checks for basic consistency
    // In a full implementation, this would validate against a schema
    
    // Check for potential inconsistencies
    bool isValid = true;
    
    if (has("neuron_count") && has("region_count")) {
        int64_t neuronCount = getOr<int64_t>("neuron_count", 0);
        int64_t regionCount = getOr<int64_t>("region_count", 0);
        
        if (neuronCount < regionCount * 10) {
            // Warning: Not enough neurons for the number of regions
            isValid = false;
        }
    }
    
    return isValid;
}

} // namespace nlm