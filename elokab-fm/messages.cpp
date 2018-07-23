#include "messages.h"

Q_GLOBAL_STATIC(Messages, mInstance)
Messages *Messages::instance()
{
   return mInstance();
}

 void Messages::debugMe(int n,int line,QString arg1,QString arg2,QString arg3)
{
    if(!instance()->isDebug()) return;

if(n==0)

        qDebug().noquote() <<"\e[0;37m[+]\e[0;34m"<<line<<"\e[0;36m"<<arg1<<"\e[0;32m"<<arg2<<"\e[0;29m"<<arg3;
else

        qDebug().noquote() <<"\e[0;37m   [-]\e[0;34m"<<line<<"\e[0;36m"<<arg1<<"\e[0;32m"<<arg2<<"\e[0;29m"<<arg3;

}
