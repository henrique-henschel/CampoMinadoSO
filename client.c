/* PROJETO CAMPO MINADO DE DOIS JOGADORES
   MAURILIO - SISTEMAS OPERACIONAIS (SO)
   DATA DE INICIO: 30/10/2024
*/

/* INCLUSAO DOS ARQUIVOS HEADER (BIBLIOTECAS): */

#include <winsock2.h> // Para a comunicacao com os sockets
#include <stdio.h> // Para funcoes de I/O padrao
#include <stdlib.h> // Para funcoes de alocacao de memoria, entre outras
#include <time.h> // Para tempo e utilizacao do relogio do dispositivo
#include <windows.h> // Para funcoes especificas do Windows
#include <ctype.h> // Para algumas funcoes de manipulacao de caracteres
#include <string.h> // Para manipulacao de strings

/* DECLARACAO DAS FUNCOES DO PROGRAMA: */

void set_color(int text_color, int bg_color) { // Esta funcao doidona aqui que permite mudar as cores da interface

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)(bg_color << 4) | text_color);
}

void printar_placar(void) {
	
	set_color(7, 0); // Cor do texto padrao (branco)
	printf("\n CLIENTE: ");
	if (pontuacao_cliente > 0) { // Se a pontuacao for positiva, printa em azul
		set_color(3, 0);
		printf("%d", pontuacao_cliente);
	} else if (pontuacao_cliente == 0) { // Se a pontuacao for zero, printa em laranja
		set_color(6, 0);
		printf("%d", pontuacao_cliente);
	} else { // Se a pontuacao for negativa, printa em vermelho
		set_color(12, 0);
		printf("%d", pontuacao_cliente);
	}
	set_color(7, 0); // Cor padrao
	printf("  X  SERVIDOR: "); // Espacos, sinal de versus e pontuacao do servidor a seguir
	if (pontuacao_servidor > 0) { // Se a pontuacao for positiva, printa em azul
		set_color(3, 0);
		printf("%d", pontuacao_servidor);
	} else if (pontuacao_servidor == 0) { // Se a pontuacao for zero, printa em laranja
		set_color(6, 0);
		printf("%d", pontuacao_servidor);
	} else { // Se a pontuacao for negativa, printa em vermelho
		set_color(12, 0);
		printf("%d", pontuacao_servidor);
	}
	set_color(7, 0); // Retorna a cor padrao por seguranca	
}

void printar_tela(void) {
	
	system("cls"); // Limpa a tela
	printar_placar();
	
    int x, y; // Variaveis para iterar sobre a matriz (tabuleiro) para imprimi-lo na tela
    
    set_color(9, 0); // Seta a cor para azul mais escuro
    printf("\n\n       1   2   3   4   5   6   7   8   9   10\n"); // Imprime os numeros de cada coluna nessa cor
    set_color(3, 0); // Seta a cor para azul mais claro
    printf("     -----------------------------------------\n"); // Imprime a primeira divisoria horizonal nessa cor

    for (x = 0; x < QTD_LINHAS; x++) { // Loop para imprimir cada linha da esquerda para a direita
    	set_color(9, 0); // Seta a cor para azul mais escuro
        printf("  %c", ('A' + x)); // Imprime as letras de cada linha nessa cor
        set_color(3, 0); // Seta a cor de volta para azul mais claro
        printf("  |"); // Imprime a primeira divisoria vertical nessa cor

        for (y = 0; y < QTD_COLUNAS; y++) { // Loop para iterar sobre as colunas
        	if (!tabuleiro[x][y].ja_clicado) { // Se a casa nao foi jogada ainda, simplesmente imprime um espaco em branco e a proxima divisoria
				printf("   |");
			} else { // Se a casa ja foi jogada
				if (!tabuleiro[x][y].tem_bomba) { // E se nao tinha bomba nela
					set_color(10, 0); // Imprime em verde
					printf(" X "); // Imprime um 'X' e os espacos, para indicar que a casa ja foi jogada e nao tinha mina nela
					
				} else { // E se tinha bomba nela, ou seja, algum jogador ja se explodiu ali anteriormente
					set_color(12, 0);
					printf(" * "); // Imprime um '*' e os espacos, para indicar que a casa ja foi jogada e tinha mina nela
				}
				set_color(3, 0); // Volta para o azul claro
				printf("|"); // Imprime a proxima divisoria vertical
			}
		}
		
        printf("\n     -----------------------------------------\n"); // Imprime a proxima divisoria horizontal
    }
    
    set_color(11, 0); // Seta a cor para ciano claro
    printf("\n--> Digite a coordenada desejada: "); // Solicita a jogada do usuario
    set_color(7, 0); // Seta a cor de volta para o padrao
}

int validar_jogada(char *entrada) {
	int tam = strlen(entrada);
	
	if (tam < 2 || tam > 3) {
		return 0;
	}
	
	char letra;
	int numero;
	
	if (isalpha(entrada[0]) && isdigit(entrada[1])) {
		letra = toupper(entrada[0]);
		numero = atoi(&entrada[1]);
	} else if (isdigit(entrada[0]) && isalpha(entrada[1])) {
		letra = toupper(entrada[1]);
		numero = atoi(&entrada[0]); 
	} else {
		return 0;
	}
	
	if (letra < 'A' || letra > 'J' || numero < 1 || numero > 10) {
		return 0;
	}
	
	if (tabuleiro[(int)letra -64][numero].ja_clicado) {
		return 0;
	}
	
	return 1;
}

int main(void) {
	
	/*WSADATA winsocketsDados;
    if (WSAStartup(MAKEWORD(2, 2), &winsocketsDados) != 0) {
        printf("Falha ao inicializar o Winsock\n");
        return 1;
    } else {
        printf("WSAStartup carregado com sucesso\n");
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        printf("Erro ao criar o socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    } else {
        printf("Socket criado com sucesso\n");
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(51171);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Erro ao conectar ao servidor: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    } else {
        printf("Conectado ao servidor\n");
    }

    char sendBuffer[512];
    char recvBuffer[512];
    int bytesReceived;

    while (1) {
        printf("Digite a mensagem para enviar ao servidor: ");
        fgets(sendBuffer, sizeof(sendBuffer), stdin);
        sendBuffer[strcspn(sendBuffer, "\n")] = 0;

        int bytesSent = send(clientSocket, sendBuffer, strlen(sendBuffer), 0);
        if (bytesSent == SOCKET_ERROR) {
            printf("Erro ao enviar dados: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
        if (strcmp(sendBuffer, "exit") == 0) {
            printf("Encerrando a conexão com o servidor\n");
            break;
        }

        bytesReceived = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (bytesReceived == SOCKET_ERROR) {
            printf("Erro ao receber dados: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        } else if (bytesReceived == 0) {
            printf("Conexão fechada pelo servidor\n");
            break;
        } else {
            recvBuffer[bytesReceived] = '\0';
            printf("Recebido: %s\n", recvBuffer);
            if (strcmp(recvBuffer, "exit") == 0) {
                printf("Servidor pediu para fechar a conexão\n");
                break;
            }
        }
    }

    closesocket(clientSocket);
    WSACleanup();*/
    
    return 0;
}
