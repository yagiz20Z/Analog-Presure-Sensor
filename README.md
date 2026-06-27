# Analog-Presure-Sensor

| Parameter | Value |
| :--- | :--- |
| Medium | liquid/gas without corrosion |
| Wiring | Gravity-3Pin (Signal-VCC-GND) |
| Pressure Measurement Range | 0~1.6 Mpa |
| Input Voltage | +5 VDC |
| Output Voltage | 0.5~4.5 V |
| Measurement Accuracy | 0.5%~1%FS (0.5%, 0~55°C) |
| Threadably | G1/4 |
| Adapter | G1/2 to G1/4 |
| Waterproof Level | IP68 |
| Operating Temperature | -20~85°C |
| Response Time | < 2.0 ms |
| Quiescent Current | 2.8 mA |
| Normal Operating Pressure | ≤2.0 Mpa |
| Damaged Pressure | ≥3.0 Mpa |
| Service Life | ≥10'000'000 times (10 million) |

# APS - Analog Pressure Sensor Library

Bu kütüphane, STM32 tabanlı projelerde **SEN0257 analog basınç sensöründen** ADC ile veri okumak, okunan ADC değerini voltaja çevirmek, voltajdan basınç hesaplamak ve yaklaşık derinlik bilgisini üretmek için hazırlanmıştır.

Kütüphane modüler yapıdadır. ADC, UART ve kalibrasyon değerleri doğrudan `APS.c` içine yazılmaz. Bunun yerine `SoH_APS_Config.c` dosyasında tanımlanan bir konfigürasyon yapısı üzerinden kütüphaneye aktarılır.

---

## Amaç

Bu yapının amacı, sensör okuma kodunu `main.c` içinden ayırarak daha düzenli, tekrar kullanılabilir ve geliştirilebilir bir kütüphane yapısı oluşturmaktır.

Kütüphane şunları yapar:

- ADC ham değerini okur.
- ADC değerini ADC pinindeki voltaja çevirir.
- Voltaj bölücü kullanılıyorsa gerçek sensör çıkış voltajını hesaplar.
- SEN0257 sensör formülüne göre basınç hesaplar.
- Yüzey voltajına göre yaklaşık derinlik hesaplar.
- İstenirse değerleri UART üzerinden terminale yazdırır.

---

## Dosya Yapısı

```text
Core/
├── Inc/
│   ├── APS.h
│   └── SoH_APS_Config.h
│
└── Src/
    ├── APS.c
    ├── SoH_APS_Config.c
    └── main.c
```

---

## Kullanılan Dosyalar

### `APS.h`

Kütüphanenin dışarıya açılan veri tiplerini ve fonksiyon prototiplerini içerir.

Önemli yapılar:

```c
typedef struct
{
    uint32_t raw;
    float adc_voltage;
    float sensor_voltage;
    float pressure_mpa;
    float pressure_bar;
    float depth_meter;
} APS_Data_t;
```

Bu yapı, sensörden okunan ve hesaplanan değerleri tutar.

```c
typedef struct
{
    ADC_HandleTypeDef  *hadc;
    UART_HandleTypeDef *huart;

    float adc_vref;
    float adc_max;
    float voltage_divider_ratio;

    float sensor_zero_voltage;
    float sensor_scale_mpa;

    float surface_voltage;
} APS_Config_t;
```

Bu yapı, kütüphanenin hangi ADC ve UART birimini kullanacağını ve hangi kalibrasyon değerleriyle çalışacağını belirler.

```c
typedef struct
{
    APS_Config_t config;
    APS_Data_t data;
} APS_Handle_t;
```

Bu yapı, kütüphanenin ana kontrol yapısıdır.

---

## Konfigürasyon Yapısı

Kütüphanenin donanım ve kalibrasyon ayarları `SoH_APS_Config.c` dosyasında tutulur.

Örnek konfigürasyon:

```c
#include "SoH_APS_Config.h"
#include "adc.h"
#include "usart.h"

APS_Config_t aps_config =
{
    .hadc = &hadc1,
    .huart = &huart1,

    .adc_vref = 3.3f,
    .adc_max = 4095.0f,

    .voltage_divider_ratio = 1.5f,

    .sensor_zero_voltage = 0.5f,
    .sensor_scale_mpa = 0.4f,

    .surface_voltage = 0.75f
};
```

Bu yapı sayesinde `APS.c` dosyası doğrudan `hadc1`, `huart1` veya sabit kalibrasyon değerlerine bağlı kalmaz.

---

## main.c İçinde Kullanım

`main.c` dosyasına şu include dosyaları eklenir:

```c
#include "APS.h"
#include "SoH_APS_Config.h"
```

Global alanda bir APS handle oluşturulur:

```c
APS_Handle_t aps;
```

STM32 HAL init fonksiyonlarından sonra APS başlatılır:

```c
MX_GPIO_Init();
MX_ADC1_Init();
MX_USART1_UART_Init();

APS_Init(&aps, &aps_config);
```

Ana döngü içinde sensör okunur ve UART üzerinden yazdırılır:

```c
while (1)
{
    APS_Read(&aps);
    APS_Print(&aps);

    HAL_Delay(500);
}
```

---

## Hesaplama Mantığı

### ADC Değerinden Voltaj Hesabı

ADC ham değeri şu formülle ADC pinindeki voltaja çevrilir:

```c
adc_voltage = (raw * adc_vref) / adc_max;
```

### Voltaj Bölücü Düzeltmesi

Eğer sensör çıkışı voltaj bölücü üzerinden ADC pinine giriyorsa gerçek sensör voltajı şu şekilde hesaplanır:

```c
sensor_voltage = adc_voltage * voltage_divider_ratio;
```

Eğer voltaj bölücü kullanılmıyorsa:

```c
.voltage_divider_ratio = 1.0f
```

olarak ayarlanmalıdır.

### Basınç Hesabı

SEN0257 için kullanılan basınç hesabı:

```c
pressure_mpa = (sensor_voltage - sensor_zero_voltage) * sensor_scale_mpa;
```

Bu projedeki varsayılan değerler:

```c
.sensor_zero_voltage = 0.5f;
.sensor_scale_mpa = 0.4f;
```

Bar hesabı:

```c
pressure_bar = pressure_mpa * 10.0f;
```

### Derinlik Hesabı

Derinlik hesabında yüzeyde okunan voltaj referans alınır.

```c
pressure_bar = 4.0f * (sensor_voltage - surface_voltage);
depth_meter = pressure_bar * 10.2f;
```

Varsayılan yüzey voltajı:

```c
.surface_voltage = 0.75f;
```

Bu şu anlama gelir:

```text
0.75 V -> 0 metre kabul edilir.
```

---

## Örnek UART Çıktısı

```text
ADC: 537 | Vadc: 0.43 V | Vsens: 0.65 V | P: 0.06 MPa | 0.60 bar | Depth: 0.00 m
```

---

## Float Print Ayarı

STM32CubeIDE içinde `%.2f` gibi float değerleri UART'a yazdırmak için float printf desteği açılmalıdır.

CubeIDE üzerinden:

```text
Project Properties
C/C++ Build
Settings
MCU Settings
Use float with printf from newlib-nano
```

Alternatif olarak linker flags kısmına şu eklenebilir:

```text
-u _printf_float
```

Sonrasında proje temizlenip tekrar derlenmelidir:

```text
Project > Clean
Project > Build
```

---

## Donanım Bağlantısı

SEN0257 sensörü 5V ile beslendiğinde analog çıkışı 4.5V seviyesine kadar çıkabilir. STM32 ADC pinleri genellikle 3.3V seviyesine kadar güvenlidir. Bu yüzden sensör sinyalinin ADC pinine doğrudan verilmesi önerilmez.

Önerilen bağlantı:

```text
SEN0257 VCC    -> 5V
SEN0257 GND    -> STM32 GND
SEN0257 Signal -> Voltaj bölücü -> STM32 ADC pini
```

Voltaj bölücü örneği:

```text
SEN0257 Signal --- R1 --- ADC pini --- R2 --- GND
```

Örnek direnç değerleri:

```text
R1 = 10k
R2 = 20k
```

Bu durumda yaklaşık oran:

```text
voltage_divider_ratio = 1.5
```

---

## Modülerlik Avantajı

Bu kütüphane yapısında ADC, UART ve kalibrasyon değerleri `APS.c` içine sabit yazılmadığı için sistem kolayca değiştirilebilir.

Örneğin UART1 yerine UART2 kullanılacaksa sadece `SoH_APS_Config.c` dosyasında şu satır değiştirilir:

```c
.huart = &huart2,
```

ADC1 yerine ADC2 kullanılacaksa:

```c
.hadc = &hadc2,
```

Yüzey voltajı değişirse:

```c
.surface_voltage = 0.72f,
```

Voltaj bölücü kullanılmıyorsa:

```c
.voltage_divider_ratio = 1.0f,
```

---

## Geliştirme Notları

İleride kütüphaneye şu özellikler eklenebilir:

- Ortalama alma filtresi
- ADC noise azaltma
- Hata kontrol sistemi
- `APS_Status_t` dönüş değerleri
- UART print fonksiyonunu opsiyonel hale getirme
- Derinlik için tatlı su / tuzlu su yoğunluğu seçimi
- Basınç sensörü kalibrasyon fonksiyonu
- FreeRTOS task yapısına uyarlama

---

## Lisans

Bu kütüphane proje içi kullanım için hazırlanmıştır. Takım üyeleri tarafından geliştirilebilir, düzenlenebilir ve farklı STM32 projelerine uyarlanabilir.



## kalman eklenecek