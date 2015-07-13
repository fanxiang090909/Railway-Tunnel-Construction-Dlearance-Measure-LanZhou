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

#ifndef VECTOR_THREEDIM_H
#define VECTOR_THREEDIM_H

#include <QObject>
#include <QColor>
#include <QGLWidget>
#include <glut.h>

#include "point3d.h"

class Vector3D : public QObject
{
public:
    Vector3D(QObject *parent);
    Vector3D(QObject *parent, qreal speedPass, qreal scalePass, QColor colorPass, bool hasTailPass);
    ~Vector3D();
    void setVector3D(qreal speedPass, qreal scalePass, QColor colorPass, bool hasTailPass);
    void isTail(bool hasTailPass);
    void setColor(QColor c);

    void setVisible(bool visible);

    Point3DGL pos();
    void setPos(Point3DGL point3d);
    void setPos(qreal x, qreal y, qreal z);

    void setRotation(qreal uPass, qreal vPass, qreal wPass);

    //void draw(QGLWidget * showWidget) const;
    void draw() const;

private:
    // properties used to draw 3d vector arrow using opengl
    qreal _speed;
    qreal _scale;
    bool hasTail;
    // location, position
    Point3DGL position;
    bool hasPos;
    // angle, rotation
    qreal _angleu;
    qreal _anglev;
    qreal _anglew;

    // color
    GLfloat colorVec[4];

    // cylinder patches of the 3d vecor arrow
    GLUquadricObj *cylinder_obj1;
    GLUquadricObj *cylinder_obj2;
    GLUquadricObj *cylinder_obj3;
    GLUquadricObj *cylinder_obj4;

};

#endif // VECTOR#D_H
