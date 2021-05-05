#ifndef PLUGININFOLABEL_H
#define PLUGININFOLABEL_H

#include <QFrame>

namespace Ui {
class PluginInfoLabel;
}

class PluginInfoLabel : public QFrame
{
    Q_OBJECT

public:
    explicit PluginInfoLabel(QWidget *parent = nullptr);
    ~PluginInfoLabel();

    void setText(QString text);
    void setState(short state);

private:
    Ui::PluginInfoLabel *ui;
};

#endif // PLUGININFOLABEL_H
