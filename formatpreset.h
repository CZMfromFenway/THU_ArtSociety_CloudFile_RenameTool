#ifndef FORMATPRESET_H
#define FORMATPRESET_H

/******************************************************************************
 * @file       formatpreset.h
 * @brief      含格式预设类
 *
 * @author     czm<chengzm23@mails.tsinghua.edu.cn>
 * @date       2025/10/07
 * @history    1.0
 *****************************************************************************/

#include <QString>
#include <QRegularExpression>
#include <QDateTime>

#define TYPE_PIC 0x01
#define TYPE_VID 0x02
#define TYPE_DOC 0x04

/**
 * @brief 格式预设类
 */
class FormatPreset
{
public:

    /**
     * @brief 构造函数
     * @param name                  预设名称
     * @param format                预设格式
     * @param content               预设内容
     * @param type                  预设所修改文件类型
     * @param customType            预设所自定义修改文件类型
     */
    FormatPreset(const QString name, const QString& format, const QString& content, const int &type = 0, const QString& customType = ".*");

    /**
     * @brief 获取预设名称
     * @return 预设名称
     */
    QString const getName();

    /**
     * @brief 获取预设格式
     * @return 预设格式
     */
    QString const getFormat();

    /**
     * @brief 获取预设默认内容
     * @return 预设默认内容
     */
    QString const getDefaultContent();

    /**
     * @brief 获取预设所修改文件类型
     * @return 类型（参考宏TYPE_XXX）
     */
    int getType();

    /**
     * @brief 获取预设所自定义修改文件类型
     * @return 扩展名过滤器
     */
    QString const getCustomType();

private:

    const QString name;                 // 预设名称
    const QString format;               // 格式信息
    const QString defaultContent;       // 默认内容
    const int fileTpye;                 // 匹配类型
    const QString CustomFileType;       // 自定义匹配类型

};

#endif // FORMATPRESET_H
