# Определяем переменные окружения сборки

# Корневой каталог исходников Arduino Core
ARDUINO_DIR=/media/andrey/Files/DPOH/Arduino/arduino-1.8.12/hardware/
ARDUINO_AVR_DIR=$$ARDUINO_DIR/arduino/avr/
ARDUINO_AVR_TOOLS_DIR=$$ARDUINO_DIR/tools/avr
# Выбираем целевой контроллер (Arduino Uno, Nano, Mini)
ARDUINO_MCU=atmega328p
# Частота тактирования контроллера
ARDUINO_FCPU = 16000000L

# Ни гуи, ни ядра Qt нам не надо!
QT -= gui core
CONFIG -= qt

# Шаблон проекта - приложение, будет собираться исполняемый файл формата ELF
TEMPLATE = app

DESTDIR = ../bin
TARGET = RemoteVillage

# Подключаем заголовочные файлы
INCLUDEPATH += $$ARDUINO_AVR_DIR/cores/arduino
INCLUDEPATH += $$ARDUINO_AVR_DIR/variants/standard
INCLUDEPATH += $$ARDUINO_AVR_DIR/libraries
INCLUDEPATH += $$ARDUINO_AVR_TOOLS_DIR/include

QMAKE_CC = $$ARDUINO_AVR_TOOLS_DIR/bin/avr-gcc
QMAKE_CFLAGS += -c -g -Os -w -ffunction-sections -fdata-sections
QMAKE_CFLAGS += -MMD -mmcu=$$ARDUINO_MCU -DF_CPU=$$ARDUINO_FCPU
QMAKE_CFLAGS += -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR

QMAKE_CXX = $$ARDUINO_AVR_TOOLS_DIR/bin/avr-g++
QMAKE_CXXFLAGS += -c -g -Os -w  -ffunction-sections -fdata-sections
QMAKE_CXXFLAGS += -fno-exceptions -fno-threadsafe-statics
QMAKE_CXXFLAGS += -MMD -mmcu=$$ARDUINO_MCU -DF_CPU=$$ARDUINO_FCPU
QMAKE_CXXFLAGS += -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR

QMAKE_LINK = $$ARDUINO_AVR_TOOLS_DIR/bin/avr-gcc
QMAKE_LFLAGS = -w -Os -Wl,--gc-sections -mmcu=$$ARDUINO_MCU
QMAKE_LIBS = -lm

QMAKE_POST_LINK += $$ARDUINO_AVR_TOOLS_DIR/bin/avr-objcopy -O ihex -j .text -j .data -S ${TARGET} ${TARGET}.hex


HEADERS += $$files($$ARDUINO_AVR_DIR/cores/arduino/*.h)
HEADERS += $$files($$ARDUINO_AVR_DIR/variants/standard/*.h)
HEADERS += $$files($$ARDUINO_AVR_DIR/libraries/SoftwareSerial/src/*.h)

SOURCES += $$files($$ARDUINO_AVR_DIR/cores/arduino/*.c)
SOURCES += $$files($$ARDUINO_AVR_DIR/cores/arduino/*.cpp)
SOURCES += $$files($$ARDUINO_AVR_DIR/libraries/SoftwareSerial/src/*.cpp)


# Исходники проекта
HEADERS += $$files(../libraries/Sim800l/*.h)
SOURCES += $$files(../libraries/Sim800l/*.cpp)

SOURCES += remotevillage.cpp
