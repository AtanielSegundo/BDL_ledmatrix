#define BDLED_IMPLEMENTATION    // NECESSÁRIO DEFINIR ANTES DE ADICIONAR O CABEÇALHO, PARA QUE AS IMPLEMENTAÇÕES SEJAM ADICIONADAS
                                // CASO SÓ DESEJE OS PROTÓTIPOS, NÃO É NECESSÁRIO ADICIONAR
#include "bdl_led_matrix.h"

#define LED_PIN 7      // Defina o pino de controle conforme sua placa, 7 é o padrão
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