import sys
import ctypes
import numpy as np
from PyQt5 import Qt, QtWidgets, QtCore, QtGui, uic
from Compute import ComputeHandler

from ComputeViewerUi import Ui_MainWindow

class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):
    
    shaderPath = ""
    
    def __init__(self, *args, obj=None, **kwargs):
        """Initializes the GUI main window"""
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setupUi(self)
        self.setupUiComponents()
        
    def setupUiComponents(self):
        """Prepares UI elements and connect to callback functions"""
        self.PreviewImageLabel.setAlignment(QtCore.Qt.AlignCenter)
        
        self.DispatchButton.clicked.connect(self.dispatchFunc)
        
        self.actionLoad.triggered.connect(self.openFileNameDialog)
        
    def openFileNameDialog(self):
        options = QtWidgets.QFileDialog.Options()
        options |= QtWidgets.QFileDialog.DontUseNativeDialog
        fileName, _ = QtWidgets.QFileDialog.getOpenFileName(self,"QFileDialog.getOpenFileName()", "","All Files (*);;Python Files (*.py)", options=options)
        if fileName:
            shaderPath = fileName
            winShaderPath = shaderPath.replace("/","\\\\")
            print(winShaderPath)
        
            computeHandler.setShaderPath(winShaderPath)
        
    def dispatchFunc(self):
        """Creates a dispatch call from current settings and preview image output"""
        
        # Gather image settings.
        imageWidth = int(self.ImageWidthLineEdit.text())
        imageHeight = int(self.ImageHeightLineEdit.text())
        texturePixelSize = 4
        textureSize = imageWidth * imageHeight * texturePixelSize
        stride = imageWidth * texturePixelSize
        
        dispatchX = int(self.DispatchXLineEdit.text())
        dispatchY = int(self.DispatchYLineEdit.text())
        dispatchZ = int(self.DispatchZLineEdit.text())
        
        # Set current texture and dispatch dimensions.
        computeHandler.setTextureSize(imageWidth, imageHeight, texturePixelSize)
        computeHandler.setDispatchSize(dispatchX, dispatchY, dispatchZ)
        
        # Perform the dispatch, storing the produced image in the compute instance.
        computeHandler.dispatch()
        
        # Read back the texture data.
        imageDataPtr = computeHandler.readBackData()
        imageData=[imageDataPtr[i] for i in range(textureSize)]
        
        # Construct image from the raw byte data.
        image = QtGui.QImage(bytearray(imageData), imageWidth, imageHeight, stride, QtGui.QImage.Format_RGBA8888 )
        
        # Create buffer and save image data into the buffer.
        # Note: Including the image header required for QPixmap to create image data.
        imageByteArray = QtCore.QByteArray()
        imageBuffer = QtCore.QBuffer(imageByteArray)
        imageBuffer.open(QtCore.QIODevice.ReadWrite)
        image.save(imageBuffer, "JPG")
        
        # Create pixmap used to present preview image.
        pixmap = QtGui.QPixmap()
        pixmap.loadFromData(imageBuffer.data())
        
        self.PreviewImageLabel.setPixmap(pixmap)
        self.PreviewImageLabel.show()

app = QtWidgets.QApplication(sys.argv)

computeHandler = ComputeHandler()

window = MainWindow()
window.show()
app.exec()