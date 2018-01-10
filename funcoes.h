#ifndef _funcoes_h
#define _funcoes_h

//STRUCTS
typedef struct
{
    int x, y;
    int velx, vely;
    BITMAP* img;
}quadrado;

typedef struct
{
    int x, y;
    int vel;
    int posanterior;
    BITMAP* img;
}palheta;

typedef struct
{
    int x, y;
    int ativo;
    int pont;
    int posanterior;
    BITMAP* img;
}blocos;

typedef struct
{
    FONT* fonte;
    int valor;
}indicador;

typedef struct
{
    char nome[10];
    int pontuacao;
}jogador;

extern volatile int exit_program;

int teclas_anteriores[KEY_MAX];

//FUNCOES
enum opmenu{joga=1, instr, rec, sair};

int menu();

void move_palheta(palheta *pal, char tecla, int vel);

void move_bolinha(quadrado* bolinha, blocos* R, blocos* G, blocos* B, palheta* pal, indicador* inds, int* iniciou);

void ativarblocos(blocos* B, char cor);

void imprimeblocos(BITMAP* buffer, blocos* R, blocos* G, blocos* B);

void imprimeTela(palheta pal, quadrado bolinha, blocos* rB, blocos* gB, blocos* bB, int iniciou, indicador* inds, char* nome);

void jogar();

void record_upd(char *nome, int pont);//struct pont

void instrucoes();

void ver_record();

void keyboard_input();

int apertou(int TECLA);

int segurou(int TECLA);

int soltou(int TECLA);

void destroi(FONT* fmenu, BITMAP* buffer , BITMAP* imagem, MIDI* hallelujah);

void colisao(quadrado* bolinha, blocos* R, blocos* G, blocos* B, palheta* pal, indicador* inds, int* iniciou, SAMPLE* somcol);
#endif
