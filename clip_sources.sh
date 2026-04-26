#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

usage() {
    cat <<EOF
Использование:
  $(basename "$0") [-h|--headers | -c|--cpp | -a|--all] <dir1> [dir2] ...

Описание:
  -h, --headers    Рекурсивно копировать только файлы .h            (по умолчанию)
  -c, --cpp        Рекурсивно копировать файлы .cpp и CMakeLists.txt
  -a, --all        Рекурсивно копировать файлы .h, .cpp и CMakeLists.txt

Примеры:
  $(basename "$0") -h gui/ lib/
  $(basename "$0") --all gui/ lib/ tests/
  $(basename "$0") lib/
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

is_git_repo() {
    command -v git >/dev/null 2>&1 \
        && git -C "$PROJECT_ROOT" rev-parse --is-inside-work-tree >/dev/null 2>&1
}

filter_git_ignored() {
    local -n _input=$1
    local -n _output=$2
    _output=()

    if [[ ${#_input[@]} -eq 0 ]]; then
        return
    fi

    local -A ignored_set=()
    local ignored_line

    while IFS= read -r -d '' ignored_line; do
        ignored_set["$ignored_line"]=1
    done < <(
        printf '%s\0' "${_input[@]}" \
            | git -C "$PROJECT_ROOT" check-ignore --stdin -z 2>/dev/null \
            || true
    )

    for f in "${_input[@]}"; do
        if [[ -z "${ignored_set[$f]+x}" ]]; then
            _output+=("$f")
        fi
    done
}

# ─── Разбор аргументов ───
if [[ $# -lt 1 ]]; then
    usage
    exit 2
fi

flag=""
dirs=()

if [[ "$1" == -* ]]; then
    flag="$1"
    shift
else
    flag="-h"
fi

if [[ $# -lt 1 ]]; then
    echo "Ошибка: не указана ни одна директория." >&2
    usage
    exit 2
fi

while [[ $# -gt 0 ]]; do
    dirs+=("$1")
    shift
done

case "$flag" in
    -h|--headers)
        find_expr=( -name '*.h' )
        ;;
    -c|--cpp)
        find_expr=( -name '*.cpp' -o -name 'CMakeLists.txt' )
        ;;
    -a|--all)
        find_expr=( -name '*.h' -o -name '*.cpp' -o -name 'CMakeLists.txt' )
        ;;
    *)
        echo "Неизвестный флаг: $flag" >&2
        usage
        exit 2
        ;;
esac

for d in "${dirs[@]}"; do
    if [[ ! -d "$d" ]]; then
        error_exit "Ошибка: каталог '$d' не существует или не является каталогом." 2
    fi
done

check_xclip

use_git_ignore=false
if is_git_repo; then
    use_git_ignore=true
fi

# ─── Преобразуем директории в абсолютные пути ОДИН раз ───
abs_dirs=()
for d in "${dirs[@]}"; do
    abs_dirs+=("$(cd "$d" && pwd)")
done

# ─── Сбор файлов (find по абсолютным путям → результат тоже абсолютный) ───
raw_files=()
for dir in "${abs_dirs[@]}"; do
    if [[ "$use_git_ignore" == true ]]; then
        mapfile -d '' -t chunk < <(
            find "$dir" -type f \( "${find_expr[@]}" \) -print0 2>/dev/null
        )
    else
        mapfile -d '' -t chunk < <(
            find "$dir" -type f \( "${find_expr[@]}" \) \
                -not -path '*/build/*' \
                -print0 2>/dev/null
        )
    fi
    raw_files+=("${chunk[@]}")
done

mapfile -d '' -t sorted_files < <(
    printf '%s\0' "${raw_files[@]}" | sort -z
)

if [[ "$use_git_ignore" == true ]]; then
    filter_git_ignored sorted_files files
else
    files=("${sorted_files[@]}")
fi

if [[ "${#files[@]}" -eq 0 ]]; then
    echo "Файлы не найдены по заданному шаблону в каталогах: ${dirs[*]}" >&2
    exit 1
fi

# ─── Формирование буфера: ноль subshell в цикле, один open на файл ───
TMPFILE=$(mktemp)
cleanup() { rm -f -- "$TMPFILE"; }
trap cleanup EXIT

prefix="$PROJECT_ROOT/"

{
    first=true
    for f in "${files[@]}"; do
        rel="${f#"$prefix"}"

        if [[ "$first" == true ]]; then
            first=false
        else
            printf '\n'
        fi

        printf '// %s\n' "$rel"
        cat -- "$f"
    done
} > "$TMPFILE"

if ! xclip -selection clipboard < "$TMPFILE"; then
    error_exit "Ошибка: xclip завершился с ошибкой." 2
fi

exit 0