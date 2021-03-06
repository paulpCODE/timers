#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->timeEdit->setDisplayFormat("hh:mm:ss");
    group = " Group 1";
    clickSound = true;
    doubleclick = false;
    hints = true;
    offButtons();
    currTime();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::offButtons() {
    ui->AddToGroupOfTimers->setEnabled(false);
    ui->RemoveFromGroupOfTimers->setEnabled(false);
    ui->start->setEnabled(false);
    ui->Delete->setEnabled(false);
    ui->StartGroup->setEnabled(false);
}

void MainWindow::update_timer(const unsigned long long int &index)
{
    QListWidgetItem *selectedTimer = new QListWidgetItem;
    selectedTimer = ui->listWidget->item(index);
    int time = timersAndAlarmsList[index].timeMillSec();
    int i = 0;
    while(i != time) {
        i += 1000;
        timersAndAlarmsList[index].clock()->singleShot(i, this, [=](){
                selectedTimer->setText("Timer: " + QTime(0,0,0).addMSecs(time - i).toString() + timersAndAlarmsList[index].node());
                if(i == time) {
                    if(!ui->DisturbBox->isTristate()) {
                        playerTimer->play();
                        QMessageBox::information(this, "Timer", "Time ended.", QMessageBox::Ok);
                        playerTimer->stop(); }
                    timersAndAlarmsList[index].clock()->stop();
                    selectedTimer->setText("Timer: " + QTime(0,0,0).addMSecs(time).toString() + timersAndAlarmsList[index].node());
                }
        });
    }
}

void MainWindow::update_alarm(const unsigned long long int &index) {
    int time = timersAndAlarmsList[index].timeMillSec();
    timersAndAlarmsList[index].clock()->singleShot(time, this, [=](){
       timersAndAlarmsList[index].clock()->stop();
       if(!ui->DisturbBox->isTristate()) {
           playerAlarm->play();
           QMessageBox::information(this, "Alarm clock", "Wake up.", QMessageBox::Ok);
           playerAlarm->stop(); }
    });
}

void MainWindow::start_alarm(const unsigned long long int &index) {
    const int currTimeMillSec = QTime(0,0,0).currentTime().second()*1000 + QTime(0,0,0).currentTime().minute()*60000 + QTime(0,0,0).currentTime().hour()*3600000;
    int time;
    int timeMillSec;
    if(currTimeMillSec > timersAndAlarmsList[index].timeMillSec()) {
        time = 24*3600000 - (currTimeMillSec - timersAndAlarmsList[index].timeMillSec());
    } else {
        time= timersAndAlarmsList[index].timeMillSec() - currTimeMillSec;
    }
    timeMillSec = timersAndAlarmsList[index].timeMillSec();
    timersAndAlarmsList[index].Set_timeMillSec(time);
    timersAndAlarmsList[index].clock()->start();
    update_alarm(index);
    timersAndAlarmsList[index].Set_timeMillSec(timeMillSec);
}

void MainWindow::currTime() {
    const int currTime = QTime(0,0,0).currentTime().second()*1000 + QTime(0,0,0).currentTime().minute()*60000 + QTime(0,0,0).currentTime().hour()*3600000;
    int i = 0;
    currTimer->start();
    while(i != 2*3600000) {
        i+=1000;
        currTimer->singleShot(i, this, [=](){
        ui->statusbar->showMessage(QTime(0,0,0).addMSecs(currTime + i).toString());
        if(i == 2*3600000) { currTimer->stop(); }
        });
    }



}

//UI:
void MainWindow::on_addTimerButton_clicked()
{
    TimerAndAlarm newTimer;
    newTimer.Set_type(1);
    if(ui->timeEdit->time().msecsSinceStartOfDay() == 0) {
        ui->statusbar->showMessage("Invalid time");
        return;
    }
    newTimer.Set_timeMillSec(ui->timeEdit->time().msecsSinceStartOfDay());
    timersAndAlarmsList.push_back(newTimer);
    QListWidgetItem *addTimer = new QListWidgetItem;
    addTimer->setText("Timer: " + QTime(0,0,0).addMSecs(timersAndAlarmsList.back().timeMillSec()).toString());
    ui->listWidget->addItem(addTimer);
}


void MainWindow::on_addAlarmButton_clicked()
{
    TimerAndAlarm newAlarm;
    newAlarm.Set_type(2);
    const int alarmtime = ui->timeEdit->time().msecsSinceStartOfDay();
    newAlarm.Set_timeMillSec(alarmtime);
    timersAndAlarmsList.push_back(newAlarm);
    QListWidgetItem *addAlarm = new QListWidgetItem;
    addAlarm->setText("Alarm: " + QTime(0,0,0).addMSecs(timersAndAlarmsList.back().timeMillSec()).toString());
    ui->listWidget->addItem(addAlarm);
}


void MainWindow::on_AddToGroupOfTimers_clicked()
{
        const unsigned long long int index = ui->listWidget->currentRow();
        timersAndAlarmsList[index].Set_node(group);
        QListWidgetItem *selectedTimer = new QListWidgetItem;
        selectedTimer = ui->listWidget->currentItem();
        if(!timersAndAlarmsList[index].clock()->isActive() && timersAndAlarmsList[index].type() == isTimer) {
            selectedTimer->setText("Timer: " + QTime(0,0,0).addMSecs(timersAndAlarmsList[index].timeMillSec()).toString() + timersAndAlarmsList[index].node());
        } else if  (timersAndAlarmsList[index].type() == isAlarm) {
            selectedTimer->setText("Alarm: " + QTime(0,0,0).addMSecs(timersAndAlarmsList[index].timeMillSec()).toString() + timersAndAlarmsList[index].node());
        }
        if (timersAndAlarmsList[index].node() == " Group 1") { selectedTimer->setTextColor("green"); }
        else if (timersAndAlarmsList[index].node() == " Group 2") { selectedTimer->setTextColor("blue"); }
        else { selectedTimer->setTextColor("red"); }
}


void MainWindow::on_start_clicked()
{
    const unsigned long long int index = ui->listWidget->currentRow();
    if(timersAndAlarmsList[index].clock()->isActive()) { return; }
    if(hints) {
        if(timersAndAlarmsList[index].node() != "") {
            QMessageBox::StandardButton reply =  QMessageBox::question(this, "Start group", "You can start group of timers. Do you want?",
                                                                      QMessageBox::Yes | QMessageBox::No);
            if(reply == QMessageBox::Yes) {
                on_StartGroup_clicked();
                return;
            }
        }
    }
    if(timersAndAlarmsList[index].type() == isTimer) {
        timersAndAlarmsList[index].clock()->start();
        update_timer(index);
    } else if(timersAndAlarmsList[index].type() == isAlarm) {
        start_alarm(index);
    }
}


void MainWindow::on_StartGroup_clicked()
{
        unsigned long long int index = ui->listWidget->currentRow();
        const QString changedGroup = timersAndAlarmsList[index].node();
        if(changedGroup == "") { return; }
            for(index = 0; index < timersAndAlarmsList.size(); ++index) {
                if(timersAndAlarmsList[index].node() == changedGroup) {
                    if(timersAndAlarmsList[index].clock()->isActive()) { continue; }
                        if(timersAndAlarmsList[index].type() == isTimer) {
                            timersAndAlarmsList[index].clock()->start();
                            update_timer(index);
                        } else {
                            start_alarm(index);
                        }
                }
            }
}

void MainWindow::on_Delete_clicked()
{
    const int index = ui->listWidget->currentRow();
    if(timersAndAlarmsList[index].clock()->isActive()) {
        QMessageBox::warning(this, "UPS", "You cant delete object while it works", QMessageBox::Ok);
        return;
    }
        timersAndAlarmsList.erase(timersAndAlarmsList.begin() + index);
        ui->listWidget->takeItem(index);
        if(timersAndAlarmsList.size() == 0) {
            offButtons();
        }
}

void MainWindow::on_DisturbBox_clicked(bool checked)
{
    if(checked) { ui->DisturbBox->setTristate(true); } else {
                  ui->DisturbBox->setTristate(false);
     }
}

void MainWindow::on_RemoveFromGroupOfTimers_clicked()
{
        const unsigned long long int index = ui->listWidget->currentRow();
        QListWidgetItem *selectedTimer = new QListWidgetItem;
        selectedTimer = ui->listWidget->currentItem();
        timersAndAlarmsList[index].Set_node("");
        if(!timersAndAlarmsList[index].clock()->isActive() && timersAndAlarmsList[index].type() == isTimer) {
            selectedTimer->setText("Timer: " + QTime(0,0,0).addMSecs(timersAndAlarmsList[index].timeMillSec()).toString() + timersAndAlarmsList[index].node());
        } else if (timersAndAlarmsList[index].type() == isAlarm) {
            selectedTimer->setText("Alarm: " + QTime(0,0,0).addMSecs(timersAndAlarmsList[index].timeMillSec()).toString() + timersAndAlarmsList[index].node());
        }
        selectedTimer->setTextColor("black");
}

void MainWindow::on_listWidget_itemSelectionChanged()
{
    if (clickSound) { playerChange->play(); }
    ui->AddToGroupOfTimers->setEnabled(true);
    ui->RemoveFromGroupOfTimers->setEnabled(true);
    ui->start->setEnabled(true);
    ui->Delete->setEnabled(true);
    ui->StartGroup->setEnabled(true);
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    item = new QListWidgetItem;
    if(doubleclick) {
        on_start_clicked();
    }
}














void MainWindow::on_actionhours_minutes_seconds_triggered()
{
    ui->timeEdit->setDisplayFormat("hh:mm:ss");
}
void MainWindow::on_actionhours_minutes_triggered()
{
    ui->timeEdit->setDisplayFormat("hh:mm");
}
void MainWindow::on_actionminutes_seconds_triggered()
{
    ui->timeEdit->setDisplayFormat("mm:ss");
}


void MainWindow::on_actionClick_Sound_triggered(bool checked)
{
    if(checked) { clickSound = false; }
    else { clickSound = true; }
}

void MainWindow::on_actionDisable_hints_triggered(bool checked)
{
    if(checked) { hints = false; }
    else { hints = true; }
}

void MainWindow::on_actionDouble_click_to_Start_triggered(bool checked)
{
    if(checked) { doubleclick = true; }
    else { doubleclick = false; }
}


void MainWindow::on_actionGroup_1_triggered()
{
    group = " Group 1";
}
void MainWindow::on_actionGroup_2_triggered()
{
    group = " Group 2";
}
void MainWindow::on_actionGroup_3_triggered()
{
    group = " Group 3";
}

//Music:
void MainWindow::on_actionbeep_beep_Standart_triggered() { playerTimer->setMedia(QUrl("qrc:/sounds/recources/alarm-clock-beep.mp3")); }
void MainWindow::on_actionVery_loud_music_triggered() { playerTimer->setMedia(QUrl("qrc:/sounds/recources/Very-loud-alarm.mp3")); }
void MainWindow::on_actionHappy_New_Year_shortened_edition_triggered() { playerTimer->setMedia(QUrl("qrc:/sounds/recources/Happy-New-Year-short-edition.mp3")); }
void MainWindow::on_actionHappy_New_Year_Standart_triggered() { playerAlarm->setMedia(QUrl("qrc:/sounds/recources/Happy-New-Year-alarm.mp3")); }
void MainWindow::on_actionMellen_Gi_Remix_triggered() { playerAlarm->setMedia(QUrl("qrc:/sounds/recources/Mellen-Gi-Remix-alarm..mp3")); }
void MainWindow::on_actionGood_sound_triggered() { playerAlarm->setMedia(QUrl("qrc:/sounds/recources/Good-sound-for-alarm.mp3")); }
void MainWindow::on_actionProsipaysa_Moi_Hazain_Fun_triggered() { playerAlarm->setMedia(QUrl("qrc:/sounds/recources/alarm-sound.mp3")); }


