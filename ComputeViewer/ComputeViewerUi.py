# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ComputeViewer.ui'
#
# Created by: PyQt5 UI code generator 5.13.1
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(763, 701)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap("Resources/Application.ico"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        MainWindow.setWindowIcon(icon)
        self.CentralWidget = QtWidgets.QWidget(MainWindow)
        self.CentralWidget.setObjectName("CentralWidget")
        self.gridLayout = QtWidgets.QGridLayout(self.CentralWidget)
        self.gridLayout.setObjectName("gridLayout")
        self.RightRegionLayout = QtWidgets.QGridLayout()
        self.RightRegionLayout.setSizeConstraint(QtWidgets.QLayout.SetDefaultConstraint)
        self.RightRegionLayout.setObjectName("RightRegionLayout")
        self.LeftRegionGroup = QtWidgets.QGroupBox(self.CentralWidget)
        self.LeftRegionGroup.setObjectName("LeftRegionGroup")
        self.LeftRegionGroupLayout = QtWidgets.QGridLayout(self.LeftRegionGroup)
        self.LeftRegionGroupLayout.setObjectName("LeftRegionGroupLayout")
        self.SaveImageButton = QtWidgets.QPushButton(self.LeftRegionGroup)
        self.SaveImageButton.setObjectName("SaveImageButton")
        self.LeftRegionGroupLayout.addWidget(self.SaveImageButton, 2, 1, 1, 1)
        self.LoadImageButton = QtWidgets.QPushButton(self.LeftRegionGroup)
        self.LoadImageButton.setObjectName("LoadImageButton")
        self.LeftRegionGroupLayout.addWidget(self.LoadImageButton, 2, 0, 1, 1)
        self.DispatchButton = QtWidgets.QPushButton(self.LeftRegionGroup)
        font = QtGui.QFont()
        font.setPointSize(15)
        self.DispatchButton.setFont(font)
        self.DispatchButton.setObjectName("DispatchButton")
        self.LeftRegionGroupLayout.addWidget(self.DispatchButton, 4, 0, 1, 2)
        self.DispatchSettingsLayout = QtWidgets.QGridLayout()
        self.DispatchSettingsLayout.setObjectName("DispatchSettingsLayout")
        self.DispatchSettingsBox = QtWidgets.QGroupBox(self.LeftRegionGroup)
        self.DispatchSettingsBox.setObjectName("DispatchSettingsBox")
        self.DispatchSettingsBoxLayout = QtWidgets.QGridLayout(self.DispatchSettingsBox)
        self.DispatchSettingsBoxLayout.setObjectName("DispatchSettingsBoxLayout")
        self.DispatchDimensionsGroupBox = QtWidgets.QGroupBox(self.DispatchSettingsBox)
        self.DispatchDimensionsGroupBox.setObjectName("DispatchDimensionsGroupBox")
        self.DispatchDimensionsGroupBoxlayout = QtWidgets.QGridLayout(self.DispatchDimensionsGroupBox)
        self.DispatchDimensionsGroupBoxlayout.setObjectName("DispatchDimensionsGroupBoxlayout")
        self.DispatchZLabel = QtWidgets.QLabel(self.DispatchDimensionsGroupBox)
        self.DispatchZLabel.setObjectName("DispatchZLabel")
        self.DispatchDimensionsGroupBoxlayout.addWidget(self.DispatchZLabel, 2, 0, 1, 1)
        self.DispatchZLineEdit = QtWidgets.QLineEdit(self.DispatchDimensionsGroupBox)
        self.DispatchZLineEdit.setObjectName("DispatchZLineEdit")
        self.DispatchDimensionsGroupBoxlayout.addWidget(self.DispatchZLineEdit, 2, 1, 1, 1)
        self.DispatchYLabel = QtWidgets.QLabel(self.DispatchDimensionsGroupBox)
        self.DispatchYLabel.setObjectName("DispatchYLabel")
        self.DispatchDimensionsGroupBoxlayout.addWidget(self.DispatchYLabel, 1, 0, 1, 1)
        self.DispatchYLineEdit = QtWidgets.QLineEdit(self.DispatchDimensionsGroupBox)
        self.DispatchYLineEdit.setObjectName("DispatchYLineEdit")
        self.DispatchDimensionsGroupBoxlayout.addWidget(self.DispatchYLineEdit, 1, 1, 1, 1)
        self.DispatchXLabel = QtWidgets.QLabel(self.DispatchDimensionsGroupBox)
        self.DispatchXLabel.setObjectName("DispatchXLabel")
        self.DispatchDimensionsGroupBoxlayout.addWidget(self.DispatchXLabel, 0, 0, 1, 1)
        self.DispatchXLineEdit = QtWidgets.QLineEdit(self.DispatchDimensionsGroupBox)
        self.DispatchXLineEdit.setObjectName("DispatchXLineEdit")
        self.DispatchDimensionsGroupBoxlayout.addWidget(self.DispatchXLineEdit, 0, 1, 1, 1)
        self.DispatchSettingsBoxLayout.addWidget(self.DispatchDimensionsGroupBox, 0, 0, 1, 1)
        self.DispatchSettingsLayout.addWidget(self.DispatchSettingsBox, 0, 0, 1, 1)
        self.LeftRegionGroupLayout.addLayout(self.DispatchSettingsLayout, 3, 1, 1, 1)
        self.PreviewImageLayout = QtWidgets.QGridLayout()
        self.PreviewImageLayout.setObjectName("PreviewImageLayout")
        self.PreviewImageLabel = QtWidgets.QLabel(self.LeftRegionGroup)
        self.PreviewImageLabel.setStyleSheet("background-color: palette(dark)")
        self.PreviewImageLabel.setFrameShape(QtWidgets.QFrame.Box)
        self.PreviewImageLabel.setFrameShadow(QtWidgets.QFrame.Plain)
        self.PreviewImageLabel.setLineWidth(2)
        self.PreviewImageLabel.setText("")
        self.PreviewImageLabel.setObjectName("PreviewImageLabel")
        self.PreviewImageLayout.addWidget(self.PreviewImageLabel, 0, 0, 1, 1)
        self.LeftRegionGroupLayout.addLayout(self.PreviewImageLayout, 1, 0, 1, 2)
        self.ImageSettingsLayout = QtWidgets.QGridLayout()
        self.ImageSettingsLayout.setObjectName("ImageSettingsLayout")
        self.ImageSettingsGroupBox = QtWidgets.QGroupBox(self.LeftRegionGroup)
        self.ImageSettingsGroupBox.setObjectName("ImageSettingsGroupBox")
        self.ImageSettingsGroupBoxLayout = QtWidgets.QGridLayout(self.ImageSettingsGroupBox)
        self.ImageSettingsGroupBoxLayout.setObjectName("ImageSettingsGroupBoxLayout")
        self.ImageDimensionsGroupBox = QtWidgets.QGroupBox(self.ImageSettingsGroupBox)
        self.ImageDimensionsGroupBox.setObjectName("ImageDimensionsGroupBox")
        self.ImageDimensionsGroupBoxLayout = QtWidgets.QGridLayout(self.ImageDimensionsGroupBox)
        self.ImageDimensionsGroupBoxLayout.setObjectName("ImageDimensionsGroupBoxLayout")
        self.ImageHeightLabel = QtWidgets.QLabel(self.ImageDimensionsGroupBox)
        self.ImageHeightLabel.setObjectName("ImageHeightLabel")
        self.ImageDimensionsGroupBoxLayout.addWidget(self.ImageHeightLabel, 1, 0, 1, 1)
        self.ImageWidthLabel = QtWidgets.QLabel(self.ImageDimensionsGroupBox)
        self.ImageWidthLabel.setObjectName("ImageWidthLabel")
        self.ImageDimensionsGroupBoxLayout.addWidget(self.ImageWidthLabel, 0, 0, 1, 1)
        self.ImageWidthLineEdit = QtWidgets.QLineEdit(self.ImageDimensionsGroupBox)
        self.ImageWidthLineEdit.setObjectName("ImageWidthLineEdit")
        self.ImageDimensionsGroupBoxLayout.addWidget(self.ImageWidthLineEdit, 0, 1, 1, 1)
        self.ImageHeightLineEdit = QtWidgets.QLineEdit(self.ImageDimensionsGroupBox)
        self.ImageHeightLineEdit.setObjectName("ImageHeightLineEdit")
        self.ImageDimensionsGroupBoxLayout.addWidget(self.ImageHeightLineEdit, 1, 1, 1, 1)
        self.ImageSettingsGroupBoxLayout.addWidget(self.ImageDimensionsGroupBox, 1, 0, 1, 1)
        self.InputImageSettings = QtWidgets.QComboBox(self.ImageSettingsGroupBox)
        self.InputImageSettings.setObjectName("InputImageSettings")
        self.InputImageSettings.addItem("")
        self.InputImageSettings.addItem("")
        self.InputImageSettings.addItem("")
        self.ImageSettingsGroupBoxLayout.addWidget(self.InputImageSettings, 0, 0, 1, 1)
        self.ImageSettingsLayout.addWidget(self.ImageSettingsGroupBox, 0, 1, 1, 1)
        self.LeftRegionGroupLayout.addLayout(self.ImageSettingsLayout, 3, 0, 1, 1)
        self.RightRegionLayout.addWidget(self.LeftRegionGroup, 0, 1, 1, 1)
        self.RightRegionGroup = QtWidgets.QGroupBox(self.CentralWidget)
        self.RightRegionGroup.setObjectName("RightRegionGroup")
        self.RightRegionGroupLayout = QtWidgets.QGridLayout(self.RightRegionGroup)
        self.RightRegionGroupLayout.setObjectName("RightRegionGroupLayout")
        self.EditorLayout = QtWidgets.QGridLayout()
        self.EditorLayout.setObjectName("EditorLayout")
        self.plainTextEdit = QtWidgets.QPlainTextEdit(self.RightRegionGroup)
        self.plainTextEdit.setObjectName("plainTextEdit")
        self.EditorLayout.addWidget(self.plainTextEdit, 3, 0, 1, 1)
        self.RightRegionGroupLayout.addLayout(self.EditorLayout, 0, 0, 1, 1)
        self.RightRegionLayout.addWidget(self.RightRegionGroup, 0, 2, 1, 1)
        self.gridLayout.addLayout(self.RightRegionLayout, 0, 0, 1, 1)
        MainWindow.setCentralWidget(self.CentralWidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 763, 21))
        self.menubar.setObjectName("menubar")
        self.menuFile = QtWidgets.QMenu(self.menubar)
        self.menuFile.setObjectName("menuFile")
        self.menuHelp = QtWidgets.QMenu(self.menubar)
        self.menuHelp.setObjectName("menuHelp")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.actionNew = QtWidgets.QAction(MainWindow)
        self.actionNew.setObjectName("actionNew")
        self.actionLoad = QtWidgets.QAction(MainWindow)
        self.actionLoad.setObjectName("actionLoad")
        self.actionSave = QtWidgets.QAction(MainWindow)
        self.actionSave.setObjectName("actionSave")
        self.actionSave_As = QtWidgets.QAction(MainWindow)
        self.actionSave_As.setObjectName("actionSave_As")
        self.actionExit = QtWidgets.QAction(MainWindow)
        self.actionExit.setObjectName("actionExit")
        self.actionAbout = QtWidgets.QAction(MainWindow)
        self.actionAbout.setObjectName("actionAbout")
        self.menuFile.addAction(self.actionNew)
        self.menuFile.addAction(self.actionLoad)
        self.menuFile.addAction(self.actionSave)
        self.menuFile.addAction(self.actionSave_As)
        self.menuFile.addAction(self.actionExit)
        self.menuHelp.addAction(self.actionAbout)
        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuHelp.menuAction())

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "ComputeViewer 1.0"))
        self.LeftRegionGroup.setTitle(_translate("MainWindow", "Preview"))
        self.SaveImageButton.setText(_translate("MainWindow", "Save Image"))
        self.LoadImageButton.setText(_translate("MainWindow", "Load Image"))
        self.DispatchButton.setText(_translate("MainWindow", "Dispatch"))
        self.DispatchSettingsBox.setTitle(_translate("MainWindow", "Dispatch Settings"))
        self.DispatchDimensionsGroupBox.setTitle(_translate("MainWindow", "Dimensions"))
        self.DispatchZLabel.setText(_translate("MainWindow", "Z"))
        self.DispatchZLineEdit.setText(_translate("MainWindow", "1"))
        self.DispatchYLabel.setText(_translate("MainWindow", "Y"))
        self.DispatchYLineEdit.setText(_translate("MainWindow", "0"))
        self.DispatchXLabel.setText(_translate("MainWindow", "X"))
        self.DispatchXLineEdit.setText(_translate("MainWindow", "0"))
        self.ImageSettingsGroupBox.setTitle(_translate("MainWindow", "Image Settings"))
        self.ImageDimensionsGroupBox.setTitle(_translate("MainWindow", "Dimensions"))
        self.ImageHeightLabel.setText(_translate("MainWindow", "Height"))
        self.ImageWidthLabel.setText(_translate("MainWindow", "Width"))
        self.ImageWidthLineEdit.setText(_translate("MainWindow", "0"))
        self.ImageHeightLineEdit.setText(_translate("MainWindow", "0"))
        self.InputImageSettings.setItemText(0, _translate("MainWindow", "Use Default Image"))
        self.InputImageSettings.setItemText(1, _translate("MainWindow", "Use Source Image"))
        self.InputImageSettings.setItemText(2, _translate("MainWindow", "Use Preview Image"))
        self.RightRegionGroup.setTitle(_translate("MainWindow", "Editor"))
        self.menuFile.setTitle(_translate("MainWindow", "File"))
        self.menuHelp.setTitle(_translate("MainWindow", "Help"))
        self.actionNew.setText(_translate("MainWindow", "New Shader"))
        self.actionLoad.setText(_translate("MainWindow", "Load Shader"))
        self.actionSave.setText(_translate("MainWindow", "Save Shader"))
        self.actionSave_As.setText(_translate("MainWindow", "Save Shader As"))
        self.actionExit.setText(_translate("MainWindow", "Exit"))
        self.actionAbout.setText(_translate("MainWindow", "About ComputeViewer"))
