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
