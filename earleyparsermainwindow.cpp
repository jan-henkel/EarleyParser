#include "earleyparsermainwindow.h"
#include "ui_earleyparsermainwindow.h"
#include "earleyparser.h"

EarleyParserMainWindow::EarleyParserMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EarleyParserMainWindow)
{
    ui->setupUi(this);
}

EarleyParserMainWindow::~EarleyParserMainWindow()
{
    delete ui;
}
