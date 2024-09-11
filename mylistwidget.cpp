/************
Autor:      CodeSeekerLabs

Projekt:    ToDo
            Eine einfache ToDoApp

Klasse:     MyListWidget
            Erweitert die Klasse QListWidget um ein ContextMenu

Datum:      10.09.2024
*************/

#include "mylistwidget.h"

#include "QMenu"
#include "QContextMenuEvent"
#include "QLabel"
#include "QInputDialog"

MyListWidget::MyListWidget(QWidget *parent, QLabel *aL, QAction *b)
    : QListWidget(parent)
{
    QFrame::setFrameShape(QFrame::Panel);
    QFrame::setFrameShadow(QFrame::Sunken);
    QFrame::setLineWidth(2);

    i=nullptr;
    currentList=nullptr;

    listName=aL;
    button=b;

}


/*
 * eine Liste löschen
 *
*/
void MyListWidget::delItem(){
    qDebug() << "Action 1 triggered";
    //Tabelle der ausgewählten Liste verstecken
    i->getTable()->hide();
    delete i;//ausgewählte Liste löschen
    i=nullptr;

    currentList=(TaskListItem*)item(0);//currentList auf ersteListe setzen

    //Überschrift entsprechend der currentList anzeigen
    if(currentList!=nullptr){
        listName->setText(currentList->text());
    }else{
        listName->setText("");
    }

    button->setText("Speichern *");


}

/*
 * den Namen einer Liste ändern
 *
*/
void MyListWidget::changeName(){
    qDebug() << "Action 2 triggered";

    //currentList auf die ausgewählte Liste setzen
    currentList=(TaskListItem*)currentItem();

    //Testen ob eine Liste vorhanden ist
    if (currentList) {
        bool ok;

        //neuen InputDialog öffnen um neuen Namen der Liste einzugeben
        QString newText = QInputDialog::getText(this->parentWidget(), "Name der Liste Ändern",
                                                "Liste:", QLineEdit::Normal,
                                                currentList->text(), &ok);
        //wenn ok zurückgegeben wird und der neue Name nicht leer ist, kann dieser geändert werden
        if (ok && !newText.isEmpty()) {
            currentList->setText(newText);

            listName->setText(currentList->text());

            if(button!=nullptr){
                button->setText("Speichern *");
            }
        }

    }else{
        qDebug()<<"kein Item";
    }

}


/*
 * contextMenuEvent override
 * Context Menü zum löschen einer Liste und zum Ändern eines Listennamens
 * @param event: event zum öffnen des Menüs
*/
void MyListWidget::contextMenuEvent(QContextMenuEvent *event){
    i = (TaskListItem*)itemAt(event->pos());

    if (i) {
        QMenu contextMenu(this);

        //ActionButton zum löschen einer Liste
        QAction *action1 = new QAction("Liste Löschen", this);
        QIcon icon=style()->standardIcon(QStyle::SP_TrashIcon);
        action1->setIcon(icon);
        //ActionButton zum ändern eines Namens
        QAction *action2 = new QAction("Umbenennen ...", this);
        icon=style()->standardIcon(QStyle::SP_ArrowForward);
        action2->setIcon(icon);

        //ActionButtons mit Mehtoden verknüpfen
        connect(action1, &QAction::triggered, this, &MyListWidget::delItem);
        connect(action2, &QAction::triggered, this, &MyListWidget::changeName);

        //ActionButtons dem Context Menü hinzufügen
        contextMenu.addAction(action1);
        contextMenu.addAction(action2);

        //Context Menü öffnen an der stelle des Mauszeigers
        contextMenu.exec(event->globalPos());
    }
}
