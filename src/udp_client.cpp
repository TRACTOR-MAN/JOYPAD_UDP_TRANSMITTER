/**************************************************************************
             ___  _  _   ___  _    _   _  ___   ___  ___ 
            |_ _|| \| | / __|| |  | | | ||   \ | __|/ __|
             | | | .` || (__ | |__| |_| || |) || _| \__ \
            |___||_|\_| \___||____|\___/ |___/ |___||___/
**************************************************************************/
#include <iostream>
#include <sstream>
#include <string.h>
#include "udp_client.h"

extern "C"
{

}

/**************************************************************************
                        _    _     ___   ___  
                       /_\  | |   / __| / _ \ 
                      / _ \ | |__| (_ || (_) |
                     /_/ \_\|____|\___| \___/
**************************************************************************/
/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      19/06/2020
 *
 *  \par       Description:
 *             Constructor for the udp_client_c class
 */
udp_client_c::udp_client_c ( const char * serveraddr, const char * port ) :
    exit_prog_b( false ),
    servaddr( { } ),
    sockfd( 0 ),
    buffer(  )
{
    // Creating socket file descriptor 
    if ( (sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0 ) 
    { 
        perror("socket creation failed"); 
        exit_prog_b = true; 
    } 

    // If we get here 
    if( exit_prog_b == false )
    {
        // Convert the port from string to int
        std::string port_addr( port );
        std::istringstream string_stream( port_addr );
        int port_addr_int;
        string_stream >> port_addr_int;

        // Set the server address and port
        servaddr.sin_family = AF_INET;  //IPV4
        servaddr.sin_port = htons( (uint16_t)port_addr_int );
        servaddr.sin_addr.s_addr = inet_addr(serveraddr);
        memset(servaddr.sin_zero, '\0', sizeof servaddr.sin_zero);
    }
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      19/06/2020
 *
 *  \par       Description:
 *             Member function for sending UDP data
 */
void udp_client_c::send_udp_data( uint8_t * const data, const uint8_t data_length )
{
    /*Send message to server*/
    sendto( sockfd, data, (size_t)data_length, MSG_CONFIRM, (const sockaddr_st *)&servaddr, sizeof(servaddr));

    /* Receive message from server */
    // (void)recvfrom( sockfd, buffer, 1024, 0, NULL, NULL );

    // printf("Received from server: %s\n", buffer);

}

