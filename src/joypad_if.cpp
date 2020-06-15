/**************************************************************************
             ___  _  _   ___  _    _   _  ___   ___  ___ 
            |_ _|| \| | / __|| |  | | | ||   \ | __|/ __|
             | | | .` || (__ | |__| |_| || |) || _| \__ \
            |___||_|\_| \___||____|\___/ |___/ |___||___/
**************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "joypad_if.h"

/**************************************************************************
                        _    _     ___   ___  
                       /_\  | |   / __| / _ \ 
                      / _ \ | |__| (_ || (_) |
                     /_/ \_\|____|\___| \___/
**************************************************************************/
/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      15/06/2020
 *
 *  \par       Description:
 *             Constructor for the joypad_interface_c class
 */
joypad_interface_c::joypad_interface_c( ) :
    device( "/dev/input/js0" ),
    file( fopen(device, "rb+") )
{
    // read away stuff at the begining
	//void *tmp = malloc(8*12*sizeof(char));
	//fread(tmp, sizeof(char)*8*12, 1, file);
	//free(tmp);

    while(1) 
    {
        unsigned char packet[8];
        unsigned char key[4];	

		fread( &packet, sizeof(char) * 8, 1, file );
        key[0] = packet[4];
		key[1] = packet[5];
		key[2] = packet[6];
		key[3] = packet[7];

        std::cout << "Key 0 = " << key[0] << std::endl;
        std::cout << "Key 1 = " << key[1] << std::endl;
        std::cout << "Key 2 = " << key[2] << std::endl;
        std::cout << "Key 3 = " << key[3] << std::endl;
                  
    }
}