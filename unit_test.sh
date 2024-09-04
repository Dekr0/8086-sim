#!/bin/bash

unit_test() {
    input_file="${1}"
    disassemble_file="disassemble/$(basename "${input_file}").asm"
    reassemble_file="reassemble/$(basename "${input_file}")"
    exec_ref_file="answer_unit_test/$(basename "${input_file}")"
    ./main -e -stdout "${input_file}" > "${disassemble_file}"
    nasm "${disassemble_file}" -o "${reassemble_file}"
    diff "${reassemble_file}" "${input_file}"
    diff "${disassemble_file}" "${exec_ref_file}.asm"
    echo "$(basename "${input_file}") pass"
}

if [ $# -eq 0 ]; then
    # No arguments, process all files in assemble directory
    for file in assemble/test_*; do
        unit_test "${file}"
    done
else
    # Arguments provided, process specified files
    for arg in "$@"; do
        # Pad the number with a leading zero if it's a single digit
        padded_num=$(printf "%02d" "$arg")
        file="assemble/test_$padded_num"
        if [ -f "${file}" ]; then
            unit_test "${file}"
        else
            echo "Warning: File $file does not exist"
        fi
    done
fi
