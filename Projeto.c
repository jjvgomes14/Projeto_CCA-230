#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRIORITARIO 20

//Estrutura data
typedef struct {
    int dia, mes, ano;
} Data;

//Estrutura Registro
typedef struct {
    int id; //Criado para identificação de paciente por ID
    char nome[100];
    int idade;
    char RG[20];
    Data *entrada;
} Registro;

//Estrutura ELista
typedef struct ELista {
    Registro *dados;
    struct ELista *proximo;
} ELista;

//Estrutura Lista
typedef struct {
    ELista *inicio;
    int qtde;
    int ultimoID; //Criado para reorganização dos IDs
} Lista;

//Estrutura EFila
typedef struct EFila {
    Registro *dados;
    struct EFila *proximo;
} EFila;

//Estrutura Fila
typedef struct {
    EFila *head;
    EFila *tail;
    int qtde;
} Fila;

//Estrutura Heap
typedef struct {
    Registro *dados[MAX_PRIORITARIO];
    int qtde;
} Heap;

//Estrutura EABB
typedef struct EABB {
    Registro *dados;
    struct EABB *filhoEsq;
    struct EABB *filhoDir;
} EABB;

//Estrutura ABB
typedef struct {
    EABB *raiz;
    int qtde;
} ABB;

//Estrutura Operacao
typedef struct Operacao {
    char descricao[100];
    Registro *paciente;
    struct Operacao *proximo;
} Operacao;

//Estrutura Pilha
typedef struct {
    Operacao *topo;
    int qtde;
} Pilha;

//Implementacao data
Data *nova_data(int d, int m, int a) {
    Data *data = malloc(sizeof(Data));
    data->dia = d;
    data->mes = m;
    data->ano = a;
    return data;
}

//Implementacao registro
Registro *novo_registro(int id, char *nome, int idade, char *RG, Data *entrada) {
    Registro *r = malloc(sizeof(Registro));
    r->id = id;
    strcpy(r->nome, nome);
    strcpy(r->RG, RG);
    r->idade = idade;
    r->entrada = entrada;
    return r;
}

// FUNCOES DE CADASTRO
//Criacao da lista
Lista *criar_lista() {
    Lista *l = malloc(sizeof(Lista));
    l->inicio = NULL;
    l->qtde = 0;
    l->ultimoID = 1;
    return l;
}

//Funcao para cadastrar novo paciente
void cadastrar_paciente(Lista *lista, Registro *reg) {
    ELista *novo = malloc(sizeof(ELista));
    novo->dados = reg;
    novo->proximo = lista->inicio;
    lista->inicio = novo;
    lista->qtde++;
}

//Funcao para consultar paciente pelo ID
Registro *consultar_paciente(Lista *lista, int id) {
    ELista *atual = lista->inicio;
    while (atual) {
        if (atual->dados->id == id) return atual->dados;
        atual = atual->proximo;
    }
    return NULL;
}

//Funcao para mostrar lista de pacientes cadastrados
void mostrar_lista(Lista *lista) {
    ELista *atual = lista->inicio;
    while (atual) {
        printf("ID %d | %s | %d | %s | %02d/%02d/%d\n", atual->dados->id,
               atual->dados->nome, atual->dados->idade,
               atual->dados->RG, atual->dados->entrada->dia,
               atual->dados->entrada->mes, atual->dados->entrada->ano);
        atual = atual->proximo;
    }
}

//Funcao para atualizar o cadastro do paciente
void atualizar_paciente(Registro *p) {
    printf("Novo nome: ");
    scanf(" %[^\n]", p->nome);
    printf("Nova idade: ");
    scanf("%d", &p->idade);
    printf("Novo RG: ");
    scanf(" %[^\n]", &p->RG);
    printf("Nova data (dd mm aaaa): ");
    scanf("%d %d %d", &p->entrada->dia, &p->entrada->mes, &p->entrada->ano);
}

//Funcao para remover o cadastro de um paciente
int remover_paciente(Lista *lista, int id) {
    ELista *atual = lista->inicio, *anterior = NULL;
    while (atual) {
        if (atual->dados->id == id) {
            if (anterior) anterior->proximo = atual->proximo;
            else lista->inicio = atual->proximo;
            free(atual->dados->entrada);
            free(atual->dados);
            free(atual);
            lista->qtde--;
            return 1;
        }
        anterior = atual;
        atual = atual->proximo;
    }
    return 0;
}

//Reorganiza os IDs após a remocao de um registro
void reorganizar_ids(Lista *lista) {
    ELista *atual = lista->inicio;
    int novo_id = 1;
    while (atual) {
        atual->dados->id = novo_id++;
        atual = atual->proximo;
    }
    lista->ultimoID = novo_id;
}

// FUNCOES PARA FILA DE ATENDIMENTO
//Funcao de criar a fila
Fila *criar_fila() {
    Fila *f = malloc(sizeof(Fila));
    f->head = f->tail = NULL;
    f->qtde = 0;
    return f;
}

//Funcao para enfileirar pacientes em ordem de chegada
void enfileirar(Fila *fila, Registro *reg) {
    EFila *novo = malloc(sizeof(EFila));
    novo->dados = reg;
    novo->proximo = NULL;
    if (fila->tail) fila->tail->proximo = novo;
    else fila->head = novo;
    fila->tail = novo;
    fila->qtde++;
}

//Funcao de atendimento do primeiro paciente da fila
Registro *desenfileirar(Fila *fila) {
    if (!fila->head) return NULL;
    EFila *remover = fila->head;
    fila->head = remover->proximo;
    if (!fila->head) fila->tail = NULL;
    Registro *r = remover->dados;
    free(remover);
    fila->qtde--;
    return r;
}

// FUNCOES DE HEAP
//Funcao para manter a ordem de atendimento pela idade (Alta prioridade para pacientes mais velhos)
void subir_heap(Heap *h, int idx) {
    while (idx > 0 && h->dados[(idx-1)/2]->idade < h->dados[idx]->idade) {
        Registro *tmp = h->dados[idx];
        h->dados[idx] = h->dados[(idx-1)/2];
        h->dados[(idx-1)/2] = tmp;
        idx = (idx-1)/2;
    }
}

//Funcao para manter a ordem de atendimento pela idade (Baixa prioridade para pacientes mais novos)
void descer_heap(Heap *h, int idx) {
    int maior = idx, e = 2*idx+1, d = 2*idx+2;
    if (e < h->qtde && h->dados[e]->idade > h->dados[maior]->idade) maior = e;
    if (d < h->qtde && h->dados[d]->idade > h->dados[maior]->idade) maior = d;
    if (maior != idx) {
        Registro *tmp = h->dados[idx];
        h->dados[idx] = h->dados[maior];
        h->dados[maior] = tmp;
        descer_heap(h, maior);
    }
}

//Funcao para enfileirar pacientes em ordem de idade
void enfileirar_prioritario(Heap *h, Registro *reg) {
    if (h->qtde >= MAX_PRIORITARIO) return;
    h->dados[h->qtde] = reg;
    subir_heap(h, h->qtde++);
}

//Funcao de atendimento do primeiro paciente da fila
Registro *desenfileirar_prioritario(Heap *h) {
    if (h->qtde == 0) return NULL;
    Registro *r = h->dados[0];
    h->dados[0] = h->dados[--h->qtde];
    descer_heap(h, 0);
    return r;
}

// FUNCOES DE ARVORE BINARIA DE BUSCA
//Cria a arvore binaria
ABB *criar_abb() {
    ABB *a = malloc(sizeof(ABB));
    a->raiz = NULL;
    a->qtde = 0;
    return a;
}

//Funcao recursiva para busca na arvore binaria
EABB *inserir_abb_rec(EABB *raiz, Registro *reg, int tipo) {
    if (raiz == NULL) {
        EABB *novo = malloc(sizeof(EABB));
        novo->dados = reg;
        novo->filhoEsq = NULL;
        novo->filhoDir = NULL;
        return novo;
    }

    int valorA, valorB;

    //Define a operacao utilizada
    if (tipo == 1)
        valorA = reg->entrada->ano;
    else if (tipo == 2)
        valorA = reg->entrada->mes;
    else if (tipo == 3)
        valorA = reg->entrada->dia;
    else
        valorA = reg->idade;

    if (tipo == 1)
        valorB = raiz->dados->entrada->ano;
    else if (tipo == 2)
        valorB = raiz->dados->entrada->mes;
    else if (tipo == 3)
        valorB = raiz->dados->entrada->dia;
    else
        valorB = raiz->dados->idade;

    if (valorA <= valorB)
        raiz->filhoEsq = inserir_abb_rec(raiz->filhoEsq, reg, tipo);
    else
        raiz->filhoDir = inserir_abb_rec(raiz->filhoDir, reg, tipo);

    return raiz;
}

//Insere o registro na raiz da arvore binaria para chamar a funcao recursiva
void inserir_abb(ABB *a, Registro *r, int tipo) {
    a->raiz = inserir_abb_rec(a->raiz, r, tipo);
    a->qtde++;
}

//Mostra os registros em ordem da operação selecionada
void em_ordem(EABB *raiz) {
    if (raiz) {
        em_ordem(raiz->filhoEsq);
        printf("%s | %d | %02d/%02d/%04d\n", raiz->dados->nome, raiz->dados->idade,
               raiz->dados->entrada->dia, raiz->dados->entrada->mes, raiz->dados->entrada->ano);
        em_ordem(raiz->filhoDir);
    }
}

// FUNCOES PILHA
//Cria a pilha para desfazer uma operacao
Pilha *criar_pilha() {
    Pilha *p = malloc(sizeof(Pilha));
    p->topo = NULL;
    p->qtde = 0;
    return p;
}

//Cria um log com as operacoes de fila ja realizadas
void registrar_operacao(Pilha *p, char *descricao, Registro *paciente) {
    Operacao *o = malloc(sizeof(Operacao));
    strcpy(o->descricao, descricao);
    o->paciente = paciente;
    o->proximo = p->topo;
    p->topo = o;
    p->qtde++;
}

//Desfaz a operacao selecionada pelo usuário
Operacao *desfazer_operacao(Pilha *p) {
    if (!p->topo) return NULL;
    Operacao *o = p->topo;
    p->topo = o->proximo;
    p->qtde--;
    return o;
}

//Mostra todas as operacoes relacionadas a fila ja realizadas
void mostrar_operacoes(Pilha *p) {
    Operacao *o = p->topo;
    while (o) {
        printf("%s - %s\n", o->descricao, o->paciente->nome);
        o = o->proximo;
    }
}

// FUNCOES DO ARQUIVO
//Salva os registros em um arquivo .dat
void salvar_arquivo(Lista *lista, const char *arquivo) {
    FILE *f = fopen(arquivo, "wb");
    if (!f) return;
    ELista *atual = lista->inicio;
    while (atual) {
        fwrite(atual->dados, sizeof(Registro), 1, f);
        fwrite(atual->dados->entrada, sizeof(Data), 1, f);
        atual = atual->proximo;
    }
    fclose(f);
}

//Carrega os registros do arquivo
void carregar_arquivo(Lista *lista, const char *arquivo) {
    FILE *f = fopen(arquivo, "rb");
    if (!f) return;
    Registro r; Data d;
    while (fread(&r, sizeof(Registro), 1, f)) {
        fread(&d, sizeof(Data), 1, f);
        Data *pd = nova_data(d.dia, d.mes, d.ano);
        Registro *pr = novo_registro(r.id, r.nome, r.idade, r.RG, pd);
        cadastrar_paciente(lista, pr);
    }
    fclose(f);
}

//Funcao inicial do programa
void menu() {
    Lista *lista = criar_lista();
    Fila *fila = criar_fila();
    Heap heap = {.qtde = 0};
    Pilha *pilha = criar_pilha();

    carregar_arquivo(lista, "pacientes.dat");
    int op;
    do {
        printf("\n--- MENU ---\n");
        printf("1.Cadastrar \n2.Atendimento \n3.Atendimento Prioritario \n4.Pesquisa \n5.Desfazer \n6.Salvar \n7.Sobre \n0.Sair \nOpcao: ");
        scanf("%d", &op);

        switch (op) {
            case 1: {
                int l;
                printf("\n--- Lista de Pacientes ---\n");
                printf("1.Cadastrar \n2.Consultar \n3.Mostrar \n4.Atualizar \n5.Remover \n0.Voltar\nOpcao: ");
                scanf("%d", &l);
                char nome[100], rg[20];
                int idade, d, m, a;
                int id;
                Registro *p;
                switch (l) {
                    case 1:
                        printf("Nome: "); scanf(" %[^\n]", nome);
                        printf("Idade: "); scanf("%d", &idade);
                        printf("RG: "); scanf(" %[^\n]", rg);
                        printf("Data (dd mm aaaa): "); scanf("%d %d %d", &d, &m, &a);
                        p = novo_registro(lista->ultimoID++, nome, idade, rg, nova_data(d, m, a));
                        cadastrar_paciente(lista, p);
                        printf("Paciente cadastrado com ID %d.\n", p->id);
                        break;
                    case 2:
                        printf("ID: "); scanf("%d", &id);
                        p = consultar_paciente(lista, id);
                        if (p) printf("%s | %d | %s | %02d/%02d/%04d\n", p->nome, p->idade, p->RG, p->entrada->dia, p->entrada->mes, p->entrada->ano);
                        else printf("Paciente nao encontrado\n");
                        break;
                    case 3: mostrar_lista(lista); break;
                    case 4:
                        printf("ID: "); scanf("%d", &id);
                        p = consultar_paciente(lista, id);
                        if (p) atualizar_paciente(p);
                        else printf("Paciente nao encontrado");
                        break;
                    case 5:
                        printf("ID: "); scanf("%d", &id);
                        if (remover_paciente(lista, id)) {
                            reorganizar_ids(lista);
                            printf("Registro removido e IDs reorganizados.\n");
                        } else {
                            printf("Nao encontrado\n");
                        }
                        break;
                    default: break;
                }
                break;
            }
             case 2: {
                int a;
                printf("1.Enfileirar \n2.Atender \n3.Mostrar lista de atendimento \n0.Voltar\nOpcao: ");
                scanf("%d", &a);
                int id;
                switch (a) {
                    case 1:
                        printf("ID: "); scanf(" %d", &id);
                        Registro *p = consultar_paciente(lista, id);
                        if (p) {
                            enfileirar(fila, p);
                            registrar_operacao(pilha, "Paciente em fila de atendimento comum\n", p);
                        } else printf("Nao encontrado\n");
                        break;
                    case 2:
                        p = desenfileirar(fila);
                        if (p) printf("Paciente Atendido: %s\n", p->nome);
                        else printf("Fila vazia\n");
                        break;
                    case 3: {
                        EFila *atual = fila->head;
                        while (atual) {
                            printf("%s (%d anos)\n", atual->dados->nome, atual->dados->idade);
                            atual = atual->proximo;
                        }
                        break;
                    }
                }
                break;
            }
            case 3: {
                int a;
                printf("1.Enfileirar Prioritario \n2.Atender Prioritario \n3.Mostrar ordem de atendimento \n0.Voltar\nOpcao: ");
                scanf("%d", &a);
                int id;
                switch (a) {
                    case 1:
                        printf("ID: "); scanf(" %d", &id);
                        Registro *p = consultar_paciente(lista, id);
                        if (p) {
                            enfileirar_prioritario(&heap, p);
                            registrar_operacao(pilha, "Paciente em fila de atendimento prioritario\n", p);
                        } else printf("ID Nao encontrado\n");
                        break;
                    case 2:
                        p = desenfileirar_prioritario(&heap);
                        if (p) printf("Atendido: %s (%d anos)\n", p->nome, p->idade);
                        else printf("Heap vazio\n");
                        break;
                    case 3:
                        Heap copia = heap;
                        while (copia.qtde > 0) {
                            Registro *r = desenfileirar_prioritario(&copia);
                            printf("%s (%d anos)\n", r->nome, r->idade);
                        }
                        break;
                }
                break;
            }
            case 4: {
                int tipo;
                printf("1.Ano 2.Mes 3.Dia 4.Idade\nOpcao: ");
                scanf("%d", &tipo);
                ABB *abb = criar_abb();
                ELista *it = lista->inicio;
                while (it) {
                    inserir_abb(abb, it->dados, tipo);
                    it = it->proximo;
                }
                em_ordem(abb->raiz);
                break;
            }
            case 5:
                mostrar_operacoes(pilha);
                printf("Deseja desfazer ultima operacao? (1=sim): ");
                int s;
                scanf("%d", &s);
                if (s == 1) {
                    Operacao *o = desfazer_operacao(pilha);
                    if (o) {
                        printf("Desfeita: %s de %s\n", o->descricao, o->paciente->nome);
                        free(o);
                    } else printf("Nada a desfazer\n");
                }
                break;
            case 6:
                salvar_arquivo(lista, "pacientes.dat");
                printf("Salvo\n");
                break;
            case 7:
                printf("Joao Victor Gomes Prado | 7 Ciclo | RA: 11.121.051-4 | Engenharia de Automacao e Controle | CCA-230 | 14/05/2025\n");
                printf("Cleuder Nathan Conconi Garcia | 7 Ciclo | RA: 11.122.389-7 | Engenharia de Automacao e Controle | CCA-230 | 14/05/2025\n");
                printf("https://github.com/jjvgomes14/Projeto_CCA-230.git");
                break;
            case 0:
                salvar_arquivo(lista, "pacientes.dat");
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (op != 0);
}

int main() {
    menu();
    return 0;
}