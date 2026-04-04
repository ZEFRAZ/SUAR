#include "midcircle.h"

/**
 * @brief Menggambar 8 titik simetris sekaligus untuk satu langkah perhitungan lingkaran.
 *
 * Karena lingkaran memiliki simetri di 8 oktan (seperdelapan putaran),
 * Jadi hanya perlu menghitung satu oktan lalu memancarkan hasilnya ke 7 posisi cermin lainnya.
 * Fungsi ini dipanggil di setiap langkah oleh Midcircle().
 *
 * @param cx    Koordinat X pusat lingkaran.
 * @param cy    Koordinat Y pusat lingkaran.
 * @param x     Jarak horizontal dari pusat pada langkah saat ini.
 * @param y     Jarak vertikal dari pusat pada langkah saat ini.
 * @param color Warna titik yang akan digambar.
 */
static void GambarTitikSimetri(int cx, int cy, int x, int y, Color color) {
    DrawPixel(cx + x, cy + y, color); // Oktan 1
    DrawPixel(cx + y, cy + x, color); // Oktan 2
    DrawPixel(cx + y, cy - x, color); // Oktan 3
    DrawPixel(cx + x, cy - y, color); // Oktan 4
    DrawPixel(cx - x, cy - y, color); // Oktan 5
    DrawPixel(cx - y, cy - x, color); // Oktan 6
    DrawPixel(cx - y, cy + x, color); // Oktan 7
    DrawPixel(cx - x, cy + y, color); // Oktan 8
}

/**
 * @brief Menggambar tepi lingkaran (garis keliling) menggunakan Algoritma Midpoint.
 *
 * Cara kerja: dimulai dari titik paling atas lingkaran, algoritma ini
 * memutuskan di setiap langkah apakah piksel berikutnya bergerak ke arah Timur
 * atau ke arah Tenggara, berdasarkan nilai parameter keputusan (d).
 * Semua perhitungan menggunakan bilangan bulat, tanpa pecahan.
 *
 * @param centerX Koordinat X pusat lingkaran.
 * @param centerY Koordinat Y pusat lingkaran.
 * @param radius  Jari-jari lingkaran dalam piksel.
 * @param color   Warna garis tepi lingkaran.
 */
void Midcircle(int centerX, int centerY, int radius, Color color) {
    if (radius <= 0) {
        DrawPixel(centerX, centerY, color);
        return;
    }

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius; // Nilai parameter keputusan awal

    while (y >= x) {
        GambarTitikSimetri(centerX, centerY, x, y, color);

        if (d < 0) {
            // Pilih piksel Timur: hanya geser X
            d = d + 4 * x + 6;
        } else {
            // Pilih piksel Tenggara: geser X, turunkan Y
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

/**
 * @brief Menggambar elips menggunakan Algoritma Midpoint Ellipse.
 *
 * Cara kerja: elips dibagi menjadi dua bagian (Region 1 dan Region 2)
 * berdasarkan kemiringan kurva. Di Region 1, iterasi berjalan sepanjang
 * sumbu X, sedangkan di Region 2 berjalan sepanjang sumbu Y.
 * Semua perhitungan menggunakan bilangan bulat.
 *
 * @param center_x Koordinat X pusat elips.
 * @param center_y Koordinat Y pusat elips.
 * @param rx       Jari-jari horizontal (sumbu X).
 * @param ry       Jari-jari vertikal (sumbu Y).
 * @param color    Warna tepi elips yang akan digambar.
 */
void MidpointEllipse(int center_x, int center_y, int rx, int ry, Color color) {
    if (rx == 0 || ry == 0) return;

    long long rx2 = (long long)rx * rx;
    long long ry2 = (long long)ry * ry;

    int x = 0;
    int y = ry;

    // Region 1: kemiringan kurva kurang dari 1, iterasi berdasarkan sumbu X
    long long p1 = 4 * ry2 - 4 * rx2 * ry + rx2;

    while (ry2 * x < rx2 * y) {
        DrawPixel(center_x + x, center_y + y, color);
        DrawPixel(center_x - x, center_y + y, color);
        DrawPixel(center_x + x, center_y - y, color);
        DrawPixel(center_x - x, center_y - y, color);

        x++;
        if (p1 < 0) {
            p1 += 4 * ry2 * (2 * x + 3);
        } else {
            y--;
            p1 += 4 * ry2 * (2 * x + 3) - 4 * rx2 * (2 * y + 2);
        }
    }

    // Region 2: kemiringan kurva lebih dari atau sama dengan 1, iterasi berdasarkan sumbu Y
    long long p2 = ry2 * (2 * x + 1) * (2 * x + 1) + 4 * rx2 * (y - 1) * (y - 1) - 4 * rx2 * ry2;

    while (y >= 0) {
        DrawPixel(center_x + x, center_y + y, color);
        DrawPixel(center_x - x, center_y + y, color);
        DrawPixel(center_x + x, center_y - y, color);
        DrawPixel(center_x - x, center_y - y, color);

        y--;
        if (p2 > 0) {
            p2 += 4 * rx2 * (-2 * y + 3);
        } else {
            x++;
            p2 += 4 * ry2 * (2 * x + 2) + 4 * rx2 * (-2 * y + 3);
        }
    }
}