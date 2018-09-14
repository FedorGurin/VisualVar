#include "formlistaerodroms.h"
#include "ui_formlistaerodroms.h"
#include <QMessageBox>

#include <QFile>
#define XML_AERODROMS "aerodroms.xml"

FormListAerodroms::FormListAerodroms(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormListAerodroms)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);

    QHeaderView *qhv=ui->tableWidget->horizontalHeader();
    qhv->setSectionResizeMode(QHeaderView::ResizeToContents);

    domAerodroms.clear();
    openXML(tr("./xml/")+XML_AERODROMS);
}
bool FormListAerodroms::openXML(const QString &fileName)
{
    bool openFile=false;
    QFile file(fileName);

    openFile=file.open(QIODevice::ReadOnly);
    if(openFile==true)
    {
        bool readXML=false;

        readXML=domAerodroms.setContent(&file,true);
        if(readXML==true)
        {
            QDomElement root=domAerodroms.documentElement();
            parseXML(root.toElement());
        }
        else
        {
            QMessageBox::warning(0,tr("Внимание"),
                                 tr("Ошибка в структуре XML файла \n = "))+QString(XML_AERODROMS);
            return false;
        }
    }
    /*else
    {
        QMessageBox::warning(0,tr("Внимание"),
                             tr("Файл не найден = "))+QString(VARIANT_FILE);
    }*/
    return openFile;
}
void FormListAerodroms::parseXML(const QDomElement& element)
{
    QDomElement ele=element.firstChildElement("label");
    while(!ele.isNull())
    {
        addNewAerodrom(ele);
        ele=ele.nextSiblingElement("aerodrom");
    }
}
void FormListAerodroms::addNewAerodrom(const QDomElement& element)
{
    Q_UNUSED(element);
    //AerodromObject* tempObject=new AerodromObject(name,":/svg/label",0);

    //tempLabel->lat=lat;
    //tempLabel->lon=lon;
    //aerodromObjects.push_back(tempLabel);
}
bool FormListAerodroms::event(QEvent *event)
{
    Q_UNUSED(event);
    return false;
}
FormListAerodroms::~FormListAerodroms()
{
    delete ui;
}
