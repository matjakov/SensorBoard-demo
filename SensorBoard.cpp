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
    device(deviceName),
    listener(NULL),
    active(false)
        
{
}


SensorBoard::~SensorBoard() 
{
    Stop();
}

bool SensorBoard::IsActive()
{
    return active;
}

/**
 * Starts the listener thread.
 */
bool SensorBoard::Start()
{
    if (!serial.Open(device.c_str())) {
        TRACE("Error opening device '%s'.\n", device.c_str());
        return false;
    }    
    serial.SetTimeout(5000);
    
    active = true;
    
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
    active = false;
    if (listener) {
        // wait 
        for (int i = 0; i < 10 && listener->TryJoin(); i++) {
            PRINT("Stopping SensorBoard [%d] ...\n", i); 
            Thread::Sleep(1000);
        }
        listener = NULL;        
    }    
}


/**
 * Thread-safe data push
 */
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
        // get pointer
        d = &data.front();
        // remove
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
    int i = 0;    
    while (i < 32 && (s[i] = serial.ReadChar()) != '\n')
    {        
        i++;
    }
    s[i] = 0;


    PRINT("%s\n", s);
    if (re.Match(s) == 0)
    {
        char s1[32];
        re.Sub(1,s1);
        PRINT("SOURCE: '%s', VALUE: %s\n", re.Sub(1).c_str(), re.Sub(2).c_str());
        // TODO: Q data
    }
    
}



/**
 * Thread proc.
 */
void *SensorBoard::ListenerProc(void *arg)
{
    SensorBoard *board = (SensorBoard*)arg;
    PRINT("SensorBoard START.\n"); 
    while (board->IsActive()) 
    {
        board->Poll();
        Thread::Sleep(5);
    }    
    PRINT("SensorBoard EXIT.\n"); 
    return NULL;
}

