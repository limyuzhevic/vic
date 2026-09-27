// Minimal YAML mock for testing
#ifndef YAML_CPP_YAML_H
#define YAML_CPP_YAML_H

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <variant>

namespace YAML {
    class Node {
    public:
        // Use pointers to avoid recursive variant issues
        using node_value = std::variant<int, int64_t, double, bool, std::string, std::vector<Node*>, std::map<std::string, Node*>>;
        
        // Constructors
        Node() = default;
        Node(int val) : data_(val) {}
        Node(double val) : data_(val) {}
        Node(bool val) : data_(val) {}
        Node(const std::string& val) : data_(val) {}
        
        // Access methods
        bool IsScalar() const { return is_scalar_; }
        bool IsInteger() const { return std::holds_alternative<int>(data_) || 
                                    std::holds_alternative<int64_t>(data()); }
        bool IsDouble() const { return std::holds_alternative<double>(data()); }
        bool IsBoolean() const { return std::holds_alternative<bool>(data()); }
        bool IsString() const { return std::holds_alternative<std::string>(data()); }
        bool IsSequence() const { 
            if (std::holds_alternative<std::vector<Node*>>(data_)) {
                return !std::get<std::vector<Node*>>(data_).empty();
            }
            return false; 
        }
        bool IsMap() const { return is_map_; }
        bool empty() const {
            if (std::holds_alternative<std::vector<Node*>>(data_)) {
                return std::get<std::vector<Node*>>(data_).empty();
            }
            return false;
        }
        
        // Get methods
        int as_int() const { return std::get<int>(data_); }
        int64_t as_int64() const { return std::get<int64_t>(data_); }
        double as_double() const { return std::get<double>(data_); }
        bool as_bool() const { return std::get<bool>(data_); }
        const std::string& as_string() const { return std::get<std::string>(data_); }
        const std::vector<Node*>& as_sequence() const { return std::get<std::vector<Node*>>(data_); }
        
        // Iterator support
        auto begin() { 
            if (std::holds_alternative<std::map<std::string, Node*>>(data_)) {
                return std::get<std::map<std::string, Node*>>(data_).begin();
            }
            return std::vector<std::pair<std::string, Node*>>().begin();
        }
        auto end() { 
            if (std::holds_alternative<std::map<std::string, Node*>>(data_)) {
                return std::get<std::map<std::string, Node*>>(data_).end();
            }
            return std::vector<std::pair<std::string, Node*>>().end();
        }
        
        // For the convertYamlValue function
        const Node& operator[](size_t index) const {
            return *std::get<std::vector<Node*>>(data_)[index];
        }
        
    private:
        node_value data_;
        bool is_scalar_ = true;
        bool is_map_ = false;
        
        const node_value& data() const { return data_; }
        node_value& data() { return data_; }
    };
    
    // Load function
    inline Node Load(std::istream&) {
        return Node();
    }
    
    // Emitter classes for YAML output
    class Emitter {
    public:
        Emitter() = default;
        void SetIndent(int) {}
        void SetMapFormat(int) {}
        
        // Streaming operators
        Emitter& operator<<(const std::string& val) { return *this; }
        Emitter& operator<<(int val) { return *this; }
        Emitter& operator<<(double val) { return *this; }
        Emitter& operator<<(bool val) { return *this; }
        Emitter& operator<<(const char* val) { return *this; }
        
        // YAML specific operators
        Emitter& operator<<(const Node&) { return *this; }
        Emitter& operator<<(const std::string&) { return *this; }
        
        // Begin/end document
        Emitter& operator<<(const std::string& val) { return *this; }
        
        const std::string& str() const { return buffer_; }
        
    private:
        std::string buffer_;
    };
    
    // YAML tags
    const std::string BeginDoc = "BeginDoc";
    const std::string EndDoc = "EndDoc";
    const std::string BeginMap = "BeginMap";
    const std::string EndMap = "EndMap";
    const std::string BeginSeq = "BeginSeq";
    const std::string EndSeq = "EndSeq";
    const std::string Key = "Key";
    
    // Format constants
    enum E_MultiDoc {
        E_MultiDoc = 0
    };
    
    // Exception class
    class Exception : public std::runtime_error {
    public:
        Exception(const std::string& msg) : std::runtime_error(msg) {}
    };
}

#endif // YAML_CPP_YAML_H