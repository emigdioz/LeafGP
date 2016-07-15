#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QDebug>

void MainWidget::on_treeFunctionsSource_itemClicked(QTreeWidgetItem *item, int column)
{
	QString functionString;
	int arity;
	QPixmap pix(190,40);
	QPainter painter;
	pix.fill(QColor(246,248,250,255));
	ui->label_69->setText(item->text(column));
	functionOperator.useSTIX();
	functionOperator.set_fontSize(12);
	functionOperatorToLatex(item->text(column),functionString,arity);
	functionOperator.parse(functionString);
	painter.begin(&pix);
	functionOperator.set_fontColor(QColor(45,65,102,255));
	functionOperator.draw(painter, Qt::AlignVCenter, QRectF(0,0,pix.width(), pix.height()), false);
	painter.end();
	ui->label_70->setPixmap(pix);
	ui->label_71->setText(QString::number(arity));
}

void MainWidget::functionOperatorToLatex(QString name, QString &output, int &arity)
{
	if(name.compare("Addition") == 0) { output = "\\it{x_1} + \\it{x_2}"; arity = 2; }
	if(name.compare("Division") == 0) { output = "\\it{x_1} \\div \\it{x_2}"; arity = 2; }
	if(name.compare("Modulo") == 0) { output = "mod(\\it{x_1} \\div \\it{x_2})"; arity = 2; }
	if(name.compare("Multiplication") == 0) { output = "\\it{x_1} \\times \\it{x_2}"; arity = 2; }
	if(name.compare("Subtraction") == 0) { output = "\\it{x_1} - \\it{x_2}"; arity = 2; }
	if(name.compare("If Then Else") == 0) { output = "if\\{ \\it{x_1} \\} then\\{ \\it{x_2} \\} else\\{ \\it{x_3} \\}"; arity = 3; }
	if(name.compare("And") == 0) { output = "\\it{x_1} \\land \\it{x_2}"; arity = 2; }
	if(name.compare("Maximum") == 0) { output = "max( \\it{x_1} , \\it{x_2} )"; arity = 2; }
	if(name.compare("Minimum") == 0) { output = "min( \\it{x_1} , \\it{x_2} )"; arity = 2; }
	if(name.compare("Negation") == 0) { output = "\\neg \\it{x_1}"; arity = 1; }
	if(name.compare("Or") == 0) { output = "\\it{x_1} \\lor \\it{x_2}"; arity = 2; }
	if(name.compare("Xor") == 0) { output = "\\it{x_1} \\oplus \\it{x_2}"; arity = 2; }
	if(name.compare("Absolute") == 0) { output = "\\left| \\it{x_1} \\right|"; arity = 1; }
	if(name.compare("Ceil") == 0) { output = "\\lceil \\it{x_1} \\rceil"; arity = 1; }
	if(name.compare("Cube") == 0) { output = "\\it{x_1}^3"; arity = 1; }
	if(name.compare("Exponential") == 0) { output = "exp( \\it{x_1} )"; arity = 1; }
	if(name.compare("Exponential base-10") == 0) { output = "exp_{10}( \\it{x_1} )"; arity = 1; }
	if(name.compare("Exponential base-2") == 0) { output = "exp_2( \\it{x_1} )"; arity = 1; }
	if(name.compare("Floor") == 0) { output = "\\lfloor \\it{x_1} \\rfloor"; arity = 1; }
	if(name.compare("Logarithm base-10") == 0) { output = "log_{10}( \\it{x_1} )"; arity = 1; }
	if(name.compare("Logarithm base-2") == 0) { output = "log_2( \\it{x_1} )"; arity = 1; }
	if(name.compare("Mean") == 0) { output = "( \\it{x_1} + \\it{x_2} ) \\div 2"; arity = 2; }
	if(name.compare("Natural logarithm") == 0) { output = "ln( \\it{x_1} )"; arity = 1; }
	if(name.compare("Power") == 0) { output = "\\it{x_1}^{\\it{x_2}}"; arity = 2; }
	if(name.compare("Power exponent 4") == 0) { output = "\\it{x_1}^4"; arity = 1; }
	if(name.compare("Root square") == 0) { output = "\\sqrt{ \\it{x_1} }"; arity = 1; }
	if(name.compare("Round") == 0) { output = "\\lfloor \\it{x_1} \\rfloor \\lor \\lceil \\it{x_1} \\rceil"; arity = 1; }
	if(name.compare("Sign") == 0) { output = "- \\it{x_1}"; arity = 1; }
	if(name.compare("Square") == 0) { output = "\\it{x_1}^2"; arity = 1; }
	if(name.compare("Natural logarithm") == 0) { output = "ln( \\it{x_1} )"; arity = 1; }
	if(name.compare("Equal") == 0) { output = "\\it{x_1} = \\it{x_2}"; arity = 2; }
	if(name.compare("Greater") == 0) { output = "\\it{x_1} > \\it{x_2}"; arity = 2; }
	if(name.compare("Greater or equal") == 0) { output = "\\it{x_1} \\geq \\it{x_2}"; arity = 2; }
	if(name.compare("Less") == 0) { output = "\\it{x_1} < \\it{x_2}"; arity = 2; }
	if(name.compare("Less or equal") == 0) { output = "\\it{x_1} \\leq \\it{x_2}"; arity = 2; }
	if(name.compare("Not equal") == 0) { output = "\\it{x_1} \\neq \\it{x_2}"; arity = 2; }
	if(name.compare("Equal") == 0) { output = "\\it{x_1} = \\it{x_2} )"; arity = 2; }
	if(name.compare("Cosine") == 0) { output = "cos( \\it{x_1} )"; arity = 1; }
	if(name.compare("Sine") == 0) { output = "sin( \\it{x_1} )"; arity = 1; }
	if(name.compare("Tangent") == 0) { output = "tan( \\it{x_1} )"; arity = 1; }
}

void MainWidget::on_listFunctionsTarget_itemClicked(QListWidgetItem *item)
{
	QString functionString;
	int arity;
	QPixmap pix(190,40);
	QPainter painter;
	pix.fill(QColor(246,248,250,255));
	ui->label_69->setText(item->text());
	functionOperator.useSTIX();
	functionOperator.set_fontSize(12);
	functionOperatorToLatex(item->text(),functionString,arity);
	functionOperator.parse(functionString);
	painter.begin(&pix);
	functionOperator.set_fontColor(QColor(45,65,102,255));
	functionOperator.draw(painter, Qt::AlignVCenter, QRectF(0,0,pix.width(), pix.height()), false);
	painter.end();
	ui->label_70->setPixmap(pix);
	ui->label_71->setText(QString::number(arity));
}

void MainWidget::showDeleteMenu(const QPoint &pos)
{
	QPoint globalPos = ui->listFunctionsTarget->mapToGlobal(pos);
	QMenu myMenu;
	myMenu.addAction(ui->actionRemove);
	if(ui->listFunctionsTarget->count() != 0)
		myMenu.exec(mapToGlobal(globalPos));
}



void MainWidget::on_actionRemove_triggered()
{
	qDeleteAll(ui->listFunctionsTarget->selectedItems());
}

void MainWidget::on_listTerminalsSource_itemClicked(QListWidgetItem *item)
{
	terminalListNames(item->text());
}

void MainWidget::on_listTerminalsTarget_itemClicked(QListWidgetItem *item)
{
	terminalListNames(item->text());
}

void MainWidget::terminalListNames(QString name)
{
	ui->label_75->setText(name);
	ui->lineEdit_2->setText(name);
}
