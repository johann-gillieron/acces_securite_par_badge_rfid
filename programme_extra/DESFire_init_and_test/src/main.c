/*
 * Copyright (C) 2024 Johann Gilliéron
 * SPDX-License-Identifier: Apache-2.0
 */
#include <soc.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/nfc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include "fonts/font_poppins_14.h"
#include "fonts/font_poppins_28.h"

#include "desfire/DesfireApp.h"

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define UID_MAX_LEN 10
#define ATS_MAX_LEN 64
#define SAK_LEN 1
#define ATQA_LEN 2
#define DESFIRE_SECRET_LEN 8

// Thread defines
#define READER_THREAD_DELAY_MS 200
#define STACKSIZE 8096
#define PRIORITY 5
K_THREAD_STACK_DEFINE(reader_thread_stack, STACKSIZE);
struct k_thread reader_thread_id;

#if !DT_NODE_EXISTS(DT_NODELABEL(clrc663_0))
#error "whoops, node label clrc663_0 not found"
#endif

const struct device *clrc663;

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,{0});
static struct gpio_callback button_cb_data;
static bool flag_btn = false;

/*
 * The led0 devicetree alias is optional. If present, we'll use it
 * to turn on the LED whenever the button is pressed.
 */
static struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0});

// LVGL objects for the display
static lv_obj_t *label_title;
static lv_obj_t *label_begin;
static lv_obj_t *label_uid;
static lv_obj_t *label_sak;
static lv_obj_t *label_atqa;
static lv_obj_t *label_ats;
static lv_obj_t *label_secret;
static lv_obj_t *label_no_tag;
static lv_obj_t *label_error;
static lv_style_t big_font, small_font;
lv_style_t* big_font_pointer = &big_font;
lv_style_t* small_font_pointer = &small_font;

const struct device *display_dev;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
	LOG_INF("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
	flag_btn = true;
}

// Function to test the screen display
void display_init() {
	static lv_obj_t *info_value;
	static lv_obj_t *info_text;
	static lv_obj_t *info_label;
	static lv_obj_t *info_scr = NULL;
	lv_init();
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
	lv_obj_align(info_text, LV_ALIGN_CENTER, 0, 0);

	//Sublabel for informative text
	info_label = lv_label_create(info_scr);
	lv_obj_add_style(info_label, small_font_pointer, LV_PART_MAIN);
	lv_obj_align(info_label, LV_ALIGN_BOTTOM_MID, 0, -10);

	//Text	
	lv_label_set_text(info_value, "Johann Gilliéron");
	//Subtext
	lv_label_set_text(info_text, "Travail de Bachelor");
	//Subtext
	lv_label_set_text(info_label, " Électronique compacte pour  \naccès sécurité par badge RFID");

	lv_scr_load(info_scr);

	lv_task_handler(); // Handle the LVGL tasks
	k_sleep(K_SECONDS(4)); // Wait for the display to be refreshed	
}

// Function to create the information display
void create_info_display(lv_obj_t * parent) {
	// Container for the information
	lv_obj_clean(parent);

    // Create a label for the title
    label_title = lv_label_create(parent);
	lv_obj_add_style(label_title, big_font_pointer, LV_PART_MAIN);
    lv_label_set_text_static(label_title, "NFC Info");
    lv_obj_align(label_title, LV_ALIGN_CENTER, 0, -50);

	// Create a label for the begin
	label_begin = lv_label_create(parent);
	lv_label_set_long_mode(label_begin, LV_LABEL_LONG_WRAP);
	lv_obj_set_width(label_begin, 250);
	lv_obj_add_style(label_begin, small_font_pointer, LV_PART_MAIN);
	lv_label_set_text_static(label_begin, "Ready to scan a NFC tag\nPlease press the button 1 to start the scan");
	lv_obj_set_style_text_align(label_begin, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label_begin, LV_ALIGN_CENTER, 0, 0);

	// Create a label for the error
	label_error = lv_label_create(parent);
	lv_obj_add_style(label_error, small_font_pointer, LV_PART_MAIN);
	lv_label_set_text_static(label_error, "Error in the NFC communication");
	lv_obj_align(label_error, LV_ALIGN_CENTER, 0, 0);

    // Create a label for the uid
    label_uid = lv_label_create(parent);
	lv_obj_add_style(label_uid, small_font_pointer, LV_PART_MAIN);
    lv_label_set_text(label_uid, "UID: ");
    lv_obj_align(label_uid, LV_ALIGN_LEFT_MID, 0, -30);

	// Create a label for the SAK
    label_sak = lv_label_create(parent);
	lv_obj_add_style(label_sak, small_font_pointer, LV_PART_MAIN);
    lv_label_set_text(label_sak, "SAK: ");
    lv_obj_align(label_sak, LV_ALIGN_LEFT_MID, 0, -10);

	// Create a label for the ATQA
    label_atqa = lv_label_create(parent);
	lv_obj_add_style(label_atqa, small_font_pointer, LV_PART_MAIN);
    lv_label_set_text(label_atqa, "ATQA: ");
    lv_obj_align(label_atqa, LV_ALIGN_LEFT_MID, 0, 10);

	// Create a label for the ATS
	label_ats = lv_label_create(parent);
	lv_obj_add_style(label_ats, small_font_pointer, LV_PART_MAIN);
	lv_label_set_text(label_ats, "ATS: ");
	lv_obj_align(label_ats, LV_ALIGN_LEFT_MID, 0, 30);

	label_secret = lv_label_create(parent);
	lv_obj_add_style(label_secret, small_font_pointer, LV_PART_MAIN);
	lv_label_set_text(label_secret, "Secret: ");
	lv_obj_align(label_secret, LV_ALIGN_LEFT_MID, 0, 50);

	// Create a label for the case where no tag is detected
	label_no_tag = lv_label_create(parent);
	lv_obj_add_style(label_no_tag, big_font_pointer, LV_PART_MAIN);
	lv_label_set_text_static(label_no_tag, "No NFC detected");
	lv_obj_align(label_no_tag, LV_ALIGN_CENTER, 0, 0);
	
	// Show the label begin only and hide the others
	lv_obj_add_flag(label_uid, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(label_sak, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(label_atqa, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(label_ats, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(label_secret, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(label_no_tag, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(label_error, LV_OBJ_FLAG_HIDDEN);

	lv_task_handler(); // Handle the LVGL tasks
	k_sleep(K_SECONDS(3)); // Wait for the display to be refreshed
}

// Function to refresh the display with the new information
void refresh_info_display(uint8_t *UID, uint8_t UID_length, uint8_t *SAK, uint8_t *ATQA, uint8_t *ATS, uint8_t ATS_length, char *secret) {
	lv_obj_add_flag(label_begin, LV_OBJ_FLAG_HIDDEN);
	if(UID_length <= 0){
		// Hide the labels with the information
		lv_obj_add_flag(label_uid, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(label_sak, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(label_atqa, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(label_ats, LV_OBJ_FLAG_HIDDEN);
		// Show the label for the case where no tag is detected
		lv_obj_clear_flag(label_no_tag, LV_OBJ_FLAG_HIDDEN);
	} else {
		// Update the information in the labels
		char uid_str[UID_MAX_LEN * 3]; // Chaîne de caractères pour l'UID
		char ats_str[ATS_MAX_LEN * 3]; // Chaîne de caractères pour l'ATS
		char secret_str[DESFIRE_SECRET_LEN * 3 + 1]; // Chaîne de caractères pour le secret

		// Convert the UID to a hexadecimal string
		for (int i = 0; i < UID_length; i++) {
			LOG_INF("UID[%d] = %02x", i, UID[i]);
			snprintf(uid_str + i * 3, 4, "%02x:", UID[i]);
			LOG_INF("UID_str[%d] = %s", i, uid_str);
		}
		uid_str[UID_length * 3 - 1] = '\0';
		LOG_INF("UID_str final = %s", uid_str);

		// Convert the ATS to a hexadecimal string
		for (int i = 0; i < ATS_length; i++) {
			snprintf(ats_str + i * 3, 4, "%02x ", ATS[i]);
		}
		ats_str[ATS_length * 3 - 1] = '\0';
		LOG_INF("UID_str final = %s", uid_str);

		// Convert the secret to a hexadecimal string
		for (int i = 0; i < DESFIRE_SECRET_LEN; i++) {
			snprintf(secret_str + i * 3, 4, "%02x ", secret[i]);
		}
		secret_str[DESFIRE_SECRET_LEN * 3 - 1] = '\0';
		LOG_INF("UID_str final = %s", uid_str);

		// Update the information in the labels
		LOG_INF("UID: %s", uid_str);
		lv_label_set_text_fmt(label_uid, "UID: %s", uid_str);
		lv_label_set_text_fmt(label_sak, "SAK: %02x", SAK[0]);
		lv_label_set_text_fmt(label_atqa, "ATQA: %02x %02x", ATQA[1], ATQA[0]);
		if (ATS_length > 0) {
			lv_label_set_text_fmt(label_ats, "ATS: %s", ats_str);
			lv_label_set_text_fmt(label_secret, "Secret: %s", secret_str);
		} else {
			lv_label_set_text(label_ats, "Not ISO14443-4 compliant");
		}

		// Show the labels with the information
		lv_obj_clear_flag(label_uid, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(label_sak, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(label_atqa, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(label_ats, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(label_secret, LV_OBJ_FLAG_HIDDEN);
		// Hide the label for the case where no tag is detected
		lv_obj_add_flag(label_no_tag, LV_OBJ_FLAG_HIDDEN);
	}
	lv_task_handler(); // Handle the LVGL tasks
}

// Thread to handle the NFC reading by button press
void reader_thread(void) {
	LOG_INF("Thread reader started");
	static char secret[DESFIRE_SECRET_LEN];
	static uint8_t sak[SAK_LEN];
	static uint8_t *uid;
	static uint8_t *atqa;
	static uint8_t *ats;
	static uint8_t uid_length = 0;
	static uint8_t ats_length = 0;
	atqa = k_calloc(ATQA_LEN, sizeof(uint8_t));
	uid = k_calloc(UID_MAX_LEN, sizeof(uint8_t));
	ats = k_calloc(ATS_MAX_LEN, sizeof(uint8_t));
	
	while (true) {
		if (flag_btn) {
			LOG_INF("================  Button pressed  ================");
			if(read_nfc_card(uid, &uid_length, sak, atqa, ats, &ats_length))
			{
				LOG_INF("Card detected");
				LOG_HEXDUMP_INF(atqa, 2, "ATQA: ");
				LOG_HEXDUMP_INF(uid, uid_length, "UID: ");
				LOG_HEXDUMP_INF(sak, 1, "SAK: ");
				if (ats_length > 0) {
					LOG_HEXDUMP_INF(ats, ats_length, "ATS: ");
					if(!init_card(secret)) {
						LOG_ERR("Error in the DESFire card initialization");
					}
				} else {
					LOG_INF("No ATS : NFC tag not compliant with ISO14443-4");
				}
			}
			refresh_info_display(uid, uid_length, sak, atqa, ats, ats_length, secret);
			flag_btn = false;
		}
		k_sleep(K_MSEC(READER_THREAD_DELAY_MS));
	}
}

int main(void) {
	
	int ret;

	LOG_INF("Beginning of the CLRC663 driver test program ...");
	init_defireApp();
	// Initialize the CLRC663 connexion and check if the reader is ready
	clrc663 = DEVICE_DT_GET(DT_NODELABEL(clrc663_0));
	if (clrc663 == NULL) {
		LOG_ERR("CLRC663 not found");
		return 0;
	}

	if (!device_is_ready(clrc663)) {
		LOG_ERR("Reader not ready");
		return 0;
	}
	LOG_INF("Reader ready");

	// Initialize display device
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("E-paper initialize error");
		return 1;
	}
	LOG_INF("E-paper initialized");
	display_init();
	LOG_INF("display_init initialized");
	create_info_display(lv_scr_act());

	if (!gpio_is_ready_dt(&button)) {
		LOG_ERR("Error: button device %s is not ready\n",
		       button.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret != 0) {
		LOG_ERR("Error %d: failed to configure %s pin %d\n",
		       ret, button.port->name, button.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		LOG_ERR("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button.port->name, button.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);
	printk("Set up button at %s pin %d\n", button.port->name, button.pin);

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

	// create the reader thread
	k_thread_create(&reader_thread_id, reader_thread_stack,
		K_THREAD_STACK_SIZEOF(reader_thread_stack),
		reader_thread, 
		NULL, NULL, NULL,
		PRIORITY, 0, K_NO_WAIT);

	LOG_INF("End of the CLRC663 driver main program ...");
	return 0;
}