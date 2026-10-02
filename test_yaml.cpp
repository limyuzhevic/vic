#include <yaml-cpp/yaml.h>
#include <string>
int main() {
    YAML::Node node;
    node["test"] = std::string("hello");
    return 0;
}
