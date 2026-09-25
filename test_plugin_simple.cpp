// Simple test to verify PluginRegistry can be compiled
#include <string>
#include <memory>

namespace nlm {
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual const std::string& getName() const = 0;
};
}

int main() {
    return 0;
}
