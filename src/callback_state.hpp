#ifndef CALLBACKSTATE_H
#define CALLBACKSTATE_H

#include <glog/logging.h>
#include <glm/vec2.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "challenge.hpp"        //Included for getting id_type
#include "engine.hpp"
#include "python_thread_runner.hpp"
#include "map_viewer.hpp"
#include "map_object.hpp"
#include "object_manager.hpp"


///
/// This class holds all the callback functions for user input
///
class CallbackState {
public:
    CallbackState() {

    }

    void register_number_key(unsigned int key) {
        LOG(INFO) << "changing focus to " << key;

        // Note: unsigned
        if (key >= key_to_id.size()) {
            LOG(INFO) << "changing focus aborted; no such id";
            return;
        }

        Engine::get_map_viewer()->set_map_focus_object(key_to_id[key]);
    }

    void register_number_id(int id) {
        LOG(INFO) << "changing focus to " << id;

        //TODO: handle incorrect ID
        Engine::get_map_viewer()->set_map_focus_object(id);
    }

    void restart() {

        //Reset anyOuput variable
        Engine::set_any_output(false);

        auto id = Engine::get_map_viewer()->get_map_focus_object();
        auto active_player = ObjectManager::get_instance().get_object<Object>(id);

        if (!active_player) { return; }

        active_player->daemon->value->halt_soft(PythonThreadRunner::Signal::RESTART);
    }

    void stop() {

        auto id = Engine::get_map_viewer()->get_map_focus_object();
        auto active_player = ObjectManager::get_instance().get_object<Object>(id);

        if (!active_player) { return; }

        active_player->daemon->value->halt_soft(PythonThreadRunner::Signal::STOP);
    }

    void kill() {

        auto id = Engine::get_map_viewer()->get_map_focus_object();
        auto active_player = ObjectManager::get_instance().get_object<Object>(id);

        if (!active_player) { return; }

        active_player->daemon->value->halt_soft(PythonThreadRunner::Signal::KILL);
    }

    void man_move(glm::vec2 direction) {
        auto id = Engine::get_map_viewer()->get_map_focus_object();
        Engine::move_object(id, direction);
    }
private:
    std::vector<int> key_to_id;
};

#endif
