# **Kütüphane Otomasyon Sistemi (C)**

Bu proje; kitapların, öğrencilerin, yazarların ve ödünç alma/teslim etme işlemlerinin yönetildiği, **Dosya Tabanlı (CSV)** ve **Dinamik Veri Yapılı** kapsamlı bir kütüphane yönetim sistemidir. C dilinin düşük seviyeli bellek yönetimi ve veri yapısı yeteneklerini profesyonel bir mimariyle sunar.

---

## **🎮 Modüler Menü Yapısı ve Özellikler**

Proje, 3 ana yönetim katmanından oluşur ve her katman kendine has derin algoritmalara sahiptir:

### **1. Kitap Yönetim Menüsü**
* **Dinamik Örnekleme:** Bir kitap eklendiğinde, adet sayısına göre otomatik olarak benzersiz etiketli (ISBN_1, ISBN_2...) fiziksel kopyalar oluşturulur.
* **Akıllı Düzenleme:** ISBN veya adet değiştiğinde, tüm bağlı işlemleri ve yazar eşleşmelerini otomatik olarak günceller.
* **Stok Durumu:** Kitapların hangi kopyasının rafta, hangisinin kimde olduğunun detaylı listelenmesi.
* **Gecikme Analizi:** 15 günü aşan teslimatların sistem tarafından otomatik olarak "Geç Teslim" olarak işaretlenmesi.

### **2. Öğrenci Yönetim Menüsü**
* **Gelişmiş Takip:** Öğrencinin geçmişteki tüm kitap hareketlerini (Odunc/Teslim) tarihleriyle birlikte tutar.
* **Puan ve Ceza Sistemi:** Zamanında teslim edilmeyen her kitap için öğrenciden otomatik **-10 ceza puanı** düşülür.
* **Kısıtlama:** Puanı negatif olan öğrencilerin yeni kitap ödünç alması sistem tarafından engellenir.
* **Özel Listeleme:** "Cezalı Öğrenciler" ve "Kitap Teslim Etmemiş Öğrenciler" için hızlı filtreleme modülleri.

### **3. Yazar ve Veri İlişkileri**
* **Yazar-Kitap Eşleştirme:** Yazarlar ve kitaplar arasında dinamik bir veri tablosu oluşturur.
* **İlişkisel Veri Güncelleme:** Bir yazar silindiğinde veya güncellendiğinde, kütüphanedeki tüm kitapların yazar bilgileri "Data Integrity" (Veri Bütünlüğü) prensibiyle senkronize edilir.

---

## **🛠 Teknik Altyapı ve Algoritmalar**

* **Veri Yapıları:** * **Nested Linked Lists:** Kitap-Örnek ilişkisi için iç içe bağlı listeler.
  * **Doubly Linked List:** Öğrenci yönetimi için çift yönlü bağlı liste.
  * **Dynamic Array:** Kitap-Yazar eşleşmeleri için `realloc` ile büyüyen dinamik diziler.
* **Algoritmalar:** * **Tarih Hesaplama:** İki tarih arasındaki farkı gün bazında hesaplayarak ceza sistemini çalıştıran özel mantık.
  * **String Parsing:** `sscanf` ve `strtok` ile CSV dosyalarından hatasız veri okuma.
* **Hata Yönetimi:** Bellek taşmaları (Buffer overflow) ve geçersiz ISBN formatları için kapsamlı kontroller.

