#include "mog/core/Texture2DNative.h"
#include "mog/core/Texture2D.h"
#include "mog/core/Engine.h"
#include "mog/core/FileUtils.h"
#include <QPainter>
#include <QStaticText>
#include <QFontDatabase>
#include <QFile>

using namespace mog;

unordered_map<string, string> Texture2DNative::registeredFontNames;

string Texture2DNative::registerCustomFont(const char *fontFilename) {
    string fontFilenameStr = fontFilename;
    if (registeredFontNames.count(fontFilenameStr) > 0) {
        return registeredFontNames[fontFilenameStr];
    }

    QString filenameStr = QString(":/assets_qt/%1").arg(fontFilename);
    QFile file(filenameStr);
    if (!file.exists()) {
        filenameStr = QString(":/assets/%1").arg(fontFilename);
        QFile file(filenameStr);
        if (!file.exists()) return "";
    }

    int fontId = QFontDatabase::addApplicationFont(filenameStr);
    if (fontId == -1) return "";

    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    string fontFamilyStr = fontFamily.toStdString();
    registeredFontNames[fontFilenameStr] = fontFamilyStr;

    return fontFamilyStr;
}

void Texture2DNative::loadFontTexture(Texture2D *tex2d, const char *text, float fontSize, const char *fontFilename, float height) {
    QFont font;

    if (fontFilename != nullptr && strlen(fontFilename) > 0) {
        string fontFace = registerCustomFont(fontFilename);
        if (fontFace.length() == 0) {
            fontFace = fontFilename;
        }
        font = QFont(fontFace.c_str());
    }
    if (!font.exactMatch()) {
        font = QFontDatabase::systemFont(QFontDatabase::SystemFont::GeneralFont);
    }

    font.setPixelSize((int)fontSize);

    QStaticText qtext(text);
    qtext.prepare(QTransform(), font);
    double w = qtext.size().width();
    double h = qtext.size().height();
    if (height > 0) {
        h = height;
    }

    const QSize size = QSize(ceil(w), ceil(h));
    unsigned char *data = (unsigned char *)calloc(size.width() * size.height() * 4, sizeof(unsigned char));
    QImage image(data, size.width(), size.height(), QImage::Format_RGBA8888);

    QPainter p;
    p.begin(&image);
    p.setPen(QColor::fromRgb(255, 255, 255));
    p.setFont(font);
    p.drawStaticText(0, 0, qtext);
    p.end();

    tex2d->filename = "";
    tex2d->width = size.width();
    tex2d->height = size.height();
    tex2d->data = data;
    tex2d->dataLength = size.width() * size.height() * 4;
    tex2d->textureType = TextureType::RGBA;
    tex2d->bitsPerPixel = 4;
    tex2d->isFlip = false;
}

string Texture2DNative::getLocalizedTextNative(const char *textKey, va_list args) {
    return "";
}
