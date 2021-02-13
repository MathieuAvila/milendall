import os

files = os.listdir(os.path.dirname(__file__))
modules = [ k[:-3] for k in files if '.py' in k]
print(modules)
#modules = modules.remove("__init__.py")

__all__ =  modules

roomTypes = {}

def registerRoomType(name, obj):
    roomTypes[name] = obj
    print("Registering %s" % name)

