#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

using json = nlohmann::json;

namespace nlm {

class JsonParser {
public:
    static bool parseFile(const std::string& filepath, json& outJson) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            file >> outJson;
            return true;
        } catch (const json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            std::cerr << "Error reading JSON file: " << e.what() << std::endl;
            return false;
        }
    }
    
    static bool parseString(const std::string& content, json& outJson) {
        try {
            outJson = json::parse(content);
            return true;
        } catch (const json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing JSON string: " << e.what() << std::endl;
            return false;
        }
    }
    
    static std::string toString(const json& j) {
        return j.dump(2); // Pretty print with 2-space indentation
    }
    
    static bool mergeJson(json& base, const json& override) {
        if (!override.is_object()) return false;
        
        for (auto it = override.begin(); it != override.end(); ++it) {
            std::string key = it.key();
            
            if (base.contains(key)) {
                if (base[key].is_object() && it->is_object()) {
                    mergeJson(base[key], *it);
                } else {
                    base[key] = *it; // Override
                }
            } else {
                base[key] = *it; // Add new key
            }
        }
        return true;
    }
};

} // namespace nlm
