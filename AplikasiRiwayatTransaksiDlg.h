#pragma once

struct DetailTransaksi;
struct Transaksi;

class CAplikasiRiwayatTransaksiDlg : public CDialogEx
{
// Construction
public:
	CAplikasiRiwayatTransaksiDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APLIKASIRIWAYATTRANSAKSI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedBtnKeranjang();
	afx_msg void OnBnClickedBtnSimpan();
	void TampilkanLayar(int nomorLayar);
	void GenerateIDOtomatis();
	void LoadDataFromDB();
	afx_msg void OnBnClickedBtnMenuInput();
	afx_msg void OnBnClickedBtnMenuNav();
	afx_msg void OnBnClickedBtnMenuLaporan();
	void TampilkanDataNavigasi(Transaksi* node);
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnCari();
	afx_msg void OnStnClickedStaticJudulnav();
	afx_msg void OnBnClickedBtnSaveas();
	afx_msg void OnNMClickListLaporan(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnUpdate();
	afx_msg void OnBnClickedBtnHapus();
};
