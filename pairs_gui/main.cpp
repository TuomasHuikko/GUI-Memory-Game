/* Ohjelman kirjoittaja
 * Nimi: Tuomas Huikko
 * Opiskelijanumero: H291873
 * Käyttäjätunnus: gmtuhu
 * E-Mail: tuomas.huikko@tuni.fi
 * */

#include "mainwindow.hh"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
