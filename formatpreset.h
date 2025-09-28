#ifndef FORMATPRESET_H
#define FORMATPRESET_H

#include <QString>
#include <QRegularExpression>
#include <QDateTime>

#define TYPE_PIC 0x01
#define TYPE_VID 0x02
#define TYPE_DOC 0x04

class FormatPreset
{
public:

    FormatPreset(const QString name, const QString& format, const QString& content, const int &type = 0, const QString& customType = ".*");

    // 获取预设名
    QString const getName();
    // 获取格式
    QString const getFormat();
    // 获取默认内容
    QString const getDefaultContent();
    // 获取匹配类型
    int getType();
    // 获取自定义类型
    QString const getCustomType();

private:

    const QString name;                 // 预设名称
    const QString format;               // 格式信息
    const QString defaultContent;       // 默认内容
    const int fileTpye;                 // 匹配类型
    const QString CustomFileType;       // 自定义匹配类型

};

#endif // FORMATPRESET_H
