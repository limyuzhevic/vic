#include <bits/stl_list.h>
EOF && cat > test.cpp << 'EOF'
int main() { return 0; }
EOF && g++ -c test.cpp -I/usr/include 2>&1 | head -20
