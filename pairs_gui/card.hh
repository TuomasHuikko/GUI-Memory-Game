/* Ohjelman kirjoittaja
* Nimi: Tuomas Huikko
* Opiskelijanumero: H291873
* Käyttäjätunnus: gmtuhu
* E-Mail: tuomas.huikko@tuni.fi
* */


/* Luokka: Card
 * ------------
 * Kuvaa yhtä korttia muistipelissä.
 *
 * COMP.CS.110 K2021
 * */
//


#ifndef CARD_HH
#define CARD_HH

// Kortin näkyvyystyyppi: Auki, piilotettu, tyhjä=löydetty pari

enum Visibility_type {OPEN, HIDDEN, EMPTY};
const char HIDDEN_CHAR = '#';
const char EMPTY_CHAR = '.';

class Card
{
public:
    // Oletusrakentaja: luo tyhjän kortin.

    Card();

    // Rakentaja: luo piilotetun kortin, jonka kirjaimeksi tulee annettu merkki.

    Card(const char c);

    // Ilmeiset setter- ja getter-metodit.

    void set_letter(const char c);
    void set_visibility(const Visibility_type visibility);
    char get_letter() const;
    Visibility_type get_visibility() const;

    // Kääntää kortin: vaihtaa näkyvyyden avoimesta piilotettuun ja päin vastoin.

    void turn();

    // Tulostaa kortin sen tämänhetkisen tilanteen (näkyvyyden) mukaisesti.

    void print() const;

    // Poistaa kortin pelilaudalta: muuttaa näkyvyyden tyhjäksi.

    void remove_from_game_board();

private:
    char letter_;
    Visibility_type visibility_;
};

#endif // CARD_HH
