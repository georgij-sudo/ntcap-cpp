#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<EOF
Использование:
  $(basename "$0") [-h|--headers | -cpp|--cpp] <directory>

Описание:
  -h, --headers    Рекурсивно копировать только файлы с расширением .h  (по умолчанию)
  -cpp, --cpp      Рекурсивно копировать файлы .h, .cpp и CMakeLists.txt

Поведение:
  - Перед содержимым каждого файла добавляется строка-комментарий с относительным путём
    файла от переданного каталога, например: // src/MyClass.h
  - Между файлами добавляется пустая строка.
  - Итоговый буфер отправляется в системный буфер обмена через xclip (selection clipboard).
  - Если совпадений нет — скрипт завершится с кодом 1.
  - В случае ошибок окружения (например, xclip отсутствует) — код 2.
EOF
}

error_exit() {
    echo "$1" >&2
    exit "${2:-1}"
}

check_xclip() {
    if ! command -v xclip >/dev/null 2>&1; then
        error_exit "Ошибка: xclip не найден в PATH. Установите xclip." 2
    fi
}

normalize_dir() {
    local d="$1"
    # Удалить завершающий слэш, если есть
    printf '%s' "${d%/}"
}

if [[ $# -lt 1 ]]; then
    usage
    exit 2
fi

flag=""
dir_arg=""

if [[ "$1" == -* ]]; then
    flag="$1"
    if [[ $# -lt 2 ]]; then
        usage
        exit 2
    fi
    dir_arg="$2"
else
    flag="-h"
    dir_arg="$1"
fi

case "$flag" in
    -h|--headers)
        find_expr=( -name '*.h' )
        ;;
    -cpp|--cpp)
        find_expr=( -name '*.h' -o -name '*.cpp' -o -name 'CMakeLists.txt' )
        ;;
    *)
        echo "Неизвестный флаг: $flag" >&2
        usage
        exit 2
        ;;
esac

if [[ ! -d "$dir_arg" ]]; then
    error_exit "Ошибка: каталог '$dir_arg' не существует или не является каталогом." 2
fi

dir=$(normalize_dir "$dir_arg")

check_xclip

TMPFILE=$(mktemp)
cleanup() {
    rm -f -- "$TMPFILE"
}
trap cleanup EXIT

# Собираем список файлов (безопасно для пробелов и спецсимволов)
# Сортировка делает поведение детерминированным, но порядок не критичен.
mapfile -d '' -t files < <(find "$dir" -type f \( "${find_expr[@]}" \) -print0 2>/dev/null | sort -z)

if [[ "${#files[@]}" -eq 0 ]]; then
    echo "Файлы не найдены по заданному шаблону в каталоге '$dir'." >&2
    exit 1
fi

first=true
for f in "${files[@]}"; do
    # Относительный путь от переданного каталога
    rel="${f#$dir/}"
    if [[ "$rel" == "$f" ]]; then
        rel="$f"
    fi

    if [[ "$first" == true ]]; then
        first=false
    else
        printf '\n' >> "$TMPFILE"
    fi

    printf '// %s\n' "$rel" >> "$TMPFILE"
    cat -- "$f" >> "$TMPFILE"
done

# Копируем в буфер обмена (selection clipboard)
if ! xclip -selection clipboard < "$TMPFILE"; then
    echo "Ошибка: xclip завершился с ошибкой." >&2
    exit 2
fi

exit 0