#!/bin/bash
# Read the file
cp src/core/Config/Config.cpp src/core/Config/Config.cpp.bak

# Replace the summary() function
sed -i 's/std::string Config::summary() const {\n    std::ostringstream oss;\n    oss << "Configuration (" << pImpl->entries.size() << " entries):\\n";\n    for (const auto& entry : pImpl->entries) {\n        oss << "  " << entry.key << " = [";\n        std::visit([&oss](auto&& arg) {\n            oss << arg;\n        }, entry.value);\n        oss << "] (" << static_cast<int>(entry.source) << ")\\n";\n    }\n    return oss.str();/std::string Config::summary() const {\n    std::ostringstream oss;\n    oss << "Configuration (" << pImpl->entries.size() << " entries):\\n";\n    for (const auto& entry : pImpl->entries) {\n        oss << "  " << entry.key << " = [";\n        std::visit([&oss](auto&& arg) {\n            printToStream(oss, arg);\n        }, entry.value);\n        oss << "] (" << static_cast<int>(entry.source) << ")\\n";\n    }\n    return oss.str();/' src/core/Config/Config.cpp

# Add the helper functions before the summary() function
awk '/void printToStream\(std::ostringstream\& oss, const std::vector</{print "void printToStream(std::ostringstream& oss, const std::string& value) {"} {print}' src/core/Config/Config.cpp > /tmp/temp.cpp
cp /tmp/temp.cpp src/core/Config/Config.cpp
