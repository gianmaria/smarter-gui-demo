QT += core gui widgets network

CONFIG += c++17 war_on

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET  = smarter_gui_demo

SOURCES += \
    src/axis_widget.cpp \
    src/main.cpp \
    src/main_window.cpp \
    src/smarter_protocol_communication_manager.cpp \
    src/smarter_protocol_streaming.c

HEADERS += \
    src/axis_widget.h \
    src/common.h \
    src/main_window.h \
    src/smarter_protocol_communication_manager.h \
    src/smarter_protocol_streaming.h \
    src/Leonardo_msg.h

FORMS += \
    src/axis_widget.ui \
    src/main_window.ui

OTHER_FILES +=


# Define the folder where build result are deployed
CONFIG(debug, debug|release) {
   DESTDIR = $${_PRO_FILE_PWD_}/../build/debug
}
else {
   DESTDIR = $${_PRO_FILE_PWD_}/../build/release
}

