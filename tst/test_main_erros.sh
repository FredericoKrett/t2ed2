#!/bin/sh
set -eu

input_dir="test_main_erros_in"
output_dir="test_main_erros_out"
output_file="$input_dir/saida_como_arquivo"

cleanup() {
    rm -rf "$input_dir" "$output_dir"
}

expect_failure() {
    if "$@" >/dev/null 2>&1; then
        echo "erro: comando deveria falhar: $*" >&2
        exit 1
    fi
}

cleanup
trap cleanup EXIT INT TERM

mkdir "$input_dir" "$output_dir"

cat > "$input_dir/caso.geo" <<EOF
cq 1 white black
q cep1 0 0 10 10
EOF

: > "$output_file"

expect_failure ./ted -e "$input_dir" -f ausente.geo -o "$output_dir"
expect_failure ./ted -e "$input_dir" -f caso.geo
expect_failure ./ted -e "$input_dir" -f caso.geo -v ausente.via \
    -o "$output_dir"
expect_failure ./ted -e "$input_dir" -f caso.geo -q ausente.qry \
    -o "$output_dir"
expect_failure ./ted -e "$input_dir" -f caso.geo -o "$output_file"

if find "$output_dir" -type f | grep -q .; then
    echo "erro: execucao com falha deixou arquivos de saida" >&2
    find "$output_dir" -type f >&2
    exit 1
fi
