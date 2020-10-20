#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <malloc.h>

#include "configuration.h"

/**
 * This function will take care of replacing the first ocurrence of a character, it is used to remove
 * the newline character from getLine
 * @param string A string which to replace the character in
 * @param find The character that will be replaced
 * @param replace The new character that will replace the other one
 */
void replaceFirstCharacter(char * string, char find, char replace){
    while (*(string++)){
        if (*string == find) *string = replace;
    }
}

int main() {

    //<editor-fold defaultstate=collapsed desc="Configuration">
    //Declare a new configuration
    struct configuration _configuration;
    configure(&_configuration);//Overwrite it with a configuration

    if (!_configuration.isValid) {
        printf("Configuration invalid");
        return 1;
    }

    //</editor-fold>

    // <editor-fold defaultstate=collapsed desc="Socket preparation">
    //First I need a handler for the socket I'll use to communicate with the server
    int socket_handler, valread;

    //Now the address to the server
    struct sockaddr_in serv_addr;

    //A buffer for the communication
    char buffer[256];memset(buffer, 0, 256);//Create and clear the buffer

    //First I need to create the socket in the handler
    if((socket_handler = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Socket creation error");
        return 1;
    }

    //I need to clear some values I don't need
    memset(&serv_addr, 0, sizeof(serv_addr));

    //Time to add the values I do need, first the protocol: ipv4
    serv_addr.sin_family = AF_INET;
    //Now the port
    serv_addr.sin_port = htons(_configuration.port);

    //Now I convert the address into it's binary network form
    if(inet_pton(AF_INET, _configuration.serverAddress, &serv_addr.sin_addr) <= 0){
        perror("Address invalid or not supported");
        return 1;
    }

    //Now I try connecting to the server
    if(connect(socket_handler, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0){
        perror("Couldn't connect to server");
        return 1;
    }
    // </editor-fold>

    //If the client is on auto or manual mode
    if (_configuration.clientMode){//1 for automatic mode
        printf("Automatic Mode not implemented yet\n");
    } else{
        FILE * inputFile = fopen("processes.txt", "r");
        char * line = NULL;
        size_t len = 256;

        //While there is file to read, keep reading
        while ((valread = getline(&line, &len, inputFile)) != -1){
            //Got the line, I'll copy it into the buffer
            strncpy(buffer, line, valread);//remember it includes the line separator

            replaceFirstCharacter(buffer, '\n', 0);

            //Try to send it to the server
            if ((valread = send(socket_handler, buffer, valread, 0)) < 0){
                perror("Couldn't write data to server");
                free(line);
                close(socket_handler);
                return 1;
            }
            printf("Process: %s", buffer);
            //Clear the buffer before reading again
            memset(buffer, 0, 256);
            //Now I'll output the Pid the server assigned to it
            if ((valread = recv(socket_handler, buffer, 256, 0)) < 0){
                perror("Couldn't receive back the pid assigned by the server");
                free(line);
                close(socket_handler);
                return 1;
            }
            printf(".\t Pid assigned by server: %s\n", buffer);
            memset(buffer, 0, 256);
        }
        //I need to free this, as it was allocated by the getLine call
        free(line);
    }
    close(socket_handler);

    return 0;
}
