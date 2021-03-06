#ifndef SWITCH_H
#define SWITCH_H

#include <QtWidgets>
#include "style.h"

class Animator : public QVariantAnimation {
    Q_OBJECT
    Q_PROPERTY(QObject* targetObject READ targetObject WRITE setTargetObject)

public:
    explicit Animator(QObject* target, QObject* parent = nullptr);
    ~Animator();

    QObject* targetObject() const;
    void setTargetObject(QObject* target);

    bool isRunning() const {
        return state() == Running;
    }

    void setup(int duration, QEasingCurve easing = QEasingCurve::Linear);
    void interpolate(const QVariant& start, const QVariant& end);

protected:
    void updateCurrentValue(const QVariant& value) override;
    void updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState) override;

private:
    QPointer<QObject> target;
};

class SelectionControl :public QAbstractButton {
    Q_OBJECT

public:
    explicit SelectionControl(QWidget* parent = nullptr);
    ~SelectionControl();
    void checkStateSet() override;
    Qt::CheckState checkState() const;

Q_SIGNALS:
    void stateChanged(int);

protected:
    void enterEvent(QEvent*) override;

    void nextCheckState() override;
    virtual void toggle(Qt::CheckState state) = 0;
};

class SwitchCheckBox :public SelectionControl {
    Q_OBJECT

public:
    explicit SwitchCheckBox(bool state, QWidget* parent = nullptr);
    explicit SwitchCheckBox(bool state, const QString& text, QWidget* parent = nullptr);
    SwitchCheckBox(bool state, const QString& text, const QBrush&,  QWidget* parent = nullptr);
    ~SwitchCheckBox();

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent*) override;
    void toggle(Qt::CheckState) override;

    void init();
    QRect indicatorRect();
    QRect textRect();

    static inline QColor colorFromOpacity(const QColor& c, qreal opacity) {
        return QColor(c.red(), c.green(), c.blue(), opacity * 255.0);
    }
    static inline bool ltr(QWidget* w) {
        if (w)
            return w->layoutDirection() == Qt::LeftToRight;
        return false;
    }

private:
    Style::SwitchCheckBox style;
    QPixmap shadowPixmap;
    QPointer<Animator> thumbBrushAnimation;
    QPointer<Animator> trackBrushAnimation;
    QPointer<Animator> thumbPosAniamtion;
};

#endif // SWITCH_H
