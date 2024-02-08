#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <boost/config.hpp>
#include <memory>
#include "enum_array.hpp"
#include "noncopyable.hpp"
#include "raylib-cpp.hpp"

namespace meow {

class SceneManager;

class BOOST_SYMBOL_VISIBLE Scene : noncopyable {
    friend class SceneManager;

protected:
    // NOLINTBEGIN cppcoreguidelines-non-private-member-variables-in-classes
    raylib::Window *m_window = nullptr;
    SceneManager *m_scene_manager = nullptr;
    // will be removed
    bool m_running = true;
    // NOLINTEND

public:
    explicit Scene() = default;
    virtual void draw() = 0;

    void set_window(raylib::Window *window) noexcept {
        m_window = window;
    }

    [[nodiscard]] bool running() const noexcept {
        return m_running;
    }
};

enum class SceneType { MAIN_MENU, GAME, COUNT };

class SceneManager : noncopyable {
private:
    EnumArray<SceneType, Scene *> m_scenes;
    SceneType m_active_scene = SceneType::MAIN_MENU;

public:
    explicit SceneManager() = default;
    void draw_scene() const;
    void switch_scene(SceneType scene_type);
    void set_scene(SceneType scene_type, Scene *scene);
    [[nodiscard]] Scene *active_scene();
};

}  // namespace meow

#endif  // SCENE_HPP_
