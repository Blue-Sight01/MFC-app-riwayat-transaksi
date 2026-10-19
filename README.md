# MFC-app-riwayat-transaksi
Aplikasi kasir berbasis C++ MFC dan PostgreSQL. Fitur utama meliputi input multi-item per transaksi, pencarian riwayat dengan navigasi sequential (Next/Prev), filter transaksi bulanan, serta ekspor laporan keuangan langsung ke format teks (.txt).
## 🛠️ Prasyarat
Sebelum menjalankan aplikasi ini, pastikan sistem kamu sudah terinstal:
- **Visual Studio** (pastikan workload *Desktop development with C++* dan komponen *C++ MFC for latest v142/v143 build tools* sudah dicentang).
- **PostgreSQL** (sebagai database server).
- **psqlODBC** (Driver ODBC untuk menjembatani koneksi C++ MFC ke database PostgreSQL).

## 🗄️ Persiapan Database
1. Buat database baru di PostgreSQL untuk aplikasi kasir ini.
2. Buat tabel-tabel yang dibutuhkan (sesuaikan dengan query yang ada di dalam *source code*).
3. Buka **ODBC Data Source Administrator** di Windows.
4. Buat **System DSN** atau **User DSN** baru menggunakan driver PostgreSQL. 
5. Arahkan koneksi ke database yang baru dibuat. *Catatan penting: Pastikan nama DSN, username, dan password di setting ODBC sama persis dengan string koneksi `CDatabase` yang ada di dalam kode `.cpp`.*

## 🚀 Cara Menjalankan
1. Clone repositori ini ke komputer kamu:
   ```bash
   git clone [https://github.com/Blue-Sight01/MFC-app-riwayat-transaksi.git](https://github.com/Blue-Sight01/MFC-app-riwayat-transaksi.git)
