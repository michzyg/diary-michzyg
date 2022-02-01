#ifndef DATA_LAYER_H_
#define DATA_LAYER_H_
#pragma once

#include <cstdint>
#include <ostream>
#include <istream>
#include <memory>

#include <QDateTime>
#include <QColor>
#include <QPixmap>
#include <QScopedPointer>
#include <QPixmap>

enum class entry_text_format : uint32_t
{
    italic,
    normal,
    bold
};

class entry
{
private:
    QDateTime m_timeStamp;
    QString m_text;
    QColor m_color;
    entry_text_format m_format;
    QString m_imagePath;
    QPixmap m_image;
public:
    entry() = default;
    entry(QDateTime timeStamp, QString text, QColor color, entry_text_format format, QPixmap image, QString imagePath);
    entry(const entry&) = default;
    entry(entry&&) noexcept = default;
    entry& operator=(const entry&) = default;
    entry& operator=(entry&&) noexcept = default;
    ~entry() noexcept = default;

public:
    auto dateTime() const -> const decltype (m_timeStamp)&;
    auto text() const -> const decltype (m_text)&;
    auto color() const -> const decltype (m_color)&;
    auto format() const -> decltype (m_format);
    auto image() -> decltype (m_image)&;
    auto image() const -> const decltype (m_image)&;

    QString toHtml() const;

public:
    bool operator<(const entry& rhs) const noexcept;

public:
    friend std::ostream& operator<<(std::ostream& os, const entry& w);
    friend std::istream& operator>>(std::istream& os, entry& w);
};

class services_layer;

class data_layer : public std::enable_shared_from_this<data_layer>
{
    std::weak_ptr<services_layer> m_services_layers;

    QScopedPointer<QPixmap> m_invalidImage;

    QVector<entry> m_entries;
    size_t m_currentEntry;

public:
    data_layer();
    void servicesLayer(const std::weak_ptr<services_layer>& services);
public:
    QPixmap* invalidImage() const noexcept;
public:
    auto entires() noexcept -> decltype(m_entries)&;
    auto entires() const noexcept -> const decltype(m_entries)&;
    
    auto currentEntryIndex() const noexcept -> decltype(m_currentEntry);
    void currentEntryIndex(size_t entry) noexcept;

    void addEntry(QDateTime timeStamp,
        QString text,
        QColor color,
        entry_text_format format,
        QString imagePath);

    void removeCurrentEntry();

    entry* currentEntry() noexcept;
    entry* previousEntry() noexcept;
    entry* nextEntry() noexcept;

public:
    void serialize(const QString& path) noexcept;
    void deserialize(const QString& path);

    void parseEntries(std::ifstream& fs);
    void unparseEntries(std::ofstream& fs) noexcept;
};

#endif
