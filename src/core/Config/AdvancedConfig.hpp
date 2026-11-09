// Advanced helper functions
namespace nlm {
    // Parse string to value (enhanced for advanced types)
    template<typename T>
    T stringToValue(const std::string& str);
    
    // Format value to string (enhanced for advanced types)
    template<typename T>
    std::string valueToString(const T& value);
    
    // Parse string to vector
    template<typename T>
    std::vector<T> stringToVector(const std::string& str);
    
    // Check if string represents a boolean
    bool isBoolean(const std::string& str);
    
    // Check if string represents a number
    bool isNumber(const std::string& str);
    
    // Check if string represents a vector
    bool isVector(const std::string& str);
}
