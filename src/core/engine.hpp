///
/// Defines the API used for the C++/Python integration
///

#ifndef ENGINE_H
#define ENGINE_H

#include <boost/python/object_core.hpp>

#include <glm/vec2.hpp>
#include <string>
#include <memory>
#include <vector>

#include "challenge.hpp"
#include "game_window.hpp"
#include "gui_main.hpp"
#include "text_font.hpp"
#include "typeface.hpp"

class MainWindow;
class GameMain;
class MapViewer;
class TextBox;

// Class wrapping the API calls into a static public class
class Engine {
private:
    static MapViewer *map_viewer;

    static GameMain* game_main;

    static GameWindow* game_window;

    static GUIMain *gui_main;

    static MainWindow* main_window;

    static Challenge* challenge;
    ///
    /// The size of a tile
    ///
    static int tile_size;

    ///
    /// The global tile and object scaling factor, how much to multiply widths and heights
    /// by
    ///
    static float global_scale;

public:
    ///
    /// Get the global scale
    /// @return the global scale
    ///
    static float get_global_scale() { return global_scale; }

    ///
    /// Set the global scaling factor
    /// @param _global_scale the scaling factor
    ///
    static void set_global_scale(float global_scale) { Engine::global_scale = global_scale; }



    ///
    /// Set the tile size to be used by the engine
    /// @param _tile_size the tile size
    ///
    static void set_tile_size(int tile_size) { Engine::tile_size = tile_size; }

    ///
    /// Get the tile size used by the engine  in pixels - we only support square ones
    /// @return the tile size
    ///
    static int get_tile_size() { return tile_size; }

    static float get_actual_tile_size() { return float(tile_size) * global_scale; }

    ///
    /// Set the game main instance (for access to game loop and gui)
    /// @param _game_main the game main instance
    ///
    static void set_game_main(GameMain* _game_main) { game_main = _game_main; }

    ///
    /// Set the game window
    /// @param _game_window the game window
    ///
    static void set_game_window(GameWindow* _game_window) { game_window = _game_window; }

    ///
    /// Set the main window
    /// @param _main_window the main window
    ///
    static void set_main_window(MainWindow* _main_window) { main_window = _main_window; }

    ///
    /// Get the game window
    /// @return the game window
    ///
    static GameWindow* get_game_window() { return game_window; }

    ///
    /// Set the map viewer attached to the engine
    /// @param _map_viewer the map viewer which is attached to the engine
    ///
    static void set_map_viewer(MapViewer *map_viewer) { Engine::map_viewer = map_viewer; }


    ///
    /// Get the map viewer attached to the engine
    /// @return a pointer to the map viewer
    ///
    static MapViewer *get_map_viewer() { return map_viewer; }

    ///
    /// Move sprite onscreen
    ///
    /// @param id ID of sprite to move
    /// @param dx move in x by dx tiles
    /// @param dy move in x by dy tiles
    /// @param func the callback to be called once the movement operation is complete (get's put on the event queue)
    ///

    static void move_object(int id, glm::ivec2 move_by, double duration, std::function<void ()> func);
    static void move_object(int id, glm::ivec2 move_by);

    ///
    /// Determine if a location can be walked on
    /// @param x_pos the x position to test
    /// @param y_pos the y position to test
    /// @return true if the position can be walked on, false if not
    ///
    static bool walkable(glm::ivec2 location);

    ///
    /// Change the map to the map specified by the given locatuib
    /// @param location of the .tmx map file
    ///
    static void change_map(std::string map_location);

    ///
    /// Change the tile in the map in the given layer at the provided position
    /// @param tile the x,y position of the tile to change
    /// @param layer the layer of the tile to change
    /// @param tile_name the global name of the new tile
    /// @return indicates if the operation completed successfully
    ///
    static void change_tile(glm::ivec2 tile, std::string layer_name, std::string tile_name);

    ///
    /// Get a list of objects at this point, doesn't include sprites
    /// @return a vector of object ids
    ///
    static std::vector<int> get_objects_at(glm::ivec2 location);

    ///
    /// Get the instance of the QT mainwindow
    ///
    ///
    static MainWindow* get_main_window();

    ///
    /// Get whether a sprite with a certain id is at this tile
    ///
    static bool is_object_at(glm::ivec2 location, int object_id);

    ///
    /// Get whether a vector of objects is at this tile
    ///
    static bool is_objects_at(glm::ivec2 location, std::vector<int> object_id);

    static int get_tile_type(int x, int y);

    static void set_gui(GUIMain * _gui_main) { Engine::gui_main = _gui_main; }
    static GUIMain* get_gui() { return gui_main; }

    static void add_dialogue(std::string text);
    static void add_text(std::string text);
    static void open_notification_bar(bool disable_scripting, std::function<void ()> func);
    static void open_notification_bar_with_options(bool disable_scripting, std::deque<std::pair<std::string, std::function<void ()> > > _options);
    static void close_notification_bar();
    static bool is_bar_open();
    static bool is_bar_with_options_open();

    static void show_external_script_help(std::string text);
    static void close_external_script_help();
    static bool is_external_help_open();

    static std::string get_player_name();
    static void set_player_name(std::string player_name);

    /// trigger qt mainwindow functions
    static void show_py_scripter();
    static void hide_py_scripter();
    static void enable_py_scripter();
    static void disable_py_scripter();
    static void enable_script_editing();
    static void disable_script_editing();
    static void set_py_tabs(int val);
    static void show_external_script(std::function<void ()> confirm_callback, std::function<void ()> cancel_callback, std::string external_dialogue, std::function<void ()> script_init);
    //static void hide_external_tab();
    static void update_world(std::string text);
    static void update_level(std::string text);
    static void update_coins(int value);
    static void update_totems(int value,bool show);

    static void insert_to_scripter(std::string text);
    static void clear_scripter();
    static std::string get_script();
    static std::string get_external_script();
    static void print_terminal(std::string text, bool error);
    static std::string get_terminal_text(unsigned int index);
    static void focus_next();
    static void set_any_output(bool option);
    static void set_ui_colours(int r1, int b1, int g1, int r2, int b2, int g2);
    static void set_running();
    static void set_finished();
    static void trigger_run(int script);
    static void trigger_speed();
    static int get_run_script();

    static void set_music(bool on);
    static void open_main_menu();
    static void restart_level();

    /// global access to game font
    static TextFont get_game_font();
    static Typeface get_game_typeface();

    static void set_challenge(Challenge* _challenge)
    {
        challenge = _challenge;
        game_window->update_running(false);
    }
    static Challenge* get_challenge() { return challenge; }
};

#endif
