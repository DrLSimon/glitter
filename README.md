# glitter : GL iterative tutorial tasks on efficient rendering.
This is an OpenGL Lab for students in the computer science department at
ENSICAEN, an engineering scool in Caen, Normandy, France. They can of course,
be used by any other students willing to learn OpenGL (although better
tutorials probably exist elsewhere). The labs are associated with 3 lectures
introducing to the basics of OpenGL. The slides are available here:
* [Lecture 1](http://www.ecole.ensicaen.fr/~simonl/files/OpenGL/Cours/Lesson1)
* [Lecture 2](http://www.ecole.ensicaen.fr/~simonl/files/OpenGL/Cours/Lesson2)
* [Lecture 3](http://www.ecole.ensicaen.fr/~simonl/files/OpenGL/Cours/Lesson3)

# Documentation
The list of tutorials is described in detail in the [web site](https://drlsimon.github.io/glitter/).
The doxygen documentation is available [here](http://www.ecole.ensicaen.fr/~simonl/files/OpenGL/glitter-doc/annotated.html).

# Requirements
The project relies on the following dependencies
* Cmake 2.8
* OpenGL 4.1
* GLFW 3.0
* GLM 0.9.2
* [tinyobjloader](https://github.com/syoyo/tinyobjloader)
* [stb](https://github.com/nothings/stb)
* [termcolor](https://github.com/ikalnitsky/termcolor)

**Note:** Missing dependencies will be automatically added as git submodules.


# Instructions
## Building
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE
make -j4
```

## Running
All the labs, must be launched without any optional argument, from the build root (otherwise the assets/shaders will not be found). For example,
```bash
# running glitter help
./glitter help
# getting help on pa1
./glitter help pa1
# running pa1 part 1
./glitter pa1 1
````


# Author and License
The code is published under the MIT License (MIT)

Copyright (c) 2018 Loic Simon
Copyright (c) 2018 Sebastien Fourey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
