#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtCore>
#include <QStandardItemModel>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void setThisName(QString name);

    void loadSett();


    void onCbxProfileCurrIndxChngd(int indx);

    void showMess(QString mess);



private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void saveLastSett();

    void on_pushButton_5_clicked();

    void on_pbToCsv_clicked();

    void on_pbAddProfile_clicked();

    void on_pbDelProfile_clicked();

    void on_tbSaveSett_clicked();

private:
    Ui::Widget *ui;
    QStandardItemModel *model;

    QString lastTsDir;
    QString lastGlossDir;

    QMap<QString,QString> mapSource2translation;

};

#endif // WIDGET_H
