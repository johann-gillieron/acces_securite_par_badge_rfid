/* @brief User interface for the TCP tests
 * 
 * @details This file contains the user interface for the TCP tests. It is used to interact with the user and to get the necessary input for the tests.
 * 
 * @date 2024-07-21
 */
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/nfc.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(userInterface, LOG_LEVEL_INF);
#include <stdio.h>
#include "userInterface.h"
#include "logs.h"
#include "users.h"
#include "memoryManagement.h"
#include "deviceInformation.h"
#include "desfire/DesfireApp.h"
#include "fonts/font_poppins_14.h"
#include "fonts/font_poppins_28.h"


extern const struct device *clrc663;
extern const struct device *display_dev;

#define relay_inverted True


/*********************************
 * 		Thread Configuration 	 *
 ********************************/
// User interface thread priority level
#define USERINTERFACE_STACK_SIZE 8192
// User interface thread priority level
#define USERINTERFACE_PRIORITY 6
// User interface stack definition
K_THREAD_STACK_DEFINE(USERINTERFACE_STACK, USERINTERFACE_STACK_SIZE);
// Variable to identify the user interface thread
static struct k_thread userInterfaceThread;

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
#define SW1_NODE	DT_ALIAS(sw1)
#if !DT_NODE_HAS_STATUS(SW1_NODE, okay)
#error "Unsupported board: sw1 devicetree alias is not defined"
#endif


/*
 * The led0 devicetree alias is optional. If present, we'll use it
 * to turn on and off the action in physical world.
 */
static struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0}); // it's the pin p1.06 on the nRF7002 DK

static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,{0}); // it's the pin p1.08 on the nRF7002 DK
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(SW1_NODE, gpios,{0}); // it's the pin p1.09 on the nRF7002 DK
static struct gpio_callback button0_cb_data;
static struct gpio_callback button1_cb_data;
static bool flag_btn = false;

K_MSGQ_DEFINE(add_queue, 1, 24, 1);
K_MSGQ_DEFINE(minus_queue, 1, 24, 1);
K_MSGQ_DEFINE(both_queue, 1, 1, 1);


// LVGL objects for the display
static lv_obj_t *label_title_center;
static lv_obj_t *label_title_high;
static lv_obj_t *label_subtitle;
static lv_obj_t *label_Name;
static lv_obj_t *label_Phone;
static lv_obj_t *label_TimeRemaining;
static lv_obj_t *label_lock_status;
static lv_obj_t *label_btn_right_up;
static lv_obj_t *label_btn_right_down;
static lv_obj_t *label_btn_line;
static lv_obj_t *label_btn_line;
static lv_point_t line_points[] = {{0, 0}, {0, 122}, {0, 61}, {20, 61}};
static lv_style_t big_font, small_font;
static lv_style_t style_line;
lv_style_t* big_font_pointer = &big_font;
lv_style_t* mid_font_pointer = &big_font;
lv_style_t* small_font_pointer = &small_font;

/*! Button pressed callback
 *  @brief This function is called when a button is pressed.
 *  @param dev The device structure
 *  @param cb The callback structure
 *  @param pins The pins that are pressed
*/
static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    static uint32_t last_pins = 0;
	static uint64_t last_click = 0;
    static uint8_t btn = true;
	uint32_t click;
    uint32_t current_pins = pins;
    uint64_t actual_click = k_uptime_get();
	uint64_t time_diff = actual_click - last_click;
	click = (gpio_pin_get_raw(dev, button0.pin) == gpio_pin_get_raw(dev, button1.pin));

    // Check if both buttons are pressed
    if ((click && ((time_diff) > 200)) || ((last_pins != current_pins) && (time_diff < 200))) {
        k_msgq_put(&both_queue, &btn, K_NO_WAIT);
    }
    // Check if button 0 is pressed
    else if ((current_pins & BIT(button0.pin)) && (!(last_pins & BIT(button0.pin)) || ((time_diff) > 200))) {
        k_msgq_put(&add_queue, &btn, K_NO_WAIT);
    }
    // Check if button 1 is pressed
    else if ((current_pins & BIT(button1.pin)) && (!(last_pins & BIT(button1.pin)) || ((time_diff) > 200))) {
        k_msgq_put(&minus_queue, &btn, K_NO_WAIT);
    }
    flag_btn = true;
    LOG_INF("Button pressed");

	last_click = actual_click;
    last_pins = current_pins;
}

// Function to test the screen display
void display_init() {
	static lv_obj_t *info_value;
	static lv_obj_t *info_text;
	static lv_obj_t *info_label;
	static lv_obj_t *info_scr = NULL;
	info_scr = lv_obj_create(NULL);

	//Initialize fonts
	lv_style_init(big_font_pointer);
	lv_style_init(small_font_pointer);

	lv_style_set_text_font(big_font_pointer, &font_poppins_28);
	lv_style_set_text_font(small_font_pointer, &font_poppins_14);

	//Main label for informative text	
	info_value = lv_label_create(info_scr);
	lv_obj_add_style(info_value, big_font_pointer, LV_PART_MAIN);
	lv_obj_align(info_value, LV_ALIGN_TOP_MID, 0, -10);

	// Sublabel for informative text
	info_text = lv_label_create(info_scr);
	lv_obj_add_style(info_text, small_font_pointer, LV_PART_MAIN);
	lv_obj_align(info_text, LV_ALIGN_CENTER, 0, -2);

	//Sublabel for informative text
	info_label = lv_label_create(info_scr);
	lv_obj_add_style(info_label, small_font_pointer, LV_PART_MAIN);
	lv_obj_align(info_label, LV_ALIGN_BOTTOM_MID, 0, -10);

	//Text	
	lv_label_set_text(info_value, "Johann Gilliéron");
	//Subtext
	lv_label_set_text(info_text, "Travail de Bachelor");
	//Subtext
	lv_label_set_text(info_label, "  Électronique compacte pour  \naccès sécurité par badge RFID");

	lv_scr_load(info_scr);

	lv_task_handler(); // Handle the LVGL tasks
	k_sleep(K_SECONDS(3)); // Wait for the display to be refreshed	
}


// Function to create the information display
void create_interface(lv_obj_t * parent) {
	// Container for the information
	lv_obj_clean(parent);

    /****** basic screen (title and subtext) ******/
    // Create a label for the high title
    label_title_high = lv_label_create(parent);
	lv_obj_add_style(label_title_high, big_font_pointer, LV_PART_MAIN);
    lv_label_set_text_static(label_title_high, "Système en veille");
    lv_obj_align(label_title_high, LV_ALIGN_CENTER, 0, -20);

    // Create a label for the sleep info
    label_subtitle = lv_label_create(parent);
    lv_obj_add_style(label_subtitle, small_font_pointer, LV_PART_MAIN);
    lv_label_set_text_static(label_subtitle, "              Le système est en veille\nAppuyer sur un bouton pour activer");
    lv_obj_align(label_subtitle, LV_ALIGN_CENTER, 0, 20);
    
    /****** Wait screen (Only a title in center) ******/
	// Create a label for the authentication screen
	label_title_center = lv_label_create(parent);
    lv_obj_add_style(label_title_center, big_font_pointer, LV_PART_MAIN);
    lv_label_set_text_static(label_title_center, "Authentification");
    lv_obj_align(label_title_center, LV_ALIGN_CENTER, 0, 0);

    /****** Session screen ******/
    // Create a label for the session name
    label_Name = lv_label_create(parent);
    lv_obj_add_style(label_Name, mid_font_pointer, LV_PART_MAIN);
    lv_label_set_text(label_Name, "Nom de l'utilisateur");
    lv_obj_align(label_Name, LV_ALIGN_LEFT_MID, 0, -40);

	// Create a label for the phone number
    label_Phone = lv_label_create(parent);
    lv_obj_add_style(label_Phone, small_font_pointer, LV_PART_MAIN);
    lv_label_set_text(label_Phone, "Téléphone:\n+41 00 000 00 00");
    lv_obj_align(label_Phone, LV_ALIGN_LEFT_MID, 0, 0);

	// Create a label for the Time remaining
    label_TimeRemaining = lv_label_create(parent);
    lv_obj_add_style(label_TimeRemaining, small_font_pointer, LV_PART_MAIN);
    lv_label_set_text(label_TimeRemaining, "Temps restant (hh:mm): 01:23");
    lv_obj_align(label_TimeRemaining, LV_ALIGN_LEFT_MID, 0, 40);

    // Create a label for the lock status (high left)
    label_lock_status = lv_label_create(parent);
    lv_obj_add_style(label_lock_status, small_font_pointer, LV_PART_MAIN);
    lv_label_set_text(label_lock_status, LV_SYMBOL_PAUSE); // LV_SYMBOL_PAUSE or LV_SYMBOL_PLAY
    lv_obj_align(label_lock_status, LV_ALIGN_TOP_RIGHT, 0, 0);

	// Create a label for the right button (up)
    label_btn_right_up = lv_label_create(parent);
    lv_obj_add_style(label_btn_right_up, small_font_pointer, LV_PART_MAIN);
    lv_label_set_text(label_btn_right_up, LV_SYMBOL_PLUS); // LV_SYMBOL_EYE_CLOSE
    lv_obj_align(label_btn_right_up, LV_ALIGN_RIGHT_MID, -4, -30);

    // Create a label for the right button (down)
    label_btn_right_down = lv_label_create(parent);
    lv_obj_add_style(label_btn_right_down, small_font_pointer, LV_PART_MAIN);
    lv_label_set_text(label_btn_right_down, LV_SYMBOL_MINUS); // LV_SYMBOL_EYE_OPEN
    lv_obj_align(label_btn_right_down, LV_ALIGN_RIGHT_MID, -4, 30);

    // Create a label and a style for the line
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 2);
    lv_style_set_line_color(&style_line, lv_color_black());
    lv_style_set_line_rounded(&style_line, true);
    label_btn_line = lv_line_create(parent);
    lv_line_set_points(label_btn_line, line_points, 4);
    lv_obj_add_style(label_btn_line, &style_line, 0);
    lv_obj_align(label_btn_line, LV_ALIGN_TOP_RIGHT, 0, 0);


	// Show the label begin only and hide the others
	lv_obj_add_flag(label_title_center, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_Name, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_Phone, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_TimeRemaining, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_lock_status, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_btn_right_up, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_btn_right_down, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_btn_line, LV_OBJ_FLAG_HIDDEN);

	lv_task_handler(); // Handle the LVGL tasks
	k_sleep(K_SECONDS(3)); // Wait for the display to be refreshed
}


// Function to refresh the display with the new information
void refresh_info_display(uint8_t state, uint64_t time_remaining, bool technical_user, SessionContext_t *session_context) {
    char my_text[40]; // buffer to hold the text
    // Hide all the labels 
    LOG_DBG("Refreshing the display");
    lv_obj_add_flag(label_title_center, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_title_high, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_subtitle, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_Name, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_Phone, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_TimeRemaining, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_lock_status, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_btn_right_up, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_btn_right_down, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_btn_line, LV_OBJ_FLAG_HIDDEN);

    // Show the label according to the state
    switch(state){
        case USER_INTERFACE_SLEEP:
            lv_obj_clear_flag(label_title_high, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(label_subtitle, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(label_title_high, "Système en veille");
            lv_label_set_text(label_subtitle, "              Le système est en veille\nAppuyer sur un bouton pour activer");
            break;

        case USER_INTERFACE_AUTHENTICATION:
            lv_obj_clear_flag(label_title_center, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text_static(label_title_center, "Authentification");
            break;
        case USER_INTERFACE_ACCESS_DENIED:
            lv_obj_clear_flag(label_title_center, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text_static(label_title_center, "Accès refusé");
            break;

        case USER_INTERFACE_ACCESS_GRANTED:
            lv_obj_clear_flag(label_title_center, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text_static(label_title_center, "Accès autorisé");
            break;

        case USER_INTERFACE_SESSION_ACTIF:
            lv_obj_clear_flag(label_Name, LV_OBJ_FLAG_HIDDEN);
            bytecpy(my_text, session_context->user_name, session_context->user_name_size);
            my_text[session_context->user_name_size] = '\0'; // add the end of string
            lv_label_set_text_fmt(label_Name, "%s", my_text);
            lv_obj_clear_flag(label_Phone, LV_OBJ_FLAG_HIDDEN);
            bytecpy(my_text, session_context->phone_number, 16);
            my_text[16] = '\0'; // add the end of string
            lv_label_set_text_fmt(label_Phone, "Téléphone:\n%s", my_text);
            lv_obj_clear_flag(label_TimeRemaining, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(label_lock_status, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(label_btn_right_up, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(label_btn_right_down, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(label_lock_status, LV_SYMBOL_PLAY);
            if(technical_user) {
            lv_label_set_text_fmt(label_TimeRemaining, "Mode de Maintenance");
                lv_label_set_text(label_btn_right_up, LV_SYMBOL_EYE_OPEN);
                lv_label_set_text(label_btn_right_down, LV_SYMBOL_EYE_CLOSE);
            } else {
            lv_label_set_text_fmt(label_TimeRemaining, "Temps restant(hh:mm): %02d:%02d", (uint32_t)(time_remaining / 3600000), (uint32_t)(time_remaining/60000) % 60);
                lv_label_set_text(label_btn_right_up, LV_SYMBOL_PLUS);
                lv_label_set_text(label_btn_right_down, LV_SYMBOL_MINUS);
            }
            lv_obj_clear_flag(label_btn_line, LV_OBJ_FLAG_HIDDEN);
            break;

        case USER_INTERFACE_SESSION_SLEEP:
            lv_obj_clear_flag(label_Name, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(label_Phone, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(label_TimeRemaining, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text_fmt(label_TimeRemaining, "Temps restant(hh:mm): %02d:%02d ", (uint32_t)(time_remaining / 3600000), (uint32_t)(time_remaining/60000) % 60);
            lv_obj_clear_flag(label_lock_status, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(label_lock_status, LV_SYMBOL_PAUSE);
            break;

        case USER_INTERFACE_SESSION_END:
            lv_obj_clear_flag(label_title_center, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text_static(label_title_center, "Session terminée");
            break;

        case USER_INTERFACE_MAINTENANCE_LOCK:
            lv_obj_clear_flag(label_title_center, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(label_title_center, "Maintenance");
            break;
        default:
            break;
    }
	
	lv_task_handler(); // Handle the LVGL tasks
    LOG_DBG("Display refreshed");
}

/*! authentication_process
 *  @brief This function is used to check the authentication process.
 *  @param session_context The session context
 *  @return The state of the authentication process (1 if authenticated, 2 if maintenance user 3, if same user as actual session, 0 otherwise)
*/
int authentication_process(SessionContext_t *session_context, uint8_t state) {
    int ret = 0;
    UserID_t user;
    uint8_t DESFire_ID[8];
    user.user_name_size = 17;
    // chech the NFC card
    nfc_rf_field_control(clrc663, true); // Enable RF field
    k_sleep(K_MSEC(300)); // Wait for the nf field to be enabled and stable
    ret = read_card_secret(DESFire_ID);
    LOG_HEXDUMP_INF(DESFire_ID, 8, "DESFire ID : ");
    nfc_rf_field_control(clrc663, false); // Disable RF field
    ret = users_search_by_defire_id(DESFire_ID, &user);
    if (ret < 0) {
        return 0;
    }
    if (ret == 1) {
        LOG_INF("User found");
        LOG_INF("User ID: %d", user.user_id_number);
        LOG_INF("User name: %s", user.user_name);
        LOG_INF("Phone number: %s", user.phone_number);
        switch(state)
        { 
            case USER_INTERFACE_AUTHENTICATION:
                session_context->user_id = user.user_id_number;
                session_context->user_type = user.user_type;
                session_context->user_name_size = MIN(user.user_name_size + 1, 25);
                bytecpy(session_context->user_name, user.user_name, session_context->user_name_size);
                bytecpy(session_context->phone_number, user.phone_number, 16);
                bytecpy(session_context->desfire_id, user.desfire_id, 8);
                ret = 1;
                break;
            
            case USER_INTERFACE_MAINTENANCE_LOCK:
                // direct access to the maintenance user
            
            case USER_INTERFACE_SESSION_SLEEP:
                if(session_context->user_id == user.user_id_number) { // Check if the user is the same as the actual session
                    ret = 2;
                } else if (user.user_type == 1) { // Maintenance user force the session
                    session_context->user_id = user.user_id_number;
                    session_context->user_type = user.user_type;
                    session_context->user_name_size = MIN(user.user_name_size, 18);
                    bytecpy(session_context->user_name, user.user_name, session_context->user_name_size);
                    bytecpy(session_context->phone_number, user.phone_number, 16);
                    bytecpy(session_context->desfire_id, user.desfire_id, 8);
                    ret = 3;
                } else {
                    ret = 1;
                }
                break;

            default:
                ret = 0;
                LOG_WRN("State not found");
                break;
        }
    } else {
        LOG_WRN("User not found");
    }
   return ret;
}

void add_new_log(SessionContext_t *session_context, uint8_t state) {
    // Add a new log in the flash memory
    Log_t new_log;
    uint32_t time_now = (uint32_t)(k_uptime_get()/1000) + time_offset;
    new_log.user_id = session_context->user_id;
    new_log.timestamp = time_now;
    LOG_INF("Time: %d", time_now);
    switch (state)
    {
        case USER_INTERFACE_ACCESS_GRANTED:
            new_log.log_type = LOG_ACTION_ACCESS_GRANTED;
            break;

        case USER_INTERFACE_SESSION_ACTIF:
            new_log.log_type = LOG_ACTION_ACTIVATE;
            break;

        case USER_INTERFACE_SESSION_SLEEP:
            new_log.log_type = LOG_ACTION_SLEEP;
            break;

        case USER_INTERFACE_SESSION_END:
            new_log.log_type = LOG_ACTION_DISABLE;
            break;

        case USER_INTERFACE_MAINTENANCE_LOCK:
            new_log.log_type = LOG_ACTION_LOCK;
            break;
        
        default:
            return;
    }
    // Add the log in the log queue
    k_msgq_put(&logs_add_queue, &new_log, K_NO_WAIT);
    LOG_INF("New log added");
}

/*! userInterface_init initializes the user interface
 *  @brief This function is the user interface thread.
*/
void userInterface_thread(void)
{
    int ret;
    char btn;
    uint64_t now = k_uptime_get();
    uint8_t actual_state = USER_INTERFACE_SLEEP;
    uint8_t old_state = USER_INTERFACE_AUTHENTICATION;
    uint8_t minutes = 0;
    bool technical_user = false;
    bool update_display = true;
    uint64_t last_loop = 0;
    uint64_t time_remaining = 0;
    uint64_t Next_screen_lock = 0;
    SessionContext_t local_session_context;

	if (clrc663 == NULL) {
		LOG_ERR("CLRC663 not found");
		return;
	}

	if (!device_is_ready(clrc663)) {
		LOG_ERR("Reader not ready");
		return;
	}
	init_defireApp();

	if (display_dev == NULL) {
		LOG_ERR("Display not found");
		return;
	}

	if (!device_is_ready(display_dev)) {
		LOG_ERR("Display not ready");
		return;
	}

    // Initialize the LittlevGL library
    lv_init();
    // Initialize the display
    display_init();
    LOG_DBG("Display initialized");

    // Init the buttons
    // Configure the buttons
	ret = gpio_pin_configure_dt(&button0, GPIO_INPUT);
	if (ret != 0) {
        LOG_ERR("Failed to configure the button0");
		return;
	}
	ret = gpio_pin_interrupt_configure_dt(&button0,
					      GPIO_INT_EDGE_FALLING);
	if (ret != 0) {
        LOG_ERR("Failed to configure the button0 interrupt");
		return;
	}
	ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
	if (ret != 0) {
        LOG_ERR("Failed to configure the button1");
		return;
	}
	ret = gpio_pin_interrupt_configure_dt(&button1,
					      GPIO_INT_EDGE_FALLING);
	if (ret != 0) {
        LOG_ERR("Failed to configure the button1 interrupt");
		return;
	}
    
	if (led.port && !gpio_is_ready_dt(&led)) {
		LOG_ERR("Error %d: LED device %s is not ready; ignoring it\n",
		       ret, led.port->name);
		led.port = NULL;
	}
	if (led.port) {
		ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
		if (ret != 0) {
			LOG_ERR("Error %d: failed to configure LED device %s pin %d\n",
			       ret, led.port->name, led.pin);
			led.port = NULL;
		} else {
			LOG_INF("Set up LED at %s pin %d\n", led.port->name, led.pin);
		}
	}
    gpio_init_callback(&button0_cb_data, button_pressed, BIT(button0.pin));
    gpio_add_callback(button0.port, &button0_cb_data);
    gpio_init_callback(&button1_cb_data, button_pressed, BIT(button1.pin));
    gpio_add_callback(button1.port, &button1_cb_data);
    gpio_pin_set_dt(&led, false); // Turn off the device

    // Initialize the information display
	k_sleep(K_SECONDS(3)); // Wait for the display to be refreshed
    create_interface(lv_scr_act());
    LOG_DBG("Interface created");
	k_sleep(K_SECONDS(3)); // Wait for the display to be refreshed

    while(1) {
        LOG_INF("User interface running");
        now = k_uptime_get(); // Get the current time

        // Check the time remaining
        if (time_remaining > 0 && !technical_user) {
            time_remaining -= (uint32_t)((now - last_loop));
            if (time_remaining < 0) {
                time_remaining = 0;
            }
            if (time_remaining > (TIME_MAX_SESSION * 1000)) { // overflow protection
                time_remaining = 0;
            }
            if ((time_remaining/60000) % 60 != minutes) { // update the display every minutes
                minutes = (time_remaining/60000) % 60;
                update_display = true;
            }
        }
        last_loop = now;

        // Check the state
        switch(actual_state) {
            case USER_INTERFACE_SLEEP:
                LOG_INF("USER_INTERFACE_SLEEP");
#if relay_inverted
                gpio_pin_set_dt(&led, true); // Turn off the device
#else
                gpio_pin_set_dt(&led, false); // Turn on the device
#endif
                if (flag_btn) {
                    actual_state = USER_INTERFACE_AUTHENTICATION;
                    flag_btn = false;
                }
                break;

            case USER_INTERFACE_AUTHENTICATION:
                LOG_INF("USER_INTERFACE_AUTHENTICATION");
                //add_new_log(&local_session_context, USER_INTERFACE_AUTHENTICATION);
                if(authentication_process(&local_session_context, actual_state) > 0) {
                    technical_user = (local_session_context.user_type == 1);
                    actual_state = USER_INTERFACE_ACCESS_GRANTED;
                } else {
                    actual_state = USER_INTERFACE_ACCESS_DENIED;
                }
                break;

            case USER_INTERFACE_ACCESS_DENIED:
                LOG_INF("USER_INTERFACE_ACCESS_DENIED");
                actual_state = USER_INTERFACE_SLEEP;
                break;

            case USER_INTERFACE_ACCESS_GRANTED:
                LOG_INF("USER_INTERFACE_ACCESS_GRANTED");
                //add_new_log(&local_session_context, USER_INTERFACE_ACCESS_GRANTED);
                                
#if relay_inverted
                gpio_pin_set_dt(&led, false); // Turn on the device
#else
                gpio_pin_set_dt(&led, true); // Turn on the device
#endif
                actual_state = USER_INTERFACE_SESSION_ACTIF;
                if (technical_user) {
                    time_remaining = TIME_MAX_SESSION * 1000 + 1;
                } else {
                    time_remaining = TIME_ADD_OR_MINUS * 1000;
                }
                Next_screen_lock = now + SCREEN_LOCK_TIME * 1000;
                break;

            case USER_INTERFACE_SESSION_ACTIF:
                LOG_INF("USER_INTERFACE_SESSION_ACTIF");
                if (k_uptime_get() > Next_screen_lock) {
                    LOG_INF("Screen lock");
                    if (technical_user) {
                        actual_state = USER_INTERFACE_MAINTENANCE_LOCK;
                    } else {
                        actual_state = USER_INTERFACE_SESSION_SLEEP;
                    }
                    break;
                }

                if (time_remaining == 0) {
                    LOG_INF("Time remaining 0");
                    actual_state = USER_INTERFACE_SESSION_END;
                    break;
                }

                if (flag_btn) {
                    update_display = true;
                    Next_screen_lock = now + SCREEN_LOCK_TIME * 1000;
                    if(k_msgq_get(&both_queue, &btn, K_NO_WAIT) == 0) {
                        LOG_INF("Both buttons pressed");
                        //clear the queue
                        while(k_msgq_get(&add_queue, &btn, K_NO_WAIT) == 0) {}
                        while(k_msgq_get(&minus_queue, &btn, K_NO_WAIT) == 0) {}

                        // lock interface
                        k_sleep(K_SECONDS(2));

                        // Hold of boutons
                        if((gpio_pin_get_dt(&button0) | gpio_pin_get_dt(&button1))) {
                            LOG_INF("Both buttons hold, %d, %d", gpio_pin_get_dt(&button0), gpio_pin_get_dt(&button1));
                            technical_user = 0;
                            time_remaining = 0;
                            actual_state = USER_INTERFACE_SESSION_END;
                            break;
                        } else {
                            if (technical_user) {
                                actual_state = USER_INTERFACE_MAINTENANCE_LOCK;
                            } else {
                                actual_state = USER_INTERFACE_SESSION_SLEEP;
                            }
                        }
                    }

                    if (technical_user) {
                        while(k_msgq_get(&add_queue, &btn, K_NO_WAIT) == 0) {
                            time_remaining = TIME_MAX_SESSION * 1000;
                        }

                        while(k_msgq_get(&minus_queue, &btn, K_NO_WAIT) == 0) {
                            time_remaining = 0;
                            actual_state = USER_INTERFACE_SESSION_END;
                        }
                    } else {
                        while(k_msgq_get(&add_queue, &btn, K_NO_WAIT) == 0) {
                            LOG_INF("Add button pressed");
                            time_remaining += TIME_ADD_OR_MINUS * 1000;
                            if (time_remaining > TIME_MAX_SESSION * 1000) {
                                time_remaining = TIME_MAX_SESSION * 1000;
                            }
                        }

                        while(k_msgq_get(&minus_queue, &btn, K_NO_WAIT) == 0) {
                            LOG_INF("Minus button pressed");
                            time_remaining -= TIME_ADD_OR_MINUS * 1000;
                            if ((time_remaining > TIME_MAX_SESSION * 1000) || (time_remaining < 60000)) {
                                time_remaining = 60000; // 1 minutes
                            }
                        }
                    }
                    flag_btn = false;
                }
                break;

            case USER_INTERFACE_SESSION_SLEEP:
                if (time_remaining == 0) {
                    actual_state = USER_INTERFACE_SESSION_END;
                }

                if (flag_btn) {
                    if(authentication_process(&local_session_context, actual_state) > 1) { // check if the user is the same user or a maintenance user
                        Next_screen_lock = now + SCREEN_LOCK_TIME * 1000;
                        actual_state = USER_INTERFACE_SESSION_ACTIF;
                        // clear the queues
                        k_msgq_purge(&add_queue);
                        k_msgq_purge(&minus_queue);
                        k_msgq_purge(&both_queue);
                    }
                    flag_btn = false;
                }
                LOG_INF("USER_INTERFACE_SESSION_SLEEP");
                break;

            case USER_INTERFACE_SESSION_END:
                //add_new_log(&local_session_context, USER_INTERFACE_SESSION_END);
                
#if relay_inverted
                gpio_pin_set_dt(&led, true); // Turn off the device
#else
                gpio_pin_set_dt(&led, false); // Turn off the device
#endif
                actual_state = USER_INTERFACE_SLEEP;
                LOG_INF("USER_INTERFACE_SESSION_END");
                time_remaining = 0;
                break;

            case USER_INTERFACE_MAINTENANCE_LOCK:
                LOG_INF("USER_INTERFACE_MAINTENANCE_LOCK");
                if (flag_btn) {
                    LOG_INF("Button pressed => Authentication");
                    if(authentication_process(&local_session_context, actual_state) > 1) { // check if the user is the same user or a maintenance user
                        LOG_INF("Authentication : Acces granted");
                        Next_screen_lock = k_uptime_get() + SCREEN_LOCK_TIME * 1000;
                        actual_state = USER_INTERFACE_SESSION_ACTIF;
                        // clear the queues
                        k_msgq_purge(&add_queue);
                        k_msgq_purge(&minus_queue);
                        k_msgq_purge(&both_queue);
                    } else {
                        LOG_INF("Authentication : Acces denied");
                    }
                    flag_btn = false;
                }
                break;

            default:
                break;
        }

        // Update the display
        if(actual_state != old_state || update_display) {
            refresh_info_display(actual_state, time_remaining, technical_user, &local_session_context);
            add_new_log(&local_session_context, actual_state);
            old_state = actual_state;
            update_display = false;
        }
        k_sleep(K_SECONDS(3));
    }
}

/*! userInterface_thread_init initializes the user interface thread
 *  @brief This function is important to initialize the user interface thread.
*/
void userInterface_thread_init(void){
    // Create the user interface thread
    k_thread_create(&userInterfaceThread, USERINTERFACE_STACK, USERINTERFACE_STACK_SIZE, 
            (k_thread_entry_t)userInterface_thread, NULL, NULL, NULL, USERINTERFACE_PRIORITY, 0, K_NO_WAIT);

    k_thread_name_set(&userInterfaceThread, "UserInterface");
    k_thread_start(&userInterfaceThread);
}
