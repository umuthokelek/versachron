import tkinter as tk
from tkinter import ttk
import serial

# Seri haberleşme ayarları
port = "/dev/cu.usbserial-0001"  # ESP32'nin portu
baudrate = 115200
ser = serial.Serial(port, baudrate, timeout=1)  # Seri bağlantıyı başlatır

# Zamanlayıcıyı başlatmadan ayarlamak için fonksiyon
def set_timer():
    minutes = int(minute_var.get())  # Kullanıcıdan dakika bilgisi alınır
    seconds = int(second_var.get())  # Kullanıcıdan saniye bilgisi alınır
    total_seconds = minutes * 60 + seconds  # Toplam saniyeye dönüştürülür
    ser.write(f"SET:{total_seconds}\n".encode())  # ESP32'ye "SET" komutu gönderilir
    output_label.config(text=f"Timer set to: {minutes:02d}:{seconds:02d}")  # Durum bilgisi güncellenir

# Geri sayımı başlatmak için fonksiyon
def start_timer():
    ser.write("START\n".encode())  # ESP32'ye "START" komutu gönderilir
    output_label.config(text="Timer started!")  # Durum bilgisi güncellenir

# Geri sayımı durdurmak için fonksiyon
def stop_timer():
    ser.write("STOP\n".encode())  # ESP32'ye "STOP" komutu gönderilir
    output_label.config(text="Timer stopped!")  # Durum bilgisi güncellenir

# Ana GUI penceresi
root = tk.Tk()
root.title("VersaChron: Adaptive Timer System")  # Pencere başlığı

# Pencereyi ekranın ortasında açmak için boyut ve konum ayarı
window_width = 300  # Pencerenin genişliği
window_height = 250  # Pencerenin yüksekliği
screen_width = root.winfo_screenwidth()  # Ekran genişliğini al
screen_height = root.winfo_screenheight()  # Ekran yüksekliğini al
center_x = int(screen_width / 2 - window_width / 2)  # X ekseninde merkez hesaplama
center_y = int(screen_height / 2 - window_height / 2)  # Y ekseninde merkez hesaplama
root.geometry(f"{window_width}x{window_height}+{center_x}+{center_y}")  # Pencere boyutunu ve konumunu ayarla

# Zamanlayıcı giriş alanları
minute_var = tk.StringVar(value="00")  # Dakika girişi için başlangıç değeri
second_var = tk.StringVar(value="00")  # Saniye girişi için başlangıç değeri

frame = ttk.Frame(root)  # Giriş alanlarını yerleştirmek için bir çerçeve oluşturulur
frame.pack(pady=20)

minute_label = ttk.Label(frame, text="Minutes:")  # Dakika için etiket
minute_label.grid(row=0, column=0, padx=5)
minute_entry = ttk.Entry(frame, textvariable=minute_var, width=5)  # Dakika giriş alanı
minute_entry.grid(row=0, column=1, padx=5)

second_label = ttk.Label(frame, text="Seconds:")  # Saniye için etiket
second_label.grid(row=0, column=2, padx=5)
second_entry = ttk.Entry(frame, textvariable=second_var, width=5)  # Saniye giriş alanı
second_entry.grid(row=0, column=3, padx=5)

# Zamanlayıcıyı ayarlamak için buton
set_button = ttk.Button(root, text="Set Timer", command=set_timer)  # "Set Timer" butonu
set_button.pack(pady=5)

# Zamanlayıcıyı başlatmak için buton
start_button = ttk.Button(root, text="Start Timer", command=start_timer)  # "Start Timer" butonu
start_button.pack(pady=5)

# Zamanlayıcıyı durdurmak için buton
stop_button = ttk.Button(root, text="Stop Timer", command=stop_timer)  # "Stop Timer" butonu
stop_button.pack(pady=5)

# Çıktı durumunu göstermek için bir etiket
output_label = ttk.Label(root, text="Timer not set", font=("Arial", 12))  # Başlangıç durumu
output_label.pack(pady=20)

# GUI'yi çalıştır
root.mainloop()  # Ana döngüyü başlatır


