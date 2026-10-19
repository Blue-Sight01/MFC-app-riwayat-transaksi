// AplikasiRiwayatTransaksiDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "AplikasiRiwayatTransaksi.h"
#include "AplikasiRiwayatTransaksiDlg.h"
#include "afxdialogex.h"

// WAJIB DITAMBAHKAN: Library untuk Database
#include <afxdb.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ========================================================
// STRUKTUR DATA
// ========================================================

// 1. Struktur Keranjang (Single Linked List untuk Detail)
struct DetailTransaksi {
    CString namaLayanan;
    int harga;
    DetailTransaksi* nextItem;
};
struct Transaksi {
    CString idTransaksi;
    CString tanggal;
    CString namaPelanggan;
    int totalPembayaran;

    DetailTransaksi* headDetail; 

    Transaksi* next; 
    Transaksi* prev; 
};

// ========================================================
// VARIABEL GLOBAL
// ========================================================

// Pointer pengendali Doubly Linked List (Riwayat Keseluruhan)
Transaksi* head = NULL;
Transaksi* tail = NULL;
Transaksi* nodeSekarang = NULL;

// Pointer sementara untuk keranjang belanja kasir saat ini
DetailTransaksi* keranjangSementara = NULL;
int totalSementara = 0;
int nomorUrutTransaksi = 1;
class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Implementation
protected:
    DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CAplikasiRiwayatTransaksiDlg dialog
CAplikasiRiwayatTransaksiDlg::CAplikasiRiwayatTransaksiDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_APLIKASIRIWAYATTRANSAKSI_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAplikasiRiwayatTransaksiDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAplikasiRiwayatTransaksiDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_LBN_SELCHANGE(IDC_LIST_KERANJANG, &CAplikasiRiwayatTransaksiDlg::OnLbnSelchangeList1)
    ON_BN_CLICKED(IDC_BTN_KERANJANG, &CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnKeranjang)
    ON_BN_CLICKED(IDC_BTN_SIMPAN, &CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnSimpan)
    ON_BN_CLICKED(IDC_BTN_MENU_INPUT, &CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnMenuInput)
    ON_BN_CLICKED(IDC_BTN_MENU_NAV, &CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnMenuNav)
    ON_BN_CLICKED(IDC_BTN_MENU_LAPORAN, &CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnMenuLaporan)
    ON_BN_CLICKED(IDC_BTN_NEXT, &CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnNext)
    ON_BN_CLICKED(IDC_BTN_PREV, &CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnPrev)
    ON_BN_CLICKED(IDC_BTN_CARI, &CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnCari)
    ON_BN_CLICKED(IDC_BTN_SAVEAS, &CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnSaveas)
    ON_NOTIFY(NM_CLICK, IDC_LIST_LAPORAN, &CAplikasiRiwayatTransaksiDlg::OnNMClickListLaporan)
    ON_BN_CLICKED(IDC_BTN_UPDATE, &CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnUpdate)
    ON_BN_CLICKED(IDC_BTN_HAPUS, &CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnHapus)
END_MESSAGE_MAP()


// ========================================================
// FUNGSI INISIALISASI (ON INIT DIALOG)
// ========================================================
BOOL CAplikasiRiwayatTransaksiDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // --- LOGIKA MENGAMBIL ID TERAKHIR DARI DATABASE ---
    nomorUrutTransaksi = 1;

    CDatabase db;
    try {
        db.OpenEx(_T("DSN=PostgreSQL35W;UID=postgres;PWD=12345678;"), CDatabase::noOdbcDialog);
        CRecordset rs(&db);
        rs.Open(CRecordset::forwardOnly, _T("SELECT MAX(id_transaksi) AS max_id FROM riwayat_transaksi"), CRecordset::readOnly);

        if (!rs.IsEOF()) {
            CString strMaxId;
            rs.GetFieldValue(_T("max_id"), strMaxId);
            if (!strMaxId.IsEmpty()) {
                CString strAngka = strMaxId.Right(4);
                nomorUrutTransaksi = _ttoi(strAngka) + 1;
            }
        }
        rs.Close();
        db.Close();
    }
    catch (CDBException* e) {
        AfxMessageBox(_T("Info Load ID (Abaikan jika database baru): ") + e->m_strError);
        e->Delete();
    }

    // --- SETUP TABEL LAPORAN (MAHASISWA 3) ---
    CListCtrl* pListLaporan = (CListCtrl*)GetDlgItem(IDC_LIST_LAPORAN);
    if (pListLaporan != NULL) {
        pListLaporan->InsertColumn(0, _T("ID Transaksi"), LVCFMT_LEFT, 100);
        pListLaporan->InsertColumn(1, _T("Tanggal"), LVCFMT_LEFT, 100);
        pListLaporan->InsertColumn(2, _T("Nama Pelanggan"), LVCFMT_LEFT, 150);
        pListLaporan->InsertColumn(3, _T("Total Pembayaran"), LVCFMT_LEFT, 120);
        pListLaporan->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    }

    // Panggil fungsi sinkronisasi awal
    LoadDataFromDB();
    GenerateIDOtomatis();

    // Sembunyikan layar 2 dan 3 saat aplikasi pertama kali dibuka
    TampilkanLayar(1);

    return TRUE;
}

// ========================================================
// FUNGSI SINKRONISASI DATABASE & LINKED LIST
// ========================================================
void CAplikasiRiwayatTransaksiDlg::LoadDataFromDB()
{
    CDatabase db;
    head = NULL; tail = NULL;

    try {
        db.OpenEx(_T("DSN=PostgreSQL35W;UID=postgres;PWD=12345678;"), CDatabase::noOdbcDialog);

        CRecordset rsHeader(&db);
        rsHeader.Open(CRecordset::forwardOnly, _T("SELECT * FROM riwayat_transaksi ORDER BY id_transaksi ASC"), CRecordset::readOnly);

        while (!rsHeader.IsEOF()) {
            Transaksi* nodeBaru = new Transaksi();
            CString idTrx;
            rsHeader.GetFieldValue(_T("id_transaksi"), idTrx);
            rsHeader.GetFieldValue(_T("tanggal"), nodeBaru->tanggal);
            rsHeader.GetFieldValue(_T("nama_pelanggan"), nodeBaru->namaPelanggan);
            nodeBaru->idTransaksi = idTrx;

            CString strTotal;
            rsHeader.GetFieldValue(_T("total_pembayaran"), strTotal);
            nodeBaru->totalPembayaran = _ttoi(strTotal);
            nodeBaru->headDetail = NULL;
            nodeBaru->next = NULL;
            nodeBaru->prev = tail;

            // AMBIL DETAIL UNTUK TRX INI
            CRecordset rsDetail(&db);
            CString queryDetail;
            queryDetail.Format(_T("SELECT * FROM transaksi_detail WHERE id_transaksi = '%s'"), idTrx);
            rsDetail.Open(CRecordset::forwardOnly, queryDetail, CRecordset::readOnly);

            DetailTransaksi* lastDetail = NULL;
            while (!rsDetail.IsEOF()) {
                DetailTransaksi* item = new DetailTransaksi();
                rsDetail.GetFieldValue(_T("jenis_transaksi"), item->namaLayanan);
                CString hargaStr;
                rsDetail.GetFieldValue(_T("harga"), hargaStr);
                item->harga = _ttoi(hargaStr);
                item->nextItem = NULL;

                if (nodeBaru->headDetail == NULL) nodeBaru->headDetail = item;
                else lastDetail->nextItem = item;
                lastDetail = item;

                rsDetail.MoveNext();
            }
            rsDetail.Close();

            if (head == NULL) { head = nodeBaru; tail = nodeBaru; }
            else { tail->next = nodeBaru; tail = nodeBaru; }

            rsHeader.MoveNext();
        }
        rsHeader.Close(); db.Close();
    }
    catch (CDBException* e) {
        AfxMessageBox(_T("Gagal load database: ") + e->m_strError);
        e->Delete();
    }
}

// ========================================================
// SISTEM WINDOWS BAWAAN (Jangan Diubah)
// ========================================================
void CAplikasiRiwayatTransaksiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CAplikasiRiwayatTransaksiDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CAplikasiRiwayatTransaksiDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CAplikasiRiwayatTransaksiDlg::OnLbnSelchangeList1()
{
    // Handler untuk List Keranjang (Biarkan kosong jika tidak dipakai)
}

// ========================================================
// LOGIKA MAHASISWA 1: INPUT DATA
// ========================================================
void CAplikasiRiwayatTransaksiDlg::GenerateIDOtomatis()
{
    CString strIdBaru;
    strIdBaru.Format(_T("TRX-%04d"), nomorUrutTransaksi);
    SetDlgItemText(IDC_EDIT_ID, strIdBaru);
}

void CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnKeranjang()
{
    CString strLayanan, strHarga;
    GetDlgItemText(IDC_EDIT_JTRANSAKSI, strLayanan);
    GetDlgItemText(IDC_EDIT_HARGA, strHarga);

    if (strLayanan.IsEmpty() || strHarga.IsEmpty()) {
        AfxMessageBox(_T("Layanan dan Harga tidak boleh kosong!"));
        return;
    }

    int hargaItem = _ttoi(strHarga);

    DetailTransaksi* itemBaru = new DetailTransaksi();
    itemBaru->namaLayanan = strLayanan;
    itemBaru->harga = hargaItem;
    itemBaru->nextItem = NULL;

    if (keranjangSementara == NULL) {
        keranjangSementara = itemBaru;
    }
    else {
        DetailTransaksi* bantu = keranjangSementara;
        while (bantu->nextItem != NULL) { bantu = bantu->nextItem; }
        bantu->nextItem = itemBaru;
    }

    CString teksList;
    teksList.Format(_T("%s - Rp %d"), strLayanan, hargaItem);
    ((CListBox*)GetDlgItem(IDC_LIST_KERANJANG))->AddString(teksList);

    totalSementara += hargaItem;
    CString teksTotal;
    teksTotal.Format(_T("Total: Rp %d"), totalSementara);
    SetDlgItemText(IDC_TXT_TOTAL, teksTotal);

    SetDlgItemText(IDC_EDIT_JTRANSAKSI, _T(""));
    SetDlgItemText(IDC_EDIT_HARGA, _T(""));
}

void CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnSimpan()
{
    CString strId, strTgl, strNama;
    GetDlgItemText(IDC_EDIT_ID, strId);
    GetDlgItemText(IDC_EDIT_TGL, strTgl);
    GetDlgItemText(IDC_EDIT_NAMA, strNama);

    if (strId.IsEmpty() || keranjangSementara == NULL) {
        AfxMessageBox(_T("Data Header kosong atau Keranjang masih kosong!"));
        return;
    }

    Transaksi* nodeBaru = new Transaksi();
    nodeBaru->idTransaksi = strId;
    nodeBaru->tanggal = strTgl;
    nodeBaru->namaPelanggan = strNama;
    nodeBaru->totalPembayaran = totalSementara;
    nodeBaru->headDetail = keranjangSementara;
    nodeBaru->next = NULL;
    nodeBaru->prev = NULL;

    if (head == NULL) {
        head = nodeBaru;
        tail = nodeBaru;
        nodeSekarang = head;
    }
    else {
        tail->next = nodeBaru;
        nodeBaru->prev = tail;
        tail = nodeBaru;
    }

    CDatabase db;
    try {
        db.OpenEx(_T("DSN=PostgreSQL35W;UID=postgres;PWD=12345678;"), CDatabase::noOdbcDialog);

        CString queryHeader;
        queryHeader.Format(_T("INSERT INTO riwayat_transaksi (id_transaksi, tanggal, nama_pelanggan, total_pembayaran) VALUES ('%s', '%s', '%s', %d)"),
            strId, strTgl, strNama, totalSementara);
        db.ExecuteSQL(queryHeader);

        DetailTransaksi* bantuDetail = keranjangSementara;
        while (bantuDetail != NULL) {
            CString queryDetail;
            queryDetail.Format(_T("INSERT INTO transaksi_detail (id_transaksi, jenis_transaksi, harga) VALUES ('%s', '%s', %d)"),
                strId, bantuDetail->namaLayanan, bantuDetail->harga);
            db.ExecuteSQL(queryDetail);
            bantuDetail = bantuDetail->nextItem;
        }
        db.Close();
    }
    catch (CDBException* e) {
        AfxMessageBox(_T("Gagal menyimpan ke Database: ") + e->m_strError);
        e->Delete();
        return;
    }

    AfxMessageBox(_T("Transaksi Berhasil Disimpan!"));

    SetDlgItemText(IDC_EDIT_ID, _T(""));
    SetDlgItemText(IDC_EDIT_TGL, _T(""));
    SetDlgItemText(IDC_EDIT_NAMA, _T(""));
    ((CListBox*)GetDlgItem(IDC_LIST_KERANJANG))->ResetContent();
    SetDlgItemText(IDC_TXT_TOTAL, _T("Total: Rp 0"));

    keranjangSementara = NULL;
    totalSementara = 0;

    nomorUrutTransaksi++;
    GenerateIDOtomatis();
}

// ========================================================
// SISTEM NAVIGASI & MENU
// ========================================================
void CAplikasiRiwayatTransaksiDlg::TampilkanLayar(int nomorLayar)
{
    int menu1[] = { IDC_EDIT_ID, IDC_EDIT_TGL, IDC_EDIT_NAMA, IDC_EDIT_JTRANSAKSI, IDC_EDIT_HARGA, IDC_BTN_KERANJANG, IDC_LIST_KERANJANG, IDC_TXT_TOTAL, IDC_BTN_SIMPAN, IDC_STATIC_ID, IDC_STATIC_JT, IDC_STATIC_PELANGGAN, IDC_STATIC_HARGA, IDC_STATIC_TGL, IDC_STATIC_JUDULINPUT };
    int menu2[] = { IDC_EDIT_CARI, IDC_BTN_CARI, IDC_NAV_ID, IDC_NAV_TGL, IDC_NAV_NAMA, IDC_NAV_TOTAL, IDC_NAV_LIST_DETAIL, IDC_BTN_PREV, IDC_BTN_NEXT, IDC_STATIC_IDT, IDC_STATIC_TGLNAV, IDC_STATIC_NPELANGGAN, IDC_STATIC_TP, IDC_STATIC_JUDULNAV };
    int menu3[] = { IDC_LIST_LAPORAN, IDC_STATIC_TOTAL_LAPORAN, IDC_BTN_HAPUS, IDC_BTN_SAVEAS, IDC_STATIC_JUDULLAPORAN, IDC_STATIC_LAPID, IDC_STATIC_LAPPEL, IDC_STATIC_LAPTGL, IDC_UPD_ID, IDC_UPD_NAMA, IDC_UPD_TGL, IDC_BTN_UPDATE };

    // Tampilkan/Sembunyikan komponen secara dinamis
    for (int id : menu1) { CWnd* p = GetDlgItem(id); if (p) p->ShowWindow(nomorLayar == 1 ? SW_SHOW : SW_HIDE); }
    for (int id : menu2) { CWnd* p = GetDlgItem(id); if (p) p->ShowWindow(nomorLayar == 2 ? SW_SHOW : SW_HIDE); }
    for (int id : menu3) { CWnd* p = GetDlgItem(id); if (p) p->ShowWindow(nomorLayar == 3 ? SW_SHOW : SW_HIDE); }

    // Tampilkan/Sembunyikan Bingkai Utama
    GetDlgItem(IDC_MAHASISWA1)->ShowWindow(nomorLayar == 1 ? SW_SHOW : SW_HIDE);
    GetDlgItem(IDC_MAHASISWA2)->ShowWindow(nomorLayar == 2 ? SW_SHOW : SW_HIDE);
    GetDlgItem(IDC_MAHASISWA3)->ShowWindow(nomorLayar == 3 ? SW_SHOW : SW_HIDE);
}
void CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnMenuInput() { TampilkanLayar(1); }
void CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnMenuNav()
{
    TampilkanLayar(2);
    if (head != NULL) {
        nodeSekarang = head;
        TampilkanDataNavigasi(nodeSekarang);
    }
    else {
        AfxMessageBox(_T("Belum ada data transaksi!"));
    }
}

void CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnMenuLaporan()
{   TampilkanLayar(3);
    CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_LAPORAN);
    if (pList == NULL) return;

    pList->DeleteAllItems();

    Transaksi* bantu = head;
    int barisTabel = 0;
    int totalPendapatan = 0;
    int jumlahData = 0;

    while (bantu != NULL) {
        pList->InsertItem(barisTabel, bantu->idTransaksi);
        pList->SetItemText(barisTabel, 1, bantu->tanggal);
        pList->SetItemText(barisTabel, 2, bantu->namaPelanggan);

        CString strTotal;
        strTotal.Format(_T("%d"), bantu->totalPembayaran);
        pList->SetItemText(barisTabel, 3, strTotal);

        totalPendapatan += bantu->totalPembayaran;
        jumlahData++;
        barisTabel++;
        bantu = bantu->next;
    }
    CString info;
    info.Format(_T("Total Data: %d Transaksi  |  Total Pendapatan: Rp %d"), jumlahData, totalPendapatan);
    SetDlgItemText(IDC_STATIC_TOTAL_LAPORAN, info);
}

// ========================================================
// LOGIKA MAHASISWA 2: PENCARIAN & NAVIGASI
// ========================================================
void CAplikasiRiwayatTransaksiDlg::TampilkanDataNavigasi(Transaksi* node)
{
    if (node == NULL) return;

    SetDlgItemText(IDC_NAV_ID, node->idTransaksi);
    SetDlgItemText(IDC_NAV_TGL, node->tanggal);
    SetDlgItemText(IDC_NAV_NAMA, node->namaPelanggan);

    CString strTotal;
    strTotal.Format(_T("Rp %d"), node->totalPembayaran);
    SetDlgItemText(IDC_NAV_TOTAL, strTotal);

    CListBox* pList = (CListBox*)GetDlgItem(IDC_NAV_LIST_DETAIL);
    if (pList != NULL) {
        pList->ResetContent();
        DetailTransaksi* b = node->headDetail;
        while (b != NULL) {
            CString item;
            item.Format(_T("%s - Rp %d"), b->namaLayanan, b->harga);
            pList->AddString(item);
            b = b->nextItem;
        }
    }
}

void CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnNext()
{
    if (nodeSekarang != NULL && nodeSekarang->next != NULL) {
        nodeSekarang = nodeSekarang->next;
        TampilkanDataNavigasi(nodeSekarang);
    }
    else {
        AfxMessageBox(_T("Ini data terakhir!"));
    }
}

void CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnPrev()
{
    if (nodeSekarang != NULL && nodeSekarang->prev != NULL) {
        nodeSekarang = nodeSekarang->prev;
        TampilkanDataNavigasi(nodeSekarang);
    }
    else {
        AfxMessageBox(_T("Ini data pertama!"));
    }
}

void CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnCari()
{
    CString strInputCari;
    GetDlgItemText(IDC_EDIT_CARI, strInputCari);

    if (strInputCari.IsEmpty()) {
        AfxMessageBox(_T("Masukkan ID atau Nama Pelanggan untuk mencari!"));
        return;
    }

    Transaksi* bantu = head;
    bool ketemu = false;

    while (bantu != NULL) {
        if (bantu->idTransaksi.CompareNoCase(strInputCari) == 0 ||
            bantu->namaPelanggan.CompareNoCase(strInputCari) == 0)
        {
            nodeSekarang = bantu;
            TampilkanDataNavigasi(nodeSekarang);
            ketemu = true;
            break;
        }
        bantu = bantu->next;
    }

    if (!ketemu) {
        AfxMessageBox(_T("Data tidak ditemukan! Coba periksa ID atau Nama Pelanggan."));
    }
}

// ========================================================
// LOGIKA MAHASISWA 3: LAPORAN, EDIT, HAPUS, CETAK
// ========================================================
void CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnSaveas()
{
    CFileDialog fileDlg(FALSE, _T("txt"), _T("Rekap_Laporan_Transaksi.txt"),
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"));

    if (fileDlg.DoModal() == IDOK) {
        CStdioFile file;
        if (file.Open(fileDlg.GetPathName(), CFile::modeCreate | CFile::modeWrite)) {
            file.WriteString(_T("====================================\n"));
            file.WriteString(_T("    REKAP LAPORAN TRANSAKSI\n"));
            file.WriteString(_T("====================================\n\n"));

            Transaksi* bantu = head;
            int totalSemua = 0;

            while (bantu != NULL) {
                CString baris;
                baris.Format(_T("ID: %s\nTanggal: %s\nPelanggan: %s\nTotal: Rp %d\n--------------------\n"),
                    bantu->idTransaksi, bantu->tanggal, bantu->namaPelanggan, bantu->totalPembayaran);

                file.WriteString(baris);
                totalSemua += bantu->totalPembayaran;
                bantu = bantu->next;
            }

            CString footer;
            footer.Format(_T("\nTOTAL PENDAPATAN KESELURUHAN: Rp %d\n"), totalSemua);
            file.WriteString(footer);

            file.Close();
            AfxMessageBox(_T("Laporan Berhasil Disimpan!"));
        }
    }
}

void CAplikasiRiwayatTransaksiDlg::OnNMClickListLaporan(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    int barisTerpilih = pNMItemActivate->iItem;

    if (barisTerpilih >= 0) {
        CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_LAPORAN);

        CString strId = pList->GetItemText(barisTerpilih, 0);
        CString strTgl = pList->GetItemText(barisTerpilih, 1);
        CString strNama = pList->GetItemText(barisTerpilih, 2);

        SetDlgItemText(IDC_UPD_ID, strId);
        SetDlgItemText(IDC_UPD_TGL, strTgl);
        SetDlgItemText(IDC_UPD_NAMA, strNama);
    }
    *pResult = 0;
}

void CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnUpdate()
{
    CString updId, updNama, updTgl;
    GetDlgItemText(IDC_UPD_ID, updId);
    GetDlgItemText(IDC_UPD_NAMA, updNama);
    GetDlgItemText(IDC_UPD_TGL, updTgl);

    if (updId.IsEmpty()) {
        AfxMessageBox(_T("Pilih dulu data di tabel yang mau diubah!"));
        return;
    }

    CDatabase db;
    try {
        db.OpenEx(_T("DSN=PostgreSQL35W;UID=postgres;PWD=12345678;"), CDatabase::noOdbcDialog);
        CString query;
        query.Format(_T("UPDATE riwayat_transaksi SET nama_pelanggan='%s', tanggal='%s' WHERE id_transaksi='%s'"),
            updNama, updTgl, updId);
        db.ExecuteSQL(query);
        db.Close();

        AfxMessageBox(_T("Data berhasil diperbarui!"));

        LoadDataFromDB();
        OnBnClickedBtnMenuLaporan();

        SetDlgItemText(IDC_UPD_ID, _T(""));
        SetDlgItemText(IDC_UPD_NAMA, _T(""));
        SetDlgItemText(IDC_UPD_TGL, _T(""));
    }
    catch (CDBException* e) {
        AfxMessageBox(_T("Gagal Update: ") + e->m_strError);
        e->Delete();
    }
}

void CAplikasiRiwayatTransaksiDlg::OnBnClickedBtnHapus()
{
    CString delId;
    GetDlgItemText(IDC_UPD_ID, delId);

    if (delId.IsEmpty()) {
        AfxMessageBox(_T("Pilih dulu data di tabel yang mau dihapus!"));
        return;
    }

    if (MessageBox(_T("Yakin ingin menghapus transaksi ini?"), _T("Konfirmasi"), MB_YESNO | MB_ICONWARNING) == IDYES)
    {
        CDatabase db;
        try {
            db.OpenEx(_T("DSN=PostgreSQL35W;UID=postgres;PWD=12345678;"), CDatabase::noOdbcDialog);

            CString queryDetail, queryHeader;
            queryDetail.Format(_T("DELETE FROM transaksi_detail WHERE id_transaksi='%s'"), delId);
            db.ExecuteSQL(queryDetail);

            queryHeader.Format(_T("DELETE FROM riwayat_transaksi WHERE id_transaksi='%s'"), delId);
            db.ExecuteSQL(queryHeader);

            db.Close();

            AfxMessageBox(_T("Data berhasil dihapus!"));

            LoadDataFromDB();
            OnBnClickedBtnMenuLaporan();

            SetDlgItemText(IDC_UPD_ID, _T(""));
            SetDlgItemText(IDC_UPD_NAMA, _T(""));
            SetDlgItemText(IDC_UPD_TGL, _T(""));
        }
        catch (CDBException* e) {
            AfxMessageBox(_T("Gagal Hapus: ") + e->m_strError);
            e->Delete();
        }
    }
}