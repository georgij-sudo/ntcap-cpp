#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

declare -A MODULE_DIRS=(
    [core]="core"
    [recipes]="recipes"
    [rendering]="rendering"
    [converters]="converters"
    [shell]="shell"
)

EXCLUDED_DIRS=("patterns" "tests" "build")

usage() {
    cat <<EOF
Использование:
  $(basename "$0") [-h|--headers | -c|--cpp | -a|--all] [module|ClassName] ...

Модули: ${!MODULE_DIRS[*]}
  core добавляется автоматически при указании любого модуля.
  Без аргументов копируется только core.

Аргумент с заглавной буквы — поиск файлов Name.* по проекту
(исключая patterns/, tests/, build/).

Нельзя смешивать модули и файловые паттерны в одном вызове.

Флаги (по умолчанию -a):
  -h, --headers    Только .h
  -c, --cpp        Только .cpp и CMakeLists.txt
  -a, --all        .h, .cpp и CMakeLists.txt (по умолчанию)

Примеры:
  $(basename "$0")                 # только core (-a)
  $(basename "$0") core            # только core (-a)
  $(basename "$0") rendering       # core + rendering (-a)
  $(basename "$0") -h rendering    # core + rendering (только .h)
  $(basename "$0") Automata        # файлы Automata.h / Automata.cpp
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

build_find_expr() {
    local flag="$1"
    case "$flag" in
        -h|--headers)
            echo '-name' '*.h'
            ;;
        -c|--cpp)
            echo '-name' '*.cpp' '-o' '-name' 'CMakeLists.txt'
            ;;
        -a|--all)
            echo '-name' '*.h' '-o' '-name' '*.cpp' '-o' '-name' 'CMakeLists.txt'
            ;;
    esac
}

build_exclude_args() {
    local args=()
    for d in "${EXCLUDED_DIRS[@]}"; do
        args+=(-not -path "*/${d}/*")
    done
    echo "${args[@]}"
}

is_file_pattern() {
    [[ "$1" =~ ^[A-Z] ]]
}

flag="-a"
args=()

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--headers|-c|--cpp|-a|--all)
            flag="$1"
            shift
            ;;
        --help)
            usage
            exit 0
            ;;
        -*)
            error_exit "Неизвестный флаг: $1" 2
            ;;
        *)
            args+=("$1")
            shift
            ;;
    esac
done

modules=()
file_patterns=()

for a in "${args[@]}"; do
    if is_file_pattern "$a"; then
        file_patterns+=("$a")
    else
        modules+=("$a")
    fi
done

if [[ ${#modules[@]} -gt 0 && ${#file_patterns[@]} -gt 0 ]]; then
    error_exit "Ошибка: нельзя смешивать модули и файловые паттерны." 2
fi

for m in "${modules[@]}"; do
    if [[ -z "${MODULE_DIRS[$m]+x}" ]]; then
        error_exit "Ошибка: неизвестный модуль '$m'. Доступные: ${!MODULE_DIRS[*]}" 2
    fi
done

check_xclip

use_git_ignore=false
if is_git_repo; then
    use_git_ignore=true
fi

read -ra find_expr <<< "$(build_find_expr "$flag")"
read -ra exclude_args <<< "$(build_exclude_args)"

raw_files=()

if [[ ${#file_patterns[@]} -gt 0 ]]; then
    pattern_find_expr=()
    first_pattern=true
    for p in "${file_patterns[@]}"; do
        if [[ "$first_pattern" == true ]]; then
            first_pattern=false
        else
            pattern_find_expr+=(-o)
        fi
        case "$flag" in
            -h|--headers)
                pattern_find_expr+=(-name "${p}.h")
                ;;
            -c|--cpp)
                pattern_find_expr+=(-name "${p}.cpp")
                ;;
            -a|--all)
                pattern_find_expr+=(-name "${p}.h" -o -name "${p}.cpp")
                ;;
        esac
    done

    mapfile -d '' -t raw_files < <(
        find "$PROJECT_ROOT" -type f \( "${pattern_find_expr[@]}" \) \
            "${exclude_args[@]}" -print0 2>/dev/null
    )
else
    if [[ ${#modules[@]} -eq 0 ]]; then
        modules=("core")
    fi

    has_core=false
    for m in "${modules[@]}"; do
        if [[ "$m" == "core" ]]; then
            has_core=true
            break
        fi
    done
    if [[ "$has_core" == false ]]; then
        modules=("core" "${modules[@]}")
    fi

    for m in "${modules[@]}"; do
        dir="$PROJECT_ROOT/${MODULE_DIRS[$m]}"
        if [[ ! -d "$dir" ]]; then
            echo "Предупреждение: каталог '$dir' не найден, пропускаю модуль '$m'." >&2
            continue
        fi
        mapfile -d '' -t chunk < <(
            find "$dir" -type f \( "${find_expr[@]}" \) -print0 2>/dev/null
        )
        raw_files+=("${chunk[@]}")
    done
fi

if [[ ${#raw_files[@]} -eq 0 ]]; then
    error_exit "Файлы не найдены." 1
fi

mapfile -d '' -t sorted_files < <(
    printf '%s\0' "${raw_files[@]}" | sort -uz
)

if [[ "$use_git_ignore" == true ]]; then
    filter_git_ignored sorted_files files
else
    files=("${sorted_files[@]}")
fi

if [[ "${#files[@]}" -eq 0 ]]; then
    error_exit "Все найденные файлы исключены .gitignore." 1
fi

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

line_count=$(wc -l < "$TMPFILE")

echo "Скопировано ${#files[@]} файл(ов), ${line_count} строк:" >&2
for f in "${files[@]}"; do
    echo "  ${f#"$prefix"}" >&2
done

if ! xclip -selection clipboard < "$TMPFILE"; then
    error_exit "Ошибка: xclip завершился с ошибкой." 2
fi

exit 0