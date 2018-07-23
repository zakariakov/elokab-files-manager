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

 //Q_GLOBAL_STATIC(Messages, mInstance)
class Messages

{

     public:

          Messages(){mdebug=false;}
         static Messages *instance();

          enum MsgType{
              TOP,
              BEGIN,
              END,
              NORMALE


          };

          void setDebug(bool debug){mdebug=debug;}
          bool isDebug(){return mdebug;}
          static void debugMe(int n,int line,QString arg1,QString arg2=QString(),QString arg3=QString());


          bool mdebug;

};

#endif // MESSAGES_H
