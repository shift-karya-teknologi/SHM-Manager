#include "shmmanager.h"
#include "ui_shmmanager.h"
#include "shmeditor.h"
#include "db.h"
#include <QDate>
#include <QSqlQuery>
#include <QSortFilterProxyModel>
#include <QDebug>
#include <QMessageBox>

class ShmManager::Model : public QAbstractTableModel
{
public:
    struct Data
    {
        int id;
        QString status;
        QDate tglMasuk;
        QDate tglKeluar;
        QString noShm;
        QString jenis;
        QString nama;
        QString kontak;
        QString alamat;
    };

    QList<Data> items;

    Model(QObject* parent)
        : QAbstractTableModel(parent)
    {}

    bool refresh()
    {
        QList<Data> tmpItems;

        QSqlDatabase db = Db::connection();
        QSqlQuery q(db);
        q.prepare("select * from t1 order by id asc");
        q.exec();
        while (q.next()) {
            Data d;
            d.id = q.value("id").toInt();
            d.nama = q.value("nama").toString();
            d.alamat = q.value("alamat").toString();
            d.kontak = q.value("kontak").toString();
            d.tglMasuk = q.value("tgl_masuk").toDate();
            d.tglKeluar = q.value("tgl_keluar").toDate();
            d.jenis = q.value("jenis").toString();
            d.noShm = q.value("no_shm").toString();
            d.status = q.value("status").toString();
            tmpItems.append(d);
        }

        beginResetModel();
        items.clear();
        items = tmpItems;
        endResetModel();
        return true;
    }

    int rowCount(const QModelIndex& = QModelIndex()) const
    {
        return items.size();
    }

    int columnCount(const QModelIndex& = QModelIndex()) const
    {
        return 9;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
    {
        if (role == Qt::DisplayRole) {
            Data d = items.at(index.row());
            switch (index.column()) {
            case 0: return d.id;
            case 1: return d.status;
            case 2: return d.tglMasuk;
            case 3: return d.tglKeluar;
            case 4: return d.jenis;
            case 5: return d.noShm;
            case 6: return d.nama;
            case 7: return d.kontak;
            case 8: return d.alamat;
            }
        }
        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (orientation == Qt::Horizontal) {
            if (role == Qt::DisplayRole) {
                switch (section) {
                case 0: return "Kode";
                case 1: return "Status";
                case 2: return "Tgl Masuk";
                case 3: return "Tgl Keluar";
                case 4: return "Jenis";
                case 5: return "No. SHM";
                case 6: return "Atas Nama";
                case 7: return "Kontak";
                case 8: return "Alamat";
                default: break;
                }
            }
        }

        return QVariant();
    }
};

class ShmManager::ProxyModel : public QSortFilterProxyModel
{
public:
    QString _status;
    QString _search;

    ProxyModel(QObject* parent)
        : QSortFilterProxyModel(parent)
    {
        setSortCaseSensitivity(Qt::CaseInsensitive);
    }

    void setFilter(const QString& status, const QString& search)
    {
        _status = status;
        _search = search.trimmed();
        invalidateFilter();
    }

    bool filterAcceptsRow(int source_row, const QModelIndex &/*source_parent*/) const
    {
        bool accept = false;
        Model::Data d = static_cast<Model*>(sourceModel())->items.at(source_row);

        QString st = _status.toLower();
        if (st == "semua" || d.status.toLower() == st) {
            accept = true;
        }

        QString s = _search.toLower();
        if (accept && s.isEmpty()) {
            return true;
        }

        if (accept && (d.nama.toLower().contains(s)
            || d.kontak.toLower().contains(s)
            || d.alamat.toLower().contains(s)
            || d.noShm.toLower().contains(s)
            || d.jenis.toLower().contains(s))) {
            return true;
        }

        return false;
    }
};

ShmManager::ShmManager(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint),
    ui(new Ui::ShmManager),
    model(new Model(this)),
    proxyModel(new ProxyModel(this))
{
    proxyModel->setSourceModel(model);

    ui->setupUi(this);

    ui->tableView->setModel(proxyModel);

    connect(ui->refreshButton, SIGNAL(clicked(bool)), SLOT(refresh()));
    connect(ui->addButton, SIGNAL(clicked(bool)), SLOT(add()));
    connect(ui->deleteButton, SIGNAL(clicked(bool)), SLOT(remove()));
    connect(ui->tableView, SIGNAL(activated(QModelIndex)), SLOT(edit(QModelIndex)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), SLOT(applyFilter()));
    connect(ui->searchEdit, SIGNAL(textChanged(QString)), SLOT(applyFilter()));

    refresh();
}

void ShmManager::add()
{
    if (!property("ALLOW_EDITING").toBool())
        return;

    ShmEditor editor;
    if (editor.exec())
        refresh();
}

void ShmManager::remove()
{
    if (!property("ALLOW_EDITING").toBool())
        return;

    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(0, "Informasi", "Pilih rekaman yang akan dihapus.");
        return;
    }

    if (QMessageBox::question(0, "Konfirmasi", "Hapus rekaman?", "&Ya", "&Tidak"))
        return;

    int id = index.sibling(index.row(), 0).data().toInt();
    QSqlQuery q(Db::connection());
    q.prepare("delete from t1 where id=?");
    q.bindValue(0, id);
    q.exec();

    refresh();

    QMessageBox::information(0, "Informasi", "Rekaman telah dihapus.");
}

void ShmManager::edit(const QModelIndex& index)
{
    if (!property("ALLOW_EDITING").toBool())
        return;

    if (!index.isValid()) {
        return;
    }

    ShmEditor editor;
    int id = index.sibling(index.row(), 0).data().toInt();
    editor.load(id);
    if (editor.exec())
        refresh();
}

ShmManager::~ShmManager()
{
    delete ui;
}

void ShmManager::refresh()
{
    model->refresh();

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    applyFilter();
}

void ShmManager::applyFilter()
{
    proxyModel->setFilter(ui->comboBox->currentText(), ui->searchEdit->text());

    int totalRowCount = model->rowCount();
    int filteredRowCount = proxyModel->rowCount();

    QString text;
    if (totalRowCount == filteredRowCount) {
        text = QString("Menampilkan %1 rekaman").arg(QLocale().toString(totalRowCount));
    }
    else {
        text = QString("Menampilkan %1 rekaman dari total %2 rekaman").arg(QLocale().toString(filteredRowCount), QLocale().toString(totalRowCount));
    }
    ui->label->setText(text);
}

void ShmManager::applAcl()
{
    bool allowEditing = property("ALLOW_EDITING").toBool();
    ui->addButton->setVisible(allowEditing);
    ui->deleteButton->setVisible(allowEditing);
}
