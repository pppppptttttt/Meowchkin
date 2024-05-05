#ifndef RAYLIB_GAME_VIEW_HPP_
#define RAYLIB_GAME_VIEW_HPP_

#include <filesystem>
#include <functional>
#include <memory>
#include <variant>
#include "enum_array.hpp"
#include "game_view.hpp"
#include "gui_board.hpp"
#include "gui_card_span.hpp"
#include "gui_player_statistics_menu.hpp"
#include "gui_text_chat.hpp"
#include "gui_usernames_box.hpp"
#include "timed_state_machine.hpp"

namespace meow {

class RaylibGameView : public GameView {
private:
    static constexpr int button_width = 300;
    static constexpr int button_height = 40;
    inline static const raylib::Color background_color = raylib::Color(77, 120, 204, 255);

    struct GameplayObjs {
        GuiBoard board{};
        GuiCardSpan player_hand{std::make_unique<PlayerHandDDM>(&player_hand)};
        GuiUsernamesBox usernames_box{};
        GuiTextChat text_chat{};
        GuiPlayerStatisticsMenu stats{};
    } m_gameplay_objects;

    struct PauseMenuObjs {
        enum class PauseButton { CONTINUE, BACK_TO_LOBBY, QUIT, COUNT };  // TODO: settings
        EnumArray<PauseButton, const char *> button_labels = {
            {PauseButton::CONTINUE, "Continue"},
            {PauseButton::BACK_TO_LOBBY, "Back to lobby"},
            {PauseButton::QUIT, "Quit"},
        };
        EnumArray<PauseButton, raylib::Rectangle> button_rects{};
        EnumArray<PauseButton, bool> button_pressed{};
    } m_pause_menu_objects;

    std::variant<GameplayObjs *, PauseMenuObjs *> m_active_display;

    /* background */
    raylib::Texture m_background;
    raylib::Texture m_blur;

    /* misc */
    std::vector<std::filesystem::path> m_card_image_paths;
    bool m_show_chat = false;
    raylib::Texture m_on_levelup_texture;
    std::function<void(std::chrono::milliseconds, bool)> m_levelup_blink_call =
        make_timed_state_machine([this](auto, auto) { m_on_levelup_texture.Draw(); });
    bool m_levelup_blink = false;

protected:
    void on_instances_attach() override;

public:
    RaylibGameView();
    void draw() override;

    /* Callbacks */
    void on_card_add(std::string_view card_filename) override;
    void on_card_remove(std::string_view card_filename) override;
    void on_turn_begin() override;
    void on_turn_end() override;
    void on_levelup() override;
    void on_card_receive() override;
    void on_item_equip() override;
    void on_item_loss() override;
    void on_monster_elimination() override;
    void on_being_cursed() override;

    static std::shared_ptr<RaylibGameView> make_raylib_gameview() {
        return std::make_shared<RaylibGameView>();
    }

private:
    void setup_background();
    void setup_pause_menu();
    void setup_hand();
    void draw_pause_menu();
};

}  // namespace meow

#endif  // RAYLIB_GAME_VIEW_HPP_
