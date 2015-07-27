#define GLM_FORCE_RADIANS
#include <fstream>
#include <glog/logging.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <chrono>
#include <functional>
#include <glm/vec2.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <ratio>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include "game_init.hpp"
#include "button.hpp"
#include "callback_state.hpp"
#include "challenge_data.hpp"
#include "config.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "filters.hpp"
#include "game_window.hpp"
#include "gui_manager.hpp"
#include "gui_window.hpp"
#include "input_manager.hpp"
#include "interpreter.hpp"
#include "keyboard_input_event.hpp"
#include "lifeline.hpp"
#include "map_viewer.hpp"
#include "mouse_cursor.hpp"
#include "mouse_input_event.hpp"
#include "mouse_state.hpp"
#include "notification_bar.hpp"

#ifdef USE_GLES
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"
#endif

#include "game_main.hpp"

using namespace std;

static std::mt19937 random_generator;


GameMain::GameMain(int &argc, char **argv):
    embedWindow(800, 600, argc, argv, this),
    interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize()),
    gui_manager(),
    callbackstate(),
    map_viewer(&embedWindow, &gui_manager),
    buttontype(Engine::get_game_typeface()),        //TODO : REMOVE THIS HACKY EDIT - done for the demo tomorrow
    buttonfont(Engine::get_game_font()),
    tile_identifier_text(&embedWindow, Engine::get_game_font(), false)
{
    LOG(INFO) << "Constructing GameMain..." << endl;

    map_path = ("../maps/start_screen.tmx");

    switch (argc)
    {
    default:
        std::cout << "Usage: " << argv[0] << " [EDITOR] [MAP]" << std::endl;
        return;
        // The lack of break statements is not an error!!!
    case 3:
        map_path = std::string(argv[2]);
    case 2:
        Engine::set_editor(argv[1]);
    case 1:
        break;
    }

    /// CREATE GLOBAL OBJECTS

    //Create the game embedWindow to present to the users
    embedWindow.use_context();
    Engine::set_game_window(&embedWindow);

    //Create the input manager
    input_manager = embedWindow.get_input_manager();

    Engine::set_map_viewer(&map_viewer);

    //    void (GUIManager::*mouse_callback_function) (MouseInputEvent) = &GUIManager::mouse_callback_function;

    stoptext = std::make_shared<Text>(&embedWindow, buttonfont, true);
    runtext = std::make_shared<Text>(&embedWindow, buttonfont, true);
    // referring to top left corner of text embedWindow
    //    stoptext.move(105, 240 + 20);
    //    runtext.move(5, 240 + 20);
    stoptext->set_text("Halt");
    runtext->set_text("Run");

    //Create the event manager
    em = EventManager::get_instance();

    sprite_window = std::make_shared<GUIWindow>();
    sprite_window->set_visible(false);
    run_button = std::make_shared<Button>();
    run_button->set_text(runtext);
    run_button->set_on_click([&] ()
    {
        LOG(ERROR) << "RUN";
        callbackstate.restart();
    });
    run_button->set_width(0.2f);
    run_button->set_height(0.2f);
    run_button->set_y_offset(0.8f);
    run_button->set_x_offset(0.0f);

    stop_button = std::make_shared<Button>();
    stop_button->set_text(stoptext);
    stop_button->set_on_click([&] ()
    {
        LOG(ERROR) << "STOP";
        callbackstate.stop();
    });
    stop_button->set_width(0.2f);
    stop_button->set_height(0.2f);
    stop_button->set_y_offset(0.67f);
    stop_button->set_x_offset(0.0f);

    gui_manager.set_root(sprite_window);

    notification_bar = new NotificationBar();

    Engine::set_notification_bar(notification_bar);
    //    SpriteSwitcher sprite_switcher;

    sprite_window->add(run_button);
    sprite_window->add(stop_button);

    // quick fix so buttons in correct location in initial embedWindow before gui_resize_func callback
    original_window_size = embedWindow.get_size();
    sprite_window->set_width_pixels(original_window_size.first);
    sprite_window->set_height_pixels(original_window_size.second);

    gui_manager.parse_components();

    //The GUI resize function
    gui_resize_func = [&] (GameWindow* game_window)
    {
        LOG(INFO) << "GUI resizing";
        auto window_size = (*game_window).get_size();
        sprite_window->set_width_pixels(window_size.first);
        sprite_window->set_height_pixels(window_size.second);
        gui_manager.parse_components();
    };
    gui_resize_lifeline = embedWindow.register_resize_handler(gui_resize_func);

    //The callbacks
    // WARNING: Fragile reference capture
    map_resize_lifeline = embedWindow.register_resize_handler([&] (GameWindow *)
    {
        map_viewer.resize();
    });

    stop_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("H")},
    [&] (KeyboardInputEvent)
    {
        callbackstate.stop();
    }
    ));

    restart_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("R")},
    [&] (KeyboardInputEvent)
    {
        callbackstate.restart();
    }
    ));

    editor_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("E")},
    [&] (KeyboardInputEvent)
    {
        auto id = Engine::get_map_viewer()->get_map_focus_object();
        auto active_player = ObjectManager::get_instance().get_object<Object>(id);
        if (!active_player)
        {
            return;
        }
        Engine::open_editor(active_player->get_name());
    }
    ));

    back_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("ESCAPE")},
    [&] (KeyboardInputEvent)
    {
        Engine::get_challenge()->event_finish.trigger(0);;
    }
    ));

    fast_start_ease_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY({"Left Shift", "Right Shift"})},
    [&] (KeyboardInputEvent)
    {
        start_time = std::chrono::steady_clock::now();
    }
    ));

    fast_ease_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, KEY({"Left Shift", "Right Shift"})},
    [&] (KeyboardInputEvent)
    {
        auto now(std::chrono::steady_clock::now());
        auto time_passed = now - start_time;

        float completion(time_passed / std::chrono::duration<float>(6.0f));
        completion = std::min(completion, 1.0f);

        // Using an easing function from the internetz:
        //
        //     start + (c⁵ - 5·c⁴ + 5·c³) change
        //
        float eased(1.0f + 511.0f * (
                        + 1.0f * completion * completion * completion * completion * completion
                        - 5.0f * completion * completion * completion * completion
                        + 5.0f * completion * completion * completion
                    ));

        EventManager::get_instance()->time.set_game_seconds_per_real_second(eased);
    }
    ));

    fast_finish_ease_callback = input_manager->register_keyboard_handler(filter(
    {KEY_RELEASE, KEY({"Left Shift", "Right Shift"})},
    [&] (KeyboardInputEvent)
    {
        EventManager::get_instance()->time.set_game_seconds_per_real_second(1.0);
    }
    ));


    up_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Up", "W"})},
    [&] (KeyboardInputEvent)
    {
        callbackstate.man_move(glm::ivec2( 0, 1));
    }
    ));

    down_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Down", "S"})},
    [&] (KeyboardInputEvent)
    {
        callbackstate.man_move(glm::ivec2( 0, -1));
    }
    ));

    right_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Right", "D"})},
    [&] (KeyboardInputEvent)
    {
        callbackstate.man_move(glm::ivec2( 1,  0));
    }
    ));

    left_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Left", "A"})},
    [&] (KeyboardInputEvent)
    {
        callbackstate.man_move(glm::ivec2(-1,  0));
    }
    ));

    monologue_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("M")},
    [&] (KeyboardInputEvent)
    {
        callbackstate.monologue();
    }
    ));

    mouse_button_lifeline = input_manager->register_mouse_handler(filter(
    {MOUSE_RELEASE},
    [&] (MouseInputEvent event)
    {
        gui_manager.mouse_callback_function(event);
    }));

    zoom_in_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, KEY("=")},
    [&] (KeyboardInputEvent)
    {
        Engine::set_global_scale(Engine::get_global_scale() * 1.01f);
    }
    ));

    zoom_out_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, KEY("-")},
    [&] (KeyboardInputEvent)
    {
        Engine::set_global_scale(Engine::get_global_scale() / 1.01f);
    }
    ));

    zoom_zero_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, MODIFIER({"Left Ctrl", "Right Ctrl"}), KEY("0")},
    [&] (KeyboardInputEvent)
    {
        Engine::set_global_scale(1.0f);
    }
    ));

    help_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, MODIFIER({"Left Shift", "Right Shift"}), KEY("/")},
    [&] (KeyboardInputEvent)
    {
        auto id(Engine::get_map_viewer()->get_map_focus_object());
        auto active_player(ObjectManager::get_instance().get_object<MapObject>(id));

        Engine::print_dialogue(
            active_player->get_name(),
            "placeholder string"
                //active_player->get_instructions() TODO: BLEH remove this!
        );
    }
    ));

    for (unsigned int i=0; i<10; ++i)
    {
        digit_callbacks.push_back(
            input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY(std::to_string(i))},
        [&, i] (KeyboardInputEvent)
        {
            callbackstate.register_number_key(i);
        }
        ))
        );
    }

    switch_char = input_manager->register_mouse_handler(filter(
    {MOUSE_RELEASE},
    [&] (MouseInputEvent event)
    {
        LOG(INFO) << "mouse clicked on map at " << event.to.x << " " << event.to.y << " pixel";

        glm::vec2 tile_clicked(Engine::get_map_viewer()->pixel_to_tile(glm::ivec2(event.to.x, event.to.y)));
        LOG(INFO) << "interacting with tile " << tile_clicked.x << ", " << tile_clicked.y;

        auto sprites = Engine::get_objects_at(tile_clicked);

        if (sprites.size() == 0)
        {
            LOG(INFO) << "No sprites to interact with";
        }
        else if (sprites.size() == 1)
        {
            callbackstate.register_number_id(sprites[0]);
        }
        else
        {
            LOG(WARNING) << "Not sure sprite object to switch to";
            callbackstate.register_number_id(sprites[0]);
        }
    }
    ));

    tile_identifier_text.move_ratio(1.0f, 0.0f);
    tile_identifier_text.resize(256, 64);
    tile_identifier_text.align_right();
    tile_identifier_text.vertical_align_bottom();
    tile_identifier_text.align_at_origin(true);
    tile_identifier_text.set_bloom_radius(5);
    tile_identifier_text.set_bloom_colour(0x00, 0x0, 0x00, 0xa0);
    tile_identifier_text.set_colour(0xff, 0xff, 0xff, 0xa8);
    tile_identifier_text.set_text("(?, ?)");

    func_char = [&] (GameWindow *)
    {
        LOG(INFO) << "text embedWindow resizing";
        //Engine::text_updater(); BLEH TODO: Work out what this does and if neccesary
    };

    text_lifeline_char = embedWindow.register_resize_handler(func_char);

    //Run the map
    run_game = true;

    //Setup challenge
    challenge_data = (new ChallengeData(
                          map_path,
                          &interpreter,
                          &gui_manager,
                          &embedWindow,
                          input_manager,
                          notification_bar,
                          0));

    cursor = new MouseCursor(&embedWindow);

    //Run the challenge - returns after challenge completes

    //Call this when start new challenge
//    while(!embedWindow.check_close() && run_game)
//    {
    challenge_data->run_challenge = true;
    challenge = pick_challenge(challenge_data);
    Engine::set_challenge(challenge);
    challenge->start();

    last_clock = (std::chrono::steady_clock::now());

    //Run the challenge - returns after challenge completes
    VLOG(3) << "{";


    VLOG(3) << "}";

    embedWindow.executeApp();

    // Call this when end challenge
    // Clean up after the challenge - additional, non-challenge clean-up
    em->flush_and_disable(interpreter.interpreter_context);
    delete challenge;
    em->reenable();

//    }
    LOG(INFO) << "Constructed GameMain" << endl;

}

GameMain::~GameMain()
{

    LOG(INFO) << "Destructing GameMain..." << endl;
    delete notification_bar;
    delete challenge_data;
    delete cursor;
    LOG(INFO) << "Destructed GameMain..." << endl;
}

void GameMain::game_loop()
{
    if (!challenge_data->game_window->check_close() && challenge_data->run_challenge)
    {
        //callbackstate.man_move(glm::ivec2( 0, 1));
        last_clock = std::chrono::steady_clock::now();

        VLOG(3) << "} SB | IM {";
        GameWindow::update();

        VLOG(3) << "} IM | EM {";

        do
        {
            EventManager::get_instance()->process_events(interpreter.interpreter_context);
        }
        while (
            std::chrono::steady_clock::now() - last_clock
            < std::chrono::nanoseconds(1000000000 / 60)
        );

        VLOG(3) << "} EM | RM {";
        Engine::get_map_viewer()->render();
        VLOG(3) << "} RM | TD {";
        //Engine::text_displayer(); TODO: work out what this did and if neccesary
        challenge_data->notification_bar->text_displayer();

        // This is not an input event, because the map can move with
        // the mouse staying still.
        {
            std::pair<int,int> pixels = input_manager->get_mouse_pixels();
            glm::ivec2 tile(Engine::get_map_viewer()->pixel_to_tile( {pixels.first, pixels.second}));
            if (tile != tile_identifier_old_tile)
            {
                tile_identifier_old_tile = tile;
                std::stringstream position;
                position << "(" << tile.x << ", " << tile.y << ")";

                tile_identifier_text.set_text(position.str());
            }
        }
        tile_identifier_text.display();

        cursor->display();

        VLOG(3) << "} TD | SB {";
        challenge_data->game_window->swap_buffers();
    }
    else
    {
        em->flush_and_disable(interpreter.interpreter_context);
        delete challenge;
        em->reenable();

        challenge_data->run_challenge = true;
        challenge = pick_challenge(challenge_data);
        Engine::set_challenge(challenge);
        challenge->start();

    }
    return;
}

Challenge* GameMain::pick_challenge(ChallengeData* challenge_data) {
    //int next_challenge(challenge_data->next_challenge);
    Challenge *challenge(nullptr);
    //std::string map_name = "";
    std::ifstream input_file("config.json");
    nlohmann::json j;
    input_file >> j;
    std::string map_name = j["files"]["level_location"];
    challenge_data->map_name = map_name;
    challenge = new Challenge(challenge_data);
    return challenge;
}

GameWindow* GameMain::getGameWindow()
{
    return &embedWindow;
}

