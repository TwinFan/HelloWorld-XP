// Downloaded from https://developer.x-plane.com/code-sample/hello-world-sdk-3/

#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMMenus.h"
#include "XPLMPlanes.h"
#include "XPLMDataAccess.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include <cstring>
#include <string>
#include <chrono>
#include <thread>
#if IBM
	#include <windows.h>
#endif

// OpenGL is system-dependent
#include "OpenGL/SystemGL.h"

#ifndef XPLM300
	#error This is made to be compiled against the XPLM300 SDK
#endif

// An opaque handle to the window we will create
static XPLMWindowID	g_window;
constexpr size_t NUM_BTN = 4;
static XPWidgetID g_btn[NUM_BTN] = {nullptr, nullptr, nullptr, nullptr};
static int g_delay[NUM_BTN] = { 2000, 4000, 6000, 8000 };
static XPLMMenuID hMenu = nullptr;

// Callbacks we will register when we create our window
void				draw_hello_world(XPLMWindowID in_window_id, void * in_refcon);
int					dummy_mouse_handler(XPLMWindowID /*in_window_id*/, int /*x*/, int /*y*/, int /*is_down*/, void * /*in_refcon*/) { return 0; }
XPLMCursorStatus	dummy_cursor_status_handler(XPLMWindowID /*in_window_id*/, int /*x*/, int /*y*/, void * /*in_refcon*/) { return xplm_CursorDefault; }
int					dummy_wheel_handler(XPLMWindowID /*in_window_id*/, int /*x*/, int /*y*/, int /*wheel*/, int /*clicks*/, void * /*in_refcon*/) { return 0; }
void				dummy_key_handler(XPLMWindowID /*in_window_id*/, char /*key*/, XPLMKeyFlags /*flags*/, char /*virtual_key*/, void * /*in_refcon*/, int /*losing_focus*/) { }
float               CBFlightLoop(float, float, int, void *);


// Keep track of controlling AI/TCAS
static bool bHaveControl = false;
static XPLMDataRef drTcasOverride = nullptr;

static std::string whoAskedForAI;


/// Callback function for menu
void CBMenu (void* /*inMenuRef*/, void* /*inItemRef*/)
{
    // Toggle TCAS control
    if (!bHaveControl) {
        if (XPLMAcquirePlanes(NULL, NULL, NULL) != 0)
        {
            // Got control, now switch on TCAS override
            XPLMSetDatai(drTcasOverride, 1);
            XPLMDebugString("Hello TCAS just got TCAS control!\n");
            bHaveControl = true;
        }
    } else {
        // Release control
        XPLMSetDatai(drTcasOverride, 0);
        XPLMReleasePlanes();
        XPLMDebugString("Hello TCAS just released TCAS control!\n");
        bHaveControl = false;
    }
    
    XPLMCheckMenuItem(hMenu, 0, bHaveControl ? xplm_Menu_Checked : xplm_Menu_Unchecked);
    
    whoAskedForAI.clear();
}


// Widget Callback
int CBBtn (XPWidgetMessage      inMessage,
           XPWidgetID           inWidget,
           intptr_t             /*inParam1*/,
           intptr_t             /*inParam2*/)
{
    if (inMessage != xpMsg_PushButtonPressed) return 0;
    
    for (size_t i = 0; i < NUM_BTN; i++)
    {
        if (inWidget == g_btn[i]) {
            std::this_thread::sleep_for(std::chrono::milliseconds(g_delay[i]));
            return 1;
        }
    }
    return 0;
}


PLUGIN_API int XPluginStart(
							char *		outName,
							char *		outSig,
							char *		outDesc)
{
	strcpy(outName, "Hello TCAS");
	strcpy(outSig, "twinfan.test.helloTCAS");
	strcpy(outDesc, "A Hello World plug-in for getting TCAS control.");
	
	XPLMCreateWindow_t params;
	params.structSize = sizeof(params);
	params.visible = 1;
	params.drawWindowFunc = draw_hello_world;
	// Note on "dummy" handlers:
	// Even if we don't want to handle these events, we have to register a "do-nothing" callback for them
	params.handleMouseClickFunc = dummy_mouse_handler;
	params.handleRightClickFunc = dummy_mouse_handler;
	params.handleMouseWheelFunc = dummy_wheel_handler;
	params.handleKeyFunc = dummy_key_handler;
	params.handleCursorFunc = dummy_cursor_status_handler;
	params.refcon = NULL;
	params.layer = xplm_WindowLayerFloatingWindows;
	// Opt-in to styling our window like an X-Plane 11 native window
	// If you're on XPLM300, not XPLM301, swap this enum for the literal value 1.
	params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;
	
	// Set the window's initial bounds
	// Note that we're not guaranteed that the main monitor's lower left is at (0, 0)...
	// We'll need to query for the global desktop bounds!
	int left, bottom, right, top;
	XPLMGetScreenBoundsGlobal(&left, &top, &right, &bottom);
	params.left = left + 50;
	params.bottom = bottom + 150;
	params.right = params.left + 200;
	params.top = params.bottom + 200;
	
	g_window = XPLMCreateWindowEx(&params);
	
	// Position the window as a "free" floating window, which the user can drag around
	XPLMSetWindowPositioningMode(g_window, xplm_WindowPositionFree, -1);
	// Limit resizing our window: maintain a minimum width/height of 100 boxels and a max width/height of 300 boxels
	XPLMSetWindowResizingLimits(g_window, 200, 200, 300, 300);
	XPLMSetWindowTitle(g_window, "Hello TCAS");
    
    // Add buttons to it
    int y = 50;
    for (size_t i = 0; i < NUM_BTN; ++i, y += 30) {
        char desc[20];
        snprintf(desc, sizeof(desc), "%dms", g_delay[i]);
        
        XPWidgetID& wid = g_btn[i];
        wid = XPCreateWidget(10, y, 100, y-30, 1, desc, 1, nullptr, xpWidgetClass_Button);
        if (wid) {
            XPSetWidgetProperty(wid, xpProperty_ButtonType, xpPushButton);
            XPAddWidgetCallback(wid, CBBtn);
        }
    }
    
    // Create the menu for controlling TCAS
    drTcasOverride = XPLMFindDataRef("sim/operation/override/override_TCAS");
    int my_slot = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "Hello TCAS", NULL, 0);
    hMenu = XPLMCreateMenu("Hello TCAS", XPLMFindPluginsMenu(), my_slot, CBMenu, NULL);
    XPLMAppendMenuItem(hMenu, "Toggle TCAS Control",      (void*)1, 0);

    XPLMDebugString("Hello TCAS: Started!\n");
	
	return g_window != NULL;
}

PLUGIN_API void	XPluginStop(void)
{
	// Since we created the window, we'll be good citizens and clean it up
	XPLMDestroyWindow(g_window);
	g_window = NULL;
    XPLMDebugString("Hello TCAS: Stopped!\n");
}

PLUGIN_API void XPluginDisable(void)
{
    XPLMDebugString("Hello TCAS: Disabled!\n");
}


PLUGIN_API int  XPluginEnable(void)
{
    XPLMDebugString("Hello TCAS: Enabled\n");
    
    // Let's try to get TCAS control
    if (!bHaveControl)
        CBMenu(nullptr, nullptr);
    
    return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * /*inParam*/)
{
    if (inMsg != XPLM_MSG_RELEASE_PLANES)
        return;
    
    char who[256] = "?";
    XPLMGetPluginInfo(inFrom, who, NULL, NULL, NULL);
    char msg[1000];
    snprintf(msg,sizeof(msg),"Hello TCAS: '%s' (id %d) requested us to release TCAS!\n",
             who, inFrom);
    XPLMDebugString(msg);
    
    snprintf(msg,sizeof(msg),"'%s' (id %d) requested us to release TCAS!\n",
             who, inFrom);
    whoAskedForAI = msg;
}

void	draw_hello_world(XPLMWindowID in_window_id, void * /*in_refcon*/)
{
	// Mandatory: We *must* set the OpenGL state before drawing
	// (we can't make any assumptions about it)
	XPLMSetGraphicsState(
						 0 /* no fog */,
						 0 /* 0 texture units */,
						 0 /* no lighting */,
						 0 /* no alpha testing */,
						 1 /* do alpha blend */,
						 1 /* do depth testing */,
						 0 /* no depth writing */
						 );
	
	int l, t, r, b;
	XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b);
	
	float col_white[] = {1.0, 1.0, 1.0}; // red, green, blue
	
    l += 10;
    t -= 20;
    
    for (int i = 0; i < 5; i++) {
        XPLMDrawString(col_white, l, t, (char*)"Hello World!", NULL, xplmFont_Proportional);
        t -= 20;
    }
    
    XPLMDrawString(col_white, l, t, (char*)(bHaveControl ? "HAVE TCAS control" : "DO NOT have TCAS control"), NULL, xplmFont_Proportional);
    t -= 20;
    if (!whoAskedForAI.empty())
        XPLMDrawString(col_white, l, t, (char*)whoAskedForAI.c_str(), NULL, xplmFont_Proportional);
    
}
