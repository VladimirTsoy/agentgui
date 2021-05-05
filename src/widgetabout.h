#ifndef WIDGETABOUT_H
#define WIDGETABOUT_H

#include <QWidget>

namespace Ui {
class WidgetAbout;
}

class WidgetAbout : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetAbout(QWidget *parent = nullptr);
    ~WidgetAbout();

private slots:
    void openAbout();
    void openAgreements();

private:
    Ui::WidgetAbout *ui;
};

#endif // WIDGETABOUT_H
