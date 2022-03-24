/* Ohjelman kirjoittaja
* Nimi: Tuomas Huikko
* Opiskelijanumero: H291873
* Käyttäjätunnus: gmtuhu
* E-Mail: tuomas.huikko@tuni.fi
* */


#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include "card.hh"
#include "player.hh"
#include <QMainWindow>
#include <random>
#include <QPushButton>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using Game_row_type = std::vector<Card*>;
using Game_board_type = std::vector<std::vector<Card*>>;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Muuttaa pelilautaan liittyvän game_board_ rakenteen alustetut tyhjät kortit niiden alkuarvoihin.
    // Huolehtii myös korttien paikan arpomisesta.
    void init_cards();

    // Luo pelilaudan graafisen käyttöliittymän ja lisää siihen kortteja vastaavat pushButtonit.
    void init_cards_gui();

    // Täyttää pelilautarakenteen game_board_ tyhjillä korteilla käyttäjän antaman lukumäärän mukaan.
    void init_with_empty_cards();

    // Laskee pienimmät yhteiset tekijät korttien määrälle. Tämän avulla kortit sijoitellaan riveihin ja sarakkeisiin.
    // Ottaa parametreinään muuttujat joihin tekijät sijoitetaan.
    void calculate_factors(unsigned int& smaller_factor, unsigned int& bigger_factor);

    // Tarkastelee ovatko käännetyt kortit samat vai eivät ja tämän perusteella joko poistaa kortit laudalta
    // tai kääntää ne takaisin ympääri. Sisältää myös pelin loppumisen tarkastelun.
    // Ottaa parametreinään osoittimet käännettyihin kortteihin.
    void process_pair_of_cards(Card* card_1, Card* card_2);

    // Etsii tietorakenteesta buttons_with_cards_ käännetyt(avoimet) kortit ja palauttaa ne vektorissa process_pair_of_cards metodia varten.
    std::vector<Card*> find_turned_cards();

    // Lukee pelaajien nimet ja määrän käyttäjän syötteistä ja tallentaa nimet rakenteeseen players_.
    void init_players();

    // Sallii "Start Game" nappulan käyttämisen, mikäli käyttäjän syötteet ovat hyväksyttävässä muodossa. Metodi siis sisältää
    // pelaajien nimien sekä korttien määrän virhetarkastelun.
    void enable_start_button_if_possible();

    // Tulostaa tällä hetkellä pelaavan pelaajan nimen sekä pistetilanteen pääikkunan alalaitaan.
    void print_score_board();

    // Tarkastelee onko peli loppunut(kortit lopussa)vai ei. Palauttaa true->peli loppunut ja false->peli kesken.
    bool is_game_over();

    // Päättelee pelaajien joukosta voittajan(tai voittajat) ja tulostaa lopputuloksen eli voittajan sekä hänen pisteensä
    // pääikkunan alalaitaan.
    void determine_and_print_winner();


private slots:

    // Muuttaa korttien lukumäärää käyttäjän haluamaksi.
    void on_card_spin_box_valueChanged(int arg1);

    // Muuttaa pelaajien lukumäärää käyttäjän haluamaksi.
    void on_player_spin_box_valueChanged(int arg1);

    // Käynnistää muistipelin toiminnan.
    void on_start_game_button_clicked();

    // Käsittelee kortin(nappulan) painalluksen.
    void handle_card_button_click();

    // Käsittelee avatun korttiparin.
    void process_pair_of_card_buttons();

    // Uudelleenkäynnistää pelin samoilla korttien ja pelaajien arvoilla.
    void on_restart_button_clicked();

    // Muuttaa pelaajien nimiä käyttäjän haluamaksi.
    void on_line_edit_editingFinished();

    // Laskee minuuttien ja sekunttien kulumista.
    void on_timer_timeout();

private:
    Ui::MainWindow *ui;

    // Käsittelee kortin(nappulan) painalluksen. Mikäli 2 korttia on käännetty suorittaa slotin process_pair_of_card_buttons.
    void handle_card_click();

    // Vakiot korttien geometrialle.
    const int CARD_BUTTON_WIDTH = 50;
    const int CARD_BUTTON_HEIGTH = 70;

    const std::string CARD_CHARACTER_STRING = "AABBCCDDEEFFGGHHIIJJKKLLMMNNOOPPQQRRSSTTUUVVWWXXYYZZ";
    const int DELAY = 1000;

    // timer ajanottoa varten.
    QTimer* timer;

    std::string card_string_;

    // Pelissä tarvittavat tietovarastot.
    std::vector<QPushButton*> button_vec_;
    std::map<QPushButton*, Card*> buttons_with_cards_;
    std::vector<Player* > players_;
    Game_board_type game_board_;

    Player* player_in_turn_;
    unsigned long int player_turn_counter_ = 0;
    int card_click_counter_ = 0;
    QString player_names_;

    unsigned int rows_;
    unsigned int columns_;

    // Korttien ja pelaajien lukumäärät oletusarvoilla.
    int number_of_cards_ = 2;
    int number_of_players_ = 1;
};
#endif // MAINWINDOW_HH
