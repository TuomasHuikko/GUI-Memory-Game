/* Ohjelman kirjoittaja
* Nimi: Tuomas Huikko
* Opiskelijanumero: H291873
* Käyttäjätunnus: gmtuhu
* E-Mail: tuomas.huikko@tuni.fi
* */


#include "player.hh"
#include <iostream>


Player::Player(const std::string& name):
    name_(name)
{
    number_of_pairs_ = 0;
}

std::string Player::get_name() const
{
    return name_;
}

unsigned int Player::number_of_pairs() const
{
    return number_of_pairs_;
}

void Player::add_card(Card& card_1, Card& card_2)
{
    card_1.remove_from_game_board();
    card_2.remove_from_game_board();
    number_of_pairs_ += 1;
}

void Player::print() const
{
    std::cout << "*** " << name_ << " has " << number_of_pairs_ << " pair(s)." << std::endl;
}
