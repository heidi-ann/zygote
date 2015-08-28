//This is the config file for the game, it is a jsonnet file for ease of use.
{
	"game_save": import "../game/save.json",
	"game_settings": self.game_save.settings, //Import the game settings from the game json file. (Which python can directly read an write to)
	"special_methods": {
		"walk_on" : "b" //TODO explain this!!!!
	},
	"files": {
		"game_folder": "../game",
		"level_folder": self.game_folder+"/levels/",
		"main_menu": "/start_screen",
		//"level_location": "/test_world/test_level/test_one",
		//"level_location": "/test_world/yingischallenged/main",
		//"level_location": "/world_one_old/level_one/main",
		//"level_location": "/world_one_old/level_two/main",
		"level_location": "/start_screen",
		//"level_location": "/world_1",
		//"level_location" : "/world_1/intro",
		//"level_location" : "/world_1/level_1/rossumberg",
		//"level_location" : "/world_1/level_1/player_house",
		//"level_location" : "/world_1/level_1/road_one",
		//"level_location" : "/world_1/level_3/main",
		//"level_location" : "/world_1/level_4/main_a",
		//"level_location" : "/world_1/level_4/main_b",
		//"level_location" : "/world_1/level_5/maze",
		//"level_location" : "/world_1/level_6/vine_intro",
		//"level_location" : "/world_1/level_6/maze",
		//"level_location" : "/world_1/level_7/town_hall",
		//"level_location": "/tomland/one",
		//"level_location": "/menu",
		//"level_location" : "/world_2/level_1/main",
		//"level_location" : "world_8/treasure_matrix",	
		"player_scripts": self.game_folder + "/player_scripts",
		"object_location": self.game_folder + "/objects",
		"font_location": self.game_folder + "/fonts",
		"script_running_location": self.game_folder + "/script_running",
		"dialogue_font": self.font_location + "/Ubuntu-B.ttf",
		"game_save_location": self.game_folder + "/save.json",
		"initial_save_location": self.game_folder + "/initial_save.json"
	},

	//define layer properties
	"layers": {
		"special_layer_name": "SpecialLayer" //The name of the layer which provides special properties to tiles on the map.
	},

	//define constants for rendering sizes
	"scales": {
		
		//multiplying factors for converting between window sizes and gui scales
		"x_scale": 1.0/660.0,
		"y_scale": 1.0/285.0,		

		//for placing the buttons on top of the screen
		"left_x_offset": 0.00,
		"right_x_offset": 0.91,
		"bottom_y_offset": 0.00,
		"top_y_offset": 0.87,				

		//for placing the close button once the pause window is open
		"close_x_offset": 0.05,
		"close_y_offset": 0.80,

		//for placing the title of a menu
		"title_x_offset": 0.50,
		"title_y_offset": 0.90,
		"pyguide_title_x_offset": 0.25,
		"pyguide_title_y_offset": 0.9,

		//for pause options
		"pause_x_offset": 0.40,
		"pause_y_offset": 0.60,

		//for placing an item on the menu
		"menu_x_offset": 0.05,
		"menu_y_offset": 0.70,
		"menu_width": 0.15,
		"menu_height": 0.12,
		"menu_spacing": 0.15,	
		"menu_max": 5,		

		//the sizes of the notification box
		"notification_width": 1.00,
		"notification_height": 0.30,

		//the parameters for notification text
		"notification_text_width": 0.90,
		"notification_text_height": 0.40, 
		"notification_text_x": 0.03,
		"notification_text_y": 0.70,
		"notification_text_buffer": 250,

		//the parameters for notification button (next, back etc)
		"notification_button_width": 0.50,
		"notification_button_height": 1.00, 
		"notification_button_x": 0.90,
		"notification_button_y": 0.35,

		//the parameters for the option box which is to be displayed after the box is closed
		"option_max": 2,
		"option_width": 0.4,
		"option_height": 0.30,
		"option_x": 0.60,
		"option_y": 0.20,
		"option_button_width": 0.20,
		"option_button_height": 0.40,
		"option_button_x": 0.70,
		"option_button_y": 0.40,
		"option_button_text_x": 7.30,
		"option_button_text_y": 2.10,
		"option_button_spacing": 0.50,
		
		//the parameters for the py help text box 
		"py_help_width": 0.50,
		"py_help_height": 0.90, 
		"py_help_x": 0.45,
		"py_help_y": 0.05,

		//the parameters for the py guide commands on the menu 
		"py_help_item_width": 0.18,
		"py_help_item_height": 0.12, 
		"py_help_item_x": 0.02,
		"py_help_item_y": 0.72,
		"py_help_item_spacing": 0.08,
		"py_help_item_text_x": 0.30,
		"py_help_item_text_y": 0.60,

		//the parameters for the py help text which explains commands
		"py_help_text_width": 0.90,
		"py_help_text_height": 0.50, 
		"py_help_text_x": 0.05,
		"py_help_text_y": 0.80,
		"py_help_text_buffer": 400,

		//the parameters for the buttons accompanying the text box
		"py_help_button_width": 0.10,
		"py_help_button_height": 0.10, 
		"py_help_button_x": 0.80,
		"py_help_button_y": 0.15,
		"py_help_button_spacing": 0.70,

		//the parameters for the buttons used to scroll through pages in the pyguide/bag
		"menu_move_width": 0.04,
		"menu_move_height": 0.15,
		"menu_move_x": 0.40,
		"menu_move_y": 0.15,
		"menu_move_spacing": 0.35,
		"menu_page_display_x": 0.20,
		"menu_page_display_y": 0.15,
		
		//the sizes of the buttons on top of the screen
		"button_width": 0.08,
		"button_height": 0.12,

		//the distance between two buttons
		"horizontal_button_spacing": 0.08,
		"vertical_button_spacing": 0.10,

		//the maximum number of sprite heads displayed on the screen		
		"button_max": 5
	},

	//define the text to be displayed for different PyGuide apis
	
	// the commands are indexed, so that the guide can read them in by number
	// while putting them in here, use the following syntax-
	// "index": "pyguide api: description"
	
	"pyguide_apis": {
		"number": 2,
		"0": "move_east(): This moves the present character towards the right by one step.",
		"1": "print(args): This prints the argument args to the terminal (args must be of type string)",
		
	}
}
