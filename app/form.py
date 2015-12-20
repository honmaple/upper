#*************************************************************************
#   Copyright © 2015 JiangLin. All rights reserved.
#   File Name: form.py
#   Author:JiangLin 
#   Mail:xiyang0807@gmail.com 
#   Created Time: 2015-12-20 18:27:21
#*************************************************************************
#!/usr/bin/env python
# -*- coding=UTF-8 -*-
from flask.ext.wtf import Form
from wtforms import StringField,SubmitField,\
    SelectField,TextAreaField,BooleanField

class EditForm(Form):
    display = TextAreaField('显示')
    input_box = StringField('输入框:')
    serial_num = SelectField('串口号:',
                           choices=[('COM1', 'COM1'), ('COM2', 'COM2'),
                                    ('COM3','COM3')])
    baud_rate  = SelectField('波特率:',
                           choices=[('100', '100'), ('1000', '1000'),
                                    ('10000','10000')])
    data_bit = SelectField('数据位:',
                           choices=[('2', '2'), ('4', '4'),
                                    ('8','8')])
    check_bit = SelectField('校验位:',
                           choices=[('1', '1'), ('2', '2'),
                                    ('3','3')])
    stop_bit = SelectField('停止位:',
                           choices=[('1', '1'), ('2', '2')])
    hex_display = BooleanField('HEX显示', default=False)
    hex_post = BooleanField('HEX发送', default=False)
    post = SubmitField('发送')
    delete = SubmitField('清除')
    expand = SubmitField('扩展')
    open_serial = SubmitField('串口')

