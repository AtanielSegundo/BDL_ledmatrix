---

# BITDOGLAB LED Matrix Library

Esta é uma biblioteca de cabeçalho único que segue o padrão [STB](https://github.com/nothings/stb). O objetivo é fornecer uma interface simples e eficiente para utilizar a matriz de LEDs endereçáveis presente na placa [BITDOGLAB](https://github.com/BitDogLab/BitDogLab).

Para mais informações sobre a matriz de LEDs endereçáveis, consulte o [documento de referência](https://github.com/BitDogLab/BitDogLab-C/blob/main/neopixel_pio/Readme.md).

---

## Dependências

Para compilar e vincular esta biblioteca, é necessário utilizar a toolchain do **Pi Pico SDK** e incluir as seguintes bibliotecas:

- `hardware/pio.h`
- `hardware/clocks.h`
- `pico/stdlib.h`

---

## Estruturas e Dados

### Estrutura `pixel_t`

Representa as cores dos LEDs, onde `W` indica o peso (intensidade) do LED, variando de **0.0** a **1.0**.

```c
typedef struct {
    uint8_t G, R, B;
    float W;
} pixel_t;
```

### Estrutura `LedMatrix`

Estrutura fundamental que representa a matriz de LEDs, contendo o número de linhas e colunas, a máquina de estados da interface, e os buffers para o estado atual e o próximo estado da matriz.

```c
typedef struct {
    int rows, cols;
    uint state_machine;
    PIO pio;
    pixel_t* state;
    pixel_t* next_state;
} LedMatrix;
```

---

## Funções

- ### Inicialização da Matriz

  ```c
  void bdl_matrixInit(LedMatrix *m, int control_pin, int rows, int cols);
  ```
  Inicializa a estrutura `LedMatrix`.

- ### Configuração de Pixel

  ```c
  void bdl_matrixSetPixel(LedMatrix *m, int row, int col, uint8_t r, uint8_t g, uint8_t b, float w);
  ```
  Modifica um LED, atribuindo a ele uma cor. A atualização é realizada no buffer `next_state`.

- ### Escrita na Matriz

  ```c
  void bdl_matrixWrite(LedMatrix *m);
  ```
  Aplica as alterações do buffer `next_state` ao buffer `state`, efetivando as mudanças realizadas.

- ### Limpeza da Matriz

  ```c
  void bdl_matrixClear(LedMatrix *m);
  ```
  Reseta todos os LEDs, definindo-os para zero.

- ### Início do Atualizador da Matriz

  ```c
  void bdl_startMatrixUpdater(LedMatrix *m, double fps);
  ```
  Inicia um temporizador que atualiza a matriz (chamando `bdl_matrixWrite`) a uma taxa predeterminada de frames por segundo (fps).

- ### Espera pela Atualização da Matriz

  ```c
  void bdl_waitMatrixUpdate();
  ```
  Bloqueia o fluxo do programa até que a matriz seja atualizada.

- ### Desenho de Linha na Matriz

  ```c
  void bdl_matrixDrawLine(LedMatrix *m, int row0, int col0, int row1, int col1, uint8_t r, uint8_t g, uint8_t b, float w);
  ```
  Desenha uma linha na matriz utilizando o algoritmo de [Bresenham](https://pt.wikipedia.org/wiki/Algoritmo_de_Bresenham).

---

## Exemplo

  Neste exemplo, o coração é desenhado na matriz 5x5 e sua cor muda gradualmente, gerando um efeito colorido dinâmico.
- ### Cmake Essenciais
```cmake
# Adiciona as bibliotecas de dependencias na pasta build
target_link_libraries( heart
        pico_stdlib
        pico_multicore
        hardware_pio
        hardware_clocks
        hardware_adc
        hardware_pwm
)

# Adiciona os caminhos para o cabeçalho da biblioteca
target_include_directories(heart PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/includes
)
```
- ### Código C

``` c
#define BDLED_IMPLEMENTATION    // NECESSÁRIO DEFINIR ANTES DE ADICIONAR O CABEÇALHO, PARA QUE AS IMPLEMENTAÇÕES SEJAM ADICIONADAS
                                // CASO SÓ DESEJE OS PROTÓTIPOS, NÃO É NECESSÁRIO ADICIONAR
#include "bdl_led_matrix.h"
#include "pico/stdlib.h"

#define LED_PIN 7               // Defina o pino de controle conforme sua placa, 7 é o padrão
#define MATRIX_SIZE 5
#define BRIGHT 0.05

uint8_t HEART_R  = 255;
uint8_t HEART_G  = 0;
uint8_t HEART_B  = 0;
int count = 1;

void draw_heart(LedMatrix *matrix);

int main() {
    stdio_init_all();
    LedMatrix matrix;
    bdl_matrixInit(&matrix, LED_PIN, MATRIX_SIZE, MATRIX_SIZE);
    bdl_startMatrixUpdater(&matrix, 60);
    while (true) {
        draw_heart(&matrix);
        sleep_ms(100);
    }
    return 0;
}

void draw_heart(LedMatrix *matrix) {
    bdl_matrixClear(matrix);
    bdl_matrixSetPixel(matrix, 0, 1, HEART_R, HEART_G, HEART_B, BRIGHT);
    bdl_matrixSetPixel(matrix, 0, 3, HEART_R, HEART_G, HEART_B, BRIGHT);
    bdl_matrixSetPixel(matrix, 1, 0, HEART_R, HEART_G, HEART_B, BRIGHT);
    bdl_matrixSetPixel(matrix, 1, 2, HEART_R, HEART_G, HEART_B, BRIGHT);
    bdl_matrixSetPixel(matrix, 1, 4, HEART_R, HEART_G, HEART_B, BRIGHT);
    bdl_matrixSetPixel(matrix, 2, 0, HEART_R, HEART_G, HEART_B, BRIGHT);
    bdl_matrixSetPixel(matrix, 2, 2, HEART_R, HEART_G, HEART_B, BRIGHT);
    bdl_matrixSetPixel(matrix, 2, 4, HEART_R, HEART_G, HEART_B, BRIGHT);
    bdl_matrixSetPixel(matrix, 3, 1, HEART_R, HEART_G, HEART_B, BRIGHT);
    bdl_matrixSetPixel(matrix, 3, 3, HEART_R, HEART_G, HEART_B, BRIGHT);
    bdl_matrixSetPixel(matrix, 4, 2, HEART_R, HEART_G, HEART_B, BRIGHT);
    // Converte o valor HSV para RGB, alterando a cor do coração ao longo do tempo
    bdl_hsv_to_rgb(count, 1, 1, &HEART_R, &HEART_G, &HEART_B);
    count = count + 5;
    if (count > 5 * 255) {
        count = 0; 
        HEART_R  = 255;
        HEART_G  = 0;
        HEART_B  = 0;
        bdl_matrixClear(matrix);
        sleep_ms(100);
    }
}

```

- ### Resultado

Confira abaixo um GIF demonstrando o efeito do coração colorido em ação:

![Exemplo de Coração Colorido](https://github.com/AtanielSegundo/BDL_ledmatrix/blob/main/example_bdl.gif)

- ### Observação

Para testar este projeto em sua placa, basta baixar este repositório e importar a pasta example como um projeto utilizando a extensão Raspberry Pi Pico Project.

---
