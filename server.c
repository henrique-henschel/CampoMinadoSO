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

int gerar_bombas(void) { // Funcao para distribuir as bombas aleatoriamente entre as casas do tabuleiro
	int aleatorio = (rand() % 100) + 1;
	
	if (aleatorio > 60) {
		dados.quantidade_bombas++;
		return 1;
	} else {
		return 0;
	}
}

void inicializar_tabuleiro(void) {	
	int x, y; // Variaveis locais para iteracao nos loops da funcao
	
	dados.quantidade_bombas = 0;
	dados.bombas_acionadas = 0;
	
	for (x = 0; x < QTD_LINHAS; x++) {
		for (y = 0; y < QTD_COLUNAS; y++) {
			dados.tabuleiro[x][y].ja_clicado = 0;// Nenhuma casa ainda foi jogada no inicio
			dados.tabuleiro[x][y].tem_bomba = gerar_bombas(); // Invoca a funcao de geracao aleatoria de bombas
		}
	}
}

void inicializar_placar(void) {	
	dados.placar.numero_jogada = 1;
	dados.placar.pontuacao_cliente = 0;
	dados.placar.pontuacao_servidor = 0;
}

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
	
	// (dados.tabuleiro[linha][coluna].tem_bomba) ? dados.placar.pontuacao_servidor-- : dados.placar.pontuacao_servidor++;
	if (dados.tabuleiro[linha][coluna].tem_bomba) {
		dados.placar.pontuacao_servidor--;
		dados.bombas_acionadas++;
	} else {
		dados.placar.pontuacao_servidor++;
	}
	
	dados.placar.numero_jogada++;
}

int encerrar_jogo(void) {	
	if (dados.placar.numero_jogada <= 100 && abs(dados.placar.pontuacao_servidor - dados.placar.pontuacao_cliente) <= (dados.quantidade_bombas / 2) + 1 && !(dados.bombas_acionadas == dados.quantidade_bombas)) {
		return 0;
	} else {
		return 1;
	}
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
	if (dados.placar.pontuacao_servidor > dados.placar.pontuacao_cliente) {
		// Vitoria do servidor
		printar_vitoria();
	} else if (dados.placar.pontuacao_servidor == dados.placar.pontuacao_cliente) {
		// Empate
		printar_empate();
	} else {
		// Derrota do servidor
		printar_derrota();
	}
}

int main(void) {	
	WSADATA winsocketsDados;
    int temp;

    temp = WSAStartup(MAKEWORD(2, 2), &winsocketsDados);
    if (temp != 0) {
        printf("WSAStartup falhou: %d\n", temp);
        return 1;
    } else {
        printf("WSAStartup carregado com sucesso\n");
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Erro ao criar o socket: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    } else {
        printf("Socket criado com sucesso\n");
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(51171);

    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Erro ao associar o socket: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    } else {
        printf("Bind realizado com sucesso\n");
    }

    if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
        printf("Erro ao colocar o socket em estado de escuta: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    } else {
        printf("Listen realizado com sucesso\n");
    }

    SOCKET clientSocket;
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    clientSocket = accept(sock, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        printf("Erro ao aceitar a conexao: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    } else {
        printf("Conexao aceita com sucesso\n");
    }
    
    srand(time(NULL));
	inicializar_tabuleiro();
	inicializar_placar();
	printar_tela();
	send(clientSocket, (char*)&dados, sizeof(Dados), 0);
	
	Dados recebidos;
    int bytesReceived;
    char entrada[4];
    int linha, coluna;

    while (!encerrar_jogo()) {
        bytesReceived = recv(clientSocket, (char*)&recebidos, sizeof(Dados), 0);
        
        if (bytesReceived == SOCKET_ERROR) {
            printf("Erro ao receber dados: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        } else if (bytesReceived == 0) {
            printf("Conexao fechada pelo cliente\n");
            break;
        } else {
            dados = recebidos;
        }

        do {
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
        
        linha = -1;
		coluna = -1;
    }

    closesocket(clientSocket);
    closesocket(sock);
    WSACleanup();
	
    return 0;
}
