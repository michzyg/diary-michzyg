#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include "data_layer.h"

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(QWidget *parent = nullptr);
    ~EditDialog();

    void connectSignals();
    void disconnectSignals();
    void ClearEntry();

private:
    Ui::EditDialog *ui;
    QPixmap m_current_map;
    QString m_current_map_path;
private slots:

    void AddEntry();
    void Cancel();
    void selectImage();

signals:
    void sendEntry(QDateTime timeStamp,
        QString text,
        QColor color,
        entry_text_format format,
        QString imagePath);
};

#endif // EDITDIALOG_H
