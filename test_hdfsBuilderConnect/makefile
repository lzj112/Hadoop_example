test : test.cpp
	sudo g++ -o test test.cpp -I${HADOOP_HOME}/include -L${HADOOP_HOME}/lib/native -lhdfs -L${JAVA_HOME}/lib/server -ljvm

clean :
	rm test