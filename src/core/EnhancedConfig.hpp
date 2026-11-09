// Environment variable utilities
namespace Env {
    // Get environment variable with default
    std::string get(const std::string& name, const std::string& defaultValue = "");
    
    // Get environment variable as integer
    int getInt(const std::string& name, int defaultValue = 0);
    
    // Get environment variable as float
    float getFloat(const std::string& name, float defaultValue = 0.0f);
    
    // Get environment variable as boolean
    bool getBool(const std::string& name, bool defaultValue = false);
    
    // Check if environment variable exists
    bool exists(const std::string& name);
    
    // Get all environment variables matching prefix
    std::vector<std::string> getMatching(const std::string& prefix);
    
    // Set environment variable
    void set(const std::string& name, const std::string& value);
    
    // Remove environment variable
    void remove(const std::string& name);
}

} // namespace nlm