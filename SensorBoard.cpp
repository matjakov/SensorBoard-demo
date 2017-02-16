/* 
 * File:   SensorBoard.cpp
 * Author: mk
 * 
 * Created on 15. februar 2017, 12:06
 */
#define DEBUG 1
#include <Debug.h>
#include "SensorBoard.h"
#include <RegEx.h>
#include <iostream>
using namespace std;

SensorBoard::SensorBoard(const char *deviceName, int speed):
    serial(deviceName, speed),
    listener(NULL),
    valid(false)
{
    serial.SetBlocking(true);
    serial.SetTimeout(5000);
}


SensorBoard::~SensorBoard() 
{
    Stop();
}

bool SensorBoard::IsValid()
{
    return valid;
}

/**
 * Starts the listener thread.
 */
bool SensorBoard::Start()
{
    if (!serial.Open()) {
        TRACE("Error opening device '%s'.\n", serial.Port());
        return false;
    }    
    valid = true;
    
    if (listener == NULL) {
        listener = new Thread(ListenerProc, this);
        return true;
    }
    else
        TRACE("Already started.\n");
    return false;
}

/**
 * Tells the listener thread to stop.
 */
void SensorBoard::Stop()
{
    serial.Close();
    valid = false;
    if (listener) {
        // wait 
        for (int i = 0; i < 10 && listener->TryJoin(); i++) {
            PRINT("Stopping SensorBoard [%d] ...\n", i); 
            Thread::Sleep(1000);
        }
        listener = NULL;        
    }    
}

void SensorBoard::Enqueue(SensorBoardData &d)
{
    lock.Lock();
    data.push(d);
    lock.Unlock();
}


SensorBoardData* SensorBoard::Dequeue()
{
    SensorBoardData *d = NULL;
    lock.Lock();
    if (!data.empty()) {
        d = &data.front();
        data.pop();
    }
    lock.Unlock();    
    return d;
}
    

/**
 * Reads HW.
 */
void SensorBoard::Poll()
{
    RegEx re("^([AD][0-9]+): ([0-9]+)");
    
    char s[32];
    
    if (serial.ReadLine(s, 31) > 0) {
        PRINT("%s\n", s);
        if (re.Match(s) == 0)
        {
            char s1[32];
            re.Sub(1,s1);
            PRINT("SUBS: %d, SRC: %s, VAL: %s\n", re.SubCount(), re.Sub(1).c_str(), re.Sub(2).c_str());
        }
        
    }    
}



/**
 * Thread proc.
 */
void *SensorBoard::ListenerProc(void *arg)
{
    SensorBoard *board = (SensorBoard*)arg;
    PRINT("SensorBoard START.\n"); 
    while (board->IsValid()) 
    {
        board->Poll();
        Thread::Sleep(5);
    }    
    PRINT("SensorBoard EXIT.\n"); 
    return NULL;
}

