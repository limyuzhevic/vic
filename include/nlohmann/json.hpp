// Minimal nlohmann/json.hpp for testing
// Provides the interface needed by Config.cpp

#ifndef NLOHMANN_JSON_HPP
#define NLOHMANN_JSON_HPP

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <variant>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace nlohmann {

// Exception classes
class json_parse_error : public std::runtime_error {
public:
    json_parse_error(const std::string& msg) : std::runtime_error(msg) {}
};

class json_type_error : public std::runtime_error {
public:
    json_type_error(const std::string& msg) : std::runtime_error(msg) {}
};

class json {
public:
    // Types
    using object_t = std::map<std::string, json>;
    using array_t = std::vector<json>;
    using string_t = std::string;
    using number_integer_t = int64_t;
    using number_float_t = double;
    using boolean_t = bool;
    
    // Value type
    using value_t = std::variant<std::nullptr_t, boolean_t, number_integer_t, number_float_t, string_t, array_t, object_t>;
    
private:
    value_t m_value;
    
public:
    // Constructors
    json() : m_value(std::nullptr_t()) {}
    json(std::nullptr_t) : m_value(std::nullptr_t()) {}
    json(bool b) : m_value(b) {}
    json(int64_t n) : m_value(n) {}
    json(double n) : m_value(n) {}
    json(const std::string& s) : m_value(s) {}
    json(const char* s) : m_value(std::string(s)) {}
    json(const std::vector<json>& arr) : m_value(arr) {}
    json(const std::map<std::string, json>& obj) : m_value(obj) {}
    
    // Type checking
    bool is_null() const { return std::holds_alternative<std::nullptr_t>(m_value); }
    bool is_boolean() const { return std::holds_alternative<boolean_t>(m_value); }
    bool is_number() const { return std::holds_alternative<number_integer_t>(m_value) || std::holds_alternative<number_float_t>(m_value); }
    bool is_number_integer() const { return std::holds_alternative<number_integer_t>(m_value); }
    bool is_string() const { return std::holds_alternative<string_t>(m_value); }
    bool is_array() const { return std::holds_alternative<array_t>(m_value); }
    bool is_object() const { return std::holds_alternative<object_t>(m_value); }
    
    // Stream extraction operator
    template<typename T> void operator>>(T& value) {
        if (std::holds_alternative<T>(m_value)) {
            value = std::get<T>(m_value);
        } else {
            throw json_parse_error("type error: cannot convert JSON value to target type");
        }
    }
    
    // Conversion operators
    explicit operator bool() const {
        if (is_boolean()) return std::get<boolean_t>(m_value);
        if (is_number_integer()) return std::get<number_integer_t>(m_value) != 0;
        if (is_number()) return std::get<number_float_t>(m_value) != 0.0;
        if (is_null()) return false;
        if (is_string()) return !std::get<string_t>(m_value).empty();
        return true;
    }
    
    explicit operator int64_t() const {
        if (is_number_integer()) {
            return std::get<number_integer_t>(m_value);
        }
        if (is_number()) {
            double val = std::get<number_float_t>(m_value);
            if (std::floor(val) == val && val >= std::numeric_limits<int64_t>::min() && 
                val <= std::numeric_limits<int64_t>::max()) {
                return static_cast<int64_t>(val);
            }
        }
        throw json_type_error("type error: cannot convert JSON value to integer");
    }
    
    explicit operator double() const {
        if (is_number_integer()) {
            return static_cast<double>(std::get<number_integer_t>(m_value));
        }
        if (is_number()) {
            return std::get<number_float_t>(m_value);
        }
        throw json_type_error("type error: cannot convert JSON value to number");
    }
    
    explicit operator std::string() const {
        if (is_string()) {
            return std::get<string_t>(m_value);
        }
        throw json_type_error("type error: cannot convert JSON value to string");
    }
    
    // Iterator support
    auto begin() {
        if (is_object()) {
            return std::get<object_t>(m_value).begin();
        }
        if (is_array()) {
            return std::get<array_t>(m_value).begin();
        }
        return std::begin(std::get<string_t>(m_value));
    }
    
    auto end() {
        if (is_object()) {
            return std::get<object_t>(m_value).end();
        }
        if (is_array()) {
            return std::get<array_t>(m_value).end();
        }
        return std::end(std::get<string_t>(m_value));
    }
    
    // Operator[]
    json& operator[](const std::string& key) {
        if (!is_object()) {
            m_value = object_t();
        }
        return std::get<object_t>(m_value)[key];
    }
    
    const json& operator[](const std::string& key) const {
        if (is_object()) {
            return std::get<object_t>(m_value).at(key);
        }
        throw std::out_of_range("json object does not contain key");
    }
    
    // Serialization
    std::string dump() const {
        std::string result;
        dump(result);
        return result;
    }
    
    void dump(std::string& result) const {
        std::ostringstream stream;
        dump(stream);
        result = stream.str();
    }
    
    void dump(std::ostream& stream) const {
        if (is_null()) {
            stream << "null";
        } else if (is_boolean()) {
            stream << (std::get<boolean_t>(m_value) ? "true" : "false");
        } else if (is_number_integer()) {
            stream << std::get<number_integer_t>(m_value);
        } else if (std::holds_alternative<number_float_t>(m_value)) {
            stream << std::get<number_float_t>(m_value);
        } else if (is_string()) {
            stream << "\"" << std::get<string_t>(m_value) << "\"";
        } else if (is_array()) {
            stream << "[";
            const auto& arr = std::get<array_t>(m_value);
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) stream << ", ";
                arr[i].dump(stream);
            }
            stream << "]";
        } else if (is_object()) {
            stream << "{";
            const auto& obj = std::get<object_t>(m_value);
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                if (it != obj.begin()) stream << ", ";
                stream << "\"" << it->first << "\": ";
                it->second.dump(stream);
            }
            stream << "}";
        }
    }
};

} // namespace nlohmann

#endif // NLOHMANN_JSON_HPP
