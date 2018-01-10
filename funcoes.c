#include <allegro.h>
#include <stdio.h>
#include "funcoes.h"
#define numblocos 12
#define alturaB 150
#define VIDAS 0
#define PONTUACAO 1
#define FASE 2
#define VEL 6


volatile long int taxaatu;

void fps()
{
    taxaatu++;
}
END_OF_FUNCTION(fps);

/***************************************** CAMPO FUNCAO JOGO ***************************************************/

void move_palheta(palheta *pal, char tecla, int vel)
{
    switch (tecla)
    {
        case 'L': pal->x -= vel; break;
        case 'R': pal->x += vel; break;
    }
}

void ativarblocos(blocos* B, char cor)
{
    int i;
    char caminho[50];
    strcpy(caminho, "IMG/Xbrick.bmp");
    caminho[4] = cor;
    for(i = 0; i < numblocos; i++)
    {
        B[i].img = load_bitmap(caminho, NULL);
        B[i].ativo = 1;
        if(cor == 'r')
        {
            B[i].pont = 300;
        }
        else if(cor == 'g')
        {
            B[i].pont = 200;
        }
        else if(cor == 'b')
        {
            B[i].pont = 100;
        }
        //printf("ativou!\n");
    }
}

void imprimeblocos(BITMAP* buffer, blocos* R, blocos* G, blocos* B)
{
    int i;

    for(i = 0; i < numblocos; i++)
    {
        if(R[i].ativo == 1)
        {
            draw_sprite(buffer, R[i].img,12 + i*(R[i].img->w + 5), alturaB);
            R[i].x = 12 + i*(R[i].img->w + 5);
            R[i].y = alturaB;
        }

        if(G[i].ativo == 1)
        {
            draw_sprite(buffer, G[i].img,12 + i*(G[i].img->w + 5), alturaB+5+(R[i].img->h));
            G[i].x = 12 + i*(G[i].img->w + 5);
            G[i].y = alturaB+5+(R[i].img->h);
        }

        if(B[i].ativo == 1)
        {
            draw_sprite(buffer, B[i].img,12 + i*(B[i].img->w + 5), alturaB+10+(R[i].img->h*2));
            B[i].x = 12 + i*(B[i].img->w + 5);
            B[i].y = alturaB+10+(R[i].img->h*2);
        }
    }
}

void imprimeTela(palheta pal, quadrado bolinha, blocos* rB, blocos* gB, blocos* bB, int iniciou, indicador* inds, char* nome)
{
    int cor=makecol(233, 208, 39);

    BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
    imprimeblocos(buffer, rB, gB, bB);

    textprintf_ex(buffer, inds[0].fonte, 10, 10, cor, -1, "%d", inds[VIDAS].valor);
    textprintf_centre_ex(buffer, inds[0].fonte, SCREEN_W/2, 10, cor, -1, "%s", nome);
    textprintf_right_ex(buffer, inds[1].fonte, 790, 10, cor, -1, "%d", inds[PONTUACAO].valor);
    //textprintf_centre_ex(buffer, inds[2].fonte, 750, 10, cor, -1, "%d", inds[FASE].valor);

    if(iniciou != 0)
        draw_sprite(buffer, bolinha.img, bolinha.x, bolinha.y);

    draw_sprite(buffer, pal.img, pal.x, pal.y);
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    //draw_sprite(screen, buffer,0,0);
    //rest(2);
    destroy_bitmap(buffer);
}

void colcantos(quadrado* bolinha, indicador *inds, int* iniciou)
{
    if(bolinha->y <= 0)
    {
        printf("vert %d\n", bolinha->vely);
        bolinha->vely = -bolinha->vely; // Se detectou colisão, muda o vetor na componente y

    }
    if((bolinha->x >= (SCREEN_W - bolinha->img->w) || bolinha->x <= 0))
    {
        printf("horz %d\n", bolinha->velx);
        bolinha->velx = -bolinha->velx; // Se detectou colisão, muda o vetor na componente y
    }
    if(bolinha->y >= (SCREEN_H - bolinha->img->h))
    {
        inds[VIDAS].valor--;
        *iniciou = 0;
        printf("ERROW\n");
        bolinha->vely = -bolinha->vely;
    }
}
int bounding_box_collision(int b1_x, int b1_y, int b1_w, int b1_h, int b2_x, int b2_y, int b2_w, int b2_h, int ativo, SAMPLE* somcol)
{
    if ((b1_x > b2_x + b2_w - 1) || // is b1 on the right side of b2?
        (b1_y > b2_y + b2_h - 1) || // is b1 under b2?
        (b2_x > b1_x + b1_w - 1) || // is b2 on the right side of b1?
        (b2_y > b1_y + b1_h - 1))   // is b2 under b1?
    {
        // no collision
        return 0;
    }
    if(ativo == 1)
        play_sample(somcol, 255, 255, 1000, FALSE);
    return 1;
}
int colblocos(quadrado* bolinha, blocos* B, SAMPLE* somcol)
{
    int i;
    for(i = 0; i < numblocos; i++)
    {
        if(bolinha->y > (B[i].y + B[i].img->h))
            B[i].posanterior = 101;

        else if(bolinha->y + bolinha->img->h < B[i].y)
            B[i].posanterior = 102;

        else if(bolinha->x > (B[i].x + B[i].img->w))
            B[i].posanterior = 103;

        else if(bolinha->x + bolinha->img->w < B[i].x)
            B[i].posanterior = 104;

        if(bounding_box_collision(bolinha->x, bolinha->y, bolinha->img->w, bolinha->img->h, B[i].x, B[i].y, B[i].img->w, B[i].img->h, B[i].ativo, somcol) && B[i].ativo == 1)//se ferrar foi aqui
        {

            return i;
        }

    }
    return 100;
}

void colisaodetectada(quadrado* bolinha, blocos* B, int detcolbloco, indicador* inds)
{
    if(detcolbloco < numblocos && B[detcolbloco].ativo == 1)
    {
        printf("oi adriano\n");
        B[detcolbloco].ativo = 0;
        inds[PONTUACAO].valor += B[detcolbloco].pont;

        switch(B[detcolbloco].posanterior)
        {
            case 101:
            case 102:   bolinha->vely = -bolinha->vely;
                        break;
            case 103:
            case 104:   bolinha->velx = -bolinha->velx;
                        break;
        }
    }
}

void colpalheta(quadrado* bolinha, palheta* pal, SAMPLE* somcol)
{
        if(bounding_box_collision(bolinha->x, bolinha->y, bolinha->img->w, bolinha->img->h, pal->x, pal->y, pal->img->w, pal->img->h, 1, somcol))
        {

            switch(pal->posanterior)
            {
                case 101:
                case 102:   bolinha->vely = -bolinha->vely;
                            break;
                case 103:
                case 104:   bolinha->velx = -bolinha->velx;
                            bolinha->vely = -bolinha->vely;
                            bolinha->y -= 4;
                            break;
            }
        }
        else if(bolinha->y > (pal->y + pal->img->h))
            pal->posanterior = 101;

        else if(bolinha->y + bolinha->img->h < pal->y)
            pal->posanterior = 102;

        else if(bolinha->x > (pal->x + pal->img->w))
            pal->posanterior = 103;

        else if(bolinha->x + bolinha->img->w < pal->x)
            pal->posanterior = 104;
}

void colisao(quadrado* bolinha, blocos* R, blocos* G, blocos* B, palheta* pal, indicador* inds, int* iniciou, SAMPLE* somcol)
{

    int detcolblocoR = colblocos(bolinha, R, somcol);
    int detcolblocoG = colblocos(bolinha, G, somcol);
    int detcolblocoB = colblocos(bolinha, B, somcol);


    if(detcolblocoR != 100)
    {
        colisaodetectada(bolinha, R, detcolblocoR, inds);
    }


    if(detcolblocoG != 100)
    {
        colisaodetectada(bolinha, G, detcolblocoG, inds);
    }


    if(detcolblocoB != 100)
    {

         colisaodetectada(bolinha, B, detcolblocoB, inds);
    }

    //destroy_sample(somcol);
    colpalheta(bolinha, pal, somcol);
    colcantos(bolinha, inds, iniciou);
}

void move_bolinha(quadrado* bolinha, blocos* R, blocos* G, blocos* B, palheta* pal, indicador* inds, int* iniciou)
{
    SAMPLE* somcol = load_sample("sons/pop.wav");
    bolinha->x += bolinha->velx;
    bolinha->y += bolinha->vely;

    colisao(bolinha, R, G, B, pal, inds, iniciou, somcol);
}

void initbolinha(quadrado* bolinha)
{
    bolinha->x = rand()%765+15;
    bolinha->y = 200;
    bolinha->velx = VEL;
    bolinha->vely = bolinha->velx;
}

void le_nome(char* nome)
{
    BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
    int cor = makecol(0x98,0x98,0xBE);
    clear_to_color(buffer, makecol(0,0x27,0x3f));
    textout_centre_ex(buffer, font, "DIGITE O TEU NOME:", SCREEN_W/2, SCREEN_H/2, cor, -1);
    draw_sprite(screen, buffer, 0, 0);
    int c;
    int i = 0;
    do
    {
        clear_to_color(buffer, makecol(0,0x27,0x3f));
        textout_centre_ex(buffer, font, "DIGITE O TEU NOME:", SCREEN_W/2, SCREEN_H/2, cor, -1);
        clear_keybuf();
        c = readkey();
        if((((c >> 8) == KEY_ENTER) || ((c >> 8) == KEY_ENTER_PAD)) && i > 0)
            break;
        else if((((c >> 8) == KEY_ENTER) || ((c >> 8) == KEY_ENTER_PAD)) && i == 0)
        {
            c = KEY_BACKSPACE << 8;
        }
        if((c >> 8) == KEY_BACKSPACE && i == 0)
        {
            nome[i] = '\0';
        }
        else if((c >> 8) == KEY_BACKSPACE && i > 0)
        {
            nome[--i] = '\0';
        }
        else if(i<10)
        {
            nome[i] = c & 0xff;
            nome[++i] = '\0';
        }
        textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2 + font->height, cor, -1, "%s", nome);
        draw_sprite(screen, buffer, 0, 0);

    }while(((c >> 8) != KEY_ENTER) && ((c >> 8) != KEY_ENTER_PAD));
    destroy_bitmap(buffer);
}

void gameover()
{
    clear(screen);
    FONT* ffim = load_font("fontes/fontemenu.pcx", NULL, NULL);
    textout_centre_ex(screen, ffim, "VOCE PERDEU!", SCREEN_W/2, SCREEN_H/2, makecol(255, 0, 0), 0x880000);
    rest(2000);

}

void venceu()
{
    int x = 0,y, i;
    for(i=0; i<15; i++)
    {
        x = 400;
        y = 600 - i*40;
        FONT* ffim = load_font("fontes/fontemenu.pcx", NULL, NULL);
        textout_centre_ex(screen, ffim, "VOCE VENCEU O JOGO!", x, y, makecol(233, 208, 39),-1);

    }
    rest(1000);

}

void jogar()
{
    char nome[11];
    int i, fimJogo = 0, iniciou = 0;
    taxaatu = 0;
    palheta pal;
    quadrado bolinha;
    indicador inds[3];

    le_nome(nome);

    inds[VIDAS].fonte = load_font("fontes/pont.pcx", NULL, NULL);// 0 - vidas
    inds[VIDAS].valor = 3;
    inds[PONTUACAO].fonte = load_font("fontes/pont.pcx", NULL, NULL);// 1 - pontuacao
    inds[PONTUACAO].valor = 0;
    inds[FASE].fonte = load_font("fontes/pont.pcx", NULL, NULL);// 2 - fase
    inds[FASE].valor = 1;

    blocos rB[numblocos];
    ativarblocos(rB, 'r');
    blocos gB[numblocos];
    ativarblocos(gB, 'g');
    blocos bB[numblocos];
    ativarblocos(bB, 'b');

    pal.img = load_bitmap("IMG/palheta.bmp", NULL);
    bolinha.img = load_bitmap("IMG/bolinha.bmp", NULL);
    pal.x = SCREEN_W/2-pal.img->w/2;
    pal.y = SCREEN_H/1.0733;
    pal.vel = VEL;

    initbolinha(&bolinha);

    LOCK_FUNCTION(fps);
    LOCK_VARIABLE(taxaatu);
    install_int_ex(fps, BPS_TO_TIMER(60));
    taxaatu = 0;
    while((!exit_program) && (!apertou(KEY_ESC)) && (fimJogo == 0))
    {
        while(taxaatu > 1)
        {
            if(iniciou != 0)
            {
                imprimeTela(pal, bolinha, rB, gB, bB, iniciou, inds, nome);
                move_bolinha(&bolinha, rB, gB, bB, &pal, inds, &iniciou);
                if(key[KEY_A] && pal.x > 0)
                    move_palheta(&pal, 'L', pal.vel);
                if(key[KEY_D] && pal.x < SCREEN_W-pal.img->w)
                    move_palheta(&pal, 'R', pal.vel);
            }
            else
            {
                if(apertou(KEY_SPACE))
                    iniciou = 1;
                if(key[KEY_A] && pal.x > 0)
                    move_palheta(&pal, 'L', pal.vel);
                if(key[KEY_D] && pal.x < SCREEN_W-pal.img->w)
                    move_palheta(&pal, 'R', pal.vel);
                initbolinha(&bolinha);
                imprimeTela(pal, bolinha, rB, gB, bB, iniciou, inds, nome);
            }

            if(inds[VIDAS].valor == -1)
            {
                gameover(); //pega pontuacao, mostra mensagem
                fimJogo = 1;
                record_upd(nome, inds[PONTUACAO].valor);
                break;
            }
            else if (inds[PONTUACAO].valor >= 7200)
            {
                imprimeTela(pal, bolinha, rB, gB, bB, iniciou, inds, nome);
                venceu();
                fimJogo = 1;
                record_upd(nome, inds[PONTUACAO].valor);
                break;
            }

            taxaatu--;
        }
    }

    destroy_bitmap(pal.img);
    for(i = 0; i < numblocos; i++)
    {
        destroy_bitmap(rB[i].img);
        destroy_bitmap(gB[i].img);
        destroy_bitmap(bB[i].img);
    }

}

/***************************************** CAMPO FUNCAO MENU ***************************************************/

int menu()
{
    BITMAP* buffer = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP* imagem = load_bitmap("IMG/logot.bmp", NULL);
    FONT* fmenu = load_font("fontes/fontemenu.pcx", NULL, NULL);
    MIDI* hallelujah = load_midi("hallelujah.mid");
    play_midi(hallelujah, 1);

    while(!exit_program)
    {
        keyboard_input();
        int xrefmenu = SCREEN_H*0.567;
        int acresx = 45;
        int yrefmenu = SCREEN_W/10;
        int cor = makecol(0x98,0x98,0xBE);
        int bgcolor = makecol(0,0x27,0x3f);
        clear_to_color(buffer, bgcolor);
        draw_sprite(buffer, imagem, SCREEN_W/3.2, 50);
        textout_ex(buffer, fmenu, "1 - JOGAR", yrefmenu, xrefmenu,cor, -1);
        textout_ex(buffer, fmenu, "2 - INSTRUCOES", yrefmenu, xrefmenu + acresx*1, cor, -1);
        textout_ex(buffer, fmenu, "3 - RECORDS", yrefmenu, xrefmenu + acresx*2, cor, -1);
        textout_ex(buffer, fmenu, "4 - SAIR", yrefmenu, xrefmenu + acresx*3, cor, -1);
        draw_sprite(screen, buffer, 0, 0);

        rest(20);
        if(apertou(KEY_1_PAD) || apertou(KEY_1))
        {
            destroi(fmenu, buffer, imagem, hallelujah);
            return 1;
        }
        else if(apertou(KEY_2_PAD) || apertou(KEY_2))
        {
            destroi(fmenu, buffer, imagem, hallelujah);
            return 2;
        }
        else if(apertou(KEY_3_PAD) || apertou(KEY_3))
        {
            destroi(fmenu, buffer, imagem, hallelujah);
            return 3;
        }
        else if(apertou(KEY_4_PAD) || apertou(KEY_4))
        {
            clear_to_color(screen, makecol(0,0x27,0x3f));
            textout_centre_ex(screen, fmenu, "ESCOLHA ERRADA", SCREEN_W/2, SCREEN_H/2, makecol(255, 0, 0), 0x880000);
            rest(2000);
            exit_program = 1;
            destroi(fmenu, buffer, imagem, hallelujah);
            return 4;
        }
        //textout_centre_ex(screen, font, "Nintendo", SCREEN_W/2, SCREEN_H/2, makecol(255, 255, 100), -1);
    }
    return -1;
}
void instrucoes()
{
    BITMAP* buffer = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP* imagem = load_bitmap("IMG/logot.bmp", NULL);
    FONT* fmenu = load_font("fontes/fontemenu.pcx", NULL, NULL);
    //MIDI* hallelujah = load_midi("hallelujah.mid");
    //play_midi(hallelujah, 1);

    while(!exit_program && !apertou(KEY_ESC))
    {
        keyboard_input();
        int yrefmenu = SCREEN_H*0.567;
        int acresy = 45;
        int xrefmenu = SCREEN_W/15;
        int cor = makecol(0x98,0x98,0xBE);
        int bgcolor = makecol(0,0x27,0x3f);
        clear_to_color(buffer, bgcolor);
        draw_sprite(buffer, imagem, SCREEN_W/3.2, 50);
        textout_ex(buffer, fmenu, "Teclas A e D movimentam a palheta.", xrefmenu, yrefmenu,cor, -1);
        textout_ex(buffer, fmenu, "O objetivo eh quebrar todos os blocos", xrefmenu, yrefmenu + acresy*1, cor, -1);
        textout_ex(buffer, fmenu, "sem que a bola toque o canto inferior da tela", xrefmenu, yrefmenu + acresy*2, cor, -1);
        //textout_ex(buffer, fmenu, "4 - SAIR", yrefmenu, xrefmenu + acresx*3, cor, -1);
        draw_sprite(screen, buffer, 0, 0);

        //rest(20);

        //textout_centre_ex(screen, font, "Nintendo", SCREEN_W/2, SCREEN_H/2, makecol(255, 255, 100), -1);
    }
}

void destroi(FONT* fmenu, BITMAP* buffer , BITMAP* imagem, MIDI* hallelujah)
{
    destroy_font(fmenu);
    destroy_bitmap(buffer);
    destroy_bitmap(imagem);
    destroy_midi(hallelujah);
}

/***************************************** CAMPO FUNCAO RECORD ***************************************************/
void ver_record()
{
    int i, njog, aux, pontuacao[5];
    char nome[5][11];
    FILE *fp;

    BITMAP* buffer = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP* imagem = load_bitmap("IMG/logot.bmp", NULL);
    FONT* fmenu = load_font("fontes/fontemenu.pcx", NULL, NULL);

    if((fp=fopen("Recordes.txt","r"))==NULL)
    {
        printf("Erro na abertura do arquivo");
        exit(1);
    }

    if (fscanf(fp, "%i\n", &njog)==EOF)
        njog = 0;


    keyboard_input();
    int yrefmenu = SCREEN_H*0.567;
    int xrefmenu = SCREEN_W/15;
    int cor = makecol(0x98,0x98,0xBE);
    int bgcolor = makecol(0,0x27,0x3f);
    clear_to_color(buffer, bgcolor);
    draw_sprite(buffer, imagem, SCREEN_W/3.2, 50);

    if(njog == 0)
    {
        textprintf_ex(buffer, fmenu, xrefmenu, yrefmenu, cor, -1, "Ainda nao existem recordes, seja o primeiro!!!");
    }

    for(i=0; (i<njog)&&(feof(fp)==0); i++)
    {
        fgets(nome[i],100, fp);
        aux = (strlen(nome[i]) - 1);
        nome[i][aux] =  '\0';
        fscanf(fp, "%i\n", &pontuacao[i]);
    }

    for(i=0; i<njog; i++)
    {
        textprintf_ex(buffer, fmenu, xrefmenu, yrefmenu + i*45, cor, -1, "%s %d", nome[i], pontuacao[i]);
    }

    draw_sprite(screen, buffer, 0, 0);

    while(!exit_program && !apertou(KEY_ESC));
}

void record_upd(char nome[], int pontuacao)
{
    int i=0, j, njog, auxint;
    FILE *fp;
    jogador *jog, aux;

    if ((fp = fopen("Recordes.txt","r+")) == NULL)
    {
        printf("Erro na abertura do arquivo\n");
        exit(1);
    }
    if (fscanf(fp, "%i\n", &njog)==EOF)
        njog = 0;

    if ((jog = malloc ((njog+1)*sizeof(jogador))) == NULL)
    {
        printf("Erro na Alocacao\n");
        exit(1);
    }

    for(i=0; (i<njog)&&(feof(fp)==0); i++)  // Lê do Arquivo
    {
        fgets(jog[i].nome,100, fp);
        auxint = (strlen(jog[i].nome) - 1);
        jog[i].nome[auxint] =  '\0';
        fscanf(fp, "%d\n", &jog[i].pontuacao);
        puts(jog[i].nome);
        printf("%d gols\n", jog[i].pontuacao);
    }

    strcpy(jog[i].nome, nome);              // Lê do Jogo
    jog[i].pontuacao = pontuacao;

    for(i=0; i<=njog; i++)
    {
        for(j=i+1; j<=njog; j++)
        {
            if (jog[i].pontuacao > jog[j].pontuacao)
            {
                aux = jog[j];
                jog[j] = jog[i];
                jog[i] = aux;
            }
        }
    }

    rewind(fp);

    if (njog !=5)
        fprintf(fp, "%d\n", njog+1);
    else
        fprintf(fp, "%d\n", njog);

    for(i=njog; i>=0; i--)
    {
        if(njog == 5 && i == 0)
        {
            break;
        }
        fputs(jog[i].nome, fp);
        fprintf(fp, "\n%i\n", jog[i].pontuacao);
    }

    free(jog);
    fclose(fp);

}

void keyboard_input()
{
   int i;

   for(i = 0; i < KEY_MAX; i++)
     teclas_anteriores[i] = key[i];

   //poll_keyboard();
}

int apertou(int TECLA)
{

    return(teclas_anteriores[TECLA] == 0 && key[TECLA] != 0);

}

int segurou(int TECLA)
{
    return(teclas_anteriores[TECLA] != 0 && key[TECLA] != 0);
}

int soltou(int TECLA)
{
    return(teclas_anteriores[TECLA] != 0 && key[TECLA] == 0);
}
