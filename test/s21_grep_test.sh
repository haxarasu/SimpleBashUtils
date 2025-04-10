#!/bin/bash

check_flag() {
    local flag=$1
    local pattern=$2
    local file=$3
    # Вызов вашей программы и реального grep
    ./../src/grep/s21_grep $flag "$pattern" $file > my_output.txt
    grep $flag "$pattern" $file > real_output.txt


    # Проверка результатов
    if diff -b my_output.txt real_output.txt > /dev/null; then
        echo "$flag : passed"
    else
        echo "$flag : failed"
    fi

    # Очистка временных файлов
    rm my_output.txt real_output.txt
}

file="./../src/grep/test_grep.txt"
patterns_file="./../src/grep/patterns.txt"
pattern="error"
flags=("-e" "-i" "-v" "-c" "-l" "-n" "-h" "-s" "-f" "-o")

for flag in "${flags[@]}"; do
    if [[ $flag == "-f" ]]; then
        # Для -f вместо строки передаём путь к файлу с паттернами
        check_flag $flag $patterns_file $file
    else
        check_flag $flag $pattern $file
    fi
done

echo "Press any key to exit..."
read -n 1 -s