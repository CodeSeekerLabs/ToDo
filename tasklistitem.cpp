/************
Autor:      CodeSeekerLabs

Projekt:    ToDo
            Eine einfache ToDoApp

Klasse:     TaskListItem
            Erweitert die Klasse QListWidgetItem sodass es eine Referenz auf ein MyTableWidget enthält
            und die Möglichkeit die Liste als Json Objekt zurückzugeben oder sie aus einem Json Objekt zu erstellen

Datum:      10.09.2024
*************/

#include "tasklistitem.h"

#include <QJsonObject>

using namespace std;


TaskListItem::TaskListItem(const QString &text, QAction *b)
    :QListWidgetItem(text)
{

    table=new MyTableWidget(nullptr,b);
    table->setRowCount(0);
    table->setColumnCount(2);
    QStringList header= {"Status","Aufgabe"};
    table->setHorizontalHeaderLabels(header);
    table->setColumnWidth(0,10);
    table->setColumnWidth(1,350);
    table->hide();

    button=b;



}


TaskListItem::~TaskListItem(){
    qDebug()<<"~TaskListItem";

    delete table;

}


/*
 * Tabelle der Liste zurückgeben lassen
 * @return: Pointer auf die Tablle
*/
MyTableWidget* TaskListItem::getTable(){
    return table;
}


/*
 * eine Aufgabe der Liste hinzufügen
 * @param taskName: Name/Beschreibung der Aufgabe
*/
void TaskListItem::addTask(const QString& taskName){
    table->blockSignals(true);

    //Tabelle eine Zeile hinzufügen
    table->setRowCount(table->rowCount()+1);

    //TableItem erstellen und Aufgabe eintragen
    QTableWidgetItem *ti=new QTableWidgetItem(taskName);
    table->setItem(table->rowCount()-1,1,ti);

    //TableItem erstellen und CheckBox eintragen
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setCheckState(Qt::Unchecked);
    table->setItem(table->rowCount()-1,0,item);

    if(button!=nullptr){
        button->setText("Speichern *");
    }

    table->blockSignals(false);
}


/*
 * ein JSON Objekt aus der Liste erstellen
 * @return: Liste als Json Objekt
*/
QJsonObject TaskListItem::toJson(){
    table->blockSignals(true);

    //Json Objelt erstellen und Einträge vornehmen
    QJsonObject json;
    json["name"] = text();//Listenname
    json["count"]=table->rowCount();//Anzahl der Aufgaben

    //Aufgaben einfügen
    for(int i=0; i<table->rowCount(); i++){
        qDebug()<<table->item(i,1)->text();
        json[QString::number(i)+"tasktext"]=table->item(i,1)->text();//Aufgaben Text eintragen

        //Aufgaben Status eintragen
        if(table->item(i,0)->checkState()==Qt::Checked){
            json[QString::number(i)+"taskvalue"]=1;
        }else{
            json[QString::number(i)+"taskvalue"]=0;
        }
    }

    table->blockSignals(false);

    return json;
}


/*
 * Liste aus einem Json Objekt erzeugen
 * @param json: ein Json Objekt mit den Daten der Liste
*/
void TaskListItem::fromJson(const QJsonObject &json){
    table->blockSignals(true);

    setText("");
    setText(json["name"].toString());//Namen der Liste setzen
    table->setRowCount(json["count"].toInt());//Anzahl der Aufgaben lesen und Tabellenzeilen anlegen

    QTableWidgetItem *ti = nullptr;
    QTableWidgetItem *value = nullptr;

    //Items aus den Daten erzeugen und in die Tabelle eintragen
    for(int i=0; i<table->rowCount(); i++){

        ti=new QTableWidgetItem();
        value=new QTableWidgetItem();

        ti->setText(json[QString::number(i)+"tasktext"].toString());
        table->setItem(i,1,ti);

        if(json[QString::number(i)+"taskvalue"]==1){
            value->setCheckState(Qt::Checked);
            table->itemStrikeOut(*ti,true);
        }else{
            value->setCheckState(Qt::Unchecked);
            table->itemStrikeOut(*ti,false);
        }

        table->setItem(i,0,value);
    }

    table->blockSignals(false);
}

/*
 * eine List als String im CSV Format zurückgeben (mit Komma getrennt)
 * @return: String im csv format
*/
QString TaskListItem::toCsvString(){
    QString s="";

    s = text()+"\n";//Name der Liste

    for(int i=0; i<table->rowCount(); i++){
        qDebug()<<table->item(i,1)->text();
        s=s+table->item(i,1)->text()+",";//Aufgabe mit Komma eintragen

        //Status der Aufgabe als 1/0 eintragen
        if(table->item(i,0)->checkState()==Qt::Checked){
            s=s+"1\n";
        }else{
            s=s+"0\n";
        }
    }

    return s;
}

