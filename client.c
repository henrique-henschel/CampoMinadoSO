/* PROJETO: CAMPO MINADO DE DOIS JOGADORES
   UNICESUMAR - Engenharia de Software
   TURMA: ESOFT4S-N-A
   DISCIPLINA: Sistemas Operacionais (SO)
   PROFESSOR/ORIENTADOR: Maurilio
   DATA DE INICIO: 30/10/2024
   DATA/PRAZO FINAL: 17/11/2024
*/

/* INCLUSAO DOS ARQUIVOS HEADER (BIBLIOTECAS): */

#include <winsock2.h> // Para utilizar os sockets
#include <stdio.h> // Para funcoes de I/O padrao
#include <stdlib.h> // Para funcoes de alocacao de memoria, entre outras
#include <time.h> // Para tempo e utilizacao do relogio do dispositivo
#include <windows.h> // Para funcoes especificas do Windows
#include <ctype.h> // Para algumas funcoes de manipulacao de caracteres
#include <string.h> // Para manipulacao de strings

#define QTD_LINHAS 10
#define QTD_COLUNAS 10
#define QTD_CASAS (QTD_LINHAS * QTD_COLUNAS)

typedef struct {
    int ja_clicado;
    int tem_bomba;
} Casa;

typedef struct {
	int numero_jogada;
	int pontuacao_servidor;
	int pontuacao_cliente;
} Placar;

typedef struct {
	Casa tabuleiro[QTD_LINHAS][QTD_COLUNAS];
	Placar placar;
	int quantidade_bombas;
	int bombas_acionadas;	
} Dados;

Dados dados;

/* DECLARACAO DAS FUNCOES DO PROGRAMA: */

/*void sendTabuleiro(SOCKET sock, Casa tabuleiro[QTD_LINHAS][QTD_COLUNAS]) {
    // Envia a matriz como um bloco de bytes
    send(sock, (const char *)tabuleiro, sizeof(Casa) * QTD_LINHAS * QTD_COLUNAS, 0);
}

void receiveTabuleiro(SOCKET sock, Casa tabuleiro[QTD_LINHAS][QTD_COLUNAS]) {
    // Recebe os dados
    recv(sock, (char *)tabuleiro, sizeof(Casa) * QTD_LINHAS * QTD_COLUNAS, 0);
}*/

void set_color(int text_color, int bg_color) { // Esta funcao permite mudar as cores da interface
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)(bg_color << 4) | text_color);
}

void printar_placar(void) {	
	set_color(7, 0); // Cor do texto padrao (branco)
	printf("\n CLIENTE: ");
	if (dados.placar.pontuacao_cliente > 0) { // Se a pontuacao for positiva, printa em azul
		set_color(3, 0);
		printf("%d", dados.placar.pontuacao_cliente);
	} else if (dados.placar.pontuacao_cliente == 0) { // Se a pontuacao for zero, printa em laranja
		set_color(6, 0);
		printf("%d", dados.placar.pontuacao_cliente);
	} else { // Se a pontuacao for negativa, printa em vermelho
		set_color(12, 0);
		printf("%d", dados.placar.pontuacao_cliente);
	}
	set_color(7, 0); // Cor padrao
	printf("  X  SERVIDOR: "); // Espacos, sinal de versus e pontuacao do servidor a seguir
	if (dados.placar.pontuacao_servidor > 0) { // Se a pontuacao for positiva, printa em azul
		set_color(3, 0);
		printf("%d", dados.placar.pontuacao_servidor);
	} else if (dados.placar.pontuacao_servidor == 0) { // Se a pontuacao for zero, printa em laranja
		set_color(6, 0);
		printf("%d", dados.placar.pontuacao_servidor);
	} else { // Se a pontuacao for negativa, printa em vermelho
		set_color(12, 0);
		printf("%d", dados.placar.pontuacao_servidor);
	}
	set_color(7, 0);
	printf("         JOGADA: ");
	set_color(9, 0);
	printf("%d", dados.placar.numero_jogada);
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
        	if (!dados.tabuleiro[x][y].ja_clicado) { // Se a casa nao foi jogada ainda, simplesmente imprime um espaco em branco e a proxima divisoria
				printf("   |");
			} else { // Se a casa ja foi jogada
				if (!dados.tabuleiro[x][y].tem_bomba) { // E se nao tinha bomba nela
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

int validar_jogada(char *entrada, int *linha, int *coluna) {
	int tam = strlen(entrada);

	if (tam < 2 || tam > 3) {
		return 0;
	}

	char letra;
	int numero;

	if (tam == 2) {
		if (isalpha(entrada[0]) && isdigit(entrada[1])) {
			letra = toupper(entrada[0]);
			numero = atoi(&entrada[1]);
		} else if (isdigit(entrada[0]) && isalpha(entrada[1])) {
			letra = toupper(entrada[1]);
			numero = atoi(&entrada[0]);
		} else {
			return 0;
		}
	} else if (tam == 3) {
		if (isalpha(entrada[0]) && entrada[1] == '1' && entrada[2] == '0') {
			letra = toupper(entrada[0]);
			numero = 10;
		} else if (entrada[0] == '1' && entrada[1] == '0' && isalpha(entrada[2])) {
			letra = toupper(entrada[2]);
			numero = 10;
		} else {
			return 0;
		}
	}

	if (letra < 'A' || letra > 'J' || numero < 1 || numero > 10) {
		return 0;
	}

	int posicao_linha = (int)(letra - 'A');
	int posicao_coluna = numero - 1;

	if (dados.tabuleiro[posicao_linha][posicao_coluna].ja_clicado) {
		return 0;
	}

	*linha = posicao_linha;
	*coluna = posicao_coluna;

	return 1;
}

void atualizar_dados(int linha, int coluna) {	
	dados.tabuleiro[linha][coluna].ja_clicado = 1;
	
	if (dados.tabuleiro[linha][coluna].tem_bomba) {
		dados.placar.pontuacao_cliente--;
		dados.bombas_acionadas++;
	} else {
		dados.placar.pontuacao_cliente++;
	}
	
	dados.placar.numero_jogada++;
}

void printar_vitoria(void) { // Funcao somente para teste
	set_color(2, 0);
	printf("\n\n              PARABENS!\n");
	set_color(10, 0);
	printf("             VOCE VENCEU\n\n");
	set_color(7, 0);
	printf("             ");
	set_color(14, 0);
    printf("___________\n");
    set_color(7, 0);
    printf("            ");
    set_color(14, 0);
    printf("'._==_==_=_.'\n");
	set_color(7, 0);
	printf("            ");
    set_color(14, 0);
    printf(".-\\       /-.\n");
    set_color(7, 0);
    printf("           ");
    set_color(14, 0);
    printf("| (|  YOU  |) |\n");
    set_color(7, 0);
    printf("            ");
    set_color(14, 0);
    printf("'-|  WIN! |-'\n");
    set_color(7, 0);
    printf("              ");
    set_color(14, 0);
    printf("\\       /\n");
    set_color(7, 0);
   	printf("               ");
    set_color(14, 0);
    printf("'.   .'\n");
    set_color(7, 0);
    printf("                 ");
    set_color(14, 0);
    printf(") (\n");
    set_color(7, 0);
    printf("               ");
    set_color(14, 0);
    printf("_.' '._\n");
    set_color(7, 0);
    printf("              ");
    set_color(14, 0);
    printf("`\"\"\"\"\"\"\"`\n");
    set_color(7, 0);
}

void printar_empate(void) {	
	set_color(15, 0);
	printf("\n\n\n\n	  __________________________\n");
	printf("         |                          |\n");
	printf("         |                          |\n");
	printf("         |     ");
	set_color(15, 1);
	printf("PARTIDA DIFICIL!");
	set_color(15, 0);
	printf("     |\n");
	printf("         |                          |\n");
	printf("         |          ");
	set_color(6, 0);
	printf("EMPATE");
	set_color(15, 0);
	printf("          |\n");
	printf("         |                          |\n");
	printf("         |__________________________|\n\n\n\n");
}

void printar_derrota(void) {	
	set_color(15, 0);
	printf("\n\n\n\n	  __________________________\n");
	printf("         |                          |\n");
	printf("         |                          |\n");
	printf("         |   ");
	set_color(15, 4);
	printf("NAO FOI DESSA VEZ :C");
	set_color(15, 0);
	printf("   |\n");
	printf("         |                          |\n");
	printf("         |       ");
	set_color(12, 0);
	printf("VOCE PERDEU!");
	set_color(15, 0);
	printf("       |\n");
	printf("         |                          |\n");
	printf("         |__________________________|\n\n\n\n");
}

void printar_resultado(void) {	
	if (dados.placar.pontuacao_cliente > dados.placar.pontuacao_servidor) {
		// Vitoria do cliente
		printar_vitoria();
	} else if (dados.placar.pontuacao_cliente == dados.placar.pontuacao_servidor) {
		// Empate
		printar_empate();
	} else {
		// Derrota do cliente
		printar_derrota();
	}
}

int main(void) {
	WSADATA winsocketsDados;
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
    
    Dados recebidos;
	int bytesReceived;
	char entrada[4];
	int linha, coluna;
    
    recv(clientSocket, (char*)&recebidos, sizeof(Dados), 0);
    dados = recebidos; // Importantíssimo para a inicialização no cliente

    while (1) {
        do {
			system("cls");
			fflush(stdin);
			printar_tela();
			scanf("%s", &entrada);
		} while (!validar_jogada(entrada, &linha, &coluna));
		
		atualizar_dados(linha, coluna);
		printar_tela();

        int bytesSent = send(clientSocket, (char*)&dados, sizeof(Dados), 0);
        
        if (bytesSent == SOCKET_ERROR) {
            printf("Erro ao enviar dados: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        bytesReceived = recv(clientSocket, (char*)&recebidos, sizeof(Dados), 0);
        
        if (bytesReceived == SOCKET_ERROR) {
            printf("Erro ao receber dados: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        } else if (bytesReceived == 0) {
            printf("Conexao fechada pelo servidor\n");
            break;
        } else {
            dados = recebidos;
        }
        
        printar_tela();
        
        linha = -1;
        coluna = -1;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
