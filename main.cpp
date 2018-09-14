#include "./VisualVar/mainwindow.h"
#include "./globalFunc/gl_func.h"

#include <QTranslator>
#include <QTextCodec>
#include <QSplashScreen>
#include <QResource>



int main(int argc,char *argv[])
{
    QApplication app(argc,argv);    
    app.setQuitOnLastWindowClosed(true);

    //! загрузка ресурсов
    QResource::registerResource("/res.rcc");

    //! имя файла с настройками
    nameMainFile="xml/setting.xml";
    if(argc>1)
        nameMainFile="xml/"+QString(argv[1]);

    //! задание кодека
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    //! создание главной формы
    MainWindowVisVar *mainForm=new MainWindowVisVar(0);

    //! задать активное окно
    app.setActiveWindow(mainForm);

    //! показать главную форму
    mainForm->show();

    //! запуск основного цикла обработки
    return app.exec();
}
