// Minimal JSON mock for testing
#ifndef NLOHMANN_JSON_HPP
#define NLOHMANN_JSON_HPP

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <variant>

namespace nlohmann {
    class json {
    public:
        // JSON value types
        using json_value = std::variant<int, int64_t, double, bool, std::string, std::vector<json>>;
        
        // Constructors
        json() = default;
        json(int val) : data_(val) {}
        json(double val) : data_(val) {}
        json(bool val) : data_(val) {}
        json(const std::string& val) : data_(val) {}
        json(const char* val) : data_(std::string(val)) {}
        
        // Access methods
        bool is_number() const { return std::holds_alternative<int>(data()) || 
                                  std::holds_alternative<int64_t>(data()) || 
                                  std::holds_alternative<double>(data()); }
        bool is_number_integer() const { return std::holds_alternative<int>(data()) || 
                                        std::holds_alternative<int64_t>(data()); }
        bool is_number_unsigned() const { return false; }
        bool is_boolean() const { return std::holds_alternative<bool>(data()); }
        bool is_string() const { return std::holds_alternative<std::string>(data()); }
        bool is_array() const { return std::holds_alternative<std::vector<json>>(data()); }
        
        // Get methods
        int get_int() const { return std::get<int>(data_); }
        int64_t get_int64() const { return std::get<int64_t>(data_); }
        double get_double() const { return std::get<double>(data_); }
        bool get_bool() const { return std::get<bool>(data_); }
        const std::string& get_string() const { return std::get<std::string>(data_); }
        const std::vector<json>& get_array() const { return std::get<std::vector<json>>(data_); }
        
        // Key access
        const json& operator[](const std::string& key) const { return json_map_.at(key); }
        json& operator[](const std::string& key) { return json_map_[key]; }
        
        // Utility methods
        std::string dump(int indent = -1) const {
            // Simple dump implementation
            return "{}";
        }
        
        // Begin and end for iteration
        auto begin() { return json_map_.begin(); }
        auto end() { return json_map_.end(); }
        
    private:
        json_value data_;
        std::map<std::string, json> json_map_;
        
        const json_value& data() const { return data_; }
        json_value& data() { return data_; }
    };
    
    // Parse function
    inline json json::parse(std::istream&) {
        return json();
    }
    
    // Exception class
    class json_exception : public std::runtime_error {
    public:
        json_exception(const std::string& msg) : std::runtime_error(msg) {}
    };
}

#endif // NLOHMANN_JSON_HPP