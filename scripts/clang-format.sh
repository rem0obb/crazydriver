#!/bin/bash

# Colors
GREEN='\033[0;32m'
NC='\033[0m' # No color

# Directories to be formatted
directories=("sources" "include")

# Run clang-format for each directory
for dir in "${directories[@]}"; do
  echo -e "[${GREEN}*${NC}] Formatting files in ${dir}..."
  find "$dir" -name "*.c" -o -name "*.h" | xargs clang-format --verbose -i
done

echo -e "${GREEN}Formatting completed.${NC}"
