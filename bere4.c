#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============================================================
 *  STRUCTS
 * ============================================================ */

/* ---------- Usuário ---------- */
typedef struct
{
    char login[100]; /* Login: 8 a 12 caracteres */
    char senha[9];   /* Senha: 6 a 8 caracteres  */
    int  tipo;       /* 1 = Administrador, 2 = Usuário */
} Usuario;

/* ---------- Cliente ---------- */
typedef struct
{
    int  codigo;           /* Código único gerado automaticamente (a partir de 1000) */
    char nome[100];        /* Nome completo */
    char nomeSocial[100];  /* Nome social (respeito à diversidade) */
    char cpf[25];          /* CPF do cliente */
    char rua[100];         /* Rua e número */
    char bairro[100];      /* Bairro */
    char celular[100];     /* Celular/WhatsApp */
} Clientes;

/* ---------- Produto ---------- */
typedef struct
{
    int   codigo;          /* Código único gerado automaticamente (a partir de 2000) */
    char  descricao[100];  /* Descrição do produto */
    char  categoria[100];  /* Categoria: Alimento, Limpeza, Panificação */
    float precoCompra;     /* Preço de compra */
    float margemLucro;     /* Margem de lucro em % */
    float precoVenda;      /* Preço de venda = compra + margem */
    int   estoque;         /* Quantidade atual em estoque */
    int   estoqueMinimo;   /* Quantidade mínima antes do alerta */
} Produtos;

/* ---------- Venda (cabeçalho) ---------- */
typedef struct
{
    int   numeroVenda;       /* Número único da venda */
    int   codigoCliente;     /* Código do cliente (0 = sem cliente / Consumidor) */
    char  nomeCliente[100];  /* Nome do cliente para exibição rápida */
    int   dia;               /* Dia da venda */
    int   mes;               /* Mês da venda */
    int   ano;               /* Ano da venda */
    float totalVenda;        /* Total final (após desconto) */
} Vendas;

/* ---------- Item de Venda ---------- */
typedef struct
{
    int   numeroVenda;     /* Número da venda à qual o item pertence */
    int   codigoProduto;   /* Código do produto vendido */
    char  descricao[100];  /* Descrição copiada do produto */
    float precoVenda;      /* Preço de venda no momento da venda */
    int   quantidade;      /* Quantidade vendida */
    float totalItem;       /* quantidade × precoVenda */
    char  pagamento;       /* 'a' = Aberto, 'p' = Pago */
} ItensVenda;

/* ---------- Pagamento ---------- */
typedef struct
{
    int   numeroVenda; /* Número da venda */
    float valorPago;   /* Valor pago neste registro */
    char  tipo[3];     /* "d"=Dinheiro, "c"=Cartão, "md"=Misto-Dinheiro, "mc"=Misto-Cartão */
} Pagamentos;

/* ============================================================
 *  VARIÁVEIS GLOBAIS
 * ============================================================ */

int usuarioLogado = -1;
int sistemaLogado = 0;

int totalUsuarios       = 0;  int capacidadeUsuarios    = 10;
int totalClientes       = 0;  int capacidadeClientes    = 10;
int totalProdutos       = 0;  int capacidadeProdutos    = 10;
int totalVendas         = 0;  int capacidadeVendas      = 10;
int totalItens          = 0;  int capacidadeItens       = 10;
int totalPagamentos     = 0;  int capacidadePagamentos  = 10;

int numeroVendaAtual = 1;

Usuario    *usuario;
Clientes   *cliente;
Produtos   *produto;
Vendas     *venda;
ItensVenda *item;
Pagamentos *pagamento;

int   caixaAberto  = 0;
float caixaInicial = 0.0f;
float caixaAtual   = 0.0f;
float faturamento  = 0.0f;

float historicoSangrias[50];
int   totalSangrias = 0;

/* ============================================================
 *  PROTÓTIPOS
 * ============================================================ */

float calcularPrecoVenda(float precoCompra, float margemLucro);
float aplicarDesconto(float total, float desconto);

void obterDataAtual(int *dia, int *mes, int *ano);
int  validarAdmin(void);
void limparBuffer(void);

/* ---- utilitários de tabela para arquivos .txt ---- */
int  maiorEntre(int a, int b);
int  larguraColunaTexto(const char *valores[], int totalLinhas, int larguraMinima, const char *titulo);
void escreverLinhaSeparadora(FILE *f, const int *larguras, int totalColunas);
void escreverCelulaTexto(FILE *f, const char *valor, int largura);
void escreverCelulaNumero(FILE *f, const char *valorFormatado, int largura);

void cadastrarUsuario(void);
int  autenticarUsuario(void);
int  loginExiste(char login[]);
int  ehAdministrador(void);

void menuPrincipal(void);
void menuCadastros(void);
void menuVendas(void);
void relatorios(void);

void cadastrarCliente(void);
void cadastrarProduto(void);

void listarProdutosVenda(void);

void novaVenda(void);
void pagarVendaAberta(void);

void abrirCaixa(void);
void fecharCaixa(void);
void sangria(void);
void listarSangrias(void);

void relatorioClientes(void);
void relatorioClientesOrdenado(void);
void relatorioClientesPeriodo(void);
void relatorioProdutos(void);
void relatorioProdutosOrdenado(void);
void relatorioProdutosEstoqueMinimo(void);
void relatorioProdutosMaisVendidos(void);
void relatorioVendas(void);
void relatorioVendasPeriodo(void);
void relatorioFaturamentoPeriodo(void);

void salvarUsuarios(void);    void carregarUsuarios(void);
void salvarClientes(void);    void carregarClientes(void);
void salvarProdutos(void);    void carregarProdutos(void);
void salvarVendas(void);      void carregarVendas(void);
void salvarItens(void);       void carregarItens(void);
void salvarPagamentos(void);  void carregarPagamentos(void);
void salvarCaixa(void);       void carregarCaixa(void);
void carregarTudo(void);      void salvarTudo(void);

/* ============================================================
 *  FUNÇÃO PRINCIPAL
 * ============================================================ */

int main(void)
{
    usuario   = malloc(capacidadeUsuarios   * sizeof(Usuario));
    cliente   = malloc(capacidadeClientes   * sizeof(Clientes));
    produto   = malloc(capacidadeProdutos   * sizeof(Produtos));
    venda     = malloc(capacidadeVendas     * sizeof(Vendas));
    item      = malloc(capacidadeItens      * sizeof(ItensVenda));
    pagamento = malloc(capacidadePagamentos * sizeof(Pagamentos));

    if (!usuario || !cliente || !produto || !venda || !item || !pagamento)
    {
        printf("ERRO CRITICO: Falha na alocacao de memoria!\n");
        return 1;
    }

    carregarTudo();

    if (totalUsuarios == 0)
    {
        printf("\n╔══════════════════════════════════════════════╗\n");
        printf("║   Nenhum usuario encontrado.                 ║\n");
        printf("║   Cadastre o primeiro Administrador.         ║\n");
        printf("╚══════════════════════════════════════════════╝\n");
        cadastrarUsuario();
        salvarUsuarios();
    }

    while (!autenticarUsuario())
        printf("\nCredenciais incorretas. Tente novamente!\n");

    menuPrincipal();

    salvarTudo();

    free(usuario);
    free(cliente);
    free(produto);
    free(venda);
    free(item);
    free(pagamento);

    return 0;
}

/* ============================================================
 *  UTILITÁRIOS
 * ============================================================ */

void obterDataAtual(int *dia, int *mes, int *ano)
{
    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);
    *dia = t->tm_mday;
    *mes = t->tm_mon + 1;
    *ano = t->tm_year + 1900;
}

void limparBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* ============================================================
 *  UTILITÁRIOS DE TABELA PARA OS ARQUIVOS .TXT
 *
 *  Estas funções permitem que cada arquivo .txt salvo em disco
 *  (usuarios.txt, clientes.txt, produtos.txt, etc.) fique formatado
 *  como uma tabela alinhada, com cabeçalho e bordas, ao invés de
 *  ter um valor por linha "empilhado". A largura de cada coluna se
 *  ajusta automaticamente ao maior valor presente naquela coluna
 *  (ou ao título do cabeçalho, se ele for maior), garantindo que
 *  nada fique cortado e tudo fique sempre alinhado.
 * ============================================================ */

int maiorEntre(int a, int b)
{
    return (a > b) ? a : b;
}

/* Calcula a largura ideal de uma coluna de texto, olhando o tamanho
 * de todos os valores que vao aparecer nela e do titulo do cabecalho. */
int larguraColunaTexto(const char *valores[], int totalLinhas, int larguraMinima, const char *titulo)
{
    int largura = maiorEntre(larguraMinima, (int)strlen(titulo));
    for (int i = 0; i < totalLinhas; i++)
    {
        int len = (int)strlen(valores[i]);
        if (len > largura)
            largura = len;
    }
    return largura;
}

/* Escreve uma linha separadora tipo +------------+------------+ */
void escreverLinhaSeparadora(FILE *f, const int *larguras, int totalColunas)
{
    fprintf(f, "+");
    for (int c = 0; c < totalColunas; c++)
    {
        for (int i = 0; i < larguras[c] + 2; i++)
            fprintf(f, "-");
        fprintf(f, "+");
    }
    fprintf(f, "\n");
}

/* Escreve uma celula de texto alinhada a esquerda dentro da largura da coluna.
 * Cada chamada escreve "| valor " e quem chama eh responsavel por colocar o
 * "|" final apos a ultima coluna da linha (ver helper escreverFimLinha). */
void escreverCelulaTexto(FILE *f, const char *valor, int largura)
{
    fprintf(f, "| %-*s ", largura, valor);
}

/* Escreve uma celula numerica/ja formatada alinhada a esquerda */
void escreverCelulaNumero(FILE *f, const char *valorFormatado, int largura)
{
    fprintf(f, "| %-*s ", largura, valorFormatado);
}

/* ============================================================
 *  CÁLCULOS
 * ============================================================ */

float calcularPrecoVenda(float precoCompra, float margemLucro)
{
    return precoCompra + (precoCompra * margemLucro / 100.0f);
}

float aplicarDesconto(float total, float desconto)
{
    if (desconto <= 0.0f)
        return total;
    return total - (total * desconto / 100.0f);
}

/* ============================================================
 *  VALIDAÇÃO DE ADMINISTRADOR
 * ============================================================ */

int validarAdmin(void)
{
    if (ehAdministrador())
        return 1;

    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║  Operacao restrita a Administradores.        ║\n");
    printf("║  Informe credenciais de administrador:       ║\n");
    printf("╚══════════════════════════════════════════════╝\n");

    char login[100], senha[9];
    printf("Login: ");
    scanf("%99s", login);
    printf("Senha: ");
    scanf("%8s", senha);

    for (int i = 0; i < totalUsuarios; i++)
    {
        if (strcmp(usuario[i].login, login) == 0 &&
            strcmp(usuario[i].senha, senha) == 0 &&
            usuario[i].tipo == 1)
        {
            printf("\n✔ Acesso liberado!\n");
            return 1;
        }
    }

    printf("\n✘ Credenciais invalidas ou usuario nao e administrador!\n");
    return 0;
}

/* ============================================================
 *  USUÁRIOS
 * ============================================================ */

void cadastrarUsuario(void)
{
    if (totalUsuarios == capacidadeUsuarios)
    {
        capacidadeUsuarios *= 2;
        Usuario *tmp = realloc(usuario, capacidadeUsuarios * sizeof(Usuario));
        if (!tmp) { printf("Erro de memoria!\n"); return; }
        usuario = tmp;
    }

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                     BEREZITA MARKET                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║                    CADASTRO DE USUARIO                       ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    do
    {
        printf("\n┌──────────────────────────────────────────────┐\n");
        printf("│  LOGIN  (8 a 12 caracteres)                  │\n");
        printf("└──────────────────────────────────────────────┘\n");
        printf("➜ Login: ");
        scanf("%99s", usuario[totalUsuarios].login);

        int len = (int)strlen(usuario[totalUsuarios].login);
        if (len < 8 || len > 12)
            printf("✘ Login deve ter entre 8 e 12 caracteres!\n");
        else if (loginExiste(usuario[totalUsuarios].login))
            printf("✘ Esse login ja existe!\n");
        else
            break;
    } while (1);

    do
    {
        printf("\n┌──────────────────────────────────────────────┐\n");
        printf("│  SENHA  (6 a 8 caracteres)                   │\n");
        printf("└──────────────────────────────────────────────┘\n");
        printf("➜ Senha: ");
        scanf("%8s", usuario[totalUsuarios].senha);

        int len = (int)strlen(usuario[totalUsuarios].senha);
        if (len < 6 || len > 8)
            printf("✘ Senha deve ter entre 6 e 8 caracteres!\n");
        else
            break;
    } while (1);

    if (totalUsuarios == 0)
    {
        printf("\nPrimeiro usuario sera Administrador automaticamente!\n");
        usuario[totalUsuarios].tipo = 1;
    }
    else
    {
        do
        {
            printf("\nTipo:\n  1 - Administrador\n  2 - Usuario\nEscolha: ");
            scanf("%d", &usuario[totalUsuarios].tipo);
            if (usuario[totalUsuarios].tipo != 1 && usuario[totalUsuarios].tipo != 2)
                printf("✘ Opcao invalida!\n");
        } while (usuario[totalUsuarios].tipo != 1 && usuario[totalUsuarios].tipo != 2);
    }

    totalUsuarios++;
    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║      USUARIO CADASTRADO COM SUCESSO!         ║\n");
    printf("╚══════════════════════════════════════════════╝\n");
    salvarUsuarios();
}

int autenticarUsuario(void)
{
    if (totalUsuarios == 0)
    {
        printf("Nao existem usuarios cadastrados!\n");
        return 0;
    }

    char login[100], senha[9];

    printf("\n");
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║           BEREZITA MARKET - LOGIN            ║\n");
    printf("╚══════════════════════════════════════════════╝\n");
    printf("➜ Login: ");
    scanf("%99s", login);
    printf("➜ Senha: ");
    scanf("%8s", senha);

    for (int i = 0; i < totalUsuarios; i++)
    {
        if (strcmp(usuario[i].login, login) == 0 &&
            strcmp(usuario[i].senha, senha) == 0)
        {
            usuarioLogado = i;
            sistemaLogado = 1;

            printf("\n╔══════════════════════════════════════════════╗\n");
            printf("║              ACESSO LIBERADO!                ║\n");
            printf("╚══════════════════════════════════════════════╝\n");
            printf("\nBem-vindo, %s! Perfil: %s\n",
                   usuario[i].login,
                   usuario[i].tipo == 1 ? "Administrador" : "Usuario");
            return 1;
        }
    }

    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║              ACESSO NEGADO!                  ║\n");
    printf("╚══════════════════════════════════════════════╝\n");
    printf("Login ou senha incorretos!\n");
    return 0;
}

int loginExiste(char login[])
{
    for (int i = 0; i < totalUsuarios; i++)
        if (strcmp(usuario[i].login, login) == 0)
            return 1;
    return 0;
}

int ehAdministrador(void)
{
    if (usuarioLogado < 0) return 0;
    return usuario[usuarioLogado].tipo == 1;
}

/* ============================================================
 *  MENUS
 * ============================================================ */

void menuPrincipal(void)
{
    int opcao;
    do
    {
        printf("\n");
        printf("╔══════════════════════════════════════════════════════════════╗\n");
        printf("║  ██████╗ ███████╗██████╗ ███████╗███████╗██╗████████╗ █████╗ ║\n");
        printf("║  ██╔══██╗██╔════╝██╔══██╗██╔════╝╚══███╔╝██║╚══██╔══╝██╔══██╗║\n");
        printf("║  ██████╔╝█████╗  ██████╔╝█████╗    ███╔╝ ██║   ██║   ███████║║\n");
        printf("║  ██╔══██╗██╔══╝  ██╔══██╗██╔══╝   ███╔╝  ██║   ██║   ██╔══██║║\n");
        printf("║  ██████╔╝███████╗██║  ██║███████╗███████╗██║   ██║   ██║  ██║║\n");
        printf("║  ╚═════╝ ╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚═╝   ╚═╝   ╚═╝  ╚═╝║\n");
        printf("╠══════════════════════════════════════════════════════════════╣\n");
        printf("║                     MENU PRINCIPAL                           ║\n");
        printf("╠══════════════════════════════════════════════════════════════╣\n");
        printf("║  [1] 📁  Cadastros                                           ║\n");
        printf("║  [2] 🛒  Vendas                                              ║\n");
        printf("║  [3] 💰  Abertura de Caixa                                   ║\n");
        printf("║  [4] 📊  Fechamento de Caixa                                 ║\n");
        printf("║  [5] 📑  Relatorios                                          ║\n");
        printf("║  [6] 🚪  Sair                                                ║\n");
        printf("╚══════════════════════════════════════════════════════════════╝\n");
        printf("➜ Escolha: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
        case 1: menuCadastros(); break;
        case 2: menuVendas();    break;
        case 3:
            if (validarAdmin()) abrirCaixa();
            break;
        case 4:
            if (validarAdmin()) fecharCaixa();
            break;
        case 5: relatorios(); break;
        case 6:
            printf("\nSistema encerrado. Ate logo!\n");
            break;
        default:
            printf("\nOpcao invalida!\n");
            break;
        }
    } while (opcao != 6);
}

void menuCadastros(void)
{
    int opcao;
    do
    {
        printf("\n");
        printf("╔══════════════════════════════════════╗\n");
        printf("║          MENU CADASTROS              ║\n");
        printf("╠══════════════════════════════════════╣\n");
        printf("║  [1] 👤  Cadastro de Usuarios        ║\n");
        printf("║  [2] 👥  Cadastro de Clientes        ║\n");
        printf("║  [3] 📦  Cadastro de Produtos        ║\n");
        printf("║  [4] 🏷️   Cadastro de Categoria      ║\n");
        printf("║  [5] ↩️   Retornar ao Menu Principal ║\n");
        printf("╚══════════════════════════════════════╝\n");
        printf("➜ Escolha: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
        case 1:
            if (ehAdministrador())
                cadastrarUsuario();
            else
                printf("\nApenas administradores podem cadastrar usuarios!\n");
            break;
        case 2: cadastrarCliente(); break;
        case 3: cadastrarProduto(); break;
        case 4:
            printf("\nFuncionalidade de categoria em desenvolvimento.\n");
            break;
        case 5: break;
        default:
            printf("\nOpcao invalida!\n");
            break;
        }
    } while (opcao != 5);
}

void menuVendas(void)
{
    int opcao;
    do
    {
        printf("\n");
        printf("╔══════════════════════════════════════╗\n");
        printf("║            MENU VENDAS               ║\n");
        printf("╠══════════════════════════════════════╣\n");
        printf("║  [1] 🛒  Nova Venda                  ║\n");
        printf("║  [2] 💸  Retirada de Caixa (Sangria) ║\n");
        printf("║  [3] 💳  Pagamento                   ║\n");
        printf("║  [4] ↩️   Retornar                    ║\n");
        printf("╚══════════════════════════════════════╝\n");
        printf("➜ Opcao: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
        case 1: novaVenda(); break;
        case 2:
            if (validarAdmin()) sangria();
            break;
        case 3: pagarVendaAberta(); break;
        case 4: break;
        default:
            printf("\nOpcao invalida!\n");
            break;
        }
    } while (opcao != 4);
}

/* ============================================================
 *  CADASTROS
 * ============================================================ */

void cadastrarCliente(void)
{
    if (totalClientes == capacidadeClientes)
    {
        capacidadeClientes *= 2;
        Clientes *tmp = realloc(cliente, capacidadeClientes * sizeof(Clientes));
        if (!tmp) { printf("Erro de memoria!\n"); return; }
        cliente = tmp;
    }

    cliente[totalClientes].codigo = 1000 + totalClientes;

    printf("\n");
    printf("╔══════════════════════════════════════╗\n");
    printf("║        CADASTRO DE CLIENTE           ║\n");
    printf("╚══════════════════════════════════════╝\n");
    printf("Codigo: %d\n", cliente[totalClientes].codigo);

    limparBuffer();
    printf("Nome completo     : ");
    fgets(cliente[totalClientes].nome, 100, stdin);
    cliente[totalClientes].nome[strcspn(cliente[totalClientes].nome, "\n")] = '\0';

    printf("Nome social       : ");
    fgets(cliente[totalClientes].nomeSocial, 100, stdin);
    cliente[totalClientes].nomeSocial[strcspn(cliente[totalClientes].nomeSocial, "\n")] = '\0';

    printf("CPF               : ");
    fgets(cliente[totalClientes].cpf, 25, stdin);
    cliente[totalClientes].cpf[strcspn(cliente[totalClientes].cpf, "\n")] = '\0';

    printf("Rua e numero      : ");
    fgets(cliente[totalClientes].rua, 100, stdin);
    cliente[totalClientes].rua[strcspn(cliente[totalClientes].rua, "\n")] = '\0';

    printf("Bairro            : ");
    fgets(cliente[totalClientes].bairro, 100, stdin);
    cliente[totalClientes].bairro[strcspn(cliente[totalClientes].bairro, "\n")] = '\0';

    printf("Celular/WhatsApp  : ");
    fgets(cliente[totalClientes].celular, 100, stdin);
    cliente[totalClientes].celular[strcspn(cliente[totalClientes].celular, "\n")] = '\0';

    totalClientes++;
    printf("\n✔ Cliente cadastrado com sucesso!\n");
    salvarClientes();
}

void cadastrarProduto(void)
{
    if (totalProdutos == capacidadeProdutos)
    {
        capacidadeProdutos *= 2;
        Produtos *tmp = realloc(produto, capacidadeProdutos * sizeof(Produtos));
        if (!tmp) { printf("Erro de memoria!\n"); return; }
        produto = tmp;
    }

    produto[totalProdutos].codigo = 2000 + totalProdutos;

    printf("\n");
    printf("╔══════════════════════════════════════╗\n");
    printf("║        CADASTRO DE PRODUTO           ║\n");
    printf("╚══════════════════════════════════════╝\n");
    printf("Codigo: %d\n", produto[totalProdutos].codigo);

    limparBuffer();
    printf("Descricao                          : ");
    fgets(produto[totalProdutos].descricao, 100, stdin);
    produto[totalProdutos].descricao[strcspn(produto[totalProdutos].descricao, "\n")] = '\0';

    printf("Categoria (Alimento/Limpeza/Panif.): ");
    fgets(produto[totalProdutos].categoria, 100, stdin);
    produto[totalProdutos].categoria[strcspn(produto[totalProdutos].categoria, "\n")] = '\0';

    do
    {
        printf("Preco de compra                    : R$ ");
        scanf("%f", &produto[totalProdutos].precoCompra);
        if (produto[totalProdutos].precoCompra <= 0.0f)
            printf("✘ Preco invalido! Deve ser maior que zero.\n");
    } while (produto[totalProdutos].precoCompra <= 0.0f);

    do
    {
        printf("Margem de lucro (%%)               : ");
        scanf("%f", &produto[totalProdutos].margemLucro);
        if (produto[totalProdutos].margemLucro < 0.0f)
            printf("✘ Margem invalida! Deve ser >= 0.\n");
    } while (produto[totalProdutos].margemLucro < 0.0f);

    produto[totalProdutos].precoVenda = calcularPrecoVenda(
        produto[totalProdutos].precoCompra,
        produto[totalProdutos].margemLucro);
    printf("Preco de venda calculado           : R$ %.2f\n", produto[totalProdutos].precoVenda);

    do
    {
        printf("Estoque atual                      : ");
        scanf("%d", &produto[totalProdutos].estoque);
        if (produto[totalProdutos].estoque < 0)
            printf("✘ Estoque invalido! Deve ser >= 0.\n");
    } while (produto[totalProdutos].estoque < 0);

    printf("Estoque minimo                     : ");
    scanf("%d", &produto[totalProdutos].estoqueMinimo);

    totalProdutos++;
    printf("\n✔ Produto cadastrado com sucesso!\n");
    salvarProdutos();
}

/* ============================================================
 *  VENDAS
 * ============================================================ */

void listarProdutosVenda(void)
{
    printf("\n");
    printf("╔══════════╦══════════════════════╦══════════════════════╦════════════╦══════════╗\n");
    printf("║  CODIGO  ║      DESCRICAO       ║      CATEGORIA       ║   PRECO    ║ ESTOQUE  ║\n");
    printf("╠══════════╬══════════════════════╬══════════════════════╬════════════╬══════════╣\n");
    for (int i = 0; i < totalProdutos; i++)
    {
        printf("║ %-8d ║ %-20s ║ %-20s ║ R$%-7.2f ║ %-8d ║\n",
               produto[i].codigo,
               produto[i].descricao,
               produto[i].categoria,
               produto[i].precoVenda,
               produto[i].estoque);
    }
    printf("╚══════════╩══════════════════════╩══════════════════════╩════════════╩══════════╝\n");
}

void novaVenda(void)
{
    if (caixaAberto == 0)
    {
        printf("\n✘ Abra o caixa primeiro!\n");
        return;
    }

    if (totalProdutos == 0)
    {
        printf("\n✘ Nao existem produtos cadastrados!\n");
        return;
    }

    int dia, mes, ano;
    obterDataAtual(&dia, &mes, &ano);

    int  codigoCliente = 0;
    char nomeCliente[100] = "Consumidor";
    char respCliente;

    printf("\nVincular cliente a esta venda? (s/n): ");
    scanf(" %c", &respCliente);

    if (respCliente == 's' || respCliente == 'S')
    {
        printf("Codigo do cliente: ");
        scanf("%d", &codigoCliente);
        int achouCliente = 0;
        for (int i = 0; i < totalClientes; i++)
        {
            if (cliente[i].codigo == codigoCliente)
            {
                strcpy(nomeCliente, cliente[i].nome);
                achouCliente = 1;
                break;
            }
        }
        if (!achouCliente)
        {
            printf("Cliente nao encontrado. Venda registrada como Consumidor.\n");
            codigoCliente = 0;
        }
    }

    float totalCarrinho = 0.0f;
    int   inicioItens   = totalItens;
    char  resposta;

    do
    {
        listarProdutosVenda();

        int codigo;
        printf("\nInforme o codigo do produto: ");
        scanf("%d", &codigo);

        int encontrou = 0;
        for (int i = 0; i < totalProdutos; i++)
        {
            if (codigo != produto[i].codigo)
                continue;
            encontrou = 1;

            if (produto[i].estoque == 0)
            {
                printf("\n⚠ ALERTA: Produto '%s' sem estoque! Nao e possivel vender.\n",
                       produto[i].descricao);
                break;
            }

            int quantidade;
            printf("Informe a quantidade: ");
            scanf("%d", &quantidade);

            if (quantidade <= 0)
            {
                printf("✘ Quantidade invalida!\n");
                break;
            }

            if (quantidade > produto[i].estoque)
            {
                char continuar;
                printf("\n⚠ Voce deseja vender %d produto(s). Ha somente %d em estoque! Continuar (s/n)? ",
                       quantidade, produto[i].estoque);
                scanf(" %c", &continuar);
                if (continuar != 's' && continuar != 'S')
                    break;
                quantidade = produto[i].estoque;
            }

            produto[i].estoque -= quantidade;

            if (produto[i].estoque <= produto[i].estoqueMinimo)
                printf("\n⚠ ALERTA: Estoque minimo atingido para '%s'!\n", produto[i].descricao);

            if (totalItens == capacidadeItens)
            {
                capacidadeItens *= 2;
                ItensVenda *tmp = realloc(item, capacidadeItens * sizeof(ItensVenda));
                if (!tmp) { printf("Erro de memoria!\n"); return; }
                item = tmp;
            }

            float subtotal = (float)quantidade * produto[i].precoVenda;
            totalCarrinho += subtotal;

            item[totalItens].numeroVenda   = numeroVendaAtual;
            item[totalItens].codigoProduto = produto[i].codigo;
            strcpy(item[totalItens].descricao, produto[i].descricao);
            item[totalItens].precoVenda  = produto[i].precoVenda;
            item[totalItens].quantidade  = quantidade;
            item[totalItens].totalItem   = subtotal;
            item[totalItens].pagamento   = 'a';
            totalItens++;

            printf("\n  Subtotal: R$ %.2f  |  Total carrinho: R$ %.2f\n", subtotal, totalCarrinho);
            break;
        }

        if (!encontrou)
            printf("✘ Produto nao encontrado!\n");

        printf("Novo item no carrinho? (s/n): ");
        scanf(" %c", &resposta);

    } while (resposta == 's' || resposta == 'S');

    if (totalCarrinho == 0.0f || totalItens == inicioItens)
    {
        printf("\nNenhum item no carrinho. Venda cancelada.\n");
        return;
    }

    printf("\n");
    printf("╔══════════╦══════════════════════╦════════════╦══════╦════════════╗\n");
    printf("║  CODIGO  ║      DESCRICAO       ║   PRECO    ║ QTD  ║   TOTAL    ║\n");
    printf("╠══════════╬══════════════════════╬════════════╬══════╬════════════╣\n");
    for (int i = inicioItens; i < totalItens; i++)
    {
        printf("║ %-8d ║ %-20s ║ R$%-7.2f ║ %-4d ║ R$%-7.2f ║\n",
               item[i].codigoProduto, item[i].descricao,
               item[i].precoVenda, item[i].quantidade, item[i].totalItem);
    }
    printf("╠══════════╩══════════════════════╩════════════╩══════╩════════════╣\n");
    printf("║                                Total carrinho: R$ %-12.2f ║\n", totalCarrinho);
    printf("╚══════════════════════════════════════════════════════════════════╝\n");

    float desconto;
    printf("\nHa desconto? Informe 0 (para nao) ou %% concedido: ");
    scanf("%f", &desconto);
    float totalFinal = aplicarDesconto(totalCarrinho, desconto);
    printf("Total final: R$ %.2f\n", totalFinal);

    int pago = 0;
    while (!pago)
    {
        printf("\n╔══════════════════════════════╗\n");
        printf("║         PAGAMENTO            ║\n");
        printf("╠══════════════════════════════╣\n");
        printf("║  [1] 💳 Pagamento no Cartao  ║\n");
        printf("║  [2] 💵 Pagamento em Dinheiro║\n");
        printf("║  [3] ↩️  Cancelar Venda       ║\n");
        printf("╚══════════════════════════════╝\n");
        printf("➜ Opcao: ");

        int formaPagamento;
        scanf("%d", &formaPagamento);

        if (formaPagamento == 3)
        {
            for (int i = inicioItens; i < totalItens; i++)
                for (int j = 0; j < totalProdutos; j++)
                    if (item[i].codigoProduto == produto[j].codigo)
                        produto[j].estoque += item[i].quantidade;
            totalItens = inicioItens;
            printf("\nVenda cancelada!\n");
            return;
        }

        if (totalPagamentos == capacidadePagamentos)
        {
            capacidadePagamentos *= 2;
            Pagamentos *tmp = realloc(pagamento, capacidadePagamentos * sizeof(Pagamentos));
            if (!tmp) { printf("Erro de memoria!\n"); return; }
            pagamento = tmp;
        }

        if (formaPagamento == 2)
        {
            float valorPago;
            printf("Valor recebido: R$ ");
            scanf("%f", &valorPago);

            if (valorPago < totalFinal - 0.001f)
            {
                float restante = totalFinal - valorPago;
                char usaCartao;
                printf("Valor insuficiente! Deseja pagar R$ %.2f restantes no cartao? (s/n): ", restante);
                scanf(" %c", &usaCartao);

                if (usaCartao == 's' || usaCartao == 'S')
                {
                    int aprovado;
                    printf("Pagamento no cartao aprovado? (1=Sim / 0=Nao): ");
                    scanf("%d", &aprovado);

                    if (aprovado == 1)
                    {
                        pagamento[totalPagamentos].numeroVenda = numeroVendaAtual;
                        pagamento[totalPagamentos].valorPago   = valorPago;
                        strcpy(pagamento[totalPagamentos].tipo, "md");
                        totalPagamentos++;

                        if (totalPagamentos == capacidadePagamentos)
                        {
                            capacidadePagamentos *= 2;
                            Pagamentos *tmp = realloc(pagamento, capacidadePagamentos * sizeof(Pagamentos));
                            if (!tmp) { printf("Erro de memoria!\n"); return; }
                            pagamento = tmp;
                        }

                        pagamento[totalPagamentos].numeroVenda = numeroVendaAtual;
                        pagamento[totalPagamentos].valorPago   = restante;
                        strcpy(pagamento[totalPagamentos].tipo, "mc");
                        totalPagamentos++;

                        caixaAtual  += valorPago;
                        faturamento += totalFinal;
                        for (int i = inicioItens; i < totalItens; i++)
                            item[i].pagamento = 'p';
                        pago = 1;
                        printf("\n✔ Pagamento misto realizado com sucesso!\n");
                    }
                    else
                    {
                        printf("Pagamento no cartao recusado. Escolha outra forma.\n");
                    }
                }
            }
            else
            {
                float troco = valorPago - totalFinal;
                printf("Troco: R$ %.2f\n", troco);

                pagamento[totalPagamentos].numeroVenda = numeroVendaAtual;
                pagamento[totalPagamentos].valorPago   = totalFinal;
                strcpy(pagamento[totalPagamentos].tipo, "d");
                totalPagamentos++;

                caixaAtual  += totalFinal;
                faturamento += totalFinal;
                for (int i = inicioItens; i < totalItens; i++)
                    item[i].pagamento = 'p';
                pago = 1;
                printf("\n✔ Pagamento em dinheiro realizado!\n");
            }
        }

        if (formaPagamento == 1)
        {
            int aprovado;
            printf("Pagamento na maquinha aprovado? (1=Sim / 0=Nao): ");
            scanf("%d", &aprovado);

            if (aprovado == 1)
            {
                pagamento[totalPagamentos].numeroVenda = numeroVendaAtual;
                pagamento[totalPagamentos].valorPago   = totalFinal;
                strcpy(pagamento[totalPagamentos].tipo, "c");
                totalPagamentos++;

                faturamento += totalFinal;
                for (int i = inicioItens; i < totalItens; i++)
                    item[i].pagamento = 'p';
                pago = 1;
                printf("\n✔ Pagamento no cartao realizado!\n");
            }
            else
            {
                char usaDinheiro;
                printf("Deseja pagar o valor em dinheiro? (s/n): ");
                scanf(" %c", &usaDinheiro);

                if (usaDinheiro == 's' || usaDinheiro == 'S')
                {
                    float valorCartao;
                    printf("Informe o valor aprovado no cartao (0 se nenhum): R$ ");
                    scanf("%f", &valorCartao);

                    if (valorCartao > 0.0f && valorCartao < totalFinal)
                    {
                        float restante = totalFinal - valorCartao;
                        float valorDinheiro;
                        printf("Valor em dinheiro (restante R$ %.2f): R$ ", restante);
                        scanf("%f", &valorDinheiro);

                        if (valorDinheiro >= restante - 0.001f)
                        {
                            float troco = valorDinheiro - restante;
                            printf("Troco: R$ %.2f\n", troco);

                            pagamento[totalPagamentos].numeroVenda = numeroVendaAtual;
                            pagamento[totalPagamentos].valorPago   = valorCartao;
                            strcpy(pagamento[totalPagamentos].tipo, "mc");
                            totalPagamentos++;

                            if (totalPagamentos == capacidadePagamentos)
                            {
                                capacidadePagamentos *= 2;
                                Pagamentos *tmp = realloc(pagamento, capacidadePagamentos * sizeof(Pagamentos));
                                if (!tmp) { printf("Erro de memoria!\n"); return; }
                                pagamento = tmp;
                            }

                            pagamento[totalPagamentos].numeroVenda = numeroVendaAtual;
                            pagamento[totalPagamentos].valorPago   = restante;
                            strcpy(pagamento[totalPagamentos].tipo, "md");
                            totalPagamentos++;

                            caixaAtual  += restante;
                            faturamento += totalFinal;
                            for (int i = inicioItens; i < totalItens; i++)
                                item[i].pagamento = 'p';
                            pago = 1;
                            printf("\n✔ Pagamento misto realizado!\n");
                        }
                        else
                        {
                            printf("✘ Valor em dinheiro insuficiente!\n");
                        }
                    }
                    else if (valorCartao == 0.0f)
                    {
                        float valorDinheiro;
                        printf("Valor em dinheiro: R$ ");
                        scanf("%f", &valorDinheiro);
                        if (valorDinheiro >= totalFinal - 0.001f)
                        {
                            float troco = valorDinheiro - totalFinal;
                            printf("Troco: R$ %.2f\n", troco);

                            pagamento[totalPagamentos].numeroVenda = numeroVendaAtual;
                            pagamento[totalPagamentos].valorPago   = totalFinal;
                            strcpy(pagamento[totalPagamentos].tipo, "d");
                            totalPagamentos++;

                            caixaAtual  += totalFinal;
                            faturamento += totalFinal;
                            for (int i = inicioItens; i < totalItens; i++)
                                item[i].pagamento = 'p';
                            pago = 1;
                            printf("\n✔ Pagamento em dinheiro realizado!\n");
                        }
                        else
                        {
                            printf("✘ Valor insuficiente!\n");
                        }
                    }
                    else
                    {
                        printf("✘ Valor invalido!\n");
                    }
                }
                else
                {
                    printf("Pagamento recusado. Escolha outra forma.\n");
                }
            }
        }
    }

    if (totalVendas == capacidadeVendas)
    {
        capacidadeVendas *= 2;
        Vendas *tmp = realloc(venda, capacidadeVendas * sizeof(Vendas));
        if (!tmp) { printf("Erro de memoria!\n"); return; }
        venda = tmp;
    }

    venda[totalVendas].numeroVenda   = numeroVendaAtual;
    venda[totalVendas].codigoCliente = codigoCliente;
    strcpy(venda[totalVendas].nomeCliente, nomeCliente);
    venda[totalVendas].dia        = dia;
    venda[totalVendas].mes        = mes;
    venda[totalVendas].ano        = ano;
    venda[totalVendas].totalVenda = totalFinal;
    totalVendas++;

    printf("\n✔ Venda #%d realizada com sucesso! Cliente: %s\n",
           numeroVendaAtual, nomeCliente);

    numeroVendaAtual++;

    salvarVendas();
    salvarItens();
    salvarPagamentos();
    salvarProdutos();
    salvarCaixa();
}

/* ============================================================
 *  PAGAR VENDA EM ABERTO
 * ============================================================ */

void pagarVendaAberta(void)
{
    int vendasAbertas[1000];
    int totalVendasAbertas = 0;

    for (int i = 0; i < totalItens; i++)
    {
        if (item[i].pagamento != 'a')
            continue;
        int jaTem = 0;
        for (int k = 0; k < totalVendasAbertas; k++)
            if (vendasAbertas[k] == item[i].numeroVenda) { jaTem = 1; break; }
        if (!jaTem)
        {
            vendasAbertas[totalVendasAbertas] = item[i].numeroVenda;
            totalVendasAbertas++;
        }
    }

    if (totalVendasAbertas == 0)
    {
        printf("\n✔ Nao ha vendas em aberto!\n");
        return;
    }

    printf("\n===== VENDAS EM ABERTO =====\n");
    for (int v = 0; v < totalVendasAbertas; v++)
    {
        int numV = vendasAbertas[v];
        float totalAberto = 0.0f;

        printf("\n--- Venda #%d ---\n", numV);
        for (int i = 0; i < totalItens; i++)
        {
            if (item[i].numeroVenda == numV && item[i].pagamento == 'a')
            {
                printf("  %s | Qtd: %d | Total: R$ %.2f\n",
                       item[i].descricao, item[i].quantidade, item[i].totalItem);
                totalAberto += item[i].totalItem;
            }
        }
        printf("  Total em aberto: R$ %.2f\n", totalAberto);
    }

    int numPagar;
    printf("\nInforme o numero da venda a pagar (0 = cancelar): ");
    scanf("%d", &numPagar);
    if (numPagar == 0) return;

    int encontrou = 0;
    float totalAberto = 0.0f;
    for (int i = 0; i < totalItens; i++)
        if (item[i].numeroVenda == numPagar && item[i].pagamento == 'a')
        {
            totalAberto += item[i].totalItem;
            encontrou = 1;
        }

    if (!encontrou)
    {
        printf("✘ Venda nao encontrada ou ja paga!\n");
        return;
    }

    printf("Total a pagar: R$ %.2f\n", totalAberto);

    int pago = 0;
    while (!pago)
    {
        printf("\n1. Cartao\n2. Dinheiro\n3. Cancelar\nEscolha: ");
        int forma;
        scanf("%d", &forma);
        if (forma == 3) return;

        if (totalPagamentos == capacidadePagamentos)
        {
            capacidadePagamentos *= 2;
            Pagamentos *tmp = realloc(pagamento, capacidadePagamentos * sizeof(Pagamentos));
            if (!tmp) { printf("Erro de memoria!\n"); return; }
            pagamento = tmp;
        }

        if (forma == 1)
        {
            int aprovado;
            printf("Aprovado na maquinha? (1=Sim / 0=Nao): ");
            scanf("%d", &aprovado);
            if (aprovado == 1)
            {
                pagamento[totalPagamentos].numeroVenda = numPagar;
                pagamento[totalPagamentos].valorPago   = totalAberto;
                strcpy(pagamento[totalPagamentos].tipo, "c");
                totalPagamentos++;
                for (int i = 0; i < totalItens; i++)
                    if (item[i].numeroVenda == numPagar && item[i].pagamento == 'a')
                        item[i].pagamento = 'p';
                faturamento += totalAberto;
                printf("\n✔ Venda #%d paga com sucesso!\n", numPagar);
                pago = 1;
            }
            else
            {
                printf("Pagamento recusado. Escolha outra forma.\n");
            }
        }

        if (forma == 2)
        {
            float valorPago;
            printf("Valor recebido: R$ ");
            scanf("%f", &valorPago);
            if (valorPago >= totalAberto - 0.001f)
            {
                float troco = valorPago - totalAberto;
                printf("Troco: R$ %.2f\n", troco);

                pagamento[totalPagamentos].numeroVenda = numPagar;
                pagamento[totalPagamentos].valorPago   = totalAberto;
                strcpy(pagamento[totalPagamentos].tipo, "d");
                totalPagamentos++;

                for (int i = 0; i < totalItens; i++)
                    if (item[i].numeroVenda == numPagar && item[i].pagamento == 'a')
                        item[i].pagamento = 'p';
                caixaAtual  += totalAberto;
                faturamento += totalAberto;
                printf("\n✔ Venda #%d paga com sucesso!\n", numPagar);
                pago = 1;
            }
            else
            {
                printf("✘ Valor insuficiente!\n");
            }
        }
    }

    salvarPagamentos();
    salvarItens();
    salvarCaixa();
}

/* ============================================================
 *  CAIXA
 * ============================================================ */

void abrirCaixa(void)
{
    if (caixaAberto == 1)
    {
        printf("\n✘ Caixa ja esta aberto!\n");
        return;
    }

    do
    {
        printf("\nValor de abertura do caixa: R$ ");
        scanf("%f", &caixaInicial);
        if (caixaInicial < 0.0f)
            printf("✘ Valor invalido! Deve ser >= 0.\n");
    } while (caixaInicial < 0.0f);

    caixaAtual  = caixaInicial;
    caixaAberto = 1;
    printf("\n✔ Caixa aberto com R$ %.2f!\n", caixaInicial);
    salvarCaixa();
}

void fecharCaixa(void)
{
    if (caixaAberto == 0)
    {
        printf("\n✘ Caixa nao esta aberto!\n");
        return;
    }

    float pagoD  = 0.0f;
    float pagoC  = 0.0f;
    float pagoMd = 0.0f;
    float pagoMc = 0.0f;

    for (int i = 0; i < totalPagamentos; i++)
    {
        if      (strcmp(pagamento[i].tipo, "d")  == 0) pagoD  += pagamento[i].valorPago;
        else if (strcmp(pagamento[i].tipo, "c")  == 0) pagoC  += pagamento[i].valorPago;
        else if (strcmp(pagamento[i].tipo, "md") == 0) pagoMd += pagamento[i].valorPago;
        else if (strcmp(pagamento[i].tipo, "mc") == 0) pagoMc += pagamento[i].valorPago;
    }

    float totalSangriasValor = 0.0f;
    for (int i = 0; i < totalSangrias; i++)
        totalSangriasValor += historicoSangrias[i];

    float ajuste = faturamento - caixaInicial - pagoD - pagoMd - pagoC - pagoMc;

    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║      FECHAMENTO DE CAIXA - RESUMO         ║\n");
    printf("╠════════════════════════════════════════════╣\n");
    printf("║  Qtd. vendas realizadas : %-17d║\n", totalVendas);
    printf("║  Faturamento total      : R$ %-14.2f║\n", faturamento);
    printf("║  Valor de abertura      : R$ %-14.2f║\n", caixaInicial);
    printf("║  Pago em dinheiro  (d)  : R$ %-14.2f║\n", pagoD);
    printf("║  Pago misto din.   (md) : R$ %-14.2f║\n", pagoMd);
    printf("║  Pago em cartao    (c)  : R$ %-14.2f║\n", pagoC);
    printf("║  Pago misto cart.  (mc) : R$ %-14.2f║\n", pagoMc);
    printf("║  Total sangrias         : R$ %-14.2f║\n", totalSangriasValor);
    printf("║  Caixa fisico atual     : R$ %-14.2f║\n", caixaAtual);
    printf("╠════════════════════════════════════════════╣\n");

    if (ajuste > 0.01f || ajuste < -0.01f)
    {
        printf("║  AJUSTE (divergencia)   : R$ %-14.2f║\n", ajuste);
        printf("╠════════════════════════════════════════════╣\n");
        printf("║  ⚠ ATENCAO: O caixa nao pode ser fechado  ║\n");
        printf("║  porque ha divergencia de valores!         ║\n");
        if (ajuste > 0.0f)
            printf("║  Situacao: SOBRA de R$ %-20.2f║\n", ajuste);
        else
            printf("║  Situacao: FALTA de R$ %-20.2f║\n", -ajuste);
        printf("╚════════════════════════════════════════════╝\n");
        caixaAberto = 0;
        salvarCaixa();
    }
    else
    {
        printf("║  AJUSTE                 : R$ %-14.2f║\n", ajuste);
        printf("╠════════════════════════════════════════════╣\n");
        printf("║  ✔ Caixa fechado corretamente!            ║\n");
        printf("╚════════════════════════════════════════════╝\n");
        caixaAberto = 0;
        salvarCaixa();
    }
}

/* ============================================================
 *  SANGRIA
 * ============================================================ */

void sangria(void)
{
    if (caixaAberto == 0)
    {
        printf("\n✘ Abra o caixa primeiro!\n");
        return;
    }

    float totalDinheiro = caixaInicial;
    for (int i = 0; i < totalPagamentos; i++)
    {
        if (strcmp(pagamento[i].tipo, "d")  == 0 ||
            strcmp(pagamento[i].tipo, "md") == 0)
            totalDinheiro += pagamento[i].valorPago;
    }
    for (int i = 0; i < totalSangrias; i++)
        totalDinheiro -= historicoSangrias[i];

    printf("\nTotal em dinheiro no caixa: R$ %.2f\n", totalDinheiro);

    float retirada;
    printf("Informe o valor da retirada: R$ ");
    scanf("%f", &retirada);

    if (retirada <= 0.0f)
    {
        printf("✘ Valor invalido! A retirada deve ser maior que zero.\n");
        return;
    }
    if ((totalDinheiro - retirada) < 50.0f)
    {
        printf("✘ Retirada invalida! Deve sobrar no minimo R$ 50,00 no caixa.\n");
        printf("  Maximo permitido para retirada: R$ %.2f\n", totalDinheiro - 50.0f);
        return;
    }

    caixaAtual -= retirada;

    if (totalSangrias < 50)
    {
        historicoSangrias[totalSangrias] = retirada;
        totalSangrias++;
    }
    else
    {
        printf("⚠ Limite de 50 sangrias atingido. Retirada realizada mas nao registrada no historico.\n");
    }

    printf("\n✔ Sangria de R$ %.2f realizada com sucesso!\n", retirada);
    printf("Saldo atual no caixa: R$ %.2f\n", caixaAtual);
    salvarCaixa();
}

void listarSangrias(void)
{
    printf("\n===== HISTORICO DE SANGRIAS =====\n");
    if (totalSangrias == 0)
    {
        printf("Nenhuma sangria realizada.\n");
        return;
    }
    float totalRetirado = 0.0f;
    for (int i = 0; i < totalSangrias; i++)
    {
        printf("Sangria %d: R$ %.2f\n", i + 1, historicoSangrias[i]);
        totalRetirado += historicoSangrias[i];
    }
    printf("─────────────────────────────────\n");
    printf("Total retirado: R$ %.2f\n", totalRetirado);
}

/* ============================================================
 *  RELATÓRIOS
 * ============================================================ */

void relatorios(void)
{
    int opcao;
    do
    {
        printf("\n");
        printf("╔══════════════════════════════════════════════╗\n");
        printf("║           CENTRAL DE RELATORIOS             ║\n");
        printf("╠══════════════════════════════════════════════╣\n");
        printf("║  [1] 👥  Relatorio de Clientes              ║\n");
        printf("║  [2] 📦  Relatorio de Produtos              ║\n");
        printf("║  [3] 🛒  Relatorio de Vendas                ║\n");
        printf("║  [4] 💸  Historico de Sangrias              ║\n");
        printf("║  [5] ↩️   Retornar                           ║\n");
        printf("╚══════════════════════════════════════════════╝\n");
        printf("➜ Opcao: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
        case 1: relatorioClientes(); break;
        case 2: relatorioProdutos(); break;
        case 3: relatorioVendas();   break;
        case 4: listarSangrias();    break;
        case 5: break;
        default: printf("\nOpcao invalida!\n"); break;
        }
    } while (opcao != 5);
}

void relatorioClientes(void)
{
    int opcao;
    printf("\n1. Listagem Alfabetica\n2. Clientes que Compraram (periodo)\n3. Voltar\nOpcao: ");
    scanf("%d", &opcao);
    if      (opcao == 1) relatorioClientesOrdenado();
    else if (opcao == 2) relatorioClientesPeriodo();
}

void relatorioClientesOrdenado(void)
{
    if (totalClientes == 0)
    {
        printf("\nNenhum cliente cadastrado.\n");
        return;
    }

    int indices[totalClientes];
    for (int i = 0; i < totalClientes; i++) indices[i] = i;

    for (int i = 0; i < totalClientes - 1; i++)
        for (int j = 0; j < totalClientes - i - 1; j++)
            if (strcmp(cliente[indices[j]].nome, cliente[indices[j+1]].nome) > 0)
            {
                int tmp = indices[j]; indices[j] = indices[j+1]; indices[j+1] = tmp;
            }

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                 CLIENTES (ORDEM ALFABETICA)                             ║\n");
    printf("╠═══════╦════════════════════════════╦═══════════════╦═══════════════════╣\n");
    printf("║ COD.  ║ NOME                       ║ CPF           ║ CELULAR           ║\n");
    printf("╠═══════╬════════════════════════════╬═══════════════╬═══════════════════╣\n");
    for (int i = 0; i < totalClientes; i++)
    {
        int k = indices[i];
        printf("║ %-5d ║ %-26s ║ %-13s ║ %-17s ║\n",
               cliente[k].codigo, cliente[k].nome, cliente[k].cpf, cliente[k].celular);
    }
    printf("╚═══════╩════════════════════════════╩═══════════════╩═══════════════════╝\n");
}

void relatorioClientesPeriodo(void)
{
    int diaIni, mesIni, anoIni, diaFim, mesFim, anoFim;
    printf("\nData inicial (dd mm aaaa): ");
    scanf("%d %d %d", &diaIni, &mesIni, &anoIni);
    printf("Data final   (dd mm aaaa): ");
    scanf("%d %d %d", &diaFim, &mesFim, &anoFim);

    int dataIni = anoIni * 10000 + mesIni * 100 + diaIni;
    int dataFim = anoFim * 10000 + mesFim * 100 + diaFim;

    printf("\n===== CLIENTES QUE COMPRARAM NO PERIODO =====\n");
    int encontrou = 0;

    for (int i = 0; i < totalVendas; i++)
    {
        int dataVenda = venda[i].ano * 10000 + venda[i].mes * 100 + venda[i].dia;
        if (dataVenda < dataIni || dataVenda > dataFim || venda[i].codigoCliente == 0)
            continue;

        printf("Venda #%-4d | %02d/%02d/%04d | Cliente: %-25s | Total: R$ %.2f\n",
               venda[i].numeroVenda, venda[i].dia, venda[i].mes, venda[i].ano,
               venda[i].nomeCliente, venda[i].totalVenda);
        encontrou = 1;
    }

    if (!encontrou)
        printf("Nenhum cliente encontrado no periodo.\n");
}

void relatorioProdutos(void)
{
    int opcao;
    printf("\n1. Listagem Alfabetica\n2. Estoque Zero ou Minimo\n3. Mais Vendidos (periodo)\n4. Voltar\nOpcao: ");
    scanf("%d", &opcao);
    if      (opcao == 1) relatorioProdutosOrdenado();
    else if (opcao == 2) relatorioProdutosEstoqueMinimo();
    else if (opcao == 3) relatorioProdutosMaisVendidos();
}

void relatorioProdutosOrdenado(void)
{
    if (totalProdutos == 0)
    {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    int indices[totalProdutos];
    for (int i = 0; i < totalProdutos; i++) indices[i] = i;

    for (int i = 0; i < totalProdutos - 1; i++)
        for (int j = 0; j < totalProdutos - i - 1; j++)
            if (strcmp(produto[indices[j]].descricao, produto[indices[j+1]].descricao) > 0)
            {
                int tmp = indices[j]; indices[j] = indices[j+1]; indices[j+1] = tmp;
            }

    printf("\n");
    printf("╔════════╦════════════════════════╦══════════════════╦════════════╦═════════╗\n");
    printf("║ CODIGO ║ DESCRICAO              ║ CATEGORIA        ║   PRECO    ║ ESTOQUE ║\n");
    printf("╠════════╬════════════════════════╬══════════════════╬════════════╬═════════╣\n");
    for (int i = 0; i < totalProdutos; i++)
    {
        int k = indices[i];
        printf("║ %-6d ║ %-22s ║ %-16s ║ R$%-7.2f ║ %-7d ║\n",
               produto[k].codigo, produto[k].descricao, produto[k].categoria,
               produto[k].precoVenda, produto[k].estoque);
    }
    printf("╚════════╩════════════════════════╩══════════════════╩════════════╩═════════╝\n");
}

void relatorioProdutosEstoqueMinimo(void)
{
    int indices[totalProdutos];
    int total = 0;

    for (int i = 0; i < totalProdutos; i++)
        if (produto[i].estoque <= produto[i].estoqueMinimo)
            indices[total++] = i;

    printf("\n===== PRODUTOS COM ESTOQUE ZERO OU MINIMO =====\n");

    if (total == 0)
    {
        printf("Nenhum produto com estoque critico.\n");
        return;
    }

    for (int i = 0; i < total - 1; i++)
        for (int j = 0; j < total - i - 1; j++)
            if (strcmp(produto[indices[j]].descricao, produto[indices[j+1]].descricao) > 0)
            {
                int tmp = indices[j]; indices[j] = indices[j+1]; indices[j+1] = tmp;
            }

    printf("%-8s %-24s %8s %8s %s\n", "CODIGO", "DESCRICAO", "ESTOQUE", "MINIMO", "STATUS");
    printf("──────────────────────────────────────────────────────────────\n");
    for (int i = 0; i < total; i++)
    {
        int k = indices[i];
        printf("%-8d %-24s %8d %8d %s\n",
               produto[k].codigo, produto[k].descricao,
               produto[k].estoque, produto[k].estoqueMinimo,
               produto[k].estoque == 0 ? "*** ZERADO ***" : "*** MINIMO ***");
    }
}

void relatorioProdutosMaisVendidos(void)
{
    int diaIni, mesIni, anoIni, diaFim, mesFim, anoFim;
    printf("\nData inicial (dd mm aaaa): ");
    scanf("%d %d %d", &diaIni, &mesIni, &anoIni);
    printf("Data final   (dd mm aaaa): ");
    scanf("%d %d %d", &diaFim, &mesFim, &anoFim);

    int dataIni = anoIni * 10000 + mesIni * 100 + diaIni;
    int dataFim = anoFim * 10000 + mesFim * 100 + diaFim;

    int qtdVendida[totalProdutos];
    for (int i = 0; i < totalProdutos; i++) qtdVendida[i] = 0;

    for (int v = 0; v < totalVendas; v++)
    {
        int dataVenda = venda[v].ano * 10000 + venda[v].mes * 100 + venda[v].dia;
        if (dataVenda < dataIni || dataVenda > dataFim) continue;

        for (int k = 0; k < totalItens; k++)
        {
            if (item[k].numeroVenda != venda[v].numeroVenda) continue;
            for (int p = 0; p < totalProdutos; p++)
            {
                if (produto[p].codigo == item[k].codigoProduto)
                {
                    qtdVendida[p] += item[k].quantidade;
                    break;
                }
            }
        }
    }

    int indices[totalProdutos];
    for (int i = 0; i < totalProdutos; i++) indices[i] = i;

    for (int i = 0; i < totalProdutos - 1; i++)
        for (int j = 0; j < totalProdutos - i - 1; j++)
            if (qtdVendida[indices[j]] < qtdVendida[indices[j+1]])
            {
                int tmp = indices[j]; indices[j] = indices[j+1]; indices[j+1] = tmp;
            }

    printf("\n===== PRODUTOS MAIS VENDIDOS NO PERIODO =====\n");
    printf("%-24s %12s\n", "DESCRICAO", "QTD VENDIDA");
    printf("────────────────────────────────────────\n");
    int algum = 0;
    for (int i = 0; i < totalProdutos; i++)
    {
        int k = indices[i];
        if (qtdVendida[k] == 0) break;
        printf("%-24s %12d\n", produto[k].descricao, qtdVendida[k]);
        algum = 1;
    }
    if (!algum) printf("Nenhum produto vendido no periodo.\n");
}

void relatorioVendas(void)
{
    int opcao;
    printf("\n1. Vendas por Periodo\n2. Faturamento Consolidado\n3. Voltar\nOpcao: ");
    scanf("%d", &opcao);
    if      (opcao == 1) relatorioVendasPeriodo();
    else if (opcao == 2) relatorioFaturamentoPeriodo();
}

void relatorioVendasPeriodo(void)
{
    int diaIni, mesIni, anoIni, diaFim, mesFim, anoFim;
    printf("\nData inicial (dd mm aaaa): ");
    scanf("%d %d %d", &diaIni, &mesIni, &anoIni);
    printf("Data final   (dd mm aaaa): ");
    scanf("%d %d %d", &diaFim, &mesFim, &anoFim);

    int dataIni = anoIni * 10000 + mesIni * 100 + diaIni;
    int dataFim = anoFim * 10000 + mesFim * 100 + diaFim;

    printf("\n===== VENDAS NO PERIODO =====\n");
    int   encontrou    = 0;
    float totalPeriodo = 0.0f;

    for (int i = 0; i < totalVendas; i++)
    {
        int dataVenda = venda[i].ano * 10000 + venda[i].mes * 100 + venda[i].dia;
        if (dataVenda < dataIni || dataVenda > dataFim) continue;

        printf("\nVenda #%-4d | %02d/%02d/%04d | Cliente: %s | Total: R$ %.2f\n",
               venda[i].numeroVenda, venda[i].dia, venda[i].mes, venda[i].ano,
               venda[i].nomeCliente, venda[i].totalVenda);

        for (int k = 0; k < totalItens; k++)
            if (item[k].numeroVenda == venda[i].numeroVenda)
                printf("   → %-20s | Qtd: %d | R$ %.2f\n",
                       item[k].descricao, item[k].quantidade, item[k].totalItem);

        totalPeriodo += venda[i].totalVenda;
        encontrou = 1;
    }

    if (!encontrou)
        printf("Nenhuma venda no periodo.\n");
    else
    {
        printf("\n──────────────────────────────────────────\n");
        printf("Total faturado no periodo: R$ %.2f\n", totalPeriodo);
    }
}

void relatorioFaturamentoPeriodo(void)
{
    int diaIni, mesIni, anoIni, diaFim, mesFim, anoFim;
    printf("\nData inicial (dd mm aaaa): ");
    scanf("%d %d %d", &diaIni, &mesIni, &anoIni);
    printf("Data final   (dd mm aaaa): ");
    scanf("%d %d %d", &diaFim, &mesFim, &anoFim);

    int dataIni = anoIni * 10000 + mesIni * 100 + diaIni;
    int dataFim = anoFim * 10000 + mesFim * 100 + diaFim;

    float totalD = 0.0f, totalC = 0.0f, totalMd = 0.0f, totalMc = 0.0f;

    for (int i = 0; i < totalVendas; i++)
    {
        int dataVenda = venda[i].ano * 10000 + venda[i].mes * 100 + venda[i].dia;
        if (dataVenda < dataIni || dataVenda > dataFim) continue;

        for (int k = 0; k < totalPagamentos; k++)
        {
            if (pagamento[k].numeroVenda != venda[i].numeroVenda) continue;
            if      (strcmp(pagamento[k].tipo, "d")  == 0) totalD  += pagamento[k].valorPago;
            else if (strcmp(pagamento[k].tipo, "c")  == 0) totalC  += pagamento[k].valorPago;
            else if (strcmp(pagamento[k].tipo, "md") == 0) totalMd += pagamento[k].valorPago;
            else if (strcmp(pagamento[k].tipo, "mc") == 0) totalMc += pagamento[k].valorPago;
        }
    }

    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║       FATURAMENTO CONSOLIDADO              ║\n");
    printf("╠════════════════════════════════════════════╣\n");
    printf("║  Dinheiro puro          : R$ %-14.2f║\n", totalD);
    printf("║  Misto (parte dinheiro) : R$ %-14.2f║\n", totalMd);
    printf("║  TOTAL EM DINHEIRO      : R$ %-14.2f║\n", totalD + totalMd);
    printf("╠════════════════════════════════════════════╣\n");
    printf("║  Cartao puro            : R$ %-14.2f║\n", totalC);
    printf("║  Misto (parte cartao)   : R$ %-14.2f║\n", totalMc);
    printf("║  TOTAL EM CARTAO        : R$ %-14.2f║\n", totalC + totalMc);
    printf("╠════════════════════════════════════════════╣\n");
    printf("║  TOTAL GERAL            : R$ %-14.2f║\n", totalD + totalMd + totalC + totalMc);
    printf("╚════════════════════════════════════════════╝\n");
}

/* ============================================================
 *  PERSISTÊNCIA EM ARQUIVO TEXTO (.txt) — FORMATO TABELA
 *
 *  Cada arquivo .txt agora e gravado como uma TABELA alinhada,
 *  com cabecalho e bordas, no estilo:
 *
 *  +------------+----------+----------------+
 *  | LOGIN      | SENHA    | TIPO           |
 *  +------------+----------+----------------+
 *  | admin12345 | senha123 | Administrador  |
 *  +------------+----------+----------------+
 *
 *  Isso e so para facilitar a LEITURA HUMANA do arquivo (abrindo
 *  no Notepad, por exemplo). Para o PROGRAMA conseguir ler esses
 *  dados de volta ao abrir, cada valor de cada linha de dados e
 *  marcado internamente com um separador "|" (igual a tabela
 *  visual), e a funcao de carregar sabe pular as linhas de borda
 *  (+----+) e cabecalho, lendo somente as linhas de dados.
 * ============================================================ */

/* ---------- Usuários ---------- */

/*
 * Salva usuarios em "usuarios.txt" em formato de tabela.
 * Colunas: LOGIN | SENHA | TIPO
 */
void salvarUsuarios(void)
{
    FILE *f = fopen("usuarios.txt", "w");
    if (!f) { printf("Erro ao salvar usuarios!\n"); return; }

    /* primeira linha "tecnica" com a contagem, para carregamento rapido */
    fprintf(f, "%d\n", totalUsuarios);

    if (totalUsuarios == 0)
    {
        fprintf(f, "(Nenhum usuario cadastrado)\n");
        fclose(f);
        return;
    }

    const char *tiposTexto[totalUsuarios];
    for (int i = 0; i < totalUsuarios; i++)
        tiposTexto[i] = (usuario[i].tipo == 1) ? "Administrador" : "Usuario";

    const char *loginsPtr[totalUsuarios];
    const char *senhasPtr[totalUsuarios];
    for (int i = 0; i < totalUsuarios; i++)
    {
        loginsPtr[i] = usuario[i].login;
        senhasPtr[i] = usuario[i].senha;
    }

    int lgLogin = larguraColunaTexto(loginsPtr, totalUsuarios, 5, "LOGIN");
    int lgSenha = larguraColunaTexto(senhasPtr, totalUsuarios, 5, "SENHA");
    int lgTipo  = larguraColunaTexto(tiposTexto, totalUsuarios, 4, "TIPO");

    int larguras[3] = { lgLogin, lgSenha, lgTipo };

    escreverLinhaSeparadora(f, larguras, 3);
    escreverCelulaTexto(f, "LOGIN", lgLogin);
    escreverCelulaTexto(f, "SENHA", lgSenha);
    escreverCelulaTexto(f, "TIPO", lgTipo);
    fprintf(f, "|\n");
    escreverLinhaSeparadora(f, larguras, 3);

    for (int i = 0; i < totalUsuarios; i++)
    {
        escreverCelulaTexto(f, usuario[i].login, lgLogin);
        escreverCelulaTexto(f, usuario[i].senha, lgSenha);
        escreverCelulaTexto(f, tiposTexto[i], lgTipo);
        fprintf(f, "|\n");
    }
    escreverLinhaSeparadora(f, larguras, 3);

    fclose(f);
}

/*
 * Carrega usuarios de "usuarios.txt".
 * Le a contagem na primeira linha, depois pula linhas de borda
 * (que comecam com '+') e o cabecalho, lendo somente as linhas
 * de dados (que comecam com '|').
 */
void carregarUsuarios(void)
{
    FILE *f = fopen("usuarios.txt", "r");
    if (!f) return;

    fscanf(f, "%d\n", &totalUsuarios);

    if (totalUsuarios == 0)
    {
        fclose(f);
        return;
    }

    if (totalUsuarios > capacidadeUsuarios)
    {
        capacidadeUsuarios = totalUsuarios * 2;
        Usuario *tmp = realloc(usuario, capacidadeUsuarios * sizeof(Usuario));
        if (tmp) usuario = tmp;
    }

    char linha[1024];
    int  lidos = 0;

    while (lidos < totalUsuarios && fgets(linha, sizeof(linha), f))
    {
        /* pula linhas de borda (+----+) e o cabecalho (| LOGIN | SENHA | TIPO |) */
        if (linha[0] != '|')
            continue;
        if (strstr(linha, "LOGIN") != NULL)
            continue;

        char loginTmp[100] = "";
        char senhaTmp[9]   = "";
        char tipoTmp[20]   = "";

        char *campo = strtok(linha, "|");
        int   col   = 0;
        while (campo != NULL)
        {
            /* remove espacos nas pontas do campo */
            while (*campo == ' ') campo++;
            int len = (int)strlen(campo);
            while (len > 0 && (campo[len-1] == ' ' || campo[len-1] == '\n' || campo[len-1] == '\r'))
            {
                campo[len-1] = '\0';
                len--;
            }

            if      (col == 0) strncpy(loginTmp, campo, sizeof(loginTmp) - 1);
            else if (col == 1) strncpy(senhaTmp, campo, sizeof(senhaTmp) - 1);
            else if (col == 2) strncpy(tipoTmp,  campo, sizeof(tipoTmp)  - 1);

            campo = strtok(NULL, "|");
            col++;
        }

        strcpy(usuario[lidos].login, loginTmp);
        strcpy(usuario[lidos].senha, senhaTmp);
        usuario[lidos].tipo = (strcmp(tipoTmp, "Administrador") == 0) ? 1 : 2;

        lidos++;
    }

    totalUsuarios = lidos;
    fclose(f);
}

/* ---------- Clientes ---------- */

/*
 * Salva clientes em "clientes.txt" em formato de tabela.
 * Colunas: CODIGO | NOME | NOME SOCIAL | CPF | RUA | BAIRRO | CELULAR
 */
void salvarClientes(void)
{
    FILE *f = fopen("clientes.txt", "w");
    if (!f) { printf("Erro ao salvar clientes!\n"); return; }

    fprintf(f, "%d\n", totalClientes);

    if (totalClientes == 0)
    {
        fprintf(f, "(Nenhum cliente cadastrado)\n");
        fclose(f);
        return;
    }

    char codigosTexto[totalClientes][16];
    const char *codigosPtr[totalClientes];
    const char *nomesPtr[totalClientes];
    const char *sociaisPtr[totalClientes];
    const char *cpfsPtr[totalClientes];
    const char *ruasPtr[totalClientes];
    const char *bairrosPtr[totalClientes];
    const char *celularesPtr[totalClientes];

    for (int i = 0; i < totalClientes; i++)
    {
        sprintf(codigosTexto[i], "%d", cliente[i].codigo);
        codigosPtr[i]   = codigosTexto[i];
        nomesPtr[i]     = cliente[i].nome;
        sociaisPtr[i]   = cliente[i].nomeSocial;
        cpfsPtr[i]      = cliente[i].cpf;
        ruasPtr[i]      = cliente[i].rua;
        bairrosPtr[i]   = cliente[i].bairro;
        celularesPtr[i] = cliente[i].celular;
    }

    int lgCodigo = larguraColunaTexto(codigosPtr,   totalClientes, 6,  "CODIGO");
    int lgNome   = larguraColunaTexto(nomesPtr,     totalClientes, 20, "NOME");
    int lgSocial = larguraColunaTexto(sociaisPtr,   totalClientes, 11, "NOME SOCIAL");
    int lgCpf    = larguraColunaTexto(cpfsPtr,      totalClientes, 14, "CPF");
    int lgRua    = larguraColunaTexto(ruasPtr,      totalClientes, 20, "RUA");
    int lgBairro = larguraColunaTexto(bairrosPtr,   totalClientes, 10, "BAIRRO");
    int lgCel    = larguraColunaTexto(celularesPtr, totalClientes, 13, "CELULAR");

    int larguras[7] = { lgCodigo, lgNome, lgSocial, lgCpf, lgRua, lgBairro, lgCel };

    escreverLinhaSeparadora(f, larguras, 7);
    escreverCelulaTexto(f, "CODIGO", lgCodigo);
    escreverCelulaTexto(f, "NOME", lgNome);
    escreverCelulaTexto(f, "NOME SOCIAL", lgSocial);
    escreverCelulaTexto(f, "CPF", lgCpf);
    escreverCelulaTexto(f, "RUA", lgRua);
    escreverCelulaTexto(f, "BAIRRO", lgBairro);
    escreverCelulaTexto(f, "CELULAR", lgCel);
    fprintf(f, "|\n");
    escreverLinhaSeparadora(f, larguras, 7);

    for (int i = 0; i < totalClientes; i++)
    {
        escreverCelulaTexto(f, codigosPtr[i],   lgCodigo);
        escreverCelulaTexto(f, nomesPtr[i],     lgNome);
        escreverCelulaTexto(f, sociaisPtr[i],   lgSocial);
        escreverCelulaTexto(f, cpfsPtr[i],      lgCpf);
        escreverCelulaTexto(f, ruasPtr[i],      lgRua);
        escreverCelulaTexto(f, bairrosPtr[i],   lgBairro);
        escreverCelulaTexto(f, celularesPtr[i], lgCel);
        fprintf(f, "|\n");
    }
    escreverLinhaSeparadora(f, larguras, 7);

    fclose(f);
}

/*
 * Carrega clientes de "clientes.txt".
 */
void carregarClientes(void)
{
    FILE *f = fopen("clientes.txt", "r");
    if (!f) return;

    fscanf(f, "%d\n", &totalClientes);

    if (totalClientes == 0)
    {
        fclose(f);
        return;
    }

    if (totalClientes > capacidadeClientes)
    {
        capacidadeClientes = totalClientes * 2;
        Clientes *tmp = realloc(cliente, capacidadeClientes * sizeof(Clientes));
        if (tmp) cliente = tmp;
    }

    char linha[1024];
    int  lidos = 0;

    while (lidos < totalClientes && fgets(linha, sizeof(linha), f))
    {
        if (linha[0] != '|')
            continue;
        if (strstr(linha, "CODIGO") != NULL)
            continue;

        char campos[7][100];
        for (int c = 0; c < 7; c++) campos[c][0] = '\0';

        char *campo = strtok(linha, "|");
        int   col   = 0;
        while (campo != NULL && col < 7)
        {
            while (*campo == ' ') campo++;
            int len = (int)strlen(campo);
            while (len > 0 && (campo[len-1] == ' ' || campo[len-1] == '\n' || campo[len-1] == '\r'))
            {
                campo[len-1] = '\0';
                len--;
            }
            strncpy(campos[col], campo, sizeof(campos[col]) - 1);

            campo = strtok(NULL, "|");
            col++;
        }

        cliente[lidos].codigo = atoi(campos[0]);
        strcpy(cliente[lidos].nome,       campos[1]);
        strcpy(cliente[lidos].nomeSocial, campos[2]);
        strcpy(cliente[lidos].cpf,        campos[3]);
        strcpy(cliente[lidos].rua,        campos[4]);
        strcpy(cliente[lidos].bairro,     campos[5]);
        strcpy(cliente[lidos].celular,    campos[6]);

        lidos++;
    }

    totalClientes = lidos;
    fclose(f);
}

/* ---------- Produtos ---------- */

/*
 * Salva produtos em "produtos.txt" em formato de tabela.
 * Colunas: CODIGO | DESCRICAO | CATEGORIA | PRECO COMPRA | MARGEM % |
 *          PRECO VENDA | ESTOQUE | ESTOQUE MINIMO
 */
void salvarProdutos(void)
{
    FILE *f = fopen("produtos.txt", "w");
    if (!f) { printf("Erro ao salvar produtos!\n"); return; }

    fprintf(f, "%d\n", totalProdutos);

    if (totalProdutos == 0)
    {
        fprintf(f, "(Nenhum produto cadastrado)\n");
        fclose(f);
        return;
    }

    char codigosTexto[totalProdutos][16];
    char compraTexto[totalProdutos][20];
    char margemTexto[totalProdutos][20];
    char vendaTexto[totalProdutos][20];
    char estoqueTexto[totalProdutos][16];
    char minimoTexto[totalProdutos][16];

    const char *codigosPtr[totalProdutos];
    const char *descricoesPtr[totalProdutos];
    const char *categoriasPtr[totalProdutos];
    const char *compraPtr[totalProdutos];
    const char *margemPtr[totalProdutos];
    const char *vendaPtr[totalProdutos];
    const char *estoquePtr[totalProdutos];
    const char *minimoPtr[totalProdutos];

    for (int i = 0; i < totalProdutos; i++)
    {
        sprintf(codigosTexto[i], "%d", produto[i].codigo);
        sprintf(compraTexto[i],  "R$ %.2f", produto[i].precoCompra);
        sprintf(margemTexto[i],  "%.2f%%",  produto[i].margemLucro);
        sprintf(vendaTexto[i],   "R$ %.2f", produto[i].precoVenda);
        sprintf(estoqueTexto[i], "%d", produto[i].estoque);
        sprintf(minimoTexto[i],  "%d", produto[i].estoqueMinimo);

        codigosPtr[i]    = codigosTexto[i];
        descricoesPtr[i] = produto[i].descricao;
        categoriasPtr[i] = produto[i].categoria;
        compraPtr[i]     = compraTexto[i];
        margemPtr[i]     = margemTexto[i];
        vendaPtr[i]      = vendaTexto[i];
        estoquePtr[i]    = estoqueTexto[i];
        minimoPtr[i]     = minimoTexto[i];
    }

    int lgCodigo = larguraColunaTexto(codigosPtr,    totalProdutos, 6,  "CODIGO");
    int lgDesc   = larguraColunaTexto(descricoesPtr, totalProdutos, 20, "DESCRICAO");
    int lgCat    = larguraColunaTexto(categoriasPtr, totalProdutos, 12, "CATEGORIA");
    int lgCompra = larguraColunaTexto(compraPtr,     totalProdutos, 12, "PRECO COMPRA");
    int lgMargem = larguraColunaTexto(margemPtr,     totalProdutos, 8,  "MARGEM %");
    int lgVenda  = larguraColunaTexto(vendaPtr,      totalProdutos, 11, "PRECO VENDA");
    int lgEst    = larguraColunaTexto(estoquePtr,    totalProdutos, 7,  "ESTOQUE");
    int lgMin    = larguraColunaTexto(minimoPtr,     totalProdutos, 14, "ESTOQUE MINIMO");

    int larguras[8] = { lgCodigo, lgDesc, lgCat, lgCompra, lgMargem, lgVenda, lgEst, lgMin };

    escreverLinhaSeparadora(f, larguras, 8);
    escreverCelulaTexto(f, "CODIGO", lgCodigo);
    escreverCelulaTexto(f, "DESCRICAO", lgDesc);
    escreverCelulaTexto(f, "CATEGORIA", lgCat);
    escreverCelulaTexto(f, "PRECO COMPRA", lgCompra);
    escreverCelulaTexto(f, "MARGEM %", lgMargem);
    escreverCelulaTexto(f, "PRECO VENDA", lgVenda);
    escreverCelulaTexto(f, "ESTOQUE", lgEst);
    escreverCelulaTexto(f, "ESTOQUE MINIMO", lgMin);
    fprintf(f, "|\n");
    escreverLinhaSeparadora(f, larguras, 8);

    for (int i = 0; i < totalProdutos; i++)
    {
        escreverCelulaTexto(f, codigosPtr[i],    lgCodigo);
        escreverCelulaTexto(f, descricoesPtr[i], lgDesc);
        escreverCelulaTexto(f, categoriasPtr[i], lgCat);
        escreverCelulaTexto(f, compraPtr[i],     lgCompra);
        escreverCelulaTexto(f, margemPtr[i],     lgMargem);
        escreverCelulaTexto(f, vendaPtr[i],      lgVenda);
        escreverCelulaTexto(f, estoquePtr[i],    lgEst);
        escreverCelulaTexto(f, minimoPtr[i],     lgMin);
        fprintf(f, "|\n");
    }
    escreverLinhaSeparadora(f, larguras, 8);

    fclose(f);
}

/*
 * Carrega produtos de "produtos.txt".
 */
void carregarProdutos(void)
{
    FILE *f = fopen("produtos.txt", "r");
    if (!f) return;

    fscanf(f, "%d\n", &totalProdutos);

    if (totalProdutos == 0)
    {
        fclose(f);
        return;
    }

    if (totalProdutos > capacidadeProdutos)
    {
        capacidadeProdutos = totalProdutos * 2;
        Produtos *tmp = realloc(produto, capacidadeProdutos * sizeof(Produtos));
        if (tmp) produto = tmp;
    }

    char linha[1024];
    int  lidos = 0;

    while (lidos < totalProdutos && fgets(linha, sizeof(linha), f))
    {
        if (linha[0] != '|')
            continue;
        if (strstr(linha, "CODIGO") != NULL)
            continue;

        char campos[8][100];
        for (int c = 0; c < 8; c++) campos[c][0] = '\0';

        char *campo = strtok(linha, "|");
        int   col   = 0;
        while (campo != NULL && col < 8)
        {
            while (*campo == ' ') campo++;
            int len = (int)strlen(campo);
            while (len > 0 && (campo[len-1] == ' ' || campo[len-1] == '\n' || campo[len-1] == '\r'))
            {
                campo[len-1] = '\0';
                len--;
            }
            strncpy(campos[col], campo, sizeof(campos[col]) - 1);

            campo = strtok(NULL, "|");
            col++;
        }

        produto[lidos].codigo = atoi(campos[0]);
        strcpy(produto[lidos].descricao, campos[1]);
        strcpy(produto[lidos].categoria, campos[2]);

        /* remove "R$ " e "%" antes de converter para float */
        char *pCompra = campos[3];
        if (strncmp(pCompra, "R$ ", 3) == 0) pCompra += 3;
        produto[lidos].precoCompra = (float)atof(pCompra);

        char *pMargem = campos[4];
        produto[lidos].margemLucro = (float)atof(pMargem); /* atof ignora o '%' no final */

        char *pVenda = campos[5];
        if (strncmp(pVenda, "R$ ", 3) == 0) pVenda += 3;
        produto[lidos].precoVenda = (float)atof(pVenda);

        produto[lidos].estoque       = atoi(campos[6]);
        produto[lidos].estoqueMinimo = atoi(campos[7]);

        lidos++;
    }

    totalProdutos = lidos;
    fclose(f);
}

/* ---------- Vendas (cabeçalho) ---------- */

/*
 * Salva cabeçalhos de vendas em "vendas.txt" em formato de tabela.
 * Inclui, antes da tabela: totalVendas, numeroVendaAtual, faturamento.
 * Colunas: NUMERO | COD. CLIENTE | NOME CLIENTE | DATA | TOTAL
 */
void salvarVendas(void)
{
    FILE *f = fopen("vendas.txt", "w");
    if (!f) { printf("Erro ao salvar vendas!\n"); return; }

    fprintf(f, "%d\n",   totalVendas);
    fprintf(f, "%d\n",   numeroVendaAtual);
    fprintf(f, "%.2f\n", faturamento);

    if (totalVendas == 0)
    {
        fprintf(f, "(Nenhuma venda registrada)\n");
        fclose(f);
        return;
    }

    char numerosTexto[totalVendas][16];
    char codClienteTexto[totalVendas][16];
    char datasTexto[totalVendas][16];
    char totalTexto[totalVendas][20];

    const char *numerosPtr[totalVendas];
    const char *codClientePtr[totalVendas];
    const char *nomesPtr[totalVendas];
    const char *datasPtr[totalVendas];
    const char *totalPtr[totalVendas];

    for (int i = 0; i < totalVendas; i++)
    {
        sprintf(numerosTexto[i],    "%d", venda[i].numeroVenda);
        sprintf(codClienteTexto[i], "%d", venda[i].codigoCliente);
        sprintf(datasTexto[i],      "%02d/%02d/%04d", venda[i].dia, venda[i].mes, venda[i].ano);
        sprintf(totalTexto[i],      "R$ %.2f", venda[i].totalVenda);

        numerosPtr[i]    = numerosTexto[i];
        codClientePtr[i] = codClienteTexto[i];
        nomesPtr[i]      = venda[i].nomeCliente;
        datasPtr[i]      = datasTexto[i];
        totalPtr[i]      = totalTexto[i];
    }

    int lgNum    = larguraColunaTexto(numerosPtr,    totalVendas, 6,  "NUMERO");
    int lgCod    = larguraColunaTexto(codClientePtr, totalVendas, 11, "COD. CLIENTE");
    int lgNome   = larguraColunaTexto(nomesPtr,      totalVendas, 20, "NOME CLIENTE");
    int lgData   = larguraColunaTexto(datasPtr,      totalVendas, 10, "DATA");
    int lgTotal  = larguraColunaTexto(totalPtr,      totalVendas, 10, "TOTAL");

    int larguras[5] = { lgNum, lgCod, lgNome, lgData, lgTotal };

    escreverLinhaSeparadora(f, larguras, 5);
    escreverCelulaTexto(f, "NUMERO", lgNum);
    escreverCelulaTexto(f, "COD. CLIENTE", lgCod);
    escreverCelulaTexto(f, "NOME CLIENTE", lgNome);
    escreverCelulaTexto(f, "DATA", lgData);
    escreverCelulaTexto(f, "TOTAL", lgTotal);
    fprintf(f, "|\n");
    escreverLinhaSeparadora(f, larguras, 5);

    for (int i = 0; i < totalVendas; i++)
    {
        escreverCelulaTexto(f, numerosPtr[i],    lgNum);
        escreverCelulaTexto(f, codClientePtr[i], lgCod);
        escreverCelulaTexto(f, nomesPtr[i],      lgNome);
        escreverCelulaTexto(f, datasPtr[i],      lgData);
        escreverCelulaTexto(f, totalPtr[i],      lgTotal);
        fprintf(f, "|\n");
    }
    escreverLinhaSeparadora(f, larguras, 5);

    fclose(f);
}

/*
 * Carrega cabeçalhos de vendas de "vendas.txt".
 */
void carregarVendas(void)
{
    FILE *f = fopen("vendas.txt", "r");
    if (!f) return;

    fscanf(f, "%d\n",   &totalVendas);
    fscanf(f, "%d\n",   &numeroVendaAtual);
    fscanf(f, "%f\n",   &faturamento);

    if (totalVendas == 0)
    {
        fclose(f);
        return;
    }

    if (totalVendas > capacidadeVendas)
    {
        capacidadeVendas = totalVendas * 2;
        Vendas *tmp = realloc(venda, capacidadeVendas * sizeof(Vendas));
        if (tmp) venda = tmp;
    }

    char linha[1024];
    int  lidos = 0;

    while (lidos < totalVendas && fgets(linha, sizeof(linha), f))
    {
        if (linha[0] != '|')
            continue;
        if (strstr(linha, "NUMERO") != NULL)
            continue;

        char campos[5][100];
        for (int c = 0; c < 5; c++) campos[c][0] = '\0';

        char *campo = strtok(linha, "|");
        int   col   = 0;
        while (campo != NULL && col < 5)
        {
            while (*campo == ' ') campo++;
            int len = (int)strlen(campo);
            while (len > 0 && (campo[len-1] == ' ' || campo[len-1] == '\n' || campo[len-1] == '\r'))
            {
                campo[len-1] = '\0';
                len--;
            }
            strncpy(campos[col], campo, sizeof(campos[col]) - 1);

            campo = strtok(NULL, "|");
            col++;
        }

        venda[lidos].numeroVenda   = atoi(campos[0]);
        venda[lidos].codigoCliente = atoi(campos[1]);
        strcpy(venda[lidos].nomeCliente, campos[2]);

        int d = 1, m = 1, a = 2000;
        sscanf(campos[3], "%d/%d/%d", &d, &m, &a);
        venda[lidos].dia = d;
        venda[lidos].mes = m;
        venda[lidos].ano = a;

        char *pTotal = campos[4];
        if (strncmp(pTotal, "R$ ", 3) == 0) pTotal += 3;
        venda[lidos].totalVenda = (float)atof(pTotal);

        lidos++;
    }

    totalVendas = lidos;
    fclose(f);
}

/* ---------- Itens de Venda ---------- */

/*
 * Salva itens de venda em "itens.txt" em formato de tabela.
 * Colunas: VENDA | COD. PRODUTO | DESCRICAO | PRECO | QTD | TOTAL | STATUS
 */
void salvarItens(void)
{
    FILE *f = fopen("itens.txt", "w");
    if (!f) { printf("Erro ao salvar itens!\n"); return; }

    fprintf(f, "%d\n", totalItens);

    if (totalItens == 0)
    {
        fprintf(f, "(Nenhum item registrado)\n");
        fclose(f);
        return;
    }

    char vendaTexto[totalItens][16];
    char codProdTexto[totalItens][16];
    char precoTexto[totalItens][20];
    char qtdTexto[totalItens][16];
    char totalTexto[totalItens][20];
    char statusTexto[totalItens][12];

    const char *vendaPtr[totalItens];
    const char *codProdPtr[totalItens];
    const char *descPtr[totalItens];
    const char *precoPtr[totalItens];
    const char *qtdPtr[totalItens];
    const char *totalPtr[totalItens];
    const char *statusPtr[totalItens];

    for (int i = 0; i < totalItens; i++)
    {
        sprintf(vendaTexto[i],   "%d", item[i].numeroVenda);
        sprintf(codProdTexto[i], "%d", item[i].codigoProduto);
        sprintf(precoTexto[i],   "R$ %.2f", item[i].precoVenda);
        sprintf(qtdTexto[i],     "%d", item[i].quantidade);
        sprintf(totalTexto[i],   "R$ %.2f", item[i].totalItem);
        sprintf(statusTexto[i],  "%s", item[i].pagamento == 'p' ? "Pago" : "Aberto");

        vendaPtr[i]   = vendaTexto[i];
        codProdPtr[i] = codProdTexto[i];
        descPtr[i]    = item[i].descricao;
        precoPtr[i]   = precoTexto[i];
        qtdPtr[i]     = qtdTexto[i];
        totalPtr[i]   = totalTexto[i];
        statusPtr[i]  = statusTexto[i];
    }

    int lgVenda  = larguraColunaTexto(vendaPtr,   totalItens, 5,  "VENDA");
    int lgCodP   = larguraColunaTexto(codProdPtr, totalItens, 12, "COD. PRODUTO");
    int lgDesc   = larguraColunaTexto(descPtr,    totalItens, 20, "DESCRICAO");
    int lgPreco  = larguraColunaTexto(precoPtr,   totalItens, 10, "PRECO");
    int lgQtd    = larguraColunaTexto(qtdPtr,     totalItens, 4,  "QTD");
    int lgTotal  = larguraColunaTexto(totalPtr,   totalItens, 10, "TOTAL");
    int lgStatus = larguraColunaTexto(statusPtr,  totalItens, 6,  "STATUS");

    int larguras[7] = { lgVenda, lgCodP, lgDesc, lgPreco, lgQtd, lgTotal, lgStatus };

    escreverLinhaSeparadora(f, larguras, 7);
    escreverCelulaTexto(f, "VENDA", lgVenda);
    escreverCelulaTexto(f, "COD. PRODUTO", lgCodP);
    escreverCelulaTexto(f, "DESCRICAO", lgDesc);
    escreverCelulaTexto(f, "PRECO", lgPreco);
    escreverCelulaTexto(f, "QTD", lgQtd);
    escreverCelulaTexto(f, "TOTAL", lgTotal);
    escreverCelulaTexto(f, "STATUS", lgStatus);
    fprintf(f, "|\n");
    escreverLinhaSeparadora(f, larguras, 7);

    for (int i = 0; i < totalItens; i++)
    {
        escreverCelulaTexto(f, vendaPtr[i],   lgVenda);
        escreverCelulaTexto(f, codProdPtr[i], lgCodP);
        escreverCelulaTexto(f, descPtr[i],    lgDesc);
        escreverCelulaTexto(f, precoPtr[i],   lgPreco);
        escreverCelulaTexto(f, qtdPtr[i],     lgQtd);
        escreverCelulaTexto(f, totalPtr[i],   lgTotal);
        escreverCelulaTexto(f, statusPtr[i],  lgStatus);
        fprintf(f, "|\n");
    }
    escreverLinhaSeparadora(f, larguras, 7);

    fclose(f);
}

/*
 * Carrega itens de venda de "itens.txt".
 */
void carregarItens(void)
{
    FILE *f = fopen("itens.txt", "r");
    if (!f) return;

    fscanf(f, "%d\n", &totalItens);

    if (totalItens == 0)
    {
        fclose(f);
        return;
    }

    if (totalItens > capacidadeItens)
    {
        capacidadeItens = totalItens * 2;
        ItensVenda *tmp = realloc(item, capacidadeItens * sizeof(ItensVenda));
        if (tmp) item = tmp;
    }

    char linha[1024];
    int  lidos = 0;

    while (lidos < totalItens && fgets(linha, sizeof(linha), f))
    {
        if (linha[0] != '|')
            continue;
        if (strstr(linha, "VENDA") != NULL)
            continue;

        char campos[7][100];
        for (int c = 0; c < 7; c++) campos[c][0] = '\0';

        char *campo = strtok(linha, "|");
        int   col   = 0;
        while (campo != NULL && col < 7)
        {
            while (*campo == ' ') campo++;
            int len = (int)strlen(campo);
            while (len > 0 && (campo[len-1] == ' ' || campo[len-1] == '\n' || campo[len-1] == '\r'))
            {
                campo[len-1] = '\0';
                len--;
            }
            strncpy(campos[col], campo, sizeof(campos[col]) - 1);

            campo = strtok(NULL, "|");
            col++;
        }

        item[lidos].numeroVenda   = atoi(campos[0]);
        item[lidos].codigoProduto = atoi(campos[1]);
        strcpy(item[lidos].descricao, campos[2]);

        char *pPreco = campos[3];
        if (strncmp(pPreco, "R$ ", 3) == 0) pPreco += 3;
        item[lidos].precoVenda = (float)atof(pPreco);

        item[lidos].quantidade = atoi(campos[4]);

        char *pTotal = campos[5];
        if (strncmp(pTotal, "R$ ", 3) == 0) pTotal += 3;
        item[lidos].totalItem = (float)atof(pTotal);

        item[lidos].pagamento = (strcmp(campos[6], "Pago") == 0) ? 'p' : 'a';

        lidos++;
    }

    totalItens = lidos;
    fclose(f);
}

/* ---------- Pagamentos ---------- */

/*
 * Salva pagamentos em "pagamentos.txt" em formato de tabela.
 * Colunas: VENDA | VALOR PAGO | TIPO
 */
void salvarPagamentos(void)
{
    FILE *f = fopen("pagamentos.txt", "w");
    if (!f) { printf("Erro ao salvar pagamentos!\n"); return; }

    fprintf(f, "%d\n", totalPagamentos);

    if (totalPagamentos == 0)
    {
        fprintf(f, "(Nenhum pagamento registrado)\n");
        fclose(f);
        return;
    }

    char vendaTexto[totalPagamentos][16];
    char valorTexto[totalPagamentos][20];
    char tipoDescTexto[totalPagamentos][24];

    const char *vendaPtr[totalPagamentos];
    const char *valorPtr[totalPagamentos];
    const char *tipoDescPtr[totalPagamentos];

    for (int i = 0; i < totalPagamentos; i++)
    {
        sprintf(vendaTexto[i], "%d", pagamento[i].numeroVenda);
        sprintf(valorTexto[i], "R$ %.2f", pagamento[i].valorPago);

        const char *desc;
        if      (strcmp(pagamento[i].tipo, "d")  == 0) desc = "Dinheiro";
        else if (strcmp(pagamento[i].tipo, "c")  == 0) desc = "Cartao";
        else if (strcmp(pagamento[i].tipo, "md") == 0) desc = "Misto-Dinheiro";
        else if (strcmp(pagamento[i].tipo, "mc") == 0) desc = "Misto-Cartao";
        else                                            desc = pagamento[i].tipo;
        sprintf(tipoDescTexto[i], "%s", desc);

        vendaPtr[i]    = vendaTexto[i];
        valorPtr[i]    = valorTexto[i];
        tipoDescPtr[i] = tipoDescTexto[i];
    }

    int lgVenda = larguraColunaTexto(vendaPtr,    totalPagamentos, 5,  "VENDA");
    int lgValor = larguraColunaTexto(valorPtr,    totalPagamentos, 10, "VALOR PAGO");
    int lgTipo  = larguraColunaTexto(tipoDescPtr, totalPagamentos, 14, "TIPO");

    int larguras[3] = { lgVenda, lgValor, lgTipo };

    escreverLinhaSeparadora(f, larguras, 3);
    escreverCelulaTexto(f, "VENDA", lgVenda);
    escreverCelulaTexto(f, "VALOR PAGO", lgValor);
    escreverCelulaTexto(f, "TIPO", lgTipo);
    fprintf(f, "|\n");
    escreverLinhaSeparadora(f, larguras, 3);

    for (int i = 0; i < totalPagamentos; i++)
    {
        escreverCelulaTexto(f, vendaPtr[i],    lgVenda);
        escreverCelulaTexto(f, valorPtr[i],    lgValor);
        escreverCelulaTexto(f, tipoDescPtr[i], lgTipo);
        fprintf(f, "|\n");
    }
    escreverLinhaSeparadora(f, larguras, 3);

    fclose(f);
}

/*
 * Carrega pagamentos de "pagamentos.txt".
 */
void carregarPagamentos(void)
{
    FILE *f = fopen("pagamentos.txt", "r");
    if (!f) return;

    fscanf(f, "%d\n", &totalPagamentos);

    if (totalPagamentos == 0)
    {
        fclose(f);
        return;
    }

    if (totalPagamentos > capacidadePagamentos)
    {
        capacidadePagamentos = totalPagamentos * 2;
        Pagamentos *tmp = realloc(pagamento, capacidadePagamentos * sizeof(Pagamentos));
        if (tmp) pagamento = tmp;
    }

    char linha[1024];
    int  lidos = 0;

    while (lidos < totalPagamentos && fgets(linha, sizeof(linha), f))
    {
        if (linha[0] != '|')
            continue;
        if (strstr(linha, "VALOR PAGO") != NULL)
            continue;

        char campos[3][100];
        for (int c = 0; c < 3; c++) campos[c][0] = '\0';

        char *campo = strtok(linha, "|");
        int   col   = 0;
        while (campo != NULL && col < 3)
        {
            while (*campo == ' ') campo++;
            int len = (int)strlen(campo);
            while (len > 0 && (campo[len-1] == ' ' || campo[len-1] == '\n' || campo[len-1] == '\r'))
            {
                campo[len-1] = '\0';
                len--;
            }
            strncpy(campos[col], campo, sizeof(campos[col]) - 1);

            campo = strtok(NULL, "|");
            col++;
        }

        pagamento[lidos].numeroVenda = atoi(campos[0]);

        char *pValor = campos[1];
        if (strncmp(pValor, "R$ ", 3) == 0) pValor += 3;
        pagamento[lidos].valorPago = (float)atof(pValor);

        if      (strcmp(campos[2], "Dinheiro")       == 0) strcpy(pagamento[lidos].tipo, "d");
        else if (strcmp(campos[2], "Cartao")          == 0) strcpy(pagamento[lidos].tipo, "c");
        else if (strcmp(campos[2], "Misto-Dinheiro")  == 0) strcpy(pagamento[lidos].tipo, "md");
        else if (strcmp(campos[2], "Misto-Cartao")    == 0) strcpy(pagamento[lidos].tipo, "mc");
        else strncpy(pagamento[lidos].tipo, campos[2], sizeof(pagamento[lidos].tipo) - 1);

        lidos++;
    }

    totalPagamentos = lidos;
    fclose(f);
}

/* ---------- Caixa ---------- */

/*
 * Salva estado do caixa em "caixa.txt" em formato de tabela.
 * Colunas: STATUS | VALOR ABERTURA | VALOR ATUAL
 */
void salvarCaixa(void)
{
    FILE *f = fopen("caixa.txt", "w");
    if (!f) { printf("Erro ao salvar caixa!\n"); return; }

    /* primeira linha tecnica, igual antes, para carregamento direto */
    fprintf(f, "%d\n",   caixaAberto);
    fprintf(f, "%.2f\n", caixaInicial);
    fprintf(f, "%.2f\n", caixaAtual);

    char statusTexto[16];
    char aberturaTexto[20];
    char atualTexto[20];

    sprintf(statusTexto,   "%s", caixaAberto == 1 ? "Aberto" : "Fechado");
    sprintf(aberturaTexto, "R$ %.2f", caixaInicial);
    sprintf(atualTexto,    "R$ %.2f", caixaAtual);

    const char *statusPtr[1]   = { statusTexto };
    const char *aberturaPtr[1] = { aberturaTexto };
    const char *atualPtr[1]    = { atualTexto };

    int lgStatus   = larguraColunaTexto(statusPtr,   1, 6,  "STATUS");
    int lgAbertura = larguraColunaTexto(aberturaPtr, 1, 14, "VALOR ABERTURA");
    int lgAtual    = larguraColunaTexto(atualPtr,    1, 11, "VALOR ATUAL");

    int larguras[3] = { lgStatus, lgAbertura, lgAtual };

    escreverLinhaSeparadora(f, larguras, 3);
    escreverCelulaTexto(f, "STATUS", lgStatus);
    escreverCelulaTexto(f, "VALOR ABERTURA", lgAbertura);
    escreverCelulaTexto(f, "VALOR ATUAL", lgAtual);
    fprintf(f, "|\n");
    escreverLinhaSeparadora(f, larguras, 3);

    escreverCelulaTexto(f, statusTexto,   lgStatus);
    escreverCelulaTexto(f, aberturaTexto, lgAbertura);
    escreverCelulaTexto(f, atualTexto,    lgAtual);
    fprintf(f, "|\n");
    escreverLinhaSeparadora(f, larguras, 3);

    fclose(f);
}

/*
 * Carrega estado do caixa de "caixa.txt".
 * As 3 primeiras linhas continuam no formato tecnico simples,
 * exatamente como antes, garantindo leitura 100% confiavel.
 */
void carregarCaixa(void)
{
    FILE *f = fopen("caixa.txt", "r");
    if (!f) return;

    fscanf(f, "%d\n",  &caixaAberto);
    fscanf(f, "%f\n",  &caixaInicial);
    fscanf(f, "%f\n",  &caixaAtual);
    fclose(f);
}

/* ---------- Tudo ---------- */

void carregarTudo(void)
{
    carregarUsuarios();
    carregarClientes();
    carregarProdutos();
    carregarVendas();
    carregarItens();
    carregarPagamentos();
    carregarCaixa();
}

void salvarTudo(void)
{
    salvarUsuarios();
    salvarClientes();
    salvarProdutos();
    salvarVendas();
    salvarItens();
    salvarPagamentos();
    salvarCaixa();
}
