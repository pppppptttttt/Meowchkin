#include <cassert>
#include <fstream>
#include <memory>
#include <string>
#include "client.hpp"
#include "enum_array.hpp"
#include "plugin.hpp"
#include "raylib-cpp.hpp"
#include "scene.hpp"
#include "server.hpp"

void run_bebra(meow::network::Client &client) {
    SetTraceLogLevel(LOG_ERROR);
    static constexpr int window_width = 1920;
    static constexpr int window_height = 1080;
    static constexpr const char *window_title = "meow";
    raylib::Window window(
        window_width, window_height, window_title,
        /*FLAG_WINDOW_RESIZABLE |*/ FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE
    );

    // load scenes from 'plugins'
    const meow::EnumArray<meow::SceneType, std::pair<const char *, const char *>> plugin_names{
        {meow::SceneType::MAIN_MENU, {"mainmenu-scene", "main_menu"}},
        {meow::SceneType::GAME, {"game-scene", "game_view"}},
    };
    meow::Plugin<meow::Scene> main_menu(plugin_names[meow::SceneType::MAIN_MENU]);
    meow::Plugin<meow::Scene> game_view(plugin_names[meow::SceneType::GAME]);
    main_menu->attach_window(&window);
    main_menu->m_client = &client;
    game_view->attach_window(&window);
    game_view->m_client = &client;

    SetExitKey(0);
    window.SetTargetFPS(60);

    auto scene_manager = std::make_unique<meow::SceneManager>();
    scene_manager->set_scene(meow::SceneType::MAIN_MENU, main_menu.get());
    scene_manager->set_scene(meow::SceneType::GAME, game_view.get());

    while (!window.ShouldClose() && scene_manager->active_scene()->running()) {
        // application response
        {
            if (IsKeyPressed(KEY_R)) {
                main_menu.reload(plugin_names[meow::SceneType::MAIN_MENU]);
                main_menu->attach_window(&window);
                scene_manager->set_scene(meow::SceneType::MAIN_MENU, main_menu.get());
                game_view.reload(plugin_names[meow::SceneType::GAME]);
                game_view->attach_window(&window);
                scene_manager->set_scene(meow::SceneType::GAME, game_view.get());
            }
        }
        // application render
        window.BeginDrawing();
        {
            window.ClearBackground(RAYWHITE);
            scene_manager->draw_scene();
        }
        window.EndDrawing();
    }
}

void run_server() {
    meow::network::Server server;
    std::cout << server.get_port() << '\n';
    server.start_listening(2);
    for (std::optional<meow::network::Action> action;; action = server.receive_action()) {
        if (action) {
            server.send_action(action->targeted_player, *action);
        }
    }
}

void run_client(std::string_view client_name) {
    meow::network::Client client;
    client.set_name_of_client(client_name.data());
    client.connect("localhost:" + std::string(client_name.data()));
    if (client.get_id_of_client() != 0) {
        std::cout << "bebra!!!!\n";
    }
    run_bebra(client);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    assert(argc >= 2);
    // deduce are we host or client
    if (std::strcmp(argv[1], "host") == 0) {
        run_server();
    } else if (std::strcmp(argv[1], "client") == 0) {
        assert(argc >= 4);
        run_client(argv[3]);
    }
}
