#include <stdio.h>
#include <stdlib.h>
#define MAX_LINHAS 100
#define MAX_COLUNAS 100 // padrao teste: ./trab /home/emi/Documentos/trabalho/2023/Casos-v1/Gabarito/simples/03 </home/emi/Documentos/trabalho/2023/Casos-v1/Gabarito/simples/03/jogadas.txt
// ESTRUTURAS                            ./trab CAMINHO DIRETORIO <CAMINHO ARQUIVO DE JOGADAS
typedef struct
{
    int linha;
    int coluna;
    char simbolo;
    char direcao;
} tElemento;
typedef struct
{
    char tipo;
    int qtdComida;
    int bateuParede;
    int qtd;
} tMovimentos;
typedef struct
{
    int movSemPont;
    int paredeAtin;
    int capComidas;
    int comidaEspecial;
    int movEsquerda;
    int movDireita;
    int movCima;
    int movBaixo;
} tEstatisticas;
typedef struct
{
    tElemento jogador;
    tElemento fantasmas[4]; // 0 = B, 1 = I, 2 = C, 3 = P (FANTASMAS E SUAS POSICOES)
    tEstatisticas dados;
    int mapaL;
    int mapaC;
    int qtdMov;
    int limiteMov;
    int comidaTotal;
    int movAnterior;
    int comidaAlternativa;
    tMovimentos movimentos[4]; // 0 = w, 1 = s, 3 = d, 4 = a (TIPOS DE MOVIMENTOS E SUAS POSICOES)
    int trilha[MAX_LINHAS][MAX_COLUNAS];
    char mapa[MAX_LINHAS][MAX_COLUNAS];
    char backUpMapa[MAX_LINHAS][MAX_COLUNAS];
} tJogo;
// DECLARAÇÃO DAS FUNÇÕES
tJogo inicializaVariaveis();
tJogo inicializaJogo(char arquivoMapa[], char *argv[]);
tJogo verificaElemntosJogo(int i, int j, char c, tJogo jogo);
tElemento mudaDadosPersonagem(int i, int j, char c);
void gerarArquivoInicializacao(tJogo jogo, char *argv[]);
void jogaJogo(tJogo jogo, char *argv[]);
tJogo realizaJogadas(tJogo jogo, char jogada, char *argv[]);
tJogo verificaColisao(tJogo jogo, int i, int j, char jogada);
int ehFantasma(char c);
int perdeuJogo(tJogo jogo, char *argv[]);
tJogo imprimeSaida(tJogo jogo, char jogadas, char *argv[]);
tJogo movimentaFantasma(tJogo jogo, char c, int n);
tJogo mudaMatriz(tJogo jogo);
void gerarArquivoEstatisticas(tJogo jogo, char *argv[]);
tJogo criaTrilha(tJogo jogo, int i, int j);
void gerarArquivoTrilha(tJogo jogo, char *argv[]);
tJogo inicializaDadosMovimentos(tJogo jogo);
void ImprimindoArquivoResumoJogadas(tJogo jogo, char *argv[], char jogada);
tJogo atulizaRanking(char jogada, tJogo jogo, int acao);
tJogo ordenaRanking(tJogo jogo);
void imprimindoRanking(tJogo jogo, char *argv[]);
tJogo qtdCadaMovemento(tJogo jogo);
// MAIN
int main(int argc, char *argv[])
{
    char arquivoMapa[1001];

    if (argv[1] == NULL) // SE O DIRETORIO NÃO FOR INFORMADO
    {
        printf("ERRO:  O  diretorio  de  arquivos  de configuracao nao foi informado\n");
        return 0;
    }
    sprintf(arquivoMapa, "%s/mapa.txt", argv[1]); // LE O CAMINHO DO ARQUIVO DO MAPA

    tJogo jogo = inicializaJogo(arquivoMapa, argv);
    jogaJogo(jogo, argv);

    return 0;
}
// FUNÇÕES
tJogo inicializaVariaveis()
{
    tJogo jogo = {.comidaTotal = 0, .comidaAlternativa = 0, .qtdMov = 0, .fantasmas[0].simbolo = '.', .fantasmas[1].simbolo = '.', .fantasmas[2].simbolo = '.', .fantasmas[3].simbolo = '.'};
    jogo.dados.capComidas = jogo.dados.movSemPont = jogo.dados.comidaEspecial = jogo.dados.movBaixo = jogo.dados.movCima = jogo.dados.movDireita = jogo.dados.movEsquerda = jogo.dados.paredeAtin = 0;
    jogo = inicializaDadosMovimentos(jogo);
    return jogo;
}
tJogo inicializaJogo(char arquivoMapa[], char *argv[]) // INICIALIZAR VARIAVEIS E GERA O ARQUIVO INICIAL
{
    FILE *lerMapa;

    lerMapa = fopen(arquivoMapa, "r");
    if (lerMapa == NULL) // SE NÃO EXISTIR ARQUIVO (mapa.txt)
    {
        printf("Nao foi possivel realizar a leitura do arquivo: (%s)\n", arquivoMapa);
        exit(1);
    }
    tJogo jogo = inicializaVariaveis();
    int i, j;

    // LENDO O TAMANHO DO MAPA E QTD MOVIMENTOS
    fscanf(lerMapa, "%d %d %d\n", &jogo.mapaL, &jogo.mapaC, &jogo.limiteMov);
    for (i = 0; i < jogo.mapaL; i++) // LENDO MAPA E CONTANDO ELEMENTOS
    {
        for (j = 0; j < jogo.mapaC; j++)
        {
            fscanf(lerMapa, "%c", &jogo.mapa[i][j]);
            jogo.backUpMapa[i][j] = jogo.mapa[i][j];
            if (jogo.mapa[i][j] != ' ' && jogo.mapa[i][j] != '#')
                jogo = verificaElemntosJogo(i, j, jogo.mapa[i][j], jogo);
            jogo.trilha[i][j] = -1;
        }
        fscanf(lerMapa, "%*c");
    }
    fclose(lerMapa);
    gerarArquivoInicializacao(jogo, argv);

    return jogo;
}
tJogo inicializaDadosMovimentos(tJogo jogo)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        jogo.movimentos[i].qtdComida = jogo.movimentos[i].bateuParede = 0;
    }
    jogo.movimentos[0].tipo = 'w';
    jogo.movimentos[1].tipo = 's';
    jogo.movimentos[2].tipo = 'd';
    jogo.movimentos[3].tipo = 'a';
    return jogo;
}
tJogo verificaElemntosJogo(int i, int j, char c, tJogo jogo) // REGISTRA A POSICAO E CARACTERISTICAS DOS ELEMENTOS DO JOGO
{
    if (c == '*')
        jogo.comidaTotal++;
    else if (c == '>')
        jogo.jogador = mudaDadosPersonagem(i, j, c);
    else if (c == 'B')
    {
        jogo.fantasmas[0] = mudaDadosPersonagem(i, j, c);
        jogo.fantasmas[0].direcao = 'a';
    }
    else if (c == 'I')
    {
        jogo.fantasmas[1] = mudaDadosPersonagem(i, j, c);
        jogo.fantasmas[1].direcao = 's';
    }
    else if (c == 'C')
    {
        jogo.fantasmas[2] = mudaDadosPersonagem(i, j, c);
        jogo.fantasmas[2].direcao = 'd';
    }
    else if (c == 'P')
    {
        jogo.fantasmas[3] = mudaDadosPersonagem(i, j, c);
        jogo.fantasmas[3].direcao = 'w';
    }
    return jogo;
}
tElemento mudaDadosPersonagem(int i, int j, char c)
{
    tElemento personagem;
    personagem.coluna = j;
    personagem.linha = i;
    personagem.simbolo = c;

    return personagem;
}
int conferePosicao(int i, int j, int l, int c) // VERIFICA QUE DOIS ELEMENTOS ENTAO NO MESMO LUGAR
{
    if (i == l && c == j)
        return 1;
    return 0;
}
tJogo mudaMatriz(tJogo jogo) // ARRUMA A MATRIZ COMFORME A POSICAO DOS ELEMNTOS DEPOIS DE SE MOVEREM
{
    int i, j, v, n;
    for (i = 0; i < jogo.mapaL; i++)
    {
        for (j = 0; j < jogo.mapaC; j++)
        {
            if (jogo.mapa[i][j] != '#')
            {
                n = 0;                  // SERVE PARA VER SE HOUVE AGUMA MUDANÇA NAQUELA PARTE DO MAPA
                for (v = 0; v < 4; v++) // MUDA O SIMBOLO DO JOGADOR SE TIVER COLIDIDO COM FANTASMA
                {
                    if (ehFantasma(jogo.fantasmas[v].simbolo))
                    {
                        if (conferePosicao(i, j, jogo.fantasmas[v].linha, jogo.fantasmas[v].coluna))
                        {
                            jogo.mapa[i][j] = jogo.fantasmas[v].simbolo;
                            n++;
                            if (conferePosicao(jogo.fantasmas[v].linha, jogo.fantasmas[v].coluna, jogo.jogador.linha, jogo.jogador.coluna))
                                jogo.jogador.simbolo = jogo.fantasmas[v].simbolo;
                        }
                    }
                }
                if (conferePosicao(i, j, jogo.jogador.linha, jogo.jogador.coluna))
                {
                    jogo.mapa[i][j] = jogo.jogador.simbolo;
                    n++;
                }
                if ((jogo.backUpMapa[i][j] == '*' || jogo.backUpMapa[i][j] == '%' || jogo.backUpMapa[i][j] == '!') && n == 0)
                {
                    if (jogo.backUpMapa[i][j] == '%')
                        jogo.mapa[i][j] = '%';
                    else if (jogo.backUpMapa[i][j] == '*')
                        jogo.mapa[i][j] = '*';
                    else
                        jogo.mapa[i][j] = '!';

                    n++;
                }
                if (n == 0)
                    jogo.mapa[i][j] = ' ';
            }
        }
    }
    return jogo;
}
void gerarArquivoInicializacao(tJogo jogo, char *argv[]) // ESCREVE O MAPA NO ARQUIVO
{
    FILE *arquivoInicializacao;
    char nomeArquivo[1000];
    int i, j;

    sprintf(nomeArquivo, "%s/saida/inicializacao.txt", argv[1]);
    arquivoInicializacao = fopen(nomeArquivo, "w");

    for (i = 0; i < jogo.mapaL; i++)
    {
        for (j = 0; j < jogo.mapaC; j++)
        {
            fprintf(arquivoInicializacao, "%c", jogo.mapa[i][j]);
        }
        fprintf(arquivoInicializacao, "\n");
    }
    // POSIÇAO INICIAL Pac-Man +1, NAO COMEÇA DO 0 MAS SIM 1
    fprintf(arquivoInicializacao, "Pac-Man comecara o jogo na linha %d e coluna %d\n", jogo.jogador.linha + 1, jogo.jogador.coluna + 1);

    fclose(arquivoInicializacao);
}
void jogaJogo(tJogo jogo, char *argv[]) // LE ARQUIVO jogadas.txt E TERMINA O JOGO ###
{
    char jogadas;

    while (scanf("%c\n", &jogadas) != EOF)
    {
        int i, v = 0, j;
        for (i = 0; i < 4; i++)
        {
            if (jogo.fantasmas[i].simbolo != '.')
            {
                jogo = movimentaFantasma(jogo, jogo.fantasmas[i].simbolo, i); // B, C, I, P
            }
        }
        jogo = realizaJogadas(jogo, jogadas, argv);
        jogo = mudaMatriz(jogo);
        jogo.qtdMov++;
        jogo.comidaAlternativa--;
        if (jogo.comidaAlternativa < 0)
            jogo.comidaAlternativa = 0;
        if (jogo.movAnterior == 2 || jogo.movAnterior == 3 || jogo.movAnterior == 1)
            ImprimindoArquivoResumoJogadas(jogo, argv, jogadas);
        imprimeSaida(jogo, jogadas, argv);
        if (jogo.comidaTotal == jogo.dados.capComidas || perdeuJogo(jogo, argv))
        {
            if (jogo.comidaTotal != jogo.dados.capComidas && perdeuJogo(jogo, argv) && jogo.movAnterior != 2 && jogo.qtdMov != jogo.limiteMov)
            {
                jogo.movAnterior = 2;
                ImprimindoArquivoResumoJogadas(jogo, argv, jogadas);
            }
            jogo = criaTrilha(jogo, jogo.jogador.linha, jogo.jogador.coluna);
            gerarArquivoEstatisticas(jogo, argv);
            gerarArquivoTrilha(jogo, argv);
            imprimindoRanking(jogo, argv);
            break;
        }
    }
}
tJogo realizaJogadas(tJogo jogo, char jogada, char *argv[]) // CALCULA O DESLOCAMENTO
{
    int i = jogo.jogador.linha;
    int j = jogo.jogador.coluna;

    jogo = criaTrilha(jogo, jogo.jogador.linha, jogo.jogador.coluna);
    if (jogada == 'a') // ESQUERDA
    {
        jogo.dados.movEsquerda++;
        j--;
    }
    else if (jogada == 'd') // DIREITA
    {
        jogo.dados.movDireita++;
        j++;
    }
    else if (jogada == 's') // BAIXO
    {
        jogo.dados.movBaixo++;
        i++;
    }
    else if (jogada == 'w') // CIMA
    {
        jogo.dados.movCima++;
        i--;
    }
    jogo = verificaColisao(jogo, i, j, jogada);
    return jogo;
}
tJogo verificaColisao(tJogo jogo, int i, int j, char jogada)
{
    int acao = 0;
    if (j >= 0 && j < jogo.mapaC && i >= 0 && i < jogo.mapaL)
    {
        if (jogo.mapa[i][j] == ' ')
        {
            jogo.jogador = mudaDadosPersonagem(i, j, '>');
            jogo.mapa[i][j] = '>';
            jogo.dados.movSemPont++;
        }
        else if (jogo.mapa[i][j] == '#') // COLIDOU COM A PAREDE
        {
            jogo.dados.paredeAtin++;
            jogo.dados.movSemPont++;
            acao = 1;
        }
        else if (ehFantasma(jogo.mapa[i][j])) // COLIDIU COM FANTASMAS
        {
            int v, k = 0;
            for (v = 0; v < 4; v++)
            {
                if (conferePosicao(i, j, jogo.fantasmas[v].linha, jogo.fantasmas[v].coluna))
                {

                    jogo.jogador.simbolo = jogo.mapa[i][j];
                    acao = 2;
                }
            }
            jogo.jogador = mudaDadosPersonagem(i, j, jogo.jogador.simbolo);
            jogo.dados.movSemPont++;
        }
        else if (jogo.backUpMapa[i][j] == '*' || jogo.backUpMapa[i][j] == '%' || jogo.backUpMapa[i][j] == '!') // COMEU COMIDA
        {
            int v;
            for (v = 0; v < 4; v++)
            {
                if (conferePosicao(i, j, jogo.fantasmas[v].linha, jogo.fantasmas[v].coluna))
                {
                    jogo.jogador = mudaDadosPersonagem(i, j, jogo.mapa[i][j]);
                    acao = jogo.movAnterior = 2;
                    jogo.backUpMapa[i][j] = ' ';
                    jogo.dados.movSemPont++;
                    jogo = atulizaRanking(jogada, jogo, acao);
                    return jogo;
                }
            }
            if (jogo.backUpMapa[i][j] == '!')
            {
                jogo.comidaAlternativa = jogo.comidaAlternativa + 15;
                jogo.jogador = mudaDadosPersonagem(i, j, '&');
                jogo.mapa[i][j] = '&';
            }
            else
            {
                if (jogo.backUpMapa[i][j] == '*')
                    jogo.dados.capComidas++;
                else if (jogo.backUpMapa[i][j] == '%')
                    jogo.dados.comidaEspecial += 5;
                if (jogo.comidaAlternativa > 0)
                {
                    jogo.jogador = mudaDadosPersonagem(i, j, '&');
                    jogo.mapa[i][j] = '&';
                }
                else
                {
                    jogo.jogador = mudaDadosPersonagem(i, j, '>');
                    jogo.mapa[i][j] = '>';
                }
            }
            jogo.backUpMapa[i][j] = ' ';
            acao = 3;
        }
        if (jogo.comidaAlternativa > 0)
        {
            jogo.jogador = mudaDadosPersonagem(i, j, '&');
            jogo.mapa[i][j] = '&';
        }

        jogo = atulizaRanking(jogada, jogo, acao);
        jogo.movAnterior = acao;
    }
    return jogo;
}
tJogo atulizaRanking(char jogada, tJogo jogo, int acao)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if (jogada == jogo.movimentos[i].tipo)
        {
            if (acao == 1)
                jogo.movimentos[i].bateuParede++;
            else if (acao == 3)
                jogo.movimentos[i].qtdComida++;
        }
    }
    return jogo;
}
tJogo criaTrilha(tJogo jogo, int i, int j)
{
    if (jogo.mapa[i][j] == '>')
    {
        jogo.trilha[i][j] = jogo.qtdMov;
    }
    return jogo;
}
int ehFantasma(char c)
{
    if (c == 'B' || c == 'I' || c == 'P' || c == 'C')
        return 1;
    return 0;
}
int perdeuJogo(tJogo jogo, char *argv[])
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if (ehFantasma(jogo.fantasmas[i].simbolo))
        {
            if (jogo.fantasmas[i].linha == jogo.jogador.linha && jogo.fantasmas[i].coluna == jogo.jogador.coluna)
                jogo.jogador.simbolo = jogo.fantasmas[i].simbolo;
        }
    }
    if (ehFantasma(jogo.jogador.simbolo) || jogo.qtdMov == jogo.limiteMov)
        return 1;
    return 0;
}
tJogo movimentaFantasma(tJogo jogo, char c, int n)
{
    int linhaFantasma = jogo.fantasmas[n].linha;
    int colunaFantasma = jogo.fantasmas[n].coluna;
    char direcaoFantasma = jogo.fantasmas[n].direcao;

    int novaLinha = linhaFantasma;
    int novaColuna = colunaFantasma;

    if (direcaoFantasma == 's') // BAIXO
    {
        novaLinha++;
        if (jogo.mapa[novaLinha][colunaFantasma] == '#')
        {
            novaLinha = linhaFantasma - 1;
            direcaoFantasma = 'w';
        }
    }
    else if (direcaoFantasma == 'w') // CIMA
    {
        novaLinha--;
        if (jogo.mapa[novaLinha][colunaFantasma] == '#')
        {
            novaLinha = linhaFantasma + 1;
            direcaoFantasma = 's';
        }
    }
    else if (direcaoFantasma == 'a') // ESQUERDA
    {
        novaColuna--;
        if (jogo.mapa[linhaFantasma][novaColuna] == '#')
        {
            novaColuna = colunaFantasma + 1;
            direcaoFantasma = 'd';
        }
    }
    else if (direcaoFantasma == 'd') // DIREITA
    {
        novaColuna++;
        if (jogo.mapa[linhaFantasma][novaColuna] == '#')
        {
            novaColuna = colunaFantasma - 1;
            direcaoFantasma = 'a';
        }
    }

    jogo.fantasmas[n] = mudaDadosPersonagem(novaLinha, novaColuna, c);
    jogo.fantasmas[n].direcao = direcaoFantasma;
    if (jogo.backUpMapa[linhaFantasma][colunaFantasma] == '*' || jogo.backUpMapa[linhaFantasma][colunaFantasma] == '%' || jogo.backUpMapa[linhaFantasma][colunaFantasma] == '!') // VOLTA A COMIDA DEPOIS DO FANTASMA PASSAR
    {
        if (jogo.backUpMapa[linhaFantasma][colunaFantasma] == '%')
            jogo.mapa[linhaFantasma][colunaFantasma] = '%';
        else if (jogo.backUpMapa[linhaFantasma][colunaFantasma] == '!')
            jogo.mapa[linhaFantasma][colunaFantasma] = '!';
        else
            jogo.mapa[linhaFantasma][colunaFantasma] = '*';
    }

    return jogo;
}
tJogo imprimeSaida(tJogo jogo, char jogadas, char *argv[]) // IMPRESSÃO DO ARQUIVO MOSTRA MOVIMENTACAO PAC-MAN MAPA
{
    int i, j;
    char nomeArquivo[1000];
    FILE *arquivoSaida;

    sprintf(nomeArquivo, "%s/saida/saida.txt", argv[1]);
    arquivoSaida = fopen(nomeArquivo, "a");

    fprintf(arquivoSaida, "Estado do jogo apos o movimento '%c':\n", jogadas);

    for (i = 0; i < jogo.mapaL; i++)
    {
        for (j = 0; j < jogo.mapaC; j++)
        {
            fprintf(arquivoSaida, "%c", jogo.mapa[i][j]);
        }
        fprintf(arquivoSaida, "\n");
    }
    fprintf(arquivoSaida, "Pontuacao: %d\n\n", jogo.dados.capComidas + jogo.dados.comidaEspecial);

    if (jogo.comidaTotal == jogo.dados.capComidas)
    {
        fprintf(arquivoSaida, "Voce venceu!\nPontuacao final: %d\n", jogo.dados.capComidas);
    }
    else if (perdeuJogo(jogo, argv))
    {
        fprintf(arquivoSaida, "Game over!\nPontuacao final: %d\n", jogo.dados.capComidas + jogo.dados.comidaEspecial);
    }

    fclose(arquivoSaida);
    return jogo;
}
void gerarArquivoEstatisticas(tJogo jogo, char *argv[]) // IMPRIME O ARQUIVO (estatisticas.txt)
{
    char nomeEstatisticas[1000];
    FILE *arquivoEstatisticas;
    sprintf(nomeEstatisticas, "%s/saida/estatisticas.txt", argv[1]);
    arquivoEstatisticas = fopen(nomeEstatisticas, "w");

    fprintf(arquivoEstatisticas, "Numero de movimentos: %d\n", jogo.qtdMov);
    fprintf(arquivoEstatisticas, "Numero de movimentos sem pontuar: %d\n", jogo.dados.movSemPont);
    fprintf(arquivoEstatisticas, "Numero de colisoes com parede: %d\n", jogo.dados.paredeAtin);
    fprintf(arquivoEstatisticas, "Numero de movimentos para baixo: %d\n", jogo.dados.movBaixo);
    fprintf(arquivoEstatisticas, "Numero de movimentos para cima: %d\n", jogo.dados.movCima);
    fprintf(arquivoEstatisticas, "Numero de movimentos para esquerda: %d\n", jogo.dados.movEsquerda);
    fprintf(arquivoEstatisticas, "Numero de movimentos para direita: %d\n", jogo.dados.movDireita);

    fclose(arquivoEstatisticas);
}
void gerarArquivoTrilha(tJogo jogo, char *argv[]) // IMPRIME O ARQUIVO (trilha.txt)
{
    int i, j;
    char nomeArquivo[1000];
    FILE *arquivoTrilha;

    sprintf(nomeArquivo, "%s/saida/trilha.txt", argv[1]);
    arquivoTrilha = fopen(nomeArquivo, "w");

    for (i = 0; i < jogo.mapaL; i++)
    {
        for (j = 0; j < jogo.mapaC; j++)
        {
            if (jogo.trilha[i][j] >= 0)
                fprintf(arquivoTrilha, "%d ", jogo.trilha[i][j]);
            else
                fprintf(arquivoTrilha, "# ");
        }
        fprintf(arquivoTrilha, "\n");
    }
    fclose(arquivoTrilha);
}
void ImprimindoArquivoResumoJogadas(tJogo jogo, char *argv[], char jogada) // IMPRIMIR ARQUIVO COM O RELATÓRIO DE MOVIMENTOS(resumo.txt)
{

    char gerarResumo[1000];
    FILE *arquivoResumo;
    sprintf(gerarResumo, "%s/saida/resumo.txt", argv[1]);
    arquivoResumo = fopen(gerarResumo, "a");

    if (jogo.movAnterior == 1)
    {
        fprintf(arquivoResumo, "Movimento %d (%c) colidiu na parede\n", jogo.qtdMov, jogada);
    }
    else if (jogo.movAnterior == 2)
    {
        fprintf(arquivoResumo, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", jogo.qtdMov, jogada);
    }
    else if (jogo.movAnterior == 3)
    {
        fprintf(arquivoResumo, "Movimento %d (%c) pegou comida\n", jogo.qtdMov, jogada);
    }

    fclose(arquivoResumo);
}
tJogo qtdCadaMovemento(tJogo jogo)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if (jogo.movimentos[i].tipo == 'w')
            jogo.movimentos[i].qtd = jogo.dados.movCima;
        else if (jogo.movimentos[i].tipo == 's')
            jogo.movimentos[i].qtd = jogo.dados.movBaixo;
        else if (jogo.movimentos[i].tipo == 'a')
            jogo.movimentos[i].qtd = jogo.dados.movEsquerda;
        else if (jogo.movimentos[i].tipo == 'd')
            jogo.movimentos[i].qtd = jogo.dados.movDireita;
    }
    return jogo;
}
tJogo ordenaRanking(tJogo jogo)
{
    int i, v, j;
    tMovimentos aux;
    jogo = qtdCadaMovemento(jogo);
    for (i = 0; i < 4; i++)
    {
        for (j = i; j < 4; j++)
        {
            if (jogo.movimentos[i].qtdComida < jogo.movimentos[j].qtdComida)
            {
                aux = jogo.movimentos[i];
                jogo.movimentos[i] = jogo.movimentos[j];
                jogo.movimentos[j] = aux;
            }
            else if (jogo.movimentos[i].qtdComida == jogo.movimentos[j].qtdComida)
            {
                if (jogo.movimentos[i].bateuParede > jogo.movimentos[j].bateuParede)
                {
                    aux = jogo.movimentos[i];
                    jogo.movimentos[i] = jogo.movimentos[j];
                    jogo.movimentos[j] = aux;
                }
                else if (jogo.movimentos[i].bateuParede == jogo.movimentos[j].bateuParede)
                {
                    if (jogo.movimentos[i].qtd < jogo.movimentos[j].qtd)
                    {
                        aux = jogo.movimentos[i];
                        jogo.movimentos[i] = jogo.movimentos[j];
                        jogo.movimentos[j] = aux;
                    }
                    else if (jogo.movimentos[i].qtd == jogo.movimentos[j].qtd)
                    {
                        if (jogo.movimentos[i].tipo > jogo.movimentos[j].tipo)
                        {
                            aux = jogo.movimentos[i];
                            jogo.movimentos[i] = jogo.movimentos[j];
                            jogo.movimentos[j] = aux;
                        }
                    }
                }
            }
        }
    }
    return jogo;
}
void imprimindoRanking(tJogo jogo, char *argv[]) // IMPRIME O ARQUIVO (ranking.txt)
{

    int j;
    char gerarRanking[1000];
    FILE *arquivoRanking;
    sprintf(gerarRanking, "%s/saida/ranking.txt", argv[1]);
    arquivoRanking = fopen(gerarRanking, "a");
    jogo = ordenaRanking(jogo);

    for (j = 0; j < 4; j++)
    {
        fprintf(arquivoRanking, "%c,%d,%d,%d\n", jogo.movimentos[j].tipo, jogo.movimentos[j].qtdComida, jogo.movimentos[j].bateuParede, jogo.movimentos[j].qtd);
    }

    fclose(arquivoRanking);
}