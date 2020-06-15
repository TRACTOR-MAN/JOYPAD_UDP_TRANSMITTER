#ifndef JOYPAD_IF_H
#define JOYPAD_IF_H

/**************************************************************************
                 ___  _____  ___  _   _   ___  _____  ___ 
                / __||_   _|| _ \| | | | / __||_   _|/ __|
                \__ \  | |  |   /| |_| || (__   | |  \__ \
                |___/  |_|  |_|_\ \___/  \___|  |_|  |___/
**************************************************************************/
// Virtual joystic struct
struct virjoy_st
{
   //buttons
   unsigned char VIRJOY_BTN_A         :1;
   unsigned char VIRJOY_BTN_B         :1;
   unsigned char VIRJOY_BTN_X         :1;
   unsigned char VIRJOY_BTN_Y         :1;
   unsigned char VIRJOY_BTN_TL        :1;
   unsigned char VIRJOY_BTN_TR        :1;
   unsigned char VIRJOY_BTN_THUMBL    :1;
   unsigned char VIRJOY_BTN_THUMBR    :1;
   
   unsigned char VIRJOY_BTN_DPAD_UP       :1;
   unsigned char VIRJOY_BTN_DPAD_DOWN     :1;
   unsigned char VIRJOY_BTN_DPAD_LEFT     :1;
   unsigned char VIRJOY_BTN_DPAD_RIGHT    :1;
   unsigned char VIRJOY_BTN_START         :1;
   unsigned char VIRJOY_BTN_SELECT        :1;
   unsigned char VIRJOY_BTN_MODE          :1;
   unsigned char VIRJOY_PADDING_1         :1;

   //analogue (signed)
   short VIRJOY_ABS_X;
   short VIRJOY_ABS_Y;
   short VIRJOY_ABS_RX;
   short VIRJOY_ABS_RY;
   short VIRJOY_ABS_LT;
   short VIRJOY_ABS_RT;
   short VIRJOY_ABS_HAT0X;
   short VIRJOY_ABS_HAT0Y;


   unsigned char VIRJOY_CHECKSUM;
   
};

// Virtual joystick union
union virjoy_un
{
    virjoy_st virtualjoydata;
    unsigned char raw[sizeof(virjoy_st)];
};

/**************************************************************************
                  ___  _       _    ___  ___  ___  ___ 
                 / __|| |     /_\  / __|/ __|| __|/ __|
                | (__ | |__  / _ \ \__ \\__ \| _| \__ \
                 \___||____|/_/ \_\|___/|___/|___||___/
**************************************************************************/
/*! 
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      15/06/2020
 * 
 *  \par       Description:
 *             This class contains the interface to the linux kernel joypad module
 */
class joypad_interface_c
{
public:
    joypad_interface_c( );
    ~joypad_interface_c( ){ };
};



#endif