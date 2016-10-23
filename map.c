#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

struct portinfo
{
	int port;
	char msg[256];
};

struct sockaddr_in get_serv_addr(const char * ip, int portno)
	{
	struct sockaddr_in serv_addr;
	struct hostent * server;
	server = gethostbyname(ip);
	if (!server) {
		printf("ERROR no such host\n");
		exit(1);
	}
	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr, server->h_length);
	return serv_addr;
}

int open_socket()
{
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("ERROR opening socket\n");
		exit(1);
	}
	return sockfd;
}
void scan(char * ip)
{
	struct sockaddr_in serv_addr;
	serv_addr = get_serv_addr(ip, 0);
	int closed_ports = 0, open_ports = 0;	
	struct portinfo op[1000];
	printf("STARTING SCAN\n");
	for (int i = 0; i < 1000; i++) {
		printf("\rScanning port: %d", i);
		int sockfd = open_socket();
		serv_addr.sin_port = htons(i);
		if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr)) < 0) {
			closed_ports++;
		} else {
			op[open_ports].port = i;	
			bzero(op[open_ports].msg, 256);
			read(sockfd, op[open_ports].msg, 255);
			open_ports++;
		}
		close(sockfd);
	}
	printf("\nENDING SCAN\n");
	printf("%d closed ports\n", closed_ports);
	printf("PORT INFORMATION:\n");
	for (int i = 0; i < open_ports; i++) {
		printf("port: %d open\n", op[i].port);
		printf("%s\n", op[i].msg);
	}
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("format: %s address\n", argv[0]);
		return 1;
	}
	scan(argv[1]);
	return 0;
}
