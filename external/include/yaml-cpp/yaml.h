// Minimal fallback yaml-cpp/yaml.h header for NLM project
// This provides a minimal YAML implementation for the Config class
// Based on YAML-CPP API but simplified

#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace YAML {

// Simple YAML node type
class Node {
public:
    using NodeType = std::variant<
        std::nullptr_t,
        bool,
        int,
        double,
        std::string,
        std::vector<Node>,
        std::map<std::string, Node>
    >;

private:
    NodeType data_;

public:
    // Constructors
    Node() : data_(nullptr) {}
    Node(std::nullptr_t) : data_(nullptr) {}
    Node(bool b) : data_(b) {}
    Node(int i) : data_(i) {}
    Node(double d) : data_(d) {}
    Node(float f) : data_(static_cast<double>(f)) {}
    Node(const std::string& s) : data_(s) {}
    Node(const char* s) : data_(std::string(s)) {}
    Node(const std::vector<Node>& arr) : data_(arr) {}
    Node(const std::map<std::string, Node>& obj) : data_(obj) {}

    // Type checking
    bool IsScalar() const {
        return std::holds_alternative<std::nullptr_t>(data_) ||
               std::holds_alternative<bool>(data_) ||
               std::holds_alternative<int>(data_) ||
               std::holds_alternative<double>(data_) ||
               std::holds_alternative<std::string>(data_);
    }

    bool IsIntegral() const {
        return std::holds_alternative<int>(data_);
    }

    bool IsFloat() const {
        return std::holds_alternative<double>(data_);
    }

    bool IsBool() const {
        return std::holds_alternative<bool>(data_);
    }

    bool IsSequence() const {
        return std::holds_alternative<std::vector<Node>>(data_);
    }

    bool IsMap() const {
        return std::holds_alternative<std::map<std::string, Node>>(data_);
    }

    // Type access
    int as_int() const {
        if (IsIntegral()) {
            return std::get<int>(data_);
        } else if (IsFloat()) {
            return static_cast<int>(std::get<double>(data_));
        } else {
            throw std::runtime_error("Not an integer");
        }
    }

    double as_float() const {
        if (IsFloat()) {
            return std::get<double>(data_);
        } else if (IsIntegral()) {
            return static_cast<double>(std::get<int>(data_));
        } else {
            throw std::runtime_error("Not a number");
        }
    }

    bool as_bool() const {
        if (IsBool()) {
            return std::get<bool>(data_);
        } else {
            throw std::runtime_error("Not a boolean");
        }
    }

    const std::string& as_string() const {
        if (std::holds_alternative<std::string>(data_)) {
            return std::get<std::string>(data_);
        } else if (IsScalar()) {
            // Convert scalar to string
            std::ostringstream oss;
            if (std::holds_alternative<bool>(data_)) {
                oss << (std::get<bool>(data_) ? "true" : "false");
            } else if (std::holds_alternative<int>(data_)) {
                oss << std::get<int>(data_);
            } else if (std::holds_alternative<double>(data_)) {
                oss << std::get<double>(data_);
            } else {
                oss << "null";
            }
            return *new std::string(oss.str()); // This is a memory leak but OK for prototyping
        } else {
            throw std::runtime_error("Not a string");
        }
    }

    const std::vector<Node>& as_sequence() const {
        if (IsSequence()) {
            return std::get<std::vector<Node>>(data_);
        } else {
            throw std::runtime_error("Not a sequence");
        }
    }

    const std::map<std::string, Node>& as_map() const {
        if (IsMap()) {
            return std::get<std::map<std::string, Node>>(data_);
        } else {
            throw std::runtime_error("Not a map");
        }
    }

    // Iteration
    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::pair<const std::string, Node>;
        using pointer = value_type*;
        using reference = value_type&;

        const_iterator() : current(nullptr), is_end(false) {}
        const_iterator(const std::map<std::string, Node>* map, bool end = false) : 
            current(map), is_end(end), it_map(map ? map->begin() : std::map<std::string, Node>::const_iterator()) {}
        
        reference operator*() {
            return *it_map;
        }
        
        pointer operator->() {
            return &it_map;
        }
        
        const_iterator& operator++() {
            ++it_map;
            if (it_map == current->end()) {
                is_end = true;
            }
            return *this;
        }
        
        bool operator==(const const_iterator& other) const {
            return current == other.current && it_map == other.it_map && is_end == other.is_end;
        }
        
        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }

    private:
        const std::map<std::string, Node>* current;
        bool is_end;
        std::map<std::string, Node>::const_iterator it_map;
    };

    const_iterator begin() const {
        if (IsMap()) {
            return const_iterator(&get_map().data(), false);
        }
        return const_iterator(nullptr, true);
    }

    const_iterator end() const {
        if (IsMap()) {
            return const_iterator(&get_map().data(), true);
        }
        return const_iterator(nullptr, true);
    }

    // Key access
    Node operator[](const std::string& key) const {
        if (IsMap()) {
            auto it = get_map().find(key);
            if (it != get_map().end()) {
                return it->second;
            }
        }
        return Node(); // Return null node if key not found
    }

    bool IsNull() const {
        return std::holds_alternative<std::nullptr_t>(data_);
    }

    // Helper methods for map access
    const std::map<std::string, Node>& get_map() const {
        return std::get<std::map<std::string, Node>>(data_);
    }

    // Serialization
    std::string as_string(const std::string& filepath) const {
        std::ostringstream oss;
        if (IsMap()) {
            for (const auto& pair : get_map()) {
                oss << pair.first << ": " << pair.second.dump() << "\n";
            }
        } else if (IsSequence()) {
            for (const auto& item : as_sequence()) {
                oss << "- " << item.dump() << "\n";
            }
        } else {
            oss << dump();
        }
        return oss.str();
    }

    std::string dump() const {
        std::ostringstream oss;
        if (IsNull()) {
            oss << "null";
        } else if (IsBool()) {
            oss << (as_bool() ? "true" : "false");
        } else if (IsIntegral()) {
            oss << as_int();
        } else if (IsFloat()) {
            oss << as_float();
        } else if (std::holds_alternative<std::string>(data_)) {
            oss << as_string();
        } else if (IsSequence()) {
            oss << "[";
            const auto& arr = as_sequence();
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arr[i].dump();
            }
            oss << "]";
        } else if (IsMap()) {
            oss << "{";
            const auto& obj = get_map();
            size_t i = 0;
            for (const auto& pair : obj) {
                if (i++ > 0) oss << ", ";
                oss << pair.first << ": " << pair.second.dump();
            }
            oss << "}";
        }
        return oss.str();
    }

    static Node Load(const std::string& filepath) {
        // Simple YAML loader for key=value pairs
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return Node();
        }
        
        return loadStream(file);
    }

    static Node Load(std::istream& input) {
        return loadStream(input);
    }

private:
    static Node loadStream(std::istream& input) {
        std::string line;
        std::map<std::string, Node> result_map;
        
        while (std::getline(input, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty() || line[0] == '#') {
                continue; // Skip comments and empty lines
            }
            
            // Check for YAML list item
            if (line.find("-") == 0) {
                // Handle list item (simplified)
                line = line.substr(1);
                line.erase(0, line.find_first_not_of(" \t"));
                // For now, just treat as scalar value
                // In a full implementation, this would be a more complex parser
                continue;
            }
            
            // Parse key=value
            size_t equals = line.find('=');
            if (equals != std::string::npos) {
                std::string key = line.substr(0, equals);
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                
                std::string value = line.substr(equals + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // Determine value type
                if (value == "true" || value == "false") {
                    result_map[key] = (value == "true");
                } else if (value.find('.') != std::string::npos || value[0] == '-') {
                    try {
                        size_t dot = value.find('.');
                        if (dot != std::string::npos) {
                            double num = std::stod(value);
                            result_map[key] = num;
                        } else {
                            int num = std::stoi(value);
                            result_map[key] = num;
                        }
                    } catch (...) {
                        result_map[key] = value;
                    }
                } else {
                    try {
                        int num = std::stoi(value);
                        result_map[key] = num;
                    } catch (...) {
                        result_map[key] = value;
                    }
                }
            }
        }
        
        return Node(result_map);
    }
};

// Emitter class for YAML generation
class Emitter {
public:
    Emitter() {}
    
    Emitter& << const std::string& key {
        current_key_ = key;
        return *this;
    }
    
    Emitter& << const Node& value {
        // Store key-value pair
        content_ += "  " + current_key_ + ": " + value.dump() + "\n";
        return *this;
    }
    
    Emitter& operator<<(const char* value) {
        return *this << std::string(value);
    }
    
    std::string str() const {
        return content_;
    }
    
private:
    std::string current_key_;
    std::string content_;
};

inline Node Load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return Node();
    }
    return Node::Load(file);
}

inline Node Load(std::istream& input) {
    return Node::Load(input);
}

} // namespace YAML
