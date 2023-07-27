#!/usr/bin/env bash

# O script assume que esta num diretorio "raiz" com a seguinte estrutura:
# raiz
# -- Gabarito
#    -- bonus
#    -- normais
#    -- simples
#    -- extra-01
#    -- extra-02
#    -- extra-03
# -- Trabalhos
#    -- Fulano.c
#    -- Ciclano.c
# -- correcao.sh
# 
# e vai criar uma pasta chamada "Saidas", com os trabalhos compilados e todas as respectivas saidas
# tambem  vai criar uma planilha "notas.csv" na pasta raiz, com as notas de cada aluno

# Exemplo de execucao: bash correcao.sh

# O script tambem possui o parametro TIMEOUT, que define o tempo maximo de execucao de cada caso. O default eh 10 segundos

# Disable for loop on globs without match (expand to empty string)
shopt -s nullglob

GABARITO_DIR="Gabarito"
TRABALHOS_DIR="Trabalhos"
SAIDAS_DIR="Saidas"
TIMEOUT="20s"

if [ -d "$SAIDAS_DIR" ]; then
    n_saidas="$(ls -1 $SAIDAS_DIR | wc -l)" 
    [ "$n_saidas" -gt 0 ] && echo "Limpando saída" && rm -r $SAIDAS_DIR/*
else
    mkdir $SAIDAS_DIR
fi

ROOT_DIR="$PWD"
echo "Aluno,Inicializacao,RealizarJogo,Resumo,Ranking,Estatisticas,Trilha,Bonus,Extra 01,Extra 02, Extra 03,Total" > notas.csv
for codigo_aluno in $TRABALHOS_DIR/*; do
    aluno="$(echo $codigo_aluno | cut -d"/" -f2 | sed 's|.c$||')"
    echo "+++++ Corrigindo trabalho do aluno: $aluno"
    pasta_aluno="$SAIDAS_DIR/$aluno"
    # inicializa pasta do aluno
    mkdir "$pasta_aluno"
    cp "$codigo_aluno" "$pasta_aluno/main.c"
    cd "$pasta_aluno"
    # compila codigo
    gcc main.c -o trabalho || { echo "Erro de compilação: ${codigo_aluno}" ; exit 1 ; }
    # cria pasta de testes
    cp "$ROOT_DIR/$GABARITO_DIR" Testes -r
    rm Testes/*/*/saida/*

    # inicializacao das contagens de erros e acertos
    inicializacao_correto=0
    inicializacao_incorreto=0
    realizar_correto=0
    realizar_incorreto=0
    resumo_correto=0
    resumo_incorreto=0
    ranking_correto=0
    ranking_incorreto=0
    estatisticas_correto=0
    estatisticas_incorreto=0
    heatmap_correto=0
    heatmap_incorreto=0
    bonus_correto=0
    bonus_incorreto=0
    extra_01_correto=0
    extra_01_incorreto=0
    extra_02_correto=0
    extra_02_incorreto=0
    extra_03_correto=0
    extra_03_incorreto=0
    # executa casos de teste
    for CASE in $(find Testes -mindepth 2 -maxdepth 2 -type d | cut -d"/" -f2-); do
        # executa o trabalho com um tempo maximo definido em $TIMEOUT
        eh_tunnel=$(echo $CASE | grep bonus)
        if [[ -n "$eh_tunnel"  ]]; then
            echo "Caso bonus $CASE"
        else
            echo "Caso normal $CASE"
        fi
        timeout $TIMEOUT ./trabalho "Testes/$CASE" < "Testes/$CASE/jogadas.txt" > "Testes/$CASE/saida/saida.txt"

        # caso nao seja um caso bonus ou extra, avalia separadamente cada saida
        if (echo $CASE | grep simples) || (echo $CASE | grep normais); then
            # verificacao do Inicializar Jogo
            inicializar_jogo_diff=$(diff -b "Testes/$CASE/saida/inicializacao.txt" "$ROOT_DIR/Gabarito/$CASE/saida/inicializacao.txt")
            if [[ -f "Testes/$CASE/saida/inicializacao.txt" && $inicializar_jogo_diff == "" ]]; then
                let inicializacao_correto=$inicializacao_correto+1
            else
                echo "($aluno) Diferenca encontrada no caso $CASE: inicializacao.txt"
                let inicializacao_incorreto=$inicializacao_incorreto+1
            fi

            # verificacao do Realizar Jogo
            realizar_jogo_diff=$(diff -b "Testes/$CASE/saida/saida.txt" "$ROOT_DIR/Gabarito/$CASE/saida/saida.txt")
            if [[ -f "Testes/$CASE/saida/saida.txt" && $realizar_jogo_diff == "" ]]; then
                let realizar_correto=realizar_correto+1
            else
                echo "($aluno) Diferenca encontrada no caso $CASE: saida.txt"
                let realizar_incorreto=realizar_incorreto+1
            fi

            # verificacao de Gerar Resumo
            resumo_diff=$(diff -b "Testes/$CASE/saida/resumo.txt" "$ROOT_DIR/Gabarito/$CASE/saida/resumo.txt")
            if [[ -f "Testes/$CASE/saida/resumo.txt" && $resumo_diff == "" ]]; then
                let resumo_correto=resumo_correto+1
            else
                echo "($aluno) Diferenca encontrada no caso $CASE: resumo.txt"
                let resumo_incorreto=resumo_incorreto+1
            fi

            # verificacao de Gerar Ranking
            ranking_diff=$(diff -b "Testes/$CASE/saida/ranking.txt" "$ROOT_DIR/Gabarito/$CASE/saida/ranking.txt")
            if [[ -f "Testes/$CASE/saida/ranking.txt" && $ranking_diff == "" ]]; then
                let ranking_correto=ranking_correto+1
            else
                echo "($aluno) Diferenca encontrada no caso $CASE: ranking.txt"
                let ranking_incorreto=ranking_incorreto+1
            fi

            # verificacao de Gerar Estatisticas
            estatisticas_diff=$(diff -b "Testes/$CASE/saida/estatisticas.txt" "$ROOT_DIR/Gabarito/$CASE/saida/estatisticas.txt")
            if [[ -f "Testes/$CASE/saida/estatisticas.txt" && $estatisticas_diff == "" ]]; then
                let estatisticas_correto=estatisticas_correto+1
            else
                echo "($aluno) Diferenca encontrada no caso $CASE: estatisticas.txt"
                let estatisticas_incorreto=estatisticas_incorreto+1
            fi

            # verificacao de Gerar Heatmap
            heatmap_diff=$(diff -b "Testes/$CASE/saida/trilha.txt" "$ROOT_DIR/Gabarito/$CASE/saida/trilha.txt")
            if [[ -f "Testes/$CASE/saida/trilha.txt" && $heatmap_diff == "" ]]; then
                let heatmap_correto=heatmap_correto+1
            else
                echo "($aluno) Diferenca encontrada no caso $CASE: trilha.txt"
                let heatmap_incorreto=heatmap_incorreto+1
            fi
        elif (echo $CASE | grep bonus); then
            # verificacao dos casos bonus e extras (binaria, deve acertar todas as saidas)
            #diff=$(diff -rb Testes/$CASE/saida $ROOT_DIR/Gabarito/$CASE/saida)
			# Soh tem que acertar a saida
            diff=$(diff -b "Testes/$CASE/saida/saida.txt" "$ROOT_DIR/Gabarito/$CASE/saida/saida.txt")
            if [[ $diff == "" ]]; then
                let bonus_correto=$bonus_correto+1
            else
                echo "($aluno) Diferenca encontrada no caso $CASE"
                let bonus_incorreto=$bonus_incorreto+1
            fi
        elif (echo $CASE | grep extra-01); then
            # verificacao dos extra-01 (comida bonus)
			# Soh tem que acertar a saida
            diff=$(diff -b "Testes/$CASE/saida/saida.txt" "$ROOT_DIR/Gabarito/$CASE/saida/saida.txt")
            if [[ $diff == "" ]]; then
                let extra_01_correto=$extra_01_correto+1
            else
                echo "($aluno) Diferenca encontrada no caso $CASE"
                let extra_01_incorreto=$extra_01_incorreto+1
            fi
        elif (echo $CASE | grep extra-02); then
            # verificacao dos extra-02 (item que muda sprite)
			# Soh tem que acertar a saida
            diff=$(diff -b "Testes/$CASE/saida/saida.txt" "$ROOT_DIR/Gabarito/$CASE/saida/saida.txt")
            if [[ $diff == "" ]]; then
                let extra_02_correto=$extra_02_correto+1
            else
                echo "($aluno) Diferenca encontrada no caso $CASE"
                let extra_02_incorreto=$extra_02_incorreto+1
            fi
        elif (echo $CASE | grep extra-03); then
            # verificacao dos extra-03 (invencibilidade)
			# Soh tem que acertar a saida
            diff=$(diff -b "Testes/$CASE/saida/saida.txt" "$ROOT_DIR/Gabarito/$CASE/saida/saida.txt")
            if [[ $diff == "" ]]; then
                let extra_03_correto=$extra_03_correto+1
            else
                echo "($aluno) Diferenca encontrada no caso $CASE"
                let extra_03_incorreto=$extra_03_incorreto+1
            fi
        fi
    done

    # calculo das notas parciais
    nota_inicializacao=$(bc <<< "scale=2; 1 * ($inicializacao_correto/($inicializacao_correto + $inicializacao_incorreto))")
    nota_realizar=$(bc <<< "scale=2; 3 * ($realizar_correto/($realizar_correto + $realizar_incorreto))")
    nota_resumo=$(bc <<< "scale=2; 1 * ($resumo_correto/($resumo_correto + $resumo_incorreto))")
    nota_ranking=$(bc <<< "scale=2; 1 * ($ranking_correto/($ranking_correto + $ranking_incorreto))")
    nota_estatisticas=$(bc <<< "scale=2; 1 * ($estatisticas_correto/($estatisticas_correto + $estatisticas_incorreto))")
    nota_heatmap=$(bc <<< "scale=2; 1 * ($heatmap_correto/($heatmap_correto + $heatmap_incorreto))")
    nota_bonus=$(bc <<< "scale=2; 1 * ($bonus_correto/($bonus_correto + $bonus_incorreto))")
    nota_extra_01=$(bc <<< "scale=2; 1 * ($extra_01_correto/($extra_01_correto + $extra_01_incorreto))")
    nota_extra_02=$(bc <<< "scale=2; 1 * ($extra_02_correto/($extra_02_correto + $extra_02_incorreto))")
    nota_extra_03=$(bc <<< "scale=2; 1 * ($extra_03_correto/($extra_03_correto + $extra_03_incorreto))")

    # somatorio da nota final
    nota_total=$(bc <<< "scale=2; $nota_inicializacao+$nota_realizar+$nota_resumo+$nota_ranking+$nota_estatisticas+$nota_heatmap+$nota_bonus+$nota_extra_01+$nota_extra_02+$nota_extra_03")

    # adiciona uma linha no CSV
    echo "$aluno,$nota_inicializacao,$nota_realizar,$nota_resumo,$nota_ranking,$nota_estatisticas,$nota_heatmap,$nota_bonus,$nota_extra_01,$nota_extra_02,$nota_extra_03,$nota_total" >> "$ROOT_DIR/notas.csv"

    # volta para o diretorio raiz
    cd "$ROOT_DIR"
done
