#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Estrutura para armazenar logs
typedef struct {
    char log[BUFFER_SIZE];
    struct Log *next;
} Log;

Log *log_head = NULL;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
time_t server_start_time;

// Função para adicionar logs ao monitor
void add_log(const char *message) {
    pthread_mutex_lock(&log_mutex);

    Log *new_log = (Log *)malloc(sizeof(Log));
    strcpy(new_log->log, message);
    new_log->next = NULL;

    if (log_head == NULL) {
        log_head = new_log;
    } else {
        Log *temp = log_head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_log;
    }

    pthread_mutex_unlock(&log_mutex);
}

// Função para imprimir logs a cada 10 segundos
void *log_printer(void *arg) {
    while (1) {
        sleep(10);

        pthread_mutex_lock(&log_mutex);

        Log *temp = log_head;
        while (temp != NULL) {
            printf("%s\n", temp->log);
            temp = temp->next;
        }

        pthread_mutex_unlock(&log_mutex);
    }
    return NULL;
}

// Função para processar comandos recebidos pelos clientes
void process_command(int client_socket, char *client_ip) {
    char buffer[BUFFER_SIZE];
    int read_size;
    time_t current_time;

    while ((read_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';

        // Remover caracteres de nova linha e retorno de carro
        char *command = strtok(buffer, "\r\n");

        char response[BUFFER_SIZE];
        if (strcmp(command, "DATETIME") == 0) {
            // Retorna a data e hora atuais
            time(&current_time);
            struct tm *timeinfo = localtime(&current_time);
            strftime(response, sizeof(response), "%Y-%m-%d %H:%M:%S\n", timeinfo);
        } else if (strcmp(command, "RNDNUMBER") == 0) {
            // Gera um número aleatório entre 1 e 100
            int random_number = rand() % 100 + 1;
            sprintf(response, "%d\n", random_number);
        } else if (strcmp(command, "UPTIME") == 0) {
            // Informa o tempo de execução do servidor
            time_t uptime = time(NULL) - server_start_time;
            sprintf(response, "Servidor em execução há %ld segundos\n", uptime);
        } else if (strcmp(command, "INFO") == 0) {
            // Retorna a mensagem informativa do servidor
            strcpy(response, "Servidor TCP v1.0\n");
        } else if (strcmp(command, "BYE") == 0) {
            // Encerra a conexão
            strcpy(response, "Conexão encerrada\n");
            send(client_socket, response, strlen(response), 0);
            break;
        } else {
            // Comando inválido
            strcpy(response, "Comando inválido\n");
        }

        // Enviar resposta ao cliente
        send(client_socket, response, strlen(response), 0);

        // Criar log
        char log_message[BUFFER_SIZE];
        time(&current_time);
        struct tm *timeinfo = localtime(&current_time);
        char time_str[BUFFER_SIZE];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S\n", timeinfo);
        sprintf(log_message, "[%s] Cliente %s: Comando = %s, Resposta = \"%s\"", time_str, client_ip, command, response);

        // Adicionar log
        add_log(log_message);
    }

    close(client_socket);
}

// Função para lidar com novos clientes
void *client_handler(void *client_socket) {
    int sock = *(int *)client_socket;
    free(client_socket);

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    getpeername(sock, (struct sockaddr *)&client_addr, &client_len);
    char *client_ip = inet_ntoa(client_addr.sin_addr);

    process_command(sock, client_ip);

    return NULL;
}

int main() {
    int server_socket, client_socket, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id, log_thread;

    // Inicializar o gerador de números aleatórios e registrar o tempo de início do servidor
    srand(time(NULL));
    time(&server_start_time);

    // Criar socket do servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Erro ao criar o socket");
        return 1;
    }

    // Configurar o endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Associar o socket ao endereço e porta
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro no bind");
        return 1;
    }

    // Escutar conexões
    listen(server_socket, MAX_CLIENTS);
    printf("Servidor em execução na porta %d...\n", PORT);

    // Criar thread para impressão dos logs
    pthread_create(&log_thread, NULL, log_printer, NULL);

    // Aceitar conexões de clientes
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len))) {
        new_sock = malloc(sizeof(int));
        *new_sock = client_socket;

        // Criar thread para cada cliente
        if (pthread_create(&thread_id, NULL, client_handler, (void *)new_sock) < 0) {
            perror("Erro ao criar thread");
            return 1;
        }

        pthread_detach(thread_id);
    }

    if (client_socket < 0) {
        perror("Erro ao aceitar conexão");
        return 1;
    }

    close(server_socket);
    return 0;
}
