/* Ohjelman kirjoittaja
* Nimi: Tuomas Huikko
* Opiskelijanumero: H291873
* Käyttäjätunnus: gmtuhu
* E-Mail: tuomas.huikko@tuni.fi
* */


#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include "card.hh"
#include <random>
#include <algorithm>
#include <vector>
#include <QString>
#include <QGridLayout>
#include <QGroupBox>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      timer(new QTimer(this))
{
    ui->setupUi(this);

    // Start Game ja Restart nappulat otetaan pois käytöstä.
    ui->start_game_button->setDisabled(true);
    ui->restart_button->setDisabled(true);

    timer->setInterval(DELAY);

    connect(timer, &QTimer::timeout, this, &MainWindow::on_timer_timeout);

    // Asetetaan ohjeteksti pelaajien syöttämiseen.
    QString instruction = "Enter the name of ";
    instruction += QString::number(number_of_players_);
    instruction += " players in the field below, separated by commas(,).";
    ui->textBrowser->setText(instruction);
}

MainWindow::~MainWindow()
{
    delete ui;

    for (auto row : game_board_){
        for (Card* card : row){
            delete card;
        }
    }

    for (auto player : players_){
        delete player;
    }
}


void MainWindow::init_cards()
{
    rows_ = game_board_.size();
    columns_ = game_board_.at(0).size();

    // Siemenluku otetaan tietokoneen kellosta.
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine random_engine_(seed);
    std::uniform_int_distribution<int> distribution_(0, rows_ * columns_ - 1);

    // Arvotaan satunnaislukugeneraattorille yksi tyhjä arvo.
    distribution_(random_engine_);

    // Otetaan pelissä käytettävien korttien arvot ja sekoitetaan ne.
    card_string_ = CARD_CHARACTER_STRING.substr(0,number_of_cards_);
    shuffle(card_string_.begin(), card_string_.end(), random_engine_);


    int index = 0;
    for(unsigned int r=0; r < rows_; ++r)
    {

        for (unsigned int c= 0; c< columns_; ++c){


        char letter = card_string_.at(index);

        // Asetetaan kortille arvottu kirjain sekä näkyvyys(piilotettu).
        game_board_.at(r).at(c)->set_letter(letter);
        game_board_.at(r).at(c)->set_visibility(HIDDEN);

        ++index;
        }

    }

}

void MainWindow::init_cards_gui()
{

    QGridLayout* layout = ui->gridLayout;

    unsigned int index = 0;
    int row_counter = 0;
    int column_counter = 0;

        for(auto row : game_board_){

            for(Card* card : row){

                QString letter = "";

                // Luodaan uusi pushButton ja alustetaan se.
                QPushButton* pushButton = new QPushButton(letter, this);
                pushButton->setFixedWidth(CARD_BUTTON_WIDTH);
                pushButton->setFixedHeight(CARD_BUTTON_HEIGTH);
                pushButton->setStyleSheet("background-color: red");

                // Lisätään nappula ja sitä vastaava kortti tietovarastoihin.
                button_vec_.push_back(pushButton);
                buttons_with_cards_.insert({pushButton, card});

                // Lisätään nappula layouttiin.
                layout->addWidget(pushButton, row_counter, column_counter);

                connect(pushButton, &QPushButton::clicked,
                    this, &MainWindow::handle_card_button_click);



                ++index;
                ++column_counter;
            }
            ++row_counter;
            column_counter = 0;
        }


}

void MainWindow::init_with_empty_cards()
{
    game_board_.clear();
    Game_row_type row;

    for(unsigned int r = 0; r < rows_; ++r){
        for(unsigned int c = 0; c < columns_; ++c)
        {
            Card* card = new Card;

            row.push_back(card);
        }
        game_board_.push_back(row);
        row.clear();
    }

}

void MainWindow::calculate_factors(unsigned int &smaller_factor, unsigned int &bigger_factor)
{
    unsigned int product = number_of_cards_;

    for(unsigned int i = 1; i * i <= product; ++i)
    {
        if(product % i == 0)
        {
            smaller_factor = i;
        }
    }
    bigger_factor = product / smaller_factor;

}

void MainWindow::process_pair_of_cards(Card* card_1, Card* card_2)
{
    print_score_board();

    // Korttien kirjaimet samat->pari löydetty (Lisätään kortit pelaajalle).
    if (card_1->get_letter() == card_2->get_letter())
    {


        // Haetaan tietorakenteesta oikeat kortit.
        for (auto pair : buttons_with_cards_){

            // Kortti 1 löydetty.
            if (pair.second == card_1 and pair.second->get_visibility()==OPEN){

                pair.second->turn();               
                pair.first->setVisible(false);

            }
            //Kortti 2 löydetty.
            if (pair.second == card_2 and pair.second->get_visibility()==OPEN){

                pair.second->turn();                
                pair.first->setVisible(false);

            }


        }
        player_in_turn_->add_card(* card_1, * card_2);
        print_score_board();

    }

    // Kirjaimet ei samat->paria ei löydetty (Kortit käännetään takaisin ympäri).
    else
    {

        // Vaihdetaan vuoroa, jos pelaajia on enemmän kuin yksi.
        if (not (players_.size()==1)){

        ++player_turn_counter_;
        }

        // Etsitään taas oikeat kortit.
        for (auto pair : buttons_with_cards_){

            if (pair.second == card_1 and pair.second->get_visibility()==OPEN){

                card_1->turn();
                pair.first->setDisabled(false);
                pair.first->setText("");
                pair.first->setStyleSheet("background-color: red");
            }

            if (pair.second == card_2 and pair.second->get_visibility()==OPEN){

                card_2->turn();
                pair.first->setDisabled(false);
                pair.first->setText("");
                pair.first->setStyleSheet("background-color: red");
            }


        }
        print_score_board();
    }

    // Tarkastellaan onko peli loppunut.
    if (is_game_over()){
        determine_and_print_winner();
        timer->stop();
    }

    // Poistetaan korttien painamisen esto.
    ui->gridLayoutWidget->setDisabled(false);

}

std::vector<Card*> MainWindow::find_turned_cards()
{
    std::vector<Card *> turned_cards;

    // Etsitään painetut kortit.
    for (auto pair : buttons_with_cards_){

        // Jos kortti on käännetty lisätään se palautusvektoriin.
        if (pair.second->get_visibility()==OPEN){
            turned_cards.push_back(pair.second);
        }

    }


    return turned_cards;
}

void MainWindow::init_players()
{

    // Lista pelaajien nimistä.
    QStringList name_list = player_names_.split(QLatin1Char(','));

    // Poistetaan tyhjät paikat, jos niitä on.
    for (auto name : name_list){
        if (name == ""){
            name.clear();
        }
    }

    // Lisätään pelaajat rakenteeseen players_ yksi kerrallaan.
    for (int i=0; i< number_of_players_; ++i){

        QString name = name_list.at(i);
        Player* player = new Player(name.toStdString());

        players_.push_back(player);
    }

    // Ensimmäinen pelaaja vuorossa.
    player_in_turn_ = players_.at(0);
}

void MainWindow::enable_start_button_if_possible()
{

    // Pelaajien nimiä ei syötetty.
    if (player_names_ == ""){
        return;
    }

    QStringList name_list = player_names_.split(QLatin1Char(','));

    // Poistetaan tyhjät paikat, jos niitä on.
    for (auto name : name_list){
        if (name == ""){
            name.clear();
        }
    }

    // Pelaajien nimien määrä ei vastaa pelaajien määrää.
    if (name_list.size()<number_of_players_){
        QString return_string = "Too few player's names entered!";
        ui->textBrowser->setText(return_string);
        return;
    }

    // Korttien määrä on liian suuri.
    else if (number_of_cards_ > 52){
        QString return_string = "Maximum number of cards is 52!";
        ui->textBrowser->setText(return_string);
        return;
    }

    // Korttien määrä ei ole parilllinen.
    else if ((number_of_cards_ % 2) != 0){
        QString return_string = "Number of cards must be an even number!";
        ui->textBrowser->setText(return_string);
        return;
    }

    // Otetaan käyttöön pelin aloitusnappula ja otetaan pois käytöstä uusien arvojen syöttäminen.
    ui->start_game_button->setDisabled(false);
    ui->line_edit->setDisabled(true);
    ui->card_spin_box->setDisabled(true);
    ui->player_spin_box->setDisabled(true);
}

void MainWindow::print_score_board()
{
    // Siirrytään takaisin ensimmmäiseen pelaajaan.
    if (player_turn_counter_== players_.size()){
        player_turn_counter_ = 0;

    }

    // Otetaan vuorossa oleva pelaaja rakenteesta.
    player_in_turn_ = players_.at(player_turn_counter_);

    // Luodaan tulostettavaa lausetta.
    QString score_string = "Now Playing: ";
    score_string += QString::fromStdString(player_in_turn_->get_name());
    score_string += " with ";

    // Jos pelaajalla on 0 paria lisätään nolla, muuten parien määärä.
    if (player_in_turn_->number_of_pairs() != 0){
    score_string += QString::number(player_in_turn_->number_of_pairs());
    }
    else{
        score_string += QString::number(0);
    }
    score_string += " points.";

    // Asetetaan lause.
    ui->score_text_browser->setText(score_string);
}

bool MainWindow::is_game_over()
{
    int empty_cards = 0;

    // Katsotaan montako tyhjää korttia pöydällä on.
    for (auto pair : buttons_with_cards_){

        if (not pair.first->isVisible()){
            ++empty_cards;
        }


    }

    // Kaikki kortit ovat tyhjiä.
    if (empty_cards == number_of_cards_){
        return true;
    }
    // Kaikki kortit eivät ole tyhjiä.
    else{
        return false;
    }

}

void MainWindow::determine_and_print_winner()
{

    unsigned int most_pairs;
    int same_most_pairs = 0;
    QString winner = "";
    int winning_pairs;

    // Alustetaan suurin parien määrä ensimmäisen pelaajan mukaan.
    most_pairs = players_.at(0)->number_of_pairs();

    // Määritetään suurin parien määrä silmukassa.
    for (int index=1; index<number_of_players_; ++index)
    {
        // Päivittää suurinta parien määrää, jos pelaajalla on enemmän pareja
        // kuin edellinen suurin parien määrä.
        if (players_.at(index)->number_of_pairs() > most_pairs)
        {
            most_pairs = players_.at(index)->number_of_pairs();
        }

    }

    // Määritetään onko useammalla pelaajalla suurin määrä pareja=tasapeli.
    for (Player* player : players_)
    {
        if (player->number_of_pairs() == most_pairs)
        {
            same_most_pairs += 1;
            // Voittajan nimi ja parien määrä.
            winner = QString::fromStdString(player->get_name());
            winning_pairs = player->number_of_pairs();
        }
    }
    // Toteutuu vain jos suurin määrä pareja on yhdellä pelaajalla=yksi voittaja.
    if (same_most_pairs == 1)
    {
        QString win_string = "Game over! ";
        win_string += winner;
        win_string += " has won with ";
        win_string += QString::number(winning_pairs);
        win_string += " pairs.";

        ui->score_text_browser->setText(win_string);
    }
    // Suurin määrä pareja usealla pelaajalla = tasapeli.
    else
    {
        QString tie_string = "Game over! ";
        tie_string += "Tie of ";
        tie_string += QString::number(same_most_pairs);
        tie_string += " players with ";
        tie_string += QString::number(most_pairs);
        tie_string += " pairs.";

        ui->score_text_browser->setText(tie_string);
    }

}










void MainWindow::on_card_spin_box_valueChanged(int arg1)
{
    number_of_cards_ = arg1;

    // Otetaan Start Game nappula käyttöön jos mahdollista.
    enable_start_button_if_possible();
}

void MainWindow::on_player_spin_box_valueChanged(int arg1)
{
    number_of_players_ = arg1;

    // Asetetaan ohjeteksti pelaajien syöttämiseen myös pelaajien määrän muuttamisen jälkeen.
    QString instruction = "Enter the name of ";
    instruction += QString::number(number_of_players_);
    instruction += " players in the field below, separated by commas(,).";
    ui->textBrowser->setText(instruction);

    // Otetaan Start Game nappula käyttöön jos mahdollista.
    enable_start_button_if_possible();
}

void MainWindow::on_start_game_button_clicked()
{

    // Otetaan Start Game nappula pois käytöstä.
    ui->start_game_button->setDisabled(true);
    ui->start_game_button->setVisible(false);

    // Otetaan pois käytöstä arvojen muuttaminen.
    ui->card_spin_box->setDisabled(true);
    ui->player_spin_box->setDisabled(true);

    // Lasketaan rivien ja sarakkeiden määrä.
    calculate_factors(rows_, columns_);

    // Alustetaan pelialue.
    init_with_empty_cards();
    init_cards();
    init_players();  
    init_cards_gui();

    // Tyhjennetään ohje- ja pistekentät.
    ui->textBrowser->setText("");
    ui->score_text_browser->setText("");

    // Käynnistetään ajanotto ja otetaan käyttöön Restart nappula.
    timer->start();
    ui->restart_button->setDisabled(false);

    // Tulostetaan nykyinen pelaaja ja pisteet.
    print_score_board();

}

void MainWindow::handle_card_button_click()
{

    handle_card_click();
}

void MainWindow::process_pair_of_card_buttons()
{
    std::vector<Card *> turned_cards = find_turned_cards();
    process_pair_of_cards(turned_cards.at(0), turned_cards.at(1));
}

void MainWindow::handle_card_click()
{

    for(unsigned int i = 0; i < button_vec_.size(); ++i)
    {
        // Määritetään hiiren paikka napin painohetkellä.
        const QRect widget_geometry = button_vec_.at(i)->geometry();
        const QPoint global_pos = mapFromGlobal(QCursor::pos());
        const QPoint local_pos = QPoint(global_pos.x(), global_pos.y()-100);

        // Hiiri oli kyseisen napin sisällä.
        if(widget_geometry.contains(local_pos))
        {
            Card* card = buttons_with_cards_.at(button_vec_.at(i));

            print_score_board();
            card->turn();

            // Simuloidaan kortin kääntö graafisesti.
            QString letter = "";
            letter += card->get_letter();
            button_vec_.at(i)->setText(letter);
            button_vec_.at(i)->setStyleSheet("background-color: gray");
            button_vec_.at(i)->setDisabled(true);

            ++card_click_counter_;

        }


    }


    // Kahta nappia on painettu/kaksi korttia käännetty.
    if (card_click_counter_ == 2){
        card_click_counter_ = 0;

        // Otetaan pois käytöstä korttien painaminen.
        ui->gridLayoutWidget->setDisabled(true);

        QTimer::singleShot(DELAY, this, SLOT(process_pair_of_card_buttons()));
    }

}

void MainWindow::on_restart_button_clicked()
{

    // Poistetaan edelliset kortit ja pelaajat.
    for (auto row : game_board_){
        for (Card* card : row){
            delete card;
        }
    }

    for (auto player : players_){
        delete player;
    }

    // Nollataan ajanotto.
    ui->minutes_number->display(0);
    ui->seconds_number->display(0);

    // Aloitetaan ajanotto uudestaan.
    if (not timer->isActive()){
        timer->start();
    }

    rows_ = 0;
    columns_ = 0;
    card_string_.clear();

    // Alustetaan tietorakenteet tyhjennyksen jälkeen.
    calculate_factors(rows_, columns_);

    init_with_empty_cards();
    init_cards();

    buttons_with_cards_.clear();

    // Poistetaan edelliset nappulat.
    for(auto button : button_vec_){
        button->deleteLater();
    }
    button_vec_.clear();
    players_.clear();

    player_turn_counter_ = 0;

    card_click_counter_ = 0;

    init_players();

    init_cards_gui();

    ui->score_text_browser->setText("");

    print_score_board();
}

void MainWindow::on_line_edit_editingFinished()
{
    player_names_ = ui->line_edit->text();
    enable_start_button_if_possible();
}

void MainWindow::on_timer_timeout()
{
    int minutes = ui->minutes_number->intValue();
    int seconds = ui->seconds_number->intValue();

    if (seconds == 59){

        ui->minutes_number->display(minutes+1);
        ui->seconds_number->display(0);

    }
    else{

        ui->seconds_number->display(seconds+1);
    }
}
