#include <avr/io.h>

// ---------------------- Definições de bits ----------------------
#define BIT(reg,bit)        (reg & (1<<bit))
#define SETBIT(reg,bit)     reg |=  (1<<bit)
#define CLRBIT(reg,bit)     reg &= ~(1<<bit)

// -------------------- Controle do display 7 segmentos --------------------
#define DISP7SEG_UNI_ON     SETBIT(PORTB,0)
#define DISP7SEG_UNI_OFF    CLRBIT(PORTB,0)
#define DISP7SEG_DEZ_ON     SETBIT(PORTB,1)
#define DISP7SEG_DEZ_OFF    CLRBIT(PORTB,1)

#define DIGITOS7SEG_SIZE    16

char digitos7seg[DIGITOS7SEG_SIZE] = {
    0x3F, 0x06, 0x5B, 0x4F,
    0x66, 0x6D, 0x7D, 0x07,
    0x7F, 0x6F, 0x77, 0x7C,
    0x39, 0x5E, 0x79, 0x71
};

void disp7seg_init(void)
{
    DDRD  = 0xFF;    // PORTD como saída para segmentos
    PORTD = 0x00;    // Inicia desligado
    SETBIT(DDRB, 0); // PB0 como saída - unidade
    SETBIT(DDRB, 1); // PB1 como saída - dezena
}

unsigned char disp7seg_scan = 0;

void disp7seg(unsigned char d)
{
    switch(disp7seg_scan)
    {
        case 0: // Unidade
            DISP7SEG_DEZ_OFF;
            PORTD = digitos7seg[d % 10];
            DISP7SEG_UNI_ON;
            break;
        case 1: // Dezena
            DISP7SEG_UNI_OFF;
            PORTD = digitos7seg[(d / 10) % 10];
            DISP7SEG_DEZ_ON;
            break;
    }
    ++disp7seg_scan;
    disp7seg_scan = disp7seg_scan % 2;
}

// ---------------------- Leitura de teclas -----------------------
#define B0      !BIT(PINB,5)
#define B1      !BIT(PINB,4)

void teclas_init(void)
{
    CLRBIT(DDRB, 5); // PB5 como entrada
    CLRBIT(DDRB, 4); // PB4 como entrada
    SETBIT(PORTB,5); // Pull-up ativado PB5
    SETBIT(PORTB,4); // Pull-up ativado PB4
}

char b0(void) { return B0; }
char b1(void) { return B1; }

char b0_prev = 0;
char b1_prev = 0;

char b0_rising_edge(void)
{
    if (B0 && !b0_prev)
    {
        b0_prev = B0;
        return 1;
    }
    else
    {
        b0_prev = B0;
        return 0;
    }
}

char b0_falling_edge(void)
{
    if (!B0 && b0_prev)
    {
        b0_prev = B0;
        return 1;
    }
    else
    {
        b0_prev = B0;
        return 0;
    }
}

char b1_rising_edge(void)
{
    if (B1 && !b1_prev)
    {
        b1_prev = B1;
        return 1;
    }
    else
    {
        b1_prev = B1;
        return 0;
    }
}

char b1_falling_edge(void)
{
    if (!B1 && b1_prev)
    {
        b1_prev = B1;
        return 1;
    }
    else
    {
        b1_prev = B1;
        return 0;
    }
}

// ----------------------------- main ------------------------------
int main(void)
{
    char i = 0;
    disp7seg_init();
    teclas_init();

    while(1)
    {
        if(b0_rising_edge())
        {
            i = (i < 99) ? i + 1 : 0;
        }

        if(b1_falling_edge())
        {
            i = (i > 0) ? i - 1 : 99;
        }

        disp7seg(i);
    }
}

