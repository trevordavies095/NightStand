#include <pebble.h>

static Window *mainWindow;
static TextLayer *timeLayer;
static TextLayer *dateLayer;
static TextLayer *batteryLayer;
static GFont timeFont;
static GFont dateFont;
static GFont battFont;
static GFont chargeTimeFont;

static void update_time_battery()
{
	// Get a tm struct
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	BatteryChargeState charge_state = battery_state_service_peek();
	
	// Write the current time into a buffer
	static char timeBuffer[10];
	static char dateBuffer[10];
	static char batteryBuffer[16];
	
	// Set time
	strftime(timeBuffer, sizeof(timeBuffer), clock_is_24h_style() ?
			"%H:%M" : "%I:%M", tick_time);
	
	// Set date
	strftime(dateBuffer, sizeof(dateBuffer), "%b %e", tick_time);
	
	// IF the watch is charging
	if(charge_state.is_charging)
	{
		// Turn on the backlight
		light_enable(true);
		
		// Set the battery percentage
		snprintf(batteryBuffer, sizeof(batteryBuffer), "%d%% charging",
			charge_state.charge_percent);
		
		// Hide date
		layer_set_hidden((Layer *)dateLayer, true);
		
		// Bigger font
		text_layer_set_font(timeLayer, chargeTimeFont);
	}
	
	// ELSE the watch isn't charging
	else
	{
		// Turn off the backlight
		light_enable(false);
		
		// Set the baterry percntage
		snprintf(batteryBuffer, sizeof(batteryBuffer), "%d%%", 
		charge_state.charge_percent);
		
		// Show date
		layer_set_hidden((Layer *)dateLayer, false);
		
		// Smaller font
		text_layer_set_font(timeLayer, timeFont);
	}
	
	// Put the text on the time, date, battery TextLayers
	text_layer_set_text(timeLayer, timeBuffer);
	text_layer_set_text(dateLayer, dateBuffer);
	text_layer_set_text(batteryLayer, batteryBuffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
	// Refresh time and battery
	update_time_battery();
}

static void main_window_load(Window *window)
{
	// Get info about the Window
	Layer *windowLayer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(windowLayer);
	
	// Create the time TextLayer with specific bounds
	timeLayer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 80));
	
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
	
	// Create the battery TextLayer
	batteryLayer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(30,0), bounds.size.w, 56));
	
	// Make batteryLayer look pretty
	text_layer_set_background_color(batteryLayer, GColorClear);
	text_layer_set_text_color(batteryLayer, GColorWhite);
	text_layer_set_text_alignment(batteryLayer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
	
	// Create GFont
	timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_50));
	dateFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_28));
	battFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_18));
	chargeTimeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_70));
	text_layer_set_font(timeLayer, timeFont);
	text_layer_set_font(dateLayer, dateFont);
	text_layer_set_font(batteryLayer, battFont);
	
	// Add as child layer
	layer_add_child(windowLayer, text_layer_get_layer(timeLayer));
	layer_add_child(windowLayer, text_layer_get_layer(dateLayer));
	layer_add_child(windowLayer, text_layer_get_layer(batteryLayer));
}

static void main_window_unload(Window *window)
{
	// Destoy TextLayers
	text_layer_destroy(timeLayer);
	text_layer_destroy(dateLayer);
	text_layer_destroy(batteryLayer);
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
	
	//app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  	//app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum()); 
	
	// Intialize the time and battery
	update_time_battery();
	
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