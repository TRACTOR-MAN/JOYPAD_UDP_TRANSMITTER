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
    // Check to see whether the file was successfully opened
    if( file == nullptr )
    {
        std::cout << "Could not access joystick, check it is connected" << std::endl;
    }
    else
    {
        // All is good, file is open
    }
    
    while( read_event( ) != false ) 
    {
        switch (joystick_event_s.type)
        {
            case JS_EVENT_BUTTON:
                std::cout << " Button " << (uint16_t)joystick_event_s.number << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
                break;
            case JS_EVENT_AXIS:
                size_t axis;
                axis = get_axis_state( );
                if (axis < 3)
                    std::cout << " Axis " << axis << " " << axis_state_s[axis].x << " " << axis_state_s[axis].x << std::endl;
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

    if (axis < 3)
    {
        if (joystick_event_s.number % 2 == 0)
            axis_state_s[axis].x = joystick_event_s.value;
        else
            axis_state_s[axis].y = joystick_event_s.value;
    }

    return axis;
}
