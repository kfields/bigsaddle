#pragma once

#include <chrono>

struct Timer {
public:
    typedef std::chrono::nanoseconds duration;
    typedef duration::rep rep;
    typedef duration::period period;
    typedef std::chrono::time_point<std::chrono::high_resolution_clock, duration> time_point;

    std::chrono::high_resolution_clock clock;
    time_point startTime;		//tick count at construction
    time_point lastElapsed;	//
    duration alarmInterval;	//
    time_point alarmTime;		//
    duration frequency;

    Timer() {
        Reset();
    }
    Timer(duration interval) {
        Reset();
        SetAlarm(interval);
    }
    time_point Now() { return clock.now(); }
    void Reset() {
        frequency = duration(1);
        startTime = Now();
        lastElapsed = startTime;
        SetAlarm(frequency);
    }
    void SetAlarm(duration interval) {
        auto time = Now();
        alarmInterval = interval;
        alarmTime = time + alarmInterval;
    }
    void ResetAlarm() {
        auto time = Now();
        alarmTime = time + alarmInterval;
    }
    void ResetAlarmMinus(duration interval) {
        auto time = Now();
        alarmTime = time + (alarmInterval - interval);
    }
    bool CheckAlarm() {
        auto time = Now();
        if (time >= alarmTime)
            return true;
        else
            return false;
    }
    long GetElapsed() {
        auto time = Now();
        long elapsed = long(std::chrono::duration_cast<std::chrono::milliseconds>(time - lastElapsed).count());
        lastElapsed = time;
        return elapsed;
    }
};
