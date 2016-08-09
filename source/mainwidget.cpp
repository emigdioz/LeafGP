#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QDebug>

Q_DECLARE_METATYPE(GP::treeStruct);
Q_DECLARE_METATYPE(GP::basicInfo);
Q_DECLARE_METATYPE(GP::popInfo);

MainWidget::MainWidget(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWidget)
{
	ui->setupUi(this);
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Lato-Light.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Lato-Medium.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Roboto-Light.ttf");
	QFontDatabase::addApplicationFont(":/fonts/resources/fonts/Roboto-Medium.ttf");
	qRegisterMetaType<GP::treeStruct>();
	qRegisterMetaType<GP::basicInfo>();
	qRegisterMetaType<GP::popInfo>();

	ui->tableViewDataSummary->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->listFunctionsTarget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetData, SIGNAL(currentRowChanged(int)), ui->stackedWidgetData, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetSetup, SIGNAL(currentRowChanged(int)), ui->stackedWidgetSetup, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetGP, SIGNAL(currentRowChanged(int)), ui->stackedWidgetGP, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetGPAdvanced, SIGNAL(currentRowChanged(int)), ui->stackedWidgetGPAdvanced, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetResults, SIGNAL(currentRowChanged(int)), ui->stackedWidgetResults, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetDataAdvanced, SIGNAL(currentRowChanged(int)), ui->stackedWidgetDataAdvanced, SLOT(setCurrentIndex(int)));
	connect(ui->listWidgetSetupAdvanced, SIGNAL(currentRowChanged(int)), ui->stackedWidgetSetupAdvanced, SLOT(setCurrentIndex(int)));
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
	ui->circularProgress->setDoubleProgress(false);
	ui->circularProgress->setProgress1(0);
	ui->circularProgress->setProgress2(0);
	ui->correlationGPPlot->setSizePolicy(policy);

	threadGP = new QThread();
	workerAlgorithm = new workerGP();
	workerAlgorithm->moveToThread(threadGP);
	connect(workerAlgorithm, SIGNAL(workRequested()), threadGP, SLOT(start()));
	connect(threadGP, SIGNAL(started()), workerAlgorithm, SLOT(doWork()));
	connect(workerAlgorithm, SIGNAL(finished()), threadGP, SLOT(quit()), Qt::DirectConnection);
	//connect(workerAlgorithm, SIGNAL(sendProgress1(int)), this, SLOT(receivedProgress1(int)));
	connect(workerAlgorithm, SIGNAL(sendRunProgress(int,int)), this, SLOT(receivedRunProgress(int,int)));
	connect(workerAlgorithm, SIGNAL(sendSingleTree(GP::treeStruct)), this, SLOT(receivedSingleTree(GP::treeStruct)));
	connect(workerAlgorithm, SIGNAL(sendBasicInfo(GP::basicInfo)), this, SLOT(receivedBasicInfo(GP::basicInfo)));
	connect(workerAlgorithm, SIGNAL(sendPopInfo(GP::popInfo)), this, SLOT(receivedPopInfo(GP::popInfo)));
	timerGP = new QTimer();
	connect(timerGP, SIGNAL(timeout()), this, SLOT(showElapsedTime()));
	connect(workerAlgorithm, SIGNAL(finished()), this, SLOT(algorithmFinished()));
	connect(ui->pushButton_7, SIGNAL(clicked(bool)), ui->populationMap, SLOT(showInheritance()));
	connect(ui->comboBox_7, SIGNAL(currentIndexChanged(int)), ui->populationMap, SLOT(updateStyle(int)));
	ui->correlationGPPlot->hide();
	getInfoOpenCL();

	setDefaultGPParameters(); // has to be called after worker object initialization
	trainingRandomRatio = 70;
	ui->label_105->hide();
	ui->lineEdit_6->hide();
	setupQualityPlot();
	setupSizePlot();
	ui->label_115->setPixmap(drawSmallCircle(QColor(148,204,20,255),10));
	ui->label_116->setPixmap(drawSmallCircle(QColor(75,143,185,255),10));
	ui->label_117->setPixmap(drawSmallCircle(QColor(255,174,0,255),10));
	ui->label_118->setPixmap(drawSmallCircle(QColor(255,85,85,255),10));
	ui->label_119->setPixmap(drawSmallCircle(QColor(188,95,211,255),10));
	ui->label_123->setPixmap(drawGradient(100,20));
	ui->label_120->show();
	ui->label_122->show();
	ui->label_123->show();
	ui->label_124->show();
	ui->label_110->hide();
	ui->label_111->hide();
	ui->label_112->hide();
	ui->label_113->hide();
	ui->label_114->hide();
	ui->label_115->hide();
	ui->label_116->hide();
	ui->label_117->hide();
	ui->label_118->hide();
	ui->label_119->hide();
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
  int type;
  size_t valueSize;
  size_t infoSize;
  cl_uint platformCount;
  cl_platform_id* platforms;
  cl_uint deviceCount;
  cl_device_id* devices;
  cl_uint maxComputeUnits;
  cl_device_type deviceType;
  devicesType.clear();

  const char* attributeNames[5] = { "Name", "Vendor",
        "Version", "Profile", "Extensions" };
  const cl_platform_info attributeTypes[5] = { CL_PLATFORM_NAME, CL_PLATFORM_VENDOR,
        CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE, CL_PLATFORM_EXTENSIONS };
  const int attributeCount = sizeof(attributeNames) / sizeof(char*);

  clGetPlatformIDs(0, NULL, &platformCount);
  platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
  clGetPlatformIDs(platformCount, platforms, NULL);
  int counter = 0;
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

    QVBoxLayout *vbox1 = new QVBoxLayout;
    QGroupBox *groupBoxFirst = new QGroupBox(info);
    ui->verticalLayout_24->addWidget(groupBoxFirst);
    groupBoxFirst->setLayout(vbox1);
    if(info[0] == 'I') type = 1;
    if(info[0] == 'N') type = 2;

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

      QGroupBox *groupBoxSecond = new QGroupBox(value);
      groupBoxSecond->setStyleSheet("QGroupBox { background-color: rgb(246,248,250); }");
      groupBoxSecond->setCheckable(true);
      groupBoxSecond->setChecked(false);
      groupBoxSecond->setLayout(grid);
      groupBoxSecond->setObjectName("deviceParallel"+QString::number(counter++));
      //connect(groupBoxSecond, SIGNAL(toggled(bool)), this, SLOT(parallelDevicesChecked(bool)));

      QLabel *mylabel = new QLabel();
      if(type == 1)
        mylabel->setPixmap(QPixmap(":/icons/resources/images/intel_logo.png"));
      if(type == 2)
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
      hardwareVersionName->setText(value);
      grid->addWidget(hardwareVersionName,0,1);
      free(value);

      // print software driver version
      clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
      value = (char*) malloc(valueSize);
      clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
      softwareVersionName->setText(value);
      grid->addWidget(softwareVersionName,1,1);
      free(value);

      // print c version supported by compiler for device
      clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
      value = (char*) malloc(valueSize);
      clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
       openclVersionName->setText(value);
      grid->addWidget(openclVersionName,2,1);
      free(value);

      // print parallel compute units
      clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                      sizeof(maxComputeUnits), &maxComputeUnits, NULL);
      parallelUnitsName->setText(QString::number(maxComputeUnits));
      grid->addWidget(parallelUnitsName,3,1);

      // device type
      clGetDeviceInfo(devices[j], CL_DEVICE_TYPE,
                      sizeof(deviceType), &deviceType, NULL);
      if(deviceType == CL_DEVICE_TYPE_CPU) devicesType.push_back(0);
      if(deviceType == CL_DEVICE_TYPE_GPU) devicesType.push_back(1);
    }
    free(devices);
  }
  devicesNumber = counter;
  //qDebug()<<counter;

  if(counter > 0)
  {
	  QGroupBox* firstGroup = ui->groupBox_7->findChild<QGroupBox*>("deviceParallel0");
	  firstGroup->setChecked(true);
	  deviceChecked = 0;
	  for(i = 0;i < devicesNumber;i++)
	  {
		 QGroupBox* Group = ui->groupBox_7->findChild<QGroupBox*>("deviceParallel" + QString::number(i));
		 Group->setProperty("index",i);
		 Group->setProperty("type",devicesType.at(i));
		 connect(Group, SIGNAL(toggled(bool)), this, SLOT(parallelDevicesChecked(bool)));
	  }
  }
}

void MainWidget::parallelDevicesChecked(bool t)
{
  QObject *pSender = sender();
  int index = pSender->property("index").toInt();
  int type = pSender->property("type").toInt(); // Device type, 0: CPU, 1: GPU
  deviceChecked = index;
  for(int i = 0;i < devicesNumber;i++)
  {
    QGroupBox* Group = ui->groupBox_7->findChild<QGroupBox*>("deviceParallel" + QString::number(i));
    Group->blockSignals(true);
    if(i != index)
      Group->setChecked(false);
    else
      Group->setChecked(true);
    Group->blockSignals(false);
  }
  workerAlgorithm->engineType = type;
}

void MainWidget::receivedSingleTree(GP::treeStruct data)
{
  selectedTree = data;
  int nLeaves = 0;
  for (int i = 0; i < selectedTree.name.size(); i++) {
      nLeaves = countLeaves(i, nLeaves);
  }
  positionLeaves(0,0,nLeaves);
  positionParents(0,0);
  ui->expressTreeWidget->setData(selectedTree.posX,
                      selectedTree.posY,
                      selectedTree.name,
                      selectedTree.arity);
  ui->expressTreeWidget->setLinkColor(QColor(255,174,0,255));
  ui->expressTreeWidget->setNodeColor(QColor(255,174,0,255));
  ui->expressTreeWidget->setNodeHoverColor(QColor(148,204,20,255));
  ui->label_103->setText(data.syntaxPrefix);
  //qDebug()<<nLeaves;
}

int MainWidget::countLeaves(int index, int count)
{
  if (selectedTree.subTreeSize[index] == 1) {
      selectedTree.index[index] = count;
      count += 1;
  }
  return count;
}

void MainWidget::positionLeaves(int index, int depth, int numberLeaves)
{
  if (selectedTree.subTreeSize[index] == 1) {
      selectedTree.posX[index] = (float)selectedTree.index[index] / (numberLeaves - 1);
      selectedTree.posY[index] = depth;
  }
  unsigned int j = index + 1;
  for (int i = 0; i < selectedTree.arity[index]; i++) {
      positionLeaves(j, depth + 1, numberLeaves);
      j += selectedTree.subTreeSize[j];
  }
}

void MainWidget::positionParents(int index, int depth)
{
  unsigned int j = index + 1;
  for (int i = 0; i < selectedTree.arity[index]; i++) {
      positionParents(j, depth + 1);
      j += selectedTree.subTreeSize[j];
  }
  if (selectedTree.subTreeSize[index] > 1) {
      float x = 0;
      int counter = 0;
      for (int k = 0; k < selectedTree.arity[index]; k++) {
          do // Search for childrens
          {
              if (selectedTree.posY[index + counter] == (depth + 1)) {
                  x += selectedTree.posX[index + counter];
                  counter += 1;
                  break;
              }
              counter += 1;
          } while ((index + counter) < selectedTree.name.size());
      }
      selectedTree.posX[index] = x / selectedTree.arity[index];
      selectedTree.posY[index] = depth;
  }
}

void MainWidget::receivedBasicInfo(GP::basicInfo info)
{
	float yMax = ((info.maxError - info.minError) * 0.1) + info.maxError;
	float yMin = info.minError - ((info.maxError - info.minError) * 0.1);
	float yMaxSize = (info.maxAvgSize * 0.1) + info.maxAvgSize;
	ui->label_88->setText(QString::number(info.currentGeneration));
	ui->label_92->setText(QString::number(info.currentNodesExecutions));
	ui->label_94->setText(QString::number(info.bestTrainError));
	ui->label_96->setText(QString::number(info.bestSize));
	ui->label_98->setText(QString::number(info.avgSize));
	drawCorrelationPlotGP(info.actual,info.expected);
	if(info.currentGeneration == 1)
	{
		ui->qualityPlot->graph(0)->clearData();
		ui->qualityPlot->graph(1)->clearData();
		ui->sizePlot->graph(0)->clearData();
	}
	ui->qualityPlot->graph(0)->addData(info.currentGeneration,info.bestTrainError);
	ui->qualityPlot->graph(1)->addData(info.currentGeneration,info.bestTestError);
	ui->qualityPlot->yAxis->setRange(yMin,yMax);
	ui->qualityPlot->replot();
	ui->sizePlot->graph(0)->addData(info.currentGeneration,info.avgSize);
	ui->sizePlot->yAxis->setRange(-1,yMaxSize);
	ui->sizePlot->replot();
}

void MainWidget::on_checkBox_toggled(bool checked)
{
		ui->groupBox_7->setEnabled(checked);
}

void MainWidget::on_listWidgetDataAdvanced_currentRowChanged(int currentRow)
{
    if(currentRow == 0)
    {
      ui->label_32->show();
      ui->frame_23->show();
    }
    else
    {
      ui->label_32->hide();
      ui->frame_23->hide();
    }
}

void MainWidget::on_listWidgetData_currentRowChanged(int currentRow)
{
    if(currentRow == 0)
    {
    ui->listWidgetData->setStyleSheet("QListWidget { background: rgb(182,194,214);"
      "border: none;"
      "font-family: Lato;"
      "font-weight: 200;"
      "font-size:12pt; }"

      "QListWidget::item { background-image: url(:/icons/resources/images/sidebar2_back_normal2.png);"
      "background-position: center;"
      "width: 138px;"
      "height: 45px;"
      "padding-left: 10px;"
      "color: rgb(45,65,102);}"

    "QListWidget::item:selected { background-image: url(:/icons/resources/images/sidebar2_back_selected2.png);"
      "background-position: center;"
      "color: rgb(0, 0, 0);}"

    "QListWidget::item:hover { background-image: url(:/icons/resources/images/sidebar2_back_hover2.png);"
      "background-position: center;"
      "color: rgb(0, 0, 0);}");
    }
    else
    {
      ui->listWidgetData->setStyleSheet("QListWidget { background: rgb(182,194,214);"
        "border: none;"
        "font-family: Lato;"
        "font-weight: 200;"
        "font-size:12pt; }"

        "QListWidget::item { background-image: url(:/icons/resources/images/sidebar2_back_normal.png);"
        "background-position: center;"
        "width: 138px;"
        "height: 45px;"
        "padding-left: 10px;"
        "color: rgb(45,65,102);}"

      "QListWidget::item:selected { background-image: url(:/icons/resources/images/sidebar2_back_selected.png);"
        "background-position: center;"
        "color: rgb(0, 0, 0);}"

      "QListWidget::item:hover { background-image: url(:/icons/resources/images/sidebar2_back_hover.png);"
        "background-position: center;"
        "color: rgb(0, 0, 0);}");
    }
}

void MainWidget::on_listWidgetSetup_currentRowChanged(int currentRow)
{
    if(currentRow == 0)
    {
      ui->listWidgetSetup->setStyleSheet("QListWidget { background: rgb(182,194,214);"
        "border: none;"
        "font-family: Lato;"
        "font-weight: 200;"
        "font-size:12pt; }"

        "QListWidget::item { background-image: url(:/icons/resources/images/sidebar2_back_normal2.png);"
        "background-position: center;"
        "width: 138px;"
        "height: 45px;"
        "padding-left: 10px;"
        "color: rgb(45,65,102);}"

      "QListWidget::item:selected { background-image: url(:/icons/resources/images/sidebar2_back_selected2.png);"
        "background-position: center;"
        "color: rgb(0, 0, 0);}"

      "QListWidget::item:hover { background-image: url(:/icons/resources/images/sidebar2_back_hover2.png);"
        "background-position: center;"
        "color: rgb(0, 0, 0);}");
      }
      else
      {
        ui->listWidgetSetup->setStyleSheet("QListWidget { background: rgb(182,194,214);"
          "border: none;"
          "font-family: Lato;"
          "font-weight: 200;"
          "font-size:12pt; }"

          "QListWidget::item { background-image: url(:/icons/resources/images/sidebar2_back_normal.png);"
          "background-position: center;"
          "width: 138px;"
          "height: 45px;"
          "padding-left: 10px;"
          "color: rgb(45,65,102);}"

        "QListWidget::item:selected { background-image: url(:/icons/resources/images/sidebar2_back_selected.png);"
          "background-position: center;"
          "color: rgb(0, 0, 0);}"

        "QListWidget::item:hover { background-image: url(:/icons/resources/images/sidebar2_back_hover.png);"
          "background-position: center;"
          "color: rgb(0, 0, 0);}");
      }
}

void MainWidget::setDefaultGPParameters()
{
  workerAlgorithm->gp_parameters.m_verbose = true;
  workerAlgorithm->gp_parameters.m_print_primitives = false;
  workerAlgorithm->gp_parameters.m_primitives = "sin,cos,tan,sqrt,exp,+,-,*,/,ephemeral";
  workerAlgorithm->gp_parameters.m_number_of_generations = 100;
  workerAlgorithm->gp_parameters.m_population_size = 100;
  workerAlgorithm->gp_parameters.m_crossover_probability = 0.9;
  workerAlgorithm->gp_parameters.m_mutation_probability = 0.1;
  workerAlgorithm->gp_parameters.m_maximum_tree_size = 100;
  workerAlgorithm->gp_parameters.m_minimum_tree_size = 1;
  workerAlgorithm->gp_parameters.m_tournament_size = 3;
  //m_device = DEVICE_GPU_FPI;
  //m_device = DEVICE_CPU;
  workerAlgorithm->gp_parameters.m_elitism_size = 1;
  workerAlgorithm->gp_parameters.m_error_tolerance = -1;
  workerAlgorithm->gp_parameters.m_max_local_size = 0;
  workerAlgorithm->gp_parameters.m_output_file = "gpocl.out";
  workerAlgorithm->gp_parameters.m_seed = 0;
  workerAlgorithm->gp_parameters.m_number_of_runs = 1;
}

void MainWidget::on_spinBox_5_valueChanged(int arg1)
{
  workerAlgorithm->gp_parameters.m_number_of_runs = arg1;
  if(arg1 == 1)
  {
    ui->circularProgress->setDoubleProgress(false);
    ui->label_105->hide();
    ui->lineEdit_6->hide();
  }
  else
  {
    ui->circularProgress->setDoubleProgress(true);
    ui->label_105->show();
    ui->lineEdit_6->show();
  }
}

void MainWidget::on_horizontalSlider_valueChanged(int value)
{
    trainingRandomRatio = value;
    ui->label_107->setText("Training/testing ratio (" + QString::number(value) + "%/" + QString::number(100-value) + "%)");
    workerAlgorithm->trainingRatio = value;
}

void MainWidget::on_listWidgetGP_currentRowChanged(int currentRow)
{
	if(currentRow == 0)
	{
	  ui->listWidgetGP->setStyleSheet("QListWidget { background: rgb(182,194,214);"
		 "border: none;"
		 "font-family: Lato;"
		 "font-weight: 200;"
		 "font-size:12pt; }"

		 "QListWidget::item { background-image: url(:/icons/resources/images/sidebar2_back_normal2.png);"
		 "background-position: center;"
		 "width: 138px;"
		 "height: 45px;"
		 "padding-left: 10px;"
		 "color: rgb(45,65,102);}"

	  "QListWidget::item:selected { background-image: url(:/icons/resources/images/sidebar2_back_selected2.png);"
		 "background-position: center;"
		 "color: rgb(0, 0, 0);}"

	  "QListWidget::item:hover { background-image: url(:/icons/resources/images/sidebar2_back_hover2.png);"
		 "background-position: center;"
		 "color: rgb(0, 0, 0);}");
	  }
	  else
	  {
		 ui->listWidgetGP->setStyleSheet("QListWidget { background: rgb(182,194,214);"
			"border: none;"
			"font-family: Lato;"
			"font-weight: 200;"
			"font-size:12pt; }"

			"QListWidget::item { background-image: url(:/icons/resources/images/sidebar2_back_normal.png);"
			"background-position: center;"
			"width: 138px;"
			"height: 45px;"
			"padding-left: 10px;"
			"color: rgb(45,65,102);}"

		 "QListWidget::item:selected { background-image: url(:/icons/resources/images/sidebar2_back_selected.png);"
			"background-position: center;"
			"color: rgb(0, 0, 0);}"

		 "QListWidget::item:hover { background-image: url(:/icons/resources/images/sidebar2_back_hover.png);"
			"background-position: center;"
			"color: rgb(0, 0, 0);}");
	  }
}

void MainWidget::receivedPopInfo(GP::popInfo info)
{
	if(info.currentGen == 1)
		ui->populationMap->clearPopulation();
	ui->populationMap->addSingleGeneration(info);
}

QPixmap MainWidget::drawSmallCircle(QColor color,int size)
{
	QPixmap pixmap(size,size);
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(0,0,10,10,QBrush(QColor(246,248,250,255)));
	painter.setPen(Qt::NoPen);
	painter.setBrush(color);
	painter.drawEllipse(0,0,size,size);
	painter.end();
	return pixmap;
}

QPixmap MainWidget::drawGradient(int width, int height)
{
	QLinearGradient linearGradient(0,0,width,0);
	linearGradient.setColorAt(0, QColor::fromHslF(0,0.95,0.5));
	linearGradient.setColorAt(0.125, QColor::fromHslF(0.1,0.95,0.5));
	linearGradient.setColorAt(0.25, QColor::fromHslF(0.2,0.95,0.5));
	linearGradient.setColorAt(0.375, QColor::fromHslF(0.3,0.95,0.5));
	linearGradient.setColorAt(0.5, QColor::fromHslF(0.4,0.95,0.5));
	linearGradient.setColorAt(0.625, QColor::fromHslF(0.5,0.95,0.5));
	linearGradient.setColorAt(0.75, QColor::fromHslF(0.6,0.95,0.5));
	linearGradient.setColorAt(0.875, QColor::fromHslF(0.7,0.95,0.5));
	linearGradient.setColorAt(1, QColor::fromHslF(0.8,0.95,0.5));
	QPixmap pixmap(width,height);
	QPainter painter(&pixmap);
	painter.fillRect(0,0,width,height,linearGradient);
	painter.end();
	return pixmap;
}

void MainWidget::on_comboBox_7_currentIndexChanged(int index)
{
  switch (index) {
    case 0:
      ui->label_120->show();
      ui->label_122->show();
      ui->label_123->show();
      ui->label_124->show();
      ui->label_110->hide();
      ui->label_111->hide();
      ui->label_112->hide();
      ui->label_113->hide();
      ui->label_114->hide();
      ui->label_115->hide();
      ui->label_116->hide();
      ui->label_117->hide();
      ui->label_118->hide();
      ui->label_119->hide();
      break;
    case 1:
      ui->label_120->hide();
      ui->label_122->hide();
      ui->label_123->hide();
      ui->label_124->hide();
      ui->label_110->show();
      ui->label_111->show();
      ui->label_112->show();
      ui->label_113->show();
      ui->label_114->show();
      ui->label_115->show();
      ui->label_116->show();
      ui->label_117->show();
      ui->label_118->show();
      ui->label_119->show();
      break;
  }
}
