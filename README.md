# Nome do Projeto
- Localizador Inteligente
## Descrição

"Localizador Inteligente" é uma solução que ensina e indica os pontos cardeais (norte, sul, leste e oeste) utilizando os periféricos da placa BitDogLab. 
O programa inicia com uma orientação para posicionamento adequado. Ao mover o joystick, o display OLED exibe o ponto cardeal correspondente, enquanto o buzzer 
emite um som específico para cada direção, tornando a navegação acessível também para pessoas com deficiência visual.

## Tecnologias Utilizadas

- 1 Display oled de 0,96 128x64
- 1 Joystick analógico, tipo KY023
- 1 Buzzers passivo 

## Demonstração do código e execução do projeto em vídeo: 

<!-- Link do vídeo para o youtube ou drive -->

## Instalação

O projeto faz uso da placa raspberry pi pico, que utiliza o microcontrolador RP2040 e está inserida na placa bitdoglab. Para fazer o uso do projeto, é necessário fazer a instalação e configuração do kit de desenvolvimento de software da placa. 

Para utilização do projeto, você deve seguir alguns passos: 

-Download do projeto zip e extraí-lo no seu gerenciador de arquivos
![image](https://github.com/user-attachments/assets/286da82a-0b3f-4c88-9735-dc42db5f65ab)

-Após isso, é necessário utilizar o Visual Studio Code com a extensão da raspberry pi pico já instalada e configurada no Vscode.  
![image](https://github.com/user-attachments/assets/ea136130-12f9-4a22-b1c7-2e18eb600661)

-Importe o projeto, clicando na extensão da raspberry e clicando em "import project" 
![image](https://github.com/user-attachments/assets/f82ecf64-4b00-4c80-97dc-6e98b745f2f7)

-A extensão da raspberry pi pico já configurada disponibilizará botões que permitem compilar o código e roda na sua placa bitdoglab. Então, clique em compilar, coloque sua placa em modo bootsel, aperte Run e veja o projeto ser executado!
![image](https://github.com/user-attachments/assets/393b8ae8-7abc-400b-8254-790bd8cf2d4f)

## Estrutura do Projeto

O projeto está estruturado com as seguintes pastas e arquivos: 
-Arquivo de configuração do Display oled 
![image](https://github.com/user-attachments/assets/5fa74153-6d1d-420a-b88e-b07614a92bbe)
Esse arquivo contém bibliotecas que são essenciais para a correta configuração do display oled

-Biblioteca com os Bitmaps (imagens em binário para a geração de imagens) 

![image](https://github.com/user-attachments/assets/ca89f43d-dcec-4be8-bd74-bf2f7f354806)

-Biblioteca para a geração dos sons que são emitidos pelos buzzers

![image](https://github.com/user-attachments/assets/fa76910c-984e-4a87-a0da-f5af5b7bebe5)

-Arquivo Cmake para build do nosso projeto

![image](https://github.com/user-attachments/assets/f80bf69b-d96b-42ad-9f1b-33d019a44c93)

## Estrutura do código principal 

Configuração dos pinos correspondentes aos periféricos na placa raspberry pi pico w:
```c
const uint I2C_SDA = 14; // Inicializando o pino de transmissão de dados do 
const uint I2C_SCL = 15;

const uint BUZZER_PIN = 21; //Inicializando o pino do Buzzer

const uint vRx = 26; //Inicializando o pino correspondente ao eixo x do joystick
const uint vRy = 27; //Inicializando o pino correspondente ao eixo y do joystick 
const uint ADC_CHANEL_0 = 0; //Inicialização do canal 0 do conversor AD
const uint ADC_CHANEL_1 = 1; //Inicialização do canal 1 do conversor AD
const uint SW = 22; //Inicializando o pino do botão do joystick 
```

Inicializando o joystick com conversor AD: 
```c
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
```

Inicialização do BUZZER com PWM
```c
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
```

Funções para tocar no buzzer os sons correspondentes à direção apontada
```c
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
```

Inicialização do protocolo i2c para uso do display oled:
```c
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

// Parte do código para exibir a linha no display 
/**
    ssd1306_draw_line(ssd, 10, 10, 100, 50, true);
    render_on_display(ssd, &frame_area);
*/
    ssd1306_t ssd_bm;
    ssd1306_init_bm(&ssd_bm, 128, 64, false, 0x3C, i2c1);
    ssd1306_config(&ssd_bm);
```

Loop infinito para execução da rotina do programa
```c
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
```
