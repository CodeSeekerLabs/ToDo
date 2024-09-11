/************
Autor:      CodeSeekerLabs

Projekt:    ToDo
            Eine einfache ToDoApp

Klasse:     MyTableWidget
            Erweitert die Klasse QTableWidget um ein ContextMenu und die Möglichkeit Zeilen zu verschieben

Datum:      10.09.2024
*************/

#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H


#include <QTableWidget>


class MyTableWidget: public QTableWidget
{
    Q_OBJECT
public:
    explicit MyTableWidget(QWidget *parent = nullptr, QAction *b=nullptr);

    void itemStrikeOut(QTableWidgetItem& item, const bool& b);


private slots:
    void delTask();
    void tableChanged(QTableWidgetItem *item);
    void sortieren(int logicalIndex);

protected:

    void contextMenuEvent(QContextMenuEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;




private:
    QTableWidgetItem* task;
    QAction *button;
    bool checkedFirst;

    void filterChecked(const int& rowcount);
    void filterUnchecked(const int& rowcount);

};

#endif // MYTABLEWIDGET_H
