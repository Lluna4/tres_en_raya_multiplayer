#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <Ws2tcpip.h>
#include <format>

const std::string SERVER_IP = "192.168.56.1";
std::vector<SOCKET> CLIENTS;
std::vector<char *> USERNAMES;
int NUM = 0;

class jugador
{
    public:
        jugador () {}

        jugador(SOCKET socket, char *username)
            :socket_(socket), username_(username)
        {}

        SOCKET get_socket()
        {
            return socket_;
        }

        char *get_username()
        {
            return username_;
        }
    private:
        SOCKET socket_;
        char *username_;
};

int	ft_isalpha(int a)

{
	if (a >= 65 && a <= 90)
	{
		return (1);
	}
	if (a >= 97 && a <= 122)
	{
		return (1);
	}
	return (0);
}


void partida(jugador p1, jugador p2)
{
    std::cout << "Partida creada " << p1.get_username() << " vs " << p2.get_username() << std::endl;
    char *msg1 = (char *)std::format("La partida ya ha comenzado con {}!", p2.get_username()).c_str();
    char *msg2 = (char *)std::format("La partida ya ha comenzado con {}!", p1.get_username()).c_str();
    send(p1.get_socket(), msg1, strlen(msg1), 0);
    send(p2.get_socket(), msg2, strlen(msg2), 0);
}

void cliente(SOCKET client_sock)
{
    int index = 0;
    sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    getpeername(client_sock, (sockaddr*)&client_addr, &client_addr_len);

    char client_ip[50];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, 50);

    std::cout << "\nCliente conectado en " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

    char *username = (char *)calloc(50, sizeof(char));
    recv(client_sock, username, 50, 0);
    if (CLIENTS.size() % 2 != 0)
    {
        send(client_sock, "Se esta encontrando partida...\n", 30, 0);
    }
    if (CLIENTS.size() % 2 == 0)
    {
        NUM += 2;
        std::cout << CLIENTS[NUM-2] << USERNAMES.back() << std::endl;
        std::cout << CLIENTS[NUM-1] << username << std::endl;
        partida(jugador(CLIENTS[NUM-2], USERNAMES.back()), jugador(CLIENTS[NUM-1], username));
        return ;
    }
    else
    {
        USERNAMES.push_back(username);
    }
}

int main()
{
    WSADATA wsa;
    SOCKET listen_sock, client_sock;
    struct sockaddr_in server;
    int recv_size, client_size;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }
    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());
    server.sin_port = htons(5050);
    if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }
    printf("Escuchando conexiones en 192.168.56.1:5050");
    while (true)
    {
        listen(listen_sock, SOMAXCONN);
        client_size = sizeof(struct sockaddr_in);
        client_sock = accept(listen_sock, (struct sockaddr *)&server, &client_size);
        CLIENTS.push_back(client_sock);
        std::thread cliente_th(cliente, client_sock);
        cliente_th.detach();
    }

    WSACleanup();
    return 0;
}