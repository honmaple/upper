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
from PySide import QtGui
import serial
import os
from time import sleep
from threading import Thread
import subprocess
import psycopg2

class SerialUpper(QtGui.QWidget):

    def __init__(self):
        super(SerialUpper, self).__init__()
        self.ser = serial.Serial()
        self.brud_bit_data = 4800
        self.serial_num_data = '/dev/ttyUSB0'
        self.start()
        self.initUI()

    def initUI(self):
        self.grid = QtGui.QGridLayout()
        self.display()
        self.query()
        self.setting()
        self.setLayout(self.grid)
        self.setGeometry(600, 600, 800, 600)
        self.center()
        self.setWindowTitle('上位机')
        self.show()

    '''显示窗体'''
    def display(self):
        self.post_data_label = QtGui.QTextEdit(self)
        self.grid.addWidget(self.post_data_label, 0,0,6,6)

        self.receive_data_label = QtGui.QTextEdit(self)
        self.grid.addWidget(self.receive_data_label, 0,6,6,6)

        # self.receive_data_label = QtGui.QFrame(self)
        # self.receive_data_label.setFrameShape(QtGui.QFrame.StyledPanel)
        # self.grid.addWidget(self.receive_data_label, 0,6,5,6)

        self.open_file = QtGui.QLineEdit(self)
        self.grid.addWidget(self.open_file,0,12,1,3)

        open_file_button = QtGui.QPushButton('打开文件',self)
        self.grid.addWidget(open_file_button,0,15,1,1)
        open_file_button.clicked.connect(self.do_open_file)

        write_file_button = QtGui.QPushButton('烧写',self)
        self.grid.addWidget(write_file_button,1,14,1,2)
        write_file_button.clicked.connect(self.do_write_file)

        serial_label = QtGui.QLabel('串口号:', self)
        self.grid.addWidget(serial_label, 2, 12)

        self.serial_num = QtGui.QComboBox(self)
        if os.path.exists(r'/dev/ttyUSB0'):
            self.serial_num.addItem("ttyUSB0")
        if os.path.exists(r'/dev/ttyUSB1'):
            self.serial_num.addItem("ttyUSB1")
        if os.path.exists(r'/dev/ttyUSB2'):
            self.serial_num.addItem("ttyUSB2")
        self.grid.addWidget(self.serial_num, 2,13,1,3)
        self.serial_num.activated[str].connect(self.set_serial_num)

        brud_label = QtGui.QLabel('波特率:', self)
        self.grid.addWidget(brud_label, 3, 12)

        brud_bit = QtGui.QComboBox(self)
        brud_bit.addItem("4800")
        brud_bit.addItem("9600")
        brud_bit.addItem("19200")
        self.grid.addWidget(brud_bit, 3,13,1,3)
        brud_bit.activated[str].connect(self.set_brud)

        self.open_serial_button = QtGui.QPushButton('打开串口',self)
        self.grid.addWidget(self.open_serial_button,4,12,1,2)
        self.open_serial_button.clicked.connect(self.do_open_serial)

        scan_serial_button = QtGui.QPushButton('扫描串口',self)
        self.grid.addWidget(scan_serial_button,4,14,1,2)
        scan_serial_button.clicked.connect(self.do_scan_serial)

        self.post_data_input = QtGui.QLineEdit(self)
        self.grid.addWidget(self.post_data_input,5,12,1,3)

        post_button = QtGui.QPushButton('发送',self)
        self.grid.addWidget(post_button, 5,15,1,1)
        post_button.clicked.connect(self.do_cha_input_data)

        xiu_button = QtGui.QPushButton('修改',self)
        self.grid.addWidget(xiu_button, 5,16,1,1)
        xiu_button.clicked.connect(self.do_input_data)
    '''查询窗体'''
    def query(self):
        follow_addr_button = QtGui.QPushButton('从机地址',self)
        self.grid.addWidget(follow_addr_button, 6,0,1,2)
        follow_addr_button.clicked.connect(self.query_addr)

        temperature_query_button = QtGui.QPushButton('温度查询',self)
        self.grid.addWidget(temperature_query_button,6,2,1,2)
        temperature_query_button.clicked.connect(self.query_temperature)

        range_up_button = QtGui.QPushButton('量程上限',self)
        self.grid.addWidget(range_up_button, 7,0,1,2)
        range_up_button.clicked.connect(self.query_range_up)

        range_down_button = QtGui.QPushButton('量程下限',self)
        self.grid.addWidget(range_down_button, 7,2,1,2)
        range_down_button.clicked.connect(self.query_range_down)

        report_up_button = QtGui.QPushButton('报警上限',self)
        self.grid.addWidget(report_up_button, 8,0,1,2)
        report_up_button.clicked.connect(self.query_report_up)

        report_down_button = QtGui.QPushButton('报警下限',self)
        self.grid.addWidget(report_down_button,8,2,1,2)
        report_down_button.clicked.connect(self.query_report_down)

        report_status_button = QtGui.QPushButton('报警状态',self)
        self.grid.addWidget(report_status_button,9,0,1,2)
        report_status_button.clicked.connect(self.query_report_status)

        all_query_button = QtGui.QPushButton('全部查询',self)
        self.grid.addWidget(all_query_button,9,2,1,2)
        all_query_button.clicked.connect(self.query_all)

    '''设置窗体'''
    def setting(self):
        follow_addr_label = QtGui.QLabel('设置从机地址:', self)
        self.grid.addWidget(follow_addr_label, 6,5)
        self.follow_addr_set = QtGui.QLineEdit(self)
        self.grid.addWidget(self.follow_addr_set,6,6,1,3)
        self.follow_addr_set_button = QtGui.QPushButton('确认',self)
        self.grid.addWidget(self.follow_addr_set_button,6,9,1,2)

        self.follow_addr_set_button.clicked.connect(self.set_follow_addr)

        follow_addr_now_label = QtGui.QLabel('当前从机地址:', self)
        self.grid.addWidget(follow_addr_now_label, 6,12)
        self.follow_addr_now = QtGui.QLineEdit(self)
        self.grid.addWidget(self.follow_addr_now,6,13,1,3)

        range_up_label = QtGui.QLabel('设置量程上限:', self)
        self.grid.addWidget(range_up_label,7,5)
        self.range_up_set = QtGui.QLineEdit(self)
        self.grid.addWidget(self.range_up_set,7,6,1,3)
        self.range_up_set_button = QtGui.QPushButton('确认',self)
        self.grid.addWidget(self.range_up_set_button,7,9,1,2)

        self.range_up_set_button.clicked.connect(self.set_range_up)

        follow_addr_now_label = QtGui.QLabel('当前量程上限:', self)
        self.grid.addWidget(follow_addr_now_label,7,12)
        self.range_up_now = QtGui.QLineEdit(self)
        self.grid.addWidget(self.range_up_now,7,13,1,3)

        range_down_label = QtGui.QLabel('设置量程下限:', self)
        self.grid.addWidget(range_down_label,8,5)
        self.range_down_set = QtGui.QLineEdit(self)
        self.grid.addWidget(self.range_down_set,8,6,1,3)
        self.range_down_set_button = QtGui.QPushButton('确认',self)
        self.grid.addWidget(self.range_down_set_button,8,9,1,2)

        self.range_down_set_button.clicked.connect(self.set_range_down)

        follow_addr_now_label = QtGui.QLabel('当前量程下限:', self)
        self.grid.addWidget(follow_addr_now_label,8,12)
        self.range_down_now = QtGui.QLineEdit(self)
        self.grid.addWidget(self.range_down_now,8,13,1,3)

        report_up_label = QtGui.QLabel('设置报警上限:', self)
        self.grid.addWidget(report_up_label,9,5)
        self.report_up_set = QtGui.QLineEdit(self)
        self.grid.addWidget(self.report_up_set,9,6,1,3)
        self.report_up_set_button = QtGui.QPushButton('确认',self)
        self.grid.addWidget(self.report_up_set_button,9,9,1,2)

        self.report_up_set_button.clicked.connect(self.set_report_up)

        follow_addr_now_label = QtGui.QLabel('当前报警上限:', self)
        self.grid.addWidget(follow_addr_now_label,9,12)
        self.report_up_now = QtGui.QLineEdit(self)
        self.grid.addWidget(self.report_up_now,9,13,1,3)

        report_down_label = QtGui.QLabel('设置报警下限:', self)
        self.grid.addWidget(report_down_label,10,5)
        self.report_down_set = QtGui.QLineEdit(self)
        self.grid.addWidget(self.report_down_set,10,6,1,3)
        self.report_down_set_button = QtGui.QPushButton('确认',self)
        self.grid.addWidget(self.report_down_set_button,10,9,1,2)

        self.report_down_set_button.clicked.connect(self.set_report_down)

        follow_addr_now_label = QtGui.QLabel('当前报警下限:', self)
        self.grid.addWidget(follow_addr_now_label,10,12)
        self.report_down_now = QtGui.QLineEdit(self)
        self.grid.addWidget(self.report_down_now,10,13,1,3)

        temperature_now_label = QtGui.QLabel('当前温度:', self)
        self.grid.addWidget(temperature_now_label,11,12)
        self.temperature_now = QtGui.QLineEdit(self)
        self.grid.addWidget(self.temperature_now,11,13,1,3)

        self.report_status_now_label = QtGui.QLabel('报警状态:', self)
        self.grid.addWidget(self.report_status_now_label,11,0)

        clean_data_button = QtGui.QPushButton('清除显示',self)
        self.grid.addWidget(clean_data_button, 11,5,1,2)
        clean_data_button.clicked.connect(self.do_clean_data)


        save_data_button = QtGui.QPushButton('保存数据',self)
        self.grid.addWidget(save_data_button, 11,7,1,2)
        save_data_button.clicked.connect(self.do_save_data)


        start_data_button = QtGui.QPushButton('初始化',self)
        self.grid.addWidget(start_data_button, 11,9,1,2)
        start_data_button.clicked.connect(self.start)
    '''查询功能'''
    def query_addr(self):
        self.func = '00'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()

        self.doPost('query','addr')

    def query_range_up(self):
        self.func = '01'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        self.doPost('query','range_up')

    def query_range_down(self):
        self.func = '02'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        self.doPost('query','range_down')

    def query_report_up(self):
        self.func = '03'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        self.doPost('query','report_up')

    def query_report_down(self):
        self.func = '04'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        self.doPost('query','report_down')

    def query_report_status(self):
        self.func = '06'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        self.doPost('query','report_status')

    def query_temperature(self):
        self.func = '05'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        self.doPost('query','temperature')

    def query_all(self):
        self.func = '07'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        self.doPost('query','all')

    '''串口设置'''
    def set_brud(self,text):
        self.brud_bit_data = int(text)

    def set_serial_num(self,text):
        self.serial_num_data = '/dev/' + text

    def set_check_bit(self,text):
        self.check_bit_data = int(text)

    def set_stop_bit(self,text):
        self.stop_bit_data = int(text)

    def set_data_bit(self,text):
        self.data_bit_data = int(text)

    '''设置'''
    def set_follow_addr(self):
        self.data = self.follow_addr_set.text()
        self.func = '50'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        self.doPost('set','addr')

    def set_range_up(self):
        self.data = self.range_up_set.text()
        self.func = '51'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        # self.lrcvalue = '00'
        self.doPost('set','range_up')

    def set_range_down(self):
        self.data = self.range_down_set.text()
        self.func = '52'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        # self.lrcvalue = '00'
        self.doPost('set','range_down')

    def set_report_up(self):
        self.data = self.report_up_set.text()
        self.func = '53'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        # self.lrcvalue = '00'
        self.doPost('set','report_up')

    def set_report_down(self):
        self.data = self.report_down_set.text()
        self.func = '54'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        # self.lrcvalue = '00'
        self.doPost('set','report_down')


    def count_lrc_value(self):
        send_data = self.addr + self.func + self.datalen + self.data
        count = 0
        for i in send_data:
            count += int(i)
        # count = abs((~count) + 1)
        # if count < 10:
            # count = '0' + str(count)
        # else:
            # count = str(count)
        # return count

        lrc_value =255 - count +1 ;
        a = lrc_value >> 4;
        b = lrc_value & 0x0f;
        if a < 10:
            a += 0x30;
        else:
            a += 87;
        if b < 10:
            b += 0x30;
        else:
            b += 87;
        count = chr(a) + chr(b)
        return count

    def count_data_len(self):
        if len(self.data) < 10:
            data_len = '0' + str(len(self.data))
        else:
            data_len = str(len(self.data))
        return data_len

    '''事件'''
    def do_open_serial(self):
        if self.open_serial_button.text() == '打开串口':
            self.post_data_label.setText(u'正在打开串口···')
            self.ser.baudrate = self.brud_bit_data
            print(self.ser.baudrate)
            self.ser.port = self.serial_num_data
            print(self.ser.port)
            self.ser.timeout = 0.1
            try:
                self.ser.open()
                self.post_data_label.append(u'成功打开串口···')
                self.open_serial_button.setText('关闭串口')
                # sleep(.1)
                # self.ser.write(b'#0000000000@$')
            except serial.SerialException as error:
                error = str(error)
                self.post_data_label.append(u'串口打开失败: \n%s'%error)
        else:
            self.ser.close()
            self.open_serial_button.setText('打开串口')
            self.post_data_label.append(u'串口已关闭···')

    def do_scan_serial(self):
        print(self.serial_num.currentText())
        if os.path.exists(r'/dev/ttyUSB0'):
            self.serial_num.addItem("ttyUSB0")
        if os.path.exists(r'/dev/ttyUSB1'):
            self.serial_num.addItem("ttyUSB1")
        if os.path.exists(r'/dev/ttyUSB2'):
            self.serial_num.addItem("ttyUSB2")
    def do_clean_data(self):
        self.post_data_label.clear()
        self.receive_data_label.clear()

    def do_save_data(self):
        conn = psycopg2.connect(database="ipdb", \
                                user="postgres", \
                                password="qaz123", \
                                host="127.0.0.1", \
                                port="5432")
        c51 = conn.cursor()
        # c51.execute('''CREATE TABLE C51
                # (ID  SERIAL PRIMARY KEY,
                # ADDR         CHAR(10)  NOT NULL,
                # RANGE_UP     CHAR(10)  NOT NULL,
                # RANGE_DOWN   CHAR(10)  NOT NULL,
                # REPORT_UP    CHAR(10)  NOT NULL,
                # REPORT_DOWn  CHAR(10)  NOT NULL,
                # TEMPERATURE  CHAR(10)  NOT NULL);''')
        print(self.follow_addr_now.text())
        addr = self.follow_addr_now.text()
        range_up = self.range_up_now.text()
        range_down = self.range_down_now.text()
        report_up = self.report_up_now.text()
        report_down = self.report_down_now.text()
        temperature = self.temperature_now.text()
        try:
            c51.execute("INSERT INTO C51 (ADDR,RANGE_UP,RANGE_DOWN,REPORT_UP,\
                        REPORT_DOWN,TEMPERATURE) VALUES ('%s','%s','%s',\
                                                        '%s','%s','%s')"\
                %(addr,range_up,range_down,report_up,report_down,temperature));
            conn.commit()
            self.post_data_label.append(u'数据已保存')
        except:
            conn.rollback()
            self.post_data_label.append(u'数据保存失败')
        conn.close()
        print(self.post_data_label.toPlainText())

    def do_open_file(self):
        fname, _ = QtGui.QFileDialog.getOpenFileName(self, 'Open file',
                    '/home',"hex文件(*.hex);;bin文件(*.bin)")
        self.open_file.setText(fname)

    def do_write_file(self):
        self.post_data_label.setText(u'正在烧写···')
        sleep(.5)
        p = subprocess.Popen('python stcflash.py %s --lowbaud %d'\
                             %(self.open_file.text(),self.brud_bit_data),
                             shell=True,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        p.wait()
        self.post_data_label.append(p.stdout.read().decode())
        self.post_data_label.append(u'烧写成功')

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

    '''发送数据'''
    def doPost(self,action,cate):
        t = Thread(target=self.do_send_data,args=[action,cate])
        t.start()
        # self.do_send_data(action,cate)

    def set_send_data(self,func,cate):
        post_data = self.lead + self.addr + func + self.datalen + \
            self.data + self.lrcvalue + self.last
        self.ser.write(post_data.encode('utf-8'))
        sleep(.1)
        receive_byte = self.ser.readall()
        receive_byte = receive_byte.decode("utf-8", "backslashreplace")
        if cate == 'range_up':
            self.range_up_now.setText(receive_byte[9:13])
        elif cate == 'range_down':
            self.range_down_now.setText(receive_byte[9:13])
        elif cate == 'report_up':
            self.report_up_now.setText(receive_byte[9:13])
        elif cate == 'report_down':
            self.report_down_now.setText(receive_byte[9:13])
        else:
            self.report_down_now.setText('hello world')

    def do_send_data(self,action,cate):
        if self.ser.is_open:

            post_data = self.lead + self.addr + self.func + self.datalen + \
                self.data + self.lrcvalue + self.last
            # print(post_data)

            self.post_data_label.append(post_data)
            self.ser.write(post_data.encode('utf-8'))
            # self.ser.reset_input_buffer()
            # self.ser.reset_output_buffer()
            # self.ser.flush()
            sleep(.1)
            receive_byte = self.ser.readall()
            print(len(receive_byte))
            sleep(.2)
            if len(receive_byte) == 0:
                if self.count < 3:
                    self.doPost(action,cate)
                    self.count += 1
                else:
                    self.count = 0
                    self.post_data_label.append(u'数据发送失败')
            else:
                self.count = 0
                # print(receive_byte)
                # self.receive_data_label.append('正在接收数据···')
                receive_byte = receive_byte.decode("utf-8", "backslashreplace")
                self.receive_data_label.append(receive_byte)
                try:
                    addr = receive_byte.split('#')
                    self.addr = addr[1][0:4]
                except:
                    pass
                if action == 'query':
                    if cate == 'addr':
                        self.follow_addr_now.setText(receive_byte[9:13])
                    elif cate == 'range_up':
                        self.range_up_now.setText(receive_byte[9:13])
                    elif cate == 'range_down':
                        self.range_down_now.setText(receive_byte[9:13])
                    elif cate == 'report_up':
                        self.report_up_now.setText(receive_byte[9:13])
                    elif cate == 'report_down':
                        self.report_down_now.setText(receive_byte[9:13])
                    elif cate == 'report_status':
                        if receive_byte[-5] == '6':
                            self.report_status_now_label.setText('报警状态: 正在报警')
                        else:
                            self.report_status_now_label.setText('报警状态: 未报警')
                    elif cate == 'temperature':
                        self.temperature_now.setText(receive_byte[9:16] + '°C')
                    elif cate == 'all':
                        self.follow_addr_now.setText(receive_byte[1:5])
                        self.range_up_now.setText(receive_byte[9:13])
                        self.range_down_now.setText(receive_byte[13:17])
                        self.report_up_now.setText(receive_byte[17:21])
                        self.report_down_now.setText(receive_byte[21:25])
                    else:
                        pass
                elif action == 'set':
                    if cate == 'addr':
                        self.follow_addr_now.setText(receive_byte[1:5])
                    elif cate == 'range_up':
                        func = '01'
                        self.set_send_data(func,'range_up')
                    elif cate == 'range_down':
                        func = '02'
                        self.set_send_data(func,'range_down')
                    elif cate == 'report_up':
                        func = '03'
                        self.set_send_data(func,'report_up')
                    elif cate == 'report_down':
                        func = '04'
                        self.set_send_data(func,'report_down')
                    else:
                        pass
                else:
                    pass

        else:
            self.post_data_label.append(u'串口未打开···')

    def do_input_data(self):
        self.data = self.post_data_input.text()
        self.func = '58'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        self.doPost('query','other')

    def do_cha_input_data(self):
        self.func = '08'
        self.datalen = self.count_data_len()
        self.lrcvalue = self.count_lrc_value()
        self.doPost('query','other')

    def start(self):
        self.lead = '#'
        self.addr = '0000'
        self.func = '00'
        self.data = ''
        self.datalen = '00'
        self.lrcvalue = self.count_lrc_value()
        self.last = '@$'
        self.count = 0

def main():

    app = QtGui.QApplication(sys.argv)
    ex = SerialUpper()
    ex = ex
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

