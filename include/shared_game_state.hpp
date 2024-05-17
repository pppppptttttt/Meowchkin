#ifndef SHARED_GAME_STATE_HPP_
#define SHARED_GAME_STATE_HPP_
#include <limits>
#include <vector>
#include "game_state.hpp"
#include "model_player.hpp"
#include "game_view.hpp"
#include <memory>

namespace meow::model {

struct SharedGameState {
private:
    std::vector<Player> players;
    std::size_t cur_turn = std::numeric_limits<std::size_t>::max();
    std::vector<std::shared_ptr<GameView>> observers;

public:
    void set_player_list(const std::vector<std::size_t> &users);

    [[nodiscard]] std::size_t get_current_user_id() const;

    [[nodiscard]] std::size_t get_number_of_players() const;

    [[nodiscard]] std::size_t get_card_id_from_deck() const;

    [[nodiscard]] Player *get_player_by_player_id(std::size_t player_id);

    [[nodiscard]] Player *get_player_by_user_id(std::size_t user_id);

    [[nodiscard]] std::size_t get_player_position_by_user_id(std::size_t user_id) const;

    void set_first_player_by_position(std::size_t position);

    std::vector<Player> &get_all_players();

    void end_turn();

    bool is_end() const;

    void add_observer(std::shared_ptr<GameView> observer) {
        observers.emplace_back(observer);
    }
};

}  // namespace meow::model

#endif