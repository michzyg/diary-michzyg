#include "services_layer.h"
#include "mainwindow.h"

void services_layer::mainWindowUILayer(MainWindow* ptr)
{
    m_mainWindowUILayer = ptr;
}

void services_layer::dataLayer(std::shared_ptr<data_layer> dl)
{
    m_data_layer = dl;
}

void services_layer::addEntry(QDateTime timeStamp,
    QString text,
    QColor color,
    entry_text_format format,
    QString imagePath)
{
    m_data_layer->addEntry(timeStamp, text, color, format, imagePath);
    updateMainUI();
}

void services_layer::removeCurrentEntry()
{
    m_data_layer->removeCurrentEntry();
    updateMainUI();
}

void services_layer::setCurrentEntryRelative(int32_t dif)
{
    int64_t i = static_cast<int64_t>(m_data_layer->currentEntryIndex());
    i += dif;
    m_data_layer->currentEntryIndex(i < 0 ? 0 : static_cast<size_t>(i));
    updateMainUI();
}

void services_layer::serialize(const QString& path) noexcept
{
    m_data_layer->serialize(path);
}

void services_layer::deserialize(const QString& path)
{
    m_data_layer->deserialize(path);
    updateMainUI();
}

void services_layer::updateMainUI()
{
    m_mainWindowUILayer->updateEntries(
        m_data_layer->previousEntry(),
        m_data_layer->currentEntry(),
        m_data_layer->nextEntry()
    );
}