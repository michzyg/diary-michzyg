#include "editdialog.h"
#include "ui_editdialog.h"

#include <QVariant>
#include <QFileDialog>
#include <QPainter>

EditDialog::EditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialog)
{
    ui->setupUi(this);
    connectSignals();

    ui->colorSelect->addItem("black", QColor(Qt::black));
    ui->colorSelect->addItem("red", QColor(Qt::red));
    ui->colorSelect->addItem("blue", QColor(Qt::blue));
    ui->colorSelect->setCurrentIndex(0);

    ui->formatSelect->addItem("normal", QVariant::fromValue(entry_text_format::normal));
    ui->formatSelect->addItem("bold", QVariant::fromValue(entry_text_format::bold));
    ui->formatSelect->addItem("italic", QVariant::fromValue(entry_text_format::italic));
    ui->formatSelect->setCurrentIndex(0);
}

EditDialog::~EditDialog()
{
    disconnectSignals();
    delete ui;
}

void EditDialog::connectSignals()
{
    connect(ui->add, &QPushButton::pressed, this, &EditDialog::AddEntry);
    connect(ui->cancel, &QPushButton::pressed, this, &EditDialog::Cancel);
    connect(ui->selectImage, &QPushButton::pressed, this, &EditDialog::selectImage);
}

void EditDialog::disconnectSignals()
{
    disconnect(ui->add, &QPushButton::pressed, this, &EditDialog::AddEntry);
    disconnect(ui->cancel, &QPushButton::pressed, this, &EditDialog::Cancel);
}

void EditDialog::ClearEntry()
{
    ui->selectedDate->setDateTime(QDateTime::currentDateTime());
    ui->textEdit->clear();
    ui->colorSelect->setCurrentIndex(0);
    ui->formatSelect->setCurrentIndex(0);

    m_current_map_path.clear();

    int w = ui->imagePreview->width();
    int h = ui->imagePreview->height();
    ui->imagePreview->setPixmap(m_current_map.scaled(w, h, Qt::KeepAspectRatio));
}

void EditDialog::AddEntry()
{

    emit sendEntry(
        ui->selectedDate->dateTime(),
        ui->textEdit->toPlainText(),
        ui->colorSelect->currentData().value<QColor>(),
        ui->formatSelect->currentData().value<entry_text_format>(),
        m_current_map_path
    );
    
    this->hide();
}

void EditDialog::Cancel()
{
    this->hide();
}

void EditDialog::selectImage()
{
    auto file_name = QFileDialog::getOpenFileName(this,
        tr("Choose Image"), "Desktop", tr("Image Files (*.png *.jpg *.bmp)"));

    m_current_map_path = file_name;
    m_current_map = QPixmap (file_name);
    int w = ui->imagePreview->width();
    int h = ui->imagePreview->height();
    ui->imagePreview->setPixmap(m_current_map.scaled(w, h, Qt::KeepAspectRatio));
}
