#include "mog/core/FileUtilsNative.h"
#include <QStandardPaths>
#include <QFile>>

using namespace mog;

bool FileUtilsNative::existAsset(string filename) {
    QString filenameStr = QString(":/assets_qt/%1").arg(filename.c_str());
    QFile file(filenameStr);
    if (file.exists()) return true;

    filenameStr = QString(":/assets/%1").arg(filename.c_str());
    QFile file2(filenameStr);
    return file2.exists();
}

string FileUtilsNative::readTextAsset(string filename) {
    QString filenameStr = QString(":/assets_qt/%1").arg(filename.c_str());
    QFile file(filenameStr);
    if (!file.open(QIODevice::ReadOnly)) {
        QString filenameStr = QString(":/assets/%1").arg(filename.c_str());
        QFile file(filenameStr);
        if (!file.open(QIODevice::ReadOnly)) return "";
    }

    QByteArray byteArr = file.readAll();
    string content = QString(byteArr).toStdString();
    file.close();

    return content;
}

bool FileUtilsNative::readBytesAsset(string filename, unsigned char **data, int *len) {
    QString filenameStr = QString(":/assets_qt/%1").arg(filename.c_str());
    QFile file(filenameStr);
    if (!file.open(QIODevice::ReadOnly)) {
        QString filenameStr = QString(":/assets/%1").arg(filename.c_str());
        QFile file(filenameStr);
        if (!file.open(QIODevice::ReadOnly)) return "";
    }

    QByteArray byteArr = file.readAll();
    *data = (unsigned char *)malloc(byteArr.size() * sizeof(unsigned char));
    memcpy(*data, byteArr.data(), byteArr.size() * sizeof(unsigned char));
    *len = byteArr.size();
    file.close();

    return true;
}

string FileUtilsNative::getDocumentsDirectory() {
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
}

string FileUtilsNative::getCachesDirectory() {
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation).toStdString();
}
