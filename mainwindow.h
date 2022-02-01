#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <fstream>
#include <memory>

#include <QMainWindow>

#include "editdialog.h"
#include "services_layer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void servicesLayer(const std::weak_ptr<services_layer>& servicesLayer);

public:
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent( QKeyEvent * e) override;
private:
    Ui::MainWindow *ui;
    std::weak_ptr<services_layer> m_servicesLayer;

    QScopedPointer<EditDialog> m_editDialogWindow;

public slots:
    void addEntry(QDateTime timeStamp,
        QString text,
        QColor color,
        entry_text_format format,
        QString imagePath);

private slots:
    void openEntryDialog();
    void buttonUp();
    void buttonDown();
    void deleteEntry();
    void backupSave();
    void backupRestore();
private:
    void connectSignals();
    void disconnectSignals();

public:
    void updateEntries(entry* prev, entry* cur, entry* next);

};
#endif // MAINWINDOW_H
