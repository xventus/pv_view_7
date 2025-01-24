//
// vim: ts=4 et
// Copyright (c) 2023 Petr Vanek, petr@fotoventus.cz
//
/// @file   rptask.h
/// @author Petr Vanek

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/**
 * @class RPTask
 * @brief Base class for managing FreeRTOS tasks with customizable behavior.
 */
class RPTask
{
public:
    /**
     * @brief Constructs an RPTask object.
     */
    RPTask();

    /**
     * @brief Destructor. Cleans up the task and releases resources.
     */
    virtual ~RPTask();

    /**
	 * @brief Initializes the FreeRTOS task with the specified parameters.
	 * 
	 * @param name Name of the task.
	 * @param priority Priority of the task. Default is tskIDLE_PRIORITY.
	 * @param stackDepth Stack depth for the task. Default is configMINIMAL_STACK_SIZE.
	 * @param coreId ID of the core to pin the task to (-1 for no pinning, use tskNO_AFFINITY).
	 * @return true if the task was created successfully, false otherwise.
	 */
    virtual bool init(const char *name, 
                  UBaseType_t priority = tskIDLE_PRIORITY, 
                  const configSTACK_DEPTH_TYPE stackDepth = configMINIMAL_STACK_SIZE,
                  BaseType_t coreId = tskNO_AFFINITY);


    /**
     * @brief Cleans up the task by deleting it if it is running.
     */
    virtual void done();

    /**
     * @brief Retrieves the handle of the created FreeRTOS task.
     * 
     * @return TaskHandle_t The handle of the task.
     */
    virtual TaskHandle_t task();

    /**
     * @brief Sets the priority of the running task dynamically.
     * 
     * @param priority The new priority to set.
     */
    void setPriority(UBaseType_t priority);

    /**
     * @brief Gets the current priority of the task.
     * 
     * @return UBaseType_t The current task priority, or tskIDLE_PRIORITY if the task is not running.
     */
    UBaseType_t getPriority() const;

protected:
    /**
     * @brief Static handler function passed to the FreeRTOS task creation API.
     * 
     * This function redirects execution to the `loop` method of the task instance.
     * 
     * @param pvParameters Pointer to the RPTask instance.
     */
    static void handler(void *pvParameters);

    /**
     * @brief Pure virtual function that implements the main execution loop of the task.
     * 
     * This function must be implemented by derived classes to define the task's behavior.
     */
    virtual void loop() = 0;

private:
    /**
     * @brief Handle to the FreeRTOS task. Used for task management.
     */
    TaskHandle_t _handle = NULL;
};

