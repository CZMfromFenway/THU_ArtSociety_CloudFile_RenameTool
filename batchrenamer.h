#ifndef BATCHRENAMER_H
#define BATCHRENAMER_H

/******************************************************************************
 * @file       batchrenamer.h
 * @brief      实现文件批量命名的核心类
 *
 * @author     czm<chengzm23@mails.tsinghua.edu.cn>
 * @date       2025/10/07
 * @history    1.2
 *****************************************************************************/

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

/**
 * @brief 批量重命名类
 */
class BatchRenamer
{
public:
    BatchRenamer();

    /**
     * @brief 批量重命名函数
     * @param format            命名格式
     * @param replacements      占位符
     * @param directory         重命名目录
     * @param extensionFilter   扩展名过滤器（正则表达式）
     * @return 操作结果信息
     */
    QString renameFiles(const QString& format, const QVector<QString> &replacements, const QString& directory, const QString& extensionFilter = ".*");

private:
    /**
     * @brief 占位符类型
     */
    enum class PlaceholderType
    {
        Regular,    // 普通占位符，如 \1, \2
        Numbered,   // 数字占位符，如 \d3
        RegularExpression // 正则表达式替换结果
    };

    /**
     * @brief 重命名模式
     */
    enum class RenameMode
    {
        Regular,    // 普通重命名
        Strict,     // 严格重命名
        Prepend,    // 前置重命名
        Append,     // 后置重命名
        RegularExpression // 正则表达式重命名
    };

    /**
     * @brief 占位符结构
     */
    struct Placeholder
    {
        PlaceholderType type;
        int index;          // 对于普通占位符，这是索引；对于数字占位符，这是位数
        int position;       // 在格式字符串中的位置
        int length;         // 在格式字符串中的长度
    };

    /**
     * @brief 解析后的格式
     */
    struct ParsedFormat
    {
        QString rawFormat;
        RenameMode mode;
        QVector<Placeholder> placeholders;
        bool hasNumberPlaceholder = false;
    };

    /**
     * @brief 解析格式字符串
     * @param format            格式字符串
     * @return 解析后的格式
     */
    ParsedFormat parseFormat(const QString& format);

    /**
     * @brief 构建正则表达式
     * @param parsed            解析后的命名格式
     * @param replacements      占位符
     * @param strictMode        严格模式，严格模式下准确匹配占位符
     * @return 正则表达式字符串
     */
    QString buildRegexPattern(const ParsedFormat& parsed, const QVector<QString> &replacements, bool strictMode = true);

    /**
     * @brief 根据扩展名过滤文件
     * @param file              文件列表
     * @param extensionFilter   扩展名过滤器（正则表达式）
     * @return 过滤后的文件列表
     */
    QFileInfoList filterFilesByExtension(const QFileInfoList& files, const QString& extensionFilter);

    /**
     * @brief 查找最大编号
     * @param files             文件列表
     * @param parsed            解析后的格式
     * @param replacements      占位符
     * @return 文件列表中符合格式的文件名的最大编号
     */
    int findMaxNumber(const QFileInfoList& files, const ParsedFormat& parsed, const QVector<QString> &replacements);

    /**
     * @brief 检查文件名是否符合格式
     * @param fileName          文件名
     * @param parsed            解析后的格式
     * @param replacements      占位符
     * @return 符合格式时返回true
     */
    bool matchesFormat(const QString& fileName, const ParsedFormat& parsed, const QVector<QString> &replacements);

    /**
     * @brief 提取编号
     * @param fileName          文件名
     * @param parsed            解析后的格式
     * @param replacements      占位符
     * @return 符合格式的文件名中的编号
     */
    int extractNumber(const QString& fileName, const ParsedFormat& parsed, const QVector<QString> &replacements);

    /**
     * @brief 生成新的文件名
     * @param oldName           旧文件名
     * @param parsed            解析后的格式
     * @param replacements      占位符
     * @param number            编号
     * @return 新文件名
     */
    QString generateFileName(const QString& oldName, const ParsedFormat& parsed,
                             const QVector<QString> &replacements, int number);

};

#endif // BATCHRENAMER_H
