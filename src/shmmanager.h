#ifndef SHMMANAGER_H
#define SHMMANAGER_H

#include <QDialog>

namespace Ui {
class ShmManager;
}

class ShmManager : public QDialog
{
    Q_OBJECT

public:
    explicit ShmManager(QWidget *parent = 0);
    ~ShmManager();

    void applAcl();

public slots:
    void refresh();
    void add();
    void remove();
    void edit(const QModelIndex& index);
    void applyFilter();

private:
    Ui::ShmManager *ui;
    class Model;
    class ProxyModel;
    Model* model;
    ProxyModel* proxyModel;
};

#endif // SHMMANAGER_H
