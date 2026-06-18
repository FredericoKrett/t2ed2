#!/bin/sh
set -eu

input_dir="test_main_sem_qry_in"
output_dir="test_main_sem_qry_out"

cleanup() {
    rm -rf "$input_dir" "$output_dir"
}

cleanup
trap cleanup EXIT INT TERM

mkdir "$input_dir" "$output_dir"

cat > "$input_dir/caso.geo" <<EOF
cq 1 white black
q cep1 0 0 10 10
EOF

cat > "$input_dir/caso-v.via" <<EOF
2
v v1 0 0
v v2 10 0
e v1 v2 - - 10 1 Rua_A
EOF

./ted -e "$input_dir" -f caso.geo -v caso-v.via -o "$output_dir"

if [ ! -f "$output_dir/caso-v.svg" ]; then
    echo "erro: svg base nao foi gerado" >&2
    exit 1
fi

if find "$output_dir" -name '*.txt' | grep -q .; then
    echo "erro: txt foi gerado sem arquivo qry" >&2
    find "$output_dir" -name '*.txt' >&2
    exit 1
fi
