#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <algorithm>
#include <filesystem>

using namespace std;

struct matkul
{
    string name;
    int sks;

    matkul(string Name, int Sks) : name(Name), sks(Sks) {}
};

class mahasiswa
{
private:
    string name;
    int maxSKS;
    int currentSKS = 0;
    vector<matkul> matkulPilihan;

public:
    mahasiswa(string Name, int sks) : name(Name), maxSKS(sks) {}

    string getname() const { return name; }
    int getmaxSKS() const { return maxSKS; }
    int getcurrentSKS() const { return currentSKS; }
    int getsisaSKS() const { return maxSKS - currentSKS; }

    void addmatkul(const matkul &Matkul)
    {
        if (currentSKS + Matkul.sks <= maxSKS)
        {
            matkulPilihan.push_back(Matkul);
            currentSKS += Matkul.sks;
        }
        else
        {
            cout << "Tidak bisa menambah mata kuliah, melebihi batas SKS.\n";
        }
    }

    void removematkul(int index)
    {
        if (index >= 0 && index < matkulPilihan.size())
        {
            currentSKS -= matkulPilihan[index].sks;
            matkulPilihan.erase(matkulPilihan.begin() + index);
        }
        else
        {
            cout << "Indeks tidak valid.\n";
        }
    }

    void saveFRS(const string &filename)
    {
        ofstream outFile(filename);
        if (outFile.is_open())
        {
            outFile << "Nama         : " << name << endl;
            outFile << "Batas / Sisa : " << maxSKS << " / " << getsisaSKS() << " SKS" << endl;
            outFile << string(40, '-') << endl;
            outFile << left << setw(5) << "No" << "|" << left << setw(30) << "Nama Mata Kuliah" << "| SKS" << endl;
            outFile << string(40, '-') << endl;

            for (size_t i = 0; i < matkulPilihan.size(); ++i)
            {
                outFile << left << setw(5) << (i + 1) << "|"
                        << left << setw(30) << matkulPilihan[i].name << "| "
                        << matkulPilihan[i].sks << endl;
            }
            outFile.close();
            cout << "FRS berhasil disimpan ke " << filename << "!\n";
        }
        else
        {
            cerr << "Gagal membuka file untuk menyimpan FRS!" << endl;
        }
    }

    bool loadFRS(const string &filename)
    {
        ifstream inFile(filename);
        if (!inFile.is_open())
        {
            cerr << "Gagal membuka file " << filename << "!\n";
            return false;
        }

        string line;
        getline(inFile, line);  // Nama
        name = line.substr(14); // Mengambil nama mahasiswa
        getline(inFile, line);  // Batas / Sisa

        try
        {
            size_t pos = line.find('/');
            if (pos != string::npos)
            {
                size_t start = line.find(':', pos) + 2;
                maxSKS = stoi(line.substr(start, pos - start));
            }
            else
            {
                throw invalid_argument("Format tidak valid");
            }
        }
        catch (const invalid_argument &e)
        {
            cerr << "Format SKS tidak valid: " << line << endl;
            return false;
        }

        currentSKS = 0;
        matkulPilihan.clear();
        for (int i = 0; i < 2; i++)
            getline(inFile, line); // Skip header

        while (getline(inFile, line))
        {
            if (line.empty() || line[0] == '-')
                continue; // Lewati garis pemisah
            size_t firstPipe = line.find('|');
            size_t secondPipe = line.find('|', firstPipe + 1);

            if (firstPipe != string::npos && secondPipe != string::npos)
            {
                string name = line.substr(firstPipe + 1, secondPipe - firstPipe - 1);
                string sksString = line.substr(secondPipe + 1);
                try
                {
                    int sks = stoi(sksString);
                    addmatkul(matkul(name, sks)); // Tambahkan mata kuliah
                }
                catch (const invalid_argument &e)
                {
                    cerr << "Format SKS tidak valid pada mata kuliah: " << line << endl;
                }
            }
        }
        inFile.close();
        return true;
    }

    vector<matkul> getMatkulPilihan() const { return matkulPilihan; }
};

vector<matkul> loadDaftarMatkul(const string &filename)
{
    vector<matkul> daftarMatkul;
    ifstream inFile(filename);
    string line;

    while (getline(inFile, line))
    {
        size_t pos = line.find(',');
        if (pos != string::npos)
        {
            string name = line.substr(0, pos);
            int sks = stoi(line.substr(pos + 1));
            daftarMatkul.push_back(matkul(name, sks));
        }
    }
    return daftarMatkul;
}

vector<string> listFRSFiles(const string &directory)
{
    vector<string> files;
    for (const auto &entry : filesystem::directory_iterator(directory))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".txt")
        {
            files.push_back(entry.path().filename().string());
        }
    }
    return files;
}

void saveDaftarMatkul(const vector<matkul> &daftarMatkul, const string &filename)
{
    ofstream outFile(filename);
    for (const auto &mk : daftarMatkul)
    {
        outFile << mk.name << "," << mk.sks << endl;
    }
}

void displayMatkul(const vector<matkul> &daftarMatkul)
{
    cout << left << setw(5) << "No" << "|" << left << setw(30) << "Nama Mata Kuliah" << "| SKS\n";
    cout << string(40, '-') << endl;
    for (size_t i = 0; i < daftarMatkul.size(); ++i)
    {
        cout << left << setw(5) << (i + 1) << "|"
             << left << setw(30) << daftarMatkul[i].name << "| "
             << daftarMatkul[i].sks << endl;
    }
}

int main()
{
    vector<matkul> daftarMatkul = loadDaftarMatkul("daftar_matkul.txt");
    int pilihan = 0;
    string TempName;
    int TempSKS;
    mahasiswa mahasigma("", 0);

    while (true)
    {
        cout << endl
             << string(20, '=') << " MENU " << string(20, '=') << endl;
        cout << "1. Buat FRS Baru\n2. Edit FRS\n3. Edit Daftar Mata Kuliah\n4. Keluar\n";
        cout << "Pilih opsi: ";
        cin >> pilihan;
        cin.ignore();

        switch (pilihan)
        {
        case 1:
        {
            cout << "Masukkan nama mahasiswa: ";
            getline(cin, TempName);
            cout << "Masukkan maksimum SKS mahasiswa: ";
            cin >> TempSKS;
            cin.ignore();
            cout << endl;

            mahasigma = mahasiswa(TempName, TempSKS);
            while (true)
            {
                cout << "Mata kuliah yang tersedia:\n";
                displayMatkul(daftarMatkul);

                cout << "Pilih indeks mata kuliah yang ingin ditambahkan (1-"
                     << daftarMatkul.size() << "): ";
                int index;
                cin >> index;
                cin.ignore();
                cout << endl;

                if (index > 0 && index <= daftarMatkul.size())
                {
                    mahasigma.addmatkul(daftarMatkul[index - 1]);
                    if (mahasigma.getsisaSKS() > 0)
                    {
                        cout << "Masih ada " << mahasigma.getsisaSKS() << " SKS yang tersisa, ambil mata kuliah lagi? (y/n): ";
                        char response;
                        cin >> response;
                        cin.ignore();
                        cout << endl;
                        if (response != 'y')
                            break;
                    }
                    else
                    {
                        cout << "Seluruh SKS telah diambil.\n";
                        break;
                    }
                }
                else
                {
                    cout << "Indeks tidak valid. Silakan coba lagi.\n";
                }
            }
            mahasigma.saveFRS("frs_" + mahasigma.getname() + ".txt");
            cout << "FRS berhasil dibuat dan disimpan!\n\n";
            break;
        }
        case 2:
        {
            auto files = listFRSFiles(".");
            cout << "Daftar FRS yang tersedia:\n";
            for (size_t i = 0; i < files.size(); ++i)
            {
                cout << left << setw(5) << (i + 1) << "| " << files[i] << endl;
            }
            cout << "Pilih file FRS untuk diedit (1-" << files.size() << "): ";
            int fileIndex;
            cin >> fileIndex;
            cin.ignore();
            cout << endl;

            if (fileIndex > 0 && fileIndex <= files.size())
            {
                string selectedFile = files[fileIndex - 1];
                if (mahasigma.loadFRS(selectedFile))
                {
                    while (true)
                    {
                        cout << "Mata kuliah yang terdaftar:\n";
                        displayMatkul(mahasigma.getMatkulPilihan());

                        cout << "\n1. Tambah Mata Kuliah\n2. Hapus Mata Kuliah\n3. Selesai\nPilih opsi: ";
                        int editOption;
                        cin >> editOption;
                        cin.ignore();

                        if (editOption == 1)
                        {
                            cout << "\nMata kuliah yang tersedia:\n";
                            displayMatkul(daftarMatkul);

                            cout << "\nPilih indeks mata kuliah yang ingin ditambahkan: ";
                            int index;
                            cin >> index;
                            cin.ignore();
                            cout << endl;

                            if (index > 0 && index <= daftarMatkul.size())
                            {
                                mahasigma.addmatkul(daftarMatkul[index - 1]);
                            }
                            else
                            {
                                cout << "Indeks tidak valid. Silakan coba lagi.\n";
                            }
                        }
                        else if (editOption == 2)
                        {
                            cout << "\nPilih mata kuliah yang ingin dihapus: ";
                            int removeIndex;
                            cin >> removeIndex;
                            cin.ignore();
                            cout << endl;
                            mahasigma.removematkul(removeIndex - 1);
                        }
                        else if (editOption == 3)
                        {
                            break;
                        }
                        else
                        {
                            cout << "Opsi tidak valid.\n";
                        }
                    }
                    mahasigma.saveFRS(selectedFile);
                }
            }
            else
            {
                cout << "Indeks tidak valid.\n";
            }
            break;
        }
        case 3:
        {
            while (true)
            {
                cout << "\nDaftar Mata Kuliah:\n\n";
                displayMatkul(daftarMatkul);

                cout << "\n1. Tambah Mata Kuliah\n2. Hapus Mata Kuliah\n3. Selesai\nPilih opsi: ";
                int option;
                cin >> option;
                cin.ignore();

                if (option == 1)
                {
                    cout << "\nMasukkan nama mata kuliah: ";
                    getline(cin, TempName);
                    cout << "Masukkan SKS mata kuliah: ";
                    cin >> TempSKS;
                    cin.ignore();
                    cout << endl;
                    daftarMatkul.push_back(matkul(TempName, TempSKS));
                    saveDaftarMatkul(daftarMatkul, "daftar_matkul.txt");
                    cout << "Mata kuliah berhasil ditambahkan!\n";
                }
                else if (option == 2)
                {
                    cout << "\nPilih mata kuliah yang ingin dihapus: ";
                    int removeIndex;
                    cin >> removeIndex;
                    cin.ignore();
                    cout << endl;
                    if (removeIndex > 0 && removeIndex <= daftarMatkul.size())
                    {
                        daftarMatkul.erase(daftarMatkul.begin() + (removeIndex - 1));
                        saveDaftarMatkul(daftarMatkul, "daftar_matkul.txt");
                        cout << "Mata kuliah berhasil dihapus!\n";
                    }
                    else
                    {
                        cout << "Indeks tidak valid. Silakan coba lagi.\n";
                    }
                }
                else if (option == 3)
                {
                    break; // Selesai
                }
                else
                {
                    cout << "Opsi tidak valid. Silakan coba lagi.\n";
                }
            }
            cout << "Daftar mata kuliah berhasil diperbarui!\n";
            break;
        }
        case 4:
            return 0;
        default:
            cout << "Opsi tidak valid.\n";
            break;
        }
    }
}
