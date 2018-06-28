/***************************************************************************
 *   elokab Copyright (C) 2014 AbouZakaria <yahiaui@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef MESSAGES_H
#define MESSAGES_H
#include <QDebug>
class Messages
{
     public:
          Messages(){}

          enum MsgType{
              TOP,
              BEGIN,
              END,
              NORMALE


          };

           //!
          static void showMessage(MsgType type, const QString &slotName ,const QString &msg=QString())
          {
               QString t = "=";

               switch (type) {
                    case TOP:
                          t += QString(25-slotName.size()/2, '=');
                          qDebug()<<t+slotName.toUpper()+t;
                         break;
                    case BEGIN:

                          qDebug()<<"ELOKAB-FM:   BEGIN: "<<slotName;
                         break;
                    case END:
                          qDebug()<<"ELOKAB-FM:   __END: "<<slotName;
                         break;
                    case NORMALE:
                          qDebug()<<"ELOKAB-FM:----------"<<slotName<<msg;
                         break;
                    default:
                         break;
               }
          }

};

#endif // MESSAGES_H
