#!/bin/bash

rm shaders.inc

for file in *.{vert,frag} ; do
	glslangValidator -V -o ${file/./_}.spirv $file
	xxd -i ${file/./_}.spirv >> shaders.inc
done
