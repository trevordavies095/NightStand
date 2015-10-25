#include <pebble.h>

static Window *mainWindow;
static TextLayer *timeLayer;
static TextLayer *dateLayer;
static GFont timeFont;
static GFont dateFont;

static void update_time()
{
	// Get a tm struct
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	
	// Write the current time into a buffer
	static char timeBuffer[8];
	static char dateBuffer[8];
	
	strftime(timeBuffer, sizeof(timeBuffer), clock_is_24h_style() ?
			"%H:%M" : "%I:%M", tick_time);
	
	strftime(dateBuffer, sizeof(dateBuffer), "%b %e", tick_time);
	
	// Display the time on timeLayer
	text_layer_set_text(timeLayer, timeBuffer);
	text_layer_set_text(dateLayer, dateBuffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
	update_time();
}

static void main_window_load(Window *window)
{
	// Get info about the Window
	Layer *windowLayer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(windowLayer);
	
	// Create the time TextLayer with specific bounds
	timeLayer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
	
	// Make timeLayer look pretty
	text_layer_set_background_color(timeLayer, GColorClear);
	text_layer_set_text_color(timeLayer, GColorWhite);
	text_layer_set_text_alignment(timeLayer, GTextAlignmentCenter);
	
	// Create the date TextLayer
	dateLayer = text_layer_create(
		GRect(0, 107, bounds.size.w, 56));
	
	// Make dateLayer look pretty
	text_layer_set_background_color(dateLayer, GColorClear);
	text_layer_set_text_color(dateLayer, GColorWhite);
	text_layer_set_text_alignment(dateLayer, GTextAlignmentCenter);
	
	// Create GFont
	timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TIME_FONT_50));
	dateFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DATE_FONT_28));
	text_layer_set_font(timeLayer, timeFont);
	text_layer_set_font(dateLayer, dateFont);
	
	// Add as child layer
	layer_add_child(windowLayer, text_layer_get_layer(timeLayer));
	layer_add_child(windowLayer, text_layer_get_layer(dateLayer));
}

static void main_window_unload(Window *window)
{
	// Destoy TextLayers
	text_layer_destroy(timeLayer);
	text_layer_destroy(dateLayer);
}

static void init()
{
	// Set up main window
	mainWindow = window_create();
	
	// Set handlers to manage the elements inside the window
	window_set_window_handlers(mainWindow, (WindowHandlers)
	{
		.load = main_window_load,
		.unload = main_window_unload
	});
	
	// Push the window to the watch
	window_stack_push(mainWindow, true);
	
	// Intialize the time
	update_time();
	
	// Setup TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
	// Setup background color
	window_set_background_color(mainWindow, GColorBlack);
}

static void deinit()
{
	// Destroy mainWindow
	window_destroy(mainWindow);
}

int main(void)
{
	init();
	app_event_loop();
	deinit();
}