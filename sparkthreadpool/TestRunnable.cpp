#include "stdafx.h"
#include "TestRunnable.h"

TestRunnable::TestRunnable()
{
}

TestRunnable::~TestRunnable()
{
    ReleaseRunObj();
}

void TestRunnable::SafeRun()
{
    int nTotal = 0;
    for (int i = 0; i < 1000; ++i)
    {
        nTotal += i;
    }
}
