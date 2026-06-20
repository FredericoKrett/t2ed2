#!/bin/sh
set -eu

input_dir="test_main_com_qry_in"
output_dir="test_main_com_qry_out"

cleanup() {
    rm -rf "$input_dir" "$output_dir"
}

cleanup
trap cleanup EXIT INT TERM

mkdir "$input_dir" "$output_dir"

cat > "$input_dir/caso.geo" <<EOF
cq 1 white black
q cep1 0 0 20 20
EOF

cat > "$input_dir/caso-v.via" <<EOF
3
v v1 0 0
v v2 10 0
v v3 20 0
e v1 v2 - - 10 10 Rua_A
e v2 v3 - - 10 5 Rua_B
EOF

cat > "$input_dir/consulta.qry" <<EOF
@o? R0 cep1 S 0
@o? R1 cep1 S 20
mvm 2 0 -1 11 2
regs 3
exp 3
p? R0 R1 red blue
EOF

cat > "$input_dir/inacessivel.qry" <<EOF
@o? R0 cep1 S 0
@o? R1 cep1 S 20
p? R1 R0 green orange
EOF

./ted -e "$input_dir" -f caso.geo -v caso-v.via -q consulta.qry \
    -o "$output_dir"

svg="$output_dir/caso-consulta.svg"
txt="$output_dir/caso-consulta.txt"

if [ ! -s "$svg" ] || [ ! -s "$txt" ]; then
    echo "erro: saidas da consulta nao foram geradas" >&2
    exit 1
fi

for group in origens mvm regs exp percursos; do
    if ! grep -q "id=\"$group\"" "$svg"; then
        echo "erro: grupo $group ausente no svg" >&2
        exit 1
    fi
done

for report in '@o? R0 cep1 S' 'regs 3.000000' 'p? R0 R1' \
              'percurso mais curto' 'percurso mais rapido'; do
    if ! grep -q "$report" "$txt"; then
        echo "erro: trecho ausente no txt: $report" >&2
        exit 1
    fi
done

./ted -e "$input_dir" -f caso.geo -v caso-v.via -q inacessivel.qry \
    -o "$output_dir"

inacessivel_svg="$output_dir/caso-inacessivel.svg"
inacessivel_txt="$output_dir/caso-inacessivel.txt"

if [ ! -s "$inacessivel_svg" ] || [ ! -s "$inacessivel_txt" ]; then
    echo "erro: saidas do percurso inacessivel nao foram geradas" >&2
    exit 1
fi

inacessiveis=$(grep -c 'destino inacessivel' "$inacessivel_txt" || true)
if [ "$inacessiveis" -ne 2 ]; then
    echo "erro: percursos inacessiveis nao foram reportados" >&2
    exit 1
fi
