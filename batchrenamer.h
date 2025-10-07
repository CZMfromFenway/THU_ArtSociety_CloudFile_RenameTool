#ifndef BATCHRENAMER_H
#define BATCHRENAMER_H

#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QString>
#include <QVector>
#include <QDebug>

namespace Extension
{
    // 常用扩展名
    extern QString Pic;
    extern QString Vid;
    extern QString Doc;
} // namespace Extention

class BatchRenamer
{
public:
    BatchRenamer();

    // 批量重命名函数
    QString renameFiles(const QString& format, const QVector<QString> &replacements, const QString& directory, const QString& extensionFilter = ".*");

private:
    // 占位符类型
    enum class PlaceholderType
    {
        Regular,    // 普通占位符，如 $1, $2
        Numbered,   // 数字占位符，如 $d3
        RegularExpression // 正则表达式替换结果
    };

    // 重命名模式
    enum class RenameMode
    {
        Regular,    // 普通重命名
        Strict,     // 严格重命名
        Prepend,    // 前置重命名
        Append,     // 后置重命名
        RegularExpression // 正则表达式重命名
    };

    // 占位符结构
    struct Placeholder
    {
        PlaceholderType type;
        int index;          // 对于普通占位符，这是索引；对于数字占位符，这是位数
        int position;       // 在格式字符串中的位置
        int length;         // 在格式字符串中的长度
    };

    // 解析后的格式
    struct ParsedFormat
    {
        QString rawFormat;
        RenameMode mode;
        QVector<Placeholder> placeholders;
        bool hasNumberPlaceholder = false;
    };

    // 解析格式字符串
    ParsedFormat parseFormat(const QString& format);

    // 构建正则表达式
    QString buildRegexPattern(const ParsedFormat& parsed, const QVector<QString> &replacements, bool strictMode = true);

    // 根据扩展名过滤文件
    QFileInfoList filterFilesByExtension(const QFileInfoList& files, const QString& extensionFilter);

    // 查找最大编号
    int findMaxNumber(const QFileInfoList& files, const ParsedFormat& parsed, const QVector<QString> &replacements);

    // 检查文件名是否符合格式
    bool matchesFormat(const QString& fileName, const ParsedFormat& parsed, const QVector<QString> &replacements);

    // 提取编号
    int extractNumber(const QString& fileName, const ParsedFormat& parsed, const QVector<QString> &replacements);

    // 生成新文件名
    QString generateFileName(const QString& oldName, const ParsedFormat& parsed,
                             const QVector<QString> &replacements, int number);

};

#endif // BATCHRENAMER_H
