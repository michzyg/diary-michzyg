#ifndef SERVICES_LAYER_H_
#define SERVICES_LAYER_H_
#pragma once

#include "data_layer.h"
#include <memory>

class MainWindow;
class services_layer : public std::enable_shared_from_this<services_layer>
{
    std::shared_ptr<data_layer> m_data_layer;
    MainWindow* m_mainWindowUILayer;

public:
    void mainWindowUILayer(MainWindow* ptr);
    void dataLayer(std::shared_ptr<data_layer> dl);

public:
    void addEntry(QDateTime timeStamp,
        QString text,
        QColor color,
        entry_text_format format,
        QString imagePath);

    void removeCurrentEntry();
    void setCurrentEntryRelative(int32_t dif);

    void serialize(const QString& path) noexcept;
    void deserialize(const QString& path);

private:
    void updateMainUI();
};

#endif
