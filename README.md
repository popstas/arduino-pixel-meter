Прошивка для Arduino + RGB LED WS2801 + LCD 16x2

### Библиотеки
- [Adafruit_WS2801](https://github.com/adafruit/Adafruit-WS2801-Library) - можно поставить через менеджер библиотек
- LiquidCrystal_I2C - версия из менеджера библиотек не работает! 
  Также не работает [fdebrabander/Arduino-LiquidCrystal-I2C-library](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library),
  не помню, откуда я взял эту либу.
  Нужно скопировать `libraries/LiquidCrystal_I2C` в `~/Documents/Arduino/libraries`

### Использование
Принимает строки на COM-порт, разделенные `\n`, например:

```
199|message|50
```

#### Где:
- `199` - уровень от 0 до 100 - от красного до зеленого  
        уровень от 101 до 199 - мигающий от красного до зеленого 3 раза, потом возврат к предыдущему состоянию  
        уровень от 201 до 299 - мигающий от красного до зеленого непрерывно  
        -1 - отключение пикселя

- `message` - сообщение, должно уложиться в 16х2 дисплей, разделяется символом \\

- `50` - яркость, от 0 до 100, по умолчанию 100