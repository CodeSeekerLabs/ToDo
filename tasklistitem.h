/************
Autor:      CodeSeekerLabs

Projekt:    ToDo
            Eine einfache ToDoApp

Klasse:     TaskListItem
            Erweitert die Klasse QListWidgetItem sodass es eine Referenz auf ein MyTableWidget enthält
            und die Möglichkeit die Liste als Json Objekt zurückzugeben oder sie aus einem Json Objekt zu erstellen

Datum:      10.09.2024
*************/

#ifndef TASKLISTITEM_H
#define TASKLISTITEM_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QJsonObject>

#include "mytablewidget.h"

using namespace std;

class TaskListItem: public QListWidgetItem
{
public:
    TaskListItem(const QString &text, QAction *b=nullptr);
    ~TaskListItem();

    MyTableWidget* getTable();
    void addTask(const QString& taskName);
    QJsonObject toJson();
    void fromJson(const QJsonObject &json);
    QString toCsvString();

private:
    MyTableWidget *table;
    QAction *button;
};

#endif // TASKLISTITEM_H
