// statemachine.h, part of the Finite State Machine sample at
// http://home.apu.edu/~jcox/Demos/fsm/fsm.htm
// Copyright (C) 2001 by John R. Cox.  All rights reserved.
// This source code may be freely copied, modified, and distributed.

#ifndef _MY_FSM_H
#define _MY_FSM_H

typedef enum {
    S0,       /* initial state                              */
    S1,       /* transition into comment state              */
    S2,       /* comment state                              */
    S3        /* (possible) transition out of comment state */
}KEY_STATE;
#define FSM_NUM_ROWS 4      

typedef enum {
    I0,       /* char '/'       */
    I1,       /* char '*'       */
    Iother    /* anything else  */
}INPUTVALUE;
#define FSM_NUM_COLS 3

KEY_STATE GetState(const KEY_STATE KS, const char ch);
#endif // _MY_FSM_H

