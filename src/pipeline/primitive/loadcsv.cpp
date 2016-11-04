#include "loadcsv.h"
#include <QtCore>

enum TokenType {
    END,
    LINE,
    VALUE
};

enum ParserState {
    SKIP,
    HEADER,
    BODY
};

class Parser {
public:
    Parser(DimensionList& dimensions, Variant& preamble, int preambleLineSkip)
        : _dimensions(dimensions)
        , _preamble(preamble)
        , _preambleLineSkip(preambleLineSkip)
        , _lineIndex(0)
        , _columnIndex(0)
        , _state(ParserState::SKIP)
        , _deLocale(QLocale::German)
        , _enLocale(QLocale::English)
    {
    }

    void next(TokenType type, const uchar* begin, const uchar* end)
    {
        if (_state == SKIP) {
            QString token = parseString(begin, end).trimmed();
            if (type == LINE) {
                token = +'\n';
            }
            _preamble.swap(QVariant(_preamble.toString() + token));
        }

        if (type == LINE) {
            if (_state == SKIP && _lineIndex + 1 == _preambleLineSkip) {
                _state = HEADER;
            }
            else if (_state == HEADER && !_dimensions.isEmpty()) {
                _state = BODY;
            }
            else if (_state == BODY) {
                while (_columnIndex < _dimensions.length()) {
                    _dimensions[_columnIndex].append(QVariant());
                    _columnIndex++;
                }
            }
            _lineIndex++;
            _columnIndex = 0;
        }
        else if (type == VALUE) {
            if (_state == HEADER) {
                parseHeader(begin, end);
            }
            else if (_state == BODY) {
                parseBody(begin, end);
            }
            _columnIndex++;
        }
    }

private:
    void parseHeader(const uchar* begin, const uchar* end)
    {
        _dimensions.append(VariantVector(parseString(begin, end)));
    }

    void parseBody(const uchar* begin, const uchar* end)
    {
        if (_columnIndex < _dimensions.length()) {
            _dimensions[_columnIndex].append(parseVariant(begin, end));
        }
        else {
            qWarning() << "Dropping malformed columns";
        }
    }

    QString parseString(const uchar* begin, const uchar* end)
    {
        const char* str = reinterpret_cast<const char*>(begin);
        const int size = end - begin;
        return QString::fromUtf8(str, size);
    }

    QVariant parseVariant(const uchar* begin, const uchar* end)
    {
        QString string = parseString(begin, end);
        bool ok;
        double number;

        // Test for empty/undefined.
        if (string.isEmpty()) {
            return QVariant();
        }

        // Test for localized german number.
        number = _deLocale.toDouble(string, &ok);
        if (ok) {
            return QVariant(number);
        }

        // Test for localized english number.
        number = _enLocale.toDouble(string, &ok);
        if (ok) {
            return QVariant(number);
        }

        // Surrender to string.
        return QVariant(string);
    }

    Variant& _preamble;
    DimensionList& _dimensions;
    int _preambleLineSkip;
    int _lineIndex;
    int _columnIndex;
    ParserState _state;
    QLocale _deLocale;
    QLocale _enLocale;
};

LoadCSV::LoadCSV(const QFileInfo& fileInfo, int preambleLineSkip)
    : _preamble("Preamble")
{
    Parser parser(_dimensions, _preamble, preambleLineSkip);

    QFile file(fileInfo.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Can not open file" << fileInfo.absoluteFilePath();
        return;
    }

    const qint64 FileSize = file.size();
    qint64 bufferSize = FileSize / 10;
    qint64 bufferOffset = 0;
    while (bufferOffset < FileSize) {
        // Ensure buffer does not overflow file.
        if (FileSize - bufferOffset < bufferSize) {
            bufferSize = FileSize - bufferOffset;
        }

        // Map file to buffer.
        uchar* buffer = file.map(bufferOffset, bufferSize);
        if (!buffer) {
            qWarning() << "Can not map file" << fileInfo.absoluteFilePath();
            break;
        }

        // Scan for columns in a line.
        bool foundLine = false;
        qint64 tokenStart = 0;
        for (qint64 i = 0; i < bufferSize; ++i) {
            if (buffer[i] == ';' || buffer[i] == '\t') {
                parser.next(VALUE, &buffer[tokenStart], &buffer[i]);
                tokenStart = i + 1;
            }
            else if (buffer[i] == '\r' && buffer[i] == '\n') {
                parser.next(LINE, &buffer[tokenStart], &buffer[i]);
                tokenStart = i + 2;
                foundLine = true;
            }
            else if (buffer[i] == '\n') {
                parser.next(LINE, &buffer[tokenStart], &buffer[i]);
                tokenStart = i + 1;
                foundLine = true;
            }
        }

        // Unmap file.
        if (!file.unmap(buffer)) {
            qWarning() << "Can not unmap file" << fileInfo.absoluteFilePath();
            break;
        }

        // Adjust offset or stop reading if nothing was found.
        bufferOffset += tokenStart;
        if (!foundLine) {
            break;
        }
    }

    // Create list of pointers for the framework.
    _values.append(&_preamble);
    for (auto& dimension : _dimensions) {
        _values.append(&dimension);
    }
}

const QList<const Value*> LoadCSV::values()
{
    return _values;
}

void LoadCSV::call()
{
    // Do nothing, as all the work has been done by the constructor.
}
