#include "logs.h"

#include <time.h>

const char timeMask[] = "%H:%M:%S";

void getCurrentTimeStr(char* str, size_t bufferSize)
{
    time_t timeInfo = time(NULL);
    tm* time = localtime(&timeInfo);

    strftime(str, bufferSize, timeMask, time);
}

FILE* logOpenFile(const char* fileName)
{
    if (!fileName)
        return NULL;

    FILE* logFile = fopen(fileName, "w");
    if (!logFile)
        return NULL;

    setvbuf(logFile, NULL, _IONBF, 0);

    fprintf(logFile, "<pre style=\"background: #000000;color:#000000;\">");

    return logFile;
}