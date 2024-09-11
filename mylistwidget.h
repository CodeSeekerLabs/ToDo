/************
Autor:      CodeSeekerLabs

Projekt:    ToDo
            Eine einfache ToDoApp

Klasse:     MyListWidget
            Erweitert die Klasse QListWidget um ein ContextMenu

Datum:      10.09.2024
*************/

#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QWidget>
#include "QListWidget"
#include "QLabel"
#include "tasklistitem.h"



class MyListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit MyListWidget(QWidget *parent = nullptr, QLabel *aL=nullptr, QAction *b=nullptr);

protected:

    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    TaskListItem *i;
    TaskListItem *currentList;

    QLabel *listName;
    QAction *button;



    void delItem();
    void changeName();

};

#endif // MYLISTWIDGET_H
