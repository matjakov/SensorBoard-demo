/* 
 * File:   SensorBoard.h
 * Author: mk
 *
 * Created on 15. februar 2017, 12:06
 */

#ifndef SENSORBOARD_H
#define	SENSORBOARD_H
#include <SerialPort.h>
#include <Thread.h>
#include <Lock.h>
#include <queue>

struct SensorBoardData
{
public:
    time_t time;
    char source[20];
    int value;
};



class SensorBoard {
public:
    SensorBoard(const char *deviceName, int speed = B115200);
    virtual ~SensorBoard();
    bool IsActive();
    bool Start();
    void Stop();
    virtual void Poll();   
    void Enqueue(SensorBoardData &d);
    SensorBoardData* Dequeue();
    
private:
    static void *ListenerProc(void *argv);
    
    bool active;
    SerialPort serial;    
    Thread *listener;
    Mutex lock;
    std::queue<SensorBoardData> data; 
};

#endif	/* SENSORBOARD_H */

