#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

extern "C"
{
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

typedef struct sockaddr_in sockaddr_in_st;
typedef struct sockaddr sockaddr_st;
}

/**************************************************************************
                  ___  _       _    ___  ___  ___  ___ 
                 / __|| |     /_\  / __|/ __|| __|/ __|
                | (__ | |__  / _ \ \__ \\__ \| _| \__ \
                 \___||____|/_/ \_\|___/|___/|___||___/
**************************************************************************/
/*! 
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      19/06/2020
 * 
 *  \par       Description:
 *             This class implements a full UDP client
 */
class udp_client_c
{
public:
    udp_client_c ( const char * serveraddr, const char * port );
    ~udp_client_c ( ){ };

    // Member function for sending UDP data
    void send_udp_data( uint8_t * const data, const uint8_t data_length );
    // Member function for exiting an active while loop
    void terminate_loop( void );
    // Variable to note when to exit a thread
    bool exit_prog_b;


protected:
    sockaddr_in_st servaddr;
    int sockfd;
    char buffer[1024];
};

#endif