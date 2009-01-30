/***************************************************************************
    File                 : ScriptEdit.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, knut.franke*gmx.de
    Description          : Editor widget for scripting code

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

/**
  * Mantid - The code here differs greatly from the QtiPlot class of the same name and most qtiplot
  * changes will not need to be merged here
  */ 
#include "ScriptEdit.h"
#include "Note.h"
#include "pixmaps.h"

#include <QAction>
#include <QMenu>
#include <QPrintDialog>
#include <QPrinter>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QApplication>
#include <Qsci/qscilexer.h>
#include <Qsci/qsciprinter.h>
#include "ScriptWindow.h"
#include <QPrinterInfo>

#include <iostream>

ScriptEdit::ScriptEdit(ScriptingEnv *env, QWidget *parent, const char *name)
  : QsciScintilla(parent), scripted(env), d_error(false)//Mantid
{
	myScript = scriptEnv->newScript("", this, name);
	connect(myScript, SIGNAL(error(const QString&,const QString&,int)), this, SLOT(insertErrorMsg(const QString&)));
	connect(myScript, SIGNAL(print(const QString&)), this, SLOT(scriptPrint(const QString&)));

	connect(this, SIGNAL(textChanged()), this, SLOT(undoredoAvailable()));

	//QScintilla specific stuff
	codeLexer = env->scriptCodeLexer();
	setLexer(codeLexer);
	setAutoIndent(true);
	setMarginLineNumbers(1,true);
	setMarginWidth(1, 40);
	
	//Define line marker to be a right arrow
	m_iCodeMarkerHandle = markerDefine(QsciScintilla::RightArrow);
	
	scriptsDirPath = qApp->applicationDirPath();

	actionExecute = new QAction(tr("E&xecute"), this);
	connect(actionExecute, SIGNAL(activated()), this, SLOT(execute()));

	actionExecuteAll = new QAction(tr("Execute &All"), this);
	connect(actionExecuteAll, SIGNAL(activated()), this, SLOT(executeAll()));

	actionEval = new QAction(tr("&Evaluate Expression"), this);
	connect(actionEval, SIGNAL(activated()), this, SLOT(evaluate()));

	functionsMenu = new QMenu(this);
	Q_CHECK_PTR(functionsMenu);
	connect(functionsMenu, SIGNAL(triggered(QAction *)), this, SLOT(insertFunction(QAction *)));
}

ScriptEdit::~ScriptEdit()
{
    if( codeLexer )
      delete codeLexer;
}

void ScriptEdit::customEvent(QEvent *e)
{
  if (e->type() == SCRIPTING_CHANGE_EVENT)
    {
      ScriptingChangeEvent* event = (ScriptingChangeEvent*)e;
      delete myScript;
      scriptEnv = event->scriptingEnv();
      myScript = scriptEnv->newScript("", this, name());
      connect(myScript, SIGNAL(error(const QString&,const QString&,int)), this, SLOT(insertErrorMsg(const QString&)));
      connect(myScript, SIGNAL(print(const QString&)), this, SLOT(scriptPrint(const QString&)));

      //Get new code lexer
      if( codeLexer ) delete codeLexer;
      codeLexer = scriptEnv->scriptCodeLexer();
      setLexer(codeLexer);
      setAutoIndent(true);
    }
}

void ScriptEdit::contextMenuEvent(QContextMenuEvent *e)
{
  QMenu menu(this);

  //First the options to open/save

  QAction *action = new QAction(tr("&Open..."), this);
  connect(action, SIGNAL(activated()), this, SLOT(importASCII()));
  menu.addAction(action);

  action = new QAction(tr("&Save..."), this);
  connect(action, SIGNAL(activated()), this, SLOT(exportASCII()));
  menu.addAction(action);

  if( !text().isEmpty() )
  {
    action = new QAction(QPixmap(fileprint_xpm), "Print", this);
    connect(action, SIGNAL(activated()), this, SLOT(print()));
    menu.addAction(action);
  }

  menu.insertSeparator();
  //Now the running actions (the shortcuts are assigned to the ScriptWindow actions)
  menu.addAction(actionExecute);
  menu.addAction(actionExecuteAll);  
  menu.addAction(actionEval);  

  if (parent()->isA("Note")){
    Note *sp = (Note*) parent();
    action = new QAction(tr("Auto&exec"), &menu);
    action->setToggleAction(true);
    action->setOn(sp->autoexec());
    connect(action, SIGNAL(toggled(bool)), sp, SLOT(setAutoexec(bool)));
    menu.addAction(action);
  }
  
  if( scriptEnv->scriptingLanguage() == "muParser" )
  {
    functionsMenu->clear();
    functionsMenu->setTearOffEnabled(true);
    QStringList flist = scriptEnv->mathFunctions();
    QMenu *submenu=NULL;
    for (int i=0; i<flist.size(); i++)
      {
	QAction *newAction;
	QString menupart;
	// group by text before "_" (would make sense if we renamed several functions...)
	/*if (flist[i].contains("_") || (i<flist.size()-1 && flist[i+1].split("_")[0]==flist[i]))
	  menupart = flist[i].split("_")[0];
	  else
	  menupart = "";*/
	// group by first letter, avoiding submenus with only one entry
	if ((i==0 || flist[i-1][0] != flist[i][0]) && (i==flist.size()-1 || flist[i+1][0] != flist[i][0]))
	  menupart = "";
	else
	  menupart = flist[i].left(1);
	if (!menupart.isEmpty()) {
	  if (!submenu || menupart != submenu->title())
	    submenu = functionsMenu->addMenu(menupart);
	  newAction = submenu->addAction(flist[i]);
	} else
	  newAction = functionsMenu->addAction(flist[i]);
	newAction->setData(i);
	newAction->setWhatsThis(scriptEnv->mathFunctionDoc(flist[i]));
      }
    functionsMenu->setTitle(tr("&Functions"));
    menu.addMenu(functionsMenu);
  }
  menu.exec(e->globalPos());
}

void ScriptEdit::insertErrorMsg(const QString &message)
{
  if( message.isEmpty() ) return;
  setMarkerBackgroundColor(QColor("red"), m_iCodeMarkerHandle);
  if( message.contains("SystemExit") )
  {
    //Alter it to something more meaningful
    emit outputError(QString("Information: Script execution has been cancelled."));
  }
  else
  {
    emit outputError(message);
  }
  setEditorActive(true);
}

void ScriptEdit::scriptPrint(const QString &text)
{
  if( text.isEmpty() || text.contains(QRegExp("^\\s$")) || text == QString('\n') ) return;

  //If the text contains the current line number, mark the line instead of 
  //outputting
  if( text.contains("LINENUMBER:") ) 
  {
    int lineNumber = text.section(':',1, 1).toInt();
    markerDeleteAll();
    markerAdd(lineNumber - 1, m_iCodeMarkerHandle);
  }
  else
  {
    emit outputMessage(text);
  }
}

void ScriptEdit::insertFunction(const QString &fname)
{
  append(fname);
}

void ScriptEdit::insertFunction(QAction *action)
{
  insertFunction(scriptEnv->mathFunctions()[action->data().toInt()]);
}

int ScriptEdit::lineNumber() const
{
  int linenumber(0),index(0);
  getCursorPosition(&linenumber, &index);
  return linenumber;
}

void ScriptEdit::execute()
{
  QString code = selectedText().remove("\r");
  if( code.isEmpty() )
  {
    executeAll();
    return;
  }
  int lineFrom(0), indexFrom(0), lineTo(0), indexTo(0);
  //Qscintilla function
  getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
  scriptEnv->setFirstLineNumber(lineFrom);

  //Disable editor
  setEditorActive(false);

  //If we get here everything is successful
  setMarkerBackgroundColor(QColor("lightgreen"), m_iCodeMarkerHandle);

  //Execute the code 
  myScript->setCode(code);
  myScript->exec();
  
  //Reenable editor
  setEditorActive(true);
}

void ScriptEdit::executeAll()
{
  if( text().isEmpty() ) return;
  scriptEnv->setFirstLineNumber(0);

  //Disable editor
  setEditorActive(false);

  //If we get here everything is successful
  setMarkerBackgroundColor(QColor("lightgreen"), m_iCodeMarkerHandle);
    
  //Execute the code 
  myScript->setCode(text().remove('\r'));
  myScript->exec();

  //Reenable editor
  setEditorActive(true);
}

void ScriptEdit::evaluate()
{
  QString code = selectedText().remove("\r");
  if( code.isEmpty() )
  {
    code = text(lineNumber()).remove("\r");
    myScript->setName(code);
    scriptEnv->setFirstLineNumber(lineNumber());
  }
  else
  {
    int lineFrom(0), indexFrom(0), lineTo(0), indexTo(0);
    //Qscintilla function
    getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
    scriptEnv->setFirstLineNumber(lineFrom);
  }
  if( code.isEmpty() ) return;
 
  myScript->setCode(code);
  QVariant res = myScript->eval();

  if (res.isValid())
    if (!res.isNull() && res.canConvert(QVariant::String)){
      QString strVal = res.toString();
      strVal.replace("\n", "\n#> ");
	    
      scriptPrint("\n");
      if (!strVal.isEmpty())
	scriptPrint("#> "+strVal+"\n");
    }

}

//------------------------------------
void ScriptEdit::setEditorActive(bool toggle)
{
  ScriptWindow *scriptWindow = static_cast<ScriptWindow*>(parent());
  if( scriptWindow )
  {
    scriptWindow->setEditEnabled(toggle);
  }
}

void ScriptEdit::setExecuteActionsEnabled(bool toggle)
{
  actionExecute->setEnabled(toggle);
  actionExecuteAll->setEnabled(toggle);
  actionEval->setEnabled(toggle);
}

void ScriptEdit::exportPDF(const QString&)
{
//   QTextDocument* doc = new QTextDocument(text());
//   QPrinter printer;
//   printer.setColorMode(QPrinter::GrayScale);
//   printer.setCreator("MantidPlot");
//   printer.setOutputFormat(QPrinter::PdfFormat);
//   printer.setOutputFileName(fileName);
//   doc->print(&printer);
}

void ScriptEdit::print()
{
  QsciPrinter printer(QPrinter::HighResolution);
  printer.setColorMode(QPrinter::GrayScale);
  printer.setOutputFormat(QPrinter::PostScriptFormat);
  QPrintDialog printDialog(&printer);
  printDialog.setWindowTitle("MantidPlot - Print Script");
  if (printDialog.exec() == QDialog::Accepted) 
  {
    QTextDocument* doc = new QTextDocument(text());
    doc->print(&printer);
    delete doc;
  }
}

QString ScriptEdit::importASCII(const QString &filename)
{
        if( !text().isEmpty() )
	{
	  QMessageBox msgBox(this);
	  msgBox.setWindowTitle(tr("MantidPlot - Save To File..."));
	  msgBox.setText("The script window contents will be cleared.");
	  msgBox.setInformativeText("Do you want to save your changes?");
	  msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	  msgBox.setDefaultButton(QMessageBox::Save);
	  int ret = msgBox.exec();
	  switch (ret) {
	  case QMessageBox::Save:
	    exportASCII(QString(""));
	    break;
	  case QMessageBox::Cancel:
	    return QString("");
	    break;
	  case QMessageBox::Discard:
	    break;
	  default:
	    // should never be reached
	    break;
	  }
	}

	QString filter = scriptEnv->fileFilter();
	filter += tr("Text") + " (*.txt *.TXT);;";
	filter += tr("All Files")+" (*)";

	QString f;
	if (filename.isEmpty())
		f = QFileDialog::getOpenFileName(this, tr("MantidPlot - Open a script from a file"), scriptsDirPath, filter);
	else
		f = filename;
	if (f.isEmpty()) return QString::null;
		
	QFile file(f);
	if (!file.open(IO_ReadOnly)){
		QMessageBox::critical(this, tr("MantidPlot - Error Opening File"), tr("Could not open file \"%1\" for reading.").arg(f));
		return QString::null;
	}
	
	QFileInfo fi(f);
	if (scriptsDirPath != fi.absolutePath()){
		scriptsDirPath = fi.absolutePath();
		emit dirPathChanged(scriptsDirPath);
	}
	
  clear();
  //This doesn't work for older versions of Qt for some reason
  //	read(&file);
  QTextStream s(&file);
  s.setEncoding(QTextStream::UnicodeUTF8);
  while (!s.atEnd())
    append(s.readLine()+"\n");

  file.close();
  //Set keyboard focus to edit window
  setFocus();
  return f;
}

QString ScriptEdit::exportASCII(const QString &filename)
{
	QString filter = scriptEnv->fileFilter();
	filter += tr("Text") + " (*.txt *.TXT);;";
	filter += tr("All Files")+" (*)";

	QString selectedFilter;
	QString fn;
	if (filename.isEmpty())
		fn = QFileDialog::getSaveFileName(this, tr("Save Text to File"), scriptsDirPath, filter, &selectedFilter);
	else
		fn = filename;

	if ( !fn.isEmpty() ){
		QFileInfo fi(fn);
		scriptsDirPath = fi.absolutePath();
		
		QString baseName = fi.fileName();
		if (!baseName.contains(".")){
			if (selectedFilter.contains(".txt"))
				fn.append(".txt");
			else if (selectedFilter.contains(".py"))
				fn.append(".py");
		}

		QFile f(fn);
		if (!f.open(IO_WriteOnly)){
			QMessageBox::critical(0, tr("MantidPlot - File Save Error"),
						tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that you have the right to write to this location!").arg(fn));
			return QString::null;
		}

		QTextStream t( &f );
		t.setEncoding(QTextStream::UnicodeUTF8);
		t << text();
		f.close();
	}
	return fn;
}

void ScriptEdit::undoredoAvailable()
{
  emit undoAvailable(isUndoAvailable());
  emit redoAvailable(isRedoAvailable());
}


void ScriptEdit::setDirPath(const QString& path)
{
	QFileInfo fi(path);
	if (!fi.exists() || !fi.isDir())
		return;
	
	scriptsDirPath = path;
}

