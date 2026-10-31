// Minimal fallback nlohmann/json.hpp header for NLM project
// This provides a minimal JSON implementation for the Config class
// See: https://github.com/nlohmann/json (MIT License)

#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace nlohmann {

// Simple JSON value type
class json {
public:
    using value_type = std::variant<
        std::nullptr_t,
        bool,
        double,
        std::string,
        std::vector<json>,
        std::map<std::string, json>
    >;

private:
    value_type data_;

public:
    // Constructors
    json() : data_(nullptr) {}
    json(std::nullptr_t) : data_(nullptr) {}
    json(bool b) : data_(b) {}
    json(double d) : data_(d) {}
    json(float f) : data_(static_cast<double>(f)) {}
    json(int i) : data_(static_cast<double>(i)) {}
    json(const std::string& s) : data_(s) {}
    json(const char* s) : data_(std::string(s)) {}
    json(const std::vector<json>& arr) : data_(arr) {}
    json(const std::map<std::string, json>& obj) : data_(obj) {}

    // Conversion helpers
    bool is_null() const { return std::holds_alternative<std::nullptr_t>(data_); }
    bool is_boolean() const { return std::holds_alternative<bool>(data_); }
    bool is_number() const { return std::holds_alternative<double>(data_); }
    bool is_string() const { return std::holds_alternative<std::string>(data_); }
    bool is_array() const { return std::holds_alternative<std::vector<json>>(data_); }
    bool is_object() const { return std::holds_alternative<std::map<std::string, json>>(data_); }

    // Type access
    bool get_bool() const { return std::get<bool>(data_); }
    double get_number() const { return std::get<double>(data_); }
    const std::string& get_string() const { return std::get<std::string>(data_); }
    const std::vector<json>& get_array() const { return std::get<std::vector<json>>(data_); }
    const std::map<std::string, json>& get_object() const { return std::get<std::map<std::string, json>>(data_); }

    // JSON serialization
    std::string dump(int indent = -1) const {
        std::ostringstream oss;
        if (is_null()) {
            oss << "null";
        } else if (is_boolean()) {
            oss << (get_bool() ? "true" : "false");
        } else if (is_number()) {
            oss << get_number();
        } else if (is_string()) {
            oss << "\"" << get_string() << "\"";
        } else if (is_array()) {
            oss << "[";
            const auto& arr = get_array();
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arr[i].dump(indent);
            }
            oss << "]";
        } else if (is_object()) {
            oss << "{";
            const auto& obj = get_object();
            size_t i = 0;
            for (const auto& pair : obj) {
                if (i++ > 0) oss << ", ";
                oss << "\"" << pair.first << "\": " << pair.second.dump(indent);
            }
            oss << "}";
        }
        return oss.str();
    }

    // JSON parsing (simplified)
    static json parse(const std::string& s) {
        // Very basic JSON parser for simple key=value pairs
        // This is a simplified implementation for NLM configuration
        json result;
        
        // Try to parse as simple key=value pairs (like configs/default.cfg)
        if (s.find('=') != std::string::npos) {
            std::map<std::string, json> obj;
            size_t pos = 0;
            while (pos < s.length()) {
                size_t equals = s.find('=', pos);
                if (equals == std::string::npos) break;
                
                std::string key = s.substr(pos, equals - pos);
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                
                size_t next_comma = s.find(',', equals);
                if (next_comma == std::string::npos) {
                    next_comma = s.length();
                }
                
                std::string value = s.substr(equals + 1, next_comma - equals - 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // Try to determine value type
                if (value == "true" || value == "false") {
                    obj[key] = (value == "true");
                } else {
                    try {
                        size_t dot = value.find('.');
                        if (dot != std::string::npos) {
                            obj[key] = std::stod(value);
                        } else {
                            obj[key] = std::stoi(value);
                        }
                    } catch (...) {
                        obj[key] = value;
                    }
                }
                
                pos = next_comma + 1;
                while (pos < s.length() && (s[pos] == ' ' || s[pos] == '\t')) {
                    pos++;
                }
            }
            result = obj;
        } else if (s.find('{') != std::string::npos) {
            // Try to parse as JSON object
            result = json::parseJSON(s);
        } else {
            // Fallback to simple string
            result = s;
        }
        
        return result;
    }

private:
    static json parseJSON(const std::string& s) {
        // Very simplified JSON parser
        json result;
        
        // Remove whitespace
        std::string cleaned;
        for (char c : s) {
            if (!std::isspace(c)) cleaned += c;
        }
        
        if (cleaned.empty() || cleaned[0] != '{') return result;
        
        std::map<std::string, json> obj;
        size_t pos = 1;
        
        while (pos < cleaned.length() && cleaned[pos] != '}') {
            // Find key
            size_t quote = cleaned.find('"', pos);
            if (quote == std::string::npos) break;
            
            size_t quote_end = cleaned.find('"', quote + 1);
            if (quote_end == std::string::npos) break;
            
            std::string key = cleaned.substr(quote + 1, quote_end - quote - 1);
            pos = quote_end + 1;
            
            // Skip colon
            if (pos < cleaned.length() && cleaned[pos] == ':') {
                pos++;
            }
            
            // Parse value
            size_t comma_or_brace = cleaned.find_first_of(",}", pos);
            if (comma_or_brace == std::string::npos) break;
            
            std::string value = cleaned.substr(pos, comma_or_brace - pos);
            
            // Simple value type detection
            if (value == "null" || value == "true" || value == "false") {
                bool bool_val = (value == "true");
                obj[key] = json(bool_val);
            } else if (value.find('.') != std::string::npos || value[0] == '-') {
                try {
                    double num = std::stod(value);
                    obj[key] = json(num);
                } catch (...) {
                    obj[key] = json(value);
                }
            } else {
                try {
                    int num = std::stoi(value);
                    obj[key] = json(num);
                } catch (...) {
                    obj[key] = json(value);
                }
            }
            
            pos = comma_or_brace + 1;
        }
        
        result = obj;
        return result;
    }

    // Iterator support
    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::pair<const std::string, json>;
        using pointer = value_type*;
        using reference = value_type&;

        iterator() : current(nullptr) {}
        iterator(const std::map<std::string, json>* map, bool end = false) : current(map), is_end(end) {}
        
        reference operator*() {
            return *it_map;
        }
        
        pointer operator->() {
            return &it_map;
        }
        
        iterator& operator++() {
            ++it_map;
            if (it_map == current->end()) {
                is_end = true;
            }
            return *this;
        }
        
        bool operator==(const iterator& other) const {
            return current == other.current && it_map == other.it_map;
        }
        
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

    private:
        const std::map<std::string, json>* current;
        bool is_end;
        std::map<std::string, json>::const_iterator it_map;
        
        friend class json;
    };

    friend class iterator;
    
public:
    iterator begin() const {
        if (is_object()) {
            return iterator(&get_object().data(), false);
        }
        return iterator(nullptr, true);
    }
    
    iterator end() const {
        if (is_object()) {
            return iterator(&get_object().data(), true);
        }
        return iterator(nullptr, true);
    }

    // Key access
    json& operator[](const std::string& key) {
        if (!is_object()) {
            // Convert to object
            *this = std::map<std::string, json>({{key, json()}});
        }
        return std::get<std::map<std::string, json>>(data_)[key];
    }
    
    const json& operator[](const std::string& key) const {
        return get_object().at(key);
    }
    
    bool contains(const std::string& key) const {
        return is_object() && get_object().find(key) != get_object().end();
    }
    
    // Key iteration
    std::vector<std::string> keys() const {
        if (!is_object()) return {};
        std::vector<std::string> keys;
        for (const auto& pair : get_object()) {
            keys.push_back(pair.first);
        }
        return keys;
    }
};

// Non-member functions
inline std::ostream& operator<<(std::ostream& os, const json& j) {
    os << j.dump();
    return os;
}

} // namespace nlohmann
