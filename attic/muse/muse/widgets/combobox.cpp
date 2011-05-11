//=========================================================
//  MusE
//  Linux Music Editor
//    $Id: combobox.cpp,v 1.4 2004/05/06 15:08:07 wschweer Exp $
//  (C) Copyright 2004 Werner Schweer (ws@seh.de)
//=========================================================

#include <qpopupmenu.h>
#include <qcursor.h>
#include "combobox.h"

//---------------------------------------------------------
//   ComboBox
//---------------------------------------------------------

ComboBox::ComboBox(QWidget* parent, const char* name)
   : QLabel(parent, name)
      {
      _currentItem = 0;
      _id = -1;
      list = new QPopupMenu(0, "comboPopup");
      connect(list, SIGNAL(activated(int)), SLOT(activatedIntern(int)));
      setFrameStyle(QFrame::Panel | QFrame::Raised);
      setLineWidth(2);
      }

ComboBox::~ComboBox()
      {
      delete list;
      }

//---------------------------------------------------------
//   mousePressEvent
//---------------------------------------------------------

void ComboBox::mousePressEvent(QMouseEvent*)
      {
      list->exec(QCursor::pos());
      }

//---------------------------------------------------------
//   activated
//---------------------------------------------------------

void ComboBox::activatedIntern(int n)
      {
      _currentItem = n;
      emit activated(n, _id);
      setText(list->text(_currentItem));
      }

//---------------------------------------------------------
//   setCurrentItem
//---------------------------------------------------------

void ComboBox::setCurrentItem(int i)
      {
      _currentItem = i;
      setText(list->text(list->idAt(_currentItem)));
      }

//---------------------------------------------------------
//   insertItem
//---------------------------------------------------------

void ComboBox::insertItem(const QString& s, int id, int idx)
      {
      list->insertItem(s, id, idx);
      }
