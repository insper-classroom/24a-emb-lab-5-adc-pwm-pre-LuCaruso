#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;
    int soma = 0;
    int v[5] = {0}; // Array para armazenar os últimos 5 valore

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // Adiciona o novo dado ao array e remove o mais antigo, mantendo uma janela de 5 valores
            soma -= v[0];
            soma += data;
            for (int i = 0; i < 4; i++) {
                v[i] = v[i+1];
            }
            v[4] = data;

            // Calcula a média móvel
            int media_movel = soma / 5;

            // Imprime o dado filtrado na UART
            printf("Dado filtrado: %d\n", media_movel);
        }

        // Deixar esse delay
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
