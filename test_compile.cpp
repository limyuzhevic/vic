#include <memory>
#include <string>
// Minimal test to check compilation
int main() {
    std::shared_ptr<class Config> config;
    config->set("test", 1);
    return 0;
}
