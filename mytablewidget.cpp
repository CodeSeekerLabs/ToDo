/************
Autor:      CodeSeekerLabs

Projekt:    ToDo
            Eine einfache ToDoApp

Klasse:     MyTableWidget
            Erweitert die Klasse QTableWidget um ein ContextMenu und die Möglichkeit Zeilen zu verschieben

Datum:      10.09.2024
*************/

#include "mytablewidget.h"

#include "QMenu"
#include "QContextMenuEvent"
#include "QHeaderView"

MyTableWidget::MyTableWidget(QWidget *parent, QAction *b)
    : QTableWidget(parent)
{
    task=nullptr;

    button=b;

    checkedFirst=false;

    connect(this, &QTableWidget::itemChanged, this, &MyTableWidget::tableChanged);
    //signale werden überall geblockt, wo veränderungen an der Tabelle stattfinden
    //damit das Signal itemChanged nicht zufällig durch eine andere funktion ausgelöst wird

    //header mit methode sortieren verknüpfen
    connect(horizontalHeader(), &QHeaderView::sectionClicked, this, &MyTableWidget::sortieren);

    setAcceptDrops(true);//Drops zulassen
    setDragEnabled(true);//ziehen von items zulassen
    setDragDropMode(QAbstractItemView::InternalMove);//setzt den Drag-and-Drop-Modus auf interne Verschiebungen
    setDropIndicatorShown(true);//zeigt den Drop-Indikator an
    setSelectionBehavior(QAbstractItemView::SelectRows);//stellt sicher, dass ganze Zeilen ausgewählt werden

}

/*
 * eine Aufgabe löschen
 *
*/
void MyTableWidget::delTask(){
    blockSignals(true);

    qDebug() << "Action 1 triggered";

    qDebug()<<"Reihe:"<<currentRow()<<",  Spalte:"<<currentColumn();

    removeRow(currentRow());
    //item müssen nicht einzeln gelöscht werden removeRow() ruft destruktor aller items in der reihe auf

    if(button!=nullptr){
        button->setText("Speichern *");
    }

    blockSignals(false);
}


/*
 * Bei Veränderung in der Tabelle
 * @param item: Das Item das geändert wurde
*/
void MyTableWidget::tableChanged(QTableWidgetItem *item){
    blockSignals(true);

    //wenn item in der Spalte 0 ist, dann wurde der Status verändert
    if(item->column()==0){
        //Neuen Status abfragen
        if(item->checkState()==Qt::Checked){
            itemStrikeOut(*this->item(item->row(),1),true);//bei checked wird die aufgabe durchgestrichen
        }else{
            itemStrikeOut(*this->item(item->row(),1),false);//bei unchecked wird die aufgabe nicht durchgestrichen
        }
    }

    if(button!=nullptr){
        button->setText("Speichern *");
    }

    blockSignals(false);
}


/*
 * Aufgaben werden nach Status sortiert
 * @param logicalIndex: gibt die Spalte im Header an die angeklickt wurde
*/
void MyTableWidget::sortieren(int logicalIndex){
    blockSignals(true);

    //bei spalte 0 (Status)
    if (logicalIndex == 0) {

        int rowcount=rowCount();

        //die checkedFirst false ist
        if(!checkedFirst){
            //werden die Aufgaben so sortiert, dass die checked oben sind
            filterChecked(rowcount);
            filterUnchecked(rowcount);
            checkedFirst=true;
        }else{
            //sonst werden die AUfgaben so sortiert, dass die checked unten sind
            filterUnchecked(rowcount);
            filterChecked(rowcount);
            checkedFirst=false;
        }

        //alle alten zeilen entfernen
        for(int i=0; i<rowcount; i++){
            removeRow(0);//dazu immer die erste zeile entfernen bis alter rowCount erreicht ist
        }

    }

    blockSignals(false);
}

/*
 * bereits bearbeitete Aufgaben herausfiltern
 * @param rowcount: Zeilen anzahl die vor dem Filter vorgang war
*/
void MyTableWidget::filterChecked(const int& rowcount){
    blockSignals(true);
    QTableWidgetItem *ti = nullptr;
    QTableWidgetItem *value = nullptr;

    //alle Checked Aufgaben werden herausgefiltert und in der Tabelle hinten angefügt
    //danach sind checked Aufgaben doppelt vorhanden
    for(int i=0; i<rowcount; i++){
        if(item(i,0)->checkState()==Qt::Checked){
            setRowCount(rowCount()+1);
            value=new QTableWidgetItem();
            value->setCheckState(Qt::Checked);
            setItem(rowCount()-1,0,value);

            ti=new QTableWidgetItem(item(i,1)->text());
            setItem(rowCount()-1,1,ti);
            itemStrikeOut(*ti,true);
        }
    }
    blockSignals(false);
}


/*
 * unbearbeitete Aufgaben herausfiltern
 * @param rowcount: Zeilen anzahl die vor dem Filter vorgang war
*/
void MyTableWidget::filterUnchecked(const int& rowcount){
    blockSignals(true);
    QTableWidgetItem *ti = nullptr;
    QTableWidgetItem *value = nullptr;

    //alle Unchecked Aufgaben werden herausgefiltert und in der Tabelle hinten angefügt
    //danach sind Unchecked Aufgaben doppelt vorhanden
    for(int i=0; i<rowcount; i++){
        if(item(i,0)->checkState()==Qt::Unchecked){
            setRowCount(rowCount()+1);
            value=new QTableWidgetItem();
            value->setCheckState(Qt::Unchecked);
            setItem(rowCount()-1,0,value);

            ti=new QTableWidgetItem(item(i,1)->text());
            setItem(rowCount()-1,1,ti);
            itemStrikeOut(*ti,false);
        }
    }
    blockSignals(false);
}

/*
 * ein TableWidgetItem durchstreichen/ nicht durchstreichen
 * @param item: das Item, welches durchgestrichen werden soll
 * @param b: angabe ob das Item durchgestrichen werden soll oder nicht
*/
void MyTableWidget::itemStrikeOut(QTableWidgetItem& item, const bool& b){
    if(b){
        QFont font = item.font();
        font.setStrikeOut(true);
        item.setFont(font);
    }else{
        QFont font = item.font();
        font.setStrikeOut(false);
        item.setFont(font);
    }
}


/*
 * dragMoveEvent override
 * akzeptieren, dass Zeilen in der Tabelle verschoben werden können
 * @param event: das Drag-Move-Event
*/
void MyTableWidget::dragMoveEvent(QDragMoveEvent *event){
    event->accept();
}


/*
 * dropEvent override
 * steuert das Verhalten, wenn Zeilen in der Tabelle wieder abgelegt werden
 * @param event: das Drop-Event
*/
void MyTableWidget::dropEvent(QDropEvent *event){

    blockSignals(true);

    int dropRow=indexAt(event->position().toPoint()).row();//zeile, in der das elemnt abgelegt wurde
    int rowcount=rowCount();


    qDebug()<<dropRow<<";  "<<currentRow();

    if(dropRow<0){//ausserhalb der tabelle abgelegt
        return;
    }

    bool status=false,set=false;
    QString text="";

    QTableWidgetItem *ti = nullptr;
    QTableWidgetItem *value = nullptr;

    //werte aus der Zeile die Verschoben wird abspeichern
    if(item(currentRow(),0)->checkState()==Qt::Checked){
        status=true;
    }

    text=item(currentRow(),1)->text();

    //Zeile die Verschoben wird Löschen
    removeRow(currentRow());

    for(int i=0; i<rowcount-1; i++){//bis rowcount -1 zählen da bereit eine Zeile fehlt

        //Wenn die Zeile erreicht wird, in der die Aufgabe abgelegt wurde,
        //wird diese mit dem Status dort eingefügt
        if(i==dropRow){
            setRowCount(rowCount()+1);
            ti=new QTableWidgetItem(text);
            setItem(rowCount()-1,1,ti);

            value=new QTableWidgetItem();

            if(status){
                value->setCheckState(Qt::Checked);
                itemStrikeOut(*ti,true);
            }else{
                value->setCheckState(Qt::Unchecked);
                itemStrikeOut(*ti,false);
            }

            setItem(rowCount()-1,0,value);

            set=true;//speichern das die Aufgabe in eine Zeile gesetzt wurde
        }

        //alle Aufgaben werden mit ihrem Status neu hinten in die Tabelle eingefügt
        setRowCount(rowCount()+1);
        ti=new QTableWidgetItem(item(i,1)->text());
        setItem(rowCount()-1,1,ti);

        value=new QTableWidgetItem();

        if(item(i,0)->checkState()==Qt::Checked){
            value->setCheckState(Qt::Checked);
            itemStrikeOut(*ti,true);
        }else{
            value->setCheckState(Qt::Unchecked);
            itemStrikeOut(*ti,false);
        }

        setItem(rowCount()-1,0,value);

    }


    //Wenn die Aufgabe nicht gesetzt wurde, gab es keine Zeile innerhalb der Tabelle, in der die Aufgabe abgelegt worde ist
    //in diesem Fall wird die Aufgabe hinten angefügt
    if(!set){
        setRowCount(rowCount()+1);
        ti=new QTableWidgetItem(text);
        setItem(rowCount()-1,1,ti);

        value=new QTableWidgetItem();

        if(status){
            value->setCheckState(Qt::Checked);
            itemStrikeOut(*ti,true);
        }else{
            value->setCheckState(Qt::Unchecked);
            itemStrikeOut(*ti,false);
        }

        setItem(rowCount()-1,0,value);
    }

    //alle alten zeilen entfernen
    for(int i=0; i<rowcount-1; i++){
        removeRow(0);//dazu immer die erste zeile entfernen
    }

    blockSignals(false);
}


/*
 * contextMenuEvent override
 * Context Menü zum löschen einer Aufgabe
 * @param event: event zum öffnen des Menüs
*/
void MyTableWidget::contextMenuEvent(QContextMenuEvent *event){
    task = itemAt(event->pos());

    if (task) {
        QMenu contextMenu(this);
        //ActionButton zum löschen
        QAction *action1 = new QAction("Aufgabe löschen", this);
        QIcon icon=style()->standardIcon(QStyle::SP_TrashIcon);
        action1->setIcon(icon);

        //ActionButton mit Methode verknüpfen
        connect(action1, &QAction::triggered, this, &MyTableWidget::delTask);

        //ActionButton dem Context Menü hinzufügen
        contextMenu.addAction(action1);

        //Context Menü an der stelle des Mauszeigers öffnen
        contextMenu.exec(event->globalPos());
    }
}










