#*************************************************************************
#   Copyright © 2015 JiangLin. All rights reserved.
#   File Name: views.py
#   Author:JiangLin 
#   Mail:xiyang0807@gmail.com 
#   Created Time: 2015-12-20 18:17:57
#*************************************************************************
#!/usr/bin/env python
# -*- coding=UTF-8 -*-
from app import app
from flask import render_template
from .form import EditForm


@app.route('/')
@app.route('/index')
def index():
    form = EditForm()
    return render_template('index.html',
                           form = form)

