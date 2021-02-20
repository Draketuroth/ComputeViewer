import ctypes as ct

# Set up the instance of the loaded library.
DLL_PATH = "Lib/DirectX12/DX12Compute.dll"
lib = ct.CDLL(DLL_PATH)

# Compute class is returned from DLL as void*.
InstanceHandle = ct.POINTER(ct.c_void_p)

class ComputeHandler:
  def __init__(self):
    """Initializes the compute library"""
    
    lib.createCompute.restype = InstanceHandle
    
    lib.setShaderPath.argTypes = [InstanceHandle, ct.c_wchar_p]
    
    lib.dispatch.restype = ct.c_bool
    
    # Store the compute class instance.
    self.instance = lib.createCompute()
    
  def __del__(self):
    """Deallocates compute library resources"""
    lib.destroyCompute(self.instance)
    
  def setShaderPath(self, path):
    """Sets the shader path for the shader to be executed"""
    lib.setShaderPath(self.instance, ct.c_wchar_p(path))
    
  def setTextureSize(self, textureWidth, textureHeight, texturePixelSize):
    """Sets the texture size and specifies number of channels per pixel"""
    lib.setTextureSize(self.instance, ct.c_int(textureWidth), ct.c_int(textureHeight), ct.c_int(texturePixelSize))
    
  def setDispatchSize(self, x, y, z):
    """Sets the number of dispatch groups from the CPU"""
    lib.setDispatchSize(self.instance, ct.c_int(x), ct.c_int(y), ct.c_int(z))
    
  def dispatch(self):
    """Dispatches the compute pipeline and stores image data"""
    return lib.dispatch(self.instance)