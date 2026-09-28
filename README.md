# Rastreador-De-Despesas-C

Sistema de rastreamento de despesas pessoais em C com categorias, filtros por mes, relatorio de gastos e persistencia em arquivo binario. Usa structs, alocacao dinamica e ponteiros para funcao.

## Funcionalidades

- Adicionar despesas com descricao, categoria e valor (data preenchida automaticamente)
- Listar todas as despesas cadastradas
- Filtrar despesas por mes e ano com total do periodo
- Relatorio de gastos por categoria ordenado do maior para o menor
- Remover despesa por ID
- Persistencia em arquivo binario (`despesas.dat`)

## Conceitos praticados

- Structs e alocacao dinamica (`malloc`, `realloc`, `calloc`, `free`)
- Ponteiro para funcao com `qsort`
- Arquivos binarios (`fread`, `fwrite`)
- Vetores, funcoes modulares e `time.h`

## Como compilar e rodar

```bash
gcc despesas.c -o despesas
./despesas
```

## Linguagem

C (compilavel com gcc)
