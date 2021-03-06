/**
 *  platform.h
 *
 *  A "platform" as needed by the v8 engine, implementing
 *  functionality required for, amongst other things,
 *  garbage collection.
 *
 *  @copyright 2015 Copernica B.V.
 */

/**
 *  Include guard
 */
#pragma once

/**
 *  Dependencies
 */
#include <v8-platform.h>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <mutex>
#include <deque>

/**
 *  Start namespace
 */
namespace JS {

/**
 *  Class definition
 */
class Platform : public v8::Platform
{
private:
    /**
     *  The list of tasks to execute
     *  @var    std::queue<Task*>
     */
    std::deque<v8::Task*> _tasks;

    /**
     *  Are we still supposed to be running?
     *  @var    std::atomic<bool>
     */
    std::atomic<bool> _running;

    /**
     *  Mutex to protect the task list
     *  @var    std::mutex
     */
    std::mutex _mutex;

    /**
     *  Condition variable to signal new work arriving
     *  @var    std::condition_variable
     */
    std::condition_variable _condition;

    /**
     *  The worker thread
     *  @var    std::thread
     */
    std::thread _worker;
    
    /**
     *  (blocking) method to stop running a platform.
     */
    void stop();

    /**
     *  Execute some work, this is called
     *  by the worker thread to keep going
     */
    void run();

    /**
     *  Private constructor, only one platform
     *  may be created, use the create() method
     */
    Platform();
public:
    /**
     *  Destructor
     */
    virtual ~Platform();

    /**
     *  Create a new platform if one does not exist yet.
     *
     *  This function is thread-safe.
     */
    static void create();

    /**
     *  Shutdown the platform
     *
     *  This function is thread-safe.
     */
    static void shutdown();

    /**
     *  Schedule a task to be executed on a background thread.
     *
     *  The platform takes responsibility for the task, and will
     *  free it when the task has finished executing.
     *
     *  @param  task    The task to execute
     *  @param  time    The expected run time
     */
    void CallOnBackgroundThread(v8::Task *task, ExpectedRuntime time) override;

    /**
     *  Schedule a task to be executed on a foreground thread.
     *
     *  @param  isolate The isolate the task belongs to
     *  @param  task    The task to execute
     */
    void CallOnForegroundThread(v8::Isolate *isolate, v8::Task *task) override;

    /**
     * Schedules a task to be invoked on a foreground thread wrt a specific
     * |isolate| after the given number of seconds |delay_in_seconds|.
     * Tasks posted for the same isolate should be execute in order of
     * scheduling. The definition of "foreground" is opaque to V8.
     */
    void CallDelayedOnForegroundThread(v8::Isolate *isolate, v8::Task *task, double delay_in_seconds) override;

    /**
     *  Retrieve the monotonically increasing time. The starting point
     *  is not relevant, but it must return at least millisecond-precision
     *
     *  @return time in milliseconds since an unspecified time
     */
    double MonotonicallyIncreasingTime() override;
};

/**
 *  End namespace
 */
}
