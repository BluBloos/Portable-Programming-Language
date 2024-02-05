#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE="\033[0;36m"
NC='\033[0m' # No Color

has_help=0
has_verbose=0
unrecognized_args=()

while (( $# > 0 ))
do
    if [ $1 == "-h" ]; then
        has_help=1
    elif [ $1 == "-v" ]; then
        has_verbose=1
    else
        unrecognized_args=( ${unrecognized_args[@]} $1 )
    fi
    shift 
done

if (( ${#unrecognized_args[@]} > 0 )); then
    # NOTE: -e is require for handle ANSI colors.
    echo -e "${RED}Warning:${NC} ignoring invalid argument(s); ( ${unrecognized_args[@]} )\n"
fi

echo -e "${BLUE}Test short description${NC}"

cat << EOF
======================
This is a broad test that runs all tests associated with the lexer subcomponent
of the PPL compiler toolchain.

EOF

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
# wc is "word count" command. we're using it here to count lines.
git_status_lines=$(git status -s | wc -l)
limit=10
if [ "$git_status_lines" -gt "$limit" ]; then
    # head is a program for "display first lines of a file".
    git status -s | head -n "$limit"
    cat << EOF
... output truncated, $git_status_lines changes total ...

EOF
else
    git status -s
fi

# NOTE: line below may not work in all cases.
script_dir=$(cd "$(dirname "$0")"; pwd)

ppl_artefact_path="$script_dir/../../bin/ppl"

if [ -e "$ppl_artefact_path" ]; then
    echo -e "'bin/ppl' last modified time: $(stat -f %Sm "$ppl_artefact_path")\n"
else
    echo -e "${RED}Error:${NC}"
    cat << EOF
'bin/ppl' does not exist. Please compile the compiler toolchain before
running this test.

EOF
fi

if (($has_verbose)); then
    echo "Operating System Information:"
    sw_vers

    echo
    echo "Hardware Information:"
    system_profiler SPHardwareDataType

    echo
    echo "GPU Information:"
    system_profiler SPDisplaysDataType

    echo
    echo "Disk Drive Information:"
    system_profiler SPStorageDataType

    echo "Git Version:"
    git --version
    echo
fi

echo -e "${BLUE}Running test${NC}"

cmd="$ppl_artefact_path lall"

cat << EOF
======================
With command: '$cmd'

To "attach" the debugger, open the Xcode project. Ensure that the 'test_lexer'
scheme is selected.

BEGIN command printout:
======================
EOF

if $cmd; then
    echo -e "======================\nEND command printout"
    echo -e "\n${GREEN}test command completed successfully${NC}\n"
else
    echo -e "======================\nEND command printout"
    echo -e "\n${RED}test command failed${NC}\n"
fi

