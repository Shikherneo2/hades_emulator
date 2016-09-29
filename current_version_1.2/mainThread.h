#include <windows.h>
#include <mmsystem.h>
#include <winable.h>

//VERSION 2

BOOL close2 = FALSE;
class joyThread{

private:
    
    BOOL mouseCheck1, mouseCheck2, no, middleButton;
    BOOL mouseCheck3, mouseCheck31, Xbox, ZOK, POV;
    
    char buttonState[16];
    unsigned char keybrdMapCopy[24];
    unsigned char buttons[16];
    unsigned char mouseMapCopy[10]; 
    
    char x, y, z, r, u, sens1;
    
    int xmax, xmin, ymax, ymin, zmax, zmin, rmax, rmin;
    int a, d, glob, a1, joyID, umax, umin;
    int temp, tempX, tempY, tempZ, tempR, tempU, prevClick;
    
    float butX, butY, butnegX, butnegY;
    
    INPUT mousePad, mouseScrollUp, mouseScrollDown; 
    INPUT keyboard, mouseMiddle, mouseClick[4];
    
    JOYINFOEX joy1;
    JOYCAPS joyCaps;

    void mouse( int );
    int getButton( JOYINFOEX );
    
public:
    
    joyThread( UINT currControl1, unsigned char keybrdMap1[24], unsigned char mouseMap1[10], char in, BOOL xbox ){
         
         Xbox = xbox;
         sens1 = in;
         close2 = TRUE, ZOK = FALSE;
         joyID = currControl1;
         mouseCheck1 = FALSE, mouseCheck2 = FALSE,  no = FALSE, middleButton = FALSE;
         mouseCheck3 = FALSE, mouseCheck31 = FALSE;
         
         for( int i=0; i<24; i++)
            keybrdMapCopy[i] = keybrdMap1[i];
    
         for( int i=0; i<10; i++)
            mouseMapCopy[i] = mouseMap1[i];
         
         for(int gf=0; gf<5; gf++)
                buttons[gf] = 0;
         
         for(int gfr=0; gfr<16; gfr++)
                buttonState[gfr] = 0;
         
         x = -1, y = -1, z = -1, r = -1;
         
         a = 0, d = 0, glob = 0, a1 =0;
         tempX = 0, tempY = 0, tempZ = 0, tempR = 0, prevClick = 0;
         
         butX = 0 ,butY = 0, butnegX = 0, butnegY = 0;
         
         joyGetDevCaps( joyID, &joyCaps, sizeof(joyCaps));
          xmax = ( (joyCaps.wXmax+joyCaps.wXmin)/2 ) + 4000;
        xmin = xmax - 8000;
        
        ymax = ( (joyCaps.wYmax+joyCaps.wYmin)/2 ) + 4000;
        ymin = ymax - 8000;
        
        zmax = ( (joyCaps.wZmax+joyCaps.wZmin)/2 ) + 4000;
        zmin = zmax - 8000;
     
        rmax = ( (joyCaps.wRmax+joyCaps.wRmin)/2 ) + 4000;
        rmin = rmax - 8000;
        
        if( joyCaps.wCaps & JOYCAPS_HASPOV)
            POV = TRUE;
        else 
            POV = FALSE;
    }
    
    void mainThread( );
};
    

void joyThread::mainThread(){    
           
    joyGetDevCaps( joyID, &joyCaps, sizeof(joyCaps));    
    
   if( Xbox == TRUE){
        umax = ( (joyCaps.wUmax+joyCaps.wUmin)/2 ) + 4000;
        umin = umax - 8000;
    }
            
    joy1.dwSize = sizeof( joy1 );
    joy1.dwFlags = JOY_RETURNALL;
    
    mousePad.type = INPUT_MOUSE;
    mousePad.mi.mouseData = 0;
    mousePad.mi.time = 0;
    mousePad.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    
    mouseMiddle.type = INPUT_MOUSE;
    mouseMiddle.mi.time = 0;
    mouseMiddle.mi.dwFlags = MOUSEEVENTF_WHEEL;
    mouseMiddle.mi.mouseData = WHEEL_DELTA;
    
    for( int yg = 0;yg<4; yg++){
            mouseClick[yg].type = INPUT_MOUSE;
            mouseClick[yg].mi.mouseData = 0;
            mouseClick[yg].mi.time = 0;
    }
    
    mouseClick[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    mouseClick[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    mouseClick[2].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    mouseClick[3].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    
    mouseScrollUp.type = INPUT_MOUSE;
    mouseScrollUp.mi.time = 0;
    mouseScrollUp.mi.dwFlags = MOUSEEVENTF_WHEEL;
    mouseScrollUp.mi.mouseData = 1;
    
    mouseScrollDown.type = INPUT_MOUSE;
    mouseScrollDown.mi.time = 0;
    mouseScrollDown.mi.dwFlags = MOUSEEVENTF_WHEEL;
    mouseScrollDown.mi.mouseData = -1;    

    keyboard.type = INPUT_KEYBOARD;
    
    while( joyGetPosEx( joyID, & joy1 ) == JOYERR_NOERROR && close2 != FALSE ){
             
                 // CHECK Axis Selection Variable Values
             if( mouseMapCopy[9] == 1 ){
                 if( (tempX != joy1.dwXpos || tempY != joy1.dwYpos) ){
                     mousePad.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
                     mousePad.mi.dx = joy1.dwXpos;
                     mousePad.mi.dy = joy1.dwYpos;
                     SendInput(1, &mousePad, sizeof(mousePad));
                 }
            }   
             else if( mouseMapCopy[9] == 2){
                
                if( Xbox == FALSE ){
                    if( (tempZ != joy1.dwZpos || tempR != joy1.dwRpos) ){
                             mousePad.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
                             mousePad.mi.dx = joy1.dwZpos;
                             mousePad.mi.dy = joy1.dwRpos;
                             SendInput(1, &mousePad, sizeof(mousePad));
                        }
                }
                else{
                    if( (tempU != joy1.dwUpos || tempR != joy1.dwRpos) ){
                         mousePad.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
                         mousePad.mi.dx = joy1.dwUpos;
                         mousePad.mi.dy = joy1.dwRpos;
                         SendInput(1, &mousePad, sizeof(mousePad));
                    }
                }
                
            }
            
             if( Xbox == TRUE ){
                if( joy1.dwZpos > zmax || joy1.dwZpos < zmin )
                    ZOK = TRUE;
            }
    
             
            ///////////////////////////////////////////////
            if( keybrdMapCopy[18]!=0 && joy1.dwXpos > xmax && x == -1){
                    keyboard.ki.wVk = keybrdMapCopy[18];
                    keyboard.ki.dwFlags = 0;
                    SendInput(1, &keyboard, sizeof(keyboard));
                    x = 0;   
            }
            
            else if( keybrdMapCopy[19]!=0 && joy1.dwXpos < xmin && x == -1){
                        keyboard.ki.wVk = keybrdMapCopy[19];
                        keyboard.ki.dwFlags = 0;
                        SendInput(1, &keyboard, sizeof(keyboard));
                        x = 1;
            }
            
            else if( (joy1.dwXpos > xmin && x == 1) || (joy1.dwXpos < xmax && x == 0) ){
                        keyboard.ki.wVk = keybrdMapCopy[18  + x];
                        keyboard.ki.dwFlags = KEYEVENTF_KEYUP;
                        SendInput(1, &keyboard, sizeof(keyboard));
                        x = -1;
            }
                
            if( keybrdMapCopy[17]!=0 && joy1.dwYpos > ymax && y == -1){
                    keyboard.ki.wVk = keybrdMapCopy[17];
                    keyboard.ki.dwFlags = 0;
                    SendInput(1, &keyboard, sizeof(keyboard));
                    y = 1;   
            }
            
            else if( keybrdMapCopy[16]!=0 && joy1.dwYpos < ymin && y == -1){
                        keyboard.ki.wVk = keybrdMapCopy[16];
                        keyboard.ki.dwFlags = 0;
                        SendInput(1, &keyboard, sizeof(keyboard));
                        y = 0;
            }
            
            else if( (joy1.dwYpos > ymin && y == 0) || (joy1.dwYpos < ymax && y == 1) ){
                        keyboard.ki.wVk = keybrdMapCopy[16  + y];
                        keyboard.ki.dwFlags = KEYEVENTF_KEYUP;
                        SendInput(1, &keyboard, sizeof(keyboard));
                        y = -1;
            }
            
            if( Xbox == FALSE ){
                if( keybrdMapCopy[22]!=0 && joy1.dwZpos > zmax && z == -1){
                        keyboard.ki.wVk = keybrdMapCopy[22];
                        keyboard.ki.dwFlags = 0;
                        SendInput(1, &keyboard, sizeof(keyboard));
                        z = 0;   
                }
                
                else if( keybrdMapCopy[23]!=0 && joy1.dwZpos < zmin && z == -1){
                            keyboard.ki.wVk = keybrdMapCopy[23];
                            keyboard.ki.dwFlags = 0;
                            SendInput(1, &keyboard, sizeof(keyboard));
                            z = 1;
                }
                
                else if( (joy1.dwZpos > zmin && z == 1) || (joy1.dwZpos < zmax && z == 0) ){
                            keyboard.ki.wVk = keybrdMapCopy[22  + z];
                            keyboard.ki.dwFlags = KEYEVENTF_KEYUP;
                            SendInput(1, &keyboard, sizeof(keyboard));
                            z = -1;
                }
                
                if( keybrdMapCopy[21]!=0 && joy1.dwRpos > rmax && r == -1){
                        keyboard.ki.wVk = keybrdMapCopy[21];
                        keyboard.ki.dwFlags = 0;
                        SendInput(1, &keyboard, sizeof(keyboard));
                        r = 1;   
                }
                
                else if( keybrdMapCopy[20]!=0 && joy1.dwRpos < rmin && r == -1){
                            keyboard.ki.wVk = keybrdMapCopy[20];
                            keyboard.ki.dwFlags = 0;
                            SendInput(1, &keyboard, sizeof(keyboard));
                            r = 0;
                }
                
                else if( (joy1.dwRpos > rmin && r == 0) || (joy1.dwRpos < rmax && r == 1) ){
                            keyboard.ki.wVk = keybrdMapCopy[20  + r];
                            keyboard.ki.dwFlags = KEYEVENTF_KEYUP;
                            SendInput(1, &keyboard, sizeof(keyboard));
                            r = -1;
                }
        }
        
        else if( Xbox == TRUE ){
            if( keybrdMapCopy[22]!=0 && joy1.dwRpos > rmax && r == -1){
                    keyboard.ki.wVk = keybrdMapCopy[22];
                    keyboard.ki.dwFlags = 0;
                    SendInput(1, &keyboard, sizeof(keyboard));
                    r = 0;   
            }
            
            else if( keybrdMapCopy[23]!=0 && joy1.dwRpos < rmin && r == -1){
                        keyboard.ki.wVk = keybrdMapCopy[23];
                        keyboard.ki.dwFlags = 0;
                        SendInput(1, &keyboard, sizeof(keyboard));
                        r = 1;
            }
            
            else if( (joy1.dwRpos > rmin && r == 1) || (joy1.dwRpos < rmax && r == 0) ){
                        keyboard.ki.wVk = keybrdMapCopy[22  + z];
                        keyboard.ki.dwFlags = KEYEVENTF_KEYUP;
                        SendInput(1, &keyboard, sizeof(keyboard));
                        r = -1;
            }
            
            if( keybrdMapCopy[21]!=0 && joy1.dwUpos > umax && u == -1){
                    keyboard.ki.wVk = keybrdMapCopy[21];
                    keyboard.ki.dwFlags = 0;
                    SendInput(1, &keyboard, sizeof(keyboard));
                    u = 1;   
            }
            
            else if( keybrdMapCopy[20]!=0 && joy1.dwUpos < umin && u == -1){
                        keyboard.ki.wVk = keybrdMapCopy[20];
                        keyboard.ki.dwFlags = 0;
                        SendInput(1, &keyboard, sizeof(keyboard));
                        u = 0;
            }
            
            else if( (joy1.dwUpos > umin && u == 0) || (joy1.dwUpos < umax && u == 1) ){
                        keyboard.ki.wVk = keybrdMapCopy[20  + r];
                        keyboard.ki.dwFlags = KEYEVENTF_KEYUP;
                        SendInput(1, &keyboard, sizeof(keyboard));
                        u = -1;
            }
        }
            ///////////////////////////////////////////////////////
             
        
             if( joy1.dwButtons != 0 || joy1.dwPOV < 32000 || ZOK == TRUE ){
                
                 d = getButton( joy1 );
                 
                 if ( mouseMapCopy[9] == 3)
                      mouse(-1);
                
               
                 mouseCheck1 = FALSE;  mouseCheck2 = FALSE; mouseCheck31 = FALSE;
                 for( int  y=0; y<d; y++){
                      if( keybrdMapCopy[buttons[y] - 1] != 0 ){
                          
                           if( buttonState[ buttons[y] - 1 ] == 0 ){
                               keyboard.ki.wVk = keybrdMapCopy[ buttons[y]-1 ];
                               keyboard.ki.dwFlags = 0;
                               SendInput(1, &keyboard, sizeof(keyboard));
                               buttonState[ buttons[y] - 1 ] = 1;
                           }
                           
                      }
                       
                      else{
                               if( mouseMapCopy[4] == buttons[y] ){
                                   mouse(4);
                                   mouseCheck1 = TRUE; 
                                }
                                
                                else if( mouseMapCopy[5] == buttons[y] ){
                                   mouse(5);
                                   mouseCheck2 = TRUE;
                                }
                                
                                else if( mouseMapCopy[6] == buttons[y] ){
                                    if( mouseCheck3 == FALSE ){
                                        SendInput(1, &mouseMiddle, sizeof(mouseMiddle));
                                        mouseCheck3 = TRUE;
                                    }
                                    mouseCheck31 = TRUE;
                                }
                                
                                else if( mouseMapCopy[7] == buttons[y] ){
                                   if( a1 == 800 )
                                        SendInput(1, &mouseScrollUp, sizeof(mouseScrollUp));
                  
                                     a1 = (++a1) % 801 ;
                                }
                                
                                else if( mouseMapCopy[8] == buttons[y] ){
                                  if( a1 == 800 )
                                        SendInput(1, &mouseScrollDown, sizeof(mouseScrollDown));
                  
                                     a1 = (++a1) % 801 ;
                                }
                                
                                
                      }
                 }//for  
                 
                 if( mouseCheck1 == FALSE && prevClick == 1 )
                        mouse(-2);
                 
                 else if( mouseCheck2 == FALSE && prevClick == 2 )
                        mouse(-2);
                        
                 if( mouseCheck3 == TRUE && mouseCheck31 == FALSE){
                    mouseCheck3 = FALSE;
                }
                 
                for( int f = 0; f<16 ; f++ ){
                      if( buttonState[f] !=0 ){
                          no = FALSE;
                          for( int l =0 ;l<5; l++)
                               if( buttons[l] == (f+1) )
                                   no = TRUE;
                          
                          if( no == FALSE ){
                              keyboard.ki.wVk = keybrdMapCopy[f];
                              keyboard.ki.dwFlags = KEYEVENTF_KEYUP;
                              SendInput(1, &keyboard, sizeof(keyboard));
                              buttonState[ f ] = 0;
                          } 
                      }// if butonstate
                 } //for - f
            }   //main if     
             
             else{
             if ( prevClick != 0 )
                       mouse( -2 );
            if (mouseCheck3 == TRUE)
                mouseCheck3 = FALSE;
                       
             for( int f = 0; f<16 ; f++ ){
                      if( buttonState[f] !=0 ){
                            keyboard.ki.wVk = keybrdMapCopy[f];
                            keyboard.ki.dwFlags = KEYEVENTF_KEYUP;
                            SendInput(1, &keyboard, sizeof(keyboard));
                            buttonState[ f ] = 0;
                    }
            }
        }
             tempX = joy1.dwXpos;
             tempY = joy1.dwYpos;
             tempZ = joy1.dwZpos;
             tempR = joy1.dwRpos;  
             tempU = joy1.dwUpos;
    } //while
 
}//function


int joyThread::getButton( JOYINFOEX joy  ){
    
    int inp = joy.dwButtons;
    int povinp;
    int Zpos = joy.dwZpos;
    int i=0, no1=0;
    
    if( POV == TRUE )
        povinp = joy.dwPOV;
    else
        povinp = 40000;    
    
    if ( Xbox == TRUE){
        if( joy.dwZpos > zmax ){
            buttons[i] = 9;
            i++;no1++;
            ZOK = FALSE;
        }
        else if( joy.dwZpos < zmin ){
            buttons[i] = 10;
            i++;no1++;
            ZOK = FALSE;
        }    
}
    if( povinp < 32000 ){
        switch( povinp ){
                case 0:
                     buttons[i] =  13;
                     no1++;i++;
                     break;
                     
                case 4500:
                     buttons[i] = 13; i++;
                     buttons[i] = 14; i++;
                     no1+=2;
                     break;
                     
                case 9000:
                     buttons[i] =  14;
                     no1++;i++;
                     break;
                
                case 13500:
                     buttons[i] = 14; i++;
                     buttons[i] = 15; i++;
                     no1+=2;
                     break;
                
                case 18000:
                     buttons[i] =  15;
                     no1++;i++;
                     break;
                     
                case 22500:
                     buttons[i] = 15; i++;
                     buttons[i] = 16; i++;
                     no1+=2;
                     break;
                     
                case 27000:
                     buttons[i] =  16;
                     no1++;i++;
                     break;
                
                case 31500:
                        buttons[i] = 16; i++;
                        buttons[i] = 13; i++;
                        no1+=2;
                        break;
        }     
    }
    
    
     if( inp != 0){
        
        for( int j = 1; j<=12; j++){
             
             if( inp & 0x01 ){
                 buttons[i] = j; i++;
                 no1++;
             }
             else if( inp == 0 ) 
                  break;             
             
             inp = inp >> 1;;
        }
    }

    
    return no1;
}


void joyThread::mouse( int in){
     if( in == -1 ){
         for( int b=0; b<d; b++){
              
             if( buttons[b] == mouseMapCopy[0] ){
                              butnegY = (butnegY + (sens1*0.0005) );
                              if( butnegY > 0.98 ){
                                  mousePad.mi.dwFlags = MOUSEEVENTF_MOVE;
                                  mousePad.mi.dy = -1;
                                  mousePad.mi.dx= 0;
                                  SendInput(1, &mousePad, sizeof(mousePad));
                                  butnegY=0;
                            }
            }
            
            else if( buttons[b] == mouseMapCopy[3] ){
                              butX= (butX + (sens1*0.0005));
                              if( butX > 0.98 ){
                                  mousePad.mi.dwFlags = MOUSEEVENTF_MOVE;
                                  mousePad.mi.dx = 1;
                                  mousePad.mi.dy =0;
                                  SendInput(1, &mousePad, sizeof(mousePad));
                                  butX = 0;
                            }
            }
            
            else if( buttons[b] == mouseMapCopy[1] ){
                              butY= (butY+ (sens1*0.0005));
                              if( butY > 0.98 ){
                                  mousePad.mi.dwFlags = MOUSEEVENTF_MOVE;
                                  mousePad.mi.dy = 1;
                                  mousePad.mi.dx = 0;
                                  SendInput(1, &mousePad, sizeof(mousePad));
                                  butY = 0;
                            }
            }
            
            else if( buttons[b] == mouseMapCopy[2] ){
                              butnegX= (butnegX+ (sens1*0.0005));
                              if( butnegX > 0.98 ){
                                  mousePad.mi.dwFlags = MOUSEEVENTF_MOVE;
                                  mousePad.mi.dx = -1;
                                  mousePad.mi.dy = 0;
                                  SendInput(1, &mousePad, sizeof(mousePad));
                                  butnegX=0;
                            }
            }
         }//for
     }//if
     
     else{
            if ( in == 4 && prevClick == 0){
                      SendInput(1, &mouseClick[0], sizeof(mouseClick[0]));
                        prevClick = 1;
             }
                  
             else if ( in == 5 && prevClick == 0 ){
                        SendInput(1, &mouseClick[2], sizeof(mouseClick[2]));
                        prevClick = 2;     
            }
            
             else if ( in == -2 ){
                  
                  if ( prevClick == 1 ){
                        SendInput(1, &mouseClick[1], sizeof(mouseClick[1]));
                        prevClick = 0;
                    }
                  else if( prevClick == 2 ){
                        SendInput(1, &mouseClick[3], sizeof(mouseClick[3]));
                        prevClick = 0;
                    }
            }
     }//else
}//function
