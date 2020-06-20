/**************************************************************************
             ___  _  _   ___  _    _   _  ___   ___  ___ 
            |_ _|| \| | / __|| |  | | | ||   \ | __|/ __|
             | | | .` || (__ | |__| |_| || |) || _| \__ \
            |___||_|\_| \___||____|\___/ |___/ |___||___/
**************************************************************************/
#include "iostream"
#include "joypad_if.h"
#include "udp_client.h"
#include <thread>

extern "C"
{
#include <signal.h>
}

/**************************************************************************
                 _     ___    ___    _    _     ___ 
                | |   / _ \  / __|  /_\  | |   / __|
                | |__| (_) || (__  / _ \ | |__ \__ \
                |____|\___/  \___|/_/ \_\|____||___/
**************************************************************************/
static udp_client_c *udp_client = nullptr;
static joypad_interface_c *joypad_interface = nullptr;
static bool keep_running_b = true;

/**************************************************************************
         ___  ___   ___  _____  ___  _____ __   __ ___  ___  ___ 
        | _ \| _ \ / _ \|_   _|/ _ \|_   _|\ \ / /| _ \| __|/ __|
        |  _/|   /| (_) | | | | (_) | | |   \ V / |  _/| _| \__ \
        |_|  |_|_\ \___/  |_|  \___/  |_|    |_|  |_|  |___||___/
**************************************************************************/
/* UDP processing thread */
static void         UDP_thread_20_ms                ( void );
/* Joypad proccessing thread */
static void         joypad_proc_thread              ( void );
/* exception handler */
static void         int_handler                     ( int s );

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
 *             main entry function
 */
int main ( int argc, char * argv[] )
{
    // Ctrl-C handler setup code
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = &int_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    if( argc == 3 )
    {
        // new the object pointers
        joypad_interface = new joypad_interface_c;
        udp_client = new udp_client_c( argv[1], argv[2] );

        if( joypad_interface->exit_program_b == false)
        {
            if( udp_client->exit_prog_b == false )
            {
                std::thread *joypad = new std::thread( &joypad_proc_thread );
                std::thread *udp = new std::thread( &UDP_thread_20_ms );

                while( keep_running_b != false )
                {
                    // Infinity loop

                    // sleep for 10ms
                    usleep(10000);
                }

                // Allow the threads to complete
                joypad_interface->exit_while_loops( );

                // Wait for the threads to finish
                joypad->join( );
                udp->join( );

                // Destroy the threads
                delete udp;
                delete joypad;
                joypad = nullptr;
                udp = nullptr;
            }
        }
        // delete the object pointers
        delete udp_client;
        delete joypad_interface;
        udp_client = nullptr;
        joypad_interface = nullptr;
    }
    else
    {
        std::cout << "ABORTED pass in the server IP address and port as command line args";
    }

    return EXIT_SUCCESS;
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      19/06/2020
 *
 *  \par       Description:
 *             20ms thread for sending UDP data
 */
static void UDP_thread_20_ms ( void )
{
    while( keep_running_b )
    {
        udp_client->send_udp_data( joypad_interface->virjoy_u.raw, sizeof(virjoy_st) );

        joypad_interface->joystick_event_b = false;

        // sleep for 20ms
        usleep(20000);
    }
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      19/06/2020
 *
 *  \par       Description:
 *             main thread for processing joypad data
 */
static void joypad_proc_thread ( void )
{
    joypad_interface->poll_joypad_data( );
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      19/06/2020
 *
 *  \par       Description:
 *             Ctrl C exception handler
 */
static void int_handler( int s )
{
    keep_running_b = false;
}
