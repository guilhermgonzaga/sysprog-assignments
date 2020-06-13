# Planejamento

Precisamos definir uma interface para cada módulo, além de estruturas de dados.

Etapas de um pipeline REPL:

1. Receber/processar entrada
1. Executar comandos (efeitos colaterais estão todos aqui)
1. Imprimir/redirecionar saída
1. Repetir etapas acima


## Funcionalidades

- [ ] Execução de um programa com ou sem argumentos:
	- [ ] em *foreground*;
	- [ ] em *background*;
	- [ ] com redirecionamento da entrada padrão (`<`);
	- [ ] com redirecionamento das saídas padrão (`>`) ou de erro (`2>`);
- [ ] Pipes (`|`) entre programas, sem limitação;
- [ ] Buscar programas a serem executados nos diretórios listados na variável `MYPATH`;
- [ ] Comando `export` para (re)definição de variáveis;
- [ ] Comando `cd` para mudança de diretório --- deve atualizar a variável `PWD`;
- [ ] Comando `history`, que mostra os últimos 50 comandos digitados pelo usuário (inclusive si mesmo);
- [ ] Mostrar um *prompt*, mas permitir que o texto seja redefinido pela variável `MYPS1`;
- [ ] Comando `exit` e tratamento de `^D` (não é sinal) para encerrar a *shell*;
- [ ] Tratar sinal `SIGINT`;
- [ ] Tratar sinal `SIGTSTP`;
- [ ] Comando `kill`;
- [ ] Comando `jobs`;
- [ ] Comando `fg`;
- [ ] Comando `bg`;
- [ ] Tratar sinal `SIGCHLD`;
- [ ] Comando `echo` para visualização do conteúdo de variáveis de ambiente e impressão de cadeias de caracteres;
- [ ] Comando `set` para visualizar todas as variáveis de ambiente;

- [ ] **Ponto Extra**. Implementar uma linguagem para criação e processamento de *scripts*;

- [ ] **Extra**. Aceitar setas para cima e para baixo para acessar o histórico;
- [ ] **Extra**. Aceitar ^L para limpar a tela;


## Links úteis

- [microshell](https://github.com/mtds/microshell)
- [picoshell](https://github.com/ryo1kato/picoshell)
