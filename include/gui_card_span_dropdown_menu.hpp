#ifndef GUI_CARD_SPAN_DROPDOWN_MENU_HPP_
#define GUI_CARD_SPAN_DROPDOWN_MENU_HPP_

#include "enum_array.hpp"
#include "gui_card_span.hpp"

namespace meow {

class DropDownMenu {
protected:
    GuiCardSpan *m_parental_span;
    std::list<GuiCard> &m_cards = m_parental_span->m_cards;
    std::list<GuiCard>::iterator m_card_iter = m_cards.end();
    raylib::Vector2 m_spawn_point = {-1, -1};

public:
    explicit DropDownMenu(GuiCardSpan *parental_span) : m_parental_span(parental_span) {
    }

    [[nodiscard]] virtual bool mouse_in_menu() const noexcept {
        return false;
    }

    virtual void draw() {
    }

    void detach_card() noexcept {
        m_card_iter = m_cards.end();
    }

    void attach_card(std::list<GuiCard>::iterator card_iter, const raylib::Vector2 &mouse) {
        m_card_iter = card_iter;
        m_spawn_point = mouse;
    }
};

class PlayerHandDDM : public DropDownMenu {
private:
    static constexpr int button_width = 150;
    static constexpr int button_height = 33;
    enum class Button { INSPECT, DROP, COUNT };
    static constexpr EnumArray<Button, Vector2> m_button_origins{
        {Button::INSPECT, {0, 0}},
        {Button::DROP, {0, button_height}},
    };
    static constexpr EnumArray<Button, const char *> m_button_labels{
        {Button::INSPECT, "Inspect"},
        {Button::DROP, "Drop"},
    };

public:
    explicit PlayerHandDDM(GuiCardSpan *parental_span) : DropDownMenu(parental_span) {
    }

    [[nodiscard]] bool mouse_in_menu() const noexcept override {
        if (m_spawn_point.x < 0) {
            return false;
        }
        const auto &arr = m_button_origins.data();
        raylib::Rectangle rec = {
            arr[0].x + m_spawn_point.x, arr[0].y + m_spawn_point.y, arr.back().x + button_width,
            arr.back().y + button_height};
        return rec.CheckCollision(raylib::Mouse::GetPosition());
    }

    void draw() override {
        if (m_card_iter == m_cards.end()) {
            return;
        }

        EnumArray<Button, bool> pressed;
        bool something_pressed = false;

        GuiSetAlpha(0.85f);
        for (std::size_t i = 0; i < m_button_origins.size(); i++) {
            pressed[i] = GuiButton(
                {m_button_origins[i].x + m_spawn_point.x, m_button_origins[i].y + m_spawn_point.y,
                 button_width, button_height},
                m_button_labels[i]
            );
            something_pressed = something_pressed || pressed[i];
        }
        GuiSetAlpha(1.0f);

        if (pressed[Button::DROP]) {
            m_parental_span->remove_card(m_card_iter);
        }
        if (pressed[Button::INSPECT]) {
            m_parental_span->inspected_card = &*m_card_iter;
        }
        if (something_pressed) {
            m_spawn_point = Vector2{-1, -1};
            m_card_iter = m_cards.end();
        }
    }
};

class BrawlCardsDDM : public DropDownMenu {
private:
    static constexpr int button_width = 150;
    static constexpr int button_height = 33;
    enum class Button { INSPECT, COUNT };
    static constexpr EnumArray<Button, Vector2> m_button_origins{
        {Button::INSPECT, {0, 0}},

    };
    static constexpr EnumArray<Button, const char *> m_button_labels{
        {Button::INSPECT, "Inspect"},
    };

public:
    explicit BrawlCardsDDM(GuiCardSpan *parental_span) : DropDownMenu(parental_span) {
    }

    [[nodiscard]] bool mouse_in_menu() const noexcept override {
        if (m_spawn_point.x < 0) {
            return false;
        }
        const auto &arr = m_button_origins.data();
        raylib::Rectangle rec = {
            arr[0].x + m_spawn_point.x, arr[0].y + m_spawn_point.y, arr.back().x + button_width,
            arr.back().y + button_height};
        return rec.CheckCollision(raylib::Mouse::GetPosition());
    }

    void draw() override {
        if (m_card_iter == m_cards.end()) {
            return;
        }

        EnumArray<Button, bool> pressed;
        bool something_pressed = false;

        GuiSetAlpha(0.85f);
        for (std::size_t i = 0; i < m_button_origins.size(); i++) {
            pressed[i] = GuiButton(
                {m_button_origins[i].x + m_spawn_point.x, m_button_origins[i].y + m_spawn_point.y,
                 button_width, button_height},
                m_button_labels[i]
            );
            something_pressed = something_pressed || pressed[i];
        }
        GuiSetAlpha(1.0f);

        if (pressed[Button::INSPECT]) {
            m_parental_span->inspected_card = &*m_card_iter;
        }
        if (something_pressed) {
            m_spawn_point = Vector2{-1, -1};
            m_card_iter = m_cards.end();
        }
    }
};

class KittenCardsDDM : public DropDownMenu {
private:
    static constexpr int button_width = 150;
    static constexpr int button_height = 33;
    enum class Button { INSPECT, DROP, COUNT };
    static constexpr EnumArray<Button, Vector2> m_button_origins{
        {Button::INSPECT, {0, 0}},
        {{Button::DROP}, {0, button_height}}};
    static constexpr EnumArray<Button, const char *> m_button_labels{
        {Button::INSPECT, "Inspect"},
        {Button::DROP, "Drop"},
    };

public:
    explicit KittenCardsDDM(GuiCardSpan *parental_span) : DropDownMenu(parental_span) {
    }

    [[nodiscard]] bool mouse_in_menu() const noexcept override {
        if (m_spawn_point.x < 0) {
            return false;
        }
        const auto &arr = m_button_origins.data();
        raylib::Rectangle rec = {
            arr[0].x + m_spawn_point.x, arr[0].y + m_spawn_point.y, arr.back().x + button_width,
            arr.back().y + button_height};
        return rec.CheckCollision(raylib::Mouse::GetPosition());
    }

    void draw() override {
        if (m_card_iter == m_cards.end()) {
            return;
        }

        EnumArray<Button, bool> pressed;
        bool something_pressed = false;

        GuiSetAlpha(0.85f);
        for (std::size_t i = 0; i < m_button_origins.size(); i++) {
            pressed[i] = GuiButton(
                {m_button_origins[i].x + m_spawn_point.x, m_button_origins[i].y + m_spawn_point.y,
                 button_width, button_height},
                m_button_labels[i]
            );
            something_pressed = something_pressed || pressed[i];
        }
        GuiSetAlpha(1.0f);

        if (pressed[Button::INSPECT]) {
            m_parental_span->inspected_card = &*m_card_iter;
        } else if (pressed[Button::DROP]) {
            m_parental_span->send_action_to_remove(m_card_iter->card_id);
        }
        if (something_pressed) {
            m_spawn_point = Vector2{-1, -1};
            m_card_iter = m_cards.end();
        }
    }
};

}  // namespace meow

#endif  // GUI_CARD_SPAN_DROPDOWN_MENU_HPP_
