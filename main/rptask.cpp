//
// vim: ts=4 et
// Copyright (c) 2023 Petr Vanek, petr@fotoventus.cz
//
/// @file   rptask.cpp
/// @author Petr Vanek

#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "rptask.h"

RPTask::RPTask() {
    ;
}

RPTask::~RPTask()
{
    done();
}

void RPTask::done()
{
    if (_handle != NULL)
    {
        vTaskDelete(_handle);
        _handle = NULL;
    }
}

TaskHandle_t RPTask::task()
{
    return _handle;
}

bool RPTask::init(const char *name, 
                  UBaseType_t priority, 
                  const configSTACK_DEPTH_TYPE stackDepth, 
                  BaseType_t coreId) 
{
    // Use xTaskCreatePinnedToCore to allow pinning to a specific core
    BaseType_t res = xTaskCreatePinnedToCore(
        RPTask::handler,  // Task function
        name,             // Task name
        stackDepth,       // Stack depth
        (void *)this,     // Parameters
        priority,         // Task priority
        &_handle,         // Task handle
        coreId            // Core to pin the task (-1 for no pinning)
    );

    return (res == pdPASS);
}

void RPTask::handler(void *pvParameters)
{
    RPTask *task = (RPTask *)pvParameters;
    if (task != NULL)
    {
        task->loop();
    }
}

void RPTask::setPriority(UBaseType_t priority) {
    if (_handle != NULL) {
        vTaskPrioritySet(_handle, priority);
    }
}

UBaseType_t RPTask::getPriority() const {
    if (_handle != NULL) {
        return uxTaskPriorityGet(_handle);
    }
    return tskIDLE_PRIORITY; // Return idle priority if task is not running
}
