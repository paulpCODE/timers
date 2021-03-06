#ifndef TIMECLASS_H
#define TIMECLASS_H

#include <vector>
#include <QTimer>
#include <QString>

enum type {not_setted, isTimer, isAlarm};
struct TimerAndAlarm {
private:
    QTimer *_clock;
    int _timeMillSec;
    type _type;
    QString _node;
public:

    int timeMillSec();
    type type();
    QTimer *clock();
    QString node();

    void Set_node(const QString &node);
    void Set_timeMillSec(const int &time);
    void Set_type(const int &numOftype);

    TimerAndAlarm();
};



class timeclass : public TimerAndAlarm
{
public:
    std::vector<TimerAndAlarm> timersAndAlarmsList;

    QTimer *currTimer;
    timeclass();
};

#endif // TIMECLASS_H
