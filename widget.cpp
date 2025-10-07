#include "widget.h"
#include "formatpreset.h"
#include "ui_widget.h"

QVector<FormatPreset> preset =
{
    FormatPreset("custom", "", ""),
    FormatPreset("activity", "\\1_\\2_\\d3", "Date: %MMdd, Name: 姓名", TYPE_PIC | TYPE_VID, ""),
    FormatPreset("artwork", "*\\1_\\2", "Date: %yyMMdd, Catagory: 组名", TYPE_PIC, "")
};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 写入预设
    foreach(auto ps, preset)
    {
        ui->presetBox->addItem(ps.getName());
    }
    // 初始化反馈栏
    ui->time->setText(QTime::currentTime().toString("hh:mm:ss"));
    ui->feedback->setText("Successfully initialized.");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_rename_clicked()
{
    // 提取占位符
    QString content = ui->contentEdit->text();
    QVector<QString> replacements = content.split(',');
    for(int i = 0; i < replacements.length(); i++)
    {
        QStringList parts = replacements[i].split(':');
        if(parts.length() > 1)
        { replacements[i] = parts.last(); }
        replacements[i] = replacements[i].trimmed();
    }
    // 提取格式和路径
    QString format = ui->formatEdit->text();
    QString directory = ui->pathEdit->text();
    // 提取扩展名过滤器
    QString extensionFilter;
    extensionFilter += ui->typeEdit->text();
    if(ui->pic->isChecked())
    {
        if(!extensionFilter.isEmpty())
        { extensionFilter += "|"; }
        extensionFilter += Extension::Pic;
    }
    if(ui->vid->isChecked())
    {
        if(!extensionFilter.isEmpty())
        { extensionFilter += "|"; }
        extensionFilter += Extension::Vid;
    }
    if(ui->doc->isChecked())
    {
        if(!extensionFilter.isEmpty())
        { extensionFilter += "|"; }
        extensionFilter += Extension::Doc;
    }
    extensionFilter = (extensionFilter.isEmpty()) ? ".*" : extensionFilter;
    qDebug() << extensionFilter;
    QString feedback = renamer.renameFiles(format, replacements, directory, extensionFilter);
    ui->time->setText(QTime::currentTime().toString("hh:mm:ss"));
    ui->feedback->setText(feedback);
}


void Widget::on_browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select a folder");
    ui->pathEdit->setText(dir);
}

void Widget::on_presetBox_currentIndexChanged(int index)
{
    ui->formatEdit->setText(preset[index].getFormat());
    ui->contentEdit->setText(preset[index].getDefaultContent());
    int type = preset[index].getType();
    ui->pic->setChecked(TYPE_PIC & type);
    ui->vid->setChecked(TYPE_VID & type);
    ui->doc->setChecked(TYPE_DOC & type);
    ui->typeEdit->setText(preset[index].getCustomType());
}

