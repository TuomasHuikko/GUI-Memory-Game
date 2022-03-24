/* Ohjelman kirjoittaja
* Nimi: Tuomas Huikko
* Opiskelijanumero: H291873
* Käyttäjätunnus: gmtuhu
* E-Mail: tuomas.huikko@tuni.fi
* */


/* Luokka: Player
 * --------------
 * Kuvaa yhtä pelaajaa muistipelissä.
 *
 * COMP.CS.110 K2021
 * */


#ifndef PLAYER_HH
#define PLAYER_HH

#include "card.hh"
#include <string>

class Player
{
public:
    // Rakentaja: luo annetun nimisen pelaajan.

    Player(const std::string& name);

    // Palauttaa pelaajan nimen.

    std::string get_name() const;

    // Palauttaa pelaajan tähän asti keräämien parien määrän.

    unsigned int number_of_pairs() const;

    // Siirtää annetun korttiparin pelilaudalta pelaajalle,
    // eli lisää parin pelaajan keräämiin kortteihin
    // ja poistaa ne pelilaudalta.

    void add_card(Card& card_1, Card& card_2);

    // Tulostaa pelaajan tilanteen: nimen ja tähän asti kerättyjen parien määrän.

    void print() const;

private:
    std::string name_;
    unsigned int number_of_pairs_;

};

#endif // PLAYER_HH
