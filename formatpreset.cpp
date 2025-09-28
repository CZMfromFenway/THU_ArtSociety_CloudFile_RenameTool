#include "formatpreset.h"

FormatPreset::FormatPreset(const QString name, const QString &format, const QString &content, const int &type, const QString &customType):
    name(name), format(format), defaultContent(content), fileTpye(type), CustomFileType(customType)
{}

const QString FormatPreset::getName()
{
    return this->name;
}

const QString FormatPreset::getFormat()
{
    return this->format;
}

const QString FormatPreset::getDefaultContent()
{
    // 将默认内容中的日期自动填充
    auto result = this->defaultContent;
    QDate date = QDate::currentDate();
    result.replace(QRegularExpression("%yyMMdd"), date.toString("yyMMdd"));
    result.replace(QRegularExpression("%MMdd"), date.toString("MMdd"));
    return result;
}

int FormatPreset::getType()
{
    return this->fileTpye;
}

const QString FormatPreset::getCustomType()
{
    return this->CustomFileType;
}
