#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_DESC   80
#define MAX_CAT    30
#define ARQUIVO    "despesas.dat"

typedef struct {
    int    id;
    char   descricao[MAX_DESC];
    char   categoria[MAX_CAT];
    float  valor;
    int    dia;
    int    mes;
    int    ano;
} Despesa;

Despesa *lista = NULL;
int total = 0;
int capacidade = 0;

void data_hoje(int *dia, int *mes, int *ano) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    *dia  = tm_info->tm_mday;
    *mes  = tm_info->tm_mon + 1;
    *ano  = tm_info->tm_year + 1900;
}

void garantir_capacidade(void) {
    if (total >= capacidade) {
        capacidade = (capacidade == 0) ? 4 : capacidade * 2;
        lista = (Despesa *)realloc(lista, capacidade * sizeof(Despesa));
        if (!lista) {
            fprintf(stderr, "Erro: memoria insuficiente.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void carregar_arquivo(void) {
    FILE *fp = fopen(ARQUIVO, "rb");
    if (!fp) return;

    Despesa d;
    while (fread(&d, sizeof(Despesa), 1, fp) == 1) {
        garantir_capacidade();
        lista[total++] = d;
    }
    fclose(fp);
}

void salvar_arquivo(void) {
    FILE *fp = fopen(ARQUIVO, "wb");
    if (!fp) {
        fprintf(stderr, "Erro ao abrir arquivo para escrita.\n");
        return;
    }
    fwrite(lista, sizeof(Despesa), total, fp);
    fclose(fp);
    printf("  [ok] Dados salvos em '%s'.\n", ARQUIVO);
}

void adicionar_despesa(void) {
    garantir_capacidade();
    Despesa *d = &lista[total];

    d->id = (total == 0) ? 1 : lista[total - 1].id + 1;

    printf("\n  Descricao : ");
    fgets(d->descricao, MAX_DESC, stdin);
    d->descricao[strcspn(d->descricao, "\n")] = '\0';

    printf("  Categoria (ex: Alimentacao, Transporte, Lazer): ");
    fgets(d->categoria, MAX_CAT, stdin);
    d->categoria[strcspn(d->categoria, "\n")] = '\0';

    printf("  Valor (R$): ");
    scanf("%f", &d->valor);
    limpar_buffer();

    data_hoje(&d->dia, &d->mes, &d->ano);
    printf("  Data registrada automaticamente: %02d/%02d/%04d\n",
           d->dia, d->mes, d->ano);

    total++;
    salvar_arquivo();
    printf("  Despesa #%d adicionada com sucesso!\n", d->id);
}

void imprimir_cabecalho(void) {
    printf("\n  %-4s %-25s %-15s %10s  %s\n",
           "ID", "Descricao", "Categoria", "Valor", "Data");
    printf("  %s\n", "-------------------------------------------------------------------");
}

void listar_todas(void) {
    if (total == 0) {
        printf("\n  Nenhuma despesa cadastrada.\n");
        return;
    }
    imprimir_cabecalho();
    for (int i = 0; i < total; i++) {
        Despesa *d = &lista[i];
        printf("  %-4d %-25s %-15s R$%8.2f  %02d/%02d/%04d\n",
               d->id, d->descricao, d->categoria,
               d->valor, d->dia, d->mes, d->ano);
    }
}

void filtrar_por_mes(void) {
    int mes, ano;
    printf("\n  Mes  (1-12): "); scanf("%d", &mes); limpar_buffer();
    printf("  Ano        : "); scanf("%d", &ano);  limpar_buffer();

    float total_mes = 0.0f;
    int encontrou = 0;

    imprimir_cabecalho();
    for (int i = 0; i < total; i++) {
        if (lista[i].mes == mes && lista[i].ano == ano) {
            Despesa *d = &lista[i];
            printf("  %-4d %-25s %-15s R$%8.2f  %02d/%02d/%04d\n",
                   d->id, d->descricao, d->categoria,
                   d->valor, d->dia, d->mes, d->ano);
            total_mes += d->valor;
            encontrou = 1;
        }
    }
    if (!encontrou)
        printf("  Nenhuma despesa em %02d/%04d.\n", mes, ano);
    else
        printf("\n  Total em %02d/%04d: R$ %.2f\n", mes, ano, total_mes);
}

typedef struct {
    char  nome[MAX_CAT];
    float soma;
} Categoria;

int comparar_categoria(const void *a, const void *b) {
    const Categoria *ca = (const Categoria *)a;
    const Categoria *cb = (const Categoria *)b;
    if (cb->soma > ca->soma) return  1;
    if (cb->soma < ca->soma) return -1;
    return 0;
}

void relatorio_categorias(void) {
    if (total == 0) {
        printf("\n  Sem despesas para relatorio.\n");
        return;
    }

    Categoria *cats = (Categoria *)calloc(total, sizeof(Categoria));
    if (!cats) { fprintf(stderr, "Erro de memoria.\n"); return; }
    int n_cats = 0;

    for (int i = 0; i < total; i++) {
        int achado = 0;
        for (int j = 0; j < n_cats; j++) {
            if (strcmp(cats[j].nome, lista[i].categoria) == 0) {
                cats[j].soma += lista[i].valor;
                achado = 1;
                break;
            }
        }
        if (!achado) {
            strncpy(cats[n_cats].nome, lista[i].categoria, MAX_CAT - 1);
            cats[n_cats].soma = lista[i].valor;
            n_cats++;
        }
    }

    qsort(cats, n_cats, sizeof(Categoria), comparar_categoria);

    printf("\n  === Relatorio por Categoria ===\n");
    printf("  %-20s %12s\n", "Categoria", "Total (R$)");
    printf("  %s\n", "------------------------------------");

    float grand_total = 0.0f;
    for (int i = 0; i < n_cats; i++) {
        printf("  %-20s R$ %9.2f\n", cats[i].nome, cats[i].soma);
        grand_total += cats[i].soma;
    }
    printf("  %s\n", "------------------------------------");
    printf("  %-20s R$ %9.2f\n", "TOTAL GERAL", grand_total);

    free(cats);
}

void remover_despesa(void) {
    int id;
    printf("\n  ID da despesa a remover: ");
    scanf("%d", &id); limpar_buffer();

    int idx = -1;
    for (int i = 0; i < total; i++) {
        if (lista[i].id == id) { idx = i; break; }
    }
    if (idx == -1) {
        printf("  Despesa #%d nao encontrada.\n", id);
        return;
    }

    for (int i = idx; i < total - 1; i++)
        lista[i] = lista[i + 1];
    total--;

    salvar_arquivo();
    printf("  Despesa #%d removida.\n", id);
}

void menu(void) {
    int opcao;
    do {
        printf("\n+==============================+\n");
        printf("|  Rastreador de Despesas  (C) |\n");
        printf("+==============================+\n");
        printf("|  1. Adicionar despesa        |\n");
        printf("|  2. Listar todas             |\n");
        printf("|  3. Filtrar por mes/ano      |\n");
        printf("|  4. Relatorio por categoria  |\n");
        printf("|  5. Remover despesa          |\n");
        printf("|  0. Sair                     |\n");
        printf("+==============================+\n");
        printf("  Opcao: ");
        scanf("%d", &opcao); limpar_buffer();

        switch (opcao) {
            case 1: adicionar_despesa();    break;
            case 2: listar_todas();         break;
            case 3: filtrar_por_mes();      break;
            case 4: relatorio_categorias(); break;
            case 5: remover_despesa();      break;
            case 0: printf("\n  Ate logo!\n"); break;
            default: printf("\n  Opcao invalida.\n");
        }
    } while (opcao != 0);
}

int main(void) {
    carregar_arquivo();
    menu();
    free(lista);
    return 0;
}
