/************
Autor:      CodeSeekerLabs

Projekt:    ToDo
            Eine einfache ToDoApp

Klasse:     MainWindow
            Erstellt das Hauptfenster mit einer ToolBar

Datum:      10.09.2024
*************/

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "QFile"
#include "QFileDialog"
#include "tasklistitem.h"
#include "QLayout"
#include "QJsonDocument"
#include "QJsonObject"
#include "QJsonArray"
#include "QJsonValue"
#include "QStandardPaths"
#include "QMessageBox"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Ein eigenes ListWidget erstellen
    l=new MyListWidget(this, ui->labelAktuelleListe, ui->actionSpeichern);
    l->setGeometry(30+138,90,221,541);
    l->setFont(QFont("Ubuntu",12));
    l->show();

    //Tabelle setzen
    t=nullptr;

    //Doppelklick verknüpfung auf ein ListItem
    connect(l,&MyListWidget::itemDoubleClicked,this, &MainWindow::changeTable);

    //CurrentListItem setzen
    currentList=nullptr;


    //layout von der Toolbar setzen, linksbündig
    QLayout *lay = ui->toolBar->layout();
    for(int i = 0; i < lay->count(); ++i){
        lay->itemAt(i)->setAlignment(Qt::AlignLeft);
    }

    //ActionButtons von der Toolbar mit Methoden verknüpfen
    connect(ui->actionSpeichern,&QAction::triggered,this, &MainWindow::speichern);
    connect(ui->actionListe_Exportieren,&QAction::triggered,this, &MainWindow::speichern_csv);

    //gespeicherte Listen laden
    laden();

}

MainWindow::~MainWindow()
{
    delete ui;
}



/*
 * eine neue Liste erstellen
 *
*/
void MainWindow::neueListe(){

    //wenn Listenname nicht leer ist
    if(ui->lineNeueListe->text()!=""){
        //neue Liste erstellen und als currentList setzen
        currentList=new TaskListItem(ui->lineNeueListe->text(),ui->actionSpeichern);

        //Liste dem Widget hinzufügen
        l->addItem(currentList);
        ui->lineNeueListe->clear();//eingabe leeren
        ui->labelAktuelleListe->setText(currentList->text());//Überschrift setzen auf Name der Liste
        ui->actionSpeichern->setText("Speichern *");//Speichern Symbol mit * versehen, um änderungen seit dem letzten Speichern zu markieren
        showTable();//Tabelle der Liste anzeigen
    }else{
        qDebug()<<"leere Liste";
    }
}


/*
 * eine neue Aufgabe der currentList hinzufügen
 *
*/
void MainWindow::neueAufgabe(){

    //Wenn im ListWidget min. eine Liste angelegt wurde
    if(l->count()>0){

        //Wenn Eingabe für neue Aufgabe nicht leer ist
        if(ui->lineNeueAufgabe->text()!=""){

            currentList->addTask(ui->lineNeueAufgabe->text());//der currentList eine Aufgabe hinzufügen
            showTable();//Tabelle miit Aufgaben anzeigen

        }else{
            qDebug()<<"leere Aufgabe";
        }

    }else{
        qDebug()<<"keine Liste";
    }

    ui->lineNeueAufgabe->clear();//eingabe leeren

}


/*
 * Tabelle der ausgewählten Liste anzeigen
 *
*/
void MainWindow::showTable(){
    //Tabelle auf die Tabelle der currentList setzen
    t=currentList->getTable();

    //signale blockieren, da während der ausführung der Methoden keine Signale von der Tabelle erzeugt werden sollen
    t->blockSignals(true);

    if(t->parentWidget()==nullptr){//wird getestet damit nicht immer beim anzeigen ausgeführt wird
        t->setParent(this);
    }

    t->setGeometry(144+260,90,481,541);
    t->show();
    ui->labelAktuelleListe->setText(currentList->text());

    t->blockSignals(false);
}


/*
 * Tabelle einer anderen Liste anzeigen
 *
*/
void MainWindow::changeTable(){
    qDebug()<<"Tabelle";

    //muss getestet werden, da es möglich ist, dass nach einem löschvorgang t auf nichts mehr zeigt
    if(t){
        t->hide();//Tabelle verstecken
    }

    currentList=(TaskListItem*)l->item(l->currentRow());//currentList auf das ListItem der ausgewählten reihe im ListWidget setzen

    qDebug()<<"Tabelle neu";
    showTable();//Tabelle neu anzeigen

}


/*
 * aktuellen stand der Listen in Json Datei speichern
 *
*/
void MainWindow::speichern(){
    // Der Pfad zum Verzeichnis, in dem sich die ausführbare Datei befindet
    QString appDirPath = QCoreApplication::applicationDirPath();

    //Json Datei erstellen und zum schreiben öffnen
    QFile file(appDirPath+"/object.json");
    if (file.open(QIODevice::WriteOnly)) {

        //Json array erstellen
        QJsonArray jsonArray;
        //Alle Listen in das Array speichern
        for(int i=0; i<l->count(); i++){
            jsonArray.append(((TaskListItem*)l->item(i))->toJson());//QJsonObject wird mit append in Value umgewandelt
        }

        //aus dem Array ein JsonDocument Objekt erstellen und in die Datei schreiben
        QJsonDocument doc(jsonArray);
        file.write(doc.toJson());
        file.close();
        ui->actionSpeichern->setText("Speichern");
    }
}


/*
 * gespeicherte Listen aus Json Datei lesen
 *
*/
void MainWindow::laden(){
    // Der Pfad zum Verzeichnis, in dem sich die ausführbare Datei befindet
    QString appDirPath = QCoreApplication::applicationDirPath();

    //Json Datei zum lesen öffnen
    QFile file(appDirPath+"/object.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data=file.readAll();//alle Daten als ByteArray auslesen
        QJsonDocument doc(QJsonDocument::fromJson(data));//als Json Document wandeln
        QJsonArray jsonArray = doc.array();//in Json Array wandeln
        QJsonArray::iterator it=jsonArray.begin();//Iterator erstellen um das Array durchzulaufen
        QJsonObject obj;

        //solange der iterator nicht das ende erreicht hat
        while(it!=jsonArray.end()){

            currentList=new TaskListItem("",ui->actionSpeichern);//neue (leere) Liste erstellen
            currentList->fromJson(it->toObject());//liste aus JsonIbject erstellen
            l->addItem(currentList);//Liste dem Widget hinzufügen

            it++;
        }

        //testen falls datei keine Listen enthält
        if(currentList!=nullptr){
            showTable();//letzte Liste als currentList anzeigen
        }

        file.close();
    }
}


/*
 * eine Liste als CSV Datei speichern
 *
*/
void MainWindow::speichern_csv(){

    QString path= QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);//Pfad für Dokumentenordner


    //Einen FileDialog öffnen, um Datei erstellen oder überschreiben zu können
    //Dateiname wird als QString gespeichert
    QString fileName= QFileDialog::getSaveFileName(this, "Save List as .csv", path+"/List.csv", ".csv Files (*.csv)");


    //Datei öffnen zum schreiben
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);//ein DatenStrom erstellen
        out<<currentList->toCsvString();//Liste als String mit Datenstrom schreiben
        file.close();
    }
}

/*
 * closeEvent override
 * Eventhandling wenn x gedrückt wird zum fenster schließen
*/
void MainWindow::closeEvent(QCloseEvent *event){
    //Wenn Änderungen vorgenommen wurden die nicht gespeichert sind wird neuer Dialog geöffnet
    if(ui->actionSpeichern->text().contains("*")){
        confirmationDialog();
    }

}


/*
 * Dialog mit Abfrage ob gespeichert werden soll
 *
*/
void MainWindow::confirmationDialog(){
    // Dialog MessageBox mit Question erstellen
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Speichern?", "Möchten Sie Speichern bevor Sie das Programm schließen?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        speichern();
        // Wenn Yes zurückgegeben wird
        QMessageBox::information(this, "Bestätigung", "Ihre ToDo's wurden gespeichert!");
    } else {
        // Wenn No zurückgegeben wird
        QMessageBox::information(this, "Abgebrochen", "Ihre ToDo's wurden nicht gespeichert!");
    }
}







