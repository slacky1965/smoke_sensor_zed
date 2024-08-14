# Smoke Sensor Zigbee Telink TLSR8258 (E-BYTE E180-Z5812SP)

[Repository smoke_sensor_zed](https://github.com/slacky1965/smoke_sensor_zed)


* [Описание](#description)
* [Железо](#hardware)
* [Софт](#software)  
* [Принцип работы](#firmware)
* [Настройка](#settings)
* [Home Assistant](#home_assistant)

---

## <a id="description">Описание</a>

* Сделано в виде дополнительной платы к заводскому пожарному дымовому извещателю [Рубеж ИП 212-50М2](https://yandex.ru/search?text=%D0%A0%D1%83%D0%B1%D0%B5%D0%B6+%D0%98%D0%9F+212-50%D0%9C2&lr=213).
* Изменяет статус ALARM1 в IAS при обнаружении дыма.
* Изменяет статус TAMPER в IAS при снятии извещателя с крепежной пластины.
* Посылает команду On-Off (Off-On или Toggle, в зависимости от выбранных настроек) при обнаружении дыма.
* Сохраняет настройки в энергонезависимой памяти модуля.
* Передает показания по сети Zigbee.
* Кластер On_Off имеет возможность напрямую подключаться к исполнительному устройству минуя координатор.
* Взаимодейстивие с "умными домами" через zigbee2mqtt.
* Первоначальная настройка происходит через web-интерфейс zigbee2mqtt.
* Подключиться к сети zigbee - удерживать кнопку на плате устройства более 5 секунд.
* Покинуть сеть zigbee - удерживать кнопку на плате устройства более 5 секунд.
* Сделать restart модуля - нажать кнопку 5 раз.
* При одиночном нажатии кнопки модуль просыпается и высылает отчеты.

---

## <a id="hardware">Железо</a>

В проекте используется модуль от компании E-BYTE на чипе TLSR8258F512ET32 - E180-Z5812SP.

<img src="https://raw.githubusercontent.com/slacky1965/smoke_sensor_zed/main/doc/images/E180-Z5812SP.jpg" alt="E-BYTE E180-Z5812SP"/>

Испытывалось все на вот такой плате совместной разработки (спасибо [Олегу](https://t.me/Novgorod73))

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/tlsr8258_devkit_zi.png" alt="TLSR8258 devkit_zi"/>

---

**Схема**

Схема изделия.

<img src="https://raw.githubusercontent.com/slacky1965/smoke_sensor_zrd/main/doc/images/Schematic_Smoke_Sensor_TLSR8258.jpg"/>

**Плата**

<img src="https://raw.githubusercontent.com/slacky1965/smoke_sensor_zrd/main/doc/images/board_top_empty.png"/>

<img src="https://raw.githubusercontent.com/slacky1965/smoke_sensor_zrd/main/doc/images/board_bottom_empty.png"/>

На гребенку выведены следующие пины модуля

* SWS, GND - для заливки в модуль прошивки
* TXD-DBG - на всякий случай, вдруг кому-то пригодится для отладки.

Ссылки на проект в easyeda

пока нет.

**Корпус**

Корпус напечатан на 3D принтере.

<img src="https://raw.githubusercontent.com/slacky1965/electricity_meter_zrd/main/doc/images/box/main_and_cover.png"/>

[STL файл основания](https://raw.githubusercontent.com/slacky1965/electricity_meter_zrd/main/doc/ElectricityMeter_Main1_short.STL)

[STL крышки](https://raw.githubusercontent.com/slacky1965/electricity_meter_zrd/main/doc/ElectricityMeter_Cover_New.STL)


**Готовое устройство**

<img src="https://raw.githubusercontent.com/slacky1965/electricity_meter_zrd/main/doc/images/device_3d_box.jpg"/>

<img src="https://raw.githubusercontent.com/slacky1965/electricity_meter_zrd/main/doc/images/device_3d_box.png"/>

---
