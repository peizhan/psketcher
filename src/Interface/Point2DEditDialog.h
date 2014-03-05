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


#ifndef POINT2DEDITDIALOG_H
#define POINT2DEDITDIALOG_H

#include <QDialog>
#include "../ConstraintSolver/Primitives.h"

class QDialogButtonBox;
class QLabel;
class QCheckBox;
class QLineEdit;
class QAbstractButton;

class Point2DEditDialog : public QDialog
{
	Q_OBJECT

	public:
		Point2DEditDialog(Point2DPointer point, QWidget *parent = 0);

	signals:
		void modelChanged(QString description);  // emitted when changes are applied

	private slots:
		void accept();
		void textChanged();
		void applyChanges();
		void resetDialog();

	private:
		// methods

		// Parameters
		Point2DPointer point_; // point that is being editted by this dialog box
		double starting_s_; // starting values, used if user clicks reset button
		double starting_t_;
		bool starting_s_free_;
		bool starting_t_free_;

		QDialogButtonBox *buttonBox_;
		
		QLabel *sLabel_;
		QCheckBox *sFixedCheckBox_;
		QLineEdit *sLineEdit_;

		QLabel *tLabel_;
		QCheckBox *tFixedCheckBox_;
		QLineEdit *tLineEdit_;

		// pointers to standard buttons created by QDialogButtonBox, stored for convenience
		QPushButton *okButton_;
		QPushButton *applyButton_;
		QPushButton *resetButton_;
};



#endif // POINT2DEDITDIALOG_H
