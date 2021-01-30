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
#include <unistd.h>
#include <string.h>


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
joypad_interface_c::joypad_interface_c( const char * cfgfile ) :

    exit_program_b( false ),
    joy_mutex( new std::mutex ),
    virjoy_u( { } ),
    joystick_event_s( { } ),
    exit_while_loop_b( false ),
    extended_debug_b( true )
{
    // Initialise the LB and RB data
    joy_mutex->lock( );
    virjoy_u.virtualjoydata.VIRJOY_ABS_RB = -32767;
    virjoy_u.virtualjoydata.VIRJOY_ABS_LB = -32767;
    joy_mutex->unlock( );

    if( parse_buttoncfg(&cfgfile) )
    {
        std::cout << "Failed to parse buttoncfg.cfg file" << std::endl;
        exit_program_b = true;
    }

    file = fopen(device, "rb+");

    // Check to see whether the file was successfully opened
    if( file == nullptr )
    {
        std::cout << "Could not access joystick, check it is connected" << std::endl;
        exit_program_b = true;
    }
}

/*!
 *  \author    7oxicshadow
 *  \version   1.0
 *  \date      20/01/2020
 *
 *  \par       Description:
 *             Parse button config file
 */
bool joypad_interface_c::parse_buttoncfg ( const char ** targetcfgfile )
{
    FILE *fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;
    int var;

    fp = fopen(*targetcfgfile, "r");
    if (fp == NULL)
    {
        std::cout << "Tried to open : " << *targetcfgfile << "\n";
        return true;
    }

    while ((read = getline(&line, &len, fp)) != -1) 
    {
        if ((line[0] == '#') || (line[0] == '\n') || (line[0] == ' '))
            continue;
        else if (line[0] == '!')
            break;
        else
        {
            if (i == 0)
            {
                //std::cout << line;
                //copy line to the device (note: -1 to remove the \n char)
                memcpy ( device, line, strlen(line) - 1 );
                //std::cout << device;
            }
            else
            {
                if (j < NUM_BUTTONS)
                {
                    var = atoi(line);
                    //std::cout << "got : " << var << "\n";
                    buttoncfg[j] = var;
                    j++;
                }
                else if ( k < NUM_ANA_CTL)
                {
                    var = atoi(line);
                    //std::cout << "got : " << var << "\n";
                    anacfg[k] = var;
                    k++;
                }
                else
                {
                    //out of range. do nothing.
                }
            }

            i++;

        }
    }

    fclose(fp);

    return false;
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      16/06/2020
 *
 *  \par       Description:
 *             Member function for reading a joystick event
 */
bool joypad_interface_c::read_event ( void )
{
    ssize_t objects;

    // Read the joystick event data structure from the joystick input file
    objects = fread( &joystick_event_s, sizeof(joystick_event), 1, file );

    if ( objects == 1 )
        return true;
    else
        // Error, could not read full event. 
        return false;
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      16/06/2020
 *
 *  \par       Description:
 *             Member function for returning the number of joystick axes
 */
size_t joypad_interface_c::get_axis_count ( void )
{
    __u8 axes;

    if (ioctl( fileno( file ), JSIOCGAXES, &axes) == -1)
        return 0;

    return axes;
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      16/06/2020
 *
 *  \par       Description:
 *             Member function for returning the number of buttons on the controller
 */
size_t joypad_interface_c::get_button_count ( void )
{
    __u8 buttons;

    if (ioctl( fileno( file ), JSIOCGBUTTONS, &buttons) == -1)
        return 0;

    return buttons;
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      16/06/2020
 *
 *  \par       Description:
 *             Member function for returning the axis state of the joystick axes
 */
size_t joypad_interface_c::get_axis_state ( void )
{
    size_t axis = joystick_event_s.number / 2;

    return axis;
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      18/06/2020
 *
 *  \par       Description:
 *             Member function for returning the axis state of the joystick dpad
 */
void joypad_interface_c::get_dpad_state ( void )
{
    if( joystick_event_s.number == anacfg[ANA_DPAD_Y] )
    {
        joy_mutex->lock( );
        virjoy_u.virtualjoydata.VIRJOY_ABS_HAT0Y = joystick_event_s.value;
        
        if( joystick_event_s.value <= -16384 )
        {
            if( extended_debug_b )
                std::cout << "DPAD UP" << std::endl;
            virjoy_u.virtualjoydata.VIRJOY_BTN_DPAD_UP = true;
        }
        else 
        if( joystick_event_s.value >= 16384 )
        {
            if( extended_debug_b )
                std::cout << "DPAD DOWN" << std::endl;
            virjoy_u.virtualjoydata.VIRJOY_BTN_DPAD_DOWN = true;
        }
        else
        {
            if( extended_debug_b )
                std::cout << "DPAD VERTI not pressed" << std::endl;
            virjoy_u.virtualjoydata.VIRJOY_BTN_DPAD_UP = false;
            virjoy_u.virtualjoydata.VIRJOY_BTN_DPAD_DOWN = false;
        }
        joy_mutex->unlock( );
    }
    else
    if( joystick_event_s.number == anacfg[ANA_DPAD_X] )
    {
        joy_mutex->lock( );
        virjoy_u.virtualjoydata.VIRJOY_ABS_HAT0X = joystick_event_s.value;
        
        if( joystick_event_s.value <= -16384 )
        {
            if( extended_debug_b )
                std::cout << "DPAD_LEFT" << std::endl;
            virjoy_u.virtualjoydata.VIRJOY_BTN_DPAD_LEFT = true;
        }
        else 
        if( joystick_event_s.value >= 16384 )
        {
            if( extended_debug_b )
                std::cout << "DPAD_RIGHT" << std::endl;
            virjoy_u.virtualjoydata.VIRJOY_BTN_DPAD_RIGHT = true;
        }
        else
        {
            if( extended_debug_b )
                std::cout << "DPAD HORIZ not pressed" << std::endl;
            virjoy_u.virtualjoydata.VIRJOY_BTN_DPAD_LEFT = false;
            virjoy_u.virtualjoydata.VIRJOY_BTN_DPAD_RIGHT = false;
        }
        joy_mutex->unlock( );
    }
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      18/06/2020
 *
 *  \par       Description:
 *             Member function for returning the axis state of the joystick dpad
 */
void joypad_interface_c::map_digital_button ( void )
{
    if(joystick_event_s.number == buttoncfg[BUTTON_A])
    {
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_A" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        joy_mutex->lock( );
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_A = true : virjoy_u.virtualjoydata.VIRJOY_BTN_A = false);
        joy_mutex->unlock( );
    }
    else if(joystick_event_s.number == buttoncfg[BUTTON_B])
    {
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_B" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        joy_mutex->lock( );
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_B = true : virjoy_u.virtualjoydata.VIRJOY_BTN_B = false);
        joy_mutex->unlock( );
    }
    else if(joystick_event_s.number == buttoncfg[BUTTON_Y])
    {
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_Y" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        joy_mutex->lock( );
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_Y = true : virjoy_u.virtualjoydata.VIRJOY_BTN_Y = false);
        joy_mutex->unlock( );
    }
    else if(joystick_event_s.number == buttoncfg[BUTTON_X])
    {
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_X" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        joy_mutex->lock( );
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_X = true : virjoy_u.virtualjoydata.VIRJOY_BTN_X = false);
        joy_mutex->unlock( );
    }
    else if(joystick_event_s.number == buttoncfg[BUTTON_TL])
    {
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_TL" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        joy_mutex->lock( );
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_TL = true : virjoy_u.virtualjoydata.VIRJOY_BTN_TL = false);
        joy_mutex->unlock( );
    } 
    else if(joystick_event_s.number == buttoncfg[BUTTON_TR])
    {
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_TR" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        joy_mutex->lock( );
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_TR = true : virjoy_u.virtualjoydata.VIRJOY_BTN_TR = false);
        joy_mutex->unlock( );
    }
    else if(joystick_event_s.number == buttoncfg[BUTTON_BL])
    {    
        // LB pressed, this is analog
    }
    else if(joystick_event_s.number == buttoncfg[BUTTON_BR])
    {
        // LB pressed, this is analog
    }
    else if(joystick_event_s.number == buttoncfg[BUTTON_SELECT])
    {
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_SELECT" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        joy_mutex->lock( );
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_SELECT = true : virjoy_u.virtualjoydata.VIRJOY_BTN_SELECT = false);
        joy_mutex->unlock( );
    }
    else if(joystick_event_s.number == buttoncfg[BUTTON_START])
    {    
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_START" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        joy_mutex->lock( );
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_START = true : virjoy_u.virtualjoydata.VIRJOY_BTN_START = false);
        joy_mutex->unlock( );
    }
    else if(joystick_event_s.number == buttoncfg[BUTTON_MODE])
    {
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_MODE" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        joy_mutex->lock( );
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_MODE = true : virjoy_u.virtualjoydata.VIRJOY_BTN_MODE = false);
        joy_mutex->unlock( );
    }
    else if(joystick_event_s.number == buttoncfg[BUTTON_THUMBL])
    {
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_THUMBL" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        joy_mutex->lock( );
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_THUMBL = true : virjoy_u.virtualjoydata.VIRJOY_BTN_THUMBL = false);
        joy_mutex->unlock( );
    }
    else if(joystick_event_s.number == buttoncfg[BUTTON_THUMBR])
    {
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_THUMBR" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        joy_mutex->lock( );
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_THUMBR = true : virjoy_u.virtualjoydata.VIRJOY_BTN_THUMBR = false);
        joy_mutex->unlock( );
    }
    else
    {
        //do nothing
    }

}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      18/06/2020
 *
 *  \par       Description:
 *             Member function for mapping the analog joystick values to the data structure
 */
void joypad_interface_c::map_analog_joystick ( void )
{
    if ( joystick_event_s.number == anacfg[ANA_LSTICK_X] )
    {
        joy_mutex->lock( );
        virjoy_u.virtualjoydata.VIRJOY_ABS_LX = joystick_event_s.value;
        joy_mutex->unlock( );

        if( extended_debug_b )
        {
            joy_mutex->lock( );
            std::cout << " VIRJOY_ABS_LX" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_LX << std::endl;
            joy_mutex->unlock( );
        }
    }
    else if ( joystick_event_s.number == anacfg[ANA_LT] )
    {
        joy_mutex->lock( );
        virjoy_u.virtualjoydata.VIRJOY_ABS_LB = joystick_event_s.value;
        joy_mutex->unlock( );

        if( extended_debug_b )
        {
            joy_mutex->lock( );
            std::cout << " VIRJOY_ABS_LB" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_LB << std::endl;
            joy_mutex->unlock( );
        }
    }
    else if ( joystick_event_s.number == anacfg[ANA_RSTICK_X] )
    {
        joy_mutex->lock( );
        virjoy_u.virtualjoydata.VIRJOY_ABS_RX = joystick_event_s.value;
        joy_mutex->unlock( );

        if( extended_debug_b )
        {
            joy_mutex->lock( );
            std::cout << " VIRJOY_ABS_RX" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_RX << std::endl;
            joy_mutex->unlock( );
        }
    }
    else if ( joystick_event_s.number == anacfg[ANA_LSTICK_Y] )
    {
        joy_mutex->lock( );
        virjoy_u.virtualjoydata.VIRJOY_ABS_LY = joystick_event_s.value;
        joy_mutex->unlock( );

        if( extended_debug_b )
        {
            joy_mutex->lock( );
            std::cout << " VIRJOY_ABS_LY" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_LY << std::endl;
            joy_mutex->unlock( );
        }
    }
    else if ( joystick_event_s.number == anacfg[ANA_RT] )
    {
        joy_mutex->lock( );
        virjoy_u.virtualjoydata.VIRJOY_ABS_RB = joystick_event_s.value;
        joy_mutex->unlock( );

        if( extended_debug_b )
        {
            joy_mutex->lock( );
            std::cout << " VIRJOY_ABS_RB" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_RB << std::endl;
            joy_mutex->unlock( );
        }
    }
    else if ( joystick_event_s.number == anacfg[ANA_RSTICK_Y] )
    {
        joy_mutex->lock( );
        virjoy_u.virtualjoydata.VIRJOY_ABS_RY = joystick_event_s.value;
        joy_mutex->unlock( );

        if( extended_debug_b )
        {
            joy_mutex->lock( );
            std::cout << " VIRJOY_ABS_RY" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_RY << std::endl;
            joy_mutex->unlock( );
        }
    }
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      19/06/2020
 *
 *  \par       Description:
 *             Function for polling joypad data
 */
void joypad_interface_c::poll_joypad_data ( void )
{
    // Sit in a while loop, (blocking mode) and pull out all joypad events
    while( (read_event( ) != false) && (exit_while_loop_b == false) ) 
    {
        switch (joystick_event_s.type)
        {
        case JS_EVENT_BUTTON:
            // Map the button press to the data structure
            map_digital_button( );

            break;
        case JS_EVENT_AXIS:

            map_analog_joystick( );

            get_dpad_state( );


            break;

        default:

            /* Ignore init events. */
            break;
        }
    }
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      18/06/2020
 *
 *  \par       Description:
 *             Member function for exiting any active while loops
 */
void joypad_interface_c::exit_while_loops ( void )
{
    exit_while_loop_b = true;
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      28/06/2020
 *
 *  \par       Description:
 *             Member locking the data mutex
 */
void joypad_interface_c::lock_joypad_mutex ( void )
{
    joy_mutex->lock( );
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      28/06/2020
 *
 *  \par       Description:
 *             Member unlocking the data mutex
 */
void joypad_interface_c::unlock_joypad_mutex ( void )
{
    joy_mutex->unlock( );
}


/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      19/06/2020
 *
 *  \par       Description:
 *             Destructor for the joypad_interface_c class
 */
joypad_interface_c::~joypad_interface_c( ) 
{
    // Close the file stream
    if( file != nullptr )
    {
        fclose( file );
    }

    // Delete the mutex
    delete joy_mutex;
}
