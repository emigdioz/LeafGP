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
	//qRegisterMetaType<gpExperiment>();

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
	connect(ui->listWidgetResultsAdvanced, SIGNAL(currentRowChanged(int)), ui->stackedWidgetResultsAdvanced, SLOT(setCurrentIndex(int)));

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

	resetDefaultGPParameters(); // has to be called after worker object initialization
	ui->label_105->hide();
	ui->lineEdit_6->hide();
	setupQualityPlot();
	setupSizePlot();
	setupBoxplots();
	setupQualityRunPlot();
	setupSizeRunPlot();
	setupPerformancePlot();
	ui->label_115->setPixmap(drawSmallCircle(QColor(148,204,20,255),10));
	ui->label_116->setPixmap(drawSmallCircle(QColor(75,143,185,255),10));
	ui->label_117->setPixmap(drawSmallCircle(QColor(255,174,0,255),10));
	ui->label_118->setPixmap(drawSmallCircle(QColor(255,85,85,255),10));
	ui->label_119->setPixmap(drawSmallCircle(QColor(188,95,211,255),10));
	ui->label_123->setPixmap(drawGradient(100,20));
	ui->label_129->setPixmap(drawGradient(100,20));
	ui->label_142->setPixmap(drawGradient90(20,100));
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
	ui->label_126->hide();
	ui->label_127->hide();
	ui->label_128->hide();
	ui->label_129->hide();
	connect(ui->populationMapRun, SIGNAL(individualIsSelected(int&,int&,int&)), this, SLOT(individualMapSelected(int&,int&,int&)));

	validDataProject = false;
	// ***** Temporal

//	projectFile.insertHeader(0xA0B0C0D0);
//	projectFile.insertVersion(123);

//	section Population;
//	Population.name = "Example";
//	projectFile.insertSection(Population);


//	projectFile.write("test.bin");

//	projectFile.read("test.bin");
//	QList<section> posSections;
//	projectFile.extractSections(posSections);

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

void MainWidget::on_listWidgetResults_currentRowChanged(int currentRow)
{
	if(currentRow == 0 || currentRow == 2)
	{
		ui->listWidgetResults->setStyleSheet("QListWidget { background: rgb(182,194,214);"
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
		 ui->listWidgetResults->setStyleSheet("QListWidget { background: rgb(182,194,214);"
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

void MainWidget::resetDefaultGPParameters()
{
  userExperiment.gpParams.m_verbose = true;
  userExperiment.gpParams.m_print_primitives = false;
  userExperiment.gpParams.m_primitives = "sin,cos,tan,sqrt,exp,+,-,*,/,ephemeral";
  userExperiment.gpParams.m_number_of_generations = 100;
  userExperiment.gpParams.m_population_size = 100;
  userExperiment.gpParams.m_crossover_probability = 0.9;
  userExperiment.gpParams.m_mutation_probability = 0.05;
  userExperiment.gpParams.m_maximum_tree_size = 100;
  userExperiment.gpParams.m_minimum_tree_size = 3;
  userExperiment.gpParams.m_tournament_size = 3;
  userExperiment.gpParams.m_elitism_size = 1;
  userExperiment.gpParams.m_error_tolerance = -1;
  userExperiment.gpParams.m_max_local_size = 0;
  userExperiment.gpParams.m_output_file = "gpocl.out";
  userExperiment.gpParams.m_seed = 0;
  userExperiment.gpParams.m_number_of_runs = 1;
  userExperiment.gpParams.m_trainingRatio = 70;

  workerAlgorithm->gp_parameters = userExperiment.gpParams;

  ui->spinBox->setValue(100); // Population size
  ui->spinBox_6->setValue(100); // Number of generations
  ui->comboBox_2->setCurrentIndex(0); // Initialization type: Full
  ui->spinBox_2->setValue(3); // Minimum tree size
  ui->spinBox_3->setValue(100); // Maximum tree size
  ui->lineEdit->setText("0"); // Random seed
  ui->comboBox_3->setCurrentIndex(1); // Selection type: Tournament
  ui->spinBox_4->setValue(3); // Tournament size
  ui->lineEdit_3->setText("0.9"); // Crossover probability
  ui->lineEdit_4->setText("0.05"); // Mutation probability
  ui->lineEdit_5->setText("0.05"); // Reproduction probability
  ui->spinBox_5->setValue(1); // Number of runs
  ui->horizontalSlider->setValue(70); // Training/Testing ratio
  ui->populationMap->setTotalIndividuals(100);
  ui->populationMap->setTotalGenerations(100);

}


void MainWidget::on_pushButton_13_clicked()
{
  QMessageBox msgBox;
  //validDataProject = true;
  if(validDataProject)
  {
    if(!projectFilename.isEmpty())
    {
      if(ui->checkBox_11->isChecked() || ui->checkBox_12->isChecked() || ui->checkBox_13->isChecked() || ui->checkBox_14->isChecked())
      {
        projectFile.listSections.clear();
        projectFile.insertHeader(0xA0B0C0D0);
        projectFile.insertVersion(501);
        if(ui->checkBox_11->isChecked())
          projectFile.insertPopulation(userExperiment);
        if(ui->checkBox_12->isChecked())
          populateDataForContainer();
        if(ui->checkBox_13->isChecked())
          projectFile.insertSettings(userExperiment);
        if(ui->checkBox_14->isChecked())
          projectFile.insertSummary(userExperiment);
        projectFile.write(projectFilename);
      }
      else
      {
        msgBox.setText("At least one element has to be selected.");
        msgBox.exec();
      }
    }
    else
    {
      msgBox.setText("Please input a file name.");
      msgBox.exec();
    }
  }
  else
  {
    msgBox.setText("No data has been processed.");
    msgBox.exec();
  }
}
