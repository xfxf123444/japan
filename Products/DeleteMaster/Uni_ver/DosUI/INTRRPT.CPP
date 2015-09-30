/*
 * interrupt.cpp
 *
 * Timer and disk interrupt handler.
 *
 * Created by Will, 17th May, 2004
 * Copyright(C) Shanghai S & T Dev. Ltd.
 */

#include "intrrpt.h"
#include <conio.h>

unsigned long TimerTick = 0;
void (_interrupt far *PrevTimerHandler)();
void (_interrupt far *PrevIntHandlerPri)();
void (_interrupt far *PrevIntHandlerSec)();

void _interrupt TimerHandler(void)
{
	if(PrevTimerHandler)
		PrevTimerHandler();
	
	TimerTick++;
	return;
}

void _interrupt HarddiskIntHandlerPri(void)
{
	outp(0xA0,0x20);	// EOI for slave PIC
	outp(0x20,0x20);	// EOI for master PIC
	return;
}

void interrupt HarddiskIntHandlerSec(void)
{
	_outp(0xA0,0x20);	// EOI for slave PIC
	outp(0x20,0x20);	// EOI for master PIC
	return;
}

unsigned long GetTimerTick()
{
	unsigned long Tick;

	_asm cli
	Tick = TimerTick;
	_asm sti
	return Tick;
}

void HardwareWait(unsigned long Ticks)
{
	unsigned long Tick;

	Tick = GetTimerTick();
	while(GetTimerTick() - Tick < Ticks);
}

unsigned long GetVect(int nInt)
{
	unsigned long far * lpVect;

	lpVect = (unsigned long far *)(nInt * 4);

	return *lpVect;
}

void SetVect(int nInt, unsigned long p)
{
	unsigned long far * lpVect;

	lpVect = (unsigned long far *)(nInt * 4);
	_asm cli
	*lpVect = p;
	_asm sti

	return;
}

