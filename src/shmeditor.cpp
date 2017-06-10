#include "shmeditor.h"
#include "ui_shmeditor.h"
#include "db.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QIntValidator>

ShmEditor::ShmEditor(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint),
    ui(new Ui::ShmEditor),
    id(0)
{
    ui->setupUi(this);

    QSqlQuery q(Db::connection());
    q.prepare("select jenis from t1 group by jenis order by jenis asc");
    q.exec();
    while (q.next()) {
        ui->jenisComboBox->addItem(q.value(0).toString());
    }
    ui->jenisComboBox->setCurrentIndex(-1);
    ui->jenisComboBox->view()->setAlternatingRowColors(true);

    QDate today = QDate::currentDate();
    ui->tglMasukEdit->setEnabled(false);
    ui->tglKeluarEdit->setEnabled(false);
    ui->tglMasukEdit->setDate(today);
    ui->tglKeluarEdit->setDate(today);

    QIntValidator* luasValidator = new QIntValidator(this);
    luasValidator->setBottom(0);
    ui->luasEdit->setValidator(luasValidator);

    connect(ui->saveButton, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), SLOT(confirmReject()));

    connect(ui->tglMasukCheckBox, SIGNAL(toggled(bool)), ui->tglMasukEdit, SLOT(setEnabled(bool)));
    connect(ui->tglKeluarCheckBox, SIGNAL(toggled(bool)), ui->tglKeluarEdit, SLOT(setEnabled(bool)));

    setWindowTitle("Baru");
}

ShmEditor::~ShmEditor()
{
    delete ui;
}

bool ShmEditor::load(int id)
{
    this->id = id;

    QSqlQuery q(Db::connection());
    q.prepare("select * from t1 where id=?");
    q.bindValue(0, id);
    q.exec();
    q.next();

    ui->namaEdit->setText(q.value("nama").toString());
    ui->kontakEdit->setText(q.value("kontak").toString());
    ui->ktpEdit->setText(q.value("no_ktp").toString());
    ui->alamatEdit->setText(q.value("alamat").toString());

    ui->jenisComboBox->setCurrentText(q.value("jenis").toString());
    ui->noShmEdit->setText(q.value("no_shm").toString());
    ui->luasEdit->setText(q.value("luas_tanah").toString());
    ui->statusComboBox->setCurrentText(q.value("status").toString());

    QDate tglMasuk = q.value("tgl_masuk").toDate();
    QDate tglKeluar = q.value("tgl_keluar").toDate();

    if (tglMasuk.isValid()) {
        ui->tglMasukEdit->setDate(tglMasuk);
        ui->tglMasukCheckBox->setChecked(true);
    }

    if (tglKeluar.isValid()) {
        ui->tglKeluarEdit->setDate(tglKeluar);
        ui->tglKeluarCheckBox->setChecked(true);
    }

    ui->penerimaEdit->setText(q.value("penerima").toString());
    ui->diterimaOlehEdit->setText(q.value("diterima_oleh").toString());
    ui->catatanEdit->setPlainText(q.value("catatan").toString());

    setWindowTitle("Edit #" + QString::number(id));

    return true;
}

void ShmEditor::accept()
{
    if (QMessageBox::question(0, "Konfirmasi", "Simpan perubahan rekaman?", "&Ya", "&Tidak"))
        return;

    QSqlQuery q(Db::connection());

    if (id == 0) {
        q.prepare("insert into t1"
                  " (nama, no_ktp, kontak, alamat,"
                  "  jenis, no_shm, luas_tanah, status,"
                  "  tgl_masuk, diterima_oleh, tgl_keluar, penerima,"
                  "  catatan)"
                  " values"
                  " (:nama, :no_ktp, :kontak, :alamat,"
                  "  :jenis, :no_shm, :luas_tanah, :status,"
                  "  :tgl_masuk, :diterima_oleh, :tgl_keluar, :penerima,"
                  "  :catatan)");
    }
    else {
        q.prepare("update t1 set"
                  " nama=:nama, no_ktp=:no_ktp, kontak=:kontak, alamat=:alamat,"
                  " jenis=:jenis, no_shm=:no_shm, luas_tanah=:luas_tanah, status=:status,"
                  " tgl_masuk=:tgl_masuk, diterima_oleh=:diterima_oleh, tgl_keluar=:tgl_keluar, penerima=:penerima,"
                  " catatan=:catatan"
                  " where id=:id");
        q.bindValue(":id", id);
    }
    q.bindValue(":nama", ui->namaEdit->text().trimmed());
    q.bindValue(":no_ktp", ui->ktpEdit->text().trimmed());
    q.bindValue(":kontak", ui->kontakEdit->text().trimmed());
    q.bindValue(":alamat", ui->alamatEdit->text().trimmed());
    q.bindValue(":jenis", ui->jenisComboBox->currentText().trimmed());
    q.bindValue(":no_shm", ui->noShmEdit->text().trimmed());
    q.bindValue(":luas_tanah", ui->luasEdit->text().trimmed());
    q.bindValue(":status", ui->statusComboBox->currentText().trimmed());
    q.bindValue(":tgl_masuk", ui->tglMasukCheckBox->isChecked() ? ui->tglMasukEdit->date() : QVariant());
    q.bindValue(":diterima_oleh", ui->diterimaOlehEdit->text().trimmed());
    q.bindValue(":tgl_keluar", ui->tglKeluarCheckBox->isChecked() ? ui->tglKeluarEdit->date() : QVariant());
    q.bindValue(":penerima", ui->penerimaEdit->text().trimmed());
    q.bindValue(":catatan", ui->catatanEdit->toPlainText().trimmed());
    q.exec();

    QDialog::accept();

    QMessageBox::information(0, "Informasi", "Rekaman telah disimpan.");
}

void ShmEditor::confirmReject()
{
    if (QMessageBox::question(0, "Konfirmasi", "Batalkan perubahan rekaman?", "&Ya", "&Tidak"))
        return;

    QDialog::reject();
}
