time_test.exe : main.obj name.obj time.obj utils.obj
	link /out:time_test.exe main.obj name.obj time.obj utils.obj

clean :
	del main.obj name.obj time.obj utils.obj
	del utils.pch
	del time_test.exe

name.obj : name.cpp name.h utils.pch
	cl /EHsc /c name.cpp /Yuutils.h

time.obj : time.cpp time.h utils.pch
	cl /EHsc /c time.cpp /Yuutils.h

main.obj : main.cpp name.h time.h utils.pch
	cl /EHsc /c main.cpp /Yuutils.h

utils.pch utils.obj : utils.cpp utils.h
	cl /EHsc /c utils.cpp /Ycutils.h
