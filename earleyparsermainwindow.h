#ifndef EARLEYPARSERMAINWINDOW_H
#define EARLEYPARSERMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class EarleyParserMainWindow;
}

class EarleyParserMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EarleyParserMainWindow(QWidget *parent = 0);
    ~EarleyParserMainWindow();

private:
    Ui::EarleyParserMainWindow *ui;
};

#endif // EARLEYPARSERMAINWINDOW_H
