/*
 * Projeto: Localizador Inteligente
 * Autor: Pedro Valentino
 * Instituição: Instituto Federal do Piauí - IFPI
 * 
 * Descrição:
 * Este código implementa um sistema de localização baseado na direção de um joystick,
 * utilizando um microcontrolador com conversor ADC de 12 bits. Além disso, exibe as direções 
 * em um display OLED e emite sons correspondentes a cada ponto cardeal por meio de um buzzer.
 * 
 * Data de criação: [12/02/2025]
 * Última modificação: [14/02/2025]
 * 
 * Copyright (c) 2025 Pedro Valentino
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "bitmap.h"
#include "songs.h"

//INICIALIZAÇÃO DOS PINOS

const uint I2C_SDA = 14; // Inicializando o pino de transmissão de dados do 
const uint I2C_SCL = 15;

const uint BUZZER_PIN = 21; //Inicializando o pino do Buzzer

const uint vRx = 26; //Inicializando o pino correspondente ao eixo x do joystick
const uint vRy = 27; //Inicializando o pino correspondente ao eixo y do joystick 
const uint ADC_CHANEL_0 = 0; //Inicialização do canal 0 do conversor AD
const uint ADC_CHANEL_1 = 1; //Inicialização do canal 1 do conversor AD
const uint SW = 22; //Inicializando o pino do botão do joystick 


//SETUP DO JOYSTICK

//Função para inicializar o joystick
void setup_joystick(){
    
    adc_init(); 
    adc_gpio_init(vRx); 
    adc_gpio_init(vRy); 

    gpio_init(SW); 
    gpio_set_dir(SW, GPIO_IN); 
    gpio_pull_up(SW); 
}

//Função faz a leitura da posição do joystick
void joystick_read_axist(uint16_t *eixo_x, uint16_t *eixo_y){
    adc_select_input(ADC_CHANEL_0); 
    sleep_us(2); 
    *eixo_x = adc_read(); 

    adc_select_input(ADC_CHANEL_1); 
    sleep_us(2); 
    *eixo_y = adc_read(); 
}

//SETUP DO BUZZER 

// Inicializa o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f); // Ajusta divisor de clock
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0); // Desliga o PWM inicialmente
}

// Toca uma nota com a frequência e duração especificadas
void play_tone(uint pin, uint frequency, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / frequency - 1;

    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle

    sleep_ms(duration_ms);

    pwm_set_gpio_level(pin, 0); // Desliga o som após a duração
    sleep_ms(50); // Pausa entre notas
}

// Função para tocar o som que indica Leste 
void toque_som_do_leste(uint pin) {
    for (int i = 0; i < sizeof(som_do_leste) / sizeof(som_do_leste[0]); i++) {
        if (som_do_leste[i] == 0) {
            sleep_ms(som_do_leste_duracao[i]);
        } else {
            play_tone(pin, som_do_leste[i], som_do_leste_duracao[i]);
        }
    }
}

// Função para tocar o som que indica Oeste 
void toque_som_do_oeste(uint pin) {
    for (int i = 0; i < sizeof(som_do_oeste) / sizeof(som_do_oeste[0]); i++) {
        if (som_do_oeste[i] == 0) {
            sleep_ms(som_do_oeste_duracao[i]);
        } else {
            play_tone(pin, som_do_oeste[i], som_do_oeste_duracao[i]);
        }
    }
}

//Função para tocar o som que indica Norte 
void toque_som_do_norte(uint pin) {
    for (int i = 0; i < sizeof(som_do_norte) / sizeof(som_do_norte[0]); i++) {
        if (som_do_norte[i] == 0) {
            sleep_ms(som_do_norte_duracao[i]);
        } else {
            play_tone(pin, som_do_norte[i], som_do_norte_duracao[i]);
        }
    }
}

//Função para tocar o som que indica Sul 
void toque_som_do_sul(uint pin) {
    for (int i = 0; i < sizeof(som_do_sul) / sizeof(som_do_sul[0]); i++) {
        if (som_do_sul[i] == 0) {
            sleep_ms(som_do_sul_duracao[i]);
        } else {
            play_tone(pin, som_do_sul[i], som_do_sul_duracao[i]);
        }
    }
}

//Inicializando a entrada e saída padrão e a função de inialização do joystick
void setup(){
    stdio_init_all(); 
    setup_joystick(); 
}

int main()
{
    uint16_t valor_x;
    uint16_t valor_y; 

    //incialização do buzzer
    pwm_init_buzzer(BUZZER_PIN);

    //Função que inicializa a entrada e saída padrão e a função para configuração do joystick
    setup(); 

    //INCIALIZAÇÃO DO DISPLAY:

    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 3000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

restart:

// Parte do código para exibir a linha no display (algoritmo de Bresenham)
/**
    ssd1306_draw_line(ssd, 10, 10, 100, 50, true);
    render_on_display(ssd, &frame_area);
*/
    ssd1306_t ssd_bm;
    ssd1306_init_bm(&ssd_bm, 128, 64, false, 0x3C, i2c1);
    ssd1306_config(&ssd_bm);


    //Loop infinito para execução do programa
    while(true) {
        joystick_read_axist(&valor_x, &valor_y); 

        //Lê os valores do joystick para exibir no monitor serial 
        printf("X: %d\n", valor_x); 
        printf("Y: %d\n", valor_y); 
        printf("Botão: %d\n", gpio_get(SW));

        //Valor de X lido no joystick quando o usuário aponta para direção leste
        if(valor_x >= 4000){
            ssd1306_draw_bitmap(&ssd_bm, leste_128_64);
            toque_som_do_leste(BUZZER_PIN); 
        } 

        //Valor de X lido no joystick quando o usuário aponta para direção oeste
        if(valor_x <= 50){
            ssd1306_draw_bitmap(&ssd_bm, oeste_128_64);
            toque_som_do_oeste(BUZZER_PIN); 
        }

        //Valor de X lido no joystick quando o usuário aponta para direção sul
        if(valor_y >= 4000){
            ssd1306_draw_bitmap(&ssd_bm, sul_128_64);
            toque_som_do_sul(BUZZER_PIN); 
        }

        //Valor de X lido no joystick quando o usuário aponta para direção norte
        if(valor_y <= 50){
            ssd1306_draw_bitmap(&ssd_bm, norte_128_64);
            toque_som_do_norte(BUZZER_PIN); 
        }

        //Valor de X lido no joystick quando o joystick está no centro 
        if(valor_y < 2020 && valor_y < 2020){
            ssd1306_draw_bitmap(&ssd_bm, joystick_centro);
        }
        sleep_ms(1000);
    }
    return 0;
}