#commence save-data set-up
world_name = "world1"
level_name = "level2"
map_name = "main"

engine.update_world_text("1")
engine.update_level_text("2")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

def myla_introduces_programming_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_dialogue("Myla: This is as good place to start as any!", callback = callback),
        lambda callback: engine.show_dialogue("Just a quick question though, the PyScripter uses a programming language called Python.", callback = callback),
        lambda callback: engine.show_dialogue("Have you ever programmed in Python before? (TODO ASK QUESTION AND GIVE APPROPRIATE RESPONSE)", callback = callback),
        lambda callback: engine.show_dialogue("The first thing we are going to start with is printing to the PyConsole in the bottom right-hand corner of the screen.", callback = callback),
        lambda callback: engine.show_dialogue("Printing is just the word programmers use for displaying any kind of text! Not just sending things to a printer!", callback = callback),
        lambda callback: engine.show_external_script(
            external_dialogue = "Can you please print my name? If I was printing your name I would type print(\"" + engine.get_player_name() + "\"). Click on 'Give Script' when you are done!",
            script_init = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.clear_scripter(callback = callback),
                #lambda callback: engine.insert_to_scripter("print()", callback = callback),
            ]),
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Awesome!", callback = callback), #TODO: add all the possible reactions to what the player can give Myla
            ], callback = callback),
            cancel_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Oh, you've forgotten my name already? I'm Myla! Try giving it another go.", callback = callback),
            ], callback = callback),
        ),
        lambda callback: engine.show_dialogue("print() is an example of something called a function.", callback = callback),
        lambda callback: engine.show_dialogue("A function takes what you give it in between the brackets, and does something with it!", callback = callback),
        lambda callback: engine.show_dialogue("The thing you give to a function is called an argument.", callback = callback),
        lambda callback: engine.show_dialogue("If it's a lot to take in, don't worry, we will come back to all of this again and again in many different ways.", callback = callback),
        lambda callback: engine.show_dialogue("In summary, you just used a function called print, and gave it my name as an argument.", callback = callback),
        lambda callback: engine.show_dialogue("Anyway, let's get moving, I will teach you some more in a bit.", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

myla_introduces_programming.player_walked_on = myla_introduces_programming_action

def myla_introduces_variables_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: engine.show_dialogue("Myla: Now we are going to start getting fancy!", callback = callback),
        lambda callback: engine.show_dialogue("I'm going to introduce you to variables.", callback = callback),
        lambda callback: engine.show_dialogue("A variable is something you can give a value of some kind, and then do with it what you want.", callback = callback),
        lambda callback: engine.show_dialogue("The reason it's called a variable, is because the value it contains can vary!", callback = callback),
        lambda callback: engine.show_dialogue("I'm going to give you a script.", callback = callback),
        lambda callback: engine.show_external_script(
            external_dialogue = "Try printing your name this time! However, do it by replacing my name with yours, don't put give it straight to print!",
            script_init = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.clear_scripter(callback = callback),
                lambda callback: engine.insert_to_scripter("name = \"Myla\"\n", callback = callback),
                lambda callback: engine.insert_to_scripter("print(name)", callback = callback), #TODO, add all the reactions to what the player can give Myla
            ]),
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Awesome! In this case, 'name' was the variable you used.", callback = callback),
            ], callback = callback),
            cancel_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Oh, you've forgotten your own name already? Hahaha, you're called " + engine.get_player_name() + " silly.", callback = callback),
            ], callback = callback)
        ),
        lambda callback: engine.show_dialogue("Well done " + engine.get_player_name() + "!", callback = callback),
        lambda callback: engine.show_dialogue("Variables are really useful for making your program remember things, they will really come in handy for when we do maths.", callback = callback),
        lambda callback: engine.show_dialogue("Don't worry if you don't like doing maths much, the thing that's useful about programming is that the PyScripter does all the maths for you!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

myla_introduces_variables.player_walked_on = myla_introduces_variables_action

def toll_listener_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: toll_person.face_east(callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_dialogue("Stop there! I'm afraid I can't let you through, I'm tolling this bridge and the PyRunner I used to charge people has broken because of the lightning.", callback = callback),
        lambda callback: engine.show_dialogue("So no one is getting through until it is fixed.", callback = callback),
        lambda callback: player_one.face_south(callback = callback),
        lambda callback: engine.show_dialogue("Myla: Unlike your PyScripter, the people around here have PyRunners which can only run scripts but can't change them.", callback = callback),
        lambda callback: engine.show_dialogue("You could use your PyScripter to fix the man's script for him. There was some lightning in the storm last night which broke a lot of PyRunners.", callback = callback),
        lambda callback: engine.show_dialogue("In fact, I don't think he has a license to toll people anyway, so why don't you make it so that it always says people have payed.", callback = callback),
        lambda callback: myla.stop_follow(callback = callback),
        lambda callback: myla.face_west(callback = callback),
        lambda callback: engine.show_dialogue("Myla: Oi, toll man! My friend here can fix your PyRunner for you!", callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_external_script(
            external_dialogue = "Toll Man: If you do this, I still won't let you through until you have payed me at least £1000.",
            script_init = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.clear_scripter(callback = callback),
                lambda callback: engine.insert_to_scripter("amount_payed = \"£0\"\n", callback = callback),
                lambda callback: engine.insert_to_scripter("print(amount_payed)", callback = callback), #TODO, add all the reactions to what the player can give Myla
            ]),
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Toll Man: Hahaha, you fixed my PyRunner and payed me the money, I can't believe I get away with this!", callback = callback),
                lambda callback: player_one.face_south(callback = callback),
                lambda callback: myla.face_north(callback = callback),
                lambda callback: engine.show_dialogue("Myla: *the jokes on him, mwahahahaha*", callback = callback),
                lambda callback: player_one.face_west(callback = callback),
                lambda callback: myla.face_west(callback = callback),
                lambda callback: engine.show_dialogue("Toll Man: Oi, did you say anything?", callback = callback),
                lambda callback: player_one.face_south(callback = callback),
                lambda callback: engine.show_dialogue("Myla: What, me? No not at all.", callback = callback),
                lambda callback: myla.face_north(callback = callback),
                lambda callback: engine.show_dialogue("Come on " + engine.get_player_name() + " let's get out of here!", callback = callback),
            ], callback = callback),
            cancel_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.face_south(callback = callback),
                lambda callback: engine.show_dialogue("Myla: Just try typing amount_payed = \"£1000\".", callback = callback),
            ], callback = callback)
        ),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

toll_listener.player_walked_on = toll_listener_action

#setting the player's starting position
engine.run_callback_list_sequence([
    lambda callback: player_one.move_to(exit_to_world.get_position(), callback = callback),
    lambda callback: player_one.move_north(callback = callback),
])
myla_start_position = exit_to_world.get_position()
#end setting the player's starting position





def go_to_world(player_object):
    player_data.save_and_exit("/world1")

def go_to_bogs(player_object):
    player_data.complete_level_and_save()
    player_data.save_and_exit("/world1")

exit_to_world.player_walked_on = go_to_world
exit_to_bogs.player_walked_on = go_to_bogs

myla.set_solidity(False)
myla.move_to(myla_start_position)
myla.follow(player_one)

player_one.focus()
engine.play_music("world_1_jungle")
