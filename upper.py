#*************************************************************************
#   Copyright © 2015 JiangLin. All rights reserved.
#   File Name: upper.py
#   Author:JiangLin
#   Mail:xiyang0807@gmail.com
#   Created Time: 2015-12-17 22:51:38
#*************************************************************************
#!/usr/bin/env python
# -*- coding=UTF-8 -*-
import sys
from PySide import QtGui, QtCore

class Example(QtGui.QWidget):

    def __init__(self):
        super(Example, self).__init__()

        self.grid = QtGui.QGridLayout()

        self.display()
        self.first_line()
        self.second_line()
        self.third_line()

        self.setLayout(self.grid)
        self.setGeometry(600, 600, 600, 400)
        self.center()
        self.setWindowTitle('上位机')
        self.show()

    def display(self):
        display_edit = QtGui.QTextEdit(self)

        self.grid.addWidget(display_edit, 0, 0,1,14)

    def first_line(self):
        input_label = QtGui.QLabel('输入框:',self)
        self.grid.addWidget(input_label, 1, 0)

        input_edit = QtGui.QLineEdit(self)
        self.grid.addWidget(input_edit, 1,1,1,7)

        self.pbar = QtGui.QProgressBar(self)
        self.pbar.setGeometry(30, 40, 200, 25)
        self.post_button = QtGui.QPushButton('发送',self)
        self.grid.addWidget(self.post_button, 1, 8,1,2)
        self.post_button.clicked.connect(self.doAction)

        self.timer = QtCore.QBasicTimer()
        self.step = 0

        clean_button = QtGui.QPushButton('清除',self)
        self.grid.addWidget(clean_button, 1, 10,1,2)
        clean_button.clicked.connect(self.zhilin)

        hex_display = QtGui.QCheckBox('HEX显示', self)
        self.grid.addWidget(hex_display, 1, 12,1,2)

    def second_line(self):

        serial_label = QtGui.QLabel('串口号:', self)
        self.grid.addWidget(serial_label, 2, 0)

        serial_num = QtGui.QComboBox(self)
        serial_num.addItem("COM1")
        serial_num.addItem("COM2")
        serial_num.addItem("COM3")
        self.grid.addWidget(serial_num, 2,1,1,3)

        brud_label = QtGui.QLabel('波特率:', self)
        self.grid.addWidget(brud_label, 2, 4)

        brud_bit = QtGui.QComboBox(self)
        brud_bit.addItem("100")
        brud_bit.addItem("1000")
        brud_bit.addItem("10000")
        self.grid.addWidget(brud_bit, 2, 5,1,3)

        open_serial_button = QtGui.QPushButton('打开串口',self)
        self.grid.addWidget(open_serial_button, 2, 8,1,2)

        expand_button = QtGui.QPushButton("扩展",self)
        self.grid.addWidget(expand_button, 2, 10,1,2)

        hex_post = QtGui.QCheckBox('HEX发送', self)
        self.grid.addWidget(hex_post, 2, 12,1,2)

    def third_line(self):

        data_label = QtGui.QLabel('数据位:', self)
        self.grid.addWidget(data_label, 3, 0)

        data_bit = QtGui.QComboBox(self)
        data_bit.addItem("2")
        data_bit.addItem("4")
        data_bit.addItem("8")
        self.grid.addWidget(data_bit, 3, 1,1,3)

        check_label = QtGui.QLabel('校验位:', self)
        self.grid.addWidget(check_label, 3, 4)

        check_bit = QtGui.QComboBox(self)
        check_bit.addItem("1")
        check_bit.addItem("2")
        check_bit.addItem("3")
        self.grid.addWidget(check_bit, 3, 5,1,3)

        stop_label = QtGui.QLabel('停止位:', self)
        self.grid.addWidget(stop_label, 3, 8)

        stop_bit = QtGui.QComboBox(self)
        stop_bit.addItem("1")
        stop_bit.addItem("2")
        self.grid.addWidget(stop_bit, 3,9,1,3)

    def closeEvent(self, event):

        reply = QtGui.QMessageBox.question(self, 'Message',
            "Are you sure to quit?", QtGui.QMessageBox.Yes |
            QtGui.QMessageBox.No, QtGui.QMessageBox.No)

        if reply == QtGui.QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()

    def center(self):
        '''窗体居中'''
        qr = self.frameGeometry()
        cp = QtGui.QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

    def timerEvent(self, e):

        if self.step >= 100:
            self.timer.stop()
            self.post_button.setText('Finished')
            return
        self.step = self.step + 1
        self.pbar.setValue(self.step)

    def doAction(self):

        if self.timer.isActive():
            self.timer.stop()
            self.post_button.setText('Start')
        else:
            self.timer.start(100, self)
            self.post_button.setText('Stop')

    def zhilin(self):
        self.step = 0
        self.pbar.setValue(self.step)

def main():

    app = QtGui.QApplication(sys.argv)
    ex = Example()
    ex = ex
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
