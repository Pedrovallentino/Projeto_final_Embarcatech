# 📍 Localizador Inteligente

## 📝 Descrição

O **Localizador Inteligente** é uma solução inovadora que ensina e indica os pontos cardeais (**norte, sul, leste, oeste, nordeste, noroeste, sudeste e sudoeste**) utilizando os periféricos da placa **BitDogLab**.

- 🎮 **Interação intuitiva**: O usuário move um **joystick analógico** para explorar as direções.
- 🖥️ **Feedback visual**: O **display OLED** exibe o ponto cardeal correspondente.
- 🔊 **Acessibilidade aprimorada**: Um **buzzer passivo** emite um som distinto para cada direção, auxiliando pessoas com deficiência visual.

---
## 🛠️ Tecnologias Utilizadas

- 🖥️ **Display OLED** (0,96" 128x64)
- 🎮 **Joystick Analógico** (KY-023)
- 🔊 **Buzzer Passivo**
- 🎛️ **Raspberry Pi Pico** (BitDogLab)

---
## 🎥 Demonstração do Projeto

🔗 *Confira o código em execução no seguinte vídeo: https://youtu.be/QbzM4MiIZrE

---
## 🚀 Instalação e Configuração

Este projeto utiliza a **Raspberry Pi Pico** integrada à **BitDogLab**. Para rodá-lo, siga os passos abaixo:

### 1️⃣ Baixar o Projeto
📥 Faça o download do projeto em formato **.zip** e extraia os arquivos em seu gerenciador.

### 2️⃣ Configurar o Ambiente de Desenvolvimento
⚙️ Utilize o **Visual Studio Code** com a **extensão da Raspberry Pi Pico** instalada.

### 3️⃣ Importar o Projeto
📂 Dentro da extensão da Raspberry, clique em **"Import Project"** e selecione a pasta do projeto.

### 4️⃣ Compilar e Executar
▶️ Conecte sua placa em **modo BOOTSEL**, compile o código e clique em **Run** para iniciar o projeto!

---
## 📂 Estrutura do Projeto

📌 O projeto está organizado da seguinte forma:

- 📜 **config_display.c** → Configuração do display OLED
- 🖼️ **bitmaps.h** → Biblioteca de imagens binárias
- 🔊 **buzzer_sounds.c** → Sons correspondentes a cada direção
- 🛠️ **CMakeLists.txt** → Configuração de build

---
## 🏗️ Estrutura do Código Principal

🔹 **Configuração de Pinos**:
```c/
/INICIALIZAÇÃO DOS PINOS

const uint I2C_SDA = 14; // Inicializando o pino de transmissão de dados do 
const uint I2C_SCL = 15;

const uint BUZZER_PIN = 21; //Inicializando o pino do Buzzer

const uint vRx = 26; //Inicializando o pino correspondente ao eixo x do joystick
const uint vRy = 27; //Inicializando o pino correspondente ao eixo y do joystick 
const uint ADC_CHANEL_0 = 0; //Inicialização do canal 0 do conversor AD
const uint ADC_CHANEL_1 = 1; //Inicialização do canal 1 do conversor AD
const uint SW = 22; //Inicializando o pino do botão do joystick 
```

🔹 **Inicialização do Joystick**:
```c
// SETUP DO JOYSTICK

// Função para inicializar o joystick
void setup_joystick()
{

    adc_init();
    adc_gpio_init(vRx);
    adc_gpio_init(vRy);

    gpio_init(SW);
    gpio_set_dir(SW, GPIO_IN);
    gpio_pull_up(SW);
}

// Função faz a leitura da posição do joystick
void joystick_read_axist(uint16_t *eixo_x, uint16_t *eixo_y)
{
    adc_select_input(ADC_CHANEL_0);
    sleep_us(2);
    *eixo_x = adc_read();

    adc_select_input(ADC_CHANEL_1);
    sleep_us(2);
    *eixo_y = adc_read();
}
```

🔹 **Tocar sons correspondentes a cada direção**:
```c
// SETUP DO BUZZER

// Inicializa o PWM no pino do buzzer
void pwm_init_buzzer(uint pin)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f); // Ajusta divisor de clock
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0); // Desliga o PWM inicialmente
}

// Toca uma nota com a frequência e duração especificadas
void play_tone(uint pin, uint frequency, uint duration_ms)
{
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / frequency - 1;

    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle

    sleep_ms(duration_ms);

    pwm_set_gpio_level(pin, 0); // Desliga o som após a duração
    sleep_ms(50);               // Pausa entre notas
}

// Função para tocar o som que indica Leste
void toque_som_do_leste(uint pin)
{
    for (int i = 0; i < sizeof(som_do_leste) / sizeof(som_do_leste[0]); i++)
    {
        if (som_do_leste[i] == 0)
        {
            sleep_ms(som_do_leste_duracao[i]);
        }
        else
        {
            play_tone(pin, som_do_leste[i], som_do_leste_duracao[i]);
        }
    }
}

// Função para tocar o som que indica Oeste
void toque_som_do_oeste(uint pin)
{
    for (int i = 0; i < sizeof(som_do_oeste) / sizeof(som_do_oeste[0]); i++)
    {
        if (som_do_oeste[i] == 0)
        {
            sleep_ms(som_do_oeste_duracao[i]);
        }
        else
        {
            play_tone(pin, som_do_oeste[i], som_do_oeste_duracao[i]);
        }
    }
}

// Função para tocar o som que indica Norte
void toque_som_do_norte(uint pin)
{
    for (int i = 0; i < sizeof(som_do_norte) / sizeof(som_do_norte[0]); i++)
    {
        if (som_do_norte[i] == 0)
        {
            sleep_ms(som_do_norte_duracao[i]);
        }
        else
        {
            play_tone(pin, som_do_norte[i], som_do_norte_duracao[i]);
        }
    }
}

// Função para tocar o som que indica Sul
void toque_som_do_sul(uint pin)
{
    for (int i = 0; i < sizeof(som_do_sul) / sizeof(som_do_sul[0]); i++)
    {
        if (som_do_sul[i] == 0)
        {
            sleep_ms(som_do_sul_duracao[i]);
        }
        else
        {
            play_tone(pin, som_do_sul[i], som_do_sul_duracao[i]);
        }
    }
}

// Função para tocar o som que indica Nordeste
void toque_som_do_nordeste(uint pin)
{
    for (int i = 0; i < sizeof(som_do_nordeste) / sizeof(som_do_nordeste[0]); i++)
    {
        if (som_do_nordeste[i] == 0)
        {
            sleep_ms(som_do_nordeste_duracao[i]);
        }
        else
        {
            play_tone(pin, som_do_nordeste[i], som_do_nordeste_duracao[i]);
        }
    }
}

// Função para tocar o som que indica Sudeste
void toque_som_do_sudeste(uint pin)
{
    for (int i = 0; i < sizeof(som_do_sudeste) / sizeof(som_do_sudeste[0]); i++)
    {
        if (som_do_sudeste[i] == 0)
        {
            sleep_ms(som_do_sudeste_duracao[i]);
        }
        else
        {
            play_tone(pin, som_do_sudeste[i], som_do_sudeste_duracao[i]);
        }
    }
}

// Função para tocar o som que indica Sudoeste
void toque_som_do_sudoeste(uint pin)
{
    for (int i = 0; i < sizeof(som_do_sudoeste) / sizeof(som_do_sudoeste[0]); i++)
    {
        if (som_do_sudoeste[i] == 0)
        {
            sleep_ms(som_do_sudoeste_duracao[i]);
        }
        else
        {
            play_tone(pin, som_do_sudoeste[i], som_do_sudoeste_duracao[i]);
        }
    }
}

// Função para tocar o som que indica Noroeste
void toque_som_do_noroeste(uint pin)
{
    for (int i = 0; i < sizeof(som_do_noroeste) / sizeof(som_do_noroeste[0]); i++)
    {
        if (som_do_noroeste[i] == 0)
        {
            sleep_ms(som_do_noroeste_duracao[i]);
        }
        else
        {
            play_tone(pin, som_do_noroeste[i], som_do_noroeste_duracao[i]);
        }
    }
}
```

🔹 **Exibição no OLED** e inicialização da comunicação i2c:
```c
  // Função para inicialização e configurações básicas da comunicação i2c
void setup_i2c()
{
    i2c_init(i2c1, 3600000); // Velocidade da comunicação - Está no máximo suportado pelo hardware, mas pode ser diminuida
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Definindo as variáveis declaradas acima como pinos para a comunicação do tipo i2c
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA); // Define que os dois pinos usarão resistores de  pull-up internos, necessário para a leitura correta das informações na comunicação i2c
    gpio_pull_up(I2C_SCL);
}

// Função para a inicialização do de outras funções e variáveis necessárias para o funcionamento do Display Oled
void display_init()
{
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

    ssd1306_init_bm(&ssd_bm, 128, 64, false, 0x3C, i2c1);
    ssd1306_config(&ssd_bm);
}
```

🔹 **Função main e Loop Infinito**:
```c
int main()
{
    // Função que inicializa todas as funções declaradas anteriormente
    setup();

    // Loop infinito para execução do programa
    while (true)
    {
        joystick_read_axist(&valor_x, &valor_y);
        // Valor lido pelo Conversor quando o usuário aponta o Joystick para a direção Oeste
        if (valor_x <= 20)
        {
            ssd1306_draw_bitmap(&ssd_bm, oeste_128_64);
            toque_som_do_oeste(BUZZER_PIN);
        }
        // Valor lido pelo Conversor quando o usuário aponta o Joystick para a direção Leste
        else if (valor_x > 4000)
        {
            ssd1306_draw_bitmap(&ssd_bm, leste_128_64);
            toque_som_do_leste(BUZZER_PIN);
        }
        // Valor lido pelo Conversor quando o usuário aponta o Joystick para a direção Norte
        else if (valor_y <= 20)
        {
            ssd1306_draw_bitmap(&ssd_bm, norte_128_64);
            toque_som_do_norte(BUZZER_PIN);
        }
        // Valor lido pelo Conversor quando o usuário aponta o Joystick para a direção Sul
        else if (valor_y >= 4070)
        {
            ssd1306_draw_bitmap(&ssd_bm, sul_128_64);
            toque_som_do_sul(BUZZER_PIN);
        }
        // Valor lido pelo Conversor quando o usuário aponta o Joystick para a direção Nordeste
        else if (valor_x > 3300 && valor_y < 1000)
        {
            ssd1306_draw_bitmap(&ssd_bm, nordeste_128_64);
            toque_som_do_nordeste(BUZZER_PIN);
        }
        // Valor lido pelo Conversor quando o usuário aponta o Joystick para a direção Sudeste
        else if (valor_x < 3800 && valor_x > 1000 && valor_y > 3800 && valor_y <= 4070)
        {
            ssd1306_draw_bitmap(&ssd_bm, sudeste_128_64);
            toque_som_do_sudeste(BUZZER_PIN);
        }
        // Valor lido pelo Conversor quando o usuário aponta o Joystick para a direção Sudoeste
        else if (valor_x < 1000 && valor_y > 3300 && valor_y < 3800)
        {
            ssd1306_draw_bitmap(&ssd_bm, sudoeste_128_64);
            toque_som_do_sudoeste(BUZZER_PIN);
        }
        // Valor lido pelo Conversor quando o usuário aponta o Joystick para a direção Noroeste
        else if (valor_x < 1000 && valor_x > 20 && valor_y > 20 && valor_y < 1000)
        {
            ssd1306_draw_bitmap(&ssd_bm, noroeste_128_64);
            toque_som_do_noroeste(BUZZER_PIN);
        }
        // Valor de X lido no joystick quando o joystick está no centro, sem nenhum movimento
        else if (valor_x > 1900 && valor_x < 2020)
        {
            ssd1306_draw_bitmap(&ssd_bm, joystick_centro);
        }
        sleep_ms(1000);
    }
    return 0;
}
```

---
## 📜 Licença

Este projeto está licenciado sob a Licença MIT. Consulte o arquivo [LICENSE](LICENSE) para mais detalhes.

---
## 🤝 Contribuição

Se você deseja contribuir com melhorias, siga estes passos:

1. **Fork** o repositório 📌
2. Crie uma **nova branch** 🔀
3. **Commit** suas alterações 📢
4. **Abra um Pull Request** 🚀

---

Desenvolvido por Pedro Valentino.

