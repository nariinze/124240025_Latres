#include <iostream>
#include <string>
using namespace std;

struct Video {
    string judul;
    int durasi;
    string status; // "tersedia", "dalam antrean", "sedang diputar"
};

struct Node {
    Video data;
    Node* left;
    Node* right;
};

// Playlist Queue
struct PlaylistNode {
    Video* video;
    PlaylistNode* next;
};

// Stack untuk Riwayat dan Undo
struct StackNode {
    string aksi;      // "tambah", "hapus", "playlist", "tonton"
    Video dataVideo;
    StackNode* next;
};

Node* root = nullptr;
PlaylistNode* playlistFront = nullptr;
PlaylistNode* playlistRear = nullptr;
StackNode* riwayatTop = nullptr;
StackNode* undoTop = nullptr;

// ===== BST Fungsi =====
Node* insertBST(Node* root, Video vidio) {
    if (!root) {
        Node* baru = new Node{vidio, nullptr, nullptr};
        return baru;
    }
    if (vidio.judul < root->data.judul)
        root->left = insertBST(root->left, vidio);
    else if (vidio.judul > root->data.judul)
        root->right = insertBST(root->right, vidio);
    else
        cout << "Judul sudah ada, gagal tambah.\n";
    return root;
}

Node* findMin(Node* root) {
    while (root && root->left) root = root->left;
    return root;
}

Node* deleteBST(Node* root, string judul) {
    if (!root) return nullptr;
    if (judul < root->data.judul)
        root->left = deleteBST(root->left, judul);
    else if (judul > root->data.judul)
        root->right = deleteBST(root->right, judul);
    else {
        if (!root->left) {
            Node* temp = root->right;
            delete root;
            return temp;
        } else if (!root->right) {
            Node* temp = root->left;
            delete root;
            return temp;
        } else {
            Node* temp = findMin(root->right);
            root->data = temp->data;
            root->right = deleteBST(root->right, temp->data.judul);
        }
    }
    return root;
}

bool cariJudul(Node* root, string judul) {
    if (!root) return false;
    if (root->data.judul == judul) return true;
    if (judul < root->data.judul)
        return cariJudul(root->left, judul);
    return cariJudul(root->right, judul);
}

Video* cariVideo(Node* root, string judul) {
    if (!root) return nullptr;
    if (root->data.judul == judul) return &root->data;
    if (judul < root->data.judul) return cariVideo(root->left, judul);
    return cariVideo(root->right, judul);
}

void inorderTampil(Node* root) {
    if (root) {
        inorderTampil(root->left);
        cout << "Judul: " << root->data.judul << " | Durasi: " << root->data.durasi << " menit | Status: " << root->data.status << "\n";
        inorderTampil(root->right);
    }
}

// ===== Playlist Queue =====
void enqueue(Video* vidio) {
    PlaylistNode* baru = new PlaylistNode{vidio, nullptr};
    if (!playlistFront) playlistFront = playlistRear = baru;
    else {
        playlistRear->next = baru;
        playlistRear = baru;
    }
}

Video* dequeue() {
    if (!playlistFront) return nullptr;
    PlaylistNode* temp = playlistFront;
    Video* vidio = temp->video;
    playlistFront = playlistFront->next;
    if (!playlistFront) playlistRear = nullptr;
    delete temp;
    return vidio;
}

bool adaDiPlaylist(string judul) {
    PlaylistNode* temp = playlistFront;
    while (temp) {
        if (temp->video->judul == judul)
            return true;
        temp = temp->next;
    }
    return false;
}

void hapusDariPlaylist(string judul) {
    PlaylistNode* prev = nullptr;
    PlaylistNode* curr = playlistFront;
    while (curr) {
        if (curr->video->judul == judul) {
            if (prev) prev->next = curr->next;
            else playlistFront = curr->next;
            if (curr == playlistRear) playlistRear = prev;
            delete curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// ===== Stack Riwayat & Undo =====
void pushRiwayat(Video vidio) {
    StackNode* baru = new StackNode{"tonton", vidio, riwayatTop};
    riwayatTop = baru;
}

void popRiwayat() {
    if (!riwayatTop) return;
    StackNode* temp = riwayatTop;
    riwayatTop = riwayatTop->next;
    delete temp;
}

void pushUndo(string aksi, Video vidio) {
    StackNode* baru = new StackNode{aksi, vidio, undoTop};
    undoTop = baru;
}

StackNode* popUndo() {
    if (!undoTop) return nullptr;
    StackNode* temp = undoTop;
    undoTop = undoTop->next;
    return temp;
}

// ===== Fungsi Menu =====
void tambahVideo() {
    Video vidio;
    cout << "Masukkan judul: ";
    getline(cin, vidio.judul);

    if (cariJudul(root, vidio.judul)) {
        cout << "Judul video sudah ada.\n";
        return;
    }

    cout << "Durasi (menit): ";
    while (!(cin >> vidio.durasi)) {
        cout << "Input durasi harus angka! Coba lagi: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore(1000, '\n');  

    vidio.status = "tersedia";

    root = insertBST(root, vidio);
    pushUndo("tambah", vidio);
    cout << "Video berhasil ditambahkan.\n";
}

void tampilkanDaftar() {
    cout << "=== Daftar Video ===\n";
    inorderTampil(root);

    char pilih;
    cout << "Cari video? (y/t): ";
    cin >> pilih;
    cin.ignore(1000, '\n');

    if (pilih == 'y') {
        string key;
        cout << "Judul yang dicari: ";
        getline(cin, key);
        Video* vidio = cariVideo(root, key);
        if (vidio) cout << "Ditemukan: " << vidio->judul << " - " << vidio->durasi << " menit - " << vidio->status << "\n";
        else cout << "Video tidak ditemukan.\n";
    }
}

void tambahKePlaylist() {
    string judul;
    cout << "Judul video yang ingin ditambahkan ke playlist: ";
    getline(cin, judul);

    Video* vidio = cariVideo(root, judul);
    if (!vidio) {
        cout << "Video tidak ditemukan.\n";
        return;
    }

    if (vidio->status == "tersedia") {
        if (!playlistFront)
            vidio->status = "sedang diputar";
        else
            vidio->status = "dalam antrian";
        enqueue(vidio);
        pushUndo("playlist", *vidio);
        cout << "Berhasil ditambahkan ke playlist.\n";
    } else {
        cout << "Video sudah ada di playlist atau sedang diputar.\n";
    }
}

void tontonVideo() {
    if (!playlistFront) {
        cout << "Playlist kosong.\n";
        return;
    }

    Video* vidio = dequeue();
    cout << "Memutar: " << vidio->judul << "... selesai.\n";

    vidio->status = "tersedia";
    pushRiwayat(*vidio);
    pushUndo("tonton", *vidio);

    if (playlistFront)
        playlistFront->video->status = "sedang diputar";
}

void tampilRiwayat() {
    cout << "=== Riwayat Tontonan ===\n";
    if (!riwayatTop) {
        cout << "Riwayat kosong.\n";
        return;
    }
    StackNode* temp = riwayatTop;
    while (temp) {
        cout << temp->dataVideo.judul << endl;
        temp = temp->next;
    }
}

void hapusVideo() {
    string judul;
    cout << "Masukkan judul video yang ingin dihapus: ";
    getline(cin, judul);

    Video* vidio = cariVideo(root, judul);
    if (!vidio) {
        cout << "Video tidak ditemukan.\n";
        return;
    }

    bool diPlaylist = (vidio->status == "sedang diputar" || vidio->status == "dalam antrian");
    if (diPlaylist) {
        char yakin;
        cout << "Video yang ingin dihapus [" << vidio->status << "]. Yakin untuk tetap menghapus? (y/t): ";
        cin >> yakin;
        cin.ignore(1000, '\n');
        if (yakin != 'y' && yakin != 'Y') {
            cout << "Penghapusan dibatalkan.\n";
            return;
        }
    }
    if (diPlaylist) {
        hapusDariPlaylist(judul);
    }
    Video vidBackup = *vidio;

    root = deleteBST(root, judul);
    pushUndo("hapus", vidBackup);

    cout << "Video berhasil dihapus dari daftar dan playlist.\n";
}

void undo() {
    StackNode* aksiUndo = popUndo();
    if (!aksiUndo) {
        cout << "Tidak ada aksi yang bisa di-undo.\n";
        return;
    }

    string aksi = aksiUndo->aksi;
    Video vidio = aksiUndo->dataVideo;

    if (aksi == "tambah") {
        // Undo tambah video = hapus video yg baru ditambah
        root = deleteBST(root, vidio.judul);
        cout << "Undo tambah video: Video '" << vidio.judul << "' dihapus.\n";
    } else if (aksi == "hapus") {
        // Undo hapus video = tambah kembali video ke BST
        root = insertBST(root, vidio);
        cout << "Undo hapus video: Video '" << vidio.judul << "' dikembalikan.\n";
    } else if (aksi == "playlist") {
        // Undo tambah ke playlist = hapus video dari playlist, status jadi tersedia
        hapusDariPlaylist(vidio.judul);
        Video* vid = cariVideo(root, vidio.judul);
        if (vid) vid->status = "tersedia";
        cout << "Undo tambah ke playlist: Video '" << vidio.judul << "' status dikembalikan tersedia.\n";
    } else if (aksi == "tonton") {
        // Undo tonton video = keluarkan dari riwayat, masuk ke playlist paling depan, status sedang diputar
        if (riwayatTop && riwayatTop->dataVideo.judul == vidio.judul) {
            popRiwayat();
            // Masukkan kembali ke playlist di depan
            PlaylistNode* baru = new PlaylistNode{cariVideo(root, vidio.judul), playlistFront};
            playlistFront = baru;
            if (!playlistRear) playlistRear = baru;

            Video* vid = cariVideo(root, vidio.judul);
            if (vid) vid->status = "sedang diputar";
            cout << "Undo tonton video: Video '" << vidio.judul << "' dikembalikan ke playlist depan.\n";
        } else {
            cout << "Gagal undo tonton: data riwayat tidak sesuai.\n";
        }
    }
    delete aksiUndo;
}

// ===== MAIN =====
int main() {
    int pilihan;
    do {
        cout << "\n=== IDLIX Tube Menu ===\n";
        cout << "1. Tambah Video\n";
        cout << "2. Tampilkan Daftar Video\n";
        cout << "3. Tambah ke Playlist\n";
        cout << "4. Tonton Video\n";
        cout << "5. Riwayat Tontonan\n";
        cout << "6. Hapus Video\n";
        cout << "7. Undo Tindakan Terakhir\n";
        cout << "0. Keluar\n";
        cout << "Pilih menu: ";

        while (!(cin >> pilihan)) {
            cout << "Input harus angka! Coba lagi: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        cin.ignore(1000, '\n');

        switch (pilihan) {
            case 1: tambahVideo(); break;
            case 2: tampilkanDaftar(); break;
            case 3: tambahKePlaylist(); break;
            case 4: tontonVideo(); break;
            case 5: tampilRiwayat(); break;
            case 6: hapusVideo(); break;
            case 7: undo(); break;
            case 0: cout << "Terima kasih sudah menggunakan IDLIX Tube.\n"; break;
            default: cout << "Pilihan tidak valid.\n";
        }

    } while (pilihan != 0);

    return 0;
}
