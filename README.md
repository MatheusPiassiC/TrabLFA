# Trabalho Linguagens Formais e Autômatos - FNC

## Descrição

Este projeto consiste em um trabalho prático da disciplina **Linguagens Formais e Autômatos**, ministrada pelo professor **Ricardo Terra**. O objetivo é criar um programa que lê uma Gramática Livre de Contexto (GLC) e a transforma na **Forma Normal de Chomsky (FNC)**.

Os participantes do projeto são:
- **Mateus Mendes da Silva**
- **Matheus Piassi de Carvalho**
- **Bernardo Coelho Pavani Marinho**

## Objetivo

O programa converte uma GLC em FNC seguindo os passos aprendidos no curso. Ele executa algoritmos para remoção de recursão inicial, tratamento de variáveis "nullable", eliminação de cadeias, e outras transformações necessárias para a FNC.

## Instalação

Clone o repositório:
```bash
git clone https://github.com/seu_usuario/conversor-glc-fnc.git
cd conversor-glc-fnc
```

## Como Compilar e Executar

## Pré-requisitos

- Compilador C++ (recomendado: g++ versão 7.5 ou superior)

### Compilação

No terminal, use o seguinte comando para compilar o código:

```bash
g++ -o fnc main.cpp
```

E após compilado, para executar é necessário inserir:

```bash
./fnc "ArquivoGramatica.txt"
```

– Variáveis: [A-Z] (as variáveis criadas pelo seu algoritmo podem ser T1, T2, T3, etc.)

– Terminais: [a-z]

– Operador de definição: ->

– Separador de regras: |

– Lambda: .



## Exemplo de Formato:

```plaintext
S -> Sa | bA
A -> Ab | Bc
B -> Aa | .
```
 
 ## Contribuidores : 

https://github.com/BernardoPavani

https://github.com/MatheusPiassiC

https://github.com/MateusMendes0
