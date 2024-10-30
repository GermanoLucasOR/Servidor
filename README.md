Servidor TCP com Monitor e Comunicação via Sockets
Este projeto implementa um servidor TCP que aceita conexões simultâneas de clientes e responde a comandos específicos enviados por esses clientes. O servidor também utiliza um monitor para registrar logs de cada comando executado, incluindo a data e hora do recebimento e a resposta enviada.

Funcionalidades
Comandos suportados:

DATETIME: Retorna a data e hora atuais.
RNDNUMBER: Gera e retorna um número aleatório entre 1 e 100.
UPTIME: Informa o tempo (em segundos) desde que o servidor foi iniciado.
INFO: Retorna uma mensagem informativa do servidor, como "Servidor TCP v1.0".
BYE: Encerra a conexão com o cliente.
Monitor de Logs:

Armazena logs contendo:
Data e hora do comando recebido.
Comando recebido e resposta enviada.
Usa mutexes para garantir exclusão mútua no acesso aos logs.
A cada 10 segundos, uma thread separada imprime os logs acumulados no terminal do servidor.
Conexões simultâneas:

O servidor permite conexões simultâneas de vários clientes e trata cada cliente em uma thread separada.
O cliente pode enviar múltiplos comandos antes de encerrar a conexão com o comando BYE.
Como executar o projeto
Pré-requisitos
GCC (Compilador de C)
Biblioteca pthread para threads (incluída na maioria dos sistemas Unix-like)
Cliente Telnet (para testar a comunicação com o servidor)

Passos para executar
1. Clone o repositório (se aplicável) ou copie o código para sua máquina local.

2. Compilar o código:
   Execute o seguinte comando para compilar o servidor:

bash
gcc servidor.c -o servidor -lpthread

3. Executar o servidor:
   Após compilar o código, execute o servidor com o comando:

bash
./servidor

   O servidor estará em execução na porta 8080 e aguardará conexões de clientes.

4. Testar com Telnet:
   Abra um novo terminal e conecte-se ao servidor usando o cliente Telnet:

bash
telnet 127.0.0.1 8080

Após a conexão, você pode enviar os seguintes comandos e verificar as respostas no cliente Telnet:

DATETIME
RNDNUMBER
UPTIME
INFO
BYE
Exemplo de saída esperada no Telnet:

less
2024-10-13 15:45:30
42
Servidor em execução há 35 segundos
Servidor TCP v1.0
Conexão encerrada

5. Logs no servidor:

A cada 10 segundos, os logs dos comandos executados serão exibidos no terminal do servidor, como mostrado abaixo:

csharp
[2024-10-13 15:45:30] Cliente 127.0.0.1: Comando = DATETIME, Resposta = "2024-10-13 15:45:30"
[2024-10-13 15:45:31] Cliente 127.0.0.1: Comando = RNDNUMBER, Resposta = "42"
[2024-10-13 15:45:33] Cliente 127.0.0.1: Comando = UPTIME, Resposta = "Servidor em execução há 35 segundos"
[2024-10-13 15:45:34] Cliente 127.0.0.1: Comando = INFO, Resposta = "Servidor TCP v1.0"

Observações
O servidor aceita múltiplos clientes simultâneos.
O comando BYE encerra a sessão Telnet e a conexão com o servidor.
O monitor utiliza mutexes para garantir que os logs sejam gravados de forma segura entre múltiplas threads.

Estrutura do Código
servidor.c: Contém toda a lógica do servidor TCP, incluindo a manipulação de sockets, processamento de comandos, criação de logs e threads.
Logs: São gerenciados em memória e impressos a cada 10 segundos.
