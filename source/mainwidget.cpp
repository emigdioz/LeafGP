#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QDebug>

MainWidget::MainWidget(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWidget)
{
	ui->setupUi(this);
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Lato-Light.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Lato-Medium.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Roboto-Light.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Roboto-Medium.ttf");
	ui->tableViewDataSummary->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->listFunctionsTarget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetData, SIGNAL(currentRowChanged(int)), ui->stackedWidgetData, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetSetup, SIGNAL(currentRowChanged(int)), ui->stackedWidgetSetup, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetGP, SIGNAL(currentRowChanged(int)), ui->stackedWidgetGP, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetResults, SIGNAL(currentRowChanged(int)), ui->stackedWidgetResults, SLOT(setCurrentIndex(int)));
	connect(ui->toolBoxData, SIGNAL(currentChanged(int)), ui->stackedWidgetDataAdvanced, SLOT(setCurrentIndex(int)));
	connect(ui->toolBoxSetup, SIGNAL(currentChanged(int)), ui->stackedWidgetSetupAdvanced, SLOT(setCurrentIndex(int)));
	connect(ui->tableViewDataSummary, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showSelectionMenu(const QPoint&)));
	connect(ui->listFunctionsTarget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showDeleteMenu(const QPoint&)));
	connect(ui->selectorWidget, SIGNAL(sendSelectedCoordinates(int,int)), this, SLOT(updateOtherPlots(int,int)));
	QSizePolicy sp_retain = ui->InfoBox->sizePolicy();
	sp_retain.setRetainSizeWhenHidden(true);
	ui->InfoBox->setSizePolicy(sp_retain);
	ui->InfoBox->hide();
	ui->label_30->hide();
	ui->previewPlot->hide();
	ui->label_47->hide();
	ui->pdfPlot->hide();
	ui->label_48->hide();
	ui->featurePlot->hide();
	ui->label_49->hide();
	ui->featureOutputPlot->hide();
	ui->page->setMinimumHeight(90);
	ui->frame_3->resize(150,ui->frame_3->height());
	ui->frame_8->resize(150,ui->frame_8->height());
	QPen pen(QColor(232,236,242,255), 2);
	ui->samplePlot->setGridPen(pen);
	pen.setColor(QColor(255,85,85,255));
	pen.setWidth(3);
	ui->samplePlot->setDataPen(pen);
	ui->samplePlot->setSizePolygon(0.8);
	ui->samplePlot->setLabelsFont(QFont("Roboto",9,QFont::Light));
	ui->samplePlot->setLabelsFontColor(QColor(45,65,102,255));
	ui->tableViewDataSummary->hide();
	tableSummarySelectionType = 1; // 1: Column wise, 2: row wise
	ui->label_50->hide();
	ui->samplePlot->hide();
	QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	policy.setHeightForWidth(true);
	ui->selectorWidget->setSizePolicy(policy);
	ui->selectorWidget->setNormalStateColor(QColor(232,236,242,255));
	ui->selectorWidget->setHoverStateColor(QColor(182,194,214,255));
	ui->selectorWidget->setDiagonalColor(QColor(246,248,250,255));
	ui->selectorWidget->setSelectedStateColor(QColor(148,204,20,255));
	ui->label_51->hide();
	ui->label_52->hide();
	ui->label_53->hide();
	ui->label_54->hide();
	ui->selectorWidget->hide();
	ui->correlationPlot->hide();
	ui->regressionPlot->hide();
	ui->qqPlot->hide();
	ui->treeFunctionsSource->sortByColumn(0,Qt::AscendingOrder);
	ui->circularProgress->setCircular1BackgroundColor(QColor(246,248,250,255));
	ui->circularProgress->setCircular2BackgroundColor(QColor(246,248,250,255));
	ui->circularProgress->setBackgroundColor(QColor(246,248,250,255));
	ui->circularProgress->setCircular1Color(QColor(255,174,0,255));
	ui->circularProgress->setCircular2Color(QColor(188,95,211,255));
	ui->circularProgress->setFont(QFont("Roboto",9,QFont::Light));
	ui->circularProgress->setFontColor(QColor(45,65,102));
	ui->circularProgress->setDoubleProgress(true);
	ui->circularProgress->setProgress1(400);
	ui->circularProgress->setProgress2(800);
	ui->correlationGPPlot->setSizePolicy(policy);

	threadGP = new QThread();
	workerAlgorithm = new workerGP();
	workerAlgorithm->moveToThread(threadGP);
	connect(workerAlgorithm, SIGNAL(workRequested()), threadGP, SLOT(start()));
	connect(threadGP, SIGNAL(started()), workerAlgorithm, SLOT(doWork()));
	connect(workerAlgorithm, SIGNAL(finished()), threadGP, SLOT(quit()), Qt::DirectConnection);
	connect(workerAlgorithm, SIGNAL(sendProgress1(int)), this, SLOT(receivedProgress1(int)));

	timerGP = new QTimer();
	connect(timerGP, SIGNAL(timeout()), this, SLOT(showElapsedTime()));
	connect(workerAlgorithm, SIGNAL(finished()), this, SLOT(algorithmFinished()));

	getInfoOpenCL();
}

MainWidget::~MainWidget()
{
	delete ui;
}

void MainWidget::getInfoOpenCL()
{
  int i, j;
  char* value;
  char* info;
  size_t valueSize;
  size_t infoSize;
  cl_uint platformCount;
  cl_platform_id* platforms;
  cl_uint deviceCount;
  cl_device_id* devices;
  cl_uint maxComputeUnits;
  const char* attributeNames[5] = { "Name", "Vendor",
        "Version", "Profile", "Extensions" };
  const cl_platform_info attributeTypes[5] = { CL_PLATFORM_NAME, CL_PLATFORM_VENDOR,
        CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE, CL_PLATFORM_EXTENSIONS };
  const int attributeCount = sizeof(attributeNames) / sizeof(char*);

  clGetPlatformIDs(0, NULL, &platformCount);
  platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
  clGetPlatformIDs(platformCount, platforms, NULL);

  for (i = 0; i < platformCount; i++)
  {
    // get all devices
    clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
    devices = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
    clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

    // get platform attribute value size
    clGetPlatformInfo(platforms[i], attributeTypes[1], 0, NULL, &infoSize);
    info = (char*) malloc(infoSize);

    // get platform attribute value
    clGetPlatformInfo(platforms[i], attributeTypes[1], infoSize, info, NULL);

    //printf("  %d.%d %-11s: %sn", i+1, j+1, attributeNames[j], info);
    qDebug()<<i+1<<" "<<" "<<attributeNames[1]<<" "<<info;

    QVBoxLayout *vbox1 = new QVBoxLayout;
    QGroupBox *groupBoxFirst = new QGroupBox(info);
    ui->verticalLayout_24->addWidget(groupBoxFirst);
    groupBoxFirst->setLayout(vbox1);

    free(info);

    // for each device print critical attributes
    for (j = 0; j < deviceCount; j++)
    {
      QLabel *hardwareVersionLabel = new QLabel("Hardware version");
      QLabel *hardwareVersionName = new QLabel();

      QLabel *softwareVersionLabel = new QLabel("Software version");
      QLabel *softwareVersionName = new QLabel();

      QLabel *openclVersionLabel = new QLabel("OpenCL C version");
      QLabel *openclVersionName = new QLabel();

      QLabel *parallelUnitsLabel = new QLabel("Parallel compute units");
      QLabel *parallelUnitsName = new QLabel();

      QGridLayout *grid = new QGridLayout;

      // print device name
      clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
      value = (char*) malloc(valueSize);
      clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
      //printf("%d. Device: %sn", j+1, value);
      qDebug()<<j+1<<" Device: "<<value;

      QGroupBox *groupBoxSecond = new QGroupBox(value);
      groupBoxSecond->setStyleSheet("QGroupBox { background-color: rgb(246,248,250); }");
      groupBoxSecond->setCheckable(true);
      groupBoxSecond->setLayout(grid);
      QLabel *mylabel = new QLabel();
      mylabel->setPixmap(QPixmap(":/icons/resources/images/nvidia_logo.png"));
      mylabel->setMaximumHeight(40);
      vbox1->addWidget(mylabel);
      vbox1->addWidget(groupBoxSecond);
      grid->addWidget(hardwareVersionLabel,0,0);
      grid->addWidget(softwareVersionLabel,1,0);
      grid->addWidget(openclVersionLabel,2,0);
      grid->addWidget(parallelUnitsLabel,3,0);

      free(value);

      // print hardware device version
      clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
      value = (char*) malloc(valueSize);
      clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
      //printf(" %d.%d Hardware version: %sn", j+1, 1, value);
      qDebug()<<j+1<<"1 Hardware version: "<<value;
      hardwareVersionName->setText(value);
      grid->addWidget(hardwareVersionName,0,1);
      free(value);

      // print software driver version
      clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
      value = (char*) malloc(valueSize);
      clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
      //printf(" %d.%d Software version: %sn", j+1, 2, value);
      qDebug()<<j+1<<"2 Software version: "<<value;
      softwareVersionName->setText(value);
      grid->addWidget(softwareVersionName,1,1);
      free(value);

      // print c version supported by compiler for device
      clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
      value = (char*) malloc(valueSize);
      clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
      //printf(" %d.%d OpenCL C version: %sn", j+1, 3, value);
      qDebug()<<j+1<<"3 OpenCL C version: "<<value;
      openclVersionName->setText(value);
      grid->addWidget(openclVersionName,2,1);
      free(value);

      // print parallel compute units
      clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                      sizeof(maxComputeUnits), &maxComputeUnits, NULL);
      //printf(" %d.%d Parallel compute units: %dn", j+1, 4, maxComputeUnits);
      qDebug()<<j+1<<"4 Parallel compute units: "<<maxComputeUnits;
      parallelUnitsName->setText(QString::number(maxComputeUnits));
      grid->addWidget(parallelUnitsName,3,1);

    }
    free(devices);

  }
}
