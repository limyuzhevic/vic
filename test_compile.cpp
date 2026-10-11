#include <iostream>
#include <sstream>
#include <vector>
#include <string>

int main() {
    std::ostringstream oss;
    std::vector<std::string> vec = {"test", "vector"};
    oss << "vector: [";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << vec[i];
    }
    oss << "]";
    std::cout << oss.str() << std::endl;
    return 0;
}
