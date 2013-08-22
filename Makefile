CXX = g++
CPPFLAGS = -g

# uncomment the lines for the machine you are compiling on
# comment out the lines for the other two machines
# windows/cygwin libraries
#LIBS = -lfl  -lopengl32 -lglut32 -lglu32

# linux/UNIX libraries (comment out next two lines if not using linux/unix)
#   if the glut library (libglut.so) does not live in /usr/lib
#   then add a -L/ for glut's lib directory in the next line
LIBDIRS  = -L/usr/X11R6/lib  
LIBS = -lX11 -lglut -lGL -lGLU -lm
# older versions of linux might also need -lXi and -lXmu 

# OS X (comment out next two lines if not using OSX
# LIBS = -framework GLUT -framework OpenGL -lobjc
# OSX = -D OSX

app: main.cpp QuadTree.cpp 
	$(CXX) $(CPPFLAGS) $(OSX) -o app main.cpp QuadTree.cpp $(LIBDIRS) $(LIBS)

clean:
	rm -f $(C++OBJ) app
