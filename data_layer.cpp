#include "data_layer.h"
#include "services_layer.h"

#include <algorithm>
#include <fstream>

#include <QPainter>
#include <QFont>

entry::entry(QDateTime timeStamp, QString text, QColor color, entry_text_format format, QPixmap image, QString imagePath)
    : m_timeStamp(timeStamp), m_text(text), m_color(color), m_format(format), m_image(image),
    m_imagePath(imagePath)
{}

auto entry::dateTime() const -> const decltype (m_timeStamp)&
{
    return m_timeStamp;
}

auto entry::text() const -> const decltype (m_text)&
{
    return m_text;
}

auto entry::color() const -> const decltype (m_color)&
{
    return m_color;
}

auto entry::format() const -> decltype (m_format)
{
    return m_format;
}

auto entry::image() -> decltype (m_image)&
{
    return m_image;
}

auto entry::image() const -> const decltype (m_image)&
{
    return m_image;
}

bool entry::operator<(const entry& rhs) const noexcept
{
    return m_timeStamp < rhs.m_timeStamp;
}

QString entry::toHtml() const
{
    QString format_b;
    QString format_e;

    if (m_format == entry_text_format::bold)
    {
        format_b = "<b>";
        format_e = "</b>";
    }
    else if (m_format == entry_text_format::italic)
    {
        format_b = "<i>";
        format_e = "</i>";
    }

    QString text;

    text += "<p>";
    text += "<br>";
    text += "<b>" + dateTime().toString("dd.MM.yyyy hh:mm") + "</b>";

    text += "<pre>";
    text += format_b;
    text += "<p style=color:'" + color().name(QColor::HexRgb) + "'>";
    text += this->text();
    text += format_e;
    text += "</p>";
    text += "</pre> <br>";

    text += "</p>\n";

    return text;
}

std::ostream& operator<<(std::ostream& os, const entry& w)
{
    os
        << w.m_timeStamp.toString("dd.MM.yyyy.hh.mm").toLatin1().data() << ' '
        << w.m_color.red() << ' ' << w.m_color.green() << ' ' << w.m_color.blue() << ' '
        << static_cast<uint32_t>(w.m_format) << '\n'
        << w.m_imagePath.size() << '\n'
        << w.m_imagePath.toLatin1().data() << '\n'
        << w.m_text.size() << '\n'
        << w.m_text.toLatin1().data() << '\n';

    return os;
}

std::istream& operator>>(std::istream& is, entry& w)
{
    std::string date;
    is >> date;
    w.m_timeStamp = QDateTime::fromString(QString(date.c_str()), "dd.MM.yyyy.hh.mm");

    int r, g, b;
    is >> r >> g >> b;
    w.m_color = QColor::fromRgb(r, g, b);

    int format;
    is >> format;
    w.m_format = static_cast<entry_text_format>(format);

    size_t image_path_size;
    is >> image_path_size;


    std::string image_path;
    image_path.reserve(image_path_size);
    std::copy_n(++std::istreambuf_iterator(is), image_path_size, std::back_inserter(image_path));
    ++std::istreambuf_iterator(is);
    // is.seekg(image_path_size, std::ios::cur);
    w.m_imagePath = QString(image_path.c_str());
    w.m_image = QPixmap(w.m_imagePath);

    size_t text_size;
    is >> text_size;
    std::string text;
    text.reserve(text_size);
    // is.seekg(1, std::ios::end);
    std::copy_n(++std::istreambuf_iterator(is), text_size, std::back_inserter(text));
    ++std::istreambuf_iterator(is);
    w.m_text = QString(text.c_str());

    return is;
}

static auto generateInvlaidPixmap = []() -> QPixmap
{
    auto map = QPixmap(1024, 1024);
    QPainter painter(&map);
    painter.fillRect(0, 0, 1024, 1024, QBrush(Qt::white));
    QFont font = painter.font();
    font.setPixelSize(140);
    painter.setFont(font);
    painter.drawText(60, 512, "NO IMAGE");
    return map;
};

data_layer::data_layer()
{
    m_invalidImage.reset(new QPixmap(generateInvlaidPixmap()));

    m_entries = {};
    m_currentEntry = 0;
}

void data_layer::servicesLayer(const std::weak_ptr<services_layer>& servicesLayer)
{
    m_services_layers = servicesLayer;
}

QPixmap* data_layer::invalidImage() const noexcept
{
    return m_invalidImage.get();
}

auto data_layer::entires() noexcept -> decltype(m_entries)&
{
    return m_entries;
}

auto data_layer::entires() const noexcept -> const decltype(m_entries)&
{
    return m_entries;
}

auto data_layer::currentEntryIndex() const noexcept -> decltype(m_currentEntry)
{
    return m_currentEntry;
}

void data_layer::currentEntryIndex(size_t entry) noexcept
{
    if (std::empty(m_entries))
    {
        m_currentEntry = 0;
        return;
    }


    m_currentEntry = std::min(static_cast<size_t>(std::size(m_entries) - 1), entry);
}

void data_layer::addEntry(
    QDateTime timeStamp,
    QString text,
    QColor color,
    entry_text_format format,
    QString imagePath
)
{
    QPixmap image;
    {
        if (imagePath.isEmpty())
        {
            image = *m_invalidImage.get();
        }
        else
        {
            image = QPixmap(imagePath);
        }
    }

    m_entries.emplace_back(timeStamp, text, color, format, image, imagePath);

    std::sort(std::begin(m_entries), std::end(m_entries));
}

void data_layer::removeCurrentEntry()
{
    if (std::empty(m_entries)) return;

    m_entries.erase(std::begin(m_entries) + m_currentEntry);
    currentEntryIndex(m_currentEntry);
}

entry* data_layer::currentEntry() noexcept
{
    if (std::empty(m_entries))
        return nullptr;

    return &m_entries[m_currentEntry];
}

entry* data_layer::previousEntry() noexcept
{
    if (std::empty(m_entries) || m_currentEntry == 0)
        return nullptr;

    return &m_entries[m_currentEntry - 1];
}

entry* data_layer::nextEntry() noexcept
{
    if (std::empty(m_entries) || m_currentEntry + 1 == std::size(m_entries))
        return nullptr;

    return &m_entries[m_currentEntry + 1];
}

void data_layer::serialize(const QString& path) noexcept
{
    std::ofstream entryData(path.toLatin1(), std::ios::trunc);
    unparseEntries(entryData);
}

void data_layer::deserialize(const QString& path)
{
    std::ifstream entryData(path.toLatin1());
    if (entryData.good())
    {
        parseEntries(entryData);
    }
}

void data_layer::parseEntries(std::ifstream& fs)
{
    m_entries.clear();
    std::string uuid;
    fs >> uuid;
    if(uuid != "0b4dedc4-55c0-11ec-bf63-0242ac130002")
        return; //dialog box
    
    size_t count;
    fs >> count;
    
    for(size_t i = 0; i < count; ++i)
    {
        entry w;
        fs >> w;

        if (w.image().isNull())
        {
            w.image() = *m_invalidImage.get();
        }

        m_entries.push_back(w);
    }
    
    std::sort(std::begin(m_entries), std::end(m_entries));
    if(!std::empty(m_entries))
    {
        auto r = std::min_element(std::begin(m_entries), std::end(m_entries),
                                  [now = QDateTime::currentDateTime()](const entry& lhs, const entry& rhs) -> bool
        {
            return std::abs(now.secsTo(lhs.dateTime())) < std::abs(now.secsTo(rhs.dateTime()));
        });
    
        m_currentEntry = std::distance(std::begin(m_entries), r);
    }
}

void data_layer::unparseEntries(std::ofstream& fs) noexcept
{
    fs << "0b4dedc4-55c0-11ec-bf63-0242ac130002" << '\n';
    fs << m_entries.size() << '\n';
    for(auto& w : m_entries)
    {
        fs << w;
    }
}
