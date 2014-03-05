/*
Copyright (c) 2006-2014, Michael Greminger
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF A
DVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef PSKETCHERWINDOW_H
#define PSKETCHERWINDOW_H

#include <QMainWindow>
#include <QActionGroup>

#include "pSketcherWidget.h"

class pSketcherWindow : public QMainWindow
{
	Q_OBJECT

public:
	pSketcherWindow();

private slots:
	void About();
	void TriggerSketchActionGroup();
	void SwitchToSelectionMode();

private:
	void CreateActions();
	void CreateMenus();
	void CreateToolbars();

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *viewMenu;
	QMenu *helpMenu;

	QToolBar *viewToolBar;

	QAction *exitAction;
	QAction *aboutAction;

	QAction *fitAction;
	QAction *zoomAction;

	QAction *viewShadedAction;
	QAction *viewWireframeAction;
	QAction *viewNoHiddenLineAction;

	// Debugging actions
	QAction *generateSketchAction;
	QAction *solveConstraintsAction;
	QToolBar *debugToolBar;

	// File actions
	QAction *saveAction;
	QAction *openAction;
	QAction *saveAsAction;
	QAction *newFileAction;
	QAction *exportDXFAction;

	// Edit actions
	QAction *undoAction;
	QAction *redoAction;

	// Sketching action
	QActionGroup *sketchActionGroup;
	QAction *selectAction;
	QAction *makeLineAction;
	QAction *makePointAction;
	QAction *makeArcAction;
	QAction *makeCircleAction;
	QAction *makeDistanceConstraintAction;
	QAction *makeAngleConstraintAction;
	QAction *makeParallelConstraintAction;
	QAction *makeVerticalConstraintAction;
	QAction *makeHorizontalConstraintAction;
	QAction *makeTangentConstraintAction;
	QToolBar *sketchToolBar;

	// The OpenCASCADE Qt widget and the QGraphicsScene;
	pSketcherWidget *view;
	QGraphicsScene *scene;

};

#endif

