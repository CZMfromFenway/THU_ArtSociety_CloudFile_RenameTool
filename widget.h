#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include "batchrenamer.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_rename_clicked();

    void on_browse_clicked();

    void on_presetBox_currentIndexChanged(int index);

private:
    Ui::Widget *ui;

    BatchRenamer renamer;
};
#endif // WIDGET_H
