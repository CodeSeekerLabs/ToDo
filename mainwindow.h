/************
Autor:      CodeSeekerLabs

Projekt:    ToDo
            Eine einfache ToDoApp

Klasse:     MainWindow
            Erstellt das Hauptfenster mit einer ToolBar

Datum:      10.09.2024
*************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mylistwidget.h"
#include "QTableWidget"
#include "tasklistitem.h"
#include "QPointer"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void neueListe();
    void neueAufgabe();
    void changeTable();
    void speichern(); 
    void speichern_csv();

protected:
    void closeEvent(QCloseEvent *event) override;



private:
    Ui::MainWindow *ui;
    TaskListItem *currentList;

    MyListWidget *l;
    QPointer<MyTableWidget> t;

    void laden();
    void showTable();
    void confirmationDialog();
};
#endif // MAINWINDOW_H
