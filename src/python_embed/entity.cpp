#include "python_embed_headers.hpp"

#include <boost/iterator/zip_iterator.hpp>
#include <boost/multi_index/detail/bidir_node_iterator.hpp>
#include <boost/multi_index/detail/ord_index_node.hpp>
#include <boost/python/list.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/filesystem.hpp>
#include <glm/vec2.hpp>
#include <glog/logging.h>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "config.hpp"
#include "entity.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "game_time.hpp"
#include "object_manager.hpp"
#include "map_object.hpp"
#include "map_viewer.hpp"

Entity::Entity(glm::vec2 start, std::string name, std::string file_location, int id):
    start(start), id(id), call_number(0) {
    this->name = std::string(name);
    this->file_location = std::string(file_location);

    this->sprite_location = "";
    VLOG(1) << "invalid: constructor " << this->id;

    this->animating = false;

    this->current_frame = 0;
}

//A dummy function for testing callbacks in python, TODO: once this has been refered to to implement an even-driven callback system, remove this!!!
void Entity::callback_test(PyObject *callback) {
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    EventManager::get_instance()->add_event(boost_callback);
}

void Entity::wait(double gametime, PyObject *callback) {
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    EventManager::get_instance()->add_timed_event(
        GameTime::duration(gametime),
    [boost_callback] (float completion) {
        if (completion == 1.0) {
            EventManager::get_instance()->add_event(boost_callback);
        }
        return true;
    }
    );
}

void Entity::move_by(int x, int y, double duration, PyObject *callback) {
    ++call_number;
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    auto id = this->id;
    Engine::move_object(id, glm::ivec2(x, y), duration, boost_callback);
    return;
}

void Entity::move_east(PyObject *callback) {
    return (move_by(1, 0, 0.3, callback));
}

void Entity::move_west(PyObject *callback) {
    return (move_by(-1, 0, 0.3, callback));
}

void Entity::move_north(PyObject *callback) {
    return (move_by(0, 1, 0.3, callback));
}

void Entity::move_south(PyObject *callback) {
    return (move_by(0, -1, 0.3, callback));
}

bool Entity::is_moving() {
    auto object = ObjectManager::get_instance().get_object<MapObject>(this->id);
    return object->is_moving();
}

void Entity::set_solidity(bool solidity, PyObject *callback) {

    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));

    auto id = this->id;
    Walkability w;
    if (solidity) w = Walkability::BLOCKED;
    else w = Walkability::WALKABLE;
    EventManager::get_instance()->add_event([w, id, boost_callback] () {
        auto object(ObjectManager::get_instance().get_object<MapObject>(id));
        object->set_walkability(w);
        EventManager::get_instance()->add_event(boost_callback);
    });
}

void Entity::set_visible(bool visibility, PyObject *callback) {

    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));

    auto id = this->id;
    
    EventManager::get_instance()->add_event([visibility, id, boost_callback] () {
        auto object(ObjectManager::get_instance().get_object<MapObject>(id));
        object->set_renderable(visibility);
        EventManager::get_instance()->add_event(boost_callback);
    });
}

bool Entity::is_visible() {
    auto object = ObjectManager::get_instance().get_object<MapObject>(this->id);
    return object->is_renderable();
    
}

bool Entity::is_solid() {
    auto object = ObjectManager::get_instance().get_object<MapObject>(this->id);
    Walkability w = object->get_walkability();
    if (w == Walkability::BLOCKED) {
        return true;
    } else {
        return false;
    }
}

void Entity::focus() {
    auto id = this->id;
    MapViewer *map_viewer = Engine::get_map_viewer();
    map_viewer->set_map_focus_object(id);
}

bool Entity::is_focus() {
    auto id = this->id;
    MapViewer *map_viewer = Engine::get_map_viewer();
    return (id == map_viewer->get_map_focus_object());
}

std::string Entity::get_name() { //TODO: Analyse wether it would be best to get this information from the object instance, instead of from the information copied into this instance. (and then not have that information here)
    std::string name = this->name;
    return name;
}

std::string Entity::get_location() {
    std::string file_location = this->file_location;
    return file_location;
}

std::string Entity::get_sprite() {
    return this->sprite_location;
}


//THE TWO METHODS BELOW NEED TO BE REALLY OPTIMISED TO USE THE CACHE!!!!!!! (LOOK AT MapObject::set_tile)
void Entity::set_sprite(std::string sprite_location, PyObject *callback) {
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));

    this->sprite_location = sprite_location; //TODO: Make this method safe, make it so that if the sprite location doesn't exist the game gracefully handles it.
    int id = this->id;
    std::string file_location = this->file_location;
    EventManager *em = EventManager::get_instance();
    em->add_event([id, sprite_location, file_location, boost_callback] () { //put changing the player tile on the event queue
        auto object = ObjectManager::get_instance().get_object<MapObject>(id);
        object->set_tile("../game/objects/" + file_location + "/sprites/" + sprite_location + "/0.png");
        EventManager::get_instance()->add_event(boost_callback);
    });
    return;
}

void Entity::set_animation_frame(int frame_number) {
    this->current_frame = frame_number;
    auto sprite_location = this->sprite_location;
    int id = this->id;
    std::string file_location = this->file_location;

    auto object = ObjectManager::get_instance().get_object<MapObject>(id);
    object->set_tile("../game/objects/" + file_location + "/sprites/" + sprite_location + "/" + std::to_string(frame_number) + ".png");
    return;
}


void Entity::start_animating(float speed, bool loop, bool forward, PyObject *callback) {
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));

    //to replay from start if the present frame is at the end
    if(!loop && this->current_frame == 3){
        this->current_frame = 0;
    }

    //if we have a loop or this is already being animated, run the callback immediately
    if(loop || this->animating){
        EventManager::get_instance()->add_event(boost_callback);
    }

    if(!this->animating) {
        this->animating = true;
        this->animate(this->current_frame, speed, loop, forward, boost_callback);
    }
    
}

void Entity::pause_animating() {
    this->animating = false;
}

void Entity::animate(int current_frame, float speed, bool loop, bool forward, std::function<void ()> callback) {
    if (this->animating) {
        //TODO: auto num_frame = get_number_of_animation_frames();
        auto num_frame = 4;
        EventManager::get_instance()->add_event([this, current_frame, num_frame, speed, loop, forward, callback]() {
            this->set_animation_frame(current_frame);
            int next_frame;
            if(forward){
                next_frame = ((current_frame + 1) % num_frame);
            }
            else{
                next_frame = ((current_frame - 1 + num_frame) % num_frame); // we have to add num_frame cause c++ treats negative numbers weirdly in mod
            }

            if(next_frame == 0 && (!loop)){
                this->animating = false;
                EventManager::get_instance()->add_event(callback);
                return;
            }
            EventManager::get_instance()->add_timed_event(GameTime::duration(speed), [next_frame, this, speed, loop, forward, callback] (float completion) {
                if (completion == 1.00) {
                    this->animate(next_frame, speed, loop, forward, callback);
                }
                return true;
            });
        });
    }
}


int Entity::get_number_of_animation_frames() {
    Config::json j = Config::get_instance();

    std::string config_location = j["files"]["object_location"];
    std::string full_file_location = config_location + "/" + file_location + "/sprites/" + this->sprite_location;
    std::cout << full_file_location << std::endl;

    int num_frames = (int) std::count_if(boost::filesystem::directory_iterator(full_file_location),
                                         boost::filesystem::directory_iterator(),
    [](const boost::filesystem::directory_entry & e) {
        return e.path().extension() == ".png";
    });
    return num_frames;

}

std::string Entity::get_instructions() {
    //auto id(this->id);
    //return GilSafeFuture<std::string>::execute([id] (GilSafeFuture<std::string> instructions_return) {
    //  auto sprite(ObjectManager::get_instance().get_object<MapObject>(id));
    //
    //  if (sprite) {
    //      //instructions_return.set(sprite->get_instructions()); TODO: BLEH work out what this did
    //  }
    //  else {
    //      instructions_return.set("Try thinking about the problem in a different way.");
    //  }
    //});
    return "HH";
}

// Not thread safe for efficiency reasons...
void Entity::py_print_debug(std::string text) {
    LOG(INFO) << text;
}

//void Entity::py_print_dialogue(std::string text) {
//    Engine::print_dialogue(text);
//}

void Entity::__set_game_speed(float game_seconds_per_real_second) {
    EventManager::get_instance()->time.set_game_seconds_per_real_second(game_seconds_per_real_second);
}

void Entity::py_update_status(std::string status) {
    //auto id(this->id);
    //return GilSafeFuture<void>::execute([id, status] (GilSafeFuture<void>) {
    //Engine::update_status(id, status); TODO: BLEH work out what this did
    //});
    LOG(INFO) << status;
}

py::object Entity::read_message() {
    auto id(this->id);
    auto object(ObjectManager::get_instance().get_object<MapObject>(id));
    auto *challenge(object->get_challenge());

    if (challenge) {
        return challenge->read_message(id);
    }
    else {
        return py::object();
    }
}

int Entity::get_id() {
    return this->id;
}

py::tuple Entity::get_position() {
    auto object = ObjectManager::get_instance().get_object<MapObject>(this->id);
    glm::ivec2 position = object->get_game_position();
    return py::make_tuple(position.x, position.y);
}
