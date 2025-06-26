#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Yazar {
    char* ad;
    char* soyad;
    int yazar_id;
    struct Yazar* next; 
} Yazar;

typedef struct Ogrenci {
    char* ad;
    char* soyad;
    char* ogrenci_no;
    int puan;
    struct Ogrenci* prev;
    struct Ogrenci* next;
} Ogrenci;

typedef struct Kitap_Ornek {
    char* durum;
    char* etiket_no;
    struct Kitap_Ornek* next;
} Kitap_Ornek;

typedef struct Kitap {
    char* kitap_adi;
    char* ISBN;
    int adet;
    Kitap_Ornek* ornekler;
    struct Kitap* next;
} Kitap;

typedef struct Kitap_Yazar {
    char* ISBN;
    int yazar_id;
} Kitap_Yazar;

typedef struct KitapIslem {
    char* ogrenciNo;
    char* etiketNo;
    int islemTuru;        
    char* tarih;
    struct KitapIslem* next;
} KitapIslem;

typedef struct {
    Yazar** yazar_head;
    Ogrenci** ogrenci_head;
    Kitap** kitap_head;
    KitapIslem** islem_head;
    Kitap_Yazar** kitapyazar_dizi; 
    int* kitapyazar_sayisi;      
} KutuphaneVeri;

typedef struct {
    int gun;
    int ay;
    int yil;
} TarihYapisi;

int string_to_tarih(const char* tarih_str, TarihYapisi* tarih_obj) {
    if (sscanf(tarih_str, "%d.%d.%d", &tarih_obj->gun, &tarih_obj->ay, &tarih_obj->yil) == 3) {
        if (tarih_obj->gun >= 1 && tarih_obj->gun <= 31 &&
            tarih_obj->ay >= 1 && tarih_obj->ay <= 12 &&
            tarih_obj->yil > 0 ) { 
            return 1; 
        }
    }
    return 0; 
}

int gun_farki_hesapla(TarihYapisi t1, TarihYapisi t2) {
    int gunler1 = t1.yil * 365 + t1.ay * 30 + t1.gun;
    int gunler2 = t2.yil * 365 + t2.ay * 30 + t2.gun;
    return gunler2 - gunler1;
}

void Yazar_Silindiginde_Guncelle(Kitap_Yazar* dizi, int boyut, int silinen_yazar_id);
void Kitap_Yazar_Dizisinden_ISBN_Sil(Kitap_Yazar** dizi, int* boyut, const char* ISBN_silinecek);

Yazar* Yazar_Olustur(char* ad, char* soyad, int yazar_id) {

    Yazar* yeni_yazar = (Yazar*)malloc(sizeof(Yazar));
    if (yeni_yazar == NULL) {
        printf("Hata");
        return NULL;
    }

    yeni_yazar -> ad = (char*)malloc((strlen(ad)+1) * sizeof(char));
    if (yeni_yazar -> ad == NULL) {
        printf("Hata");
        free(yeni_yazar);
        return NULL;
    }
    strcpy(yeni_yazar -> ad, ad);

    yeni_yazar -> soyad = (char*)calloc((strlen(soyad)+1), sizeof(char));
    if (yeni_yazar -> soyad == NULL) {
        printf("Hata");
        free(yeni_yazar->ad);
        free(yeni_yazar);
        return NULL;
    }
    strcpy(yeni_yazar -> soyad, soyad);

    yeni_yazar -> yazar_id = yazar_id;
    yeni_yazar -> next = NULL;
    
    return yeni_yazar;
  
}

void Dosyaya_Yazarlari_Yaz(Yazar* head, const char* Dosya_Adi);

void Yazar_Ekle(Yazar** head, char* ad, char* soyad) {

    int yeni_yazar_id;

    if(*head == NULL) {
        yeni_yazar_id = 1;
    } else {
        Yazar* temp = *head;
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        yeni_yazar_id = temp -> yazar_id + 1;
    }

    Yazar* yeni_yazar = Yazar_Olustur(ad, soyad, yeni_yazar_id);

    if(*head == NULL) {
        *head = yeni_yazar;
    } else {
        Yazar* temp = *head;
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        temp -> next = yeni_yazar;
    }
}

void Yazar_Silme(Yazar** head, int yazar_id,Kitap_Yazar* kitap_yazar_dizisi, int kitapyazar_sayisi) {
    Yazar* temp = *head;
    Yazar* prev = NULL;

    if (temp != NULL && temp->yazar_id == yazar_id) {
        *head = temp->next;
        free(temp->ad);
        free(temp->soyad);
        free(temp);
        Yazar_Silindiginde_Guncelle(kitap_yazar_dizisi, kitapyazar_sayisi, yazar_id);
        return;
    }
    while (temp != NULL && temp->yazar_id != yazar_id) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) {
        printf("Silinecek yazar ID (%d) bulunamadi.\n", yazar_id);
        return; 
    }
    prev->next = temp->next;
    free(temp->ad);
    free(temp->soyad);
    free(temp);
    
    Yazar_Silindiginde_Guncelle(kitap_yazar_dizisi, kitapyazar_sayisi, yazar_id);
    Dosyaya_Yazarlari_Yaz(*head, "yazarlar.csv");
}

void Yazar_Duzenleme(Yazar** head, char* yeni_ad, char* yeni_soyad, int yazar_id) {

    Yazar* temp = *head;

    while(temp != NULL) {
        if (temp -> yazar_id == yazar_id) {
            free(temp -> ad);
            temp -> ad = (char*)malloc((strlen(yeni_ad)+1) * sizeof(char));
            if (temp -> ad == NULL) {
                printf("Hata");
                return;
            }
            strcpy(temp->ad, yeni_ad);
            free(temp -> soyad);
            temp -> soyad = (char*)malloc((strlen(yeni_soyad)+1) * sizeof(char));
            if (temp -> soyad == NULL) {
                printf("Hata");
                return;
            }
            strcpy(temp->soyad, yeni_soyad);
            return;
        }
        temp = temp -> next;
    }
}

void Yazar_Listeleme(Yazar** head) {

    Yazar* temp = *head;
    if (temp == NULL) {
        printf("Liste bos");
        return;
    }
    
    printf("Yazar Listesi:\n");
    while(temp != NULL) {
        printf("Yazar ID: %d, Ad: %s, Soyad: %s\n", temp -> yazar_id, temp -> ad, temp -> soyad);
        temp = temp -> next;  
    }
}

void Yazar_Listeyi_Sil(Yazar** head) {
    Yazar* temp = *head;
    while (temp != NULL) {
        Yazar* sonraki = temp->next;
        free(temp->ad);
        free(temp->soyad);
        free(temp);
        temp = sonraki;
    }
    *head = NULL; 
}

FILE* DosyaAc(const char* Dosya_Adi, const char* Mod) {
    FILE* dosya = fopen(Dosya_Adi,Mod);
    if (dosya == NULL) {
        printf("Hata");
        return NULL;
    }
    return dosya;
}

void Dosyaya_Yazarlari_Yaz(Yazar* head, const char* Dosya_Adi) {
    FILE* dosya = fopen(Dosya_Adi,"w");
    if (dosya == NULL) {
        printf("Hata");
        return;
    }
    Yazar* temp = head;
    while (temp != NULL){
        fprintf(dosya, "%d,%s,%s\n", temp->yazar_id, temp->ad, temp->soyad);
        temp = temp->next;
    }
    fclose(dosya);
}

void Dosyadan_Yazar_Oku(Yazar** head, const char* Dosya_Adi) {
    FILE* dosya = fopen(Dosya_Adi, "r");
    if (dosya == NULL) {
        printf("Dosya acilamadi.\n");
        return;
    }

    char* satir = (char*)malloc(256);
    while (fgets(satir, 256, dosya)) {
        satir[strcspn(satir, "\r\n")] = 0;
        int id;
        char* ad = (char*)malloc(100);
        char* soyad = (char*)malloc(100);
        if (sscanf(satir, "%d,%99[^,],%99[^,]", &id, ad, soyad) == 3) {
            Yazar* yeni_yazar = Yazar_Olustur(ad, soyad, id);
            if (*head == NULL) {
                *head = yeni_yazar;
            } else {
                Yazar* temp = *head;
                while (temp->next != NULL)
                    temp = temp->next;
                temp->next = yeni_yazar;
            }
        }
    }

    fclose(dosya);
}

Ogrenci* Ogrenci_Olustur(char* ad, char* soyad, char* ogrenci_no, int puan) {

    if (strlen(ogrenci_no) != 8 ) {
        printf("Yeni ogrenci numarasi gecersiz.\n");
        printf("Ogrenci eklenemedi.\n");
        return NULL;
    }

    Ogrenci* yeni_ogrenci = (Ogrenci*)malloc(sizeof(Ogrenci));
    if (yeni_ogrenci == NULL) {
        printf("Hata");
        return NULL;
    }

    yeni_ogrenci -> ad = (char*)malloc((strlen(ad)+1)*sizeof(char));
    if (yeni_ogrenci -> ad == NULL) {
        printf("Hata");
        free(yeni_ogrenci);
        return NULL;
    }
    strcpy(yeni_ogrenci->ad, ad);

    yeni_ogrenci -> soyad = (char*)malloc((strlen(soyad)+1)*sizeof(char));
    if (yeni_ogrenci -> soyad == NULL) {
        printf("Hata");
        free(yeni_ogrenci->ad);
        free(yeni_ogrenci);
        return NULL;
    }
    strcpy(yeni_ogrenci-> soyad, soyad);

    yeni_ogrenci -> ogrenci_no = (char*)malloc(9*sizeof(char));
    if (yeni_ogrenci -> ogrenci_no == NULL) {
        printf("Hata");
        free(yeni_ogrenci->ad);
        free(yeni_ogrenci->soyad);
        free(yeni_ogrenci);
        return NULL;
    }
    strcpy(yeni_ogrenci-> ogrenci_no, ogrenci_no);

    yeni_ogrenci -> puan = puan;

    yeni_ogrenci -> prev = NULL;
    yeni_ogrenci -> next = NULL;

    return yeni_ogrenci;

}

void Ogrenci_Ekle(Ogrenci** head, char* ad, char* soyad, char* no, int puan) {

    Ogrenci* kontrol = *head;
    while (kontrol != NULL) {
        if (strcmp(kontrol->ogrenci_no, no) == 0) {
            printf("Hata: %s numarali ogrenci zaten mevcut!\n", no);
            return;
        }
        kontrol = kontrol->next;
    }

    Ogrenci* yeni_ogrenci = Ogrenci_Olustur(ad, soyad, no, puan);
    if (yeni_ogrenci == NULL) {
        return;
    }
    if (*head == NULL) {
        *head = yeni_ogrenci;
    } else {
        Ogrenci* temp = *head;
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        
        yeni_ogrenci->prev = temp;
        temp->next = yeni_ogrenci;
    }
    printf("Ogrenci eklendi.\n");
}

void Ogrenci_Silme(Ogrenci** head, char* no, Kitap** kitap_head, KitapIslem** islem_head) {

    Ogrenci* temp = *head;

    while(temp != NULL && strcmp(temp -> ogrenci_no, no) != 0) {
        temp = temp -> next;
    }
    if (temp == NULL) {
        printf("Ogrenci bulunamadi.\n");
        return;
    }
    if (temp->prev != NULL)
        temp->prev->next = temp->next;
    else
        *head = temp->next;

    if (temp->next != NULL)
        temp->next->prev = temp->prev;

    Kitap* mevcut_kitap = *kitap_head;
    while (mevcut_kitap != NULL) {
        Kitap_Ornek* ornek = mevcut_kitap->ornekler;
        while (ornek != NULL) {
            if (strcmp(ornek->durum, "RAFTA") != 0 && strcmp(ornek->durum, no) == 0) {
                free(ornek->durum);
                ornek->durum = strdup("RAFTA");
                if (ornek->durum == NULL) {
                   printf("Bellek hatasi");
                }
            }
            ornek = ornek->next;
        }
        mevcut_kitap = mevcut_kitap->next;
    }
    free(temp->ad);
    free(temp->soyad);
    free(temp->ogrenci_no);
    free(temp);

}

void Ogrenci_Duzenleme(Ogrenci** head, char* eski_no, char* yeni_ad, char* yeni_soyad, char* yeni_no,Kitap** kitap_head, KitapIslem** islem_head) {

    Ogrenci* temp = *head;
    while (temp != NULL && strcmp(temp->ogrenci_no, eski_no) != 0) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Ogrenci bulunamadi: %s\n", eski_no); 
        return;
    }
    if (strlen(yeni_no) != 8) {
        printf("Yeni ogrenci numarasi gecersiz. 8 haneli olmalidir.\n");
        return;
    }
    if (strcmp(eski_no, yeni_no) != 0) {
        Ogrenci* kontrol = *head;
        while (kontrol != NULL) {
            if (kontrol != temp && strcmp(kontrol->ogrenci_no, yeni_no) == 0) {
                printf("Hata: Yeni ogrenci numarasi (%s) zaten baska bir ogrenciye ait.\n", yeni_no);
                return; 
            }
            kontrol = kontrol->next;
        }
    }
    if (strlen(yeni_ad) > 0) {
        free(temp->ad); 
        temp->ad = (char*)malloc(strlen(yeni_ad) + 1);
        if (temp->ad == NULL) {
            printf("Bellek tahsisi hatasi (yeni ad)");
            return; 
        }
        strcpy(temp->ad, yeni_ad);
    }
    if (strlen(yeni_soyad) > 0) {
        free(temp->soyad); 
        temp->soyad = (char*)malloc(strlen(yeni_soyad) + 1);
        if (temp->soyad == NULL) {
            printf("Bellek tahsisi hatasi (yeni soyad)");
            return;
        }
        strcpy(temp->soyad, yeni_soyad);
    }

    if (strcmp(eski_no,yeni_no) != 0) {
        Kitap* mevcut_kitap = *kitap_head;
        while (mevcut_kitap != NULL) {
          Kitap_Ornek* ornek = mevcut_kitap -> ornekler;
          while (ornek != NULL) {
            if (strcmp(ornek -> durum, "RAFTA") != 0 && strcmp(ornek -> durum, eski_no) == 0) {
                free(ornek->durum);
                ornek -> durum = strdup(yeni_no);
                if (ornek -> durum == NULL){
                    printf("Hata");
                }           
            }
            ornek = ornek -> next;
        }
        mevcut_kitap = mevcut_kitap -> next;
    }
    KitapIslem* yeni_islem = *islem_head;
    while (yeni_islem != NULL) {
        if (strcmp(yeni_islem->ogrenciNo,eski_no) == 0) {
            free(yeni_islem -> ogrenciNo);
            yeni_islem -> ogrenciNo = strdup(yeni_no);
            if (yeni_islem -> ogrenciNo == NULL){
                printf("Hata");
            }
        }
        yeni_islem = yeni_islem -> next;
    }
    free(temp -> ogrenci_no);
    temp -> ogrenci_no = strdup(yeni_no);
    if (temp-> ogrenci_no == NULL) {
        printf("Hata");
    }
}
    printf("Ogrenci basariyla duzenlendi.\n");
}


void Ogrenci_Listeleme(Ogrenci** head) {
    
    Ogrenci* temp = *head;
    if (temp == NULL) {
        printf("Liste bos.\n");
        return;
    }

    printf("Ogrenci Listesi:\n");
    while(temp != NULL) {
        printf("No: %s, Ad: %s, Soyad: %s, Puan: %d\n", temp->ogrenci_no, temp->ad, temp->soyad, temp->puan);
        temp = temp->next;
    }
}

void Ogrenci_Listeyi_Sil(Ogrenci** head) {
    Ogrenci* temp = *head;
    while (temp != NULL) {
        Ogrenci* sonraki = temp->next;
        free(temp->ad);
        free(temp->soyad);
        free(temp->ogrenci_no);
        free(temp);
        temp = sonraki;
    }
    *head = NULL;
}

void Dosyaya_Ogrencileri_Yaz(Ogrenci* head, const char* Dosya_Adi) {
    FILE* dosya = fopen(Dosya_Adi, "w");
    if (dosya == NULL) {
        printf("Dosya acilamadi.\n");
        return;
    }
    Ogrenci* temp = head;
    while (temp != NULL) {
        fprintf(dosya, "%s,%s,%s,%d\n", temp->ogrenci_no, temp->ad, temp->soyad, temp->puan);
        temp = temp->next;
    }
    fclose(dosya);
}

void Dosyadan_Ogrenci_Oku(Ogrenci** head, const char* Dosya_Adi) {
    FILE* dosya = fopen(Dosya_Adi, "r");
    if (dosya == NULL) {
        printf("Dosya acilamadi.\n");
        return;
    }

    char* satir = (char*)malloc(256);
    while (fgets(satir, 256, dosya)) {
        char* ogrenci_no = (char*)malloc(9);
        char* ad = (char*)malloc(100);
        char* soyad = (char*)malloc(100);
        int puan;

        if (sscanf(satir, "%8[^,],%99[^,],%99[^,],%d", ogrenci_no, ad, soyad, &puan) == 4) {
            Ogrenci* yeni_ogrenci = Ogrenci_Olustur(ad, soyad, ogrenci_no, puan);
            if (*head == NULL) {
                *head = yeni_ogrenci;
            } else {
                Ogrenci* temp = *head;
                while (temp->next != NULL)
                    temp = temp->next;
                temp->next = yeni_ogrenci;
                yeni_ogrenci->prev = temp;
            }
        }
    }
    fclose(dosya);
}

Kitap_Ornek* Ornek_Listesi_Olustur(char* ISBN, int adet) {
    Kitap_Ornek* head = NULL;
    Kitap_Ornek* tail = NULL;

    int i;

    for (i = 1; i <= adet; i++) {
        Kitap_Ornek* yeni_ornek = (Kitap_Ornek*)malloc(sizeof(Kitap_Ornek));
        if (!yeni_ornek) {
            printf("Bellek ayirma hatasi!\n");
            return NULL;
        }
        yeni_ornek->durum = (char*)malloc(strlen("RAFTA") + 1);
        if (yeni_ornek -> durum == NULL) {
            printf("Hata");
            free(yeni_ornek);
        }
        strcpy(yeni_ornek->durum, "RAFTA");

        int etiketsize = strlen(ISBN) + 1 + 10 + 1; 
        yeni_ornek->etiket_no = (char*)malloc(etiketsize);
        if (yeni_ornek -> etiket_no == NULL) {
            printf("Hata");
            free(yeni_ornek -> durum);
            free(yeni_ornek);
        }
        snprintf(yeni_ornek->etiket_no, etiketsize, "%s_%d", ISBN, i);

        yeni_ornek->next = NULL;

        if (head == NULL) {
            head = yeni_ornek;
            tail = yeni_ornek;
        } else {
            tail->next = yeni_ornek;
            tail = yeni_ornek;
        }
    }

    return head;
}

Kitap* Kitap_Olustur(char* ad, char* ISBN, int adet) {

    if (strlen(ISBN) != 13) {
        printf("Hata: ISBN numarasi 13 haneli olmalidir.\n");
        return NULL;
    }

    if (adet <= 0) {
        printf("Hata: Adet pozitif olmalidir.\n");
        return NULL;
    }
    
    Kitap* yeni_kitap = (Kitap*)malloc(sizeof(Kitap));
    if (yeni_kitap==NULL) {
        printf("Hata");
        return NULL;
    }

    yeni_kitap -> kitap_adi = (char*)malloc((strlen(ad)+1)*sizeof(char));
    if (yeni_kitap -> kitap_adi == NULL) {
        printf("Hata");
        free(yeni_kitap);
        return NULL;
    }
    strcpy(yeni_kitap -> kitap_adi, ad);

    yeni_kitap -> ISBN = (char*)malloc((strlen(ISBN)+1)*sizeof(char));
    if (yeni_kitap -> ISBN == NULL) {
        printf("Hata");
        free(yeni_kitap->kitap_adi);
        free(yeni_kitap);
        return NULL;
    }
    strcpy(yeni_kitap -> ISBN, ISBN);

    yeni_kitap -> adet = adet;
    yeni_kitap->ornekler = Ornek_Listesi_Olustur(ISBN, adet);
    yeni_kitap -> next = NULL;

    return yeni_kitap;
}

void Kitap_Ekle(Kitap** head, char* ad, char* ISBN, int adet) {

    Kitap* kontrol = *head;
    while(kontrol != NULL) {
        if (strcmp(kontrol->ISBN, ISBN) == 0) {
            printf("Hata: %s ISBN numarali kitap zaten mevcut.\n", ISBN);
            return;
        }
        kontrol = kontrol->next;
    }

    Kitap* yeni_kitap = Kitap_Olustur(ad, ISBN, adet);
    if (*head == NULL) {
        *head = yeni_kitap;
    } else {
        Kitap* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = yeni_kitap;
    }
}

void Kitap_Ornekleri_Temizle(Kitap_Ornek* head) {
    while (head != NULL) {
        Kitap_Ornek* temp = head;
        head = head->next;

        free(temp->durum);
        free(temp->etiket_no);
        free(temp);
    }
}

void Kitap_Sil(Kitap** head, char* ISBN, Kitap_Yazar** kitapyazar_dizi, int* kitapyazar_sayisi) {
    if (*head == NULL) {
        printf("Kutuphane bos.\n");
        return;
    }
    Kitap* temp = *head;
    Kitap* prev = NULL;

    while (temp != NULL && strcmp(temp->ISBN, ISBN) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Kitap bulunamadi: %s\n", ISBN);
        return;
    }

    if (prev == NULL) {
        *head = temp->next;
    } else {
        prev->next = temp->next;
    }
    Kitap_Ornekleri_Temizle(temp->ornekler);
    free(temp->kitap_adi);
    free(temp->ISBN);
    free(temp);
    Kitap_Yazar_Dizisinden_ISBN_Sil(kitapyazar_dizi, kitapyazar_sayisi, ISBN);
    printf("Kitap silindi: %s\n", ISBN);
}
void Islem_Listesinden_Belirli_Etiketi_Sil(KitapIslem** head, const char* etiket_no_to_delete) {
    KitapIslem* temp = *head;
    KitapIslem* prev = NULL;

    while (temp != NULL) {
        if (strcmp(temp->etiketNo, etiket_no_to_delete) == 0) {
            KitapIslem* silinecek = temp;
            printf("Uyari: Adet azaltma nedeniyle silinen '%s' etiket nolu ornege ait islem kaydi siliniyor.\n", etiket_no_to_delete);

            if (prev == NULL) { 
                *head = temp->next;
                temp = *head;
            } else {
                prev->next = temp->next;
                temp = temp->next;
            }

            free(silinecek->etiketNo);
            free(silinecek->ogrenciNo);
            free(silinecek->tarih);
            free(silinecek);
        } else {
            prev = temp;
            temp = temp->next;
        }
    }
}

void Kitap_Duzenleme(Kitap** head,char* ISBN_eski,char* yeni_kitap_adi,int yeni_kitap_adeti,char* yeni_ISBN,KitapIslem** islem_listesi_head,Kitap_Yazar** kitapyazar_dizisi_ptr,int* kitapyazar_sayisi_ptr) {

    Kitap *duzenlenecek_kitap = *head;
    int kitap_bulundu_flag = 0;
    while (duzenlenecek_kitap != NULL && !kitap_bulundu_flag) {
        if (strcmp(duzenlenecek_kitap->ISBN, ISBN_eski) == 0) {
            kitap_bulundu_flag = 1;
        }
        if (!kitap_bulundu_flag) { 
            duzenlenecek_kitap = duzenlenecek_kitap->next;
        }
    }

    if (!kitap_bulundu_flag) {
        printf("HATA: Duzenlenecek kitap (ISBN: %s) bulunamadi.\n", ISBN_eski);
        return;
    }

    if (strlen(yeni_ISBN) != 13) {
        printf("HATA: Yeni ISBN numarasi 13 haneli olmalidir.\n");
        return;
    }
    if (yeni_kitap_adeti <= 0) {
        printf("HATA: Yeni adet pozitif bir sayi olmalidir.\n");
        return;
    }

    int isbn_degisiyor_mu = strcmp(ISBN_eski, yeni_ISBN) != 0;
    int yeni_isbn_kullanilabilir = 1; 
    if (isbn_degisiyor_mu) {
        Kitap* kontrol_kitap = *head;
        while (kontrol_kitap != NULL && yeni_isbn_kullanilabilir) {
            if (kontrol_kitap != duzenlenecek_kitap && strcmp(kontrol_kitap->ISBN, yeni_ISBN) == 0) {
                printf("Hata.\n", yeni_ISBN);
                yeni_isbn_kullanilabilir = 0; 
            }
            if (yeni_isbn_kullanilabilir) { 
                kontrol_kitap = kontrol_kitap->next;
            }
        }
        if (!yeni_isbn_kullanilabilir) {
            return; 
        }
    }

    char* onceki_kitap_adi = duzenlenecek_kitap->kitap_adi;
    duzenlenecek_kitap->kitap_adi = strdup(yeni_kitap_adi);
    if (duzenlenecek_kitap->kitap_adi == NULL) {
        printf("Hata.\n");
        duzenlenecek_kitap->kitap_adi = onceki_kitap_adi;
        return;
    }
    if (onceki_kitap_adi != NULL && onceki_kitap_adi != duzenlenecek_kitap->kitap_adi) {
         free(onceki_kitap_adi);
    }
    if (isbn_degisiyor_mu) {
        KitapIslem* mevcut_islem = *islem_listesi_head;
        while (mevcut_islem != NULL) {
            int islem_guncellendi_bu_iterasyonda = 0; 
            if (mevcut_islem->etiketNo != NULL &&
                strncmp(mevcut_islem->etiketNo, ISBN_eski, strlen(ISBN_eski)) == 0 &&
                mevcut_islem->etiketNo[strlen(ISBN_eski)] == '_') {
                char* numara_kismi = strrchr(mevcut_islem->etiketNo, '_');
                if (numara_kismi != NULL) {
                    int yeni_etiket_uzunlugu = strlen(yeni_ISBN) + strlen(numara_kismi) + 1;
                    char* yeni_etiket_no = (char*)malloc(yeni_etiket_uzunlugu);
                    if (yeni_etiket_no == NULL) {
                        printf("Hata.\n");
                    } else {
                        strcpy(yeni_etiket_no, yeni_ISBN);
                        strcat(yeni_etiket_no, numara_kismi);
                        free(mevcut_islem->etiketNo);
                        mevcut_islem->etiketNo = yeni_etiket_no;
                        islem_guncellendi_bu_iterasyonda = 1;
                    }
                }
            }
            mevcut_islem = mevcut_islem->next;
        }
        Kitap_Ornek* mevcut_ornek = duzenlenecek_kitap->ornekler;
        int ornek_sayac = 1;
        while (mevcut_ornek != NULL) {
            char* onceki_ornek_etiketi = mevcut_ornek->etiket_no;
            int yeni_ornek_etiket_boyutu = strlen(yeni_ISBN) + 1 + 10 + 1;
            mevcut_ornek->etiket_no = (char*)malloc(yeni_ornek_etiket_boyutu);
            if (mevcut_ornek->etiket_no == NULL) {
                printf("Hata.\n");
                mevcut_ornek->etiket_no = onceki_ornek_etiketi;
            } else {
                snprintf(mevcut_ornek->etiket_no, yeni_ornek_etiket_boyutu, "%s_%d", yeni_ISBN, ornek_sayac++);
                if (onceki_ornek_etiketi != NULL && onceki_ornek_etiketi != mevcut_ornek->etiket_no) {
                    free(onceki_ornek_etiketi);
                }
            }
            mevcut_ornek = mevcut_ornek->next;
        }

        Kitap_Yazar* mevcut_eslesme_dizisi = *kitapyazar_dizisi_ptr;
        int mevcut_eslesme_sayisi = *kitapyazar_sayisi_ptr;
        int i;
        for (i = 0; i < mevcut_eslesme_sayisi; i++) {
            if (mevcut_eslesme_dizisi[i].ISBN != NULL && strcmp(mevcut_eslesme_dizisi[i].ISBN, ISBN_eski) == 0) {
                char* onceki_eslesme_isbn = mevcut_eslesme_dizisi[i].ISBN;
                mevcut_eslesme_dizisi[i].ISBN = strdup(yeni_ISBN);
                if (mevcut_eslesme_dizisi[i].ISBN == NULL) {
                    printf("Hata.\n");
                    mevcut_eslesme_dizisi[i].ISBN = onceki_eslesme_isbn;
                } else {
                     if(onceki_eslesme_isbn != NULL && onceki_eslesme_isbn != mevcut_eslesme_dizisi[i].ISBN){
                        free(onceki_eslesme_isbn);
                     }
                }
            }
        }

        char* onceki_ana_isbn = duzenlenecek_kitap->ISBN;
        duzenlenecek_kitap->ISBN = strdup(yeni_ISBN);
        if (duzenlenecek_kitap->ISBN == NULL) {
            printf("Hata.\n");
            duzenlenecek_kitap->ISBN = onceki_ana_isbn;
            return;
        }
        if(onceki_ana_isbn != NULL && onceki_ana_isbn != duzenlenecek_kitap->ISBN){
             free(onceki_ana_isbn);
        }
    }

    if (yeni_kitap_adeti > duzenlenecek_kitap->adet) {
        Kitap_Ornek *son_ornek = duzenlenecek_kitap->ornekler;
        if (son_ornek != NULL) {
            while (son_ornek->next != NULL) {
                son_ornek = son_ornek->next;
            }
        }
        int i = duzenlenecek_kitap->adet + 1;
        int adet_artirma_devam = (i <= yeni_kitap_adeti); 
        while(adet_artirma_devam){
            Kitap_Ornek *yeni_eklenecek_ornek = (Kitap_Ornek*)malloc(sizeof(Kitap_Ornek));
            int ornek_ekleme_basarili = 0; 

            if (yeni_eklenecek_ornek != NULL) {
                yeni_eklenecek_ornek->durum = strdup("RAFTA");
                if (yeni_eklenecek_ornek->durum != NULL) {
                    int etiket_boyutu = strlen(duzenlenecek_kitap->ISBN) + 1 + 10 + 1;
                    yeni_eklenecek_ornek->etiket_no = (char*)malloc(etiket_boyutu);
                    if (yeni_eklenecek_ornek->etiket_no != NULL) {
                        snprintf(yeni_eklenecek_ornek->etiket_no, etiket_boyutu, "%s_%d", duzenlenecek_kitap->ISBN, i);
                        yeni_eklenecek_ornek->next = NULL;
                        if (son_ornek == NULL) {
                            duzenlenecek_kitap->ornekler = yeni_eklenecek_ornek;
                        } else {
                            son_ornek->next = yeni_eklenecek_ornek;
                        }
                        son_ornek = yeni_eklenecek_ornek;
                        ornek_ekleme_basarili = 1;
                    } else {
                        printf("Hata.\n");
                        free(yeni_eklenecek_ornek->durum);
                        free(yeni_eklenecek_ornek);
                    }
                } else {
                    printf("Hata.\n");
                    free(yeni_eklenecek_ornek);
                }
            } else {
                printf("Hata.\n");
            }

            if (!ornek_ekleme_basarili) {
                duzenlenecek_kitap->adet = i - 1;
                adet_artirma_devam = 0; 
            } else {
                i++;
                if (i > yeni_kitap_adeti) {
                    adet_artirma_devam = 0; 
                }
            }
        }
    } else if (yeni_kitap_adeti < duzenlenecek_kitap->adet) {
        Kitap_Ornek* aktif_ornek = duzenlenecek_kitap->ornekler;
        Kitap_Ornek* onceki_ornek = NULL;
        int sayac = 0;
        int adet_azaltma_bitti = 0; 

        while (aktif_ornek != NULL && !adet_azaltma_bitti) {
            sayac++;
            if (sayac > yeni_kitap_adeti) {
                Kitap_Ornek* silinecek_ornek_basi = aktif_ornek;
                if (onceki_ornek == NULL) {
                    duzenlenecek_kitap->ornekler = NULL;
                } else {
                    onceki_ornek->next = NULL;
                }
                adet_azaltma_bitti = 1; 

                while (silinecek_ornek_basi != NULL) {
                    Kitap_Ornek* sonraki_silinecek = silinecek_ornek_basi->next;
                    if (strcmp(silinecek_ornek_basi->durum, "RAFTA") != 0) {
                        printf("Adet azaltma nedeniyle odunc verilmis olan '%s' etiketli kitap ornegi siliniyor!\n", silinecek_ornek_basi->etiket_no);
                    }
                    Islem_Listesinden_Belirli_Etiketi_Sil(islem_listesi_head, silinecek_ornek_basi->etiket_no);
                    free(silinecek_ornek_basi->durum);
                    free(silinecek_ornek_basi->etiket_no);
                    free(silinecek_ornek_basi);
                    silinecek_ornek_basi = sonraki_silinecek;
                }
            }
            if (!adet_azaltma_bitti) { 
                onceki_ornek = aktif_ornek;
                aktif_ornek = aktif_ornek->next;
            }
        }
    }
    duzenlenecek_kitap->adet = yeni_kitap_adeti;

    printf("Kitap (ISBN: %s) basariyla duzenlendi.\n", duzenlenecek_kitap->ISBN);
}
void Kitap_Listele(Kitap** head) {
    if (head == NULL && *head == NULL) {
        printf("Kutuphanede kayitli kitap yok.\n");
        return;
    }

    Kitap* temp = *head;
    printf("Kitap Listesi:\n");
    while (temp != NULL) {
        printf("\nKitap Adi: %s\n", temp->kitap_adi);
        printf("ISBN: %s\n", temp->ISBN);
        printf("Adet: %d\n", temp->adet);
        printf("Ornekler:\n");

        Kitap_Ornek* ornek = temp->ornekler;
        while (ornek != NULL) {
            printf("  Etiket No: %s | Durum: %s\n", ornek->etiket_no, ornek->durum);
            ornek = ornek->next;
        }
        temp = temp->next;
    }
}

void Kitap_Ornek_Listeyi_Sil(Kitap_Ornek** head) {
    Kitap_Ornek* temp = *head;
    while (temp != NULL) {
        Kitap_Ornek* sonraki = temp->next;
        free(temp->durum);
        free(temp->etiket_no);
        free(temp);
        temp = sonraki;
    }
    *head = NULL;
}

void Kitap_Listeyi_Sil(Kitap** head) {
    Kitap* temp = *head;
    while (temp != NULL) {
        Kitap* sonraki = temp->next;

        free(temp->kitap_adi);
        free(temp->ISBN);

        Kitap_Ornek_Listeyi_Sil(&temp->ornekler);

        free(temp);
        temp = sonraki;
    }
    *head = NULL;
}

void Dosyaya_Kitaplari_Yaz(Kitap* head, const char* Dosya_Adi1, const char* Dosya_Adi2);
void Dosyaya_Islem_Listesi_Yaz(KitapIslem* head, const char* dosyaAdi);

void Kitap_Odunc_Al(Kitap** kitap_head, Ogrenci** ogrenci_head, KitapIslem** islem_head, char* isbn, char* ogrenci_id, char* tarih) {
    Ogrenci* ogr = *ogrenci_head;
    while (ogr && strcmp(ogr->ogrenci_no, ogrenci_id) != 0) {
        ogr = ogr->next;
    }
    if (ogr == NULL) {
        printf("Ogrenci bulunamadi!\n");
        return;
    }
    if (ogr->puan < 0) {
        printf("Ogrencinin puani negatif. Odunc islemi yapilamaz!\n");
        return;
    }
    Kitap* kitap = *kitap_head;
    while (kitap && strcmp(kitap->ISBN, isbn) != 0)
        kitap = kitap->next;
    if (kitap == NULL) {
        printf("Kitap bulunamadi!\n");
        return;
    }
    Kitap_Ornek* ornek = kitap->ornekler;
    while (ornek && strcmp(ornek->durum, "RAFTA") != 0)
        ornek = ornek->next;
    if (ornek == NULL) {
        printf("Kitabin tum ornekleri odunc alindi. Islem basarisiz!\n");
        return;
    }
    TarihYapisi odunc_tarihi;
    if (!string_to_tarih(tarih, &odunc_tarihi)) {
        printf("Gecersiz odunc tarihi formati. GG.AA.YYYY kullanin. Islem iptal edildi.\n");
        return;
    }

    free(ornek->durum);
    ornek->durum = strdup(ogrenci_id);
    KitapIslem* yeni = (KitapIslem*)malloc(sizeof(KitapIslem));
    yeni->ogrenciNo = strdup(ogrenci_id);
    yeni->etiketNo= strdup(ornek->etiket_no);
    yeni->islemTuru = 0;
    yeni->tarih = strdup(tarih);
    if (yeni->tarih == NULL) {
        printf("Hata");
        free(yeni->ogrenciNo);
        free(yeni->etiketNo);
        free(yeni);
    }
    yeni->next = NULL; 
       if (*islem_head == NULL) { 
        *islem_head = yeni;
    } else {
        KitapIslem* temp = *islem_head;
        while (temp->next != NULL) { 
            temp = temp->next;
        }
        temp->next = yeni; 
    }
    Dosyaya_Islem_Listesi_Yaz(*islem_head, "KitapOdunc.csv");
    Dosyaya_Kitaplari_Yaz(*kitap_head, "kitaplar.csv", "ornekler.csv");
    printf("Kitap (Etiket: %s) basariyla %s nolu ogrenciye odunc verildi (%s).\n", ornek->etiket_no, ogrenci_id, tarih);
}

void Kitap_Teslim_Et(Kitap** kitap_head, Ogrenci** ogrenci_head, KitapIslem** islem_head, char* etiket_no, char* teslim_tarihi_str) {
    
    char* isbn_teslim_edilen = (char*)malloc(14);
    strncpy(isbn_teslim_edilen, etiket_no, 13);
    isbn_teslim_edilen[13] = '\0';

    Kitap* kitap_node = *kitap_head;
    Kitap_Ornek* ornek_node = NULL;
    Kitap* kitap_arama = kitap_node;
    Kitap_Ornek* ornek_bul = NULL;

    while (kitap_arama != NULL && strcmp(kitap_arama->ISBN, isbn_teslim_edilen) != 0) {
     kitap_arama = kitap_arama->next;
    }
    if (kitap_arama != NULL) {
     Kitap_Ornek* o_iter = kitap_arama->ornekler;
     while (o_iter != NULL && strcmp(o_iter->etiket_no, etiket_no) != 0) {
        o_iter = o_iter->next;
     }
     ornek_bul = o_iter;
    }

    kitap_node = kitap_arama;
    ornek_node = ornek_bul;

    if (ornek_node == NULL) {
        printf("Hata: Teslim edilmek istenen kitap ornegi (%s) sistemde bulunamadi.\n", etiket_no);
        return;
    }

    if (strcmp(ornek_node->durum, "RAFTA") == 0) {
        printf("Bilgi: Bu kitap ornegi (%s) zaten RAFTA gorunuyor. Islem yapilmadi.\n", etiket_no);
        return;
    }

    KitapIslem* islem_iter = *islem_head;
    KitapIslem* son_odunc_islemi = NULL;
    TarihYapisi en_son_aktif_odunc_tarihi = {0,0,0}; 

    while (islem_iter != NULL) {
        if (strcmp(islem_iter->etiketNo, etiket_no) == 0 &&
            islem_iter->islemTuru == 0 &&
            strcmp(islem_iter->ogrenciNo, ornek_node->durum) == 0) { 
            TarihYapisi bu_islem_tarihi;
            if (string_to_tarih(islem_iter->tarih, &bu_islem_tarihi)) {
                if (son_odunc_islemi == NULL || gun_farki_hesapla(en_son_aktif_odunc_tarihi, bu_islem_tarihi) >= 0) {
                    son_odunc_islemi = islem_iter;
                    en_son_aktif_odunc_tarihi = bu_islem_tarihi;
                }
            }
        }
        islem_iter = islem_iter->next;
    }

    if (son_odunc_islemi == NULL) {
        printf("Hata: %s etiket nolu kitap icin aktif bir odunc kaydi bulunamadi (ornek durumu: %s).\n", etiket_no, ornek_node->durum);
        return;
    }

    Ogrenci* ogr = *ogrenci_head;
    while (ogr != NULL && strcmp(ogr->ogrenci_no, son_odunc_islemi->ogrenciNo) != 0) {
        ogr = ogr->next;
    }
    if (ogr == NULL) {
        printf("Hata: Odunc alan ogrenci (%s) sistemde bulunamadi.\n", son_odunc_islemi->ogrenciNo);
        return;
    }
    TarihYapisi odunc_tarihi_obj, teslim_tarihi_obj;
    if (!string_to_tarih(son_odunc_islemi->tarih, &odunc_tarihi_obj) || !string_to_tarih(teslim_tarihi_str, &teslim_tarihi_obj)) {
        printf("Hata: Tarih formatlari gecersiz. GG.AA.YYYY kullanin. Teslim islemi iptal edildi.\n");
        return;
    }
    if (gun_farki_hesapla(odunc_tarihi_obj, teslim_tarihi_obj) < 0) {
        printf("Hata: Teslim tarihi (%s), odunc alma tarihinden (%s) once olamaz. Islem iptal edildi.\n", teslim_tarihi_str, son_odunc_islemi->tarih);
        return;
    }
    int gun_farki = gun_farki_hesapla(odunc_tarihi_obj, teslim_tarihi_obj);
    if (gun_farki > 15) { 
        ogr->puan -= 10;
        printf("Ogrenci %s, kitabi %d gun gec teslim ettigi icin -10 ceza puani aldi. Yeni Puan: %d\n", ogr->ogrenci_no, gun_farki, ogr->puan);
    }

    free(ornek_node->durum);
    ornek_node->durum = strdup("RAFTA");
    if (!ornek_node->durum) { 
        printf("Hata."); 
        return;
    }
    KitapIslem* yeni_teslim_islemi = (KitapIslem*)malloc(sizeof(KitapIslem));
    if (!yeni_teslim_islemi) { 
        printf("Hata"); 
        return; }
    yeni_teslim_islemi->ogrenciNo = strdup(son_odunc_islemi->ogrenciNo);
    yeni_teslim_islemi->etiketNo = strdup(etiket_no);
    yeni_teslim_islemi->tarih = strdup(teslim_tarihi_str);
    yeni_teslim_islemi->islemTuru = 1; 
    yeni_teslim_islemi->next = NULL;  
    if (!yeni_teslim_islemi->ogrenciNo || !yeni_teslim_islemi->etiketNo || !yeni_teslim_islemi->tarih) {
        printf("Hata.");
        free(yeni_teslim_islemi->ogrenciNo); free(yeni_teslim_islemi->etiketNo); free(yeni_teslim_islemi->tarih);
        free(yeni_teslim_islemi);
        free(ornek_node->durum);
        ornek_node->durum = strdup(son_odunc_islemi->ogrenciNo); 
        return;
    }
    if (*islem_head == NULL) {
        *islem_head = yeni_teslim_islemi;
    } else {
        KitapIslem* temp_islem = *islem_head;
        while (temp_islem->next != NULL) {
            temp_islem = temp_islem->next;
        }
        temp_islem->next = yeni_teslim_islemi;
    }
    printf("Kitap (Etiket: %s) basariyla %s nolu ogrenci tarafindan teslim edildi (%s).\n",
           etiket_no, ogr->ogrenci_no, teslim_tarihi_str);
}


void Dosyaya_Kitaplari_Yaz(Kitap* head, const char* Dosya_Adi1, const char* Dosya_Adi2) {
    FILE *kf = fopen(Dosya_Adi1, "w");
    FILE *of = fopen(Dosya_Adi2, "w");

    Kitap *temp = head;
    while(temp) {
        fprintf(kf, "%s,%s,%d\n", temp->kitap_adi, temp->ISBN, temp->adet);
        Kitap_Ornek *o = temp->ornekler;
        while(o) {
            fprintf(of, "%s,%s\n", o->etiket_no, o->durum);
            o = o->next;
        }
        temp = temp->next;
    }

    fclose(kf);
    fclose(of);
}

void Dosyaya_Islem_Listesi_Yaz(KitapIslem* head, const char* dosyaAdi) {
    FILE* dosya = fopen(dosyaAdi, "w");
    if (!dosya) {
        printf("Dosya yazilamadi!\n");
        return;
    }

    KitapIslem* temp = head;
    while (temp) {
        fprintf(dosya, "%s,%s,%d,%s\n",
            temp->etiketNo,
            temp->ogrenciNo,
            temp->islemTuru,
            temp->tarih);
        temp = temp->next;
    }

    fclose(dosya);
}

void KitapIslem_Listeyi_Sil(KitapIslem** head) {
    KitapIslem* current = *head;
    KitapIslem* next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current->ogrenciNo);
        free(current->etiketNo);
        free(current->tarih);
        free(current);
        current = next_node;
    }
    *head = NULL;
}

void Dosyadan_Islem_Oku(KitapIslem** head, const char* dosyaAdi) {
    FILE* dosya = fopen(dosyaAdi, "r");
    if (dosya == NULL) {
        return; 
    }
    char* satir = (char*)malloc(256);
    KitapIslem* tail = NULL; 
    while (fgets(satir, 256, dosya)) {
        KitapIslem* yeni_islem = (KitapIslem*)malloc(sizeof(KitapIslem));
        if (yeni_islem == NULL) {
            printf("Hata.");
            fclose(dosya);
            return;
        }
        char* etiketNo_temp = (char*)malloc(100);   
        char* ogrenciNo_temp = (char*)malloc(20);  
        char* tarih_temp = (char*)malloc(20);       
        if (sscanf(satir, "%99[^,],%19[^,],%d,%19s",
                   etiketNo_temp, ogrenciNo_temp, &yeni_islem->islemTuru, tarih_temp) == 4) {
            yeni_islem->etiketNo = strdup(etiketNo_temp);
            yeni_islem->ogrenciNo = strdup(ogrenciNo_temp);
            yeni_islem->tarih = strdup(tarih_temp);
            yeni_islem->next = NULL;
            if (yeni_islem->etiketNo == NULL || yeni_islem->ogrenciNo == NULL || yeni_islem->tarih == NULL) {
                printf("Hata.");
                free(yeni_islem->etiketNo); 
                free(yeni_islem->ogrenciNo);
                free(yeni_islem->tarih);
                free(yeni_islem);
            } else {
                if (*head == NULL) {
                    *head = yeni_islem;
                    tail = yeni_islem;
                } else {
                    tail->next = yeni_islem;
                    tail = yeni_islem;
                }
            }
        } else {
            free(yeni_islem); 
        }
    }
    fclose(dosya);
}

void Islem_Listesinden_Kitap_Sil(KitapIslem** head, const char* ISBN) {
    KitapIslem* temp = *head;
    KitapIslem* prev = NULL;

    while (temp != NULL) {
        if (strncmp(temp->etiketNo, ISBN, strlen(ISBN)) == 0) {
            KitapIslem* silinecek = temp;

            if (prev == NULL) {
                *head = temp->next;  
                temp = *head;
            } else {
                prev->next = temp->next;
                temp = temp->next;
            }

            free(silinecek->etiketNo);
            free(silinecek->ogrenciNo);
            free(silinecek->tarih);
            free(silinecek);

        } else {
            prev = temp;
            temp = temp->next;
        }
    }
}

void Islem_Listesinden_Ogrenci_Sil(KitapIslem** head, const char* ogrenci_no) {
    KitapIslem* temp = *head;
    KitapIslem* prev = NULL;

    while (temp != NULL) {
        if (strcmp(temp->ogrenciNo, ogrenci_no) == 0) {
            KitapIslem* silinecek = temp;

            if (prev == NULL) {
                *head = temp->next;  
                temp = *head;
            } else {
                prev->next = temp->next;
                temp = temp->next;
            }

            free(silinecek->etiketNo);
            free(silinecek->ogrenciNo);
            free(silinecek->tarih);
            free(silinecek);
        } else {
            prev = temp;
            temp = temp->next;
        }
    }
}
void Dosyadan_Kitap_Oku(Kitap** head, const char* Dosya_Adi1_Kitaplar, const char* Dosya_Adi2_Ornekler) {
    FILE *kf = fopen(Dosya_Adi1_Kitaplar, "r");
    if (kf == NULL) {
        printf("Hata\n");
        return;
    } else {
        char* satir_kitap = (char*)malloc(256);
        while (fgets(satir_kitap, 256, kf)) {
            satir_kitap[strcspn(satir_kitap, "\r\n")] = 0; 

            char* isim_temp = (char*)malloc(101);
            char* isbn_temp = (char*) malloc(14);
            int adet_temp = 0;
            if (sscanf(satir_kitap, "%100[^,],%13[^,],%d", isim_temp, isbn_temp, &adet_temp) == 3) {
                Kitap_Ekle(head, isim_temp, isbn_temp, adet_temp);
            } else {
            }
        }
        fclose(kf);
    } 
    if (*head == NULL && kf == NULL) { 
        printf("Hata\n");
        return;
    }
    FILE *of = fopen(Dosya_Adi2_Ornekler, "r");
    if (of == NULL) { 
        return; 
    }
    char* satir_ornek = (char*)malloc(256); 
    while (fgets(satir_ornek, 256, of)) {
        satir_ornek[strcspn(satir_ornek, "\r\n")] = 0; 
        char* etiket_temp = (char*)malloc(30); 
        char* durum_temp = (char*)malloc(30);  
        if (sscanf(satir_ornek, "%29[^,],%29[^\n]", etiket_temp, durum_temp) == 2) {
            char* isbn_cikar_temp = (char*)malloc(14);
            strncpy(isbn_cikar_temp, etiket_temp, 13);
            isbn_cikar_temp[13] = '\0'; 

            Kitap *k_iter = *head;
            int kitap_bulundu_flag = 0; 
            while (k_iter != NULL && !kitap_bulundu_flag) {
                if (k_iter->ISBN != NULL && strcmp(k_iter->ISBN, isbn_cikar_temp) == 0) {
                    kitap_bulundu_flag = 1; 

                    Kitap_Ornek *o_iter = k_iter->ornekler;
                    int ornek_bulundu_flag = 0; 
                    while (o_iter != NULL && !ornek_bulundu_flag) {
                        if (o_iter->etiket_no != NULL && strcmp(o_iter->etiket_no, etiket_temp) == 0) {
                            ornek_bulundu_flag = 1; 
                            if (o_iter->durum != NULL) {
                                free(o_iter->durum);
                            }
                            o_iter->durum = strdup(durum_temp);
                            if (o_iter->durum == NULL) {
                                printf("HATA\n", etiket_temp);
                            }
                        }
                        if (!ornek_bulundu_flag) { 
                            o_iter = o_iter->next;
                        }
                    } 
                }
                if (!kitap_bulundu_flag) { 
                    k_iter = k_iter->next;
                }
            } 
        } else {
        }
    } 
    fclose(of);
}

void Kitap_Yazar_Eslestir(Kitap_Yazar** dizi, int* boyut, Kitap* kitaplar, Yazar* yazarlar, char* ISBN, int id) {
    
    Kitap* kitap = kitaplar;
    int kflag = 0;

    while(kitap && kflag == 0) {
        if (strcmp(kitap->ISBN, ISBN)==0) {
            kflag = 1;
        }
        kitap = kitap -> next;
    }

    Yazar* yazar = yazarlar;
    int yflag = 0;
    while(yazar && yflag == 0) {
        if (yazar -> yazar_id == id) {
            yflag = 1;
        }
        yazar = yazar -> next;
    }

    if(!kflag) {
        printf("Kitap bulunamadi");
        return;
    }

    if(!yflag) {
        printf("Yazar bulunamadi");
        return;
    }

    *dizi = realloc(*dizi,(*boyut + 1) * sizeof(Kitap_Yazar));
    if (*dizi == NULL) {
        printf("Hata");
        return;
    }

    (*dizi)[*boyut].ISBN = strdup(ISBN);
    if ((*dizi)[*boyut].ISBN == NULL) {
        printf("Hata");
        return;
    }
    (*dizi)[*boyut].yazar_id = id;
    (*boyut)++;
    
    printf("Eslestirme eklendi: %s - %d\n", ISBN, id);

}

void Yazar_Silindiginde_Guncelle(Kitap_Yazar* dizi, int boyut, int silinen_yazar_id) {
    int i;
    for (i = 0; i < boyut; i++) {
        if (dizi[i].yazar_id == silinen_yazar_id) {
            dizi[i].yazar_id = -1;
        }
    }
}

void Kitap_Yazar_Dizisinden_ISBN_Sil(Kitap_Yazar** dizi, int* boyut, const char* ISBN_silinecek) {
    if (*dizi == NULL || *boyut == 0) return;

    int yeni_boyut = 0;
    Kitap_Yazar* yeni_dizi = NULL;
    int i;

    for (i = 0; i < *boyut; i++) {
        if (strcmp((*dizi)[i].ISBN, ISBN_silinecek) != 0) {
            Kitap_Yazar* temp_yeni_dizi = realloc(yeni_dizi, (yeni_boyut + 1) * sizeof(Kitap_Yazar));
            if (temp_yeni_dizi == NULL) {
                printf("Hata.\n");
                int j;
                for(j=0; j<yeni_boyut; ++j) free(yeni_dizi[j].ISBN); 
                free(yeni_dizi);
                return;
            }
            yeni_dizi = temp_yeni_dizi;
            yeni_dizi[yeni_boyut].ISBN = strdup((*dizi)[i].ISBN);
            if (yeni_dizi[yeni_boyut].ISBN == NULL){
                printf("Hata.\n");
            }
            yeni_dizi[yeni_boyut].yazar_id = (*dizi)[i].yazar_id;
            yeni_boyut++;
        }
        free((*dizi)[i].ISBN);
    }
    free(*dizi); 
    *dizi = yeni_dizi;
    *boyut = yeni_boyut;
}

void Kitabin_Yazarini_Guncelle(Kitap_Yazar* dizi, int boyut, Yazar* yazar_listesi, Kitap* kitap_listesi, const char* ISBN, int eski_yazar_id, int yeni_yazar_id) {
    int bulundu_kitap = 0;
    Kitap* k_iter = kitap_listesi;
    while(k_iter != NULL && !bulundu_kitap){ 
        if(strcmp(k_iter->ISBN, ISBN) == 0){
            bulundu_kitap = 1;
        }
        if (!bulundu_kitap) { 
            k_iter = k_iter->next;
        }
    }
    if(!bulundu_kitap){
        printf("Hata: %s ISBN numarali kitap sistemde bulunamadi.\n", ISBN);
        return;
    }
    Yazar* y_kontrol_yeni = yazar_listesi;
    int yeni_yazar_sistemde_var = 0;
    while (y_kontrol_yeni != NULL && !yeni_yazar_sistemde_var) {
        if (y_kontrol_yeni->yazar_id == yeni_yazar_id) {
            yeni_yazar_sistemde_var = 1;
        }
        if(!yeni_yazar_sistemde_var){
            y_kontrol_yeni = y_kontrol_yeni->next;
        }
    }
    if (!yeni_yazar_sistemde_var) { 
        printf("Hata: Yeni yazar ID (%d) sistemde kayitli degil.\n", yeni_yazar_id);
        return;
    }
    int guncellendi = 0;
    int i = 0;
    int eski_eslesme_bulundu_dongude = 0; 
    while (i < boyut && !guncellendi) { 
        if (strcmp(dizi[i].ISBN, ISBN) == 0 && dizi[i].yazar_id == eski_yazar_id) {
            eski_eslesme_bulundu_dongude = 1; 
            int zaten_yeni_ile_eslesik = 0;
            int j = 0;
            while (j < boyut && !zaten_yeni_ile_eslesik) {
                if (i != j && strcmp(dizi[j].ISBN, ISBN) == 0 && dizi[j].yazar_id == yeni_yazar_id) {
                    zaten_yeni_ile_eslesik = 1;
                }
                if (!zaten_yeni_ile_eslesik) {
                    j++;
                }
            }
            if (zaten_yeni_ile_eslesik) {
                printf("Hata: Bu kitap (%s) zaten bu yeni yazar (ID: %d) ile eslestirilmis.\n", ISBN, yeni_yazar_id);
                guncellendi = 1; 
            } else {
                dizi[i].yazar_id = yeni_yazar_id;
                printf("Kitap (ISBN: %s) icin yazar ID %d'den %d'ye guncellendi.\n", ISBN, eski_yazar_id, yeni_yazar_id);
                guncellendi = 1; 
            }
        }
        i++; 
    }
    if (!eski_eslesme_bulundu_dongude && !guncellendi) { 
        printf("Hata: Kitap (ISBN: %s) ile belirtilen eski yazar ID (%d) arasinda bir eslestirme bulunamadi.\n", ISBN, eski_yazar_id);
    }
}

void Dosyaya_Kitap_Yazar_Yaz(Kitap_Yazar* dizi, int eleman_sayisi, const char* dosya_adi) {
    FILE* dosya = fopen(dosya_adi, "w");
    if (dosya == NULL) {
        printf("Dosya yazilamadi: %s\n", dosya_adi);
        return;
    }
    int i;
    for (i = 0; i < eleman_sayisi; i++) {
        fprintf(dosya, "%s,%d\n", dizi[i].ISBN, dizi[i].yazar_id);
    }

    fclose(dosya);
}

void Dosyadan_KitapYazar_Oku_Dizi(Kitap_Yazar** dizi, int* sayi, const char* dosya_adi) {
    FILE* dosya = fopen(dosya_adi, "r");
    if (dosya == NULL) {
        printf("Dosya bulunamadi: %s\n", dosya_adi);
        return;
    }

    char* satir = (char*)malloc(256);
    if (satir == NULL) {
        printf("Hata.\n");
        fclose(dosya);
        return;
    }
    while (fgets(satir, 256, dosya)) {
        char* isbn = (char*)malloc(20);
        int yazar_id;

        if (sscanf(satir, "%19[^,],%d", isbn, &yazar_id) == 2) {
            *dizi = realloc(*dizi, (*sayi + 1) * sizeof(Kitap_Yazar));
            if (*dizi == NULL) {
                printf("Hata.\n");
                fclose(dosya);
                return;
            }
            (*dizi)[*sayi].ISBN = isbn;
            (*dizi)[*sayi].yazar_id = yazar_id;
            (*sayi)++;
        } else {
            free(isbn); 
        }
    }
    
    free(satir);
    fclose(dosya);
}

void KitapYazar_Listeyi_Sil(Kitap_Yazar** dizi, int* boyut) {
    if (dizi == NULL || *dizi == NULL || boyut == NULL || *boyut == 0) return;
    int i;
    for (i = 0; i < *boyut; i++) {
        if ((*dizi)[i].ISBN != NULL) { 
            free((*dizi)[i].ISBN);
        }
    }
    free(*dizi);
    *dizi = NULL;
    *boyut = 0;
}

void Cezali_Ogrencileri_Listele(Ogrenci* head ){
    printf("\n---Cezali Ogrenciler---\n");
    Ogrenci* temp = head;
    int cezali_bulundu = 0;
    while(temp != NULL) {
        if(temp->puan < 100) {
            printf("Ogrenci No: %s, Ad Soyad: %s %s, Puan: %d\n", temp->ogrenci_no, temp->ad, temp->soyad, temp->puan);
            cezali_bulundu = 1;
        }
        temp = temp -> next;
    }
    if(cezali_bulundu==0) {
        printf("Cezali ogrenci yok.\n");
    }
}

void Kitap_Teslim_Etmemis_Ogrencileri_Listele(Ogrenci* ogrenci_head, KitapIslem* islem_head) {
    printf("\n---Kitap Teslim Etmemis Ogrenciler---\n");

    Ogrenci* ogr = ogrenci_head;
    int teslim_etmeyen_ogrenci_bulundu_genel = 0;
    while(ogr) {
        KitapIslem* oduncislem = islem_head;
        int bu_ogrencinin_teslim_etmedigi_kitap_var = 0;
        while(oduncislem && bu_ogrencinin_teslim_etmedigi_kitap_var == 0) {
            if (strcmp(ogr->ogrenci_no, oduncislem->ogrenciNo)==0 && oduncislem->islemTuru==0) {
                KitapIslem* teslimislem = islem_head;
                int teslim_edilmis = 0;
                TarihYapisi odunc_tarihi_obj, teslim_tarihi_kontrol_obj;
                string_to_tarih(oduncislem->tarih, &odunc_tarihi_obj);
                while(teslimislem && teslim_edilmis==0) {
                    if (strcmp(teslimislem->ogrenciNo, ogr->ogrenci_no) == 0 &&
                        strcmp(teslimislem->etiketNo, oduncislem->etiketNo) == 0 &&
                        teslimislem->islemTuru == 1) {
                        string_to_tarih(teslimislem->tarih, &teslim_tarihi_kontrol_obj);
                        if (gun_farki_hesapla(odunc_tarihi_obj, teslim_tarihi_kontrol_obj) >= 0) {
                           teslim_edilmis = 1;
                        }
                    }
                    if (!teslim_edilmis) {
                        teslimislem = teslimislem->next;
                    }
                }
                if (!teslim_edilmis) {
                    bu_ogrencinin_teslim_etmedigi_kitap_var = 1;
                }
            }
            if (!bu_ogrencinin_teslim_etmedigi_kitap_var) {
                 oduncislem = oduncislem->next;
            }
        }
        if (bu_ogrencinin_teslim_etmedigi_kitap_var) {
            printf("Ogrenci No: %s, Ad: %s, Soyad: %s\n",
                   ogr->ogrenci_no,
                   ogr->ad,
                   ogr->soyad);
            teslim_etmeyen_ogrenci_bulundu_genel = 1;
        }
        ogr = ogr -> next;
    }
    if (!teslim_etmeyen_ogrenci_bulundu_genel) {
        printf("Sistemde kitap teslim etmemis ogrenci bulunmamaktadir.\n");
    }
}

void Ogrenci_Bilgileri_Goruntuleme(Ogrenci* ogrenci_head, KitapIslem* islem_head) {
    char *id = (char*)malloc(9);
    printf("Ogrenci id: ");
    fgets(id,9,stdin);
    id[strcspn(id, "\r\n")] = 0; 
    Ogrenci* temp_ogr = ogrenci_head;
    Ogrenci* bulundu = NULL;
    while(temp_ogr !=NULL && bulundu == NULL) {
        if (strcmp(temp_ogr->ogrenci_no,id)==0) {
            bulundu = temp_ogr;
        }
        if (bulundu == NULL) {
            temp_ogr = temp_ogr -> next;
        }
    }
    if (bulundu == NULL) {
        printf("Ogrenci bulunamadi.\n");
    }
    printf("\n---Ogrenci Bilgileri---\n");
    printf("Ogrenci no: %s\n", bulundu->ogrenci_no);
    printf("Ogrenci adi: %s\n", bulundu->ad);
    printf("Ogrenci soyadi: %s\n", bulundu->soyad);
    printf("Ogrenci puani: %d\n", bulundu->puan);
    KitapIslem* islem = islem_head;
    int islem_bulundu = 0;
    while(islem!=NULL) {
        if (strcmp(islem->ogrenciNo,bulundu->ogrenci_no)==0) {
            printf("Etiket no: %s | Islem %s | Tarih %s\n", islem->etiketNo, (islem->islemTuru == 0 ? "Odunc Alma" : "Teslim Etme"), islem->tarih);
            islem_bulundu = 1;
        }
        islem = islem -> next;
    }
    if (!islem_bulundu) {
            printf("Bu ogrenciye ait kitap hareketi bulunmamaktadir.\n");
    }
}

void Raftaki_Kitaplari_Listele(Kitap* kitap_head) {
    printf("\n--- Raftaki Kitaplar ---\n");
    Kitap* temp_kitap = kitap_head;
    int genel_rafta_kitap_bulundu_mu = 0; 
    while (temp_kitap != NULL) { 
        int bu_kitaptan_rafta_yazdirildi_mi = 0;
        Kitap_Ornek* ornek = temp_kitap->ornekler; 
        while (ornek != NULL) {
            if (ornek->durum != NULL && strcmp(ornek->durum, "RAFTA") == 0) {
                if (!bu_kitaptan_rafta_yazdirildi_mi) {
                    printf("\nKitap Adi: %s (ISBN: %s)\n", temp_kitap->kitap_adi, temp_kitap->ISBN);
                    bu_kitaptan_rafta_yazdirildi_mi = 1; 
                }
                printf(" Etiket No: %s (Durum: RAFTA)\n", ornek->etiket_no);
                genel_rafta_kitap_bulundu_mu = 1; 
            }
            ornek = ornek->next;
        }
        temp_kitap = temp_kitap->next;
    }
    if (!genel_rafta_kitap_bulundu_mu) {
        printf("Rafta hic kitap bulunmamaktadir.\n"); 
    }
}

void Gec_Teslim_Edilmis_Kitaplari_Listele(Kitap* kitap_listesi_head, KitapIslem* islem_listesi_head) {
    printf("\n--- Gec Teslim Edilmis Kitaplar (15 Gunu Asan Teslimatlar) ---\n");
    int genel_gec_teslim_bulundu_flag = 0;

    KitapIslem* mevcut_teslim_islemi = islem_listesi_head;
    while (mevcut_teslim_islemi != NULL) {
        if (mevcut_teslim_islemi->islemTuru == 1 && 
            mevcut_teslim_islemi->etiketNo != NULL &&
            mevcut_teslim_islemi->ogrenciNo != NULL &&
            mevcut_teslim_islemi->tarih != NULL) {

            KitapIslem* odunc_islemi_arama_iter = islem_listesi_head;
            KitapIslem* eslesen_odunc_islemi = NULL;
            TarihYapisi en_son_uygun_odunc_tarihi = {0, 0, 0};

            while (odunc_islemi_arama_iter != NULL) {
                if (odunc_islemi_arama_iter->islemTuru == 0 &&
                    odunc_islemi_arama_iter->etiketNo != NULL &&
                    strcmp(odunc_islemi_arama_iter->etiketNo, mevcut_teslim_islemi->etiketNo) == 0 &&
                    odunc_islemi_arama_iter->ogrenciNo != NULL &&
                    strcmp(odunc_islemi_arama_iter->ogrenciNo, mevcut_teslim_islemi->ogrenciNo) == 0 &&
                    odunc_islemi_arama_iter->tarih != NULL) {

                    TarihYapisi bu_odunc_tarihi_obj;
                    TarihYapisi bu_teslim_tarihi_obj;

                    if (string_to_tarih(odunc_islemi_arama_iter->tarih, &bu_odunc_tarihi_obj) &&
                        string_to_tarih(mevcut_teslim_islemi->tarih, &bu_teslim_tarihi_obj)) {
                        if (gun_farki_hesapla(bu_odunc_tarihi_obj, bu_teslim_tarihi_obj) >= 0) {
                            if (eslesen_odunc_islemi == NULL || gun_farki_hesapla(en_son_uygun_odunc_tarihi, bu_odunc_tarihi_obj) > 0) {
                                en_son_uygun_odunc_tarihi = bu_odunc_tarihi_obj;
                                eslesen_odunc_islemi = odunc_islemi_arama_iter;
                            }
                        }
                    }
                }
                odunc_islemi_arama_iter = odunc_islemi_arama_iter->next;
            }

            if (eslesen_odunc_islemi != NULL) {
                TarihYapisi odunc_tarihi_final, teslim_tarihi_final;
                if (eslesen_odunc_islemi->tarih != NULL && mevcut_teslim_islemi->tarih != NULL &&
                    string_to_tarih(eslesen_odunc_islemi->tarih, &odunc_tarihi_final) &&
                    string_to_tarih(mevcut_teslim_islemi->tarih, &teslim_tarihi_final)) {

                    int gun_farki_sonuc = gun_farki_hesapla(odunc_tarihi_final, teslim_tarihi_final);

                    if (gun_farki_sonuc > 15) {
                        char* isbn_cikar = malloc(14);
                        strncpy(isbn_cikar, mevcut_teslim_islemi->etiketNo, 13);
                        isbn_cikar[13] = '\0';

                        char* kitap_adi_bulunan;
                        Kitap* kitap_arama_iter = kitap_listesi_head;
                        int kitap_bulundu_flag_ic = 0;
                        while (kitap_arama_iter != NULL && !kitap_bulundu_flag_ic) {
                            if (kitap_arama_iter->ISBN != NULL && strcmp(kitap_arama_iter->ISBN, isbn_cikar) == 0) {
                                kitap_adi_bulunan = kitap_arama_iter->kitap_adi;
                                kitap_bulundu_flag_ic = 1;
                            }
                            if (!kitap_bulundu_flag_ic) {
                                kitap_arama_iter = kitap_arama_iter->next;
                            }
                        }

                        printf("Kitap Adi: %s\n", kitap_adi_bulunan);
                        printf("ISBN     : %s\n", isbn_cikar); 
                        printf("Etiket No: %s\n", mevcut_teslim_islemi->etiketNo); 
                        printf("(Bu ornek %d gun sonra teslim edilmis)\n\n", gun_farki_sonuc);
                        genel_gec_teslim_bulundu_flag = 1;
                    }
                }
            }
        } 
        mevcut_teslim_islemi = mevcut_teslim_islemi->next;
    } 
    if (!genel_gec_teslim_bulundu_flag) {
        printf("15 gunu asan gec teslim edilmis kitap kaydi bulunmamaktadir.\n");
    }
}

void Yazar_Bilgisi_Goruntule_ve_Kitaplari(Yazar* yazar_listesi_head,Kitap* kitap_listesi_head,Kitap_Yazar* eslesme_dizisi,int eslesme_sayisi) {
    char* ad = (char*)malloc(50);
    char* soyad = (char*)malloc(50);
    Yazar* bulunan_yazar = NULL;
    int yazar_bulundu_flag = 0; 

    printf("\nYazarin Adini Girin: ");
    if (fgets(ad, 50, stdin) == NULL) {
        printf("Hata.\n"); 
        return;
    }
    ad[strcspn(ad, "\r\n")] = 0; 

    printf("Yazarin Soyadini Girin: ");
    if (fgets(soyad, 50, stdin) == NULL) {
        printf("Hata.\n"); return;
    }
    soyad[strcspn(soyad, "\r\n")] = 0; 
    Yazar* temp_yazar = yazar_listesi_head;
    while (temp_yazar != NULL && !yazar_bulundu_flag) {
        if (temp_yazar->ad != NULL && temp_yazar->soyad != NULL &&
            strcmp(temp_yazar->ad, ad) == 0 &&
            strcmp(temp_yazar->soyad, soyad) == 0) {
            bulunan_yazar = temp_yazar;
            yazar_bulundu_flag = 1; 
        }
        if (!yazar_bulundu_flag) { 
            temp_yazar = temp_yazar->next;
        }
    }
    if (!yazar_bulundu_flag) {
        printf("\nSistemde adi '%s', soyadi '%s' olan yazar bulunamadi.\n", ad,soyad);
    } else {
        printf("--- Yazar Bilgileri ---\n");
        printf("Yazar ID : %d\n", bulunan_yazar->yazar_id);
        printf("Adi      : %s\n", bulunan_yazar->ad);   
        printf("Soyadi   : %s\n", bulunan_yazar->soyad);  
        printf("\n--- Bu Yazara Ait Kitaplar ve Ornekleri ---\n");
        int yazarin_kitabi_listelendi_genel = 0;
        int i;
        for (i = 0; i < eslesme_sayisi; i++) {
            if (eslesme_dizisi[i].yazar_id == bulunan_yazar->yazar_id) {
                Kitap* kitap_ara = kitap_listesi_head;
                int kitap_bu_isbn_icin_bulundu = 0;

                while (kitap_ara != NULL && !kitap_bu_isbn_icin_bulundu) {
                    if (kitap_ara->ISBN && eslesme_dizisi[i].ISBN &&
                        strcmp(kitap_ara->ISBN, eslesme_dizisi[i].ISBN) == 0) {

                        printf("\n  Kitap Adi: %s\n", kitap_ara->kitap_adi);
                        printf("  ISBN     : %s\n", kitap_ara->ISBN);
                        printf("  Adet     : %d\n", kitap_ara->adet);
                        printf("  Ornekleri:\n");

                        Kitap_Ornek* mevcut_ornek = kitap_ara->ornekler;
                        if (mevcut_ornek == NULL) {
                            printf("Bu kitaba ait kayitli ornek bulunmamaktadir.\n");
                        } else {
                            while (mevcut_ornek != NULL) {
                                char* etiket_str = (char*)malloc(50);
                                char* durum_str = (char*)malloc(50);
                                if (mevcut_ornek->etiket_no) {
                                    strncpy(etiket_str, mevcut_ornek->etiket_no, 50 - 1);
                                    etiket_str[50 - 1] = '\0';
                                }
                                if (mevcut_ornek->durum) {
                                    strncpy(durum_str, mevcut_ornek->durum, 50 - 1);
                                    durum_str[50 - 1] = '\0';
                                }
                                printf(" Etiket No: %s | Durum: %s\n", etiket_str, durum_str);
                                mevcut_ornek = mevcut_ornek->next;
                            }
                        }
                        yazarin_kitabi_listelendi_genel = 1;
                        kitap_bu_isbn_icin_bulundu = 1;
                    }
                    if (!kitap_bu_isbn_icin_bulundu) kitap_ara = kitap_ara->next;
                }
            }
        } 

        if (!yazarin_kitabi_listelendi_genel) {
            printf("Bu yazara ait kayitli kitap bulunamadi.\n");
        }
    }
}


void Yazar_Menusu(void* p) {
    KutuphaneVeri* veri = (KutuphaneVeri*)p;
    Ogrenci** head1 = veri->ogrenci_head;
    Kitap** head2 = veri->kitap_head;
    Yazar** head3 = veri->yazar_head;
    KitapIslem** head4 = veri->islem_head;
    Kitap_Yazar** head5 = veri->kitapyazar_dizi; 
    int* sayi = veri->kitapyazar_sayisi;  
    int secim;
    char* ad = (char*)malloc(100);
    char* soyad = (char*)malloc(100);
    int yazar_id;
    
    do {
        printf("\n--- Yazar Menu ---\n");
        printf("1. Yazar Ekle\n");
        printf("2. Yazar Sil\n");
        printf("3. Yazar Duzenle\n");
        printf("4. Yazar Bilgisi Goruntuleme\n");
        printf("5. Ana Menu\n");
        printf("Seciminizi girin: ");
        scanf("%d", &secim);
        getchar();  
        
        switch (secim) {
            case 1:
                printf("Yazar adi: ");
                fgets(ad, 100, stdin);
                ad[strcspn(ad, "\n")] = '\0'; 
                printf("Yazar soyadi: ");
                fgets(soyad, 100, stdin);
                soyad[strcspn(soyad, "\n")] = '\0';
                Yazar_Ekle(head3, ad, soyad);
                Dosyaya_Yazarlari_Yaz(*head3, "yazarlar.csv");
                printf("Yazar eklendi.\n");
                break;
            case 2:
                printf("Silinecek yazar ID: ");
                scanf("%d", &yazar_id);
                Yazar_Silme(head3, yazar_id, *head5, *sayi);
                Dosyaya_Yazarlari_Yaz(*head3, "yazarlar.csv");
                Dosyaya_Kitap_Yazar_Yaz(*head5, *sayi, "KitapYazar.csv");
                printf("Yazar silindi.\n");
                break;
            case 3:
                printf("Duzenlenecek yazar ID: ");
                scanf("%d", &yazar_id);
                getchar();
                printf("Yeni adi: ");
                fgets(ad, 100, stdin);
                ad[strcspn(ad, "\n")] = '\0';
                printf("Yeni soyadi: ");
                fgets(soyad, 100, stdin);
                soyad[strcspn(soyad, "\n")] = '\0';
                Yazar_Duzenleme(head3, ad, soyad, yazar_id);
                Dosyaya_Yazarlari_Yaz(*head3, "yazarlar.csv");
                printf("Yazar basariyla duzenlendi.\n");
                break;
            case 4:
                Yazar_Bilgisi_Goruntule_ve_Kitaplari(*head3,*head2,*head5,*sayi);
                break;
            case 5:
                printf("Cikis yapiliyor.\n");
                break;
            default:
                printf("Gecersiz secim.\n");
                break;
        }
        
    } while (secim != 5);

    free(ad);
    free(soyad);
}

void Ogrenci_Menusu(void* p) {
    KutuphaneVeri* veri = (KutuphaneVeri*)p;
    Ogrenci** head1 = veri->ogrenci_head;
    Kitap** head2 = veri->kitap_head;
    Yazar** head3 = veri->yazar_head;
    KitapIslem** head4 = veri->islem_head;
    int secim;
    char* ad = (char*)malloc(100);
    char* soyad = (char*)malloc(100);
    char* no = (char*)malloc(10);
    char* yeni_no = (char*)malloc(10);
    char* ISBN = (char*)malloc(14);
    char* etiketno = (char*)malloc(20);
    char* tarih = (char*)malloc(11);
    int puan;  
    do {
        printf("\n--- Ogrenci Menu ---\n");
        printf("1. Ogrenci Ekle\n");
        printf("2. Ogrenci Sil\n");
        printf("3. Ogrenci Duzenle\n");
        printf("4. Ogrencileri Listele\n");
        printf("5. Kitap Odunc Al\n");
        printf("6. Kitap Teslim Et\n");
        printf("7. Cezali Ogrencileri Listele\n");
        printf("8. Kitap Teslim Etmemis Ogrencileri Listele\n");
        printf("9. Ogrenci Bilgileri Goruntuleme\n");
        printf("10. Ana Menu\n");
        printf("Seciminizi girin: ");
        scanf("%d", &secim);
        getchar();  

        switch (secim) {
            case 1:
                printf("Ogrenci adi: ");
                fgets(ad, 100, stdin);
                ad[strcspn(ad, "\n")] = '\0';
                printf("Ogrenci soyadi: ");
                fgets(soyad, 100, stdin);
                soyad[strcspn(soyad, "\n")] = '\0';
                printf("Ogrenci no: ");
                fgets(no, 10, stdin);
                no[strcspn(no, "\n")] = '\0';
                Ogrenci_Ekle(head1, ad, soyad, no, 100);
                Dosyaya_Ogrencileri_Yaz(*head1, "Ogrenciler.csv");
                break;
            case 2:
                printf("Silinecek ogrenci no: ");
                fgets(no, 10, stdin);
                no[strcspn(no, "\n")] = '\0';
                Ogrenci_Silme(head1, no, head2, head4);
                Dosyaya_Ogrencileri_Yaz(*head1, "Ogrenciler.csv");
                Islem_Listesinden_Ogrenci_Sil(head4, no);
                Dosyaya_Islem_Listesi_Yaz(*head4, "KitapOdunc.csv");
                Dosyaya_Kitaplari_Yaz(*head2, "kitaplar.csv", "ornekler.csv");
                printf("Ogrenci silindi.\n");
                break;
            case 3:
                printf("Duzenlenecek ogrenci no: ");
                fgets(no, 10, stdin);
                no[strcspn(no, "\n")] = '\0';
                printf("Yeni adi: ");
                fgets(ad, 100, stdin);
                ad[strcspn(ad, "\n")] = '\0';
                printf("Yeni soyadi: ");
                fgets(soyad, 100, stdin);
                soyad[strcspn(soyad, "\n")] = '\0';
                printf("Yeni ogrenci no: ");
                fgets(yeni_no, 10, stdin);
                yeni_no[strcspn(yeni_no, "\n")] = '\0';
                Ogrenci_Duzenleme(head1, no, ad, soyad, yeni_no, head2, head4);
                Dosyaya_Ogrencileri_Yaz(*head1, "Ogrenciler.csv");
                Dosyaya_Kitaplari_Yaz(*head2, "kitaplar.csv", "ornekler.csv");
                Dosyaya_Islem_Listesi_Yaz(*head4, "KitapOdunc.csv");
                break;
            case 4:
                Ogrenci_Listeleme(head1);
                break;
            case 5:
                printf("Alinacak kitabin ISBN numarasini giriniz: ");
                fgets(ISBN, 20, stdin);
                ISBN[strcspn(ISBN, "\n")] = 0;
                printf("Alicak ogrencinin numarasini giriniz: ");
                fgets(no, 10, stdin);
                no[strcspn(no, "\n")] = '\0';
                printf("Alinan gunun tarihini giriniz: ");
                fgets(tarih, 11, stdin);
                tarih[strcspn(tarih, "\n")] = 0;
                Kitap_Odunc_Al(head2, head1 , head4, ISBN, no, tarih);
                Dosyaya_Islem_Listesi_Yaz(*head4, "KitapOdunc.csv");
                Dosyaya_Kitaplari_Yaz(*head2, "kitaplar.csv", "ornekler.csv");
                break;
            case 6:
                printf("Teslim edilecek kitabin ETIKET numarasini giriniz: "); 
                fgets(etiketno, 20, stdin); 
                etiketno[strcspn(etiketno, "\r\n")] = '\0';
                printf("Teslim gununun tarihini giriniz (GG.AA.YYYY): "); 
                fgets(tarih, 11, stdin); tarih[strcspn(tarih, "\r\n")] = '\0';
                Kitap_Teslim_Et(head2, head1, head4, etiketno, tarih);
                Dosyaya_Ogrencileri_Yaz(*head1, "Ogrenciler.csv"); 
                Dosyaya_Kitaplari_Yaz(*head2, "kitaplar.csv", "ornekler.csv"); 
                Dosyaya_Islem_Listesi_Yaz(*head4, "KitapOdunc.csv"); 
                break;
            case 7:
                Cezali_Ogrencileri_Listele(*head1);
                break;
            case 8:
                Kitap_Teslim_Etmemis_Ogrencileri_Listele(*head1,*head4);
                break;
            case 9:
                Ogrenci_Bilgileri_Goruntuleme(*head1, *head4);
                break;
            case 10:
                printf("Ana Menuye Donuldu\n");
                break;
            default:
                printf("Gecersiz secim.\n");
                break;
        }
    } while (secim != 10);
    free(ad);
    free(soyad);
    free(no);
    free(yeni_no);
}
void Kitap_Menusu(void *p) {
    KutuphaneVeri* veri = (KutuphaneVeri*)p;
    Ogrenci** head1 = veri->ogrenci_head;
    Kitap** head2 = veri->kitap_head;
    Yazar** head3 = veri->yazar_head;
    KitapIslem** head4 = veri->islem_head;
    Kitap_Yazar** head5 = veri->kitapyazar_dizi; 
    int* sayi = veri->kitapyazar_sayisi;  
    int secim;
    char* ad = (char*)malloc(100);
    char* ISBN = (char*)malloc(20);
    int adet;
    int id;
    int yeni_id;
    char* yeni_ad = (char*)malloc(100);
    char* yeni_ISBN = (char*)malloc(20);
    int yeni_adet;

    do {
        printf("\n*** Kitap Menu ***\n");
        printf("1. Kitap Ekle\n");
        printf("2. Kitap Sil\n");
        printf("3. Kitap Duzenle\n");
        printf("4. Raftaki Kitaplari Listele\n");
        printf("5. Kitap Yazar Eslestir\n");
        printf("6. Kitabin Yazarini Guncelle\n");
        printf("7. Zamaninda Teslim Edilmeyen Kitaplari Listele\n");
        printf("8. Kitaplari Listele\n");
        printf("9. Ana Menu\n");
        printf("Seciminiz: ");
        scanf("%d", &secim);
        getchar(); 

        switch(secim) {
            case 1:
                printf("Kitap Adi: ");
                fgets(ad, 100, stdin);
                ad[strcspn(ad, "\n")] = 0;
                printf("ISBN (13 haneli): ");
                fgets(ISBN, 20, stdin);
                ISBN[strcspn(ISBN, "\n")] = 0;
                printf("Adet: ");
                scanf("%d", &adet);
                getchar();
                Kitap_Ekle(head2, ad, ISBN, adet);
                Dosyaya_Kitaplari_Yaz(*head2, "kitaplar.csv", "ornekler.csv");
                printf("Kitap eklendi.");
                break;
            case 2:
                printf("Silinecek Kitabin ISBN'si: ");
                fgets(ISBN, 20, stdin);
                ISBN[strcspn(ISBN, "\n")] = 0;
                Kitap_Sil(head2, ISBN, head5, sayi);
                Dosyaya_Kitaplari_Yaz(*head2, "kitaplar.csv", "ornekler.csv");
                Dosyaya_Kitap_Yazar_Yaz(*head5, *sayi, "KitapYazar.csv");
                Islem_Listesinden_Kitap_Sil(head4, ISBN);  
                Dosyaya_Islem_Listesi_Yaz(*head4, "KitapOdunc.csv");  
                break;
            case 3:
                printf("Duzenlenecek Kitabin ISBN'si: ");
                fgets(ISBN, 20, stdin);
                ISBN[strcspn(ISBN, "\n")] = 0;
                printf("Yeni Kitap Adi: ");
                fgets(yeni_ad, 100, stdin);
                yeni_ad[strcspn(yeni_ad, "\n")] = 0;
                printf("Yeni ISBN (13 haneli): ");
                fgets(yeni_ISBN, 20, stdin);
                yeni_ISBN[strcspn(yeni_ISBN, "\n")] = 0;
                printf("Yeni Adet: ");
                scanf("%d", &yeni_adet);
                getchar();
                Kitap_Duzenleme(head2, ISBN, yeni_ad, yeni_adet, yeni_ISBN, head4, head5, sayi);
                Dosyaya_Kitaplari_Yaz(*head2, "kitaplar.csv", "ornekler.csv");
                Dosyaya_Kitap_Yazar_Yaz(*head5, *sayi, "KitapYazar.csv");
                Dosyaya_Islem_Listesi_Yaz(*head4, "KitapOdunc.csv");
                break;
            case 4:
                Raftaki_Kitaplari_Listele(*head2);
                break;
            case 5:
                printf("Kitabin ISBN'si: ");
                fgets(ISBN,20,stdin);
                ISBN[strcspn(ISBN,"\r\n")] = 0;
                printf("Eslestirilecek yazarin Id'si: ");
                scanf("%d",&id);
                getchar();
                Kitap_Yazar_Eslestir(head5,sayi,*head2,*head3,ISBN,id);
                Dosyaya_Kitap_Yazar_Yaz(*head5,*sayi,"KitapYazar.csv");
                break;
            case 6:
                printf("Yazar guncellenecek kitabin ISBN'si: ");
                fgets(ISBN,20,stdin);
                ISBN[strcspn(ISBN,"\n")] = 0;
                printf("Guncellenecek eski yazar id'si: ");
                scanf("%d",&id);
                printf("Yeni id: ");
                scanf("%d",&yeni_id);
                Kitabin_Yazarini_Guncelle(*head5, *sayi, *head3, *head2, ISBN, id, yeni_id);
                Dosyaya_Kitap_Yazar_Yaz(*head5, *sayi, "KitapYazar.csv");
                break;
            case 7:
                Gec_Teslim_Edilmis_Kitaplari_Listele(*head2,*head4);
                break;
            case 8:
                Kitap_Listele(head2);
                break;
            case 9:
                printf("Ana Menu\n");
                break;
            default:
                printf("Gecersiz secim!\n");
                break;
        }
    } while(secim != 9);

    free(ad);
    free(yeni_ad);
    free(ISBN);
    free(yeni_ISBN);
}

int main() {
    Yazar* yazarlar = NULL;
    Ogrenci* ogrenciler = NULL;
    Kitap* kitaplar = NULL;
    KitapIslem* islemler = NULL;
    Kitap_Yazar* kitaplarveyazarlar = NULL;
    int kitapyazar = 0;

    Dosyadan_Yazar_Oku(&yazarlar, "yazarlar.csv");
    Dosyadan_Ogrenci_Oku(&ogrenciler, "Ogrenciler.csv");
    Dosyadan_Kitap_Oku(&kitaplar, "kitaplar.csv", "ornekler.csv");
    Dosyadan_Islem_Oku(&islemler, "KitapOdunc.csv");
    Dosyadan_KitapYazar_Oku_Dizi(&kitaplarveyazarlar, &kitapyazar, "KitapYazar.csv");

    KutuphaneVeri veri = { &yazarlar, &ogrenciler, &kitaplar, &islemler, &kitaplarveyazarlar, &kitapyazar };

    void (*menu_fonksiyonlari[])(void*) = {
        Yazar_Menusu,
        Ogrenci_Menusu,
        Kitap_Menusu
    };

    int secim = 0;
    do {
        printf("\n--- Ana Menu ---\n");
        printf("1. Yazar Menusu\n");
        printf("2. Ogrenci Menusu\n");
        printf("3. Kitap Menusu\n");
        printf("4. Cikis\n");
        printf("Seciminizi girin: ");
        scanf("%d", &secim);
        getchar();

        if (secim >= 1 && secim <= 3) {
            menu_fonksiyonlari[secim - 1](&veri);  
        } else if (secim == 4) {
            printf("Programdan cikiliyor.\n");
        } else {
            printf("Gecersiz secim.\n");
        }
    } while (secim != 4);

    Dosyaya_Yazarlari_Yaz(yazarlar, "yazarlar.csv");
    Dosyaya_Ogrencileri_Yaz(ogrenciler, "Ogrenciler.csv");
    Dosyaya_Kitaplari_Yaz(kitaplar, "kitaplar.csv", "ornekler.csv");
    Dosyaya_Islem_Listesi_Yaz(islemler, "KitapOdunc.csv");
    Dosyaya_Kitap_Yazar_Yaz(kitaplarveyazarlar,kitapyazar, "KitapYazar.csv");

    Yazar_Listeyi_Sil(&yazarlar);
    Ogrenci_Listeyi_Sil(&ogrenciler);
    Kitap_Listeyi_Sil(&kitaplar);
    KitapIslem_Listeyi_Sil(&islemler);
    KitapYazar_Listeyi_Sil(&kitaplarveyazarlar,&kitapyazar);


    return 0;
}