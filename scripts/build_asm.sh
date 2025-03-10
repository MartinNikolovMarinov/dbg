#!/bin/bash

# Determine the program name
if [ -n "$1" ]; then
    PROGRAM_NAME="$1"
elif [ -n "${PROGRAM_NAME}" ]; then
    echo "Using PROGRAM_NAME from environment: ${PROGRAM_NAME}"
else
    echo "Usage: $0 <program_name>"
    echo "Or set the PROGRAM_NAME environment variable."
    exit 1
fi

# Assemble the program using NASM
nasm -f elf64 -o "${PROGRAM_NAME}.o" "${PROGRAM_NAME}"
if [ $? -ne 0 ]; then
    echo "Assembly failed."
    exit 1
fi

# Link the object file to create the executable
ld -o "${PROGRAM_NAME}" "${PROGRAM_NAME}.o"
if [ $? -ne 0 ]; then
    echo "Linking failed."
    exit 1
fi

echo "Successfully created executable: ${PROGRAM_NAME}"
