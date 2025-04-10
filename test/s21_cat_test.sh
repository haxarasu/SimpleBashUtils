#!/bin/bash

check_flag() {
    local flag=$1
    local file=$2
    ./../src/cat/s21_cat $flag $file > my_output.txt
    cat $flag $file > real_output.txt

    if diff -b my_output.txt real_output.txt > /dev/null; then
        echo "$flag : passed"
    else
        echo "$flag : failed"
    fi

    rm my_output.txt real_output.txt
}

file="./../src/cat/test_cat.txt"

flags=("-n" "-b" "-e" "-t" "-s" "-v")

for flag in "${flags[@]}"; do
    check_flag $flag $file
done

echo "Press any key to exit..."
read -n 1 -s
