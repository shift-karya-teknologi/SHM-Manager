#ifndef SHMEDITOR_H
#define SHMEDITOR_H

#include <QDialog>

namespace Ui {
class ShmEditor;
}

class ShmEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ShmEditor(QWidget *parent = 0);
    ~ShmEditor();

    bool load(int id);

public slots:
    void accept();
    void confirmReject();

private:
    Ui::ShmEditor *ui;
    int id;
};

#endif // SHMEDITOR_H
