#include "gui_card_span.hpp"
#include <chrono>
#include <cstdlib>
#include "gui_card_loader.hpp"
#include "gui_card_span_dropdown_menu.hpp"
#include "raylib.h"
#include "timed_state_machine.hpp"

namespace meow {

void GuiCardSpan::recalculate_card_rects() noexcept {
    // some magic for proper alignment
    // scaling ratio
    const double ratio = std::sqrt(m_span_borders.height / m_window->GetHeight());
    m_card_gap *= ratio * ratio;
    // delta between original card width and scaled card
    const double dx = (GuiCard::width - ratio * GuiCard::width) / 2;
    const int offsetx_from_borders =
        (m_span_borders.width - m_cards.size() * (GuiCard::width + m_card_gap) + m_card_gap) / 2;
    int i = 0;
    for (GuiCard &card : m_cards) {
        card.target_position.x =
            dx + m_offset.x + offsetx_from_borders + i * (GuiCard::width + m_card_gap);
        card.border.y = card.target_position.y =
            m_span_borders.height - (GuiCard::height * ratio + m_offset.y);
        card.border.width = GuiCard::width * ratio;
        card.border.height = GuiCard::height * ratio;
        i++;
    }
}

void GuiCardSpan::add_card(std::string_view path_to_texture) {
    raylib::Image img, img2;
    try {
        img = meow::load_card_img(path_to_texture);
        // img.Load(path_to_texture.data());
        // img.Mipmaps();
    } catch (const raylib::RaylibException &) {
        img = raylib::Image::Color(GuiCard::width, GuiCard::height, raylib::Color::Green());
    }
    img2 = img;
    img.Resize(GuiCard::width, GuiCard::height);
    raylib::Texture tex = raylib::Texture(img);
    tex.GenMipmaps();
    add_card(
        {raylib::Rectangle(m_window->GetWidth(), 0, 0, 0), raylib::Vector2(0), std::move(tex), img2,
         path_to_texture.data()}
    );
}

void GuiCardSpan::add_card(GuiCard &&card) {
    m_cards.emplace_back(std::move(card));
    m_card_gap -= 10;
    recalculate_card_rects();
}

void GuiCardSpan::remove_card(std::list<GuiCard>::iterator card_iter) {
    m_removed_cards.push_back({std::move(*card_iter), 0});
    m_removed_cards.back().card.target_position =
        raylib::Vector2(-GuiCard::width, -GuiCard::height);
    m_removed_cards.back().fading_coeff = (m_removed_cards.back().card.border.GetPosition() -
                                           m_removed_cards.back().card.target_position)
                                              .LengthSqr();
    m_cards.erase(card_iter);
    m_card_gap += 10;
    recalculate_card_rects();
}

void GuiCardSpan::remove_card() {
    m_removed_cards.push_back({std::move(m_cards.back()), 0});
    m_removed_cards.back().card.target_position =
        raylib::Vector2(-GuiCard::width, -GuiCard::height);
    m_removed_cards.back().fading_coeff = (m_removed_cards.back().card.border.GetPosition() -
                                           m_removed_cards.back().card.target_position)
                                              .LengthSqr();
    m_cards.pop_back();
    m_card_gap += 10;
    recalculate_card_rects();
}

void GuiCardSpan::draw_cards(float frame_time) {
    const bool can_be_dragged = !something_dragged;
    if (!can_be_dragged || raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_LEFT)) {
        m_selected = m_cards.end();
    }
    if (!can_be_dragged || ((raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT) ||
                             raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_RIGHT)) &&
                            !m_dropdown_menu->mouse_in_menu())) {
        m_dropdown_menu->detach_card();
    }
    for (auto card_it = m_cards.begin(); card_it != m_cards.end(); card_it++) {
        if (card_it->border.CheckCollision(raylib::Mouse::GetPosition())) {
            if (can_be_dragged && m_selected == m_cards.end() &&
                raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT) &&
                !m_dropdown_menu->mouse_in_menu()) {
                m_selected = card_it;
                m_dropdown_menu->detach_card();
            } else if (can_be_dragged && m_selected == m_cards.end() &&
                       raylib::Mouse::IsButtonPressed(MOUSE_RIGHT_BUTTON)) {
                m_dropdown_menu->attach_card(card_it, raylib::Mouse::GetPosition());
            }
        }
    }

    // ratio is for texture; maybe i should store ratio as class member
    const double ratio = std::sqrt(m_span_borders.height / m_window->GetHeight());
    for (auto card_it = m_cards.begin(); card_it != m_cards.end(); ++card_it) {
        if (card_it == m_selected) {
            continue;
        }

        if (m_selected == m_cards.end()) {
            card_it->border.DrawRounded(0.1f, 4, raylib::Color(0x004400FF));
        } else {
            card_it->border.DrawRounded(0.1f, 4, raylib::Color(0x00440088));
        }

        card_it->texture.Draw(
            card_it->border.GetPosition(), 0, ratio, raylib::Color(255, 255, 255, 200)
        );
        card_it->border.DrawRoundedLines(0.1f, 4, 3, raylib::Color::White());
        card_it->border.SetPosition(
            Vector2Lerp(card_it->border.GetPosition(), card_it->target_position, frame_time * 4)
        );
    }

    if (m_selected != m_cards.end()) {
        m_selected->border.SetPosition(
            m_selected->border.GetPosition() + raylib::Mouse::GetDelta()
        );
        const double selected_scale = 5.0 / 4;
        m_selected->border.SetSize(m_selected->border.GetSize() * selected_scale);
        m_selected->border.y -= m_selected->border.height * 1.0 / 5;
        m_selected->texture.Draw(m_selected->border.GetPosition(), 0, selected_scale * ratio);
        m_selected->border.DrawRoundedLines(0.1f, 4, 3, raylib::Color::Red());
        m_selected->border.y += m_selected->border.height * 1.0 / 5;
        m_selected->border.SetSize(m_selected->border.GetSize() / selected_scale);
    }

    for (auto card_it = m_removed_cards.begin(); card_it != m_removed_cards.end(); card_it++) {
        auto &card = *card_it;
        raylib::Color c(
            255, 255, 255,
            (unsigned char)((card.card.border.GetPosition() - card.card.target_position)
                                .LengthSqr() /
                            card.fading_coeff * 255)
        );
        card.card.border.SetPosition(
            Vector2Lerp(card.card.border.GetPosition(), card.card.target_position, frame_time * 4)
        );
        card.card.texture.Draw(card.card.border.GetPosition(), c);
        if (c.a == 0) {
            card_it = m_removed_cards.erase(card_it);
        }
    }
    m_dropdown_menu->draw();
    something_dragged = m_selected != m_cards.end();
}

void GuiCardSpan::draw_inspected_card(int window_width, int window_height) {
    if (inspected_card == nullptr) {
        return;
    }
    if (!inspected_card_texture) {
        inspected_card_texture = raylib::Texture(inspected_card->orig_img);
    }
    raylib::Color col{255, 255, 255, 255};
    // static auto blinker = make_timed_state_machine([&col](auto st, auto et) {
    //     auto t =
    //         (float)(et - std::chrono::steady_clock::now()).count() / (et - st).count();  // [0..1]
    //     t = std::abs((t - 0.5) * 255 / 2.0f);  // [-125..125] * 1/4
    //     col.a = 3 * 255 / 4.0f + t;            // [125..255]
    //     // col.a = 255 - col.g;//(float)(et - std::chrono::steady_clock::now()).count() / (et -
    //     // st).count();
    // });
    const raylib::Vector2 pos{
        (window_width - inspected_card->orig_img.width) / 2.0f,
        (window_height - inspected_card->orig_img.height) / 2.0f
    };
    // blinker(std::chrono::seconds(5), true);
    inspected_card_texture->Draw(pos, col);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // inspected_card_texture = std::nullopt;
        inspected_card = nullptr;
    }
}

}  // namespace meow
