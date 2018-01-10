#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "funcoes.h"

volatile int exit_program = 0;
volatile long int timer;

void incr_timer()
{
    timer++;
}
END_OF_FUNCTION(incr_timer);

void fecha_programa()
{
    exit_program = 1;
}
END_OF_FUNCTION(fecha_programa)

int main()
{
    enum opmenu opcao;

    set_uformat(U_ASCII);
    allegro_init();
    install_timer();
    install_keyboard();
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
    set_color_depth(32);

    LOCK_VARIABLE(exit_program);
    LOCK_FUNCTION(fecha_programa);
    set_close_button_callback(fecha_programa);

    timer = 0;
    LOCK_FUNCTION(incr_timer);
    LOCK_VARIABLE(timer);
    install_int_ex(incr_timer, BPS_TO_TIMER(1000));

    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);

    set_window_title("Adadur Pro W2");



    do
    {
        //printf("%i\n", (int)timer);
        opcao = menu();

        stop_midi();
        switch(opcao)
        {
            case joga:  printf("jogar!\n");
                        jogar();
                        break;

            case instr: printf("Jogace acim: \n");
                        instrucoes();
                        break;

            case rec:   printf("Voce eh o melhor!\n");
                        ver_record();
                        break;

            case sair:  printf("Voce saiu!\n");
                        break;

            default:    exit(1);

        }

    }while(!exit_program);

    allegro_exit();

    return 0;
}
END_OF_MAIN()
