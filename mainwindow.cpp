#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "services_layer.h"

#include <QWheelEvent>
#include <QShortcut>
#include <QPainter>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    m_editDialogWindow(new EditDialog(this))
{
    ui->setupUi(this);
    ui->prev->setReadOnly(true);
    ui->current->setReadOnly(true);
    ui->next->setReadOnly(true);

    this->setFocusPolicy(Qt::StrongFocus);

    connectSignals();
}

MainWindow::~MainWindow()
{
    disconnectSignals();
    delete ui;
}

void MainWindow::servicesLayer(const std::weak_ptr<services_layer>& servicesLayer)
{
    m_servicesLayer = servicesLayer;
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    QPoint num_degrees = event->angleDelta() / 8;
    QPoint num_steps = num_degrees / 15;

    int steps = -num_steps.y();
    m_servicesLayer.lock()->setCurrentEntryRelative(steps);
}

void MainWindow::keyPressEvent( QKeyEvent * e)
{
    if(e->key() == 16777235)
        ui->up->animateClick();
    else if(e->key() == 16777237)
        ui->down->animateClick();

    return QMainWindow::keyPressEvent(e);
}

void MainWindow::connectSignals()
{
    connect(m_editDialogWindow.get(), &EditDialog::sendEntry, this, &MainWindow::addEntry);

    connect(ui->dodajWpis, &QPushButton::pressed, this, &MainWindow::openEntryDialog);
    connect(ui->up, &QPushButton::pressed, this, &MainWindow::buttonUp);
    connect(ui->down, &QPushButton::pressed, this, &MainWindow::buttonDown);
    connect(ui->usunWpis, &QPushButton::pressed, this, &MainWindow::deleteEntry);

    connect(ui->backupSave, &QPushButton::pressed, this, &MainWindow::backupSave);
    connect(ui->backupRestore, &QPushButton::pressed, this, &MainWindow::backupRestore);
}

void MainWindow::disconnectSignals()
{
    disconnect(ui->dodajWpis, &QPushButton::pressed, this, &MainWindow::openEntryDialog);
}

void MainWindow::openEntryDialog()
{
    m_editDialogWindow->ClearEntry();
    m_editDialogWindow->show();
}

void MainWindow::buttonUp()
{
    m_servicesLayer.lock()->setCurrentEntryRelative(1);
}

void MainWindow::buttonDown()
{
    m_servicesLayer.lock()->setCurrentEntryRelative(-1);
}

void MainWindow::deleteEntry()
{
    m_servicesLayer.lock()->removeCurrentEntry();
}

void MainWindow::backupSave()
{
    auto file_name = QFileDialog::getSaveFileName(this,
                                                  tr("Choose File"), "Backup", tr("(*.txt)"));

    m_servicesLayer.lock()->serialize(file_name);
}

void MainWindow::backupRestore()
{
    auto file_name = QFileDialog::getOpenFileName(this,
        tr("Choose Backup"), "Backup.txt", tr("(*.txt)"));

    m_servicesLayer.lock()->deserialize(file_name);
}

void MainWindow::addEntry(QDateTime timeStamp,
    QString text,
    QColor color,
    entry_text_format format,
    QString imagePath)
{
    m_servicesLayer.lock()->addEntry(timeStamp, text, color, format, imagePath);
}

void MainWindow::updateEntries(entry* prev, entry* cur, entry* next)
{
    ui->image->clear();

    if (prev == nullptr)
    {
        ui->prev->setText("NO EARLIER ENTRIES");
    }
    else
    {
        ui->prev->setHtml(prev->toHtml());
    }
    
    if (next == nullptr)
    {
        ui->next->setText("NO LATER ENTRIES");
    }
    else
    {
        ui->next->setHtml(next->toHtml());
    }

    if (cur == nullptr)
    {
        ui->current->setText("NO EXISTRING ENTRIES");
    }
    else
    {
        ui->current->setHtml(cur->toHtml());
        int w = ui->image->width();
        int h = ui->image->height();
        ui->image->setPixmap(cur->image().scaled(w, h, Qt::KeepAspectRatio));
    }
}
