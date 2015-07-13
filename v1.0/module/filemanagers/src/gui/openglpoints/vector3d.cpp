/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGLWidget>
//#include <QMatrix4x4>
#include <glut.h>
#include <qmath.h>
#include <iostream>

#include "vector3d.h"

//#include <qDebug>

const float pi = 3.14159265359;

Vector3D::Vector3D(QObject *parent) : QObject(parent)
{
    //Ȼ���ٳ�ʼ��
    cylinder_obj1= gluNewQuadric();
    cylinder_obj2= gluNewQuadric();
    cylinder_obj3= gluNewQuadric();
    cylinder_obj4= gluNewQuadric();
    hasTail = false;
    // ��ʼ��λ�õ�

    position.x = (qreal)-1;
    position.y = (qreal)-1;
    position.z = (qreal)-1;
    hasPos = false;
    _angleu = -1;
    _anglev = -1;
    _anglew = -1;

    // ��ɫ
    //setColor(Qt::black);
}

Vector3D::Vector3D(QObject *parent, qreal speedPass, qreal scalePass, QColor colorPass, bool hasTailPass): QObject(parent)
{
    // ��Ȼ���ˣ������˸��ƻ���ʲô����취
    //Ȼ���ٳ�ʼ��
    cylinder_obj1= gluNewQuadric();
    cylinder_obj2= gluNewQuadric();
    cylinder_obj3= gluNewQuadric();
    cylinder_obj4= gluNewQuadric();
    hasTail = false;
    // ��ʼ��λ�õ�

    position.x = (qreal)-1;
    position.y = (qreal)-1;
    position.z = (qreal)-1;
    hasPos = false;
    _angleu = -1;
    _anglev = -1;
    _anglew = -1;

    _speed = speedPass;
    _scale = scalePass;
    hasTail = hasTailPass;

    setColor(colorPass);
}

Vector3D::~Vector3D()
{
    gluDeleteQuadric(cylinder_obj1);
    gluDeleteQuadric(cylinder_obj2);
    gluDeleteQuadric(cylinder_obj3);
    gluDeleteQuadric(cylinder_obj4);

}

void Vector3D::setVector3D(qreal speedPass, qreal scalePass, QColor colorPass, bool hasTailPass)
{
    _speed = speedPass;
    _scale = scalePass;
    hasTail = hasTailPass;
    setColor(colorPass);

}

void Vector3D::isTail(bool hasTailPass)
{
    hasTail = hasTailPass;
}

void Vector3D::setColor(QColor c)
{
    using namespace std;
    //std::cout << c.redF();
    colorVec[0] = c.redF();
    colorVec[1] = c.greenF();
    colorVec[2] = c.blueF();
    colorVec[3] = c.alphaF();
}

/*void Vector3D::setRotation(qreal angle)
{

}*/

void Vector3D::setRotation(qreal uPass, qreal vPass, qreal wPass)
{
    _angleu = uPass;
    _anglev = vPass;
    _anglew = wPass;
}

void Vector3D::setVisible(bool visible)
{

}

Point3DGL Vector3D::pos()
{
    return position;
}

void Vector3D::setPos(Point3DGL point3d)
{
    position = point3d;
    hasPos = true;
}

void Vector3D::setPos(qreal x, qreal y, qreal z)
{
    position.x = x;
    position.y = y;
    position.z = z;
    hasPos = true;
}

//void Vector3D::draw(QGLWidget * showWidget) const
void Vector3D::draw() const
{
    //std::cout << "drawvector" << std::endl;

    //qDebug() << "drawvector2\n";

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    //glClearColor(0.0, 0.0, 0.0, 0.0);
    //���õ�ǰ�����ɫ
    //glClear(GL_COLOR_BUFFER_BIT);
    //GL_COLOR_BUFFER_BIT��־ �� glClearColor���õ���ɫ ��� ��ɫ������
    //glColor3f(_colour.red(), _colour.green(), _colour.blue());
    //glColor{34}{b s i f d ub us ui}{v}(....)
     //��Բ����
    //GLfloat mat_amb_diff[] = {_colour.red(),_colour.green(), _colour.blue(), 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorVec);

    glPushMatrix();

    //glRotatef(60,1.0,0.0,0.0);//��y����ת�ĽǶ�
    //glRotatef(30,0.0,1.0,0.0);//��z����ת�ĽǶ�
    //glRotatef(90,0.0,0.0,1.0);//��x����ת�ĽǶ�



    // �ٶ���ʼƽ����x�ᣬ��ת��ͷ
    if (_angleu >= 0 && _anglev >= 0 && _anglew >= 0)
    {
        double angle = atan2(_anglev,_angleu);
        // ��y����ת angle����
        glRotatef(-angle/pi*180.0,0,1,0);
        angle = atan2(_anglew,sqrt(_angleu * _angleu + _anglev * _anglev));
        // ��z����ת angle����
        glRotatef(-angle/pi*180.0,0,0,1);
    }

    // ��������λ��
    if (hasPos)
    {
        glTranslatef(position.x,0,0);
        glTranslatef(0,position.y,0);
        glTranslatef(0,0,position.z);
    }

    qreal _length;
    _length = _scale * _speed;

    glTranslatef(0.0,0.0, - _length);
    //glPushMatrix();

    if (!hasTail)
    {
        qreal _width;

        _width = 0.15 * _length;

        // ����ͷԲ׶
        gluCylinder(cylinder_obj1, 0, _width, _length, 100, 50);
        glTranslatef(0.0,0.0,_length);
        // ��ͷԲ��
        gluDisk(cylinder_obj2,0,_width,100,50);

    }
    else
    {
        qreal _headWidth;
        qreal _tailWidth;
        qreal _lineLength;
        qreal _headLength;

        _headWidth = 0.10 * _length;
        _tailWidth = 0.25 * _headWidth;
        _lineLength = 0.75 * _length;
        _headLength = _length - _lineLength;

        // ����ͷԲ׶
        gluCylinder(cylinder_obj1, 0, _headWidth, _headLength, 100, 50);
        glTranslatef(0.0,0.0,_headLength);
        // ��ͷԲ��
        gluDisk(cylinder_obj2,0,_headWidth,100,50);
        // ��βԲ��
        gluCylinder(cylinder_obj3, _tailWidth, _tailWidth, _lineLength, 200, 300);
        // ��βԲ��
        glTranslatef(0.0,0.0,_lineLength);
        gluDisk(cylinder_obj4,0,_tailWidth,100,50);
        glTranslatef(0.0,0.0,-_length);

    }

    //glPopMatrix();

    glPopMatrix();

    glFlush();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

}
//! [4]
