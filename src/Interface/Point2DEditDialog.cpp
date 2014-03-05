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


#include <QtGui>
#include "Point2DEditDialog.h"

Point2DEditDialog::Point2DEditDialog(Point2DPointer point, QWidget *parent) :
point_(point), QDialog(parent)
{
	// store initial values
	starting_s_ = point_->GetSValue(); // starting values, used if user clicks reset button
	starting_t_ = point_->GetTValue();
	starting_s_free_ = point_->SIsFree();
	starting_t_free_ = point_->TIsFree();

	// create widgets
	sLabel_ = new QLabel(tr("&s value:"));
	sLineEdit_ = new QLineEdit;
	sLineEdit_->setValidator(new QDoubleValidator(this));
	connect(sLineEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged()));
	sLabel_->setBuddy(sLineEdit_);
	sFixedCheckBox_ = new QCheckBox(tr("Fixed Parameter"));

	tLabel_ = new QLabel(tr("&t value:"));
	tLineEdit_ = new QLineEdit;
	tLineEdit_->setValidator(new QDoubleValidator(this));
	connect(tLineEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged()));
	tLabel_->setBuddy(tLineEdit_);
	tFixedCheckBox_ = new QCheckBox(tr("Fixed Parameter"));

	buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Ok
                                    | QDialogButtonBox::Cancel
                                    | QDialogButtonBox::Apply
                                    | QDialogButtonBox::Reset);

	okButton_ = buttonBox_->button(QDialogButtonBox::Ok);
	applyButton_ = buttonBox_->button(QDialogButtonBox::Apply);
	resetButton_ = buttonBox_->button(QDialogButtonBox::Reset);

	connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttonBox_, SIGNAL(clicked(QAbstractButton *)), this, SLOT(buttonClicked(QAbstractButton*)));

	connect(applyButton_, SIGNAL(clicked()), this, SLOT(applyChanges()));
	connect(resetButton_, SIGNAL(clicked()), this, SLOT(resetDialog()));


	// package widgets
	QGridLayout *grid = new QGridLayout;
	grid->addWidget(sLabel_,0,0);
	grid->addWidget(sLineEdit_,0,1);
	grid->addWidget(sFixedCheckBox_,0,2);
	grid->addWidget(tLabel_,1,0);
	grid->addWidget(tLineEdit_,1,1);
	grid->addWidget(tFixedCheckBox_,1,2);

	QVBoxLayout *hbox = new QVBoxLayout();
	hbox->addLayout(grid);
	hbox->addWidget(buttonBox_);

	setLayout(hbox);

	setWindowTitle(tr("Edit Point"));

	// initialize dialog
	resetDialog();	// initialize the displayed dialog values
}

// user accepted changes, so apply the changes and let QDialog take care of the rest
void Point2DEditDialog::accept()
{
	applyChanges();	
	QDialog::accept();
}

// initialize dialog to the initial values
void Point2DEditDialog::resetDialog()
{
	sLineEdit_->setText(QString("%1").arg(starting_s_));
	tLineEdit_->setText(QString("%1").arg(starting_t_));
	sFixedCheckBox_->setChecked( ! starting_s_free_);
	tFixedCheckBox_->setChecked( ! starting_t_free_);
}

// apply the changes if valid values have been entered
void Point2DEditDialog::applyChanges()
{
	point_->SetSValue(sLineEdit_->text().toDouble());
	point_->SetTValue(tLineEdit_->text().toDouble());
	point_->SetSFree( ! sFixedCheckBox_->isChecked());
	point_->SetTFree( ! tFixedCheckBox_->isChecked());

	emit modelChanged(tr("Point Edit"));
}


void Point2DEditDialog::textChanged()
{
	bool acceptable_input;

	acceptable_input = sLineEdit_->hasAcceptableInput() && tLineEdit_->hasAcceptableInput();

	okButton_->setEnabled(acceptable_input);
	applyButton_->setEnabled(acceptable_input);
}
