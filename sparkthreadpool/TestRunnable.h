#pragma once
#include "sparkthreadpool\sparkrunnable.hpp"

class TestRunnable :public Runnable
{
public:
    TestRunnable();
    ~TestRunnable();

protected:
    virtual void SafeRun();
};

