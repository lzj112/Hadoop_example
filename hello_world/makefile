hello_world : hello_world.cpp
	sudo g++ -o hello_world hello_world.cpp -I${HADOOP_HOME}/include -L${HADOOP_HOME}/lib/native -lhdfs -L${JAVA_HOME}/lib/server -ljvm

clean :
	rm hello_world