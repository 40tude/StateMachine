// FSM.c, part of the Finite State Machine sample at
// http://home.apu.edu/~jcox/Demos/fsm/fsm.htm
// Copyright (C) 2001 by John R. Cox.  All rights reserved.
// This source code may be freely copied, modified, and distributed.

#include "statemachine.h"

// State Transition Table (STT)
KEY_STATE STT[FSM_NUM_ROWS][FSM_NUM_COLS] = {

            /*  I0      I1      Iother        */
            /*  '/'     '*'     anything else */            
/* S0 */    {   S1,     S0,         S0},     
/* S1 */    {   S0,     S2,         S0},     
/* S2 */    {   S2,     S3,         S2},     
/* S3 */    {   S0,     S3,         S2}     
};


KEY_STATE GetState(const KEY_STATE KS, const char ch){

  INPUTVALUE iv;

  switch (ch){
    case '/':   
      iv = I0;    
    break;
  
    case '*':   
      iv = I1;    
    break;
  
    default:    
      iv = Iother;
  }
  return STT[KS][iv];
}

