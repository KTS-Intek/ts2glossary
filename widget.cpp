#include "widget.h"
#include "ui_widget.h"

#include <QFileDialog>
#include <QtXml>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QSettings>

#include "addprofiledialog.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    model = new QStandardItemModel(ui->tableView);


    ui->tableView->setModel(model);

    loadSett();

    connect(ui->cbxProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbxProfileCurrIndxChngd(int)) );

    onCbxProfileCurrIndxChngd(ui->cbxProfile->currentIndex());
}
//---------------------------------------------------------------------------------------------------
Widget::~Widget()
{
    saveLastSett();
    delete ui;
}
//---------------------------------------------------------------------------------------------------
void Widget::setThisName(QString name)
{
    name = name.simplified().trimmed();
    if(name.isEmpty()){
        showMess(tr("Name is not valid("));
        return;
    }

    bool isExists = false;
    if(true){
        QSettings settings("ZikranetsBohdan", "ts2glossary");
        settings.beginGroup("lastTsFiles");
        isExists =(settings.value("profiles").toHash().contains(name));
        settings.endGroup();
    }

    if(isExists){
        showMess(tr("Profile with this name already exists"));
        ui->pbAddProfile->animateClick();
        return;
    }

    ui->cbxProfile->addItem(name);
    ui->cbxProfile->setCurrentIndex(ui->cbxProfile->findText(name));

    if(true){
        QSettings settings("ZikranetsBohdan", "ts2glossary");
        settings.beginGroup("lastTsFiles");
        QVariantHash hash = settings.value("profiles").toHash();

        if(true){
            QVariantHash h = hash.value(ui->cbxProfile->currentText()).toHash();
            h.insert("lastTsFiles",  ui->plainTextEdit->toPlainText());
            h.insert("lastGlossDir", lastGlossDir);
            h.insert("lastTsDir", lastTsDir);

            h.insert("le2", ui->lineEdit_2->text());
            h.insert("le3", ui->lineEdit_3->text());

            hash.insert(name, h);
        }
        settings.setValue("profiles", hash);
        settings.endGroup();
    }

}
//---------------------------------------------------------------------------------------------------
void Widget::loadSett()
{
    QString lastProfileName;
    QVariantHash hash;

    if(true){
        QSettings settings("ZikranetsBohdan", "ts2glossary");
        settings.beginGroup("lastTsFiles");
        ui->plainTextEdit->setPlainText(settings.value("lastTsFiles").toString());
        lastGlossDir = settings.value("lastGlossDir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
        lastTsDir = settings.value("lastTsDir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();

        ui->lineEdit_2->setText(settings.value("le2").toString());
        ui->lineEdit_3->setText(settings.value("le3", "uk_UA").toString());

        lastProfileName = settings.value("profile").toString();
        hash = settings.value("profiles").toHash();

        settings.endGroup();

    }


    QList<QString> lk = hash.keys();

    if(lk.isEmpty()){
        //Solovjina uk_UA
        setThisName("Solovjina uk_UA");//init step

    }else{
        std::sort(lk.begin(), lk.end());

        int indx = -1;
        for(int i = 0, iMax = lk.size(); i < iMax; i++){
            if(indx < 0 && lastProfileName == lk.at(i))
                indx = i;
            ui->cbxProfile->addItem(lk.at(i));
        }
    }



}

//---------------------------------------------------------------------------------------------------
void Widget::onCbxProfileCurrIndxChngd(int indx)
{
    if(indx < 0)
        return;

    QVariantHash h;

    if(true){
        QSettings settings("ZikranetsBohdan", "ts2glossary");
        settings.beginGroup("lastTsFiles");
        settings.setValue("profile", ui->cbxProfile->currentText());


        h = settings.value("profiles").toHash().value(ui->cbxProfile->currentText()).toHash();

        settings.endGroup();

    }


    ui->plainTextEdit->setPlainText(h.value("lastTsFiles").toString());
    lastGlossDir = h.value("lastGlossDir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    lastTsDir = h.value("lastTsDir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();

    ui->lineEdit_2->setText(h.value("le2").toString());
    ui->lineEdit_3->setText(h.value("le3", "uk_UA").toString());


}
//---------------------------------------------------------------------------------------------------
void Widget::showMess(QString mess)
{
    QMessageBox::information(this, "", mess);

}
//---------------------------------------------------------------------------------------------------
void Widget::on_pushButton_clicked()
{
    //open TS file

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open TS file"), lastTsDir, tr("TS Files (*.ts *.qph)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    lastTsDir = fi.absoluteDir().path();
    QStringList l = ui->plainTextEdit->toPlainText().split("\n");
    if(!l.contains(fileName))
        ui->plainTextEdit->appendPlainText(fileName);

}
//---------------------------------------------------------------------------------------------------
void Widget::on_pushButton_2_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save glossary file"), lastGlossDir, tr("Glossary Files (*.qph)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    lastGlossDir = fi.absoluteDir().path();
    ui->lineEdit_2->setText(fileName);

    saveLastSett();

}

void Widget::on_pushButton_3_clicked()
{

    QString fileName = ui->lineEdit_3->text().simplified().trimmed();
    if(fileName.isEmpty() || !fileName.contains("_") || fileName.contains(" ")){
        QMessageBox::critical(this, "", tr("Lang not valid("));

        return;
    }
    fileName = ui->lineEdit_2->text().simplified().trimmed();

    if(fileName.right(4).toLower() != ".qph")
        fileName.append(".qph");

    QFileInfo fi(fileName);
    if(fi.exists()){

        QFile file(fileName);
        if(!file.open(QFile::ReadOnly|QFile::Text)){
            qDebug() << "can't open file " << file.errorString();
            QMessageBox::critical(this, "", tr("can't open file %1").arg(file.errorString()));
            return;
        }
        QXmlStreamReader xml(&file);

        while(!xml.atEnd() && !xml.hasError()){
            QXmlStreamReader::TokenType token = xml.readNext();
            if(token == QXmlStreamReader::StartDocument){
                continue;
            }



            if(token == QXmlStreamReader::StartElement){
                if(xml.name() != "phrase" )
                    continue;

                 QString lastKey, lastValue;
                 xml.readNext();

                 while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "message")){
                     if(xml.tokenType() == QXmlStreamReader::StartElement){
                         if(xml.name() == "source"){
                             xml.readNext();
                             lastKey = xml.text().toString();
                         }else{
                             if(xml.name() == "target"){
                                 xml.readNext();
                                 lastValue = xml.text().toString();
                             }
                         }
                     }
                     xml.readNext();
                 }

                 if(!lastKey.isEmpty() && !lastValue.isEmpty())
                     mapSource2translation.insert(lastKey, lastValue);

            }
            xml.readNext();
        }
        file.close();

    }

    int counter = 0;

    QSaveFile sFile(fileName);
    if(sFile.open(QSaveFile::WriteOnly)){
        QTextStream stream(&sFile);
        stream << "<!DOCTYPE QPH>" << endl;
        stream << QString("<QPH language=\"%1\">").arg(ui->lineEdit_3->text().simplified().trimmed()) << endl;

        QList<QString> lk = mapSource2translation.keys();
        for(int i = 0, imax = lk.size(); i < imax; i++){
            stream << "<phrase>" << endl;
            stream << QString("<source>%1</source>").arg(lk.at(i).toHtmlEscaped()) << endl;
            stream << QString("<target>%1</target>").arg(mapSource2translation.value(lk.at(i)).toHtmlEscaped()) << endl;
            stream << "</phrase>" << endl;
            counter++;
        }

        stream << "</QPH>" << endl;

        bool r = sFile.commit();
        QMessageBox::critical(this, "", r ? tr("OK") : tr("Error: %1").arg(sFile.errorString()));

        qDebug() << "sFile " << sFile.fileName();
    }
    qDebug() << "saved counter=" << counter;
    saveLastSett();
}

void Widget::on_pushButton_4_clicked()
{

    QStringList l = ui->plainTextEdit->toPlainText().split("\n");
    model->clear();
    mapSource2translation.clear();
    model->setHorizontalHeaderLabels(tr("Source,Translation").split(","));
    int counter = 0;
    for(int f = 0, fmax = l.size(); f < fmax; f++){

        QString fileName = l.at(f);

        QFile file(fileName);
        qDebug() << "open file " << file.fileName();
        if(!file.open(QFile::ReadOnly|QFile::Text)){
            qDebug() << "can't open file " << file.errorString();
            QMessageBox::critical(this, "", tr("can't open file %1").arg(file.errorString()));
            return;
        }

        bool isDictionary = (fileName.right(4).toLower() == ".qph");
        QXmlStreamReader xml(&file);

        QString startTransltn = isDictionary ? "phrase" : "message";
        QString trancltn = isDictionary ? "target" : "translation";

        while(!xml.atEnd() && !xml.hasError()){
            QXmlStreamReader::TokenType token = xml.readNext();
            if(token == QXmlStreamReader::StartDocument){
                continue;
            }

            if(token == QXmlStreamReader::StartElement){
                if(xml.name() != startTransltn)
                    continue;

                QString lastKey, lastValue;
                //            QString s = xml.v
                xml.readNext();

                while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == startTransltn)){
                    if(xml.tokenType() == QXmlStreamReader::StartElement){
                        if(xml.name() == "source"){
                            xml.readNext();
                            lastKey = xml.text().toString();
                        }else{
                            if(xml.name() == trancltn){
                                xml.readNext();
                                lastValue = xml.text().toString();
                            }
                        }
                    }
                    xml.readNext();
                }

                if(!lastKey.isEmpty() && !lastValue.isEmpty()){
                    if(mapSource2translation.contains(lastKey))
                        continue;
                    QList<QStandardItem*> li ;
                    li.append(new QStandardItem(lastKey));
                    li.append(new QStandardItem(lastValue));
                    model->appendRow(li);
                    mapSource2translation.insert(lastKey, lastValue);
                    counter++;
                }

            }
        }
        file.close();

    }
    qDebug() << "counter= " << counter;
    saveLastSett();
}

//---------------------------------------------------------------------------------------------------

void Widget::saveLastSett()
{
    QSettings settings("ZikranetsBohdan", "ts2glossary");
    settings.beginGroup("lastTsFiles");
    settings.setValue("lastTsFiles",  ui->plainTextEdit->toPlainText());
    settings.setValue("lastGlossDir", lastGlossDir);
    settings.setValue("lastTsDir", lastTsDir);

    settings.setValue("le2", ui->lineEdit_2->text());
    settings.setValue("le3", ui->lineEdit_3->text());

    settings.setValue("profile", ui->cbxProfile->currentText());


    QVariantHash hash = settings.value("profiles").toHash();

    if(true){
        QVariantHash h = hash.value(ui->cbxProfile->currentText()).toHash();
        h.insert("lastTsFiles",  ui->plainTextEdit->toPlainText());
        h.insert("lastGlossDir", lastGlossDir);
        h.insert("lastTsDir", lastTsDir);

        h.insert("le2", ui->lineEdit_2->text());
        h.insert("le3", ui->lineEdit_3->text());

        hash.insert(ui->cbxProfile->currentText(), h);
    }
    settings.setValue("profiles", hash);

    settings.endGroup();
}
//---------------------------------------------------------------------------------------------------

void Widget::on_pushButton_5_clicked()
{
    ui->plainTextEdit->clear();
}
//---------------------------------------------------------------------------------------------------
void Widget::on_pbToCsv_clicked()
{
    QString fileName = ui->lineEdit_3->text().simplified().trimmed();
    if(fileName.isEmpty() || !fileName.contains("_") || fileName.contains(" ")){
        QMessageBox::critical(this, "", tr("Lang not valid("));

        return;
    }
    fileName = ui->lineEdit_2->text().simplified().trimmed();

    if(fileName.right(4).toLower() != ".csv")
        fileName.append(".csv");




    QSaveFile sFile(fileName);
    if(sFile.open(QSaveFile::WriteOnly)){
        QTextStream stream(&sFile);
        stream << QString("<QPH language=\"%1\">").arg(ui->lineEdit_3->text().simplified().trimmed()) << endl;

        stream << QString("en_UK\tuk_UA\tSource") << endl;

        for(int i = 0, iMax = model->rowCount(); i < iMax; i++){
            stream << "" << "\t" + model->item(i,1)->text().replace("\t", ";;;").replace("\r", "|||").replace("\n", "___").simplified() << "\t" + model->item(i,0)->text().replace("\t",";;;").replace("\r", "|||").replace("\n", "___").simplified() << endl;

        }
        bool r = sFile.commit();
        QMessageBox::critical(this, "", r ? tr("OK") : tr("Error: %1").arg(sFile.errorString()));

        qDebug() << "sFile " << sFile.fileName();
    }
    saveLastSett();
}
//---------------------------------------------------------------------------------------------------

void Widget::on_pbAddProfile_clicked()
{
    AddProfileDialog *d = new AddProfileDialog(this);
    connect(d, SIGNAL(setThisName(QString)), this, SLOT(setThisName(QString)));
    d->exec();
    d->deleteLater();
}
//---------------------------------------------------------------------------------------------------
void Widget::on_pbDelProfile_clicked()
{
    if(ui->cbxProfile->count() < 2){
        showMess(tr("Not allowed("));
        return;
    }

    if(true){
        QSettings settings("ZikranetsBohdan", "ts2glossary");
        settings.beginGroup("lastTsFiles");


        QVariantHash hash = settings.value("profiles").toHash();
        hash.remove(ui->cbxProfile->currentText());

        settings.setValue("profiles", hash);

        settings.endGroup();

    }


    ui->cbxProfile->removeItem(ui->cbxProfile->currentIndex());
    ui->cbxProfile->setCurrentIndex(0);
}

void Widget::on_tbSaveSett_clicked()
{
    saveLastSett();
}
