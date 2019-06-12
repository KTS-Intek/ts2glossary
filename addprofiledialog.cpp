#include "addprofiledialog.h"
#include "ui_addprofiledialog.h"

AddProfileDialog::AddProfileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddProfileDialog)
{
    ui->setupUi(this);
}

AddProfileDialog::~AddProfileDialog()
{
    delete ui;
}

void AddProfileDialog::on_buttonBox_accepted()
{
    emit setThisName(ui->lineEdit->text());
}
