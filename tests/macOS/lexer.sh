#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE="\033[0;36m"
NC='\033[0m' # No Color

echo -e "${BLUE}Test short description${NC}"

cat << EOF
======================
This test is a broad test that runs all tests associated with the lexer
subcomponent of the PPL compiler toolchain.

EOF

has_help=0
unrecognized_args=()

while (( $# > 0 ))
do
    if [ $1 == "-h" ]; then
        has_help=1
    else
        unrecognized_args=( ${unrecognized_args[@]} $1 )
    fi
    shift 
done

if (( ${#unrecognized_args[@]} > 0 )); then
    echo -e "${RED}Warning:${NC} ignoring invalid argument(s); ( ${unrecognized_args[@]} )\n"
fi

if (( has_help )); then
    echo -e "${BLUE}Help menu${NC}"
    cat << EOF
======================
The help menu is empty for now.

EOF
    exit
fi

echo -e "${BLUE}Current state${NC}"
echo "======================"
git log -1 --format="Git HEAD = '%h'"
git_status_lines=$(git status -s | wc -l)
limit=10
if [ "$git_status_lines" -gt "$limit" ]; then
    git status -s | head -n "$limit"
    echo "... output truncated, $git_status_lines changes total ..."
else
    git status -s
fi




