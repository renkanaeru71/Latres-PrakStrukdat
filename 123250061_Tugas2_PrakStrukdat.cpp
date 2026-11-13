#include <iostream>
using namespace std;

// Antrean

struct NodeAntrean {
    string namaPelanggan;
    NodeAntrean* next;
};

struct Antrean {
    NodeAntrean* front;
    NodeAntrean* rear;
    int count;
};

void init(Antrean& q) {
    q.front = nullptr;
    q.rear = nullptr;
    q.count = 0;
}

bool isEmpty(Antrean& q) {
    return q.front == nullptr;
}

void enAntrean(Antrean& q, string nama) {
    NodeAntrean* baru = new NodeAntrean();
    baru->namaPelanggan = nama;
    baru->next = nullptr;
    if (isEmpty(q)) {
        q.front = baru;
        q.rear = baru;
    } else {
        q.rear->next = baru;
        q.rear = baru;
    }
    q.count++;
}

bool deAntrean(Antrean& q, string& namaKeluar) {
    if (isEmpty(q)) {
        return false;
    }
    NodeAntrean* bantu = q.front;
    namaKeluar = bantu->namaPelanggan;
    q.front = q.front->next;
    if (q.front == nullptr) {
        q.rear = nullptr;
    }
    delete bantu;
    q.count--;
    return true;
}

// Tambah di posisi depan untuk keperluan undo
void pushFrontAntrean(Antrean& q, string nama) {
    NodeAntrean* baru = new NodeAntrean();
    baru->namaPelanggan = nama;
    baru->next = q.front;
    q.front = baru;
    if (q.rear == nullptr) {
        q.rear = baru;
    }
    q.count++;
}

// Hapus satu pelanggan dari waiting list saat undo pesanan
bool hapusAntrian(Antrean& q, string nama) {
    NodeAntrean* prev = nullptr;
    NodeAntrean* curr = q.front;
    while (curr != nullptr) {
        if (curr->namaPelanggan == nama) {
            if (prev == nullptr) {
                q.front = curr->next;
            } else {
                prev->next = curr->next;
            }
            if (curr == q.rear) {
                q.rear = prev;
            }
            delete curr;
            q.count--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

void clearAntrean(Antrean& q) {
    NodeAntrean* curr = q.front;
    while (curr != nullptr) {
        NodeAntrean* bantu = curr;
        curr = curr->next;
        delete bantu;
    }
    init(q);
}

// binary search

struct Node {
    string nama;
    string kategori;
    int harga;
    bool tersedia; // true = tersedia, false = abis
    Antrean waitingList;
    Node* left;
    Node* right;
};

// Inorder --> left --> root --> right
void inorder(Node* node) {
    if (node == nullptr) {
        return;
    }
    inorder(node->left); // untuk menelusuri ke kiri
    cout << "- " << node->nama << " | " << node->kategori << " | Rp" << node->harga
         << " | " << (node->tersedia ? "Tersedia" : "Habis") << "\n";
    inorder(node->right); // untuk menelusuri ke kanan
}

bool insert(Node*& node, string nama, string kategori, int harga) {
    // ketika nodenya itu kosong atau nullptr
    if (node == nullptr) {
        node = new Node();
        node->nama = nama;
        node->kategori = kategori;
        node->harga = harga;
        node->tersedia = true;
        init(node->waitingList);
        node->left = nullptr;
        node->right = nullptr;
        return true; // berhasil ngeinsert data
    }
    // ketika nilai kurang dari node/root
    if (nama < node->nama) {
        return insert(node->left, nama, kategori, harga); // data bakal menelusuri ke kiri
    }
    // ketika nilai lebih dari node/root
    if (nama > node->nama) {
        return insert(node->right, nama, kategori, harga); // data bakal menelusuri ke kanan
    }
    return false;
}

Node* cari(Node* node, string target) {
    // kondisi ketemu atau kosong
    if (node == nullptr || target == node->nama) {
        return node;
    }
    // kondisi ketika yang mau dicari itu kurang dari node
    if (target < node->nama) {
        return cari(node->left, target);
    }
    return cari(node->right, target);
}

// fungsi buat nyari nilai yang paling kecil
Node* minimum(Node* node) {
    if (node == nullptr) {
        return nullptr;
    }
    while (node->left != nullptr) {
        node = node->left; // bakal geser ke kiri
    }
    return node;
}   

bool hapus(Node*& node, string target) {
    if (node == nullptr) {
        return false;
    }
    if (target < node->nama) {
        return hapus(node->left, target);
    } else if (target > node->nama) {
        return hapus(node->right, target);
    } else {
        // ada beberapa kasus, childnya 0
        if (node->left == nullptr && node->right == nullptr) {
            clearAntrean(node->waitingList);
            delete node;
            node = nullptr;
        }
        // child 1 tapi yang sebelah kanan
        else if (node->left == nullptr) {
            Node* alatBantu = node; // membantu nyimpen node
            clearAntrean(node->waitingList);
            node = node->right;
            delete alatBantu;
        }
        // child 1 tapi yang sebelah kiri
        else if (node->right == nullptr) {
            Node* alatBantu = node;
            clearAntrean(node->waitingList);
            node = node->left;
            delete alatBantu;
        }
        // childnya itu ada 2
        else {
            Node* successor = minimum(node->right); // nilai paling kecil dari subtree kanannya root
            clearAntrean(node->waitingList);
            node->nama = successor->nama;
            node->kategori = successor->kategori;
            node->harga = successor->harga;
            node->tersedia = successor->tersedia;
            node->waitingList = successor->waitingList;
            init(successor->waitingList);
            hapus(node->right, successor->nama);
        }
        return true;
    }
}

void printAntrianSemua(Node* node, bool& ada) {
    if (node == nullptr) return;
    printAntrianSemua(node->left, ada);

    NodeAntrean* curr = node->waitingList.front;
    while (curr != nullptr) {
        cout << curr->namaPelanggan << " menunggu menu " << node->nama << "\n";
        ada = true;
        curr = curr->next;
    }
    printAntrianSemua(node->right, ada);
}

void clearTree(Node*& node) {
    if (node == nullptr) return;
    clearTree(node->left);
    clearTree(node->right);
    clearAntrean(node->waitingList);
    delete node;
    node = nullptr;
}
// Stack
struct NodeStack {
    string jenis; // "PESAN" atau "STATUS"
    string namaMenu;
    string pelanggan;
    NodeStack* next;
};

struct linkedStack {
    NodeStack* top;
};

void buatStack(linkedStack& s) {
    s.top = nullptr;
}

bool isEmpty(linkedStack& s) {
    return s.top == nullptr;
}

void push(linkedStack& s, string jenis, string namaMenu, string pelanggan) {
    NodeStack* newNode = new NodeStack();
    newNode->jenis = jenis;
    newNode->namaMenu = namaMenu;
    newNode->pelanggan = pelanggan;
    newNode->next = s.top;
    s.top = newNode;
}

bool pop(linkedStack& s, NodeStack& hasil) {
    if (isEmpty(s)) {
        return false;
    }
    NodeStack* temp = s.top;
    hasil = *temp;
    s.top = s.top->next;
    delete temp;
    return true;
}

void clearstack(linkedStack& s) {
    while (s.top != nullptr) {
        NodeStack* temp = s.top;
        s.top = s.top->next;
        delete temp;
    }
}
// operasi restoran
void menuTambah(Node*& root) {
    string nama, kategori;
    int harga;

    cout << "Nama menu     : ";
    getline(cin, nama);
    cout << "Kategori      : ";
    getline(cin, kategori);
    cout << "Harga         : ";
    cin >> harga;
    cin.ignore(1000, '\n');

    if (insert(root, nama, kategori, harga)) {
        cout << "Menu berhasil ditambahkan\n";
    } else {
        cout << "Menu sudah ada, gagal ditambahkan\n";
    }
}

void menuLihat(Node* root) {
    if (root == nullptr) {
        cout << "Belum ada menu\n";
        return;
    }
    cout << "Daftar menu:\n";
    inorder(root);
}

void menuPesan(Node*& root, linkedStack& riwayat) {
    string pelanggan, namaMenu;

    cout << "Nama pelanggan: ";
    getline(cin, pelanggan);
    cout << "Nama menu     : ";
    getline(cin, namaMenu);

    Node* target = cari(root, namaMenu);
    if (target == nullptr) {
        cout << "Menu tidak ditemukan!\n";
        return;
    }

    if (target->tersedia) {
        cout << "Pesanan " << pelanggan << " berhasil dibuat\n";
        target->tersedia = false;
        push(riwayat, "PESAN", namaMenu, pelanggan);
    } else {
        enAntrean(target->waitingList, pelanggan);
        cout << "Menu sedang habis, " << pelanggan << " masuk ke dalam waiting list\n";
        push(riwayat, "PESAN", namaMenu, pelanggan);
    }
}

void menuTersediaKembali(Node*& root, linkedStack& riwayat) {
    string namaMenu;
    cout << "Nama menu: ";
    getline(cin, namaMenu);

    Node* target = cari(root, namaMenu);
    if (target == nullptr) {
        cout << "Menu tidak ditemukan\n";
        return;
    }

    target->tersedia = true;
    cout << "Menu tersedia kembali\n";

    string pelanggan;
    if (deAntrean(target->waitingList, pelanggan)) {
        cout << "Pesanan langsung diberikan kepada " << pelanggan << " dari waiting list\n";
        target->tersedia = false;
    }

    push(riwayat, "STATUS", namaMenu, pelanggan);
}

void menuHapus(Node*& root) {
    string namaMenu;
    cout << "Nama menu: ";
    getline(cin, namaMenu);

    if (hapus(root, namaMenu)) {
        cout << "Menu berhasil dihapus\n";
    } else {
        cout << "Menu tidak ditemukan\n";
    }
}

void menuUndo(Node*& root, linkedStack& riwayat) {
    NodeStack aksi;
    if (!pop(riwayat, aksi)) {
        cout << "Tidak ada aksi untuk di-undo\n";
        return;
    }

    Node* target = cari(root, aksi.namaMenu);

    if (aksi.jenis == "PESAN") {
        if (target != nullptr) {
            if (!hapusAntrian(target->waitingList, aksi.pelanggan)) {
                target->tersedia = true;
            }
        }
        cout << "Undo: Pesanan menu " << aksi.namaMenu << " dibatalkan\n";
    } else {
        if (target != nullptr) {
            target->tersedia = false;
            if (aksi.pelanggan != "") {
                pushFrontAntrean(target->waitingList, aksi.pelanggan);
            }
        }
        cout << "Undo: Menu " << aksi.namaMenu << " kembali habis\n";
    }
}

void menuLihatAntrian(Node* root) {
    bool ada = false;
    printAntrianSemua(root, ada);
    if (!ada) {
        cout << "Antrian kosong\n";
    }
}

int main() {
    Node* root = nullptr;
    linkedStack riwayat;
    buatStack(riwayat);

    int pilihan = 0;
    while (pilihan != 8) {
        cout << "\n=== RESTORAN PAK HAMBALI ===\n"
             << "1. Tambah menu\n"
             << "2. Lihat daftar menu\n"
             << "3. Catat pesanan\n"
             << "4. Menu tersedia kembali\n"
             << "5. Hapus menu\n"
             << "6. Undo aksi terakhir\n"
             << "7. Lihat antrian pelanggan\n"
             << "8. Keluar\n"
             << "Pilihan: ";
        cin >> pilihan;
        cin.ignore(1000, '\n');

        switch (pilihan) {
            case 1: menuTambah(root); break;
            case 2: menuLihat(root); break;
            case 3: menuPesan(root, riwayat); break;
            case 4: menuTersediaKembali(root, riwayat); break;
            case 5: menuHapus(root); break;
            case 6: menuUndo(root, riwayat); break;
            case 7: menuLihatAntrian(root); break;
            case 8: cout << "Program selesai\n"; break;
            default: cout << "Pilihan tidak valid\n"; break;
        }
    }
    clearTree(root);
    clearstack(riwayat);
    return 0;
}
