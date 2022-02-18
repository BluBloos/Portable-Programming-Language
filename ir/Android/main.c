/* This source file is in reference to 
both https://github.com/cnlohr/rawdrawandroid
and https://github.com/android/ndk-samples/blob/master/native-activity/app/src/main/cpp/main.cpp
*/

#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

struct engine {
    struct android_app* app;
    int32_t width;
    int32_t height;
};

// TODO(Noah): Use ANativeWindow to create a window and software raster
// using a full-blown font asset (just like HandmadeHero).
// Documentation for ANativeWindow: 
// https://developer.android.com/ndk/reference/group/a-native-window#anativewindow_acquire

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    auto* engine = (struct engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        //engine->animating = 1;
        //engine->state.x = AMotionEvent_getX(event, 0);
        //engine->state.y = AMotionEvent_getY(event, 0);
        return 1;
    }
    return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    auto* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            // engine->app->savedState = malloc(sizeof(struct saved_state));
            // *((struct saved_state*)engine->app->savedState) = engine->state;
            // engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            /* if (engine->app->window != nullptr) {
                engine_init_display(engine);
                engine_draw_frame(engine);
            } */
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            // engine_term_display(engine);

            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            /*if (engine->accelerometerSensor != nullptr) {
                ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                                               engine->accelerometerSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                                               engine->accelerometerSensor,
                                               (1000L/60)*1000);
            } */
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            /* if (engine->accelerometerSensor != nullptr) {
                ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                                                engine->accelerometerSensor);
            }
            // Also stop animating.
            engine->animating = 0;
            engine_draw_frame(engine);
            */
            break;
        default:
            break;
    }
}

/**
 * This is the function that application code must implement, representing
 * the main entry to the app. Provided via android_native_app_glue.h.
 */
extern void android_main(struct android_app* app) {

    struct engine engine = {};
    memset(&engine, 0, sizeof(engine));
    
    app->userData = &engine;
    app->onAppCmd = engine_handle_cmd;
    app->onInputEvent = engine_handle_input;
    app.app = state;

    if (state->savedState != nullptr) {
        // We are starting with a previous saved state; restore from it.
        // engine.state = *(struct saved_state*)state->savedState;
    }

    // Just want to log Hello, World to the debug console.
    // Then have the app stall and do nothing for the 
    // rest of time.
    bool suspended = true;

    while(1)
	{
		// Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while (
            // First param of ALooper_pollAll is the timeout. If -1, waits indefinitely until 
            // an event appears.
            (ident=ALooper_pollAll(-1, nullptr, &events, (void**)&source)) 
                >= 0
        ) {

            // Process this event.
            if (source != nullptr) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
            /* if (ident == LOOPER_ID_USER) {
                if (engine.accelerometerSensor != nullptr) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
                                                       &event, 1) > 0) {
                        LOGI("accelerometer: x=%f y=%f z=%f",
                             event.acceleration.x, event.acceleration.y,
                             event.acceleration.z);
                    }
                }
            } */

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                // engine_term_display(&engine);
                return;
            }
        }

		/* if( suspended ) { 
            usleep(50000); 
            continue; 
        }*/ 

        {
            /*
                Here we can do stuff that we desire our application to uniquely do.
            */
        }

	}

	return(0);
}