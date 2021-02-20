import sys
import ctypes
import mmap
import numpy as np
from PyQt5 import Qt, QtWidgets, QtCore, QtGui, uic
from Compute import ComputeHandler

from ComputeViewerUi import Ui_MainWindow

class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):
    
    shaderPath = ""
    imagePath = ""
    inputImage = QtGui.QImage()
    
    def __init__(self, *args, obj=None, **kwargs):
        """Initializes the GUI main window"""
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setupUi(self)
        self.setupUiComponents()
        
    def setupUiComponents(self):
        """Prepares UI elements and connect to callback functions"""
        self.PreviewImageLabel.setAlignment(QtCore.Qt.AlignCenter)
        
        self.DispatchButton.clicked.connect(self.dispatchFunc)
        self.LoadImageButton.clicked.connect(self.loadImageFile)
        
        self.actionLoad.triggered.connect(self.loadShaderFile)
        
        self.actionAbout.triggered.connect(self.openAboutDialog)
        
        self.actionExit.triggered.connect(self.quitApplication)
        
    def quitApplication(self):
        """Quits the application"""
        
        app.exit() 
        
    def openAboutDialog(self):
        """Displays information about the current version of ComputeViewer"""
        
        msg = "Compute Viewer 1.0\nAuthor: Fredrik Linde"
        QtWidgets.QMessageBox.about(self, "About", msg)
        
    def openFileNameDialog(self):
        """Opens a file dialog to receive path to file on disk"""
        
        options = QtWidgets.QFileDialog.Options()
        options |= QtWidgets.QFileDialog.DontUseNativeDialog
        fileName, _ = QtWidgets.QFileDialog.getOpenFileName(self,"QFileDialog.getOpenFileName()", "","All Files (*);;Python Files (*.py)", options=options)
        return fileName
        
    def loadShaderFile(self):
        fileName = self.openFileNameDialog()
        if fileName:
            shaderPath = fileName
            winShaderPath = shaderPath.replace("/","\\\\")
        
            computeHandler.setShaderPath(winShaderPath)
            
    def loadImageFile(self):
        fileName = self.openFileNameDialog()
        if fileName:
            imagePath = fileName
            
            self.inputImage = QtGui.QImage(imagePath)
            
            pixmap = QtGui.QPixmap()
            pixmap.convertFromImage(self.inputImage)
            
            self.ImageWidthLineEdit.setText(str(pixmap.width()))
            self.ImageHeightLineEdit.setText(str(pixmap.height()))
            self.UsePreviewDataCheckBox.setChecked(True)
            
            self.PreviewImageLabel.setPixmap(pixmap)
            self.PreviewImageLabel.show()
        
    def dispatchFunc(self):
        """Creates a dispatch call from current settings and updates preview image"""
        
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
        
        # Set preview image as source texture (if option is checked)
        usePreviewData = self.UsePreviewDataCheckBox.isChecked()
        if usePreviewData:
            imgData = []
            for x in range(imageWidth):
                for y in range(imageHeight):
                    r, g, b, a = QtGui.QColor(self.inputImage.pixel(y, x)).getRgb()
                    imgData.append(r)
                    imgData.append(g)
                    imgData.append(b)
                    imgData.append(a)
            
            #arr = (ctypes.c_ubyte * len(imgData))(*imgData)
            qImageDataPtr = mmap.mmap(-1, textureSize, "Global\\ImageMappingObject", mmap.ACCESS_WRITE)
            qImageDataPtr.write(bytearray(imgData))
        
        # Perform the dispatch, storing the produced image in the compute instance.
        computeHandler.dispatch()
        
        # Read back the texture data from shared memory.
        imageDataPtr = mmap.mmap(-1, textureSize, "Global\\ImageMappingObject", mmap.ACCESS_READ)
        if imageDataPtr:
            
            # Construct image from the raw byte data.
            image = QtGui.QImage(bytearray(imageDataPtr), imageWidth, imageHeight, stride, QtGui.QImage.Format_RGBA8888 )
        
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
        else:
            print("[ERROR] Image data is empty")

app = QtWidgets.QApplication(sys.argv)

computeHandler = ComputeHandler()

window = MainWindow()
window.show()
app.exec()