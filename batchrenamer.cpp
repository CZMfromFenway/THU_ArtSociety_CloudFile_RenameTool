#include "batchrenamer.h"

QString Extension::Pic = "jpeg|jpg|png|bmp|webp|raw|avif|gif";
QString Extension::Vid = "mp4|flv|gif|f4v|mov|m4v|avi|mpg|mpeg|wmv";
QString Extension::Doc = "txt|md|doc|pdf|ppt|docx|pptx|xls|xlsx|rtf|csv";

BatchRenamer::BatchRenamer() {}

QString BatchRenamer::renameFiles(const QString& format, const QVector<QString> &replacements, const QString& directory, const QString& extensionFilter)
{
    QDir dir(directory);
    if(!dir.exists())
    {
        qWarning() << "Directory does not exist: " << directory;
        return "Directory does not exist: " + directory;
    }
    // 解析格式字符串
    ParsedFormat parsed = parseFormat(format);
    if(parsed.placeholders.isEmpty())
    {
        qWarning() << "Invalid format string: " << format;
        return "Invalid format string: " + format;
    }
    // 检查占位符数量是否匹配
    int regularPlaceholders = 0;
    for(const auto& ph : parsed.placeholders)
    {
        if(ph.type == PlaceholderType::Regular) { regularPlaceholders++; }
    }
    if(regularPlaceholders != replacements.size())
    {
        qWarning() << "Number of replacements does not match number of regular placeholders.";
        return "Number of replacements does not match number of regular placeholders.";
    }
    // 获取目录中所有文件
    QFileInfoList allFiles = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    // 根据扩展名过滤文件
    QFileInfoList files = filterFilesByExtension(allFiles, extensionFilter);
    if(files.isEmpty())
    {
        qWarning() << "No files match the extension filter: " << extensionFilter;
        return "No files match the extension filter: " + extensionFilter;
    }
    // 查找最大编号（如果有数字占位符）
    int maxNumber = -1;
    if(parsed.hasNumberPlaceholder)
    {
        maxNumber = findMaxNumber(files, parsed, replacements);
    }
    // 重命名文件
    int successCount = 0;
    for(const QFileInfo& fileInfo : files)
    {
        // 跳过已经符合格式的文件
        if(matchesFormat(fileInfo.fileName(), parsed, replacements))
        {
            continue;
        }
        // 生成新文件名
        QString newName = generateFileName(fileInfo.fileName(), parsed, replacements, ++maxNumber);
        QString newPath = dir.absoluteFilePath(newName);
        // 检查新文件名是否已存在
        if(QFile::exists(newPath))
        {
            qWarning() << "Target file already exists: " << newPath;
            continue;
        }
        // 重命名文件
        if(dir.rename(fileInfo.fileName(), newName))
        {
            successCount++;
        }
        else
        {
            qWarning() << "Failed to rename: " << fileInfo.fileName() << " to " << newName;
        }
    }
    return "Successfully renamed " + QString::number(successCount) + " file(s).";
}

BatchRenamer::ParsedFormat BatchRenamer::parseFormat(const QString& format)
{
    ParsedFormat result;
    // 先判断重命名模式
    if(format.front() == "+" && format.back() == "+")
    {
        result.mode = RenameMode::Strict;
    }
    else
        if(format.front() == '+')
        {
            result.mode = RenameMode::Append;
        }
        else
            if(format.back() == "+")
            {
                result.mode = RenameMode::Prepend;
            }
            else
            {
                result.mode = RenameMode::Regular;
            }
    result.rawFormat = format;
    result.rawFormat.remove('+');
    // 再搜索占位符
    QRegularExpression regex(R"(\$(\d+)|(\$d(\d+)))");
    auto matches = regex.globalMatch(result.rawFormat);
    while(matches.hasNext())
    {
        auto match = matches.next();
        Placeholder ph;
        ph.position = match.capturedStart();
        ph.length = match.capturedLength();
        // 判断是普通占位符还是数字占位符
        if(!match.captured(1).isEmpty())
        {
            ph.type = PlaceholderType::Regular;
            ph.index = match.captured(1).toInt();
        }
        else
        {
            ph.type = PlaceholderType::Numbered;
            ph.index = match.captured(3).toInt();
            result.hasNumberPlaceholder = true;
        }
        result.placeholders.append(ph);
    }
    return result;
}

QString BatchRenamer::buildRegexPattern(const ParsedFormat& parsed, const QVector<QString> &replacements, bool strictMode)
{
    QString pattern = parsed.rawFormat;
    // 从后向前替换，避免位置变化影响
    for(int i = parsed.placeholders.size() - 1; i >= 0; i--)
    {
        const auto& ph = parsed.placeholders[i];
        QString replacement;
        if(ph.type == PlaceholderType::Regular)
        {
            if(ph.index > 0 && ph.index <= replacements.size())
            {
                // 普通占位符：在严格模式下匹配具体的替换文本
                if(strictMode)
                {
                    replacement = (replacements[ph.index - 1]);
                }
                else
                {
                    replacement = "[^_]+";
                }
            }
            else
            {
                // 无效的占位符：匹配任意非空字符串
                replacement = "[^_]+";
            }
        }
        else
        {
            // 数字占位符：匹配指定位数的数字
            replacement = QString("(\\d{%1})").arg(ph.index);
        }
        pattern.replace(ph.position, ph.length, replacement);
    }
    switch(parsed.mode)
    {
        case RenameMode::Strict:
            pattern = "^" + pattern + "$";
            break;
        case RenameMode::Append:
            pattern = pattern + "$";
            break;
        case RenameMode::Prepend:
            pattern = "^" + pattern;
            break;
        case RenameMode::Regular:
            break;
        default:
            break;
    }
    return pattern;
}

QFileInfoList BatchRenamer::filterFilesByExtension(const QFileInfoList& files, const QString& extensionFilter)
{
    QFileInfoList result;
    QRegularExpression regex(extensionFilter, QRegularExpression::CaseInsensitiveOption);
    if(!regex.isValid())
    {
        qWarning() << "Invalid regex pattern in extension filter:" << extensionFilter;
        return files; // 返回所有文件作为降级处理
    }
    for(const QFileInfo& fileInfo : files)
    {
        QString ext = fileInfo.suffix();
        if(regex.match(ext).hasMatch())
        {
            result.append(fileInfo);
        }
    }
    return result;
}

int BatchRenamer::findMaxNumber(const QFileInfoList& files, const ParsedFormat& parsed, const QVector<QString> &replacements)
{
    int maxNumber = -1;
    for(const QFileInfo& fileInfo : files)
    {
        if(matchesFormat(fileInfo.fileName(), parsed, replacements))
        {
            // 提取数字部分
            int number = extractNumber(fileInfo.fileName(), parsed, replacements);
            if(number > maxNumber)
            {
                maxNumber = number;
            }
        }
    }
    return maxNumber;
}

bool BatchRenamer::matchesFormat(const QString& fileName, const ParsedFormat& parsed, const QVector<QString> &replacements)
{
    // 构建正则表达式模式
    QString patternStr = buildRegexPattern(parsed, replacements, false);
    QRegularExpression regex(patternStr);
    if(!regex.isValid())
    {
        qWarning() << "Invalid regex pattern:" << patternStr;
        return false;
    }
    // 只匹配文件名（不含扩展名）
    QFileInfo info(fileName);
    QString baseName = info.completeBaseName(); // 包含所有点号的基本名称
    QRegularExpressionMatch match = regex.match(baseName);
    return match.hasMatch();
}

int BatchRenamer::extractNumber(const QString& fileName, const ParsedFormat& parsed, const QVector<QString> &replacements)
{
    // 构建正则表达式
    QString patternStr = buildRegexPattern(parsed, replacements);
    QRegularExpression regex(patternStr);
    if(!regex.isValid())
    {
        qWarning() << "Invalid regex pattern for extraction:" << patternStr;
        return -1;
    }
    // 只匹配文件名（不含扩展名）
    QFileInfo info(fileName);
    QString baseName = info.completeBaseName();
    QRegularExpressionMatch match = regex.match(baseName);
    if(match.hasMatch())
    {
        // 查找数字捕获组
        for(int i = 1; i <= match.lastCapturedIndex(); i++)
        {
            QString captured = match.captured(i);
            bool ok;
            int number = captured.toInt(&ok);
            if(ok)
            {
                return number;
            }
        }
    }
    return -1;
}

QString BatchRenamer::generateFileName(const QString& oldName, const ParsedFormat& parsed,
                                       const QVector<QString> &replacements, int number)
{
    QString result = parsed.rawFormat;
    // 从后向前替换，避免位置变化影响
    for(int i = parsed.placeholders.size() - 1; i >= 0; i--)
    {
        const auto& ph = parsed.placeholders[i];
        QString replacement;
        if(ph.type == PlaceholderType::Regular)
        {
            if(ph.index > 0 && ph.index <= replacements.size())
            {
                replacement = replacements[ph.index - 1];
            }
            else
            {
                replacement = "unknown";
            }
        }
        else
        {
            // 数字占位符，格式化为指定位数的数字
            replacement = QString("%1").arg(number, ph.index, 10, QChar('0'));
        }
        result.replace(ph.position, ph.length, replacement);
    }
    // 保留原文件扩展名
    QFileInfo info(oldName);
    switch(parsed.mode)
    {
        case RenameMode::Prepend:
            result = result + "_" + oldName;
            break;
        case RenameMode::Append:
            result = info.baseName() + "_" + result + "." + info.suffix();
            break;
        case RenameMode::Regular:
        case RenameMode::Strict:
            result = result + "." + info.suffix();
        default:
            break;
    }
    return result;
}
