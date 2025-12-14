#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

EXEC="./build/final"
TEST_DIR="./testcases"

# 编译
echo "编译..."
cmake -B build && make -C build || {
    echo -e "${RED}编译失败${NC}"
    exit 1
}

# 运行测试
passed=0
failed=0

for in_file in "$TEST_DIR"/*.in; do
    test_name=$(basename "$in_file" .in)
    out_file="$TEST_DIR/$test_name.out"

    if [ ! -f "$out_file" ]; then
        echo -e "${YELLOW}跳过 $test_name (无.out文件)${NC}"
        continue
    fi

    if diff -u "$out_file" <("$EXEC" < "$in_file" 2>&1) > /dev/null; then
        echo -e "${GREEN}✓ $test_name${NC}"
        ((passed++))
    else
        echo -e "${RED}✗ $test_name${NC}"
        ((failed++))
    fi
done

echo ""
echo "通过: $passed"
echo "失败: $failed"

[ $failed -eq 0 ] && exit 0 || exit 1