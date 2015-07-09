//--
// Modified by Alex Bradbury (asb@asbradbury.org) to demonstrate embedding SDL2+OpenGL
// in a Qt application, originally part of Sonic Pi.
//
// This file is part of Sonic Pi: http://sonic-pi.net
// Full project source: https://github.com/samaaron/sonic-pi
// License: https://github.com/samaaron/sonic-pi/blob/master/LICENSE.md
//
// Copyright 2013, 2014 by Sam Aaron (http://sam.aaron.name).
// All rights reserved.
//
// Permission is granted for use, copying, modification, distribution,
// and distribution of modified versions of this work as long as this
// notice is included.
//++

// Standard stuff
#include <iostream>
#include <math.h>
#include <sstream>
#include <assert.h>

// SDL stuff
#include <SDL2/SDL.h>
#include <GL/glu.h>

// Qt stuff
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QDockWidget>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QTextEdit>
#include <QToolBar>
#include <QProcess>
#include <QFont>
#include <QTabWidget>
#include <QString>
#include <QStringList>
#include <QSplitter>
#include <QTextStream>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QScrollArea>
#include <QScrollBar>

// QScintilla stuff
#include <Qsci/qsciapis.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerpython.h>

#include "mainwindow.h"

#include "h_tab_bar.hpp"

// Need to access the SDL_Window internals to set the opengl flag
struct SDL_Window {
  const void *magic;
  Uint32 id;
  char *title;
  SDL_Surface *icon;
  int x, y;
  int w, h;
  int min_w, min_h;
  int max_w, max_h;
  Uint32 flags;
};
typedef struct SDL_Window SDL_Window;

MainWindow::MainWindow() {
  this->setUnifiedTitleAndToolBarOnMac(true);

  // create workspace with tabs
  tabs = new QHTabWidget();
  tabs->setTabsClosable(false);
  tabs->setMovable(false);
  tabs->setTabPosition(QTabWidget::East);

  // create zoom buttons
  QHBoxLayout *zoomLayout = new QHBoxLayout;

  buttonIn = new QPushButton("+");
  buttonOut = new QPushButton("-");

  buttonIn->setMaximumWidth(20);
  buttonOut->setMaximumWidth(20);

  zoomLayout->addWidget(buttonIn);
  zoomLayout->addWidget(buttonOut);

  zoomLayout->setContentsMargins(0,0,0,0);
  zoomLayout->setSpacing(0);

  QWidget *zoomWidget = new QWidget;

  zoomWidget->setLayout(zoomLayout);

  // make zoom buttons slightly transparent
  zoomWidget->setStyleSheet("background-color: rgba(245,245,165,70);");

  // add the workspace and zoom buttons to the same grid position
  QGridLayout *textLayout = new QGridLayout;

  textLayout->addWidget(tabs,0,0);
  textLayout->addWidget(zoomWidget,0,0);

  // position zoom widget
  textLayout->setAlignment(zoomWidget,Qt::AlignTop | Qt::AlignRight);
  zoomWidget->setContentsMargins(0,3,26,0);
  textLayout->setContentsMargins(0,0,0,0);

  QWidget *textWidget = new QWidget;

  textWidget->setLayout(textLayout);

  // create workspaces and add them to the tabs
  for(int ws = 0; ws < workspace_max; ws++) {
	  workspaces[ws] = new QsciScintilla;
	  QString w = QString("%1").arg(QString::number(ws + 1));
	  tabs->addTab(workspaces[ws], w);
  }

  lexer = new QsciLexerPython;
  lexer->setAutoIndentStyle(QsciScintilla::AiMaintain);

  // Autocompletion stuff
  QsciAPIs* api = new QsciAPIs(lexer);
  QStringList api_names;
  // yes, really
  #include "api_list.h"
  for (int api_iter = 0; api_iter < api_names.size(); ++api_iter) {
	  api->add(api_names.at(api_iter));
  }
  api->prepare();
  QFont font("Monospace");
  font.setStyleHint(QFont::Monospace);
  lexer->setDefaultFont(font);

  // Setup terminal panes
  QVBoxLayout *terminalLayout = new QVBoxLayout;

  terminalDisplay = new QTextEdit;
  terminalDisplay->setReadOnly(true);
  terminalDisplay->setLineWrapMode(QTextEdit::NoWrap);
  terminalDisplay->document()->setMaximumBlockCount(1000);
  terminalDisplay->zoomIn(1);
  terminalDisplay->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  QHBoxLayout *terminalButtonLayout = new QHBoxLayout;

  // Setup terminal buttons
  QPushButton *buttonRun = new QPushButton("Run");
  QPushButton *buttonSpeed = new QPushButton("Speed: Slow");


  buttonRun->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  buttonSpeed->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  terminalButtonLayout->addWidget(buttonRun);
  terminalButtonLayout->addWidget(buttonSpeed);


  terminalButtonLayout->setContentsMargins(0,0,0,0);

  QWidget *buttons = new QWidget;
  buttons->setLayout(terminalButtonLayout);

  terminalLayout->addWidget(terminalDisplay);
  terminalLayout->addWidget(buttons);

  terminalLayout->setContentsMargins(0,0,0,0);

  terminalLayout->setStretchFactor(terminalDisplay,4);
  terminalLayout->setStretchFactor(buttons,1);

  QWidget *terminal = new QWidget;
  terminal->setLayout(terminalLayout);

  for(int ws = 0; ws < workspace_max; ws++) {
	  initWorkspace(workspaces[ws]);
  }

  // Setup draggable splitter for script window and terminal
  splitter = new QSplitter(Qt::Horizontal);

  splitter->addWidget(textWidget);
  splitter->addWidget(terminal);

  splitter->setCollapsible(0,0);
  splitter->setCollapsible(1,0);
  splitter->setStretchFactor(0,5);
  splitter->setStretchFactor(1,3);

  // Setup window
  QVBoxLayout *windowLayout = new QVBoxLayout;

  QWidget *gameWidget = new QWidget;

  gameWidget->setAttribute(Qt::WA_NativeWindow);

  windowLayout->addWidget(gameWidget);
  windowLayout->addWidget(splitter);

  windowLayout->setStretchFactor(gameWidget,3);
  windowLayout->setStretchFactor(splitter,2);

  QWidget *mainWidget = new QWidget;

  mainWidget->setLayout(windowLayout);

  setWindowTitle(tr("Pyland"));
  mainWidget->setWindowIcon(QIcon("/images/icon.png"));
  QPalette colourPalette(palette());
  colourPalette.setColor(QPalette::Background,QColor(250,250,197));
  colourPalette.setColor(QPalette::Button,QColor(245,245,165));

  mainWidget->setPalette(colourPalette);
  mainWidget->setAutoFillBackground(true);

  mainWidget->setMinimumSize(424,318);

  this->setCentralWidget(mainWidget);
  this->showMaximized();

  createActions();
  //createToolBar();
  //createStatusBar();

  std::cout << gameWidget->winId() << "\n";

  int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  if (result != 0) {
	std::cout << "failed to init SDL\n";
  }

  embedWindow = SDL_CreateWindowFrom((void*)(gameWidget->winId()));
  SDL_SetWindowSize(embedWindow, 200, 200);
  glViewport(0, 0, 200, 200);
  embedWindow->flags |= SDL_WINDOW_OPENGL;
  SDL_GL_LoadLibrary(NULL);
  glContext = SDL_GL_CreateContext(embedWindow);
  glClearColor(0.25f, 0.50f, 1.0f, 1.0f);
  std::cout << "created context\n";
  gameWidget->installEventFilter(this);
  gameWidget->setFocusPolicy(Qt::ClickFocus);
  eventTimer = new QTimer(this);
  eventTimer->setSingleShot(false);
  eventTimer->setInterval(0);
  connect(eventTimer, SIGNAL(timeout()), this, SLOT(timerHandler()));
  eventTimer->start();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  QKeyEvent *keyEvent = NULL;
  if (event->type() == QEvent::KeyPress) {
	keyEvent = static_cast<QKeyEvent*>(event);
	if (keyEvent->key()) {
	  SDL_Event sdlEvent;
	  sdlEvent.type = SDL_KEYDOWN;
	  sdlEvent.key.state = SDL_PRESSED;
	  SDL_PushEvent(&sdlEvent);
	  std::cout << "got a Qt keydown event\n";
	}
  } else {
	return QObject::eventFilter(obj, event);
  }
  return true;
}

void MainWindow::timerHandler() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
	switch(event.type) {
	  case SDL_KEYDOWN:
		std::cout << " got an SDL keydown event\n";
		break;
	}
  }
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(embedWindow);
}

void MainWindow::initWorkspace(QsciScintilla* ws) {
  ws->setAutoIndent(true);
  ws->setIndentationsUseTabs(false);
  ws->setBackspaceUnindents(true);
  ws->setTabIndents(true);
  ws->setMatchedBraceBackgroundColor(QColor("dimgray"));
  ws->setMatchedBraceForegroundColor(QColor("white"));
  ws->setIndentationWidth(2);
  ws->setIndentationGuides(true);
  ws->setIndentationGuidesForegroundColor(QColor("deep pink"));
  ws->setBraceMatching(QsciScintilla::SloppyBraceMatch);
  ws->setCaretLineVisible(true);
  ws->setCaretLineBackgroundColor(QColor("whitesmoke"));
  ws->setFoldMarginColors(QColor("whitesmoke"),QColor("whitesmoke"));
  ws->setMarginLineNumbers(0, true);
  ws->setMarginWidth(0, "100000000");
  ws->setMarginsBackgroundColor(QColor("whitesmoke"));
  ws->setMarginsForegroundColor(QColor("dark gray"));
  ws->setMarginsFont(QFont("Menlo",5, -1, true));
  ws->setUtf8(true);
  ws->setText("");
  ws->setLexer(lexer);
  ws->zoomIn(13);
  ws->setAutoCompletionThreshold(4);
  ws->setAutoCompletionSource(QsciScintilla::AcsAPIs);
  ws->setSelectionBackgroundColor("DeepPink");
  ws->setSelectionForegroundColor("white");
  ws->setCaretWidth(5);
  ws->setMarginWidth(1,5);
  ws->setCaretForegroundColor("deep pink");
  //ws->hide(ws->horizontalScrollBar());
//  QScrollBar *scrollBar = new QScrollBar(Qt::Vertical);
//  scrollBar->setContentsMargins(0,500,-50,-300);
//  scrollBar->move(250,1000);
//  scrollBar->show();
//  scrollBar->setVisible(true);
//  scrollBar->setMaximumHeight(100);
//  scrollBar->setMaximumHeight(100);
//  //scrollBar->setMinimum(100);
//  ws->setVerticalScrollBar(scrollBar);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
}

bool MainWindow::saveAs()
{
  return false;
}

void MainWindow::runCode()
{
  terminalDisplay->clear();
  terminalDisplay->hide();
  statusBar()->showMessage(tr("Running...."), 2000);
  std::string code = ((QsciScintilla*)tabs->currentWidget())->text().toStdString();
}

void MainWindow::zoomFontIn()
{
  ((QsciScintilla*)tabs->currentWidget())->zoomIn(3);
}

void MainWindow::zoomFontOut()
{
  ((QsciScintilla*)tabs->currentWidget())->zoomOut(3);
}


void MainWindow::documentWasModified()
{
  setWindowModified(textEdit->isModified());
}


void MainWindow::clearOutputPanels()
{
	terminalDisplay->clear();
}

void MainWindow::createActions()
{
  connect(buttonIn,SIGNAL(released()),this,SLOT (zoomFontIn()));
  connect(buttonOut,SIGNAL(released()),this,SLOT (zoomFontOut()));

//  runAct = new QAction(QIcon(":/images/run.png"), tr("&Run"), this);
//  runAct->setShortcut(tr("Ctrl+R"));
//  runAct->setStatusTip(tr("Run the code in the current workspace"));
//  runAct->setToolTip(tr("Run the code in the current workspace (Ctrl-R)"));
//  connect(runAct, SIGNAL(triggered()), this, SLOT(runCode()));
//
//  stopAct = new QAction(QIcon(":/images/stop.png"), tr("&Stop"), this);
//  stopAct->setShortcut(tr("Ctrl+Q"));
//  stopAct->setStatusTip(tr("Stop all running code"));
//  stopAct->setToolTip(tr("Stop all running code (Ctrl-Q)"));
//
//  saveAsAct = new QAction(QIcon(":/images/save.png"), tr("&Save &As..."), this);
//  saveAsAct->setStatusTip(tr("Save the current workspace under a new name"));
//  saveAsAct->setToolTip(tr("Save the current workspace under a new name"));
//  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
//
//  textIncAct = new QAction(QIcon(":/images/size_up.png"), tr("&Increase &Text &Size"), this);
//  textIncAct->setStatusTip(tr("Make text bigger"));
//  textIncAct->setToolTip(tr("Make text bigger"));
//  connect(textIncAct, SIGNAL(triggered()), this, SLOT(zoomFontIn()));
//
//  textDecAct = new QAction(QIcon(":/images/size_down.png"), tr("&Decrease &Text &Size"), this);
//  textDecAct->setStatusTip(tr("Make text smaller"));
//  textDecAct->setToolTip(tr("Make text smaller"));
//  connect(textDecAct, SIGNAL(triggered()), this, SLOT(zoomFontOut()));
//
//  QAction *reloadAct = new QAction(this);
//  reloadAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
//  addAction(reloadAct);
//  runAct = new QAction(QIcon(":/images/run.png"), tr("&Run"), this);
//  runAct->setShortcut(tr("Ctrl+R"));
//  runAct->setStatusTip(tr("Run the code in the current workspace"));
//  runAct->setToolTip(tr("Run the code in the current workspace (Ctrl-R)"));
//  connect(runAct, SIGNAL(triggered()), this, SLOT(runCode()));
//
//  stopAct = new QAction(QIcon(":/images/stop.png"), tr("&Stop"), this);
//  stopAct->setShortcut(tr("Ctrl+Q"));
//  stopAct->setStatusTip(tr("Stop all running code"));
//  stopAct->setToolTip(tr("Stop all running code (Ctrl-Q)"));
//
//  saveAsAct = new QAction(QIcon(":/images/save.png"), tr("&Save &As..."), this);
//  saveAsAct->setStatusTip(tr("Save the current workspace under a new name"));
//  saveAsAct->setToolTip(tr("Save the current workspace under a new name"));
//  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
//
//  textIncAct = new QAction(QIcon(":/images/size_up.png"), tr("&Increase &Text &Size"), this);
//  textIncAct->setStatusTip(tr("Make text bigger"));
//  textIncAct->setToolTip(tr("Make text bigger"));
//  connect(textIncAct, SIGNAL(triggered()), this, SLOT(zoomFontIn()));
//
//  textDecAct = new QAction(QIcon(":/images/size_down.png"), tr("&Decrease &Text &Size"), this);
//  textDecAct->setStatusTip(tr("Make text smaller"));
//  textDecAct->setToolTip(tr("Make text smaller"));
//  connect(textDecAct, SIGNAL(triggered()), this, SLOT(zoomFontOut()));
//
//  QAction *reloadAct = new QAction(this);
//  reloadAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
//  addAction(reloadAct);
}

void MainWindow::createToolBar()
{

  QWidget *spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  toolBar = addToolBar(tr("Tools"));

  toolBar->setIconSize(QSize(270/3, 111/3));
  toolBar->addAction(runAct);
  toolBar->addAction(stopAct);

  toolBar->addAction(saveAsAct);
  toolBar->addWidget(spacer);

  toolBar->addAction(textDecAct);
  toolBar->addAction(textIncAct);

}


void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}
