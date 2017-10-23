/* 
 * File:   sequencer.h
 * Author: M'nu
 *
 * Created on 9 mars 2017, 23:03
 */

#ifndef SEQUENCER_H
#define	SEQUENCER_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct 
    {
        bool Elapsed;
        bool Loop;
        int Ticks;
        int Counter;
    } Timer;
    
    void Sequencer_Init();
    void Sequencer_Start();
    void Sequencer_StartTimer(float ATime, bool ALoop);
    void Sequencer_StopTimer();
    void Sequencer_TimerEvent();
    
    Timer _Timer;

#ifdef	__cplusplus
}
#endif

#endif	/* SEQUENCER_H */

