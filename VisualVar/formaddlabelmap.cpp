#include "formaddlabelmap.h"
#include "ui_formaddlabelmap.h"
#include "../globalFunc/gl_func.h"
#include <QLineEdit>

#define XML_LABEL "labelsVar.xml"
FormAddLabelMap::FormAddLabelMap(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAddLabelMap)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);

    QHeaderView *qhv=ui->tableWidget->horizontalHeader();
    qhv->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->pushButtonClose,    SIGNAL(clicked()),  SLOT(slotCancel()));
    connect(ui->pushButtonAdd,      SIGNAL(clicked()),  SLOT(slotAdd()));
    connect(ui->pushButtonGoto,     SIGNAL(clicked()),  SLOT(slotGoto()));
    connect(ui->pushButtonDelete,   SIGNAL(clicked()),  SLOT(slotDelete()));
    connect(ui->tableWidget,        SIGNAL(cellChanged(int,int)),this, SLOT(slotCellChanged(int,int)));

    domLabels.clear();
    openXML(tr("./xml/")+XML_LABEL);
}
void FormAddLabelMap::slotCellChanged(int row,int column)
{
    if(column==1)
    {
        QTableWidgetItem* item=ui->tableWidget->item(row,column);
        if(labelObjects.size()!=0)
            labelObjects[row-1]->setName(item->text());
    }
}
void FormAddLabelMap::addNewLabel(QString name,double lat,double lon)
{
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,
                             new QTableWidgetItem(name));
    QLineEdit *lineEdit_lat=new QLineEdit();
    lineEdit_lat->setInputMask(tr("000°00'00.000000\""));
    lineEdit_lat->setText(geo_double_to_string(lat));

    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1,1,
                             lineEdit_lat);

    QLineEdit *lineEdit_lon=new QLineEdit();
    lineEdit_lon->setInputMask(tr("000°00'00.000000\""));
    lineEdit_lon->setText(geo_double_to_string(lon));

    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1,2,
                             lineEdit_lon);

    LabelObject* tempLabel=new LabelObject(name,QString(":/svg/label"),0);

    tempLabel->lat=lat;
    tempLabel->lon=lon;
    labelObjects.push_back(tempLabel);
}

void FormAddLabelMap::addNewLabel(double lat,double lon)
{
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    QString name=tr("Новая метка ")+QString::number(ui->tableWidget->rowCount());
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,
                             new QTableWidgetItem(name));
    QLineEdit *lineEdit_lat=new QLineEdit();
    lineEdit_lat->setInputMask(tr("000°00'00.000000\""));
    lineEdit_lat->setText(geo_double_to_string(lat));

    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1,1,
                             lineEdit_lat);

    QLineEdit *lineEdit_lon=new QLineEdit();
    lineEdit_lon->setInputMask(tr("000°00'00.000000\""));
    lineEdit_lon->setText(geo_double_to_string(lon));

    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1,2,
                             lineEdit_lon);

    LabelObject* tempLabel=new LabelObject(name,QString(":/svg/label"),0);

    tempLabel->lat=lat;
    tempLabel->lon=lon;
    labelObjects.push_back(tempLabel);
}

void FormAddLabelMap::saveXML(const QString &fileName)
{
    bool fileOpen=false;
    QFile file(fileName);
    fileOpen=file.open(QIODevice::WriteOnly|QIODevice::Text);

    if(fileOpen==true)
    {
        saveDomLabels.clear();
        QTextStream out(&file);
        rootNode=saveDomLabels.createElement("data");
        saveDomLabels.appendChild(rootNode);

        for(int i=0;i<(ui->tableWidget->rowCount());i++)
        {
            QTableWidgetItem  *item=ui->tableWidget->item(i,0);

            QLineEdit *line_lat=static_cast<QLineEdit* > (ui->tableWidget->cellWidget(i,1));
            QLineEdit *line_lon=static_cast<QLineEdit* > (ui->tableWidget->cellWidget(i,2));

            QDomElement tempNode=saveDomLabels.createElement("label");

            tempNode.setAttribute("name",item->text());
            tempNode.setAttribute("lat",geo_string_to_double(line_lat->text()));
            tempNode.setAttribute("lon",geo_string_to_double(line_lon->text()));

            rootNode.appendChild(tempNode);
        }
        QDomNode xmlNode=saveDomLabels.createProcessingInstruction("xml","version=\"1.0\" encoding=\"windows-1251\"");
        saveDomLabels.insertBefore(xmlNode,saveDomLabels.firstChild());
        saveDomLabels.save(out,4);
    }
}
bool FormAddLabelMap::openXML(const QString &fileName)
{
    bool openFile=false;
    QFile file(fileName);

    openFile=file.open(QIODevice::ReadOnly);
    if(openFile==true)
    {
        bool readXML=false;

        readXML=domLabels.setContent(&file,true);
        if(readXML==true)
        {
            QDomElement root=domLabels.documentElement();
            parseXML(root.toElement());
        }
        else
        {
            QMessageBox::warning(0,tr("Внимание"),
                                 tr("Ошибка в структуре XML файла \n = "))+QString(XML_LABEL);
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
void FormAddLabelMap::parseXML(const QDomElement& element)
{
    QDomElement ele=element.firstChildElement("label");
    while(!ele.isNull())
    {
        double lat=(ele.attribute("lat","0")).toDouble();
        double lon=(ele.attribute("lon","0")).toDouble();

        addNewLabel(ele.attribute("name","0"),lat,lon);

        ele=ele.nextSiblingElement("label");
    }
}

void FormAddLabelMap::slotCancel()
{
    //ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);
    hide();
}
void FormAddLabelMap::slotGoto()
{
    int cRow=ui->tableWidget->currentRow();
    double lat=0,lon=0;

    QLineEdit *line_lat=static_cast<QLineEdit* > (ui->tableWidget->cellWidget(cRow,1));
    QLineEdit *line_lon=static_cast<QLineEdit* > (ui->tableWidget->cellWidget(cRow,2));

    lat=geo_string_to_double(line_lat->text());
    lon=geo_string_to_double(line_lon->text());

    emit gotoLatLon(lat,lon);
}
void FormAddLabelMap::slotAdd()
{
    addNewLabel(0.0,0.0);
}
void FormAddLabelMap::slotDelete()
{
    int cRow=ui->tableWidget->currentRow();
    ui->tableWidget->removeRow(cRow);

    emit deleteLabel(labelObjects[cRow-1]);

    labelObjects.removeAt(cRow);
}
bool FormAddLabelMap::event(QEvent *event)
{
    if(event->type()==QEvent::Close)
    {
        double telat,telon;
        pixelXYToLatLong(5083406,2637371,15,telat,telon);
        double k=2 * M_PI*EarthRadius/mapSize(16);

        mercXYtoLonLat(5083406*k,2637371*k,telon,telat);

        //! сохранить вариант
        saveXML(tr("./xml/")+XML_LABEL);
    }
    return QWidget::event(event);
}
FormAddLabelMap::~FormAddLabelMap()
{
    delete ui;
}
